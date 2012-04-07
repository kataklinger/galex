
/*! \file ChromosomeGroup.cpp
    \brief This file contains implementation of class that handles chromosome groups.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#include "RandomSequence.h"
#include "Population.h"

namespace Population
{

	// Inserts new chromosome after the last chromosome currently in the group
	int GaChromosomeGroup::Add(GaChromosomeStorage* chromosome)
	{
		// is chromosome already a member of this group
		if( _membershipFlag && chromosome->GetFlags().IsFlagSetAll( _membershipFlag ) )
			// if membership flag is used, single chromosome cannot inserted in the same group multiple times 
			return -1;

		_hasShuffleBackup = false;

		// group is full?
		if( _count == _array.GetSize() )
		{
			GA_ASSERT( Common::Exceptions::GaInvalidOperationException, _sizable, "This chromosome group is full.", "Population" );

			// increase size of the group so it can accommodate new chromosome
			IncreaseSize();
		}

		// insert chromosome and mark it as a member
		int pos = _count++;
		_chromosomes[ pos ] = chromosome;
		chromosome->GetFlags().SetFlags( _membershipFlag );

		return pos;
	}

	// Atomically inserts chromosome into the group
	int GaChromosomeGroup::AddAtomic(GaChromosomeStorage* chromosome)
	{
		// is chromosome already a member of this group
		if( _membershipFlag && chromosome->GetFlags().IsFlagSetAll( _membershipFlag ) )
			// if membership flag is used, single chromosome cannot inserted in the same group multiple times 
			return -1;

		_hasShuffleBackup = false;

		// insert chromosome and mark it as a member
		int pos = Common::Threading::GaAtomicOps<int>::Add( &_count, 1 );
		_chromosomes[ pos ] = chromosome;
		chromosome->GetFlags().SetFlags( _membershipFlag );

		return pos;
	}

	// Removes chromosome from the group
	bool GaChromosomeGroup::Remove(GaChromosomeStorage* chromosome,
		bool dontRecycle/* = false*/)
	{
		// check membership flag to see if this chromosome is a member of the group
		if( _membershipFlag && !chromosome->GetFlags().IsFlagSetAll( _membershipFlag ) )
			return false;

		_hasShuffleBackup = false;

		int j = 0;
		// find and remove chromosomes
		for( int i = 0; i < _count - 1; i++ )
		{
			if( _chromosomes[ i ] != chromosome )
				// current chromosome is not the one that should be removed
				_chromosomes[ j++ ] = _chromosomes[ i ];
			else
				RemoveHelper( chromosome, dontRecycle );
		}

		if( _count == j )
			// chromosome is not found
			return false;

		_count = j;
		return true;
	}

	// Removes chromosome from the group
	void GaChromosomeGroup::Remove(int index,
		bool dontRecycle/* = false*/)
	{
		_hasShuffleBackup = false;

		_count--;

		RemoveHelper( _chromosomes[ index ], dontRecycle );

		// move other chromosomes to fill the gap
		for( int i = index; i < _count; i++ )
			_chromosomes[ i ] = _chromosomes[ i + 1 ];
	}

	void GACALL GaChromosomeGroup::Remove(bool dontRecycle/* = false*/)
	{
		int newCount = 0;
		for( int i = 0; i < _count; i++ )
		{
			// keep chromosome?
			if( !_chromosomes[ i ]->GetFlags().IsFlagSetAny( GaChromosomeStorage::GACF_REMOVE_CHROMOSOME ) )
				_chromosomes[ newCount++ ] = _chromosomes[ i ];
			else
				RemoveHelper( _chromosomes[ i ], dontRecycle );
		}

		_count = newCount;
	}

	// Removes chromosomes at the bottom of the population
	void GaChromosomeGroup::Trim(int newCount,
		bool dontRecycle/* = false*/)
	{
		GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, newCount >= 0, "newCount", "Count cannot be negative value.", "Population" );

		_hasShuffleBackup = false;

		while( _count > newCount )
			RemoveHelper( _chromosomes[ --_count ], dontRecycle );
	}

	// Removes last chromosome from the population
	GaChromosomeStorage* GaChromosomeGroup::PopLast(bool dontRecycle/* = false*/)
	{
		GaChromosomeStorage* last = NULL;

		if( _count )
		{
			// save last chromosome
			if( dontRecycle )
				last = _chromosomes[ _count - 1 ];

			// remove it
			Trim( _count - 1, dontRecycle );
		}

		return last;
	}

	// Removes all chromosomes from the group
	void GaChromosomeGroup::Clear(bool dontRecycle/* = false*/)
	{
		// already empty?
		if( !_count )
			return;

		_hasShuffleBackup = false;

		// need shrinking?
		bool shrink = _sizable && ( 2 * _count <= _array.GetSize() );

		if( _recycleObjects && !dontRecycle && _population )
		{
			// recycle all chromosome objects using provided pool
			while( _count > 0 )
				_population->ReleaseStorageObject( _chromosomes[ --_count ] );
		}
		else if( _membershipFlag )
		{
			// chromosome objects are not recycled - just clear mebership flags
			while( _count > 0 )
				_chromosomes[ --_count ]->GetFlags().ClearFlags( _membershipFlag );
		}
		else
			_count = 0;

		// shrink size of array that will store chromosomes
		if( shrink )
			ResizeArray( _array.GetSize() / 2 );
	}

	// Shrinks size of array that stores chromosomes if it is possible and reasonable
	bool GaChromosomeGroup::Shrink()
	{
		// cannot shrink chromosome group with fixed size
		if( !_sizable )
			return false;

		// is it reasonable to shrink size?
		int newSize = _count * 2;
		if( newSize > _array.GetSize() )
		{
			ResizeArray( newSize );
			return true;
		}

		return false;
	}

	// Shuffles chromosomes in the group
	void GaChromosomeGroup::Shuffle(bool backup)
	{
		if( backup )
		{
			// backup
			GaChromosomeStorage** helper = _array.GetArray() == _chromosomes ? _helper.GetArray() : _array.GetArray();
			for( int i = _count - 1; i >= 0; i-- )
				helper[ i ] = _chromosomes[ i ];
		}

		Common::Random::GaShuffle( _chromosomes, _count );
		_hasShuffleBackup = backup;
	}

	// Restores chromosomes in oreder prior they were shuffled
	void GaChromosomeGroup::RestoreShuffle()
	{
		GA_ASSERT( Common::Exceptions::GaInvalidOperationException, _hasShuffleBackup, "Group does not contains backup of shuffle operation.", "Population" );

		// restore from backup
		GaChromosomeStorage** helper = _array.GetArray() == _chromosomes ? _helper.GetArray() : _array.GetArray();
		for( int i = _count - 1; i >= 0; i-- )
			_chromosomes[ i ] = _helper[ i ];

		_hasShuffleBackup = false;
	}

	// Sets new size of array that stores chromosomes
	void GaChromosomeGroup::SetSize(int size)
	{
		if( size != _array.GetSize() )
		{
			GA_ASSERT( Common::Exceptions::GaInvalidOperationException, !_sizable, "This chromosome group manage its size automatically.", "Population" );
			GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, size >= 0, "size", "Group size cannot be negative value.", "Population" );

			// with new size of group cannot store chromosmes?
			if( !size )
				// removes all chromosomes from the group
				Clear();
			else
			{
				int limit = 0;
				if( !_array.IsEmpty() )
				{
					limit = _array.GetSize() < _count ? _array.GetSize() : _count;

					if( _recycleObjects && _population )
					{
						// recycle chromosome objects using provided pool
						for( int i = limit; i < _count; i++ )
							_population->ReleaseStorageObject( _chromosomes[ i ] );
					}
					else if( _membershipFlag )
					{
						// chromosome objects are not recycled - just clear mebership flags
						for( int i = limit; i < _count; i++ )
							_chromosomes[ i ]->GetFlags().SetFlags( _membershipFlag );
					}
				}

				_count = limit;
				ResizeArray( size );
			}
		}
	}

	// Sets flag that will be used to indicate chromosome's membership to this group.
	void GaChromosomeGroup::SetMembershipFlag(GaChromosomeStorage::GaFlagType::GaDataType flag)
	{
		// update flags of the chromosomes currently in the group
		for( int i = _count - 1; i >= 0; i-- )
		{
			_chromosomes[ i ]->GetFlags().ClearFlags( _membershipFlag );
			_chromosomes[ i ]->GetFlags().SetFlags( flag );
		}

		_membershipFlag = flag;
	}

	void GaChromosomeGroup::RemoveHelper(GaChromosomeStorage* chromosome,
		bool dontRecycle)
	{
		if( _recycleObjects && !dontRecycle && _population )
			// recycle chromosome objects using provided pool
			_population->ReleaseStorageObject( chromosome );
		else if( _membershipFlag )
			// chromosome object is not recycled when it is removed - just clear membership flag
			chromosome->GetFlags().ClearFlags( _membershipFlag );
	}

	// Increases size of array that stores chromosomes
	void GaChromosomeGroup::IncreaseSize()
	{
		// increase size of array
		int size = _array.GetSize() * 2;
		ResizeArray( size ? size : 4 );
	}

	// Resizes arrays and updates sorting algorithm
	void GaChromosomeGroup::ResizeArray(int newSize)
	{
		bool useHelper = _helper.GetArray() == _chromosomes;

		// resize
		_array.SetSize( newSize ); 
		_helper.SetSize( newSize );

		// determin main storage array
		_chromosomes = useHelper ? _helper.GetArray() : _array.GetArray();

		// update sorting algorithm to use resized arrays
		_sorting.SetArray( _chromosomes, useHelper ? _array.GetArray() : _helper.GetArray(), _chromosomes ? &_count : NULL );
	}

} // Population


/*! \file ChromosomeGroup.h
    \brief This file contains declaration of class that handles chromosome groups.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_CHROMOSOME_GROUP_H__
#define __GA_CHROMOSOME_GROUP_H__

#include "ChromosomeStorage.h"

namespace Population
{

	class GaPopulation;

	/// <summary><c>GaChromosomeGroup</c> class manages chromosome groups.
	///
	/// This class has built-in synchronizator so it is allowed to use <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros with instances of this class,
	/// but public or private methods are thread-safe.</summary>
	class GaChromosomeGroup
	{

		GA_SYNC_CLASS

	protected:

		/// <summary>Pointer to array that is currently used to store chromosomes.</summary>
		GaChromosomeStorage** _chromosomes;

		/// <summary>Original array used for storing chromosomes.</summary>
		Common::Data::GaSingleDimensionArray<GaChromosomeStorage*> _array;

		/// <summary>Helper array used for sorting.</summary>
		Common::Data::GaSingleDimensionArray<GaChromosomeStorage*> _helper;

		/// <summary>Number of chromosomes currently in the group.</summary>
		int _count;

		/// <summary>Indicates whether the group can shrink and expand automatically to accommodate all inserted chromosomes, or that is fixed size.</summary>
		bool _sizable;

		/// <summary>Algorithm for sorting chromosomes.</summary>
		Common::Sorting::GaMergeSortAlgorithm<GaChromosomeStorage*> _sorting;

		/// <summary>Chromosome flag used for indicating chromosome's membership to this group.</summary>
		GaChromosomeStorage::GaFlagType::GaDataType _membershipFlag;

		/// <summary>Population to which this group is bound.</summary>
		GaPopulation* _population;

		/// <summary>Indicates whether the chromosome storage objects should be recycled using population's object pool when chromosomes are removed from the group.</summary>
		bool _recycleObjects;

		/// <summary>Indicates that group has backup made before shuffle operation performed.</summary>
		bool _hasShuffleBackup;

	public:

		/// <summary>This constructor initializes chromosome group.</summary>
		/// <param name="sizable">if this parameter is set to <c>true</c> the size of this group can change dynamically to store all inserted chromosomes,
		/// otherwise size of group is fixed.</param>
		/// <param name="size">initial size of the group (number of chromosomes it can store).</param>
		/// <param name="membershipFlag">chromosome flag used for indicating chromosome's membership to this group.
		/// If this value is 0, chromosomes are not marked as members of this group.</param>
		/// <param name="population">pointer to population to which this group will be bound.</param>
		/// <param name="recycleObjects">if this parameter is set to <c>true</c> it instructs group to recycle chromosome storage objects population's object pool
		/// when chromosomes are removed from this group.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>size</c> is negative value.</exception>
		/// <exception cref="GaArgumentException" />Thrown if user tries to enable chromosome storage object recycling but does not specify population.</exception>
		GAL_API
		GaChromosomeGroup(bool sizable = false,
			int size = 0,
			GaChromosomeStorage::GaFlagType::GaDataType membershipFlag = 0,
			GaPopulation* population = NULL,
			bool recycleObjects = false) : _count(0),
			_sizable(sizable),
			_sorting(true),
			_membershipFlag(membershipFlag),
			_population(population),
			_recycleObjects(recycleObjects),
			_hasShuffleBackup(false) { ResizeArray( size ); }

		/// <summary>Frees memory used for storing chromosome.</summary>
		~GaChromosomeGroup() { Clear(); }

		/// <summary><c>Add</c> method inserts new chromosome after the last chromosome in the group. If membership flag is used to mark members of the group
		/// one chromosome cannot be inserted multiple time.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="chromosome">pointer to chromosome that should be added to the group.</param>
		/// <exception cref="GaInvalidOperationException" />Thrown if the group is full when user tries to insert new chromosome and the group is not sizable.</exception>
		/// <returns>Method returns position to which the chromosomes is inserted or -1 if the chromosome is already marked as member of the group.</returns>
		GAL_API
		int GACALL Add(GaChromosomeStorage* chromosome);

		/// <summary><c>AddSorted</c> method inserts new chromosome to the group preserving sorted order. When the group is full new chromosome is inserted only if it has
		/// better ranking than the last chromosome currently in the group by removing last chromosome from the group. If membership flag is used to mark members of the group
		/// one chromosome cannot be inserted multiple time. When sorting criteria is not specified method use calls <see cref="Add" /> to perform insertion.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="CRITERIA">type of sorting criteria used for sorting.</param>
		/// <param name="criteria">criteria used for sorting chromosomes in the group.</param>
		/// <param name="chromosome">pointer to chromosome that should be added to the group.</param>
		/// <param name="sortCriteria">reference to sort criteria used for determining position at which the chromosomes should be inserted.</param>
		/// <parm name="topLimit">number of chromosomes at the top of chromosome group that are protected from being removed.</param>
		/// <returns>Method returns <c>true</c> if the chromosome was inserted into group successfully.</returns>
		/// <exception cref="GaInvalidOperationException" />Thrown if the group is full when user tries to insert new chromosome and the group is not sizable.</exception>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>topLimit</c> is greater that current size of group or equal to maximal size.</exception>
		template<typename CRITERIA>
		bool GACALL AddSorted(GaChromosomeStorage* chromosome,
			const CRITERIA& criteria,
			int topLimit = 0)
		{
			GA_ASSERT( Common::Exceptions::GaInvalidOperationException, _array.GetSize() > 0 || _sizable,
				"This chromosome group cannot store chromosomes because its size is 0.", "Population" );

			GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, topLimit <= _count && topLimit < _array.GetSize(), "topLimit",
				"Protection limit is out of current range.", "Population" );

			// is chromosome already a member of this group
			if( _membershipFlag && chromosome->GetFlags().IsFlagSetAll( _membershipFlag ) )
				// if membership flag is used, single chromosome cannot inserted in the same group multiple times 
				return false;

			GaChromosomeStorage** first = _chromosomes + topLimit;
			GaChromosomeStorage** last = first + ( _count - topLimit - 1 );

			// group is full?
			if( _count == _array.GetSize() )
			{
				// group can increase size?
				if( _sizable )
					// expand group to accommodate new chromosome
					IncreaseSize();
				else if( _count )
				{
					// is chromosome good enough to be inserted?
					if( criteria( *last, chromosome ) < 0 )
						// new chromosome has worse ranking than last chromosome currently in the group - do not inser it
						return false;
					else
					{
						// remove last chromosome to make space for new which has better ranking
						( *last )->GetFlags().ClearFlags( _membershipFlag );
						last--;
						_count--;
					}
				}
			}

			// find position to which new chromosome should be inserted based on sorting criteria
			// and move other chromosomes to make space
			for( ; last >= first && criteria( *last, chromosome ) > 0; last-- )
				*( last + 1 ) = *last;

			// insert chromosome and mark it as a member
			*( last + 1 ) = chromosome;
			chromosome->GetFlags().SetFlags( _membershipFlag );
			_count++;

			return true;
		}

		/// <summary><c>AddAtomic</c> method performs atomic insertion of chromosomes into the group after the last chromosome currently in the group.
		/// Different threads can safely execute this method simultaneously, but no other methods on this object should be executed from other threads at that time.
		/// <c>AddAtomic</c> method does not perform additional checking to protect group from overflowing. 
		/// If membership flag is used to mark members of the group one chromosome cannot be inserted multiple time
		///
		/// This method is not thread-safe.</summary>
		/// <param name="chromosome">pointer to chromosome that should be added to the group.</param>
		/// <returns>Method returns position to which the chromosomes is inserted.</returns>
		GAL_API
		int GACALL AddAtomic(GaChromosomeStorage* chromosome);

		/// <summary><c>Remove</c> method removes specified chromosome from the group.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="chromosome">pointer to chromosome that should be removed.</param>
		/// <param name="dontRecycle">this parameter is used for overwriting recycle policy of the group.
		/// If it is set to <c>true</c> removed storage object will not be recycled even if the recycling is turned on by the group policy.</param>
		/// <returns>Method returns <c>true</c> if specified chromosome is found and removed from the group successfully.</returns>
		GAL_API
		bool GACALL Remove(GaChromosomeStorage* chromosome,
			bool dontRecycle = false);

		/// <summary><c>Remove</c> method removes chromosome at specified position from the group.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="index">index of the chromosome that should be removed.</param>
		/// <param name="dontRecycle">this parameter is used for overwriting recycle policy of the group.
		/// If it is set to <c>true</c> removed storage object will not be recycled even if the recycling is turned on by the group policy.</param>
		GAL_API
		void GACALL Remove(int index,
			bool dontRecycle = false);

		/// <summary></summary>
		/// <param name="dontRecycle"></name>
		GAL_API
		void GACALL Remove(bool dontRecycle = false);

		/// <summary><c>Trim</c> method removes chromosomes at the bottom of the group.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="newCount">number of chromosomes that will be saved.</param>
		/// <param name="dontRecycle">this parameter is used for overwriting recycle policy of the group.
		/// If it is set to <c>true</c> removed storage object will not be recycled even if the recycling is turned on by the group policy.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if the <c>newCount</c> is negative value.</exception>
		GAL_API
		void GACALL Trim(int newCount,
			bool dontRecycle = false);

		/// <summary><c>PopLast</c> method removes last chromosome in the group.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="dontRecycle">this parameter is used for overwriting recycle policy of the group.
		/// If it is set to <c>true</c> removed storage object will not be recycled even if the recycling is turned on by the group policy.</param>
		/// <returns>Method returns to the storage object that was removed from the group or <c>NULL</c> if object was recycled or the group was empty.</returns>
		GAL_API
		GaChromosomeStorage* GACALL PopLast(bool dontRecycle = false);

		/// <summary><c>Clear</c> method removes all chromosomes from the group. If the group is sizable, its size is shrank to reduce memory usage.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="dontRecycle">this parameter is used for overwriting recycle policy of the group.
		/// If it is set to <c>true</c> removed storage object will not be recycled even if the recycling is turned on by the group policy.</param>
		GAL_API
		void GACALL Clear(bool dontRecycle = false);

		/// <summary><c>Shrink</c> method decrease size of chromosome group when it is possible and reasonable.</summary>
		/// <returns>Method returns <c>true</c> if shrinking is done.</returns>
		GAL_API
		bool GACALL Shrink();

		/// <summary><c>Sort</c> method sorts chromosome group using provided sorting algorithm and criteria.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="CRITERIA">type of sorting criteria used for sorting.</param>
		/// <param name="sortCriteria">criteria used for sorting chromosomes in the group.</param>
		template<typename CRITERIA>
		inline void GACALL Sort(const CRITERIA& sortCriteria)
		{
			_chromosomes = _sorting.Sort( sortCriteria );
			_hasShuffleBackup = false;
		}

		/// <summary><c>Shuffle</c> method arranges chromosomes of the group in random order.
		/// Backup of this operation is only valid until the first operation that modifies content group.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="backup">if this parameter is set to <c>true</c> method backups order of chromosomes before it shuffles them.</param>
		GAL_API
		void GACALL Shuffle(bool backup);

		/// <summary><c>RestoreShuffle</c> restore order of chromosomes that was before shuffle operation if valid backup is available, otherwise it throws an exception.
		///
		/// This method is not thread-safe.</summary>
		/// <exception cref="GaInvalidOperationException" />Thrown if the group does not contains backup made before performing shuffle operation.</exception>
		GAL_API
		void GACALL RestoreShuffle();

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to array that is used by the group for storing chromosomes.</returns>
		inline GaChromosomeStorage** GACALL GetChromosomes() { return _chromosomes; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to array that is used by the group for storing chromosomes.</returns>
		inline const GaChromosomeStorage* const* GACALL GetChromosomes() const { return _chromosomes; }

		/// <summary><c>GetAt</c> method returns chromosome at specified position in the group.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="index">index of queried chromosomes.</param>
		/// <returns>Method returns reference to chromosome at specified index in the group.</returns>
		inline GaChromosomeStorage& GACALL GetAt(int index) { return *_chromosomes[ index ]; }

		/// <summary><c>GetAt</c> method returns chromosome at specified position in the group.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="index">index of queried chromosomes.</param>
		/// <returns>Method returns reference to chromosome at specified index in the group.</returns>
		inline const GaChromosomeStorage& GACALL GetAt(int index) const { return *_chromosomes[ index ]; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns size of array that stores chromosomes.</returns>
		inline int GACALL GetSize() const { return _array.GetSize(); }

		/// <summary><c>SetSize</c> method sets new size of array that will store chromosomes. Chromosomes that cannot fit new size are removed from the group.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="size">new size of array the stores chromosomes.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>size</c> is negative value.</exception>
		/// <exception cref="GaInvalidOperationException" />Thrown if the group is sizable and has no fixed size.</exception>
		GAL_API
		void GACALL SetSize(int size);

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns number of chromosomes currently in the group.</returns>
		inline int GACALL GetCount() const { return _count; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if the group does not contain chromosomes.</returns>
		inline bool GACALL IsEmpty() const { return !_count; }

		/// <summary><c>SetAsSizable</c> method sets group to be sizble so it can accommodate all chromosomes that are inserted.
		///
		/// This method is not thread-safe.</summary>
		inline void GACALL SetAsSizable() { _sizable = true; }

		/// <summary><c>SetAsNotSizable</c> method sets group to fixed size. All chromosomes that are currently in the group and cannot fit new size are removed from the group.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="size">new fixed size of the group.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>size</c> is negative value.</exception>
		inline void GACALL SetAsNotSizable(int size)
		{
			_sizable = false;
			SetSize( size );
		}

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if the size of this group is fixed.</returns>
		inline bool GACALL IsSizable() const { return _sizable; }

		/// <summary><c>SetMembershipFlag</c> method sets flag that will be used to indicate chromosome's membership to this group.
		/// It also updates flags of chromosomes currently in the group.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="flag">chromosome flag used for indicating chromosome's membership.</param>
		GAL_API
		void GACALL SetMembershipFlag(GaChromosomeStorage::GaFlagType::GaDataType flag);

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns chromosome flag used for indicating chromosome's membership to this group.</returns>
		inline const GaChromosomeStorage::GaFlagType::GaDataType GACALL GetMembershipFlag() const { return _membershipFlag; }

		/// <summary><c>SetPopulation</c> method bounds chromosome group to the specified population. Method removes all chromosomes currently in the group first.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="population">pointer to population to ehich this group will be bound.</param>
		inline void GACALL SetPopulation(GaPopulation* population)
		{
			Clear();
			_population = population;
		}

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to population to whch this group is bound or <c>NULL</c> if group is not bound.</returns>
		inline GaPopulation* GACALL GetPopulation() { return _population; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to population to whch this group is bound or <c>NULL</c> if group is not bound.</returns>
		inline const GaPopulation* GACALL GetPopulation() const { return _population; }

		/// <summary><c>SetObjectRecycling</c> method sets chromosome sotage object recycling.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="recycle">if this parameter is set to <c>true</c> it instructs group to recycle chromosome storage objects population's object pool
		/// when chromosomes are removed from this group.</param>
		inline void GACALL SetObjectRecycling(bool recycle) { _recycleObjects = recycle; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if chromosome storage object recycling is enabled.</returns>
		inline bool GACALL IsObjectRecyclingEnabled() const { return _recycleObjects; }

		/// <summary><c>operator []</c> returns chromosome at specified position in the group.
		///
		/// This operator is not thread-safe.</summary>
		/// <param name="index">index of queried chromosomes.</param>
		/// <returns>Operator returns reference to chromosome at specified index in the group.</returns>
		inline const GaChromosomeStorage& GACALL operator [](int index) const { return *_chromosomes[ index ]; }
		
		/// <summary><c>operator []</c> returns chromosome at specified position in the group.
		///
		/// This operator is not thread-safe.</summary>
		/// <param name="index">index of queried chromosomes.</param>
		/// <returns>Operator returns reference to chromosome at specified index in the group.</returns>
		inline GaChromosomeStorage& GACALL operator [](int index) { return *_chromosomes[ index ]; }

	private:

		/// <summary></summary>
		/// <param name="chromosome"></param>
		/// <param name="dontRecycle"></param>
		GAL_API
		void GACALL RemoveHelper(GaChromosomeStorage* chromosome,
			bool dontRecycle);

		/// <summary><c>IncreaseSize</c> increases size of array that stores chromosomes so it can accommodate new chromosomes.</summary>
		GAL_API
		void GACALL IncreaseSize();

		/// <summary>This method resize array and helper array and updates sorting algorithm.</summary>
		/// <param name="newSize">new size of arrays.</param>
		GAL_API
		void GACALL ResizeArray(int newSize);

	};

} // Population

#endif // __GA_CHROMOSOME_GROUP_H__

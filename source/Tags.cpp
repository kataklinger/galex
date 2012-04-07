
/*! \file Tags.cpp
	\brief This file contains implementations of classes that is used for managing tags and tag buffers.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#include "Tags.h"

namespace Common
{
	namespace Data
	{

		// Adds tag
		int GaTagManager::AddTag(int tagID,
			const GaTagLifecycle& tagLifecycle,
			bool shouldThrow/* = false*/)
		{
			GaTagIndicesTable::iterator it = _tagIndices.find( tagID );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, it == _tagIndices.end() || !shouldThrow, "tagID", "Tag with specified ID already exists.", "Data" );

			// return index of tag if it exists
			if( it != _tagIndices.end() )
				return it->second.first;

			if( !_freeIndicesCount )
				// if the buffers are full, double their sizes
				ChangeBufferSize( _bufferSize );

			// get free slot, create copy of prototype and map tag it to slot index
			int index = _freeIndices[ --_freeIndicesCount ];
			_tagIndices[ tagID ] = std::make_pair( index, tagLifecycle.Clone() );

			// insert tag to tag buffers
			if( !_update.IsNull() )
				_update->AddTag( index, tagLifecycle );

			return index;
		}

		// Removes tag
		bool GaTagManager::RemoveTag(int tagID)
		{
			// tag exists?
			GaTagIndicesTable::iterator it = _tagIndices.find( tagID );
			if( it == _tagIndices.end() )
				return false;

			int index = it->second.first;

			// free memory used by lifecycle policies
			delete it->second.second;

			// free slot and erase mapping
			_freeIndices[ _freeIndicesCount++ ] = index;
			_tagIndices.erase( tagID );

			// remove tag from buffers
			if( !_update.IsNull() )
				_update->RemoveTag( index );

			// shrink size of buffers
			ShrinkTagBuffer();

			return true;
		}

		// Replaces tags
		int GaTagManager::ReplaceTag(int tagID,
			const GaTagLifecycle& tagLifecycle,
			bool shouldThrow/* = false*/)
		{
			GaTagIndicesTable::iterator it = _tagIndices.find( tagID );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, it != _tagIndices.end() || !shouldThrow, "tagID", "Tag does not exist.", "Data" );

			// add tag if it does not exists
			if( it == _tagIndices.end() )
				return AddTag( tagID, tagLifecycle );

			int index = it->second.first;

			// free memory used by old lifecycle policies
			delete it->second.second;
			it->second.second = tagLifecycle.Clone();

			// insert tag to tag buffers
			if( !_update.IsNull() )
				_update->AddTag( index, tagLifecycle );

			return index;
		}

		// Updates tag
		void GaTagManager::UpdateTag(int tagID,
			const GaTagUpdate& tagUpdate)
		{
			GaTagIndicesTable::iterator it = _tagIndices.find( tagID );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, it != _tagIndices.end() , "tagID", "Tag does not exist.", "Data" );

			// perform update only if actual change has occured
			if( tagUpdate.IsRequired( *it->second.second ) )
			{
				// perform update on tag lifecycle policies
				tagUpdate( *it->second.second );

				// update tags in all buffers
				if( !_update.IsNull() )
					_update->UpdateTag( it->second.first, tagUpdate );
			}
		}

		// Ghanges ID of the tag
		void GaTagManager::ChangeTagID(int currentTagID,
			int newTagID)
		{
			// no change
			if( currentTagID != newTagID )
			{
				GaTagIndicesTable::iterator it = _tagIndices.find( currentTagID );

				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, it != _tagIndices.end(), "currentTagID", "There is no tag with specified ID.", "Data" );
				GA_ARG_ASSERT( Exceptions::GaArgumentException, _tagIndices.find( newTagID ) == _tagIndices.end(), "newTagID", "There already is tag with specified ID.", "Data" );

				// change tag ID
				_tagIndices[ newTagID ] = it->second;
				_tagIndices.erase( it );
			}
		}

		// Shrinks tag buffer if it is possible
		void GaTagManager::ShrinkTagBuffer()
		{
			int index = _bufferSize - 1;

			// is there unused slots at the end of tag buffers?
			bool found = false;
			for( int i = _freeIndicesCount - 1; !found && i >= 0; i-- )
				found = _freeIndices[ i ] == index;

			if( !found )
				// there is not unused slots at the end of tag buffers - abort shrinking
				return;

			// number of unused slots at the end of tag buffers
			index++;
			for( int i = _freeIndicesCount - 1; !found && i >= 0; i-- )
			{
				if( _freeIndices[ i ] < index )
				{
					bool skip = false;
					for( GaTagIndicesTable::iterator it = _tagIndices.begin(); !skip && it != _tagIndices.end(); ++it )
						skip = it->second.first > _freeIndices[ i ];

					if( !skip )
						index = _freeIndices[ i ];
				}
			}

			// calculate preferred size of tag buffers
			int deleteSize = _bufferSize - index;
			int required = _bufferSize == _freeIndicesCount ? 1 : ( _bufferSize - _freeIndicesCount ) / 4;
			int allowed = _freeIndicesCount - required;

			if( deleteSize > allowed )
				deleteSize = allowed;

			// shrink buffers to preferred size
			if( deleteSize >= required )
				ChangeBufferSize( -deleteSize );
		}

		// Changes tag buffer size
		void GaTagManager::ChangeBufferSize(int delta)
		{
			int oldSize = _bufferSize;
			_bufferSize += delta;

			// create new array that will store indices of unused slots
			int* newFreeIndicesBuffer = new int[ _bufferSize ];

			if( delta > 0 )
			{
				// size should increase

				// mark added slots as unused
				for( int i = oldSize; i < _bufferSize; i++ )
					newFreeIndicesBuffer[ _freeIndicesCount++ ] = i;
			}
			else
			{
				// size should decrease

				// remove unused slots that cannot fit new size
				int newFreeIndicesCount = 0;
				for( ; newFreeIndicesCount < _freeIndicesCount; newFreeIndicesCount++ )
				{
					if( _freeIndices[ newFreeIndicesCount ] < _bufferSize )
						newFreeIndicesBuffer[ newFreeIndicesCount ] = _freeIndices[ newFreeIndicesCount ];
				}

				_freeIndicesCount = newFreeIndicesCount;
			}

			// use new array that stores indices of unused slots
			_freeIndices = Memory::GaAutoPtr<int>( newFreeIndicesBuffer, Memory::GaArrayDeletionPolicy<int>::GetInstance() );

			// update size of buffers
			if( !_update.IsNull() )
				_update->SetBufferSize( _bufferSize );
		}

	} // Data
} // Common

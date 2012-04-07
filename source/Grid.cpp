
/*! \file Grid.cpp
    \brief This file implements classes that represnt and handle grids.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#include "Grid.h"

namespace Common
{
	namespace Grid
	{

		// Compares coordinates of two hyperboxes
		int GaHyperBoxSortCriteria::operator ()(GaConstType& object1,
			GaConstType& object2) const
		{
			// get the first pair of coordinate that are no equal
			int res = 0;
			for( int i = object1.GetSize() - 1; i >= 0 && !res; i-- )
				res = object1[ i ] - object2[ i ];

			// compare pair to see which is located lower in coordinate syste,
			return res < 0 ? -1 : ( res > 0 ? 1 : 0 );
		}

		// Sets the first index in the collection that can contain item that belong to hyperbox.
		void GaHyperBoxInfo::Init(int start,
			const GaHyperBox& hyperBox)
		{
			_hyperBox = hyperBox;

			// set information
			_start = start;
			_end = start;
			_count = 1;
		}


		// Adds new entry to the buffer for specified hyperbox
		GaHyperBoxInfo* GaHyperBoxInfoBuffer::Add(int startIndex,
			const GaHyperBox& hyperBox)
		{
			// get next free entry and store info
			GaHyperBoxInfo* info = &_buffer[ _count++ ];
			info->Init( startIndex, hyperBox );

			return info;
		}

		// Sets size of the buffer
		void GaHyperBoxInfoBuffer::SetSize(int size,
			int branchCount)
		{
			Clear();
			_buffer.SetSize( size );
		}

		// Restart iterator to the first hyperbox that should be visited
		void GaHyperBoxNeighbour::Begin(const Common::Grid::GaHyperBox& center,
			const int* limits,
			int level)
		{
			// calculates limits of iteration
			for( int i = _coords.GetSize() - 1; i >= 0; i-- )
			{
				// min and max coordinate
				int j = i << 1;
				int k = j + 1;
				_limits[ j ] = center[ i ] - level;
				_limits[ k ] = center[ i ] + level;

				// min coordinate value cannot be negative
				_skip[ j ] = _limits[ j ] < 0;
				if( _skip[ j ] )
					_limits[ j ] = 0;

				// max coordinate value cannot be greater than specified limit
				_skip[ k ] = _limits[ k ] >= limits[ i ];
				if( _skip[ k ] )
					_limits[ k ] = limits[ i ] - 1;

				// move iteration to the first hyperbox
				_coords[ i ] = _limits[ j ];
			}

			_index = 0;

			// selected hyperbox is out fo range?
			if( _skip[ 0 ] )
				// find next hyperbox which is within the range
				Find();
		}

		// Moves iterator to the next hyperbox
		bool GaHyperBoxNeighbour::Next()
		{
			int c = _coords.GetSize() - 1;
			int i = _index >> 1;

			// move to the next hyperbox
			for( ; c >= 0; c-- )
			{
				if( c != i )
				{
					// increment current coordinate
					_coords[ c ]++;

					// no overflow?
					if( _coords[ c ] <= _limits[ ( c << 1 ) + 1 ] )
						break;

					// restart this coordinate and move to next coordinate
					_coords[ c ] = _limits[ c << 1 ];
				}
			}

			// find next hyperbox within the defined limits if the end is not reached
			return !( c < 0 && !Find() );
		}

		// Sets number fo dimension that hypergrid has
		void GaHyperBoxNeighbour::SetCoordinatesCount(int count)
		{
			if( _coords.GetSize() != count )
			{
				_coords.SetSize( count );
				_limits.SetSize( count * 2 );
				_skip.SetSize( count * 2 );
			}
		}

		// Finds the next hyperbox whose coordinates are within the range
		bool GaHyperBoxNeighbour::Find()
		{
			// remove visited edge from iteration path
			if( !_skip[ _index ] )
				_limits[ _index ] += ( _index & 1 ) ? -1 : 1;

			// move coordinates to an lower edge
			_coords[ _index >> 1 ] = _limits[ _index & ~1 ];

			// find next edge
			while( ++_index < _limits.GetSize() )
			{
				// edge within the limits?
				if( !_skip[ _index ] )
				{
					// move current coordinates
					_coords[ _index >> 1 ] = _limits[ _index ];
					return true;
				}

				// move coordinates to an lower edge
				_coords[ _index >> 1 ] = _limits[ _index & ~1 ];
			}

			return false;
		}

	} // Grid
} // Common

#include "Array.h"

namespace Common
{
	namespace Data
	{
		
		// Calculates coordinates of the next element
		void GACALL GaNextCoord(GaSingleDimensionArray<int>& coordinates,
			const int* dimensionSizes)
		{
			int count = coordinates.GetSize();

			// calculate next coordinate
			for( int j = 0; j < count; j++ )
			{
				// next coordinate
				coordinates[ j ]++;
				if( coordinates[ j ] < dimensionSizes[ j ] )
					break;

				coordinates[ j ] = 0;
			}
		}

		// Calculates coordinates of the next element
		void GACALL GaNextCoordWithSkip(GaSingleDimensionArray<int>& coordinates,
			const int* dimensionSizes,
			int firstSkipDimension,
			int lastSkipDimension)
		{
			int count = coordinates.GetSize();

			// calculate next coordinate
			for( int j = 0; j < count; j++ )
			{
				// skip required coordinates
				if( j == firstSkipDimension )
				{
					j = lastSkipDimension - 1;
					continue;
				}

				// next coordinate
				coordinates[ j ]++;
				if( coordinates[ j ] < dimensionSizes[ j ] )
					break;

				coordinates[ j ] = 0;
			}
		}

		// Calculates coordinates of the next element
		void GACALL GaNextCoordWithSkip(GaSingleDimensionArray<int>& coordinates,
			const int* dimensionSizes,
			int firstSkipDimension,
			int lastSkipDimension,
			int& skip)
		{
			int count = coordinates.GetSize();

			// calculate next coordinate
			for( int j = 0; j < count; j++ )
			{
				// next coordinate
				coordinates[ j ]++;
				if( coordinates[ j ] < dimensionSizes[ j ] )
				{
					if( j >= firstSkipDimension && j < lastSkipDimension && coordinates[ j ] == 1 )
						// coordinate is out of range
						skip++;

					break;
				}

				if( j >= firstSkipDimension && j < lastSkipDimension )
					// coordinate is out of range
					skip--;

				coordinates[ j ] = 0;
			}
		}

		// Calculates coordinates of the next element
		void GACALL GaNextCoordWithSkip(GaSingleDimensionArray<int>& coordinates,
			const int* dimensionSizes,
			const int* skipLimits,
			int& skip)
		{
			int count = coordinates.GetSize();

			// calculate next coordinate
			for( int j = 0; j < count; j++ )
			{
				// next coordinate
				coordinates[ j ]++;

				if( coordinates[ j ] == skipLimits[ j ] )
					// coordinate is out of range
					skip++;

				if( coordinates[ j ] < dimensionSizes[ j ] )
					break;

				if( coordinates[ j ] >= skipLimits[ j ] )
					// coordinate is out of range
					skip--;

				coordinates[ j ] = 0;
			}
		}

	} // Data
} // Common

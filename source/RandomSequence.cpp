
/*! \file RandomSequence.cpp
    \brief This file contains implementation of methods that generates sequence of random numbers.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#include "RandomSequence.h"

namespace Common
{
	namespace Random
	{

		/// <summary><c>GaGenerateRandomSequenceSort</c> method generates sequence of unique random integer numbers in sorted order withing given interval (<c>min</c>, <c>max</c>).
		///
		/// This method is not thread-safe.</summary>
		/// <param name="T">type of sorting of random sequence.</param>
		/// <param name="min">minimal value that can be generated.</param>
		/// <param name="max">maximal value that can be generated.</param>
		/// <param name="count">size of sequence (number of generated numbers).</param>
		/// <param name="generated">output array to which the generated sequence will be stored.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if maximal value is lower then minimal or if count is lower then or equal to 0
		/// or greater then size of the interval.</exception>
		/// <exception cref="GaNullArgumentException" />Thrown when <c>generated</c> is set to <c>NULL</c>.</exception>
		template<typename T>
		void GaGenerateRandomSequenceSort(int min,
			int max,
			int count,
			int* generated)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, generated != NULL, "generated", "Output array must be specified.", "Random" );
			GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, max > min, "max", "Maximal value must be greater then minimal value.", "Random" );
			GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, count > 0 && count <= max - min + 1,
				"count", "Count must be greater then 0 and lower then or equal to size of the interval.", "Random" );

			for( int i = 0, j; i < count; i++ )
			{
				// generate index of random number
				int num = T::Generate( max - min /*+ 1*/, i );

				// convert index of random number to actual number and find place for it in output array
				for( j = count - i; j < count; j++, T::Correct( &num ) )
				{
					if( T::Compare( ( num + min ), generated[ j ] ) < 0 )
						break;

					generated[ j - 1 ] = generated[ j ];
				}

				// save number to output sequence
				generated[ j - 1 ] = num + min;
			}
		}

		// Generates sequence of unique random integer numbers in ascendent order
		void GaGenerateRandomSequenceAsc(int min,
			int max,
			int count,
			int* generated)
		{
			// provides methods used by seqence generator to produce numbers in ascendent order
			struct AcsSort
			{

				// generates index of random number
				static inline int Generate(int max,
					int left) { return GaGlobalRandomIntegerGenerator->Generate( max - left ); }

				// compare two random numbers
				static inline int Compare(int a, int b) { return a - b; }

				// makse correction when convertin index of random number to the actual number
				static inline void Correct(int* number) { ++*number; }

			};

			GaGenerateRandomSequenceSort<AcsSort>( min, max, count, generated );
		}

		// Generates sequence of unique random integer numbers in descendant order
		void GaGenerateRandomSequenceDsc(int min,
			int max,
			int count,
			int* generated)
		{
			// provides methods used by seqence generator to produce numbers in descendant order
			struct DcsSort
			{

				// generates index of random number
				static inline int Generate(int max, int left) { return max - GaGlobalRandomIntegerGenerator->Generate( max - left )/* - 1*/; }

				// compare two random numbers
				static inline int Compare(int a, int b) { return b - a; }

				// makse correction when convertin index of random number to the actual number
				static inline void Correct(int* number) { --*number; }

			};

			GaGenerateRandomSequenceSort<DcsSort>( min, max, count, generated );
		}

		// Generates unsorted sequence of uniq random numbers
		void GaGenerateRandomSequence(int min,
			int max,
			int count,
			int* generated)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, generated != NULL, "generated", "Output array must be specified.", "Random" );
			GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, max > min, "max", "Maximal value must be greater then minimal value.", "Random" );
			GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, count > 0 && count <= max - min + 1,
				"count", "Count must be greater then 0 and lower then or equal to size of the interval.", "Random" );

			for( int i = 0; i < count; i++ )
			{
				bool found = false;
				int p;
				do
				{
					// generate random number
					p = min + GaGlobalRandomIntegerGenerator->Generate( max - min );
					found = false;

					// check if the generated numbe is already in the sequence
					for( int j = i - 1; j >= 0; j-- )
					{
						if( p == generated[ j ] )
						{
							found = true;
							break;
						}
					}
				}
				while( found );

				// save generated number to output sequence
				generated[ i ] = p;
			}
		}

	} // Random
} // Common

/*! \file RandomSequence.h
    \brief This file contains declaration and implementation of methods that generates sequence of random numbers.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_RANDOM_SEQUENCE_H__
#define __GA_RANDOM_SEQUENCE_H__

#include "GlobalRandomGenerator.h"

namespace Common
{
	namespace Random
	{

		/// <summary><c>GaGenerateRandomSequenceAsc</c> method generates sequence of random integer numbers
		/// in ascendent order withing given interval (<c>min</c>, <c>max</c>).</summary>
		/// <param name="min">minimal value that can be generated.</param>
		/// <param name="max">maximal value that can be generated.</param>
		/// <param name="count">size of sequence (number of generated numbers).</param>
		/// <param name="unique">when set to <c>true</c> function will generate sequence of unique numbers.</param>
		/// <param name="generated">output array to which the generated sequence will be stored.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if maximal value is lower then minimal or if count is lower then or equal to 0
		/// or greater then size of the interval.</exception>
		/// <exception cref="GaNullArgumentException" />Thrown when <c>generated</c> is set to <c>NULL</c>.</exception>
		GAL_API
		void GACALL GaGenerateRandomSequenceAsc(int min,
			int max,
			int count,
			bool unique,
			int* generated);

		/// <summary><c>GaGenerateRandomSequenceDsc</c> method generates sequence of random integer numbers
		/// in descendant order withing given interval (<c>min</c>, <c>max</c>).</summary>
		/// <param name="min">minimal value that can be generated.</param>
		/// <param name="max">maximal value that can be generated.</param>
		/// <param name="count">size of sequence (number of generated numbers).</param>
		/// <param name="unique">when set to <c>true</c> function will generate sequence of unique numbers.</param>
		/// <param name="generated">output array to which the generated sequence will be stored.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if maximal value is lower then minimal or if count is lower then or equal to 0
		/// or greater then size of the interval.</exception>
		/// <exception cref="GaNullArgumentException" />Thrown when <c>generated</c> is set to <c>NULL</c>.</exception>
		GAL_API
		void GACALL GaGenerateRandomSequenceDsc(int min,
			int max,
			int count,
			bool unique,
			int* generated);

		/// <summary><c>GaGenerateRandomSequence</c> method generates unsorted sequence of random integer numbers
		/// withing given interval (<c>min</c>, <c>max</c>).</summary>
		/// <param name="min">minimal value that can be generated.</param>
		/// <param name="max">maximal value that can be generated.</param>
		/// <param name="count">size of sequence (number of generated numbers).</param>
		/// <param name="unique">when set to <c>true</c> function will generate sequence of unique numbers.</param>
		/// <param name="generated">output array to which the generated sequence will be stored.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if maximal value is lower then minimal or if count is lower then or equal to 0
		/// or greater then size of the interval.</exception>
		/// <exception cref="GaNullArgumentException" />Thrown when <c>generated</c> is set to <c>NULL</c>.</exception>
		GAL_API
		void GACALL GaGenerateRandomSequence(int min,
			int max,
			int count,
			bool unique,
			int* generated);

		/// <summary><c>GaGenerateRandomSequence</c> method generates randomly ordered sequence of integer numbers
		/// withing given interval (<c>min</c>, <c>max</c>) where each value shows exactly one time.</summary>
		/// <param name="min">minimal value that can be generated.</param>
		/// <param name="max">maximal value that can be generated.</param>
		/// <param name="generated">output array to which the generated sequence will be stored.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if maximal value is lower then minimal or if count is lower then or equal to 0
		/// or greater then size of the interval.</exception>
		/// <exception cref="GaNullArgumentException" />Thrown when <c>generated</c> is set to <c>NULL</c>.</exception>
		GAL_API
		void GACALL GaGenerateRandomSequence(int min,
			int max,
			int* generated);

		/// <summary><c>GaShuffle</c> method shuffles array of elements of specified type.</summary>
		/// <typeparam name="TYPE">type of elements in the array.</typeparam>
		/// <param name="arr">pointer to array of elements that should be shuffled.</param>
		/// <param name="count">number of elements in the array.</param>
		/// <exception cref="GaNullArgumentException" />Thrown if <c>arr</c> is set to <c>NULL</c>.</exception>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>count</c> is negative number.</exception>
		template<typename TYPE>
		void GACALL GaShuffle(TYPE* arr,
			int count)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, arr != NULL, "arr", "Input array must be specified.", "Random" );
			GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, count >= 0, "count", "Count of elements in the array must be positive number.", "Random" );

			// shuffle array
			while( count > 1 )
			{
				// generate random position of next element
				int j = GaGlobalRandomIntegerGenerator->Generate( --count );

				// swap elements
				TYPE t = arr[ count ];
				arr[ count ] = arr[ j ];
				arr[ j ] = t;
			}
		}

		/// <summary><c>GaShuffleTo</c> method shuffles input array of elements of specified type and stores result to provided output array.</summary>
		/// <typeparam name="TYPE">type of elements in the array.</typeparam>
		/// <param name="inputArray">pointer to array of elements that should be shuffled.</param>
		/// <param name="outputArray">pointer to array that will stores shuffled elements.</param>
		/// <param name="count">number of elements in the array.</param>
		/// <exception cref="GaNullArgumentException" />Thrown if <c>inputArray</c> is set to <c>NULL</c>.</exception>
		/// <exception cref="GaNullArgumentException" />Thrown if <c>outputArray</c> is set to <c>NULL</c>.</exception>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>count</c> is negative number.</exception>
		template<typename TYPE>
		void GACALL GaShuffleTo(const TYPE* inputArray,
			TYPE* outputArray,
			int count)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, inputArray != NULL, "inputArray", "Input array must be specified.", "Random" );
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, outputArray != NULL, "outputArray", "Output array must be specified.", "Random" );
			GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, count >= 0, "count", "Count of elements in the array must be positive number.", "Random" );

			// copy elements to output array
			for( int i = count - 1; i >= 0; i-- )
				outputArray[ i ] = inputArray[ i ];

			// shuffle output array
			while( count > 1 )
			{
				// generate random position of next element
				int j = GaGlobalRandomIntegerGenerator->Generate( --count );

				// swap elements
				TYPE t = outputArray[ count ];
				outputArray[ count ] = outputArray[ j ];
				outputArray[ j ] = t;
			}
		}

	} // Random
} // Common

#endif // __GA_RANDOM_SEQUENCE_H__

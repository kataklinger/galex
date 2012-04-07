
/*! \file Sorting.h
    \brief This file contains declarations of classes, datatypes and methods that implements sorting algorithms.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_SORTING_H__
#define __GA_SORTING_H__

#include "SmartPtr.h"

namespace Common
{
	/// <summary><c>Sorting</c> namespace contains classes and datatypes that provide sorting services.</summary>
	namespace Sorting
	{

		/// <summary><c>GaSortingCriteria</c> template class represents interface for criteria used to sort collections by sorting algorithms.</summary>
		/// <param name="TYPE">datatype of values that are comapred for sorting.</param>
		template<typename TYPE>
		class GaSortingCriteria
		{

		public:

			/// <summary>Type of date used by this criteria.</summary>
			typedef TYPE GaType;

			/// <summary>Constant type of date used by this criteria.</summary>
			typedef const TYPE GaConstType;

		};

		/// <summary><c>GaAscendingSortingCriteria</c> template class represents simple comparison that using operator > to sort itemse in ascending order.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		/// <param name="TYPE">datatype of values that are comapred for sorting.</param>
		template<typename TYPE>
		class GaAscendingSortingCriteria : public GaSortingCriteria<TYPE>
		{

		public:

			/// <summary>><c>operator ()</c> compares two objects using operator > and returns results of comparison.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="object1">reference to the first object that is being compared.</param>
			/// <param name="object2">reference to the second object that is being compared.</param>
			/// <returns>Method should returns:
			/// <br/>a.  1 if <c>object1 > object2</c> is <c>true</c>,
			/// <br/>b. -1 if <c>object2 > object1</c> is <c>true</c>,
			/// <br/>c.  0 if both <c>object1 > object2</c> and <c>object2 > object1</c> are <c>false</c>.</returns>
			inline int GACALL operator ()(GaConstType& object1,
				GaConstType& object2) const { return object1 > object2 ? 1 : ( object2 > object1 ? -1 : 0 ); }

		};

		/// <summary><c>GaDescendingSortingCriteria</c> template class represents simple comparison that using operator > to sort itemse in descending order.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		/// <param name="TYPE">datatype of values that are comapred for sorting.</param>
		template<typename TYPE>
		class GaDescendingSortingCriteria : public GaSortingCriteria<TYPE>
		{

		public:

			/// <summary><c>operator ()</c> compares two objects using operator > and returns results of comparison.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="object1">reference to the first object that is being compared.</param>
			/// <param name="object2">reference to the second object that is being compared.</param>
			/// <returns>Method should returns:
			/// <br/>a. -1 if <c>object1 > object2</c> is <c>true</c>,
			/// <br/>b.  1 if <c>object2 > object1</c> is <c>true</c>,
			/// <br/>c.  0 if both <c>object1 > object2</c> and <c>object2 > object1</c> are <c>false</c>.</returns>
			inline int GACALL operator ()(GaConstType& object1,
				GaConstType& object2) const { return object1 > object2 ? -1 : ( object2 > object1 ? 1 : 0 ); }

		};

		/// <summary><c>GaMutableSortCriteria</c> template class should be used where it is posssible to change sort criteria at run-time.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public method or private methods are thread-safe.</summary>
		/// <param name="TYPE">datatype of values that are comapred for sorting.</param>
		template<typename TYPE>
		class GaMutableSortCriteria : public GaSortingCriteria<TYPE>
		{

		public:

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaMutableSortCriteria() { }

			/// <summary><c>Clone</c> method creates copy of the criteria object.</summary>
			/// <returns>Method returns pointer to newly create object.</returns>
			virtual GaMutableSortCriteria<GaType>* Clone() const = 0;

			/// <summary><c>operator ()</c> compares two objects using adopted sorting criteria.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="object1">reference to the first object that is being compared.</param>
			/// <param name="object2">reference to the second object that is being compared.</param>
			/// <returns>Method should returns:
			/// <br/>a. -1 if <c>object1</c> has better ranking then <c>object2</c>,
			/// <br/>b.  1 if <c>object2</c> has better ranking then <c>object1</c>,
			/// <br/>c.  0 if both <c>object1</c> <c>object2</c> has same ranking.</returns>
			inline int GACALL operator ()(GaConstType& object1,
				GaConstType& object2) const { return Compare( object1, object2 ); }

		protected:

			/// <summary><c>Compare</c> method operator compares two objects using adopted sorting criteria.</summary>
			/// <param name="object1">reference to the first object that is being compared.</param>
			/// <param name="object2">reference to the second object that is being compared.</param>
			/// <returns>Method should returns:
			/// <br/>a. -1 if <c>object1</c> has better ranking then <c>object2</c>,
			/// <br/>b.  1 if <c>object2</c> has better ranking then <c>object1</c>,
			/// <br/>c.  0 if both <c>object1</c> <c>object2</c> has same ranking.</returns>
			virtual int GACALL Compare(GaConstType& object1,
				GaConstType& object2) const = 0;

		};

		/// <summary><c>GaMutableSortCriteriaAdapter</c> template class provides interface adaption for sort criteria that are not derived from <c>GaMutableSortCriteria</c> class,
		/// so they can be used where run-time changing of criteria is required.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public method or private methods are thread-safe.</summary>
		/// <param name="CRITERIA">type of sort criteria that is adopted.</param>
		template<typename CRITERIA>
		class GaMutableSortCriteriaAdapter : public  GaMutableSortCriteria<typename CRITERIA::GaType>
		{

		public:

			/// <summary>Type of adopted criteria.</summary>
			typedef CRITERIA GaCriteria;

		private:

			/// <summary>Adopted criteria.</summary>
			GaCriteria _criteria;

		public:

			/// <summary>Adopts specified sort criteria.</summary>
			/// <param name="criteria">reference to criteria that should be adopted.</param>
			GaMutableSortCriteriaAdapter(const GaCriteria& criteria) : _criteria(criteria) { }

			/// <summary>Adopts default criteria.</summary>
			GaMutableSortCriteriaAdapter() { }

			/// <summary>More details are given in specification of <see cref="GaMutableSortCriteria::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaMutableSortCriteria<GaType>* Clone() const { return new GaMutableSortCriteria<GaType( *this ); }

			/// <summary><c>SetCriteria</c> method sets criteria that should be adopted.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="criteria">reference to criteria that should be adopted.</param>
			inline void GACALL SetCriteria(const GaCriteria& criteria) { _criteria = criteria; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to adopted sort criteria.</returns>
			inline GaCriteria& GetCriteria() { return _criteria; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to adopted sort criteria.</returns>
			inline const GaCriteria& GetCriteria() const { return _criteria; }

		protected:

			/// <summary>More details are given in specification of <see cref="GaMutableSortCriteria::Compare" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual int GACALL Compare(GaConstType& object1,
				GaConstType& object2) const { return _criteria( object1, object2 ); }

		};

		/// <summary><c>GaSortCriteriaConverter</c> template class provides way to sort items of one type using criteria implemented for another type of data
		/// by specifying object that converts/extracts required data from original items.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		/// <param name="INPUT_TYPE">tye of input items.</param>
		/// <param name="COMPARE_CRITERIA">actual sorting criteria that uses extracted data from input itmes.</param>
		/// <param name="CONVERTER">converter that extracts required data from input items passes them to actual sorting criteria.</param>
		template<typename INPUT_TYPE,
			typename COMPARE_CRITERIA,
			typename CONVERTER>
		class GaSortCriteriaConverter : public GaSortingCriteria<INPUT_TYPE>
		{

		public:

			/// <summary>Actual sorting criteria that uses extracted data from input itmes.</summary>
			typedef COMPARE_CRITERIA GaCompareCriteria;

			/// <summary></summary>
			typedef CONVERTER GaConverter;

		private:

			/// <summary>Sorting criteria that uses extracted data from input items.</summary>
			GaCompareCriteria _criteria;

			/// <summary>Converter that extracts data required by actual sorting criteria from input items.</summary>
			GaConverter _converter;

		public:

			/// <summary>Initializes sorting criteria with specified converter and actual criteria.</summary>
			/// <param name="criteria">sorting criteria that uses extracted data from input items.</param>
			/// <param name="converter">converter that extracts data required by actual sorting criteria from input items</param>
			GaSortCriteriaConverter(const GaCompareCriteria& criteria,
				const GaConverter& converter) : _criteria(criteria),
				_converter(converter) { }

			/// <summary>Initializes sorting criteria with actual criteria.</summary>
			/// <param name="criteria">sorting criteria that uses extracted data from input items.</param>
			GaSortCriteriaConverter(const GaCompareCriteria& criteria) : _criteria(criteria) { }

			/// <summary>Initializes sorting criteria with specified converter.</summary>
			/// <param name="converter">converter that extracts data required by actual sorting criteria from input items</param>
			GaSortCriteriaConverter(const GaConverter& converter) : _converter(converter) { }

			/// <summary>Initializes sorting criteria with default behaviour.</summary>
			GaSortCriteriaConverter() { }

			/// <summary><c>Compare</c> method operator compares two objects using actual sorting criteria.</summary>
			/// <param name="object1">reference to the first object that is being compared.</param>
			/// <param name="object2">reference to the second object that is being compared.</param>
			/// <returns>Method should returns:
			/// <br/>a. -1 if <c>object1</c> has better ranking then <c>object2</c>,
			/// <br/>b.  1 if <c>object2</c> has better ranking then <c>object1</c>,
			/// <br/>c.  0 if both <c>object1</c> <c>object2</c> has same ranking.</returns>
			inline int GACALL operator ()(GaConstType& object1,
				GaConstType& object2) const { return _criteria( _converter( object1 ), _converter( object2 ) ); }

		};

		/// <summary><c>GaMergeSortAlgorithm</c> template class implements mergesort algorithm for sorting arrays.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="T">type of data in array that should be sorted.</param>
		template<typename T>
		class GaMergeSortAlgorithm
		{

		private:

			/// <summary>Pointer to the array that should be sorted.</summary>
			T* _array;

			/// <summary>Helper array needed by mergesort algorithm.</summary>
			T* _helperArray;

			/// <summary>Pointer to varibale that sotres number of elements currently in the array.</summary>
			int* _count;

			/// <summary>Indicates that helper array can returned as sorted array instead of original to improve performance.</summary>
			bool _swapable;

		public:

			/// <summary>Initializes mergesort algorithm with no array for sorting.</summary>
			/// <param name="swapable">if this parameter is set to <c>true</c> it indicates that helper array can returned as sorted array
			/// instead of original to improve performance.</param>
			GaMergeSortAlgorithm(bool swapable) : _array(NULL),
				_helperArray(NULL),
				_count(NULL),
				_swapable(swapable) { }

			/// <summary>Initializes mergesort algorithm with an array on which the algorithm should operate and creates helper array.</summary>
			/// <param name="array">pointer to the array that should be sorted.</param>
			/// <param name="helperArray">pointer to helper array.</param>
			/// <param name="count">pointer to varibale that sotres number of elements currently in the array.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>size</c> is negative or if it is 0 but <c>array</c> is not <c>NULL</c>.</exception>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>array</c> or <c>count</c> is <c>NULL</c> but the size is greater then 0.</exception>
			/// <param name="swapable">if this parameter is set to <c>true</c> it indicates that helper array can returned as sorted array
			/// instead of original to improve performance.</param>
			GaMergeSortAlgorithm(T* array,
				T* helperArray,
				int* count,
				bool swapable) : _swapable(swapable) { SetArray( array, helperArray, count ); }

			/// <summary><c>Sort</c> method sorts array using provided sorting criteria. Algorithm may take ownership of he original array
			/// and return heler array as sorted one if it improves performances and user allowed taking of the ownership.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="CRITERIA">type of sorting criteria used for sorting.</param>
			/// <param name="criteria">reference to criteria used to sort array.</param>
			/// <returns>Method returns pointer to sorted array (sorted array is not nesseserily the same memory location as original one).</returns>
			template<typename CRITERIA>
			T* Sort(const CRITERIA& criteria)
			{
				T* sortedArray = Sort( _array, *_count, _helperArray, criteria );

				// resulting array is not same as original?
				if( sortedArray != _array )
				{
					if( _swapable )
					{
						// only swap the arrays to improve performance
						_helperArray = _array;
						_array = sortedArray;
					}
					else
					{
						// copy sorted array to original array
						for( int i = *_count - 1; i >= 0; i-- )
							_array[ i ] = _helperArray[ i ];

						return _array;
					}
				}

				return sortedArray;
			}

			/// <summary><c>Sort</c> method sorts only a portion of the array using provided sorting criteria. Algorithm may take ownership of he original array
			/// and return heler array as sorted one if it improves performances and user allowed taking of the ownership.
			/// Method can be executed concurently from different threads only when sorting non-overlapping portions of the array.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="CRITERIA">type of sorting criteria used for sorting.</param>
			/// <param name="begin">index of the first element in the array that should be sorted.</param>
			/// <param name="end">index of the last element in the array that hsould be sorted.</param>
			/// <param name="criteria">reference to criteria used to sort array.</param>
			/// <returns>Method returns pointer to sorted array.</returns>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>begin</c> is below 0 or above <c>end</c> or
			/// if <c>end</c> is below <c>begin</c> or above count of elements in the array.</exception>
			template<typename CRITERIA>
			T* Sort(int begin,
				int end,
				const CRITERIA& criteria)
			{
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, begin >= 0 || begin < end, "begin", "Start index of sorting must be in range (0, end].", "Random" );
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, end < *_count, "end", "End index must be in range [begin, count of elements]", "Random" );

				// size of portion of the array that should be sorted.
				int size = end - begin + 1;
				if( size != *_count )
				{
					T* array = _array + begin;

					// determinate portion of helper array that should be used to sort original array
					T* helperArray = _helperArray + begin;
					T* helperArrayEnd = _helperArray + size;

					T* sortedArray = Sort( array, size, helperArray, criteria );

					// resulting array is not same as original?
					if( sortedArray != array )
					{
						// copy sorted elements from helper array to the original array
						while( helperArray < helperArrayEnd )
							*( array++ ) = *( helperArray++ );
					}

					return _array;
				}
				else
					return Sort( criteria );
			}

			/// <summary><c>SetArray</c> method sets pointer to array that should be sorted by the algorithm and its size as well as pointer
			/// to variable that stores current number of elements in the array.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="array">pointer to the array that should be sorted.</param>
			/// <param name="helperArray">pointer to helper array.</param>
			/// <param name="count">pointer to varibale that sotres number of elements currently in the array.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>size</c> is negative or if it is 0 but <c>array</c> is not <c>NULL</c>.</exception>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>array</c> or <c>helperArray</c> is <c>NULL</c> but the size is greater then 0 or
			/// when <c>criteria</c> is set to <c>NULL</c>.</exception>
			void SetArray(T* array,
				T* helperArray,
				int* count)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, array == NULL || helperArray != NULL, "helperArray", "Helper array for sorting must be provided.", "Random" );
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, array == NULL || count != NULL,
					"count", "Variable that store number of elements in the array must be specified.", "Random" );

				_array = array;
				_helperArray = helperArray;
				_count = count;
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>This method returns pointer to array on which algorithm should sort.</returns>
			inline T* GetArray() { return _array; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>This method returns pointer to array on which algorithm should sort.</returns>
			inline const T* GetArray() const { return _array; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the helper array used by mergesrot algorithm.</returns>
			inline T* GetHelperArray() { return _helperArray; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the helper array used by mergesrot algorithm.</returns>
			inline const T* GetHelperArray() const { return _helperArray; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of elements in the array.</returns>
			inline int GetCount() const { return *_count; }

			/// <summary><c>SetSwapable</c> method instructs sorting algorithm whether it can return helper array as sorted array insted of original to improve performance.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="swapable">if this parameter is set to <c>true</c> it indicates that helper array can returned as sorted array.</param>
			inline void SetSwapable(bool swapable) { _swapable = swapable; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if sorting algorithm can return helper array as sorted array instead of original to improve performance.</returns>
			inline bool IsSwapable() const { return _swapable; }

		private:

			/// <summary><c>Sort</c> method sorts specified portion of the array using provided sorting criteria.
			/// Method can be used concurently only when sorting non-overlapping portions of the array.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="CRITERIA">type of sorting criteria used for sorting.</param>
			/// <param name="array">pointer to the first element of the array that should be sorted.</param>
			/// <param name="arraySize">number of elements of the array that should be sorted.</param>
			/// <param name="helperArray">pointer to the first element of helper array used for sorting.</param>
			/// <param name="criteria">reference to criteria used to sort array.</param>
			/// <returns>Method returns pointer to sorted array (sorted array is not nesseserily the same memory location as original one).</returns>
			template<typename CRITERIA>
			T* Sort(T* array,
				int arraySize,
				T* helperArray,
				const CRITERIA& criteria)
			{
				if( arraySize < 2 )
					return array;

				T* helperArrayEnd = helperArray + arraySize;
				T* mergeDestination = helperArray;

				// perform merging of sorted blocks of array, each step doubles size of merging blocks
				for( int blockSize = 1; blockSize < arraySize; blockSize *= 2 )
				{
					// beginings of two sorted blocks of the array that should be merged
					T* mergeBlock1 = mergeDestination == helperArray ? array : helperArray;
					T* mergeBlock2 = mergeBlock1 + blockSize;

					T* mergeSourceEnd = mergeBlock1 + arraySize;
					T* mergeDestinationEnd = mergeDestination + arraySize;

					// until all pairs of blocks are merged
					while( mergeBlock2 < mergeSourceEnd )
					{
						// ends of two sorted blocks of the array that should be merged
						T* mergeBlock1End = mergeBlock2 - 1;
						T* mergeBlock2End = mergeBlock2 + blockSize - 1;
						if( mergeBlock2End >= mergeSourceEnd )
							mergeBlock2End = mergeSourceEnd - 1;

						// merge blocks to helper array preserving sorting
						for( ; mergeBlock1 <= mergeBlock1End && mergeBlock2 <= mergeBlock2End; )
							*( mergeDestination++ ) = criteria( *mergeBlock1, *mergeBlock2 ) < 0 ? *( mergeBlock1++ ) : *( mergeBlock2++ );

						// copy any element left in merging blocks to helper array
						while( mergeBlock1 <= mergeBlock1End )
							*( mergeDestination++ ) = *( mergeBlock1++ );
						while( mergeBlock2 <= mergeBlock2End )
							*( mergeDestination++ ) = *( mergeBlock2++ );

						// move to the next pair of block
						mergeBlock1 = mergeBlock2End + 1;
						mergeBlock2 = mergeBlock1 + blockSize;
					}

					// copy last block that is unpaired
					while( mergeDestination < mergeDestinationEnd )
						*( mergeDestination++ ) = *( mergeBlock1++ );

					// swap helper array and original array
					mergeDestination = mergeDestination == helperArrayEnd ? array : helperArray;
				}

				// return pointer to array that contains elements in sorted order
				return mergeDestination == array ? helperArray : array;
			}

		};

	} // Sorting
} // Common

#endif // __GA_SORTING_H__

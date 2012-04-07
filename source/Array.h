
/*! \file Array.h
	\brief This file declares and implements classes that represents different types of arrays.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_ARRAY_H__
#define __GA_ARRAY_H__

#include "SysInfo.h"
#include "SmartPtr.h"

/// <summary><c>GaAllocatorHelper</c> struct is used for detonating custom <c>new</c> and <c>delete</c> operators for managing arrays' memory.</summary>
struct GaAllocatorHelper { };

/// <summary>This version of <c>operator new</c> uses memory acquired by another mean and allows it be treated as proper array of elements.</summary>
/// <param name="size">size of the memory chunk.</param>
/// <param name="address">adddress of acquired memory chunk.</param>
/// <param name="x">dummy paramenter that indicates that custom operator is used.</param>
/// <returns>Operator returns pointer to raw memory that is converted to an array.</returns>
inline void* operator new(size_t size,
	void* address,
	GaAllocatorHelper x) { return address; }

/// <summary>This version of <c>operator delete</c> treats memory used by and array as raw memory.</summary>
/// <param name="address1">pointer to the first element of the array.</param>
/// <param name="address2">dummy parameter.</param>
/// <param name="x">dummy paramenter that indicates that custom operator is used.</param>
inline void operator delete(void* address1,
	void* address2,
	GaAllocatorHelper x) { ::operator delete( address1 ); }

namespace Common
{
	namespace Data
	{

		/// <summary><c>GaScalarType</c> struct is used for detonating functions and methods which are specific to scalar types.</summary>
		struct GaScalarType { };

		/// <summary><c>GaNonscalarType</c> struct is used for detonating functions and methods which are specific to nonscalar types.</summary>
		struct GaNonscalarType { };

		/// <summary>This verison of <c>GaTypeCategoryHelper</c> function is triggered by scalar types.</summary>
		/// <param name="T">type of object.</param>
		/// <param name="...">dummy paramenter.</param>
		/// <returns>Method returns helper object which indicates that provided type is scalar type.</returns>
		template<typename T>
		inline static GaScalarType GaTypeCategoryHelper(...) { return GaScalarType(); }

		/// <summary>This verison of <c>GaTypeCategoryHelper</c> function is triggered by nonscalar types.</summary>
		/// <param name="T">type of object.</param>
		/// <returns>Method returns helper object which indicates that provided type is not scalar type.</returns>
		template<typename T>
		inline static GaNonscalarType GaTypeCategoryHelper(void ( T::* )()) { return GaNonscalarType(); }

		/// <summary><c>GaConstructObject</c> method constructs and initialize object of specified type on provided address.</summary>
		/// <param name="T">type object that should be constructed.</param>
		/// <param name="addr">address on whic the object should be constructed.</param>
		/// <param name="val">value to which the object should be initialized.</param>
		template<typename T>
		inline void GACALL GaConstructObject(void* addr,
			const T& val) { new ( addr, GaAllocatorHelper() ) T( val ); }

		/// <summary><c>GaDestructObject</c> method destorys objects of nonscalar types.</summary>
		/// <param name="T">type object that should be destoryed.</param>
		/// <param name="data">pointer to object that should be destoryed.</param>
		/// <param name="t">dummy paramenter that indicates that object destruction is trigered by nonscalar type.</param>
		template<typename T>
		inline void GaDestructObject(T* data,
			GaNonscalarType t) { data->~T(); }

		/// <summary><c>GaDestructObject</c> method destorys objects of scalar types.</summary>
		/// <param name="T">type object that should be destoryed.</param>
		/// <param name="data">pointer to object that should be destoryed.</param>
		/// <param name="t">dummy paramenter that indicates that object destruction is trigered by nonscalar type.</param>
		template<typename T>
		inline void GaDestructObject(T* data,
			GaScalarType t) { }

		/// <summary><c>GaDestructObject</c> function destorys objects stored in an array.</summary>
		/// <param name="T">type of object that should be destoryed.</param>
		/// <param name="data">pointer to object that should be destoryed.</param>
		template<typename T>
		inline void GaDestructObject(T* data) { GaDestructObject( data, GaTypeCategoryHelper<T>( 0 ) ); }

		/// <summary><c>GaGetObject</c> method calculates address of an element in the array at specified index.</summary>
		/// <param name="T">type of elements in the array.</param>
		/// <param name="arr">address of the first element.</param>
		/// <param name="index">index of the element.</param>
		/// <param name="step">number of bytes that is required for a single element.</param>
		template<typename T>
		inline T* GACALL GaGetObject(T* arr,
			int index,
			int step) { return (T*)( (const char*)arr + index * step ); }

		/// <summary><c>GaDestructArray</c> method destructs elements of the specified array.</summary>
		/// <param name="T">type of elements in the array.</param>
		/// <param name="arr">address of the first element.</param>
		/// <param name="count">number of elements in the array.</param>
		/// <param name="step">number of bytes that is required for a single element.</param>
		template<typename T>
		inline void GACALL GaDestructArray(T* arr,
			int count,
			int step = sizeof(T))
		{
			// destroy objects
			for( int i = count - 1; i >= 0; i-- )
				GaDestructObject( GaGetObject( arr, i, step ) );
		}

		/// <summary><c>GaInitializeArray</c> method constructs and initializes elements of the array.</summary>
		/// <param name="T">type of elements in the array.</param>
		/// <param name="dst">address of the first element.</param>
		/// <param name="count">number of elements in the array.</param>
		/// <param name="step">number of bytes that is required for a single element.</param>
		/// <param name="defaultValue">value to which all elements in the array should be initialized.</param>
		template<typename T>
		void GACALL GaInitializeArray(T* dst,
			int count,
			int step = sizeof(T),
			const T& defaultValue = T())
		{
			int i = count - 1;
			try
			{
				// construct and initialize elements of the array
				for( ; i >= 0; i-- )
					GaConstructObject( GaGetObject( dst, i, step ), defaultValue );
			}
			catch( ... )
			{
				// destroy objects that has been constructed
				GaDestructArray( GaGetObject( dst, i, step ), count - i + 1, step );
				throw;
			}
		}

		/// <summary><c>GaCopyArray</c> method copies elements from one array to another.</summary>
		/// <param name="T">type of elements in the arrays.</param>
		/// <param name="dst">address of the first element of destination array.</param>
		/// <param name="src">address of the first element of source array.</param>
		/// <param name="count">number of elements that should be copied.</param>
		/// <param name="dstStep">number of bytes that is required for a single element in destination array.</param>
		/// <param name="srcStep">number of bytes that is required for a single element in source array.</param>
		template<typename T>
		void GACALL GaCopyArray(T* dst,
			const T* src,
			int count,
			int dstStep = sizeof(T),
			int srcStep = sizeof(T))
		{
			int i = count - 1;
			try
			{
				// construct and copy elements of the array
				for( ; i >= 0; i-- )
					GaConstructObject( GaGetObject( dst, i, dstStep ), *GaGetObject( src, i, srcStep ) );
			}
			catch( ... )
			{
				// destroy objects that has been constructed
				GaDestructArray( GaGetObject( dst, i, dstStep ), count - i, dstStep );
				throw;
			}
		}

		/// <summary><c>GaArrayBase</c> is base class for handling arrays that stores elements of specified type.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaArrayBase
		{

		public:

			/// <summary>Defines type of base array structure for inherited classes.</summary>
			typedef GaArrayBase GaBaseStructure;

		protected:

			/// <summary>Number of elements that array has.</summary>
			int _size;

		public:

			/// <summary>This constiructor initializes array with size of zero.</summary>
			GaArrayBase() : _size(0) { }

			/// <summary>This constructor initializes array by copying another one.</summary>
			/// <param name="rhs">reference to array that should be copied.</param>
			GaArrayBase(const GaArrayBase& rhs) : _size(rhs._size) { }

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaArrayBase() { }

			/// <summary><c>Clone</c> method creates new array of the same size and copies elements in it.</summary>
			virtual GaArrayBase* GACALL Clone() const = 0;

			/// <summary><c>Copy</c> method destorys all elements currently in the array and copies elements from another array.</summary>
			/// <param name="source">array from which the elements is copied.</param>
			virtual void GACALL Copy(const GaArrayBase* source) = 0;

			/// <summary><c>Copy</c> method copies only defined range of elements from source array to specified location in the destination.</summary>
			/// <param name="source">array from which the elements is copied.</param>
			/// <param name="sourceStart">index of the first element in the source array that should be copied.</param>
			/// <param name="destinationStart">index in destination array at which the copying should start.</param>
			/// <param name="size">number of elements that should be copied.</param>
			virtual void GACALL Copy(const GaArrayBase* source,
				int sourceStart,
				int destinationStart,
				int size) = 0;

			/// <summary><c>Clear</c> method destroys all elements in the array and resizes it to zero size.</summary>
			virtual void GACALL Clear() = 0;

			/// <summary><c>Swap</c> method swaps positions of the two elements.</summary>
			/// <param name="valueIndex1">index of the first element that should be swapped.</param>
			/// <param name="valueIndex2">index of the second element that should be swapped.</param>
			virtual void GACALL Swap(int valueIndex1,
				int valueIndex2) = 0;

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of elements that array has.</returns>
			inline int GACALL GetSize() const { return _size; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if array size is zero.</returns>
			inline bool GACALL IsEmpty() const { return _size == 0; }

		private:

			/// <summary>Disables <c>operator =</c> on arrays for which the exact type is unknown.</summary>
			GaArrayBase& GACALL operator =(const GaArrayBase& rhs) { return *this; }

		};

		/// <summary><c>GaArray</c> template class provide basic services for handling arrays of known types.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of elements stored in the array.</param>
		template<typename VALUE_TYPE>
		class GaArray : public GaArrayBase
		{

		public:

			/// <summary>Type of elements stored in the array.</summary>
			typedef VALUE_TYPE GaValueType; 

		protected:

			/// <summary>Pointer to memory used by the elements of the array.</summary>
			GaValueType* _array;

			/// <summary>Size of allocated memory for the array in a number of elements that it can store.</summary>
			int _allocatedSize;

		public:

			/// <summary>This constiructor initializes array with size of zero.</summary>
			GaArray() : _array(NULL),
				_allocatedSize(0) { }

			/// <summary>This constructor initializes array by copying another one.</summary>
			/// <param name="rhs">reference to array that should be copied.</param>
			GaArray(const GaArray<GaValueType>& rhs) : _array(NULL),
				_allocatedSize(0) { GaArray<GaValueType>::Copy( &rhs ); }

			/// <summary>Destorys elements of the array and free used memory.</summary>
			virtual ~GaArray() { Clear(); }

			/// <summary>More details are given in specification of <see cref="GaArrayBase::Copy" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>source</c> is set to <c>NULL</c>.</exception>
			virtual void GACALL Copy(const GaArrayBase* source)
			{
				if( source != this )
				{
					GA_ARG_ASSERT( Exceptions::GaNullArgumentException, source != NULL, "source", "Source array must be specified.", "Data" );

					GaArray<GaValueType>* src = (GaArray<GaValueType>*)source;

					if( src->_array )
					{
						if( src->_size > _allocatedSize )
						{
							// allocate memory large enough to store source elements and copy them then destroy existing elements in the destination
							Memory::GaAutoPtr<char> memory = src->AllocateMemory();
							GaCopyArray( (GaValueType*)memory.GetRawPtr(), src->_array, src->_size );
							Clear();

							// apply new array
							_array = (GaValueType*)memory.DetachPointer();
							_size = src->_size;
							_allocatedSize = _size;
						}
						else
						{
							// destory existing elements and copy elements from the source
							GaDestructArray( _array, _size );
							GaCopyArray( _array, src->_array, src->_size );

							// apply new array
							_size = src->_size;
						}
					}
					else
						Clear();
				}
			}

			/// <summary>More details are given in specification of <see cref="GaArrayBase::Copy" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>source</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>sourceStart</c> or <c>destinationStart</c> are negative values or greater or equal to sizes of arrays.
			/// It is also thrown if <c>size</c> parameter will cause copying to go over limits of arrays.</exception>
			virtual void GACALL Copy(const GaArrayBase* source,
				int sourceStart,
				int destinationStart,
				int size)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, source != NULL, "source", "Source array must be specified.", "Data" );
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, sourceStart >= 0 && sourceStart < source->GetSize(), "sourceStart", "Source index is out of range.", "Data" );
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, destinationStart >= 0 && destinationStart < _size,
					"destinationStart", "Destination index is out of range.", "Data" );
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, size >= 0 && size <= source->GetSize() - sourceStart && size <= _size - destinationStart,
					"size", "Size is out of range.", "Data" );

				// copy elements from source
				for( int i = size - 1; i >= 0; i-- )
					_array[ destinationStart++ ] = ( (GaArray<GaValueType>*)source )->_array[ sourceStart++ ];
			}

			/// <summary>More details are given in specification of <see cref="GaArrayBase::Clear" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Clear()
			{
				if( _array )
				{
					GaDestructArray( _array, _size );

					// free memory used by the array
					delete[] (char*)_array;

					_array = NULL;
					_size = 0;
				}
			}

			/// <summary>More details are given in specification of <see cref="GaArrayBase::Swap" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Swap(int valueIndex1,
				int valueIndex2)
			{
				// swap element positions
				GaValueType temp = _array[ valueIndex1 ];
				_array[ valueIndex1 ] = _array[ valueIndex2 ];
				_array[ valueIndex2 ] = temp;
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the first element of the array.</returns>
			inline GaValueType* GACALL GetArray() { return _array; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the first element of the array.</returns>
			inline const GaValueType* GACALL GetArray() const { return _array; }

			/// <summary><c>SetAt</c> method sets element at specified position in the array.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="index">element that should be set.</param>
			/// <param name="value">new value of the element.</param>
			inline void GACALL SetAt(int index,
				const GaValueType& value) { _array[ index ] = value; }

			/// <summary><c>GetAt</c> method returns value of element at specified index in the array.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="index">index of queried element.</param>
			/// <returns>Method returns reference to queried element.</returns>
			inline GaValueType& GACALL GetAt(int index) { return _array[ index ]; }

			/// <summary><c>GetAt</c> method returns value of element at specified index in the array.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="index">index of queried element.</param>
			/// <returns>Method returns reference to queried element.</returns>
			inline const GaValueType& GACALL GetAt(int index) const { return _array[ index ]; }

			/// <summary><c>operator []</c> returns value of element at specified index in the array.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="index">index of queried element.</param>
			/// <returns>Operator returns reference to queried element.</returns>
			inline GaValueType& GACALL operator [](int index) { return _array[ index ]; }

			/// <summary><c>operator []</c> returns value of element at specified index in the array.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="index">index of queried element.</param>
			/// <returns>Operator returns reference to queried element.</returns>
			inline const GaValueType& GACALL operator [](int index) const { return _array[ index ]; }

		protected:

			/// <summary><c>AllocateMemory</c> method allocates memory to large enough to store defined number of elements.</summary>
			/// <param name="size">number of elements that should be stored in the memory location.</param>
			/// <returns>Method returns auto pointer to allocated block.</returns>
			inline Memory::GaAutoPtr<char> GACALL AllocateMemory(int size)
				{ return Memory::GaAutoPtr<char>( new char[ size * sizeof( GaValueType ) ], Memory::GaArrayDeletionPolicy<char>::GetInstance() ); }

			/// <summary><c>AllocateMemory</c> method allocates memory to large enough to store current number of elements.</summary>
			/// <returns>Method returns auto pointer to allocated block.</returns>
			inline Memory::GaAutoPtr<char> GACALL AllocateMemory() { return AllocateMemory( _size ); }

		private:

			/// <summary>Disables <c>operator =</c> on arrays for which the exact type is unknown.</summary>
			GaArray<GaValueType>& GACALL operator =(const GaArray<GaValueType>& rhs) { return *this; }

		};

		/// <summary><c>GaSingleDimensionArray</c> template class handles single-dimensional arrays.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of elements stored in the array.</param>
		template<typename VALUE_TYPE>
		class GaSingleDimensionArray : public GaArray<VALUE_TYPE>
		{

		public:

			/// <summary>This constructor initializes array with specified size and elements sets to user-defined value.</summary>
			/// <param name="source">values to which elements of the new array should be set.</param>
			/// <param name="size">size of the array.</param>
			GaSingleDimensionArray(const GaValueType* source,
				int size)
			{
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, size >= 0, "size", "Size cannot be negative.", "Data" );
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, size == 0 || source != NULL , "source", "Source must be specified.", "Data" );

				if( source )
				{
					// allocate memory for the array and copy elements from the source
					Memory::GaAutoPtr<char> memory = AllocateMemory( size );
					GaCopyArray( (GaValueType*)memory.GetRawPtr(), source, size );

					// apply new array
					_array = (GaValueType*)memory.DetachPointer();
					_size = size;
				}
			}

			/// <summary>This constructor initializes array with specified size and all elements are set to default value for the type.</summary>
			/// <param name="size">size of the array.</param>
			/// <param name="defaultValue">default value to which the elements in the array will be initialized.</param>
			GaSingleDimensionArray(int size,
				const GaValueType& defaultValue = GaValueType()) { SetSize( size, defaultValue ); }

			/// <summary>This constructor initializes array by copying another one.</summary>
			/// <param name="rhs">reference to array that should be copied.</param>
			GaSingleDimensionArray(const GaSingleDimensionArray<GaValueType>& rhs) : GaArray<GaValueType>(rhs) { }

			/// <summary>This constiructor initializes array with size of zero.</summary>
			GaSingleDimensionArray() { }

			/// <summary>More details are given in specification of <see cref="GaArrayBase::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaArrayBase* GACALL Clone() const { return new GaSingleDimensionArray<GaValueType>( *this ); }

			/// <summary><c>SetSize</c> method sets size of the array.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="size">new size of the array.</param>
			/// <param name="defaultValue">default value to which the new elements of the arry array will be initialized.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>size</c> is negative value.</exception>
			void GACALL SetSize(int size,
				const GaValueType& defaultValue = GaValueType())
			{
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, size >= 0, "size", "Size cannot be negative.", "Data" );

				if( size )
				{
					if( size > _allocatedSize )
					{
						// allocate memory that can store all new elements
						Memory::GaAutoPtr<char> memory = AllocateMemory( size );

						// copy elements that will be kept in the array from old memory
						int limit = size <= _size ? size : _size;
						GaCopyArray( (GaValueType*)memory.GetRawPtr(), _array, limit );

						// fill newly added elements and destroy elements stored at old location
						GaInitializeArray( (GaValueType*)memory.GetRawPtr() + limit, size - limit, sizeof( GaValueType ), defaultValue );
						Clear();

						// apply new array
						_array = (GaValueType*)memory.DetachPointer();
						_size = size;
						_allocatedSize = _size;
					}
					else if( size > _size )
						// intialize new elements
						GaInitializeArray( _array + _size, size - _size, sizeof( GaValueType ), defaultValue );
					else
						// destory unneeded elements
						GaDestructArray( _array + size, _size - size );
				}
				else
					Clear();
			}

			/// <summary><c>operator =</c> clears current content of the array and copies element from another.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="rhs">array that should be copied.</param>
			/// <returns>Method returns reference to <c>this</c> object.</returns>
			GaSingleDimensionArray<GaValueType>& GACALL operator =(const GaSingleDimensionArray<GaValueType>& rhs) { GaSingleDimensionArray<GaValueType>::Copy( &rhs ); return *this; }

		};

		/// <summary><c>operator ==</c> compares elements of the two single-dimensional arrays.</summary>
		/// <param name="VALUE_TYPE">type of elements stored in the arrays.</param>
		/// <param name="lhs">the first array.</param>
		/// <param name="rhs">the second array.</param>
		/// <returns>Method returns <c>true</c> if elements in both arrays are equal.</returns>
		template<typename VALUE_TYPE>
		bool GACALL operator ==(const GaSingleDimensionArray<VALUE_TYPE>& lhs,
			const GaSingleDimensionArray<VALUE_TYPE>& rhs)
		{
			// if sizes are not equal - arrays are not equal
			if( lhs.GetSize() != rhs.GetSize() )
				return false;

			// compare pairs of elements
			for( int i = lhs.GetSize() - 1; i >= 0; i-- )
			{
				if( lhs[ i ] != rhs[ i ] )
					return false;
			}

			return true;
		}

		/// <summary><c>operator !=</c> compares elements of the two single-dimensional arrays.</summary>
		/// <param name="VALUE_TYPE">type of elements stored in the arrays.</param>
		/// <param name="lhs">the first array.</param>
		/// <param name="rhs">the second array.</param>
		/// <returns>Method returns <c>true</c> if arrays conatin elements that are not equal.</returns>
		template<typename VALUE_TYPE>
		inline bool GACALL operator !=(const GaSingleDimensionArray<VALUE_TYPE>& lhs,
			const GaSingleDimensionArray<VALUE_TYPE>& rhs) { return !( lhs == rhs ); }

		/// <summary><c>GaAlignedArray</c> template class handles single-dimensional arrays whose elements should be aligned on specified boundary.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of elements stored in the array.</param>
		template<typename DATA_TYPE>
		class GaAlignedArray : public GaArrayBase
		{

		public:

			/// <summary>Type of elements stored in the array.</summary>
			typedef DATA_TYPE GaValueType;

		private:

			/// <summary>Boundary on which the elements of the array should be aligned.</summary>
			int _alignment;

			/// <summary>Size of the element with padding required for next element in the array to be aligned.</summary>
			int _elementSize;

			/// <summary>Address of memory block alocated for storing elements.</summary>
			char* _memoryBlock;

			/// <summary>Address of the first element of the array, aligned on required boundary.</summary>
			char* _arrayStart;

		public:

			/// <summary>This constructor initializes array with specified size and alignment and all elements are set to default value for the type.</summary>
			/// <param name="size">size of the array.</param>
			/// <param name="alignment">boundary on which the elements of the array should be aligned.</param>
			/// <param name="defaultValue">default value to which the elements in the array will be initialized.</param>
			GaAlignedArray(int size,
				int alignment,
				const GaValueType& defaultValue = GaValueType()) : _alignment(alignment),
				_memoryBlock(NULL),
				_arrayStart(NULL)
			{
				// caluclate required padding
				_elementSize = GetElementSize();

				SetSize( size, defaultValue );
			}

			/// <summary>This constructor initializes array with specified size and all elements are set to default value for the type.</summary>
			/// <param name="size">size of the array.</param>
			/// <param name="defaultValue">default value to which the elements in the array will be initialized.</param>
			GaAlignedArray(int size,
				const GaValueType& defaultValue = GaValueType()) : _memoryBlock(NULL),
				_arrayStart(NULL)
			{
				// caluclate required padding
				_alignment = GaCpuInfo::GetInstance().GetCacheLineSize();
				_elementSize = GetElementSize();

				SetSize( size, defaultValue );
			}

			/// <summary>This constructor initializes array by copying another one.</summary>
			/// <param name="rhs">reference to array that should be copied.</param>
			GaAlignedArray(const GaAlignedArray<GaValueType>& rhs) : GaArrayBase(rhs),
				_alignment(rhs._alignment),
				_elementSize(rhs._elementSize),
				_memoryBlock(NULL),
				_arrayStart(NULL)
			{
				if( rhs._memoryBlock )
				{
					// allocate memory large enough to store source elements
					Memory::GaAutoPtr<char> memory = AllocateMemory();
					char* start = GetArrayStart( memory.GetRawPtr() );

					// copy elemnts from the source array
					GaCopyArray( (GaValueType*)start, (GaValueType*)rhs._arrayStart, _size, _elementSize, _elementSize );

					// apply new array
					_memoryBlock = memory.DetachPointer();
					_arrayStart = start;
				}
			}

			/// <summary>This constiructor initializes array with size of zero.</summary>
			GaAlignedArray() : _memoryBlock(NULL),
				_arrayStart(NULL)
			{
				// caluclate required padding
				_alignment = GaCpuInfo::GetInstance().GetCacheLineSize();
				_elementSize = GetElementSize();
			}

			/// <summary>Destorys elements of the array and free used memory.</summary>
			virtual ~GaAlignedArray() { Clear(); }

			/// <summary>More details are given in specification of <see cref="GaArrayBase::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaArrayBase* GACALL Clone() const { return new GaAlignedArray<GaValueType>( *this ); }

			/// <summary>More details are given in specification of <see cref="GaArrayBase::Copy" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>source</c> is set to <c>NULL</c>.</exception>
			virtual void GACALL Copy(const GaArrayBase* source)
			{
				if( source != this )
				{
					GA_ARG_ASSERT( Exceptions::GaNullArgumentException, source != NULL, "source", "Source array must be specified.", "Data" );

					GaAlignedArray<GaValueType>* src = (GaAlignedArray<GaValueType>*)source;

					if( src->_memoryBlock )
					{
						// allocate memory large enough to store source elements
						Memory::GaAutoPtr<char> memory = src->AllocateMemory();
						char* start = GetArrayStart( memory.GetRawPtr() );

						// copy source elements then destroy existing elements in the destination
						GaCopyArray( (GaValueType*)start, (GaValueType*)src->_arrayStart, src->_size, src->_elementSize, src->_elementSize );
						Clear();

						// apply new array
						_memoryBlock = memory.DetachPointer();
						_arrayStart = start;
						_alignment = src->_alignment;
						_elementSize = src->_elementSize;
						_size = src->_size;
					}
					else
						Clear();
				}
			}

			/// <summary>More details are given in specification of <see cref="GaArrayBase::Copy" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>source</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>sourceStart</c> or <c>destinationStart</c> are negative values or greater or equal to sizes of arrays.
			/// It is also thrown if <c>size</c> parameter will cause copying to go over limits of arrays.</exception>
			virtual void GACALL Copy(const GaArrayBase* source,
				int sourceStart,
				int destinationStart,
				int size)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, source != NULL, "source", "Source array must be specified.", "Data" );
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, sourceStart >= 0 && sourceStart < source->GetSize(), "sourceStart", "Source index is out of range.", "Data" );
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, destinationStart >= 0 && destinationStart < _size,
					"destinationStart", "Destination index is out of range.", "Data" );
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, size >= 0 && size <= source->GetSize() - sourceStart && size <= _size - destinationStart,
					"size", "Size is out of range.", "Data" );

				// copy elements from source
				for( int i = size - 1; i >= 0; i-- )
					( *this )[ destinationStart++ ] = ( (GaAlignedArray<GaValueType>&)*source )[ sourceStart++ ];
			}

			/// <summary>More details are given in specification of <see cref="GaArrayBase::Clear" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Clear()
			{
				if( _memoryBlock )
				{
					// call destructors for all elements and free memory
					GaDestructArray( (GaValueType*)_arrayStart, _size, _elementSize );
					delete[] _memoryBlock;

					_memoryBlock = _arrayStart = NULL;
					_size = 0;
				}
			}

			/// <summary>More details are given in specification of <see cref="GaArrayBase::Swap" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Swap(int valueIndex1,
				int valueIndex2)
			{
				GaValueType tmp = ( *this )[ valueIndex1 ];
				( *this )[ valueIndex1 ] = ( *this )[ valueIndex2 ];
				( *this )[ valueIndex2 ] = tmp;
			}

			/// <summary><c>SetSize</c> method sets size of the array.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="size">new size of the array.</param>
			/// <param name="defaultValue">default value to which the new elements of the arry array will be initialized.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>size</c> is negative value.</exception>
			void GACALL SetSize(int size,
				const GaValueType& defaultValue = GaValueType())
			{
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, size >= 0, "size", "Size cannot be negative.", "Data" );

				if( _size != size )
				{
					if( size )
					{
						// allocate memory that can store all new elements
						Memory::GaAutoPtr<char> memory = AllocateMemory( size );
						char* start = GetArrayStart( memory.GetRawPtr() );

						// copy elements that will be kept in the array from old memory
						int limit = size <= _size ? size : _size;
						GaCopyArray( (GaValueType*)start, (GaValueType*)_arrayStart, limit, _elementSize, _elementSize );

						// fill newly added elements and destroy elements stored at old location
						GaInitializeArray( GaGetObject( (GaValueType*)start, limit, _elementSize ), size - limit, _elementSize, defaultValue );
						Clear();

						// apply new array
						_memoryBlock = memory.DetachPointer();
						_arrayStart = start;
						_size = size;
					}
					else
						Clear();
				}
			}

			/// <summary></summary>
			inline int GACALL GetAlignment() const { return _alignment; }

			/// <summary><c>GetAt</c> method returns value of element at specified index in the array.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="index">index of queried element.</param>
			/// <returns>Method returns reference to queried element.</returns>
			inline GaValueType& GACALL GetAt(int index) { return *(GaValueType*)( _arrayStart + index * _elementSize ); }

			/// <summary><c>GetAt</c> method returns value of element at specified index in the array.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="index">index of queried element.</param>
			/// <returns>Method returns reference to queried element.</returns>
			inline const GaValueType& GACALL GetAt(int index) const { return *(const GaValueType*)( _arrayStart + index * _elementSize ); }

			/// <summary><c>operator []</c> returns value of element at specified index in the array.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="index">index of queried element.</param>
			/// <returns>Operator returns reference to queried element.</returns>
			inline GaValueType& GACALL operator[](int index) { return GetAt( index ); }

			/// <summary><c>operator []</c> returns value of element at specified index in the array.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="index">index of queried element.</param>
			/// <returns>Operator returns reference to queried element.</returns>
			inline const GaValueType& GACALL operator[](int index) const { return GetAt( index ); }

			/// <summary><c>operator =</c> clears current content of the array and copies element from another.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="rhs">array that should be copied.</param>
			/// <returns>Method returns reference to <c>this</c> object.</returns>
			GaAlignedArray<GaValueType>& GACALL operator =(const GaAlignedArray<GaValueType>& rhs) { GaAlignedArray<GaValueType>::Copy( &rhs ); return *this; }

		private:

			/// <summary><c>GetElementSize</c> method calculates size of the element with padding required for next element in the array to be aligned</summary>
			/// <returns>Method returns size in bytes of the element with padding required for next element in the array to be aligned</returns>
			inline int GACALL GetElementSize() const { return  ( sizeof( GaValueType ) / _alignment ) + ( sizeof( GaValueType ) % _alignment ? _alignment : 0 ); }

			/// <summary><c>GetArrayStart</c> method calculates address of the first element of the array, aligned on required boundary for specified memory block.</summary>
			/// <param name="memoryBlock">address of memory block that stores elements fot he array.</param>
			/// <returns>Method returns address of the first element of the array, aligned on required boundary.</returns>
			inline char* GACALL GetArrayStart(char* memoryBlock) const { return memoryBlock - (GaIntPtr<sizeof( memoryBlock )>::GaType)memoryBlock % _alignment + _alignment; }

			/// <summary><c>AllocateMemory</c> method allocates memory to large enough to store defined number of elements which are aligned on required boundary.</summary>
			/// <param name="size">number of elements that should be stored in the memory location.</param>
			/// <returns>Method returns auto pointer to allocated block.</returns>
			inline Memory::GaAutoPtr<char> GACALL AllocateMemory(int size)
				{ return Memory::GaAutoPtr<char>( new char[ size * _elementSize + _alignment ], Memory::GaArrayDeletionPolicy<char>::GetInstance() ); }

			/// <summary><c>AllocateMemory</c> method allocates memory to large enough to store current number of elements which are aligned on required boundary.</summary>
			/// <returns>Method returns auto pointer to allocated block.</returns>
			inline Memory::GaAutoPtr<char> GACALL AllocateMemory() { return AllocateMemory( _size ); }

		};

		/// <summary><c>operator ==</c> compares elements of the two single-dimensional aligned arrays.</summary>
		/// <param name="VALUE_TYPE">type of elements stored in the arrays.</param>
		/// <param name="lhs">the first array.</param>
		/// <param name="rhs">the second array.</param>
		/// <returns>Method returns <c>true</c> if elements in both arrays are equal.</returns>
		template<typename VALUE_TYPE>
		bool GACALL operator ==(const GaAlignedArray<VALUE_TYPE>& lhs,
			const GaAlignedArray<VALUE_TYPE>& rhs)
		{
			// if sizes are not equal - arrays are not equal
			if( lhs.GetSize() != rhs.GetSize() )
				return false;

			// compare pairs of elements
			for( int i = lhs.GetSize() - 1; i >= 0; i-- )
			{
				if( lhs[ i ] != rhs[ i ] )
					return false;
			}

			return true;
		}

		/// <summary><c>operator !=</c> compares elements of the two single-dimensional aligned arrays.</summary>
		/// <param name="VALUE_TYPE">type of elements stored in the arrays.</param>
		/// <param name="lhs">the first array.</param>
		/// <param name="rhs">the second array.</param>
		/// <returns>Method returns <c>true</c> if arrays conatin elements that are not equal.</returns>
		template<typename VALUE_TYPE>
		inline bool GACALL operator !=(const GaAlignedArray<VALUE_TYPE>& lhs,
			const GaAlignedArray<VALUE_TYPE>& rhs) { return !( lhs == rhs ); }

		/// <summary><c>GaNextCoord</c> calculates coordinates of the next element in multi-dimanesional array.</summary>
		/// <param name="coordinates">array should store coordinates of the current element at input. On output it will store coordinates of the next element of the array.</param>
		/// <param name="dimensionSizes">sizes of array's dimension.</param>
		GAL_API
		void GACALL GaNextCoord(GaSingleDimensionArray<int>& coordinates,
			const int* dimensionSizes);

		/// <summary><c>GaNextCoordWithSkip</c> method calculates coordinates of the next element in multi-dimanesional array but ignores range of dimensions
		/// (it treats skipped dimensions as they have size of 1).</summary>
		/// <param name="coordinates">array should store coordinates of the current element at input. On output it will store coordinates of the next element of the array.</param>
		/// <param name="dimensionSizes">sizes of array's dimension.</param>
		/// <param name="firstSkipDimension">the first dimension that should be skipped.</param>
		/// <param name="lastSkipDimension">the last dimension that should be skipped.</param>
		GAL_API
		void GACALL GaNextCoordWithSkip(GaSingleDimensionArray<int>& coordinates,
			const int* dimensionSizes,
			int firstSkipDimension,
			int lastSkipDimension);

		/// <summary><c>GaNextCoordWithSkip</c> method calculates coordinates of the next element in multi-dimanesional array and identify number of coordinates that are 
		/// out of range for selected dimension that should be skipped (range size is 1).</summary>
		/// <param name="coordinates">array should store coordinates of the current element at input. On output it will store coordinates of the next element of the array.</param>
		/// <param name="dimensionSizes">sizes of array's dimension.</param>
		/// <param name="firstSkipDimension">the first dimension that should be skipped.</param>
		/// <param name="lastSkipDimension">the last dimension that should be skipped.</param>
		/// <param name="skip">on input variable should store number of coordinates that are out of range. On output it will store new number of out-of-range coordinates.</param>
		GAL_API
		void GACALL GaNextCoordWithSkip(GaSingleDimensionArray<int>& coordinates,
			const int* dimensionSizes,
			int firstSkipDimension,
			int lastSkipDimension,
			int& skip);

		/// <summary><c>GaNextCoordWithSkip</c> method calculates coordinates of the next element in multi-dimanesional array and identify number of coordinates that are 
		/// out of range for selected dimension that should be skipped.</summary>
		/// <param name="coordinates">array should store coordinates of the current element at input. On output it will store coordinates of the next element of the array.</param>
		/// <param name="dimensionSizes">sizes of array's dimension.</param>
		/// <param name="skipLimits">stores maximal values that coordinates can have before they go out of range.</param>
		/// <param name="skip">on input variable should store number of coordinates that are out of range. On output it will store new number of out-of-range coordinates.</param>
		GAL_API
		void GACALL GaNextCoordWithSkip(GaSingleDimensionArray<int>& coordinates,
			const int* dimensionSizes,
			const int* skipLimits,
			int& skip);

		/// <summary><c>GaMultiDimensionArray</c> template class handles multi-dimensional arrays.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of elements stored in the array.</param>
		template<typename VALUE_TYPE>
		class GaMultiDimensionArray : public GaArray<VALUE_TYPE>
		{

		private:

			/// <summary>Sizes of array's dimensions.</summary>
			int* _dimensionSizes;

			/// <summary>Number of dimensions that array has.</summary>
			int _dimensionCount;

		public:

			/// <summary>This constructor initializes array with number of dimensions and their size. Elements are initializes to default values for the type.</summary>
			/// <param name="dimensionCount">number of dimensions that array should have.</param>
			/// <param name="dimensionSizes">sizes of array's dimensions.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>dimensionSizes</c> or any dimension size is negative value.</exception>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>dimensionSizes</c> is set to <c>NULL</c> and <c>dimensionCount</c> has value other than 0.</exception>
			GaMultiDimensionArray(int dimensionCount,
				const int* dimensionSizes) : _dimensionSizes(NULL),
				_dimensionCount(0)
			{
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, dimensionCount >= 0, "dimensionCount", "Number of dimension must be greater or equal to 0.", "Data" );
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, dimensionCount == 0 || source == NULL,
					"dimensionSizes", "Array that contains sizes of dimensions must be specified.", "Data" );

				if( dimensionCount )
				{
					Memory::GaAutoPtr<int> ds( new int[ _dimensionCount ], Memory::GaArrayDeletionPolicy<int>::GetInstance() );

					// check and copy dimension sizes
					int size = 1;
					for( int i = 0; i < dimensionCount; i++ )
					{
						if( dimensionSizes[ i ] < 0 )
							GA_ARG_THROW( Exceptions::GaArgumentOutOfRangeException, "dimensionSizes", "Contains negative size of a dimension.", "Data" );

						// store dimension size
						ds[ i ] = dimensionSizes[ i ];
						size *= dimensionSizes[ i ];
					}

					// allocate memory and initializes elements to default values
					Memory::GaAutoPtr<GaValueType> arr = AllocateMemory( size );
					GaInitializeArray( arr.GetRawPtr(), size );

					_array = arr.DetachPointer();
					_dimensionSizes = ds.DetachPointer();
					_dimensionCount = dimensionCount;
					_size = size;
				}
			}

			/// <summary>This constructor initializes array with number of dimensions but they are all set to zero size.</summary>
			/// <param name="dimensionCount">number of dimensions that array should have.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>dimensionCount</c> is negative value.</exception>
			GaMultiDimensionArray(int dimensionCount)
			{
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, dimensionCount >= 0, "dimensionCount", "Number of dimension must be greater or equal to 0.", "Data" );

				_dimensionCount = dimensionCount;

				// allocate memory to store dimension sizes
				if( _dimensionCount )
					_dimensionSizes = new int[ _dimensionCount ];

				// set dimension sizes to 0
				for( int i = _dimensionCount - 1; i >= 0; i-- )
					_dimensionSizes[ i ] = 0;
			}

			/// <summary>This constructor initializes array by copying another one.</summary>
			/// <param name="rhs">reference to array that should be copied.</param>
			GaMultiDimensionArray(const GaMultiDimensionArray<GaValueType>& rhs) : _dimensionSizes(NULL),
				_dimensionCount(0) { GaMultiDimensionArray<GaValueType>::Copy( &rhs ); }

			/// <summary>This constructor initializes array with zero dimensions.</summary>
			GaMultiDimensionArray() : _dimensionSizes(NULL),
				_dimensionCount(0) { }

			/// <summary>Destorys elements of the array and free used memory.</summary>
			virtual ~GaMultiDimensionArray() { Clear(); }

			/// <summary>More details are given in specification of <see cref="GaArrayBase::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaArrayBase* GACALL Clone() const { return new GaMultiDimensionArray<GaValueType>( *this ); }

			/// <summary>More details are given in specification of <see cref="GaArrayBase::Copy" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Copy(const GaMultiDimensionArray<GaValueType>* source)
			{
				if( source->_dimensionSizes )
				{
					// copy dimesnion sizes
					Memory::GaAutoPtr<int> ds( new int[ source->_dimensionCount ], Memory::GaArrayDeletionPolicy<int>::GetInstance() );
					for( int i = source->_dimensionCount - 1; i >= 0; i-- )
						ds[ i ] = source->_dimensionSizes[ i ];

					// copy elements from the source array
					GaArray<GaValueType>::Copy( source );

					if( _dimensionSizes )
						delete[] _dimensionSizes;

					// apply new array
					_dimensionSizes = ds.DetachPointer();
					_dimensionCount = source->_dimensionCount;
				}
				else
					Clear();
			}

			/// <summary>More details are given in specification of <see cref="GaArrayBase::Copy" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Clear()
			{
				// destory array's elements
				GaArray<GaValueType>::Clear();

				// free memory used to store dimensions sizes of the array
				if( _dimensionSizes )
					delete[] _dimensionSizes;

				_dimensionCount = 0;
			}

			/// <summary><c>AddDimension</c> method adds dimensions to the array.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="dimension">dimensiont before which the new dimensions should be added.</param>
			/// <param name="sizes">sizes of new dimensions.</param>
			/// <param name="count">number of new dimensions that should be added.</param>
			/// <param name="defaultValue">value to which the new elements should be initialized.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if provided dimension does not exist or <c>count</c> is negative value
			/// or if any of dimensions sizes is negative.</exception>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>sizes</c> is set to <c>NULL</c>.</exception>
			void GACALL AddDimension(int dimension,
				const int* sizes,
				int count,
				const GaValueType& defaultValue = GaValueType())
			{
				// no dimension should be added
				if( count == 0 )
					return;

				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, dimension >= 0 && dimension <= _dimensionCount, "dimension", "Dimension does not exist.", "Data" );
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, count >= 0 , "count", "Number of new dimensions that should be created must be positive.", "Data" );
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, sizes != NULL, "sizes", "Array that contains sizes of new dimensions must be specified.", "Data" );

				// check to see if there if all sizes are positive
				for( int i = count - 1; i >= 0; i-- )
					GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, sizes[ i ] >= 0 , "dimensionSizes", "Contains negative size of a dimension.", "Data" );

				int lastDimension = dimension + count;

				// allocate memory to store dimension sizes
				int newDimensionCount = _dimensionCount + count;
				Memory::GaAutoPtr<int> newDimensionSizes( new int[ newDimensionCount ], Memory::GaArrayDeletionPolicy<int>::GetInstance() );
				int newSize = 1;

				// copy sizes of existing dimensions that comes before new dimensions
				for( int i = 0; i < dimension; i++ )
				{
					newDimensionSizes[ i ] = _dimensionSizes[ i ];
					newSize *= _dimensionSizes[ i ];
				}

				// copy sizes of existing dimensions that comes after new dimensions
				for( int i = dimension, j = 0; i < lastDimension; i++, j++ )
				{
					newDimensionSizes[ i ] = sizes[ j ];
					newSize *= sizes[ j ];
				}

				// copy sizes of new dimensions
				for( int i = lastDimension, j = dimension; j < _dimensionCount; i++, j++ )
				{
					newDimensionSizes[ i ] = _dimensionSizes[ j ];
					newSize *= _dimensionSizes[ j ];
				}

				Memory::GaAutoPtr<char> newArray;

				if( newSize )
				{
					// allocate new memory for storing elements
					newArray = AllocateMemory( newSize );

					// coordinates used for iterating the source and destination arrays
					GaSingleDimensionArray<int> sourceCoordinates( _dimensionCount );
					GaSingleDimensionArray<int> destinationCoordinates( newDimensionCount );

					// copy elements from old array
					int skip = _size > 0 ? 0 : 1;
					int i = newSize;

					try
					{
						for( ; i > 0; i-- )
						{
							// initialize element
							new ( (GaValueType*)newArray.GetRawPtr() + GetIndex( destinationCoordinates.GetArray(), newDimensionSizes.GetRawPtr(), newDimensionCount ), GaAllocatorHelper() )
								GaValueType( skip == 0 ? _array[ GetIndex( sourceCoordinates.GetArray() ) ] : defaultValue );

							// move to next element of the source (old) array and destination (new) array
							GaNextCoordWithSkip( destinationCoordinates, newDimensionSizes.GetRawPtr(), dimension, lastDimension, skip );
							if( skip == 0 )
								GaNextCoord( sourceCoordinates, _dimensionSizes );
						}
					}
					catch( ... )
					{
						for( int j = destinationCoordinates.GetSize() - 1; j >= 0; j-- )
							destinationCoordinates[ j ] = 0;

						// destory constructed elements
						for( i += 1; i <= newSize; i++ )
						{
							GaDestructObject( &newArray[ GetIndex( destinationCoordinates.GetArray(), newDimensionSizes.GetRawPtr(), newDimensionCount ) ] );
							GaNextCoord( destinationCoordinates, newDimensionSizes.GetRawPtr() );
						}

						throw;
					}
				}

				// destory current elements and free memory
				Clear();

				// accept new elements and settings
				_array = (GaValueType*)newArray.DetachPointer();
				_dimensionSizes = newDimensionSizes.DetachPointer();
				_dimensionCount = newDimensionCount;
				_size = newSize;
			}

			/// <summary><c>AddDimension</c> method adds dimensions to the array.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="dimension">dimensiont before which the new dimensions should be added.</param>
			/// <param name="sizes">sizes of new dimensions.</param>
			inline void GACALL AddDimension(int dimension,
				const GaSingleDimensionArray<int>& sizes,
				const GaValueType& defaultValue = GaValueType()) { AddDimension( dimension, sizes.GetArray(), sizes.GetSize(), defaultValue ); }

			/// <summary><c>AddDimension</c> method adds new dimensions to the array as the last dimensions.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="sizes">sizes of new dimensions.</param>
			/// <param name="count">number of new dimensions that should be added.</param>
			inline void AddDimension(const int* sizes,
				int count,
				const GaValueType& defaultValue = GaValueType()) { AddDimension( _dimensionCount, sizes, count, defaultValue ); }

			/// <summary><c>AddDimension</c> method adds new dimensions to the array as the last dimensions.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="sizes">sizes of new dimensions.</param>
			inline void AddDimension(const GaSingleDimensionArray<int>& sizes,
				const GaValueType& defaultValue = GaValueType()) { AddDimension( _dimensionCount, sizes.GetArray(), sizes.GetSize(), defaultValue ); }

			/// <summary><c>RemoveDimension</c> method removes dimension(s) from the array.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="dimension">the first dimension that should be removed.</param>
			/// <param name="count">number of dimension that should be removed.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if provided dimension does not exist or the <c>count</c> is negative value or
			/// it causes remove operation to exceed number of dimension that array has.</exception>
			void RemoveDimension(int dimension,
				int count)
			{
				if( count == 0 )
					return;

				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, dimension >= 0 && dimension <= _dimensionCount, "dimension", "Dimension does not exist.", "Data" );
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, count >= 0 && count <= _dimensionCount - dimension,
					"count", "Number of dimension that should removed must positive and less then count of dimensions.", "Data" );

				if( count == _dimensionCount )
				{
					Clear();
					return;
				}

				int lastDimension = dimension + count;

				// allocate memory to store new dimension sizes
				int newDimensionCount = _dimensionCount - count;
				Memory::GaAutoPtr<int> newDimensionSizes( new int[ newDimensionCount ], Memory::GaArrayDeletionPolicy<int>::GetInstance() );
				int newSize = 1;

				// copy sizes of dimensions that comes before removed dimensions
				for( int i = 0; i < dimension; i++ )
				{
					newDimensionSizes[ i ] = _dimensionSizes[ i ];
					newSize *= _dimensionSizes[ i ];
				}

				// copy sizes of dimensions that comes after removed dimensions
				for( int i = lastDimension, j = dimension; i < _dimensionCount; i++, j++ )
				{
					newDimensionSizes[ j ] = _dimensionSizes[ i ];
					newSize *= _dimensionSizes[ i ];
				}

				Memory::GaAutoPtr<char> newArray;

				if( newSize )
				{
					// allocate new memory for storing elements
					newArray = AllocateMemory( newSize );

					// coordinates used for iterating the source and destination arrays
					GaSingleDimensionArray<int> sourceCoordinates( _dimensionCount );
					GaSingleDimensionArray<int> destinationCoordinates( newDimensionCount );

					// copy elements from old array
					int i = newSize;

					try
					{
						for( ; i > 0; i-- )
						{
							// invoke element's copy constructor
							new ( (GaValueType*)newArray.GetRawPtr() + GetIndex( destinationCoordinates.GetArray(), newDimensionSizes.GetRawPtr(), newDimensionCount ), GaAllocatorHelper() )
								GaValueType( _array[ GetIndex( sourceCoordinates.GetArray() ) ] );

							// move to next element of the source (old) array and destination (new) array
							GaNextCoordWithSkip( sourceCoordinates, _dimensionSizes, dimension, lastDimension );
							GaNextCoord( destinationCoordinates, newDimensionSizes.GetRawPtr() );
						}
					}
					catch( ... )
					{
						for( int j = destinationCoordinates.GetSize() - 1; j >= 0; j-- )
							destinationCoordinates[ j ] = 0;

						// destory constructed elements
						for( i += 1; i <= newSize; i++ )
						{
							GaDestructObject( &newArray[ GetIndex( destinationCoordinates.GetArray(), newDimensionSizes.GetRawPtr(), newDimensionCount ) ] );
							GaNextCoord( destinationCoordinates, newDimensionSizes.GetRawPtr() );
						}

						throw;
					}
				}

				// destory current elements and free memory
				Clear();

				// accept new elements and settings
				_array = (GaValueType*)newArray.DetachPointer();
				_dimensionSizes = newDimensionSizes.DetachPointer();
				_dimensionCount = newDimensionCount;
				_size = newSize;
			}

			/// <summary><c>RemoveDimension</c> method removes N last dimension(s) from the array.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="count">number of dimension that should be removed.</param>
			inline void GACALL RemoveDimension(int count) { RemoveDimension( _dimensionCount - count - 1, count ); }

			/// <summary><c>GetIndex</c> method converts element's coordinates to index into memory that stores elements of the array.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="coordinates">coordinates that should be converted to index.</param>
			/// <returns>Method return index of the element at specified coordinates.</returns>
			inline int GACALL GetIndex(const int* coordinates) const { return GetIndex( coordinates, _dimensionSizes, _dimensionCount ); }

			/// <summary><c>GetCoordinates</c> method converts index of the element in memory that stores array to its coordinates in the array.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="index">index of the element.</param>
			/// <param name="coordinates">array that will store element's coordinates.</param>
			void GACALL GetCoordinates(int index,
				int* coordinates) const
			{
				for( int i = i; i < _dimensionSizes; i++ )
				{
					coordinates[ i ] = index % _dimensionSizes[ i ];
					index /= _dimensionSizes[ i ];
				}
			}
/*
			int GACALL GetCoordinateWeight(int dimension) const
			{
				int coordinateWeight = 1;
				for( int i = 0; i < dimension; coordinateWeight *= _dimensionSizes[ i++ ] )
					;

				return coordinateWeight;
			}

			void GACALL GetCoordinateWeights(int* weights,
				int dimension) const
			{
				for( int i = 0, coordinateWeight = 1; i <= dimension; coordinateWeight *= _dimensionSizes[ i++ ] )
					weights[ i ] = coordinateWeight;
			}
*/

			using GaArray::GetAt;

			/// <summary><c>GetAt</c> method returns element of the array at specified coordinates.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="coordinates">coordinates of queried element.</param>
			/// <returns>Method returns reference to element at specified coordinates.</returns>
			inline GaValueType& GACALL GetAt(const int* coordinates) { return *( _array + GetIndex( coordinates ) ); }

			/// <summary><c>GetAt</c> method returns element of the array at specified coordinates.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="coordinates">coordinates of queried element.</param>
			/// <returns>Method returns reference to element at specified coordinates.</returns>
			inline const GaValueType& GACALL GetAt(const int* coordinates) const { return *( _array + GetIndex( coordinates ) ); }

			/// <summary><c>GetAt</c> method returns element of the array at specified coordinates.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="coordinates">coordinates of queried element.</param>
			/// <returns>Method returns reference to element at specified coordinates.</returns>
			inline GaValueType& GACALL GetAt(const GaSingleDimensionArray<int>& coordinates) { return GetAt( coordinates.GetArray() ); }

			/// <summary><c>GetAt</c> method returns element of the array at specified coordinates.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="coordinates">coordinates of queried element.</param>
			/// <returns>Method returns reference to element at specified coordinates.</returns>
			inline const GaValueType& GACALL GetAt(const GaSingleDimensionArray<int>& coordinates) const { return GetAt( coordinates.GetArray() ); }

			/// <summary><c>SetDimenstionSize</c> method sets size of a array's dimension.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="dimension">dimension whose size is being set.</param>
			/// <param name="size">new size of the dimension.</param>
			/// <param name="defaultValue">value to which the new elements should be initialized.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if provided dimension does not exist or the size is negative value.</exception>
			inline void GACALL SetDimenstionSize(int dimension,
				int size,
				const GaValueType& defaultValue = GaValueType())
			{
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, dimension >= 0 && dimension <= _dimensionCount, "dimension", "Dimension does not exist.", "Data" );
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, size >= 0, "size", "Size of dimension cannot be negative.", "Data" );

				// dimension's size has not been changed?
				if( _dimensionSizes[ dimension ] == size )
					return;

				// backup dimension sizes
				Memory::GaAutoPtr<int> newDimensionSizes( new int[ _dimensionCount ], Memory::GaArrayDeletionPolicy<int>::GetInstance() );

				// copy sizes of dimensions that comes before removed dimensions
				for( int i = 0; i < _dimensionCount; i++ )
					newDimensionSizes[ i ] = _dimensionSizes[ i ];
				newDimensionSizes[ dimension ] = size;

				// calculate new count of elements in the array
				int newSize = 1;
				for( int i = 0; i < _dimensionCount; i++ )
					newSize *= newDimensionSizes[ i ];

				Memory::GaAutoPtr<char> newArray;

				if( newSize )
				{
					// allocate new memory for storing elements
					newArray = AllocateMemory( newSize );

					GaSingleDimensionArray<int> coordinates( _dimensionCount );

					// copy elements from old array
					int skip = _size > 0 ? 0 : 1;
					int i = newSize;

					try
					{
						for( ; i > 0; i-- )
						{
							// initialize element
							new ( (GaValueType*)newArray.GetRawPtr() + GetIndex( coordinates.GetArray(), newDimensionSizes.GetRawPtr(), _dimensionCount ), GaAllocatorHelper() )
								GaValueType( skip == 0 ? _array[ GetIndex( coordinates.GetArray() ) ] : defaultValue );

							// move to next element
							GaNextCoordWithSkip( coordinates, newDimensionSizes.GetRawPtr(), _dimensionSizes, skip );
						}
					}
					catch( ... )
					{
						for( int j = coordinates.GetSize() - 1; j >= 0; j-- )
							coordinates[ j ] = 0;

						// destory constructed elements
						for( i += 1; i <= newSize; i++ )
						{
							GaDestructObject( &newArray[ GetIndex( coordinates.GetArray(), newDimensionSizes.GetRawPtr(), _dimensionCount ) ] );
							GaNextCoord( coordinates, newDimensionSizes.GetRawPtr() );
						}

						throw;
					}
				}

				int dimensionCount = _dimensionCount;

				// destory current elements and free memory
				Clear();

				// accept new elements and settings
				_array = (GaValueType*)newArray.DetachPointer();
				_dimensionSizes = newDimensionSizes.DetachPointer();
				_dimensionCount = dimensionCount;
				_size = newSize;
			}

			/// <summary><c>GetDimensionSize</c> method returns size of single dimension of the array.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="dimension">queried dimension.</param>
			/// <returns>Method returns size of queried dimension of the array.</returns>
			inline int GACALL GetDimensionSize(int dimension) const { return _dimensionSizes[ dimension ]; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns array that contains dimension sizes of the array.</returns>
			inline const int* GACALL GetDimensionSizes() const { return _dimensionSizes; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of dimensions that array has.</returns>
			inline int GACALL GetDimensionCount() const { return _dimensionCount; }

			using GaArray::operator [];

			/// <summary><c>operator []</c> returns element of the array at specified coordinates.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="coordinates">coordinates of queried element.</param>
			/// <returns>Operator returns reference to element at specified coordinates.</returns>
			inline GaValueType& GACALL operator [](const int* coordinates) { return GetAt( coordinates ); }

			/// <summary><c>operator []</c> returns element of the array at specified coordinates.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="coordinates">coordinates of queried element.</param>
			/// <returns>Operator returns reference to element at specified coordinates.</returns>
			inline const GaValueType& GACALL operator [](const int* coordinates) const { return GetAt( coordinates ); }

			/// <summary><c>operator []</c> returns element of the array at specified coordinates.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="coordinates">coordinates of queried element.</param>
			/// <returns>Operator returns reference to element at specified coordinates.</returns>
			inline GaValueType& GACALL operator [](const GaSingleDimensionArray<int>& coordinates) { return GetAt( coordinates ); }

			/// <summary><c>operator []</c> returns element of the array at specified coordinates.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="coordinates">coordinates of queried element.</param>
			/// <returns>Operator returns reference to element at specified coordinates.</returns>
			inline const GaValueType& GACALL operator [](const GaSingleDimensionArray<int>& coordinates) const { return GetAt( coordinates ); }

			/// <summary><c>operator []</c> returns element of the array at specified coordinates.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="coordinates">coordinates of queried element.</param>
			/// <returns>Operator returns reference to element at specified coordinates.</returns>
			GaMultiDimensionArray<GaValueType>& GACALL operator =(const GaMultiDimensionArray<GaValueType>& rhs) { GaMultiDimensionArray<GaValueType>::Copy( &rhs ); return *this; }

		private:

			/// <summary><c>GetIndex</c> method converts element's coordinates to index into memory that stores elements of the array.</summary>
			/// <param name="coordinates">coordinates that should be converted to index.</param>
			/// <param name="dimensionSizes">array that contains sizes of array dimensions.</param>
			/// <param name="dimensionCount">number of dimensions that array has.</param>
			/// <returns>Method return index of the element at specified coordinates.</returns>
			static int GACALL GetIndex(const int* coordinates,
				const int* dimensionSizes,
				int dimensionCount)
			{
				int index = 0;
				for( int i = 0, coordinateWeight = 1; i < dimensionCount; coordinateWeight *= dimensionSizes[ i++ ] )
					index += coordinates[ i ] * coordinateWeight;

				return index;
			}

		};

		/// <summary><c>operator ==</c> compares elements of the two multi-dimensional arrays.</summary>
		/// <param name="VALUE_TYPE">type of elements stored in the arrays.</param>
		/// <param name="lhs">the first array.</param>
		/// <param name="rhs">the second array.</param>
		/// <returns>Method returns <c>true</c> if elements in both arrays are equal.</returns>
		template<typename VALUE_TYPE>
		bool GACALL operator ==(const GaMultiDimensionArray<VALUE_TYPE>& lhs,
			const GaMultiDimensionArray<VALUE_TYPE>& rhs)
		{
			// if number of dimensions is not equal - arrays are not equal
			if( lhs.GetDimensionCount() != rhs.GetDimensionCount() )
				return false;

			// if dimension sizes are not equal - arrays are not equal
			for( int i = lhs.GetDimensionCount() - 1; i >= 0; i-- )
			{
				if( lhs.GetDimensionSizes[ i ] != rhs.GetDimensionSizes[ i ] )
					return false;
			}

			// compare pairs of elements
			for( int i = GetSize() - 1; i >= 0; i-- )
			{
				if( lhs[ i ] != rhs[ i ] )
					return false;
			}

			return true;
		}

		/// <summary><c>operator !=</c> compares elements of the two multi-dimensional arrays.</summary>
		/// <param name="VALUE_TYPE">type of elements stored in the arrays.</param>
		/// <param name="lhs">the first array.</param>
		/// <param name="rhs">the second array.</param>
		/// <returns>Method returns <c>true</c> if arrays conatin elements that are not equal.</returns>
		template<typename VALUE_TYPE>
		inline bool GACALL operator !=(const GaMultiDimensionArray<VALUE_TYPE>& lhs,
			const GaMultiDimensionArray<VALUE_TYPE>& rhs) { return !( lhs == rhs ); }

	} // Data
} // Common

#endif // __GA_ARRAY_H__

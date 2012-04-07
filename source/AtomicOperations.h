
/*! \file AtomicOperations.h
	\brief This file defines macros that abstract atomic operations that are platform specific.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#ifndef __GA_ATOMIC_OPERATIONS_H__
#define __GA_ATOMIC_OPERATIONS_H__

#include "Platform.h"

#if defined(GAL_PLATFORM_WIN)

	#include <windows.h>

	#if defined(GAL_SYNTAX_MSVC) && !defined(GAL_SYNTAX_INTL)

		#include <intrin.h>

	#endif

#elif defined(GAL_PLATFORM_NIX)

	#if defined(GAL_PLATFORM_MACOS)

		#include <libkern/OSAtomic.h>
		#include <sys/types.h>

	#endif

	#if defined(GAL_PLATFORM_SOL)

		#include <atomic.h>

	#endif

#endif


namespace Common
{
	namespace Threading
	{


		/// <summary><c>GaAtomicOps</c> structure isolates platform specific atomic operations over 32-bits or 64-bits datatypes.</summary>
		/// <param name="TYPE">type of data.</param>
		/// <param name="SIZE">size of datatype in bytes.</param>
		template<typename TYPE,
			int SIZE = sizeof( TYPE )>
		struct GaAtomicOps
		{

			#if defined( __GAL_DOCUMENTATION__ )

			/// <summary>Type of pointer to value used by system sepcific atomic operations.</summary>
			typedef _SYSTEM_OR_COMPILER_SPECIFIC_ GaPtr;

			/// <summary>Type of value used by system specific atomic operations.</summary>
			typedef _SYSTEM_OR_COMPILER_SPECIFIC_ GaVal;

			/// <summary><c>Inc</c> method performs atomic incrementation of the variable.</summary>
			/// <param name="value">pointer to variable that should be incremented.</param>
			/// <returns>Method returns new value of the variable.</returns>
			static inline TYPE GACALL Inc(volatile TYPE* value) { }

			/// <summary><c>Inc</c> method performs atomic decrementation of the variable.</summary>
			/// <param name="value">pointer to variable that should be decremented.</param>
			/// <returns>Method returns new value of the variable.</returns>
			static inline TYPE GACALL Dec(volatile TYPE* value) { }

			/// <summary><c>Add</c> method performs atomic addition operation.</summary>
			/// <param name="value1">pointer to target varaible.</param>
			/// <param name="value2">value that should be added to the target.</param>
			/// <returns>Method returns old value of the variable.</returns>
			static inline TYPE GACALL Add(volatile TYPE* value1,
				TYPE value2) { }

			/// <summary><c>Sub</c> method performs atomic substraction operation.</summary>
			/// <param name="value1">pointer to target varaible.</param>
			/// <param name="value2">value that should be substracted from the target.</param>
			/// <returns>Method returns old value of the variable.</returns>
			static inline TYPE GACALL Sub(volatile TYPE* value1,
				TYPE value2) { }

			/// <summary><c>And</c> method performs atomic arithmetical AND operation.</summary>
			/// <param name="value1">pointer to target varaible.</param>
			/// <param name="value2">value against which the operation is performed.</param>
			static inline void GACALL And(volatile TYPE* value1,
				TYPE value2) { }

			/// <summary><c>And</c> method performs atomic arithmetical OR operation.</summary>
			/// <param name="value1">pointer to target varaible.</param>
			/// <param name="value2">value against which the operation is performed.</param>
			static inline void GACALL Xor(volatile TYPE* value1,
				TYPE value2) { }

			/// <summary><c>And</c> method performs atomic arithmetical XOR operation.</summary>
			/// <param name="value1">pointer to target varaible.</param>
			/// <param name="value2">value against which the operation is performed.</param>
			static inline void GACALL Or(volatile TYPE* value1,
				TYPE value2) { }

			/// <summary><c>Exchange</c> method stores new value to the destination variable and returns it's old value atomically.</summary>
			/// <param name="value1">reference to target variable.</param>
			/// <param name="value2">new value that should be stored to the target variable.</param>
			/// <returns>Method returns old value of the target.</returns>
			static inline TYPE GACALL Xchg(volatile TYPE* value1,
				TYPE value2) { }

			/// <summary><c>CmpXchg</c> method stores <c>newValue</c> in the target variable if it is equals to <c>comperand</c>, otherwise this method has no effect.
			///
			/// This method is thread-safe.</summary>
			/// <param name="value">pointer to target variable.</param>
			/// <param name="comperand">value against  which the current value is compared.</param>
			/// <param name="newValue">new value that will be stores if succeed.</param>
			/// <returns>Method returns <c>true</c> if the swap has occured.</returns>
			static inline bool GACALL CmpXchg(volatile TYPE* value,
				TYPE comperand,
				TYPE newValue) { }

			#endif

		};

		/// <summary><c>GaCmpXchg2</c> struct isolate platform dependant compare and swap operation for datatypes that has length of two CPU words.</summary>
		/// <param name="TYPE">type of data.</param>
		/// <param name="SIZE">size of datatype in bytes.</param>
		template<typename TYPE,
			int SIZE = sizeof( TYPE )>
		struct GaCmpXchg2
		{

			/// <summary><c>Op</c> method stores <c>newValue</c> in the target variable if it is equals to <c>comperand</c>, otherwise this method has no effect.
			///
			/// This method is thread-safe.</summary>
			/// <param name="value">pointer to target variable.</param>
			/// <param name="comperand">value against  which the current value is compared.</param>
			/// <param name="newValue">new value that will be stores if succeed.</param>
			/// <returns>Method returns <c>true</c> if the swap has occured.</returns>
			static inline bool GACALL Op(volatile TYPE* value,
				TYPE* comperand,
				const TYPE& newValue) { }

		};

		/// <summary><c>GaQWordExtract</c> sturct provides methods for extracting CPU words from datatypes that has twice the length.</summary>
		/// <param name="TYPE">type of data.</param>
		template<typename TYPE>
		struct GaQWordExtract
		{

			/// <summary><c>Low</c> method extract lower-significant CPU word.</summary>
			/// <param name="oWord">double CPU word.</param>
			/// <returns>Method returns CPU word.</returns>
			static inline long long GACALL Low(const TYPE& oWord) { }

			/// <summary><c>Hi</c> method extract higher-significant CPU word.</summary>
			/// <param name="oWord">double CPU word.</param>
			/// <returns>Method returns CPU word.</returns>
			static inline long long GACALL Hi(const TYPE& oWord) { }

		};

		#if !defined( __GAL_DOCUMENTATION__ )

		#if defined(GAL_PLATFORM_WIN) || ( defined(GAL_PLATFORM_NIX) && defined(GAL_SYNTAX_INTL) )

			#if defined(GAL_PLATFORM_NIX) || ( defined(GAL_PLATFORM_WIN) && defined(GAL_SYNTAX_MSVC) )

				#define INTELOCKED_PREFIX(OPERATION) _##OPERATION

			#else

				#define INTELOCKED_PREFIX(OPERATION) ##OPERATION

			#endif

		template<typename TYPE>
		struct GaAtomicOps<TYPE, 4>
		{

			typedef volatile long* GaPtr;
			typedef long GaVal;

			static inline TYPE GACALL Inc(volatile TYPE* value) { return (TYPE)INTELOCKED_PREFIX( InterlockedIncrement )( (GaPtr)value ); }
			static inline TYPE GACALL Dec(volatile TYPE* value) { return (TYPE)INTELOCKED_PREFIX( InterlockedDecrement )( (GaPtr)value ); }
			static inline TYPE GACALL Add(volatile TYPE* value1, TYPE value2) { return (TYPE)INTELOCKED_PREFIX( InterlockedExchangeAdd )( (GaPtr)value1, (GaVal)value2 ); }
			static inline TYPE GACALL Sub(volatile TYPE* value1, TYPE value2) { return (TYPE)INTELOCKED_PREFIX( InterlockedExchangeAdd )( (GaPtr)value1, -(GaVal)value2 ); }
			static inline void GACALL And(volatile TYPE* value1, TYPE value2) { INTELOCKED_PREFIX( InterlockedAnd )( (GaPtr)value1, (GaVal)value2 ); }
			static inline void GACALL Xor(volatile TYPE* value1, TYPE value2) { INTELOCKED_PREFIX( InterlockedXor )( (GaPtr)value1, (GaVal)value2 ); }
			static inline void GACALL Or(volatile TYPE* value1, TYPE value2) { INTELOCKED_PREFIX( InterlockedOr )( (GaPtr)value1, (GaVal)value2 ); }
			static inline TYPE GACALL Xchg(volatile TYPE* value1, TYPE value2) { return (TYPE)INTELOCKED_PREFIX( InterlockedExchange )( (GaPtr)value1, (GaVal)value2 ); }
			static inline bool GACALL CmpXchg(volatile TYPE* value, TYPE comperand, TYPE newValue)
				{ return INTELOCKED_PREFIX( InterlockedCompareExchange )( (GaPtr)value, (GaVal)newValue, (GaVal)comperand ) == (GaVal)comperand; }

		};

		template<typename TYPE>
		struct GaAtomicOps<TYPE, 8>
		{

			typedef volatile long long* GaPtr;
			typedef long long GaVal;

			static inline TYPE GACALL Inc(volatile TYPE* value) { return (TYPE)INTELOCKED_PREFIX( InterlockedIncrement64 )( (GaPtr)value ); }
			static inline TYPE GACALL Dec(volatile TYPE* value) { return (TYPE)INTELOCKED_PREFIX( InterlockedDecrement64 )( (GaPtr)value ); }
			static inline TYPE GACALL Add(volatile TYPE* value1, TYPE value2) { return (TYPE)INTELOCKED_PREFIX( InterlockedExchangeAdd64 )( (GaPtr)value1, (GaVal)value2 ); }
			static inline TYPE GACALL Sub(volatile TYPE* value1, TYPE value2) { return (TYPE)INTELOCKED_PREFIX( InterlockedExchangeAdd64 )( (GaPtr)value1, -(GaVal)value2 ); }
			static inline void GACALL And(volatile TYPE* value1, TYPE value2) { INTELOCKED_PREFIX( InterlockedAnd64 )( (GaPtr)value1, (GaVal)value2 ); }
			static inline void GACALL Xor(volatile TYPE* value1, TYPE value2) { INTELOCKED_PREFIX( InterlockedXor64 )( (GaPtr)value1, (GaVal)value2 ); }
			static inline void GACALL Or(volatile TYPE* value1, TYPE value2) { INTELOCKED_PREFIX( InterlockedOr64 )( (GaPtr)value1, (GaVal)value2 ); }
			static inline TYPE GACALL Xchg(volatile TYPE* value1, TYPE value2) { return (TYPE)INTELOCKED_PREFIX( InterlockedExchange64 )( (GaPtr)value1, (GaVal)value2 ); }
			static inline bool GACALL CmpXchg(volatile TYPE* value, TYPE comperand, TYPE newValue)
				{ return INTELOCKED_PREFIX( InterlockedCompareExchange64 )( (GaPtr)value, (GaVal)newValue, (GaVal)comperand ) == (GaVal)comperand; }

		};

		template<typename TYPE>
		struct GaCmpXchg2<TYPE, 8>
		{

			static inline bool GACALL Op(volatile TYPE* value, TYPE* comperand, const TYPE& newValue)
				{ return INTELOCKED_PREFIX( InterlockedCompareExchange64 )( (volatile long long*)value, *(const long long*)&newValue, *(long long*)comperand ) == *(long long*)comperand; }

		};

		template<typename TYPE>
		struct GaCmpXchg2<TYPE, 16>
		{

			static inline bool GACALL Op(volatile TYPE* value, TYPE* comperand, const TYPE& newValue)
			{
				return INTELOCKED_PREFIX( InterlockedCompareExchange128 )( (volatile long long*)value,
					GaQWordExtract<TYPE>::Hi( newValue ), GaQWordExtract<TYPE>::Low( newValue ), (long long*)comperand ) == 1;
			}

		};

		#elif defined(GAL_PLATFORM_SOL)

		template<typename TYPE>
		struct GaAtomicOps<TYPE, 4>
		{

			typedef volatile uint32_t* GaPtr;
			typedef uint32_t GaVal;

			static inline TYPE GACALL Inc(volatile TYPE* value) { return (TYPE)atomic_inc_32_nv( (GaPtr)value ); }
			static inline TYPE GACALL Dec(volatile TYPE* value) { return (TYPE)atomic_dec_32_nv( (GaPtr)value ); }
			static inline TYPE GACALL Add(volatile TYPE* value1, TYPE value2) { return (TYPE)atomic_add_32( (GaPtr)value1, (GaVal)value2 ); }
			static inline TYPE GACALL Sub(volatile TYPE* value1, TYPE value2) { return (TYPE)atomic_add_32( (GaPtr)value1, (GaVal)( -(int)value2 ) ); }
			static inline void GACALL And(volatile TYPE* value1, TYPE value2) { atomic_and_32_nv( (GaPtr)value1, (GaVal)value2 ); }
			static inline void GACALL Xor(volatile TYPE* value1, TYPE value2) { atomic_xor_32_nv( (GaPtr)value1, (GaVal)value2 ); }
			static inline void GACALL Or(volatile TYPE* value1, TYPE value2) { atomic_or_32_nv( (GaPtr)value1, (GaVal)value2 ); }
			static inline TYPE GACALL Xchg(volatile TYPE* value1, TYPE value2) { return (TYPE)atomic_swap_32( (GaPtr)value1, (GaVal)value2 ); }
			static inline bool GACALL CmpXchg(volatile TYPE* value, TYPE comperand, TYPE newValue)
				{ return atomic_cas_32( (GaPtr)value, (GaVal)comperand, (GaVal)newValue ) == (GaVal)comperand; }

		};

		template<typename TYPE>
		struct GaAtomicOps<TYPE, 8>
		{

			typedef volatile uint64_t* GaPtr;
			typedef uint64_t GaVal;

			static inline TYPE GACALL Inc(volatile TYPE* value) { return (TYPE)atomic_inc_64_nv( (GaPtr)value ); }
			static inline TYPE GACALL Dec(volatile TYPE* value) { return (TYPE)atomic_dec_64_nv( (GaPtr)value ); }
			static inline TYPE GACALL Add(volatile TYPE* value1, TYPE value2) { return (TYPE)atomic_add_64( (GaPtr)value1, (GaVal)value2 ); }
			static inline TYPE GACALL Sub(volatile TYPE* value1, TYPE value2) { return (TYPE)atomic_add_64( (GaPtr)value1, (GaVal)( -(int)value2 ) ); }
			static inline void GACALL And(volatile TYPE* value1, TYPE value2) { atomic_and_64_nv( (GaPtr)value1, (GaVal)value2 ); }
			static inline void GACALL Xor(volatile TYPE* value1, TYPE value2) { atomic_xor_64_nv( (GaPtr)value1, (GaVal)value2 ); }
			static inline void GACALL Or(volatile TYPE* value1, TYPE value2) { atomic_or_64_nv( (GaPtr)value1, (GaVal)value2 ); }
			static inline TYPE GACALL Xchg(volatile TYPE* value1, TYPE value2) { return (TYPE)atomic_swap_64( (GaPtr)value1, (GaVal)value2 ); }
			static inline bool GACALL CmpXchg(volatile TYPE* value, TYPE comperand, TYPE newValue)
				{ return atomic_cas_64( (GaPtr)value, (GaVal)comperand, (GaVal)newValue ) == (GaVal)comperand; }

		};

		template<typename TYPE>
		struct GaCmpXchg2<TYPE, 8>
		{

			static inline bool GACALL Op(volatile TYPE* value, TYPE* comperand, const TYPE& newValue)
				{ return atomic_cas_64( (volatile uint64_t*)value, *(int64_t*)comperand, *(const int64_t*)&newValue ) == (uint64_t)comperand; }

		};

		template<typename TYPE>
		struct GaCmpXchg2<TYPE, 16>
		{

			static inline bool GACALL Op(volatile TYPE* value, TYPE* comperand, const TYPE& newValue)
			{
				return atomic_cas_128( (volatile uint64_t*)value, 
					*(int64_t*)comperand, GaQWordExtract<TYPE>::Hi( newValue ), GaQWordExtract<TYPE>::Low( newValue ) ) == (uint64_t)comperand;
			}

		};

		#elif defined(GAL_PLATFORM_NIX)
		// --mcx16

		template<int SIZE> struct GaGccAtomicTypes { };

		template<>
		struct GaGccAtomicTypes<4>
		{

			typedef volatile long* GaPtr;

			typedef long GaVal;

		};

		template<>
		struct GaGccAtomicTypes<8>
		{

			typedef volatile long long * GaPtr;

			typedef long long GaVal;

		};

		template<typename TYPE,
			int SIZE>
		struct GaAtomicOps
		{

			typedef GaGccAtomicTypes<SIZE>::GaPtr GaPtr;
			typedef GaGccAtomicTypes<SIZE>::GaVal GaVal;

			static inline TYPE GACALL Inc(volatile TYPE* value) { return (TYPE)__sync_add_and_fetch( (GaPtr)value, 1L ); }
			static inline TYPE GACALL Dec(volatile TYPE* value) { return (TYPE)__sync_sub_and_fetch( (GaPtr)value, 1L ); }
			static inline TYPE GACALL Add(volatile TYPE* value1, TYPE value2) { return (TYPE)__sync_fetch_and_add( (GaPtr)value1, (GaVal)value2 ); }
			static inline TYPE GACALL Sub(volatile TYPE* value1, TYPE value2) { return (TYPE)__sync_fetch_and_sub( (GaPtr)value1, (GaVal)value2 ); }
			static inline void GACALL And(volatile TYPE* value1, TYPE value2) { __sync_and_and_fetch( (GaPtr)value1, (GaVal)value2 ); }
			static inline void GACALL Xor(volatile TYPE* value1, TYPE value2) { __sync_xor_and_fetch( (GaPtr)value1, (GaVal)value2 ); }
			static inline void GACALL Or(volatile TYPE* value1, TYPE value2) { __sync_or_and_fetch( (GaPtr)value1, (GaVal)value2 ); }
			static inline TYPE GACALL Xchg(volatile TYPE* value1, TYPE value2) { return (TYPE)__sync_lock_test_and_set( (GaPtr)value1, (GaVal)value2 ); }
			static inline bool GACALL CmpXchg(volatile TYPE* value, TYPE comperand, TYPE newValue)
				{ return __sync_bool_compare_and_swap( (GaPtr)value, (GaVal)comperand, (GaVal)newValue ); }

		};

		template<typename TYPE>
		struct GaCmpXchg2<TYPE, 8>
		{

			static inline bool GACALL Op(volatile TYPE* value, TYPE* comperand, const TYPE& newValue)
				{ return __sync_bool_compare_and_swap( (volatile long long*)value, *(long long*)comperand, *(const long long*)&newValue ); }

		};

		template<typename TYPE>
		struct GaCmpXchg2<TYPE, 16>
		{

			static inline bool GACALL Op(volatile TYPE* value, TYPE* comperand, const TYPE& newValue)
				{ return __sync_bool_compare_and_swap( (volatile int128_t*)value, *(int128_t*)comperand, (int128_t)( newValue ) ); }

		};

		#endif

		#endif

		/// <summary><c>GaAtomic</c> template class provides atomic operations for 32-bits or 64-bits datatypes. Data should be be aligned to word size.
		///
		/// This class has built-in synchronizator so it is allowed to use <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros with instances of this class,
		/// but publicmethods are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of stored value.</param>
		template<typename VALUE_TYPE>
		class GaAtomic
		{

		public:

			/// <summary>Type of stored value.</summary>
			typedef VALUE_TYPE GaValue;

			/// <summary>Type of stored value with disabled optimization.</summary>
			typedef volatile GaValue GaVolatileValue;

		private:

			/// <summary>Stored value.</summary>
			GaVolatileValue _value;

		public:

			/// <summary>Initializes stored value.</summary>
			/// <param name="value">initial value.</value>
			GaAtomic(GaValue value) : _value(value) { }

			/// <summary>Initializes with default value.</summary>
			GaAtomic() : _value() { }

			/// <summary><c>Exchange</c> swaps stored values atomically.
			///
			/// This method is thread-safe.</summary>
			/// <param name="value">reference to the second object that stores value that is being swapped.</param>
			inline void GACALL Exchange(GaAtomic<GaValue>& value) { Exchange( value._value ); }

			/// <summary><c>Exchange</c> method stores new value and returns old atomically.
			///
			/// This method is thread-safe.</summary>
			/// <param name="value">reference to the second value that is being swapped.</param>
			inline void GACALL Exchange(GaValue& value) { GaAtomicOps::Xchg( &_value, &value ); }

			/// <summary><c>CompareAndExchange</c> method stores <c>newValue</c> if the current value is equals to <c>comperand</c>, otherwise this method has no effect.
			///
			/// This method is thread-safe.</summary>
			/// <param name="comperand">value against  which the current value is compared.</param>
			/// <param name="newValue">new value that will be stores if succeed.</param>
			/// <returns>Method returns <c>true</c> if the swap has occured.</returns>
			inline bool GACALL CompareAndExchange(GaValue comperand,
				GaValue newValue) { return GaAtomicOps<GaValue>::CmpXchg( &_value, comperand, newValue ); }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Mehtod returns stored value.</returns>
			inline GaValue GACALL GetValue() const { return _value; }

			/// <summary><c>operator ++</c> increments stored value atomically.
			///
			/// This operator is thread-safe.</summary>
			/// <returns>Operator returns new value after increment operation.</returns>
			inline GaValue GACALL operator ++() { return GaAtomicOps<GaValue>::Add( &_value, 1 ) + 1; }

			/// <summary><c>operator ++</c> increments stored value atomically.
			///
			/// This operator is thread-safe.</summary>
			/// <returns>Operator returns old value before increment operation.</returns>
			inline GaValue GACALL operator ++(int) { return GaAtomicOps<GaValue>::Add( &_value, 1 ); }

			/// <summary><c>operator --</c> decrements stored value atomically.
			///
			/// This operator is thread-safe.</summary>
			/// <returns>Operator returns new value after decrement operation.</returns>
			inline GaValue GACALL operator --() { return GaAtomicOps<GaValue>::Add( &_value, -1 ) - 1; }

			/// <summary><c>operator --</c> decrements stored value atomically.
			///
			/// This operator is thread-safe.</summary>
			/// <returns>Operator returns old value before decrement operation.</returns>
			inline GaValue GACALL operator --(int) {  return GaAtomicOps<GaValue>::Sub( &_value, 1 ); }

			/// <summary><c>operator +=</c> adds value to the stored value atomically.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">value that is added to the stored value.</param>
			/// <returns>Operator returns reference to <c>this</c> object.</returns>
			inline GaAtomic<GaValue>& GACALL operator +=(GaValue rhs) { GaAtomicOps<GaValue>::Add( &_value, rhs ); return *this; }

			/// <summary><c>operator -=</c> substracts value to the stored value atomically.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">value that is substracted from the stored value.</param>
			/// <returns>Operator returns reference to <c>this</c> object.</returns>
			inline GaAtomic<GaValue>& GACALL operator -=(GaValue rhs) { GaAtomicOps<GaValue>::Sub( &_value,  rhs ); return *this; }

			/// <summary><c>operator &=</c> performs arithmetical AND against stored value and <c>rhs</c> and store results atomically.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">value against which arithmetical AND operation is performed.</param>
			/// <returns>Operator returns reference to <c>this</c> object.</returns>
			inline GaAtomic<GaValue>& GACALL operator &=(GaValue rhs) { GaAtomicOps<GaValue>::And( &_value, rhs ); return *this; }

			/// <summary><c>operator |=</c> performs arithmetical OR against stored value and <c>rhs</c> and store results atomically.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">value against which arithmetical OR operation is performed.</param>
			/// <returns>Operator returns reference to <c>this</c> object.</returns>
			inline GaAtomic<GaValue>& GACALL operator |=(GaValue rhs) { GaAtomicOps<GaValue>::Or( &_value, rhs ); return *this; }

			/// <summary><c>operator ^=</c> performs arithmetical XOR against stored value and <c>rhs</c> and store results atomically.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">value against which arithmetical XOR operation is performed.</param>
			/// <returns>Operator returns reference to <c>this</c> object.</returns>
			inline GaAtomic<GaValue>& GACALL operator ^=(GaValue rhs) { GaAtomicOps<GaValue>::Xor( &_value, rhs ); return *this; }

			/// <summary><c>operator =</c> stores new value.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">new value.</param>
			/// <returns>Operator returns reference to <c>this</c> object.</returns>
			inline GaAtomic<GaValue>& GACALL operator =(GaValue rhs) { GaAtomicOps<GaValue>::Xchg( &_value, rhs ); return *this;  }

			/// <summary><c>operator ==</c> compares stored value against <c>rhs</c>.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">value against which the stored value is compared.</param>
			/// <returns>Operator returns <c>true</c> if the stored and provided values are equal.</returns>
			inline bool GACALL operator ==(GaValue rhs) const { return _value == rhs; }

			/// <summary><c>operator !=</c> compares stored value against <c>rhs</c>.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">value against which the stored value is compared.</param>
			/// <returns>Operator returns <c>true</c> if the stored and provided values are not equal.</returns>
			inline bool GACALL operator !=(GaValue rhs) const { return _value != rhs; }

			/// <summary>This operator is thread-safe.</summary>
			/// <returns>Operator returns stored pointer.</returns>
			inline operator GaValue() const { return _value; }

		};

		/// <summary>This specialization fo <c>GaAtomic</c> template  and provides atomic operations for pointers. Data should be be aligned to word size.
		///
		/// This class has built-in synchronizator so it is allowed to use <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros with instances of this class,
		/// but publicmethods are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of pointer value.</param>
		template<typename VALUE_TYPE>
		class GaAtomic<VALUE_TYPE*>
		{

		public:

			/// <summary>Type of pointer value.</summary>
			typedef VALUE_TYPE* GaValue;

			/// <summary>Type of pointer value. with disable optimization.</summary>
			typedef volatile GaValue GaVolatileValue;

		private:

			/// <summary>Stored pointer.</summary>
			GaVolatileValue _value;

		public:

			/// <summary>Initializes pointer.</summary>
			/// <param name="value">initial pointer.</param>
			GaAtomic(GaValue value) : _value(value) { }

			/// <summary>Initializes <c>NULL</c> pointer.</summary>
			GaAtomic() : _value() { }

			/// <summary><c>Exchange</c> swaps pointers atomically.
			///
			/// This method is thread-safe.</summary>
			/// <param name="value">reference to the second object that stores pointer that is being swapped.</param>
			inline void GACALL Exchange(GaAtomic<GaValue>& value) { Exchange( value._value ); }

			/// <summary><c>Exchange</c> method stores new value and returns old atomically.
			///
			/// This method is thread-safe.</summary>
			/// <param name="value">reference to the second pointer that is being swapped.</param>
			inline void GACALL Exchange(GaValue& value) { GaAtomicOps::Xchg( &_value, &value ); }

			/// <summary><c>CompareAndExchange</c> method stores <c>newValue</c> if the current pointer is equals to <c>comperand</c>, otherwise this method has no effect.
			///
			/// This method is thread-safe.</summary>
			/// <param name="comperand">pointer against which the current address is compared.</param>
			/// <param name="newValue">new pointer that will be stores if succeed.</param>
			/// <returns>Method returns <c>true</c> if the swap has occured.</returns>
			inline bool GACALL CompareAndExchange(GaValue comperand,
				GaValue newValue) { return GaAtomicOps<GaValue>::CmpXchg( &_value, comperand, newValue ); }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns stored pointer.</returns>
			inline GaValue GACALL GetValue() const { return _value; }

			/// <summary><c>operator ++</c> moves pointer to the a memory location after the current atomically.
			///
			/// This operator is thread-safe.</summary>
			/// <returns>Operator returns new address after increment operation.</returns>
			inline GaValue GACALL operator ++() { return GaAtomicOps<GaValue>::Add( &_value, (GaValue)sizeof( VALUE_TYPE ) ) + 1; }

			/// <summary><c>operator ++</c> moves pointer to the a memory location after the current atomically.
			///
			/// This operator is thread-safe.</summary>
			/// <returns>Operator returns old address before increment operation.</returns>
			inline GaValue GACALL operator ++(int) { return GaAtomicOps<GaValue>::Add( &_value, (GaValue)sizeof( VALUE_TYPE ) ); }

			/// <summary><c>operator --</c> moves pointer to the a memory location before the current atomically.
			///
			/// This operator is thread-safe.</summary>
			/// <returns>Operator returns new address after decrement operation.</returns>
			inline GaValue GACALL operator --() { return GaAtomicOps<GaValue>::Sub( &_value, (GaValue)sizeof( VALUE_TYPE ) ) - 1; }

			/// <summary><c>operator --</c> moves pointer to the a memory location before the current atomically.
			///
			/// This operator is thread-safe.</summary>
			/// <returns>Operator returns old address before decrement operation.</returns>
			inline GaValue GACALL operator --(int) { return GaAtomicOps<GaValue>::Sub( &_value, (GaValue)sizeof( VALUE_TYPE ) ); }

			/// <summary><c>operator +=</c> adds defined number of memory locations to the current address atomically.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">number of memory locations that is substracted from the stored pointer.</param>
			/// <returns>Operator returns reference to <c>this</c> object.</returns>
			inline GaAtomic<GaValue>& GACALL operator +=(int rhs) { GaAtomicOps<GaValue>::Add( &_value, (GaValue)( rhs * sizeof( VALUE_TYPE ) ) ); return *this; }

			/// <summary><c>operator -=</c> substracts defined number of memory locations from the current address atomically.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">number of memory locations that is substracted from the stored pointer.</param>
			/// <returns>Operator returns reference to <c>this</c> object.</returns>
			inline GaAtomic<GaValue>& GACALL operator -=(int rhs) { GaAtomicOps<GaValue>::Sub( &_value, (GaValue)( rhs * sizeof( VALUE_TYPE ) ) ); return *this; }

			/// <summary><c>operator =</c> stores new address.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">new address.</param>
			/// <returns>Operator returns reference to <c>this</c> object.</returns>
			inline GaAtomic<GaValue>& GACALL operator =(const GaValue rhs) { _value = rhs; return *this;  }

			/// <summary><c>operator ==</c> compares stored pointer against <c>rhs</c>.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">value against which the stored pointer is compared.</param>
			/// <returns>Operator returns <c>true</c> if the stored and provided pointers are equal.</returns>
			inline bool GACALL operator ==(GaValue rhs) const { return _value == rhs; }

			/// <summary><c>operator !=</c> compares stored pointer against <c>rhs</c>.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">value against which the stored pointer is compared.</param>
			/// <returns>Operator returns <c>true</c> if the stored and provided pointers are not equal.</returns>
			inline bool GACALL operator !=(GaValue rhs) const { return _value != rhs; }

			/// <summary>This operator is thread-safe.</summary>
			/// <returns>Operator returns stored pointer.</returns>
			inline operator GaValue() const { return _value; }

		};

	} // Threading
} // Common

#endif // __GA_ATOMIC_OPERATIONS_H__

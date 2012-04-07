
/*! \file Flags.h
	\brief This file declares and implements template class that store and manage flags.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_FLAGS_H__
#define __GA_FLAGS_H__

#include "Exceptions.h"
#include "SmartPtr.h"

namespace Common
{
	namespace Data
	{

		/// <summary><c>GaFlags</c> class stores and performs operations on flags. The class uses single variable of specified type to store flags.
		/// Number of flags that can be stored depends on the underlying type.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="T">type of underlying datatype which is used to store flags.</param>
		template<typename T>
		class GaFlags
		{

		public:

			/// <summary>Type of underlying datatype which is used to store flags.</summary>
			typedef T GaDataType;

		private:

			/// <summary>Variable that stores states of flags.</summary>
			GaDataType _flags;

		public:

			/// <summary>This constoructor initializes flag manager and clears all flags.</summary>
			GaFlags() : _flags(0) { }

			/// <summary>This constoructor initializes flag manager and sets specified flags.</summary>
			/// <param name="flags">flags which should be set upon initialization of flag manager.</param>
			GaFlags(GaDataType flags) : _flags(flags) { }

			/// <summary><c>CopyAllFlags</c> method sets states of all flags to states specified in <c>flags</c> parameter.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="flags">desired states of flags.</param>
			inline void GACALL CopyAllFlags(GaDataType flags) { _flags = flags; }

			/// <summary><c>CopyMaskedFlags</c> method sets states of flags specified in <c>mask</c> parameter to states specified in <c>flags</c> parameter.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="flags">desired states of flags.</param>
			/// <param name="mask">specifies what flags will be affected by this method.</param>
			inline void GACALL CopyMaskedFlags(GaDataType flags,
				GaDataType mask) { _flags = ( _flags & ~mask ) | ( flags & mask ); }

			/// <summary><c>SetAllFlags</c> method sets all flags.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL SetAllFlags() { _flags = (GaDataType)-1L; }

			/// <summary><c>SetFlags</c> method sets flags that are specified in <c>mask</c> parameter.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="mask">specifies what flags will be affected by this method.</param>
			inline void GACALL SetFlags(GaDataType mask) { _flags |= mask; }

			/// <summary><c>SetFlagsToState</c> method sets masked all flags that specified state.
			///
			/// This method is not thread-safe</summary>
			/// <param name="mask">specifies what flags will be affected by this method.</param>
			/// <param name="state">state to which all the masked flags will be set.</param>
			inline void GACALL SetFlagsToState(GaDataType mask,
				bool state) { state ? SetFlags( mask ) : ClearFlags( mask ); }

			/// <summary><c>ClearAllFlags</c> method clears all flags.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL ClearAllFlags() { _flags = 0; }

			/// <summary><c>ClearFlags</c> method clears flags that are specified in <c>mask</c> parameter.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="mask">specifies what flags will be affected by this method.</param>
			inline void GACALL ClearFlags(GaDataType mask) { _flags &= ~mask; }

			/// <summary><c>InvertAllFlags</c> method inverts states of all flags.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL InvertAllFlags() { _flags ~= _flags; }

			/// <summary><c>InvertFlags</c> method inverts states of flags that are specified in <c>mask</c> parameter.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="mask">specifies what flags will be affected by this method.</param>
			inline void GACALL InvertFlags(GaDataType mask) { _flags ^= mask; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method return copy of variable that stores states of flags.</returns>
			inline GaDataType GACALL GetAllFlags() const { return _flags; }

			/// <summary><c>GetMaskedFlags</c> method returns states od desired flags specified by <c>mask</c> parameter.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="mask">specifies what flags will be copied.</param>
			/// <returns>Method returns variable that stores states of desire flags.</returns>
			inline GaDataType GACALL GetMaskedFlags(GaDataType mask) const { return _flags & mask; }

			/// <summary><c>IsFlagSetAny</c> method tests states flags specified in <c>mask</c> parameter.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="mask">specifies what flags will be tested.</param>
			/// <returns>Method returns <c>true</c> if any of desired flags is set.</returns>
			inline bool GACALL IsFlagSetAny(GaDataType mask) const { return ( _flags & mask ) != 0; }

			/// <summary><c>IsFlagSetAll</c> method tests states flags specified in <c>mask</c> parameter.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="mask">specifies what flags will be tested.</param>
			/// <returns>Method returns <c>true</c> if all of desired flags are set.</returns>
			inline bool GACALL IsFlagSetAll(GaDataType mask) const { return ( _flags & mask ) == mask; }

			/// <summary><c>TestAllFlags</c> method tests states of all flags against those that are specified in <c>flags</c> parameter.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="flags">states of flags against which the stores flags are tested.</param>
			/// <returns>Method returns <c>true</c> if states of all flags are equal.</returns>
			inline bool GACALL TestAllFlags(GaDataType flags) const { return _flags == flags; }

			/// <summary><c>TestAllFlags</c> method tests states of flags specified in <c>mask</c> parameter against those that are specified in <c>flags</c> parameter.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="mask">specifies what flags will be tested.</param>
			/// <param name="flags">states of flags against which the stores flags are tested.</param>
			/// <returns>Method returns <c>true</c> if states of desired flags are equal.</returns>
			inline bool GACALL TestFlags(GaDataType flags,
				GaDataType mask) const { return ( _flags & mask ) == ( flags & mask ); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of flags that underlying datatype can store.</returns>
			static inline int GACALL GetSize() { return sizeof( GaDataType ); }

			/// <summary>This operator is not thread-safe.</summary>
			/// <returns>Method return copy of variable that stores states of flags.</returns>
			inline operator GaDataType() const { return _flags; }

		};

		/// <summary><c>GaFlags8</c> type is instance of <see cref="GaFlags" /> template class that use <c>unsigned char</c> as underlying data type to store flags.
		/// This instance of <see cref="GaFlags" /> can store up to 8 flags</summary>
		typedef GaFlags<unsigned char> GaFlags8;

		/// <summary><c>GaFlags8</c> type is instance of <see cref="GaFlags" /> template class that use <c>unsigned short</c> as underlying data type to store flags.
		/// This instance of <see cref="GaFlags" /> can store up to 16 flags</summary>
		typedef GaFlags<unsigned short> GaFlags16;

		/// <summary><c>GaFlags8</c> type is instance of <see cref="GaFlags" /> template class that use <c>unsigned int</c> as underlying data type to store flags.
		/// This instance of <see cref="GaFlags" /> can store up to 32 flags</summary>
		typedef GaFlags<unsigned int> GaFlags32;

		/// <summary><c>GaFlags8</c> type is instance of <see cref="GaFlags" /> template class that use <c>unsigned long long</c> as underlying data type to store flags.
		/// This instance of <see cref="GaFlags" /> can store up to 64 flags</summary>
		typedef GaFlags<unsigned long long> GaFlags64;

		/// <summary><c>GaFlagManager</c> class stores and manages unused flags. 
		/// It can be used in pair with <see cref="GaFlags" .> class to provide global and unified meaning automatic run-time acquisition and releasing of flags.
		///
		/// This class has built-in synchronizator so it is allowed to use <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros with instances of this class,
		/// but public or private methods are thread-safe.</summary>
		/// <param name="T">datatype which is used to store flags.</param>
		template<typename T> 
		class GaFlagManager
		{

			GA_SYNC_CLASS

		public:

			/// <summary>Datatype which is used to store flags.</summary>
			typedef T GaDataType;

		private:

			/// <summary>Number of unused flags in the buffer.</summary>
			int _unusedFlagsCount;

			/// <summary>Pointer to buffer that stores unused flags.</summary>
			Memory::GaAutoPtr<T> _unusedFlags;

		public:

			/// <summary>Initializes flag manager with specified number of flags.</summary>
			GaFlagManager() : _unusedFlagsCount( sizeof( GaDataType ) ),
				_unusedFlags(new GaDataType[ _unusedFlagsCount ], Memory::GaArrayDeletionPolicy<T>::GetInstance())
			{
				// initialize buffer that stores unused flags
				for( int i = sizeof( GaDataType ) - 1; i >= 0; i-- )
					_unusedFlags[ i ] = (GaDataType)1 << i;
			}

			/// <summary><c>AcquireUnusedFlag</c> method returns one unused flag and removes it from the buffer.
			///
			/// This method is not thred-safe.</summary>
			/// <returns>Method returns unused flag.</returns>
			/// <exception cref="GaInvalidOperationException" />Thrown when there is no more unused flags.</exception>
			inline GaDataType AcquireUnusedFlag()
			{
				// is manager empty?
				if( _unusedFlagsCount )
					// return unused flag
					return _unusedFlags[ --_unusedFlagsCount ];

				GA_THROW( Exceptions::GaInvalidOperationException, "No more unused flags.", "Flags" );
			}

			/// <summary><c>ReleaseFlag</c> method returns specified flag to buffer of unused flags so it can be used again.
			///
			/// This method is not thred-safe.</summary>
			/// <param name="flag">flag that should be freed and stored to buffer of unused flags.</param>
			/// <exception cref="GaInvalidOperationException" />Thrown when the buffer of flag manager that stores unused is full (all flags are unused).</exception>
			inline void ReleaseFlag(GaDataType flag)
			{
				// is the manager full?
				if( _unusedFlagsCount < sizeof( GaDataType ) )
				{
					// store flag as unused
					_unusedFlags[ _unusedFlagsCount++ ] = flag;
					return;
				}

				GA_THROW( Exceptions::GaInvalidOperationException, "Flag manager is full.", "Flags" );
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns maximum number of unused flags that can be stored in the buffer of this manager.</returns>
			inline int GetFlagsCount() const { return sizeof( GaDataType ); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of unused flags.</returns>
			inline int GetUnusedFlagsCount() const { return _unusedFlagsCount; }

		};

	} // Data
} // Common

#endif // __GA_FLAGS_H__

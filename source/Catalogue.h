
/*! \file Catalogue.h
	\brief This file contains declaration and implementation of catalogue template class used to store genetic operations and other stateless objects.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_CATALOGUE_H__
#define __GA_CATALOGUE_H__

#include "Hashmap.h"

namespace Common
{
	/// <summary>Contains classes and datatypes that handles different data structures.</summary>
	namespace Data
	{

		/// <summary><C>GaCatalogue</c> template class stores and manages catalogues for genetic operations.
		/// Operations can be accessed by their names. When user inserts new item into catalogue, catalogue takes over the responsibility for the memory
		/// used by the item (memory must be from the heap). Duplicates of names in catalogues are not allowed.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="T">type of stored items in the catalogue.</param>
		template <typename T>
		class GaCatalogue
		{

		private:

			/// <summary>Pointer to global instance of catalogue for type <c>T</c>.</summary>
			GAL_API
			static GaCatalogue<T>* _instance;

			/// <summary>Hash table which stores keys and data of the catalogue.</summary>
			STLEXT::hash_map<std::string, T*> _entries;

		public:

			/// <summary><c>GetInstance</c> method returns reference to global instance of catalogue for type <c>T</c>.</summary>
			/// <returns>Returns pointer to global instance of catalogue for type <c>T</c>.</returns>
			static inline GaCatalogue<T>* GACALL GetInstance() { return _instance; }

			/// <summary>Method makes global instance of catalogue if it was not instanced before.
			/// This method must be called before prior any usage of global instance of catalogue. All built-in catalogues are instanced in <see cref="GaInitialization" /> function.
			///
			/// This method is not thread safe.</summary>
			/// <exception cref="GaInvalidOperationException" />Thrown if global instance already exists.</exception>
			static void GACALL MakeInstance()
			{
				GA_ASSERT( Exceptions::GaInvalidOperationException, _instance == NULL, "Global instance already exists.", "Catalogues" );
				_instance = new GaCatalogue();
			}

			/// <summary>Frees memory used by the global instance of the catalogue. This method must be called before application quits
			/// or unloads library to prevent memory leakage. All instances of built-in global catalogues are freed in <see cref="GaFinalize" /> function.
			///
			/// This method is not thread safe.</summary>
			/// <exception cref="GaInvalidOperationException" />Thrown if global instance does not exist.</exception>
			static void GACALL FreeInstance()
			{
				GA_ASSERT( Exceptions::GaInvalidOperationException, _instance != NULL, "Global instance does not exists.", "Catalogues" );
				delete _instance;
			}

			/// <summary>Frees memory used by the entries of the catalogue.</summary>
			~GaCatalogue()
			{
				for( typename STLEXT::hash_map<std::string, T*>::const_iterator it = _entries.begin(); it != _entries.end(); it++ )
					delete it->second;
			}

			/// <summary><c>Register</c> method inserts an item with specified name to the catalogue. If there is an entry with specified key in catalogue
			/// and <c>replace</c> is set to <c>false</c>, insertion will fail because duplicate of keys are not allowed.
			/// When <c>replace</c> is set to <c>true</c>, item with the specified name will be replaced or if such entry does not exist new entry will be inserted.
			/// Catalogue takes over the responsibility for memory used by the inserted item. Item must be located at heap.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="name">name of new or existing entry.</param>
			/// <param name="data">pointer to item which should be inserted.</param>
			/// <param name="replace">tells method whether it should replace item there is already an entry with specified name.</param>
			/// <exception cref="GaArgumentException" />Thrown when the name is empty string.</exception>
			/// <exception cref="GaInvalidOperationException" />Thrown when user tries to inser new item with already registered name, but the replacement is not allowed.</exception>
			void GACALL Register(const std::string& name,
					T* data,
					bool replace = false)
			{
				GA_ARG_ASSERT( Exceptions::GaArgumentException, name.length() > 0, "name", "Name must be specified.", "Catalogues" );

				// is there an entry with specified name?
				typename STLEXT::hash_map<std::string, T*>::iterator it = _entries.find( name );
				bool exists = it != _entries.end();

				GA_ASSERT( Exceptions::GaInvalidOperationException, !exists || replace,
					"Trying to register new item with name that is already registered, but the replacement is not allowed.", "Catalogues" );

				// erase existing entry it replacement is enabled
				if( exists && replace )
				{
					delete it->second;
					it->second = data;
				}
				else
					// insert new entry to the catalogue
					_entries[ name ] = data;
			}

			/// <summary><c>Unregister</c> method removes entry with specified name from the catalogue and frees memory used by it.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="name">name of the desired entry.</param>
			/// <returns>Method returns <c>true</c> if entry with specified name is successfully removed. If the entry cannot be found method returns <c>false</c>.</returns>
			bool GACALL Unregister(const std::string& name)
			{
				// delete entry and remove it from the catalogue if it exists
				typename STLEXT::hash_map<std::string, T*>::iterator it = _entries.find( name );
				if( it != _entries.end() )
				{
					delete it->second;
					_entries.erase( it );

					return true;
				}

				return false;
			}

			/// <summary><c>GetEntry</c> method returns pointer to entry with specified name.
			///
			/// This method is not thread-safe.</summary>
			/// <param name=name">name of the desired entry.</param>
			/// <returns>Method returns pointer to catalogue entry with specified name if it exists, otherwise it returns <c>NULL</c>.</returns>
			inline T* GACALL GetEntry(const std::string& name)
			{
				// find entry and return pointer to entry
				typename STLEXT::hash_map<std::string, T*>::iterator it = _entries.find( name );
				return it == _entries.end() ? NULL : it->second;
			}

			/// <summary><c>GetEntry</c> method returns pointer to entry with specified name.
			///
			/// This method is not thread-safe.</summary>
			/// <param name=name">name of the desired entry.</param>
			/// <returns>Method returns pointer to catalogue entry with specified name if it exists, otherwise it returns <c>NULL</c>.</returns>
			inline const T* GACALL GetEntry(const std::string& name) const
			{
				// find entry and return pointer to entry
				typename STLEXT::hash_map<std::string, T*>::const_iterator it = _entries.find( name );
				return it == _entries.end() ? NULL : it->second;
			}

			/// <summary><c>operator []</c> returns pointer to entry with specified name.
			///
			/// This operator is thread-safe.</summary>
			/// <param name=name">name of the desired entry.</param>
			/// <returns>Operator returns pointer to catalogue entry with specified name if it exists, otherwise it returns <c>NULL</c>.</returns>
			inline T* GACALL operator [](const std::string& name) { return GetEntry( name ); }

			/// <summary><c>operator []</c> returns pointer to entry with specified name.
			///
			/// This operator is thread-safe.</summary>
			/// <param name=name">name of the desired entry.</param>
			/// <returns>Operator returns pointer to catalogue entry with specified name if it exists, otherwise it returns <c>NULL</c>.</returns>
			inline const T* GACALL operator [](const std::string& name) const { return GetEntry( name ); }

			/// <summary><c>GetNames</c> method returns all registered names in catalogue. The method allocates memory for array of registered names.
			/// The caller must call <c>FreeNames</c> method to free memory used by returned array.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="count">reference to variable that will store number of names.</param>
			/// <returns>Method returns pointer to array of names.</returns>
			std::string** GACALL GetNames(int& count) const
			{
				count = (int)_entries.size();

				// empty directory
				if( !count )
					return NULL;

				// create buffer and fill it with names
				std::string** names = new std::string*[ count ];
				int i = 0;
				for( typename STLEXT::hash_map<std::string, T*>::const_iterator it = _entries.begin(); it != _entries.end(); ++it, i++ )
					names[ i ] = new std::string( it->first );

				return names;
			}

			/// <summary><c>FreeNames</c> method frees memory used by array of registered names returned by the <c>GetNames</c> method.
			///
			/// This method is thread-safe.</summary>
			/// <param name="names">pointer to array that stores names.</param>
			/// <param name="count">number of names in the array.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown when the <c>names</c> is not <c>NULL</c> and <c>count</c> is lower then 1.</exception>
			void GACALL FreeNames(std::string** names,
				int count) const
			{
				if( names )
				{
					GA_ARG_ASSERT( Exceptions::GaArgumentException, count > 0, "count", "Count of names in the array must be positiv number.", "Catalogues" );

					// delete names
					for( count--; count >= 0; count-- )
						delete names[ count ];

					// delete array that was storing names
					delete[] names;
				}
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of entries in catalogue.</returns>
			inline int GACALL GetCount() const { return _entries.size(); }

			/// <summary><c>IsExist</c> method checks for existence of entry with specified name in the catalogue.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="name">name of the desired entry.</param>
			/// <returns>Method returns <c>true</c> if there is an entry with specified name key in catalogue.</returns>
			inline bool GACALL IsExist(const std::string& name) const { return _entries.count( name ) > 0; }

		};

		#if defined(GAL_API_EXPORTING)

			template <typename T>
			GaCatalogue<T>* GaCatalogue<T>::_instance = NULL;

		#elif defined(GAL_PLATFORM_NIX) && !defined(GAL_SYNTAX_INTL) && !defined(GAL_SYNTAX_SUNC)

			template <typename T>
			extern GaCatalogue<T>* GaCatalogue<T>::_instance;

		#endif

	} // Data
} // Common

#endif //__GA_CATALOGUE_H__

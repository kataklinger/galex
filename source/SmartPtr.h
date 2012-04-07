
/*! \file SmartPtr.h
    \brief This file contains declaration and implementation of template classes and datatypes that handles smart pointers used by the library.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_SMART_PTR_H__
#define __GA_SMART_PTR_H__

#include "Synchronization.h"

namespace Common
{
	/// <summary>Contains set of classes and data types for memory management.</summary>
	namespace Memory
	{

		/// <summary><c>GaDeletionPolicy</c> is interface for classes that implements deletion policies for objects managed by smart pointers.</summary>
		/// <param name="T">type of object managed by this policy.</param>
		template<typename T>
		class GaDeletionPolicy
		{

		public:

			/// <summary>This operator should implement deletion policy.</summary>
			/// <param name="object">pointer to object that should be freed by the deletion policy.</param>
			virtual void GACALL operator ()(T* object) const = 0;

		};

		/// <summary><c>GaObjectDeletionPolicy</c> class implements default deletion policy using <c>delete</c> operator.</summary>
		/// <param name="T">type of object managed by this policy.</param>
		template<typename T>
		class GaObjectDeletionPolicy : public GaDeletionPolicy<T>
		{

		private:

			/// <summary>Global instance of default deletion policy for specified type of objects.</summary>
			//GAL_API
			static GaObjectDeletionPolicy<T> _instance;

		public:

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns pointer to global instance of default deletion policy for specified type of objects.</returns>
			static inline GaObjectDeletionPolicy<T>* GACALL GetInstance() { return &_instance; }

			/// <summary>This operator implements deletion policy using <c>delete</c> operator.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="memory">pointer to object that should be freed by the deletion policy.</param>
			virtual void GACALL operator ()(T* memory) const { delete memory; }

		};

		//#if defined(GAL_API_EXPORTING)

			template <typename T>
			GaObjectDeletionPolicy<T> GaObjectDeletionPolicy<T>::_instance;

		//#elif defined(GAL_PLATFORM_NIX) && !defined(GAL_SYNTAX_INTL) && !defined(GAL_SYNTAX_SUNC)

		//	template <typename T>
		//	extern GaObjectDeletionPolicy<T> GaObjectDeletionPolicy<T>::_instance;

		//#endif

		/// <summary><c>GaArrayDeletionPolicy</c> class implements default deletion policy for arrays using <c>delete[]</c> operator.</summary>
		/// <param name="T">type of objects in the array managed by this policy.</param>
		template<typename T>
		class GaArrayDeletionPolicy : public GaDeletionPolicy<T>
		{

		private:

			/// <summary>Global instance of default deletion policy for arrays of object of specified type.</summary>
			//GAL_API
			static GaArrayDeletionPolicy<T> _instance;

		public:

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns pointer to global instance of default deletion policy for arrays of object of specified type.</returns>
			static inline GaArrayDeletionPolicy<T>* GACALL GetInstance() { return &_instance; }

			/// <summary>This operator implements deletion policy using <c>delete[]</c> operator.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="memory">pointer to array of object that should be freed by the deletion policy.</param>
			virtual void GACALL operator ()(T* memory) const { delete[] memory; }

		};

		//#if defined(GAL_API_EXPORTING)

			template <typename T>
			GaArrayDeletionPolicy<T> GaArrayDeletionPolicy<T>::_instance;

		//#elif defined(GAL_PLATFORM_NIX) && !defined(GAL_SYNTAX_INTL) && !defined(GAL_SYNTAX_SUNC)

		//	template <typename T>
		//	extern GaArrayDeletionPolicy<T> GaArrayDeletionPolicy<T>::_instance;

		//#endif

		/// <summary><c>GaNoDeletionPolicy</c> class implements policy that does not perform any operations on provided object.</summary>
		/// <param name="T">type of object managed by this policy.</param>
		template<typename T>
		class GaNoDeletionPolicy : public GaDeletionPolicy<T>
		{

		private:

			/// <summary>Global instance of deletion policy for the specified type.</summary>
			//GAL_API
			static GaNoDeletionPolicy<T> _instance;

		public:

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns pointer to global instance of deletion policy for the specified type.</returns>
			static inline GaNoDeletionPolicy<T>* GACALL GetInstance() { return &_instance; }

			/// <summary>This operator implements does not perform operations on provided object.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="memory">pointer to object .</param>
			virtual void GACALL operator ()(T* memory) const { }

		};

		//#if defined(GAL_API_EXPORTING)

			template <typename T>
			GaNoDeletionPolicy<T> GaNoDeletionPolicy<T>::_instance;

		//#elif defined(GAL_PLATFORM_NIX) && !defined(GAL_SYNTAX_INTL) && !defined(GAL_SYNTAX_SUNC)

		//	template <typename T>
		//	extern GaNoDeletionPolicy<T> GaNoDeletionPolicy<T>::_instance;

		//#endif

		template <typename T>
		class GaSmartPtr;

		/// <summary><c>GaSmartStorage</c> template class provides reference-counting for smart pointers.
		/// <c>GaSmartStorage</c> objects holds address of user data and number of references (smart pointers) which point to the data.
		/// Memory used by user data and reference-counting object are destoryed when there is no more smar pointers that points to the data
		/// To use reference-counting object with arrays or objects from object pools custom deletion policy must be provided.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros
		/// cannot be used with instances of this class, but all public method and operators are thread-safe.</summary>
		template <typename T>
		class GaSmartStorage
		{

			friend class GaSmartPtr<T>;

		private:

			/// <summary>Holds number of references (smart pointers) to the data.</summary>
			mutable Threading::GaAtomic<int> _count;

			/// <summary>Pointer to user data.</summary>
			T* _data;

			/// <summary>Pointer to object that frees memory used by the data.</summary>
			const GaDeletionPolicy<T>* _delete;

		public:

			/// <summary>This method decrements number of references, if that number reaches 0, the data is destroyed and memory is freed.
			/// <c>RemoveReference</c> method is called when smart pointers are destructed or when set to point to new location.
			///
			/// This method is thread-safe.</summary>
			/// <param name="location">pointer to location which is decremented.</param>
			static inline void GACALL RemoveReference(GaSmartStorage<T>* location)
			{
				if( location && --location->_count == 0 )
					delete location;
			}

			/// <summary>Initializes reference-counting object with pointer to user data.</summary>
			/// <param name="data">pointer to user data.</param>
			GaSmartStorage(T* data) : _data(data),
				_delete(GaObjectDeletionPolicy<T>::GetInstance()) { }

			/// <summary>Initializes reference-counting object with pointer to user data and deletion object.</summary>
			/// <param name="data">pointer to user data.</param>
			/// <param name="del">pointer to object that frees memory used by the data.</param>
			GaSmartStorage(T* data,
				const GaDeletionPolicy<T>* del) : _count(0), 
				_data(data),
				_delete(del) { }

			/// <summary>Destructor is called when there are no more references which points to the data. Destructor frees memory used by the data. </summary>
			~GaSmartStorage()
			{
				// free user data using deletion object
				if( _data )
					( *_delete )( _data );
			}

			/// <summary>This method increments number of references to the data. <c>AddReference</c> method is called when smart pointer
			/// is set to point to the location.
			///
			/// This method is thread-safe.</summary>
			inline void GACALL AddReference() { ++_count; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns pointer to user data.</returns>
			inline T* GACALL GetData() const { return _data; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns number of references (smart pointers) which points to this location.</returns>
			inline int GACALL GetCount() const { return _count; }

		};

		/// <summary><c>GaSmartPtr</c> template class wraps C++ raw pointers, and takes over responsibility of managing the allocated memory.
		/// Smart pointer holds address of user data and reference to an object which is responsible for counting number of references to data,
		/// when there are no instances of <c>GaSmartPtr</c> pointing to location of the data (reference count of the location reaches 0),
		/// object is destroyed and memory used by the object is freed. Memory management by <c>GaSmartPtr</c> class is thread-safe,
		/// but after dereferencing smart pointer to access the data, it cannot be guaranteed that memory will not be freed if some other thread
		/// changes dereferenced pointer. To use smart pointers with arrays or objects from object pools custom deletion policy must be provided.
		/// Circular references may cause memory leakage.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of
		/// this class, but all public method and operators are thread-safe.</summary>
		/// <param name="T">type of data to which smart pointer references.</param>
		template <typename T>
		class GaSmartPtr
		{

		public:

			/// <summary><c>GaType</c> defines type of data which this instance of templte class references.</summary>
			typedef T GaType;

		private:

			GA_DECLARE_SPINLOCK( _lock );

			/// <summary>Pointer to user data.</summary>
			GaType* _data;

			/// <summary>Pointer to object which holds reference-count and address of data.</summary>
			GaSmartStorage<GaType>* _location;

		public:

			/// <summary><c>NullPtr</c> is global constant <c>NULL</c> pointer for <c>T</c> type.</summary>
			//GAL_API
			static const GaSmartPtr<GaType> NullPtr;

			/// <summary>This constructor makes new reference to data that are managed by <see cref="GaSmertStorage" />.</summary>
			/// <param name="storage">reference to object responsible for reference-counting.</param>
			GaSmartPtr(GaSmartStorage<GaType>* storage)
			{
				_location = storage;
				if( _location )
				{
					// new reference
					_location->AddReference();
					_data = _location->_data;
				}
				else
					_data = NULL;
			}

			/// <summary>This constructor make instance of <see cref="GaSmartStorage" /> and binds unmanaged memory to the smart storage.
			/// If the provided memory is already managed other smart pointer, it can cause unexpected results.</summary>
			/// <param name="rawPtr">raw pointer to user data.</param>
			GaSmartPtr(GaType* rawPtr)
			{
				_location = rawPtr ? new GaSmartStorage<GaType>( rawPtr ) : NULL;
				if( _location )
				{
					// new reference
					_location->AddReference();
					_data = _location->_data;
				}
				else
					_data = NULL;
			}

			/// <summary>This constructor make instance of <see cref="GaSmartStorage" /> and binds unmanaged memory to the smart storage.
			/// If the provided memory is already managed other smart pointer, it can cause unexpected results.</summary>
			/// <param name="rawPtr">raw pointer to user data.</param>
			/// <param name="del">deletiong policy.</param>
			GaSmartPtr(GaType* rawPtr,
				const GaDeletionPolicy<GaType>* del)
			{
				_location = rawPtr ? new GaSmartStorage<GaType>( rawPtr, del ) : NULL;
				if( _location )
				{
					// new reference
					_location->AddReference();
					_data = _location->_data;
				}
				else
					_data = NULL;
			}

			/// <summary>Copy constructor makes new reference to data to which <c>ptr</c> points.</summary>
			/// <param name="ptr">reference to smart pointer which should be copied.</param>
			GaSmartPtr(const GaSmartPtr<GaType>& ptr)
			{
				GA_LOCK( ptr._lock );

				_location = ptr._location;
				if( _location )
				{
					// new reference
					_location->AddReference();
					_data = _location->_data;
				}
				else
					_data = NULL;

				GA_UNLOCK( ptr._lock );
			}

			/// <summary>Default constructor, initializes pointer as <c>NULL</c> pointer.</summary>
			GaSmartPtr() : _location(NULL),
				_data(NULL) { }

			/// <summary>Decrements number of references to data. If there is no more references, memory used my data is freed and object is destroyed.</summary>
			~GaSmartPtr() { GaSmartStorage<GaType>::RemoveReference( _location ); }
			
			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns raw pointer to user data.</returns>
			inline GaType* GACALL GetRawPtr() { return _data; }
			
			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns raw pointer to user data.</returns>
			inline const GaType* GACALL GetRawPtr() const { return _data; }

			/// <summary>Checks pointer against <c>NULL</c> value.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Returns <c>true</c> if this is <c>NULL</c> pointer.</returns>
			inline bool GACALL IsNull() const { return !_data; }

			/// <summary>Operator provides access to data to which smart pointer points.
			///
			/// This method is thread safe.</summary>
			/// <returns>Operator returns pointer to user data.</returns>
			inline GaType* GACALL operator ->() { return _data; }

			/// <summary>Operator provides access to data to which smart pointer points.
			///
			/// This method is thread safe.</summary>
			/// <returns>Operator returns pointer to user data.</returns>
			inline const GaType* GACALL operator ->() const { return _data; }

			/// <summary>Operator provides access to data to which smart pointer points.
			///
			/// This method is thread safe.</summary>
			/// <returns>Operator returns reference to user data.</returns>
			inline GaType& GACALL operator *() { return *_data; }

			/// <summary>Operator provides access to data to which smart pointer points.
			///
			/// This method is thread safe.</summary>
			/// <returns>Operator returns reference to user data.</returns>
			inline const GaType& GACALL operator *() const { return *_data; }

			/// <summary>Operator provides access to data of the array to which smart pointer points.
			///
			/// This method is thread safe.</summary>
			/// <returns>Operator returns reference to user data.</returns>
			inline GaType& GACALL operator [](int index) { return *( _data + index ); }

			/// <summary>Operator provides access to data of the array to which smart pointer points.
			///
			/// This method is thread safe.</summary>
			/// <returns>Operator returns reference to user data.</returns>
			inline const GaType& GACALL operator [](int index) const { return *( _data + index ); }

			/// <summary>Checks pointer against <c>NULL</c> value.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Returns <c>true</c> if this is not <c>NULL</c> pointer.</returns>
			inline GACALL operator bool() { return _data != NULL; }

			/// <summary>This operator converts this pointer to smart pointer to constant data.
			///
			/// This operator is thread-safe.</summary>
			/// <returns>Operator returns smart pointer to constant data.</returns>
			inline GACALL operator GaSmartPtr<const GaType>() const
			{
				GA_LOCK( _lock );

				GaSmartPtr<const GaType> ptr( (GaSmartStorage<const GaType>*)_location );

				GA_UNLOCK( _lock );

				return ptr;
			}
			
			/// <summary>Sets smart pointer to points to same location as <c>rhs</c> pointer.
			/// It also decrements number of references of old smart locationa and increments number of new smart location.
			/// If number of references of old location reached zero, this operator frees used memory.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">smart pointer which holds address to which <c>this</c> pointer should point.</param>
			/// <returns>Operator returns reference to <c>this</c> object.</returns>
			inline GaSmartPtr<GaType>& GACALL operator =(const GaSmartPtr<GaType>& rhs)
			{
				if( this != &rhs )
				{
					GaSmartStorage<GaType>* old = NULL;

					_lock.Lock( &rhs._lock );

					if( rhs._location != _location )
					{
						old = _location;
						_location = rhs._location;
						if( _location )
						{
							// new reference
							_location->AddReference();
							_data = _location->_data;
						}
						else
							_data = NULL;
					}

					_lock.Unlock( &rhs._lock );

					// remove old reference
					GaSmartStorage<GaType>::RemoveReference( old );
				}
				return *this;
			}

			/// <summary>Sets smart pointer to points to <c>rhs</c> smart location for storing data.
			/// It also decrements number of references of old smart locationa and increments number of new smart location.
			/// If number of references of old location reached zero, this operator frees used memory.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">reference to smart location to which <c>this</c> pointer should point.</param>
			/// <returns>Operator returns reference to <c>this</c> object.</returns>
			inline GaSmartPtr<GaType>& GACALL operator =(GaSmartStorage<GaType>& rhs)
			{
				GaSmartStorage<GaType>* old = NULL;

				GA_LOCK( _lock );

				if( &rhs != _location )
				{
					old = _location;
					_location = &rhs;
					if( _location )
					{
						// new reference
						_location->AddReference();
						_data = _location->_data;
					}
					else
						_data = NULL;
				}

				GA_UNLOCK( _lock );

				// remove old reference
				GaSmartStorage<GaType>::RemoveReference( old );

				return *this;
			}

			/// <summary>This operator makes new instance of <see cref="GaSmartStorage" />, binds unmanaged memory to the smart storage and
			/// sets pointer to it. It also decrements number of references of old smart locationa and increments number of new smart location.
			/// When number of references of old location reaches zero, this operator frees used memory.
			/// If the provided memory is already managed other smart pointer, it can cause unexpected results.</summary>
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">raw pointer to user data which should be managed.</param>
			/// <returns>Operator returns reference to <c>this</c> object.</returns>
			inline GaSmartPtr<GaType>& GACALL operator =(GaType* rhs)
			{
				// make smart storage and bind data referenced by raw pointer to it
				GaSmartStorage<GaType>* newLocation = rhs ? new GaSmartStorage<GaType>( rhs ) : NULL;
				GaSmartStorage<GaType>* old = NULL;

				GA_LOCK( _lock );

				old = _location;

				_location = newLocation;
				if( _location )
				{
					// new reference
					_location->AddReference();
					_data = _location->_data;
				}
				else
					_data = NULL;

				GA_UNLOCK( _lock );

				// remove old reference
				GaSmartStorage<GaType>::RemoveReference( old );

				return *this;
			}

			/// <summary>Compares two smart pointers to see they points to same data.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">the second smart pointer in the expression.</param>
			/// <returns>Operator returns <c>true</c> if two pointers point to same location.</returns>
			inline bool GACALL operator ==(const GaSmartPtr<GaType>& rhs) const { return _location == rhs._location; }

			/// <summary>Compares this smart pointer with an raw pointer to see if they points to different data.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">raw pointer in the expression.</param>
			/// <returns>Operator returns <c>true</c> if two pointers point to different location.</returns>
			inline bool GACALL operator ==(const GaType* rhs) const { return _data == rhs; }

			/// <summary>Compares two smart pointers to see they points to same data.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">the second smart pointer in the expression.</param>
			/// <returns>Operator returns <c>true</c> if two pointers point to same location.</returns>
			inline bool GACALL operator !=(const GaSmartPtr<GaType>& rhs) const { return _location != rhs._location; }

			/// <summary>Compares this smart pointer with an raw pointer to see if they points to different data.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">raw pointer in the expression.</param>
			/// <returns>Operator returns <c>true</c> if two pointers point to different location.</returns>
			inline bool GACALL operator !=(const GaType* rhs) const { return _data != rhs; }

		};

		//#if defined(GAL_API_EXPORTING)

			template <typename T>
			const GaSmartPtr<T> GaSmartPtr<T>::NullPtr;

		//#elif defined(GAL_PLATFORM_NIX) && !defined(GAL_SYNTAX_INTL) && !defined(GAL_SYNTAX_SUNC)

		//	template <typename T>
		//	extern const GaSmartPtr<T> GaSmartPtr<T>::NullPtr;

		//#endif

		/// <summary><c>GaAutoPtr</c> template class wraps C++ raw pointers, and takes responsibility for the memory.
		/// When the auto pointer is out of scope or if the new memory is assigned to it, previously assigned memory is freed and object is destructed.
		/// It is possible to detach memory from the auto pointer and it can be assigned to another pointer or it can be managed manually.
		/// It is not possible that two or more auto pointers manage same memory. To use smart pointers with arrays or objects from object pools custom deletion policy must be provided.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros
		/// cannot be used with instances of this class, but all public method and operators are thread-safe.</summary>
		template <typename T>
		class GaAutoPtr
		{

		public:

			/// <summary><c>GaType</c> defines type of data which this instance of templte class references.</summary>
			typedef T GaType;

		private:

			GA_DECLARE_SPINLOCK( _lock );
			
			/// <summary>Pointer to user data.</summary>
			GaType* _data;

			/// <summary>Pointer to object that frees memory used by the data.</summary>
			const GaDeletionPolicy<GaType>* _delete;

		public:

			/// <summary><c>NullPtr</c> is global constant <c>NULL</c> pointer for <c>T</c> type.</summary>
			//GAL_API
			static const GaAutoPtr<GaType> NullPtr;

			/// <summary>This constructor binds unmanaged memory to the auto pointer.</summary>
			/// <param name="data">raw pointer to user data which should be managed.</param>
			GaAutoPtr(GaType* data) : _data(data),
				_delete(GaObjectDeletionPolicy<GaType>::GetInstance()) { }

			/// <summary>This constructor binds unmanaged memory to the auto pointer and sets deletion policy.</summary>
			/// <param name="data">raw pointer to user data which should be managed.</param>
			/// <param name="del">pointer to object that frees memory used by the data.</param>
			GaAutoPtr(GaType* data,
				const GaDeletionPolicy<GaType>* del) : _data(data),
				_delete(del) { }

			/// <summary>This constructor takes ownership of the memory from specified auto pointer and assign it to this auto pointer.</summary>
			/// <param name="data">auto pointer from which the ownership should be taken over.</param>
			GaAutoPtr(GaAutoPtr<GaType>& data)
			{
				GA_LOCK( data._lock );

				// set pointer to the memory
				_data = data._data;
				_delete = data._delete;

				// detach memory from previous pointer
				data._data = NULL;
				data._delete = NULL;

				GA_UNLOCK( data._lock );
			}

			/// <summary>Default constructor, initializes pointer as <c>NULL</c> pointer.</summary>
			GaAutoPtr() : _data(NULL),
				_delete(NULL) { }

			/// <summary>The destructor frees memory to which this pointer points.</summary>
			~GaAutoPtr()
			{
				// free user data using deletion object
				if( _data )
					( *_delete )( _data );
			}

			/// <summary><c>MakeWeak</c> method creates auto pointer that referenes same data but does not take ownership and has no deletion policy.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns weak auto pointer to data.</returns>
			inline GaAutoPtr<GaType> GACALL MakeWeak() { return GaAutoPtr<GaType>( _data, &GaNoDeletionPolicy<GaType>::_instance ); }

			/// <summary><c>DetachPointer</c> method removes ownership of memory from the auto pointer and sets this pointer to <c>NULL.</c>.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns raw pointer to data to which the auto pointer has been previously pointed.</returns>
			inline GaType* GACALL DetachPointer()
			{
				GA_LOCK( _lock );

				// save old value and clear pointer
				GaType* old = _data;
				_data = NULL;
				_delete = NULL;

				GA_UNLOCK( _lock );

				return old;
			}

			/// <summary><c>SetPointer</c> method binds unmanaged memory to the auto pointer and sets deletion policy.
			///
			/// This method is thread-safe.</summary>
			/// <param name="data">raw pointer to user data which should be managed.</param>
			/// <param name="del">pointer to object that frees memory used by the data.</param>
			inline void GACALL SetPointer(GaType* data,
				const GaDeletionPolicy<GaType>* del)
			{
				GA_LOCK( _lock );

				if( data == _data )
				{
					GA_UNLOCK( _lock );
					return;
				}

				// save pointer to old memory
				GaType* oldData = _data;
				const GaDeletionPolicy<GaType>* oldDelete = _delete;

				// set new pointer to new memory
				_data = data;
				_delete = del;

				GA_UNLOCK( _lock );

				// frees previously managed memory using deletion object
				if( oldData )
					( *oldDelete )( oldData );
			}

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns raw pointer to user data.</returns>
			inline GaType* GACALL GetRawPtr() { return _data; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns raw pointer to user data.</returns>
			inline const GaType* GACALL GetRawPtr() const { return _data; }

			/// <summary>Checks pointer against <c>NULL</c> value.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Returns <c>true</c> if this is <c>NULL</c> pointer.</returns>
			inline bool GACALL IsNull() const { return !_data; }

			/// <summary>Operator provides access to data to which auto pointer points.
			///
			/// This method is thread safe.</summary>
			/// <returns>Operator returns pointer to user data.</returns>
			inline GaType* GACALL operator ->() { return _data; }

			/// <summary>Operator provides access to data to which auto pointer points.
			///
			/// This method is thread safe.</summary>
			/// <returns>Operator returns pointer to user data.</returns>
			inline const GaType* GACALL operator ->() const { return _data; }

			/// <summary>Operator provides access to data to which auto pointer points.
			///
			/// This method is thread safe.</summary>
			/// <returns>Operator returns reference to user data.</returns>
			inline GaType& GACALL operator *() { return *_data; }

			/// <summary>Operator provides access to data to which auto pointer points.
			///
			/// This method is thread safe.</summary>
			/// <returns>Operator returns reference to user data.</returns>
			inline const GaType& GACALL operator *() const { return *_data; }

			/// <summary>Operator provides access to data of the array to which smart pointer points.
			///
			/// This method is thread safe.</summary>
			/// <returns>Operator returns reference to user data.</returns>
			inline GaType& GACALL operator [](int index) { return *( _data + index ); }

			/// <summary>Operator provides access to data of the array to which smart pointer points.
			///
			/// This method is thread safe.</summary>
			/// <returns>Operator returns reference to user data.</returns>
			inline const GaType& GACALL operator [](int index) const { return *( _data + index ); }

			/// <summary>Checks pointer against <c>NULL</c> value.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Returns <c>true</c> if this is not <c>NULL</c> pointer.</returns>
			inline GACALL operator bool() { return _data != NULL; }

			/// <summary>This operator converts this pointer to auto pointer to constant data and detaches data from this pointer.
			///
			/// This operator is thread-safe.</summary>
			/// <returns>Operator returns auto pointer to constant data.</returns>
			inline GACALL operator GaAutoPtr<const GaType>()
			{
				GA_LOCK( _lock );

				// save data
				GaType* data = _data;
				GaDeletionPolicy<GaType>* del = _delete;

				// detach memory from previous pointer
				_data = NULL;
				_delete = NULL;

				GA_UNLOCK( _lock );

				return GaAutoPtr<const GaType>( data, del );
			}

			/// <summary>This operator detaches data from this auto pointer and binds it to reference-counting smart pointer.
			///
			/// This operator is thread-safe.</summary>
			/// <returns>Operator returns reference-counting smart pointer to user data.</returns>
			inline GACALL operator GaSmartPtr<GaType>()
			{
				GA_LOCK( _lock );

				// save data
				GaType* data = _data;
				const GaDeletionPolicy<GaType>* del = _delete;

				// detach memory from previous pointer
				_data = NULL;
				_delete = NULL;

				GA_UNLOCK( _lock );

				return GaSmartPtr<GaType>( data, del );
			}

			/// <summary>This operator binds unmanaged memory to the auto pointer. It also frees memory to which this pointer has been previously pointing.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">raw pointer to user data which should be managed.</param>
			/// <returns>Operator returns reference to <c>this</c> object.</returns>
			inline GaAutoPtr<GaType>& GACALL operator =(GaType* rhs)
			{
				GA_LOCK( _lock );

				// pointer did not changed
				if( _data == rhs )
				{
					GA_UNLOCK( _lock );
					return *this;
				}

				// save pointer to old memory
				GaType* oldData = _data;
				const GaDeletionPolicy<GaType>* oldDelete = _delete;

				// set new pointer to new memory
				_data = rhs;
				_delete = GaObjectDeletionPolicy<GaType>::GetInstance();

				GA_UNLOCK( _lock );

				// frees previously managed memory using deletion object
				if( oldData )
					( *oldDelete )( oldData );

				return *this;
			}

			/// <summary>This operator takes ownership of the memory from source auto pointer and assign it to this auto pointer.
			/// It also frees memory to which this pointer has been previously pointing.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">auto pointer from which the ownership should be taken over.</param>
			/// <returns>Operator returns reference to <c>this</c> object.</returns>
			inline GaAutoPtr<GaType>& GACALL operator =(GaAutoPtr<GaType>& rhs)
			{
				_lock.Lock( &rhs._lock );

				// save pointer to old memory
				GaType* oldData = _data;
				const GaDeletionPolicy<GaType>* oldDelete = _delete;

				// set new pointer to new memory
				_data = rhs._data;
				_delete = rhs._delete;

				// detach memory from previous pointer
				rhs._data = NULL;
				rhs._delete = NULL;

				_lock.Unlock( &rhs._lock );
				
				// frees previously managed memory using deletion object
				if( oldData )
					( *oldDelete )( oldData );

				return *this;
			}

			/// <summary>Compares two auto pointers to see if they points to same data.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">the second auto pointer in the expression.</param>
			/// <returns>Operator returns <c>true</c> if two pointers point to same location.</returns>
			inline bool GACALL operator ==(const GaAutoPtr<GaType>& rhs) { return _data == rhs._data; }

			/// <summary>Compares this auto pointer with an raw pointer to see if they points to same data.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">raw pointer in the expression.</param>
			/// <returns>Operator returns <c>true</c> if two pointers point to same location.</returns>
			inline bool GACALL operator ==(const GaType* rhs) { return _data == rhs; }

			/// <summary>Compares two auto pointers to see if they points to different data.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">the second auto pointer in the expression.</param>
			/// <returns>Operator returns <c>true</c> if two pointers point to different location.</returns>
			inline bool GACALL operator !=(const GaAutoPtr<GaType>& rhs) { return _data != rhs._data; }

			/// <summary>Compares this auto pointer with an raw pointer to see if they points to different data.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">raw pointer in the expression.</param>
			/// <returns>Operator returns <c>true</c> if two pointers point to different location.</returns>
			inline bool GACALL operator !=(const GaType* rhs) { return _data != rhs; }

		};

		//#if defined(GAL_API_EXPORTING)

			template <typename T>
			const GaAutoPtr<T> GaAutoPtr<T>::NullPtr;

		//#elif defined(GAL_PLATFORM_NIX) && !defined(GAL_SYNTAX_INTL) && !defined(GAL_SYNTAX_SUNC)

		//	template <typename T>
		//	extern const GaAutoPtr<T> GaAutoPtr<T>::NullPtr;

		//#endif

	} // Memory
} // Common

#endif //__GA_SMART_PTR_H__

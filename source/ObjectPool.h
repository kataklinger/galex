
/*! \file ObjectPool.h
    \brief This file contains classes that are used for implementation of object pools.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_OBJECT_POOL_H__
#define __GA_OBJECT_POOL_H__

#include "Exceptions.h"
#include "SmartPtr.h"
#include "AtomicList.h"

namespace Common
{
	namespace Memory
	{

		/// <summary><c>GaPoolObjectCreate</c> template provides basic creation and initialization of object using using <c>operator new</c> and default constructor.</summary>
		/// <typeparam name="T">type of object.</typeparam>
		template<typename T>
		class GaPoolObjectCreate
		{

		public:

			/// <summary>This operator creates new object and initializes it using <c>operator new</c> and default constructor.
			///
			/// This operator is thread-safe.</summary>
			/// <returns>Returns pointer to newly created object.</returns>
			inline T* GACALL operator() () const { return new T(); }

		};

		/// <summary><c>GaPoolObjectDelete</c> template provides basic deletion policy for object pool using <c>operator delete</c> and destructor of the specified type.</summary>
		/// <typeparam name="T">type of object.</typeparam>
		template<typename T>
		class GaPoolObjectDelete
		{

		public:

			/// <summary><c>operator ()</c> destorys provided object by calling <c>operator delete</c> and object destructor to cleanup.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="object">pointer to object which should be destroyed.</param>
			inline void GACALL operator ()(T* object) const { delete object; }

		};

		/// <summary><c>GaPoolObjectInit</c> template proveds default initialization policy for the object pool that does not perform any initialization
		/// when object is retreived from the pool.</summary>
		/// <typeparam name="T">type of object.</typeparam>
		template<typename T>
		class GaPoolObjectInit
		{

		public:

			/// <summary><c>operator ()</c> initializes existing object that has been retreived from the object pool.</summary>
			/// <param name="object">pointer to obect which should be initialized before use.</param>
			inline void GACALL operator() (T* object) const { }

		};

		/// <summary><c>GaPoolObjectClean</c> template proveds default cleaning policy for the object pool that does not perform any clean up
		/// when object is returned to the pool.</summary>
		/// <typeparam name="T">type of object.</typeparam>
		template<typename T>
		class GaPoolObjectClean
		{

		public:

			/// <summary><c>operator ()</c> cleans provided object before the object is returned to pool of free object.</summary>
			/// <param name="object">pointer to object which should be cleaned.</param>
			inline void GACALL operator ()(T* object) const { }

		};

		/// <summary><c>GaPoolObjectUpdate</c> template is used as an interface by object pools to apply changes to the objects in the pool.
		/// This class does not implement any behaviors.</summary>
		/// <typeparam name="T">type of object.</typeparam>
		template<typename T>
		class GaPoolObjectUpdate
		{

		public:

			/// <summary><c>operator ()</c> updates provided object. This operator should be overridden in classes that inherits this class.</summary>
			/// <param name="object">pointer to object which should be updated.</param>
			virtual void GACALL operator ()(T* object) const = 0;

		};

		/// <summary><c>GaObjectPool</c> template class manages object pools. Object pools are used for recycling object to improve performace. 
		/// Recycling only cleans and initializes only nessesry parts of object and it releases object's memory only when the pool is full and it cannot be stored or
		/// when the pool is invalidate. Object pool uses four lifecycle control objects (creation, pereparation, cleanup and destruction objects) to menaged
		/// life of objects used by the pool.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros
		/// cannot be used with instances of this class. Only <c>AcquireObject</c> and <c>ReleaseObject</c> methods are thread-safe other methods are not thread-safe.</summary>
		/// <typeparam name="TYPE">type of objects in the pool.</typeparam>
		/// <typeparam name="CREATE_POLICY">type of creation policy.</typeparam>
		/// <typeparam name="DELETE_POLICY">type of deletion policy.</typeparam>
		/// <typeparam name="INIT_POLICY">type initialization policy.</typeparam>
		/// <typeparam name="CLEAN_POLICY">type of cleanup policy.</typeparam>
		template<typename TYPE,
			typename CREATE_POLICY = GaPoolObjectCreate<TYPE>,
			typename DELETE_POLICY = GaPoolObjectDelete<TYPE>,
			typename INIT_POLICY = GaPoolObjectInit<TYPE>,
			typename CLEAN_POLICY = GaPoolObjectClean<TYPE> >
		class GaObjectPool
		{

		public:

			/// <summary>Type of objects in the pool.</summary>
			typedef TYPE GaType;

			/// <summary>Type of creation policy.</summary>
			typedef CREATE_POLICY GaCreate;

			/// <summary>Type of deletion policy.</summary>
			typedef DELETE_POLICY GaDelete;

			/// <summary>Type of initialization policy.</summary>
			typedef INIT_POLICY GaInit;

			/// <summary>Type of cleanup policy.</summary>
			typedef CLEAN_POLICY GaClean;

			/// <summary>Type of update policy.</summary>
			typedef GaPoolObjectUpdate<GaType> GaUpdate;

		private:

			/// <summary><c>GaPoolDeletionPolicy</c> class implements deletion policy that returns object to its pool.</summary>
			class GaPoolDeletionPolicy : public GaDeletionPolicy<GaType>
			{

			private:

				/// <summary>Pointer to pool to which objects is returned by this policy.</summary>
				GaObjectPool<GaType, GaCreate, GaDelete, GaInit, GaClean>* _pool;

			public:

				/// <summary>This constructor initializes deletion policy with unspecified pool.</summary>
				GaPoolDeletionPolicy() : _pool(NULL) { }

				/// <summary>This operator implements deletion policy by returning objects to their object pools.</summary>
				/// <param name="memory">pointer to object that should be freed by the deletion policy.</param>
				virtual void GACALL operator ()(GaType* memory) const { _pool->ReleaseObject( memory ); }

				/// <summary><c>SetPool</c> sets pool that to which this deletion policy should return freed objects.</summary>
				/// <param name="pool">pointer to pool to which objects is returned by this policy.</param>
				inline void SetPool(GaObjectPool<GaType, GaCreate, GaDelete, GaInit, GaClean>* pool) { _pool = pool; }

			};

			/// <summary>List of pointers to objects which are currenty in the pool.</summary>
			Data::GaAtomicList<GaType> _entries;

			/// <summary>Number of object that this pool can store.</summary>
			int _size;

			/// <summary>Oobject that creates and initializes new object for the pool.</summary>
			GaCreate _create;

			/// <summary>Object that destorys objects when they are removed from the pool.</summary>
			GaDelete _delete;

			/// <summary>Object that prepares existing object retrieved from the pool.</summary>
			GaInit _init;

			/// <summary>Object that cleanup objects before they get returned to the pool.</summary>
			GaClean _clean;

			/// <summary>Pointer to object that is used by smart pointer as deletion policy.</summary>
			GaPoolDeletionPolicy _deletionPolicy;

		public:

			/// <summary>This constructor initializes pool with all lifecycle control objects (pool takes respnsibility for memory used by these object).</summary>
			/// <param name="size">number of object that this pool can store.</param>
			/// <param name="create">pointer to object that creates and initializes new object for the pool. This pointer cannot be <c>NULL</c>.</param>
			/// <param name="del">pointer to object that destorys objects when they are removed from the pool. This pointer cannot be <c>NULL</c>.</param>
			/// <param name="init">>pointer to object that prepares existing object retrieved from the pool.</param>
			/// <param name="clean">pointer to object that cleanup objects before they get returned to the pool.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if the <c>size</c> has negative value.</exception>
			GaObjectPool(int size,
				const GaCreate& create = GaCreate(),
				const GaDelete& del = GaDelete(),
				const GaInit& init = GaInit(),
				const GaClean& clean = GaClean()) : _size(0),
				_create(create),
				_delete(del),
				_init(init),
				_clean(clean)
			{
				_deletionPolicy.SetPool( this );
				SetSize( size );
			}

			/// <summary>Destorys object left in the pool.</summary>
			~GaObjectPool() { Invalidate(); }

			/// <summary><c>AcquireObject</c> returns pointer to object retrieved from the pool. The object is initialized using preparation object if it is provided.
			/// If the pool is empty this method creates new object using provided creation object.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns pointer to retrieved object.</returns>
			GaType* GACALL AcquireObject()
			{
				GaType* entry = _entries.Pop();

				if( !entry )
					// create new object if the pool is empty
					return _create();
				else
				{
					// initialize retrieved object
					_init( entry );
					return entry;
				}
			}

			/// <summary><c>AcquireObjectWithAutoPtr</c> retrieve object from the pool and returns auto pointer to it.
			/// More details are provided in sepcification of <see cref="AcquireObject" /> method to retrieve.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns auto pointer to retrieved object.</returns>
			inline GaAutoPtr<GaType> GACALL AcquireObjectWithAutoPtr() { return GaAutoPtr<GaType>( AcquireObject(), &_deletionPolicy ); }

			/// <summary><c>AcquireObjectWithSmartPtr</c> retrieve object from the pool and returns smart pointer to it.
			/// More details are provided in sepcification of <see cref="AcquireObject" /> method to retrieve.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns smart pointer to retrieved object.</returns>
			inline GaSmartPtr<GaType> GACALL AcquireObjectWithSmartPtr() { return GaSmartPtr<GaType>( AcquireObject(), &_deletionPolicy ); }

			/// <summary><c>ReleaseObject</c> method returns obect to the pool. This operation performs cleanup of the object if the cleanup object is provided.
			/// If the pool is full, this method destorys object uding provided destruction object.
			///
			/// This method is thread-safe.</summary>
			/// <param name="object">pointer to object which should be returned to the pool.</param>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>object</c> to <c>NULL</c>.</exception>
			void GACALL ReleaseObject(GaType* object)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, object != NULL, "object", "Object must be specified.", "Memory" );

				// cleanup released object
				_clean( object );

				// return object to the pool if it is not full
				if( _entries.GetCount() < _size )
					_entries.Push( object );
				else
					// destroy object if it cannot be stored in the pool
					_delete( object );
			}

			/// <summary><c>Invalidate</c> method destorys all objects in the pool.
			///
			/// This method is thread-safe.</summary>
			void GACALL Invalidate()
			{
				for( GaType *entry = _entries.Clear(), *next = NULL; entry; entry = next )
				{
					next = _entries.GetNext( entry );
					_delete( entry );
				}
			}

			/// <summary><c>UpdateObjects</c> method applies changes to all objects in the pool using provided update object.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="update">pointer to object that implements update method.</param>
			void UpdateObjects(GaUpdate& update)
			{
				for( GaType* entry = _entries.GetHead(); entry; entry = _entries.GetNext( entry ) )
					update( entry );
			}

			/// <summary>This method is not thread-safe</summary>
			/// <returns>Method returns number of objects that this pool can store.</returns>
			inline int GACALL GetSize() const { return _size; }

			/// <summary><c>SetSize</c> method sets size of the pool (number of object that the pool can store).
			///
			/// This method is not thread-safe.</summary>
			/// <param name="size">new size of the pool.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if the <c>size</c> has negative value.</exception>
			inline void GACALL SetSize(int size)
			{
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, size >= 0, "size", "Size cannot be negative value.", "Memory" );
				_size = size;
			}

			/// <summary><c>SetObjectCreate</c> method sets object that creates and initializes new object for the pool. User must provide this object to the pool.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="create">creation object.</param>
			inline void GACALL SetObjectCreate(const GaCreate& create) { _create = create; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to object that cretaes and initializes new object for the pool.</returns>
			inline GaCreate& GACALL GetObjectCreate() { return _create; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to object that cretaes and initializes new object for the pool.</returns>
			inline const GaCreate& GACALL GetObjectCreate() const { return _create; }

			/// <summary><c>SetObjectInit</c> method sets object that prepares existing object retrieved from the pool.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="init">initialization object.</param>
			inline void GACALL SetObjectInit(const GaInit& init) { _init = init; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to object that prepares existing object retrieved from the pool.</returns>
			inline GaInit& GACALL GetObjectInit() { return _init; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to object that prepares existing object retrieved from the pool.</returns>
			inline const GaInit& GACALL GetObjectInit() const { return _init; }

			/// <summary><c>SetObjectDelete</c> method sets object that destorys objects when they are removed from the pool. User must provide this object to the pool.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="del">deletion object.</param>
			inline void GACALL SetObjectDelete(const GaDelete& del) { _delete = del; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to object that destorys objects when they are removed from the pool.</returns>
			inline GaDelete& GACALL GetObjectDelete() { return _delete; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to object that destorys objects when they are removed from the pool.</returns>
			inline const GaDelete& GACALL GetObjectDelete() const { return _delete; }

			/// <summary><c>SetObjectClean</c> method sets object that cleanup objects before they get returned to the pool.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="clean">cleanup object.</param>
			inline void GACALL SetObjectClean(const GaClean& clean) { _clean = clean; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to object that cleanup objects before they get returned to the pool.</returns>
			inline GaClean& GACALL GetObjectClean() { return _clean; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to object that cleanup objects before they get returned to the pool.</returns>
			inline const GaClean& GACALL GetObjectClean() const { return _clean; }

		};

	} // Memory
} // Common

#endif // __GA_OBJECT_POOL_H__

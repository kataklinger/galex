
/*! \file Workflows.h
	\brief This file contains declaration of classes of workflow framework.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_WORKFLOWS_H__
#define __GA_WORKFLOWS_H__

#include <vector>
#include <set>
#include "Hashmap.h"
#include "Exceptions.h"
#include "Flags.h"
#include "ThreadPool.h"
#include "Barrier.h"
#include "SmartPtr.h"

namespace Common
{
	/// <summary>Contains classes and datatypes that implements workflow framework.</summary>
	namespace Workflows
	{

		class GaDataStorage;

		/// <summary><c>GaDataEntryBase</c> class provides basic mechanism (such as reference counting) for storing data stored into the workflow data storage.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public method and operators are thread-safe.</summary>
		class GaDataEntryBase
		{

			friend class GaDataStorage;

		private:

			/// <summary>ID of the data in the storage object.</summary>
			int _dataID;

			/// <summary>Pointer to storage object that owns this entry.</summary>
			GaDataStorage* _storage;

			/// <summary>Number of references to this data entry of storage object.</summary>
			mutable Threading::GaAtomic<int> _referenceCount;

		public:

			/// <summary><c>RemoveReference</c> method decrements number of references to this entry.
			/// Number of references is incremented each time the <see cref="GaDataStorage::GetData" /> or <see cref="GaDataStorage::FindData" /> methods called,
			/// so user must call this method when the access to this entry is no longer needed so the data can be disposed.
			///
			/// This method is thread-safe.</summary>
			/// <param name="entry">pointer to data entry.</param>
			static inline void GACALL RemoveReference(const GaDataEntryBase* entry)
			{
				if( entry )
					--entry->_referenceCount;
			}

			/// <summary>The constructor initializes data entry.</summary>
			/// <param name="dataID">ID of the data in the storage object.</param>
			GaDataEntryBase(int dataID) : _dataID(dataID) { }

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaDataEntryBase() { }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns ID of the data in the storage object.</returns>
			inline int GACALL GetDataID() const { return _dataID; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Returns pointer to storage object that is owner of the entry.</returns>
			inline GaDataStorage* GACALL GetDataStorage() { return _storage; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Returns pointer to storage object that is owner of the entry.</returns>
			inline const GaDataStorage* GACALL GetDataStorage() const { return _storage; }

		private:

			/// <summary>Increment number of references to this entry.</summary>
			inline void GACALL AddReference() const { ++_referenceCount; }

			/// <summary><c>GetReferenceCount</c> method returns number of references to this entry..</summary>
			/// <returns>Method returns current number of references to this entry.</returns>
			inline int GACALL GetReferenceCount() const { return _referenceCount; }

			/// <summary>Sets data storage to which this entry belongs.</summary>
			/// <param name="storage">pointer to storage boject that will be owner of the entry.</param>
			inline void GACALL SetDataStorage(GaDataStorage* storage) { _storage = storage; }

		};

		/// <summary><c>GaDataEntry</c> template class represent storage for data of specified type in workflow storage object.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public method and operators are thread-safe.</summary>
		/// <param name="DATA_TYPE">type of data that entry stores.</param>
		template<typename DATA_TYPE>
		class GaDataEntry : public GaDataEntryBase
		{

		public:

			/// <summary>Type of data that entry stores.</summary>
			typedef DATA_TYPE GaDataType;

		private:

			/// <summary>Smart pointer to stored data.</summary>
			Common::Memory::GaAutoPtr<GaDataType> _data;

		public:

			/// <summary>The constructor initializes entry and stores data.</summary>
			/// <param name="dataID">ID of the data in the storage object.</param>
			/// <param name="data">smart pointer to data that will be stored in this entry.</param>
			GaDataEntry(int dataID,
				Common::Memory::GaAutoPtr<GaDataType> data) : GaDataEntryBase(dataID),
				_data(data) { }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns reference to data stored in this entry.</returns>
			inline GaDataType& GACALL GetData() { return *_data; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns reference to data stored in this entry.</returns>
			inline const GaDataType& GACALL GetData() const { return *_data; }

		};

		/// <summary>This enumeration defines level of data storages (<see cref="GaDataStorage" />).</summary>
		enum GaDataStorageLevel
		{
			/// <summary>Data of the global storage level are available in all workflows and their subelements.</summary>
			GADSL_GLOBAL = 0,

			/// <summary>Data of the workflow storage level are available in single workflow and its elements (except in inner workflows).</summary>
			GADSL_WORKFLOW,

			/// <summary>Data of the branch group storage level are available in single branch groups.</summary>
			GADSL_BRANCH_GROUP,

			/// <summary>Data of the branch storage level are available in branch groups.</summary>
			GADSL_BRANCH,

			/// <summary>This value defines number of storage levels in the library.</summary>
			GADSL_NUMBER_OF_LEVELS
		};

		/// <summary><c>GaDataStorage</c> class represents storage object for data of the workflows. Each storage object has specified level defined by 
		/// <see cref="GaDataStorageLevel" /> which determines scope of stored data. Storage objects has connections with storage object at higher level
		/// so it is possibile access global data using local storage objects. Data are stored as a pair of ID and actual data.
		///
		/// This class has built-in synchronizator so it is allowed to use <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros with instances of this class.
		/// All public methods are thread-safe.</summary>
		class GaDataStorage
		{

			GA_SYNC_CLASS

		private:

			/// <summary>Hash map which stores data of the storage objects.</summary>
			STLEXT::hash_map<int, GaDataEntryBase*> _data;

			/// <summary>Level of the storage object.</summary>
			GaDataStorageLevel _level;

			/// <summary>Table of pointers to storage objects which are on higher level then this object's level.</summary>
			GaDataStorage* _levelTable[ GADSL_NUMBER_OF_LEVELS ];

		public:

			/// <summary>Initializes storage object with its level and connections to higher level objects.</summary>
			/// <param name="level">level of this storage object.</param>
			/// <param name="...">pointers to higher level objects. Pointers must be passes in order, from highest level object to the lowest level object.</param>
			GAL_API
			GaDataStorage(GaDataStorageLevel level, ...);

			/// <summary>Free resources used by the data storage.<summary>
			GAL_API
			~GaDataStorage();

			/// <summary><c>SetHigherLevelStorage</c> method sets pointer to higher level storage object.
			///
			/// This method is thread-safe.</summary>
			/// <param name="level">level for which the storage object is set.</param>
			/// <param name="storage">pointer to new storage object.</param>
			/// <exception cref="GaArgumentException" />Thrown when <c>level</c> is below this object's level.</exception>
			GAL_API
			void GACALL SetHigherLevelStorage(GaDataStorageLevel level,
				GaDataStorage* storage);

			/// <summary><c>GetHigherLevelStorage</c> returns pointer to higher level storage object.
			///
			/// This method is thread-safe.</summary>
			/// <param name="level">level of the storage object.</param>
			/// <returns>Method returns pointer to the storage object at the specified level, or <c>NULL</c> if it is not available.</returns>
			inline GaDataStorage* GACALL GetHigherLevelStorage(GaDataStorageLevel level)
			{
				GA_LOCK_THIS_OBJECT( lock );
				return _levelTable[ level ];
			}

			/// <summary><c>GetHigherLevelStorage</c> returns pointer to higher level storage object.
			///
			/// This method is thread-safe.</summary>
			/// <param name="level">level of the storage object.</param>
			/// <returns>Method returns pointer to the storage object at the specified level, or <c>NULL</c> if it is not available.</returns>
			inline const GaDataStorage* GACALL GetHigherLevelStorage(GaDataStorageLevel level) const
			{
				GA_LOCK_THIS_OBJECT( lock );
				return _levelTable[ level ];
			}

			/// <summary><c>FindData</c> method searches for data with specified ID. Caller specifies range of searched storage object,
			/// by defining the lower level and the higher level objects. User must call <see cref="GaDataStorage::GaWorkflowDataEntry::CloseReference"/> after
			/// the data is no longer needed.
			///
			/// This method is thread-safe.</summary>
			/// <param name="startStorageLevel">the lowest level at which the search begins.</param>
			/// <param name="maxStorageDepth">the highest level object which will be searched in.</param>
			/// <param name="dataID">ID of the searched data.</param>
			/// <returns>Returns pointer to entry which contains data that has specified ID if it is found, or <c>NULL</c> if the data is not found.</returns>
			/// <exception cref="GaArgumentException" />Thrown when <c>startStorageLevel</c> is higher then <c>maxStorageDepth</c> or if it is below this object's level.</exception>
			GAL_API
			GaDataEntryBase* GACALL FindData(GaDataStorageLevel startStorageLevel,
				GaDataStorageLevel maxStorageDepth,
				int dataID);

			/// <summary><c>FindData</c> method searches for data with specified ID in this object and higher level objects. Caller specifies highest level searched.
			/// by defining the lower level and the higher level objects. User must call <see cref="GaDataStorage::GaWorkflowDataEntry::CloseReference"/> after
			/// the data is no longer needed.
			///
			/// This method is thread-safe.</summary>
			/// <param name="maxDepth">the highest level object which will be searched in.</param>
			/// <param name="dataID">ID of the searched data.</param>
			/// <returns>Returns pointer to entry which contains data that has specified ID if it is found, or <c>NULL</c> if the data is not found.</returns>
			/// <exception cref="GaArgumentException" />Thrown when the level of this object is higher then <c>maxDepth</c>.</exception>
			inline GaDataEntryBase* GACALL FindData(GaDataStorageLevel maxDepth,
				int dataID) { return FindData( _level, maxDepth, dataID ); }

			/// <summary><c>FindData</c> method searches for data with specified ID in this object and all higher level objects. 
			/// by defining the lower level and the higher level objects. User must call <see cref="GaDataStorage::GaWorkflowDataEntry::CloseReference"/> after
			/// the data is no longer needed.
			///
			/// This method is thread-safe.</summary>
			/// <param name="dataID">ID of the searched data.</param>
			/// <returns>Returns pointer to entry which contains data that has specified ID if it is found, or <c>NULL</c> if the data is not found.</returns>
			inline GaDataEntryBase* GACALL FindData(int dataID) { return FindData( _level, GADSL_GLOBAL, dataID ); }

			/// <summary><c>GetData</c> returns pointer to data with specified ID at specified storage level.
			/// User must call <see cref="GaDataStorage::GaWorkflowDataEntry::CloseReference"/> after the data is no longer needed.
			///
			/// This method is thread-safe.</summary>
			/// <param name="level">storage level which is queried for data.</param>
			/// <param name="dataID">ID of the queried data.</param>
			/// <returns>Method returns pointer to entry which contains data with specified ID, or <c>NULL</c> if the data does not exist.</returns>
			/// <exception cref="GaArgumentException" />Thrown when the <c>level</c> is below level of this object.</exception>
			GAL_API
			inline GaDataEntryBase* GACALL GetData(GaDataStorageLevel level,
				int dataID);

			/// <summary><c>GetData</c> method returns pointer to data with specified ID stored in this object.
			/// User must call <see cref="GaDataStorage::GaWorkflowDataEntry::CloseReference"/> after the data is no longer needed.
			///
			/// This method is thread-safe.</summary>
			/// <param name="dataID">ID of the queried data.</param>
			/// <returns>Method returns pointer to entry which contains data with specified ID, or <c>NULL</c> if the data does not exist.</returns>
			inline GaDataEntryBase* GACALL GetData(int dataID)
			{
				GA_LOCK_THIS_OBJECT( lock );
				STLEXT::hash_map<int, GaDataEntryBase*>::iterator it = _data.find( dataID );
				if( it != _data.end() )
				{
					it->second->AddReference();
					return it->second;
				}
				
				return NULL;
			}

			/// <summary><c>AddData</c> method inserts new data into the storage object.
			///
			/// This method is thread-safe.</summary>
			/// <param name="entry">pointer to entry object that contains data.</param>
			/// <param name="level">level at which the data should be inserted.</param>
			/// <exception cref="GaNullArgumentException" />Thrown when the <c>data</c> is <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when the <c>level</c> is below level of this object or 
			/// when data with the specified ID already exists in the storage object.</exception>
			GAL_API
			void GACALL AddData(GaDataEntryBase* entry,
				GaDataStorageLevel level);

			/// <summary><c>RemoveData</c> method removes data from the storage object and deletes them.
			///
			/// This method is thread-safe.</summary>
			/// <param name="dataID">ID of the data which should be removed.</param>
			/// <param name="level">level at which the data is stored.</param>
			/// <exception cref="GaArgumentException" />Thrown when the <c>level</c> is below level of this object.</exception>
			/// <exception cref="GaInvalidOperationtException" />Thrown if the specified data entry has valid reference to it.</exception>
			GAL_API
			void GACALL RemoveData(int dataID,
				GaDataStorageLevel level);

			/// <summary><c>Clear</c> method removes all date from the storage.
			///
			/// This method is thread-safe.</summary>
			inline void GACALL Clear()
			{
				GA_LOCK_THIS_OBJECT( lock );
				_data.clear();
			}

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns storage level of this storage.</returns>
			inline GaDataStorageLevel GetStorageLevel() const { return _level; }

		};

		/// <summary><c>GaDataCache</c> class caches data from single entry of the storage object. User should use smart pointers to manipulate cached data.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public methods are thread-safe.</summary>
		/// <param name="DATA_TYPE">type of cached data.</param>
		template<typename DATA_TYPE>
		class GaDataCache
		{

		public:

			/// <summary>Type of cached data.</summary>
			typedef DATA_TYPE GaDataType;

			/// <summary>Type of workflow data entry that contains data.</summary>
			typedef GaDataEntry<DATA_TYPE> GaEntryType;

		private:

			/// <summary>Pointer to entry of the storage object that contains data.</summary>
			GaEntryType* _data;

		public:

			/// <summary>This constructor initializes cache object with data ID and retrieves data entry from storage object.</summary>
			/// <param name="dataStorage">pointer to storage object whose data should be cached.</param>
			/// <param name="dataID">ID of the data that should be cached.</param>
			GaDataCache(GaDataStorage* dataStorage,
				int dataID) : _data(NULL) { SetData( dataStorage, dataID ); }

			/// <summary>This constructor initializes cache object from provided entry object of storage object.</summary>
			/// <param name="data">pointer to entry of the storage object whose data should be cached.</param>
			GaDataCache(GaEntryType* data) { operator =( data ); }

			/// <summary>This constructor initializes empty cache object.</summary>
			GaDataCache() : _data(NULL) { }

			/// <summary><c>Clear</c> method clears cache object and removes references to cached data.</summary>
			~GaDataCache() { Clear(); }

			/// <summary><c>SetData</c> method caches data from specified storage boject with defined data ID.
			/// If the cache object already contains cached data they are cleared and references to these data are removed.
			///
			/// This method is thread-safe.</summary>
			/// <param name="dataStorage">storage object whose data should be cached.</param>
			/// <param name="dataID">ID of the data that should be cached.</param>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>dataStorage</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when object specified by <c>dataStorage</c> parameter is branch level storage.</exception>
			void GACALL SetData(GaDataStorage* dataStorage,
				int dataID)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, dataStorage != NULL, "dataStorage", "Storage object must be specified.", "Workflows" );
				GA_ARG_ASSERT( Exceptions::GaArgumentException, dataStorage->GetStorageLevel() != GADSL_BRANCH,
					"dataStorage", "Cannot use storage object at branch level to cache data.", "Workflows" );

				GaDataEntryBase::RemoveReference( Threading::GaAtomicOps<GaEntryType*>::Xchg( &_data, (GaEntryType*)dataStorage->GetData( dataID ) ) );
			}

			/// <summary><c>SetData</c> method caches data from specified entry of data storage object.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="entry">pointer to entry of storage object whose data should be cached.</param>
			inline void GACALL SetData(GaEntryType* entry)
				{ GaDataStorage::GaWorkflowDataEntry::RemoveReference( Threading::GaAtomicOps<GaEntryType*>::Xchg( &_data, (GaEntryType*)entry ) ); }

			/// <summary><c>Clear</c> method clears cache object and removes references to cached data.
			///
			/// This method is thread-safe.</summary>
			inline void GACALL Clear() { GaDataEntryBase::RemoveReference( Threading::GaAtomicOps<GaEntryType*>::Xchg( &_data, NULL ) ); }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns smart pointer to cached data.</returns>
			inline typename GaDataType& GACALL GetData() { return _data->GetData(); }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns smart pointer to cached data.</returns>
			inline typename const GaDataType& GACALL GetData() const { return _data->GetData(); }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns identification number of cached data.</returns>
			inline int GACALL GetDataID() const { return _data->GetDataID(); }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if this object has cached data.</returns>
			inline bool GACALL HasData() const { return _data != NULL; }

			/// <summary>This operator caches data from specified entry of data storage object.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="rhs">pointer to entry of storage object whose data should be cached.</param>
			inline GaDataCache<GaDataType>& GACALL operator =(GaEntryType* rhs) { SetData( rhs ); return *this; }

		private:

			/// <summary>Prevents copying of the cache object.</summary>
			GaDataCache(const GaDataCache<GaDataType>&) { }

			/// <summary>Prevents copying of the cache object.</summary>
			GaDataCache<GaDataType>& operator =(const GaDataCache<GaDataType>&) { return *this; }

		};

		/// <summary><c>GaDataBinder</c> class manages binding of workflow data. It updates destination each time user bind new source.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Public methods are not thread-safe.</summary>
		/// <param name="SOURCE_TYPE">type of source data.</param>
		/// <param name="DESTINATION_TYPE">type of destination data.</param>
		template<typename SOURCE_TYPE,
			typename DESTINATION_TYPE>
		class GaDataBinder
		{

		public:

			/// <summary>Type of source data.</summary>
			typedef SOURCE_TYPE GaSourceType;

			/// <summary>Type of destination data.</summary>
			typedef DESTINATION_TYPE GaDestinationType;

			/// <summary>Type of pointer to method that updates destination with reference to source data.</summary>
			typedef void (GACALL GaDestinationType::*GaSourceUpdateMethod)(GaSourceType* data);

		private:

			/// <summary>Cached source data.</summary>
			GaDataCache<GaSourceType> _source;

			/// <summary>Cached destination data.</summary>
			GaDataCache<GaDestinationType> _destination;

			/// <summary>Pointer to method that updates destination with reference to source data.</summary>
			GaSourceUpdateMethod _updateMethod;

		public:

			/// <summary>Initializes empty binder and binds specified data.</summary>
			/// <param name="sourceDataStorage">ID of the source data.</param>
			/// <param name="sourceDataID">pointer to storage object that stores source data.</param>
			/// <param name="destinationDataStorage">pointer to storage object that stores destination data.</param>
			/// <param name="destinationDataID">ID of the destination data.</param>
			/// <param name="updateMethod">pointer to method that updates destination with reference to source data.</param>
			GaDataBinder(GaDataStorage* sourceDataStorage,
				int sourceDataID,
				GaDataStorage* destinationDataStorage,
				int destinationDataID,
				GaSourceUpdateMethod updateMethod) : _source(sourceDataID, sourceDataStorage),
				_destination(destinationDataID, destinationDataStorage),
				_updateMethod(updateMethod) { }

			/// <summary>Initializes empty binder.</summary>
			GaDataBinder() { }

			/// <summary>Disconnect binding.</summary>
			~GaDataBinder() { ClearDestination(); }

			/// <summary><c>Unbind</c> method unbinds the source or destination.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="source">if this parameter is set to <c>true</c> this method will remove source from the binding.</param>
			/// <param name="destination">if this parameter is set to <c>true</c> this method will remove source from the destination.</param>
			void GACALL Unbind(bool source,
				bool destination)
			{
				if( source || destination )
					ClearDestination();

				if( source )
					_source.Clear();

				if( destination )
					_destination.Clear();
			}

			/// <summary><c>SetSource</c> method sets source of the binder. If destination is already bound, this method clears destination's reference to previous source data
			/// and stores new reference into destination.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="dataStorage">pointer to storage object that stores source data.</param>
			/// <param name="dataID">ID of the source data.</param>
			void GACALL SetSource(GaDataStorage* dataStorage,
				int dataID)
			{
				_source.SetData( dataStorage, dataID );
				UpdateDestination();
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to source data.</returns>
			inline GaSourceType& GetSourceData() { return _source.GetData(); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to source data.</returns>
			inline const GaSourceType& GetSourceData() const { return _source.GetData(); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if destination is specified.</returns>
			inline bool GACALL IsSourceBound() { return _source.HasData(); }

			/// <summary><c>SetDestination</c> method sets destination of the binder. If destination is already bound, this method clears destination's reference to source data
			/// and stores reference to source into new destination using provided method.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="dataStorage">pointer to storage object that stores destination data.</param>
			/// <param name="dataID">ID of the destination data.</param>
			/// <param name="updateMethod">pointer to method that updates destination with reference to source data.</param>
			void GACALL SetDestination(GaDataStorage* dataStorage,
				int dataID,
				GaSourceUpdateMethod updateMethod)
			{
				ClearDestination();

				_updateMethod = updateMethod;
				_destination.SetData( dataStorage, dataID );
				UpdateDestination();
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to destination data.</returns>
			inline GaDestinationType& GetDestinationData() { return _destination.GetData(); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to destination data.</returns>
			inline const GaDestinationType& GetDestinationData() const { return _destination.GetData(); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if destination is specified.</returns>
			inline bool GACALL IsDestinationBound() { return _destination.HasData(); }

		private:

			/// <summary>Updates destination with new source data.</summary>
			inline void GACALL UpdateDestination()
			{
				if( _destination.HasData() )
					( _destination.GetData().*_updateMethod )( _source.HasData() ? &_source.GetData() : NULL );
			}

			/// <summary>Clears destination's reference to source data.</summary>
			inline void GACALL ClearDestination()
			{
				if( _destination.HasData() )
					( _destination.GetData().*_updateMethod )( NULL );
			}

		};

		class GaFlowStep;
		class GaFlowConnection;

		/// <summary><c>GaFlow</c> class is base class for flows in the library. Flow is set of steps (objects that performs some task) and
		/// connections (directional objects that defines order of execution of the steps).</summary>
		class GaFlow
		{

		public:

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaFlow() { }

			/// <summary><c>AddStep</c> method should inserts setp object to the flow.</summary>
			/// <param name="step">pointer to the step object.</param>
			virtual void GACALL AddStep(GaFlowStep* step) = 0;

			/// <summary><c>RemoveStep</c> method removes step from the flow. Only the steps which are not connected to any other step can be removed from the flow/</summary>
			/// <param name="step">pointer to step objec that should be removed.</param>
			/// <param name="disconnect">if this parameter is set to <c>true</c> this method disconnect specified step before removing it from the flow.</param>
			/// <param name="destroy">if this parameter is set to <c>true</c> this method will destroy  step object and free used resources.</param>
			virtual void GACALL RemoveStep(GaFlowStep* step,
				bool disconnect,
				bool destroy) = 0;

			/// <summary><c>ConnectSteps</c> method make directional connection between two steps which are members of the flow.
			/// If the steps do not belong to this flow they are automatically added.</summary>
			/// <param name="outboundStep">pointer to step object to whose output point this connection is attached.</param>
			/// <param name="inboundStep">pointer to step object to whose input point this connection is attached.</param>
			/// <param name="connectionID">ID can be used by flow or its subelements to identify connection, make decisions about execution or to define type of the connection.</param>
			/// <returns>Method returns pointer to the connection object that just has been made to connect these two steps.</returns>
			virtual GaFlowConnection* GACALL ConnectSteps(GaFlowStep* outboundStep,
				GaFlowStep* inboundStep,
				int connectionID) = 0;

			/// <summary><c>RemoveConnection</c> method disconnect steps connected by this object and removes it from the flow.</summary>
			/// <param name="connection">pointer to connection object which should be removed.</param>
			/// <param name="destory">if this parameter is set to <c>true</c> this method will distory connection object and free used resources.</param>
			virtual void GACALL RemoveConnection(GaFlowConnection* connection,
				bool destory) = 0;

		};

		class GaBranch;

		/// <summary><c>GaFlowStep</c> is base class for flow steps. Step object performs single operation in the flow and it can be member of only one flow at a time.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaFlowStep
		{

		protected:

			/// <summary>Pointer to flow to which this step belongs.</summary>
			GaFlow* _flow;

			/// <summary>List of pointer to outbound connection objects (connections attached to output points of the step).</summary>
			std::set<GaFlowConnection*> _outboundConnections;

			/// <summary>List of pointer to inbound connection objects (connections attached to input points of the step).</summary>
			std::set<GaFlowConnection*> _inboundConnections;

		public:

			/// <summary>Initializes flow step.</summary>
			GaFlowStep() : _flow(NULL) { }

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaFlowStep() { }

			/// <summary><c>operator ()</c> performs step operation. This operator should be overridden to perform required action.</summary>
			/// <param name="branch">pointer to branch that executes this step.</param>
			virtual void GACALL operator ()(GaBranch* branch) = 0;

			/// <summary><c>Enter</c> method determins whether the branch should perfrom step's operation and do preparations for its execution.</summary>
			/// <param name="branch">pointer to branch that executes this step.</param>
			/// <returns>Method should return <c>true</c> if branch should proceed with execution of step's operation.</returns>
			virtual bool GACALL Enter(GaBranch* branch) { return true; }

			/// <summary><c>Exit</c> method performs additional actions after the step's operation is executed.</summary>
			/// <param name="branch">pointer to branch that executes this step.</param>
			virtual void GACALL Exit(GaBranch* branch) { }

			/// <summary><c>BindToFlow</c> method binds step to a single flow after it was inserted into that flow.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="flow">pointer to flow to which this step is added.</param>
			/// <exception cref="GaInvalidOperationException" />Thrown when the step already belongs to this or some other flow.</exception>
			virtual void GACALL BindToFlow(GaFlow* flow)
			{
				GA_ASSERT( Exceptions::GaInvalidOperationException, _flow == NULL, "Step already belongs to this or some other flow.", "Workflows" );
				_flow = flow;
			}

			/// <summary><c>UnbindFromFlow</c> method removes binding between step and the flow to which it belonged.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL UnbindFromFlow() { _flow = NULL; }

			/// <summary><c>FlowUpdated</c> method is called by flow when it's changed.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL FlowUpdated() { }

			/// <summary><c>GetNextStep</c> method should return pointer to the next step which should be performed by the branch.
			/// Returned step does not have to be same all the thim it can depend of the state of the branch which requests the next step.
			/// This method should be overridden by classes that inherits this class.</summary>
			/// <param name="branch">pointer to branch that has executed this step.</param>
			/// <returns>Method returns pointer to next step, or <c>NULL</c> if there is no next step for the branch to execute.</returns>
			virtual GaFlowStep* GACALL GetNextStep(GaBranch* branch) = 0;

			/// <summary><c>AttachNextStep</c> method connnects two steps by attaching input point of the next step with output point of this step.
			/// This method should be overridden by classes that inherits this class.</summary>
			/// <param name="nextStep">pointer to object which connects these two steps.</param>
			virtual void GACALL AttachNextStep(GaFlowConnection* nextStep) = 0;

			/// <summary><c>AttachNextStep</c> method connnects two steps by attaching output point of the previous step with input point of this step.
			/// This method should be overridden by classes that inherits this class.</summary>
			/// <param name="previousStep">pointer to object which connects these two steps.</param>
			virtual void GACALL AttachPreviousStep(GaFlowConnection* previousStep) = 0;

			/// <summary><c>DetachNextStep</c> method disconnect two steps by detaching input point of the next step with output point of this step.
			/// This method should be overridden by classes that inherits this class.</summary>
			/// <param name="nextStep">pointer to connection object which should be disconnected.</param>
			virtual void GACALL DetachNextStep(GaFlowConnection* nextStep) = 0;

			/// <summary><c>DetachPreviousStep</c> method disconnect two steps by detaching output point of the previous step with input point of this step.
			/// This method should be overridden by classes that inherits this class.</summary>
			/// <param name="previousStep">pointer to connection object which should be disconnected.</param>
			virtual void GACALL DetachPreviousStep(GaFlowConnection* previousStep) = 0;

			/// <summary><c>ConnectionIDChanged</c> method notifies flow step that ID of the one connection attached to this step has changed.
			/// This method should be overridden by classes that inherits this class.</summary>
			/// <param name="connection">pointer to connection object whose ID has changed.</param>
			/// <param name="oldConnectionID">old ID of the connection.</param>
			virtual void GACALL ConnectionIDChanged(GaFlowConnection* connection,
				int oldConnectionID) = 0;

			/// <summary>Disconnects all connections attached to output points of the step.
			///
			/// This method is not thread-safe.</summary>
			GAL_API
			void GACALL DisconnectAllOutboundConnections();

			/// <summary>Disconnects all connections attached to input points of the step.
			///
			/// This method is not thread-safe.</summary>
			GAL_API
			void GACALL DisconnectAllInboundConnections();

			/// <summary>Disconnects all connections attached to input or output points of the step.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL DisconnectAllConnections()
			{
				DisconnectAllOutboundConnections();
				DisconnectAllInboundConnections();
			}

			/// <summary><c>SetFlow</c> method sets new flow as the owner of the step.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="flow">pointer to new flow that owns this step.</param>
			inline void GACALL SetFlow(GaFlow* flow) { _flow = flow; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to flow that owns this step.</returns>
			inline GaFlow* GACALL GetFlow() { return _flow; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to flow that owns this step.</returns>
			inline const GaFlow* GACALL GetFlow() const { return _flow; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to set of outbound connection.</returns>
			inline std::set<GaFlowConnection*>& GACALL GetOutboundConnections() { return _outboundConnections; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to set of outbound connection.</returns>
			inline const std::set<GaFlowConnection*>& GACALL GetOutboundConnections() const { return _outboundConnections; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to set of inbound connection.</returns>
			inline std::set<GaFlowConnection*>& GACALL GetInboundConnections() { return _inboundConnections; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to set of inbound connection.</returns>
			inline const std::set<GaFlowConnection*>& GACALL GetInboundConnections() const { return _inboundConnections; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if the step has at least one connection attached to its output points.</returns>
			inline bool GACALL HasOutboundConnections() const { return _outboundConnections.size() > 0; }

			/// <summary>This method is not thread-safe.</summary>
			/// <param name="connection">pointer to queried connection object.</param>
			/// <returns>Method returns <c>true</c> if the step has attached specified connection object to one of its input points.</returns>
			inline bool GACALL HasOutboundConnection(GaFlowConnection* connection) const { return _outboundConnections.find( connection ) != _outboundConnections.end(); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if the step has at least one connection attached to its input points.</returns>
			inline bool GACALL HasInboundConnections() const { return _inboundConnections.size() > 0; }

			/// <summary>This method is not thread-safe.</summary>
			/// <param name="connection">pointer to queried connection object.</param>
			/// <returns>Method returns <c>true</c> if the step has attached specified connection object to one of its output points.</returns>
			inline bool GACALL HasInboundConnection(GaFlowConnection* connection) const { return _inboundConnections.find( connection ) != _inboundConnections.end(); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if the step has at least one connection attached to its output or input points.</returns>
			inline bool GACALL HasConnections() const { return HasOutboundConnections() || HasInboundConnections(); }

		protected:

			/// <summary>This method adds connection object to the set of inbound connection.</summary>
			/// <param name="connection">pointer to connection object that should be added to the set of connection.
			///
			/// This method is not thread-safe.</param>
			inline void GACALL AddOutboundConnection(GaFlowConnection* connection) { _outboundConnections.insert( connection ); }

			/// <summary>This method removes connection object to the set of outbound connection.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="connection">pointer to connection object that should be added to the set of connection.</param>
			inline void GACALL RemoveOutboundConnection(GaFlowConnection* connection) { _outboundConnections.erase( connection ); }

			/// <summary>This method adds connection object to the set of outbound connection.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="connection">pointer to connection object that should be added to the set of connection.</param>
			inline void GACALL AddInboundConnection(GaFlowConnection* connection) { _inboundConnections.insert( connection ); }

			/// <summary>This method removes connection object to the set of inbound connection.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="connection">pointer to connection object that should be added to the set of connection.</param>
			inline void GACALL RemoveInboundConnection(GaFlowConnection* connection) { _inboundConnections.erase( connection ); }

		};
		
		/// <summary><c>GaBasicStep</c> class handles attaching and detaching of inboud connections. 
		/// Flow steps that inherits this class should be able to handle multiple inbound connections.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaBasicStep : public GaFlowStep
		{

		public:

			/// <summary>For more details see specification of <see cref="GaFlowStep::AttachPreviousStep" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>previousStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when provided connection object has invalid configuration or if it is already attached tho this step.</exception>
			GAL_API
			virtual void GACALL AttachPreviousStep(GaFlowConnection* previousStep);

			/// <summary>For more details see specification of <see cref="GaFlowStep::DetachPreviousStep" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>previousStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when provided connection object is not attached tho this step.</exception>
			GAL_API
			virtual void GACALL DetachPreviousStep(GaFlowConnection* previousStep);

		};

		/// <summary><c>GaFlowConnection</c> is base class for connections between the flow steps. Connections are directional and defines order of execution of flow steps.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaFlowConnection
		{

		protected:

			/// <summary>Pointer to step to which output point this connection is attached.</summary>
			GaFlowStep* _outboundStep;

			/// <summary>Pointer to step to which input point this connection is attached.</summary>
			GaFlowStep* _inboundStep;

			/// <summary>Value assigned to connection which can be used by flows or their subelements to identify connections or type of connections.</summary>
			int _connectionID;

		public:

			/// <summary>This constructor initializes connection with default ID value (0).</summary>
			GaFlowConnection() : _outboundStep(NULL),
				_inboundStep(NULL),
				_connectionID(0) { }

			/// <summary>This constructor initializes connection with user-defined ID value.</summary>
			/// <param name="connectionID">connection ID.</param>
			GaFlowConnection(int connectionID) : _outboundStep(NULL),
				_inboundStep(NULL),
				_connectionID(connectionID) { }

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaFlowConnection() { }

			/// <summary><c>ConnectSteps</c> sets inbound and outbound steps and attaches connection to both of them if the connection is valid.
			/// The method uses <see cref="CheckConnectionValidity" /> to test connection validity.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="outboundStep">pointer to flow step to whose output point the connection is attached.</param>
			/// <param name="inboundStep">pointer to flow step to whose input point the connection is attached.</param>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>outboundStep</c> or <c>inboundStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when <c>outboundStep</c> or <c>inboundStep</c> cannot be make valid connected.</exception>
			/// <exception cref="GaInvalidOperationException" />Thrown when the connection already have set outbound or outbound step </exception>
			GAL_API
			void GACALL ConnectSteps(GaFlowStep* outboundStep,
				GaFlowStep* inboundStep);

			/// <summary><c>ConnectOutboundStep</c> sets outbound step of the connection. If both steps (outbound and inboud) are defined,
			/// this method also attaches connection object to both step if the connection is valid. The method uses <see cref="CheckConnectionValidity" />
			/// to test connection validity.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="outboundStep">pointer to outbound flow step of the connection.</param>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>outboundStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when <c>outboundStep</c> cannot make valid connection with inbound step.</exception>
			/// <exception cref="GaInvalidOperationException" />Thrown when the connection already have set outbound step.</exception>
			GAL_API
			void GACALL ConnectOutboundStep(GaFlowStep* outboundStep);

			/// <summary><c>ConnectOutboundStep</c> attaches connection to input point of inbound step. If both steps (outbound and inboud) are defined,
			/// this method also attaches connection object to both step if the connection is valid. The method uses <see cref="CheckConnectionValidity" />
			/// to test connection validity.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="inboundStep">pointer to inbound flow step of the connection.</param>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>inboundStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when <c>inboundStep</c> cannot make valid connection with outbound step.</exception>
			/// <exception cref="GaInvalidOperationException" />Thrown when connection already have set inbound step.</exception>
			GAL_API
			void GACALL ConnectInboundStep(GaFlowStep* inboundStep);

			/// <summary><c>DisconnectOutboundStep</c> method resets outbound steps of the connection and detach connection from inbound and outbound steps if it was attached.
			///
			/// This method is not thread-safe.</summary>
			GAL_API
			void GACALL DisconnectOutboundStep();

			/// <summary><c>DisconnectInboundStep</c> method resets inbound steps of the connection and detach connection from inbound and outbound steps if it was attached.
			///
			/// This method is not thread-safe.</summary>
			GAL_API
			void GACALL DisconnectInboundStep();

			/// <summary><c>DisconnectSteps</c> method resets both steps of the connection and detach connection from inbound and outbound steps if it was attached.
			///
			/// This method is not thread-safe.</summary>
			GAL_API
			void GACALL DisconnectSteps();

			/// <summary><c>SetConnectionID</c> method changes connection ID and notifies steps to which the connection is attached about the change.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="id">new connection ID.</param>
			GAL_API
			virtual void GACALL SetConnectionID(int id);

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns connection ID.</returns>
			inline int GACALL GetConnectionID() const { return _connectionID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to outbound step.</returns>
			inline GaFlowStep* GACALL GetOutboundStep() { return _outboundStep; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to outbound step.</returns>
			inline const GaFlowStep* GACALL GetOutboundStep() const { return _outboundStep; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to inbound step.</returns>
			inline GaFlowStep* GACALL GetInboundStep() { return _inboundStep; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to inbound step.</returns>
			inline const GaFlowStep* GACALL GetInboundStep() const { return _inboundStep; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if the connection has outbound step defined.</returns>
			inline bool GACALL HasOutboundStep() const { return _outboundStep != NULL; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if the connection has inbound step defined.</returns>
			inline bool GACALL HasInboundStep() const { return _inboundStep != NULL; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if the connection has both steps defined (outbound and inbound).</returns>
			inline bool GACALL IsFullyConnected() const { return _outboundStep != NULL && _inboundStep != NULL; }

		protected:

			/// <summary><c>CheckConnectionValidity</c> method test validity of the connection between two steps.
			/// This method should be overridden by inheriting classes to provide specific checking.</summary>
			/// <param name="outboundStep">pointer to outbound step to which connection should be attached.</param>
			/// <param name="inboundStep">pointer to inbound step to which connection should be attached.</param>
			/// <returns>Method returns <c>true</c> if the connection is valid.</returns>
			virtual bool GACALL CheckConnectionValidity(GaFlowStep* outboundStep,
				GaFlowStep* inboundStep) = 0;

		};

		class GaBranchGroup;

		/// <summary><c>GaBranchGroupFlow</c> class represents flow of branches that are members of branch group which owns this flow.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaBranchGroupFlow : public GaFlow
		{

		protected:

			/// <summary>Pointer to branch group that owns this flow.</summary>
			GaBranchGroup* _branchGroup;

			/// <summary>Steps in the flow.</summary>
			std::set<GaFlowStep*> _steps;

			/// <summary>Connections in the flow.</summary>
			std::set<GaFlowConnection*> _connections;

			/// <summary>Pointer to the first step executed by the branches of the group.</summary>
			GaFlowStep* _firstStep;

		public:

			/// <summary>This constructor initializes flow with its owner.</summary>
			/// <param name="branchGroup">pointer to branch group that owns this flow.</param>
			GaBranchGroupFlow(GaBranchGroup* branchGroup) : _branchGroup(branchGroup),
				_firstStep(NULL) { }

			/// <summary>Destorys step and connection objects that belongs to this flow.</summary>
			GAL_API
			virtual ~GaBranchGroupFlow();

			/// <summary>For more details see specification of <see cref="GaFlow::AddStep" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>step</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when the step is connected to some other step.</exception>
			GAL_API
			virtual void GACALL AddStep(GaFlowStep* step);

			/// <summary>The method does not destory removed step object. For more details see specification of <see cref="GaFlow::RemoveStep" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>step</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown if specified step does not belong to this flow.</exception>
			/// <exception cref="GaInvalidOperationException" />When trying to remove connected step without disconnecting it.</exception>
			GAL_API
			virtual void GACALL RemoveStep(GaFlowStep* step,
				bool disconnect,
				bool destroy);

			/// <summary>For more details see specification of <see cref="GaFlow::ConnectSteps" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>outboundStep</c> or <c>inboundStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown if the steps belongs to another flow.</exception>
			GAL_API
			GaFlowConnection* GACALL ConnectSteps(GaFlowStep* outboundStep,
				GaFlowStep* inboundStep,
				int connectionID);

			/// <summary>The method does not destory removed connection object. For more details see specification of <see cref="GaFlow::RemoveConnection" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>connection</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown if provided connection object does not belongs to this flow or when method is unable to disconnect step.</exception>
			GAL_API
			virtual void GACALL RemoveConnection(GaFlowConnection* connection,
				bool destory);

			/// <summary><c>SetFirstStep</c> method sets the first step which is executed by branches of the group which owns this flow.
			/// If the specified step is not member of this flow is is automatically added.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="step">new step which should be executed first.</param>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>step</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown if the step is memeber of another flow.</exception>
			GAL_API
			void GACALL SetFirstStep(GaFlowStep* step);
			
			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to branch group which executes this flow.</returns>
			inline GaBranchGroup& GACALL GetBranchGroup() { return *_branchGroup; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to branch group which executes this flow.</returns>
			inline const GaBranchGroup& GACALL GetBranchGroup() const { return *_branchGroup; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns set of steps in the flow.</returns>
			inline std::set<GaFlowStep*>& GACALL GetFlowSteps() { return _steps; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns set of steps in the flow.</returns>
			inline const std::set<GaFlowStep*>& GACALL GetFlowSteps() const { return _steps; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns set of connections in the flow.</returns>
			inline std::set<GaFlowConnection*>& GACALL GetConnections() { return _connections; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns set of connections in the flow.</returns>
			inline const std::set<GaFlowConnection*>& GACALL GetConnections() const { return _connections; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the first step that is execcuted by the branches.</returns>
			inline GaFlowStep* GACALL GetFirstStep() { return _firstStep; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the first step that is execcuted by the branches.</returns>
			inline const GaFlowStep* GACALL GetFirstStep() const { return _firstStep; }

		};

		/// <summary><c>GaBrachGroupFlowConnection</c> class represent type of connection that connects steps in branch group flow.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaBrachGroupFlowConnection : public GaFlowConnection
		{

		public:

			/// <summary>This constructor initializes connection ID with user-defined value.</summary>
			/// <param name="connectionID">value of the connection ID.</param>
			GaBrachGroupFlowConnection(int connectionID) : GaFlowConnection(connectionID) { }

		protected:

			/// <summary>For more details see specification of <see cref="GaFlowConnection::CheckConnectionValidity" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <returns>The method returns <c>true</c> if both steps belongs to the same flow.</returns>
			bool GACALL CheckConnectionValidity(GaFlowStep* outboundStep,
				GaFlowStep* inboundStep) { return outboundStep->GetFlow() == inboundStep->GetFlow(); }

		};

		/// <summary><c>GaBranchFilterInfo</c> class stores information for allowing or disallowing specific branches of the group to execute flow step.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaBranchFilterInfo
		{

		protected:

			/// <summary><c>GaFilterEntry</c> structure stores represent single entry in of the filter that stores branch information.</summary>
			struct GaFilterEntry
			{

				/// <summary>Indicates whether the branch is allowed by the filter.</summary>
				bool _allowed;

				/// <summary>Stores filtered ID of the branch.</summary>
				int _filteredID;

			};

			/// <summary>Indicates whether the filter is active.</summary>
			bool _active;

			/// <summary>Number of branches that are allowed by the filter.</summary>
			int _count;

			/// <summary>Entires that stores branch inforamtion.</summary>
			std::vector<GaFilterEntry> _filter;

		public:

			/// <summary>Initializes branch filter.</summary>
			GaBranchFilterInfo() : _active(false) { SetAll(); }

			/// <summary><c>Activate</c> turns on branch filter.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL Activate() { _active = true; }

			/// <summary><c>Deactivate</c> turns off branch filter.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL Deactivate() { _active = false; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if branch filter is turned on.</returns>
			inline bool GACALL IsActive() const { return _active; }

			/// <summary><c>GetFilteredID</c> method converts raw branch ID to ID of filtered branch.</summary>
			/// <param name="branchID">ID of the branch.</param>
			/// <returns>Method returns filtered branch ID.</returns>
			inline int GACALL GetFilteredID(int branchID) const { return _active ? ( _filter[ branchID ]._allowed ? _filter[ branchID ]._filteredID : -1 ) : branchID; }

			/// <summary><c>ClearBranchMask</c> method sets filter mask to disallow branch with specfied ID.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="branchID">ID of the branch.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>branchID</c> is negative or has ID value greater than the filter supports.</exception>
			GAL_API
			void GACALL ClearBranchMask(int branchID);

			/// <summary><c>SetBranchMask</c> method sets filter mask to allow branch with speficied ID.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="branchID">ID of the branch.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>branchID</c> is negative or has ID value greater than the filter supports.</exception>
			GAL_API
			void GACALL SetBranchMask(int branchID);

			/// <summary><c>ClearAll</c> method sets filter to disallow all branches.
			///
			/// This method is not thread-safe.</summary>
			GAL_API
			void GACALL ClearAll();

			/// <summary><c>SetAll</c> method sets filter to allow all branches.
			///
			/// This method is not thread-safe.</summary>
			GAL_API
			void GACALL SetAll();

			/// <summary><c>SetSize</c> method sets number of branch entries that filter can store.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="size">number of entries.</param>
			GAL_API
			void GACALL SetSize(int size);

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of branch entries that filter can store.</returns>
			inline int GACALL GetSize() const { return (int)_filter.size(); }

			/// <summary><c>GetBranchMask</c> method get state of mask for specified branch.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="branchID">ID of the branch.</param>
			/// <returns>Method returns <c>true</c> if the branch with specified ID is allowed.</returns>
			inline bool GACALL GetBranchMask(int branchID) const { return _filter[ branchID ]._allowed; }

			/// <summary><c>CanExecute</c> method verifies whthter the branch with specified ID is allowed.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="branchID">ID of the branch.</param>
			/// <returns>Method returns <c>true</c> if the branch with specified ID is allowed.</returns>
			inline bool GACALL CanExecute(int branchID) const { return !_active || GetBranchMask( branchID ); }

			/// <summary><c>GetBranchCount</c> method returns number of branches that are allowed by the filter.
			///
			/// Method returns mask of allowed threads.</summary>
			/// <returns>Method returns number of branches that are allowed by the filter.</returns>
			inline int GACALL GetBranchCount() const { return _active ? _count : GetSize(); }

		};

		/// <summary><c>GaBranchFilter</c> class in addition <c>GaBranchFilterInfo</c> class provides synchronization object for allowed branched.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaBranchFilter : public GaBranchFilterInfo
		{

		private:

			/// <summary>Barrier for synchronization of branches allowed by the filter.</summary>
			mutable Common::Threading::GaBarrier _barrier;

		public:

			/// <summary><c>SetInfo</c> method sets branch information of the filter.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="info">branch information.</param>
			inline void GACALL SetInfo(const GaBranchFilterInfo& info) { GaBranchFilterInfo::operator =( info ); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns barrier that can be used for synchronization of allowed branches.</returns>
			inline Common::Threading::GaBarrier& GACALL GetBarrier() const { return _barrier; }

		};

		/// <summary><c>GaBranch</c> class represent branch. Each branch represent independent thread of execution of flow steps. Branches are executed as work items of thread pool.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaBranch
		{

		private:

			/// <summary>Pointer to the branch group which owns this branch.</summary>
			GaBranchGroup* _branchGroup;

			/// <summary>ID of the branch in the branch group. Each branch has unique ID in branch group.
			/// This value is in interval (<c>0..number_of_branches-1</c>).</summary>
			int _branchID;

			/// <summary>Filter that this branch currently uses, if branch does not use any filters this attribute should be <c>NULL</c>.</summary>
			const GaBranchFilter* _currentFilter;

			/// <summary>Local data storage for the branch.</summary>
			GaDataStorage _data;

			/// <summary>Pointer to previously executed step by this branch.</summary>
			GaFlowStep* _previousStep;

			/// <summary>This attribute stores decision made by last decsiion step.</summary>
			int _lastDecision;

			/// <summary>Inidicates whether the branch executes its last step.</summary>
			bool _executingLastStep;

		public:

			/// <summary>This constructor initializes branch with information needed for starting the branch.</summary>
			/// <param name="branchGroup">pointer to the branch group which owns this branch.</param>
			/// <param name="branchID">ID of the branch in the branch group.</param>
			GAL_API
			GaBranch(GaBranchGroup* branchGroup,
				int branchID);

			/// <summary><c>StartBranch</c> method starts execution of the branch from the specified step.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="firstStep">pointer to the first step which is going to be executed by the branch. If this parameter is <c>NULL</c>
			/// branch start execution from the first step specified in branch group flow.</param>
			GAL_API
			void GACALL StartBranch(GaFlowStep* firstStep);
			
			/// <summary><c>SplitWork</c> method calculates equal distribution of workload among branches of the branch group.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="totalWork">size of total workload that should be done by all branches of the branch group.</param>
			/// <param name="branchWork">reference to variable which will store size of workload that branch should do.</param>
			GAL_API
			void GACALL SplitWork(int totalWork,
				int& branchWork);

			/// <summary><c>SplitWork</c> method calculates equal distribution of workload among branches of the branch group.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="totalWork">size of total workload that should be done by all branches of the branch group.</param>
			/// <param name="branchWork">reference to variable which will store size of workload that branch should do.</param>
			/// <param name="branchStart">starting index of workload that should be done by this branch.</param>
			GAL_API
			void GACALL SplitWork(int totalWork,
				int& branchWork,
				int& branchStart);

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to branch group which owns this branch.</returns>
			inline GaBranchGroup& GACALL GetBranchGroup() { return *_branchGroup; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to branch group which owns this branch.</returns>
			inline const GaBranchGroup& GACALL GetBranchGroup() const { return *_branchGroup; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of the branch in the branch group.</returns>
			inline int GACALL GetBranchID() const { return _branchID; }

			/// <summary><c>SetCurrentFilter</c> method sets filter that will be used by this branch.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="filter">pointer to the filter. If branch should not use any filters ste this parameter to <c>NULL</c>.</param>
			inline void GACALL SetCurrentFilter(const GaBranchFilter* filter) { _currentFilter = filter; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to filter that this branch currently uses, if branch does not use any filters method returns <c>NULL</c>.</returns>
			inline const GaBranchFilter* GetCurrentFilter() const { return _currentFilter; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns barrier that should be used for synchonizing branches in the group that executes current flow step.</returns>
			GAL_API
			Threading::GaBarrier& GACALL GetBarrier();

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of branches that should reach barrier.</returns>
			GAL_API
			int GACALL GetBarrierCount() const;

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns filtered ID of the branch if it is currently using filter of raw branch ID if the filter is not used.</returns>
			GAL_API
			int GACALL GetFilteredID() const;

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to storage object used to store data that are local this branch.</returns>
			inline GaDataStorage* GACALL GetData() { return &_data; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to storage object used to store data that are local this branch.</returns>
			inline const GaDataStorage* GACALL GetData() const { return &_data; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to previously executed step by this branch.</returns>
			inline GaFlowStep* GACALL GetPreviousStep() { return _previousStep; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to previously executed step by this branch.</returns>
			inline const GaFlowStep* GACALL GetPreviousStep() const { return _previousStep; }

			/// <summary><c>SetLastDecision</c> method stores decision made by decision step which has been executed by the branch.</summary>
			/// <param name="decision">integer number which represents decision.</param>
			inline void GACALL SetLastDecision(int decision) { _lastDecision = decision; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns integer number which repesents stored decision make by last executed decision step.</returns>
			inline int GACALL GetLastDecision() const { return _lastDecision; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if the branch should execute its last step.</returns>
			inline bool GACALL IsExecutingLastStep() const { return _executingLastStep; }

		private:

			/// <summary>Method that executes steps of the branch group flow. This method exits when the last step of the branch is executed.
			///
			/// This method is thread-safe.</summary>
			/// <param name="firstStep">pointer to the first step which is going to be executed by the branch.</param>
			GAL_API
			void GACALL BranchFlow(GaFlowStep* firstStep);

		};

		/// <summary><c>GaAbstractBarrier</c> is base class for barrier steps in the flow. Barrier is flow step which waits for all inbound branch groups to hit barrier
		/// before it performs an operation.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Only some methods are thread-safe.</summary>
		class GaAbstractBarrier : public GaBasicStep
		{

		protected:

			/// <summary>Counter of inbound branch groups that has finished execution.</summary>
			Threading::GaAtomic<int> _branchGroupHitCount;

		public:

			/// <summary>This constructor initializes barrier.</summary>
			GaAbstractBarrier() { }

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaAbstractBarrier() { }

			/// <summary><c>Enter</c> method triggers wait process which waits for all inbound branch groups to hit barrier before.
			/// More details are given in specification of <see cref="GaFlowStep::operator ()" />.
			///
			/// This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> then the last branch group hits barrier.</returns>
			GAL_API
			virtual bool GACALL Enter(GaBranch* branch);

			/// <summary>For more details see specification of <see cref="GaFlowStep::GetNextStep" /> method.</summary>
			/// <returns>This method always returns <c>NULL</c>, bacues no branch can cross barrier.</returns>
			virtual GaFlowStep* GACALL GetNextStep(GaBranch* branch) { return NULL; }

		protected:

			/// <summary><c>NumberOfInboundBranchGroups</c> determins number of inbound branch groups for the barrier.</summary>
			/// <summary>Method returns number of inbound branch groups.</summary>
			virtual int NumberOfInboundBranchGroups() const = 0;

		};

		class GaWorkflow;

		/// <summary><c>GaBranchGroup</c> class manages group branches that execute same branch group flow.
		/// Branch group is a type of connection that connects barrier steps in the workflow.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// Only some methods are thread-safe.</summary></summary>
		class GaBranchGroup : public GaFlowConnection
		{

		protected:

			/// <summary>Pointer to workflow to which this branch group belongs.</summary>
			GaWorkflow* _workflow;

			/// <summary>Array of branches owned by this group.</summary>
			std::vector<GaBranch*> _branches;

			/// <summary>Data storage for data that are available for all branches of this group.</summary>
			GaDataStorage _data;

			/// <summary>Flow that is executed by the branches of this group.</summary>
			GaBranchGroupFlow _branchGroupFlow;

			/// <summary>Pointer to step that should be executed as last step by the branches of the group.</summary>
			GaAbstractBarrier* _lastStep;

			/// <summary>Number of branches that are currently running.</summary>
			Threading::GaAtomic<int> _numberOfActiveBranches;

			/// <summary>Barrier that user can use to synchronize branches of the group.</summary>
			Threading::GaBarrier _barrier;

		public:

			/// <summary>This constructor creates group and its branches.</summary>
			/// <param name="workflow">pointer to workflow to which this branch group belongs.</param>
			/// <param name="numberOfBranches">number of branches that should be created.</param>
			GAL_API
			GaBranchGroup(GaWorkflow* workflow,
				int numberOfBranches);

			/// <summary>The destructor closes all branches.</summary>
			GAL_API
			virtual ~GaBranchGroup();

			/// <summary>Connection ID of the branch group is used to identify connection type, so it cannot be changed.
			/// More details are given in specification of <see cref="GaFlowConnection::SetConnectionID" />.</summary>
			/// <exception cref="GaInvalidOperationException" />Thrown always when this method is called.</exception>
			virtual void GACALL SetConnectionID(int id) { GA_THROW( Exceptions::GaInvalidOperationException, "Cannot change connection ID for this connection type.", "Workflows" ); }
			
			/// <summary><c>StartBranches</c> starts all branches of the group.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="firstStep">pointer to the first step which is going to be executed by the branch group. This parameter overriedes
			/// the default first step of the branch group flow. If this parameters is <c>NULL</c> branch group start with the default first step.</param>
			GAL_API
			void GACALL StartBranches(GaFlowStep* firstStep = NULL);

			/// <summary>Notifies branch group that one of its branches has finished execution. If this is the last running branch
			/// this method also signals the end of branch group.
			///
			/// This method is thread-safe.</summary>
			/// <param name="branch">pointer to branch of this group which has finished execution.</param>
			GAL_API
			void GACALL BranchEnd(GaBranch* branch);

			/// <summary><c>SetLastStep</c> sets step that should be executed last by all branches fo this group. If there is already some branches that waits
			/// for the last step to be set, this method releases them. It the step is already set call to this method has on effect.
			///
			/// This method is thread-safe.</summary>
			/// <param name="lastStep">pointer to step that is set as last step.</param>
			/// <returns>Method returns <c>true</c> if the set is set successfully, the last step was not set before this call.</returns>
			inline bool GACALL SetLastStep(GaAbstractBarrier* lastStep) { return Threading::GaAtomicOps<GaAbstractBarrier*>::CmpXchg( &_lastStep, NULL, lastStep ); }

			/// <summary><c>ExecuteBranchLastStep</c> method is called by the branches of the group when they have executed all steps of the branch group flow.
			/// The method executes last step of the branches. If it is not set, this methob blocks branch that execute this method until the last step is not set.
			/// It also notifies workflow that owns this group when all of the branches of this group have executed the last step.
			/// This method is thread-safe.</summary>
			/// <param name="branch">pointer to brach group which executes the last step.</param>
			GAL_API
			void GACALL ExecuteBranchLastStep(GaBranch* branch);

			/// <summary><c>CheckBranchGroupCompatibility</c> method checks whether this branch group is compatible with specified branch group
			/// which means that worklow control can be transfered between these two group using <see cref="GaBranchGroupTransition"/ > flow step.
			///
			/// This method is thread-safe.</summary>
			/// <param name="compatibleGroup">pointer to branch group whose compatibility should be tested with this branch group.</param>
			/// <param name="ignoreGroup">pointer to branch group that should be skiped during testing.</param>
			/// <returns>Method returns <c>true</c> if the two branch groups are compatible.</returns>
			inline bool GACALL GACALL CheckBranchGroupCompatibility(const GaBranchGroup* compatibleGroup,
				const GaBranchGroup* ignoreGroup = NULL) const
			{
				return ( CheckCompatibilityDown( compatibleGroup, ignoreGroup ) && compatibleGroup->CheckCompatibilityUp( this, ignoreGroup ) ) ||
					( compatibleGroup->CheckCompatibilityDown( this, ignoreGroup ) && CheckCompatibilityUp( compatibleGroup, ignoreGroup ) ); 
			}
			
			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to workflow to which this branch group belongs.</returns>
			inline GaWorkflow* GACALL GetWorkflow() { return _workflow; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to workflow to which this branch group belongs.</returns>
			inline const GaWorkflow* GACALL GetWorkflow() const { return _workflow; }

			/// <summary><c>SetBranchCount</c> method sets number of branches in the group. If the new number is greater
			/// this method creates new branches, otherwise it destroys unneeded branches.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="count">new number of branches that group will contain.</param>
			GAL_API
			void GACALL SetBranchCount(int count);

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of branches in the group.</returns>
			inline int GACALL GetBranchCount() const { return (int)_branches.size(); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to flow that is executed by the branches of this group.</returns>
			inline GaBranchGroupFlow* GACALL GetBranchGroupFlow() { return &_branchGroupFlow; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to flow that is executed by the branches of this group.</returns>
			inline const GaBranchGroupFlow* GACALL GetBranchGroupFlow() const { return &_branchGroupFlow; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to storage object for data that are available for all branches of this group.</returns>
			inline GaDataStorage* GACALL GetData() { return &_data; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to storage object for data that are available for all branches of this group.</returns>
			inline const GaDataStorage* GACALL GetData() const { return &_data; }

			/// <summary>This method is thread-safe.<summary>
			/// <returns>Method returns pointer to barrier object that user can use to synchronize branches of the this group.</returns>
			inline Threading::GaBarrier& GACALL GetBarrier() { return _barrier; }

		protected:

			/// <summary>For more details see specification of <see cref="GaFlowConnection::CheckConnectionValidity" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <returns>The method returns <c>true</c> if both steps that should be connected by the branch group belongs to the same workflow.</returns>
			bool GACALL CheckConnectionValidity(GaFlowStep* outboundStep,
				GaFlowStep* inboundStep)
			{ 
				return ( (GaBranchGroupFlow*)outboundStep->GetFlow() )->GetBranchGroup().GetWorkflow() == ( (GaBranchGroupFlow*)inboundStep->GetFlow() )->GetBranchGroup().GetWorkflow();
			}

			/// <summary><c>CheckCompatibilityUp</c> method checks whether this branch group is compatible with specified parent branch group.
			/// <param name="compatibleGroup">pointer to branch group whose compatibility should be tested with this branch group.</param>
			/// <param name="ignoreGroup">pointer to branch group that should be skiped during testing.</param>
			/// <returns>Method returns <c>true</c> if the parent branch group is compatible.</returns>
			GAL_API
			bool GACALL CheckCompatibilityUp(const GaBranchGroup* compatibleGroup,
				const GaBranchGroup* ignoreGroup) const;
			
			/// <summary><c>CheckCompatibilityDown</c> method checks whether this branch group is compatible with specified child branch group.
			/// <param name="compatibleGroup">pointer to branch group whose compatibility should be tested with this branch group.</param>
			/// <param name="ignoreGroup">pointer to branch group that should be skiped during testing.</param>
			/// <returns>Method returns <c>true</c> if the child branch group is compatible.</returns>
			GAL_API
			bool GACALL CheckCompatibilityDown(const GaBranchGroup* compatibleGroup,
				const GaBranchGroup* ignoreGroup) const;

		};

		/// <summary><c>GaBranchGroupTransitionConnection</c> class represent connection between branch groups which used to transfer control from one branch group to another.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// Only some methods are thread-safe.</summary>
		class GaBranchGroupTransitionConnection : public GaFlowConnection
		{

		public:

			/// <summary>This constructor initializes connection with default values.</summary>
			GaBranchGroupTransitionConnection() : GaFlowConnection(1) { }

			/// <summary>Connection ID of the branch group is used to identify connection type, so it cannot be changed.
			/// More details are given in specification of <see cref="GaFlowConnection::SetConnectionID" />.</summary>
			/// <exception cref="GaInvalidOperationException" />Thrown always when this method is called.</exception>
			virtual void GACALL SetConnectionID(int id) { GA_THROW( Exceptions::GaInvalidOperationException, "Cannot change connection ID for this connection type.", "Workflows" ); }

		protected:

			/// <summary>For more details see specification of <see cref="GaFlowConnection::CheckConnectionValidity" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <returns>The method returns <c>true</c> if steps belongs to flows of compatible branch groups.</returns>
			bool GACALL CheckConnectionValidity(GaFlowStep* outboundStep,
				GaFlowStep* inboundStep) 
			{ 
				return ( (GaBranchGroupFlow*)outboundStep->GetFlow() )->GetBranchGroup().CheckBranchGroupCompatibility( &( (GaBranchGroupFlow*)inboundStep->GetFlow() )->GetBranchGroup() );
			}
		};

		/// <summary><c>GaSimpleWorkStep</c> is base class for flow steps that performs actual work. Steps of this type can have only one outbound step.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// No public or private methods are thread-safe.</summary>
		class GaSimpleWorkStep : public GaBasicStep
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaFlowStep::GetNextStep" />.
			///
			/// This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the next step in the flow if it is attached. If there is no attached step this method returns <c>NULL</c>.</returns>
			virtual GaFlowStep* GACALL GetNextStep(GaBranch* branch) { return _outboundConnections.empty() ? NULL : ( *_outboundConnections.begin() )->GetInboundStep(); };

			/// <summary>More details are given in specification of <see cref="GaFlowStep::AttachNextStep" />.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>nextStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when the configuration of the connection object is incorrect.</exception>
			/// <exception cref="GaInvalidOperationException" />Thrown when there is already attached step.</exception>
			GAL_API
			virtual void GACALL AttachNextStep(GaFlowConnection* nextStep);

			/// <summary>More details are given in specification of <see cref="GaFlowStep::DetachNextStep" />.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>nextStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when the specified the connection object is not attached to outpoint of this step.</exception>
			GAL_API
			virtual void GACALL DetachNextStep(GaFlowConnection* nextStep);

			/// <summary>This method performs no actions. More details are given in specification of <see cref="GaFlowStep::ConnectionIDChanged" />.</summary>
			virtual void GACALL ConnectionIDChanged(GaFlowConnection* connection,
				int oldConnectionID) { }

		};

		/// <summary><c>GaNopStep</c> represents simple working step of the flow which does not perform any operations.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// No public or private methods are thread-safe.</summary>
		class GaNopStep : public GaSimpleWorkStep
		{

		public:

			/// <summary><c>Enter</c> method instructs branch to skip this step.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method always returns <c>false</c>.</returns>
			virtual bool GACALL Enter(GaBranch* branch) { return false; }

			/// <summary><c>operator ()</c> performs no actions.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL operator ()(GaBranch* branch) { }

		};

		/// <summary><c>GaBinaryDecision</c> is base class for step that control flow execution. This type of decision can have two possible (<c>true</c> or <c>false</c>)
		/// outgoing paths that depends on made decision. This class has decision table which maps specific decision to specific otgoing connection.
		/// The table uses connection ID to bind connection to specific decision.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// No public or private methods are thread-safe.</summary>
		class GaBinaryDecision : public GaBasicStep
		{

		protected:

			/// <summary>Decision table, contains outgoing connections for specific decision.</summary>
			GaFlowConnection* _branches[ 2 ];

		public:

			/// <summary>This constructors initializes default content of the decision table.</summary>
			GaBinaryDecision() { _branches[ 0 ] = _branches[ 1 ] = NULL; }

			/// <summary>This operator performs decision making operation and stores that decision in branch that executes this step. This operator performs no actions.
			/// More details are given in specification of <see cref="GaFlowStep::operator ()" />.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL operator ()(GaBranch* branch) { branch->SetLastDecision( Decision( branch ) ? 1 : 0 ); }

			/// <summary>This method uses stored decision in branch group as and index into decision table and returns outgoing steps that is bound to that decision as next step.
			/// More details are given in specification of <see cref="GaFlowStep::GetNextStep" />.
			///
			/// This method is not thread-safe.</summary>
			GAL_API
			virtual GaFlowStep* GACALL GetNextStep(GaBranch* branch);

			/// <summary>This method adds connection to decision table. Connection ID is used to bind connection object to specific decision.
			/// If the connection ID is <c>0</c>, connection is bound to <c>false</c> decision, other values of connection ID ar bound to <c>true</c> decision.
			/// More details are given in specification of <see cref="GaFlowStep::AttachNextStep" />.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>nextStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when the configuration of the connection object is incorrect.</exception>
			/// <exception cref="GaInvalidOperationException" />Thrown when there is already attached connection for specified decision.</exception>
			GAL_API
			virtual void GACALL AttachNextStep(GaFlowConnection* nextStep);

			/// <summary>This method removes connection object from decision table. More details are given in specification of <see cref="GaFlowStep::DetachNextStep" />.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>nextStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when the specified the connection object is not attached to outpoint of this step.</exception>
			GAL_API
			virtual void GACALL DetachNextStep(GaFlowConnection* nextStep);

			/// <summary>This method updates decision table when the ID of outbound connection is changed.
			/// More details are given in specification of <see cref="GaFlowStep::ConnectionIDChanged" />.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>nextStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when the connection object is not in the decision table.</exception>
			/// <exception cref="GaInvalidOperationException" />Thrown when new ID tries to map connection object to a decision which already has defined outgoing connection.</exception>
			GAL_API
			virtual void GACALL ConnectionIDChanged(GaFlowConnection* connection,
				int oldConnectionID);

		protected:

			/// <summary>This method should be overridden by inheriting classes and it should implement decision making.</summary>
			/// <param name="branch">pointer to branch that execute this step.</param>
			/// <returns>Method returns decision results, which can be used to control flow of execution.</returns>
			virtual bool GACALL Decision(GaBranch* branch) = 0;

		};

		/// <summary><c>GaDecision</c> is base class for step that control flow execution. This type of decision can have multiple possible outgoing paths
		/// that depends on made decision. This class has decision table which maps specific decision to specific otgoing connection.
		/// The table uses connection ID to bind connection to specific decision.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// No public or private methods are thread-safe.</summary>
		class GaDecision : public GaBasicStep
		{

		protected:

			/// <summary>Decision table, contaions outgoing connections for specific decision.</summary>
			STLEXT::hash_map<int, GaFlowConnection*> _branches;

		public:

			/// <summary>This method uses stored decision in branch group as and index into decision table and returns outgoing steps that is bound to that decision as next step.
			/// More details are given in specification of <see cref="GaFlowStep::GetNextStep" />.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL operator ()(GaBranch* branch) { branch->SetLastDecision( Decision( branch ) ); }

			/// <summary>This method uses stored decision in branch group as and index into decision table and returns outgoing steps that is bound to that decision as next step.
			/// More details are given in specification of <see cref="GaFlowStep::GetNextStep" />.
			///
			/// This method is not thread-safe.</summary>
			GAL_API
			virtual GaFlowStep* GACALL GetNextStep(GaBranch* branch);

			/// <summary>This method adds connection to decision table. Connection ID is used to bind connection object to specific decision.
			/// More details are given in specification of <see cref="GaFlowStep::AttachNextStep" />.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>nextStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when the configuration of the connection object is incorrect.</exception>
			/// <exception cref="GaInvalidOperationException" />Thrown when there is already attached connection for specified decision.</exception>
			GAL_API
			virtual void GACALL AttachNextStep(GaFlowConnection* nextStep);

			/// <summary>This method removes connection object from decision table. More details are given in specification of <see cref="GaFlowStep::DetachNextStep" />.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>nextStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when the specified the connection object is not attached to outpoint of this step.</exception>
			GAL_API
			virtual void GACALL DetachNextStep(GaFlowConnection* nextStep);

			/// <summary>This method updates decision table when the ID of outbound connection is changed.
			/// More details are given in specification of <see cref="GaFlowStep::ConnectionIDChanged" />.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>nextStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when the connection object is not in the decision table.</exception>
			/// <exception cref="GaInvalidOperationException" />Thrown when new ID tries to map connection object to a decision which already has defined outgoing connection.</exception>
			GAL_API
			virtual void GACALL ConnectionIDChanged(GaFlowConnection* connection,
				int oldConnectionID);

		protected:

			/// <summary>This method should be overridden by inheriting classes and it should implement decision making.</summary>
			/// <param name="branch">pointer to branch that execute this step.</param>
			/// <returns>Method returns decision results, which can be used to control flow of execution.</returns>
			virtual int GACALL Decision(GaBranch* branch) = 0;

		};

		/// <summary><c>GaMethodExecPassBranch</c> class is used for executing methods methods that requires pointer branch as additional parameter.</summary>
		/// <param name="OBJECT_TYPE">object whose method is executed</param>
		template<typename OBJECT_TYPE>
		class GaMethodExecPassBranch
		{

		public:

			/// <summary>Type of object whose method is executed.</summary>
			typedef OBJECT_TYPE GaObjectType;

			/// <summary>Type of method which is executed over specifed object.</summary>
			typedef void (GACALL GaObjectType::*GaMethodPtr)(Common::Workflows::GaBranch* branch);

			/// <summary><c>operator ()</c> executes specified method over provided object.</summary>
			/// <param name="object">object whose method is executed.</param>
			/// <param name="method">method which is executed over specifed object.</param>
			/// <param name="branch">branch that execute method.</param>
			inline void GACALL operator ()(GaObjectType& object,
				GaMethodPtr method,
				Common::Workflows::GaBranch* branch) { ( object.*method )( branch ); }

		};

		/// <summary><c>GaMethodExecIgnoreBranch</c> class is used for executing methods methods that does not require pointer branch as additional parameter.</summary>
		/// <param name="OBJECT_TYPE">object whose method is executed</param>
		template<typename OBJECT_TYPE>
		class GaMethodExecIgnoreBranch
		{

		public:

			/// <summary>Type of object whose method is executed.</summary>
			typedef OBJECT_TYPE GaObjectType;

			/// <summary>Type of method which is executed over specifed object.</summary>
			typedef void (GACALL GaObjectType::*GaMethodPtr)();

			/// <summary><c>operator ()</c> executes specified method over provided object.</summary>
			/// <param name="object">object whose method is executed.</param>
			/// <param name="method">method which is executed over specifed object.</param>
			/// <param name="branch">branch that execute method.</param>
			inline void GACALL operator ()(GaObjectType& object,
				GaMethodPtr method,
				Common::Workflows::GaBranch* branch)
			{
				GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
					( object.*method )();
			}

		};

		/// <summary><c>GaSimpleMethodExecStep</c> template class represents configurable workflow step that execute one method of an object without any additional parameters.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="OBJECT_TYPE">type of object on which the step is executed.</param>
		/// <param name="METHOD_EXEC">type of method execution.</param>
		template<typename OBJECT_TYPE,
			typename METHOD_EXEC = GaMethodExecPassBranch<OBJECT_TYPE> >
		class GaSimpleMethodExecStep: public Common::Workflows::GaSimpleWorkStep
		{

		public:

			/// <summary>Type of method execution.</summary>
			typedef METHOD_EXEC GaMethodExec;

		private:

			/// <summary>Method execution.</summary>
			GaMethodExec _exec;

			/// <summary>Pointer to method that is called on provided object when step is executed.</summary>
			typename GaMethodExec::GaMethodPtr _method;

			/// <summary>Object on which the step is performed.</summary>
			Common::Workflows::GaDataCache<typename GaMethodExec::GaObjectType> _object;

		public:

			/// <summary>Initializes workflow step with object and pointer to method that should be executed.</summary>
			/// <param name="method">pointer to method that is called on provided object when step is executed.</param>
			/// <param name="dataStorage">pointer to workflow that stores object used by the step.</param>
			/// <param name="objectID">ID of object in workflow storage.</param>
			GaSimpleMethodExecStep(typename GaMethodExec::GaMethodPtr method,
				Common::Workflows::GaDataStorage* dataStorage,
				int objectID) : _object(dataStorage, objectID),
				_method(method) { }

			/// <summary>Initializes workflow step with pointer to method that should be executed.</summary>
			/// <param name="method">pointer to method that is called on provided object when step is executed.</param>
			GaSimpleMethodExecStep(typename GaMethodExec::GaMethodPtr method) : _method(method) { }

			/// <summary>Initializes workflow step without object or method.</summary>
			GaSimpleMethodExecStep() : _method(NULL) { }

			/// <summary><c>operator ()</c> executes method over specified object.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="branch">pointer to workflow branch that executes this step.</param>
			virtual void GACALL operator ()(Common::Workflows::GaBranch* branch) { _exec( _object.GetData(), _method, branch ); }

			/// <summary><c>SetMethod</c> method specify which method should be executed on provided object.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="method">pointer to method that is called on provided object when step is executed.</param>
			inline void GACALL SetMethod(typename GaMethodExec::GaMethodPtr method) { _method = method; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to method that is called on provided object when step is executed.</returns>
			inline typename GaMethodExec::GaMethodPtr GACALL GetMethod() const { return _method; }

			/// <summary><c>SetObjectID</c> method sets object that should be used by the operation executed by this step.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="dataStorage">pointer to workflow that stores object used by the step.</param>
			/// <param name="objectID">ID of object in workflow storage.</param>
			inline void GACALL SetObjectID(Common::Workflows::GaDataStorage* dataStorage,
				int objectID) { _object.SetData( dataStorage, objectID ); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of the object in the wokrflow.</returns>
			inline int GACALL GetObjectID() const { return _object.GetDataID(); }

		};

		/// <summary><c>GaFilteredStep</c> class is base class for steps that supports branch filtering.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// No public or private methods are thread-safe.</summary>
		class GaFilteredStep : public GaSimpleWorkStep
		{

		protected:

			/// <summary>Branch filter used by the step.</summary>
			GaBranchFilter _branchFilter;

			/// <summary>This attribute indicates whether the step should synchronize branches on exit.</summary>
			bool _syncOnExit;

		public:

			/// <summary>Initializes step.</summary>
			GaFilteredStep() { }

			/// <summary><c>Enter</c> method verifies whether the branch can execute this step by checking branch filter of the step.
			/// More details are given in specification of <see cref="GaFlowStep::Enter" />.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Methos returns <c>true</c> if the current branch is allowed (by branch filter) to execute this step.</returns>
			GAL_API
			virtual bool GACALL Enter(GaBranch* branch);

			/// <summary><c>Exit</c> method synchronizes branches that execute this step on exit if needed. More details are given in specification of <see cref="GaFlowStep::Exit" />.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Exit(GaBranch* branch);

			/// <summary><c>BindToFlow</c> method updates branch filter size so it can store information for all branches in the group that this flow now belongs.
			/// See specification of <see cref="GaFlowSetp::BindToFlow" /> method for more details.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL BindToFlow(GaFlow* flow)
			{
				GaSimpleWorkStep::BindToFlow( flow );
				_branchFilter.SetSize( ( (GaBranchGroupFlow*)_flow )->GetBranchGroup().GetBranchCount() );
				BranchCountChanged();
			}

			/// <summary><c>UnbindFromFlow</c> method sets branch filter size to zero.
			/// See specification of <see cref="GaFlowSetp::UnbindFromFlow" /> method for more details.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL UnbindFromFlow()
			{
				_branchFilter.ClearAll();
				_branchFilter.SetSize( 0 );
			}

			/// <summary><c>FlowUpdated</c> method updates size of branch filter so it can store information for all branches in the group.
			/// See specification of <see cref="GaFlowSetp::FlowUpdated" /> method for more details.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL FlowUpdated()
			{
				_branchFilter.SetSize( ( (GaBranchGroupFlow*)_flow )->GetBranchGroup().GetBranchCount() );
				BranchCountChanged();
			}

			/// <summary><c>SetBranchFilterInfo</c> info sets branch filter inforamtion which will be used by the step to allow or disallow specific branches to execut step.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="info">branch filter information.</param>
			inline void GACALL SetBranchFilterInfo(const GaBranchFilterInfo& info)
			{
				_branchFilter.SetInfo( info );
				BranchCountChanged();
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to branch filter used by the step.</returns>
			inline const GaBranchFilter& GACALL GetBranchFilter() const { return _branchFilter; }

			/// <summary><c>SetSyncOnExit</c> method sets whether the step should synchronize branches on exit.</summary>
			/// <param name="sync">this parameter should be set to <c>true</c> if synchronization of branches is needed on exit.</param>
			inline void GACALL SetSyncOnExit(bool sync) { _syncOnExit = sync; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if the step should synchronize branches on exit.</returns>
			inline bool GACALL GetSyncOnExit() const { return _syncOnExit; }

		protected:

			/// <summary>This method is called when step detects that number of branches that can execute it has been changed.</summary>
			virtual void GACALL BranchCountChanged() = 0;

		};

		/// <summary><c>GaOperationStep1</c> template class represents configurable workflow step that execute single operation with defined parameter and configuration
		/// that operates on single data object.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="SETUP_TYPE">operation setup type.</param>
		/// <param name="DATA_TYPE">type of data on which this step operates.</param>
		template<typename SETUP_TYPE,
			typename DATA_TYPE>
		class GaOperationStep1 : public GaFilteredStep
		{

		public:

			/// <summary>Operation setup type.</summary>
			typedef SETUP_TYPE GaSetupType;

			/// <summary>Type of data on which this step operates.</summary>
			typedef DATA_TYPE GaDataType;

		protected:

			/// <summary>Setup of the operation that should be executed by the step.</summary>
			GaSetupType _setup;

			/// <summary>Input-output data used by operation execute by this step use.</summary>
			Common::Workflows::GaDataCache<GaDataType> _data;

		public:

			/// <summary>Initializes workflow step with operation that it should execute and data used by the operation.</summary>
			/// <param name="setup">setup of the operation that should be executed by the step.</param>
			/// <param name="dataStorage">pointer to workflow that stores data used by the operation.</param>
			/// <param name="dataID">ID of data in workflow storage.</param>
			GaOperationStep1(const GaSetupType& setup,
				Common::Workflows::GaDataStorage* dataStorage,
				int dataID) : _setup(setup) { SetIOData( dataStorage, dataID ); }

			/// <summary>Initializes workflow step with operation that it should execute.</summary>
			/// <param name="setup">setup of the operation that should be executed by the step.</param>
			GaOperationStep1(const GaSetupType& setup) : _setup(setup) { }

			/// <summary>Initializes workflow step with data that should be used by operation executed by this step.</summary>
			/// <param name="dataStorage">pointer to workflow that stores data used by the operation.</param>
			/// <param name="dataID">ID of data in workflow storage.</param>
			GaOperationStep1(Common::Workflows::GaDataStorage* dataStorage,
				int dataID) { SetIOData( dataStorage, dataID ); }

			/// <summary>Initializes workflow step without operation and data.</summary>
			GaOperationStep1() { }

			/// <summary>Destructor cleans data object if it was attached.</summary>
			virtual ~GaOperationStep1() { Clear(); }

			/// <summary><c>operator ()</c> executes specified operation.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="branch">pointer to workflow branch that executes this step.</param>
			virtual void GACALL operator ()(Common::Workflows::GaBranch* branch)
				{ _setup.GetOperation()( _data.GetData(), _setup.GetParameters(), _setup.GetConfiguration(), branch ); }

			/// <summary><c>SetSetup</c> sets setup of the operation that should be executed by the step.
			/// 
			/// This operator is not thread-safe.</summary>
			/// <param name="setup">operation setup.</param>
			inline void GACALL SetSetup(const GaSetupType& setup)
			{
				Clear();
				_setup = setup;

				try
				{
					Prepare();
				}
				catch( ... )
				{
					// undo changes
					Clear();

					throw;
				}
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to setup of the operation that should be executed by the step.</returns>
			inline const GaSetupType& GACALL GetSetup() const { return _setup; }

			/// <summary><c>SetIOData</c> method sets data that should be used by the operation executed by this step.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="dataStorage">pointer to workflow that stores data used by the operation.</param>
			/// <param name="dataID">ID of data in workflow storage.</param>
			inline void GACALL SetIOData(Common::Workflows::GaDataStorage* dataStorage,
				int dataID)
			{
				Clear();
				_data.SetData( dataStorage, dataID );

				try
				{
					Prepare();
				}
				catch( ... )
				{
					// undo changes
					Clear();

					throw;
				}
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of the data in workflow storage used by operation executed by this step.</returns>
			inline int GACALL GetIODataID() const { return _data.GetDataID(); }

		protected:

			/// <summary>Updates attached data object to reflect changes made in the underlying flow object.
			/// See specification of <see cref="GaFlowSetp::BindToFlow" /> method for more details.</summary>
			virtual void GACALL BranchCountChanged()
			{
				if( _setup.HasOperation() && _data.HasData() )
					_setup.GetOperation().Update( _data.GetData(), _setup.GetParameters(), _setup.GetConfiguration(), _branchFilter.GetBranchCount() );
			}

			/// <summary>Cleans changed made by the operation to attached data object.</summary>
			inline void GACALL Clear()
			{
				if( _setup.HasOperation() && _data.HasData() )
					_setup.GetOperation().Clear( _data.GetData(), _setup.GetParameters(), _setup.GetConfiguration(), _branchFilter.GetBranchCount() );
			}

			/// <summary>Prepares data object for execution of the operation.</summary>
			inline void GACALL Prepare()
			{
				if( _setup.HasOperation() && _data.HasData() )
					_setup.GetOperation().Prepare( _data.GetData(), _setup.GetParameters(), _setup.GetConfiguration(), _branchFilter.GetBranchCount() );
			}

		};

		/// <summary><c>GaOperationStep2</c> template class represents configurable workflow step that execute single operation with defined parameter and configuration
		/// using separate objects for input and output data.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="SETUP_TYPE">operation setup type.</param>
		/// <param name="IN_DATA_TYPE">type of input data.</param>
		/// <param name="OUT_DATA_TYPE">type of output data.</param>
		template<typename SETUP_TYPE, typename IN_DATA_TYPE, typename OUT_DATA_TYPE>
		class GaOperationStep2 : public GaFilteredStep
		{

		public:

			/// <summary>Operation setup type.</summary>
			typedef SETUP_TYPE GaSetupType;

			/// <summary>Type of data input on which this step operates.</summary>
			typedef IN_DATA_TYPE GaInDataType;

			/// <summary>Type of data output on which this step operates.</summary>
			typedef OUT_DATA_TYPE GaOutDataType;

		protected:

			/// <summary>Setup of the operation that should be executed by the step.</summary>
			GaSetupType _setup;

			/// <summary>Input data used by operation execute by this step use.</summary>
			Common::Workflows::GaDataCache<GaInDataType> _inData;

			/// <summary>Output data which operation of this step use.</summary>
			Common::Workflows::GaDataCache<GaOutDataType> _outData;

		public:

			/// <summary>Initializes workflow step with operation that it should execute and data used by the operation.</summary>
			/// <param name="setup">setup of the operation that should be executed by the step.</param>
			/// <param name="inDataStorage">pointer to workflow that stores input data used by the operation.</param>
			/// <param name="inDataID">ID of input data in workflow storage.</param>
			/// <param name="outDataStorage">pointer to workflow that stores output data used by the operation.</param>
			/// <param name="outDataID">ID of output data in workflow storage.</param>
			GaOperationStep2(const GaSetupType& setup,
				Common::Workflows::GaDataStorage* inDataStorage,
				int inDataID,
				Common::Workflows::GaDataStorage* outDataStorage,
				int outDataID) : _setup(setup)
			{
				SetInputData( inDataStorage, inDataID );
				SetOutputData( outDataStorage, outDataID );
			}

			/// <summary>Initializes workflow step with operation that it should execute.</summary>
			/// <param name="setup">setup of the operation that should be executed by the step.</param>
			GaOperationStep2(const GaSetupType& setup) : _setup(setup) { }

			/// <summary>Initializes workflow step with data that should be used by operation executed by this step.</summary>
			/// <param name="inDataStorage">pointer to workflow that stores input data used by the operation.</param>
			/// <param name="inDataID">ID of input data in workflow storage.</param>
			/// <param name="outDataStorage">pointer to workflow that stores output data used by the operation.</param>
			/// <param name="outDataID">ID of output data in workflow storage.</param>
			GaOperationStep2(Common::Workflows::GaDataStorage* inDataStorage,
				int inDataID,
				Common::Workflows::GaDataStorage* outDataStorage,
				int outDataID)
			{
				SetInputData( inDataStorage, inDataID );
				SetOutputData( outDataStorage, outDataID );
			}

			/// <summary>Initializes workflow step without operation and data.</summary>
			GaOperationStep2() { }

			/// <summary>Destructor cleans data objects if they were attached.</summary>
			virtual ~GaOperationStep2() { Clear(); }

			/// <summary><c>operator ()</c> executes specified operation.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="branch">pointer to workflow branch that executes this step.</param>
			virtual void GACALL operator ()(Common::Workflows::GaBranch* branch)
				{ _setup.GetOperation()( _inData.GetData(), _outData.GetData(), _setup.GetParameters(), _setup.GetConfiguration(), branch ); }

			/// <summary><c>SetSetup</c> sets setup of the operation that should be executed by the step.
			/// 
			/// This operator is not thread-safe.</summary>
			/// <param name="setup">operation setup.</param>
			inline void GACALL SetSetup(const GaSetupType& setup)
			{
				Clear();
				_setup = setup;

				try
				{
					Prepare();
				}
				catch( ... )
				{
					// undo changes
					Clear();

					throw;
				}
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to setup of the operation that should be executed by the step.</returns>
			inline const GaSetupType& GACALL GetSetup() const { return _setup; }

			/// <summary><c>SetInputData</c> method sets input data that should be used by the operation executed by this step.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="dataStorage">pointer to workflow that stores data used by the operation.</param>
			/// <param name="dataID">ID of data in workflow storage.</param>
			inline void GACALL SetInputData(Common::Workflows::GaDataStorage* dataStorage,
				int dataID)
			{
				Clear();
				_inData.SetData( dataStorage, dataID );

				try
				{
					Prepare();
				}
				catch( ... )
				{
					// undo changes
					Clear();

					throw;
				}
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of input data in workflow storage used by operation executed by this step.</returns>
			inline int GACALL GetInputDataID() const { return _inData.GetDataID(); }

			/// <summary><c>SetInputData</c> method sets output data that should be used by the operation executed by this step.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="dataStorage">pointer to workflow that stores data used by the operation.</param>
			/// <param name="dataID">ID of data in workflow storage.</param>
			inline void GACALL SetOutputData(Common::Workflows::GaDataStorage* dataStorage,
				int dataID)
			{
				Clear();
				_outData.SetData( dataStorage, dataID );

				try
				{
					Prepare();
				}
				catch( ... )
				{
					// undo changes
					Clear();

					throw;
				}
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of output data in workflow storage used by operation executed by this step.</returns>
			inline int GACALL GetOutputDataID() const { return _outData.GetDataID(); }

		protected:

			/// <summary>Updates attached data objects to reflect changes made in the underlying flow object.
			/// See specification of <see cref="GaFlowSetp::BindToFlow" /> method for more details.</summary>
			virtual void GACALL BranchCountChanged()
			{
				if( _setup.HasOperation() && _inData.HasData() && _outData.HasData() )
					_setup.GetOperation().Update( _inData.GetData(), _outData.GetData(), _setup.GetParameters(), _setup.GetConfiguration(), _branchFilter.GetBranchCount() );
			}

			/// <summary>Cleans changed made by the operation to attached data objects.</summary>
			inline void GACALL Clear()
			{
				if( _setup.HasOperation() && _inData.HasData() && _outData.HasData() )
					_setup.GetOperation().Clear( _inData.GetData(), _outData.GetData(), _setup.GetParameters(), _setup.GetConfiguration(), _branchFilter.GetBranchCount() );
			}

			/// <summary>Prepares data objects for execution of the operation.</summary>
			inline void GACALL Prepare()
			{
				if( _setup.HasOperation() && _inData.HasData() && _outData.HasData() )
					_setup.GetOperation().Prepare( _inData.GetData(), _outData.GetData(), _setup.GetParameters(), _setup.GetConfiguration(), _branchFilter.GetBranchCount() );
			}

		};
		
		/// <summary><c>GaBranchGroupTransition</c> class represents flow step which transfers control from parent branch group to attachet outgoing brach group.
		/// Parent and outgoing branch groups must be compatible branch groups. This step uses <see cref="GaBranchGroupTransitionConnection" /> to connect 
		/// parent branch group to single step (which is executed first whent the control is transfered) of the outgoing branch group flow.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// Only some methods are thread-safe.</summary>
		class GaBranchGroupTransition : public GaAbstractBarrier
		{

		public:

			/// <summary><c>operator ()</c> transfers control from parent branch group to outgoing branch group.
			/// More details are given in specification of <see cref="GaAbstractBarrier::operator ()" />.
			///
			/// This operator is not thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaBranch* branch);

			/// <summary>When the first branch executes this method, it sets this step as a step that should be perofrmed at the end of the branch.
			/// When the branches execute this step as their last step, only one of these branches start waiting process and all other finishes their execution.
			/// More details are given in specification of <see cref="GaAbstractBarrier::Enter" />.
			///
			/// This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if the last branch of the group has executed this step as last step.</returns>
			GAL_API
			virtual bool GACALL Enter(GaBranch* branch);

			/// <summary>This method attaches outgoing branch group. More details are given in specification of <see cref="GaFlowStep::AttachNextStep" />.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>nextStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when the configuration of the connection object is incorrect.</exception>
			/// <exception cref="GaInvalidOperationException" />Thrown when there is already attached connection for specified decision.</exception>
			GAL_API
			virtual void GACALL AttachNextStep(GaFlowConnection* nextStep);

			/// <summary>This method detaches outgoing branch group. More details are given in specification of <see cref="GaFlowStep::DetachNextStep" />.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>nextStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when the specified the connection object is not attached to outpoint of this step.</exception>
			GAL_API
			virtual void GACALL DetachNextStep(GaFlowConnection* nextStep);

			/// <summary>This method performs no actions. More details are given in specification of <see cref="GaFlowStep::ConnectionIDChanged" />.</summary>
			virtual void GACALL ConnectionIDChanged(GaFlowConnection* connection,
				int oldConnectionID) { }

		protected:

			/// <summary>For more details see documentation of <see cref="GaAbstractBarrier::NumberOfInboundBranchGroups" /> method</summary>
			/// <returns>Method always returns 1.</returns>
			virtual int NumberOfInboundBranchGroups() const { return 1; }

		};

		/// <summary><c>GaWorkflowBarrier</c> class is a barrier that can have multiple inbound and outbound branch groups. The barrier waits for
		/// all inbound branch groups before it starts all outgoing branch groups.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// Only some methods are thread-safe.</summary>
		class GaWorkflowBarrier : public GaAbstractBarrier
		{

		public:

			/// <summary><c>operator ()</c> starts all outgoing branches and restarts branch group counter.
			/// More details are given in specification of <see cref="GaAbstractBarrier::operator ()" />.
			///
			/// This operator is not thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaBranch* branch);

			/// <summary>This method attaches outgoing branch group. More details are given in specification of <see cref="GaFlowStep::AttachNextStep" />.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>nextStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when the configuration of the connection object is incorrect or when there is already attached connection
			/// for specified decision.</exception>
			GAL_API
			virtual void GACALL AttachNextStep(GaFlowConnection* nextStep);

			/// <summary>This method detaches outgoing branch group. More details are given in specification of <see cref="GaFlowStep::DetachNextStep" />.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>nextStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when the specified the connection object is not attached to outpoint of this step.</exception>
			GAL_API
			virtual void GACALL DetachNextStep(GaFlowConnection* nextStep);

			/// <summary>This method performs no actions. More details are given in specification of <see cref="GaFlowStep::ConnectionIDChanged" />.</summary>
			virtual void GACALL ConnectionIDChanged(GaFlowConnection* connection,
				int oldConnectionID) { }

		protected:

			/// <summary>For more details see documentation of <see cref="GaAbstractBarrier::NumberOfInboundBranchGroups" /> method</summary>
			virtual int NumberOfInboundBranchGroups() const { return (int)_inboundConnections.size(); }

		};

		/// <summary><c>GaStartStep</c> represents the first step of the workflow.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// Only some methods are thread-safe.</summary>
		class GaStartStep : public GaWorkflowBarrier
		{

		public:

			/// <summary>This method always throws exception beacues the first step of the workflow cannot have inbound connections.
			/// More details are given in specification of <see cref="GaWorkflowBarrier::AttachPreviousStep" />.</summary>
			/// <exception cref="GaInvalidOperationException" />Thrown always when this method is called.</exception>
			virtual void GACALL AttachPreviousStep(GaFlowConnection* previousStep)
				{ GA_THROW( Exceptions::GaInvalidOperationException, "Start step cannot have inbound connection.", "Workflows" ); }

			/// <summary>This method always throws exception beacues the first step of the workflow cannot have inbound connections.
			/// More details are given in specification of <see cref="GaWorkflowBarrier::DetachPreviousStep" />.</summary>
			/// <exception cref="GaInvalidOperationException" />Thrown always when this method is called.</exception>
			virtual void GACALL DetachPreviousStep(GaFlowConnection* previousStep)
				{ GA_THROW( Exceptions::GaInvalidOperationException, "Start step cannot have inbound connection.", "Workflows" ); }

		protected:

			/// <summary>For more details see documentation of <see cref="GaAbstractBarrier::NumberOfInboundBranchGroups" /> method</summary>
			/// <returns>This method always returns 1.</returns>
			virtual int NumberOfInboundBranchGroups() const { return 1; }

		};

		/// <summary><c>GaFinishStep</c> represents the last step of the workflow.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// Only some methods are thread-safe.</summary>
		class GaFinishStep : public GaWorkflowBarrier
		{

		public:

			/// <summary>This constructor initializes barrier..</summary>
			GaFinishStep() { }

			/// <summary>This method always throws exception beacues the last step of the workflow cannot have outbound connections.
			/// More details are given in specification of <see cref="GaWorkflowBarrier::AttachPreviousStep" />.</summary>
			/// <exception cref="GaInvalidOperationException" />Thrown always when this method is called.</exception>
			virtual void GACALL AttachNextStep(GaFlowConnection* nextStep)
				{ GA_THROW( Exceptions::GaInvalidOperationException, "Finish step cannot have outbound connection.", "Workflows" ); }

			/// <summary>This method always throws exception beacues the last step of the workflow cannot have outbound connections.
			/// More details are given in specification of <see cref="GaWorkflowBarrier::DetachNextStep" />.</summary>
			/// <exception cref="GaInvalidOperationException" />Thrown always when this method is called.</exception>
			virtual void GACALL DetachNextStep(GaFlowConnection* nextStep)
				{ GA_THROW( Exceptions::GaInvalidOperationException, "Finish step cannot have outbound connection.", "Workflows" ); }

		};

		/// <summary>This enumeration defines states of an workflow.</summary>
		enum GaWorkflowState
		{
			/// <summary>execution of the workflow is stopped.</summary>
			GAWS_STOPPED = 0x1, 

			/// <summary>execution of the workflow is temporary paused.</summary>
			GAWS_PAUSED = 0x2, 

			/// <summary>workfow is running.</summary>
			GAWS_RUNNING = 0x4, 

			/* --- combined states --- */

			/// <summary>used for testing of state of an workflow it combines all states except running state.</summary>
			GAWS_NOT_RUNNING = GAWS_STOPPED | GAWS_PAUSED, 

		};

		/// <summary><c>GaWorkflow</c> class represents workflow. Workflow is set of barriers connected with branch groups and branch group tranistion connections. 
		/// Each workflow has at least two steps (start step and finish step) connected by one branch group. Users can control state of the execution directly
		/// which is not possible with oher types of flows.
		/// 
		/// This class has built-in synchronizator so it is allowed to use <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros with instances of this class.
		/// Only some methods are thread-safe.</summary>
		class GaWorkflow : public GaFlow
		{

			GA_SYNC_CLASS

		private:
			
			/// <summary>Pointer to data storage for global data.</summary>
			GaDataStorage* _globalData;

			/// <summary>Data storage for workflow data.</summary>
			GaDataStorage _workflowData;

			/// <summary>Set of workflow's barriers.</summary>
			std::set<GaAbstractBarrier*> _steps;

			/// <summary>Set of workflow's branch groups.</summary>
			std::set<GaFlowConnection*> _branchGroups;

			/// <summary>Set of branch group transition connections.</summary>
			std::set<GaFlowConnection*> _branchGroupTransitions;

			/// <summary>The first step which is executed by workflow. For more details see <see cref="GaStartStep" />.</summary>
			GaStartStep* _firstStep;

			/// <summary>The last step which is executed by workflow. For more details see <see cref="GaFinishStep" />.</summary>
			GaFinishStep* _lastStep;

			/// <summary>Current state of workflow's execution.</summary>
			GaWorkflowState _state;

			/// <summary>Pointer to event obiject which is used to signal successfull state change of the workflow
			/// (when all branch groups and barriers are started, stoped, resumed or paused.</summary>
			Memory::GaAutoPtr<Threading::GaEvent> _stateChangeEvent;

			/// <summary>Pointer to event object which is used to pause or resume active branch groups and barriers.</summary>
			Memory::GaAutoPtr<Threading::GaEvent> _pauseEvent;

			/// <summary>Pointer to event object which is used to signla threads that waits for the work flow to finish executeion.</summary>
			Memory::GaAutoPtr<Threading::GaEvent> _endEvent;

			/// <summary>Defines available counters.</summary>
			enum GaCounterTypes
			{

				/// <summary>Counts currently active (running or paused) branches.</summary>
				GACT_ACTIVE_BRANCHES = 0,

				/// <summary>Counts currently active but paused branches.</summary>
				GACT_PAUSED_BRANCHES,

				/// <summary>Number of defined counters.</summary>
				GACT_NUMBER_OF_COUNTERS

			};

			/// <summary>Array of counters for counting currently active and paused elements of the workflow.</summary>
			Threading::GaAtomic<int> _counters[ GACT_NUMBER_OF_COUNTERS ];

		public:

			/// <summary>This constructor initializes workflow with its first and last step and it connects them with one branch group.</summary>
			GAL_API
			GaWorkflow(GaDataStorage* globalData);

			/// <summary>The destructor destorys workflow bariers, branch group and branch group transition connection which are members of the workflow.</summary>
			GAL_API
			virtual ~GaWorkflow();

			/// <summary>For more details see specification of <see cref="GaFlow::AddStep" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>step</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when the step is connected to some other step.</exception>
			GAL_API
			virtual void GACALL AddStep(GaFlowStep* step);

			/// <summary>The method does not destory removed step object. For more details see specification of <see cref="GaFlow::RemoveStep" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>step</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when the step does not belong to this flow.</exception>
			/// <exception cref="GaInvalidOperationException" />Thrown when user tries to remove start or finish step of the workflow,
			/// or if user when trying to remove connected step without disconnecting it or when the method is unable to disconnect the step.</exception>
			GAL_API
			virtual void GACALL RemoveStep(GaFlowStep* step,
				bool disconnect,
				bool destroy);

			/// <summary>This can make branch group or banch group transition between specified steps, dependion on the value of connection ID.
			/// For more details see specification of <see cref="GaFlow::ConnectSteps" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="connectionID">if this parameter is set to <c>0</c> the method makes branch group between the steps, all other values instructs
			/// the method to make branch group tranistion connection.</param>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>outboundStep</c> or <c>inboundStep</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown when the steps blong to another flow flow.</exception>
			/// <exception cref="GaInvalidOperationException" />Thrown when adding specfied branch group breaks validity of branch group tranistion connections.</exception>
			GAL_API
			virtual GaFlowConnection* GACALL ConnectSteps(GaFlowStep* outboundStep,
				GaFlowStep* inboundStep,
				int connectionID);

			/// <summary>The method does not destory removed connection object. For more details see specification of <see cref="GaFlow::RemoveConnection" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown when <c>connection</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentException" />Thrown if provided connection object does not belongs to this flow .</exception>
			/// <exception cref="GaInvalidOperationException" />Thrown when removing of the branch group breaks validity of branch group tranistion connections.</exception>
			GAL_API
			virtual void GACALL RemoveConnection(GaFlowConnection* connection,
				bool destory);

			/// <summary><c>Start</c> method starts execution of the workflow step.The method suspend calling thread until the workflow is started successfully.
			///
			/// This method is thread-safe.</summary>
			/// <exception cref="GaInvalidOperationException" />Thrown when the workflow is in running or paused state.</exception>
			GAL_API
			void GACALL Start();

			/// <summary><c>Resume</c> method resumes paused execution of the workflow. The method suspend calling thread until the workflow is resumed successfully.
			///
			/// This method is thread-safe.</summary>
			/// <exception cref="GaInvalidOperationException" />Thrown when the workflow is not in paused state.</exception>
			GAL_API
			void GACALL Resume();

			/// <summary><c>Pause</c> method pauses execution of the workflow. The method suspend calling thread until the workflow is paused successfully.
			///
			/// This method is thread-safe.</summary>
			/// <exception cref="GaInvalidOperationException" />Thrown when the workflow is in running state.</exception>
			GAL_API
			void GACALL Pause();

			/// <summary><c>Stop</c> method stops execution of the workflow. The method suspend calling thread until the workflow is stopped successfully.
			/// If the workflow is already stopped this method has no effect.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			void GACALL Stop();

			/// <summary><c>Wait</c> method suspends calling thread until the workflow finishes its execution.
			///
			/// This method is thread-safe.</summary>
			inline void GACALL Wait() { _endEvent->Wait(); }

			/// <summary><c>BranchStart</c> method increments number of active branches.
			///
			/// This method is thread-safe.</summary>
			inline void GACALL BranchStart() { ++_counters[ GACT_ACTIVE_BRANCHES ]; }

			/// <summary><c>BranchEnd</c> method decrements number of active branches and if there is no more active branches this method set stopped state
			/// and signals successful state change and wakes threads that wait for the workflow that execution has finished.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			void GACALL BranchEnd();

			/// <summary><c>CheckWorkflowState</c> method checks current state of the workflow and returns <c>true</c> if the calling branch or barrier
			/// should continue execution or <c>false</c> if the execution should be stopped. If the workflow is paused this method suspends calling branch or barrier
			/// until user resume or stop execution. When this method pauses all running branches and barriers it signals successaful state change.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if the branch or barrier should continue execution normally or <c>false</c> if the execution should be stopped.</returns>
			GAL_API
			bool GACALL CheckWorkflowState();

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns pointer to data storage for global data.</returns>
			inline GaDataStorage* GACALL GetGlobalData() { return _globalData; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns pointer to data storage for global data.</returns>
			inline const GaDataStorage* GACALL GetGlobalData() const { return _globalData; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns pointer to data storage for workflow data.</returns>
			inline GaDataStorage* GACALL GetWorkflowData() { return &_workflowData; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns pointer to data storage for workflow data.</returns>
			inline const GaDataStorage* GACALL GetWorkflowData() const { return &_workflowData; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns pointer to the first step of the workflow.</returns>
			inline GaStartStep* GACALL GetFirstStep() { return _firstStep; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns pointer to the first step of the workflow.</returns>
			inline const GaStartStep* GACALL GetFirstStep() const { return _firstStep; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns pointer to the last step of the workflow.</returns>
			inline GaFinishStep* GACALL GetLastStep() { return _lastStep; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns pointer to the last step of the workflow.</returns>
			inline const GaFinishStep* GACALL GetLastStep() const { return _lastStep; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns current state of the workflow's execution.</returns>
			inline GaWorkflowState GACALL GetState() const { return _state; }

		protected:

			/// <summary><c>CheckBranchGroupTransitionConnections</c> method check all branch group tranistion connections.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="ignoreConnection">pointer to branch group that should be skiped during testing.</param>
			/// <returns>Method returns <c>true</c> if all branch group tranistion connections are valid.</returns>
			GAL_API
			bool GACALL CheckBranchGroupTransitionConnections(const GaBranchGroup* ignoreConnection = NULL);

		};

		/// <summary><c>GaParallelExec</c> is base class for parallel execution of operations.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but because objects of this class are immutable, all methods are thread-safe.</summary>
		class GaParallelExec
		{

		protected:

			/// <summary>Reference to branch that executes operation.</summary>
			GaBranch& _branch;

		public:

			/// <summary>Initializes parallel operation.</summary>
			/// <param name="branch">reference to branch that executes operation.</param>
			GaParallelExec(GaBranch& branch) : _branch(branch) { }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns reference to branch that executes operation.</returns>
			inline GaBranch& GetBranch() { return _branch; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns reference to branch that executes operation.</returns>
			inline const GaBranch& GetBranch() const { return _branch; }

		protected:

			/// <summary><c>Sync</c> method synchronize all branches at execute operation.</summary>
			inline void GACALL Sync() { _branch.GetBarrier().Enter( true, _branch.GetBarrierCount() ); }

		};

		/// <summary><c>GaDefaultItemProvider</c> template represents default work provider of items from storage object used by parallel operations.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but because objects are stateless, all methods are thread-safe.</summary>
		/// <param name="STORAGE">type of the storage object.</param>
		/// <param name="ITEM">type of items in the storage.</param>
		template<typename STORAGE,
			typename ITEM>
		class GaDefaultItemProvider
		{

		public:

			/// <summary><c>operator ()</c> operator retrieves item at specified index from the storage object.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="storage">storage object from which item should be extracted.</param>
			/// <param name="index">index of queried item.</param>
			/// <returns>Method returns reference to item at specified index.</returns>
			inline ITEM& GACALL operator ()(STORAGE& storage,
				int index) const { return storage[ index ]; }

			/// <summary><c>GetCount</c> method returns number of itmes in the storage object.
			///
			/// This method is thread-safe.</summary>
			/// <param name="storage">storage object whose size is queried.</param>
			inline int GACALL GetCount(const STORAGE& storage) const { return storage.GetCount(); }

			/// <summary>This method is thread-safe.<summary>
			/// <returns>Method returns index of the first element that should be processed.</returns>
			inline int GACALL GetStart() const { return 0; }

		};

		/// <summary><c>GaLimitedItemProvider1</c> template represents work provider of items from storage object used by parallel operations
		/// that allows user to control start position of the operation.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but because objects are stateless, all methods are thread-safe.</summary>
		/// <param name="STORAGE">type of the storage object.</param>
		/// <param name="ITEM">type of items in the storage.</param>
		template<typename STORAGE,
			typename ITEM>
		class GaLimitedItemProvider1
		{

		private:

			/// <summary>Index of the first element that should be processed.</summary>
			int _start;

		public:

			/// <summary>Initializes work provider with limits.</summary>
			/// <param name="start">index of the first element that should be processed.</param>
			GaLimitedItemProvider1(int start) : _start(start) { }

			/// <summary><c>operator ()</c> operator retrieves item at specified index from the storage object.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="storage">storage object from which item should be extracted.</param>
			/// <param name="index">index of queried item.</param>
			/// <returns>Method returns reference to item at specified index.</returns>
			inline ITEM& GACALL operator ()(STORAGE& storage,
				int index) const { return storage[ index ]; }

			/// <summary><c>GetCount</c> method returns number of itmes that should be processed.
			///
			/// This method is thread-safe.</summary>
			/// <param name="storage">storage object whose size is queried.</param>
			inline int GACALL GetCount(const STORAGE& storage) const { return storage.GetCount() - _start; }

			/// <summary>This method is thread-safe.<summary>
			/// <returns>Method returns index of the first element that should be processed.</returns>
			inline int GACALL GetStart() const { return _start; }

		};

		/// <summary><c>GaLimitedItemProvider2</c> template represents work provider of items from storage object used by parallel operations
		/// that allows user to control start and end position of the operation.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but because objects are stateless, all methods are thread-safe.</summary>
		/// <param name="STORAGE">type of the storage object.</param>
		/// <param name="ITEM">type of items in the storage.</param>
		template<typename STORAGE,
			typename ITEM>
		class GaLimitedItemProvider2
		{

		private:

			/// <summary>Index of the first element that should be processed.</summary>
			int _start;

			/// <summary>Index of the last element that should be processed.</summary>
			int _end;

		public:

			/// <summary>Initializes work provider with limits.</summary>
			/// <param name="start">index of the first element that should be processed.</param>
			/// <param name="end">index of the last element that should be processed.</param>
			GaLimitedItemProvider2(int start,
				int end) : _start(start),
				_end(end) { }

			/// <summary><c>operator ()</c> operator retrieves item at specified index from the storage object.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="storage">storage object from which item should be extracted.</param>
			/// <param name="index">index of queried item.</param>
			/// <returns>Method returns reference to item at specified index.</returns>
			inline ITEM& GACALL operator ()(STORAGE& storage,
				int index) const { return storage[ index ]; }

			/// <summary><c>GetCount</c> method returns number of itmes that should be processed.
			///
			/// This method is thread-safe.</summary>
			/// <param name="storage">storage object whose size is queried.</param>
			inline int GACALL GetCount(const STORAGE& storage) const { return _end - _start + 1; }

			/// <summary>This method is thread-safe.<summary>
			/// <returns>Method returns index of the first element that should be processed.</returns>
			inline int GACALL GetStart() const { return _start; }

		};

		/// <summary><c>GaParallelExecT</c> template is base class for parallel execution of operations over a storage object.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but because objects of this class are immutable, all methods are thread-safe.</summary>
		/// <param name="STORAGE">type of storage object.</param>
		/// <param name="ITEM">type of item in the storage.</param>
		/// <param name="PROVIDER">type of object used for extracting items from the storage.</param>
		template<typename STORAGE,
			typename ITEM,
			typename PROVIDER>
		class GaParallelExecT : public GaParallelExec
		{

		public:

			/// <summary>Type of storage object.</summary>
			typedef STORAGE GaStorage;

			/// <summary>Type of item in the storage.</summary>
			typedef ITEM GaItem;

			/// <summary>Type of object used for extracting items from the storage.</summary>
			typedef PROVIDER GaProvider;

		protected:

			/// <summary>Storage object on which the operations are performed.</summary>
			GaStorage& _storage;

			/// <summary>Object that is used for retrieving items from storage.</summary>
			GaProvider _provider;

			/// <summary>Initializes parallel operation with storage object and accessor.</summary>
			/// <param name="branch">reference to branch that executes operation.</param>
			/// <param name="storage">storage object on which the operations are performed.</param>
			/// <param name="provider">object that is used for retrieving items from storage.</param>
			GaParallelExecT(GaBranch& branch,
				GaStorage& storage,
				const GaProvider& provider) : GaParallelExec(branch), 
				_storage(storage),
				_provider(provider) { }

			/// <summary>This method is thread-safe</summary>
			/// <returns>Method returns reference to storage object on which the operations are performed.</returns>
			inline GaStorage& GetStorage() { return _storage; } 

			/// <summary>This method is thread-safe</summary>
			/// <returns>Method returns reference to storage object on which the operations are performed.</returns>
			inline const GaStorage& GetStorage() const { return _storage; } 

			/// <summary>This method is thread-safe</summary>
			/// <returns>Method returns reference to object that is used for retrieving items from storage.</returns>
			inline GaProvider& GetProvider() { return _provider; }

			/// <summary>This method is thread-safe</summary>
			/// <returns>Method returns reference to object that is used for retrieving items from storage.</returns>
			inline const GaProvider& GetProvider() const { return _provider; }

		};

		/// <summary><c>GaParallelExec1</c> template class provides framework for parallel execution of operations over items in specified storage.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but still some public methods are thread-safe.</summary>
		/// <param name="STORAGE">type of storage object.</param>
		/// <param name="ITEM">type of item in the storage.</param>
		/// <param name="PROVIDER">type of object used for extracting items from the storage.</param>
		template<typename STORAGE,
			typename ITEM,
			typename PROVIDER = GaDefaultItemProvider<STORAGE, ITEM> >
		class GaParallelExec1 : public GaParallelExecT<STORAGE, ITEM, PROVIDER>
		{

		private:

			/// <summary>Index of the first item that branch should process.</summary>
			int _branchStart;

			/// <summary>Number of items that branch should process.</summary>
			int _branchCount;

		public:

			/// <summary>Initializes parallel operation with storage object and accessor.</summary>
			/// <param name="branch">reference to branch that executes operation.</param>
			/// <param name="storage">storage object on which the operations are performed.</param>
			/// <param name="provider">object that is used for retrieving items from storage.</param>
			GaParallelExec1(GaBranch& branch,
				GaStorage& storage,
				const GaProvider& provider = GaProvider()) : GaParallelExecT(branch, storage, provider) { Update(); }

			/// <summary><c>Execute</c> method executes specified operation over each item in the storage.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="OPERATION">type of the operation performed.</param>
			/// <param name="operation">operation that is performed.</param>
			/// <param name="syncOnEnd">if this parameter is set to <c>true</c> branch will wait for other branches in the same group 
			/// to finish execution of their portion of workload.</param>
			template<typename OPERATION>
			void GACALL Execute(OPERATION& operation,
				bool syncOnEnd)
			{
				// perform operation on items in the strage
				for( int i = _branchStart + _branchCount - 1; i >= _branchStart; i-- )
					operation( _provider( _storage, i ), i );

				// synchronize branches if needed
				if( syncOnEnd )
					Sync();
			}

			/// <summary><c>Update</c> method calculates and updates workload that branch should handle.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL Update()
			{
				_branch.SplitWork( _provider.GetCount( _storage ), _branchCount, _branchStart );
				_branchStart += _provider.GetStart();
			}

		};

		/// <summary><c>GaParallelExec2</c> template class provides framework for parallel execution of operations over pair of items in specified storage.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but still some public methods are thread-safe.</summary>
		/// <param name="STORAGE">type of storage object.</param>
		/// <param name="ITEM">type of item in the storage.</param>
		/// <param name="PROVIDER">type of object used for extracting items from the storage.</param>
		template<typename STORAGE,
			typename ITEM,
			typename PROVIDER = GaDefaultItemProvider<STORAGE, ITEM> >
		class GaParallelExec2 : public GaParallelExecT<STORAGE, ITEM, PROVIDER>
		{

		private:

			/// <summary>Number of items in the storage.</summary>
			int _storageSize;

			/// <summary>Indices of items that represent the first pair that should be processed by branch</summary>
			int _branchStart[ 2 ];

			/// <summary>Number of pairs that should be processed by the branch.</summary>
			int _branchCount;

		public:

			/// <summary>Initializes parallel operation with storage object and accessor.</summary>
			/// <param name="branch">reference to branch that executes operation.</param>
			/// <param name="storage">storage object on which the operations are performed.</param>
			/// <param name="provider">object that is used for retrieving items from storage.</param>
			GaParallelExec2(GaBranch& branch,
				GaStorage& storage,
				const GaProvider& provider = GaProvider()) : GaParallelExecT(branch, storage, provider) { Update(); }

			/// <summary><c>Execute</c> method executes operations over all items and each pair of items in the storage.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="OPERATION1">type of operation that should be performed only on the first items of pairs.</param>
			/// <param name="OPERATION2">type of operation that should be performed pairs of items.</param>
			/// <param name="operation1">operation that should be performed on the first items on the pairs.</param>
			/// <param name="operation2">operation that should be performed on pairs.</param>
			/// <param name="syncOnEnd">if this parameter is set to <c>true</c> branch will wait for other branches in the same group 
			/// to finish execution of their portion of workload.</param>
			template<typename OPERATION1,
				typename OPERATION2>
			void GACALL Execute(OPERATION1& operation1,
				OPERATION2& operation2,
				bool syncOnEnd)
			{
				// perform operation on pair of items in the strage
				int i = _branchStart[ 0 ];
				for( int j = _branchStart[ 1 ], count = _branchCount; count; j = ( ++i ) + 1 )
				{
					GaItem& item1 = _provider( _storage, i );

					// perform operation on the first item of the pair
					if( j == i + 1 )
						operation1( item1, i );

					// perform operation on the pair of items
					for( ; j < _storageSize && count; j++, count-- )
						operation2( item1, _provider( _storage, j ), i, j );
				}

				// perform operation on the last item
				if( i == _storageSize - 1 )
					operation1( _provider( _storage, i ), i );

				// synchronize branches if needed
				if( syncOnEnd )
					Sync();
			}

			/// <summary><c>Execute</c> method executes same operation over all items and each pair of items in the storage.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="OPERATION">type of operation that should be performed.</param>
			/// <param name="operation">operation that should be performed on the first items of pairs and all pairs</param>
			/// <param name="syncOnEnd">if this parameter is set to <c>true</c> branch will wait for other branches in the same group 
			/// to finish execution of their portion of workload.</param>
			template<typename OPERATION>
			inline void GACALL Execute(OPERATION& operation,
				bool syncOnEnd) { Execute( operation, operation, syncOnEnd ); }

			/// <summary><c>Update</c> method calculates and updates workload that branch should handle.
			///
			/// This method is not thread-safe.</summary>
			void GACALL Update()
			{
				// calculate number of items that branch should process
				_storageSize = _provider.GetCount( _storage );
				int start, size = _storageSize - 1;
				_branch.SplitWork( ( size * ( size + 1 ) ) / 2, _branchCount, start );

				if( _branchCount > 0 )
				{
					// calculate start position (indices) for branch
					start++;
					for( _branchStart[ 0 ] = size; start > size; size-- )
						start -= size;
					_branchStart[ 0 ] = _branchStart[ 0 ] - size + _provider.GetStart();
					_branchStart[ 1 ] = _branchStart[ 0 ] + start;

					_storageSize += _provider.GetStart();
				}
				// process just one item
				else if( _storageSize > 0 && _branch.GetFilteredID() == 0 )
				{
					_branchStart[ 0 ] = _branchStart[ 1 ] = 0;
					_storageSize += _provider.GetStart();
				}
				else
					_storageSize = _branchStart[ 0 ] = _branchStart[ 1 ] = -1;
			}

		};

	} // Workflows
} // Common

#endif // __GA_WORKFLOWS_H__

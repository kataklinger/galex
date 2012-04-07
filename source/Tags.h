
/*! \file Tags.h
	\brief This file contains declaration of classes that is used for managing tags and tag buffers.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_TAGS_H__
#define __GA_TAGS_H__

#include "HashMap.h"
#include "SmartPtr.h"
#include "Array.h"
#include "List.h"
#include "Flags.h"

namespace Common
{
	namespace Data
	{

		/// <summary><c>GaTag</c> class represents interface for tag objects.</summary>
		class GaTag
		{

		public:

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaTag() { }

		};

		/// <summary><c>GaTagLifecycle</c> is interface for managing tag lifecycle.</summary>
		class GaTagLifecycle
		{

		public:

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaTagLifecycle() { }
			
			/// <summary><c>Clone</c> method creates copy of tag lifecycle policy.</summary>
			/// <returns>Method returns pointer to a newly created copy of lifecycle policy.</returns>
			virtual GaTagLifecycle* Clone() const = 0; 

			/// <summary>CreateTag</c> method should implement tag creation policy.</summary>
			/// <returns>Method returns pointer to a newly created tag.</returns>
			virtual GaTag* GACALL CreateTag() const = 0;

			/// <summary>PrepareTag</c> method should implement tag preparation policy.</summary>
			/// <param name="tag">tag that should be prepared befor use.</param>
			virtual void GACALL PrepareTag(GaTag& tag) const = 0;

			/// <summary>CleanTag</c> method should implement tag clean-up policy.</summary>
			/// <param name="tag">tag that should be cleaned after use.</param>
			virtual void GACALL CleanTag(GaTag& tag) const = 0;

		};

		/// <summary><c>GaTagUpdate</c> base class for tag update policy.</summary>
		class GaTagUpdate
		{

		public:

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaTagUpdate() { }

			/// <summary><c>operator()</c> performs update operation over specified tag.</summary>
			/// <param name="tag">reference to the tag that should be updated.</param>
			virtual void GACALL operator()(GaTag& tag) const = 0;

			/// <summary><c>operator()</c> udates lifecycle manager to relfect changes made on tags.</summary>
			/// <param name="tagLifecycle">tag lifecycle manager that should be updated.</param>
			virtual void GACALL operator()(GaTagLifecycle& tagLifecycle) const = 0;

			/// <summary><c>IsRequired</c> method checks tag's lifecycle manager to see if the update is really required.</summary>
			/// <param name="tagLifecycle">tag lifecycle manager of tag that chould be updated.</param>
			/// <returns>Method returns <c>true</c> if the update should be performed.</returns>
			virtual bool GACALL IsRequired(const GaTagLifecycle& tagLifecycle) const = 0;

		};

		/// <summary><c>GaTypedTag</c> template class represent tag that can store single object or reference to data of specified type.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// bit all public are thread-safe.</summary>
		/// <param name="TYPE">type of data stored in the tag.</param>
		template<typename TYPE>
		class GaTypedTag : public GaTag
		{

		public:

			/// <summary>Type of data stored in the tag.</summary>
			typedef TYPE GaType;

		protected:

			/// <summary>Object that stores tag data or reference to them. Storing of the tag data is defined by external policy.</summary>
			GaType _data;

		public:

			/// <summary>This constructor copy provided data and stores them to the new tag.</summary>
			/// <param name="data">data that should be stored in tag.</param>
			GaTypedTag(const GaType& data) : _data(data) { }

			/// <summary>This constructor copy data from another tag.</summary>
			/// <param name="rhs">source tag that should be copied.</param>
			GaTypedTag(const GaTypedTag<TYPE>& rhs) : _data(rhs._data) { }

			/// <summary>This constructor initializes empty tag.</summary>
			GaTypedTag() : _data() { }

			/// <summary><c>SetData</c> method stores copies provided data and stores tham data to the tag.
			/// Copying of data is defined by external policy.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="data">data which should be stored in tag.</param>
			inline void GACALL SetData(const GaType& data) { _data = data; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to data stored in the tag.</returns>
			inline typename GaType& GACALL GetData() { return _data; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to data stored in the tag.</returns>
			inline typename const GaType& GACALL GetData() const { return _data; }

		};

		/// <summary><c>GaTypedTagLifecycle</c> class implements lifecycle manager for tags that stores data of specified type.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="DATA_TYPE">type of data stored in the tag.</param>
		template<typename DATA_TYPE>
		class GaTypedTagLifecycle : public GaTagLifecycle
		{

		public:

			/// <summary>Type of data stored in the tag.</summary>
			typedef DATA_TYPE GaDataType;

		public:

			/// <summary>More details are given in specification of <see cref="GaTagLifecycle::Clone" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual GaTagLifecycle* Clone() const { return new GaTypedTagLifecycle<GaDataType>( *this ); }

			/// <summary>More details are given in specification of <see cref="GaTagLifecycle::CreateTag" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual GaTag* GACALL CreateTag() const { return new GaTypedTag<GaDataType>(); }

			/// <summary>More details are given in specification of <see cref="GaTagLifecycle::PrepareTag" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL PrepareTag(GaTag& tag) const { }

			/// <summary>More details are given in specification of <see cref="GaTagLifecycle::CleanTag" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL CleanTag(GaTag& tag) const { }

		};

		/// <summary><c>GaTagBuffer</c> class provides storage for object's tags.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaTagBuffer
		{

		private:

			/// <summary>Array that stores pointers to tags.</summary>
			Data::GaSingleDimensionArray<GaTag*> _tags;

		public:

			/// <summary>Initializes buffer with defined initial size.</summary>
			/// <param name="initialSize">initial size of the buffer.</param>
			GaTagBuffer(int initialSize) : _tags(initialSize) { }

			/// <summary>Initializes empty tag buffer.</summary>
			GaTagBuffer() { }

			/// <summary>Free resource used by the tags stored in this buffer and buffer itself.</summary>
			~GaTagBuffer()
			{
				// delete all tags in buffer
				for( int i = _tags.GetSize() - 1; i >= 0; i-- )
				{
					if( _tags[ i ] )
						delete _tags[ i ];
				}
			}

			/// <summary><c>SetTag</c> method stores pointer to the tag at the specified position in the buffer.
			/// If there is already tag at specified position, this method deletes old tag.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="index">index at whic the tag should be stored in the buffer.</param>
			/// <param name="tag">pointer to the tag.</param>
			inline void GACALL SetTag(int index,
				GaTag* tag)
			{
				// delete old tag
				if( _tags[ index ] )
					delete _tags[ index ];

				_tags[ index ] = tag;
			}

			/// <summary><c>RemoveTag</c> method deletes tag at specified index if it exists.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="index">index of the tag in the buffer.</param>
			inline void GACALL RemoveTag(int index) { SetTag( index, NULL ); }

			/// <summary><c>GetAt</c> method retrieves tag from the buffer.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="index">index of the tag in the buffer.</param>
			/// <returns>Method returns reference to tag at specified index in the buffer.</returns>
			inline GaTag& GACALL GetAt(int index) { return *_tags[ index ]; } 

			/// <summary><c>GetAt</c> method retrieves tag from the buffer.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="index">index of the tag in the buffer.</param>
			/// <returns>Method returns reference to tag at specified index in the buffer.</returns>
			inline const GaTag& GACALL GetAt(int index) const { return *_tags[ index ]; }

			/// <summary><c>SetSize</c> method resizes buffer. If the new size is smaller than the old, this method deletes tags that are located in truncated part of the buffer.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="size">new size of the buffer.</param>
			inline void GACALL SetSize(int size)
			{
				// delete shrinked tags
				for( int i = _tags.GetSize() - 1; i >= size; i-- )
				{
					if( _tags[ i ] )
						delete _tags[ i ];
				}

				_tags.SetSize( size );
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns current size of the buffer.</returns>
			inline int GACALL GetSize() const { return _tags.GetSize(); }

			/// <summary><c>operator []</c> retrieves tag from the buffer.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="index">index of the tag in the buffer.</param>
			/// <returns>Operator returns reference to tag at specified index in the buffer.</returns>
			inline GaTag& GACALL operator [](int index) { return GetAt( index ); }

			/// <summary><c>operator []</c> retrieves tag from the buffer.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="index">index of the tag in the buffer.</param>
			/// <returns>Operator returns reference to tag at specified index in the buffer.</returns>
			inline const GaTag& GACALL operator [](int index) const { return GetAt( index ); }

		};

		/// <summary><c>GaTagUpdate</c> class is interface for tag buffer update operations.</summary>
		class GaTagBufferUpdate
		{

		public:

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaTagBufferUpdate() { }

			/// <summary><c>AddTag</c> method adds specified tag to tag buffers.</summary>
			/// <param name="index">index at which tag should be inserted into </param>
			/// <param name="tagLifecycle">lifecycle manager of new tag.</param>
			virtual void GACALL AddTag(int index,
				const GaTagLifecycle& tagLifecycle) const = 0;

			/// <summary><c>RemoveTag</c> method removes tag from tag buffers.</summary>
			/// <param name="index">index of the tag that should be removed.</param>
			virtual void GACALL RemoveTag(int index) const = 0;

			/// <summary><c>UpdateTag</c> method tags in tag buffers.</summary>
			/// <param name="index">index of the tag that should be updated.</param>
			/// <param name="update">update operation that should be performed over tag.</param>
			virtual void GACALL UpdateTag(int index,
				const GaTagUpdate& update) const = 0;

			/// <summary><c>SetBufferSize</c> method change size of tag buffers.</summary>
			/// <param name="newSize">new size of tag buffer.</param>
			virtual void GACALL SetBufferSize(int newSize) const = 0;

		};

		/// <summary><c>GaTagManager</c> class manages tags and tag buffers.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaTagManager
		{

		private:

			/// <summary>Type of hash map that stores translation of tag IDs to indices in tag buffer.</summary>
			typedef STLEXT::hash_map<int, std::pair<int, GaTagLifecycle*> > GaTagIndicesTable;

			/// <summary>Object used for updating tag buffers when tags are added, removed or modified.</summary>
			Memory::GaAutoPtr<GaTagBufferUpdate> _update;

			/// <summary>Stores maping of tag IDs to indices in tag buffer and tag prototypes.</summary>
			GaTagIndicesTable _tagIndices;

			/// <summary>Array that stores indices of unused slots in tag buffer.</summary>
			Memory::GaAutoPtr<int> _freeIndices;

			/// <summary>Count of unused slots in tag buffer.</summary>
			int _freeIndicesCount;

			/// <summary>Number of tags that currently tag buffers can store.</summary>
			int _bufferSize;

		public:

			/// <summary>Initialize tag manager with population which it manages and initial size of tag buffers.</summary>
			/// <param name="initialSize">initial size of tag buffers that should be created.</param>
			/// <param name="update">pointer to object used for updating tag buffers when tags are added, removed or modified. Tag manager takes ownership of the update object.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>initialSize</c> is negative value or 0.</exception>
			GaTagManager(int initialSize,
				GaTagBufferUpdate* update) : _update(update),
				_freeIndicesCount(0),
				_bufferSize(0)
			{
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, initialSize > 0, "initialSize", "Initial size cannot be negative value or zero", "Data" );
				ChangeBufferSize( initialSize );
			}

			/// <summary>Deletes tag prototypes.</summary>
			~GaTagManager()
			{
				for( GaTagIndicesTable::iterator it = _tagIndices.begin(); it != _tagIndices.end(); ++it )
					delete it->second.second;
			}

			/// <summary><c>AddTag</c> inserts new tag to tag buffers and maps ID of new tag to index of slot int tag buffer that will be used to store tag.
			/// If tag buffers ar full this method increase size of tag buffers.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the new tag.</param>
			/// <param name="tagLifecycle">lifecycle manager of new tag.</param>
			/// <param name="shouldThrow">if this parameter is set to <c>true</c> method will throw exception if tag with specified ID already exists.</param>
			/// <returns>Method returns index of slot in tag buffers at which the tag is located.</returns>
			/// <exception cref="GaArgumentException" />Thrown if tag with specified ID already exists and <c>shouldThrow</c> parameter is set to <c>true</c>.</exception>
			GAL_API
			int GACALL AddTag(int tagID,
				const GaTagLifecycle& tagLifecycle,
				bool shouldThrow = false);

			/// <summary><c>RemoveTag</c> method removes tag from tag buffers. The method can shrink size of tag buffers after it removes tag from them.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag that should be removed.</param>
			/// <returns>Method returns <c>true</c> if tag are found and removed from tag bufers.</returns>
			GAL_API
			bool GACALL RemoveTag(int tagID);

			/// <summary><c>ReplaceTag</c> method replaces old tag with new in tag buffers.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag that should be replaced.</param>
			/// <param name="tagLifecycle">lifecycle manager of new tag.</param>
			/// <param name="shouldThrow">if this parameter is set to <c>true</c> method will throw exception if tag with specified ID does not exist.</param>
			/// <returns>Method returns index of slot in tag buffers at which new tag is inseted or -1 if tag is removed.</returns>
			/// <exception cref="GaArgumentException" />Thrown if tag does not exist and and <c>shouldThrow</c> parameter is set to <c>true</c>.</exception>
			GAL_API
			int GACALL ReplaceTag(int tagID,
				const GaTagLifecycle& tagLifecycle,
				bool shouldThrow = false);

			/// <summary><c>UpdateTag</c> method updates tag with specified ID.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag that should be updated.</param>
			/// <param name="tagUpdate">tag update policy.</param>
			/// <exception cref="GaArgumentException" />Thrown if tag does not exist.</exception>
			GAL_API
			void GACALL UpdateTag(int tagID,
				const GaTagUpdate& tagUpdate);

			/// <summary><c>ChangeTagID</c> method changes tag of the ID.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="currentTagID">current ID of the tag.</param>
			/// <param name="newTagID">desired ID of the tag.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if tag with specified current ID does not exist.</exception>
			/// <exception cref="GaArgumentException" />Thrown if tag with requested new ID already exists.</exception>
			GAL_API
			void GACALL ChangeTagID(int currentTagID,
				int newTagID);

			/// <summary><c>GetTagIndex</c> method returns index of slot in tag buffer in which the tag with specified id is stored.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag whose slot index is queried.</param>
			/// <returns>Method returns slot index if the tag with specified ID exist or -1 if it does not exist.</returns>
			inline int GACALL GetTagIndex(int tagID) const
			{
				GaTagIndicesTable::const_iterator it = _tagIndices.find( tagID );
				return it != _tagIndices.end() ? it->second.first : -1;
			}

			/// <summary><c>InitBuffer</c> method initializes and fills provided tag buffer,
			///
			/// This method is not thread-safe.</summary>
			/// <param name="buffer">reference to buffer that should be initialized.</param>
			void GACALL InitBuffer(GaTagBuffer& buffer) const
			{
				buffer.SetSize( _bufferSize );

				// fill buffer with tag created using stored prototypes
				for( GaTagIndicesTable::const_iterator it = _tagIndices.begin(); it != _tagIndices.end(); ++it )
					buffer.SetTag( it->second.first, it->second.second->CreateTag() );
			}

			/// <summary><c>PrepareBuffer</c> method prepares tags in specified buffer before use.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="buffer">reference to buffer that should be prepared.</param>
			void GACALL PrepareBuffer(GaTagBuffer& buffer) const
			{
				for( GaTagIndicesTable::const_iterator it = _tagIndices.begin(); it != _tagIndices.end(); ++it )
					it->second.second->PrepareTag( buffer[ it->second.first ] );
			}

			/// <summary><c>CleanBuffer</c> method clears tags in specified buffer that contains data.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="buffer">reference to buffer that should be cleared.</param>
			void GACALL CleanBuffer(GaTagBuffer& buffer) const
			{
				for( GaTagIndicesTable::const_iterator it = _tagIndices.begin(); it != _tagIndices.end(); ++it )
					it->second.second->CleanTag( buffer[ it->second.first ] );
			}

			/// <summary><c>SetUpdate</c> method sets object that will be used for updating tag buffers when tags are added, removed or modified.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="update">update object. Tag manager takes ownership of the update object.</param>
			inline void GACALL SetUpdate(Memory::GaAutoPtr<GaTagBufferUpdate> update) { _update = update; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to object used for updating tag buffers when tags are added, removed or modified.</returns>
			inline GaTagBufferUpdate& GACALL GetUpdate() { return *_update; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to object used for updating tag buffers when tags are added, removed or modified.</returns>
			inline const GaTagBufferUpdate& GACALL GetUpdate() const { return *_update; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if the tag buffer update object is specified.</returns>
			inline bool GACALL HasUpdate() const { return !_update.IsNull(); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of tags that currenty tag buffers can store.</returns>
			inline int GACALL GetTagBufferSize() const { return _bufferSize; }

		private:

			/// <summary><c>ShrinkTagBuffer</c> shrinks size of tag buffers if it is possible and reasonable.</summary>
			GAL_API
			void GACALL ShrinkTagBuffer();

			/// <summary><c>ChangeBufferSize</c> method changes size of tag buffers by specified value.</summary>
			/// <param name="delta">value by which the size of buffers should increase or decrease. This parameter can have positive or negative value.</param>
			GAL_API
			void GACALL ChangeBufferSize(int delta);

		};

		/// <summary><c>GaSizableTagLifecycle</c> class implements lifecycle manager for tags that stores data of specified type, but which also requires size management.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="TYPE">type of data stored in the tag.</param>
		template<typename DATA_TYPE>
		class GaSizableTagLifecycle : public GaTagLifecycle
		{

		public:

			/// <summary>Type of data stored in the tag.</summary>
			typedef DATA_TYPE GaDataType;

		private:

			/// <summary>Size of tag's data.</summary>
			int _size;

		public:

			/// <summary>This constructor initializes lifecycle manager with specified size of tag's data.</summary>
			/// <param name="size">size of tag's data.</param>
			GaSizableTagLifecycle(int size) : _size(size) { }

			/// <summary>This constructor initializes lifecycle manager with size of tag's data set to <c>0</c>.</summary>
			GaSizableTagLifecycle() : _size(0) { }

			/// <summary>More details are given in specification of <see cref="GaTagLifecycle::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaTagLifecycle* Clone() const { return new GaSizableTagLifecycle<GaDataType>( *this ); }

			/// <summary>More details are given in specification of <see cref="GaTagLifecycle::CreateTag" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaTag* GACALL CreateTag() const { return new GaTypedTag<GaDataType>( _size ); }

			/// <summary>More details are given in specification of <see cref="GaTagLifecycle::PrepareTag" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL PrepareTag(GaTag& tag) const { }

			/// <summary>More details are given in specification of <see cref="GaTagLifecycle::CleanTag" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL CleanTag(GaTag& tag) const { }

			/// <summary><c>SetSize</c> method sets size of tag's data.</summary>
			/// <param name="size">new size of data.</param>
			inline void GACALL SetSize(int size) { _size = size; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns size of tag's data.</returns>
			inline int GACALL GetSize() const { return _size; }

		};

		/// <summary><c>GaSizableTagUpdate</c> class implements policy that updates size of data stored int the tag.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="TYPE">type of data stored in the tag.</param>
		/// <param name="MERGE_METHOD">method that is used for merging data that was trimmed with ones that stays.</param>
		template<typename DATA_TYPE,
			void GACALL MERGE_METHOD(DATA_TYPE&, int, int)>
		class GaSizableTagUpdate : public GaTagUpdate
		{

		public:

			/// <summary>Type of data stored in the tag.</summary>
			typedef DATA_TYPE GaDataType;

			/// <summary>Type of tag's lifecycle manager.</summary>
			typedef GaSizableTagLifecycle<GaDataType> GaLifecycleType;

		private:

			/// <summary>New size of tag's data.</summary>
			int _newSize;

		public:

			/// <summary>This constructor initializes update operation with new size of tag's data.</summary>
			/// <param name="newSize">new size of data.</param>
			GaSizableTagUpdate(int newSize) : _newSize(newSize) { }

			/// <summary><c>operator ()</c> updates resizes data stored in the tag and merges trimmed data if required.
			/// For more details see specification of <see cref="GaTagUpdate::operator ()" />.
			///
			/// This operator is thread-safe.</summary>
			virtual void GACALL operator()(GaTag& tag) const
			{
				GaDataType& data = ( (GaTypedTag<GaDataType>&)tag ).GetData();

				MERGE_METHOD( data, data.GetSize(), _newSize );
				data.SetSize( _newSize );
			}

			/// <summary><c>operator ()</c> method updates tag's lifecycle manager with new data size.
			/// For more details see specification of <see cref="GaTagUpdate::operator ()" /> method.
			///
			/// This operator is thread-safe.</summary>
			virtual void GACALL operator()(GaTagLifecycle& tagLifecycle) const { ( (GaLifecycleType&)tagLifecycle ).SetSize( _newSize ); }

			/// <summary><c>IsRequired</c> method checks if the size of data has been changed.
			/// For more details see specification of <see cref="GaTagUpdate::IsRequired" /> method.
			///
			/// This operator is thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if the size is change.</returns>
			virtual bool GACALL IsRequired(const GaTagLifecycle& tagLifecycle) const { return ( (const GaLifecycleType&)tagLifecycle ).GetSize() != _newSize; }

		};

		/// <summary><c>GaTagIgnoreMerge</c> function rejects trimmed data.</summary>
		/// <param name="STORAGE_TYPE">type that of storage used for storing tag's data.</param>
		/// <param name="storage">reference to storage that contains tag's data.</param>
		/// <param name="oldSize">old count of data paritions.</param>
		/// <param name="newSize">new count of data paritions.</param>
		template<typename STORAGE_TYPE>
		inline void GACALL GaTagIgnoreMerge(STORAGE_TYPE& storage,
			int oldSize,
			int newSize) { }

		/// <summary><c>GaTagSumMerge</c> function merges trimmed data with the ones that stays by summing them.</summary>
		/// <param name="STORAGE_TYPE">type that of storage used for storing tag's data.</param>
		/// <param name="storage">reference to storage that contains tag's data.</param>
		/// <param name="oldSize">old count of data paritions.</param>
		/// <param name="newSize">new count of data paritions.</param>
		template<typename STORAGE_TYPE>
		inline void GACALL GaTagSumMerge(STORAGE_TYPE& storage,
			int oldSize,
			int newSize)
		{
			for( int j = oldSize - 1; j >= newSize; j++ )
				storage[ j % newSize ] += storage[ j ];
		}

		/// <summary><c>GaTagListMerge</c> function merges trimmed linked lists with the ones that stays.</summary>
		/// <param name="STORAGE_TYPE">type that of storage used for storing tag's data.</param>
		/// <param name="storage">reference to storage that contains tag's data.</param>
		/// <param name="oldSize">old count of data paritions.</param>
		/// <param name="newSize">new count of data paritions.</param>
		template<typename STORAGE_TYPE>
		inline void GACALL GaTagListMerge(STORAGE_TYPE& storage,
			int oldSize,
			int newSize)
		{
			for( int j = oldSize - 1; j >= newSize; j++ )
				storage[ j % newSize ].MergeLists( &storage[ j ] );
		}

		/// <summary>Datatype used for storing partial sum in tags.</summary>
		typedef Common::Data::GaAlignedArray<float> GaPartialSum;

		/// <summary>Datatype used for storing partial counter in tags.</summary>
		typedef Common::Data::GaAlignedArray<int> GaPartialCount;

		/// <summary>Datatype used for updating number of partitions for tags that stores partial sum.</summary>
		typedef GaSizableTagUpdate<GaPartialSum, GaTagSumMerge<GaPartialSum> > GaPartialSumSizeUpdate;

		/// <summary>Datatype used for updating number of partitions for tags that stores partial count.</summary>
		typedef GaSizableTagUpdate<GaPartialCount, GaTagSumMerge<GaPartialCount> > GaPartialCountSizeUpdate;

		/// <summary>This structure defines datatype used for storing partial list in tags.</summary>
		/// <param name="DATA_TYPE">type of data stored in the list.</param>
		template<typename DATA_TYPE>
		struct GaPartialList
		{

			/// <summary>Datatype used for storing partial list in tags.</summary>
			typedef typename Common::Data::GaAlignedArray<Common::Data::GaList<DATA_TYPE> > GaType;

		};

		/// <summary>This structure defines datatype used for updating number of partitions for tags that stores partial list.</summary>
		/// <param name="DATA_TYPE">type of data stored in the list.</param>
		template<typename DATA_TYPE>
		struct GaPartialListSizeUpdate
		{

			/// <summary>Datatype used for updating number of partitions for tags that stores partial list.</summary>
			typedef GaSizableTagUpdate<typename GaPartialList<DATA_TYPE>::GaType, GaTagListMerge<typename GaPartialList<DATA_TYPE>::GaType> > GaType;

		};

		/// <summary><c>GaDefaultTagAccessor</c> class implements default policy for accessing object's tags.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but because objects of this class are stateless all public an private method are thread-safe.</summary>
		/// <param name="OBJECT_TYPE">type of object whose tags are queried.</param>
		template<typename OBJECT_TYPE>
		class GaDefaultTagAccessor
		{

		public:

			/// <summary><c>Get</c> method extract tag at specified index from object's tag buffer.
			///
			/// This method is thread-safe.</summary>
			/// <param name="object">object whose taq is queried.</param>
			/// <param name="index">index of the tag.</param>
			/// <returns>Method returns reference to the extracted tag.</returns>
			static inline GaTag& GACALL Get(OBJECT_TYPE& object,
				int index) { return object.GetTags()[ index ]; }

			/// <summary><c>Get</c> method extract tag at specified index from object's tag buffer.
			///
			/// This method is thread-safe.</summary>
			/// <param name="object">object whose taq is queried.</param>
			/// <param name="index">index of the tag.</param>
			/// <returns>Method returns reference to the extracted tag.</returns>
			static inline const GaTag& GACALL Get(const OBJECT_TYPE& object,
				int index) { return object.GetTags()[ index ]; }

		};

		/// <summary><c>GaTagGetter</c> class provides access to typed data of object's tags.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="DATA_TYPE">type of data stored in tag.</param>
		/// <param name="OBJECT_TYPE">type of object whose tags are queried.</param>
		/// <param name="TAG_ACCESSOR">access policy for object's tags.</param>
		template<typename DATA_TYPE,
			typename OBJECT_TYPE,
			typename TAG_ACCESSOR = GaDefaultTagAccessor<OBJECT_TYPE> >
		class GaTagGetter
		{

		public:

			/// <summary>Type of data stored in tag.</summary>
			typedef DATA_TYPE GaDataType;

			/// <summary>Type of object whose tags are queried</summary>
			typedef OBJECT_TYPE GaObjectType;

			/// <summary>Access policy for object's tags.</summary>
			typedef TAG_ACCESSOR GaTagAccessor;

		private:

			/// <summary>Index at which the tag is located in tag buffer of the object.</summary>
			int _tagIndex;

		public:

			/// <summary>Initializes tag getter for specific tag.</summary>
			/// <param name="tagID">tag ID.</param>
			/// <param name="tagManager">pointer to manager that handles tags and tag buffers</param>
			GaTagGetter(int tagID,
				const GaTagManager& tagManager) { SetTagID( tagID, tagManager ); }

			/// <summary>Default constructor.</summary>
			GaTagGetter() : _tagIndex(-1) { }

			/// <summary><c>SetTagID</c> sets which tag should be accessed.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="tagID">tag ID.</param>
			/// <param name="tagManager">pointer to manager that handles tags and tag buffers</param>
			inline void GACALL SetTagID(int tagID,
				const GaTagManager& tagManager) { _tagIndex = tagID >= 0 ? tagManager.GetTagIndex( tagID ) : -1; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns index at which the tag is located in tag buffer of the object.</returns>
			inline int GACALL GetTagIndex() const { return _tagIndex; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Returns <c>true</c> if tag existed when tag ID was set.</returns>
			inline bool GACALL IsExist() const { return _tagIndex >= 0; }

			/// <summary><c>operator ()</c> operator extracts tag data from object's tag buffer.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="object">reference to object from which the tag data should be extracted.</param>
			/// <returns>Operator returns reference to tag's data.</returns>
			inline GaDataType& GACALL operator ()(GaObjectType& object)
				{ return ( (Common::Data::GaTypedTag<GaDataType>&)GaTagAccessor::Get( object, _tagIndex ) ).GetData(); }

			/// <summary><c>operator ()</c> operator extracts tag data from object's tag buffer.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="object">reference to object from which the tag data should be extracted.</param>
			/// <returns>Operator returns reference to tag's data.</returns>
			inline const GaDataType& GACALL operator ()(const GaObjectType& object) const
				{ return ( (const Common::Data::GaTypedTag<GaDataType>&)GaTagAccessor::Get( object, _tagIndex ) ).GetData(); }

			/// <summary><c>operator ()</c> operator extracts tag data from object's tag buffer.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="object">reference to object from which the tag data should be extracted.</param>
			/// <returns>Operator returns reference to tag's data.</returns>
			inline GaDataType& GACALL operator ()(GaObjectType* object) { return operator ()( *object ); }

			/// <summary><c>operator ()</c> operator extracts tag data from object's tag buffer.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="object">reference to object from which the tag data should be extracted.</param>
			/// <returns>Operator returns reference to tag's data.</returns>
			inline const GaDataType& GACALL operator ()(GaObjectType* object) const { return operator ()( *object ); }

		};

	} // Data
} // Common

#endif // __GA_TAGS_H__

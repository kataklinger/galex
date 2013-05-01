
/*! \file List.h
	\brief This file declares classes that implaments doubly linked lists.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_LIST_H__
#define __GA_LIST_H__

#include "Exceptions.h"
#include "Node.h"

namespace Common
{
	namespace Data
	{

		class GaListBase;

		/// <summary><c>GaListNodeBase</c> represent base class for nodes of doubly linked list.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaListNodeBase : public GaNodeBase
		{

			friend class GaListBase;

		protected:

			/// <summary>Pointer to the next node in the list.</summary>
			GaListNodeBase* _next;

			/// <summary>Pointer to the previous node in the list.</summary>
			GaListNodeBase* _previous;

		public:

			/// <summary>This constructor initializes empty and unconnected node.</summary>
			GaListNodeBase() : _next(NULL),
				_previous(NULL) { }

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaListNodeBase() { }

			/// <summary><c>SetNext</c> method sets the next node.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="next">node that should be linked as next.</param>
			inline void GACALL SetNext(GaListNodeBase* next) { _next = next; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the next node in the list or <c>NULL</c> if there is no such a node.</returns>
			inline GaListNodeBase* GACALL GetNext() { return _next; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the next node in the list or <c>NULL</c> if there is no such a node.</returns>
			inline const GaListNodeBase* GACALL GetNext() const { return _next; }

			/// <summary><c>SetPrevious</c> method sets the previous node.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="previous">node that should be linked as previous.</param>
			inline void GACALL SetPrevious(GaListNodeBase* previous) { _previous = previous; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the previous node in the list or <c>NULL</c> if there is no such a node.</returns>
			inline GaListNodeBase* GACALL GetPrevious() { return _previous; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the previous node in the list or <c>NULL</c> if there is no such a node.</returns>
			inline const GaListNodeBase* GACALL GetPrevious() const { return _previous; }

		};

		/// <summary><c>GaListBase</c> is base class for doubly linked list that provides basic operations for handling the structure of the list.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaListBase
		{

		public:

			/// <summary>Defines type of base list structure for inherited classes.</summary>
			typedef GaListBase GaBaseStructure;

		protected:

			/// <summary>The first node in the list.</summary>
			GaListNodeBase* _head;

			/// <summary>The last node in the list.</summary>
			GaListNodeBase* _tail;

			/// <summary>Number of nodes in the list.</summary>
			int _count;

		public:

			/// <summary>This constructor initializes empty list.</summary>
			GaListBase() : _head(NULL),
				_tail(NULL),
				_count(0) { }

			/// <summary>This is constructor creates copy of provided list.</summary>
			/// <param name="rhs">list that should be copied.</param>
			GaListBase(const GaListBase& rhs) : _head(NULL),
				_tail(NULL),
				_count(0) { Copy( &rhs ); }

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaListBase() { }

			/// <summary><c>InsertHead</c> method inserts specified node as the first node of the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">node that should be inserted.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>node</c> is set to <c>NULL</c>.</exception>
			GAL_API
			void GACALL InsertHead(GaListNodeBase* node);

			/// <summary><c>InsertTail</c> method inserts specified node as the last node of the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">node that should be inserted.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>node</c> is set to <c>NULL</c>.</exception>
			GAL_API
			void GACALL InsertTail(GaListNodeBase* node);

			/// <summary><c>InsertAt</c> method inserts privided node at specified position (index) in the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="position">position (index) at which the node should be inserted. If position is out of range, node will be inserted at the first or last node.</param>
			/// <param name="node">node that should be inserted.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>node</c> is set to <c>NULL</c>.</exception>
			GAL_API
			void GACALL InsertAt(int position,
				GaListNodeBase* node);

			/// <summary><c>InsertBefore</c> method method inserts new node before specified node in the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="baseNode">node before which the new node should be inserted. If this parameter is set to <c>NULL</c>
			/// new node will be added at the end of the list.</param>
			/// <param name="node">node that should be inserted.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>node</c> is set to <c>NULL</c>.</exception>
			GAL_API
			void GACALL InsertBefore(GaListNodeBase* baseNode,
				GaListNodeBase* node);

			/// <summary><c>InsertAfter</c> method method inserts new node after specified node in the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="baseNode">node after which the new node should be inserted. If this parameter is set to <c>NULL</c>
			/// new node will be added at the beggining of the list.</param>
			/// <param name="node">node that should be inserted.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>node</c> is set to <c>NULL</c>.</exception>
			GAL_API
			void GACALL InsertAfter(GaListNodeBase* baseNode,
				GaListNodeBase* node);

			/// <summary><c>MergeLists</c> method moves all nodes from provided list at the end of this list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="list">list that should be merged.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>list</c> is set to <c>NULL</c>.</exception>
			GAL_API
			void GACALL MergeLists(GaListBase* list);

			/// <summary><c>SwapLists</c> swaps all nodes between the two list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="list">list with which the nodes are should be swapped.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>list</c> is set to <c>NULL</c>.</exception>
			GAL_API
			void GACALL SwapLists(GaListBase* list);

			/// <summary><c>RemoveHead</c> method removes the first node from the list.
			///
			/// This method is not thread-safe.</summary>
			void GACALL RemoveHead()
			{
				if( _head )
					delete DetachNode( _head );
			}

			/// <summary><c>RemoveHead</c> method removes the last node from the list.
			///
			/// This method is not thread-safe.</summary>
			void GACALL RemoveTail()
			{
				if( _tail )
					delete DetachNode( _tail );
			}

			/// <summary><c>RemoveAt</c> method removes node(s) at specified position (index) from the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="position">position (index) of node that should be removed.</param>
			/// <param name="count">number of nodes that should be removed form specified onwards.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>position</c> is less than 0 and greater or equal to size of the list.</exception>
			inline void GACALL RemoveAt(int position,
				int count = 1)
			{
				GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, position >= 0 && position < _count, "position", "Specified position is out of range.", "Data" );
				Remove( GetAt( position ), count );
			}

			/// <summary><c>Remove</c> method removes specified node(s) from the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">the first node that should be removed.</param>
			/// <param name="count">number of nodes that should be removed form specified onwards.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>node</c> is set to <c>NULL</c>.</exception>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>count</c> is less than  or equal to 0.</exception>
			GAL_API
			void GACALL Remove(GaListNodeBase* node,
				int count = 1);

			/// <summary><c>Move</c> method moves node to new position in the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">node that should be moved.</param>
			/// <param name="newPosition">new position (index) at which the node should be placed.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>node</c> is set to <c>NULL</c>.</exception>
			GAL_API
			void GACALL Move(GaListNodeBase* node,
				int newPosition);

			/// <summary><c>Copy</c> method removes all nodes currently in the list and copies the nodes from the specified list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="list">list that should be copied.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>list</c> is set to <c>NULL</c>.</exception>
			GAL_API
			void GACALL Copy(const GaListBase* list);

			/// <summary><c>Clear</c> method removes all the nodes from the list and frees used resource.
			///
			/// This method is not thread-safe.</summary>
			GAL_API
			void GACALL Clear();

			/// <summary><c>GetNodePosition</c> method returns position of specified node in the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">node whose position is queried.</param>
			/// <returns>Method returns position of the node in the list or -1 if the node is not in the list.</returns>
			GAL_API
			int GACALL GetNodePosition(const GaListNodeBase* node) const;

			/// <summary><c>GetAt</c> method returns node at specifies position in the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="position">position of queried node.</param>
			/// <returns>Method returns pointer to the node at the specified position.</returns>
			GAL_API
			GaListNodeBase* GACALL GetAt(int position);

			/// <summary><c>GetAt</c> method returns node at specifies position in the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="position">position of queried node.</param>
			/// <returns>Method returns pointer to the node at the specified position.</returns>
			GAL_API
			const GaListNodeBase* GACALL GetAt(int position) const;

			/// <summary><c>Compare</c> method compares data stored in the two lists.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="list">list against which the comparison is performed.</param>
			/// <returns>Method returns <c>true</c> if the lists contain same data.</returns>
			GAL_API
			bool GACALL Compare(const GaListBase* list) const;

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of nodes currently in the list.</returns>
			inline int GACALL GetCount() const { return _count; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the first node of the list.</returns>
			inline GaListNodeBase* GACALL GetHead() { return _head; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the first node of the list.</returns>
			inline const GaListNodeBase* GACALL GetHead() const { return _head; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the last node of the list.</returns>
			inline GaListNodeBase* GACALL GetTail() { return _tail; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the last node of the list.</returns>
			inline const GaListNodeBase* GACALL GetTail() const { return _tail; }

			/// <summary><c>operator =</c> removes all nodes currently in the list and copies the nodes from the specified list.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="rhs">list that should be copied.</param>
			/// <returns>Operator returns reference to <c>this</c> object.</returns>
			inline GaListBase& GACALL operator =(const GaListBase& rhs)
			{
				Copy( &rhs );
				return *this;
			}

			/// <summary><c>operator ==</c> compares data stored in the two lists.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="rhs">list against which the comparison is performed.</param>
			/// <returns>Operator returns <c>true</c> if the lists contain same data.</returns>
			inline bool GACALL operator ==(const GaListBase& rhs) const  { return Compare( &rhs ); }

			/// <summary><c>operator !=</c> compares data stored in the two lists.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="rhs">list against which the comparison is performed.</param>
			/// <returns>Operator returns <c>true</c> if the lists do not contain same data.</returns>
			inline bool GACALL operator !=(const GaListBase& rhs) const  { return !Compare( &rhs ); }

			/// <summary><c>operator []</c> returns node at specifies position in the list.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="position">position of queried node.</param>
			/// <returns>Operator returns pointer to the node at the specified position.</returns>
			inline GaListNodeBase* GACALL operator [](int position) { return GetAt( position ); }
			
			/// <summary><c>operator []</c> returns node at specifies position in the list.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="position">position of queried node.</param>
			/// <returns>Operator returns pointer to the node at the specified position.</returns>
			inline const GaListNodeBase* GACALL operator [](int position) const { return GetAt( position ); }

		protected:

			/// <summary><c>DetachNode</c> method detaches specified node from the list and update node count but does not destory node.</summary>
			/// <param name="node">pointer to node that should be detached.</param>
			/// <returns>Method returns pointer to detached node.</returns>
			GAL_API
			GaListNodeBase* GACALL DetachNode(GaListNodeBase* node);

		};

		template<typename VALUE_TYPE>
		class GaList;

		/// <summary><c>GaListNode</c> represent node of a doubly linked list that stores values of specified type.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <typeparam name="VALUE_TYPE">type of stored value.</typeparam>
		template<typename VALUE_TYPE>
		class GaListNode : public GaListNodeBase
		{

			friend class GaList<VALUE_TYPE>;

		public:

			/// <summary>Type of stored value.</summary>
			typedef VALUE_TYPE GaValueType;

		protected:

			/// <summary>Value stored in the node.</summary>
			GaValueType _value;

		public:

			/// <summary>This method initializes node with user-defined value.</summary>
			/// <param name="value">value that should be stored in the node.</param>
			GaListNode(const GaValueType& value) : _value(value) { }

			/// <summary>This method initializes node with default value for of the type.</summary>
			GaListNode() : _value() { }

			/// <summary>More details are given in specification of <see cref="GaNode::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaNodeBase* GACALL Clone() const { return new GaListNode<GaValueType>( _value );  }

			/// <summary>More details are given in specification of <see cref="GaNode::SwapData" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>node</c> is set to <c>NULL</c>.</exception>
			virtual void GACALL SwapData(GaNodeBase* node)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, node != NULL, "node", "The second node must be specified.", "Data" );

				if( node != this )
				{
					// swap stored values
					GaValueType temp = _value;
					_value = ( (GaListNode<GaValueType>*)node )->_value;
					( (GaListNode<GaValueType>*)node )->_value = temp;
				}
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the next node in the list or <c>NULL</c> if there is no such a node.</returns>
			inline GaListNode<GaValueType>* GACALL GetNext() { return (GaListNode<GaValueType>*)_next; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the next node in the list or <c>NULL</c> if there is no such a node.</returns>
			inline const GaListNode<GaValueType>* GACALL GetNext() const { return (GaListNode<GaValueType>*)_next; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the previous node in the list or <c>NULL</c> if there is no such a node.</returns>
			inline GaListNode<GaValueType>* GACALL GetPrevious() { return (GaListNode<GaValueType>*)_previous; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the previous node in the list or <c>NULL</c> if there is no such a node.</returns>
			inline const GaListNode<GaValueType>* GACALL GetPrevious() const { return (GaListNode<GaValueType>*)_previous; }

			/// <summary><c>SetValue</c> method stores new value in the node.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">value that should be stored.</param>
			inline void GACALL SetValue(const GaValueType& value) { _value = value; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to value stored in the node.</returns>
			inline GaValueType& GACALL GetValue() { return _value; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to value stored in the node.</returns>
			inline const GaValueType& GACALL GetValue() const { return _value; }

			/// <summary><c>operator ==</c> compares values stored in the two nodes.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="node">node against whose value comparison is performed.</param>
			/// <returns>Operator returns <c>true</c> if values stored in nodes are equal.</returns>
			virtual bool GACALL operator ==(const GaNodeBase& node) const { return _value == ( (const GaListNode<GaValueType>&)node )._value; }

			/// <summary><c>operator !=</c> compares values stored in the two nodes.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="node">node against whose value comparison is performed.</param>
			/// <returns>Operator returns <c>true</c> if values stored in nodes are not equal.</returns>
			virtual bool GACALL operator !=(const GaNodeBase& node) const { return _value != ( (const GaListNode<GaValueType>&)node )._value; }

		};

		/// <summary><c>GaList</c> represent doubly linked list that stores values of specified type.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <typeparam name="VALUE_TYPE">type of stored value.</typeparam>
		template<typename VALUE_TYPE>
		class GaList : public GaListBase
		{

		public:

			/// <summary>Type of stored value.</summary>
			typedef VALUE_TYPE GaValueType;

			/// <summary>This constructor initializes empty list.</summary>
			GaList() { }

			/// <summary>This is constructor creates copy of provided list.</summary>
			/// <param name="rhs">list that should be copied.</param>
			GaList(const GaList& rhs) : GaListBase(rhs) { }

			/// <summary>Frees mamory used by list nodes.</summary>
			~GaList() { Clear(); }

			using GaListBase::InsertHead;

			/// <summary><c>InsertHead</c> method inserts specified value as the first value of the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">value that should be inserted.</param>
			inline void GACALL InsertHead(GaValueType value) { InsertHead( new GaListNode<GaValueType>( value ) ); }

			using GaListBase::InsertTail;

			/// <summary><c>InsertTail</c> method inserts specified value as the last value of the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">value that should be inserted.</param>
			inline void GACALL InsertTail(GaValueType value) { InsertTail( new GaListNode<GaValueType>( value ) ); }

			using GaListBase::InsertAt;

			/// <summary><c>InsertAt</c> method inserts privided value at specified position (index) in the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="position">position (index) at which the value should be inserted. If position is out of range, node will be inserted at the first or value node.</param>
			/// <param name="value">value that should be inserted.</param>
			inline void GACALL InsertAt(int position,
				GaValueType value) { InsertAt( position, new GaListNode<GaValueType>( value ) ); }

			using GaListBase::InsertBefore;

			/// <summary><c>InsertBefore</c> method method inserts new value before specified node in the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">node before which the new value should be inserted. If this parameter is set to <c>NULL</c>
			/// new value will be added at the end of the list.</param>
			/// <param name="value">value that should be inserted.</param>
			inline void GACALL InsertBefore(GaListNode<GaValueType>* node,
				GaValueType value) { InsertBefore( node, new GaListNode<GaValueType>( value ) ); }

			using GaListBase::InsertAfter;

			/// <summary><c>InsertAfter</c> method method inserts new value after specified node in the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">node after which the new value should be inserted. If this parameter is set to <c>NULL</c>
			/// new value will be added at the beggining of the list.</param>
			/// <param name="value">value that should be inserted.</param>
			inline void GACALL InsertAfter(GaListNode<GaValueType>* node,
				GaValueType value) { InsertAfter( node, new GaListNode<GaValueType>( value ) ); }

			using GaListBase::Remove;

			/// <summary><c>RemoveAt</c> method removes specified value from the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">value that shoudl be removed from the list.</param>
			/// <param name="all">if this parameter is set to <c>true</c> all nodes that contains specified value are removed from the list,
			/// otherwise only the first node that is found is removed.</param>
			/// <param name="start">node from which the searching for value starts. If this paramenter is set to <c>NULL</c> search starts form list's head.</param>
			/// <returns>Method returns pointer to the first node after the last removed node.</returns>
			GaListNode<GaValueType>* GACALL Remove(GaValueType value,
				bool all = false,
				GaListNode<GaValueType>* start = NULL)
			{
				// search for value from specified position
				GaListNode<GaValueType>* next = NULL;
				for( GaListNode<GaValueType>* node = start ? start : (GaListNode<GaValueType>*)_head; node; )
				{
					next = (GaListNode<GaValueType>*)node->_next;

					// node contains specified value
					if( node->_value == value )
					{
						// remove node form the list
						Remove( node );

						// should all occurrences of the value be removed 
						if( !all )
							break;
					}

					node = next;
				}

				// return node after the last removed node
				return next;
			}

			/// <summary><c>Find</c> method searches for nodes that contains specified value.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">value which should be found.</param>
			/// <param name="start">node from which the search starts. If this paramenter is set to <c>NULL</c> search starts form list's head.</param>
			/// <returns>Method returns pointer to the first node that contains specified value or <c>NULL</c> if list does not contain value.</returns>
			GaListNode<GaValueType>* GACALL Find(GaValueType value,
				GaListNode<GaValueType>* start = NULL)
			{
				// search for value from specified start position
				for( GaListNode<GaValueType>* node = start ? start : (GaListNode<GaValueType>*)_head; node; node = (GaListNode<GaValueType>*)node->_next )
				{
					// current node contains specified value?
					if( node->_value == value )
						return node;
				}

				return NULL;
			}

			/// <summary><c>Find</c> method searches for nodes that contains specified value.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">value which should be found.</param>
			/// <param name="start">node from which the search starts. If this paramenter is set to <c>NULL</c> search starts form list's head.</param>
			/// <returns>Method returns pointer to the first node that contains specified value or <c>NULL</c> if list does not contain value.</returns>
			const GaListNode<GaValueType>* GACALL Find(GaValueType value,
				const GaListNode<GaValueType>* start = NULL) const
			{
				// search for value from specified start position
				for( const GaListNode<GaValueType>* node = start ? start : (GaListNode<GaValueType>*)_head; node; node = (GaListNode<GaValueType>*)node->_next )
				{
					// current node contains specified value?
					if( node->_value == value )
						return node;
				}

				return NULL;
			}

			/// <summary><c>GetPosition</c> method finds position of the first occurrence of the spcified value in the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">value whose position is queried.</param>
			/// <returns>Method returns position (index) of the specified value in the list or -1 if list does not conatin value.</returns>
			int GACALL GetPosition(GaValueType value) const
			{
				// search for the value
				int position = 0;
				for( const GaListNode<GaValueType>* node = (GaListNode<GaValueType>*)_head; node; node = (GaListNode<GaValueType>*)node->_next, position++ )
				{
					// current node contains specified value?
					if( node->_value == value )
						// return index of the node
						return position;
				}

				// value was not found
				return -1;
			}

			/// <summary><c>SetValue</c> method stores value in node at specified position in the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="position">position of the node to which the value will be stored.</param>
			/// <param name="value">value which should be stored in the list.</param>
			inline void GACALL SetValue(int position,
				GaValueType value) { GetAt( position )->_value = value; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the first node of the list.</returns>
			inline GaListNode<GaValueType>* GACALL GetHead() { return (GaListNode<GaValueType>*)_head; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the first node of the list.</returns>
			inline const GaListNode<GaValueType>* GACALL GetHead() const { return (GaListNode<GaValueType>*)_head; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the last node of the list.</returns>
			inline GaListNode<GaValueType>* GACALL GetTail() { return (GaListNode<GaValueType>*)_tail; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the last node of the list.</returns>
			inline const GaListNode<GaValueType>* GACALL GetTail() const { return (GaListNode<GaValueType>*)_tail; }

			/// <summary><c>GetValue</c> method returns value stored in node at specified position in the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="position">position of the node.</param>
			/// <returns>Method returns value stored in the specified node.</returns>
			inline GaValueType& GACALL GetValue(int position) { return GetAt( position )->_value; }

			/// <summary><c>GetValue</c> method returns value stored in node at specified position in the list.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="position">position of the node.</param>
			/// <returns>Method returns value stored in the specified node.</returns>
			inline const GaValueType& GACALL GetValue(int position) const { return GetAt( position )->_value; }

			/// <summary><c>operator []</c> returns value stored in node at specified position in the list.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="position">position of the node.</param>
			/// <returns>Operator returns value stored in the specified node.</returns>
			inline GaValueType& GACALL operator [](int position) { return GetValue( position ); }

			/// <summary><c>operator []</c> returns value stored in node at specified position in the list.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="position">position of the node.</param>
			/// <returns>Operator returns value stored in the specified node.</returns>
			inline const GaValueType& GACALL operator [](int position) const { return GetValue( position ); }

		};

	} // Data
} // Common

#endif // __GA_LIST_H__

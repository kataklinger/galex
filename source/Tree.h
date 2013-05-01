
/*! \file Tree.h
	\brief This file declares classes that implaments tree data structure.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_TREE_H__
#define __GA_TREE_H__

#include "List.h"

namespace Common
{
	namespace Data
	{

		class GaTreeBase;

		/// <summary><c>GaTreeNodeBase</c> represent base class for nodes of tree data structure.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaTreeNodeBase : public GaNodeBase
		{

		protected:

			/// <summary>Pointer to the parent node.</summary>
			GaTreeNodeBase* _parent;

			/// <summary>List of node's children.</summary>
			GaList<GaTreeNodeBase*> _children;

		public:

			/// <summary>This constructor initializes empty tree node.</summary>
			GaTreeNodeBase() : _parent(NULL) { }

			/// <summary>This constructor copies whole subtree whose root is provided node.</summary>
			/// <param name="rhs">root node of subtree that should be copied.</param>
			GAL_API
			GaTreeNodeBase(const GaTreeNodeBase& rhs);

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			GAL_API
			virtual ~GaTreeNodeBase();

			/// <summary><c>CopySubtree</c> method creates copy of the subtree.</summary>
			/// <returns>Method returns pointer to the root node of the new subtree.</returns>
			virtual GaTreeNodeBase* CopySubtree() const = 0;

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of descendants this node has.</returns>
			GAL_API
			int GACALL GetCount() const;

			/// <summary><c>InsertChild</c> method attaches a child to the node.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">node that should be attached as child to this node.</param>
			/// <param name="position">position at which the child node should be inserted in list of node's children.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>node</c> is set to <c>NULL</c>.</exception>
			GAL_API
			void GACALL InsertChild(GaTreeNodeBase* node,
				int position = -1);

			/// <summary><c>DetachChild</c> method detaches specified child from node.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">child that should be detached.</param>
			/// <exception cref="GaInvalidOperationException" />Thrown if <c>node</c> is not child of this node.</exception>
			GAL_API
			void GACALL DetachChild(GaTreeNodeBase* node);

			/// <summary><c>Detach</c> method detaches node from its parent if it was attached.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL Detach() { if( _parent ) _parent->DetachChild( this ); }

			/// <summary><c>DeleteChildren</c> method deletes node's children and their subtrees.
			///
			/// This method is not thread-safe.</summary>
			GAL_API
			void GACALL DeleteChildren();

			/// <summary><c>MoveChildren</c> method moves children of this node to the specified node.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="destination">node to which the children should be moved.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>destination</c> is set to <c>NULL</c>.</exception>
			inline void GACALL MoveChildren(GaTreeNodeBase* destination)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, destination != NULL, "destination", "New parent must be specified.", "Data" );
				destination->_children.MergeLists( &_children );
			}

			/// <summary><c>SwapChildren</c> method swaps children of the two nodes.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">node with which the swap should occur.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>node</c> is set to <c>NULL</c>.</exception>
			inline void GACALL SwapChildren(GaTreeNodeBase* node)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, node != NULL, "node", "Node whose children should be moved must be specified.", "Data" );
				node->_children.SwapLists( &_children );
			}

			/// <summary><c>IsConnected</c> checks whether the two nodes are ancestor or descendant to each other.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">pointer to node which is tested for realtion with this node.</param>
			/// <returns>Method returns <c>true</c> if the two nodes are directly related.</returns>
			inline bool GACALL IsConnected(GaTreeNodeBase* node) { return IsAncestor( node ) || IsDescendant( node ); }

			/// <summary><c>IsDescendant</c> method checks whether the speficied node is descendant of this node.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">node that is checked if it is descendant.</param>
			/// <returns>Method returns <c>true</c> if the specified node is descendant.</returns>
			inline bool GACALL IsDescendant(GaTreeNodeBase* node) const { return node->IsAncestor( this ); }

			/// <summary><c>IsAncestor</c> method checks whether the speficied node is ancestor of this node.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">node that is checked if it is ancestor.</param>
			/// <returns>Method returns <c>true</c> if the specified node is ancestor.</returns>
			GAL_API
			bool GACALL IsAncestor(const GaTreeNodeBase* node) const;

			/// <summary><c>IsChild</c> method checks whether the speficied node is child of this node.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">node that is checked if it is child.</param>
			/// <returns>Method returns <c>true</c> if the specified node is child.</returns>
			inline bool GACALL IsChild(const GaTreeNodeBase* node) const { return _children.Find( (GaTreeNodeBase*)node ) != NULL; }

			/// <summary><c>SetParent</c> method sets parent of the node.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="parent"></param>
			inline void GACALL SetParent(GaTreeNodeBase* parent) { _parent = parent; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the parent node.</returns>
			inline GaTreeNodeBase* GACALL GetParent() { return _parent; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the parent node.</returns>
			inline const GaTreeNodeBase* GACALL GetParent() const { return _parent; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to list of node's children.</returns>
			inline GaList<GaTreeNodeBase*>* GACALL GetChildren() { return &_children; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to list of node's children.</returns>
			inline const GaList<GaTreeNodeBase*>* GACALL GetChildren() const { return &_children; }

		};

		/// <summary><c>GaTreeBase</c> is base class for trees that provides basic operations for handling the structure of the tree.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaTreeBase
		{

		public:

			/// <summary>Defines type of base tree structure for inherited classes.</summary>
			typedef GaTreeBase GaBaseStructure;

			/// <summary>Defines the ways that tree comparison </summary>
			enum GaTreeCompare
			{

				/// <summary>Comparison method should only compare topology of the trees.</summary>
				GATC_TOPOLOGY,

				/// <summary>Comparison method should compare topology of the trees as well as data stored in the nodes.</summary>
				GATC_DATA,

			};

		protected:

			/// <summary>Root node of the tree.</summary>
			GaTreeNodeBase* _root;

			/// <summary>Number of nodes in the tree.</summary>
			mutable int _count;

			/// <summary>Indicates whether the nodes have been added or removed from the three since the last time node count is calculated.</summary>
			mutable bool _modified;

		public:

			/// <summary>This constructor initializes empty tree.</summary>
			GaTreeBase() : _root(NULL),
				_count(0),
				_modified(false) { }

			/// <summary>This is constructor creates copy of provided tree.</summary>
			/// <param name="rhs">tree that should be copied.</param>
			GaTreeBase(const GaTreeBase& rhs) : _root(NULL),
				_count(rhs._count),
				_modified(rhs._modified)  { Copy( &rhs ); }

			/// <summary>Frees mamory used by tree nodes.</summary>
			virtual ~GaTreeBase() { Clear(); }

			/// <summary><c>Insert</c> method inserts new node as child of specified node in the tree.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">node that should be inserted.</param>
			/// <param name="parent">parent of the new node. If this paramenter is <c>NULL</c> new node will be the root of the tree and
			/// current root will be the child of the new node.</param>
			/// <param name="position">position in child list at which the new node is inserted. If this value is negative, new node will be inserted as last child.</param>
			GAL_API
			void GACALL Insert(GaTreeNodeBase* node,
				GaTreeNodeBase* parent,
				int position = -1);

			/// <summary><c>Remove</c> method removes specified node and all its descendants from the tree.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">node that should be removed.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>node</c> is set to <c>NULL</c>.</exception>
			GAL_API
			void GACALL Remove(GaTreeNodeBase* node);

			/// <summary><c>Remove</c> method removes children of the specified node and their descendants from the tree.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">node whose children should be removed.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>node</c> is set to <c>NULL</c>.</exception>
			GAL_API
			void GACALL RemoveChildren(GaTreeNodeBase* node);

			/// <summary><c>MoveNode</c> method moves specified node from its current parent to a new parent.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">node that should be moved.</param>
			/// <param name="newParent">parent node to which the node should be moved.</param>
			/// <param name="position">position in child list of the new parent at which the new node is inserted.
			/// If this value is negative, new node will be inserted as last child.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>node</c> or <c>newParent</c> are set to <c>NULL</c>.</exception>
			GAL_API
			void GACALL MoveNode(GaTreeNodeBase* node,
				GaTreeNodeBase* newParent,
				int position = -1);

			/// <summary><c>MoveChildren</c> method moves all children of the specified node to a new parent.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node">node whose children should be moved.</param>
			/// <param name="newParent">new parent of the children.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>node</c> is set to <c>NULL</c>.</exception>
			inline void GACALL MoveChildren(GaTreeNodeBase* node,
			GaTreeNodeBase* newParent)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, node != NULL, "node", "Node whose children should be moved must be specified.", "Data" );
				node->MoveChildren( newParent );
			}

			/// <summary><c>SwapNodes</c> method swaps positions of the nodes in the tree.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node1">the first node that should be swapped.</param>
			/// <param name="node2">the second node that should be swapped.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>node1</c> or <c>node2</c> are set to <c>NULL</c>.</exception>
			GAL_API
			void GACALL SwapNodes(GaTreeNodeBase* node1,
				GaTreeNodeBase* node2);

			/// <summary><c>SwapChildren</c> method swaps nodes' children.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="node1">the first node whose children should be swapped.</param>
			/// <param name="node2">the second node whose children should be swapped.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>node1</c> is set to <c>NULL</c>.</exception>
			inline void GACALL SwapChildren(GaTreeNodeBase* node1,
				GaTreeNodeBase* node2)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, node1 != NULL, "node1", "Node whose children should be moved must be specified.", "Data" );
				node1->SwapChildren( node2 );
			}

			/// <summary><c>DetachNode</c> method detaches node and all its descendant from the tree but does not destory the nodes.
			///
			/// <This method is not thread-safe./summary>
			/// <param name="node">node that shoudl be detached from the tree.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>node</c> is set to <c>NULL</c>.</exception>
			GAL_API
			void GACALL DetachNode(GaTreeNodeBase* node);

			/// <summary><c>Copy</c> method removes all nodes currently in the tree and copies the nodes from the specified tree.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tree">tree that should be copied.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>tree</c> is set to <c>NULL</c>.</exception>
			GAL_API
			void GACALL Copy(const GaTreeBase* tree);

			/// <summary><c>Clear</c> method removes all the nodes from the tree and frees used resource.
			///
			/// This method is not thread-safe.</summary>
			GAL_API
			void GACALL Clear();

			/// <summary><c>GetRandomNode</c> method selects random node from the tree and returns pointer to it.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="excludeRoot">if this paramenter is set to <c>true</c> root node will neot be considered during selection.</param>
			/// <param name="skip">pointer to node which also should be excluded as well as its descendants. This parameter can be <c>NULL</c>.</param>
			/// <returns>Method returns pointer to randomly selected onde of the tree.</returns>
			GAL_API
			GaTreeNodeBase* GACALL GetRandomNode(bool excludeRoot = false,
				GaTreeNodeBase* skip = NULL);

			/// <summary><c>Compare</c> method compares data stored in the two trees or their topology.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="compareType">type of tree comparison.</param>
			/// <param name="tree">tree against which the comparison is performed.</param>
			/// <returns>Method returns <c>true</c> if the lists contain same data or have same topology.</returns>
			inline bool GACALL Compare(const GaTreeBase* tree,
				GaTreeCompare compareType) const { return compareType == GATC_TOPOLOGY ? CompareNodesTopology( _root, tree->_root ) : CompareNodes( _root, tree->_root ); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to root node of the tree.</returns>
			inline GaTreeNodeBase* GACALL GetRoot() { return _root; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to root node of the tree.</returns>
			inline const GaTreeNodeBase* GACALL GetRoot() const { return _root; }

			/// <summary><c>GetCount</c> method calculates and returns current number of nodes in the tree.
			///
			/// This method is not thread-safe.</summary>
			/// <returns>Method returns number of nodes in the tree.</returns>
			inline int GACALL GetCount() const
			{
				// nodes were added or removed since the last counting
				if( _modified )
				{
					// recount nodes and clear modified flag
					_count = _root ? _root->GetCount() : 0;
					_modified = false;
				}

				return _count;
			}

			/// <summary><c>operator =</c> removes all nodes currently in the tree and copies the nodes from the specified tree.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="rhs">tree that should be copied.</param>
			/// <returns>Operator returns reference to <c>this</c> object.</returns>
			inline GaTreeBase& GACALL operator =(const GaTreeBase& rhs)
			{
				Copy( &rhs );
				return *this;
			}

			/// <summary><c>operator ==</c> compares data stored in the two trees.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="rhs">tree against which the comparison is performed.</param>
			/// <returns>Operator returns <c>true</c> if the trees contain same data and have same topology.</returns>
			inline bool GACALL operator ==(const GaTreeBase& rhs) const { return Compare( &rhs, GATC_DATA ); }

			/// <summary><c>operator ==</c> compares data stored in the two trees.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="rhs">tree against which the comparison is performed.</param>
			/// <returns>Operator returns <c>true</c> if the trees do not contain same data or have different topology.</returns>
			inline bool GACALL operator !=(const GaTreeBase& rhs) const { return !Compare( &rhs, GATC_DATA ); }

		protected:

			/// <summary><c>CompareNodesTopology</c> method compares topology of two subtrees.</summary>
			/// <param name="n1">root of the first subtree.</param>
			/// <param name="n2">root of the second subtree.</param>
			/// <returns>Method returns <c>true</c> if both subtress have same topology.</returns>
			GAL_API
			bool GACALL CompareNodesTopology(const GaTreeNodeBase* n1,
				const GaTreeNodeBase* n2) const;

			/// <summary><c>CompareNodes</c> method compares topology and data stored in two subtrees.</summary>
			/// <param name="n1">root of the first subtree.</param>
			/// <param name="n2">root of the second subtree.</param>
			/// <returns>Method returns <c>true</c> if both subtress contains same data and have same topology.</returns>
			GAL_API
			bool GACALL CompareNodes(const GaTreeNodeBase* n1,
				const GaTreeNodeBase* n2) const;

			/// <summary><c>GetNode</c> method gets Nth node in preorder tree traversal sequence.</summary>
			/// <param name="node">root node of subtree which should be traversed.</param>
			/// <param name="skipNode">pointer to node which should be skipped during traversal as well as its descendants.</param>
			/// <param name="index">pointer to traversal position counter. When counter reaches 0, traversal stops.</param>
			/// <returns>Method returns pointer to Nth node.</returns>
			GAL_API
			GaTreeNodeBase* GACALL GetNode(GaTreeNodeBase* node,
				GaTreeNodeBase* skipNode,
				int* index);

		};

		template<typename VALUE_TYPE>
		class GaTree;

		/// <summary><c>GaTreeNode</c> represent node of a tree data structure that stores values of specified type.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <typeparam name="VALUE_TYPE">type of stored value.</typeparam>
		template<typename VALUE_TYPE>
		class GaTreeNode : public GaTreeNodeBase
		{

		public:

			/// <summary>Type of stored value.</summary>
			typedef VALUE_TYPE GaValueType;

		protected:

			/// <summary>Value stored in the node.</summary>
			GaValueType _value;

		public:

			/// <summary>This method initializes node with user-defined value.</summary>
			/// <param name="value">value that should be stored in the node.</param>
			GaTreeNode(const GaValueType& value) : _value(value) { }

			/// <summary>This constructor copies whole subtree whose root is provided node.</summary>
			/// <param name="rhs">root node of subtree that should be copied.</param>
			GaTreeNode(const GaTreeNode& rhs) : GaTreeNodeBase(rhs),
				_value(rhs._value) { }

			/// <summary>This method initializes node with default value for of the type.</summary>
			GaTreeNode() : _value() { }

			/// <summary>More details are given in specification of <see cref="GaNode::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaTreeNode* GACALL Clone() const { return new GaTreeNode<GaValueType>( _value );  }

			/// <summary>More details are given in specification of <see cref="GaTreeNodeBase::CopySubtree" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaTreeNodeBase* CopySubtree() const { return new GaTreeNode<GaValueType>( *this ); }

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
					_value = ( (GaTreeNode<GaValueType>*)node )->_value;
					( (GaTreeNode<GaValueType>*)node )->_value = temp;
				}
			}

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

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the parent node.</returns>
			inline GaTreeNode<GaValueType>* GACALL GetParent() { return (GaTreeNode<GaValueType>*)_parent; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the parent node.</returns>
			inline const GaTreeNode<GaValueType>* GACALL GetParent() const { return (const GaTreeNode<GaValueType>*)_parent; }

			/// <summary><c>operator ==</c> compares values stored in the two nodes.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="node">node against whose value comparison is performed.</param>
			/// <returns>Operator returns <c>true</c> if values stored in nodes are equal.</returns>
			virtual bool GACALL operator ==(const GaNodeBase& node) const { return _value == ( (const GaTreeNode<GaValueType>&)node )._value; }

			/// <summary><c>operator !=</c> compares values stored in the two nodes.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="node">node against whose value comparison is performed.</param>
			/// <returns>Operator returns <c>true</c> if values stored in nodes are not equal.</returns>
			virtual bool GACALL operator !=(const GaNodeBase& node) const { return _value != ( (const GaTreeNode<GaValueType>&)node )._value; }

		};

		/// <summary><c>GaTree</c> represent tree data structure that stores values of specified type.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <typeparam name="VALUE_TYPE">type of stored value.</typeparam>
		template<typename VALUE_TYPE>
		class GaTree : public GaTreeBase
		{

		public:

			/// <summary>Type of stored value.</summary>
			typedef VALUE_TYPE GaValueType;

			/// <summary>This constructor initializes empty tree.</summary>
			GaTree() { }

			/// <summary>This is constructor creates copy of provided tree.</summary>
			/// <param name="rhs">tree that should be copied.</param>
			GaTree(const GaTree& rhs) : GaTreeBase(rhs) { }

			using GaTreeBase::Insert;

			/// <summary><c>Insert</c> method inserts new value as child of specified node in the tree.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">value that should be inserted.</param>
			/// <param name="parent">parent of the new node. If this paramenter is <c>NULL</c> new node will be the root of the tree and
			/// current root will be the child of the new node.</param>
			/// <param name="position">position in child list at which the new node is inserted. If this value is negative, new node will be inserted as last child.</param>
			GaTreeNode<GaValueType>* Insert(GaValueType value,
				GaTreeNode<GaValueType>* parent,
				int position = -1)
			{
				// create new node that stores value and add it to the tree
				GaTreeNode<GaValueType>* newNode = new GaTreeNode<GaValueType>( value );
				Insert( newNode, parent, position );

				return newNode;
			}

			/// <summary><c>GetRandomNode</c> method selects random node from the tree and returns pointer to it.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="excludeRoot">if this paramenter is set to <c>true</c> root node will neot be considered during selection.</param>
			/// <param name="skip">pointer to node which also should be excluded as well as its descendants. This parameter can be <c>NULL</c>.</param>
			/// <returns>Method returns pointer to randomly selected onde of the tree.</returns>
			GaTreeNode<GaValueType>* GACALL GetRandomNode(bool excludeRoot = false,
				GaTreeNodeBase* skip = NULL) { return (GaTreeNode<GaValueType>*)GaTreeBase::GetRandomNode( excludeRoot, skip ); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to root node of the tree.</returns>
			inline GaTreeNode<GaValueType>* GACALL GetRoot() { return (GaTreeNode<GaValueType>*)_root; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to root node of the tree.</returns>
			inline const GaTreeNode<GaValueType>* GACALL GetRoot() const { return (const GaTreeNode<GaValueType>*)_root; }

			/// <summary><c>Find</c> method finds the first node in the tree that contains specified value. Search is done in preorder tree traversal.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">value that should be found.</param>
			/// <param name="start">pointer to node who's subtree is searched. If this paramenter is <c>NULL</c> search will start from tree's root node.</param>
			/// <param name="compareStartPosition">if this parameter is set to <c>true</c> search will also look at (sub)tree' root for the value,
			/// otherwise it will be skipped.</param>
			/// <returns>Method returns pointer to the node that contains value or <c>NULL</c> if there is no such a value in the (sub)tree.</returns>
			GaTreeNode<GaValueType>* GACALL Find(GaValueType value,
				GaTreeNode<GaValueType>* start = NULL,
				bool compareStartPosition = true)
			{
				// start from tree's root?
				if( !start )
				{
					// is tree empty?
					if( !_root )
						return NULL;

					start = _root;
				}

				// search the (sub)tree root?
				if( compareStartPosition && start->_value == value )
					return start;

				// search root's children and their subtrees
				for( GaListNodeBase* child = start->_children.GetHead(); child; child = child->GetNext() )
				{
					GaTreeNode<GaValueType>* node = Find( value, (GaTreeNode<GaValueType>*)( ( (GaListNode<GaTreeNodeBase*>*)child )->GetValue() ) );
					if( node )
						return node;
				}

				return NULL;
			}

			/// <summary><c>Find</c> method finds the first node in the tree that contains specified value. Search is done in preorder tree traversal.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">value that should be found.</param>
			/// <param name="start">pointer to node who's subtree is searched. If this paramenter is <c>NULL</c> search will start from tree's root node.</param>
			/// <param name="compareStartPosition">if this parameter is set to <c>true</c> search will also look at (sub)tree' root for the value,
			/// otherwise it will be skipped.</param>
			/// <returns>Method returns pointer to the node that contains value or <c>NULL</c> if there is no such a value in the (sub)tree.</returns>
			const GaTreeNode<GaValueType>* GACALL Find(GaValueType value,
				const GaTreeNode<GaValueType>* start = NULL,
				bool compareStartPosition = true) const
			{
				// start from tree's root?
				if( !start )
				{
					// is tree empty?
					if( !_root )
						return NULL;

					start = _root;
				}

				// search the (sub)tree root?
				if( compareStartPosition && start->_value == value )
					return start;

				// search root's children and their subtrees
				for( const GaListNodeBase* child = start->_children.GetHead(); child; child = child->GetNext() )
				{
					// search child
					const GaTreeNode<GaValueType>* node = Find( value, (const GaTreeNode<GaValueType>*)( ( (const GaListNode<GaTreeNodeBase*>*)child )->GetValue() ) );

					// node found?
					if( node )
						return node;
				}

				return NULL;
			}

			/// <summary><c>ContinueFind</c> method continues previous search of nodes that contains specified value.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">value that should be found.</param>
			/// <param name="start">pointer to the node where the previous search has stopped.</param>
			/// <returns>Method returns pointer to the node that contains value or <c>NULL</c> if there is no such a value in the (sub)tree.</returns>
			GaTreeNode<GaValueType>* GACALL ContinueFind(GaValueType value,
				GaTreeNode<GaValueType>* start)
			{
				// go upwards through the tree
				for( GaTreeNode<GaValueType>* parent = start->GetParent(); parent; parent = parent->GetParent() )
				{
					// continue search from previous position
					for( GaListNode<GaTreeNodeBase*>* level = parent->GetChildren()->Find( start )->GetNext(); level; level = level->GetNext() )
					{
						// try to find value in the subtree
						node = Find( value, (GaTreeNode<GaValueType>*)level->GetValue(), true );

						// value found?
						if( node )
							return node;
					}

					start = parent;
				}

				return NULL;
			}

			/// <summary><c>ContinueFind</c> method continues previous search of nodes that contains specified value.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">value that should be found.</param>
			/// <param name="start">pointer to the node where the previous search has stopped.</param>
			/// <returns>Method returns pointer to the node that contains value or <c>NULL</c> if there is no such a value in the (sub)tree.</returns>
			const GaTreeNode<GaValueType>* GACALL ContinueFind(GaValueType value,
				GaTreeNode<GaValueType>* start) const
			{
				// go upwards through the tree
				for( const GaTreeNode<GaValueType>* parent = start->GetParent(); parent; parent = parent->GetParent() )
				{
					// continue search from previous position
					for( const GaListNode<GaTreeNodeBase*>* level = parent->GetChildren()->Find( start )->GetNext(); level; level = level->GetNext() )
					{
						// try to find value in the subtree
						node = Find( value, (const GaTreeNode<GaValueType>*)level->GetValue(), true );

						// value found?
						if( node )
							return node;
					}

					start = parent;
				}

				return NULL;
			}

		};

	} // Data
} // Common

#endif // __GA_TREE_H__

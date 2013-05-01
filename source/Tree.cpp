
/*! \file Tree.cpp
	\brief This file contains implementation of classes that manage tree data structures.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#include "GlobalRandomGenerator.h"
#include "Tree.h"

namespace Common
{
	namespace Data
	{

		// Copies node and its subtree
		GaTreeNodeBase::GaTreeNodeBase(const GaTreeNodeBase& rhs) : _parent(NULL)
		{
			for( const GaListNode<GaTreeNodeBase*>* child = rhs._children.GetHead(); child; child = child->GetNext() )
				InsertChild( child->GetValue()->CopySubtree() );
		}

		// Deletes node and its children
		GaTreeNodeBase::~GaTreeNodeBase()
		{
			for( GaListNode<GaTreeNodeBase*>* child = _children.GetHead(); child; child = child->GetNext() )
				delete child->GetValue();
		}

		// Attaches child to node
		void GaTreeNodeBase::InsertChild(GaTreeNodeBase* node,
			int position /*= -1*/)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, node != NULL, "node", "Node which is tested must be specified.", "Data" );

			// detach node from its current parent
			node->Detach();

			// attach node to this parent
			position >= 0 ? _children.InsertAt( position, node ) : _children.InsertTail( node );
			node->SetParent( this );
		}

		// Removes node's child
		void GaTreeNodeBase::DetachChild(GaTreeNodeBase* node)
		{
			GaListNode<GaTreeNodeBase*>* position = _children.Find( node );
			GA_ASSERT( Exceptions::GaInvalidOperationException, position != NULL, "Specified node is not child of this node.", "Data" );

			// detach child from this node
			_children.Remove( position );
			node->_parent = NULL;
		}

		// Deletes node's children
		void GaTreeNodeBase::DeleteChildren()
		{
			// free memory used by the node's children
			for( GaListNode<GaTreeNodeBase*>* child = _children.GetHead(); child; child = child->GetNext() )
				delete child->GetValue();

			_children.Clear();
		}

		// Calculates number of number of descendants
		int GaTreeNodeBase::GetCount() const
		{
			int count = 1;

			// sum number of descendants of all childrens nodes recursivly
			for( const GaListNode<GaTreeNodeBase*>* child = _children.GetHead(); child; child = child->GetNext() )
				count += child->GetValue()->GetCount();

			return count;
		}

		// Checks whether the provided node is ancestor of this node
		bool GaTreeNodeBase::IsAncestor(const GaTreeNodeBase* node) const
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, node != NULL, "node", "Node which is tested must be specified.", "Data" );

			if( node != this )
			{
				// go upward through the tree quering parents
				for( const GaTreeNodeBase* current = _parent; current; current = current->_parent )
				{
					if( current == node )
						return true;
				}
			}

			return false;
		}

		// Inserts new node to list
		void GaTreeBase::Insert(GaTreeNodeBase* node,
			GaTreeNodeBase* parent,
			int position/* = -1*/)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, node != NULL, "node", "New node must be specified.", "Data" );

			_modified = true;

			// tree is empty?
			if( !_root )
			{
				// new node is root
				node->Detach();
				_root = node;
			}
			else
			{
				// add new node as root?
				if( !parent )
				{
					// add current root as child of the new node and set it as new root
					node->InsertChild( _root, position );
					_root = node;
				}
				else
					// insert new node at specified position in parent's child list
					parent->InsertChild( node, position );
			}
		}

		// Removes node and its descendants from the list
		void GaTreeBase::Remove(GaTreeNodeBase* node)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, node != NULL, "node", "Node that should be removed must be specified.", "Data" );

			// root should be detached?
			if( _root == node )
				Clear();
			else
			{
				_modified = true;

				// detach node from its parent and delete it
				node->Detach();
				delete node;
			}
		}

		// Removes node's children and their descendants
		void GaTreeBase::RemoveChildren(GaTreeNodeBase* node)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, node != NULL, "node", "Node whose children should be removed must be specified.", "Data" );

			_modified = true;
			node->DeleteChildren();
		}

		// Moves node to another parent
		void GaTreeBase::MoveNode(GaTreeNodeBase* node,
			GaTreeNodeBase* newParent,
			int position/* = -1*/)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, node != NULL, "node", "Node that should be moved must be specified.", "Data" );
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, newParent != NULL, "newParent", "New parent of the moved node must be specified.", "Data" );

			// cannot move node to new parent if they are already related
			if( node->IsDescendant( newParent ) )
				return;

			// detach node from current parent and attach it to the new one
			newParent->InsertChild( node, position );
		}

		// Swaps positions of nodes in the list
		void GaTreeBase::SwapNodes(GaTreeNodeBase* node1,
			GaTreeNodeBase* node2)
		{
			GaTreeNodeBase* parent1 = node1->GetParent();
			GaTreeNodeBase* parent2 = node2->GetParent();

			// get positions of nodes in their parent's children list
			int position1 = parent1->GetChildren()->GetPosition( node1 );
			int position2 = parent2->GetChildren()->GetPosition( node2 );

			// move the first node
			MoveNode( node1, parent2, position2 );

			try
			{
				// move the second node
				MoveNode( node2, parent1, position1 );
			}
			catch( ... )
			{
				// if moveing second node fails - place back the first node
				MoveNode( node1, parent1, position1 );
				throw;
			}
		}

		// Detaches node from the tree
		void GaTreeBase::DetachNode(GaTreeNodeBase* node)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, node != NULL, "node", "Node that should be detached from the tree must be specified.", "Data" );

			if( node == _root )
			{
				// mark tree as empty
				_root = NULL;
				_count = 0;
				_modified = false;
			}
			else
			{
				// detach node from it's parent
				_modified = true;
				node->Detach();
			}
		}

		// Copies source tree
		void GaTreeBase::Copy(const GaTreeBase* tree)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, tree != NULL, "tree", "Source tree must be specified.", "Data" );

			// removes previous content of the tree
			Clear();

			_count = tree->_count;
			_modified = tree->_modified;

			// copies node from source tree
			if( tree->_root )
				_root = tree->_root->CopySubtree();
		}

		// Clears content of the tree and frees memory
		void GaTreeBase::Clear()
		{
			delete _root;

			_root = NULL;
			_count = 0;
			_modified = false;
		}

		// Returns pointer to randomly selected tree node
		GaTreeNodeBase* GaTreeBase::GetRandomNode(bool excludeRoot/* = false*/,
				GaTreeNodeBase* skipNode/* = NULL*/)
		{
			if( !_root )
				return NULL;

			int skipCount = 0;
			if( skipNode )
				skipCount = skipNode->GetCount();

			if( !excludeRoot )
			{
				// get random position in preorder tree traversal sequence and get node at the position
				int index = GaGlobalRandomIntegerGenerator->Generate( GetCount() - skipCount - 1 );
				return GetNode( _root, skipNode, &index );
			}
			else
			{
				// get random position in preorder tree traversal sequence get node at the position
				int index = GaGlobalRandomIntegerGenerator->Generate( 0, GetCount() - skipCount - 2 );

				for( GaListNode<GaTreeNodeBase*>* child = _root->GetChildren()->GetHead(); child; child = child->GetNext() )
				{
					GaTreeNodeBase* childNode = child->GetValue();
					GaTreeNodeBase* returnNode = GetNode( childNode, skipNode, &index );

					// required node reached?
					if( returnNode )
						return returnNode;
				}
			}

			return NULL;
		}

		// Compare topologies of the two subtrees
		bool GaTreeBase::CompareNodesTopology(const GaTreeNodeBase* n1,
			const GaTreeNodeBase* n2) const
		{
			// if two nods has different number of children, their topologies are different
			if( n1->GetChildren()->GetCount() != n2->GetChildren()->GetCount() )
				return false;

			// compare topologies of all children's subtrees
			for( const GaListNode<GaTreeNodeBase*> *c1 = n1->GetChildren()->GetHead(), *c2 = n2->GetChildren()->GetHead(); c1 ; c1 = c1->GetNext(), c2 = c2->GetNext() )
			{
				if( !CompareNodesTopology( c1->GetValue(), c2->GetValue() ) )
					return false;
			}

			return true;
		}

		// Compare topologies of the two subtrees and data they contains
		bool GaTreeBase::CompareNodes(const GaTreeNodeBase* n1,
			const GaTreeNodeBase* n2) const
		{
			// if two nods has different number of children, their topologies are different
			if( *n1 != *n2 || n1->GetChildren()->GetCount() != n2->GetChildren()->GetCount() )
				return false;

			// compare topologies of all children's subtrees and data they contain
			for( const GaListNode<GaTreeNodeBase*> *c1 = n1->GetChildren()->GetHead(), *c2 = n2->GetChildren()->GetHead(); c1 ; c1 = c1->GetNext(), c2 = c2->GetNext() )
			{
				if( !CompareNodes( c1->GetValue(), c2->GetValue() ) )
					return false;
			}

			return true;
		}

		// Get Nth tree node in preorder tree traversal sequence
		GaTreeNodeBase* GaTreeBase::GetNode(GaTreeNodeBase* node,
			GaTreeNodeBase* skipNode,
			int* index)
		{
			// do not traverse subtree that should be skipped
			if( node == skipNode )
				return NULL;

			// requreid node found?
			if( !*index )
				return node;

			// on less element to the required position
			( *index )--;

			// traverse subtrees of the children nodes
			for( GaListNode<GaTreeNodeBase*>* child = node->GetChildren()->GetHead(); child; child = child->GetNext() )
			{
				GaTreeNodeBase* childNode = child->GetValue();

				// traverse subtree
				GaTreeNodeBase* returnNode = GetNode( childNode, skipNode, index );

				// required node reached?
				if( returnNode )
					return returnNode;
			}

			return NULL;
		}

	} // Data
} // Common

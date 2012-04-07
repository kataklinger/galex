
/*! \file List.cpp
	\brief This file contains implementation of classes that manage doubly linked lists.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#include "List.h"

namespace Common
{
	namespace Data
	{

		// Adds node as the first node of the list
		void GaListBase::InsertHead(GaListNodeBase* node)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, node != NULL, "node", "New node must be specified.", "Data" );

			// is the list empty?
			if( _head )
			{
				// connect new node to the list
				node->_next = _head;
				_head->_previous = node;
				node->_previous = NULL;

				// new head is this node
				_head = _head->_previous;
			}
			else
			{
				// node has no neighbour nodes
				node->_next = NULL;
				node->_previous = NULL;

				// new node is both head and tail of the list
				_tail = _head = node;
			}

			// increment node count
			_count++;
		}

		// Adds node as the last node of the list
		void GaListBase::InsertTail(GaListNodeBase* node)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, node != NULL, "node", "New node must be specified.", "Data" );

			if( _head )
			{
				// connect new node to the list
				node->_previous = _tail;
				_tail->_next = node;
				node->_next = NULL;

				// new tail is this node
				_tail = _tail->_next;
			}
			else
			{
				// node has no neighbour nodes
				node->_next = NULL;
				node->_previous = NULL;

				// new node is both head and tail of the list
				_tail = _head = node;
			}

			// increment node count
			_count++;
		}

		// Adds node at specified position in the list
		void GaListBase::InsertAt(int position,
			GaListNodeBase* node)
		{
			// should be inserted before head?
			if( position <= 0 )
				InsertHead( node );
			// should be inserted after tail?
			else if( position >= _count )
				InsertTail( node );
			else
				// find node at specified position and new node before it
				InsertBefore( GetAt( position ), node );
		}

		// Adds new node before specified node
		void GaListBase::InsertBefore(GaListNodeBase* baseNode,
			GaListNodeBase* node)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, node != NULL, "node", "New node must be specified.", "Data" );

			// should the new node be a head?
			if( !baseNode || baseNode == _head )
				InsertHead( node );
			else
			{
				// connect new node to the list

				node->_next = baseNode;
				node->_previous = baseNode->_previous;

				baseNode->_previous->_next = node;
				baseNode->_previous = node;
			}

			_count++;
		}

		// Adds new node after specified node
		void GaListBase::InsertAfter(GaListNodeBase* baseNode,
			GaListNodeBase* node)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, node != NULL, "node", "New node must be specified.", "Data" );

			// should the new node be a tail?
			if( !baseNode || baseNode == _tail )
				InsertTail( node );
			else
			{
				// connect new node to the list

				node->_next = baseNode->_next;
				node->_previous = baseNode;

				baseNode->_next->_previous = node;
				baseNode->_next = node;
			}

			_count++;
		}

		// Merges two lists
		void GaListBase::MergeLists(GaListBase* list)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, list != NULL, "list", "List that should be merged with this list must be specified.", "Data" );

			// is the destination list empty?
			if( !_tail )
			{
				_head = list->_head;
				_tail = list->_tail;
				_count = list->_count;
			}
			else if( list->_head )
			{
				// connect head node of the source list to the tail of destination node
				_tail->_next = list->_head;
				list->_head->_previous = _tail;

				// tail of source list is now tail of destination list
				_tail = list->_tail;

				// update node count of destination 
				_count += list->_count;
			}

			// clear source list
			list->_head = NULL;
			list->_tail = NULL;
			list->_count = 0;
		}

		// Swap content of the lists
		void GaListBase::SwapLists(GaListBase* list)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, list != NULL, "list", "The second list of the swapping operation must be specified.", "Data" );

			GaListNodeBase* tempNode = NULL;

			// swap heads
			tempNode = _head;
			_head = list->_head;
			list->_head = tempNode;

			// swap tails
			tempNode = _tail;
			_tail = list->_tail;
			list->_tail = tempNode;

			// swap node counts
			int tempCount = _count;
			_count = list->_count;
			list->_count = tempCount;
		}

		// Removes group of nodes at specified position from the list
		void GaListBase::Remove(GaListNodeBase* node,
			int count/* = 1*/)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, node != NULL, "node", "The first node that should be removed must be specified.", "Data" );
			GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, count > 0, "count", "Count of nodes that should be removed must be greater then 0.", "Data" );

			if( node == _head )
			{
				// detach and delete the nodes
				while( count-- && _count )
					delete DetachNode( _head );
			}
			else if( node == _tail )
			{
				// detach and delete the nodes
				while( count-- && _count )
					delete DetachNode( _tail );
			}
			else
			{
				// detach and delete the nodes
				for( GaListNodeBase* next = node; node && count; node = next, count-- )
				{
					next = node->_next;
					delete DetachNode( node );
				}
			}
		}

		// Moves node to another position in the list
		void GaListBase::Move(GaListNodeBase* node,
			int newPosition)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, node != NULL, "node", "The first node that should be removed must be specified.", "Data" );

			// should the node be moved at the end of the list
			bool asLast = newPosition >= _count;

			// find the node before which the specified node should be moved
			GaListNodeBase* base = GetAt( newPosition );
			if( base == node )
				return;

			// detach node that should be moved
			GaListNodeBase* previous = DetachNode( node );

			try
			{
				// insert detached node to new position
				if( asLast )
					InsertAfter( base, node );
				else
					InsertBefore( base, node );
			}
			catch( ... )
			{
				// if move cannot be completed try to restore node to its previous position

				if( previous )
					InsertBefore( previous, node );
				else
					InsertHead( node );

				throw;
			}
		}

		// Copies nodes from another list
		void GaListBase::Copy(const GaListBase* list)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, list != NULL, "list", "Source list must be specified.", "Data" );

			// removes previous content of the list
			Clear();

			// copy each node of the source list and insert those copies into destination list
			for( const GaListNodeBase* node = list->_head; node; node = node->_next )
				InsertTail( (GaListNodeBase*)node->Clone() );
		}

		// Removes all nodes from the list
		void GaListBase::Clear()
		{
			// delete nodes of the list
			for( GaListNodeBase* current = _head; _head; current = _head )
			{
				_head = _head->_next;
				_count--;

				delete current;
			}

			// set list as empty
			_tail = NULL;
		}

		// Finds position of node in the list
		int GaListBase::GetNodePosition(const GaListNodeBase* node) const
		{
			// iterate through the list to find the node
			int position = 0;
			for( const GaListNodeBase* find = _head; find; find = find->_next, position++ )
			{
				// is the current the one which is queried?
				if( find == node )
					return position;
			}

			return -1;
		}

		// Returns node at specified losition in the list
		GaListNodeBase* GaListBase::GetAt(int position)
		{
			GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, position >= 0 && position < _count, "position", "Specified position is out of range.", "Data" );

			// move to node at specified position
			GaListNodeBase* node = _head;
			while( position )
			{
				node = node->_next;
				position--;
			}

			return node;
		}

		// Returns node at specified losition in the list
		const GaListNodeBase* GaListBase::GetAt(int position) const
		{
			GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, position >= 0 && position < _count, "position", "Specified position is out of range.", "Data" );

			// move to node at specified position
			const GaListNodeBase* node = _head;
			while( position )
			{
				node = node->_next;
				position--;
			}

			return node;
		}

		// Compares content of the two list
		bool GaListBase::Compare(const GaListBase* list) const
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, list != NULL, "list", "List that should be compared with this list must be specified.", "Data" );

			// if list does not have same number of nodes they cannot be equal
			if( _count != list->_count )
				return false;

			// compare each pair of nodes
			for( const GaListNodeBase *n1 = _head, *n2 = list->_head; n1 ; n1 = n1->_next, n2 = n2->_next )
			{
				// compares data stored in nodes
				if( *n1 != *n2 )
					return false;
			}

			return true;
		}

		// Detaches node from the list and upsates node count
		GaListNodeBase* GaListBase::DetachNode(GaListNodeBase* node)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, node != NULL, "node", "Node that should be detached from the list must be specified.", "Data" );

			// if head is detached
			if( node == _head )
			{
				// find a new head
				_head = _head->_next;

				// this was the last node in the list
				if( !_head )
					// list is empty so there's no tail either
					_tail = NULL;
				else
					// disconnect node from new head
					_head->_previous = NULL;
			}
			// if tail is detached
			else if( node == _tail )
			{
				// find a new tile
				_tail = _tail->_previous;

				// this was the last node in the list
				if( !_tail )
					// list is empty so there's no head either
					_head = NULL;
				else
					// disconnect node from new tail
					_tail->_next = NULL;
			}
			else
			{
				// detach node and recconect its neighbours
				node->_previous->_next = node->_next;
				node->_next->_previous = node->_previous;
			}

			// mark node as unconnected
			node->_next = node->_previous = NULL;

			// decrement nod count of the list
			_count--;

			return node;
		}

	} // Data
} // Common

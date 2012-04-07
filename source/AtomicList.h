
/*! \file AtomicList.h
	\brief This file contains classes and datatypes that implements atomic singly linked list.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#ifndef __GA_ATOMIC_LIST_H__
#define __GA_ATOMIC_LIST_H__

#include "AtomicOperations.h"
#include "Exceptions.h"

namespace Common
{
	namespace Data
	{

		/// <summary><c>GA_DEFINE_ATOM_NODE</c> macro inserts members to a class needed so it can be handled by atomic lists and default next accessor.</summary>
		/// <param name="CLASS_NAME">name of the class to which the members should be inserted.</param>
		#define GA_DEFINE_ATOM_NODE(CLASS_NAME)	private: CLASS_NAME* _nextAtomicNode; \
												public: inline CLASS_NAME*& GetNextAtomicNode() { return _nextAtomicNode; }

		/// <summary><c>GaDefaultNextAccessor</c> template provides default mechanism that extracts pointer to next node in the atomic list.
		/// Node class must use <c>GA_DEFINE_ATOM_NODE</c> macro.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this object are stateless all public method are thread-safe.</summary>
		/// <param name="TYPE">type of node.</param>
		template<typename TYPE>
		class GaDefaultNextAccessor
		{

		public:

			/// <summary><c>operator ()</c> extracts pointer to next node.
			///
			/// This method is thread-safe.</summary>
			/// <param name="node">node whose next node is queried.</param>
			/// <returns>Method returns pointer to next node.</returns>
			inline TYPE*& GACALL operator ()(TYPE& node) { return node.GetNextAtomicNode(); }

		};

		/// <summary><c>GaAtomicList</c> template implements lock-free and thread-safe queue for storing any type of elements.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public method are thread-safe.</summary>
		/// <param name="TYPE">type of queue node.</param>
		/// <param name="NEXT_ACCESSOR">mechanism for extracing pointer to next node.</param>
		template<typename TYPE,
			typename NEXT_ACCESSOR = GaDefaultNextAccessor<TYPE> >
		class GaAtomicList
		{

		public:

			/// <summary>Type of queue node.</summary>
			typedef TYPE GaType;

			/// <summary>Mechanism for extracing pointer to next node.</summary>
			typedef NEXT_ACCESSOR GaNextAccessor;

		private:

			/// <summary>Defines datatype used for storing element count and unique tag that identifies changes made to queue.</summary>
			template<int SIZE> struct GaTag { };

			/// <summary>Defines datatype used for storing element count and unique tag that identifies changes made to queue on 32-bit systems.</summary>
			template<> struct GaTag<4> { typedef unsigned long GaType; };

			/// <summary>Defines datatype used for storing element count and unique tag that identifies changes made to queue on 64-bit systems.</summary>
			template<> struct GaTag<8> { typedef unsigned long long GaType; };

			/// <summary>Datatype used for storing element count and unique tag that identifies changes made to queue depending on current compiler settings.</summary>
			typedef typename GaTag<sizeof( void* )>::GaType GaHeadTag;

			/// <summary>Bitmask for extracting element count from queue head tag.</summary>
			static const int GaMask = ( 1 << ( sizeof( GaHeadTag ) * 4 ) ) - 1;

			/// <summary><c>GaHead</c> struct represents header for atomic queue.</summary>
			struct GaHead
			{

				/// <summary>Stores element count and unique tag that identifies changes made to queue.</summary>
				GaHeadTag _tag;

				/// <summary>Pointer to the first element in the queue.</summary>
				GaType* _first;

				/// <summary>Initializes queue head.</summary>
				GaHead() : _tag(0),
					_first(NULL) { }

			};

			/// <summary>Queue head.</summary>
			GaHead _head;

			/// <summary>Mechanism for extracing pointer to next node.</summary>
			GaNextAccessor _nextAccess;

		public:

			/// <summary>This constructor initializes empty queue.</summary>
			/// <param name="nextAccessor">mechanism for extracing pointer to next node.</param>
			GaAtomicList(const GaNextAccessor& nextAccessor = GaNextAccessor()) : _nextAccess(nextAccessor) { }

			/// <summary><c>Push</c> method puts specified element at the begining of the queue.
			///
			/// This method is thread-safe.</summary>
			/// <param name="entry">new node that should be inserted.</param>
			void Push(GaType* entry)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, entry != NULL, "entry", "New entry that should be inserted must be specified.", "Data" );

				GaHead newHead, oldHead;
				newHead._first = entry;

				do
				{
					oldHead = _head;

					// link new element
					_nextAccess( *entry ) = oldHead._first;

					// new head unique tag and increment count
					newHead._tag = oldHead._tag + GaMask + 2;

					// try to replacing head atomically
				} while( !Threading::GaCmpXchg2<GaHead>::Op( &_head, &oldHead, newHead ) );
			}

			/// <summary><c>Pop</c> method removes the first element in the queue it there is any.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Mehtod returns pointer to removed element or <c>NULL</c> if the queue is empty.</returns>
			GaType* Pop()
			{
				GaHead newHead, oldHead;

				do
				{
					// queue is empty?
					if( !_head._first )
						return NULL;

					oldHead = _head;

					// move head to next element
					newHead._first = _nextAccess( *oldHead._first );

					// decrement count
					newHead._tag = oldHead._tag - 1;

					// try to replacing head atomically
				} while( !Threading::GaCmpXchg2<GaHead>::Op( &_head, &oldHead, newHead ) );

				return oldHead._first;
			}

			/// <summary><c>Clear</c> method detaches all elements in the queue.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Mehtod returns pointer to the first element that was in the queue in case additional clean-up is needed.</returns>
			GaType* Clear()
			{
				GaHead newHead, oldHead;

				do
				{
					// queue is empty?
					if( !_head._first )
						return NULL;

					oldHead = _head;

					// restart head to represent empty queue
					newHead._first = NULL;
					newHead._tag = oldHead._tag & ~GaMask;

					// try to replacing head atomically
				} while( !Threading::GaCmpXchg2<GaHead>::Op( &_head, &oldHead, newHead ) );

				return (GaType*)oldHead._first;
			}

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns pointer to the first element in the queue or <c>NULL</c> if the queue is empty.</returns>
			GaType* GetHead() { return _head._first; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns number of items in the queue.</returns>
			inline int GetCount() const { return (int)( _head._tag & GaMask ); }

			/// <summary><c>GetNext</c> method queries the next element of the specified node.
			///
			/// This method is thread-safe.</summary>
			/// <param name="node">node whose next element is queried.</param>
			/// <returns>Method returns pointe to the next node, or <c>NULL</c> if it does not exist.</returns>
			inline GaType* GetNext(GaType* node) { return _nextAccess( *node ); }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns mechanism for extracing pointer to next node.</returns>
			inline GaNextAccessor& GetNextAccessor() { return _nextAccess; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns mechanism for extracing pointer to next node.</returns>
			inline const GaNextAccessor& GetNextAccessor() const { return _nextAccess; }

		};

	} // Data
} // Common

#endif // __GA_ATOMIC_LIST_H__

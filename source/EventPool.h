
/*! \file EventPool.h
    \brief This file conatins classes that implements event pool.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_EVENT_POOL_H__
#define __GA_EVENT_POOL_H__

#include "ObjectPool.h"

namespace Common
{
	namespace Threading
	{

		/// <summary><c>GaEventPool</c> class manages pool of event objects.
		/// 
		/// This class has no built-in synchronization objects,
		/// but all public methods are thread-safe except <c>MakeInstance</c> and <c>FreeInstance</c>.</summary>
		class GaEventPool
		{

		private:

			/// <summary><c>GaCreateEvent</c> class implements event creation policy for object pool used by the event pool.</summary>
			class GaCreateEvent
			{

			private:

				/// <summary>Type of events that will be created by this creatin object.</summary>
				GaEvent::GaEventType _type;

			public:

				/// <summary>Initilizaes creation object with type of events it will create.</summary>
				/// <param name="type">type of events that will be created.</param>
				GaCreateEvent(GaEvent::GaEventType type) : _type(type) { }

				/// <summary><c>operator()</c> creates event object of specified type.
				///
				/// This operator is thread-safe.</summary>
				/// <returns>Returns pointer to newly created object.</returns>
				virtual GaEvent* GACALL operator() () const { return new GaEvent( _type ); }

			};

			/// <summary>Event deletion policy for object pool used by the event pool.</summary>
			typedef Memory::GaPoolObjectDelete<GaEvent> GaDeleteEvent;

			/// <summary>Event initialization policy for object pool used by the event pool.</summary>
			typedef Memory::GaPoolObjectInit<GaEvent> GaInitEvent;

			/// <summary><c>GaCleanEvent</c> class implements event cleanup policy for object pool used by the event pool.</summary>
			class GaCleanEvent
			{

			public:

				/// <summary><c>operator()</c> restarts specified event object.
				///
				/// This operator is thread-safe.</summary>
				/// <param name="object">pointer event object which should be restarted.</param>
				virtual void GACALL operator ()(GaEvent* object) const { object->Reset(); }

			};

			/// <summary>Type of object pool that stores event objects.</summary>
			typedef Memory::GaObjectPool<GaEvent, GaCreateEvent, GaDeleteEvent, GaInitEvent, GaCleanEvent> GaEventObjectPool;

			/// <summary>Global instance of event object pool.</summary>
			GAL_API
			static GaEventPool* _instance;

		public:

			/// <summary><c>GetInstance</c> method return pointer to global instance of event object pool.</summary>
			/// <returns>Method returns pointer to global instance of event object pool.</returns>
			static inline GaEventPool* GACALL GetInstance() { return _instance; }

			/// <summary><c>MakeInstance</c> method makes global instance of event object pool if it is not exist.</summary>
			/// <param name="autoMaxSize">size of pool of auto-reset events.</param>
			/// <param name="manualMaxSize">size of pool of manual-reset events.</param>
			/// <exception cref="GaInvalidOperationException" />Thrown if global instance already exists.</exception>
			static void GACALL MakeInstance(int autoMaxSize,
				int manualMaxSize)
			{
				GA_ASSERT( Exceptions::GaInvalidOperationException, _instance == NULL, "Global instance already exists.", "Threading" );
				_instance = new GaEventPool( autoMaxSize, manualMaxSize );
			}

			/// <summary><c>FreeInstance</c> method deletes global instance of event object pool and frees used resources.</summary>
			/// <exception cref="GaInvalidOperationException" />Thrown if global instance does not exist.</exception>
			static void GACALL FreeInstance()
			{
				GA_ASSERT( Exceptions::GaInvalidOperationException, _instance != NULL, "Global instance does not exists.", "Threading" );
				delete _instance;
			}

		private:

			/// <summary>Object pools used to store event objects.</summary>
			GaEventObjectPool* _pools[ 2 ];

		public:

			/// <summary>Initializes event pool.</summary>
			/// <param name="autoMaxSize">size of pool of auto-reset events.</param>
			/// <param name="manualMaxSize">size of pool of manual-reset events.</param>
			GaEventPool(int autoMaxSize,
				int manualMaxSize)
			{
				_pools[ GaEvent::GAET_MANUAL ] = new GaEventObjectPool( manualMaxSize, GaCreateEvent( GaEvent::GAET_MANUAL ) );
				_pools[ GaEvent::GAET_AUTO ] = new GaEventObjectPool( autoMaxSize, GaCreateEvent( GaEvent::GAET_AUTO ) );
			}

			/// <summary>Frees resources used by the pool.</summary>
			~GaEventPool()
			{
				delete _pools[ GaEvent::GAET_MANUAL ];
				delete _pools[ GaEvent::GAET_AUTO ];
			}

			/// <summary><c>GetEvent</c> method returns pointer to event object retrived from the pool. If the pool is empty new event object is made.
			///
			/// This method is thread-safe.</summary>
			/// <param name="type">required type of event.</param>
			/// <returns>Method returns pointer to event object retrived from the pool.</returns>
			inline GaEvent* GACALL GetEvent(GaEvent::GaEventType type) { return _pools[ type ]->AcquireObject(); }

			/// <summary><c>GetEventWithAutoPtr</c> method returns auto pointer to event object retrived from the pool. If the pool is empty new event object is made.
			/// Auto pointer returns event object to pool automatically when it does not reference the object any longer.
			///
			/// This method is thread-safe.</summary>
			/// <param name="type">required type of event.</param>
			/// <returns>Method returns auto pointer to event object retrived from the pool.</returns>
			inline Memory::GaAutoPtr<GaEvent> GACALL GetEventWithAutoPtr(GaEvent::GaEventType type) { return _pools[ type ]->AcquireObjectWithAutoPtr(); }

			/// <summary><c>PutEvent</c> method returns event object to the pool. If the pool is full, event object is destroyed.
			///
			/// This method is thread-safe.</summary>
			/// <param name="e">event object which should be returned to the pool.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if the <c>e</c> is set to <c>NULL</c>.</exception>
			inline void GACALL PutEvent(GaEvent* e)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, e != NULL, "e", "Event object must be specified.", "Threading" );
				_pools[ e->GetType() ]->ReleaseObject( e );
			}

			/// <summary><c>SetSize</c> method sets size of object pool.
			///
			/// This method is thread-safe.</summary>
			/// <param name="size">new size of the pool.</param>
			/// <param name="type">type of the pool.</param>
			inline void GACALL SetSize(int size,
				GaEvent::GaEventType type) { _pools[ type ]->SetSize( size ); }

			/// <summary><c>GetSize</c> method returns size of the pool.
			/// 
			/// This method is thread-safe.</summary>
			/// <param name="type">type of the pool.</param>
			inline int GACALL GetSize(GaEvent::GaEventType type) const { return _pools[ type ]->GetSize(); }

		};

	} // Threading
} // Common

#endif // __GA_EVENT_POOL_H__
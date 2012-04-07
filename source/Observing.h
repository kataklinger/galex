
/*! \file Observing.h
    \brief This file contains declarations of classes for event handling.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#ifndef __GA_OBSERVING_H__
#define __GA_OBSERVING_H__

#include "HashMap.h"
#include "List.h"

namespace Common
{

	/// <summary>Contains classes that provides observing services.</summary>
	namespace Observing
	{

		/// <summary><c>GaEventData</c> is a base class for data that are passed to an event handler.</summary>
		class GaEventData
		{

		public:

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaEventData() { }

		};

		class GaEventManager;

		/// <summary><c>GaEventHandler</c> is base class for event handlers. Each handler can be bound to multiple event managers and can events with different event IDs.</summary>
		class GaEventHandler
		{

		private:

			/// <summary>Type of hash table that stores information about event managers to which hander is registered.</summary>
			typedef STLEXT::hash_map<GaEventManager*, int> GaManagersTable;

			/// <summary>Managers to which this hander is registered.</summary>
			GaManagersTable _managers;

		public:

			/// <summary>Initializes event handler.</summary>
			GaEventHandler() { }

			/// <summary>Unbinds handler from all event managers.</summary>
			virtual ~GaEventHandler() { UnbindFromAllManagers(); }

			/// <summary><c>Execute</c> should handle raised event. It is called by event manager when event is raised.</summary>
			/// <param name="eventID">ID of the event.</param>
			/// <param name="data">additional event data.</param>
			virtual void GACALL Execute(int eventID,
				GaEventData& data) = 0;

			/// <summary><c>UnbindFromAllManagers</c> method unregister event handler from all event managers.
			///
			/// This mehtod is not thread-safe.</summary>
			GAL_API
			void GACALL UnbindFromAllManagers();

			/// <summary><c>Bind</c> method updates table of bound managers, after it has been register to event manager.
			///
			///  This method is not thread-safe.</summary>
			/// <param name="manager">manager to which the handler is unregistered.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>manager</c> is set to <c>NULL</c>.</exception>
			inline void GACALL Bind(GaEventManager* manager)
			{ 
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, manager != NULL, "manager", "Event manager must be specified.", "Observing" );
				_managers[ manager ]++;
			}

			/// <summary><c>Unbind</c> method updates table of bound managers, after it has been unregister from event manager.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="manager">manager from which the handler is unregistered.</param>
			GAL_API
			void GACALL Unbind(GaEventManager* manager);

			/// <summary>This method is not thread-safe.</summary>
			/// <returns></returns>
			inline bool GACALL IsBound() const { return _managers.size() > 0; }

		};

		/// <summary><c>GaNonmemberEventHandler</c> class represent handler that pass control to a function when the event is raised.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaNonmemberEventHandler : public GaEventHandler
		{

		public:

			/// <summary>Type of pointer to function which is called when handler is executed.</summary>
			typedef void (GACALL *GaFunctionPtr)(int eventID, GaEventData& data);

		private:

			/// <summary>Function which is called when handler is executed.</summary>
			GaFunctionPtr _function;

		public:

			/// <summary>Initializes event handler with underlying function.</summary>
			/// <param name="function">function which should be called when handler is executed.</para>
			GaNonmemberEventHandler(GaFunctionPtr function = NULL) { SetFunction( function ); }

			/// <summary><c>Execute</c> method calls underlying function and pass the parameters.
			/// For more details see specification of <see cref="GaEventHandler::Execute" /> method.
			///
			/// This operator is thread-safe.</summary>
			virtual void GACALL Execute(int eventID,
				GaEventData& data) { _function( eventID, data ); }

			/// <summary><c>SetFunction</c> method sets function that should be called when handler is executed.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="function">function which should be called when handler is executed.</para>
			inline void GACALL SetFunction(GaFunctionPtr function) { _function = function; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to function which is called when handler is executed.</returns>
			inline GaFunctionPtr GACALL GetFunction() const { return _function; }

		};

		/// <summary><c>GaMethodPtrWrapper</c> defines pointer to method of specified object type that can handle events.</summary>
		template<typename OBJECT_TYPE>
		struct GaMethodPtrWrapper
		{

			/// <summary>Pointer to method.</summary>
			typedef void (GACALL OBJECT_TYPE::*GaPtr)(int eventID, GaEventData& data);

		};

		/// <summary><c>GaMethodPtrWrapper</c> defines pointer to <c>const</c> method of specified object type that can handle events
		/// that does not have to change state of an object.</summary>
		template<typename OBJECT_TYPE>
		struct GaMethodPtrWrapper<const OBJECT_TYPE>
		{

			/// <summary>Pointer to method.</summary>
			typedef void (GACALL OBJECT_TYPE::*GaPtr)(int eventID, GaEventData& data) const;

		};

		/// <summary><c>GaMemberEventHandler</c> class represent handler that pass control to a method of an object when the event is raised.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="OBJECT_TYPE">type of object that handles events.</name>
		template<typename OBJECT_TYPE>
		class GaMemberEventHandler : public GaEventHandler
		{

		public:

			/// <summary>Type of object that handles events.</summary>
			typedef OBJECT_TYPE GaObjectType;

			/// <summary>Type of pointer to method of the object that handles events.</summary>
			typedef typename GaMethodPtrWrapper<GaObjectType>::GaPtr GaMethodPtr;

		private:

			/// <summary>Object that handles events.</summary>
			GaObjectType* _object;

			/// <summary>Method of the object that handles events.</summary>
			GaMethodPtr _method;

		public:

			/// <summary>Initializes event handler with object and method that will handle events.</summary>
			/// <param name="object">object that handles events.</param>
			/// <param name="method">method of the object that handles events.</param>
			GaMemberEventHandler(GaObjectType* object = NULL,
				GaMethodPtr method = NULL) { SetMethod( object, method ); }

			/// <summary><c>Execute</c> method calls underlying method of the object and pass the parameters.
			/// For more details see specification of <see cref="GaEventHandler::Execute" /> method.
			///
			/// This operator is thread-safe.</summary>
			virtual void GACALL Execute(int eventID,
				GaEventData& data) { ( _object->*_method )( eventID, data ); }

			/// <summary><c>SetMethod</c> method sets object and method that will handle events.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="object">object that handles events.</param>
			/// <param name="method">method of the object that handles events.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>method</c> is set, but <c>object</c> is <c>NULL</c>.</exception>
			inline void GACALL SetMethod(GaObjectType* object,
				GaMethodPtr method)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, object != NULL || method == NULL, "method", "Method cannot be set if object is not specified.", "Observing" );

				_object = object;
				_method = method;
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the object that handles events.</returns>
			inline GaObjectType* GACALL GetObject() { return _object; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the object that handles events.</returns>
			inline const GaObjectType* GACALL GetObject() const { return _object; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the method of the object that handles events.</returns>
			inline GaMethodPtr GACALL GetMethod() const { return _method; }

		};

		/// <summary><c>GaEventManager</c> class stores and manages table of registered handlers for events.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaEventManager
		{

		private:

			/// <summary>List of registered handlers for an event.</summary>
			typedef Common::Data::GaList<GaEventHandler*> GaHandlersList;

			/// <summary>Hash table that contains registered event handlers.</summary>
			typedef STLEXT::hash_map<int, GaHandlersList> GaHandlersTable;

			/// <summary>Registered handlers.</summary>
			GaHandlersTable _handlers;

		public:

			/// <summary>Unbinds all handlers from all events.</summary>
			~GaEventManager() { RemoveEventHandlers(); }

			/// <summary><c>RaiseEvent</c> method executes all registered handlers for specified event and pass additional data to them.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="eventID">ID of the event.</param>
			/// <param name="data">additional event data.</param>
			GAL_API
			void GACALL RaiseEvent(int eventID,
				GaEventData& data);

			/// <summary><c>AddEventHandler</c> method register handler to the event with specified ID.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="eventID">ID of the event.</param>
			/// <param name="handler">handler that should be bound.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>handler</c> is set to <c>NULL</c>.</exception>
			GAL_API
			void GACALL AddEventHandler(int eventID,
				GaEventHandler* handler);

			/// <summary><c>RemoveEventHandler</c> unbinds handlers from event with specified ID to which it was registerd.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="eventID">ID of the event.</param>
			/// <param name="handler">handler that should be unbound.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>handler</c> is set to <c>NULL</c>.</exception>
			GAL_API
			void GACALL RemoveEventHandler(int eventID,
				GaEventHandler* handler);

			/// <summary><c>RemoveEventHandler</c> unbinds handlers from all event to which it was registerd.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="handler">handler that should be unbound.</param>
			GAL_API
			void GACALL RemoveEventHandler(GaEventHandler* handler);

			/// <summary><c>RemoveEventHandlers</c> unbinds all handlers from event with specified ID.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="eventID">ID of the event.</param>
			GAL_API
			void GACALL RemoveEventHandlers(int eventID);

			/// <summary><c>RemoveEventHandlers</c> unbinds all handlers from all events.
			///
			/// This method is not thread-safe.</summary>
			GAL_API
			void GACALL RemoveEventHandlers();

		};

	} // Observing
} // Common

#endif // __GA_OBSERVING_H__

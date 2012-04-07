
/*! \file Observing.cpp
    \brief This file contains implementations of classes for event handling.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#include "Observing.h"

namespace Common
{
	namespace Observing
	{

		// Removes event manager from list of registerd managers
		void GaEventHandler::Unbind(GaEventManager* manager)
		{
			GaManagersTable::iterator it = _managers.find( manager );
			if( it != _managers.end() && !--( it->second ) )
				_managers.erase( it );
		}

		// Unregister handler from all event managers
		void GaEventHandler::UnbindFromAllManagers()
		{
			GaManagersTable copy = _managers;

			for( GaManagersTable::iterator it = copy.begin(); it != copy.end(); ++it )
				it->first->RemoveEventHandler( this );
		}

		// Executes all registered handlers for the event
		void GaEventManager::RaiseEvent(int eventID,
			GaEventData& data)
		{
			// get current list of registerd handlers
			GaHandlersTable::iterator ti = _handlers.find( eventID );
			if( ti != _handlers.end() )
			{
				GaHandlersList& list = ti->second;

				// call all handlers
				for( Data::GaListNode<GaEventHandler*>* current = list.GetHead(); current; current = current->GetNext() )
					current->GetValue()->Execute( eventID, data );
			}
		}

		// Register handler to an event
		void GaEventManager::AddEventHandler(int eventID,
			GaEventHandler* handler)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, handler != NULL, "handler", "Handler must be specified.", "Observing" );

			// get current list of registerd handlers
			GaHandlersList& list = _handlers[ eventID ];
			if( !list.Find( handler ) )
			{
				// add handler to list
				list.InsertTail( handler );
				handler->Bind( this );
			}
		}

		// Unbind specified handlers from an event
		void GaEventManager::RemoveEventHandler(int eventID,
			GaEventHandler* handler)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, handler != NULL, "handler", "Handler must be specified.", "Observing" );

			// get current list of registerd handlers
			GaHandlersTable::iterator ti = _handlers.find( eventID );
			if( ti != _handlers.end() )
			{
				GaHandlersList& list = ti->second;

				// remove handler from the list
				handler->Unbind( this );
				list.Remove( handler );
			}
		}

		// Unbind specified handlers from all events
		void GaEventManager::RemoveEventHandler(GaEventHandler* handler)
		{
			for( GaHandlersTable::iterator ti = _handlers.begin(); ti != _handlers.end(); ++ ti )
			{
				GaHandlersList& list = ti->second;

				// was the handler registered for this event?
				Data::GaListNodeBase* li = list.Find( handler );
				if( li )
				{
					// remove handler from the list
					handler->Unbind( this );
					list.Remove( li );
				}
			}
		}

		// Unbind all handlers from specified events
		void GaEventManager::RemoveEventHandlers(int eventID)
		{
			// get list of register events
			GaHandlersTable::iterator ti = _handlers.find( eventID );
			if( ti != _handlers.end() )
			{
				GaHandlersList& list = ti->second;

				// unbind all handlers
				for( Data::GaListNode<GaEventHandler*>* current = list.GetHead(); current; current = current->GetNext() )
					current->GetValue()->Unbind( this );

				list.Clear();
			}
		}

		// Unbind all handlers from all events
		void GaEventManager::RemoveEventHandlers()
		{
			for( GaHandlersTable::iterator ti = _handlers.begin(); ti != _handlers.end(); ++ ti )
			{
				GaHandlersList& list = ti->second;

				// unbind handler from the event
				for( Data::GaListNode<GaEventHandler*>* current = list.GetHead(); current; current = current->GetNext() )
					current->GetValue()->Unbind( this );

				list.Clear();
			}

			_handlers.clear();
		}

	} // Observing
} // Common

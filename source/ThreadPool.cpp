
/*! \file ThreadPool.cpp
	\brief This file contains implementation of classes needed for thread pools.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#include "ThreadPool.h"

namespace Common
{
	namespace Threading
	{

		// Creates thread and initializes worker
		GaWorkerThread::GaWorkerThread(GaWorkerThreadCounter* threadCounter) : _threadCounter(threadCounter),
			_currentWork(NULL),
			_closed(false)
		{
			// set worker wrapper
			_threadStartParams._functionPointer = GaWorkerThread::WorkerWrapper;
			_threadStartParams._functionParameters = this;

			_event = GaEventPool::GetInstance()->GetEventWithAutoPtr( GaEvent::GAET_AUTO );
			_thread = new GaThread( _threadStartParams, true );
		}

		// Frees used resources
		GaWorkerThread::~GaWorkerThread()
		{
			_event = NULL;
			_thread = NULL;
			_exceptionHandler = NULL;

			_threadCounter->WorkerThreadEnd();
		}

		// Entry point for worker thread
		ThreadFunctionReturn GaWorkerThread::WorkerWrapper(GaThread* thread,
			void* params)
		{
			GaWorkerThread* pThis = (GaWorkerThread*)params;

			while( 1 )
			{
				// wait for work
				pThis->_event->Wait();

				// exit if thread is marked for closing
				if( pThis->_closed && !pThis->_currentWork )
					break;

				try
				{
					// execute work item
					( *pThis->_currentWork )();
				}
				catch( Exceptions::GaException& exception )
				{
					if( pThis->_currentWork->GetResults() )
						// store exception in result object
						pThis->_currentWork->GetResults()->SetException( exception );
					else if( !pThis->_exceptionHandler.IsNull() )
						// call generic handler
						pThis->_exceptionHandler->OnException( exception );
					else
						throw;
				}

				// destroy work item object
				pThis->_currentWork = NULL;

				// return thread to the pool
				GaPoolPtr ptr = pThis->_pool;
				pThis->_pool = NULL;
				ptr->ReleaseObject( pThis );
			}

			// destroy thread object before thread exits
			delete pThis;

			return 0;
		}

		// Pointer to global instance of thread pool
		GaThreadPool* GaThreadPool::_instance = NULL;

		// Queues user work item
		GaWorkResults* GaThreadPool::ExecuteWork(GaWorkItem* workItem,
			bool makeResultsObject/* = true*/)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, workItem != NULL, "workItem", "Work item must be specified.", "Threading" );

			GaWorkerThread::GaPoolPtr ptr = _pool;

			// make copy of work item and create object that will store results
			GaWorkItem* copy = workItem->MakeCopy( makeResultsObject );
			GaWorkResults* res = copy->GetResults();

			ptr->AcquireObject()->SetWork( copy, ptr, _exceptionHandler );

			// return pointer to results object of work item or NULL if it is not required
			return res;
		}

	} // Threading
} // Common

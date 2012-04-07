
/*! \file Threading.cpp
	\brief This file contains implementation classes and types used to abstract operating system specific threads control and synchronization.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#include "Threading.h"
#include "EventPool.h"

namespace Common
{
	namespace Threading
	{

		// Pointer to global instance of the event object pool
		GaEventPool* GaEventPool::_instance = NULL;

		// Initialize thread parameters
		GaThread::GaThread(const GaThreadStartInfo& parameters,
			bool started) : _objectDestroyed(NULL)
		{
			GA_LOCK_THIS_OBJECT( lock );

			_parameters = parameters;

			if( started )
			{
				#if defined(GAL_PLATFORM_WIN)
				
				_thread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunctionWraper, this, 0, &_id );
				GA_SYS_ASSERT( _thread != NULL, "Threading" );
				
				#elif defined(GAL_PLATFORM_NIX)
				
				int error = pthread_create( &_thread, NULL, (void* (*)( void* ))ThreadFunctionWraper, this );
				GA_SYS_ASSERT_CODE( error, "Threading" );
				
				_id = _thread;
				
				#endif

				// set status of thread
				_status = _thread ? GATS_RUNNING : GATS_STOPPED;

				return;
			}
			
			// stopped status
			_status = GATS_STOPPED;
			_thread = 0;
		}

		// Frees aquired resources.
		GaThread::~GaThread()
		{
			{
				// set flag to inidicate that object was destoryed by thread itself
				GA_LOCK_THIS_OBJECT( lock );
				if( _objectDestroyed )
					*_objectDestroyed = true;
			}

			// do not abort thread if it tries to delete its own object
			if( GetCurrentThreadId() != _id )
				Abort();
			else
			{
				#if defined(GAL_PLATFORM_WIN)

				CloseHandle( _thread );

				#endif
			}
		}

		// Starts thread if it is not running.
		void GaThread::Start()
		{
			GA_LOCK_THIS_OBJECT( lock );

			switch( _status )
			{

			case GATS_STOPPED:

				#if defined(GAL_PLATFORM_WIN)
				
				_thread = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunctionWraper, this, 0, &_id );
				GA_SYS_ASSERT( _thread != NULL, "Threading" );
				
				#elif defined(GAL_PLATFORM_NIX)
				
				int error = pthread_create( &_thread, NULL, (void* (*)( void* ))ThreadFunctionWraper, this );
				GA_SYS_ASSERT_CODE( error, "Threading" );

				_id = _thread;
				
				#endif

				_status = GATS_RUNNING;
				return;

			case GATS_PAUSED:

				#if defined(GAL_PLATFORM_WIN)
				
				if( ResumeThread( _thread ) == -1 )
					GA_SYS_THROW( "Threading" );
				
				#endif
				
				_status = GATS_RUNNING;
				return;

			case GATS_RUNNING:
				GA_THROW( Exceptions::GaInvalidOperationException, "Thread is already running.", "Threading" );

			}
		}

		// Temproary pause the execution of the thread and saves current state.
		// Returns TRUE if execution is paused.
		void GaThread::Pause()
		{
			#if defined(GAL_PLATFORM_WIN)

			GA_LOCK_THIS_OBJECT( lock );

			GA_ASSERT( Exceptions::GaInvalidOperationException, _thread && _status == GATS_RUNNING, "Thread is not running.", "Threading" );
			GA_ASSERT( Exceptions::GaInvalidOperationException, GetCurrentThreadId() != _id, "Thread is not running.", "Threading" );

			if( SuspendThread( _thread ) == -1 )
				GA_SYS_THROW( "Threading" );

			_status = GATS_PAUSED;

			#elif defined(GAL_PLATFORM_NIX)

			GA_THROW( Exceptions::GaInvalidOperationException, "This operation is not implemented on *nix systems.", "Threading" );

			#endif

		}

		// Stop the execution of the thread and discarge current state.
		void GaThread::Abort()
		{
			GA_LOCK_THIS_OBJECT( lock );

			GA_ASSERT( Exceptions::GaInvalidOperationException, _thread && _status != GATS_STOPPED, "Thread is not running.", "Threading" );

			#if defined(GAL_PLATFORM_WIN)
			
			// is thread terminates itself?
			if( GetCurrentThreadId() == _id )
			{
				if( !CloseHandle( _thread ) )
					GA_SYS_THROW( "Threading" );

				_status = GATS_STOPPED;
				_thread = NULL;
				_id = 0;

				GA_UNLOCK( lock );

				ExitThread( 0 );
			}
			else
			{
				if( TerminateThread( _thread, 0 ) && CloseHandle( _thread ) )
					GA_SYS_THROW( "Threading" );
			}
			
			#elif defined(GAL_PLATFORM_NIX)
			
			if( !pthread_equal( pthread_self(), _thread ) )
			{
				_status = GATS_STOPPED;
				_id = _thread = 0;
				
				GA_UNLOCK( lock );

				pthread_exit( 0 );
			}
			else
			{
				int error = pthread_cancel( _thread );
				GA_SYS_ASSERT_CODE( error, "Threading" );
			}
			
			#endif

			_status = GATS_STOPPED;
			_thread = 0;
			_id = 0;
		}

		// Wait for thread to finish
		void GaThread::Join()
		{
			GA_LOCK_THIS_OBJECT( lock );

			SystemThread old = _thread;

			GA_UNLOCK( lock );

			#if defined(GAL_PLATFORM_WIN)

			if( WaitForSingleObject( old, INFINITE ) == WAIT_FAILED )
				GA_SYS_THROW( "Threading" );

			#elif defined(GAL_PLATFORM_NIX)
			
			int error = pthread_join( old, NULL );
			GA_SYS_ASSERT_CODE( error, "Threading" );

			#endif
		}

		// Wraper for thread function
		ThreadFunctionReturn GaThread::ThreadFunctionWraper(GaThread* thread)
		{
			bool objectDestroyed = false;
			{
				GA_LOCK_OBJECT( lock, thread );
				// set object cleanup inidicator
				thread->_objectDestroyed = &objectDestroyed;
			}

			// execute function specified in thread start block
			ThreadFunctionReturn ret = thread->_parameters._functionPointer( thread, thread->_parameters._functionParameters );

			if( !objectDestroyed )
			{
				// set stopped status
				GA_LOCK_OBJECT( lock, thread );
				thread->_status = GATS_STOPPED;

				// unbind indicator
				thread->_objectDestroyed = 0;
			}

			return ret;
		}

	} // Threading
} // Common

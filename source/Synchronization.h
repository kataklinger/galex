
/*! \file Synchronization.h
	\brief This file contains declaration of datatypes and classes and their implementation that handles thread synchronization.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_SYNCHRONIZATION_H__
#define __GA_SYNCHRONIZATION_H__

#include "AtomicList.h"

#if defined(GAL_PLATFORM_NIX)

	#include <pthread.h>
	#include <sched.h>
	#include <semaphore.h>
	#include <errno.h>

	#if defined(GAL_PLATFORM_MACOS)

		#include <stdio.h>
		#include <string.h>

	#endif

#endif

/// <summary><c>GA_DECLARE_SPINLOCK</c> macro declares spinlock as class member.</summary>
/// <param name="LOCK_NAME">name that will be used for spinlock object.</param>
#define GA_DECLARE_SPINLOCK(LOCK_NAME) mutable Common::Threading::GaSpinlock LOCK_NAME

/// <summary><c>GA_SYNC_CLASS</c> macro inserts members to class which are needed to synchronize access to an object.
/// Synchronization is done by using <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros.</summary>
#define GA_SYNC_CLASS	protected: mutable Common::Threading::GaCriticalSection _synchronizator; \
							public: inline Common::Threading::GaCriticalSection* GACALL GetSynchronizator() const { return &_synchronizator; }

/// <summary>Macro is used to acquire access to critical section protected by synchronization object (<see cref="GaSectionLock" /> and <see cref="GaCriticalSection" />).</summary>
/// <param name="LOCK_NAME">synchronization object.</param>
#define GA_LOCK(LOCK_NAME) ( LOCK_NAME ).Lock()

/// <summary>Macro is used when thread exits critical section and releases access to synchronization object (<see cref="GaSectionLock" /> and <see cref="GaCriticalSection" />).</summary>
/// <param name="LOCK_NAME">synchronization object.</param>
#define GA_UNLOCK(LOCK_NAME) ( LOCK_NAME ).Unlock()

/// <summary>Macro acquires access to an object with built-in synchronizator and prevents concurrent access.
/// It instantiate <see cref="GaSectionLock" /> object with name lock and acquire access to the object, when
/// execution leave the scope in which <c>GA_LOCK_OBJECT</c> is specified, <see cref="GaSectionLock" /> object
/// is destroyed and access to the locked object is released. Unlocking access to the object before leaving
/// scope can be done by calling <c>GA_UNLOCK(lock_name)</c> macro.</summary>
/// <param name="LOCK_NAME">name of <c>GaSectionLock</c> object.</param>
/// <param name="OBJECT">object which is synchronized.</param>
#define GA_LOCK_OBJECT(LOCK_NAME, OBJECT) Common::Threading::GaSectionLock LOCK_NAME( ( OBJECT )->GetSynchronizator(), true )

/// <summary>Macro acquires access to this and prevents concurrent access.
/// It declares and instantiates <see cref="GaSectionLock" /> object with name lock and acquire access to
/// this object, when execution leave the scope in which <c>GA_LOCK_OBJECT</c> is specified,
/// <see cref="GaSectionLock" /> object is destroyed and access to this object is released. Unlocking
/// access to this before leaving scope can be done by calling <c>GA_UNLOCK(lock_name)</c> macro.</summary>
/// <param name="LOCK_NAME">name of <c>GaSectionLock</c> object.</param>
#define GA_LOCK_THIS_OBJECT(LOCK_NAME) Common::Threading::GaSectionLock LOCK_NAME( &this->_synchronizator, true )

/// <summary>Macro acquires access to critical section.
/// It instantiate <see cref="GaSectionLock" /> object with name lock and acquire access to critical section, when
/// execution leaves the scope in which <c>GA_LOCK_OBJECT</c> is specified, <see cref="GaSectionLock" /> object
/// is destroyed and access to the locked section is released. Unlocking access to the object before leaving
/// scope can be done by calling <c>GA_UNLOCK(lock_name)</c> macro.</summary>
/// <param name="LOCK_NAME">name of <c>GaSectionLock</c> object.</param>
/// <param name="SECTION">critical section object.</param>
#define GA_LOCK_SECTION(LOCK_NAME, SECTION) Common::Threading::GaSectionLock LOCK_NAME( &( SECTION ), true )

namespace Common
{
	namespace Threading
	{

		#ifdef __GAL_DOCUMENTATION__

			/// <summary>This type defines system specific type for storing synchronization objects or handles to them.
			/// System specific synchronization is wrapped by <see cref="GaCriticalSection" /> class.</summary>
			typedef _SYSTEM_OR_COMPILER_SPECIFIC_ SysSyncObject;

			/// <summary>This type defines system specific type for storing semaphores objects or handles to them.
			/// Manipulation over semaphores is done by <c>MAKE_SEMAPHORE</c>, <c>FREE_SEMAPHORE</c>, <c>LOCK_SEMAPHORE</c> and 
			/// <c>UNLOCK_SEMAPHORE</c> macros.</summary>
			typedef _SYSTEM_OR_COMPILER_SPECIFIC_ SysSemaphoreObject;

			/// <summary>This type defines system specific type for storing events objects or handles to them.
			/// Manipulation over events is done by <c>MAKE_EVENT</c>, <c>FREE_EVENT</c>, <c>WAIT_FOR_EVENT</c> and 
			/// <c>SIGNAL_EVENT</c> macros.</summary>
			typedef _SYSTEM_OR_COMPILER_SPECIFIC_ SysEventObject;

			/// <summary>Yields logical processor to another thread.</summary>
			inline void GACALL GaYieldProcessor() { }

			/// <summary>Yield execution to another ready thread.</summary>
			inline void GACALL GaYieldThread() { }

		#endif

		#if defined(GAL_PLATFORM_WIN)

			typedef CRITICAL_SECTION SysSyncObject;
			typedef HANDLE SysSemaphoreObject;
			typedef HANDLE SysEventObject;

			inline void GACALL GaYieldProcessor() { __asm { rep nop } }
			inline void GACALL GaYieldThread() { SwitchToThread(); }

		#elif defined(GAL_PLATFORM_NIX)

			typedef pthread_mutex_t SysSyncObject;

			#if defined(GAL_PLATFORM_MACOS)

				typedef sem_t* SysSemaphoreObject;
				typedef sem_t* SysEventObject;

			#else

				typedef sem_t SysSemaphoreObject;
				typedef sem_t SysEventObject;

			#endif

			inline void GACALL GaYieldProcessor() { asm( "rep nop" ); }
			inline void GACALL GaYieldThread() { sched_yield(); }
		
		#endif

		/// <summary><c>GaCriticalSection</c> class is wrapper class for system synchronization object.
		/// 
		/// This class provides basic synchronization and protection from concurrent access to the objects and resources.
		/// Built-in synchronizators used in other classes are instances of <c>GaCriticalSection</c> class. <c>GA_LOCK</c> and
		/// <c>GA_UNLOCK</c> macros can operate on instances of this class.</summary>
		class GaCriticalSection
		{

		private:

			/// <summary>System specific object used for synchronization of critical section.</summary>
			SysSyncObject _section;

		public:

			/// <summary>Constructor performs system specific initialization of synchronization object if needed.</summary>
			GaCriticalSection()
			{
				#if defined(GAL_PLATFORM_WIN)

				InitializeCriticalSection( (LPCRITICAL_SECTION)&_section );

				#elif defined(GAL_PLATFORM_NIX)
				
				pthread_mutex_init( &_section, NULL );
				
				#endif
			}

			/// <summary>This constructor creates copy of a critical section.</summary>
			GaCriticalSection(const GaCriticalSection& rhs)
			{
				#if defined(GAL_PLATFORM_WIN)

				InitializeCriticalSection( (LPCRITICAL_SECTION)&_section );

				#elif defined(GAL_PLATFORM_NIX)
				
				pthread_mutex_init( &_section, NULL );
				
				#endif
			}

			/// <summary>Frees resources used by system synchronization object.</summary>
			~GaCriticalSection()
			{
				#if defined(GAL_PLATFORM_WIN)

				DeleteCriticalSection( (LPCRITICAL_SECTION)&_section );

				#elif defined(GAL_PLATFORM_NIX)
				
				pthread_mutex_destroy( &_section );
				
				#endif
			}

			/// <summary><c>Lock</c> method acquires synchronization object and prevents other thread to access protected section simultaneously.
			/// If another thread had already acquired synchronization object current thread is put to sleep, and it is waked when synchronization object is released.</summary>
			inline void GACALL Lock()
			{
				#if defined(GAL_PLATFORM_WIN)

				EnterCriticalSection( (LPCRITICAL_SECTION)&_section );

				#elif defined(GAL_PLATFORM_NIX)
				
				pthread_mutex_lock( &_section );
				
				#endif
			}

			/// <summary><c>Unlock</c> method release synchronization object and wakes one of the threads that was waiting for the object.</summary>
			inline void GACALL Unlock()
			{
				#if defined(GAL_PLATFORM_WIN)

				LeaveCriticalSection( (LPCRITICAL_SECTION)&_section );

				#elif defined(GAL_PLATFORM_NIX)
				
				pthread_mutex_unlock( &_section );
				
				#endif
			}

		};

		/// <summary><c>GaSpinlock</c> implements busy-waiting synchronization mechanism (spinlocks).
		/// 
		/// This class provides basic synchronization and protection from concurrent access to the objects and resources.
		/// Built-in synchronizators used in other classes are instances of <c>GaCriticalSection</c> class. <c>GA_LOCK</c> and
		/// <c>GA_UNLOCK</c> macros can operate on instances of this class.</summary>
		class GaSpinlock
		{

		private:

			/// <summary>Number of retries to get the lock before the execution is thransfered to another thread.</summary>
			static const int YieldThreshold = 64 - 1;

			/// <summary>Varaible used for busy-waiting.</summary>
			GaAtomic<int> _lock;

		public:

			/// <summary>Initializes spinlock.</summary>
			GaSpinlock() { }

			/// <summary>This method acquires spinlock if it is available. If the spinlock is already acquired this method performes busy-waiting
			/// until the lock is not released and the it acquires the lock.</summary>
			inline void GACALL Lock() { Spin(); }
			
			/// <summary>This method acquires two spinlocks if they are available. If the spinlocks are already acquired this method performes busy-waiting
			/// until the locks are not released and the it acquires the locks. Order of locking is defined by order of spinlocks' addresses to prevent deadlocks
			/// Method locks spinlock at lower address firstly then it locks spinlock on higher address.</summary>
			/// <param name="second">pointer to the second spinlock that should be acquired.</param>
			inline void GACALL Lock(GaSpinlock* second)
			{
				// spinlocks are locked in order by their addresses to prevent deadlocks
				if( this < second )
				{
					Spin();
					second->Spin();
				}
				else
				{
					second->Spin();
					Spin();
				}
			}

			/// <summary>This method unlocks spinlock and release single thread it there was threads that wait for the spinlock.</summary>
			inline void GACALL Unlock() { _lock = 0; }

			/// <summary>This method unlocks two spinlocks and release threads it there was threads that wait for the spinlocks.</summary>
			/// <param name="second">pointer to the second spinlock that should be released.</param>
			inline void GACALL Unlock(GaSpinlock* second)
			{
				Unlock();
				second->Unlock();
			}

		private:

			/// <summary>This method performes busy-waiting until the spinlock is no released.</summary>
			inline void GACALL Spin()
			{
				do
				{ 
					for( int counter = 0; _lock != 0; counter = ( counter + 1 ) & YieldThreshold )
					{
						// run another thread
						if( !counter )
							GaYieldThread();

						// release logical processor
						GaYieldProcessor();
					}
				} while( !_lock.CompareAndExchange( 0, 1 ) );
			}

		};
		
		/// <summary><c>GaSectionLock</c> class is used for automatic access control with help of <see cref="GaCriticalSection" /> class.
		/// Synchronization object can be automatically acquired when instance of <c>GaSectionLock</c> is created. If synchronization object
		/// is locked by instance of <c>GaSectionLock</c> it is released when the instance goes out of scope. This mechanism provides simple
		/// way of managing critical sections because users don't have to worry about releasing of synchronization object in most cases,
		/// but for more complicated cases <c>GA_LOCK</c> and <c>GA_UNLOCK</c> macros can be used with instances of this class or with 
		/// <see cref="GaCriticalSection" /> class. <c>GaSectionLock</c> is mainly employed by <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c>.
		///
		/// Object of this class shouldn't be used from multiple threads simultaneously.</summary>
		class GaSectionLock
		{

		private:

			/// <summary>Pointer to underlying synchronization object which is managed by this lock.</summary>
			GaCriticalSection* _section;

			/// <summary>Keeps track whether underlying synchronization object is locked by this instance of <c>GaSectionLock</c> class.</summary>
			bool _locked;

		public:

			/// <summary>Constructor associates new instance of this class with underlying synchronization object.</summary>
			/// <param name="section">pointer to underlying synchronization object.</param>
			/// <param name="aquireLock">if this parameter is set to <c>true</c> constructor tries to acquire underlying object immediately.</param>
			GaSectionLock(GaCriticalSection* section,
				bool aquireLock) : _section(section),
				_locked(false)
			{
				if( aquireLock )
					Lock();
			}

			/// <summary>Destructor releases underlying synchronization object if it was acquired by this instance of <c>GaSectionLock</c> class.</summary>
			~GaSectionLock() { Unlock(); }

			/// <summary><c>Lock</c> method locks underlying synchronization object. If it was already locked by this instance of <c>GaSectionLock</c> class, call has no effect.
			/// Details of locking synchronization object are described in specification of <c>GaCriticalSection::Lock</c> method.</summary>
			inline void GACALL Lock()
			{
				if( !_locked )
				{
					_section->Lock();
					_locked = true;
				}
			}

			/// <summary>Unlock method unlocks underlying synchronization object. If it was not locked by this instance of <c>GaSectionLock</c> class, call has no effect.
			/// Details of unlocking synchronization object are described in specification of <c>GaCriticalSection::Unlock</c> method.</summary>
			inline void GACALL Unlock()
			{
				if( _locked )
				{
					_section->Unlock();
					_locked = false;
				}
			}

		};

		/// <summary><c>GaSemaphore</c> class is wrapper class that manages semaphore system object.</summary>
		class GaSemaphore
		{

		private:

			/// <summary>System specific object that represents semaphore.</summary>
			SysSemaphoreObject _semaphore;

		public:

			/// <summary>This constructor makes semaphore and initializes it.</summary>
			/// <param name="initialCount">initial count of semaphore, value must be greater or equals to the 0 and less then or equals to <c>maxCount</c>.</param>
			/// <param name="maxCount">maximum count of semaphore, value must be greater then 0. On *nix systems this parameter is ignored.</param>
			/// <exception cref="GaSystemException" />Thrown if the semaphore cannot be created.</exception>
			GaSemaphore(int initialCount,
				int maxCount)
			{
				#if defined(GAL_PLATFORM_WIN)

				_semaphore = CreateSemaphore( NULL, initialCount, maxCount, NULL );
				GA_SYS_ASSERT( _semaphore != NULL, "Threading" );

				#elif defined(GAL_PLATFORM_MACOS)

				static volatile int sem_counter = 0;
				char sem_name[ 40 ];
				strcpy( sem_name, "/tmp/gal_sem" );
				sprintf( sem_name + 12, "_%d", getpid() );
				sprintf( sem_name + strlen( sem_name ), "_%d", GaAtomicOps<int>::Add( &sem_counter, 1 ) + 1 ) );

				_semaphore = sem_open( sem_name, O_CREAT, 0644, initialCount );
				GA_SYS_ASSERT( _semaphore != SEM_FAILED, "Threading" );

				#elif defined(GAL_PLATFORM_NIX)

				if( sem_init( &_semaphore, 0, initialCount ) == -1 )
					GA_SYS_THROW( "Threading" );

				#endif
			}

			/// <summary>The destructor frees system semaphore.</summary>
			~GaSemaphore()
			{
				#if defined(GAL_PLATFORM_WIN)

				CloseHandle( _semaphore );

				#elif defined(GAL_PLATFORM_MACOS)

				sem_destroy( _semaphore );

				#elif defined(GAL_PLATFORM_NIX)

				sem_destroy( &_semaphore );

				#endif
			}

			/// <summary>This method is used to acquire access to critical section protected by semaphore.</summary>
			/// <exception cref="GaSystemException" />Thrown if the semaphore is not locked successfully.</exception>
			inline void GACALL Lock()
			{
				#if defined(GAL_PLATFORM_WIN)

				if( WaitForSingleObject( _semaphore, INFINITE ) == WAIT_FAILED )
					GA_SYS_THROW( "Threading" );

				#elif defined(GAL_PLATFORM_MACOS)

				if( sem_wait( _semaphore ) == -1 )
					GA_SYS_THROW( "Threading" );

				#elif defined(GAL_PLATFORM_NIX)

				if( sem_wait( &_semaphore ) == -1 )
					GA_SYS_THROW( "Threading" );

				#endif
			}

			/// <summary>This method is used to release access to critical section protected by semaphore.</summary>
			/// <param name="count">amount by which semaphore's count is increased, value must be grater then 0 and sum
			/// of the value and current semaphore's count must be less then or equals to maximal count of semaphore.</param>
			/// <exception cref="GaSystemException" />Thrown if the semaphore is not unlocked successfully.</exception>
			inline void GACALL Unlock(int count = 1)
			{
				#if defined(GAL_PLATFORM_WIN)
			
				if( !ReleaseSemaphore( _semaphore, count, NULL ) )
					GA_SYS_THROW( "Threading" );

				#elif defined(GAL_PLATFORM_MACOS)

				for( ; count > 0; count-- )
				{
					if( sem_post( _semaphore ) == -1 )
						GA_SYS_THROW( "Threading" );
				}

				#elif defined(GAL_PLATFORM_NIX)

				for( ; count > 0; count-- )
				{
					if( sem_post( &_semaphore ) == -1 )
						GA_SYS_THROW( "Threading" );
				}
			
				#endif
			}

			#if defined(GAL_PLATFORM_NIX)

			/// <summary><c>TryLock</c> method decrements semaphore count by specified value. If semaphore count reaches zero before it is decremented
			/// by desired value method exits.
			///
			/// This method is available only on *nix platforms.</summary>
			/// <param name="count">value by which semaphore count should be decremented.</param>
			/// <returns>Returns difference between desired decrement value, and actual value by which the semaphore is decremented.</returns>
			/// <exception cref="GaSystemException" />Thrown if the semaphore is not locked successfully.</exception>
			inline int GACALL TryLock(int count = 1)
			{
				// decrement semaphore count
				for( ; count > 0; count-- )
				{
					if( sem_trywait( &_semaphore ) )
					{
						if( errno() == EAGAIN )
							// semaphore count reached zero
							break;

						GA_SYS_THROW( "Threading" );
					}
				}

				// return 
				return count;
			}

			#endif

		};


		/// <summary><c>GaEvent</c> class is wrapper class that manages evet system object.</summary>
		class GaEvent
		{

			GA_DEFINE_ATOM_NODE( GaEvent )

		public:

			/// <summary>Types of events.</summary>
			enum GaEventType
			{

				/// <summary>For manual events user must call <see cref="GaEvent::Reset" /> method to put event in non-signaled state.</summary>
				GAET_MANUAL = 0,

				/// <summary>Auto events are immidiately put to non-signaled state after the thread that waits for event has been released.</summary>
				GAET_AUTO = 1
			};

		private:

			#if defined(GAL_PLATFORM_WIN)

			/// <summary>System specific object that represents event.</summary>
			SysEventObject _event;

			#elif defined(GAL_PLATFORM_NIX)

			/// <summary>Semaphore object used for signaling threads which waits for event.</summary>
			GaSemaphore _semaphore;

			/// <summary>Critical section used to protect event signaling process from concurent access.</summary>
			GaCriticalSection _section;
			
			/// <summary>Number of threads which waits for manual reset event.</summary>
			int _numberOfThreadsWaiting;

			/// <summary>Inidicates whether the event is in signaled state.</summary>
			bool _signaled;

			#endif

			/// <summary>Type of event.</summary>
			GaEventType _type;

		public:

			/// <summary>The constructor creates system event and initializes it.</summary>
			/// <param name="type">type of event.</param>
			/// <exception cref="GaSystemException" />Thrown if the event cannot be created.</exception>
			GaEvent(GaEventType type) : _type(type)
				#if defined(GAL_PLATFORM_NIX)

				,_semaphore(0, 1),
				_threadsWaiting(0),
				_signaled(false)

				#endif
			{
				#if defined(GAL_PLATFORM_WIN)

				_event = CreateEvent( NULL, _type == GAET_MANUAL, FALSE, NULL );
				GA_SYS_ASSERT( _event != NULL, "Threading" );

				#endif
			}

			/// <summary>The destructor frees system event object.</summary>
			~GaEvent()
			{
				#if defined(GAL_PLATFORM_WIN)

				CloseHandle( _event );

				#endif
			}

			/// <summary>This method is used to block calling thread until event reaches signaled state.
			/// When calling thread is released, auto events are restared to non-signaled state and manual events stay signaled.</summary>
			/// <exception cref="GaSystemException" />Thrown if an error has occured during the waiting for signal.</exception>
			inline void GACALL Wait()
			{
				#if defined(GAL_PLATFORM_WIN)

				if( WaitForSingleObject( _event, INFINITE ) == WAIT_FAILED )
					GA_SYS_THROW( "Threading" );

				#elif defined(GAL_PLATFORM_NIX)

				// indicates wheter the event is in signaled state
				bool wait = true;

				GA_LOCK_SECTION( lock, _section );

				// manual-reset events requires counting of waiting threads
				if( type == GAET_MANUAL )
				{
					// thread should wait on semaphore if the event is not signaled
					wait = !_signaled;

					// increment number of waiting threads
					if( wait )
						_numberOfThreadsWaiting++;
				}

				GA_UNLOCK( lock );

				// wait for signal if the event is not in signaled state
				if( wait )
					_semaphore.Lock();

				#endif
			}

			/// <summary>This method is used to set event to signaled state.</summary>
			/// <exception cref="GaSystemException" />Thrown if the event was not signaled successfully.</exception>
			inline void GACALL Signal()
			{
				#if defined(GAL_PLATFORM_WIN)

				if( !SetEvent( _event ) )
					GA_SYS_THROW( "Threading" );

				#elif defined(GAL_PLATFORM_NIX)

				GA_LOCK_SECTION( lock, _section );

				// signal event only if it is not already signaled
				if( !_signaled )
				{
					_signaled = true;
					// release waiting thread(s)
					_semaphore.Unlock( _type == GAET_AUTO ? 1 : _numberOfThreadsWaiting );
					_numberOfThreadsWaiting = 0;
				}

				#endif
			}

			/// <summary>This method is used to reset event to non-signaled state.</summary>
			/// <exception cref="GaSystemException" />Thrown if the event was not reset to non-signaled state.</exception>
			inline void GACALL Reset()
			{
				#if defined(GAL_PLATFORM_WIN)

				if( !ResetEvent( _event ) )
					GA_SYS_THROW( "Threading" );

				#elif defined(GAL_PLATFORM_NIX)

				GA_LOCK_SECTION( lock, _section );

				// restart only signaled events
				if( _signaled )
				{
					// set to non-signaled state
					_signaled = false;
					// restart semaphore used for signaling
					_numberOfThreadsWaiting = _semaphore.TryLock( _numberOfThreadsWaiting );
				}

				#endif
			}

			/// <summary>This method returns type of event.</summary>
			/// <summary>Method returns type of event.</summary>
			inline GaEventType GetType() const { return _type; }
		};

	} // Threading
} // Common

#endif // __GA_SYNCHRONIZATION_H__

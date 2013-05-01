
/*! \file Threading.h
    \brief This file contains declaration of classes and types used to abstract operating system specific threads control and synchronization.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_THREADING_H__
#define __GA_THREADING_H__

#include "Synchronization.h"

namespace Common
{

	/// <summary>Contains classes and types used for abstracting operating system specific threads control.</summary>
	namespace Threading
	{

		#ifdef __GAL_DOCUMENTATION__

			/// <summary>Datatype used for storing threads objects or handles to them.</summary>
			typedef _SYSTEM_OR_COMPILER_SPECIFIC_ SystemThread;

			/// <summary>Type of return value of the functions that are used as entry point of the thread by the operating system.</summary>
			typedef _SYSTEM_OR_COMPILER_SPECIFIC_ ThreadFunctionReturn;

			/// <summary>Type of object uset for storing ID of the thread.</summary>
			typedef _SYSTEM_OR_COMPILER_SPECIFIC_ ThreadID;

		#endif

		#if defined(GAL_PLATFORM_WIN)
		
			typedef HANDLE SystemThread;
			typedef DWORD ThreadID;
			typedef int ThreadFunctionReturn;

		#elif defined(GAL_PLATFORM_NIX)
		
			typedef pthread_t SystemThread;
			typedef pthread_t ThreadID;
			typedef void* ThreadFunctionReturn;
		
		#endif


		class GaThread;

		/// <summary><c>ThreadFunctionPointer</c> is pointer to function used as thread's entry point. Entry point function must obey restriction of this type:
		/// <br/>1. Function must return value of <see cref="ThreadFunctionReturn" /> type.
		/// <br/>2. Function must use <c>GACALL</c> calling convention.
		/// <br/>3. Function must have two parameters.
		/// <br/>4. First parameter must be pointer <see cref="GaThread" /> (<see cref="GaThread" />*).
		/// <br/>5. Second parameter must be pointer to <c>void</c> (<c>void</c>*).</summary>
		typedef ThreadFunctionReturn (GACALL *ThreadFunctionPointer)(GaThread*, void*);

		/// <summary><c>GaThreadStartInfo</c> structure contains information needed to start new thread.
		/// It has pointer to function which is entry point of thread and pointer to parameters which will be passed to the function.
		/// Restrictions of entry point's function of a thread are described by <c>GaThreadFunctionPointer</c> type.</summary>
		struct GaThreadStartInfo
		{

		public:

			/// <summary>Pointer to function which represents entry point of the thread.</summary>
			ThreadFunctionPointer _functionPointer;

			/// <summary>Pointer to parameters which will be passed to function which represents thread's entry point.</summary>
			void* _functionParameters;

			/// <summary>This constructor initializes thread start pameters with default values.</summary>
			GaThreadStartInfo() : _functionPointer(NULL),
				_functionParameters(NULL) { }

			/// <summary>This constructor initializes thread start pameters with user-defined values.</summary>
			/// <param name="functionPointer">pointer to function which represents entry point of the thread.</param>
			/// <param name="functionParameters">pointer to parameters which will be passed to function which represents thread's entry point.</param>
			GaThreadStartInfo(ThreadFunctionPointer functionPointer,
				void* functionParameters) : _functionPointer(functionPointer),
				_functionParameters(functionParameters) { }

		};

		/// <summary>This enumeration defines possible states of threads.</summary>
		enum GaThreadStatus
		{

			/// <summary>Thread is running.</summary>
			GATS_RUNNING = 0x1, 

			/// <summary>Thread execution is terminated, but thread object still exists and can be used to restart execution.</summary>
			GATS_STOPPED = 0x2, 

			/// <summary>Thread execution is temporary is suspended.</summary>
			GATS_PAUSED = 0x4, 

			/// <summary>Used for checking running state of thread.</summary>
			GATS_NOT_RUNNING = GATS_STOPPED | GATS_PAUSED

		};

		/// <summary><c>GaThread</c> class controls system threads. It wraps system specific control of threading.
		///
		/// This class has built-in synchronizator so it is allowed to use <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros with instances of this class.
		/// All public methods are thread-safe.</summary>
		class GaThread
		{
			GA_SYNC_CLASS

		private:

			/// <summary>This attribute holds running status of the thread.</summary>
			GaThreadStatus _status;

			/// <summary>System specific the thread object or handle to it.</summary>
			SystemThread _thread;

			/// <summary>stores identification number of thread if it is running or it is suspended.</summary>
			ThreadID _id;

			/// <summary>User defined information (thread's entry point and custom parameters passed to the thread) needed to start thread.</summary>
			GaThreadStartInfo _parameters;

			/// <summary>Pointer to the flag that inidicates whether the thread object was destoryed by the thread itself.</summary>
			bool* _objectDestroyed;

		public:

			/// <summary>Initializes thread object and stores thread parameters.
			/// If user specified, thread can be started automatically when object is created.</summary>
			/// <param name="parameters">thread parameters (entry point and pointer to custom parameters passed to the thread).</param>
			/// <param name="started">if this parameter is set to <c>true</c> then thread start execution immediately. </param>
			/// <exception cref="GaSystemException" />Thrown if the thread cannot be created.</exception>
			GAL_API
			GaThread(const GaThreadStartInfo& parameters,
				bool started);

			/// <summary>Stops thread if it is running and releases system object and closes handles to it.</summary>
			GAL_API
			~GaThread();

			/// <summary>Method starts execution of the thread or resumes execution if it was suspended.
			/// If thread already running or system is unable to start/resume thread call fails and method returns <c>false</c>.
			///
			/// This method is thread-safe.</summary>
			/// <exception cref="GaSystemException" />Thrown if the thread cannot be started or resumed.</exception>
			/// <exception cref="GaInvalideOperationException" />Thrown if the thread is already in running state.</exception>
			GAL_API
			void GACALL Start();

			/// <summary>Method suspends thread execution if it is running.
			/// If thread is stopped or system cannot suspend thread, call fails and method returns <c>false</c>. Pause method is intended only for debugging purposes.
			/// This method is not implemented on *nix systems.
			///
			/// This method is thread-safe.</summary>
			/// <exception cref="GaSystemException" />Thrown if the operating system cannot suspend thread.</exception>
			/// <exception cref="GaInvalideOperationException" />Thrown if the thread is not running, when thread tries to suspend itself
			/// or when the method is executed on *nix systems.</exception>
			GAL_API
			void GACALL Pause();

			/// <summary>Method stops execution of thread forcibly. Calling this method can cause problems because closing thread cannot release acquired resources
			/// properly and do necessary cleanups.
			///
			/// This method is thread-safe.</summary>
			/// <exception cref="GaSystemException" />Thrown if the operating system cannot abort execution of the thread.</exception>
			/// <exception cref="GaInvalideOperationException" />Thrown if the thread is not running.</exception>
			GAL_API
			void GACALL Abort();

			/// <summary>Suspends execution of thread that called this method until thread which is managed by this instance of <c>GaThread</c> class finish execution.
			///
			/// This method is thread-safe.</summary>
			/// <exception cref="GaSystemException" />Thrown if an error has occured during the waiting for the thread.</exception>
			GAL_API
			void GACALL Join();

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns current status of the thread.</returns>
			inline GaThreadStatus GACALL Status()
			{
				GA_LOCK_THIS_OBJECT( lock );
				return _status;
			}

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns identification number of thread if it is running or suspended. If thread is stopped, method returns 0.</returns>
			inline ThreadID GACALL GetId()
			{
				GA_LOCK_THIS_OBJECT( lock );
				return _id;
			}

		private:

			/// <summary>This method is used as entry point of new thread. Operating system calls this method when it starts thread.
			/// After that, when thread enters <c>ThreadFunctionWrapper</c> it calls user entry point which is specified in thread parameters.</summary>
			/// <param name=""thread>pointer to object which manages new thread.</param>
			/// <returns>Method returns result of user defined thread's entry point function when thread finishes its execution.</returns>
			static ThreadFunctionReturn APICALL ThreadFunctionWraper(GaThread* thread);

		};

	} // Threading
} // Common

#endif // __GA_THREADING_H__

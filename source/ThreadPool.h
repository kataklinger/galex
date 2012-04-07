
/*! \file ThreadPool.h
	\brief This file contains declaration of classes that implement thread pools.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_THREAD_POOL_H__
#define __GA_THREAD_POOL_H__

#include "Threading.h"
#include "EventPool.h"

namespace Common
{
	namespace Threading
	{

		class GaThreadPool;
		class GaWorkItem;

		/// <summary><c>GaWorkerThread</c> class manages worker thread of the thread pool. 
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaWorkerThread
		{

			GA_DEFINE_ATOM_NODE( GaWorkerThread )

		public:

			/// <summary><c>GaWorkerThreadCounter</c> class tracks number of existing worker threads. 
			/// And provides synchronizatiom method for safe cleaup of resources used by those threads.
			/// 
			/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
			/// but all public methods are thread-safe.</summary>
			class GaWorkerThreadCounter
			{

			private:

				/// <summary>Count of existing worker threads.</summary>
				GaAtomic<int> _count;

				/// <summary>Event raises when count of worker threads reache zero.</summary>
				Memory::GaAutoPtr<GaEvent> _finishEvent;

			public:

				/// <summary>Initializes counter.</summary>
				GaWorkerThreadCounter()
				{
					_finishEvent = GaEventPool::GetInstance()->GetEventWithAutoPtr( GaEvent::GAET_MANUAL );
					_finishEvent->Signal();
				}

				/// <summary><c>WorkerThreadStart</c> method increments count of existing threads, and reset event if it was raised.
				///
				/// This method is thread-safe.</summary>
				inline void GACALL WorkerThreadStart()
				{
					if( ++_count == 1 )
						_finishEvent->Reset();
				}

				/// <summary><c>WorkerThreadEnd</c> method decrements count of existing threads, and raise event if it reaches zero.
				///
				/// This method is thread-safe.</summary>
				inline void GACALL WorkerThreadEnd()
				{
					if( --_count == 0 )
						_finishEvent->Signal();
				}

				/// <summary>Wait for thread counter to reach zero.
				///
				/// This method is thread-safe.</summary>
				inline void GACALL WaitForThreads() { _finishEvent->Wait(); }

			};

			/// <summary><c>GaWorkerThreadCreate</c> class implements creation policy for objects of worker threads used by thread pool.</summary>
			class GaWorkerThreadCreate
			{

			private:

				/// <summary>Counter that tracks threads created by this policy object.</summary>
				GaWorkerThreadCounter* _threadCounter;

			public:

				/// <summary>Initializes creation policy.</summary>
				/// <param name="threadCounter">pointer to counter that will track threads created by this policy object.</param>
				GaWorkerThreadCreate(GaWorkerThreadCounter* threadCounter) : _threadCounter(threadCounter) { }

				/// <summary><c>operator()</c> creates and starts new worker thread.
				///
				/// This operator is thread safe.</summary>
				inline GaWorkerThread* GACALL operator() () const
				{
					_threadCounter->WorkerThreadStart();
					return new GaWorkerThread( _threadCounter );
				}

			};

			/// <summary><c>GaWorkerThreadClose</c> class implements deletion policy for objects of worker threads used by thread pool.</summary>
			class GaWorkerThreadClose
			{

			public:

				/// <summary><c>operator()</c> marks provided worker thread as closed.
				///
				/// This operator is thread safe.</summary>
				/// <param name="object">pointer to worker thread that should be closed.</param>
				inline void GACALL operator ()(GaWorkerThread* object) const { object->Close(); }

			};

			/// <summary>Type of object pool that stores worker threads.</summary>
			typedef Memory::GaObjectPool<GaWorkerThread, GaWorkerThreadCreate, GaWorkerThreadClose> GaPool;

			/// <summary>Type of smart pointer to object pool that stores worker threads.</summary>
			typedef Memory::GaSmartPtr<GaPool> GaPoolPtr;

		private:

			/// <summary>Pointer to event object that signals that the work is ready and that thread can begin execution.</summary>
			Memory::GaAutoPtr<GaEvent> _event;

			/// <summary>Thread start parameters (entry point and pointer to parameters).</summary>
			GaThreadStartInfo _threadStartParams;

			/// <summary>Pointer to object of the thread that executes work.</summary>
			Memory::GaAutoPtr<GaThread> _thread;

			/// <summary>Counter that tracks this worker thread.</summary>
			GaWorkerThreadCounter* _threadCounter;

			/// <summary>Pointer to object pool that owens this worker thread.</summary>
			GaPoolPtr _pool;

			/// <summary>Pointer to work item which is currently executing. This attribut is <c>NULL</c> if the thread waits for work.</summary>
			Memory::GaAutoPtr<GaWorkItem> _currentWork;

			/// <summary>Indicates whether the worker thread is marked for closing.</summary>
			bool _closed;

			/// <summary>Generic exception handler for exceptions that occured during the execution of work items but are not stored in result object of those items.</summary>
			Memory::GaSmartPtr<Exceptions::GaExceptionHandler> _exceptionHandler;

		public:

			/// <summary>Creates thread and initializes worker.</summary>
			/// <param name="threadCounter">pointer to counter that tracks this worker thread.</param>
			GAL_API
			GaWorkerThread(GaWorkerThreadCounter* threadCounter);

			/// <summary>Frees used resources.</summary>
			GAL_API
			~GaWorkerThread();

			/// <summary><c>StartWorker</c> starts worker thread. The method should be called when worker thread is fully initialized.
			///
			/// This mehtod is not thread-safe.</summary>
			inline void GACALL StartWorker() { _thread->Start(); }

			/// <summary><c>SetWork</c> method assigns work item and wakes the thread.
			///
			/// This mehtod is not thread-safe.</summary>
			/// <param name="item">pointer to work item which has been assigned to the thread.</param>
			/// <param name="ownerPool">smart pointer to object pool from which this thread is retrieved.</param>
			/// <param name="exceptionHandler">pointer to generic exception handler for exceptions that occured during the execution of work items
			/// but are not stored in result object of those items.</param>
			inline void GACALL SetWork(GaWorkItem* item,
				GaPoolPtr ownerPool,
				Memory::GaSmartPtr<Exceptions::GaExceptionHandler> exceptionHandler)
			{
				_pool = ownerPool;
				_exceptionHandler = exceptionHandler;
				_currentWork = item;
				_event->Signal();
			}

			/// <summary><c>Close</c> method marks thread as clased.
			///
			/// This mehtod is not thread-safe.</summary>
			inline void GACALL Close()
			{
				_closed = true;
				_event->Signal();
			}

			/// <summary>This mehtod is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if the thread is marked for clasing.</returns>
			inline bool GACALL IsClosed() const { return _closed; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to thread object.</returns>
			inline GaThread* GACALL GetThread() { return _thread.GetRawPtr(); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to thread object.</returns>
			inline const GaThread* GACALL GetThread() const { return _thread.GetRawPtr(); }

		private:

			/// <summary>This method represents entry point for worker thread.</summary>
			/// <param name="thread">pointer to thread object. This parameter is ignores.</param>
			/// <param name="params">pointer to <c>GaWorkerThread</c> object speficied in thread start parameters.</param>
			GAL_API
			static ThreadFunctionReturn GACALL WorkerWrapper(GaThread* thread,
				void* params);

		};

		class GaWorkItem;

		/// <summary><c>GaWorkResults</c> represents base class for result objects of work items. The calls implements mechanism for notification of work item
		/// completion (or abortion) and waiting for results of the work item.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public methods that can be called by the user are thread-safe.<summary>
		class GaWorkResults
		{

		protected:

			/// <summary>Pointer to event object which signals threads that wait for result that the work is complited.</summary>
			GaEvent* _event;

			/// <summary>Pointer to work item which owns this result object.</summary>
			GaWorkItem* _workItem;

			/// <summary>This attribute indicates whether the work is complited successfully.</summary>
			bool _ready;

			/// <summary>Exception thrown during the execution of the work item.</summary>
			Memory::GaAutoPtr<Exceptions::GaException> _exception;

		public:

			/// <summary>This constructor initializes work result object.</summary>
			/// <param name="workItem">pointer to work item which owns this result object.</param>
			GaWorkResults(GaWorkItem* workItem) : _workItem(workItem),
				_ready(false),
				_exception(NULL) { _event = GaEventPool::GetInstance()->GetEvent( GaEvent::GAET_MANUAL ); }

			/// <summary>The destructor frees used resources.</summary>
			virtual ~GaWorkResults() { GaEventPool::GetInstance()->PutEvent( _event ); }

			/// <summary><c>Wait</c> method suspends calling thread until the work item which owns the result object is not complited (or aborted).
			/// The method should be called before user try to use results.
			///
			/// This method is thread-safe.</summary>
			/// <param name="throwException">if this parameter is set to <c>true</c> it instructs this method to rethrow exception
			/// if was caught during the execution of the work item. Rethrowing exception detaches exception object from result object.</param>
			/// <exception cref="GaException" />Thrown if an exception was caught during the execution of the work item and user has instructed this method to rethrow it.</exception>
			inline void GACALL Wait(bool throwException = false)
			{
				_event->Wait();

				// throw caught exception
				if( throwException && _exception )
					_exception->Throw();
			}
			
			/// <summary><c>SetAsReady</c> method marks result as ready and releases all threads that is waiting for the results.
			/// The method is called by thread pool when worker thread successfully finishes execution of work item.
			/// This method should be used only by thread pool, users should not call this method directly.</summary>
			inline void GACALL SetAsReady()
			{
				_ready = true;
				_event->Signal();
			}

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if the work item which owns this object is successfully complited.</returns>
			inline bool GACALL IsReady() const { return _ready; }

			/// <summary><c>SetException</c> method stores exception to the result object and releases all threads that is waiting for the results.
			/// The method is called by thread pool when an error occures during the execution of the work item.
			/// This method should be used only by thread pool, users should not call this method directly.</summary>
			/// <param name="exception">reference to exception that has occured.</param>
			inline void GACALL SetException(const Exceptions::GaException& exception)
			{
				_exception = exception.Clone();
				_event->Signal();
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to exception that was thrown during the execution of the work item.
			/// If there was no exception, this method returns <c>NULL</c>.</returns>
			inline Exceptions::GaException* GACALL GetException() { return _exception.GetRawPtr(); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if an exception was thrown during the execution of the work item.</returns>
			inline bool GACALL HasException() { return !_exception.IsNull(); }

			/// <summary><c>ThrowException</c> method throws stored exception caught during the execution of the work item.
			/// Throwing exception using this method detaches exception object from result object. If there was no caught exception this method has no effect.
			///
			/// This method is not thread-safe.</summary>
			/// <exception cref="GaException" />Thrown if an exception was caught during the execution of the work item.</exception>
			inline void GACALL ThrowException()
			{
				if( _exception )
					_exception->Throw();
			}

		};

		/// <summary><c>GaTypedWorkResults</c> template class stores results of work item.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public methods that can be called by the user are thread-safe.</summary>
		/// <param name="RT">type of results.</param>
		template<typename RT>
		class GaTypedWorkResults : public GaWorkResults
		{

		protected:

			/// <summary>This attribute stores results of work item.</summary>
			RT _results;

		public:

			/// <summary>This constructor initializes result object.</summary>
			/// <param name="workItem">pointer to work item which owns this result object.</param>
			GaTypedWorkResults(GaWorkItem* workItem) : GaWorkResults(workItem) { }

			/// <summary><c>SetResults</c> method stores results of work item.
			/// This method is called by thread pool when work item is completed successfully and it should not be used by user directly.</summary>
			/// <param name="results">results that should be stored in this object.</param>
			inline void GACALL SetResults(const RT& results)
			{
				_results = results;
				SetAsReady();
			}

			/// <summary><c>GetResults</c> return results of work item. It this method is called without calling <c>Wait</c> method, returned results are undefined.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns results of work item.</returns>
			inline const RT& GACALL GetResults() const { return _results; }

		};

		/// <summary>This template class is specialization of <c>GaTypedWorkResults</c> template class fo <c>void</c> type.
		/// It doesn't store results of work item becaues it is not needed.
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public methods that can be called by the user are thread-safe.</summary>
		template<>
		class GaTypedWorkResults<void> : public GaWorkResults
		{

		public:

			/// <summary>This constructor initializes result object.</summary>
			/// <param name="workItem">pointer to work item which owns this result object.</param>
			GaTypedWorkResults(GaWorkItem* workItem) : GaWorkResults(workItem) { }

		};

		/// <summary><c>GaWorkItem</c> is abstract class which is used to represent and manage work items for worker threads and thread pools.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public methods that can be called by the user are thread-safe.
		/// </summary>
		class GaWorkItem
		{

		protected:

			/// <summary>Pointer to object that stores results of work item.</summary>
			GaWorkResults* _results;

		public:

			/// <summary>This constructor initializes work item object without object that stores results.</summary>
			GaWorkItem() : _results(NULL) { }

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaWorkItem() { }

			/// <summary><c>MakeCopy</c> makes copy of work item object and creates new object that stores results if needed.
			/// This method is called by thread pool when user assigns work item to the pool.</summary>
			/// <param name="makeResultsObject">indicate whether the method should make object for storing results. If it is not required
			/// to store results and users will nott wait for the work item to finish this paramenter should be set to <c>false</c>.</param>
			/// <returns>Method returns pointer to newly created copy of work item object.</returns>
			virtual GaWorkItem* GACALL MakeCopy(bool makeResultsObject) const= 0;

			/// <summary><c>operator()</c> executes work item.</summary>
			virtual void GACALL operator()() = 0;

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns pointer to object that stores results of the work item.</returns>
			inline GaWorkResults* GACALL GetResults() { return _results; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns pointer to object that stores results of the work item.</returns>
			inline const GaWorkResults* GACALL GetResults() const { return _results; }

		};

		/// <summary>This macro implements <c>MakeCopyForQueue</c> method that is required by <see cref="GaWorkItem" /> class.</summary>
		/// <param name="WORKITEM_CLASS_NAME">name of work item class which implements <c>MakeCopyForQueue</c> method.</param>
		/// <param name="WORKRESULT_CLASS_NAME">name of work result class required by work item class.</param>
		#define WORKITEM_COPY_CONSTRUCTOR(WORKITEM_CLASS_NAME, WORKRESULT_CLASS_NAME) \
			virtual GaWorkItem* GACALL MakeCopy(bool makeResultsObject) const \
			{ \
				WORKITEM_CLASS_NAME* copy = new WORKITEM_CLASS_NAME( *this ); \
				if( makeResultsObject ) \
					copy->_results = new WORKRESULT_CLASS_NAME( copy ); \
				return copy; \
			}

		/// <summary><c>GaMethodWorkItem</c> template class is used to represent class method as an object as work item.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public methods that can be called by the user are thread-safe.</summary>
		/// <param name="RT">return type of the method.</param>
		/// <param name="OT">type of the object whose method is executed.</param>
		/// <param name="PT">type of method's parameter.</param>
		template<typename RT, typename OT, typename PT>
		class GaMethodWorkItem : public GaWorkItem
		{

		protected:

			/// <summary>Pointer to object on which the method is performed.</summary>
			OT* _object;

			/// <summary>Pointer to class method which should be executed.</summary>
			RT (GACALL OT::*_method)(PT);

			/// <summary>This atribute stores parameters which are passed to method.</summary>
			PT _parameters;

		public:

			/// <summary>This constructor initializes work item with object and parameters required by the method which is executed.</summary>
			/// <param name="object">pointer to object on which the method is performed.</param>
			/// <param name="method">pointer to class method which should be executed.</param>
			/// <param name="parameters">parameters which are passed to method.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if the <c>object</c> or <c>method</c> parameters are set to <c>NULL</c>.</exception>
			GaMethodWorkItem(OT* object, RT (GACALL OT::*method)(PT), PT parameters) : _object(object),
				_method(method),
				_parameters(parameters)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, object != NULL, "object", "Object on which the method should be performed must be specified.", "Threading" );
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, method != NULL, "method", "Pointer to method which should be executed must be specified.", "Threading" );
			}

			/// <summary>This operator executes provided method and stores results.</summary>
			virtual void GACALL operator()()
			{ 
				// execute method
				RT results = ( _object->*_method )( _parameters );

				// store results if needed
				if( _results )
					( (GaTypedWorkResults<RT>*)_results )->SetResults( results );
			}

			WORKITEM_COPY_CONSTRUCTOR( GaMethodWorkItem, GaTypedWorkResults<RT> )

		};

		/// <summary>This template class is specialization of <c>GaMethodWorkItem</c> template class for methods which doesn't return value (<c>void</c> return type).
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public methods that can be called by the user are thread-safe.</summary>
		/// <param name="OT">type of the object whose method is executed.</param>
		/// <param name="PT">type of method's parameter.</param>
		template<typename OT, typename PT>
		class GaMethodWorkItem<void, OT, PT> : public GaWorkItem
		{

		protected:

			/// <summary>Pointer to object on which the method is performed.</summary>
			OT* _object;

			/// <summary>Pointer to class method which should be executed.</summary>
			void (GACALL OT::*_method)(PT);

			/// <summary>This atribute stores parameters which are passed to method.</summary>
			PT _parameters;

		public:

			/// <summary>This constructor initializes work item with object and parameters required by the method which is executed.</summary>
			/// <param name="object">pointer to object on which the method is performed.</param>
			/// <param name="method">pointer to class method which should be executed.</param>
			/// <param name="parameters">parameters which are passed to method.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if the <c>object</c> or <c>method</c> parameters are set to <c>NULL</c>.</exception>
			GaMethodWorkItem(OT* object, void (GACALL OT::*method)(PT), PT parameters) : _object(object),
				_method(method),
				_parameters(parameters)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, object != NULL, "object", "Object on which the method should be performed must be specified.", "Threading" );
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, method != NULL, "method", "Pointer to method which should be executed must be specified.", "Threading" );
			}

			/// <summary>This operator executes provided method and stores results.</summary>
			virtual void GACALL operator()()
			{
				// execute method
				( _object->*_method )( _parameters );

				// signal that work item is complited
				if( _results )
					_results->SetAsReady();
			}

			WORKITEM_COPY_CONSTRUCTOR( GaMethodWorkItem, GaTypedWorkResults<void> )

		};

		/// <summary>This template class is specialization of <c>GaMethodWorkItem</c> template class for methods which doesn't require parameters.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public methods that can be called by the user are thread-safe.</summary>
		/// <param name="RT">return type of the method.</param>
		/// <param name="OT">type of the object whose method is executed.</param>
		template<typename RT, typename OT>
		class GaMethodWorkItem<RT, OT, void> : public GaWorkItem
		{

		protected:

			/// <summary>Pointer to object on which the method is performed.</summary>
			OT* _object;

			/// <summary>Pointer to class method which should be executed.</summary>
			RT (GACALL OT::*_method)();

		public:

			/// <summary>This constructor initializes work item with object required by the method which is executed.</summary>
			/// <param name="object">pointer to object on which the method is performed.</param>
			/// <param name="method">pointer to class method which should be executed.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if the <c>object</c> or <c>method</c> parameters are set to <c>NULL</c>.</exception>
			GaMethodWorkItem(OT* object, RT (GACALL OT::*method)()) : _object(object),
				_method(method)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, object != NULL, "object", "Object on which the method should be performed must be specified.", "Threading" );
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, method != NULL, "method", "Pointer to method which should be executed must be specified.", "Threading" );
			}

			/// <summary>This operator executes provided method and stores results.</summary>
			virtual void GACALL operator()()
			{
				// execute method
				RT results = ( _object->*_method )();

				// store results if needed
				if( _results )
					( (GaTypedWorkResults<RT>*)_results )->SetResults( results );
			}

			WORKITEM_COPY_CONSTRUCTOR( GaMethodWorkItem, GaTypedWorkResults<RT> )

		};

		/// <summary>This template class is specialization of <c>GaMethodWorkItem</c> template class for methods
		/// which doesn't return value (<c>void</c> return type) and doesn't require parameters.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public methods that can be called by the user are thread-safe.</summary>
		/// <param name="OT">type of the object whose method is executed.</param>
		template<typename OT>
		class GaMethodWorkItem<void, OT, void> : public GaWorkItem
		{

		protected:

			/// <summary>Pointer to object on which the method is performed.</summary>
			OT* _object;

			/// <summary>Pointer to class method which should be executed.</summary>
			void (GACALL OT::*_method)();

		public:

			/// <summary>This constructor initializes work item with object required by the method which is executed.</summary>
			/// <param name="object">pointer to object on which the method is performed.</param>
			/// <param name="method">pointer to class method which should be executed.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if the <c>object</c> or <c>method</c> parameters are set to <c>NULL</c>.</exception>
			GaMethodWorkItem(OT* object, void (GACALL OT::*method)()) : _object(object),
				_method(method)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, object != NULL, "object", "Object on which the method should be performed must be specified.", "Threading" );
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, method != NULL, "method", "Pointer to method which should be executed must be specified.", "Threading" );
			}

			/// <summary>This operator executes provided method and stores results.</summary>
			virtual void GACALL operator()()
			{
				// execute method
				( _object->*_method )();

				// signal that work item is complited
				if( _results )
					_results->SetAsReady();
			}

			WORKITEM_COPY_CONSTRUCTOR( GaMethodWorkItem, GaTypedWorkResults<void> )

		};

		/// <summary><c>GaFunctionWorkItem</c> template class is used to represent standalone function as an object as work item.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public methods that can be called by the user are thread-safe.</summary>
		/// <param name="RT">return type of the function.</param>
		/// <param name="PT">type of functions's parameter.</param>
		template<typename RT, typename PT>
		class GaFunctionWorkItem : public GaWorkItem
		{

		protected:

			/// <summary>Pointer to function which should be executed.</summary>
			RT (GACALL *_function)(PT);

			/// <summary>This atribute stores parameters which are passed to function.</summary>
			PT _parameters;

		public:

			/// <summary>This constructor initializes work item with parameters required by the function which is executed.</summary>
			/// <param name="function">pointer to function which should be executed.</param>
			/// <param name="parameters">parameters which are passed to function.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if the <c>function</c> is set to <c>NULL</c>.</exception>
			GaFunctionWorkItem(RT (GACALL *function)(PT), PT parameters) : _function(function),
				_parameters(parameters)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, function != NULL, "function", "Pointer to function which should be executed must be specified.", "Threading" );
			}

			/// <summary>This operator executes provided function and store results.</summary>
			virtual void GACALL operator()()
			{
				// execute function
				RT results = _function( _parameters );

				// store results if required
				if( _results )
					( (GaTypedWorkResults<RT>*)_results )->SetResults( results );
			}

			WORKITEM_COPY_CONSTRUCTOR( GaFunctionWorkItem, GaTypedWorkResults<RT> )

		};

		/// <summary>This template class is specialization of <c>GaFunctionWorkItem</c> template class for function
		/// which doesn't return value (<c>void</c> return type).
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public methods that can be called by the user are thread-safe.</summary>
		/// <param name="PT">type of functions's parameter.</param>
		template<typename PT>
		class GaFunctionWorkItem<void, PT> : public GaWorkItem
		{

		protected:

			/// <summary>Pointer to function which should be executed.</summary>
			void (GACALL *_function)(PT);

			/// <summary>This atribute stores parameters which are passed to function.</summary>
			PT _parameters;

		public:

			/// <summary>This constructor initializes work item with parameters required by the function which is executed.</summary>
			/// <param name="function">pointer to function which should be executed.</param>
			/// <param name="parameters">parameters which are passed to function.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if the <c>function</c> is set to <c>NULL</c>.</exception>
			GaFunctionWorkItem(void (GACALL *function)(PT), PT parameters) : _function(function),
				_parameters(parameters)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, function != NULL, "function", "Pointer to function which should be executed must be specified.", "Threading" );
			}

			/// <summary>This operator executes provided function and store results.</summary>
			virtual void GACALL operator()()
			{ 
				// execute function
				_function( _parameters );

				// signal that work item is complited
				if( _results )
					_results->SetAsReady();
			}

			WORKITEM_COPY_CONSTRUCTOR( GaFunctionWorkItem, GaTypedWorkResults<void> )

		};

		/// <summary>This template class is specialization of <c>GaFunctionWorkItem</c> template class for function which doesn't require parameters.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public methods that can be called by the user are thread-safe.</summary>
		/// <param name="RT">return type of the function.</param>
		template<typename RT>
		class GaFunctionWorkItem<RT, void> : public GaWorkItem
		{

		protected:

			/// <summary>Pointer to function which should be executed.</summary>
			RT (GACALL *_function)();

		public:

			/// <summary>This constructor initializes work item with the function which is executed.</summary>
			/// <param name="function">pointer to function which should be executed.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if the <c>function</c> is set to <c>NULL</c>.</exception>
			GaFunctionWorkItem(RT (GACALL *function)()) : _function(function)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, function != NULL, "function", "Pointer to function which should be executed must be specified.", "Threading" );
			}

			/// <summary>This operator executes provided function and store results.</summary>
			virtual void GACALL operator()()
			{
				// execute function
				RT results = _function();

				// store results if required
				if( _results )
					( (GaTypedWorkResults<RT>*)_results )->SetResults( results );
			}

			WORKITEM_COPY_CONSTRUCTOR( GaFunctionWorkItem, GaTypedWorkResults<RT> )

		};

		/// <summary>This template class is specialization of <c>GaFunctionWorkItem</c> template class for function
		/// which doesn't return value (<c>void</c> return type) and doesn't require parameters.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public methods that can be called by the user are thread-safe.<summary>
		template<>
		class GaFunctionWorkItem<void, void> : public GaWorkItem
		{

		protected:

			/// <summary>Pointer to function which should be executed.</summary>
			void (GACALL *_function)();

		public:

			/// <summary>This constructor initializes work item with the function which is executed.</summary>
			/// <param name="function">pointer to function which should be executed.</param>
			/// <exception cref="GaNullArgumentException" />Thrown if the <c>function</c> is set to <c>NULL</c>.</exception>
			GaFunctionWorkItem(void (GACALL *function)()) : _function(function)
			{
				GA_ARG_ASSERT( Exceptions::GaNullArgumentException, function != NULL, "function", "Pointer to function which should be executed must be specified.", "Threading" );
			}

			/// <summary>This operator executes provided function and store results.</summary>
			virtual void GACALL operator()()
			{ 
				// execute function
				_function();

				// signal that work item is complited
				if( _results )
					_results->SetAsReady();
			}

			WORKITEM_COPY_CONSTRUCTOR( GaFunctionWorkItem, GaTypedWorkResults<void> )

		};

		/// <summary><c>GaThreadPool</c> class manages thread pools and work item lists.
		///
		/// This class has built-in synchronizator so it is allowed to use <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros with instances of this class.
		/// All public methods are thread-safe except <c>MakeInstance</c> and <c>FreeInstance</c>.</summary>
		class GaThreadPool
		{

			GA_SYNC_CLASS

		private:

			/// <summary>Pointer to global instance of thread pool.</summary>
			GAL_API
			static GaThreadPool* _instance;

		public:

			/// <summary><c>GetInstance</c> method return pointer to global instance of thread pool.</summary>
			/// <returns>Method returns pointer to global instance of thread pool.</returns>
			static inline GaThreadPool& GACALL GetInstance() { return *_instance; }

			/// <summary><c>MakeInstance</c> method makes global instance of thread pool if it is not already exist.
			/// Global event pool (see <see cref="GaEventPool" /> class) should be initialized first.</summary>
			/// <param name="poolSize">number of threads in the pool.</param>
			/// <exception cref="GaInvalidOperationException" />Thrown if global instance already exists.</exception>
			static void GACALL MakeInstance(int poolSize)
			{
				GA_ASSERT( Exceptions::GaInvalidOperationException, _instance == NULL, "Global instance already exists.", "Threading" );
				_instance = new GaThreadPool( poolSize );
			}

			/// <summary><c>FreeInstance</c> method deletes global instance of thread pool and frees used resources.
			/// This thread pool must be destroyed before global event pool (see <see cref="GaEventPool" /> class).</summary>
			/// <exception cref="GaInvalidOperationException" />Thrown if global instance does not exist.</exception>
			static void GACALL FreeInstance()
			{
				GA_ASSERT( Exceptions::GaInvalidOperationException, _instance != NULL, "Global instance does not exists.", "Threading" );
				delete _instance;
			}

		private:

			/// <summary>Counter that keeps number of existing worker threads that belong to this pool and provides synchronization for safe resource cleanup.</summary>
			GaWorkerThread::GaWorkerThreadCounter _threadCounter;

			/// <summary>Pointer to object pool that stores worker thread objects.</summary>
			GaWorkerThread::GaPoolPtr _pool;

			/// <summary>Generic exception handler for exceptions that occured during the execution of work items but are not stored in result object of those items.</summary>
			Memory::GaSmartPtr<Exceptions::GaExceptionHandler> _exceptionHandler;

		public:

			/// <summary>This constructor creates and initializes thread pool.
			/// Global event pool (see <see cref="GaEventPool" /> class) should be initialized, before user makes any thread pool.</summary>
			/// <param name="poolSize">number of threads in the pool.</param>
			/// <param name="exceptionHandler">pointer to generic exception handler for exceptions that occured during the execution of work items
			/// but are not stored in result object of those items.</param>
			GaThreadPool(int poolSize,
				Exceptions::GaExceptionHandler* exceptionHandler = NULL) : _exceptionHandler(exceptionHandler)
				{ _pool = new GaWorkerThread::GaPool( poolSize, GaWorkerThread::GaWorkerThreadCreate( &_threadCounter ) ); }

			/// <summary>Waits for all threads that belong to this pool to be closed and then frees resources.</summary>
			~GaThreadPool()
			{
				_pool = NULL;
				_threadCounter.WaitForThreads();
			}

			/// <summary><c>ExecuteWork</c> method creates copy of user's work item object (copy is created using <c>MakeCopy</c> method) and creates bound result object.
			/// Then it assigns the item to available thread, or if there is no available threads it creates new.
			///
			/// This method is thread-safe.</summary>
			/// <param name="workItem">pointer to object of work item whose copy is queued.</param>
			/// <param name="makeResultsObject">indicate whether the method should make object that will store results. If it is not required
			/// to store results and users will not wait for the work item to finish this paramenter sould be set to <c>false</c>.</param>
			/// <returns>Method returns newly create result object for work item. User is responsible for destruction of the object.
			/// The object should not be destroyed before calling <c>Wait</c> method. If <c>makeResultsObject</c> parameter is <c>false</c>
			/// this method returns <c>NULL</c>.</returns>
			/// <exception cref="GaNullArgumentException" />Thrown if the <c>workItem</c> is set to <c>NULL</c>.</exception>
			GAL_API
			GaWorkResults* GACALL ExecuteWork(GaWorkItem* workItem,
				bool makeResultsObject = true);

			/// <summary><c>SetSize</c> method sets new number of threads that will be available for the pool.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="size">new number of threads in the pool.</param>
			void GACALL SetSize(int size) { _pool->SetSize( size ); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of thread available in the pool.</returns>
			inline int GACALL GetSize() const { return _pool->GetSize(); }

			/// <summary><c>SetExceptionHandler</c> method sets generic exception handler for exceptions that occured during the execution of work items
			/// but are not stored in result object of those items.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="exceptionHandler">pointer to exception handler.</param>
			inline void GACALL SetExceptionHandler(Exceptions::GaExceptionHandler* exceptionHandler) { _exceptionHandler = exceptionHandler; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to generic exception handler for exceptions that occured during the execution of work items
			/// but are not stored in result object of those items.</returns>
			inline Exceptions::GaExceptionHandler* GACALL GetExceptionHandler() { return _exceptionHandler.GetRawPtr(); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to generic exception handler for exceptions that occured during the execution of work items
			/// but are not stored in result object of those items.</returns>
			inline const Exceptions::GaExceptionHandler* GACALL GetExceptionHandler() const { return _exceptionHandler.GetRawPtr(); }

		};

	} // Threading
} // Common

#endif // __GA_THREAD_POOL_H__


/*! \file Barrier.h
    \brief This file conatins declaration of barrier class.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_BARRIER_H__
#define __GA_BARRIER_H__

#include "EventPool.h"

/// <summary>This macro synchronize access of multiple threads to a block of code by allowing only one thread to execute it and blocking all others until that block is excuted.</summary>
/// <param name="LOCK_NAME">name of <c>GaBarrierLock</c> object.</param>
/// <param name="BARRIER">barrier that should be used for thread synchronization.</param>
/// <param name="COUNT">number of threads that should reach the barrier so it can be released.</param>
#define GA_BARRIER_SYNC(LOCK_NAME, BARRIER, COUNT) if( Common::Threading::GaBarrierLock LOCK_NAME = Common::Threading::GaBarrierLock( &( BARRIER ), COUNT ) )

namespace Common
{
	namespace Threading
	{

		/// <summary><c>GaBarrier</c> class represent synchronization object which blocks threads which enter it until all required thread reach the barrier.</summary>
		class GaBarrier
		{

		private:

			/// <summary>Event used to guard barrier from overlapping usage of the barrier. It blocks barrier from using it again while all threads
			/// exit previsouly initiated barrier.</summary>
			Memory::GaAutoPtr<GaEvent> _event1;

			/// <summary>Event object used to block threads until all reach the barrier.</summary>
			Memory::GaAutoPtr<GaEvent> _event2;

			/// <summary>Number of threads currently waiting for barrier to be released.</summary>
			Common::Threading::GaAtomic<int> _currentCount;

		public:

			/// <summary>This counstructor initializes barrier with thread count.</summary>
			GaBarrier()
			{
				_event1 = GaEventPool::GetInstance()->GetEventWithAutoPtr( GaEvent::GAET_MANUAL );
				_event1->Signal();

				_event2 = GaEventPool::GetInstance()->GetEventWithAutoPtr( GaEvent::GAET_MANUAL );
			}

			/// <summary><c>Enter</c> blocks calling thread until all threads reach the barrier. If this method does not automatically release the barrier, the last thread is not blocked
			/// and it should call <see cref="Release" /> method to release other threads that wait for the barrier.
			///
			/// This method is thread-safe.</summary>
			/// <param name="releaseIfLast">if the parameter is set to <c>true</c> it instructs method to release the barrier when the last thread enters the barrier,
			/// otherwise user should call <see cref="Release" /> method to release the barrier. All calls should have same value of this parameters.</param>
			/// <returns>Method returns <c>true</c> if the thread that called this method is the last required thread for barrier to be released.</returns>
			/// <param name="count">number of threads that should reach the barrier so it can be released.</param>
			GAL_API
			bool GACALL Enter(bool releaseIfLast,
				int count);

			/// <c>Release</c> method should be called if the <c>Enter</c> does not release the barrier. The method release all threads that currently wait for the barrier.<summary>
			///
			/// This method is thread-safe.</summary>
			/// <param name="count">number of threads that should reach the barrier so it can be released.</param>
			inline void GACALL Release(int count)
			{
				if( count > 1 )
				{
					_event2->Signal();
					Exit( count );
				}
			}

		private:

			/// <summary>Exits barrier and if the last thread exits it unblocks barrier so it can be used again.</summary>
			/// <param name="count">number of threads that should reach the barrier so it can be released.</param>
			inline void GACALL Exit(int count)
			{
				if( count > 1 && ++_currentCount == count )
				{
					_currentCount = 0;
					_event2->Reset();
					_event1->Signal();
				}
			}

		};

		/// <summary><c>GaBarrierLock</c> class provides mechanism to synchronize access of multiple threads to a block of code by allowing only
		/// one thread to execute it and blocking all others until that block is excuted.</summary>
		class GaBarrierLock
		{

		private:

			/// <summary>Pointer to barrier on which this lock operates.</summary>
			GaBarrier* _barrier;

			/// <summary>Indicates that the thread which has entered the barrier is the last thread required to release barrier.</summary>
			bool _last;

			/// <summary>Number of threads that should reach the barrier so it can be released.</summary>
			int _count;

		public:

			/// <summary>Blocks all threads that construct barrier lock except the last one </summary>
			/// <param name="barrier">pointer to barrier on which this lock operates.</param>
			/// <param name="count">number of threads that should reach the barrier so it can be released.</param>
			GaBarrierLock(GaBarrier* barrier,
				int count) : _barrier(barrier),
				_count(count) { _last = _barrier->Enter( false, count ); }

			/// <summary>Destructor releases barriere if this lock belongs to the last thread that entered the barrier.</summary>
			~GaBarrierLock()
			{
				if( _last )
					_barrier->Release( _count );
			}

			/// <summary>This operator is thread-safe.</summary>
			/// <returns>Operator returns <c>true</c> if this lock belongs to the last thread that entered the barrier.</returns>
			inline GACALL operator bool() { return _last; }

		};

	} // Threading
} // Common

#endif // __GA_BARRIER_H__

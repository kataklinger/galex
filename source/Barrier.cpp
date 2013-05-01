
/*! \file Barrier.cpp
    \brief This file conatins implementation of barrier class.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#include "Barrier.h"

namespace Common
{
	namespace Threading
	{

		// Blocks thread untill all threads reache the barrier
		bool GaBarrier::Enter(bool releaseIfLast,
			int count)
		{
			// don't lock barrier one thread only
			if( count > 1 )
			{
				// wait for previous barrier to finish
				_event1->Wait();

				// sets number of threads that should reach barrier
				_currentCount.CompareAndExchange( 0, count );

				// decrease number of thread that should reach the barrier before it can be released
				bool last = --_currentCount == 0;

				// all threads have reached barrier?
				if( last )
				{
					// block usage of this barrier until all waiting threads exit current barrier
					_event1->Reset();

					// should the last thread that enter the barrier release other threads?
					if( !releaseIfLast )
						return true;

					// release threads waiting for barrier
					_event2->Signal();
				}
				else
					// wait for other threads to reach barrier
					_event2->Wait();

				// exit barrier and wakes all thread that waits
				Exit( count );

				return last;
			}
			else
				return true;
		}

	} // Threading
} // Common
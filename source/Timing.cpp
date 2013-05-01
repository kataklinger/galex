
/*! \file Timing.cpp
    \brief This file contains implementations for classes that provides services for measuring time.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#include "Timing.h"

namespace Common
{
	namespace Timing
	{

		// Restarts stopwatch
		long long GaStopwatchLowRes::Restart()
		{
			long long old = _start;
			_start = clock();
			return TimeDiff( old, _start );
		}

		#if defined(GAL_PLATFORM_WIN)

		// Initializes stop watch
		GaStopwatchHiRes::GaStopwatchHiRes()
		{
			QueryPerformanceFrequency( &_frquency );
			QueryPerformanceCounter( &_start );
		}

		// Restarts stopwatch
		long long GaStopwatchHiRes::Restart()
		{
			LARGE_INTEGER old = _start;

			// store time of restart
			QueryPerformanceCounter( &_start );

			// calculate time that has passed since previous restart
			return TimeDiff( _start, old );
		}

		// Returns current time
		long long GaStopwatchHiRes::GetClock() const
		{
			// current time
			LARGE_INTEGER end;
			QueryPerformanceCounter( &end );

			// calculate time that has passed since last restart
			return TimeDiff( end, _start );
		}

		#elif defined(GAL_PLATFORM_NIX)

		// Initializes stop watch
		GaStopwatchHiRes::GaStopwatchHiRes()
		{
			timezone tz;
			gettimeofday( &_start, &tz );
		}

		// Restarts stopwatch
		long long GaStopwatchHiRes::Restart()
		{
			timeval old = _start;

			// store time of restart
			timezone tz;
			gettimeofday( &_start, &tz );

			// calculate time that has passed since previous restart
			return TimeDiff( _start, old );
		}

		// Returns current time
		long long GaStopwatchHiRes::GetClock() const
		{
			timeval end;

			// current time
			timezone tz;
			gettimeofday( &end, &tz );

			// calculate time that has passed since last restart
			return TimeDiff( end, _start );
		}

		#endif

	} // Timing
} // Common


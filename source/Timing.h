
/*! \file Timing.h
    \brief This file contains declarations for classes that provides services for measuring time.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_TIMING_H__
#define __GA_TIMING_H__

#include <ctime>
#include "Platform.h"

#if defined(GAL_PLATFORM_WIN)

	#include <windows.h>

#elif defined(GAL_PLATFORM_NIX)

	#include <sys/time.h>

#endif

namespace Common
{
	/// <summary>Contains classes that provides time measurement services.</summary>
	namespace Timing
	{

		/// <summary><c>GaStopwatch</c> class is interface for time time measurement services.</summary>
		class GaStopwatch
		{

		public:

			/// <summary><c>Restart</c> method sets stopwatch to time to <c>0</c>.</summary>
			/// <returns>Method returns amount of time that has passed since previous restart in microseconds.</returns>
			virtual long long GACALL Restart() = 0;

			/// <summary><c>GetClock</c> method queries time that has passed since laststopwatch restart.</summary>
			/// <returns>Method returns amount of time that has passed in microseconds.</returns>
			virtual long long GACALL GetClock() const = 0;

		};

		/// <summary><c>GaStopwatchLowRes</c> class provides time measurement with lower accuracy that can be used for timing longer events.
		///
		/// This class has built-in synchronizator so it is allowed to use <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros
		/// with instances of this class, but no public or private methods are thread-safe.</summary>
		class GaStopwatchLowRes : public GaStopwatch
		{

		private:

			/// <summary>Time of the last stopwatch restart.</summary>
			long long _start;

		public:

			/// <summary>Initializes stopwatch.</summary>
			GaStopwatchLowRes() { _start = clock(); }

			/// <summary>More details are given in specification of <see cref="GaStopwatch::Restart" /> method.
			///
			/// This method is not thread-safe.</summary>
			GAL_API
			virtual long long GACALL Restart();

			/// <summary>More details are given in specification of <see cref="GaStopwatch::GetClock" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual long long GACALL GetClock() const { return TimeDiff( _start, clock() ); }

		private:

			/// <summary><c>TimeDiff</c> calculates time difference between to two events.
			///
			/// This method is thread-safe.</summary>
			/// <param name="start">time of the first event.</param>
			/// <param name="end">time of the second event.</param>
			/// <returns>Method returns time difference im microsecond.</returns>
			inline long long GACALL TimeDiff(long long start,
				long long end) const { return ( end - start ) * 1000; }

		};

		#if defined(GAL_PLATFORM_WIN)

		/// <summary><c>GaStopwatchLowRes</c> class provides time measurement with higher accuracy that can be used for timing shorter events.
		///
		/// This class has built-in synchronizator so it is allowed to use <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros
		/// with instances of this class, but no public or private methods are thread-safe.</summary>
		class GaStopwatchHiRes : public GaStopwatch
		{

		private:

			/// <summary>Time of the last stopwatch restart.</summary>
			LARGE_INTEGER _start;

			/// <summary>Frequency of used timer.</summary>
			LARGE_INTEGER _frquency;

		public:

			/// <summary>Initializes stopwatch.</summary>
			GAL_API
			GaStopwatchHiRes();

			/// <summary>More details are given in specification of <see cref="GaStopwatch::Restart" /> method.
			///
			/// This method is not thread-safe.</summary>
			GAL_API
			virtual long long GACALL Restart();

			/// <summary>More details are given in specification of <see cref="GaStopwatch::GetClock" /> method.
			///
			/// This method is not thread-safe.</summary>
			GAL_API
			virtual long long GACALL GetClock() const;

		private:

			/// <summary><c>TimeDiff</c> calculates time difference between to two events.
			///
			/// This method is thread-safe.</summary>
			/// <param name="start">time of the first event.</param>
			/// <param name="end">time of the second event.</param>
			/// <returns>Method returns time difference im microsecond.</returns>
			inline long long GACALL TimeDiff(LARGE_INTEGER start,
				LARGE_INTEGER end) const { return ( end.QuadPart - start.QuadPart ) * 10000000 / _frquency.QuadPart; }

		};

		#elif defined(GAL_PLATFORM_NIX)

		/// <summary><c>GaStopwatchLowRes</c> class provides time measurement with higher accuracy that can be used for timing shorter events.
		///
		/// This class has built-in synchronizator so it is allowed to use <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros
		/// with instances of this class, but no public or private methods are thread-safe.</summary>
		class GaStopwatchHiRes : public GaStopwatch
		{

		private:

			/// <summary>Time of the last stopwatch restart.</summary>
			timeval _start;

		public:

			/// <summary>Initializes stopwatch.</summary>
			GAL_API
			GaStopwatchHiRes();

			/// <summary>More details are given in specification of <see cref="GaStopwatch::Restart" /> method.
			///
			/// This method is not thread-safe.</summary>
			GAL_API
			virtual long long GACALL Restart();

			/// <summary>More details are given in specification of <see cref="GaStopwatch::GetClock" /> method.
			///
			/// This method is not thread-safe.</summary>
			GAL_API
			virtual long long GACALL GetClock() const;

		private:

			/// <summary><c>TimeDiff</c> calculates time difference between to two events.
			///
			/// This method is thread-safe.</summary>
			/// <param name="start">time of the first event.</param>
			/// <param name="end">time of the second event.</param>
			/// <returns>Method returns time difference im microsecond.</returns>
			inline long long GACALL TimeDiff(const timeval& start,
				const timeval& end) const { return ( (long long)end.tv_sec - start.tv_sec ) * 1000000 + end.usec - start.usec; }

		};

		#elif __GAL_DOCUMENTATION__

		/// <summary><c>GaStopwatchLowRes</c> class provides time measurement with higher accuracy that can be used for timing shorter events.
		///
		/// This class has built-in synchronizator so it is allowed to use <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros
		/// with instances of this class, but no public or private methods are thread-safe.</summary>
		class GaStopwatchHiRes : public GaStopwatch
		{

		public:

			/// <summary>Initializes stopwatch.</summary>
			GaStopwatchHiRes() { }

			/// <summary>More details are given in specification of <see cref="GaStopwatch::GetClock" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual long long GACALL Restart() { }

			/// <summary>More details are given in specification of <see cref="GaStopwatch::GetClock" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual long long GACALL GetClock() const { }

		};

		#endif

	} // Timing
} // Common

#endif // __GA_TIMING_H__

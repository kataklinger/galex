
/*! \file StopCriteria.cpp
    \brief This file implements classes that represent stop criterias for genetic algorithms.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#include "StopCriteria.h"

namespace Algorithm
{
	namespace StopCriteria
	{

		// Relative progress stop criteria
		bool GaStatsRelativeProgressCriterion::operator ()(const Statistics::GaStatistics& stats,
			const GaStopCriterionParams& params,
			const GaStopCriterionConfig& config,
			Common::Workflows::GaBranch* branch) const
		{
			const GaStatsRelativeProgressCriterionParams& p = (const GaStatsRelativeProgressCriterionParams&)params;

			// enough depth?
			const Statistics::GaValueHistoryBase& statValue = stats.GetValue( p.GetValueID() );
			if( statValue.GetCurrentHistoryDepth() < p.GetDepth() )
				return false;

			// check progress
			return IsCriterionReached( statValue.GetRelativeProgress( p.GetDepth() ), p.GetDesiredValue(), p.GetComparator(), p.GetStopType() );
		}

	} // StopCriteria
} // Algorithm

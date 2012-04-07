
/*! \file VEGA.cpp
    \brief This file conatins implementation of classes that implement Vector Evaluated Genetic Algorithm (VEGA).
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#include "FitnessComparators.h"
#include "PopulationStatistics.h"
#include "VEGA.h"

namespace Multiobjective
{
	namespace VEGA
	{

		/// <summary><c>GaVEGAAssignFitness</c> class represents operation that assigns scaled fitness to chromosome by using selection probability base of raw fitness.</summary>
		class GaVEGAAssignFitness
		{

		public:

			/// <summary><c>operator ()</c> perform assigning scaled fitness value to chromosome.</summary>
			/// <param name="chromosome">chromosome whose scaled fitness should be set.</param>
			/// <param name="index">index of the the chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				( (Fitness::Representation::GaSVFitness<float>&)chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_SCALED ) ).SetValue(
					chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_RAW ).GetProbabilityBase() );
			}

		};

		// Execute scaling operation
		void GaVEGA::operator ()(Population::GaPopulation& population,
			const Population::GaScalingParams& parameters,
			const Population::GaScalingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			Population::GaOperationTime timer( population, Population::GADV_SCALING_TIME );

			// assign scaled fitness
			Common::Workflows::GaParallelExec1<Population::GaPopulation, Population::GaChromosomeStorage> popWorkDist( *branch, population );
			popWorkDist.Execute( GaVEGAAssignFitness(), false );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
			{
				// in next generation use next value of raw fitness as probability base
				( ( Fitness::Representation::GaMVFitnessParams& )population.GetFitnessParams() ).NextProbabilityIndex();

				// mark population as rescaled
				population.GetFlags().SetFlags( Population::GaPopulation::GAPF_COMPLETE_SCALED_FITNESS_UPDATE );
			}

			// update operation time statistics
			timer.UpdateStatistics();
		}

	} // VEGA
} // Multiobjective

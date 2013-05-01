
/*! \file Couplings.cpp
    \brief This file implements classes that represent coupling operations.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#include "GlobalRandomGenerator.h"
#include "PopulationStatistics.h"
#include "Couplings.h"

namespace Population
{
	namespace CouplingOperations
	{

		/// <summary><c>GaCouplingFlow</c> function is template for coupling operation. It provides customization of the way the parent pairs are made.</summary>
		/// <typeparam name="TO_INDEX">type of functor.</typeparam>
		/// <param name="toIndex">functor that defines the way parent pairs are made.</param>
		/// <param name="input">group that stores parent chromosomes.</param>
		/// <param name="output">group that will store offspring chromosomes.</param>
		/// <param name="parameters">parameters of coupling operation.</param>
		/// <param name="configuration">configuration of the coupling operation.</param>
		/// <param name="branch">branch that executes couling operation.</param>
		template<typename TO_INDEX>
		void GACALL GaCouplingFlow(const TO_INDEX& toIndex,
			GaChromosomeGroup& input,
			GaChromosomeGroup& output,
			const GaCouplingParams& parameters,
			const GaCouplingConfig& configuration,
			Common::Workflows::GaBranch* branch)
		{
			GaPopulation& population = *input.GetPopulation();

			GaCouplingCounters counters( population, GADV_COUPLING_TIME );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
				output.Clear();

			const Chromosome::GaMatingConfig& matingConf = configuration.GetMating().GetConfiguration();
			int parentPerCrossover = matingConf.GetParentCount(), offspringCount = matingConf.GetOffspringCount();

			GaCrossoverStorageBuffer& crossoverBuffer = population.GetTagByID<GaPartialCrossoverBuffer>( parameters.GetCrossoverBuffersTagID() )[ branch->GetFilteredID() ];
			GaChromosomeGroup& parents = crossoverBuffer.GetParents();
			GaChromosomeGroup& offspring = crossoverBuffer.GetOffspring();

			// get workload for this branch
			int start, work;
			branch->SplitWork( parameters.GetNumberOfOffsprings() / offspringCount, work, start );
			int end = start + work;

			int round = 0, parentCount = output.GetCount();
			for( int i = start; i < end; i++ )
			{
				// get enough parents to perofrm to produce offspring usign mating operation
				int s = i * parentPerCrossover;
				for( int j = 0; j < parentPerCrossover; j++ )
					parents.Add( &parents[ toIndex( s + j, parentCount ) ] );

				// produce offspring
				configuration.Mate( crossoverBuffer );

				// store offspring to result set
				while( GaChromosomeStorage* child = offspring.PopLast( true ) )
					output.AddAtomic( child );

				// update operation counters
				counters.CollectCrossoverBufferCounters( crossoverBuffer );

				crossoverBuffer.Clear();
			}

			// update population statistics with new state of counters
			counters.UpdateStatistics();
		}

		/// Chooses the first and the second chromosome in result set as pair for mating, than the third and the forth and so on.
		void GaSimpleCoupling::operator ()(GaChromosomeGroup& input,
			GaChromosomeGroup& output,
			const GaCouplingParams& parameters,
			const GaCouplingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			struct ToIndex { inline int GACALL operator ()(int index, int size) const { return index % size; } };
			GaCouplingFlow( ToIndex(), input, output, parameters, configuration, branch );
		}

		// Chooses the best and the worst chromosomes in selection result set as pair for mating, then selects the second best and the second worst and so on.
		void GaInverseCoupling::operator ()(GaChromosomeGroup& input,
			GaChromosomeGroup& output,
			const GaCouplingParams& parameters,
			const GaCouplingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			struct ToIndex { inline int GACALL operator ()(int index, int size) const { return index % 2 ? size - 1 - ( index / 2 ) % size : ( index / 2 ) % size; } };
			GaCouplingFlow( ToIndex(), input, output, parameters, configuration, branch );
		}

		// Chooses chromosomes pairs from selection result set randomly for mating.
		void GaRandomCoupling::operator ()(GaChromosomeGroup& input,
			GaChromosomeGroup& output,
			const GaCouplingParams& parameters,
			const GaCouplingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
				input.Shuffle( true );

			struct ToIndex { inline int GACALL operator ()(int index, int size) const { return index < size ? index : GaGlobalRandomIntegerGenerator->Generate( size - 1 ); } };
			GaCouplingFlow( ToIndex(), input, output, parameters, configuration, branch );
		}

	} // CouplingOperations
} // Population

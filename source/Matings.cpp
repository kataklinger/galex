
/*! \file Matings.cpp
\brief This file implements classes that represent mating operations.
*/

/*
* 
* website: N/A
* contact: kataklinger@gmail.com
*
*/

#include "Matings.h"

namespace Chromosome
{
	namespace MatingOperations
	{

		void GaBasicMatingOperation::operator ()(const GaMatingConfig& operations,
			GaCrossoverBuffer& crossoverBuffer,
			const GaMatingParams& parameters) const
		{
			// perform crossover and mutation operations
			bool crossoverPerformed = PerformCrossover( operations, crossoverBuffer );
			PerformMutation( operations, crossoverBuffer, crossoverPerformed );
		}

		// Perfomrs crossover operation
		bool GaBasicMatingOperation::PerformCrossover(const GaMatingConfig& operations,
			GaCrossoverBuffer& crossoverBuffer) const
		{
			// should the crossover be perofrmed
			if( operations.CrossoverProbability( crossoverBuffer ) )
			{
				operations.Crossover( crossoverBuffer );
				return true;
			}
			else
			{
				// crossover is not preformed - offsprings are same as parents
				int parentCount = operations.GetParentCount();
				for( int i = operations.GetOffspringCount() - 1; i >= 0; i-- )
					crossoverBuffer.StoreOffspringChromosome( GaChromosomePtr::NullPtr, i % parentCount );

				return false;
			}
		}

		// Perfomrs mutation operation
		void GaBasicMatingOperation::PerformMutation(const GaMatingConfig& operations,
			GaCrossoverBuffer& crossoverBuffer,
			bool crossoverPerformed) const
		{
			// try to perform mutation on all chromosomes
			bool improvingMutations = operations.GetMutation().GetParameters().GetImprovingMutationsFlag();
			for( int i = operations.GetOffspringCount() - 1; i >= 0; i-- )
			{
				GaChromosomePtr chromosome = crossoverBuffer.GetOffspringChromosome( i );

				// should the mutation be performed on the chromosome
				if( operations.MutationProbability( chromosome ) )
				{
					// make copy of offspring chromosome if it is just a reference to it's parent
					if( !crossoverPerformed )
						chromosome = chromosome->Clone();
					// prepare chromosomes for mutation
					else if( improvingMutations )
						chromosome->MutationEvent( GaChromosome::GAME_PREPARE );

					operations.Mutation( chromosome );

					GaChromosome::GaMuataionEvent evt = GaChromosome::GAME_ACCEPT;
					if( improvingMutations )
					{
						// check whether the mutation has improved chromosome
						if( !crossoverBuffer.ReplaceIfBetter( i, chromosome ) )
							// reject if it made the chromosome worse
							evt = GaChromosome::GAME_REJECT;
					}
					else
						// always accept mutation
						crossoverBuffer.ReplaceOffspringChromosome( i, chromosome );

					// handle mutation acceptanece or rejection
					if( crossoverPerformed )
						chromosome->MutationEvent( evt );
				}
			}
		}

		//void GaAdvanceMatingOperation::operator ()(const GaMatingConfig& operations,
		//	GaCrossoverBuffer& crossoverBuffer,
		//	const GaMatingParams& parameters) const
		//{
		//	GaBasicMatingOperation::operator ()( operations, crossoverBuffer, parameters );
		//}

	} // MatingOperations
} // Chromosome


/*! \file Replacements.cpp
    \brief This file implements classes that represent replacement operations.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#include "Population.h"
#include "Replacements.h"
#include "RandomSequence.h"
#include "PopulationStatistics.h"

namespace Population
{
	namespace ReplacementOperations
	{

		/// <summary><c>GaCheckDuplicatesOp</c> class represents operation that identfies chromosomes that are duplicates.</summary>
		class GaCheckDuplicatesOp
		{

		private:

			/// <summary>Population against which the chromosomes are checked.</summary>
			const GaPopulation& _population;

			/// <summary>Comparator that is used for comparing chromosomes.</summary>
			const Chromosome::GaChromosomeComparatorSetup& _comparator;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="population">population against which the chromosomes are checked.</param>
			/// <param name="comparator">comparator that will be used for comparing chromosomes.</param>
			GaCheckDuplicatesOp(const GaPopulation& population,
				const Chromosome::GaChromosomeComparatorSetup& comparator) : _population(population),
				_comparator(comparator) { }

			/// <summary><c>operator ()</c> checks whether the chromosomes is a duplicate of a chromosome in the population.</summary>
			/// <param name="chromosome">chromosome that should be checked.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(GaChromosomeStorage& chromosome,
				int index)
			{
				// chromosome is alredy identified as duplicate?
				if( !chromosome.GetFlags().IsFlagSetAny( GaChromosomeStorage::GACF_REMOVE_CHROMOSOME ) )
				{
					// compare chromosome against the ones in population
					for( int i = _population.GetCount() - 1; i >= 0; i-- )
					{
						// chromosomes are the same?
						if( _comparator.GetOperation().Equal( *chromosome.GetChromosome(), *_population[ i ].GetChromosome(), _comparator.GetParameters() ) )
						{
							// new chromosome is duplicate and it should not be inserted
							chromosome.GetFlags().SetFlags( GaChromosomeStorage::GACF_REMOVE_CHROMOSOME );
							break;
						}
					}
				}
			}

			/// <summary><c>operator ()</c> checks whether one chromosomes against the other and marks one of them for removal if they are the same.</summary>
			/// <param name="chromosome1">chromosome against the check is performed.</param>
			/// <param name="chromosome2">chromosome that is checked whether it is duplicate.</param>
			/// <param name="index1">index of the first chromosome.</param>
			/// <param name="index2">index of the second chromosome.</param>
			inline void GACALL operator ()(GaChromosomeStorage& chromosome1,
				GaChromosomeStorage& chromosome2,
				int index1,
				int index2)
			{
				// compare chromosomes but skip the ones that are already marked as duplicates
				if( !chromosome1.GetFlags().IsFlagSetAny( GaChromosomeStorage::GACF_REMOVE_CHROMOSOME ) &&
					!chromosome2.GetFlags().IsFlagSetAny( GaChromosomeStorage::GACF_REMOVE_CHROMOSOME ) &&
					_comparator.GetOperation().Equal( *chromosome1.GetChromosome(), *chromosome2.GetChromosome(), _comparator.GetParameters() ) )
					// new chromosome is duplicate and it should not be inserted
					chromosome2.GetFlags().SetFlags( GaChromosomeStorage::GACF_REMOVE_CHROMOSOME );
			}

		};

		/// <summary><c>IdentifyDuplicates</c> function identifies and marks duplicates in the group of chromosomes that should be inserted into population.</summary>
		/// <param name="input">group of chromosomes that should be inserted int population.</param>
		/// <param name="population">population into which the chromosomes should be inserted.</param>
		/// <param name="comparator">comparator that will be used gor comparing chromosomes against each other.</param>
		/// <param name="branch">work flow branch that executes the operation.</param>
		void GACALL IdentifyDuplicates(GaChromosomeGroup& input,
			const GaPopulation& population,
			const Chromosome::GaChromosomeComparatorSetup& comparator,
			Common::Workflows::GaBranch* branch)
		{
			// identify duplicates ofly if chromosome comparator is supplied
			if( comparator.HasOperation() )
			{
				Common::Workflows::GaParallelExec1<GaChromosomeGroup, GaChromosomeStorage> popWorkDist( *branch, input );
				popWorkDist.Execute( GaCheckDuplicatesOp( population, comparator ), false );
			}
		}

		/// <summary><c>GetReplacementSize</c> function calculates allowed replacement size.</summary>
		/// <param name="input">group that contains offspring chromosomes.</param>
		/// <param name="population">population over which the replacement is performed.</param>
		/// <param name="parameters">paramenters of replacement operation.</param>
		/// <returns>Method returns replacement size.</returns>
		inline int GACALL GetReplacementSize(GaChromosomeGroup& input,
			GaPopulation& population,
			const GaReplacementParams& parameters)
		{
			int size = parameters.GetReplacementSize() < population.GetCount() ? parameters.GetReplacementSize() : population.GetCount();
			return size <= input.GetCount() ? size : input.GetCount();
		}

		/// <summary><c>PrepareInput</c> function removes maked chromosomes from input group and trims it to required size.</summary>
		/// <param name="input">chromosome group that should be prepared.</param>
		/// <param name="population">population on which the replacement operation is performed.</param>
		/// <param name="parameters">paraemters of replacement operation.</param>
		void GACALL PrepareInput(GaChromosomeGroup& input,
			GaPopulation& population,
			const GaReplacementParams& parameters)
		{
			input.Remove();
			input.Trim( GetReplacementSize( input, population, parameters ) );
		}

		// Replaces worst chromosomes in the population
		void GaWorstReplacement::operator ()(GaChromosomeGroup& input,
			GaPopulation& population,
			const GaReplacementParams& parameters,
			const GaReplacementConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			GaOperationTime timer( population, GADV_REPLACEMENT_TIME );

			IdentifyDuplicates( input, population, configuration.GetChromosomeComparator(), branch );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
			{
				// trim offspring count to replacement size
				PrepareInput( input, population, parameters );

				// remove worst chromosomes and insert offspring chromosomes
				population.Remove();
				population.Trim( input.GetCount() );
				population.Insert( input.GetChromosomes(), input.GetCount() );

				input.Clear( true );
			}

			// update operation time statistics
			timer.UpdateStatistics();
		}

		// Inserts new chromosomes to crowding area of th epopulation without removing existing chromosomes
		void GaCrowdingReplacement::operator ()(GaChromosomeGroup& input,
			GaPopulation& population,
			const GaReplacementParams& parameters,
			const GaReplacementConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			GaOperationTime timer( population, GADV_REPLACEMENT_TIME );

			IdentifyDuplicates( input, population, configuration.GetChromosomeComparator(), branch );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
			{
				PrepareInput( input, population, parameters );

				// remove chromosomes in crwding area and insert new chromosomes
				population.Remove();
				population.Trim();
				population.Insert( input.GetChromosomes(), input.GetCount() );

				input.Clear( true );
			}

			// update operation time statistics
			timer.UpdateStatistics();
		}

		// Prepares data for replacement operation
		void GaRandomReplacement::Prepare(GaChromosomeGroup& input,
			GaPopulation& population,
			const GaReplacementParams& parameters,
			const GaReplacementConfig& configuration,
			int branchCount) const
		{
			population.GetTagManager().AddTag( ( (const GaRandomReplacementParams&)parameters ).GetIndicesBufferTagID(),
				Common::Data::GaSizableTagLifecycle<Common::Data::GaSingleDimensionArray<int> >( population.GetPopulationParams().GetPopulationSize() ) );
		}

		// Clears population from data used by the operation
		void GaRandomReplacement::Clear(GaChromosomeGroup& input,
			GaPopulation& population,
			const GaReplacementParams& parameters,
			const GaReplacementConfig& configuration,
			int branchCount) const { population.GetTagManager().RemoveTag( ( (const GaRandomReplacementParams&)parameters ).GetIndicesBufferTagID() ); }

		// Replaces random chromosomes
		void GaRandomReplacement::Exec(GaChromosomeGroup& input,
			GaPopulation& population,
			const GaRandomReplacementParams& parameters,
			const GaReplacementConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			GaOperationTime timer( population, GADV_REPLACEMENT_TIME );

			IdentifyDuplicates( input, population, configuration.GetChromosomeComparator(), branch );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
			{
				PrepareInput( input, population, parameters );

				Common::Data::GaSingleDimensionArray<int>& indices = population.GetTagByID<Common::Data::GaSingleDimensionArray<int> >( parameters.GetIndicesBufferTagID() );
				indices.SetSize( population.GetPopulationParams().GetPopulationSize() );

				// choose random chromosomes from the population and replace them with offspring chromosomes
				if( input.GetCount() )
				{
					Common::Random::GaGenerateRandomSequenceAsc( parameters.GetElitism(), population.GetCount() - 1, input.GetCount(), indices.GetArray() );
					population.Replace( indices.GetArray(), input.GetChromosomes(), input.GetCount() );
				}

				input.Clear( true );
			}

			// update operation time statistics
			timer.UpdateStatistics();
		}

		// Replaces parent chromosomes with their children
		void GaParentReplacement::operator ()(GaChromosomeGroup& input,
			GaPopulation& population,
			const GaReplacementParams& parameters,
			const GaReplacementConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			GaOperationTime timer( population, GADV_REPLACEMENT_TIME );

			IdentifyDuplicates( input, population, configuration.GetChromosomeComparator(), branch );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
			{
				PrepareInput( input, population, parameters );

				for( int i = input.GetCount() - 1; i >= 0; i-- )
					input[ i ].GetParent()->GetFlags().SetFlags( Population::GaChromosomeStorage::GACF_REMOVE_CHROMOSOME );

				// replaces parent chromosomes with offspring
				population.Remove();
				population.Insert( input.GetChromosomes(), input.GetCount() );

				input.Clear( true );
			}

			// update operation time statistics
			timer.UpdateStatistics();
		}

		// Replaces whole population of chromosomes
		void GaPopulationReplacement::Exec(GaChromosomeGroup& input,
			GaPopulation& population,
			const GaElitismReplacementParams& parameters,
			const GaReplacementConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			GaOperationTime timer( population, GADV_REPLACEMENT_TIME );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
			{
				// remove all chromosome except the defined number of the best chromosomes in current generation
				population.Trim( population.GetCount() - parameters.GetElitism() );

				// how much offspring chromosomes should be inserted
				int size = population.GetPopulationParams().GetPopulationSize() - population.GetCount();
				if( input.GetCount() < size  )
					size = input.GetCount();

				// trim offspring count to replacement size
				input.Trim( size );

				// insert offspring chromosomes and remove crowded chromosomes from previous generation
				population.Insert( input.GetChromosomes(), size );

				input.Clear( true );
			}

			// update operation time statistics
			timer.UpdateStatistics();
		}

	} // ReplacementOperations
} // Population

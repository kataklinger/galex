
/*! \file Selections.cpp
    \brief This file implements classes that represent selection operations.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#include "RandomSequence.h"
#include "Population.h"
#include "PopulationStatistics.h"
#include "Selections.h"

namespace Population
{
	namespace SelectionOperations
	{

		/// <summary><c>GetSelectedChromosome</c> function returns chromosomes storage object at specifed index in population and marks it as selected.</summary>
		/// <param name="population">population tho whic the chromosome belongs.</param>
		/// <param name="selectedIndex">index of queried chromosome.</param>
		/// <param name="duplicate">tag that contains information whether the chromosomes is already selected or not. If tag does not exist chromosome is always selected.</param>
		/// <returns>Method returns pointer to storage object if wasn;t already selected, otherwise it returns <c>NULL</c>.</returns>
		inline GaChromosomeStorage* GACALL GetSelectedChromosome(GaPopulation& population,
			int selectedIndex,
			GaChromosomeTagGetter<Common::Threading::GaAtomic<int> >& duplicate)
		{
			GaChromosomeStorage& selected = population[ selectedIndex ];

			// mark chromosomes as selected if needed
			return !duplicate.IsExist() || duplicate( selected ).CompareAndExchange( 0, 1 ) ? &selected : NULL;
		}

		/// <summary><c>StoreSelectedChromosome</c> function stores selected chromosome in selection result set.
		/// If crossover buffer is specified this operation puts chromosomes in it instead of result set and when the buffer is fuul mating operation 
		/// is performed and offspring chromosomes are stored in result set.</summary>
		/// <param name="chromosome">chromosome that should be stored to selection result set.</param>
		/// <param name="output">selection result set.</param>
		/// <param name="crossoverBuffer">crossover buffer that will be used for producing offspring chromosomes. If this parameter is set <c>NULL</c>,
		/// specified chromosome will be put streight into result set.</param>
		/// <param name="counters">counters of performed operations.</param>
		/// <param name="configuration">selection configuration.</param>
		void GACALL StoreSelectedChromosome(GaChromosomeStorage* chromosome,
			GaChromosomeGroup& output,
			GaCrossoverStorageBuffer* crossoverBuffer,
			GaCouplingCounters& counters,
			const GaCouplingConfig& configuration)
		{
			// perform mating?
			if( crossoverBuffer )
			{
				// add chromosome to crossover buffer
				GaChromosomeGroup& parents = crossoverBuffer->GetParents(); 
				if( parents.Add( chromosome ) + 1 == parents.GetSize() )
				{
					// buffer is full - performe crossover
					configuration.Mate( *crossoverBuffer );

					// add offspring chromosomes to output buffer
					GaChromosomeGroup& offspring = crossoverBuffer->GetOffspring(); 
					while( GaChromosomeStorage* child = offspring.PopLast( true ) )
						output.AddAtomic( child );

					// update operation counters
					counters.CollectCrossoverBufferCounters( *crossoverBuffer );
					crossoverBuffer->Clear();
				}
			}
			else
				// just add selected chromosome to result set
				output.AddAtomic( chromosome );
		}

		/// <summary><c>GaClearDuplicateFlagsOp</c> class represents operation that clears chromosomes tag which indicates whether the chromosome was selected.</summary>
		class GaClearDuplicateFlagsOp
		{

		private:

			/// <summary>Tag that stores information whether the chromosome was selected.</summary>
			GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > _duplicate;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="duplicate">tag that stores information whether the chromosome was selected.</param>
			GaClearDuplicateFlagsOp(const GaChromosomeTagGetter<Common::Threading::GaAtomic<int> >& duplicate) : _duplicate(duplicate) { }

			/// <summary><c>operator ()</c> clears specified chromosome tag.</summary>
			/// <param name="chromosome">chromosome whose flag should be cleared.</param>
			/// <param name="index">index of the chromosomes.</param>
			inline void GACALL operator ()(GaChromosomeStorage& chromosome,
				int index) { _duplicate( chromosome ) = 0; }

		};

		/// <summary><c>ClearDuplicateFlags</c> function clears tags of all chromosomes in populatio indicate whether the chromosomes were selected.</summary>
		/// <param name="population">populations whose chromosomes should be updates.</param>
		/// <param name="duplicate">tag that stores information whether the chromosome was selected.</param>
		/// <param name="branch">workflow branch that executes selecteion operation.</param>
		inline void GACALL ClearDuplicateFlags(GaPopulation& population,
			const GaChromosomeTagGetter<Common::Threading::GaAtomic<int> >& duplicate,
			Common::Workflows::GaBranch* branch)
		{
			// is clearing needed?
			if( duplicate.IsExist() )
			{
				branch->GetBarrier().Enter( true, branch->GetBarrierCount() );

				// clear tags for all chromosomes
				Common::Workflows::GaParallelExec1<GaPopulation, GaChromosomeStorage> workDist( *branch, population );
				workDist.Execute( GaClearDuplicateFlagsOp( duplicate ), true );
			}
		}

		/// <summary><c>GetCrossoverBuffer</c> function returns crossover buffer that should be used by workflow branch to perform mating.</summary>
		/// <param name="population">population over which the selection operation is performed.</param>
		/// <param name="parameters">paramenters of selection operation.</param>
		/// <param name="branch">workflow branch that performs selection operation.</param>
		/// <returns>Method returns pointer to crossover buffer that workflow branch should use or <c>NULL</c> if selection operation should not produce offspring chromosomes.</returns>
		inline GaCrossoverStorageBuffer* GACALL GetCrossoverBuffer(GaPopulation& population,
			const GaSelectionParams& parameters,
			Common::Workflows::GaBranch* branch)
		{
			int crossoverBufferTagID = parameters.GetCrossoverBuffersTagID();
			return crossoverBufferTagID < 0 ? NULL : &population.GetTagByID<GaPartialCrossoverBuffer>( crossoverBufferTagID )[ branch->GetFilteredID() ];
		}

		// Performs selection of the first N chromosomes in the population
		void GaTopSelection::operator ()(GaPopulation& population,
			GaChromosomeGroup& output,
			const GaSelectionParams& parameters,
			const GaCouplingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			GaSelectionCounters counters( population, GADV_SELECTION_TIME );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
				output.Clear();

			GaCrossoverStorageBuffer* crossoverBuffer = GetCrossoverBuffer( population, parameters, branch );

			// get workload for this branch
			int count, start;
			branch->SplitWork( GetSelectionCount( parameters, &configuration.GetMating().GetConfiguration() ), count, start );

			// fill result set
			for( int i = start + count - 1; i >= start; i-- )
			{
				// store selected chromosome to result set
				StoreSelectedChromosome( &population[ i ], output, crossoverBuffer, counters, configuration );

				// update selection counter
				counters.IncSelectionCounter();
			}

			// update population statistics with new state of counters
			counters.UpdateStatistics();
		}

		// Performs selection of the last N chromosomes in the population
		void GaBottomSelection::operator ()(GaPopulation& population,
			GaChromosomeGroup& output,
			const GaSelectionParams& parameters,
			const GaCouplingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			GaSelectionCounters counters( population, GADV_SELECTION_TIME );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
				output.Clear();

			GaCrossoverStorageBuffer* crossoverBuffer = GetCrossoverBuffer( population, parameters, branch );

			// get workload for this branch
			int count, start, total = GetSelectionCount( parameters, &configuration.GetMating().GetConfiguration() );
			branch->SplitWork( total, count, start );
			start += population.GetCount() - total;

			// fill result set
			for( int i = start + count - 1; i >= start; i-- )
			{
				// store selected chromosome to result set
				StoreSelectedChromosome( &population[ i ], output, crossoverBuffer, counters, configuration );

				// update selection counter
				counters.IncSelectionCounter();
			}

			// update population statistics with new state of counters
			counters.UpdateStatistics();
		}

		// Prepares population for selection operation
		void GACALL GaDuplicatesSelection::Prepare(GaPopulation& population,
			GaChromosomeGroup& output,
			const GaSelectionParams& parameters,
			const GaCouplingConfig& configuration,
			int branchCount) const
		{
			// add tag that store inforamtion whether the chromosome is already selected
			int selectedTagID = ( (const GaDuplicatesSelectionParams&)parameters ).GetSelectedTagID();
			if( selectedTagID >= 0 )
				population.GetChromosomeTagManager().AddTag( selectedTagID, Common::Data::GaTypedTagLifecycle<Common::Threading::GaAtomic<int> >() );

			GaSelectionOperation::Prepare( population, output, parameters, configuration, branchCount );
		}

		// Removes data that was needed by the selection operation
		void GACALL GaDuplicatesSelection::Clear(GaPopulation& population,
			GaChromosomeGroup& output,
			const GaSelectionParams& parameters,
			const GaCouplingConfig& configuration,
			int branchCount) const
		{
			// remove tag that store inforamtion whether the chromosome is already selected
			int selectedTagID = ( (const GaDuplicatesSelectionParams&)parameters ).GetSelectedTagID();
			if( selectedTagID >= 0 )
				population.GetChromosomeTagManager().RemoveTag( selectedTagID );

			GaSelectionOperation::Clear( population, output, parameters, configuration, branchCount );
		}

		// Performs random selection of chromosomes in the population
		void GaRandomSelection::Exec(GaPopulation& population,
			GaChromosomeGroup& output,
			const GaDuplicatesSelectionParams& parameters,
			const GaCouplingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			GaSelectionCounters counters( population, GADV_SELECTION_TIME );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
				output.Clear();

			GaCrossoverStorageBuffer* crossoverBuffer = GetCrossoverBuffer( population, parameters, branch );
			GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > getDuplicate( parameters.GetSelectedTagID(), population.GetChromosomeTagManager() );

			// get workload for this branch and fill the result set
			int count,  maxIndex = population.GetCount() - 1;
			for( branch->SplitWork( GetSelectionCount( parameters, &configuration.GetMating().GetConfiguration() ), count ); count ; )
			{
				// get random chromosome
				GaChromosomeStorage* selected = GetSelectedChromosome( population, GaGlobalRandomIntegerGenerator->Generate( 0, maxIndex ), getDuplicate );

				// chromosome successfully selected?
				if( selected )
				{
					// store selected chromosome to result set
					StoreSelectedChromosome( selected, output, crossoverBuffer, counters, configuration );
					count--;
				}

				// update selection counter
				counters.IncSelectionCounter();
			}

			// clear indication of selection duplicates
			ClearDuplicateFlags( population, getDuplicate, branch );

			// update population statistics with new state of counters
			counters.UpdateStatistics();
		}

		/// <summary><c>GaRouletteWheelIndex</c> function implements roulette wheel mechanism for selection chromosomes.</summary>
		/// <param name="population">population over which the selection operation is perofmed.</param>
		/// <param name="totalProbability">sum of selection probabilities of all chromosomes in the population.</param>
		/// <returns>Method returns index of selected chromosome.</returns>
		int GACALL GaRouletteWheelIndex(GaPopulation& population,
			float totalProbability)
		{
			// generate random value
			float random = GaGlobalRandomFloatGenerator->Generate( totalProbability );
			float current = 0;

			// choose chromosome using roulette wheel mechanism
			int selected = 0;
			for( int maxIndex = population.GetCount() - 1; selected < maxIndex; selected++ )
			{
				current += population[ selected ].GetFitness( GaChromosomeStorage::GAFT_SCALED ).GetProbabilityBase();

				if( current >= random )
					break;
			}

			return selected;
		}

		/// <summary><c>GaRandomIndex</c> function implements mechansim for random chromosomes selection.</summary>
		/// <param name="population">population over which the selection operation is perofmed.</param>
		/// <param name="totalProbability">sum of selection probabilities of all chromosomes in the population.</param>
		/// <returns>Method returns index of selected chromosome.</returns>
		int GACALL GaRandomIndex(GaPopulation& population,
			float totalProbability) { return GaGlobalRandomIntegerGenerator->Generate( 0, population.GetCount() - 1 ); }

		// Performs roulette wheel selection of chromosomes in the population
		void GaRouletteWheelSelection::Exec(GaPopulation& population,
			GaChromosomeGroup& output,
			const GaDuplicatesSelectionParams& parameters,
			const GaCouplingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			GaSelectionCounters counters( population, GADV_SELECTION_TIME );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
				output.Clear();

			GaCrossoverStorageBuffer* crossoverBuffer = GetCrossoverBuffer( population, parameters, branch );
			GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > getDuplicate( parameters.GetSelectedTagID(), population.GetChromosomeTagManager() );

			// get sum of selection probabilities of all chromosomes in the population.
			float totalProbability = population.GetStatistics().GetValue<Fitness::GaFitness>( GADV_TOTAL_FITNESS_SCALED ).GetCurrentValue().GetProbabilityBase();

			// get workload for this branch and fill the result set
			int count,  maxIndex = population.GetCount() - 1;
			for( branch->SplitWork( GetSelectionCount( parameters, &configuration.GetMating().GetConfiguration() ), count ); count ; )
			{
				// get chromosome with cerian probability
				GaChromosomeStorage* selected = GetSelectedChromosome( population, GaRouletteWheelIndex( population, totalProbability ), getDuplicate );

				// chromosome successfully selected?
				if( selected )
				{
					// store selected chromosome to result set
					StoreSelectedChromosome( selected, output, crossoverBuffer, counters, configuration );
					count--;
				}

				// update selection counter
				counters.IncSelectionCounter();
			}

			// clear indication of selection duplicates
			ClearDuplicateFlags( population, getDuplicate, branch );

			// update population statistics with new state of counters
			counters.UpdateStatistics();
		}

		// Performs tournament selection of chromosomes in the population
		void GaTournamentSelection::Exec(GaPopulation& population,
			GaChromosomeGroup& output,
			const GaTournamentSelectionParams& parameters,
			const GaTournamentSelectionConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			GaSelectionCounters counters( population, GADV_SELECTION_TIME );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
				output.Clear();

			GaCrossoverStorageBuffer* crossoverBuffer = GetCrossoverBuffer( population, parameters, branch );
			GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > getDuplicate( parameters.GetSelectedTagID(), population.GetChromosomeTagManager() );

			// get sum of selection probabilities of all chromosomes in the population.
			float totalProbability = population.GetStatistics().GetValue<Fitness::GaFitness>( GADV_TOTAL_FITNESS_SCALED ).GetCurrentValue().GetProbabilityBase();

			// underlying selection mechanism
			int (GACALL *indexMethod )(GaPopulation&, float) = parameters.GetType() == GaTournamentSelectionParams::GATST_RANDOM_SELECTION ? GaRandomIndex : GaRouletteWheelIndex;

			// get workload for this branch and fill the result set
			int count;
			for( branch->SplitWork( GetSelectionCount( parameters, &configuration.GetMating().GetConfiguration() ), count ); count ; count-- )
			{
				GaChromosomeStorage* selected = NULL;

				// chromosome successfully selected?
				while( !selected )
				{
					// get chromosome using choosen selection method
					selected = GetSelectedChromosome( population, indexMethod( population, totalProbability ), getDuplicate );

					// update selection counter
					counters.IncSelectionCounter();
				}

				// perform selection deifined number of times to find the best chromosome
				for( int j = ( (GaTournamentSelectionParams&)parameters ).GetNumberOfSelections() - 1; j > 0; j-- )
				{
					GaChromosomeStorage* newSelected = NULL;

					// chromosome successfully selected?
					while( !newSelected )
					{
						// get chromosome using choosen selection method
						newSelected = GetSelectedChromosome( population, indexMethod( population, totalProbability ), getDuplicate );

						// update selection counter
						counters.IncSelectionCounter();
					}

					// choose the better chromosome
					if( configuration.CompareFitnesses( selected->GetFitness( GaChromosomeStorage::GAFT_SCALED ), newSelected->GetFitness( GaChromosomeStorage::GAFT_SCALED ) ) > 0 )
						selected = newSelected;
				}

				// store the best chromosome to result set
				StoreSelectedChromosome( selected, output, crossoverBuffer, counters, configuration );
			}

			// clear indication of selection duplicates
			ClearDuplicateFlags( population, getDuplicate, branch );

			// update population statistics with new state of counters
			counters.UpdateStatistics();
		}

	} // SelectionOperations
} // Population

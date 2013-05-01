
/*! \file PAES.cpp
    \brief This file conatins implementation of classes that implement Pareto Archived Evolution Strategy (PAES).
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#include "PAES.h"
#include "PopulationStatistics.h"

namespace Multiobjective
{
	namespace PAES
	{

		// Produces offspring chromosome
		void GaPAESMating::operator ()(const Chromosome::GaMatingConfig& operations,
			Chromosome::GaCrossoverBuffer& crossoverBuffer,
			const Chromosome::GaMatingParams& parameters) const
	{
			// make offspring by copying parent
			crossoverBuffer.StoreOffspringChromosome( Chromosome::GaChromosomePtr::NullPtr, 0 );

			// perform mutation
			Chromosome::GaChromosomePtr offspring = crossoverBuffer.GetOffspringChromosome( 0 )->Clone();
			operations.Mutation( offspring );

			// store mutated chromosome
			crossoverBuffer.ReplaceOffspringChromosome( 0, offspring );
		}

		// Uses current solution to produce new offspring chromosomes
		void GaPAESSelection::operator ()(Population::GaPopulation& population,
			Population::GaChromosomeGroup& output,
			const Population::GaSelectionParams& parameters,
			const Population::GaCouplingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			Population::GaCouplingCounters counters( population, Population::GADV_SELECTION_TIME );

			Population::GaCrossoverStorageBuffer& buffer = population.GetTagByID<Population::GaPartialCrossoverBuffer>( parameters.GetCrossoverBuffersTagID() )[ branch->GetFilteredID() ];

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
			{
				output.Clear();

				// first generation?
				if( population.GetCount() > 0 )
				{
					// use current solution as parent for new chromosome
					buffer.GetParents().Add( population.GetTagByID<Population::GaChromosomeStorage*>( ( (const GaPAESSelectionParams&)parameters ).GetCurrentSolutionTagID() ) );

					// create offspring solution
					configuration.Mate( buffer );
					output.Add( buffer.GetOffspring().PopLast( true ) );

					// update statistics
					counters.CollectCrossoverBufferCounters( buffer );
					buffer.Clear();
				}
				else
				{
					// make random solution for the first generation
					Population::GaChromosomeStorage* chromosome = population.MakeFromPrototype();
					output.Add( chromosome );
					population.SetTagByID( ( (const GaPAESSelectionParams&)parameters ).GetCurrentSolutionTagID(), chromosome );
				}
			}

			// update operation time and counters statistics
			counters.UpdateStatistics();
		}

		/// <summary><c>GaPESADominanceOp</c> class represents operation which compares new solution against chromosomes currently in the population
		/// to determin whether it should be archived.</summary>
		class GaPAESDominanceOp
		{

		public:

			/// <summary>This enum defines flags to indicated dominance status of new solution compared to achived chromosomes.</summary>
			enum GaDominanceFlags
			{
				/// <summary>Indicates that new solution is dominated by at least one archived chromosome.</summary>
				GAPD_DOMINATED = 0x1,

				/// <summary>Indicates that new solution dominates at least one archived chromosome.</summary>
				GAPD_DOMINANT = 0x2

			};

		private:

			/// <summary>Population over whose chromosomes dominance is determined.</summary>
			Population::GaPopulation& _population;

			/// <summary>Chromosome tag that indicates dominance status of chromosome compared to others.</summary>
			Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > _dominance;

			/// <summary>New solution that should be checked whether it should be archived.</summary>
			Population::GaChromosomeStorage& _newSolution;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="population">population over whose chromosomes dominance is determined.</param>
			/// <param name="dominance">chromosome tag that indicates dominance status of chromosome compared to others.</param>
			/// <param name="newSolution">new solution that should be checked whether it should be archived.</param>
			GaPAESDominanceOp(Population::GaPopulation& population,
				const Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> >& dominance,
				Population::GaChromosomeStorage& newSolution) : _population(population),
				_dominance(dominance),
				_newSolution(newSolution) { }

			/// <summary><c>operator ()</c> check whether currently archived chromosome dominates new solution.</summary>
			/// <param name="chromosome">archived chromosome whose dominance over new solution should be checked.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				// get dominant chromosome
				int dom = _population.CompareFitness( _newSolution, chromosome );

				// new solution dominates archived one?
				if( dom < 0 )
				{
					// remove archived
					chromosome.GetFlags().SetFlags( Population::GaChromosomeStorage::GACF_REMOVE_CHROMOSOME );

					// mark new solution as dominant
					_dominance( _newSolution ) |= GAPD_DOMINANT;
				}
				// archived chromosome dominates new solution?
				else if( dom > 0 )
					// mark new solution as dominated
					_dominance( _newSolution ) |= GAPD_DOMINATED;
			}

		};

		// Prepares population for replacement operation
		void GaPAES::Prepare(Population::GaChromosomeGroup& input,
			Population::GaPopulation& population,
			const Population::GaReplacementParams& parameters,
			const Population::GaReplacementConfig& configuration,
			int branchCount) const
		{
			const GaPAESParams& params = (const GaPAESParams&)parameters;

			// add required chromosome tags
			population.GetChromosomeTagManager().AddTag( params.GetDominanceTagID(), Common::Data::GaTypedTagLifecycle<Common::Threading::GaAtomic<int> >() );
			population.GetChromosomeTagManager().AddTag( params.GetHyperBoxTagID(), Common::Data::GaSizableTagLifecycle<Common::Grid::GaHyperBox>() );
			population.GetChromosomeTagManager().AddTag( params.GetHyperBoxInfoTagID(), Common::Data::GaTypedTagLifecycle<Common::Grid::GaHyperBoxInfo*>() );

			// add required population tags
			population.GetTagManager().AddTag( params.GetCurrentSolutionTagID(), Common::Data::GaTypedTagLifecycle<Population::GaChromosomeStorage*>() );
			population.GetTagManager().AddTag( params.GetCrowdingStorageTagID(), Common::Data::GaTypedTagLifecycle<GaCrowdingStorage>() );
			population.GetTagManager().AddTag( params.GetHyperBoxInfoBufferTagID(), Common::Data::GaTypedTagLifecycle<Common::Grid::GaHyperBoxInfoBuffer>() );

			GaReplacementOperation::Prepare( input, population, parameters, configuration, branchCount );
		}

		// Removes data that was needed by the replacement operation
		void GaPAES::Clear(Population::GaChromosomeGroup& input,
			Population::GaPopulation& population,
			const Population::GaReplacementParams& parameters,
			const Population::GaReplacementConfig& configuration,
			int branchCount) const
		{
			const GaPAESParams& params = (const GaPAESParams&)parameters;

			// remove used chromosome tags
			population.GetChromosomeTagManager().RemoveTag( params.GetDominanceTagID() );
			population.GetChromosomeTagManager().RemoveTag( params.GetHyperBoxTagID() );
			population.GetChromosomeTagManager().RemoveTag( params.GetHyperBoxInfoTagID() );

			// remove required population tags
			population.GetTagManager().RemoveTag( params.GetCurrentSolutionTagID() );
			population.GetTagManager().RemoveTag( params.GetCrowdingStorageTagID() );
			population.GetTagManager().RemoveTag( params.GetHyperBoxInfoBufferTagID() );

			GaReplacementOperation::Clear( input, population, parameters, configuration, branchCount );
		}

		// Updates population for replacement operation
		void GaPAES::Update(Population::GaChromosomeGroup& input,
			Population::GaPopulation& population,
			const Population::GaReplacementParams& parameters,
			const Population::GaReplacementConfig& configuration,
			int branchCount) const
		{
			// update population tags
			population.GetTagByID<GaCrowdingStorage>( ( (const GaPAESParams&)parameters ).GetCrowdingStorageTagID() ).SetSize( branchCount );

			GaReplacementOperation::Update( input, population, parameters, configuration, branchCount );
		}

		// Executes replacement operation
		void GaPAES::Exec(Population::GaChromosomeGroup& input,
			Population::GaPopulation& population,
			const GaPAESParams& parameters,
			const GaPAESConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			Population::GaOperationTime timer( population, Population::GADV_SCALING_TIME );

			int branchID = branch->GetFilteredID();
			int branchCount = branch->GetBarrierCount();

			// population tags
			Common::Grid::GaHyperBoxInfoBuffer& hyperBoxInfoBuffer = population.GetTagByID<Common::Grid::GaHyperBoxInfoBuffer>( parameters.GetHyperBoxInfoBufferTagID() );
			GaCrowdingStorage& crowding = population.GetTagByID<GaCrowdingStorage>( parameters.GetCrowdingStorageTagID() );
			Population::GaChromosomeStorage* currentSolution = population.GetTagByID<Population::GaChromosomeStorage*>( parameters.GetCurrentSolutionTagID() );

			Population::GaChromosomeStorage* newSolution = input.PopLast( true );

			// chromosome tags
			Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > getDominance( parameters.GetDominanceTagID(), population.GetChromosomeTagManager() );
			Population::GaChromosomeTagGetter<Common::Grid::GaHyperBox> getHyperBox( parameters.GetHyperBoxTagID(), population.GetChromosomeTagManager() );
			Population::GaChromosomeTagGetter<Common::Grid::GaHyperBoxInfo*> getHyperBoxInfo( parameters.GetHyperBoxInfoTagID(), population.GetChromosomeTagManager() );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
			{
				// insert offspring chromosome
				population.Insert( newSolution );
				getDominance( newSolution ) = 0;

				// update size of chromosome tag that stores coordinates of hyperbox to which the chromosome belongs
				population.GetChromosomeTagManager().UpdateTag( parameters.GetHyperBoxTagID(),
					Common::Grid::GaHyperBoxUpdate( ( (const Fitness::Representation::GaMVFitnessParams&)population.GetFitnessParams() ).GetValueCount() ) );

				// update buffer sizes when population size is changed
				int populationSize = population.GetPopulationParams().GetPopulationSize();
				hyperBoxInfoBuffer.SetSize( populationSize, branchCount );
			}

			// determins whether the new solution should be archived and which archived chromosomes should be removed
			Common::Workflows::GaParallelExec1<Population::GaPopulation, Population::GaChromosomeStorage> popWorkDist( *branch, population );
			popWorkDist.Execute( GaPAESDominanceOp( population, getDominance, *newSolution ), true );

			int dominance = getDominance( newSolution );

			// new solution is not dominated nor it dominates any of archived chromosomes?
			if( dominance == 0 )
			{
				// identify hyperboxes
				popWorkDist.Execute( GaHyperBoxOp( configuration.GetGrid(), getHyperBox ), false );

				// sort archived chromosomes according to hyperbox coordinates and fill hyperbox info object buffer
				GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
					GaGridDensityOp( population, hyperBoxInfoBuffer, getHyperBox, getHyperBoxInfo );

				// identify mostcrowded hyperbox
				crowding[ branchID ].Clear();
				Common::Workflows::GaParallelExec1<Common::Grid::GaHyperBoxInfoBuffer, Common::Grid::GaHyperBoxInfo> hbiWorkDist( *branch, hyperBoxInfoBuffer );
				hbiWorkDist.Execute( GaCrowdingOp( crowding[ branchID ] ), false );
			}

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
			{
				// only nondominated solutions are archived
				if( ( dominance & GaPAESDominanceOp::GAPD_DOMINATED ) == 0 )
				{
					if( ( dominance & GaPAESDominanceOp::GAPD_DOMINANT ) == 0 )
					{
						// synchronize crowding results from different branches
						Common::Grid::GaHyperBoxInfo& hbi = hyperBoxInfoBuffer[ crowding.CalculateGlobalMaxCrowding() ];
						Population::GaChromosomeStorage* remove = &population[ hbi.GetStart() ];

						// remove archived chromosome that is located in most crowded region if population is full
						if( population.GetCount() > population.GetPopulationParams().GetPermanentSpaceSize() )
						{
							// do not remove current solution if another chromosome can be removed from same hyperbox
							if( ( remove == currentSolution || remove == newSolution ) && hbi.GetCount() > 1 )
								remove = &population[ hbi.GetStart() + 1 ];

							// remove chromosome
							remove->GetFlags().SetFlags( Population::GaChromosomeStorage::GACF_REMOVE_CHROMOSOME );
							hbi.RemoveItem();
						}

						// accept new solution if it is located in less crowded region than current solution
						if( ( getHyperBoxInfo( newSolution )->GetCount() < getHyperBoxInfo( currentSolution )->GetCount() || remove == currentSolution ) && remove != newSolution )
							population.SetTagByID( parameters.GetCurrentSolutionTagID(), newSolution );
					}
					else
						// new solution was dominant over some archived chromosome - accept it as current solution
						population.SetTagByID( parameters.GetCurrentSolutionTagID(), newSolution );
				}
				else
					// new solution is dominated - remove it
					newSolution->GetFlags().SetFlags( Population::GaChromosomeStorage::GACF_REMOVE_CHROMOSOME );

				// remove chromosomes from archive that were dominated by new solution
				population.Remove();
			}

			// update operation time statistics
			timer.UpdateStatistics();
		}

	} // PAES
} // Multiobjective

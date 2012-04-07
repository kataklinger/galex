
/*! \file PESA.cpp
    \brief This file conatins implementation of classes that implement Pareto Envelope-based Selection Algorithm and Region-based Selection (PESA-I and PESA-II).
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#include "PopulationStatistics.h"
#include "PESA.h"

namespace Multiobjective
{
	namespace PESA
	{

		/// <summary><c>GaPESADominanceOp</c> class represents operation which compares chromosomes in the population and determins the dominant ones.</summary>
		class GaPESADominanceOp
		{

		private:

			/// <summary>Population over whose chromosomes dominance is determined.</summary>
			Population::GaPopulation& _population;

			/// <summary>Chromosome tag that stores information whether the chromosome is dominated by any other chromosome in the population.</summary>
			Population::GaChromosomeTagGetter<int> _dominated;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="population">population over whose chromosomes dominance is determined.</param>
			/// <param name="dominated">chromosome tag that stores information whether the chromosome is dominated by any other chromosome.</param>
			GaPESADominanceOp(Population::GaPopulation& population,
				const Population::GaChromosomeTagGetter<int>& dominated) : _population(population),
				_dominated(dominated)  { }

			/// <summary><c>operator ()</c> operators determins dominated chromosome and marks it.</summary>
			/// <param name="chromosome1">the first chromosomes that is checked.</param>
			/// <param name="chromosome1">the second chromosomes that is checked.</param>
			/// <param name="index1">index of the first chromosome.</param>
			/// <param name="index2">index of the second chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome1,
				Population::GaChromosomeStorage& chromosome2,
				int index1,
				int index2)
			{
				// there is no need to compare chromosomes if both are already dominanted by other chromosomes
				if( !_dominated( chromosome1 ) || !_dominated( chromosome2 ) )
				{
					// get dominant chromosome
					int dom = _population.CompareFitness( chromosome1, chromosome2 );

					// the first one is dominant
					if( dom < 0 )
						// mark domnated
						_dominated( chromosome2 ) = 1;
					// the second one is dominant
					else if( dom > 0 )
						// mark domnated
						_dominated( chromosome1 ) = 1;
				}
			}

		};

		/// <summary><c>GaPESAFrontOp</c> class represents operation that puts nontominated chromosomes in separate buffer.</summary>
		class GaPESAFrontOp
		{

		private:

			/// <summary>Chromosome tag that stores information whether the chromosome is dominated by any other chromosome in the population.</summary>
			Population::GaChromosomeTagGetter<int> _dominated;

			/// <summary>Group that will store nondominated chromosomes.</summary>
			Population::GaChromosomeGroup& _nondominated;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="dominated">chromosome tag that stores information whether the chromosome is dominated by any other chromosome in the population.</param>
			/// <param name="nondominated">group that will store nondominated chromosomes.</param>
			GaPESAFrontOp(const Population::GaChromosomeTagGetter<int>& dominated,
				Population::GaChromosomeGroup& nondominated) : _dominated(dominated),
				_nondominated(nondominated) { }

			/// <summary><c>operator ()</c> determins whether the chromosome is nondominated and puts it into buffer.</summary>
			/// <param name="chromosome">chromosome which should be put to buffer.</param>
			/// <param name="index">index of the chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				// is chromosome nondominated
				if( !_dominated( chromosome ) )
					_nondominated.AddAtomic( &chromosome );
			}

		};

		/// <summary><c>GaPESAFitnessAssignOp</c> class represents operation that assigns scaled fitness value to chromosome
		/// based on how many chromosomes are there in its hyperbox.</summary>
		class GaPESAFitnessAssignOp
		{

		private:

			/// <summary>Chromosome tag that stores information whether the chromosome is dominated by any other chromosome in the population.</summary>
			Population::GaChromosomeTagGetter<int> _dominated;

			/// <summary>Chromosome tag that references info object of hyperbox to which the chromosome belongs.</summary>
			Population::GaChromosomeTagGetter<Common::Grid::GaHyperBoxInfo*> _hyperBoxInfo;

			/// <summary>Indicates whether the sharing of fitness for a region (hyperbox) should be engaged.</summary>
			bool _regionSharing;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="dominated">chromosome tag that stores information whether the chromosome is dominated by any other chromosome in the population.</param>
			/// <param name="hyperBoxInfo">chromosome tag that references info object of hyperbox to which the chromosome belongs</param>
			/// <param name="regionSharing">if this parameters is set to <c>true</c> chromosomes in same region (hyperbox) share fitness value.</param>
			GaPESAFitnessAssignOp(const Population::GaChromosomeTagGetter<int>& dominated,
				const Population::GaChromosomeTagGetter<Common::Grid::GaHyperBoxInfo*>& hyperBoxInfo,
				bool regionSharing) : _dominated(dominated),
				_hyperBoxInfo(hyperBoxInfo),
				_regionSharing(regionSharing) { }

			/// <summary><c>operator ()</c> calculates and assigns scaled fitness value to chromosome.</summary>
			/// <param name="chromosome">chromosome whose scaled fitness should be calculated.</param>
			/// <param name="index">index of the chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				GaPESAFitness& fitness = (GaPESAFitness&)chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_SCALED );

				// only nondomnated chromosomes have scaled fitness other then 0
				if( !_dominated( chromosome ) )
				{
					Common::Grid::GaHyperBoxInfo* info = _hyperBoxInfo( chromosome );
					if( info )
					{
						// get number of chromosomes in same hyperbox and use sharing if required
						int value = _regionSharing ? info->GetCount() * info->GetCount() : info->GetCount();

						// assign fitness
						fitness.SetValue( 1.0f / value );
					}
				}
				else
					fitness.SetValue( 0 );

				_dominated( chromosome ) = 0;
			}

		};

		// Prepares population for scaling operation
		void GaPESA::Prepare(Population::GaPopulation& population,
			const Population::GaScalingParams& parameters,
			const Population::GaScalingConfig& configuration,
			int branchCount) const
		{
			const GaPESAParams& params = (const GaPESAParams&)parameters;

			// add required chromosome tags
			population.GetChromosomeTagManager().AddTag( params.GetDominatedTagID(), Common::Data::GaTypedTagLifecycle<int>() );
			population.GetChromosomeTagManager().AddTag( params.GetHyperBoxTagID(), Common::Data::GaSizableTagLifecycle<Common::Grid::GaHyperBox>() );
			population.GetChromosomeTagManager().AddTag( params.GetHyperBoxInfoTagID(), Common::Data::GaTypedTagLifecycle<Common::Grid::GaHyperBoxInfo*>() );

			// add required population tags
			population.GetTagManager().AddTag( params.GetNondominatedTagID(), Common::Data::GaTypedTagLifecycle<Population::GaChromosomeGroup>() );
			population.GetTagManager().AddTag( params.GetCrowdingStorageTagID(), Common::Data::GaTypedTagLifecycle<GaCrowdingStorage>() );
			population.GetTagManager().AddTag( params.GetHyperBoxInfoBufferTagID(), Common::Data::GaTypedTagLifecycle<Common::Grid::GaHyperBoxInfoBuffer>() );

			GaScalingOperation::Prepare( population, parameters, configuration, branchCount );
		}

		// Removes data that was needed by the scaling operation
		void GaPESA::Clear(Population::GaPopulation& population,
			const Population::GaScalingParams& parameters,
			const Population::GaScalingConfig& configuration,
			int branchCount) const
		{
			const GaPESAParams& params = (const GaPESAParams&)parameters;

			// remove used chromosome tags
			population.GetChromosomeTagManager().RemoveTag( params.GetDominatedTagID() );
			population.GetChromosomeTagManager().RemoveTag( params.GetHyperBoxTagID() );
			population.GetChromosomeTagManager().RemoveTag( params.GetHyperBoxInfoTagID() );

			// remove required population tags
			population.GetTagManager().RemoveTag( params.GetCrowdingStorageTagID() );
			population.GetTagManager().RemoveTag( params.GetNondominatedTagID() );
			population.GetTagManager().RemoveTag( params.GetHyperBoxInfoBufferTagID() );

			GaScalingOperation::Clear( population, parameters, configuration, branchCount );
		}

		// Updates population for scaling operation
		void GaPESA::Update(Population::GaPopulation& population,
			const Population::GaScalingParams& parameters,
			const Population::GaScalingConfig& configuration,
			int branchCount) const
		{
			// update population tags
			population.GetTagByID<GaCrowdingStorage>( ( (const GaPESAParams&)parameters ).GetCrowdingStorageTagID() ).SetSize( branchCount );

			GaScalingOperation::Update( population, parameters, configuration, branchCount );
		}

		// Execute scaling operation
		void GaPESA::Exec(Population::GaPopulation& population,
			const GaPESAParams& parameters,
			const GaPESAConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			Population::GaOperationTime timer( population, Population::GADV_SCALING_TIME );

			int branchID = branch->GetFilteredID();
			int branchCount = branch->GetBarrierCount();

			// population tags
			Population::GaChromosomeGroup& nondminated = population.GetTagByID<Population::GaChromosomeGroup>( parameters.GetNondominatedTagID() );
			Common::Grid::GaHyperBoxInfoBuffer& hyperBoxInfoBuffer = population.GetTagByID<Common::Grid::GaHyperBoxInfoBuffer>( parameters.GetHyperBoxInfoBufferTagID() );
			GaCrowdingStorage& crowding = population.GetTagByID<GaCrowdingStorage>( parameters.GetCrowdingStorageTagID() );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
			{
				// mark population as re-scaled
				population.GetFlags().SetFlags( Population::GaPopulation::GAPF_COMPLETE_SCALED_FITNESS_UPDATE );

				// update size of chromosome tag that stores coordinates of hyperbox to which the chromosome belongs
				population.GetChromosomeTagManager().UpdateTag( parameters.GetHyperBoxTagID(),
					Common::Grid::GaHyperBoxUpdate( ( (const Fitness::Representation::GaMVFitnessParams&)population.GetFitnessParams() ).GetValueCount() ) );

				// update buffer sizes when population size is changed
				int populationSize = population.GetPopulationParams().GetPopulationSize();
				nondminated.SetSize( populationSize );
				hyperBoxInfoBuffer.SetSize( populationSize, branchCount );

				nondminated.Clear();
				crowding.RestartGlobalMaxCrowding();
			}

			// chromosome tags
			Population::GaChromosomeTagGetter<int> getDominated( parameters.GetDominatedTagID(), population.GetChromosomeTagManager() );
			Population::GaChromosomeTagGetter<Common::Grid::GaHyperBox> getHyperBox( parameters.GetHyperBoxTagID(), population.GetChromosomeTagManager() );
			Population::GaChromosomeTagGetter<Common::Grid::GaHyperBoxInfo*> getHyperBoxInfo( parameters.GetHyperBoxInfoTagID(), population.GetChromosomeTagManager() );

			GaCrowding& maxCrowd = crowding[ branchID ];
			maxCrowd.Clear();

			Common::Workflows::GaParallelExec1<Population::GaPopulation, Population::GaChromosomeStorage> popWorkDist( *branch, population );
			Common::Workflows::GaParallelExec2<Population::GaPopulation, Population::GaChromosomeStorage> domWorkDist( *branch, population );

			// identify hyperboxes and nondominated chromosomes
			domWorkDist.Execute( GaHyperBoxOp( configuration.GetGrid(), getHyperBox ), GaPESADominanceOp( population, getDominated ), true );
			popWorkDist.Execute( GaPESAFrontOp( getDominated, nondminated ), true );

			// sort nondominated chromosomes according to hyperbox coordinates and fill hyperbox info object buffer
			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
				GaGridDensityOp( nondminated, hyperBoxInfoBuffer, getHyperBox, getHyperBoxInfo );

			Common::Workflows::GaParallelExec1<Common::Grid::GaHyperBoxInfoBuffer, Common::Grid::GaHyperBoxInfo> hbiWorkDist( *branch, hyperBoxInfoBuffer );

			// select chromosome in most crowded hyperboxes which should be removed
			int requiredCount = population.GetPopulationParams().GetPermanentSpaceSize();
			for( int nondominatedCount = nondminated.GetCount(); nondominatedCount > requiredCount; nondominatedCount-- )
			{
				// has the chromosome from hyperbox selected by this branch been removed?
				if( crowding.RequiresUpdate( branchID ) )
				{
					// yes - branch should search again for most crowded hyperbox
					maxCrowd.Clear();
					hbiWorkDist.Execute( GaCrowdingOp( maxCrowd ), false );
				}

				GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
				{
					// get most crowded hyperbox
					Common::Grid::GaHyperBoxInfo& hb = hyperBoxInfoBuffer[ crowding.CalculateGlobalMaxCrowding() ];
					Population::GaChromosomeStorage* chromosome = NULL;
					
					// remove random chromosome from the hyperbox
					do
					{
						chromosome = &nondminated[ GaGlobalRandomIntegerGenerator->Generate( hb.GetStart(), hb.GetEnd() ) ];
					} while( chromosome->GetFlags().IsFlagSetAny( Population::GaChromosomeStorage::GACF_REMOVE_CHROMOSOME ) );

					// mark chromosome for removing
					chromosome->GetFlags().SetFlags( Population::GaChromosomeStorage::GACF_REMOVE_CHROMOSOME );

					hb.RemoveItem();
				}
			}

			// assign scaled fitness to chromosomes
			popWorkDist.Execute( GaPESAFitnessAssignOp( getDominated, getHyperBoxInfo, parameters.GetRegionSharing() ), false );

			// update operation time statistics
			timer.UpdateStatistics();
		}

	} // PESA
} // Multiobjective

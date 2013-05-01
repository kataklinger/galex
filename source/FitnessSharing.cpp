
/*! \file FitnessSharing.cpp
    \brief This file declares classes that implements fitness sharing scaling.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#include "PopulationStatistics.h"
#include "FitnessSharing.h"

namespace Population
{
	namespace ScalingOperations
	{

		/// <summary><c>GaStoreOp</c> stores value of scaled fitness to fitness object dedicated fot that purpose in storage object.</summary>
		struct GaStoreOp
		{

			/// <summary><c> operator ()</c> stores scaled fitness to chromosome storage object.</summary>
			/// <param name="chromosome">chromosome storage object to which the scaled fitness should be stored.</param>
			/// <param name="sum">scaled fitness fitness.</param>
			inline void GACALL operator ()(GaChromosomeStorage& chromosome,
				float sum)
			{
				( (Fitness::Representation::GaSVFitness<float>&)chromosome.GetFitness( GaChromosomeStorage::GAFT_SCALED ) ).SetValue(
					chromosome.GetFitness( GaChromosomeStorage::GAFT_RAW ).GetProbabilityBase() / sum );
			}

		};

		/// <summary><c>GaFitnessSharingCleanOp</c> class represents operation that clears partial sharing fitness tags.</summary>
		class GaFitnessSharingCleanOp
		{

		private:

			/// <summary>Chromosome tag that contains partially calculated shared fitness.</summary>
			GaChromosomeTagGetter<Common::Data::GaPartialSum> _partialSum;

		public:

			/// <summary>Initalizes operation.</summary>
			/// <summary>chromosome tag that contains partially calculated shared fitness.</summary>
			GaFitnessSharingCleanOp(const GaChromosomeTagGetter<Common::Data::GaPartialSum>& partialSum) : _partialSum(partialSum) { }

			/// <summary><c>operator ()</c> clears chromosome's partial sharing fitness tag.</summary>
			/// <param name="chromosome">the from which the infomration should be removed.</param>
			/// <param name="index">index of the the chromosome.</param>
			inline void GACALL operator ()(GaChromosomeStorage& chromosome,
				int index)
			{
				Common::Data::GaPartialSum& sum = _partialSum( chromosome );
				for( int i = sum.GetSize() - 1; i >= 0; i-- )
					sum[ i ] = 0.0f;
			}

		};

		/// <summary><c>GaRemoveFitnessSharingOp</c> class represents operation that updates partial sharing fitness tags of chromosomes in the population
		/// when chromosomes are removed from the population.</summary>
		class GaRemoveFitnessSharingOp
		{

		private:

			/// <summary>Population over which the scaling operation is performed.</summary>
			GaPopulation& _population;

			/// <summary>Chromosome tag that contains partially calculated shared fitness.</summary>
			GaChromosomeTagGetter<Common::Data::GaPartialSum> _partialSum;

			/// <summary>Comparator used for calculating difference between two chromosomes.</summary>
			const Chromosome::GaChromosomeComparatorSetup& _comparator;

			/// <summary>Dinstance cutoff.</summary>
			float _cutoff;

			/// <summary>Curvature of sharing function.</summary>
			float _alpha;

			/// <summary>ID of the brach that executes operation.</summary>
			int _branchID;

		public:

			/// <summary>Initalizes operation.</summary>
			/// <param name="population">population over which the scaling operation is performed.</param>
			/// <param name="partialSum">chromosome that that contains partially calculated shared fitness.</param>
			/// <param name="comparator">comparator used for calculating difference between two chromosomes.</param>
			/// <param name="cutoff">dinstance cutoff.</param>
			/// <param name="alpha">curvature of sharing function.</param>
			/// <param name="branchID">ID of the brach that executes operation.</param>
			GaRemoveFitnessSharingOp(GaPopulation& population,
				const GaChromosomeTagGetter<Common::Data::GaPartialSum>& partialSum,
				const Chromosome::GaChromosomeComparatorSetup& comparator,
				float cutoff,
				float alpha,
				int branchID) : _population(population),
				_partialSum(partialSum),
				_comparator(comparator),
				_cutoff(cutoff),
				_alpha(alpha),
				_branchID(branchID) { }

			/// <summary><c>operator ()</c> removes sharing data of removed chromosomes from other chromosomes in the population.</summary>
			/// <param name="chromosome">the from which the infomration should be removed.</param>
			/// <param name="index">index of the the chromosome.</param>
			inline void GACALL operator ()(GaChromosomeStorage& chromosome,
				int index)
			{
				for( int i = _population.GetCount() - 1; i >= 0; i-- )
				{
					GaChromosomeStorage& chromosome2 = _population[ i ];

					// only old chromosomes are updated
					if( !chromosome2.GetFlags().IsFlagSetAny( GaChromosomeStorage::GACF_NEW_CHROMOSOME ) )
						// substract sharing factor from chromosome that stays
						_partialSum( chromosome2 )[ _branchID ] -= GaCalculateSharing( _comparator, *chromosome.GetChromosome(), *chromosome2.GetChromosome(), _cutoff, _alpha );
					else
						// prepare partial sum storage of new chromosome
						_partialSum( chromosome )[ _branchID ] = 0.0f;
				}
			}

		};

		/// <summary><c>GaAddFitnessSharingOp</c> class represents operation that updates partial sharing fitness tags of chromosomes in the population
		/// when chromosomes are added to the population.</summary>
		class GaAddFitnessSharingOp
		{

		private:

			/// <summary>Population over which the scaling operation is performed.</summary>
			GaPopulation& _population;

			/// <summary>Chromosome that that contains partially calculated shared fitness.</summary>
			GaChromosomeTagGetter<Common::Data::GaPartialSum> _partialSum;

			/// <summary>Comparator used for calculating difference between two chromosomes.</summary>
			const Chromosome::GaChromosomeComparatorSetup& _comparator;

			/// <summary>Dinstance cutoff.</summary>
			float _cutoff;

			/// <summary>Curvature of sharing function.</summary>
			float _alpha;

			/// <summary>ID of the brach that executes operation.</summary>
			int _branchID;

		public:

			/// <summary>Initalizes operation.</summary>
			/// <param name="population">population over which the scaling operation is performed.</param>
			/// <param name="partialSum">chromosome that that contains partially calculated shared fitness.</param>
			/// <param name="comparator">comparator used for calculating difference between two chromosomes.</param>
			/// <param name="cutoff">dinstance cutoff.</param>
			/// <param name="alpha">curvature of sharing function.</param>
			/// <param name="branchID">ID of the brach that executes operation.</param>
			GaAddFitnessSharingOp(GaPopulation& population,
				const GaChromosomeTagGetter<Common::Data::GaPartialSum>& partialSum,
				const Chromosome::GaChromosomeComparatorSetup& comparator,
				float cutoff,
				float alpha,
				int branchID) : _population(population),
				_partialSum(partialSum),
				_comparator(comparator),
				_cutoff(cutoff),
				_alpha(alpha),
				_branchID(branchID) { }

			/// <summary><c>operator ()</c> adds sharing data of new chromosomes to other chromosomes in the population.</summary>
			/// <param name="chromosome">the from which the infomration should be removed.</param>
			/// <param name="index">index of the the chromosome.</param>
			inline void GACALL operator ()(GaChromosomeStorage& chromosome,
				int index)
			{
				for( int i = _population.GetCount() - 1; i >= 0; i-- )
				{
					// only with old chromosomes sharing factor is calculated
					GaChromosomeStorage& chromosome2 = _population[ i ];

					if( !chromosome2.GetFlags().IsFlagSetAny( GaChromosomeStorage::GACF_NEW_CHROMOSOME ) )
					{
						//calculate sharing factor
						float distance = GaCalculateSharing( _comparator, *chromosome.GetChromosome(), *chromosome2.GetChromosome(), _cutoff, _alpha );

						// add sharing factor to existing chromosomes
						_partialSum( chromosome )[ _branchID ] += distance;
						_partialSum( chromosome2 )[ _branchID ] += distance;
					}
				}
			}

		};

		// Prepares data for scaling operation
		void GaShareFitnessScaling::Prepare(GaPopulation& population,
			const GaScalingParams& parameters,
			const GaScalingConfig& configuration,
			int branchCount) const
		{
			population.GetChromosomeTagManager().AddTag( ( (const GaShareFitnessParams&)parameters ).GetPartialSumTagID(),
				Common::Data::GaSizableTagLifecycle<Common::Data::GaPartialSum>( branchCount ) );

			GaScalingOperation::Prepare( population, parameters, configuration, branchCount );
		}

		// Removes data that was needed by the scaling operation
		void GaShareFitnessScaling::Clear(GaPopulation& population,
			const GaScalingParams& parameters,
			const GaScalingConfig& configuration,
			int branchCount) const
		{
			population.GetChromosomeTagManager().RemoveTag( ( (const GaShareFitnessParams&)parameters ).GetPartialSumTagID() );

			GaScalingOperation::Clear( population, parameters, configuration, branchCount );
		}

		// Updates population for scaling operation
		void GaShareFitnessScaling::Update(GaPopulation& population,
			const GaScalingParams& parameters,
			const GaScalingConfig& configuration,
			int branchCount) const
		{
			population.GetChromosomeTagManager().UpdateTag( ( (const GaShareFitnessParams&)parameters ).GetPartialSumTagID(), Common::Data::GaPartialSumSizeUpdate( branchCount ) );

			GaScalingOperation::Update( population, parameters, configuration, branchCount );
		}

		// Execute scaling operation
		void GaShareFitnessScaling::Exec(GaPopulation& population,
			const GaShareFitnessParams& parameters,
			const GaShareFitnessScalingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			GaOperationTime timer( population, GADV_SCALING_TIME );

			int branchID = branch->GetFilteredID();
			int branchCount = branch->GetBarrierCount();

			const Chromosome::GaChromosomeComparatorSetup& comparator = configuration.GetComparator();
			float cutoff = parameters.GetCutoff();
			float alpha = parameters.GetAlpha();

			GaChromosomeTagGetter<Common::Data::GaPartialSum> getPartialSum( parameters.GetPartialSumTagID(), population.GetChromosomeTagManager() );

			// raw fitness updated for all chromosomes or scaling operation is changed?
			if( population.GetFlags().IsFlagSetAny( GaPopulation::GAPF_COMPLETE_FITNESS_UPDATE | GaPopulation::GAPF_SCALED_FITNESS_PROTOTYPE_CHANGED ) )
			{
				// prepare tags that stores sharing data
				Common::Workflows::GaParallelExec1<GaPopulation, GaChromosomeStorage> popWorkDist( *branch, population );
				popWorkDist.Execute( GaFitnessSharingCleanOp( getPartialSum ), true );

				// recalculate partial fitness sharing for all chromosomes in population
				Common::Workflows::GaParallelExec2<GaPopulation, GaChromosomeStorage> shareWorkDist( *branch, population );
				shareWorkDist.Execute( GaFitnessSharingOp( getPartialSum, comparator, cutoff, alpha, branchID ), false );

				// mark population that all chromosomes are updated
				GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
					population.GetFlags().SetFlags( GaPopulation::GAPF_COMPLETE_SCALED_FITNESS_UPDATE );
			}
			else
			{
				// update only data for new and removed chromosomes

				// remove sharing data of removed chromosomes
				Common::Workflows::GaParallelExec1<GaChromosomeGroup, GaChromosomeStorage> remWorkDist( *branch, population.GetRemovedChromosomes() );
				remWorkDist.Execute( GaRemoveFitnessSharingOp( population, getPartialSum, comparator, cutoff, alpha, branchID ), true );

				// add sharing data of new chromosomes to old chromosomes
				Common::Workflows::GaParallelExec1<GaChromosomeGroup, GaChromosomeStorage> newWorkDist1( *branch, population.GetNewChromosomes() );
				newWorkDist1.Execute( GaAddFitnessSharingOp( population, getPartialSum, comparator, cutoff, alpha, branchID ), false );

				// update sharing data between new chromosomes
				Common::Workflows::GaParallelExec2<GaChromosomeGroup, GaChromosomeStorage> newWorkDist2( *branch, population.GetNewChromosomes() );
				newWorkDist2.Execute( GaFitnessSharingOp( getPartialSum, comparator, cutoff, alpha, branchID ), true );
			}

			// sum partial fitness sharing and store calculated fitness
			Common::Workflows::GaParallelExec1<GaPopulation, GaChromosomeStorage> workDist( *branch, population );
			workDist.Execute( GaSumFitnessSharingOp<GaStoreOp>( getPartialSum, GaStoreOp() ), false );

			// update operation time statistics
			timer.UpdateStatistics();
		}

	} // ScalingOperations
} // Population

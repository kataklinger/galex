
/*! \file Scalings.cpp
    \brief This file implements classes that represent scaling operations.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#include <math.h>
#include "PopulationStatistics.h"
#include "Population.h"
#include "Scalings.h"

namespace Population
{
	namespace ScalingOperations
	{

		/// <summary><c>GaPerformScaling</c> function performs provided scaling operation on chromosomes in the population.</summary>
		/// <typeparam name="OPERATION">type of scaling operation.</typeparam>
		/// <param name="operation">scaling operation that should be performed on chromosomes.</param>
		/// <param name="completeUpdate">it this paramenter is set to <c>true</c> it instructs function to perform scalin on all chromosomes in the population.</param>
		/// <param name="population">population on whose chromosomes scaling is performed.</param>
		/// <param name="parameters">parametenrs of scaling operation.</param>
		/// <param name="configuration">configuration of scaling operation.</param>
		/// <param name="branch">pointer to wrokflow branch that executes operation.</param>
		template<typename OPERATION>
		void GACALL GaPerformScaling(OPERATION& operation,
			bool completeUpdate,
			GaPopulation& population,
			const GaScalingParams& parameters,
			const GaScalingConfig& configuration,
			Common::Workflows::GaBranch* branch)
		{
			// complete rescale needed?
			if( completeUpdate || population.GetFlags().IsFlagSetAny( GaPopulation::GAPF_COMPLETE_FITNESS_UPDATE | GaPopulation::GAPF_SCALED_FITNESS_PROTOTYPE_CHANGED ) )
			{
				// rescale all chromosomes in population
				Common::Workflows::GaParallelExec1<GaPopulation, GaChromosomeStorage> workDist( *branch, population );
				workDist.Execute( operation, false );

				// mark population as rescaled
				GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
					population.GetFlags().SetFlags( GaPopulation::GAPF_COMPLETE_SCALED_FITNESS_UPDATE );
			}
			else
			{
				// perform scaling only on new chromosomes
				Common::Workflows::GaParallelExec1<GaChromosomeGroup, GaChromosomeStorage> workDist( *branch, population.GetNewChromosomes() );
				workDist.Execute( operation, false );
			}
		}

		/// <summary><c>GaNoScalingOp</c> class represent operation that sets probability base of chromosome's fitness as it's scaled fitness.</summary>
		class GaNoScalingOp
		{

		public:

			/// <summary><c>operator ()</c> perform scaling operation on chromosome.</summary>
			/// <param name="chromosome">chromosome whose fitness should be scaled.</param>
			/// <param name="index">index of the the chromosome.</param>
			inline void GACALL operator ()(GaChromosomeStorage& chromosome,
				int index)
			{
				( (Fitness::Representation::GaSVFitness<float>&)chromosome.GetFitness( GaChromosomeStorage::GAFT_SCALED ) ).SetValue(
					chromosome.GetFitness( GaChromosomeStorage::GAFT_RAW ).GetProbabilityBase() );
			}

		};

		// Converts probability base of raw fitness of chromosomes to scaled fitness
		void GaNoScaling::operator ()(GaPopulation& population,
			const GaScalingParams& parameters,
			const GaScalingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			GaOperationTime timer( population, GADV_SCALING_TIME );

			// scale chromosomes
			GaPerformScaling( GaNoScalingOp(), false, population, parameters, configuration, branch );

			// update operation time statistics
			timer.UpdateStatistics();
		}

		/// <summary><c>GaWindowScalingOp</c> class represent operation that calculates scaled fitness of a chromosome using windowing method.</summary>
		class GaWindowScalingOp
		{

		private:

			/// <summary>Probability base of the worst chromosome's raw fitness.</summary>
			float _worst;

		public:

			/// <summary>Initalizes operation.</summary>
			/// <param name="worst">probability base of the worst chromosome's raw fitness.</param>
			GaWindowScalingOp(float worst) : _worst(worst) { }

			/// <summary><c>operator ()</c> perform scaling operation on chromosome.</summary>
			/// <param name="chromosome">chromosome whose fitness should be scaled.</param>
			/// <param name="index">index of the the chromosome.</param>
			inline void GACALL operator ()(GaChromosomeStorage& chromosome,
				int index)
			{
				( (Fitness::Representation::GaSVFitness<float>&)chromosome.GetFitness( GaChromosomeStorage::GAFT_SCALED ) ).SetValue(
					chromosome.GetFitness( GaChromosomeStorage::GAFT_RAW ).GetProbabilityBase() - _worst );
			}

		};

		// Performs window scaling on chromosomes in the population
		void GaWindowScaling::operator ()(GaPopulation& population,
			const GaScalingParams& parameters,
			const GaScalingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			GaOperationTime timer( population, GADV_SCALING_TIME );

			Statistics::GaStatistics& stats = population.GetStatistics();

			// update statistics with the raw fitness of the worst chromosome
			population.UpdateTracker( GaRawFitnessTracker::TRACKER_ID, branch );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
			{
				// the worst raw fitness has changed? - whole population needs to be rescaled
				if( stats.GetValue<Fitness::GaFitness>( GADV_WORST_FITNESS ).IsChanged() )
					population.GetFlags().SetFlags( GaPopulation::GAPF_COMPLETE_SCALED_FITNESS_UPDATE );
			}

			// scale chromosomes
			Statistics::GaValueHistory<Fitness::GaFitness>& worstStat = stats.GetValue<Fitness::GaFitness>( GADV_WORST_FITNESS );
			GaPerformScaling( GaWindowScalingOp( worstStat.GetCurrentValue().GetProbabilityBase() ), worstStat.IsChanged(), population, parameters, configuration, branch );

			// update operation time statistics
			timer.UpdateStatistics();
		}

		/// <summary><c>GaRankingScalingOp</c> class represent operation that uses chromosome ranking as scaled fitness of a chromosome.</summary>
		class GaRankingScalingOp
		{

		private:

			/// <summary>Rank that the best chromosome will have.</summary>
			int _maxRank;

		public:

			/// <summary>Initalizes operation.</summary>
			/// <param name="maxRank">rank that the best chromosome will have.</param>
			GaRankingScalingOp(int maxRank) : _maxRank(maxRank) { }

			/// <summary><c>operator ()</c> perform scaling operation on chromosome.</summary>
			/// <param name="chromosome">chromosome whose fitness should be scaled.</param>
			/// <param name="index">index of the the chromosome.</param>
			inline void GACALL operator ()(GaChromosomeStorage& chromosome,
				int index) { ( (Fitness::Representation::GaSVFitness<float>&)chromosome.GetFitness( GaChromosomeStorage::GAFT_SCALED ) ).SetValue( (float)( _maxRank - index ) ); 	}

		};

		// Performs ranking scaling on chromosomes in the population
		void GaRankingScaling::operator ()(GaPopulation& population,
			const GaScalingParams& parameters,
			const GaScalingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			GaOperationTime timer( population, GADV_SCALING_TIME );

			// scale chromosomes
			Common::Workflows::GaParallelExec1<GaPopulation, GaChromosomeStorage> workDist( *branch, population );
			workDist.Execute( GaRankingScalingOp( population.GetCount() - 1 ), false );

			// mark population as rescaled
			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
				population.GetFlags().SetFlags( GaPopulation::GAPF_COMPLETE_SCALED_FITNESS_UPDATE );

			// update operation time statistics
			timer.UpdateStatistics();
		}

		/// <summary><c>GaExponentialScalingOp</c> class represent operation that calculates scaled fitness by raising chromosome's raw fitness to specified power.</summary>
		class GaExponentialScalingOp
		{

		private:

			/// <summary>Power on which the probability base of chromosome's raw fitness is raised.</summary>
			float _factor;

		public:

			/// <summary>Initalizes operation.</summary>
			/// <param name="factor">power on which the probability base of chromosome's raw fitness is raised.</param>
			GaExponentialScalingOp(float factor) : _factor(factor) { }

			/// <summary><c>operator ()</c> perform scaling operation on chromosome.</summary>
			/// <param name="chromosome">chromosome whose fitness should be scaled.</param>
			/// <param name="index">index of the the chromosome.</param>
			inline void GACALL operator ()(GaChromosomeStorage& chromosome,
				int index)
			{
				( (Fitness::Representation::GaSVFitness<float>&)chromosome.GetFitness( GaChromosomeStorage::GAFT_SCALED ) ).SetValue(
					pow( chromosome.GetFitness( GaChromosomeStorage::GAFT_RAW ).GetProbabilityBase(), _factor ) );
			}

		};

		// Performs exponential scaling on chromosomes in the population
		void GaExponentialScaling::operator ()(GaPopulation& population,
			const GaScalingParams& parameters,
			const GaScalingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			GaOperationTime timer( population, GADV_SCALING_TIME );

			// scale chromosomes
			GaPerformScaling( GaExponentialScalingOp( ( (const GaScalingFactorParams&)parameters ).GetFactor() ), false, population, parameters, configuration, branch );

			// update operation time statistics
			timer.UpdateStatistics();
		}

		/// <summary><c>GaLinearScalingOp</c> class represent operation that calculates scaled fitness by applying linear function to chromosome's raw fitness.</summary>
		class GaLinearScalingOp
		{

		private:

			/// <summary><c>a</c> component of linear function: <c>a * raw_fitness + b</c>.</summary>
			float _a;

			/// <summary><c>b</c> component of linear function: <c>a * raw_fitness + b</c>.</summary>
			float _b;

		public:

			/// <summary>Initalizes operation.</summary>
			/// <param name="a"><c>a</c> component of linear function: <c>a * raw_fitness + b</c>.</param>
			/// <param name="b"><c>b</c> component of linear function: <c>a * raw_fitness + b</c>.</param>
			GaLinearScalingOp(float a,
				float b) : _a(a),
				_b(b) { }

			/// <summary><c>operator ()</c> perform scaling operation on chromosome.</summary>
			/// <param name="chromosome">chromosome whose fitness should be scaled.</param>
			/// <param name="index">index of the the chromosome.</param>
			inline void GACALL operator ()(GaChromosomeStorage& chromosome,
				int index)
			{
				( (Fitness::Representation::GaSVFitness<float>&)chromosome.GetFitness( GaChromosomeStorage::GAFT_SCALED ) ).SetValue(
					_a * chromosome.GetFitness( GaChromosomeStorage::GAFT_RAW ).GetProbabilityBase() + _b );
			}

		};

		// Performs linear scaling on chromosomes in the population
		void GaLinearScaling::operator ()(GaPopulation& population,
			const GaScalingParams& parameters,
			const GaScalingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			GaOperationTime timer( population, GADV_SCALING_TIME );

			population.UpdateTracker( GaPopulationSizeTracker::TRACKER_ID, branch );
			population.UpdateTracker( GaRawFitnessTracker::TRACKER_ID, branch );

			// mark population as rescaled
			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
				population.GetFlags().SetFlags( GaPopulation::GAPF_COMPLETE_SCALED_FITNESS_UPDATE );

			// get stats needed to calculate a and b
			float max = population.GetStatistics().GetValue<Fitness::GaFitness>( GADV_BEST_FITNESS ).GetCurrentValue().GetProbabilityBase();
			float min = population.GetStatistics().GetValue<Fitness::GaFitness>( GADV_WORST_FITNESS ).GetCurrentValue().GetProbabilityBase();
			float avg = population.GetStatistics().GetValue<Fitness::GaFitness>( GADV_AVG_FITNESS ).GetCurrentValue().GetProbabilityBase();

			float factor = ( (const GaScalingFactorParams&)parameters ).GetFactor();

			float a, b, d;

			if( min > ( factor * avg - max ) / ( factor - 1 ) )
			{
				d = max - avg;

				if( fabs( d ) < 0.00001 )
					a = 1, b = 0;
				else
				{
					a = avg / d;
					b = a * ( max - factor * avg );
					a *= ( factor - 1 );
				}
			}
			else
			{
				d = avg - min;

				if( fabs( d ) < 0.00001 )
					a = 1, b = 0;
				else
				{
					a = avg / d;
					b = -min * a;
				}
			}

			// scale chromosomes
			Common::Workflows::GaParallelExec1<GaPopulation, GaChromosomeStorage> workDist( *branch, population );
			workDist.Execute( GaLinearScalingOp( a, b ), false );

			// update operation time statistics
			timer.UpdateStatistics();
		}

		/// <summary><c>GaSigmaTruncationScalingOp</c> class represent operation that calculates scaled fitness by applying sigma truncation method.</summary>
		class GaSigmaTruncationScalingOp
		{

		private:

			/// <summary>Average probability base of chromosomes' fitnesses in the population.</summary>
			float _average;

			/// <summary>Total deviation of the population.</summary>
			float _deviation;

			/// <summary>Truncation multiplier.</summary>
			float _factor;

		public:

			/// <summary>Initalizes operation.</summary>
			/// <param name="average">average probability base of chromosomes' fitnesses in the population.</param>
			/// <param name="deviation">total deviation of the population.</param>
			/// <param name="factor">truncation multiplier.</param>
			GaSigmaTruncationScalingOp(float average,
				float deviation,
				float factor) : _average(average),
				_deviation(deviation),
				_factor(factor) { }

			/// <summary><c>operator ()</c> perform scaling operation on chromosome.</summary>
			/// <param name="chromosome">chromosome whose fitness should be scaled.</param>
			/// <param name="index">index of the the chromosome.</param>
			inline void GACALL operator ()(GaChromosomeStorage& chromosome,
				int index)
			{
				( (Fitness::Representation::GaSVFitness<float>&)chromosome.GetFitness( GaChromosomeStorage::GAFT_SCALED ) ).SetValue(
					chromosome.GetFitness( GaChromosomeStorage::GAFT_RAW ).GetProbabilityBase() - ( _average - _factor * _deviation ) );
			}

		};

		// Performs sigma truncation scaling on chromosomes in the population
		void GaSigmaTruncationScaling::operator ()(GaPopulation& population,
			const GaScalingParams& parameters,
			const GaScalingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			GaOperationTime timer( population, GADV_SCALING_TIME );

			population.UpdateTracker( GaPopulationSizeTracker::TRACKER_ID, branch );
			population.UpdateTracker( GaRawFitnessTracker::TRACKER_ID, branch );
			population.UpdateTracker( GaRawDeviationTracker::TRACKER_ID, branch );

			// mark population as rescaled
			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
				population.GetFlags().SetFlags( GaPopulation::GAPF_COMPLETE_SCALED_FITNESS_UPDATE );

			float avg = population.GetStatistics().GetValue<Fitness::GaFitness>( GADV_AVG_FITNESS ).GetCurrentValue().GetProbabilityBase();
			float dev = population.GetStatistics().GetValue<float>( GADV_DEVIATION ).GetCurrentValue();

			// scale chromosomes
			Common::Workflows::GaParallelExec1<GaPopulation, GaChromosomeStorage> workDist( *branch, population );
			workDist.Execute( GaSigmaTruncationScalingOp( avg, dev, ( (const GaScalingFactorParams&)parameters ).GetFactor() ), false );

			// update operation time statistics
			timer.UpdateStatistics();
		}

	} // ScalingOperations
} // Population

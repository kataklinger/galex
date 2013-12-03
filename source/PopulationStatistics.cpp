
/*! \file PopulationStatistics.cpp
    \brief This file contains implementations of classes that keep track of population's statistics.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#include "PopulationStatistics.h"
#include "Population.h"

namespace Population
{

	/// <summary>Default combiner for summing fitness values.</summary>
	static Statistics::GaSumValueCombiner<Fitness::GaFitness> _sumFitnessCombiner;

	/// <summary>Default combiner for summing integer values.</summary>
	static Statistics::GaSumValueCombiner<int> _sumIntCombiner;

	/// <summary>Default combiner for summing 64-bit integer values.</summary>
	static Statistics::GaSumValueCombiner<long long> _sumInt64Combiner;

	/// <summary>Default combiner for summing float values.</summary>
	static Statistics::GaSumValueCombiner<float> _sumFloatCombiner;

	/// <summary>Map of default value combiners.</summary>
	static Statistics::GaValueCombiner* _defaultCombiners[ GADV_NUMBER_OF_BUILTIN_VALUES ] =
	{
		&_sumIntCombiner,
		NULL, NULL,
		&_sumFitnessCombiner, NULL,
		NULL, NULL,
		&_sumFitnessCombiner, NULL,
		&_sumFloatCombiner, &_sumFloatCombiner,
		NULL, NULL, NULL, NULL,
		&_sumIntCombiner, &_sumIntCombiner, &_sumIntCombiner, &_sumIntCombiner, 
		&_sumIntCombiner,
		&_sumInt64Combiner, &_sumInt64Combiner, &_sumInt64Combiner, &_sumInt64Combiner, 
		&_sumInt64Combiner
	};

	/// <summary>Default evaluator for calculating average raw fitness value.</summary>
	static Statistics::GaDivEvaluator<Fitness::GaFitness, int, Fitness::GaFitness> _avgFitnessEvaluator( GADV_TOTAL_FITNESS, GADV_POPULATION_SIZE, GADV_AVG_FITNESS );

	/// <summary>Default evaluator for calculating average scaled fitness value.</summary>
	static Statistics::GaDivEvaluator<Fitness::GaFitness, int, Fitness::GaFitness> _avgScaledFitnessEvaluator( GADV_TOTAL_FITNESS_SCALED, GADV_POPULATION_SIZE, GADV_AVG_FITNESS_SCALED );

	/// <summary>Default evaluator for calculating average variance of the fitness values.</summary>
	static Statistics::GaDivEvaluator<float, int, float> _varEvaluator( GADV_VARIANCE_BASE, GADV_POPULATION_SIZE, GADV_VARIANCE );

	/// <summary>Default evaluator for calculating average variance of the scaled fitness values.</summary>
	static Statistics::GaDivEvaluator<float, int, float> _varScaledEvaluator( GADV_VARIANCE_BASE_SCALED, GADV_POPULATION_SIZE, GADV_VARIANCE_SCALED );

	/// <summary>Default evaluator for calculating average deviation of the fitness values.</summary>
	static Statistics::SqrtEvaluator<float, float> _devEvaluator( GADV_VARIANCE, GADV_DEVIATION );

	/// <summary>Default evaluator for calculating average deviation of the scaled fitness values.</summary>
	static Statistics::SqrtEvaluator<float, float> _devScaledEvaluator( GADV_VARIANCE_SCALED, GADV_DEVIATION_SCALED );

	/// <summary>Map of default value evaluators.</summary>
	static Statistics::GaValueEvaluator* _defaultEvaluators[ GADV_NUMBER_OF_BUILTIN_VALUES ] =
	{
		NULL,
		NULL, NULL, NULL,
		&_avgFitnessEvaluator,
		NULL, NULL, NULL,
		&_avgScaledFitnessEvaluator,
		NULL, NULL,
		&_varEvaluator, &_varScaledEvaluator,
		&_devEvaluator, &_devScaledEvaluator,
		NULL, NULL, NULL, NULL, 
		NULL,
		NULL, NULL, NULL, NULL, 
		NULL
	};

	// Global instance of default factory
	GaDefaultValueHistoryFactory* GaDefaultValueHistoryFactory::_instance = NULL;

	// Creates statistical value with default combiner (if it is independent value) or default eveluator (if it is evaluated value)
	Statistics::GaValueHistoryBase* GaDefaultValueHistoryFactory::CreateValue(Statistics::GaStatistics& statistics,
		int valueID,
		int historyDepth/* = -1*/) const
	{
		switch( valueID )
		{

		case GADV_POPULATION_SIZE:
		case GADV_VARIANCE_BASE:
		case GADV_VARIANCE_BASE_SCALED:
		case GADV_BEST_FITNESS:
		case GADV_WORST_FITNESS:
		case GADV_TOTAL_FITNESS:
		case GADV_TOTAL_FITNESS_SCALED:
		case GADV_BEST_FITNESS_SCALED:
		case GADV_WORST_FITNESS_SCALED:
		case GADV_MATING_COUNT:
		case GADV_CROSSOVER_COUNT:
		case GADV_MUTATION_COUNT:
		case GADV_ACCEPTED_MUTATION_COUNT:
		case GADV_SELECTION_COUNT:
		case GADV_SELECTION_TIME:
		case GADV_COUPLING_TIME:
		case GADV_REPLACEMENT_TIME:
		case GADV_SCALING_TIME:
		case GADV_GENERATION_TIME:
			return CreateValue( statistics, valueID, _defaultCombiners[ valueID ], historyDepth );

		case GADV_AVG_FITNESS:
		case GADV_AVG_FITNESS_SCALED:
		case GADV_VARIANCE:
		case GADV_VARIANCE_SCALED:
		case GADV_DEVIATION:
		case GADV_DEVIATION_SCALED:
			return CreateValue( statistics, valueID, _defaultEvaluators[ valueID ], historyDepth );

		default:

			GA_ARG_THROW( Common::Exceptions::GaArgumentOutOfRangeException, "id", "This factory method cannot recognize specified value ID.", "Statistics" );

		}
	}

	// Creates new independent statistical value and insert it into the statistics object 
	Statistics::GaValueHistoryBase* GaDefaultValueHistoryFactory::CreateValue(Statistics::GaStatistics& statistics,
		int valueID,
		Statistics::GaValueCombiner* combiner,
		int historyDepth/* = -1*/) const
	{
		Statistics::GaValueHistoryBase* value = NULL;
		bool accumulating = false;

		switch( valueID )
		{

		case GADV_MATING_COUNT:
		case GADV_CROSSOVER_COUNT:
		case GADV_MUTATION_COUNT:
		case GADV_ACCEPTED_MUTATION_COUNT:
		case GADV_SELECTION_COUNT:

			accumulating = true;

		case GADV_POPULATION_SIZE:

			// create value
			value = historyDepth < 0
				? new Statistics::GaValueHistory<int>( &statistics, combiner, accumulating )
				: new Statistics::GaValueHistory<int>( &statistics, combiner, historyDepth, accumulating );

			break;

		case GADV_COUPLING_TIME:
		case GADV_REPLACEMENT_TIME:
		case GADV_SCALING_TIME:
		case GADV_GENERATION_TIME:

			accumulating = true;

			// create value
			value = historyDepth < 0
				? new Statistics::GaValueHistory<long long>( &statistics, combiner, accumulating )
				: new Statistics::GaValueHistory<long long>( &statistics, combiner, historyDepth, accumulating );

			break;

		case GADV_VARIANCE_BASE:
		case GADV_VARIANCE_BASE_SCALED:

			// create value
			value = historyDepth < 0
				? new Statistics::GaValueHistory<float>( &statistics, combiner, accumulating )
				: new Statistics::GaValueHistory<float>( &statistics, combiner, historyDepth, accumulating );

			break;

		case GADV_BEST_FITNESS:
		case GADV_WORST_FITNESS:
		case GADV_TOTAL_FITNESS:
		case GADV_TOTAL_FITNESS_SCALED:
		case GADV_BEST_FITNESS_SCALED:
		case GADV_WORST_FITNESS_SCALED:

			// create value
			value = historyDepth < 0
				? new Statistics::GaValueHistory<Fitness::GaFitness>( &statistics, combiner, accumulating )
				: new Statistics::GaValueHistory<Fitness::GaFitness>( &statistics, combiner, historyDepth, accumulating );

			break;

		default:

			GA_ARG_THROW( Common::Exceptions::GaArgumentOutOfRangeException, "id", "This factory method cannot recognize specified value ID.", "Statistics" );

		}

		// inset value into statistics object
		statistics.AddValue( valueID, value );
		return value;
	}

	// Creates new evaluated statistical value and insert it into the statistics object 
	Statistics::GaValueHistoryBase* GaDefaultValueHistoryFactory::CreateValue(Statistics::GaStatistics& statistics,
		int valueID,
		Statistics::GaValueEvaluator* evaluator,
		int historyDepth/* = -1*/) const
	{
		Statistics::GaValueHistoryBase* value = NULL;

		// evaluator is not specified - use default evaluator
		if( !evaluator && valueID >=0 && valueID < GADV_NUMBER_OF_BUILTIN_VALUES )
			evaluator = _defaultEvaluators[ valueID ];

		switch( valueID )
		{

		case GADV_AVG_FITNESS:
		case GADV_AVG_FITNESS_SCALED:

			// create value
			value = historyDepth < 0
				? new Statistics::GaValueHistory<Fitness::GaFitness>( &statistics, evaluator, false )
				: new Statistics::GaValueHistory<Fitness::GaFitness>( &statistics, evaluator, historyDepth, false );

			break;

		case GADV_VARIANCE:
		case GADV_VARIANCE_SCALED:
		case GADV_DEVIATION:
		case GADV_DEVIATION_SCALED:

			// create value
			value = historyDepth < 0
				? new Statistics::GaValueHistory<float>( &statistics, evaluator, false )
				: new Statistics::GaValueHistory<float>( &statistics, evaluator, historyDepth, false );

			break;

		default:

			GA_ARG_THROW( Common::Exceptions::GaArgumentOutOfRangeException, "id", "This factory method cannot recognize specified value ID.", "Statistics" );

		}

		// inset value into statistics object
		statistics.AddValue( valueID, value );
		return value;
	}

	// Inserts specified statistical values to statistics object
	void GaPopulationStatTracker::InsertValues(Statistics::GaStatistics& statistics,
		Statistics::GaValueHistoryFactory& factory,
		const int* IDs,
		int count) const
	{
		GA_ARG_ASSERT( Common::Exceptions::GaNullArgumentException, IDs != NULL, "IDs", "Array that contains IDs of values that should be added must be specified.", "Population" );
		GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, count >= 0, "count", "Count of values that should be added must be positive or 0.", "Population" );

		int i = 0;

		try
		{
			// create values and insert them to statistics
			for( ; i < count; i++ )
				factory.CreateValue( statistics, IDs[ i ] );
		}
		catch( ... )
		{
			RemoveValues( statistics, IDs, i );
			throw;
		}
	}

	// Removes specified statistical values to statistics object
	void GaPopulationStatTracker::RemoveValues(Statistics::GaStatistics& statistics,
		const int* IDs,
		int count) const
	{
		GA_ARG_ASSERT( Common::Exceptions::GaNullArgumentException, IDs != NULL, "IDs", "Array that contains IDs of values that should be removed must be specified.", "Population" );
		GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, count >= 0, "count", "Count of values that should be removed must be positive or 0.", "Population" );

		// remove statistical values from statistics object and destory them
		for( count--; count >= 0; count-- )
			statistics.RemoveValue( IDs[ count ] );
	}

	// Tracked values
	const int GaPopulationSizeTracker::BindingValues[ GaPopulationSizeTracker::BindingValuesCount ] =
	{
		GADV_POPULATION_SIZE
	};

	// Adds tracked values to population's statistics
	void GaPopulationSizeTracker::Bind(GaPopulation& population)
	{
		InsertValues( population.GetStatistics(), *GaDefaultValueHistoryFactory::GetInstance(), BindingValues, BindingValuesCount );
	}

	// Removes tracked values from population's statistics
	void GaPopulationSizeTracker::Unbind(GaPopulation& population)
	{
		RemoveValues( population.GetStatistics(), BindingValues, BindingValuesCount );
	}

	// Update statistics
	void GaPopulationSizeTracker::Evaluate(GaPopulation& population,
		Common::Workflows::GaBranch* branch) const
	{
		GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
			population.GetStatistics().GetValue<int>( BindingValues[ 0 ] ).SetCurrent( population.GetCount() );
	}

	/// <summary><c>GaTotalFitnessUpdateAddOp</c> class represents operation that sums fitness values of chromosomes into total fitness of the population.</summary>
	class GaTotalFitnessUpdateAddOp
	{

	private:

		/// <summary>Type of fitness value that is used (raw or scaled).</summary>
		int _fitnessType;

		/// <summary>Fitness object that stores total fitness sum.</summary>
		Fitness::GaFitness& _totalFitness;

	public:

		/// <summary>Initializes operation.</summary>
		/// <param name="fitnessType">type of fitness value that is used (raw or scaled). Use <see cref="GaChromosomeStorage::GaFitnessType" /> enum values.</param>
		/// <param name="totalFitness">fitness object that stores total fitness sum.</param>
		GaTotalFitnessUpdateAddOp(int fitnessType,
			Fitness::GaFitness& totalFitness) : _fitnessType(fitnessType),
			_totalFitness(totalFitness) { }

		/// <summary><c>operator ()</c> sums fitness of the chromosome with the total fitness.</summary>
		/// <param name="chromosome">chromosomes whose fitness is summed.</param>
		/// <param name="index">index of the chromosomes.</param>
		inline void GACALL operator ()(GaChromosomeStorage& chromosome,
			int index) { _totalFitness += chromosome.GetFitness( (GaChromosomeStorage::GaFitnessType)_fitnessType ); }

	};

	/// <summary><c>GaTotalFitnessUpdateSubOp</c> class represents operation that substract fitness values of chromosomes from the total fitness of the population.</summary>
	class GaTotalFitnessUpdateSubOp
	{

	private:

		/// <summary>Type of fitness value that is used (raw or scaled).</summary>
		int _fitnessType;

		/// <summary>Fitness object that stores total fitness sum.</summary>
		Fitness::GaFitness& _totalFitness;

	public:

		/// <summary>Initializes operation.</summary>
		/// <param name="fitnessType">type of fitness value that is used (raw or scaled). Use <see cref="GaChromosomeStorage::GaFitnessType" /> enum values.</param>
		/// <param name="totalFitness">fitness object that stores total fitness sum.</param>
		GaTotalFitnessUpdateSubOp(int fitnessType,
			Fitness::GaFitness& totalFitness) : _fitnessType(fitnessType),
			_totalFitness(totalFitness) { }

		/// <summary><c>operator ()</c> substracts fitness of the chromosome from the total fitness.</summary>
		/// <param name="chromosome">chromosomes whose fitness is substracted.</param>
		/// <param name="index">index of the chromosomes.</param>
		inline void GACALL operator ()(GaChromosomeStorage& chromosome,
			int index) { _totalFitness -= chromosome.GetFitness( (GaChromosomeStorage::GaFitnessType)_fitnessType ); }

	};

	// Tracked values
	const int GaFitnessTracker::BindingValues[ 2 ][ GaFitnessTracker::BindingValuesCount ] =
	{
		//raw
		{
			GADV_BEST_FITNESS,
			GADV_WORST_FITNESS,
			GADV_TOTAL_FITNESS,
			GADV_AVG_FITNESS
		},

		// scaled
		{
			GADV_BEST_FITNESS_SCALED,
			GADV_WORST_FITNESS_SCALED,
			GADV_TOTAL_FITNESS_SCALED,
			GADV_AVG_FITNESS_SCALED
		}
	};

	// Flags that indicates that all chromosomes in the population have updated fitness values
	const int GaFitnessTracker::CompleteFitnessUpdateFlags[ 2 ] = { GaPopulation::GAPF_COMPLETE_FITNESS_UPDATE, GaPopulation::GAPF_COMPLETE_SCALED_FITNESS_UPDATE };

	// IDs of fitness operating change events
	const int GaFitnessTracker::TrackedEvent[ 2 ] = { GaPopulation::GAPE_FITNESS_OPERATION_CHANGED, GaPopulation::GAPE_SCALED_FITNESS_PROTOTYPE_CHANGED, };

	// Prepares population's statistics for the tracker
	void GaFitnessTracker::Prepare(GaPopulation& population)
	{
		GaChromosomeStorage::GaFitnessType ft = (GaChromosomeStorage::GaFitnessType)_fitnessType;

		// check if the fitness object that should store value is already created
		Statistics::GaValueHistory<Fitness::GaFitness>& history = population.GetStatistics().GetValue<Fitness::GaFitness>( BindingValues[ ft ][ 2 ] );
		if( !history.GetCurrent().HasValue() )
		{
			// create fitness object that will store value
			Common::Memory::GaAutoPtr<Fitness::GaFitness> fitnessPrototype = population.CreateFitnessObject( ft );
			if( !fitnessPrototype.IsNull() )
				history.SetCurrent( *fitnessPrototype );
		}
	}

	// Adds tracked values to population's statistics
	void GaFitnessTracker::Bind(GaPopulation& population)
	{
		InsertValues( population.GetStatistics(), *GaDefaultValueHistoryFactory::GetInstance(), BindingValues[ _fitnessType ], BindingValuesCount );
		population.GetEventManager().AddEventHandler( TrackedEvent[ _fitnessType ], &_eventHandler );
	}

	// Removes tracked values from population's statistics
	void GaFitnessTracker::Unbind(GaPopulation& population)
	{
		RemoveValues( population.GetStatistics(), BindingValues[ _fitnessType ], BindingValuesCount );
		population.GetEventManager().RemoveEventHandler( TrackedEvent[ _fitnessType ], &_eventHandler );
	}

	// Update statistics
	void GaFitnessTracker::Evaluate(GaPopulation& population,
		Common::Workflows::GaBranch* branch) const
	{
		int branchID = branch->GetFilteredID();
		int branchCount= branch->GetBarrierCount();

		GaChromosomeStorage::GaFitnessType ft = (GaChromosomeStorage::GaFitnessType)_fitnessType;
		Statistics::GaStatistics& stats = population.GetStatistics();

		Common::Memory::GaAutoPtr<Fitness::GaFitness> totalFitness = population.CreateFitnessObject( ft );

		// fitness of all chromosome in the population has been updated?
		bool complete = population.GetFlags().IsFlagSetAny( CompleteFitnessUpdateFlags[ ft ] ) ||
			population.GetCount() < population.GetNewChromosomes().GetCount() + population.GetRemovedChromosomes().GetCount();
		if( complete )
		{
			// sum fitness values of all chromosomes in the population
			Common::Workflows::GaParallelExec1<GaPopulation, GaChromosomeStorage> popWorkDist( *branch, population );
			popWorkDist.Execute( GaTotalFitnessUpdateAddOp( ft, *totalFitness ), false );
		}
		else
		{
			if( branchID == 0 )
				( *totalFitness ) += stats.GetValue<Fitness::GaFitness>( BindingValues[ ft ][ 2 ] ).GetCurrent();

			// add fitness of new chromosomes to total fitness
			Common::Workflows::GaParallelExec1<GaChromosomeGroup, GaChromosomeStorage> newWorkDist( *branch, population.GetNewChromosomes() );
			newWorkDist.Execute( GaTotalFitnessUpdateAddOp( ft, *totalFitness ), false );

			// substract fitness of removed chromosomes from total fitness
			Common::Workflows::GaParallelExec1<GaChromosomeGroup, GaChromosomeStorage> oldWorkDist( *branch, population.GetRemovedChromosomes() );
			oldWorkDist.Execute( GaTotalFitnessUpdateSubOp( ft, *totalFitness ), false );
		}

		Statistics::GaValueHistory<Fitness::GaFitness>& value = stats.GetValue<Fitness::GaFitness>( BindingValues[ ft ][ 2 ] );

		GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
		{
			// get chromosomes with the best and worst fitness
			stats.GetValue<Fitness::GaFitness>( BindingValues[ ft ][ 0 ] ).SetCurrent( population[ 0 ].GetFitness( ft ) );
			stats.GetValue<Fitness::GaFitness>( BindingValues[ ft ][ 1 ] ).SetCurrent( population[ population.GetCount() - 1 ].GetFitness( ft ) );

			value.GetCurrent().GetValue().Clear();
		}

		GA_LOCK_OBJECT( lock, &stats );

		// sum results of all branches
		value.SetCurrent( value.GetCurrentValue() + *totalFitness );
	}

	// Handles fitness operation changes
	void GaFitnessTracker::FitnessOperationChanged(int id,
			Common::Observing::GaEventData& eventData) const
	{
		GaPopulation& population = ( (GaPopulationEventData&)eventData ).GetPopulation();

		for( int i = BindingValuesCount - 1; i >= 0; i-- )
			population.GetStatistics().GetValue( BindingValues[ _fitnessType ][ i ] ).Clear();

		GaChromosomeStorage::GaFitnessType ft = (GaChromosomeStorage::GaFitnessType)_fitnessType;

		Common::Memory::GaAutoPtr<Fitness::GaFitness> fitnessPrototype = population.CreateFitnessObject( ft );
		if( !fitnessPrototype.IsNull() )
			population.GetStatistics().GetValue<Fitness::GaFitness>( BindingValues[ ft ][ 2 ] ).SetCurrent( *fitnessPrototype );
	}

	/// <summary><c>GaVarianceUpdateAddOp</c> class represents operation that sums varaince of chromosomes into total variance of the population.</summary>
	class GaVarianceUpdateAddOp
	{

	private:

		/// <summary>Type of fitness value that is used (raw or scaled).</summary>
		int _fitnessType;

		/// <summary>Average fitness of the chromosomes in the population.</summary>
		float _avgFitness;

		/// <summary>Reference to variable that stores total variance of the population.</summary>
		float& _variance;

	public:

		/// <summary>Initializes operation.</summary>
		/// <param name="fitnessType">type of fitness value that is used (raw or scaled). Use <see cref="GaChromosomeStorage::GaFitnessType" /> enum values.</param>
		/// <param name="avgFitness">average fitness of the chromosomes in the population.</param>
		/// <param name="variance">reference to variable that stores total variance of the population.</param>
		GaVarianceUpdateAddOp(int fitnessType,
			float avgFitness,
			float& variance) : _fitnessType(fitnessType),
			_avgFitness(avgFitness),
			_variance(variance) { }

		/// <summary><c>operator ()</c> sums variance of the chromosome with the total variance of the population.</summary>
		/// <param name="chromosome">chromosomes whose fitness is summed.</param>
		/// <param name="index">index of the chromosomes.</param>
		inline void GACALL operator ()(GaChromosomeStorage& chromosome,
			int index)
		{
			float diff = chromosome.GetFitness( (GaChromosomeStorage::GaFitnessType)_fitnessType ).GetProbabilityBase() - _avgFitness;
			_variance += diff * diff;
		}

	};

	/// <summary><c>GaVarianceUpdateSubOp</c> class represents operation that substract variance of chromosomes from the total variance of the population.</summary>
	class GaVarianceUpdateSubOp
	{

	private:

		/// <summary>Type of fitness value that is used (raw or scaled)</summary>
		int _fitnessType;

		/// <summary>Average fitness of the chromosomes in the population.</summary>
		float _avgFitness;

		/// <summary>Reference to variable that stores total variance of the population.</summary>
		float& _variance;

	public:

		/// <summary>Initializes operation.</summary>
		/// <param name="fitnessType">type of fitness value that is used (raw or scaled). Use <see cref="GaChromosomeStorage::GaFitnessType" /> enum values.</param>
		/// <param name="avgFitness">average fitness of the chromosomes in the population.</param>
		/// <param name="variance">reference to variable that stores total variance of the population.</param>
		GaVarianceUpdateSubOp(int fitnessType,
			float avgFitness,
			float& variance) : _fitnessType(fitnessType),
			_avgFitness(avgFitness),
			_variance(variance) { }

		/// <summary><c>operator ()</c> substracts varaince of the chromosome from the total variance of the population.</summary>
		/// <param name="chromosome">chromosomes whose variance is substracted.</param>
		/// <param name="index">index of the chromosomes.</param>
		inline void GACALL operator ()(GaChromosomeStorage& chromosome,
			int index)
		{
			float diff = chromosome.GetFitness( (GaChromosomeStorage::GaFitnessType)_fitnessType ).GetProbabilityBase() - _avgFitness;
			_variance -= diff * diff;
		}

	};

	// Tracked values
	const int GaDeviationTracker::BindingValues[ 2 ][ GaDeviationTracker::BindingValuesCount ] =
	{
		// raw
		{
			GADV_VARIANCE_BASE,
			GADV_VARIANCE,
			GADV_DEVIATION
		},

		//scaled
		{
			GADV_VARIANCE_BASE_SCALED,
			GADV_VARIANCE_SCALED,
			GADV_DEVIATION_SCALED
		}
	};

	// IDs of statistical values that stores source for calculating deviation and variance 
	const int GaDeviationTracker::AvgFitnessSource[ 2 ] = { GADV_AVG_FITNESS, GADV_AVG_FITNESS_SCALED };

	// Flags that indicates that all chromosomes in the population have updated fitness values
	const int GaDeviationTracker::CompleteFitnessUpdateFlags[ 2 ] = { GaPopulation::GAPF_COMPLETE_FITNESS_UPDATE, GaPopulation::GAPF_COMPLETE_SCALED_FITNESS_UPDATE };

	// IDs of fitness operating change events
	const int GaDeviationTracker::TrackedEvent[ 2 ] = { GaPopulation::GAPE_FITNESS_OPERATION_CHANGED, GaPopulation::GAPE_SCALED_FITNESS_PROTOTYPE_CHANGED, };

	// Adds tracked values to population's statistics
	void GaDeviationTracker::Bind(GaPopulation& population)
	{
		InsertValues( population.GetStatistics(), *GaDefaultValueHistoryFactory::GetInstance(), BindingValues[ _fitnessType ], BindingValuesCount );
		population.GetEventManager().AddEventHandler( TrackedEvent[ _fitnessType ], &_eventHandler );
	}

	// Removes tracked values from population's statistics
	void GaDeviationTracker::Unbind(GaPopulation& population)
	{
		RemoveValues( population.GetStatistics(), BindingValues[ _fitnessType ], BindingValuesCount);
		population.GetEventManager().RemoveEventHandler( TrackedEvent[ _fitnessType ], &_eventHandler );
	}

	// Update statistics
	void GaDeviationTracker::Evaluate(GaPopulation& population,
		Common::Workflows::GaBranch* branch) const
	{
		int branchID = branch->GetFilteredID();
		int branchCount= branch->GetBarrierCount();

		GaChromosomeStorage::GaFitnessType ft = (GaChromosomeStorage::GaFitnessType)_fitnessType;

		Statistics::GaStatistics& stats = population.GetStatistics();
		float avg = stats.GetValue<Fitness::GaFitness>( AvgFitnessSource[ _fitnessType ] ).GetCurrentValue().GetProbabilityBase(), variance = 0;

		// fitness of all chromosome in the population has been updated?
		bool complete = population.GetFlags().IsFlagSetAny( CompleteFitnessUpdateFlags[ ft ] ) ||
			population.GetCount() < population.GetNewChromosomes().GetCount() + population.GetRemovedChromosomes().GetCount();
		if( complete )
		{
			// calculate variance for all chromosomes
			Common::Workflows::GaParallelExec1<GaPopulation, GaChromosomeStorage> popWorkDist( *branch, population );
			popWorkDist.Execute( GaVarianceUpdateAddOp( ft, avg, variance ), false );
		}
		else
		{
			if( branchID == 0 )
				variance += stats.GetValue<float>( BindingValues[ ft ][ 0 ] ).GetCurrentValue();

			// add variance of new chromosomes
			Common::Workflows::GaParallelExec1<GaChromosomeGroup, GaChromosomeStorage> newWorkDist( *branch, population.GetNewChromosomes() );
			newWorkDist.Execute( GaVarianceUpdateAddOp( ft, avg, variance ), false );

			// substract variance of removed chromosomes
			Common::Workflows::GaParallelExec1<GaChromosomeGroup, GaChromosomeStorage> oldWorkDist( *branch, population.GetRemovedChromosomes() );
			oldWorkDist.Execute( GaVarianceUpdateSubOp( ft, avg, variance ), false );
		}

		GA_LOCK_OBJECT( lock, &stats );

		// sum results of all branches
		float current = stats.GetValue<float>( BindingValues[ ft ][ 0 ] ).GetCurrentValue();
		stats.GetValue<float>( BindingValues[ ft ][ 0 ] ).SetCurrent( current + variance );
	}

	// Handles fitness operation changes
	void GaDeviationTracker::FitnessOperationChanged(int id,
		Common::Observing::GaEventData& eventData) const
	{
		for( int i = BindingValuesCount - 1; i >= 0; i-- )
			( (GaPopulationEventData&)eventData ).GetPopulation().GetStatistics().GetValue( BindingValues[ _fitnessType ][ i ] ).Clear();
	}

	// Tracked values
	const int GaOperationCountTracker::BindingValues[ GaOperationCountTracker::BindingValuesCount ] =
	{
		GADV_MATING_COUNT,
		GADV_CROSSOVER_COUNT,
		GADV_MUTATION_COUNT,
		GADV_ACCEPTED_MUTATION_COUNT,
		GADV_SELECTION_COUNT
	};

	// Adds tracked values to population's statistics
	void GaOperationCountTracker::Bind(GaPopulation& population)
	{
		InsertValues( population.GetStatistics(), *GaDefaultValueHistoryFactory::GetInstance(), BindingValues, BindingValuesCount );
	}

	// Removes tracked values from population's statistics
	void GaOperationCountTracker::Unbind(GaPopulation& population)
	{
		RemoveValues( population.GetStatistics(), BindingValues, BindingValuesCount );
	}

	// Tracked values
	const int GaOperationTimeTracker::BindingValues[ GaOperationTimeTracker::BindingValuesCount ] =
	{
		GADV_SELECTION_TIME,
		GADV_COUPLING_TIME,
		GADV_REPLACEMENT_TIME,
		GADV_SCALING_TIME,
		GADV_GENERATION_TIME
	};

	// Adds tracked values to population's statistics
	void GaOperationTimeTracker::Bind(GaPopulation& population)
	{
		InsertValues( population.GetStatistics(), *GaDefaultValueHistoryFactory::GetInstance(), BindingValues, BindingValuesCount );
	}

	// Removes tracked values from population's statistics
	void GaOperationTimeTracker::Unbind(GaPopulation& population)
	{
		RemoveValues( population.GetStatistics(), BindingValues, BindingValuesCount );
	}

	// Update statistics
	void GaOperationTimeTracker::Evaluate(GaPopulation& population,
		Common::Workflows::GaBranch* branch) const
	{
		Statistics::GaStatistics& stats = population.GetStatistics();

		GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
		{
			// calculate and store time that has passed during one generation
			stats.GetValue<long long>( GADV_GENERATION_TIME ).SetCurrent( stats.GetCurrentTimeLowRes() );
			stats.ResetTime();
		}
	}

} // Population

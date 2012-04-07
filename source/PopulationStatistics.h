
/*! \file PopulationStatistics.h
    \brief This file contains declarations of classes that keep track of population's statistics.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_POPULATION_STATISTICS_H__
#define __GA_POPULATION_STATISTICS_H__

#include "Observing.h"
#include "Workflows.h"
#include "Statistics.h"

namespace Population
{

	/// <summary><c>GaDefaultValues</c> enumeration defines built-in values tracked by <see cref="GaStatistics" /> class.</summary>
	enum GaDefaultValues
	{

		/// <summary>Number of chromosomes in population.</summary>
		GADV_POPULATION_SIZE,

		/// <summary>Fitness value of the best chromosome in population(s).</summary>
		GADV_BEST_FITNESS, 

		/// <summary>Fitness value of the worst chromosome in population(s).</summary>
		GADV_WORST_FITNESS, 

		/// <summary>Sum of fitness values of all chromosomes in population(s).</summary>
		GADV_TOTAL_FITNESS, 

		/// <summary>Average fitness value of all chromosomes in population(s).</summary>
		GADV_AVG_FITNESS, 

		/// <summary>Scaled fitness value of the best chromosome in population.</summary>
		GADV_BEST_FITNESS_SCALED, 

		/// <summary>Scaled fitness value of the worst chromosome in population.</summary>
		GADV_WORST_FITNESS_SCALED, 

		/// <summary>Sum of scaled of all chromosomes in population.</summary>
		GADV_TOTAL_FITNESS_SCALED,

		/// <summary>Average scaled fitness value of all chromosomes in population.</summary>
		GADV_AVG_FITNESS_SCALED, 

		/// <summary>Total variance of fitness values.</summary>
		GADV_VARIANCE_BASE,

		/// <summary>Total variance of scaled fitness values.</summary>
		GADV_VARIANCE_BASE_SCALED,

		/// <summary>Average variance of fitness values.</summary>
		GADV_VARIANCE,

		/// <summary>Average variance of scaled fitness values.</summary>
		GADV_VARIANCE_SCALED,

		/// <summary>Average deviation of fitness values.</summary>
		GADV_DEVIATION,

		/// <summary>Average deviation of scaled fitness values.</summary>
		GADV_DEVIATION_SCALED,

		/// <summary>Count of mating operations performed.</summary>
		GADV_MATING_COUNT,

		/// <summary>Count of crossover operations performed.</summary>
		GADV_CROSSOVER_COUNT,

		/// <summary>Count of mutation operations performed.</summary>
		GADV_MUTATION_COUNT,

		/// <summary>Count of mutation operations that are accepted.</summary>
		GADV_ACCEPTED_MUTATION_COUNT,

		/// <summary>Count of chromosome selections performed.</summary>
		GADV_SELECTION_COUNT,

		/// <summary>CPU time used to perform selection operation.</summary>
		GADV_SELECTION_TIME,

		/// <summary>CPU time used to perform coupling operation.</summary>
		GADV_COUPLING_TIME,

		/// <summary>CPU time used to perform replacement operation.</summary>
		GADV_REPLACEMENT_TIME,

		/// <summary>CPU time used to perform scaling operation.</summary>
		GADV_SCALING_TIME,

		/// <summary>CPU time used for evolving single generation.</summary>
		GADV_GENERATION_TIME,

		/// <summary>Number of defined built-in values.</summary>
		GADV_NUMBER_OF_BUILTIN_VALUES

	};

	/// <summary><c>GaDefaultValueHistoryFactory</c> class represents statistical value factory that can create all built-in values.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// Because this value factory is stateless all public method are thread-safe.</summary>
	class GaDefaultValueHistoryFactory : public Statistics::GaValueHistoryFactory
	{

	private:

		/// <summary>Global instance of default factory.</summary>
		GAL_API
		static GaDefaultValueHistoryFactory* _instance;

	public:

		/// <summary><c>GetInstance</c> method return pointer to global instance of default factory.</summary>
		/// <returns>Method returns pointer to global instance of default factory.</returns>
		static inline GaDefaultValueHistoryFactory* GACALL GetInstance() { return _instance; }

		/// <summary><c>MakeInstance</c> method makes global instance of default factory if it is not exist.</summary>
		/// <exception cref="GaInvalidOperationException" />Thrown if global instance already exists.</exception>
		static inline void GACALL MakeInstance()
		{
			GA_ASSERT( Common::Exceptions::GaInvalidOperationException, _instance == NULL, "Global instance already exists.", "Population" );
			_instance = new GaDefaultValueHistoryFactory();
		}

		/// <summary><c>FreeInstance</c> method deletes global instance of default factory.</summary>
		/// <exception cref="GaInvalidOperationException" />Thrown if global instance does not exist.</exception>
		static inline void GACALL FreeInstance()
		{
			GA_ASSERT( Common::Exceptions::GaInvalidOperationException, _instance != NULL, "Global instance does not exists.", "Population" );
			delete _instance;
		}

		/// <summary>More details are given in specification of <see cref="GaValueHistoryFactory::CreateValue" /> method.
		///
		/// This method is thread-safe.</summary>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown when the value with <c>valueID</c> cannot be produce by this method.</exception>
		GAL_API
		virtual Statistics::GaValueHistoryBase* GACALL CreateValue(Statistics::GaStatistics& statistics,
			int valueID,
			int historyDepth = -1) const;

		/// <summary>More details are given in specification of <see cref="GaValueHistoryFactory::CreateValue" /> method.
		///
		/// This method is thread-safe.</summary>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown when the value with <c>valueID</c> cannot be produce by this method.</exception>
		GAL_API
		virtual Statistics::GaValueHistoryBase* GACALL CreateValue(Statistics::GaStatistics& statistics,
			int valueID,
			Statistics::GaValueCombiner* combiner,
			int historyDepth = -1) const;

		/// <summary>More details are given in specification of <see cref="GaValueHistoryFactory::CreateEvaluatedValue" /> method.
		///
		/// This method is thread-safe.</summary>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown when the value with <c>valueID</c> cannot be produce by this method.</exception>
		GAL_API
		virtual Statistics::GaValueHistoryBase* GACALL CreateValue(Statistics::GaStatistics& statistics,
			int valueID,
			Statistics::GaValueEvaluator* evaluator,
			int historyDepth = -1) const;

	};

	class GaPopulation;

	/// <summary><c>GaPopulationStatTracker</c> class is interface for statistic trackers that evaluates population and produce statistical data.</summary>
	class GaPopulationStatTracker
	{

	public:

		/// <summary>Virtual destructor must be defined because this is base class.</summary>
		virtual ~GaPopulationStatTracker() { }

		/// <summary>This method binds tracker to the specified statistics object.</summary>
		/// <param name="population">population to which tracker is bound.</param>
		virtual void GACALL Bind(GaPopulation& population) = 0;

		/// <summary>This method unbinds tracker form specified statistics object.</summary>
		/// <param name="population">population to which tracker is bound.</param>
		virtual void GACALL Unbind(GaPopulation& population) = 0;

		/// <summary><c>Evaluate</c> method evaluations population date and stores results to its statistics object.</summary>
		/// <param name="population">reference to population which is evaluated.</param>
		/// <param name="branch">pointer to workflow barnch that performs evaluation.</param>
		virtual void GACALL Evaluate(GaPopulation& population,
			Common::Workflows::GaBranch* branch) const = 0;

	protected:

		/// <summary><c>InsertValues</c> method creates statistical values using provided factory based on specified IDs and inserts them to population's statistics.</summary>
		/// <param name="statistics">reference to statistics object to which the values inserted.</param>
		/// <param name="factory">factory that is used for producing statistical value object.</param>
		/// <param name="IDs">array of identification numbers of statistical values that should be removed.</param>
		/// <param name="count">number of statistical values that should be inserted to statistics object.</param>
		/// <exception cref="GaNullArgumentException" />Thrown if <c>IDs</c> parameter is set to <c>NULL</c>.</exception>
		/// <exception cref="GaArgumentOuntOfRangeException" />Thrown if <c>count</c> is negative number.</exception>
		GAL_API
		void GACALL InsertValues(Statistics::GaStatistics& statistics,
			Statistics::GaValueHistoryFactory& factory,
			const int* IDs,
			int count) const;

		/// <summary><c>RemoveValues</c> method removes statistical values with IDs from population's statistics.</summary>
		/// <param name="statistics">reference to statistics object to which the values inserted.</param>
		/// <param name="IDs">array of identification numbers of statistical values that should be inserted.</param>
		/// <param name="count">number of statistical values that should be removed from statistics object.</param>
		/// <exception cref="GaNullArgumentException" />Thrown if <c>IDs</c> parameter is set to <c>NULL</c>.</exception>
		/// <exception cref="GaArgumentOuntOfRangeException" />Thrown if <c>count</c> is negative number.</exception>
		GAL_API
		void GACALL RemoveValues(Statistics::GaStatistics& statistics,
			const int* IDs,
			int count) const;

	};

	/// <summary><c>GaPopulationSizeTracker</c> class tracks population size.
	///
	/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.
	class GaPopulationSizeTracker : public GaPopulationStatTracker
	{

	public:

		/// <summary>ID used for registring tracker.</summary>
		static const int TRACKER_ID = 1;

		/// <summary>Number of statistical values that is being tracked.</summary>
		static const int BindingValuesCount = 1;

		/// <summary>IDs of statistical values that is being tracked.</summary>
		GAL_API
		static const int BindingValues[ BindingValuesCount ];

		/// <summary>More details are given in specification of <see cref="GaPopulationStatTracker::Bind" /> method.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		virtual void GACALL Bind(GaPopulation& population);

		/// <summary>More details are given in specification of <see cref="GaPopulationStatTracker::Unbind" /> method.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		virtual void GACALL Unbind(GaPopulation& population);

		/// <summary>More details are given in specification of <see cref="GaPopulationStatTracker::Evaluate" /> method.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		virtual void GACALL Evaluate(GaPopulation& population,
			Common::Workflows::GaBranch* branch) const;

	};

	/// <summary><c>GaPopulationSizeTracker</c> base class for trackers that updates general fitness statistics for chromosomes in the population (for raw or scaled fitness values).
	///
	/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.
	class GaFitnessTracker : public GaPopulationStatTracker
	{

	protected:

		/// <summary>Indicates whether the tracker use raw or scaled fitness values.</summary>
		int _fitnessType;

		/// <summary>Event handler that is notified when the fitness operation is changed.</summary>
		Common::Observing::GaMemberEventHandler<const GaFitnessTracker> _eventHandler;

	public:

		/// <summary>Number of statistical values that is being tracked.</summary>
		static const int BindingValuesCount = 4;

		/// <summary>IDs of statistical values that is being tracked.</summary>
		GAL_API
		static const int BindingValues[ 2 ][ BindingValuesCount ];

		/// <summary>Flags that indicates that update of fitness values for all chromosomes in population has occured.</summary>
		GAL_API
		static const int CompleteFitnessUpdateFlags[ 2 ];

		/// <summary>IDs of the fitness operation change events.</summary>
		GAL_API
		static const int TrackedEvent[ 2 ];

		/// <summary>Initializes tracker.</summary>
		/// <param name="fitnessType">indicates whether the tracker use raw or scaled fitness values.
		/// Use <see cref="GaChromosomeStorage::GaFitnessType" /> enum values.</param>
		GaFitnessTracker(int fitnessType) : _fitnessType(fitnessType),
			_eventHandler(this, &GaFitnessTracker::FitnessOperationChanged) { }

		/// <summary>More details are given in specification of <see cref="GaPopulationStatTracker::Bind" /> method.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		virtual void GACALL Bind(GaPopulation& population);

		/// <summary>More details are given in specification of <see cref="GaPopulationStatTracker::Unbind" /> method.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		virtual void GACALL Unbind(GaPopulation& population);

		/// <summary>More details are given in specification of <see cref="GaPopulationStatTracker::Evaluate" /> method.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		virtual void GACALL Evaluate(GaPopulation& population,
			Common::Workflows::GaBranch* branch) const;

	protected:

		/// <summary><c>FitnessOperationChanged</c> handles events raised when fitness operation or its parameters and configuration has been changed.</summary>
		/// <param name="id">ID of the event.</param>
		/// <param name="eventData">additional event data.</param>
		GAL_API
		void GACALL FitnessOperationChanged(int id,
			Common::Observing::GaEventData& eventData) const;

	};

	/// <summary><c>GaRawFitnessTracker</c> class represent trackers that updates general fitness statistics for raw fitness values.
	///
	/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.
	class GaRawFitnessTracker : public GaFitnessTracker
	{

	public:

		/// <summary>ID used for registring tracker.</summary>
		static const int TRACKER_ID = 2;

		/// <summary>Initializes tracker.</summary>
		GaRawFitnessTracker() : GaFitnessTracker(0) { }

	};

	/// <summary><c>GaScaledFitnessTracker</c> class represent trackers that updates general fitness statistics for scaled fitness values.
	///
	/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.
	class GaScaledFitnessTracker : public GaFitnessTracker
	{

	public:

		/// <summary>ID used for registring tracker.</summary>
		static const int TRACKER_ID = 3;

		/// <summary>Initializes tracker.</summary>
		GaScaledFitnessTracker() : GaFitnessTracker(1) { }

	};

	/// <summary><c>GaDeviationTracker</c> base class for trackers that updates deviation and variance of raw or scaled fitness values of chromosomes in the population.
	///
	/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.
	class GaDeviationTracker : public GaPopulationStatTracker
	{

	protected:

		/// <summary>Indicates whether the tracker use raw or scaled fitness values.</summary>
		int _fitnessType;

		/// <summary>Event handler that is notified when the fitness operation is changed.</summary>
		Common::Observing::GaMemberEventHandler<const GaDeviationTracker> _eventHandler;

	public:

		/// <summary>Number of statistical values that is being tracked.</summary>
		static const int BindingValuesCount = 3;

		/// <summary>IDs of statistical values that is being tracked.</summary>
		GAL_API
		static const int BindingValues[ 2 ][ BindingValuesCount ];

		/// <summary>IDs of source fitness value used for calculating deviation and variance.</summary>
		GAL_API
		static const int AvgFitnessSource[ 2 ];

		/// <summary>Flags that indicates that update of fitness values for all chromosomes in population has occured.</summary>
		GAL_API
		static const int CompleteFitnessUpdateFlags[ 2 ];

		/// <summary>IDs of the fitness operation change events.</summary>
		GAL_API
		static const int TrackedEvent[ 2 ];

		/// <summary>Initializes tracker.</summary>
		/// <param name="fitnessType">indicates whether the tracker use raw or scaled fitness values.
		/// Use <see cref="GaChromosomeStorage::GaFitnessType" /> enum values.</param>
		GaDeviationTracker(int fitnessType) : _fitnessType(fitnessType),
			_eventHandler(this, &GaDeviationTracker::FitnessOperationChanged) { }

		/// <summary>More details are given in specification of <see cref="GaPopulationStatTracker::Bind" /> method.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		virtual void GACALL Bind(GaPopulation& population);

		/// <summary>More details are given in specification of <see cref="GaPopulationStatTracker::Unbind" /> method.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		virtual void GACALL Unbind(GaPopulation& population);

		/// <summary>More details are given in specification of <see cref="GaPopulationStatTracker::Evaluate" /> method.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		virtual void GACALL Evaluate(GaPopulation& population,
			Common::Workflows::GaBranch* branch) const;

	protected:

		/// <summary><c>FitnessOperationChanged</c> handles events raised when fitness operation or its parameters and configuration has been changed.</summary>
		/// <param name="id">ID of the event.</param>
		/// <param name="eventData">additional event data.</param>
		GAL_API
		void GACALL FitnessOperationChanged(int id,
			Common::Observing::GaEventData& eventData) const;

	};

	/// <summary><c>GaRawDeviationTracker</c> base class for trackers that updates deviation and variance of raw fitness values of chromosomes in the population.
	///
	/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.
	class GaRawDeviationTracker : public GaDeviationTracker
	{

	public:

		/// <summary>ID used for registring tracker.</summary>
		static const int TRACKER_ID = 4;

		/// <summary>Initializes tracker.</summary>
		GaRawDeviationTracker() : GaDeviationTracker(0) { }

	};

	/// <summary><c>GaRawDeviationTracker</c> base class for trackers that updates deviation and variance of scaled fitness values of chromosomes in the population.
	///
	/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.
	class GaScaledDeviationTracker : public GaDeviationTracker
	{

	public:

		/// <summary>ID used for registring tracker.</summary>
		static const int TRACKER_ID = 5;

		/// <summary>Initializes tracker.</summary>
		GaScaledDeviationTracker() : GaDeviationTracker(1) { }

	};

	/// <summary><c>GaOperationCountTracker</c> base class for trackers that updates counters of basic genetic operation.
	///
	/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.
	class GaOperationCountTracker : public GaPopulationStatTracker
	{

	public:

		/// <summary>ID used for registring tracker.</summary>
		static const int TRACKER_ID = 6;

		/// <summary>Number of statistical values that is being tracked.</summary>
		static const int BindingValuesCount = 5;

		/// <summary>IDs of statistical values that is being tracked.</summary>
		GAL_API
		static const int BindingValues[ BindingValuesCount ];

		/// <summary>More details are given in specification of <see cref="GaPopulationStatTracker::Bind" /> method.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		virtual void GACALL Bind(GaPopulation& population);

		/// <summary>More details are given in specification of <see cref="GaPopulationStatTracker::Unbind" /> method.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		virtual void GACALL Unbind(GaPopulation& population);

		/// <summary>More details are given in specification of <see cref="GaPopulationStatTracker::Evaluate" /> method.
		///
		/// This method is not thread-safe.</summary>
		virtual void GACALL Evaluate(GaPopulation& population,
			Common::Workflows::GaBranch* branch) const { };

	};

	/// <summary><c>GaOperationCountTracker</c> base class for trackers that updates amount of time spent executing basic genetic operation.
	///
	/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.
	class GaOperationTimeTracker : public GaPopulationStatTracker
	{

	public:

		/// <summary>ID used for registring tracker.</summary>
		static const int TRACKER_ID = 7;

		/// <summary>Number of statistical values that is being tracked.</summary>
		static const int BindingValuesCount = 5;

		/// <summary>IDs of statistical values that is being tracked.</summary>
		GAL_API
		static const int BindingValues[ BindingValuesCount ];

		/// <summary>More details are given in specification of <see cref="GaPopulationStatTracker::Bind" /> method.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		virtual void GACALL Bind(GaPopulation& population);

		/// <summary>More details are given in specification of <see cref="GaPopulationStatTracker::Unbind" /> method.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		virtual void GACALL Unbind(GaPopulation& population);

		/// <summary>More details are given in specification of <see cref="GaPopulationStatTracker::Evaluate" /> method.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		virtual void GACALL Evaluate(GaPopulation& population,
			Common::Workflows::GaBranch* branch) const;

	};

} // Population

#endif // __GA_POPULATION_STATISTICS_H__

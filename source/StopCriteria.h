
/*! \file StopCriteria.h
    \brief This file declares classes that represent stop criteria for genetic algorithms.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_STOP_CRITERIA_H__
#define __GA_STOP_CRITERIA_H__

#include "Operation.h"
#include "Workflows.h"
#include "Statistics.h"
#include "Fitness.h"

namespace Algorithm
{

	/// <summary>Contains built-in algorithm stop criteria.</summary>
	namespace StopCriteria
	{

		/// <summary><c>GaStopCriterionParams</c> class is base for parameters of stop criteria.</summary>
		class GaStopCriterionParams : public Common::GaParameters { };

		/// <summary><c>GaStopCriterionConfig</c> class is base for configuration of stop criteria.</summary>
		class GaStopCriterionConfig : public Common::GaConfiguration { };

		/// <summary>This class is interface for scaling operations which transform fitness values of chromosomes into selection probability.</summary>
		class GaStopCriterion : public Common::GaOperation
		{

		public:

			/// <summary><c>operator ()</c> performs evaluates current state of the algorithm and decides whether it should continute execution.</summary>
			/// <param name="stats">object that stores statistical information.</param>
			/// <param name="params">parameters of stop criterion.</param>
			/// <param name="config">configuration of stop criterion.</param>
			/// <param name="branch">pointer to workflow branch that executes stop criterion.</param>
			/// <returns>Method returns <c>true</c> if the criterion is reached and algorithm should stop execution.</returns>
			virtual bool GACALL operator ()(const Statistics::GaStatistics& stats,
				const GaStopCriterionParams& params,
				const GaStopCriterionConfig& config,
				Common::Workflows::GaBranch* branch) const = 0;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateConfiguration" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method always returns <c>NULL</c>.</returns>
			virtual Common::GaConfiguration* GACALL CreateConfiguration() const { return NULL; }

		};

		/// <summary><c>GaStopCriterionSetup</c> type is instance of <see cref="GaOperationSetup" /> template class and represents setup of
		/// stop criterion and its parameters. Detailed description can be found in specification of  <see cref="GaOperationSetup" /> template class.</summary>
		typedef Common::GaOperationSetup<GaStopCriterion, GaStopCriterionParams, GaStopCriterionConfig> GaStopCriterionSetup;

		/// <summary><c>GaStopCriterionStep</c> class represent workflow step which decides when the algorithm should stop execution.
		/// 
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// No public or private methods are thread-safe.</summary>
		class GaStopCriterionStep : public Common::Workflows::GaBinaryDecision
		{

		protected:

			/// <summary>Setup of the stop criterion.</summary>
			GaStopCriterionSetup _criterionSetup;

			/// <summary>Object which stores statistical information used by stop criterion.</summary>
			Common::Workflows::GaDataCache<Statistics::GaStatistics> _data;

		public:

			/// <summary>Initializes step with stop criterion and statistical data on which it operates.</summary>
			/// <param name="setup">stop criterion.</param>
			/// <param name="dataStorage">data storage that contains data.</param>
			/// <param name="dataID">ID of the statistical data.</param>
			GaStopCriterionStep(const GaStopCriterionSetup& criterion,
				Common::Workflows::GaDataStorage* dataStorage,
				int dataID) : _criterionSetup(criterion),
				_data(dataStorage, dataID) { }

			/// <summary>Initializes step with stop criterion.</summary>
			/// <param name="setup">stop criterion.</param>
			GaStopCriterionStep(const GaStopCriterionSetup& criterion) : _criterionSetup(criterion) { }

			/// <summary>Initializes empty step.</summary>
			GaStopCriterionStep() { }

			/// <summary><c>SetOperationSetup</c> method sets new stop criterion.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="setup">new stop criterion.</param>
			inline void GACALL SetOperationSetup(const GaStopCriterionSetup& setup) { _criterionSetup = setup; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns setup of the stop criterion.</returns>
			inline const GaStopCriterionSetup& GACALL GetOperationSetup() const { return _criterionSetup; }

		protected:

			/// <summary><c>Decision</c> method execute stop criterion and returns decision.
			/// More details are given in specification of <see cref="GaBinaryDecision::Decision" />.
			///
			/// This method is not thread-safe.</summary>
			/// <returns>Method returns results of stop criterion.</returns>
			virtual bool GACALL Decision(Common::Workflows::GaBranch* branch) 
				{ return _criterionSetup.GetOperation()( _data.GetData(), _criterionSetup.GetParameters(), _criterionSetup.GetConfiguration(), branch ); }

		};

		/// <summary><c>GaGenerationCriterionParams</c> class is stores parameters for stop criterion that makes decision based on number of generations that have passed.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaGenerationCriterionParams : public GaStopCriterionParams
		{

		private:

			/// <summary>Number of generation that algorithm should execute.</summary>
			int _generationCount;

		public:

			/// <summary>Initializes parameters with number of generation.</summary>
			/// <param name="generationCount">number of generation that algorithm should execute.</param>
			GaGenerationCriterionParams(int generationCount) : _generationCount(generationCount) { }

			/// <summary>Initializes parameters with default number of generation (default value is 1000 generations).</summary>
			GaGenerationCriterionParams() : _generationCount(1000) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaGenerationCriterionParams( *this ); }

			/// <summary><c>SetGenerationCount</c> sets the number of generation that algorithm should execute.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="count">number of generation that algorithm should execute.</param>
			inline void GACALL SetGenerationCount(int count) { _generationCount = count; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of generation that algorithm should execute.</returns>
			inline int GACALL GetGenerationCount() const { return _generationCount; }

		};

		/// <summary><c>GaGenerationCriterion</c> class represent stop criterion that makes decision whether that algorithm should continue execution
		/// based on number of generations that have passed.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaGenerationCriterion : public GaStopCriterion
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaStopCriterion::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual bool GACALL operator ()(const Statistics::GaStatistics& stats,
				const GaStopCriterionParams& params,
				const GaStopCriterionConfig& config,
				Common::Workflows::GaBranch* branch) const { return stats.GetCurrentGeneration() == ( (const GaGenerationCriterionParams&)params ).GetGenerationCount(); }

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaGenerationCriterionParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaGenerationCriterionParams(); }

		};

		/// <summary><c>GaStatsCriterionComparator</c> class handles default value comparison for stop criteria.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this objects of this class are stateless all public method are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of values that are compared.</param>
		template<typename VALUE_TYPE>
		class GaStatsCriterionComparator
		{

		public:

			/// <summary>Type of values that are compared.</summary>
			typedef VALUE_TYPE GaValueType;

			/// <summary><c>operator()</c> compares two values.
			///
			/// This method is thread-safe.</summary>
			/// <param name="value1">the first value.</param>
			/// <param name="value2">the second value.</param>
			/// <returns>Method returns:
			/// <br/>a. -1 if the second value is greater then the first.
			/// <br/>b.  0 if the values are equal.
			/// <br/>c.  1 if the first value is greater then the second.</returns>
			inline int GACALL operator()(const GaValueType& value1,
				const GaValueType& value2) const { return value1 > value2 ? 1 : ( value2 > value1 ? -1 : 0 ); }

		};

		/// <summary>Specialization of <c>GaStatsCriterionComparator</c> for comparison of fitness values that uses fitness comparators.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this objects of this class are stateless all public method are thread-safe.</summary>
		template<>
		class GaStatsCriterionComparator<Fitness::GaFitness>
		{

		private:

			/// <summary>Fitness comparator for comparing values.</summary>
			Fitness::GaFitnessComparatorSetup _comparator;

		public:

			/// <summary>Initializes fitness comparator.</summary>
			/// <param name="comparator">fitness comparator.</param>
			GaStatsCriterionComparator(const Fitness::GaFitnessComparatorSetup& comparator) : _comparator(comparator) { }

			/// <summary>Initializes empty comparator.</summary>
			GaStatsCriterionComparator() { }

			/// <summary><c>operator()</c> compares two fitness values.
			///
			/// This method is thread-safe.</summary>
			/// <param name="v1">the first fitness value.</param>
			/// <param name="v2">the second fitness value.</param>
			/// <returns>Method returns:
			/// <br/>a. -1 if the first chromosome has better fitness value then the second.
			/// <br/>b.  0 if the fitness values are equal.
			/// <br/>c.  1 if the first chromosome has worse fitness value then the second.</returns>
			inline int GACALL operator()(const Fitness::GaFitness& v1,
				const Fitness::GaFitness& v2) const { return _comparator.GetOperation()( v1, v2, _comparator.GetParameters() ); }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns used fitness comparator.</returns>
			inline Fitness::GaFitnessComparatorSetup& GACALL GetFitnessComparator() { return _comparator; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns used fitness comparator.</returns>
			inline const Fitness::GaFitnessComparatorSetup& GACALL GetFitnessComparator() const { return _comparator; }

		};

		/// <summary><c>GaStopType</c> defines possible ways of reaching desired value.</summary>
		enum GaStopType
		{

			/// <summary>Criterion is satisified if the statistical value is lower then desired value.</summary>
			GAST_STOP_IF_LOWER_THEN = 0x1,

			/// <summary>Criterion is satisified if the statistical value is greater then desired value.</summary>
			GAST_STOP_IF_HIGHER_THEN = 0x2,

			/// <summary>Criterion is satisified if the statistical value is equal to desired value.</summary>
			GAST_STOP_IF_EQUAL_TO = 0x4

		};

		/// <summary><c>GaStatsCriterionParams</c> class is base class for parameters of stop criteria that are based on statistical value comparison.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of compared values.</param>
		/// <param name="COMPARATOR_TYPE">type of comparator.</param>
		template<typename VALUE_TYPE,
			typename COMPARATOR_TYPE = GaStatsCriterionComparator<VALUE_TYPE> >
		class GaStatsCriterionParams : public GaStopCriterionParams
		{

		public:

			/// <summary>Type of compared values.</summary>
			typedef VALUE_TYPE GaValueType;

			/// <summary>Type of comparator.</summary>
			typedef COMPARATOR_TYPE GaComparatorType;

		private:

			/// <summary>ID of the statistical value that is compared against desired value.</summary>
			int _valueID;

			/// <summary>Desired value that should be reached.</summary>
			Statistics::GaValue<GaValueType> _desiredValue;

			/// <summary>This attribute defines the way the desired value should be reached.</summary>
			int _stopType;

			/// <summary>Value comparator.</summary>
			GaComparatorType _comparator;

		public:

			/// <summary>Initializes parameters.</summary>
			/// <param name="valueID">ID of the statistical value that is compared against desired value.</param>
			/// <param name="desiredValue">desired value that should be reached.</param>
			/// <param name="stopType">this parameter defines the way the desired value should be reached.</param>
			/// <param name="comparator">value comparator.</param>
			GaStatsCriterionParams(int valueID,
				const GaValueType& desiredValue,
				int stopType,
				const GaComparatorType& comparator = GaComparatorType()) : _valueID(valueID),
				_desiredValue(desiredValue),
				_comparator(comparator),
				_stopType(stopType) { }

			/// <summary>Initializes parameters with default values. <br/>
			/// Value ID is 0, stop criterion is satisifed if the value is greater then or equal to desired value and default value comarator is used for the type.</summary>
			GaStatsCriterionParams() : _valueID(0),
				_stopType(GAST_STOP_IF_HIGHER_THEN | GAST_STOP_IF_EQUAL_TO) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaStatsCriterionParams<GaValueType, GaComparatorType>( *this ); }

			/// <summary><c>SetValueID</c> method sets the ID of statistical value that is used for comparison.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="id">ID of the new value.</param>
			inline void GACALL SetValueID(int id) { _valueID = id; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns value ID of the statistical value that is used for comparison.</returns>
			inline int GACALL GetValueID() const { return _valueID; }

			/// <summary><c>SetDesiredValue</c> method sets desired value.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="desired">new desired value.</param>
			inline void GACALL SetDesiredValue(const GaValueType& desired) { _desiredValue = desired; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns desiired value.</returns>
			inline const GaValueType& GACALL GetDesiredValue() const { return _desiredValue.GetValue(); }

			/// <summary><c>SetStopType</c> method the way desired value should be reached.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="type">defines how desired value should be reached.</param>
			inline void GACALL SetStopType(int type) { _stopType = type; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns the way desired value should be reached.</returns>
			inline int GACALL GetStopType() const { return _stopType; }

			/// <summary><c>SetComparator</c> method sets comparator that is going to be used for value comparison.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="comparator">new value comparator.</param>
			inline void GACALL SetComparator(const GaComparatorType& comparator) { _comparator = comparator; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns used value comparator.</returns>
			inline const GaComparatorType& GetComparator() const { return _comparator; }

		};

		/// <summary><c>IsCriterionReached</c> method compares current and desired values in specified way.</summary>
		/// <param name="TYPE">value type.</param>
		/// <param name="COMPARATOR">type of value comparator.</param>
		/// <param name="current">current value.</param>
		/// <param name="desired">desired value.</param>
		/// <param name="comparator">value comparator.</param>
		/// <param name="type">the way the desired value can be reached.</param>
		/// <returns>Method returns <c>true</c> if the desired value is reached in specified way.</returns>
		template<typename TYPE,
			typename COMPARATOR>
		inline bool IsCriterionReached(const TYPE& current,
			const TYPE& desired,
			const COMPARATOR& comparator,
			int type)
		{
			int cmp = comparator( current, desired );
			return ( type & GAST_STOP_IF_HIGHER_THEN ) && cmp == 1 || 
				( type & GAST_STOP_IF_LOWER_THEN ) && cmp == -1 ||
				( type & GAST_STOP_IF_EQUAL_TO ) && cmp == 0;
		}

		/// <summary><c>GaStatsCriterion</c> class represent stop criterion that makes decision whether that algorithm should continue execution
		/// based on the current statistical value.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of compared values.</param>
		/// <param name="COMPARATOR_TYPE">type of comparator.</param>
		template<typename VALUE_TYPE,
			typename COMPARATOR_TYPE = GaStatsCriterionComparator<VALUE_TYPE> >
		class GaStatsCriterion : public GaStopCriterion
		{

		public:

			/// <summary>Type of compared values.</summary>
			typedef VALUE_TYPE GaValueType;

			/// <summary>Type of comparator.</summary>
			typedef COMPARATOR_TYPE GaComparatorType;

			/// <summary>More details are given in specification of <see cref="GaStopCriterion::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual bool GACALL operator ()(const Statistics::GaStatistics& stats,
				const GaStopCriterionParams& params,
				const GaStopCriterionConfig& config,
				Common::Workflows::GaBranch* branch) const
			{
				const GaStatsCriterionParams<GaValueType, GaComparatorType>& p = (const GaStatsCriterionParams<GaValueType, GaComparatorType>&)params;
				return IsCriterionReached( stats.GetValue<GaValueType>( p.GetValueID() ).GetCurrentValue(), p.GetDesiredValue(), p.GetComparator(), p.GetStopType() );
			}

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaStatsCriterionParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaStatsCriterionParams<GaValueType, GaComparatorType>(); }

		};

		/// <summary><c>GaStatsProgressCriterionParams</c> class is base class for parameters of stop criteria that are based on progress of statistical values.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of compared values.</param>
		/// <param name="COMPARATOR_TYPE">type of comparator.</param>
		template<typename VALUE_TYPE,
			typename COMPARATOR_TYPE = GaStatsCriterionComparator<VALUE_TYPE> >
		class GaStatsProgressCriterionParams : public GaStatsCriterionParams<VALUE_TYPE, COMPARATOR_TYPE>
		{

		private:

			/// <summary>Number of past generations that are taken into account.</summary>
			int _depth;

		public:

			/// <summary>Initializes parameters.</summary>
			/// <param name="depth">number of past generations that are taken into account.</param>
			/// <param name="valueID">ID of the statistical value that is compared against desired value.</param>
			/// <param name="desiredValue">desired value that should be reached.</param>
			/// <param name="stopType">this parameter defines the way the desired value should be reached.</param>
			/// <param name="comparator">value comparator.</param>
			GaStatsProgressCriterionParams(int depth,
				int valueID,
				const GaValueType& desiredValue,
				int stopType,
				const GaComparatorType& comparator = GaComparatorType()) : GaStatsCriterionParams(valueID, desiredValue, stopType, comparator) { SetDepth( depth ); }

			/// <summary>Initializes parameters with default values. Depth: 1.</summary>
			GaStatsProgressCriterionParams() : _depth(1) { }


			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaStatsProgressCriterionParams<GaValueType, GaComparatorType>( *this ); }

			/// <summary><c>SetDepth</c> method number of past generations that are taken into account.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="depth">generation count.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>depth</c> is negative value.</exception>
			inline void GACALL SetDepth(int depth)
			{
				GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, depth > 0, "depth", "Depth must be positive and greatr then 0.", "Algorithms" );
				_depth = depth;
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of past generations that are taken into account.</returns>
			inline int GACALL GetDepth() const { return _depth; }

		};

		/// <summary><c>GaStatsProgressCriterion</c> class represent stop criterion that makes decision whether that algorithm should continue execution
		/// based on absolute progress of statistical value.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of compared values.</param>
		/// <param name="COMPARATOR_TYPE">type of comparator.</param>
		template<typename VALUE_TYPE,
			typename COMPARATOR_TYPE = GaStatsCriterionComparator<VALUE_TYPE> >
		class GaStatsProgressCriterion : public GaStopCriterion
		{

		public:

			/// <summary>Type of compared values.</summary>
			typedef VALUE_TYPE GaValueType;

			/// <summary>Type of comparator.</summary>
			typedef COMPARATOR_TYPE GaComparatorType;

			/// <summary>More details are given in specification of <see cref="GaStopCriterion::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual bool GACALL operator ()(const Statistics::GaStatistics& stats,
				const GaStopCriterionParams& params,
				const GaStopCriterionConfig& config,
				Common::Workflows::GaBranch* branch) const
			{
				const GaStatsProgressCriterionParams<GaValueType, GaComparatorType>& p = (const GaStatsProgressCriterionParams<GaValueType, GaComparatorType>&)params;

				// enough depth?
				const Statistics::GaValueHistory<GaValueType>& statValue = stats.GetValue<GaValueType>( p.GetValueID() );
				if( statValue.GetCurrentHistoryDepth() < p.GetDepth() )
					return false;

				// check progress
				return IsCriterionReached( statValue.GetProgress( p.GetDepth() ).GetValue(), p.GetDesiredValue(), p.GetComparator(), p.GetStopType() );
			}

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaStatsCriterionParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaStatsCriterionParams<GaValueType, GaComparatorType>(); }

		};

		/// <summary><c>GaStatsRelativeProgressCriterionParams</c> is specialization of <see cref="GaStatsProgressCriterionParams" /> template and is used as parameters
		/// for <see cref="GaStatsRelativeProgressCriterion" /> stop criterion.</summary>
		typedef GaStatsProgressCriterionParams<float> GaStatsRelativeProgressCriterionParams;

		/// <summary><c>GaStatsRelativeProgressCriterion</c> class represent stop criterion that makes decision whether that algorithm should continue execution
		/// based on relative progress of statistical value.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of compared values.</param>
		/// <param name="COMPARATOR_TYPE">type of comparator.</param>
		class GaStatsRelativeProgressCriterion : public GaStopCriterion
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaStopCriterion::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual bool GACALL operator ()(const Statistics::GaStatistics& stats,
				const GaStopCriterionParams& params,
				const GaStopCriterionConfig& config,
				Common::Workflows::GaBranch* branch) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaStatsRelativeProgressCriterionParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaStatsRelativeProgressCriterionParams(); }

		};

		/// <summary><c>GaStatsChangesCriterionParams</c> class is base class for parameters of stop criteria that are based on how long the statistical value has changed.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaStatsChangesCriterionParams : public GaStopCriterionParams
		{

		private:

			/// <summary>ID of the statistical value that is compared against desired value.</summary>
			int _valueID;

			/// <summary>Number of past generations that are taken into account.</summary>
			int _generationCount;

		public:

			/// <summary>Initializes parameters.</summary>
			/// <param name="valueID">ID of statistical value that is used for comparison</param>
			/// <param name="generationCount">number of past generations that are taken into account.</param>
			GaStatsChangesCriterionParams(int valueID,
				int generationCount) : _valueID(valueID),
				_generationCount(generationCount) { }

			/// <summary>Initializes parameters with default values. Value ID is 0 and generation count is 1.</summary>
			GaStatsChangesCriterionParams() :_valueID(0),
				_generationCount(1) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaStatsChangesCriterionParams( *this ); }

			/// <summary><c>SetValueID</c> method sets the ID of statistical value that is used for comparison.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="id">ID of the new value.</param>
			inline void GACALL SetValueID(int id) { _valueID = id; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns value ID of the statistical value that is used for comparison.</returns>
			inline int GACALL GetValueID() const { return _valueID; }

			/// <summary><c>SetGenerationCount</c> method sets number of past generations that are taken into account.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="count">generation count.</param>
			inline void GACALL SetGenerationCount(int count) { _generationCount = count; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of past generations that are taken into account.</returns>
			inline int GACALL GetGenerationCount() const { return _generationCount; }

		};

		/// <summary><c>GaStatsChangesCriterion</c> class represent stop criterion that makes decision whether that algorithm should continue execution
		/// based on how long the statistical value has changed.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaStatsChangesCriterion : public GaStopCriterion
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaStopCriterion::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual bool GACALL operator ()(const Statistics::GaStatistics& stats,
				const GaStopCriterionParams& params,
				const GaStopCriterionConfig& config,
				Common::Workflows::GaBranch* branch) const
			{
				const GaStatsChangesCriterionParams& p = (const GaStatsChangesCriterionParams&)params;
				return stats.GetValue( p.GetValueID() ).GetLastChange() >= p.GetGenerationCount();
			}

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaStatsChangesCriterionParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaStatsChangesCriterionParams(); }

		};

	} // StopCriteria
} // Algorithm

#endif // __GA_STOP_CRITERIA_H__

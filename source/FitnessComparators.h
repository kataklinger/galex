
/*! \file FitnessComparators.h
    \brief This file contains declarations of classes that represent fitness comparators.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_FITNESS_COMPARATORS_H__
#define __GA_FITNESS_COMPARATORS_H__

#include "Fitness.h"

namespace Fitness
{

	/// <summary>Contains built-in fitness comparators.</summary>
	namespace Comparators
	{

		/// <summary>Type of fitness values comparison.</summary>
		enum GaComparisonType
		{

			/// <summary>Instructs comparator that minimization of fitness values is required.</summary>
			GACT_MINIMIZE_ALL = -1,

			/// <summary>Instructs comparator that maximization of fitness values is required.</summary>
			GACT_MAXIMIZE_ALL = 1

		};

		/// <summary><c>GaSimpleComparatorParams</c> class represent parameters for fitness comparators that requires only comarions type.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaSimpleComparatorParams : public GaFitnessComparatorParams
		{

		protected:

			/// <summary>Comparion type that is performed by the comparator</summary>
			GaComparisonType _type;

		public:

			/// <summary>This constructor initializes parameters with comaprion type that will be performed by fitness comparator.</summary>
			/// <param name="type">comparison type.</param>
			GaSimpleComparatorParams(GaComparisonType type) : _type(type) { }

			/// <summary>This constructor initializes parameters to instruct fitness comparator that maximization of fitness values is required.</summary>
			GaSimpleComparatorParams() : _type(GACT_MAXIMIZE_ALL) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaSimpleComparatorParams( *this ); }

			/// <summary><c>SetType</c> method sets comparion type that will be performed by the comparator.</summary>
			/// <param name="type">comparison type.</param>
			inline void GACALL SetType(GaComparisonType type) { _type = type; }

			/// <summary>This method is not thrad-safe.</summary>
			/// <returns>Method returns comparison type perofmed .</returns>
			inline GaComparisonType GACALL GetType() const { return _type; }

		};

		/// <summary><c>GaSimpleComparator</c> class implements comparator for fitness object with only one value.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaSimpleComparator : public GaFitnessComparator
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaFitnessComparator::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual int GACALL operator ()(const GaFitness& fitness1,
				const GaFitness& fitness2,
				const GaFitnessComparatorParams& parameters) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaSimpleComparatorParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaSimpleComparatorParams(); }

		};

		/// <summary><c>GaPositionalComparator</c> class implements comparator that use position of values in the fitness objects as weight while comparing.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaPositionalComparator : public GaFitnessComparator
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaFitnessComparator::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual int GACALL operator ()(const GaFitness& fitness1,
				const GaFitness& fitness2,
				const GaFitnessComparatorParams& parameters) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaSimpleComparatorParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaSimpleComparatorParams(); }

		};

		/// <summary><c>GaDominanceComparator</c> class implements pareto dominace comparison for fitness object with multiple values.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaDominanceComparator : public GaFitnessComparator
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaFitnessComparator::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual int GACALL operator ()(const GaFitness& fitness1,
				const GaFitness& fitness2,
				const GaFitnessComparatorParams& parameters) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaSimpleComparatorParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaSimpleComparatorParams(); }

		};

		/// <summary><c>GaVegaComparatorParams</c> class represents parameters for VEGA fitness comparator.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaVegaComparatorParams : public GaSimpleComparatorParams
		{

		protected:

			/// <summary>Index of fitness value used for comparison.</summary>
			int _vectorIndex;

		public:


			/// <summary>This constiructor initializes paramenters with index of fitness value used for comparion and type of comparion perofmed by fitness comparator.</summary>
			/// <param name="vectorIndex">index of fitness value used for comparison.</param>
			/// <param name="type">comparison type.</param>
			GaVegaComparatorParams(int vectorIndex,
				GaComparisonType type) : GaSimpleComparatorParams(type) { }

			/// <summary>This constiructor initializes paramenters with default values. The first fitness value of fitness object is used and maximization of values is assumed.</summary>
			GaVegaComparatorParams() : _vectorIndex(0) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaVegaComparatorParams( *this ); }

			/// <summary><c>NextVectorIndex</c> method moves index of used value for comparison to next value in fitness object.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="limit">number of values in fitness object.</param>
			inline void GACALL NextVectorIndex(int limit) { _vectorIndex = ( _vectorIndex + 1 ) % limit; }

			/// <summary><c>SetVectorIndex</c> method sets index of fitness value used for comparison.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="index">value index.</param>
			inline void GACALL SetVectorIndex(int index) { _vectorIndex = index; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns index of fitness value used for comparison.</returns>
			inline int GACALL GetVectorIndex() const { return _vectorIndex; }

		};

		/// <summary><c>GaVegaComparator</c> class implements comparator that comapres only a single pair of values at specified index in fitness objects.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaVegaComparator : public GaFitnessComparator
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaFitnessComparator::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual int GACALL operator ()(const GaFitness& fitness1,
				const GaFitness& fitness2,
				const GaFitnessComparatorParams& parameters) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaVegaComparatorParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaVegaComparatorParams(); }

		};

	} // Comparators
} // Fitness

#endif // __GA_FITNESS_COMPARATORS_H__

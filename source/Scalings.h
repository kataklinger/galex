
/*! \file Scalings.h
    \brief This file declares classes that represent scaling operations.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#ifndef __GA_SCALINGS_H__
#define __GA_SCALINGS_H__

#include "PopulationOperations.h"
#include "FitnessValues.h"
#include "FitnessComparators.h"

namespace Population
{
	/// <summary>Contains built-in scaling operations.</summary>
	namespace ScalingOperations
	{

		/// <summary><c>GaNoScaling</c> class repesents scaling operation which just sets scaled fitness to probability base of raw chromosome fitness.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaNoScaling : public GaScalingOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaPopulation& population,
				const GaScalingParams& parameters,
				const GaScalingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method always returns <c>NULL</c>.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

			/// <summary>More details are given in specification of <see cref="GaFitnessOperation::CreateFitnessObject" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaSVFitness&lt;float&gt;" /> class.</returns>
			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const
				{ return new Fitness::Representation::GaSVFitness<float>( params ); }

		};

		/// <summary><c>GaWindowScaling</c> class repesents scaling operation that calculates scaled fitness value of chromosome by subtracting raw fitness value 
		/// of worst chromosome from raw fitness value of chromosome which is scaled. This operation can work with minimization or maximization of fitness values, 
		/// as well as negative fitness values.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaWindowScaling : public GaScalingOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaPopulation& population,
				const GaScalingParams& parameters,
				const GaScalingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method always returns <c>NULL</c>.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

			/// <summary>More details are given in specification of <see cref="GaFitnessOperation::CreateFitnessObject" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaSVFitness&lt;float&gt;" /> class.</returns>
			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const
				{ return new Fitness::Representation::GaSVFitness<float>( params ); }

		};

		/// <summary><c>GaRankingScaling</c> class repesents scaling operation calculates scaled fitness based on ranking of chromosome.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaRankingScaling : public GaScalingOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaPopulation& population,
				const GaScalingParams& parameters,
				const GaScalingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method always returns <c>NULL</c>.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

			/// <summary>More details are given in specification of <see cref="GaFitnessOperation::CreateFitnessObject" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaSVFitness&lt;float&gt;" /> class.</returns>
			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const
				{ return new Fitness::Representation::GaSVFitness<float>( params ); }

		};

		/// <summary><c>GaScalingFactorParams</c> class represents parametners for scaling operations which use user defined factor of scaling.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaScalingFactorParams : public GaScalingParams
		{

		protected:

			/// <summary>Scaling factor.</summary>
			float _factor;

		public:

			/// <summary>This constructor initializes parameters with user defined scale factor.</summary>
			/// <param name="factor">scaling factor.</param>
			GaScalingFactorParams(float factor) : _factor(factor) { }

			/// <summary>This constructor initializes parameters with default values. Default scaling factor is 1.</summary>
			GaScalingFactorParams() : _factor(1) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaScalingFactorParams( *this ); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns scaling factor.</returns>
			inline void GACALL SetFactor(float factor) { _factor = factor; }

			/// <summary><c>SetFactor</c> method sets scaling factor. 
			///
			/// This method is not thread-safe.</summary>
			/// <param name="factor">scaling factor.</param>
			inline float GACALL GetFactor() const { return _factor; }

		};

		/// <summary><c>GaExponentialScaling</c> class repesents scaling operation that calculates scaled fitness value of chromosome by raising raw fitness value to specified power
		/// which is defined by scale factor in parameters.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaExponentialScaling : public GaScalingOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaPopulation& population,
				const GaScalingParams& parameters,
				const GaScalingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaScalingFactorParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaScalingFactorParams(); }

			/// <summary>More details are given in specification of <see cref="GaFitnessOperation::CreateFitnessObject" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaSVFitness&lt;float&gt;" /> class.</returns>
			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const
				{ return new Fitness::Representation::GaSVFitness<float>( params ); }

		};

		/// <summary><c>GaLinearScaling</c> class repesents scaling operation that calculates scaled fitness value of chromosome by applying linear function <c>a * raw_fitness + b</c>,
		/// where <c>a</c> and <c>b</c> are calculated from scale factor which is provided in operation parameters.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaLinearScaling : public GaScalingOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaPopulation& population,
				const GaScalingParams& parameters,
				const GaScalingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaScalingFactorParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaScalingFactorParams(); }

			/// <summary>More details are given in specification of <see cref="GaFitnessOperation::CreateFitnessObject" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaSVFitness&lt;float&gt;" /> class.</returns>
			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const
				{ return new Fitness::Representation::GaSVFitness<float>( params ); }

		};

		/// <summary><c>GaSigmaTruncationScaling</c> class repesents scaling operation that calculates scaled fitness based on variation of chromosome's raw fitness
		/// variation from the average raw fitness value of chromosomes in the population using function <c>raw_fitness - (raw_fitness_avg - factor * raw_deviation)</c>.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaSigmaTruncationScaling : public GaScalingOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaPopulation& population,
				const GaScalingParams& parameters,
				const GaScalingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaScalingFactorParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaScalingFactorParams(); }

			/// <summary>More details are given in specification of <see cref="GaFitnessOperation::CreateFitnessObject" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaSVFitness&lt;float&gt;" /> class.</returns>
			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const
				{ return new Fitness::Representation::GaSVFitness<float>( params ); }

		};

	} // ScalingOperations
} // Population

#endif // __GA_SCALINGS_H__

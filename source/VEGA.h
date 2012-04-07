
/*! \file VEGA.h
    \brief This file conatins declaration of classes that implement Vector Evaluated Genetic Algorithm (VEGA).
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __VEGA_H__
#define __VEGA_H__

#include "Population.h"
#include "FitnessValues.h"
#include "FitnessComparators.h"

namespace Multiobjective
{

	/// <summary>Contains classes and datatypes that implement Vector Evaluated Genetic Algorithm.</summary>
	namespace VEGA
	{

		/// <summary><c>GaVEGA</c> class repesents scaling operation that implements Vector Evaluated Genetic Algorithm.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaVEGA : public Population::GaScalingOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(Population::GaPopulation& population,
				const Population::GaScalingParams& parameters,
				const Population::GaScalingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaSelectionParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

			/// <summary>More details are given in specification of <see cref="GaFitnessOperation::CreateFitnessObject" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaSVFitness&lt;float&gt;" /> class.</returns>
			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const
				{ return new Fitness::Representation::GaSVFitness<float>( params ); }

		};

	} // VEGA
} // Multiobjective

#endif // __VEGA_H__

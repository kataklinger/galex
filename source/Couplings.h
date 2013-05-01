
/*! \file Couplings.h
    \brief This file declares classes that represent coupling operations.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_COUPLINGS_H__
#define __GA_COUPLINGS_H__

#include "Population.h"

namespace Population
{
	/// <summary>Contains built-in copuling operations.</summary>
	namespace CouplingOperations
	{

		/// <summary><c>GaSimpleCoupling</c> class represent coupling operation that chooses the first and the second chromosome in result set as pair for mating,
		/// than the third and the forth and so on.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaSimpleCoupling : public GaCouplingOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaCouplingOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaChromosomeGroup& parents,
				GaChromosomeGroup& output,
				const GaCouplingParams& parameters,
				const GaCouplingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaCouplingParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaCouplingParams(); }

		};

		/// <summary><c>GaInverseCoupling</c> class represent coupling operation that chooses the best and the worst chromosomes in selection result set as pair for mating,
		/// then selects the second best and the second worst and so on.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaInverseCoupling : public GaCouplingOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaCouplingOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaChromosomeGroup& parents,
				GaChromosomeGroup& output,
				const GaCouplingParams& parameters,
				const GaCouplingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaCouplingParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaCouplingParams(); }

		};

		/// <summary><c>GaRandomCoupling</c> class represent coupling operation that chooses chromosomes pairs from selection result set randomly for mating.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaRandomCoupling : public GaCouplingOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaCouplingOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaChromosomeGroup& parents,
				GaChromosomeGroup& output,
				const GaCouplingParams& parameters,
				const GaCouplingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaCouplingParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaCouplingParams(); }

		};

	} // CouplingOperations
} // Population

#endif // __GA_COUPLINGS_H__


/*! \file Crossovers.h
    \brief This file declares classes that represent crossover operations.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#ifndef __GA_CROSSOVERS_H__
#define __GA_CROSSOVERS_H__

#include "ChromosomeOperations.h"

namespace Chromosome
{

	/// <summary>Contains built-in crossover operations.</summary>
	namespace CrossoverOperations
	{

		/// <summary><c>GaArrayMultipointCrossover</c> class represent crossover operation for chromosomes that are repesented by arrays.
		/// This operation supports multipoint crossover.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaArrayMultipointCrossover : public GaCrossoverOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaCrossoverOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaCrossoverBuffer& crossoverBuffer,
				const GaCrossoverParams& parameters) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaCrossoverPointParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaCrossoverPointParams(); }

			/// <summary>More details are given in specification of <see cref="GaCrossoverOperation::GetParentCount" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method always returns <c>2</c>.</returns>
			virtual int GACALL GetParentCount(const GaCrossoverParams& parameters) const { return 2; }

			/// <summary>More details are given in specification of <see cref="GaCrossoverOperation::GetOffspringCount" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns number of offspring chromosomes set by the operation parameters.</returns>
			virtual int GACALL GetOffspringCount(const GaCrossoverParams& parameters) const { return parameters.GetNumberOfOffspring(); }

		};

		/// <summary><c>GaListMultipointCrossover</c> class represent crossover operation for chromosomes that are repesented by linked lists.
		/// This operation supports multipoint crossover.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaListMultipointCrossover : public GaCrossoverOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaCrossoverOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaCrossoverBuffer& crossoverBuffer,
				const GaCrossoverParams& parameters) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaCrossoverPointParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaCrossoverPointParams(); }

			/// <summary>More details are given in specification of <see cref="GaCrossoverOperation::GetParentCount" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method always returns <c>2</c>.</returns>
			virtual int GACALL GetParentCount(const GaCrossoverParams& parameters) const { return 2; }

			/// <summary>More details are given in specification of <see cref="GaCrossoverOperation::GetOffspringCount" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns number of offspring chromosomes set by the operation parameters.</returns>
			virtual int GACALL GetOffspringCount(const GaCrossoverParams& parameters) const { return parameters.GetNumberOfOffspring(); }

		};

		/// <summary><c>GaTreeSinglepointCrossover</c> class represent crossover operation for chromosomes that are repesented by trees.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaTreeSinglepointCrossover : public GaCrossoverOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaCrossoverOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaCrossoverBuffer& crossoverBuffer,
				const GaCrossoverParams& parameters) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaCrossoverPointParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaCrossoverParams(); }

			/// <summary>More details are given in specification of <see cref="GaCrossoverOperation::GetParentCount" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method always returns <c>2</c>.</returns>
			virtual int GACALL GetParentCount(const GaCrossoverParams& parameters) const { return 2; }

			/// <summary>More details are given in specification of <see cref="GaCrossoverOperation::GetOffspringCount" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns number of offspring chromosomes set by the operation parameters.</returns>
			virtual int GACALL GetOffspringCount(const GaCrossoverParams& parameters) const { return parameters.GetNumberOfOffspring(); }

		};

	} // CrossoverOperations
} // Chromosome

#endif // __GA_CROSSOVERS_H__


/*! \file Matings.h
\brief This file declares classes that represent mating operations.
*/

/*
* 
* website: http://www.coolsoft-sd.com/
* contact: support@coolsoft-sd.com
*
*/

#ifndef __GA_MATINGS_H__
#define __GA_MATINGS_H__

#include "ChromosomeOperations.h"

namespace Chromosome
{

	/// <summary>Contains built-in mating operations.</summary>
	namespace MatingOperations
	{

		/// <summary><c>GaBasicMatingOperation</c> class represent chromosome mating that implements basic genetic operations - crossover and mutation.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaBasicMatingOperation : public GaMatingOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaMatingOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(const GaMatingConfig& operations,
				GaCrossoverBuffer& crossoverBuffer,
				const GaMatingParams& parameters) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns always returns NULL.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

		protected:

			/// <summary><c>PerformCrossover</c> method execute crossover operation and produces offspring chromosomes.
			///
			/// This method is thread-safe.</summary>
			/// <param name="operations">reference to object that stores setup of mating operation.</param>
			/// <param name="crossoverBuffer">reference to crossover buffer that stores parent chromosome and which will store offspring.</param>
			/// <returns>Method returns <c>true</c> if the crossover operation was performed. It this method returns <c>false</c> it indicates
			/// that offspring chromosomes are just references to their parents.</returns>
			GAL_API
			bool GACALL PerformCrossover(const GaMatingConfig& operations,
				GaCrossoverBuffer& crossoverBuffer) const;

			/// <summary><c>PerformMutation</c> method tries to execute mutation operation on all offspring chromosomes stored in crossover buffer.
			///
			/// This method is thread-safe.</summary>
			/// <param name="operations">reference to object that stores setup of mating operation.</param>
			/// <param name="crossoverBuffer">crossover buffer that stores offspring chromosomes.</param>
			/// <param name="crossoverPerformed">when this parameter is set to <c>true</c> it indicates whether the corssover operation is performed,
			/// otherwise offspring chromosomes are treated as references to the parent chromosomes.</param>
			GAL_API
			void GACALL PerformMutation(const GaMatingConfig& operations,
				GaCrossoverBuffer& crossoverBuffer,
				bool crossoverPerformed) const;

		};

		//class GaAdvanceMatingOperation : public GaBasicMatingOperation
		//{

		//public:

		//	GAL_API
		//	virtual void GACALL operator ()(const GaMatingConfig& operations,
		//		GaCrossoverBuffer& crossoverBuffer,
		//		const GaMatingParams& parameters) const;

		//};

	} // MatingOperations
} // Chromosome

#endif

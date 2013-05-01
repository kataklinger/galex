
/*! \file PAESStub.h
    \brief This file conatins declaration of classes that implement algorithm stub for Pareto Archived Evolution Strategy (PAES).
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_PAES_STUB_H__
#define __GA_PAES_STUB_H__

#include "AlgorithmStub.h"

#include "PopulationStatistics.h"
#include "PAES.h"

namespace Algorithm
{
	namespace Stubs
	{

		/// <summary><c>GaPAESStub</c> class represents stub that implement PAES.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaPAESStub : public GaBasicStub
		{

		protected:

			/// <summary>Flow steps which checks whether initialization of the population is required.</summary>
			Common::Workflows::GaFlowStep* _checkStep;

			/// <summary>Flow step that initializes population.</summary>
			Common::Workflows::GaFlowStep* _initStep;

			/// <summary>Dummy crossover operation that is used by algorithm.</summary>
			Multiobjective::PAES::GaPAESCrossover _crossover;

			/// <summary>Mutation operation that is used by algorithm to produce new chromosome.</summary>
			Chromosome::GaMutationSetup _mutation;

			/// <summary>Mating operation that is used by algorithm to produce new chromosome.</summary>
			Multiobjective::PAES::GaPAESMating _matingOperation;

			/// <summary>Parameters of selection operation.</summary>
			Multiobjective::PAES::GaPAESSelectionParams _selectionParameters;

			/// <summary>Selection operation used by the algorithm.</summary>
			Multiobjective::PAES::GaPAESSelection _selectionOperation;

			/// <summary>Flow step that performs selection operation.</summary>
			Population::GaSelectionStep* _selectionStep;

			/// <summary>PAES parameters.</summary>
			Multiobjective::PAES::GaPAESParams _paesParameters;

			/// <summary>Grid that splits raw fitness hyperspace.</summary>
			Common::Memory::GaAutoPtr<Multiobjective::GaFitnessHyperGrid> _grid;

			/// <summary>PAES operation.</summary>
			Multiobjective::PAES::GaPAES _paesOperation;

			/// <summary>Flow step that performs replacement operation.</summary>
			Population::GaReplacementStep* _replacementStep;

			/// <summary>Flow step that perpares population for the next generation.</summary>
			Common::Workflows::GaFlowStep* _nextGenStep;

		public:

			/// <summary>Initializes PAES stub with population on which it will operate and all operations and parameters.</summary>
			/// <param name="populationID">ID of the population on which the stub will operate.</param>
			/// <param name="statisticsID">ID of the object that whill store statistical information.</param>
			/// <param name="initializator">initialization operation for the population.</param>
			/// <param name="fitnessOperation">fitness operation that evaluates chromosomes in the population.</param>
			/// <param name="comparator">fitness comparator used by the population.</param>
			/// <param name="populationParameters">parameters of the population on which the stub operates.</param>
			/// <param name="mutation">mutation operation that produce new chromosome.</param>
			/// <param name="selectionParameters">parameters of the selection operation.</param>
			/// <param name="paesParameters">parameters for PAES operation.</param>
			/// <param name="grid">grid that splits raw fitness hyperspace.</param>
			GAL_API
			GaPAESStub(int populationID,
				int statisticsID,
				const Chromosome::GaInitializatorSetup& initializator,
				const Population::GaPopulationFitnessOperationSetup& fitnessOperation,
				const Fitness::GaFitnessComparatorSetup& comparator,
				const Population::GaPopulationParams& populationParameters,
				const Chromosome::GaMutationSetup& mutation,
				const Multiobjective::PAES::GaPAESSelectionParams& selectionParameters,
				const Multiobjective::PAES::GaPAESParams& paesParameters,
				const Multiobjective::GaFitnessHyperGrid& grid);

			/// <summary>Initializes PAES stub without population on which it will operate and with default operations and parameters.</summary>
			GaPAESStub() { Clear(); }

			/// <summary><c>SetPopulationParameters</c> method new parameters for the population.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="params">new population parameters.</param>
			GAL_API
			void GACALL SetPopulationParameters(Population::GaPopulationParams params);

			/// <summary><c>SetInitializator</c> method sets new population initialization operation.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="initializator">new initialization operation.</param>
			inline void GACALL SetInitializator(const Chromosome::GaInitializatorSetup& initializator) { UpdateInitializator( initializator ); }

			/// <summary><c>SetFitnessOperation</c> method sets new fitness operation for evaluation population's chromosomes.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="fitnessOperation">new fitness operation.</param>
			inline void GACALL SetFitnessOperation(const Population::GaPopulationFitnessOperationSetup& fitnessOperation) { UpdateFitnessOperation( fitnessOperation ); }

			/// <summary><c>SetFitnessComparator</c> method sets new fitness comparator used by the population.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="comparator">new fitness comparator.</param>
			inline void GACALL SetFitnessComparator(const Fitness::GaFitnessComparatorSetup& comparator) { UpdateFitnessComparator( comparator ); }

			/// <summary><c>SetMutation</c> method sets mutation operation that will be used for producing new chromosome.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="mutation">new mutation setup.</param>
			GAL_API
			void GACALL SetMutation(const Chromosome::GaMutationSetup& mutation);

			/// <summary><c>SetSelectionParameters</c> method sets parameter that is used by selection operation.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="params">new selection parameters.</param>
			GAL_API
			void GACALL SetSelectionParameters(Multiobjective::PAES::GaPAESSelectionParams params);

			/// <summary><c>SetPAESParameters</c> method sets parameters for PESA operation.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="params">new PAES parameters.</param>
			GAL_API
			void GACALL SetPAESParameters(const Multiobjective::PAES::GaPAESParams& params);

			/// <summary><c>SetGrid</c> method sets grid that will split raw fitness hyperspace.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="grid">new grid.</param>
			GAL_API
			void GACALL SetGrid(const Multiobjective::GaFitnessHyperGrid& grid);

		protected:

			/// <summary>Prepares workflow for the execution.</summary>
			GAL_API
			virtual void GACALL Connected();

			/// <summary>Cleans workflow.</summary>
			GAL_API
			virtual void GACALL Disconnecting();

			/// <summary>Clears internal bookkeeping of created data objects and flow steps.</summary>
			GAL_API
			void GACALL Clear();

		};

	} // Stubs
} // Algorithm

#endif // __GA_PAES_STUB_H__


/*! \file RDGAStub.h
    \brief This file conatins declaration of classes that implement algorithm stubs for Rank-Desity based Genetic Algorithm (RDGA).
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_PESA_STUB_H__
#define __GA_PESA_STUB_H__

#include "AlgorithmStub.h"

#include "PopulationStatistics.h"
#include "Selections.h"
#include "CellularCoupling.h"
#include "Replacements.h"
#include "FitnessComparators.h"
#include "RDGA.h"

namespace Algorithm
{
	namespace Stubs
	{

		/// <summary><c>GaRDGAStub</c> class represents stub that implement RDGA.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaRDGAStub : public GaBasicStub
		{

		protected:

			/// <summary>Tracker of population size.</summary>
			Population::GaPopulationSizeTracker _sizeTracker;

			/// <summary>Tracker of statistical information about scaled fitness values.</summary>
			Population::GaScaledFitnessTracker _scaledTracker;

			/// <summary>Flow steps which checks whether initialization of the population is required.</summary>
			Common::Workflows::GaFlowStep* _checkStep;

			/// <summary>Flow step that initializes population.</summary>
			Common::Workflows::GaFlowStep* _initStep;

			/// <summary>Fitness comparator used for sorting chromosomes in the population.</summary>
			Fitness::Comparators::GaSimpleComparator _scaledFitnessComparator;

			/// <summary>Parameters of selection operation.</summary>
			Population::SelectionOperations::GaDuplicatesSelectionParams _selectionParameters;

			/// <summary>Selection operation used by the algorithm.</summary>
			Population::SelectionOperations::GaRandomSelection _selectionOperation;

			/// <summary>Flow step that performs selection operation.</summary>
			Population::GaSelectionStep* _selectionStep;

			/// <summary>Mating operation that is used by algorithm to produce offspring.</summary>
			Chromosome::GaMatingSetup _mating;

			/// <summary>Parameters for the coupling operation.</summary>
			Population::CouplingOperations::GaCellularCouplingParams _couplingParameters;

			/// <summary>Coupling operation use by the algorithm</summary>
			Population::CouplingOperations::GaCellularCoupling _couplingOperation;

			/// <summary>Flow step that performs coupling operation.</summary>
			Population::GaCouplingStep* _couplingStep;

			/// <summary>Flow step that replaces population re-evaluation step if it is not used.</summary>
			Common::Workflows::GaNopStep* _nopStep;

			/// <summary>Flow connection between replacement operation and fitness operation. Indicates whether the fitness operation that eveluates whole population is used.</summary>
			Common::Workflows::GaFlowConnection* _fitnessConnection;

			/// <summary>Flow step evaluates whole population and assigns raw fitness to chromosomes in the population.</summary>
			Population::GaPopulationFitnessStep* _fitnessStep;

			/// <summary>RDGA parameters.</summary>
			Multiobjective::RDGA::GaRDGAParams _rdgaParameters;

			/// <summary>Adaptive grid that splits raw fitness hyperspace.</summary>
			Common::Memory::GaAutoPtr<Multiobjective::GaFitnessAdaptiveGrid> _grid;

			/// <summary>RDGA operation.</summary>
			Multiobjective::RDGA::GaRDGA _rdgaOperation;

			/// <summary>Flow step that performs replacement operation.</summary>
			Population::GaReplacementStep* _replacementStep;

			/// <summary>Flow step that perpares population for the next generation.</summary>
			Common::Workflows::GaFlowStep* _nextGenStep;

		public:

			/// <summary>Initializes RDGA stub with population on which it will operate and all operations and parameters.</summary>
			/// <param name="populationID">ID of the population on which the stub will operate.</param>
			/// <param name="statisticsID">ID of the object that whill store statistical information.</param>
			/// <param name="initializator">initialization operation for the population.</param>
			/// <param name="fitnessOperation">fitness operation that evaluates chromosomes in the population.</param>
			/// <param name="comparator">fitness comparator used by the population.</param>
			/// <param name="populationParameters">parameters of the population on which the stub operates.</param>
			/// <param name="selectionParameters">parameters of the selection operation.</param>
			/// <param name="mating">mating operation that produce offspring chromosomes.</param>
			/// <param name="couplingParameters">parameters for coupling operation.</param>
			/// <param name="rdgaParameters">parameters for RDGA operation.</param>
			/// <param name="grid">adaptive grid that splits raw fitness hyperspace.</param>
			GAL_API
			GaRDGAStub(int populationID,
				int statisticsID,
				const Chromosome::GaInitializatorSetup& initializator,
				const Population::GaPopulationFitnessOperationSetup& fitnessOperation,
				const Fitness::GaFitnessComparatorSetup& comparator,
				const Population::GaPopulationParams& populationParameters,
				const Population::SelectionOperations::GaDuplicatesSelectionParams& selectionParameters,
				const Chromosome::GaMatingSetup& mating,
				const Population::CouplingOperations::GaCellularCouplingParams& couplingParameters,
				const Multiobjective::RDGA::GaRDGAParams& rdgaParameters,
				const Multiobjective::GaFitnessAdaptiveGrid& grid);

			/// <summary>Initializes RDGA stub without population on which it will operate and with default operations and parameters.</summary>
			GaRDGAStub() { Clear(); }

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
			GAL_API
			void GACALL SetFitnessOperation(const Population::GaPopulationFitnessOperationSetup& fitnessOperation);

			/// <summary><c>SetFitnessComparator</c> method sets new fitness comparator used by the population.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="comparator">new fitness comparator.</param>
			inline void GACALL SetFitnessComparator(const Fitness::GaFitnessComparatorSetup& comparator) { UpdateFitnessComparator( comparator ); }

			/// <summary><c>SetMating</c> method sets new mating operation used by the algorithm.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="mating">new mating operation.</param>
			GAL_API
			void GACALL SetMating(const Chromosome::GaMatingSetup& mating);

			/// <summary><c>SetSelectionParameters</c> method sets parameter that is used by selection operation.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="params">new selection parameters.</param>
			GAL_API
			void GACALL SetSelectionParameters(Population::SelectionOperations::GaDuplicatesSelectionParams params);

			/// <summary><c>SetCouplingParameters</c> method sets parameters for coupling operation.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="params">new coupling operation parameters.</param>
			GAL_API
			void GACALL SetCouplingParameters(const Population::CouplingOperations::GaCellularCouplingParams& params);

			/// <summary><c>SetRDGAParameters</c> method sets parameters for RDGA operation.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="params">new RDGA parameters.</param>
			GAL_API
			void GACALL SetRDGAParameters(const Multiobjective::RDGA::GaRDGAParams& params);

			/// <summary><c>SetGrid</c> method sets adaptive grid that will split raw fitness hyperspace.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="grid">new adaptive grid.</param>
			GAL_API
			void GACALL SetGrid(const Multiobjective::GaFitnessAdaptiveGrid& grid);

		protected:

			/// <summary><c>UseFitnessStep</c> method determins whether the step that re-evaluates whole population is used.</summary>
			/// <returns>Method returns <c>true</c> if re-evaluation of the whole population is active.</returns>
			inline bool GACALL UseFitnessStep() { return _fitnessOperation.HasOperation() && !_fitnessOperation.GetOperation().AllowsIndividualEvaluation(); }

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

#endif // __GA_PESA_STUB_H__


#ifndef __GA_PESA_STUB_H__
#define __GA_PESA_STUB_H__

#include "AlgorithmStub.h"

#include "PopulationStatistics.h"
#include "Selections.h"
#include "Replacements.h"
#include "FitnessComparators.h"
#include "PESA.h"

namespace Algorithm
{
	namespace Stubs
	{

		/// <summary><c>GaPESAStub</c> class represents stub that implement PESA.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaPESAStub : public GaBasicStub
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

			/// <summary>Mating operation that is used by algorithm to produce offspring.</summary>
			Chromosome::GaMatingSetup _mating;

			/// <summary>Fitness comparator used for sorting chromosomes in the population.</summary>
			Fitness::Comparators::GaSimpleComparator _scaledFitnessComparator;

			/// <summary>Parameters of selection operation.</summary>
			Population::SelectionOperations::GaDuplicatesSelectionParams _selectionParameters;

			/// <summary>Selection operation used by the algorithm.</summary>
			Population::SelectionOperations::GaTournamentSelection _selectionOperation;

			/// <summary>Replacement operation used by the algorithm.</summary>
			Population::ReplacementOperations::GaCrowdingReplacement _replacementOperation;

			/// <summary>Flow step that performs selection operation.</summary>
			Population::GaSelectionStep* _selectionStep;

			/// <summary>Flow connection between selection operation and coupling operation. Indicates whether the couling operation is used.</summary>
			Common::Workflows::GaFlowConnection* _couplingConnection;

			/// <summary>Setup of the coupling operation used by the algorithm.</summary>
			Population::GaCouplingSetup _couplingSetup;

			/// <summary>Flow step that performs coupling operation.</summary>
			Population::GaCouplingStep* _couplingStep;

			/// <summary>Flow step that performs replacement operation.</summary>
			Population::GaReplacementStep* _replacementStep;

			/// <summary>Flow step that replaces population re-evaluation step if it is not used.</summary>
			Common::Workflows::GaNopStep* _nopStep;

			/// <summary>Flow connection between replacement operation and fitness operation. Indicates whether the fitness operation that eveluates whole population is used.</summary>
			Common::Workflows::GaFlowConnection* _fitnessConnection;

			/// <summary>Flow step evaluates whole population and assigns raw fitness to chromosomes in the population.</summary>
			Population::GaPopulationFitnessStep* _fitnessStep;

			/// <summary>PESA parameters.</summary>
			Multiobjective::PESA::GaPESAParams _pesaParameters;

			/// <summary>Grid that splits raw fitness hyperspace.</summary>
			Common::Memory::GaAutoPtr<Multiobjective::GaFitnessHyperGrid> _grid;

			/// <summary>PESA operation.</summary>
			Multiobjective::PESA::GaPESA _pesaOperation;

			/// <summary>Scaling operation that transform raw multi-value fitness to single value fitness (SPEA or SPEA2 operation).</summary>
			Population::GaScalingStep* _scalingStep;

			/// <summary>Flow that sorts population according to scaled fitness of chromosomes.</summary>
			Population::GaSortPopulationStep<Population::GaFitnessComparatorSortingCriteria>* _sortStep;

			/// <summary>Flow step that perpares population for the next generation.</summary>
			Common::Workflows::GaFlowStep* _nextGenStep;

		public:

			/// <summary>Initializes PESA stub with population on which it will operate and all operations and parameters.</summary>
			/// <param name="populationID">ID of the population on which the stub will operate.</param>
			/// <param name="statisticsID">ID of the object that whill store statistical information.</param>
			/// <param name="initializator">initialization operation for the population.</param>
			/// <param name="fitnessOperation">fitness operation that evaluates chromosomes in the population.</param>
			/// <param name="comparator">fitness comparator used by the population.</param>
			/// <param name="populationParameters">parameters of the population on which the stub operates.</param>
			/// <param name="mating">mating operation that produce offspring chromosomes.</param>
			/// <param name="coupling">coupling operation used by the stub.</param>
			/// <param name="selectionParameters">parameters of the selection operation.</param>
			/// <param name="params">parameters for PESA operation.</param>
			/// <param name="grid">grid that splits raw fitness hyperspace.</param>
			GAL_API
			GaPESAStub(int populationID,
				int statisticsID,
				const Chromosome::GaInitializatorSetup& initializator,
				const Population::GaPopulationFitnessOperationSetup& fitnessOperation,
				const Fitness::GaFitnessComparatorSetup& comparator,
				const Population::GaPopulationParams& populationParameters,
				const Chromosome::GaMatingSetup& mating,
				const Population::GaCouplingSetup& coupling,
				const Population::SelectionOperations::GaTournamentSelectionParams& selectionParameters,
				const Multiobjective::PESA::GaPESAParams& params,
				const Multiobjective::GaFitnessHyperGrid& grid);

			/// <summary>Initializes PESA stub without population on which it will operate and with default operations and parameters.</summary>
			GaPESAStub() { Clear(); }

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
			/// <param name="fitnessOperation">new fitness comparator.</param>
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
			void GACALL SetSelectionParameters(Population::SelectionOperations::GaTournamentSelectionParams params);

			/// <summary><c>SetCoupling</c> method sets new coupling operation that is used by the algorithm.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="coupling">new coupling operation.</param>
			GAL_API
			void GACALL SetCoupling(const Population::GaCouplingSetup& coupling);

			/// <summary><c>SetPESAParameters</c> method sets parameters for PESA operation.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="params">new PESA parameters.</param>
			GAL_API
			void GACALL SetPESAParameters(const Multiobjective::PESA::GaPESAParams& params);

			/// <summary><c>SetGrid</c> method sets grid that will split raw fitness hyperspace.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="grid">new grid.</param>
			GAL_API
			void GACALL SetGrid(const Multiobjective::GaFitnessHyperGrid& grid);

		protected:

			/// <summary><c>UseFitnessStep</c> method determins whether the step that re-evaluates whole population is used.</summary>
			/// <returns>Method returns <c>true</c> if re-evaluation of the whole population is active.</returns>
			inline bool GACALL UseFitnessStep() { return _fitnessOperation.HasOperation() && !_fitnessOperation.GetOperation().AllowsIndividualEvaluation(); }

			/// <summary><c>IsCouplingUsed</c> method determins whether the coupling operation is used.</summary>
			/// <returns>Method returns <c>true</c> if the coupling step is used.</returns>
			inline bool GACALL IsCouplingUsed() const { return _selectionParameters.GetCrossoverBuffersTagID() < 0; }

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

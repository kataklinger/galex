
/*! \file SimpleStub.h
    \brief This file conatins declaration of classes that implement algorithm stubs for simple GA with overlapping population.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_SIMPLE_STUB_H__
#define __GA_SIMPLE_STUB_H__

#include "AlgorithmStub.h"

namespace Algorithm
{
	namespace Stubs
	{

		/// <summary><c>GaSimpleGAStub</c> class represents a simple genetic algorithm for single-objective optimization that uses an overlapping population.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaSimpleGAStub : public GaBasicStub
		{

		public:

			/// <summary>Hash map that stores trackers for statistical information that are required by the algorithm.</summary>
			typedef STLEXT::hash_map<int, Population::GaPopulationStatTracker*> GaStatTrackersCollection;

		protected:

			/// <summary>Statistical information trackers that are required by the algorithm.</summary>
			GaStatTrackersCollection _statTrackers;

			/// <summary>Flow steps which checks whether initialization of the population is required.</summary>
			Common::Workflows::GaFlowStep* _checkStep;

			/// <summary>Flow step that initializes population.</summary>
			Common::Workflows::GaFlowStep* _initStep;

			/// <summary>Mating operation that is used by algorithm to produce offspring.</summary>
			Chromosome::GaMatingSetup _mating;

			/// <summary>Setup of the selection operation used by the algorithm.</summary>
			Population::GaSelectionSetup _selection;

			/// <summary>Flow step that performs selection operation.</summary>
			Population::GaSelectionStep* _selectionStep;

			/// <summary>Flow connection between selection operation and coupling operation. Indicates whether the coupling operation is used.</summary>
			Common::Workflows::GaFlowConnection* _couplingConnection;

			/// <summary>Setup of the coupling operation used by the algorithm.</summary>
			Population::GaCouplingSetup _coupling;

			/// <summary>Flow connection between selection operation and coupling operation. Indicates whether the couling operation is used.</summary>
			Population::GaCouplingStep* _couplingStep;

			/// <summary>Setup of the replacement operation used by the algorithm.</summary>
			Population::GaReplacementSetup _replacement;

			/// <summary>Flow step that performs replacement operation.</summary>
			Population::GaReplacementStep* _replacementStep;

			/// <summary>Flow step that replaces population re-evaluation step if it is not used.</summary>
			Common::Workflows::GaNopStep* _nopStep1;

			/// <summary>Flow connection between replacement operation and fitness operation. Indicates whether the fitness operation that eveluates whole population is used.</summary>
			Common::Workflows::GaFlowConnection* _fitnessConnection;

			/// <summary>Flow step evaluates whole population and assigns raw fitness to chromosomes in the population.</summary>
			Population::GaPopulationFitnessStep* _fitnessStep;

			/// <summary>Flow step that replaces population scaling step if it is not used.</summary>
			Common::Workflows::GaNopStep* _nopStep2;

			/// <summary>Flow connection to the scaling operation and it indicates whether the operation is used.</summary>
			Common::Workflows::GaFlowConnection* _scalingConnection;

			/// <summary>Setup of the scaling operation used by the algorithm.</summary>
			Population::GaScalingSetup _scaling;

			/// <summary>Flow step that performs scaling operation.</summary>
			Population::GaScalingStep* _scalingStep;

			/// <summary>Flow step that replaces population sort step if it is not used.</summary>
			Common::Workflows::GaNopStep* _nopStep3;

			/// <summary>Flow connection to the sort step and it indicates whether the population sorting is used.</summary>
			Common::Workflows::GaFlowConnection* _sortConnection;

			/// <summary>Criteria used by sort step for sorting chromosomes in the population.</summary>
			Population::GaFitnessComparatorSortingCriteria _populationSort;

			/// <summary>Flow that sorts population according to scaled fitness of chromosomes.</summary>
			Population::GaSortPopulationStep<Population::GaFitnessComparatorSortingCriteria>* _sortStep;

			/// <summary>Flow step that perpares population for the next generation.</summary>
			Common::Workflows::GaFlowStep* _nextGenStep;

		public:

			/// <summary>Initializes simple algorithm stub with population on which it will operate and all operations and parameters.</summary>
			/// <param name="populationID">ID of the population on which the stub will operate.</param>
			/// <param name="statisticsID">ID of the object that whill store statistical information.</param>
			/// <param name="initializator">initialization operation for the population.</param>
			/// <param name="fitnessOperation">fitness operation that evaluates chromosomes in the population.</param>
			/// <param name="comparator">fitness comparator used by the population.</param>
			/// <param name="populationParameters">parameters of the population on which the stub operates.</param>
			/// <param name="statTrackers">required population statistics trackers.</param>
			/// <param name="mating">mating operation that produce offspring chromosomes.</param>
			/// <param name="selection">selection operation used by the stub.</param>
			/// <param name="coupling">coupling operation used by the stub.</param>
			/// <param name="replacement">replacement operation used by the stub.</param>
			/// <param name="scaling">scaling operation used by the stub.</param>
			/// <param name="populationSort">soritng criteria used for soritng population.</param>
			GAL_API
			GaSimpleGAStub(int populationID,
				int statisticsID,
				const Chromosome::GaInitializatorSetup& initializator,
				const Population::GaPopulationFitnessOperationSetup& fitnessOperation,
				const Fitness::GaFitnessComparatorSetup& comparator,
				const Population::GaPopulationParams& populationParameters,
				const GaStatTrackersCollection& statTrackers,
				const Chromosome::GaMatingSetup& mating,
				const Population::GaSelectionSetup& selection,
				const Population::GaCouplingSetup& coupling,
				const Population::GaReplacementSetup& replacement,
				const Population::GaScalingSetup& scaling,
				const Population::GaFitnessComparatorSortingCriteria& populationSort);

			/// <summary>Initializes simple algorithm stub without population on which it will operate and with default operations and parameters.</summary>
			GaSimpleGAStub() { Clear(); }

			/// <summary><c>SetPopulationParameters</c> method new parameters for the population.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="params">new population parameters.</param>
			inline void GACALL SetPopulationParameters(Population::GaPopulationParams params) { UpdatePopulationParameters( params ); }

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

			/// <summary><c>SetSelection</c> method sets new selection operation that is used by the algorithm.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="selection">new selection operation.</param>
			GAL_API
			void GACALL SetSelection(const Population::GaSelectionSetup& selection);

			/// <summary><c>SetCoupling</c> method sets new coupling operation that is used by the algorithm.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="coupling">new coupling operation.</param>
			GAL_API
			void GACALL SetCoupling(const Population::GaCouplingSetup& coupling);

			/// <summary><c>SetReplacement</c> method sets new replacement operation that is used by the algorithm.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="replacement">new replacement operation.</param>
			GAL_API
			void GACALL SetReplacement(const Population::GaReplacementSetup& replacement);

			/// <summary><c>SetScaling</c> method sets new scaling operation that is used by the algorithm.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="scaling">new scaling operation.</param>
			GAL_API
			void GACALL SetScaling(const Population::GaScalingSetup& scaling);

			/// <summary><c>SetPopulationSort</c> method sets new criteria for sorting population at the end of generation.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="sort">soritng criteria used for soritng population.</param>
			GAL_API
			void GACALL SetPopulationSort(const Population::GaFitnessComparatorSortingCriteria& sort);

			/// <summary><c>SetStatsTrackers</c> method registers required population statistics trackers.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="trackers">trackers that should be registered.</param>
			GAL_API
			void GACALL SetStatsTrackers(const GaStatTrackersCollection& trackers);

		protected:

			/// <summary><c>UseFitnessStep</c> method determins whether the step that re-evaluates whole population is used.</summary>
			/// <returns>Method returns <c>true</c> if re-evaluation of the whole population is active.</returns>
			inline bool GACALL UseFitnessStep() { return _fitnessOperation.HasOperation() && !_fitnessOperation.GetOperation().AllowsIndividualEvaluation(); }

			/// <summary><c>IsCouplingUsed</c> method determins whether the coupling operation is used.</summary>
			/// <returns>Method returns <c>true</c> if the coupling step is used.</returns>
			inline bool GACALL IsCouplingUsed() const { return !_selection.HasParameters() || _selection.GetParameters().GetCrossoverBuffersTagID() < 0; }

			/// <summary><c>IsScalingUsed</c> method determins whether the scaling operation is used.</summary>
			/// <returns>Method returns <c>true</c> if the scaling step is used.</returns>
			inline bool GACALL IsScalingUsed() const { return _scaling.HasOperation(); }

			/// <summary><c>IsSortingUsed</c> method determins whether the sorting operation is used.</summary>
			/// <returns>Method returns <c>true</c> if the sorting step is used.</returns>
			inline bool GACALL IsSortingUsed() const { return _populationSort.GetComparator().HasOperation(); }

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

#endif // __GA_SIMPLE_STUB_H__

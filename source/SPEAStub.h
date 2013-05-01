
/*! \file SPEAStub.h
    \brief This file conatins declaration of classes that implement algorithm stubs for Strength Pareto Evolutionary Algorithm I and II (SPEA-I and SPEA-II).
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_SPEA_STUB_H__
#define __GA_SPEA_STUB_H__

#include "AlgorithmStub.h"

#include "PopulationStatistics.h"
#include "Selections.h"
#include "Replacements.h"
#include "FitnessComparators.h"
#include "SPEA.h"

namespace Algorithm
{
	namespace Stubs
	{

		/// <summary><c>GaSPEAStubBase</c> is base class for stubs that implement SPEA.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaSPEAStubBase : public GaBasicStub
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

			/// <summary>Replacement operation used by the algorithm</summary>
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

			/// <summary>Scaling operation that transform raw multi-value fitness to single value fitness (SPEA or SPEA2 operation).</summary>
			Population::GaScalingStep* _scalingStep;

			/// <summary>Flow that sorts population according to scaled fitness of chromosomes.</summary>
			Population::GaSortPopulationStep<Population::GaFitnessComparatorSortingCriteria>* _sortStep;

			/// <summary>Flow step that perpares population for the next generation.</summary>
			Common::Workflows::GaFlowStep* _nextGenStep;

		public:

			/// <summary>Initializes SPEA stub with population on which it will operate and all operations and parameters.</summary>
			/// <param name="populationID">ID of the population on which the stub will operate.</param>
			/// <param name="statisticsID">ID of the object that whill store statistical information.</param>
			/// <param name="initializator">initialization operation for the population.</param>
			/// <param name="fitnessOperation">fitness operation that evaluates chromosomes in the population.</param>
			/// <param name="comparator">fitness comparator used by the population.</param>
			/// <param name="populationParameters">parameters of the population on which the stub operates.</param>
			/// <param name="mating">mating operation that produce offspring chromosomes.</param>
			/// <param name="coupling">coupling operation used by the stub.</param>
			/// <param name="selectionParameters">parameters of the selection operation.</param>
			GAL_API
			GaSPEAStubBase(int populationID,
				int statisticsID,
				const Chromosome::GaInitializatorSetup& initializator,
				const Population::GaPopulationFitnessOperationSetup& fitnessOperation,
				const Fitness::GaFitnessComparatorSetup& comparator,
				const Population::GaPopulationParams& populationParameters,
				const Chromosome::GaMatingSetup& mating,
				const Population::GaCouplingSetup& coupling,
				const Population::SelectionOperations::GaTournamentSelectionParams& selectionParameters);

			/// <summary>Initializes SPEA stub without population on which it will operate and with default operations and parameters.</summary>
			GaSPEAStubBase() { Clear(); }

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
			inline void GACALL SetSelectionParameters(Population::SelectionOperations::GaTournamentSelectionParams params);

			/// <summary><c>SetCoupling</c> method sets new coupling operation that is used by the algorithm.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="coupling">new coupling operation.</param>
			GAL_API
			void GACALL SetCoupling(const Population::GaCouplingSetup& coupling);

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

		/// <summary><c>GaSPEAStub</c> class represents stub that implement SPEA.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaSPEAStub : public GaSPEAStubBase
		{

		protected:

			/// <summary>SPEA parameters.</summary>
			Multiobjective::SPEA::GaSPEAParams _speaParameters;

			/// <summary>SPEA operation.</summary>
			Multiobjective::SPEA::GaSPEA _speaOperation;

		public:

			/// <summary>Initializes SPEA stub with population on which it will operate and all operations and parameters.</summary>
			/// <param name="populationID">ID of the population on which the stub will operate.</param>
			/// <param name="statisticsID">ID of the object that whill store statistical information.</param>
			/// <param name="initializator">initialization operation for the population.</param>
			/// <param name="fitnessOperation">fitness operation that evaluates chromosomes in the population.</param>
			/// <param name="comparator">fitness comparator used by the population.</param>
			/// <param name="populationParameters">parameters of the population on which the stub operates.</param>
			/// <param name="mating">mating operation that produce offspring chromosomes.</param>
			/// <param name="coupling">coupling operation used by the stub.</param>
			/// <param name="selectionParameters">parameters of the selection operation.</param>
			/// <param name="speaParameters">SPEA parameters.</param>
			GaSPEAStub(int populationID,
				int statisticsID,
				const Chromosome::GaInitializatorSetup& initializator,
				const Population::GaPopulationFitnessOperationSetup& fitnessOperation,
				const Fitness::GaFitnessComparatorSetup& comparator,
				const Population::GaPopulationParams& populationParameters,
				const Chromosome::GaMatingSetup& mating,
				const Population::GaCouplingSetup& coupling,
				const Population::SelectionOperations::GaTournamentSelectionParams& selectionParameters,
				const Multiobjective::SPEA::GaSPEAParams& speaParameters) : 
				GaSPEAStubBase(populationID, statisticsID, initializator, fitnessOperation, comparator, populationParameters, mating, coupling, selectionParameters)
					{ SetSPEAParameters( speaParameters ); }

			/// <summary>Initializes SPEA stub without population on which it will operate and with default operations and parameters.</summary>
			GaSPEAStub() { }

			/// <summary><c>SetSPEAParameters</c> method sets parameters for SPEA operation.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="params">new SPEA parameters.</param>
			GAL_API
			void GACALL SetSPEAParameters(const Multiobjective::SPEA::GaSPEAParams& params);

		protected:

			/// <summary>Prepares workflow for the execution.</summary>
			virtual void GACALL Connected()
			{
				GaSPEAStubBase::Connected();
				_scalingStep->SetSetup( Population::GaScalingSetup( &_speaOperation, &_speaParameters, &Population::GaScalingConfig( &Fitness::Representation::GaMVFitnessParams( 2 ) ) ) );
			}

		};

		/// <summary><c>GaSPEA2Stub</c> class represents stub that implement SPEA-II.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaSPEA2Stub : public GaSPEAStubBase
		{

		protected:

			/// <summary>SPEA2 parameters.</summary>
			Multiobjective::SPEA::GaSPEA2Params _speaParameters;

			/// <summary>SPEA2 operation.</summary>
			Multiobjective::SPEA::GaSPEA2 _speaOperation;

		public:

			/// <summary>Initializes SPEA stub with population on which it will operate and all operations and parameters.</summary>
			/// <param name="populationID">ID of the population on which the stub will operate.</param>
			/// <param name="statisticsID">ID of the object that whill store statistical information.</param>
			/// <param name="initializator">initialization operation for the population.</param>
			/// <param name="fitnessOperation">fitness operation that evaluates chromosomes in the population.</param>
			/// <param name="comparator">fitness comparator used by the population.</param>
			/// <param name="populationParameters">parameters of the population on which the stub operates.</param>
			/// <param name="mating">mating operation that produce offspring chromosomes.</param>
			/// <param name="coupling">coupling operation used by the stub.</param>
			/// <param name="selectionParameters">parameters of the selection operation.</param>
			/// <param name="speaParameters">SPEA parameters.</param>
			GaSPEA2Stub(int populationID,
				int statisticsID,
				const Chromosome::GaInitializatorSetup& initializator,
				const Population::GaPopulationFitnessOperationSetup& fitnessOperation,
				const Fitness::GaFitnessComparatorSetup& comparator,
				const Population::GaPopulationParams& populationParameters,
				const Chromosome::GaMatingSetup& mating,
				const Population::GaCouplingSetup& coupling,
				const Population::SelectionOperations::GaTournamentSelectionParams& selectionParameters,
				const Multiobjective::SPEA::GaSPEA2Params& speaParameters) : 
				GaSPEAStubBase(populationID, statisticsID, initializator, fitnessOperation, comparator, populationParameters, mating, coupling, selectionParameters)
					{ SetSPEAParameters( speaParameters ); }

			/// <summary>Initializes SPEA stub without population on which it will operate and with default operations and parameters.</summary>
			GaSPEA2Stub() { }

			/// <summary><c>SetSPEAParameters</c> method sets parameters for SPEA operation.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="params">new SPEA parameters.</param>
			GAL_API
			void GACALL SetSPEAParameters(const Multiobjective::SPEA::GaSPEA2Params& params);

		protected:

			/// <summary>Prepares workflow for the execution.</summary>
			virtual void GACALL Connected()
			{
				GaSPEAStubBase::Connected();
				_scalingStep->SetSetup( Population::GaScalingSetup( &_speaOperation, &_speaParameters, &Population::GaScalingConfig( &Fitness::Representation::GaMVFitnessParams( 2 ) ) ) );
			}

		};

	} // Stubs
} // Algorithm

#endif // __GA_SPEA_STUB_H__


#pragma once

#include "..\..\source\Initialization.h"
#include "..\..\Problems\CSP.h"
#include "..\..\source\Crossovers.h"
#include "..\..\source\SimpleStub.h"
#include "..\..\source\Matings.h"
#include "..\..\source\PopulationStatistics.h"
#include "..\..\source\StopCriteria.h"

#include "..\..\source\Selections.h"
#include "..\..\source\Couplings.h"
#include "..\..\source\Replacements.h"
#include "..\..\source\FitnessSharing.h"
#include "..\..\source\Scalings.h"
#include "..\..\source\StopCriteria.h"

class CSPAlgorithm
{
private:
	enum WorkflowDataIDs
	{
		WDID_POPULATION,
		WDID_POPULATION_STATS
	};

	Problems::CSP::CspInitializator _initializator;
	Problems::CSP::CspCrossoverOperation _crossover;
	Problems::CSP::CspMutationOperation _mutation;
	Problems::CSP::CspFitnessOperation _fitnessOperation;
	Fitness::Comparators::GaSimpleComparator _fitnessComparator;

	Population::GaCombinedFitnessOperation _populationFitnessOperation;

	Chromosome::MatingOperations::GaBasicMatingOperation _mating;

	Population::GaPopulationSizeTracker _sizeTracker;
	Population::GaRawFitnessTracker _rawTracker;
	Population::GaScaledFitnessTracker _scaledTracker;

	Population::SelectionOperations::GaTournamentSelection _selection;
	Population::CouplingOperations::GaSimpleCoupling _coupling;
	Population::ReplacementOperations::GaWorstReplacement _replacement;
	Population::ScalingOperations::GaNoScaling _scaling;

	Algorithm::StopCriteria::GaStatsChangesCriterion _stopCriterion;

	Common::Workflows::GaWorkflow _workflow;


public:
	CSPAlgorithm(Common::Observing::GaEventHandler* newGenHandler, Common::Observing::GaEventHandler* stateChangedHandler);
	~CSPAlgorithm();

	void SetParameters(int sWidth, int sHeight, int iMinWidth, int iMaxWidth, int iMinHeight, int iMaxHeight, int iCount);

	inline void Start() { _workflow.Start(); }
	inline void Stop() { _workflow.Stop(); }

};

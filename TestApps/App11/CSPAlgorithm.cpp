
#include "CSPAlgorithm.h"

CSPAlgorithm::CSPAlgorithm(Common::Observing::GaEventHandler* newGenHandler) : 
	_populationFitnessOperation(&_fitnessOperation),
	_workflow(NULL)
{
	Chromosome::GaMatingConfig matingConfiguration(
		Chromosome::GaCrossoverSetup( &_crossover, &Chromosome::GaCrossoverPointParams( 0.80f, 2, 1 ), NULL ),
		Chromosome::GaMutationSetup( &_mutation, &Chromosome::GaMutationSizeParams( 0.33f, true, 2L ), NULL ) );

	Fitness::GaFitnessComparatorSetup fitnessComparatorSetup( &_fitnessComparator,
		&Fitness::Comparators::GaSimpleComparatorParams( Fitness::Comparators::GACT_MAXIMIZE_ALL ), NULL );

	Algorithm::Stubs::GaSimpleGAStub::GaStatTrackersCollection trackers;
	trackers[ Population::GaPopulationSizeTracker::TRACKER_ID ] =  &_sizeTracker;
	trackers[ Population::GaRawFitnessTracker::TRACKER_ID ] =  &_rawTracker;
	trackers[ Population::GaScaledFitnessTracker::TRACKER_ID ] =  &_scaledTracker;

	Population::GaSelectionSetup selectionSetup( &_selection,
		&Population::SelectionOperations::GaTournamentSelectionParams( 8, -1, 2, 2, Population::SelectionOperations::GaTournamentSelectionParams::GATST_ROULETTE_WHEEL_SELECTION ),
		&Population::SelectionOperations::GaTournamentSelectionConfig( fitnessComparatorSetup, Chromosome::GaMatingSetup() ) );

	Population::GaCouplingSetup couplingSetup( &_coupling, &Population::GaCouplingParams( 20, 1 ),
		&Population::GaCouplingConfig( Chromosome::GaMatingSetup( &_mating, NULL, &matingConfiguration ) ) );

	Population::GaReplacementSetup replacementSetup( &_replacement, &Population::GaReplacementParams( 20 ), &Population::GaReplacementConfig() );
	Population::GaScalingSetup scalingSetup( &_scaling, NULL, &Population::GaScalingConfig() );

	_simpleGA = new Algorithm::Stubs::GaSimpleGAStub( WDID_POPULATION, WDID_POPULATION_STATS,
		Chromosome::GaInitializatorSetup(),
		Population::GaPopulationFitnessOperationSetup( &_populationFitnessOperation, NULL,
		&Fitness::GaFitnessOperationConfig( NULL ) ),
		fitnessComparatorSetup,
		Population::GaPopulationParams( /*100*/40, 0, Population::GaPopulationParams::GAPFO_FILL_ON_INIT ),
		trackers,
		Chromosome::GaMatingSetup(),
		selectionSetup,
		couplingSetup,
		replacementSetup,
		scalingSetup,
		Population::GaFitnessComparatorSortingCriteria( fitnessComparatorSetup, Population::GaChromosomeStorage::GAFT_RAW ) );
	_simpleGA->SetBranchCount( 2 );

	_workflow.RemoveConnection( *_workflow.GetFirstStep()->GetOutboundConnections().begin(), true );

	_barrier = new Common::Workflows::GaWorkflowBarrier();
	_simpleGA->Connect( _workflow.GetFirstStep(), _barrier );

	_branchGroup = (Common::Workflows::GaBranchGroup*)_workflow.ConnectSteps( _barrier, _workflow.GetLastStep(), 0 );

	_stopStep = new Algorithm::StopCriteria::GaStopCriterionStep(
		Algorithm::StopCriteria::GaStopCriterionSetup( &_stopCriterion,
		&Algorithm::StopCriteria::GaStatsChangesCriterionParams(
		Population::GADV_BEST_FITNESS, 20 ), NULL ), _workflow.GetWorkflowData(), WDID_POPULATION_STATS );

	_branchTransition = new Common::Workflows::GaBranchGroupTransition();

	_branchGroup->GetBranchGroupFlow()->SetFirstStep( _stopStep );
	_branchGroup->GetBranchGroupFlow()->ConnectSteps( _stopStep, _branchTransition, 0 );
	_transitionConnection = _workflow.ConnectSteps( _branchTransition, _simpleGA->GetStubFlow().GetFirstStep(), 1 );

	Common::Workflows::GaDataCache<Population::GaPopulation> population( _workflow.GetWorkflowData(), WDID_POPULATION );

	population.GetData().GetEventManager().AddEventHandler( Population::GaPopulation::GAPE_NEW_GENERATION, newGenHandler );
}

CSPAlgorithm::~CSPAlgorithm()
{
	_workflow.Wait();

	Common::Workflows::GaDataCache<Population::GaPopulation> population( _workflow.GetWorkflowData(), WDID_POPULATION );
	population.GetData().GetEventManager().RemoveEventHandlers( Population::GaPopulation::GAPE_NEW_GENERATION );
	population.Clear();

	_workflow.RemoveConnection( _transitionConnection, true );
	_branchGroup->GetBranchGroupFlow()->RemoveStep( _stopStep, true, true );
	_branchGroup->GetBranchGroupFlow()->RemoveStep( _branchTransition, true, true );
	_workflow.RemoveStep( _barrier, true, true );

	_simpleGA->Disconnect();

	delete _simpleGA;
}

void CSPAlgorithm::SetParameters(int sWidth, int sHeight, int iMinWidth, int iMaxWidth, int iMinHeight, int iMaxHeight, int iCount)
{
	Common::Data::GaSingleDimensionArray<Problems::CSP::Item> items( iCount );
	for( int i = 0; i < iCount; i++ )
		items[ i ] = Problems::CSP::Item( 
		Problems::CSP::Size(GaGlobalRandomIntegerGenerator->Generate( iMinWidth, iMaxWidth ), GaGlobalRandomIntegerGenerator->Generate( iMinHeight, iMaxHeight ) ),
		std::string( "L" ),  i );

	Chromosome::GaInitializatorSetup initializatorSetup( &_initializator, NULL, &Chromosome::GaInitializatorConfig(
		&Problems::CSP::CspConfigBlock( items, Problems::CSP::Size( sWidth, sHeight ) ) ) );

	Common::Workflows::GaDataCache<Population::GaPopulation> population( _workflow.GetWorkflowData(), WDID_POPULATION );
	population.GetData().SetInitializator( initializatorSetup );
}

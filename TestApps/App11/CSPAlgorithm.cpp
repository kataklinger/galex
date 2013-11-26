
#include "CSPAlgorithm.h"

CSPAlgorithm::CSPAlgorithm(Common::Observing::GaEventHandler* newGenHandler) : 
	_populationFitnessOperation(&_fitnessOperation),
	_workflow(NULL)
{
	Chromosome::GaMatingConfig matingConfiguration(
		Chromosome::GaCrossoverSetup( &_crossover, &Chromosome::GaCrossoverPointParams( 1.0f, 2, 1 ), NULL ),
		Chromosome::GaMutationSetup( &_mutation, &Chromosome::GaMutationSizeParams( 0.66f, true, 2L ), NULL ) );

	Fitness::GaFitnessComparatorSetup fitnessComparatorSetup( &_fitnessComparator,
		&Fitness::Comparators::GaSimpleComparatorParams( Fitness::Comparators::GACT_MAXIMIZE_ALL ), NULL );

	Algorithm::Stubs::GaSimpleGAStub::GaStatTrackersCollection trackers;
	trackers[ Population::GaPopulationSizeTracker::TRACKER_ID ] =  &_sizeTracker;
	trackers[ Population::GaRawFitnessTracker::TRACKER_ID ] =  &_rawTracker;
	trackers[ Population::GaScaledFitnessTracker::TRACKER_ID ] =  &_scaledTracker;

	Population::GaSelectionSetup selectionSetup( &_selection,
		&Population::SelectionOperations::GaTournamentSelectionParams( 2, -1, 2, 2, Population::SelectionOperations::GaTournamentSelectionParams::GATST_ROULETTE_WHEEL_SELECTION ),
		&Population::SelectionOperations::GaTournamentSelectionConfig( fitnessComparatorSetup, Chromosome::GaMatingSetup() ) );

	Population::GaCouplingSetup couplingSetup( &_coupling, &Population::GaCouplingParams( 50, 1 ),
		&Population::GaCouplingConfig( Chromosome::GaMatingSetup( &_mating, NULL, &matingConfiguration ) ) );

	Population::GaReplacementSetup replacementSetup( &_replacement, &Population::GaReplacementParams( 50 ), &Population::GaReplacementConfig() );
	Population::GaScalingSetup scalingSetup( &_scaling, NULL, &Population::GaScalingConfig() );

	Algorithm::Stubs::GaSimpleGAStub simpleGA( WDID_POPULATION, WDID_POPULATION_STATS,
		Chromosome::GaInitializatorSetup(),
		Population::GaPopulationFitnessOperationSetup( &_populationFitnessOperation, NULL,
		&Fitness::GaFitnessOperationConfig( NULL ) ),
		fitnessComparatorSetup,
		Population::GaPopulationParams( 100, 0, Population::GaPopulationParams::GAPFO_FILL_ON_INIT ),
		trackers,
		Chromosome::GaMatingSetup(),
		selectionSetup,
		couplingSetup,
		replacementSetup,
		scalingSetup,
		Population::GaFitnessComparatorSortingCriteria( fitnessComparatorSetup, Population::GaChromosomeStorage::GAFT_RAW ) );
	simpleGA.SetBranchCount( 2 );

	_workflow.RemoveConnection( *_workflow.GetFirstStep()->GetOutboundConnections().begin(), true );

	Common::Workflows::GaWorkflowBarrier* br1 = new Common::Workflows::GaWorkflowBarrier();
	simpleGA.Connect( _workflow.GetFirstStep(), br1 );

	Common::Workflows::GaBranchGroup* bg1 = (Common::Workflows::GaBranchGroup*)_workflow.ConnectSteps( br1, _workflow.GetLastStep(), 0 );

	Algorithm::StopCriteria::GaStopCriterionStep* stopStep = new Algorithm::StopCriteria::GaStopCriterionStep(
		Algorithm::StopCriteria::GaStopCriterionSetup( &_stopCriterion,
		&Algorithm::StopCriteria::GaStatsChangesCriterionParams(
		Population::GADV_BEST_FITNESS, 100), NULL ), _workflow.GetWorkflowData(), WDID_POPULATION_STATS );

	Common::Workflows::GaBranchGroupTransition* bt1 = new Common::Workflows::GaBranchGroupTransition();

	bg1->GetBranchGroupFlow()->SetFirstStep( stopStep );
	bg1->GetBranchGroupFlow()->ConnectSteps( stopStep, bt1, 0 );
	_workflow.ConnectSteps( bt1, simpleGA.GetStubFlow().GetFirstStep(), 1 );

	Common::Workflows::GaDataCache<Population::GaPopulation> population( _workflow.GetWorkflowData(), WDID_POPULATION );

	population.GetData().GetEventManager().AddEventHandler( Population::GaPopulation::GAPE_NEW_GENERATION, newGenHandler );
}

CSPAlgorithm::~CSPAlgorithm()
{
	_workflow.Wait();
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

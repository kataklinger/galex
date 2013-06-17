
#include "..\..\source\Initialization.h"
#include "..\..\Problems\TNG.h"
#include "..\..\source\SimpleStub.h"
#include "..\..\source\Matings.h"
#include "..\..\source\PopulationStatistics.h"
#include "..\..\source\StopCriteria.h"

#include "..\..\source\Selections.h"
#include "..\..\source\Replacements.h"
#include "..\..\source\FitnessSharing.h"
#include "..\..\source\Scalings.h"
#include "..\..\source\StopCriteria.h"

enum WorkflowDataIDs
{
	WDID_POPULATION,
	WDID_POPULATION_STATS
};

Problems::TNG::TngInitializator initializator;
Problems::TNG::TngCrossover crossover;
Problems::TNG::TngMutation mutation;
Problems::TNG::TngChromosomeComparator chromosomeComparator;
Problems::TNG::TngFitnessOperation fitnessOperation;
Fitness::Comparators::GaSimpleComparator fitnessComparator;

Population::GaCombinedFitnessOperation populationFitnessOperation( &fitnessOperation );

Chromosome::MatingOperations::GaBasicMatingOperation mating;

Population::GaPopulationSizeTracker sizeTracker;
Population::GaRawFitnessTracker rawTracker;
Population::GaScaledFitnessTracker scaledTracker;

//Population::SelectionOperations::GaRandomSelection selection;
Population::SelectionOperations::GaRouletteWheelSelection selection;
//Population::ReplacementOperations::GaParentReplacement replacement;
Population::ReplacementOperations::GaRandomReplacement replacement;
//Population::ScalingOperations::GaLinearScaling scaling;
Population::ScalingOperations::GaNoScaling scaling;
//Population::ScalingOperations::GaShareFitnessScaling scaling;

Algorithm::StopCriteria::GaStatsCriterion<Fitness::GaFitness> stopCriterion;

void GACALL MyHandler(int id, Common::Observing::GaEventData& data)
{
	const Population::GaPopulation& population = ( (Population::GaPopulationEventData&)data ).GetPopulation();

	const Problems::TNG::TngChromosome& chromosome = (const Problems::TNG::TngChromosome&)*population[ 0 ].GetChromosome();
	const Problems::TNG::TngConfigBlock& ccb = (const Problems::TNG::TngConfigBlock&)*chromosome.GetConfigBlock();

	//if( population.GetStatistics().GetCurrentGeneration() % 100 != 1 )//&&
	////	( (Fitness::Representation::GaSVFitness<float>&)population[ 0 ].GetFitness( Population::GaChromosomeStorage::GAFT_RAW ) ).GetValue() != 0.0f )
	//	return;

	if( population.GetStatistics().GetCurrentGeneration() != 1 && !population.GetStatistics().GetValue<Fitness::GaFitness>( Population::GADV_BEST_FITNESS ).IsChanged( 2 ) )
		return;
	const int* nums = ccb.GetNumbers();

	float prog = population.GetStatistics().GetValue( Population::GADV_BEST_FITNESS ).GetRelativeProgress( 2 );

	printf( "\n==========================================\n\n" );
	printf( "Generation: %d\n", population.GetStatistics().GetCurrentGeneration() );
	printf( "| %d | %d | %d | %d | %2d | %3d || %3d ||", nums[ 0 ], nums[ 1 ], nums[ 2 ], nums[ 3 ], nums[ 4 ], nums[ 5 ], ccb.GetTargetNumber() );
	printf( "\n==========================================\n\n" );

	for( int i = 0; i < population.GetCount(); i++ )
	{
		const Problems::TNG::TngChromosome& chromosome = (const Problems::TNG::TngChromosome&)*population[ i ].GetChromosome();

		int index = 0;
		int value = Problems::TNG::TngCalculateValue(
			(const Common::Data::GaTreeNode<Problems::TNG::TngNode>*)chromosome.GetGenes().GetRoot(), nums );
		//Problems::TNG::TngCalculateValue( (Common::Data::GaTreeNode<Problems::TNG::TngNode>*)chromosome.GetGenes().GetRoot(), Problems::TNG::TngValueInterpret( nums ), &penalty );
		float fitness1 = ( (Fitness::Representation::GaSVFitness<float>&)population[ i ].GetFitness( Population::GaChromosomeStorage::GAFT_RAW ) ).GetValue();
		float fitness2 = ( (Fitness::Representation::GaSVFitness<float>&)population[ i ].GetFitness( Population::GaChromosomeStorage::GAFT_SCALED ) ).GetValue();

		printf( "(%5.2f) %d=", fitness1, value );
		Problems::TNG::TngPrintChromosome( chromosome );
	}
}

Common::Observing::GaNonmemberEventHandler newGenHandler( MyHandler );

int main()
{
	GaInitialize();

	{

		int values[ Problems::TNG::TNG_NUMBER_COUNT ];

		values[ 0 ] = GaGlobalRandomIntegerGenerator->Generate( 1, 9 );
		values[ 1 ] = GaGlobalRandomIntegerGenerator->Generate( 1, 9 );
		values[ 2 ] = GaGlobalRandomIntegerGenerator->Generate( 1, 9 );
		values[ 3 ] = GaGlobalRandomIntegerGenerator->Generate( 1, 9 );
		values[ 4 ] = ( 10 + GaGlobalRandomIntegerGenerator->Generate( 0, 2 ) * 5 );
		values[ 5 ] = ( 25 + GaGlobalRandomIntegerGenerator->Generate( 0, 3 ) * 25 );

		int target = ( GaGlobalRandomIntegerGenerator->Generate( 100, 999 ) );

		Chromosome::GaMatingConfig matingConfiguration(
			Chromosome::GaCrossoverSetup( &crossover, &Chromosome::GaCrossoverPointParams( 0.8f, 2, 1 ), NULL ),
			Chromosome::GaMutationSetup( &mutation, &Chromosome::GaMutationSizeParams( 0.3f, true, 2L ), NULL ) );

		Chromosome::GaInitializatorSetup initializatorSetup( &initializator, NULL, &Chromosome::GaInitializatorConfig(
			&Problems::TNG::TngConfigBlock( values, target, &Chromosome::Representation::GaBinaryChromosomeParams() ) ) );
		Fitness::GaFitnessComparatorSetup fitnessComparatorSetup( &fitnessComparator, &Fitness::Comparators::GaSimpleComparatorParams( Fitness::Comparators::GACT_MAXIMIZE_ALL ), NULL );

		Algorithm::Stubs::GaSimpleGAStub::GaStatTrackersCollection trackers;
		trackers[ Population::GaPopulationSizeTracker::TRACKER_ID ] =  &sizeTracker;
		trackers[ Population::GaRawFitnessTracker::TRACKER_ID ] =  &rawTracker;
		trackers[ Population::GaScaledFitnessTracker::TRACKER_ID ] =  &scaledTracker;

		Population::GaSelectionSetup selectionSetup( &selection, &Population::SelectionOperations::GaDuplicatesSelectionParams( 8, 1, 2 ),
			&Population::GaCouplingConfig( Chromosome::GaMatingSetup( &mating, NULL, &matingConfiguration ) ) );

		Population::GaReplacementSetup replacementSetup( &replacement, &Population::ReplacementOperations::GaRandomReplacementParams( 8, 4, 3 ),
			&Population::GaReplacementConfig( Chromosome::GaChromosomeComparatorSetup( &chromosomeComparator, NULL, NULL ) ) );

		Population::GaScalingSetup scalingSetup( &scaling, &Population::ScalingOperations::GaScalingFactorParams( 2.0f )/*&Population::ScalingOperations::GaShareFitnessParams( 0.2f, 1.0f, 4 )*/,
			&Population::ScalingOperations::GaShareFitnessScalingConfig( NULL, Chromosome::GaChromosomeComparatorSetup( &chromosomeComparator, NULL, NULL ) ) );

		/*
		Population::GaPopulation* population = new Population::GaPopulation( Population::GaPopulationParams( 32, 0, Population::GaPopulationParams::GAPFO_FILL_ON_INIT ),
		Chromosome::GaInitializatorSetup( &initializator, NULL, &Chromosome::GaInitializatorConfig(
		&Problems::TNG::TngConfigBlock( values, target, &Chromosome::Representation::GaBinaryChromosomeParams() ) ) ),
		Population::GaPopulationFitnessOperationSetup( &populationFitnessOperation, NULL, &Fitness::GaFitnessOperationConfig() ),
		fitnessComparatorSetup );
		*/

		Algorithm::Stubs::GaSimpleGAStub simpleGA(  WDID_POPULATION, WDID_POPULATION_STATS,
			initializatorSetup,
			Population::GaPopulationFitnessOperationSetup( &populationFitnessOperation, NULL, &Fitness::GaFitnessOperationConfig() ),
			fitnessComparatorSetup,
			Population::GaPopulationParams( 32, 0, Population::GaPopulationParams::GAPFO_FILL_ON_INIT ),
			trackers,
			Chromosome::GaMatingSetup(),
			selectionSetup,
			Population::GaCouplingSetup(),
			replacementSetup,
			scalingSetup,
			Population::GaFitnessComparatorSortingCriteria( fitnessComparatorSetup, Population::GaChromosomeStorage::GAFT_RAW ) );

		/*
		Common::Workflows::GaWorkflow workflow( NULL );
		Common::Workflows::GaDataStorage* storage = workflow.GetWorkflowData();

		storage->AddData( new Common::Workflows::GaDataEntry<Population::GaPopulation>( 1, population ), Common::Workflows::GADSL_WORKFLOW );
		storage->AddData( new Common::Workflows::GaDataEntry<Statistics::GaStatistics>( 2, Common::Memory::GaAutoPtr<Statistics::GaStatistics>( &population->GetStatistics(),
		Common::Memory::GaNoDeletionPolicy<Statistics::GaStatistics>::GetInstance() ) ), Common::Workflows::GADSL_WORKFLOW );
		storage->AddData( new Common::Workflows::GaDataEntry<Population::GaChromosomeGroup>( 3, new Population::GaChromosomeGroup( false ) ), Common::Workflows::GADSL_WORKFLOW );

		Common::Workflows::GaWorkflowBarrier* br1 = new Common::Workflows::GaWorkflowBarrier();
		Common::Workflows::GaWorkflowBarrier* br2 = new Common::Workflows::GaWorkflowBarrier();

		Common::Workflows::GaBranchGroup* bg1 = (Common::Workflows::GaBranchGroup*)workflow.ConnectSteps( workflow.GetFirstStep(), br1, 0 );
		Common::Workflows::GaBranchGroup* bg2 = (Common::Workflows::GaBranchGroup*)workflow.ConnectSteps( br1, br2, 0 );
		Common::Workflows::GaBranchGroup* bg3 = (Common::Workflows::GaBranchGroup*)workflow.ConnectSteps( br2, workflow.GetLastStep(), 0 );

		Common::Workflows::GaFlowStep* initStep =
		new Common::Workflows::GaSimpleMethodExecStep<Population::GaPopulation, Common::Workflows::GaMethodExecIgnoreBranch<Population::GaPopulation> >(
		&Population::GaPopulation::Initialize, storage, 1 );

		bg1->GetBranchGroupFlow()->SetFirstStep( initStep );

		Common::Workflows::GaFlowStep* sortStep = new Population::GaSortPopulationStep<Population::GaFitnessSortingCriteria>( storage, 1, Population::GaFitnessSortingCriteria( population ) );

		Common::Workflows::GaFlowStep* scalingStep = new Population::GaScalingStep(
		Population::GaScalingSetup( &scaling, &Population::ScalingOperations::GaScalingFactorParams( 2.0f ), //&Population::ScalingOperations::GaShareFitnessParams( 0.2f, 1.0f, 4 ),
		&Population::ScalingOperations::GaShareFitnessScalingConfig( NULL, Chromosome::GaChromosomeComparatorSetup( &chromosomeComparator, NULL, NULL ) ) ), storage, 1 );

		Chromosome::GaMatingConfig matingConfiguration(
		Chromosome::GaCrossoverSetup( &crossover, &Chromosome::GaCrossoverPointParams( 0.8f, 2, 1 ), NULL ),
		Chromosome::GaMutationSetup( &mutation, &Chromosome::GaMutationSizeParams( 0.3f, true, 2L ), NULL ) );

		Common::Workflows::GaFlowStep* selectionStep = new Population::GaSelectionStep(
		Population::GaSelectionSetup( &selection, &Population::SelectionOperations::GaDuplicatesSelectionParams( 8, 1, 2 ),
		&Population::GaCouplingConfig( Chromosome::GaMatingSetup( &mating, NULL, &matingConfiguration ) ) ), storage, 1, storage, 3 );

		Common::Workflows::GaFlowStep* replacementStep = new Population::GaReplacementStep(
		Population::GaReplacementSetup( &replacement, &Population::ReplacementOperations::GaRandomReplacementParams( 8, 4, 3 ),
		&Population::GaReplacementConfig( Chromosome::GaChromosomeComparatorSetup( &chromosomeComparator, NULL, NULL ) ) ),
		storage, 3, storage, 1 );

		Common::Workflows::GaFlowStep* nextGenStep = new Common::Workflows::GaSimpleMethodExecStep<Population::GaPopulation>( &Population::GaPopulation::NextGeneration, storage, 1 );

		Common::Workflows::GaBranchGroupFlow* flow2 = bg2->GetBranchGroupFlow();
		flow2->SetFirstStep( sortStep );
		flow2->ConnectSteps( sortStep, scalingStep, 0 );
		flow2->ConnectSteps( scalingStep, nextGenStep, 0 );
		flow2->ConnectSteps( selectionStep, replacementStep, 0 );
		flow2->ConnectSteps( replacementStep, sortStep, 0 );

		Fitness::Representation::GaSVFitness<float> targetFitness( 1.0f, Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams>::NullPtr );

		Algorithm::StopCriteria::GaStopCriterionStep* stopStep = new Algorithm::StopCriteria::GaStopCriterionStep(
		Algorithm::StopCriteria::GaStopCriterionSetup( &stopCriterion,
		&Algorithm::StopCriteria::GaStatsCriterionParams<Fitness::GaFitness>(
		Population::GADV_BEST_FITNESS, targetFitness, Algorithm::StopCriteria::GAST_STOP_IF_EQUAL_TO,
		Algorithm::StopCriteria::GaStatsCriterionComparator<Fitness::GaFitness>( fitnessComparatorSetup ) ), NULL ), storage, 2 );

		Common::Workflows::GaBranchGroupTransition* bt32 = new Common::Workflows::GaBranchGroupTransition();

		Common::Workflows::GaBranchGroupFlow* flow3 = bg3->GetBranchGroupFlow();

		flow3->SetFirstStep( stopStep );
		flow3->ConnectSteps( stopStep, bt32, 0 );
		workflow.ConnectSteps( bt32, selectionStep, 1 );

		population->GetEventManager().AddEventHandler( Population::GaPopulation::GAPE_NEW_GENERATION, &newGenHandler );

		workflow.Start();
		workflow.Wait();
		*/

		Common::Workflows::GaWorkflow workflow( NULL );

		workflow.RemoveConnection( *workflow.GetFirstStep()->GetOutboundConnections().begin(), true );

		Common::Workflows::GaWorkflowBarrier* br1 = new Common::Workflows::GaWorkflowBarrier();
		simpleGA.Connect( workflow.GetFirstStep(), br1 );

		Common::Workflows::GaBranchGroup* bg1 = (Common::Workflows::GaBranchGroup*)workflow.ConnectSteps( br1, workflow.GetLastStep(), 0 );

		Fitness::Representation::GaSVFitness<float> targetFitness( 1.0f, Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams>::NullPtr );

		Algorithm::StopCriteria::GaStopCriterionStep* stopStep = new Algorithm::StopCriteria::GaStopCriterionStep(
			Algorithm::StopCriteria::GaStopCriterionSetup( &stopCriterion,
			&Algorithm::StopCriteria::GaStatsCriterionParams<Fitness::GaFitness>(
			Population::GADV_BEST_FITNESS, targetFitness, Algorithm::StopCriteria::GAST_STOP_IF_EQUAL_TO,
			Algorithm::StopCriteria::GaStatsCriterionComparator<Fitness::GaFitness>( fitnessComparatorSetup ) ), NULL ), workflow.GetWorkflowData(), WDID_POPULATION_STATS );

		Common::Workflows::GaBranchGroupTransition* bt1 = new Common::Workflows::GaBranchGroupTransition();

		bg1->GetBranchGroupFlow()->SetFirstStep( stopStep );
		bg1->GetBranchGroupFlow()->ConnectSteps( stopStep, bt1, 0 );
		workflow.ConnectSteps( bt1, simpleGA.GetStubFlow().GetFirstStep(), 1 );

		Common::Workflows::GaDataCache<Population::GaPopulation> population( workflow.GetWorkflowData(), WDID_POPULATION );

		population.GetData().GetEventManager().AddEventHandler( Population::GaPopulation::GAPE_NEW_GENERATION, &newGenHandler );

		workflow.Start();
		workflow.Wait();
	}

	GaFinalize();

	return 0;
}

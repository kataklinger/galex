

#include "..\..\source\Initialization.h"
#include "..\..\Problems\F1.h"
#include "..\..\source\Matings.h"
#include "..\..\source\SPEAStub.h"
#include "..\..\source\StopCriteria.h"

enum WorkflowDataIDs
{
	WDID_POPULATION,
	WDID_POPULATION_STATS
};

enum ChromosomeTagIDs
{
	CTID_DOMINANCE_LIST,
	CTID_STRENGTH,
	CTID_DOMINATED,
};

enum PopulationTagIDs
{
	PTID_CROSSOVER_BUFFER,
	PTID_CLUSTER_STORAGE
};

Problems::F1::F1Initializator initializator;
Problems::F1::F1Crossover crossover;
Problems::F1::F1Mutation mutation;
Problems::F1::F1FitnessOperation fitnessOperation;
Problems::F1::F1FitnessComparator fitnessComparator;
Problems::F1::F1ChromosomeComparator chromosomeComparator;

Population::GaCombinedFitnessOperation populationFitnessOperation( &fitnessOperation );

Chromosome::MatingOperations::GaBasicMatingOperation mating;

Algorithm::StopCriteria::GaGenerationCriterion stopCriterion;

void GACALL MyHandler(int id, Common::Observing::GaEventData& data)
{
	const Population::GaPopulation& population = ( (Population::GaPopulationEventData&)data ).GetPopulation();

	for( int j = 0; j < population.GetCount() / 2; j++ )
	{
		for( int i = 0; i < 2; i++ )
		{
			float prob = population[ j * 2 + i ].GetFitness( Population::GaChromosomeStorage::GAFT_SCALED ).GetProbabilityBase();

			float x = ( (Problems::F1::F1Chromosome*)population[ j * 2 + i ].GetChromosome().GetRawPtr() )->GetGene();

			float y1 = ( (Problems::F1::F1Fitness&)population[ j * 2 + i ].GetFitness( Population::GaChromosomeStorage::GAFT_RAW ) ).GetTypedValues()[ 0 ];
			float y2 = ( (Problems::F1::F1Fitness&)population[ j * 2 + i ].GetFitness( Population::GaChromosomeStorage::GAFT_RAW ) ).GetTypedValues()[ 1 ];

			printf( "(%5.2f = %5.2f, %5.2f | %5.2f) ", x, y1, y2, prob );
		}

		printf( "\n" );
	}

	printf( "\n==========================================\n\n" );
}

Common::Observing::GaNonmemberEventHandler newGenHandler( MyHandler );

int main()
{
	GaInitialize();

	{
		Chromosome::GaMatingConfig matingConfiguration(
			Chromosome::GaCrossoverSetup( &crossover, &Chromosome::GaCrossoverParams( 0.8f, 2 ), NULL ),
			Chromosome::GaMutationSetup( &mutation, &Chromosome::GaMutationParams( 0.03f, false ), NULL ) );

		Algorithm::Stubs::GaSPEAStub speaStub( WDID_POPULATION, WDID_POPULATION_STATS, 
			Chromosome::GaInitializatorSetup( &initializator, NULL, &Chromosome::GaInitializatorConfig() ),
			Population::GaPopulationFitnessOperationSetup( &populationFitnessOperation, NULL, &Fitness::GaFitnessOperationConfig( &Fitness::Representation::GaMVFitnessParams( 2 ) ) ),
			Fitness::GaFitnessComparatorSetup( &fitnessComparator, &Fitness::Comparators::GaSimpleComparatorParams( Fitness::Comparators::GACT_MINIMIZE_ALL ), NULL ),
			Population::GaPopulationParams( 64, 0 ),
			Chromosome::GaMatingSetup( &mating, NULL, &matingConfiguration ),
			Population::GaCouplingSetup(),
			Population::SelectionOperations::GaTournamentSelectionParams( 32, PTID_CROSSOVER_BUFFER, -1, 2,
			Population::SelectionOperations::GaTournamentSelectionParams::GATST_ROULETTE_WHEEL_SELECTION ),
			Multiobjective::SPEA::GaSPEAParams( CTID_DOMINANCE_LIST, CTID_STRENGTH, CTID_DOMINATED, PTID_CLUSTER_STORAGE ) );

		Common::Workflows::GaWorkflow workflow( NULL );

		workflow.RemoveConnection( *workflow.GetFirstStep()->GetOutboundConnections().begin(), true );

		Common::Workflows::GaWorkflowBarrier* br1 = new Common::Workflows::GaWorkflowBarrier();
		speaStub.Connect( workflow.GetFirstStep(), br1 );

		Common::Workflows::GaBranchGroup* bg1 = (Common::Workflows::GaBranchGroup*)workflow.ConnectSteps( br1, workflow.GetLastStep(), 0 );

		Algorithm::StopCriteria::GaStopCriterionStep* stopStep = new Algorithm::StopCriteria::GaStopCriterionStep( 
			Algorithm::StopCriteria::GaStopCriterionSetup( &stopCriterion, &Algorithm::StopCriteria::GaGenerationCriterionParams( 100 ), NULL ),
			workflow.GetWorkflowData(), WDID_POPULATION_STATS );

		Common::Workflows::GaBranchGroupTransition* bt1 = new Common::Workflows::GaBranchGroupTransition();

		bg1->GetBranchGroupFlow()->SetFirstStep( stopStep );
		bg1->GetBranchGroupFlow()->ConnectSteps( stopStep, bt1, 0 );
		workflow.ConnectSteps( bt1, speaStub.GetStubFlow().GetFirstStep(), 1 );

		Common::Workflows::GaDataCache<Population::GaPopulation> population( workflow.GetWorkflowData(), WDID_POPULATION );

		population.GetData().GetEventManager().AddEventHandler( Population::GaPopulation::GAPE_NEW_GENERATION, &newGenHandler );

		workflow.Start();
		workflow.Wait();
	}

	GaFinalize();
}

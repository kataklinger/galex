
#include "..\..\source\Initialization.h"
#include "..\..\Problems\F1.h"
#include "..\..\source\Matings.h"
#include "..\..\source\PESAStub.h"
#include "..\..\source\StopCriteria.h"

enum WorkflowDataIDs
{
	WDID_POPULATION,
	WDID_POPULATION_STATS
};

enum ChromosomeTagIDs
{
	CTID_DOMINATED,
	CTID_HYPERBOX,
	CTID_HYPERBOX_INFO
};

enum PopulationTagIDs
{
	PTID_CROSSOVER_BUFFER,
	PTID_HYPERBOX_INFO_BUFFER,
	PTID_NONDOMINATED_BUFFER,
	PTID_DISTANCE_STORAGE,
	PTID_CROWDING_STORAGE
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

	Population::GaChromosomeTagGetter<Common::Grid::GaHyperBox> hyperBox( CTID_HYPERBOX, population.GetChromosomeTagManager() );

	for( int j = 0; j < population.GetCount() / 2; j++ )
	{
		for( int i = 0; i < 2; i++ )
		{
			const Population::GaChromosomeStorage& chromosome = population[ j * 2 + i ];

			float prob = chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_SCALED ).GetProbabilityBase();

			float x = ( (Problems::F1::F1Chromosome*)chromosome.GetChromosome().GetRawPtr() )->GetGene();

			float y1 = ( (Problems::F1::F1Fitness&)chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_RAW ) ).GetTypedValues()[ 0 ];
			float y2 = ( (Problems::F1::F1Fitness&)chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_RAW ) ).GetTypedValues()[ 1 ];

			printf( "(%5.2f = %5.2f, %5.2f | [%2d,%2d] %5.2f) ", x, y1, y2, hyperBox( chromosome )[ 0 ], hyperBox( chromosome )[ 1 ], prob );
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

		float gridSize[] = { 0.5, 0.5 };

		Algorithm::Stubs::GaPESAStub pesaStub( WDID_POPULATION, WDID_POPULATION_STATS, 
			Chromosome::GaInitializatorSetup( &initializator, NULL, &Chromosome::GaInitializatorConfig() ),
			Population::GaPopulationFitnessOperationSetup( &populationFitnessOperation, NULL, &Fitness::GaFitnessOperationConfig( &Fitness::Representation::GaMVFitnessParams( 2 ) ) ),
			Fitness::GaFitnessComparatorSetup( &fitnessComparator, &Fitness::Comparators::GaSimpleComparatorParams( Fitness::Comparators::GACT_MINIMIZE_ALL ), NULL ),
			Population::GaPopulationParams( 64, 0 ),
			Chromosome::GaMatingSetup( &mating, NULL, &matingConfiguration ),
			Population::GaCouplingSetup(),
			Population::SelectionOperations::GaTournamentSelectionParams( 32, PTID_CROSSOVER_BUFFER, -1, 2,
			Population::SelectionOperations::GaTournamentSelectionParams::GATST_ROULETTE_WHEEL_SELECTION ),
			Multiobjective::PESA::GaPESAParams( CTID_DOMINATED,	CTID_HYPERBOX, CTID_HYPERBOX_INFO, PTID_HYPERBOX_INFO_BUFFER, PTID_NONDOMINATED_BUFFER, PTID_CROWDING_STORAGE, false ),
			Common::Grid::GaHyperGrid<Fitness::GaFitness, float, Multiobjective::GaFitnessCoordiantesGetter<float> >( gridSize, 2 ) );

		Common::Workflows::GaWorkflow workflow( NULL );

		workflow.RemoveConnection( *workflow.GetFirstStep()->GetOutboundConnections().begin(), true );

		Common::Workflows::GaWorkflowBarrier* br1 = new Common::Workflows::GaWorkflowBarrier();
		pesaStub.Connect( workflow.GetFirstStep(), br1 );

		Common::Workflows::GaBranchGroup* bg1 = (Common::Workflows::GaBranchGroup*)workflow.ConnectSteps( br1, workflow.GetLastStep(), 0 );

		Algorithm::StopCriteria::GaStopCriterionStep* stopStep = new Algorithm::StopCriteria::GaStopCriterionStep( 
			Algorithm::StopCriteria::GaStopCriterionSetup( &stopCriterion, &Algorithm::StopCriteria::GaGenerationCriterionParams( 100 ), NULL ),
			workflow.GetWorkflowData(), WDID_POPULATION_STATS );

		Common::Workflows::GaBranchGroupTransition* bt1 = new Common::Workflows::GaBranchGroupTransition();

		bg1->GetBranchGroupFlow()->SetFirstStep( stopStep );
		bg1->GetBranchGroupFlow()->ConnectSteps( stopStep, bt1, 0 );
		workflow.ConnectSteps( bt1, pesaStub.GetStubFlow().GetFirstStep(), 1 );

		Common::Workflows::GaDataCache<Population::GaPopulation> population( workflow.GetWorkflowData(), WDID_POPULATION );

		population.GetData().GetEventManager().AddEventHandler( Population::GaPopulation::GAPE_NEW_GENERATION, &newGenHandler );

		workflow.Start();
		workflow.Wait();
	}

	GaFinalize();
}

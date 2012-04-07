
#include "..\..\source\Initialization.h"
#include "..\..\Problems\F1.h"
#include "..\..\source\Matings.h"
#include "..\..\source\PAESStub.h"
#include "..\..\source\StopCriteria.h"

enum WorkflowDataIDs
{
	WDID_POPULATION,
	WDID_POPULATION_STATS
};

enum ChromosomeTagIDs
{
	CTID_DOMINANCE,
	CTID_HYPERBOX,
	CTID_HYPERBOX_INFO
};

enum PopulationTagIDs
{
	PTID_CROSSOVER_BUFFER,
	PTID_CURRENT_SOLUTION,
	PTID_HYPERBOX_INFO_BUFFER,
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

	for( int i = 0; i < population.GetCount(); i++ )
	{
		const Population::GaChromosomeStorage& chromosome = population[ i ];

		float x = ( (Problems::F1::F1Chromosome*)chromosome.GetChromosome().GetRawPtr() )->GetGene();

		float y1 = ( (Problems::F1::F1Fitness&)chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_RAW ) ).GetTypedValues()[ 0 ];
		float y2 = ( (Problems::F1::F1Fitness&)chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_RAW ) ).GetTypedValues()[ 1 ];

		printf( "(%5.2f = %5.2f, %5.2f | [%2d,%2d]) ", x, y1, y2, hyperBox( chromosome )[ 0 ], hyperBox( chromosome )[ 1 ] );

		if( i % 2 == 1 )
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

		Algorithm::Stubs::GaPAESStub paesStub( WDID_POPULATION, WDID_POPULATION_STATS, 
			Chromosome::GaInitializatorSetup( &initializator, NULL, &Chromosome::GaInitializatorConfig() ),
			Population::GaPopulationFitnessOperationSetup( &populationFitnessOperation, NULL, &Fitness::GaFitnessOperationConfig( &Fitness::Representation::GaMVFitnessParams( 2 ) ) ),
			Fitness::GaFitnessComparatorSetup( &fitnessComparator, &Fitness::Comparators::GaSimpleComparatorParams( Fitness::Comparators::GACT_MINIMIZE_ALL ), NULL ),
			Population::GaPopulationParams( 33, 1 ),
			Chromosome::GaMutationSetup( &mutation, &Chromosome::GaMutationParams( 0.03f, false ), NULL ),
			Multiobjective::PAES::GaPAESSelectionParams( PTID_CURRENT_SOLUTION, PTID_CROSSOVER_BUFFER),
			Multiobjective::PAES::GaPAESParams( CTID_DOMINANCE, CTID_HYPERBOX, CTID_HYPERBOX_INFO, PTID_HYPERBOX_INFO_BUFFER, PTID_CURRENT_SOLUTION, PTID_CROWDING_STORAGE ),
			Common::Grid::GaHyperGrid<Fitness::GaFitness, float, Multiobjective::GaFitnessCoordiantesGetter<float> >( gridSize, 2 ) );

		Common::Workflows::GaWorkflow workflow( NULL );

		workflow.RemoveConnection( *workflow.GetFirstStep()->GetOutboundConnections().begin(), true );

		Common::Workflows::GaWorkflowBarrier* br1 = new Common::Workflows::GaWorkflowBarrier();
		paesStub.Connect( workflow.GetFirstStep(), br1 );

		Common::Workflows::GaBranchGroup* bg1 = (Common::Workflows::GaBranchGroup*)workflow.ConnectSteps( br1, workflow.GetLastStep(), 0 );

		Algorithm::StopCriteria::GaStopCriterionStep* stopStep = new Algorithm::StopCriteria::GaStopCriterionStep( 
			Algorithm::StopCriteria::GaStopCriterionSetup( &stopCriterion, &Algorithm::StopCriteria::GaGenerationCriterionParams( 1000 ), NULL ),
			workflow.GetWorkflowData(), WDID_POPULATION_STATS );

		Common::Workflows::GaBranchGroupTransition* bt1 = new Common::Workflows::GaBranchGroupTransition();

		bg1->GetBranchGroupFlow()->SetFirstStep( stopStep );
		bg1->GetBranchGroupFlow()->ConnectSteps( stopStep, bt1, 0 );
		workflow.ConnectSteps( bt1, paesStub.GetStubFlow().GetFirstStep(), 1 );

		Common::Workflows::GaDataCache<Population::GaPopulation> population( workflow.GetWorkflowData(), WDID_POPULATION );

		population.GetData().GetEventManager().AddEventHandler( Population::GaPopulation::GAPE_NEW_GENERATION, &newGenHandler );

		workflow.Start();
		workflow.Wait();
	}

	GaFinalize();
}

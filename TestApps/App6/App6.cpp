

#include "..\..\source\Initialization.h"
#include "..\..\Problems\F1.h"
#include "..\..\source\Matings.h"
#include "..\..\source\RDGAStub.h"
#include "..\..\source\StopCriteria.h"

enum WorkflowDataIDs
{
	WDID_POPULATION,
	WDID_POPULATION_STATS
};

enum ChromosomeTagIDs
{
	CTID_SELECTED,
	CTID_DOMINATION_COUNT,
	CTID_DOMINATION_LIST,
	CTID_RANK,
	CTID_HYPERBOX,
	CTID_HYPERBOX_INFO,
	CTID_NEXT_IN_FRONT
};

enum PopulationTagIDs
{
	PTID_CROSSOVER_BUFFER,
	PTID_BEST_INDIVIDUALS_MATRIX,
	PTID_UNRANKED_LIST,
	PTID_HYPERGRID,
	PTID_HYPERBOX_INFO_BUFFER,
	PTID_NEIGHBOUR_HYPERBOX
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
			float prob = 0;//chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_SCALED ).GetProbabilityBase();

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

		int gridSize[] = { 10, 10 };

		Algorithm::Stubs::GaRDGAStub rdgaStub( WDID_POPULATION, WDID_POPULATION_STATS, 
			Chromosome::GaInitializatorSetup( &initializator, NULL, &Chromosome::GaInitializatorConfig() ),
			Population::GaPopulationFitnessOperationSetup( &populationFitnessOperation, NULL, &Fitness::GaFitnessOperationConfig( &Fitness::Representation::GaMVFitnessParams( 2 ) ) ),
			Fitness::GaFitnessComparatorSetup( &fitnessComparator, &Fitness::Comparators::GaSimpleComparatorParams( Fitness::Comparators::GACT_MINIMIZE_ALL ), NULL ),
			Population::GaPopulationParams( 32, 8 ),
			Population::SelectionOperations::GaTournamentSelectionParams( 8, -1, CTID_SELECTED, 2,
			Population::SelectionOperations::GaTournamentSelectionParams::GATST_ROULETTE_WHEEL_SELECTION ),
			//Population::SelectionOperations::GaDuplicatesSelectionParams( 8, -1, CTID_SELECTED ),
			Chromosome::GaMatingSetup( &mating, NULL, &matingConfiguration ),
			Population::CouplingOperations::GaCellularCouplingParams( CTID_RANK, CTID_HYPERBOX, PTID_BEST_INDIVIDUALS_MATRIX, PTID_NEIGHBOUR_HYPERBOX, 8, PTID_CROSSOVER_BUFFER ),
			Multiobjective::RDGA::GaRDGAParams( 8, CTID_DOMINATION_COUNT, CTID_DOMINATION_LIST, CTID_RANK, CTID_HYPERBOX, CTID_HYPERBOX_INFO, CTID_NEXT_IN_FRONT,
			PTID_BEST_INDIVIDUALS_MATRIX, PTID_UNRANKED_LIST, PTID_HYPERGRID, PTID_HYPERBOX_INFO_BUFFER ),
			Common::Grid::GaAdaptiveGrid<Fitness::GaFitness, float, Multiobjective::GaFitnessCoordiantesGetter<float> >( gridSize, 2 ) );

		Common::Workflows::GaWorkflow workflow( NULL );

		workflow.RemoveConnection( *workflow.GetFirstStep()->GetOutboundConnections().begin(), true );

		Common::Workflows::GaWorkflowBarrier* br1 = new Common::Workflows::GaWorkflowBarrier();
		rdgaStub.Connect( workflow.GetFirstStep(), br1 );

		Common::Workflows::GaBranchGroup* bg1 = (Common::Workflows::GaBranchGroup*)workflow.ConnectSteps( br1, workflow.GetLastStep(), 0 );

		Algorithm::StopCriteria::GaStopCriterionStep* stopStep = new Algorithm::StopCriteria::GaStopCriterionStep( 
			Algorithm::StopCriteria::GaStopCriterionSetup( &stopCriterion, &Algorithm::StopCriteria::GaGenerationCriterionParams( 1000 ), NULL ),
			workflow.GetWorkflowData(), WDID_POPULATION_STATS );

		Common::Workflows::GaBranchGroupTransition* bt1 = new Common::Workflows::GaBranchGroupTransition();

		bg1->GetBranchGroupFlow()->SetFirstStep( stopStep );
		bg1->GetBranchGroupFlow()->ConnectSteps( stopStep, bt1, 0 );
		workflow.ConnectSteps( bt1, rdgaStub.GetStubFlow().GetFirstStep(), 1 );

		Common::Workflows::GaDataCache<Population::GaPopulation> population( workflow.GetWorkflowData(), WDID_POPULATION );

		population.GetData().GetEventManager().AddEventHandler( Population::GaPopulation::GAPE_NEW_GENERATION, &newGenHandler );

		workflow.Start();
		workflow.Wait();
	}

	GaFinalize();
}


#include "..\..\source\Initialization.h"
#include "..\..\Problems\XKCD.h"
#include "..\..\source\Crossovers.h"
#include "..\..\source\SimpleStub.h"
#include "..\..\source\Matings.h"
#include "..\..\source\PopulationStatistics.h"
#include "..\..\source\StopCriteria.h"

#include "..\..\source\Selections.h"
#include "..\..\source\Replacements.h"
#include "..\..\source\FitnessSharing.h"
#include "..\..\source\Scalings.h"
#include "..\..\source\StopCriteria.h"

#include <iostream>
#include <iomanip>

enum WorkflowDataIDs
{
	WDID_POPULATION,
	WDID_POPULATION_STATS
};

Problems::XKCD::XkcdInitializator initializator;
Chromosome::CrossoverOperations::GaListMultipointCrossover crossover;
Problems::XKCD::XkcdMutationOperation mutation;
Problems::XKCD::XkcdChromosomeComparator chromosomeComparator;
Problems::XKCD::XkcdFitnessOperation fitnessOperation;
Fitness::Comparators::GaSimpleComparator fitnessComparator;

Population::GaCombinedFitnessOperation populationFitnessOperation( &fitnessOperation );

Chromosome::MatingOperations::GaBasicMatingOperation mating;

Population::GaPopulationSizeTracker sizeTracker;
Population::GaRawFitnessTracker rawTracker;
Population::GaScaledFitnessTracker scaledTracker;

Population::SelectionOperations::GaRouletteWheelSelection selection;
Population::ReplacementOperations::GaWorstReplacement replacement;
Population::ScalingOperations::GaNoScaling scaling;

Algorithm::StopCriteria::GaStatsChangesCriterion stopCriterion;

void GACALL MyHandler(int id, Common::Observing::GaEventData& data)
{
	const Population::GaPopulation& population = ( (Population::GaPopulationEventData&)data ).GetPopulation();

	const Problems::XKCD::XkcdChromosome& chromosome = (const Problems::XKCD::XkcdChromosome&)*population[ 0 ].GetChromosome();
	const Problems::XKCD::XkcdConfigBlock& ccb = (const Problems::XKCD::XkcdConfigBlock&)*chromosome.GetConfigBlock();

	const Statistics::GaStatistics& stats = population.GetStatistics();

	if( population.GetStatistics().GetCurrentGeneration() != 1 && !population.GetStatistics().GetValue<Fitness::GaFitness>( Population::GADV_BEST_FITNESS ).IsChanged( 2 ) )
		return;

	const Common::Data::GaSingleDimensionArray<Problems::XKCD::XkcdConfigBlock::Appetizer>& appetizer = ccb.GetAppetizers();

	std::cout << "generation: " << population.GetStatistics().GetCurrentGeneration() << std::endl;
	std::cout << "-------------------------------------------------------------------------------\n";

	for( int i = 0; i < population.GetCount(); i++ )
	{
		const Problems::XKCD::XkcdChromosome& chromosome = (const Problems::XKCD::XkcdChromosome&)*population[ i ].GetChromosome();

		const Problems::XKCD::XkcdFitness f = (const Problems::XKCD::XkcdFitness&)population[ i ].GetFitness( Population::GaChromosomeStorage::GAFT_RAW );
		std::cout.setf( std::ios::fixed, std:: ios::floatfield );
		std::cout << '#' << std::setw( 2 ) << ( i + 1 ) << " price = " << std::setprecision( 3 ) << f.GetValue( 0 ) << ", time = " << f.GetValue( 1 )
			<< ", fitness = " << f.GetWeightedSum() << ", items: ";

		for ( const Problems::XKCD::XkcdGeneList::GaNodeType* node = chromosome.GetGenes().GetHead(); node != NULL; node = node->GetNext() )
			std::cout << ' ' << node->GetValue().GetValue();

		std::cout << std::endl;
	}

	std::cout << "-------------------------------------------------------------------------------\n";
}

Common::Observing::GaNonmemberEventHandler newGenHandler( MyHandler );

int main()
{
	GaInitialize();

	{
		Problems::XKCD::XkcdConfigBlock::Appetizer appetizers[] =
		{
			Problems::XKCD::XkcdConfigBlock::Appetizer("mixed fruit", 2.15f, 3),
			Problems::XKCD::XkcdConfigBlock::Appetizer("french fries", 2.75f, 2),
			Problems::XKCD::XkcdConfigBlock::Appetizer("side salad", 3.35f, 5),
			Problems::XKCD::XkcdConfigBlock::Appetizer("hot wings", 3.55f, 3),
			Problems::XKCD::XkcdConfigBlock::Appetizer("mozzarella sticks", 4.20f, 4),
			Problems::XKCD::XkcdConfigBlock::Appetizer("sampler plate", 5.80f, 7),
		};

		float targetPrice = 0;
		std::cout << "enter target price: ";
		std::cin >> targetPrice;

		Chromosome::GaMatingConfig matingConfiguration(
			Chromosome::GaCrossoverSetup( &crossover, &Chromosome::GaCrossoverPointParams( 0.8f, 2, 1 ), NULL ),
			Chromosome::GaMutationSetup( &mutation, &Chromosome::GaMutationSizeParams( 0.3f, true, 1L ), NULL ) );

		Chromosome::GaInitializatorSetup initializatorSetup( &initializator, NULL, &Chromosome::GaInitializatorConfig(
			&Problems::XKCD::XkcdConfigBlock( Common::Data::GaSingleDimensionArray<Problems::XKCD::XkcdConfigBlock::Appetizer>( appetizers, 6 ) ) ) );
		Fitness::GaFitnessComparatorSetup fitnessComparatorSetup( &fitnessComparator,
			&Fitness::Comparators::GaSimpleComparatorParams( Fitness::Comparators::GACT_MAXIMIZE_ALL ), NULL );

		Algorithm::Stubs::GaSimpleGAStub::GaStatTrackersCollection trackers;
		trackers[ Population::GaPopulationSizeTracker::TRACKER_ID ] =  &sizeTracker;
		trackers[ Population::GaRawFitnessTracker::TRACKER_ID ] =  &rawTracker;
		trackers[ Population::GaScaledFitnessTracker::TRACKER_ID ] =  &scaledTracker;

		Population::GaSelectionSetup selectionSetup( &selection, &Population::SelectionOperations::GaDuplicatesSelectionParams( 8, 1, 2 ),
			&Population::GaCouplingConfig( Chromosome::GaMatingSetup( &mating, NULL, &matingConfiguration ) ) );
		Population::GaReplacementSetup replacementSetup( &replacement, &Population::GaReplacementParams( 8 ),
			&Population::GaReplacementConfig( Chromosome::GaChromosomeComparatorSetup( &chromosomeComparator, NULL, NULL ) ) );
		Population::GaScalingSetup scalingSetup( &scaling, NULL, &Population::GaScalingConfig() );

		float fitnessWights[] = { 2.0f, 1.0f };

		Algorithm::Stubs::GaSimpleGAStub simpleGA( WDID_POPULATION, WDID_POPULATION_STATS,
			initializatorSetup,
			Population::GaPopulationFitnessOperationSetup( &populationFitnessOperation, &Problems::XKCD::XkcdFitnessOperationParams( targetPrice ),
			&Fitness::GaFitnessOperationConfig( &Fitness::Representation::GaWeightedFitnessParams<float>( fitnessWights, 2 ) ) ),
			fitnessComparatorSetup,
			Population::GaPopulationParams( 32, 0, Population::GaPopulationParams::GAPFO_FILL_ON_INIT ),
			trackers,
			Chromosome::GaMatingSetup(),
			selectionSetup,
			Population::GaCouplingSetup(),
			replacementSetup,
			scalingSetup,
			Population::GaFitnessComparatorSortingCriteria( fitnessComparatorSetup, Population::GaChromosomeStorage::GAFT_RAW ) );

		Common::Workflows::GaWorkflow workflow( NULL );

		workflow.RemoveConnection( *workflow.GetFirstStep()->GetOutboundConnections().begin(), true );

		Common::Workflows::GaWorkflowBarrier* br1 = new Common::Workflows::GaWorkflowBarrier();
		simpleGA.Connect( workflow.GetFirstStep(), br1 );

		Common::Workflows::GaBranchGroup* bg1 = (Common::Workflows::GaBranchGroup*)workflow.ConnectSteps( br1, workflow.GetLastStep(), 0 );

		Algorithm::StopCriteria::GaStopCriterionStep* stopStep = new Algorithm::StopCriteria::GaStopCriterionStep(
			Algorithm::StopCriteria::GaStopCriterionSetup( &stopCriterion,
			&Algorithm::StopCriteria::GaStatsChangesCriterionParams(
			Population::GADV_BEST_FITNESS, 100), NULL ), workflow.GetWorkflowData(), WDID_POPULATION_STATS );

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

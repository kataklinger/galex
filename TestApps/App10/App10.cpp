
#include "..\..\source\Initialization.h"
#include "..\..\Problems\BPP.h"
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
#include <vector>

enum WorkflowDataIDs
{
	WDID_POPULATION,
	WDID_POPULATION_STATS
};

Problems::BPP::BinInitializator initializator;
Problems::BPP::BinCrossoverOperation crossover;
Problems::BPP::BinMutationOperation mutation;
Problems::BPP::BinFitnessOperation fitnessOperation;
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

	const Problems::BPP::BinChromosome& chromosome = (const Problems::BPP::BinChromosome&)*population[ 0 ].GetChromosome();
	const Problems::BPP::BinConfigBlock& ccb = (const Problems::BPP::BinConfigBlock&)*chromosome.GetConfigBlock();

	const Statistics::GaStatistics& stats = population.GetStatistics();

	if( population.GetStatistics().GetCurrentGeneration() != 1 && !population.GetStatistics().GetValue<Fitness::GaFitness>( Population::GADV_BEST_FITNESS ).IsChanged( 2 ) )
		return;

	const Common::Data::GaSingleDimensionArray<Problems::BPP::BinConfigBlock::Item>& items = ccb.GetItems();

	std::cout << "generation: " << population.GetStatistics().GetCurrentGeneration() << std::endl;
	std::cout << "-------------------------------------------------------------------------------\n";

	for( int i = 0; i < population.GetCount(); i++ )
	{
		const Problems::BPP::BinChromosome& chromosome = (const Problems::BPP::BinChromosome&)*population[ i ].GetChromosome();

		const Problems::BPP::BinFitness f = (const Problems::BPP::BinFitness&)population[ i ].GetFitness( Population::GaChromosomeStorage::GAFT_RAW );
		std::cout.setf( std::ios::fixed, std:: ios::floatfield );
		std::cout << '#' << std::setw( 2 ) << ( i + 1 ) << " f(" << std::setprecision( 2 ) << f.GetValue()
			<< ") c(" << chromosome.GetStructure().GetCount() << ") ";

		//for( const Problems::BPP::BinList::GaNodeType* bNode = chromosome.GetGenes().GetHead(); bNode != NULL; bNode = bNode->GetNext() )
		//{
		//	std::cout << "{ ";

		//	for( const Problems::BPP::Bin::ItemList::GaNodeType* iNode = bNode->GetValue().GetItems().GetHead(); iNode != NULL; iNode = iNode->GetNext() )
		//		std::cout << items[ iNode->GetValue() ]._size << " " ;

		//	std::cout << "} ";
		//}

		std::cout << std::endl;
	}

	std::cout << "-------------------------------------------------------------------------------\n";
}

Common::Observing::GaNonmemberEventHandler newGenHandler( MyHandler );

int main()
{
	float binSize = 0.0f;
	std::cout << "enter bin size: ";
	std::cin >> binSize;

	float minItemSize = 0.0f;
	do
	{
		std::cout << "enter min. item size: ";
		std::cin >> minItemSize;
	} while ( minItemSize <= 0 || minItemSize >= binSize );

	float maxItemSize = 0.0f;
	do
	{
		std::cout << "enter max. item size: ";
		std::cin >> maxItemSize;
	} while ( minItemSize > maxItemSize || maxItemSize >= binSize );

	int itemCount = 0;
	do
	{
		std::cout << "enter number of items: ";
		std::cin >> itemCount;
	} while ( itemCount <= 0 );

	GaInitialize();

	{
		Common::Data::GaSingleDimensionArray<Problems::BPP::BinConfigBlock::Item> items( itemCount );
		for( int i = 0; i < itemCount; i++ )
			items[ i ] = Problems::BPP::BinConfigBlock::Item( std::string( "L" ), GaGlobalRandomFloatGenerator->Generate( minItemSize, maxItemSize ) );

		Chromosome::GaMatingConfig matingConfiguration(
			Chromosome::GaCrossoverSetup( &crossover, &Chromosome::GaCrossoverPointParams( 0.8f, 2, 1 ), NULL ),
			Chromosome::GaMutationSetup( &mutation, &Chromosome::GaMutationSizeParams( 0.3f, true, 1L ), NULL ) );

		Chromosome::GaInitializatorSetup initializatorSetup( &initializator, NULL, &Chromosome::GaInitializatorConfig(
			&Problems::BPP::BinConfigBlock( items, binSize ) ) );
		Fitness::GaFitnessComparatorSetup fitnessComparatorSetup( &fitnessComparator,
			&Fitness::Comparators::GaSimpleComparatorParams( Fitness::Comparators::GACT_MAXIMIZE_ALL ), NULL );

		Algorithm::Stubs::GaSimpleGAStub::GaStatTrackersCollection trackers;
		trackers[ Population::GaPopulationSizeTracker::TRACKER_ID ] =  &sizeTracker;
		trackers[ Population::GaRawFitnessTracker::TRACKER_ID ] =  &rawTracker;
		trackers[ Population::GaScaledFitnessTracker::TRACKER_ID ] =  &scaledTracker;

		Population::GaSelectionSetup selectionSetup( &selection, &Population::SelectionOperations::GaDuplicatesSelectionParams( 8, 1, 2 ),
			&Population::GaCouplingConfig( Chromosome::GaMatingSetup( &mating, NULL, &matingConfiguration ) ) );
		Population::GaReplacementSetup replacementSetup( &replacement, &Population::GaReplacementParams( 8 ), &Population::GaReplacementConfig() );
		Population::GaScalingSetup scalingSetup( &scaling, NULL, &Population::GaScalingConfig() );

		Algorithm::Stubs::GaSimpleGAStub simpleGA( WDID_POPULATION, WDID_POPULATION_STATS,
			initializatorSetup,
			Population::GaPopulationFitnessOperationSetup( &populationFitnessOperation, &Problems::BPP::BinFitnessOperationParams( 2 ),
			&Fitness::GaFitnessOperationConfig( NULL ) ),
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

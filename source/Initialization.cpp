
/*! \file Initialization.h
	\brief This file contains implementation of functions for initialization and finalization of the library.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#include "Initialization.h"
#include "SysInfo.h"
#include "Random.h"
#include "ThreadPool.h"

#include "Population.h"
#include "PopulationStatistics.h"

//#include "CrossoverOperations.h"
//#include "MutationOperations.h"
//#include "FitnessComparators.h"
//
//#include "Population.h"
//
//#include "SelectionOperations.h"
//#include "CouplingOperations.h"
//#include "ReplacementOperations.h"
//#include "ScalingOperations.h"
//
//#include "StopCriterias.h"

using namespace Common;
using namespace Common::Random;
using namespace Common::Threading;

using namespace Chromosome;
//using namespace Chromosome::CrossoverOperations;
//using namespace Chromosome::MutationOperations;
//using namespace Chromosome::FitnessComparators;

using namespace Population;
//using namespace Population::SelectionOperations;
//using namespace Population::CouplingOperations;
//using namespace Population::ReplacementOperations;
//using namespace Population::ScalingOperations;
//
//using namespace Algorithm;
//using namespace Algorithm::StopCriterias;


void GACALL GaInitialize()
{
	GaCpuInfo::MakeInstance();

	GaGlobalRandomIntegerGenerator = new GaRandom<int>();
	GaGlobalRandomLongIntegerGenerator = new GaRandom<long long>();
	GaGlobalRandomFloatGenerator = new GaRandom<float>();
	GaGlobalRandomDoubleGenerator = new GaRandom<double>();
	GaGlobalRandomBoolGenerator = new GaRandom<bool>();

	// catalogue of crossover operations
	GaCrossoverCatalogue::MakeInstance();
	//GaCrossoverCatalogue::GetInstance().Register( "GaAddCrossover", new GaAddCrossover() );
	//GaCrossoverCatalogue::GetInstance().Register( "GaMidpointCrossover", new GaMidpointCrossover() );
	//GaCrossoverCatalogue::GetInstance().Register( "GaMultiValueCrossover", new GaMultiValueCrossover() );
	//GaCrossoverCatalogue::GetInstance().Register( "GaSubCrossover", new GaSubCrossover() );

	// catalogue of mutation operations
	GaMutationCatalogue::MakeInstance();
	//GaMutationCatalogue::GetInstance().Register( "GaFlipMutation", new GaFlipMutation() );
	//GaMutationCatalogue::GetInstance().Register( "GaInvertMutation", new GaInvertMutation() );
	//GaMutationCatalogue::GetInstance().Register( "GaSwapMutation", new GaSwapMutation() );
/*
	// catalogue of mutation operations
	GaFitnessComparatorCatalogue::MakeInstance();
	GaFitnessComparatorCatalogue::GetInstance().Register( "GaMinFitnessComparator", new GaMinFitnessComparator() );
	GaFitnessComparatorCatalogue::GetInstance().Register( "GaMaxFitnessComparator", new GaMaxFitnessComparator() );

	// catalogue of selection operations
	GaSelectionCatalogue::MakeInstance();
	GaSelectionCatalogue::GetInstance().Register( "GaSelectBest", new GaSelectBest() );
	GaSelectionCatalogue::GetInstance().Register( "GaSelectRandom", new GaSelectRandom() );
	GaSelectionCatalogue::GetInstance().Register( "GaSelectRandomBest", new GaSelectRandomBest() );
	GaSelectionCatalogue::GetInstance().Register( "GaSelectRouletteWheel", new GaSelectRouletteWheel() );
	GaSelectionCatalogue::GetInstance().Register( "GaSelectTournament", new GaSelectTournament() );
	GaSelectionCatalogue::GetInstance().Register( "GaSelectWorst", new GaSelectWorst() );

	// catalogue of coupling operations
	GaCouplingCatalogue::MakeInstance();
	GaCouplingCatalogue::GetInstance().Register( "GaBestAlwaysCoupling", new GaBestAlwaysCoupling() );
	GaCouplingCatalogue::GetInstance().Register( "GaCrossCoupling", new GaCrossCoupling() );
	GaCouplingCatalogue::GetInstance().Register( "GaInverseCoupling", new GaInverseCoupling() );
	GaCouplingCatalogue::GetInstance().Register( "GaRandomCoupling", new GaRandomCoupling() );
	GaCouplingCatalogue::GetInstance().Register( "GaSimpleCoupling", new GaSimpleCoupling() );

	// catalogue of replacement operations
	GaReplacementCatalogue::MakeInstance();
	GaReplacementCatalogue::GetInstance().Register( "GaReplaceBest", new GaReplaceBest() );
	GaReplacementCatalogue::GetInstance().Register( "GaReplaceParents", new GaReplaceParents() );
	GaReplacementCatalogue::GetInstance().Register( "GaReplaceRandom", new GaReplaceRandom() );
	GaReplacementCatalogue::GetInstance().Register( "GaReplaceWorst", new GaReplaceWorst() );

	// catalogue of scaling operations
	GaScalingCatalogue::MakeInstance();
	GaScalingCatalogue::GetInstance().Register( "GaExponentialScaling", new GaExponentialScaling() );
	GaScalingCatalogue::GetInstance().Register( "GaLinearScaling", new GaLinearScaling() );
	GaScalingCatalogue::GetInstance().Register( "GaNormalizationScaling", new GaNormalizationScaling() );
	GaScalingCatalogue::GetInstance().Register( "GaWindowScaling", new GaWindowScaling() );

	// catalogue of stop criterias for genetica algorithms
	GaStopCriteriaCatalogue::MakeInstance();
	GaStopCriteriaCatalogue::GetInstance().Register( "GaFitnessCriteria", new GaFitnessCriteria() );
	GaStopCriteriaCatalogue::GetInstance().Register( "GaFitnessProgressCriteria", new GaFitnessProgressCriteria() );
	GaStopCriteriaCatalogue::GetInstance().Register( "GaGenerationCriteria", new GaGenerationCriteria() );

	// default setup for population operations
	GaPopulationConfiguration::MakeDefault();
*/
	// initialize thread and event pool
	Common::Threading::GaEventPool::MakeInstance( 20, 20 );
	Common::Threading::GaThreadPool::MakeInstance( 10 );

	Population::GaDefaultValueHistoryFactory::MakeInstance();
}

void GACALL GaFinalize()
{
	Population::GaDefaultValueHistoryFactory::FreeInstance();

	GaThreadPool::FreeInstance();
	GaEventPool::FreeInstance();

	delete GaGlobalRandomIntegerGenerator;
	delete GaGlobalRandomLongIntegerGenerator;
	delete GaGlobalRandomFloatGenerator;
	delete GaGlobalRandomDoubleGenerator;
	delete GaGlobalRandomBoolGenerator;

	GaCrossoverCatalogue::FreeInstance();
	GaMutationCatalogue::FreeInstance();
	//GaFitnessComparatorCatalogue::FreeInstance();

	//GaSelectionCatalogue::FreeInstance();
	//GaCouplingCatalogue::FreeInstance();
	//GaReplacementCatalogue::FreeInstance();
	//GaScalingCatalogue::FreeInstance();

	//GaStopCriteriaCatalogue::FreeInstance();

	//GaPopulationConfiguration::FreeDefault();

	GaCpuInfo::FreeInstance();
}

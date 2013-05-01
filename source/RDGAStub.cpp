
/*! \file RDGAStub.cpp
    \brief This file conatins implementation of classes that implement algorithm stubs for Rank-Desity based Genetic Algorithm (RDGA).
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#include "RDGAStub.h"

namespace Algorithm
{
	namespace Stubs
	{

		// Initailizes RDGA stub
		GaRDGAStub::GaRDGAStub(int populationID,
			int statisticsID,
			const Chromosome::GaInitializatorSetup& initializator,
			const Population::GaPopulationFitnessOperationSetup& fitnessOperation,
			const Fitness::GaFitnessComparatorSetup& comparator,
			const Population::GaPopulationParams& populationParameters,
			const Population::SelectionOperations::GaDuplicatesSelectionParams& selectionParameters,
			const Chromosome::GaMatingSetup& mating,
			const Population::CouplingOperations::GaCellularCouplingParams& couplingParameters,
			const Multiobjective::RDGA::GaRDGAParams& rdgaParameters,
			const Multiobjective::GaFitnessAdaptiveGrid& grid) : GaBasicStub(populationID, statisticsID)
		{
			// stores operations and their parameters
			SetPopulationParameters( populationParameters );
			SetInitializator( initializator );
			SetFitnessOperation( fitnessOperation );
			SetFitnessComparator( comparator );
			SetMating( mating );
			SetSelectionParameters( selectionParameters );
			SetCouplingParameters( couplingParameters );
			SetRDGAParameters( rdgaParameters );
			SetGrid( grid );

			Clear();
		}

		// Sets population parameters
		void GaRDGAStub::SetPopulationParameters(Population::GaPopulationParams params)
		{
			params.GetFillOptions().CopyAllFlags( Population::GaPopulationParams::GAPFO_FILL_ON_INIT );

			// store new parameters
			UpdatePopulationParameters( params );
		}

		// Sets fitness operation 
		void GaRDGAStub::SetFitnessOperation(const Population::GaPopulationFitnessOperationSetup& fitnessOperation)
		{
			// disconnect or connect fitness step depanding on whether the new fitness operation does not require re-evaluation of whole population in each generation
			bool useFitnessStep = !fitnessOperation.GetOperation().AllowsIndividualEvaluation();
			if( useFitnessStep != UseFitnessStep() )
			{
				Common::Workflows::GaBranchGroupFlow* flow = _brachGroup->GetBranchGroupFlow();
				flow->RemoveConnection( _fitnessConnection, true );
				_fitnessConnection = useFitnessStep ? flow->ConnectSteps( _nopStep, _fitnessStep, 0 ) : flow->ConnectSteps( _nopStep, _replacementStep, 0 );
			}

			// store fitness operation
			UpdateFitnessOperation( fitnessOperation );
		}

		// Sets mating operation
		void GaRDGAStub::SetMating(const Chromosome::GaMatingSetup& mating)
		{
			_mating = mating;

			// update coupling operation setup stored in flow step if stub is connected
			if( IsConnected() )
			{
				Population::GaCouplingSetup setup = _couplingStep->GetSetup();
				setup.SetConfiguration( &Population::GaCouplingConfig( _mating ) );
				_couplingStep->SetSetup( setup );
			}
		}

		// Sets parameters of the selection operation
		void GaRDGAStub::SetSelectionParameters(Population::SelectionOperations::GaDuplicatesSelectionParams/*GaTournamentSelectionParams*/ params)
		{
			params.SetCrossoverBuffersTagID( -1 );

			// update selection operation setup stored in flow step if stub is connected
			if( IsConnected() )
			{
				Population::GaSelectionSetup setup = _selectionStep->GetSetup();
				setup.SetParameters( &params );
				_selectionStep->SetSetup( setup );
			}

			_selectionParameters = params;
		}

		// Sets parameters for coupling operation
		void GaRDGAStub::SetCouplingParameters(const Population::CouplingOperations::GaCellularCouplingParams& params)
		{
			_couplingParameters = params;

			// update coupling and replacement operations setups stored in flow steps if stub is connected
			if( IsConnected() )
			{
				Population::GaCouplingSetup couplingSetup = _couplingStep->GetSetup();
				couplingSetup.SetParameters( &_couplingParameters );
				_couplingStep->SetSetup( couplingSetup );

				Population::GaReplacementSetup replacementSetup = _replacementStep->GetSetup();
				replacementSetup.GetParameters().SetReplacementSize( params.GetNumberOfOffsprings() );
				_replacementStep->SetSetup( replacementSetup );

			}
		}

		// Sets RDGA parameters
		void GaRDGAStub::SetRDGAParameters(const Multiobjective::RDGA::GaRDGAParams& params)
		{
			_rdgaParameters = params;

			// update operation setup stored in flow step if stub is connected
			if( IsConnected() )
			{
				Population::GaReplacementSetup setup = _replacementStep->GetSetup();
				setup.SetParameters( &_rdgaParameters );
				_replacementStep->SetSetup( setup );
			}
		}

		// Sets adaptive grid that splits fitness hyperscape
		void GaRDGAStub::SetGrid(const Multiobjective::GaFitnessAdaptiveGrid& grid)
		{
			_grid = grid.Clone();

			// update RDGA operation setup stored in flow step if stub is connected
			if( IsConnected() )
			{
				Population::GaReplacementSetup setup = _replacementStep->GetSetup();
				( (Multiobjective::RDGA::GaRDGAConfig&)setup.GetConfiguration() ).SetAdaptiveGrid( *_grid );
				_replacementStep->SetSetup( setup );
			}
		}

		// Connects stub to the workflow
		void GaRDGAStub::Connected()
		{
			GaBasicStub::Connected();

			GaCachedPopulation population( GetWorkflowStorage(), _populationID );

			// register statistics trackers required by RDGA
			population.GetData().RegisterTracker( Population::GaPopulationSizeTracker::TRACKER_ID, &_sizeTracker );

			// create data objects required by the RDGA and insert them to workflow data storage 

			Population::GaChromosomeGroup* selectionOutput = new Population::GaChromosomeGroup();
			Population::GaChromosomeGroup* coulingOutput = new Population::GaChromosomeGroup();

			Common::Workflows::GaDataStorage* bgStorage = _brachGroup->GetData();
			bgStorage->AddData( new Common::Workflows::GaDataEntry<Population::GaChromosomeGroup>( GADID_SELECTION_OUTPUT, selectionOutput ), Common::Workflows::GADSL_BRANCH_GROUP );
			bgStorage->AddData( new Common::Workflows::GaDataEntry<Population::GaChromosomeGroup>( GADID_COUPLING_OUTPUT, coulingOutput ), Common::Workflows::GADSL_BRANCH_GROUP );

			// create flow steps that 

			_checkStep = new GaCheckPopulationStep( GetWorkflowStorage(), _populationID );
			_initStep = new Common::Workflows::GaSimpleMethodExecStep<Population::GaPopulation, Common::Workflows::GaMethodExecIgnoreBranch<Population::GaPopulation> >
				( &Population::GaPopulation::Initialize, GetWorkflowStorage(), _populationID );

			_selectionStep = new Population::GaSelectionStep( 
				Population::GaSelectionSetup( &_selectionOperation, &_selectionParameters, &Population::SelectionOperations::GaTournamentSelectionConfig(
				Fitness::GaFitnessComparatorSetup( &_scaledFitnessComparator, &Fitness::Comparators::GaSimpleComparatorParams(), NULL ), _mating ) ),
				GetWorkflowStorage(), _populationID, bgStorage, GADID_SELECTION_OUTPUT );

			_couplingStep = new Population::GaCouplingStep( Population::GaCouplingSetup( &_couplingOperation, &_couplingParameters, &Population::GaCouplingConfig( _mating ) ),
				bgStorage, GADID_SELECTION_OUTPUT, bgStorage, GADID_COUPLING_OUTPUT );

			_nopStep = new Common::Workflows::GaNopStep();

			_fitnessStep = new Population::GaPopulationFitnessStep( GetWorkflowStorage(), _populationID );

			_replacementStep = new Population::GaReplacementStep( Population::GaReplacementSetup( &_rdgaOperation, &_rdgaParameters, &Multiobjective::RDGA::GaRDGAConfig( *_grid ) ),
				bgStorage, GADID_COUPLING_OUTPUT, GetWorkflowStorage(), _populationID );

			_nextGenStep = new Common::Workflows::GaSimpleMethodExecStep<Population::GaPopulation>( &Population::GaPopulation::NextGeneration, GetWorkflowStorage(), _populationID );

			Common::Workflows::GaBranchGroupFlow* flow = _brachGroup->GetBranchGroupFlow();

			// connect created flow steps 

			flow->SetFirstStep( _checkStep );
			flow->ConnectSteps( _checkStep, _selectionStep, 1 );
			flow->ConnectSteps( _selectionStep, _couplingStep, 0 );
			flow->ConnectSteps( _couplingStep, _nopStep, 0 );

			flow->ConnectSteps( _fitnessStep, _replacementStep, 0 );

			flow->ConnectSteps( _replacementStep, _nextGenStep, 0 );
			
			// do not use fitness step if population does not have to be re-evaluated in each generation
			_fitnessConnection = UseFitnessStep() ? flow->ConnectSteps( _nopStep, _fitnessStep, 0 ) : flow->ConnectSteps( _nopStep, _replacementStep, 0 );

			flow->ConnectSteps( _checkStep, _initStep, 0 );
			flow->ConnectSteps( _initStep, _nopStep, 0 );
		}

		// Disconnects stub from the workflow
		void GaRDGAStub::Disconnecting()
		{
			Common::Workflows::GaBranchGroupFlow* flow = _brachGroup->GetBranchGroupFlow();

			// disconnect and destroy flow steps created by the stub

			flow->RemoveStep( _checkStep, true, true );
			flow->RemoveStep( _initStep, true, true );
			flow->RemoveStep( _selectionStep, true, true );
			flow->RemoveStep( _couplingStep, true, true );
			flow->RemoveStep( _replacementStep, true, true );
			flow->RemoveStep( _nopStep, true, true );
			flow->RemoveStep( _fitnessStep, true, true );
			flow->RemoveStep( _nextGenStep, true, true );

			// clear internal bookkeeping
			Clear();

			// remove data from the workflow storage
			Common::Workflows::GaDataStorage* bgStorage = _brachGroup->GetData();
			bgStorage->RemoveData( GADID_SELECTION_OUTPUT, Common::Workflows::GADSL_BRANCH_GROUP );
			bgStorage->RemoveData( GADID_COUPLING_OUTPUT, Common::Workflows::GADSL_BRANCH_GROUP );

			GaCachedPopulation population( GetWorkflowStorage(), _populationID );

			// remove statistics trackers
			population.GetData().UnregisterTracker( Population::GaPopulationSizeTracker::TRACKER_ID );
			population.GetData().UnregisterTracker( Population::GaScaledFitnessTracker::TRACKER_ID );

			GaBasicStub::Disconnecting();
		}

		// Clears internal bookkeeping.
		void GaRDGAStub::Clear()
		{
			// clear pointers that cache flow connections used by stub
			_fitnessConnection = NULL;

			// clear pointers that cache flow steps used by stub
			_checkStep = NULL;
			_initStep = NULL;
			_selectionStep = NULL;
			_couplingStep = NULL;
			_replacementStep = NULL;
			_nopStep = NULL;
			_fitnessStep = NULL;
			_nextGenStep = NULL;
		}

	} // Stubs
} // Algorithm

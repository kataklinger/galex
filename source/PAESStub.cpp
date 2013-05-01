
/*! \file PAESStub.cpp
    \brief This file conatins implementation of classes that implement algorithm stub for Pareto Archived Evolution Strategy (PAES).
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#include "PAESStub.h"

namespace Algorithm
{
	namespace Stubs
	{

		// Initailizes PESA stub
		GaPAESStub::GaPAESStub(int populationID,
			int statisticsID,
			const Chromosome::GaInitializatorSetup& initializator,
			const Population::GaPopulationFitnessOperationSetup& fitnessOperation,
			const Fitness::GaFitnessComparatorSetup& comparator,
			const Population::GaPopulationParams& populationParameters,
			const Chromosome::GaMutationSetup& mutation,
			const Multiobjective::PAES::GaPAESSelectionParams& selectionParameters,
			const Multiobjective::PAES::GaPAESParams& paesParameters,
			const Multiobjective::GaFitnessHyperGrid& grid) : GaBasicStub(populationID, statisticsID)
		{
			// stores operations and their parameters
			SetPopulationParameters( populationParameters );
			SetInitializator( initializator );
			SetFitnessOperation( fitnessOperation );
			SetFitnessComparator( comparator );
			SetMutation( mutation );
			SetSelectionParameters( selectionParameters );
			SetPAESParameters( paesParameters );
			SetGrid( grid );

			Clear();
		}

		// Sets population parameters
		void GaPAESStub::SetPopulationParameters(Population::GaPopulationParams params)
		{
			params.GetFillOptions().CopyAllFlags( 0 );

			// update setups of the operations stored in flow steps if stub is connected
			if( IsConnected() )
			{
				Population::GaSelectionSetup setup = _selectionStep->GetSetup();
				setup.GetParameters().SetSelectionSize( params.GetPopulationSize() );
				_selectionStep->SetSetup( setup );
			}

			// store new parameters
			UpdatePopulationParameters( params );
		}

		// Sets mutation operation that produces new chromosome
		void GaPAESStub::SetMutation(const Chromosome::GaMutationSetup& mutation)
		{
			_mutation = mutation;

			// update selection operation setup stored in flow step if stub is connected
			if( IsConnected() )
			{
				Population::GaSelectionSetup setup = _selectionStep->GetSetup();
				setup.GetConfiguration().GetMating().SetConfiguration( &Chromosome::GaMatingConfig( Chromosome::GaCrossoverSetup( &_crossover, NULL, NULL ), _mutation ) );
				_selectionStep->SetSetup( setup );
			}
		}

		// Sets parameters of the selection operation
		inline void GaPAESStub::SetSelectionParameters(Multiobjective::PAES::GaPAESSelectionParams params)
		{
			_selectionParameters = params;

			// update selection operation setup stored in flow step if stub is connected
			if( IsConnected() )
			{
				Population::GaSelectionSetup setup = _selectionStep->GetSetup();
				setup.SetParameters( &params );
				_selectionStep->SetSetup( setup );
			}
		}

		// Sets PAES parameters
		void GaPAESStub::SetPAESParameters(const Multiobjective::PAES::GaPAESParams& params)
		{
			_paesParameters = params;

			// update operation setup stored in flow step if stub is connected
			if( IsConnected() )
			{
				Population::GaReplacementSetup setup = _replacementStep->GetSetup();
				setup.SetParameters( &_paesParameters );
				_replacementStep->SetSetup( setup );
			}
		}

		// Sets grid that splits fitness hyperscape
		void GaPAESStub::SetGrid(const Multiobjective::GaFitnessHyperGrid& grid)
		{
			_grid = grid.Clone();

			// update PAES operation setup stored in flow step if stub is connected
			if( IsConnected() )
			{
				Population::GaReplacementSetup setup = _replacementStep->GetSetup();
				( (Multiobjective::PAES::GaPAESConfig&)setup.GetConfiguration() ).SetGrid( *_grid );
				_replacementStep->SetSetup( setup );
			}
		}

		// Connects stub to the workflow
		void GaPAESStub::Connected()
		{
			GaBasicStub::Connected();

			GaCachedPopulation population( GetWorkflowStorage(), _populationID );

			// create data objects required by the SPEA and insert them to workflow data storage 
			Population::GaChromosomeGroup* selectionOutput = new Population::GaChromosomeGroup();

			Common::Workflows::GaDataStorage* bgStorage = _brachGroup->GetData();
			bgStorage->AddData( new Common::Workflows::GaDataEntry<Population::GaChromosomeGroup>( GADID_SELECTION_OUTPUT, selectionOutput ), Common::Workflows::GADSL_BRANCH_GROUP );

			// create flow steps that 

			_checkStep = new GaCheckPopulationStep( GetWorkflowStorage(), _populationID );
			_initStep = new Common::Workflows::GaSimpleMethodExecStep<Population::GaPopulation, Common::Workflows::GaMethodExecIgnoreBranch<Population::GaPopulation> >
				( &Population::GaPopulation::Initialize, GetWorkflowStorage(), _populationID );

			_selectionStep = new Population::GaSelectionStep( Population::GaSelectionSetup( &_selectionOperation, &_selectionParameters,
				&Population::GaCouplingConfig( 
				Chromosome::GaMatingSetup( &_matingOperation, NULL, &Chromosome::GaMatingConfig( Chromosome::GaCrossoverSetup( &_crossover, NULL, NULL ), _mutation ) ) ) ),
				GetWorkflowStorage(), _populationID, bgStorage, GADID_SELECTION_OUTPUT );

			_replacementStep = new Population::GaReplacementStep( Population::GaReplacementSetup( &_paesOperation, &_paesParameters, &Multiobjective::PAES::GaPAESConfig( *_grid ) ), 
				bgStorage, GADID_SELECTION_OUTPUT, GetWorkflowStorage(), _populationID );

			_nextGenStep = new Common::Workflows::GaSimpleMethodExecStep<Population::GaPopulation>( &Population::GaPopulation::NextGeneration, GetWorkflowStorage(), _populationID );

			Common::Workflows::GaBranchGroupFlow* flow = _brachGroup->GetBranchGroupFlow();

			// connect created flow steps 

			flow->SetFirstStep( _checkStep );
			flow->ConnectSteps( _checkStep, _selectionStep, 1 );
			flow->ConnectSteps( _selectionStep, _replacementStep, 0 );
			flow->ConnectSteps( _replacementStep, _nextGenStep, 0 );

			flow->ConnectSteps( _checkStep, _initStep, 0 );
			flow->ConnectSteps( _initStep, _nextGenStep, 0 );
		}

		// Disconnects stub from the workflow
		void GaPAESStub::Disconnecting()
		{
			Common::Workflows::GaBranchGroupFlow* flow = _brachGroup->GetBranchGroupFlow();

			// disconnect and destroy flow steps created by the stub

			flow->RemoveStep( _checkStep, true, true );
			flow->RemoveStep( _initStep, true, true );
			flow->RemoveStep( _selectionStep, true, true );
			flow->RemoveStep( _replacementStep, true, true );
			flow->RemoveStep( _nextGenStep, true, true );

			// clear internal bookkeeping
			Clear();

			// remove data from the workflow storage
			Common::Workflows::GaDataStorage* bgStorage = _brachGroup->GetData();
			bgStorage->RemoveData( GADID_SELECTION_OUTPUT, Common::Workflows::GADSL_BRANCH_GROUP );
			bgStorage->RemoveData( GADID_COUPLING_OUTPUT, Common::Workflows::GADSL_BRANCH_GROUP );

			GaBasicStub::Disconnecting();
		}

		// Clears internal bookkeeping.
		void GaPAESStub::Clear()
		{
			// clear pointers that cache flow steps used by stub
			_checkStep = NULL;
			_initStep = NULL;
			_selectionStep = NULL;
			_replacementStep = NULL;
			_nextGenStep = NULL;
		}

	} // Stubs
} // Algorithm

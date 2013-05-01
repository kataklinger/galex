
/*! \file SPEAStub.cpp
    \brief This file conatins implementation of classes that implement algorithm stubs for Strength Pareto Evolutionary Algorithm I and II (SPEA-I and SPEA-II).
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#include "SPEAStub.h"

namespace Algorithm
{
	namespace Stubs
	{

		// Initializes SPEA basic stub
		GaSPEAStubBase::GaSPEAStubBase(int populationID,
			int statisticsID,
			const Chromosome::GaInitializatorSetup& initializator,
			const Population::GaPopulationFitnessOperationSetup& fitnessOperation,
			const Fitness::GaFitnessComparatorSetup& comparator,
			const Population::GaPopulationParams& populationParameters,
			const Chromosome::GaMatingSetup& mating,
			const Population::GaCouplingSetup& coupling,
			const Population::SelectionOperations::GaTournamentSelectionParams& selectionParameters) : GaBasicStub(populationID, statisticsID)
		{
			// stores operations and their parameters
			SetPopulationParameters( populationParameters );
			SetInitializator( initializator );
			SetFitnessOperation( fitnessOperation );
			SetFitnessComparator( comparator );
			SetMating( mating );
			SetSelectionParameters( selectionParameters );
			SetCoupling( coupling );

			Clear();
		}

		// Sets population parameters
		void GaSPEAStubBase::SetPopulationParameters(Population::GaPopulationParams params)
		{
			// use half of the population as crowding space
			if( params.GetPopulationSize() % 2 != 0 )
				params.SetPopulationSize( params.GetPopulationSize() + 1 );
			params.SetCrowdingSize( params.GetPopulationSize() / 2 );

			params.GetFillOptions().CopyAllFlags( Population::GaPopulationParams::GAPFO_FILL_ON_INIT );

			// correcting number of offspring that should be produced

			if( !IsCouplingUsed() )
				_selectionParameters.SetSelectionSize( params.GetCrowdingSize() );
			if( _couplingSetup.HasParameters() )
				_couplingSetup.GetParameters().SetNumberOfOffsprings( _populationParameters.GetCrowdingSize() );

			// update setups of the operations stored in flow steps if stub is connected
			if( IsConnected() )
			{
				if( !IsCouplingUsed() )
				{
					// updates number of chromosomes that should be produced by selection operation
					Population::GaSelectionSetup setup = _selectionStep->GetSetup();
					setup.GetParameters().SetSelectionSize( params.GetCrowdingSize() );
					_selectionStep->SetSetup( setup );
				}
				else
				{
					// updates number of chromosomes that should be produced by coupling operation
					_couplingSetup.GetParameters().SetNumberOfOffsprings( params.GetCrowdingSize() );
					_couplingStep->SetSetup( _couplingSetup );
				}

				// update setup of replacement operation
				Population::GaReplacementSetup setup = _replacementStep->GetSetup();
				setup.GetParameters().SetReplacementSize( params.GetCrowdingSize() );
				_replacementStep->SetSetup( setup );
			}

			// store new parameters
			UpdatePopulationParameters( params );
		}

		// Sets fitness operation 
		void GACALL GaSPEAStubBase::SetFitnessOperation(const Population::GaPopulationFitnessOperationSetup& fitnessOperation)
		{
			// disconnect or connect fitness step depanding on whether the new fitness operation does not require re-evaluation of whole population in each generation
			bool useFitnessStep = !fitnessOperation.GetOperation().AllowsIndividualEvaluation();
			if( useFitnessStep != UseFitnessStep() )
			{
				Common::Workflows::GaBranchGroupFlow* flow = _brachGroup->GetBranchGroupFlow();
				flow->RemoveConnection( _fitnessConnection, true );
				_fitnessConnection = useFitnessStep ? flow->ConnectSteps( _replacementStep, _fitnessStep, 0 ) : flow->ConnectSteps( _replacementStep, _scalingStep, 0 );
			}

			// store fitness operation
			UpdateFitnessOperation( fitnessOperation );
		}

		// Sets mating operation
		void GaSPEAStubBase::SetMating(const Chromosome::GaMatingSetup& mating)
		{
			_mating = mating;

			// update selection operation setup stored in flow step if stub is connected
			if( IsConnected() )
			{
				Population::GaSelectionSetup setup = _selectionStep->GetSetup();
				setup.SetConfiguration( &Population::GaCouplingConfig( _mating ) );
				_selectionStep->SetSetup( setup );
			}

			// update coupling setup to use new mating operation
			if( _couplingSetup.HasConfiguration() )
			{
				_couplingSetup.GetConfiguration().SetMating( _mating );

				// update couling operation setup stored in flow step if stub is connected
				if( IsConnected() )
					_couplingStep->SetSetup( _couplingSetup );
			}
		}

		// Sets parameters of the selection operation
		void GaSPEAStubBase::SetSelectionParameters(Population::SelectionOperations::GaTournamentSelectionParams params)
		{
			// decide whether the coupling operation should be used
			bool useCoupling =  params.GetCrossoverBuffersTagID() < 0;
			bool changeConnection = useCoupling != IsCouplingUsed();

			// correcting number of offspring that should be produced
			if( !useCoupling )
				params.SetSelectionSize( _populationParameters.GetCrowdingSize() );

			if( IsConnected() )
			{
				// update selection operation setup stored in flow step if stub is connected
				Population::GaSelectionSetup setup = _selectionStep->GetSetup();
				setup.SetParameters( &params );
				_selectionStep->SetSetup( setup );

				if( changeConnection )
				{
					Common::Workflows::GaBranchGroupFlow* flow = _brachGroup->GetBranchGroupFlow();

					// connect or disconnect couling step
					flow->RemoveConnection( _couplingConnection, true );
					_couplingConnection = useCoupling ? flow->ConnectSteps( _selectionStep, _couplingStep, 0 ) : flow->ConnectSteps( _selectionStep, _replacementStep , 0 );
					_replacementStep->SetInputData( _brachGroup->GetData(), useCoupling ? GADID_COUPLING_OUTPUT : GADID_SELECTION_OUTPUT );
				}
			}

			_selectionParameters = params;
		}

		// Sets coupling operation
		void GaSPEAStubBase::SetCoupling(const Population::GaCouplingSetup& coupling)
		{
			_couplingSetup = coupling;

			// correcting number of offspring that should be produced
			if( _couplingSetup.HasParameters() )
				_couplingSetup.GetParameters().SetNumberOfOffsprings( _populationParameters.GetCrowdingSize() );

			// update coupling operation setup stored in flow step if stub is connected
			if( IsConnected() )
			{
				if( coupling.HasConfiguration() )
				{
					// extract and store mating operation from the coupling configuration
					_mating = coupling.GetConfiguration().GetMating();

					Population::GaSelectionSetup setup = _selectionStep->GetSetup();
					setup.SetConfiguration( &Population::GaCouplingConfig( _mating ) );
					_selectionStep->SetSetup( setup );
				}

				_couplingStep->SetSetup( _couplingSetup );
			}
		}

		// Connects stub to the workflow
		void GaSPEAStubBase::Connected()
		{
			GaBasicStub::Connected();

			GaCachedPopulation population( GetWorkflowStorage(), _populationID );

			// register statistics trackers required by SPEA
			population.GetData().RegisterTracker( Population::GaPopulationSizeTracker::TRACKER_ID, &_sizeTracker );
			population.GetData().RegisterTracker( Population::GaScaledFitnessTracker::TRACKER_ID, &_scaledTracker );

			// create data objects required by the SPEA and insert them to workflow data storage 
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

			_couplingStep = new Population::GaCouplingStep( _couplingSetup, bgStorage, GADID_SELECTION_OUTPUT, bgStorage, GADID_COUPLING_OUTPUT );

			_replacementStep = new Population::GaReplacementStep(
				Population::GaReplacementSetup( &_replacementOperation, &Population::GaReplacementParams( _populationParameters.GetCrowdingSize() ), &Population::GaReplacementConfig() ), 
				bgStorage, IsCouplingUsed() ? GADID_COUPLING_OUTPUT : GADID_SELECTION_OUTPUT, GetWorkflowStorage(), _populationID );

			_nopStep = new Common::Workflows::GaNopStep();

			_fitnessStep = new Population::GaPopulationFitnessStep( GetWorkflowStorage(), _populationID );

			_scalingStep = new Population::GaScalingStep( Population::GaScalingSetup(), GetWorkflowStorage(), _populationID );

			_sortStep = new Population::GaSortPopulationStep<Population::GaFitnessComparatorSortingCriteria>( GetWorkflowStorage(), _populationID, 
				Population::GaFitnessComparatorSortingCriteria(Fitness::GaFitnessComparatorSetup( &_scaledFitnessComparator, &Fitness::Comparators::GaSimpleComparatorParams(), NULL ),
				Population::GaChromosomeStorage::GAFT_SCALED ) );

			_nextGenStep = new Common::Workflows::GaSimpleMethodExecStep<Population::GaPopulation>( &Population::GaPopulation::NextGeneration, GetWorkflowStorage(), _populationID );

			Common::Workflows::GaBranchGroupFlow* flow = _brachGroup->GetBranchGroupFlow();

			// connect created flow steps 

			flow->SetFirstStep( _checkStep );
			flow->ConnectSteps( _checkStep, _selectionStep, 1 );

			flow->ConnectSteps( _couplingStep, _replacementStep, 0 );

			// do not connect coupling step if selection operation is used to produce offspring chromosomes
			_couplingConnection = IsCouplingUsed() ?  flow->ConnectSteps( _selectionStep, _couplingStep, 0 ) : flow->ConnectSteps( _selectionStep, _replacementStep, 0 );

			flow->ConnectSteps( _replacementStep, _nopStep, 0 );

			flow->ConnectSteps( _fitnessStep, _scalingStep, 0 );
			flow->ConnectSteps( _scalingStep, _sortStep, 0 );
			flow->ConnectSteps( _sortStep, _nextGenStep, 0 );
			
			// do not use fitness step if population does not have to be re-evaluated in each generation
			_fitnessConnection = UseFitnessStep() ? flow->ConnectSteps( _nopStep, _fitnessStep, 0 ) : flow->ConnectSteps( _nopStep, _scalingStep, 0 );

			flow->ConnectSteps( _checkStep, _initStep, 0 );
			flow->ConnectSteps( _initStep, _nopStep, 0 );
		}

		// Disconnects stub from the workflow
		void GaSPEAStubBase::Disconnecting()
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
			flow->RemoveStep( _scalingStep, true, true );
			flow->RemoveStep( _sortStep, true, true );
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
		void GaSPEAStubBase::Clear()
		{
			// clear pointers that cache flow connections used by stub
			_fitnessConnection = NULL;
			_couplingConnection = NULL;

			// clear pointers that cache flow steps used by stub
			_checkStep = NULL;
			_initStep = NULL;
			_selectionStep = NULL;
			_couplingStep = NULL;
			_replacementStep = NULL;
			_nopStep = NULL;
			_fitnessStep = NULL;
			_scalingStep = NULL;
			_sortStep = NULL;
			_nextGenStep = NULL;
		}

		// Sets SPEA parameters
		void GaSPEAStub::SetSPEAParameters(const Multiobjective::SPEA::GaSPEAParams& params)
		{
			_speaParameters = params;

			// update operation setup stored in flow step if stub is connected
			if( IsConnected() )
			{
				Population::GaScalingSetup setup = _scalingStep->GetSetup();
				setup.SetParameters( &_speaParameters );
				_scalingStep->SetSetup( setup );
			}
		}

		// Sets SPEA parameters
		void GaSPEA2Stub::SetSPEAParameters(const Multiobjective::SPEA::GaSPEA2Params& params)
		{
			_speaParameters = params;

			// update operation setup stored in flow step if stub is connected
			if( IsConnected() )
			{
				Population::GaScalingSetup setup = _scalingStep->GetSetup();
				setup.SetParameters( &_speaParameters );
				_scalingStep->SetSetup( setup );
			}
		}

	} // Stubs
} // Algorithm

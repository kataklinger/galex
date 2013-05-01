
/*! \file SimpleStub.cpp
    \brief This file conatins implementation of classes that implement algorithm stubs for simple GA with overlapping population.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#include "SimpleStub.h"

namespace Algorithm
{
	namespace Stubs
	{

		// Initializes simple GA stub
		GaSimpleGAStub::GaSimpleGAStub(int populationID,
			int statisticsID,
			const Chromosome::GaInitializatorSetup& initializator,
			const Population::GaPopulationFitnessOperationSetup& fitnessOperation,
			const Fitness::GaFitnessComparatorSetup& comparator,
			const Population::GaPopulationParams& populationParameters,
			const GaStatTrackersCollection& statTrackers,
			const Chromosome::GaMatingSetup& mating,
			const Population::GaSelectionSetup& selection,
			const Population::GaCouplingSetup& coupling,
			const Population::GaReplacementSetup& replacement,
			const Population::GaScalingSetup& scaling,
			const Population::GaFitnessComparatorSortingCriteria& populationSort) : GaBasicStub(populationID, statisticsID)
		{
			// stores operations and their parameters
			SetPopulationParameters( populationParameters );
			SetInitializator( initializator );
			SetFitnessOperation( fitnessOperation );
			SetFitnessComparator( comparator );
			SetStatsTrackers( statTrackers );
			SetMating( mating );
			SetSelection( selection );
			SetCoupling( coupling );
			SetReplacement( replacement );
			SetScaling( scaling );
			SetPopulationSort( populationSort );
		}

		// Sets fitness operation 
		void GACALL GaSimpleGAStub::SetFitnessOperation(const Population::GaPopulationFitnessOperationSetup& fitnessOperation)
		{
			// disconnect or connect fitness step depanding on whether the new fitness operation does not require re-evaluation of whole population in each generation
			bool useFitnessStep = !fitnessOperation.GetOperation().AllowsIndividualEvaluation();
			if( useFitnessStep != UseFitnessStep() )
			{
				Common::Workflows::GaBranchGroupFlow* flow = _brachGroup->GetBranchGroupFlow();
				flow->RemoveConnection( _fitnessConnection, true );
				_fitnessConnection = useFitnessStep ? flow->ConnectSteps( _nopStep1, _fitnessStep, 0 ) : flow->ConnectSteps( _nopStep1, _nopStep2, 0 );
			}

			// store fitness operation
			UpdateFitnessOperation( fitnessOperation );
		}

		// Sets mating operation
		void GaSimpleGAStub::SetMating(const Chromosome::GaMatingSetup& mating)
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
			if( _coupling.HasConfiguration() )
			{
				_coupling.GetConfiguration().SetMating( _mating );

				// update couling operation setup stored in flow step if stub is connected
				if( IsConnected() )
					_couplingStep->SetSetup( _coupling );
			}
		}

		// Sets selection operation
		void GaSimpleGAStub::SetSelection(const Population::GaSelectionSetup& selection)
		{
			_selection = selection;

			// decide whether the coupling operation should be used
			bool useCoupling =  selection.GetParameters().GetCrossoverBuffersTagID() < 0;
			bool changeConnection = useCoupling != IsCouplingUsed();

			if( !useCoupling )
			{
				// store mating operation
				if( _selection.GetConfiguration().GetMating().HasOperation() )
					_mating = _selection.GetConfiguration().GetMating();
				else
					_selection.GetConfiguration().SetMating( _mating );
			}

			if( IsConnected() )
			{
				// update selection operation setup stored in flow step if stub is connected
				_selectionStep->SetSetup( _selection );

				if( changeConnection )
				{
					Common::Workflows::GaBranchGroupFlow* flow = _brachGroup->GetBranchGroupFlow();

					// connect or disconnect couling step
					flow->RemoveConnection( _couplingConnection, true );
					_couplingConnection = useCoupling ? flow->ConnectSteps( _selectionStep, _couplingStep, 0 ) : flow->ConnectSteps( _selectionStep, _replacementStep , 0 );
					_replacementStep->SetInputData( _brachGroup->GetData(), useCoupling ? GADID_COUPLING_OUTPUT : GADID_SELECTION_OUTPUT );
				}
			}
		}

		// Sets coupling operation
		void GaSimpleGAStub::SetCoupling(const Population::GaCouplingSetup& coupling)
		{
			_coupling = coupling;

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

				_couplingStep->SetSetup( _coupling );
			}
		}

		// Sets replacment operation
		void GaSimpleGAStub::SetReplacement(const Population::GaReplacementSetup& replacement)
		{
			_replacement = replacement;

			// update replacment operation setup stored in flow step if stub is connected
			if( IsConnected() )
				_replacementStep->SetSetup( replacement );
		}

		// Sets scaling operation
		void GaSimpleGAStub::SetScaling(const Population::GaScalingSetup& scaling)
		{
			_scaling = scaling;

			bool useScaling =  scaling.HasOperation();
			bool changeConnection = useScaling != IsScalingUsed();

			// update scaling operation setup stored in flow step if stub is connected
			if( IsConnected() )
			{
				_scalingStep->SetSetup( _scaling );

				if( changeConnection )
				{
					Common::Workflows::GaBranchGroupFlow* flow = _brachGroup->GetBranchGroupFlow();

					// connect or disconnect scaling step
					flow->RemoveConnection( _scalingConnection, true );
					_scalingConnection = useScaling ? flow->ConnectSteps( _nopStep2, _scalingStep, 0 ) : flow->ConnectSteps( _nopStep2, _nopStep3 , 0 );
				}
			}
		}

		// Sets population sorting criteria
		void GaSimpleGAStub::SetPopulationSort(const Population::GaFitnessComparatorSortingCriteria& sort)
		{
			_populationSort = sort;

			bool useSort =  sort.GetComparator().HasOperation();
			bool changeConnection = useSort != IsSortingUsed();

			// update sort criteria stored in flow step if stub is connected
			if( IsConnected() )
			{
				_sortStep->SetCriteria( _populationSort );

				if( changeConnection )
				{
					Common::Workflows::GaBranchGroupFlow* flow = _brachGroup->GetBranchGroupFlow();

					// connect or disconnect sort step
					flow->RemoveConnection( _sortConnection, true );
					_sortConnection = useSort ? flow->ConnectSteps( _nopStep3, _sortStep, 0 ) : flow->ConnectSteps( _nopStep3, _nextGenStep, 0 );

				}
			}
		}

		// Sets statistical trackers that are required by the algorithm
		void GaSimpleGAStub::SetStatsTrackers(const GaSimpleGAStub::GaStatTrackersCollection& trackers)
		{
			if( IsConnected() )
			{
				GaCachedPopulation population( GetWorkflowStorage(), _populationID );

				// unregister previously registered trackers
				for( GaStatTrackersCollection::iterator it = _statTrackers.begin(); it != _statTrackers.end(); ++it )
					population.GetData().UnregisterTracker( it->first );

				// register new trackers
				for( GaStatTrackersCollection::const_iterator it = trackers.begin(); it != trackers.end(); ++it )
					population.GetData().RegisterTracker( it->first, it->second );
			}

			// store required trackers
			_statTrackers.clear();
			_statTrackers = trackers;
		}

		// Connects stub to the workflow
		void GaSimpleGAStub::Connected()
		{
			GaBasicStub::Connected();

			GaCachedPopulation population( GetWorkflowStorage(), _populationID );

			// register statistics trackers required by the algorithm
			for( GaStatTrackersCollection::iterator it = _statTrackers.begin(); it != _statTrackers.end(); ++it )
				population.GetData().RegisterTracker( it->first, it->second );

			// create data objects required by the simple GA and insert them to workflow data storage 
			Population::GaChromosomeGroup* selectionOutput = new Population::GaChromosomeGroup();
			Population::GaChromosomeGroup* coulingOutput = new Population::GaChromosomeGroup();

			Common::Workflows::GaDataStorage* bgStorage = _brachGroup->GetData();
			bgStorage->AddData( new Common::Workflows::GaDataEntry<Population::GaChromosomeGroup>( GADID_SELECTION_OUTPUT, selectionOutput ), Common::Workflows::GADSL_BRANCH_GROUP );
			bgStorage->AddData( new Common::Workflows::GaDataEntry<Population::GaChromosomeGroup>( GADID_COUPLING_OUTPUT, coulingOutput ), Common::Workflows::GADSL_BRANCH_GROUP );

			// create flow steps that 

			_checkStep = new GaCheckPopulationStep( GetWorkflowStorage(), _populationID );
			_initStep = new Common::Workflows::GaSimpleMethodExecStep<Population::GaPopulation, Common::Workflows::GaMethodExecIgnoreBranch<Population::GaPopulation> >
				( &Population::GaPopulation::Initialize, GetWorkflowStorage(), _populationID );

			_selectionStep = new Population::GaSelectionStep( _selection, GetWorkflowStorage(), _populationID, bgStorage, GADID_SELECTION_OUTPUT );

			_couplingStep = new Population::GaCouplingStep( _coupling, bgStorage, GADID_SELECTION_OUTPUT, bgStorage, GADID_COUPLING_OUTPUT );

			_replacementStep = new Population::GaReplacementStep( _replacement, bgStorage, IsCouplingUsed() ? GADID_COUPLING_OUTPUT : GADID_SELECTION_OUTPUT,
				GetWorkflowStorage(), _populationID );

			_nopStep1 = new Common::Workflows::GaNopStep();

			_fitnessStep = new Population::GaPopulationFitnessStep( GetWorkflowStorage(), _populationID );

			_nopStep2 = new Common::Workflows::GaNopStep();

			_scalingStep = new Population::GaScalingStep( _scaling, GetWorkflowStorage(), _populationID );

			_nopStep3 = new Common::Workflows::GaNopStep();

			_sortStep = new Population::GaSortPopulationStep<Population::GaFitnessComparatorSortingCriteria>( GetWorkflowStorage(), _populationID, _populationSort );

			_nextGenStep = new Common::Workflows::GaSimpleMethodExecStep<Population::GaPopulation>( &Population::GaPopulation::NextGeneration, GetWorkflowStorage(), _populationID );

			Common::Workflows::GaBranchGroupFlow* flow = _brachGroup->GetBranchGroupFlow();

			// connect created flow steps 

			flow->SetFirstStep( _checkStep );
			flow->ConnectSteps( _checkStep, _selectionStep, 1 );

			flow->ConnectSteps( _couplingStep, _replacementStep, 0 );
			flow->ConnectSteps( _replacementStep, _nopStep1, 0 );
			flow->ConnectSteps( _fitnessStep, _nopStep2, 0 );
			flow->ConnectSteps( _scalingStep, _nopStep3, 0 );
			flow->ConnectSteps( _sortStep, _nextGenStep, 0 );

			// connect required operations
			_couplingConnection = IsCouplingUsed() ?  flow->ConnectSteps( _selectionStep, _couplingStep, 0 ) : flow->ConnectSteps( _selectionStep, _replacementStep, 0 );
			_fitnessConnection = UseFitnessStep() ? flow->ConnectSteps( _nopStep1, _fitnessStep, 0 ) : flow->ConnectSteps( _nopStep1, _nopStep2, 0 );
			_scalingConnection = IsScalingUsed() ? flow->ConnectSteps( _nopStep2, _scalingStep, 0 ) : flow->ConnectSteps( _nopStep2, _nopStep3, 0 );
			_sortConnection = IsSortingUsed() ? flow->ConnectSteps( _nopStep3, _sortStep, 0 ) : flow->ConnectSteps( _nopStep3, _nextGenStep, 0 );

			flow->ConnectSteps( _checkStep, _initStep, 0 );
			flow->ConnectSteps( _initStep, _nopStep1, 0 );
		}

		// Disconnects stub from the workflow
		void GaSimpleGAStub::Disconnecting()
		{
			Common::Workflows::GaBranchGroupFlow* flow = _brachGroup->GetBranchGroupFlow();

			// disconnect and destroy flow steps created by the stub

			flow->RemoveStep( _checkStep, true, true );
			flow->RemoveStep( _initStep, true, true );
			flow->RemoveStep( _selectionStep, true, true );
			flow->RemoveStep( _couplingStep, true, true );
			flow->RemoveStep( _replacementStep, true, true );
			flow->RemoveStep( _nopStep1, true, true );
			flow->RemoveStep( _fitnessStep, true, true );
			flow->RemoveStep( _nopStep2, true, true );
			flow->RemoveStep( _scalingStep, true, true );
			flow->RemoveStep( _nopStep3, true, true );
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
			for( GaStatTrackersCollection::iterator it = _statTrackers.begin(); it != _statTrackers.end(); ++it )
				population.GetData().UnregisterTracker( it->first );

			GaBasicStub::Disconnecting();
		}

		// Clears internal bookkeeping.
		void GaSimpleGAStub::Clear()
		{
			// clear pointers that cache flow connections used by stub
			_sortConnection = NULL;
			_scalingConnection = NULL;
			_fitnessConnection = NULL;
			_couplingConnection = NULL;

			// clear pointers that cache flow steps used by stub
			_checkStep = NULL;
			_initStep = NULL;
			_selectionStep = NULL;
			_couplingStep = NULL;
			_replacementStep = NULL;
			_nopStep1 = NULL;
			_fitnessStep = NULL;
			_nopStep2 = NULL;
			_scalingStep = NULL;
			_nopStep3 = NULL;
			_sortStep = NULL;
			_nextGenStep = NULL;
		}
		
	} // Stubs
} // Algorithm

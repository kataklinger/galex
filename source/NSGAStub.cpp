
#include "NSGAStub.h"

namespace Algorithm
{
	namespace Stubs
	{

		// Initializes SPEA basic stub
		GaNSGAStubBase::GaNSGAStubBase(int populationID,
			int statisticsID,
			const Chromosome::GaInitializatorSetup& initializator,
			const Population::GaPopulationFitnessOperationSetup& fitnessOperation,
			const Fitness::GaFitnessComparatorSetup& comparator,
			const Chromosome::GaMatingSetup& mating,
			const Population::GaCouplingSetup& coupling) : GaBasicStub(populationID, statisticsID)
		{
			// stores operations and their parameters
			SetInitializator( initializator );
			SetFitnessOperation( fitnessOperation );
			SetFitnessComparator( comparator );
			SetMating( mating );
			SetCoupling( coupling );

			Clear();
		}

		// Sets fitness operation 
		void GACALL GaNSGAStubBase::SetFitnessOperation(const Population::GaPopulationFitnessOperationSetup& fitnessOperation)
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
		void GaNSGAStubBase::SetMating(const Chromosome::GaMatingSetup& mating)
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

		// Sets coupling operation
		void GaNSGAStubBase::SetCoupling(const Population::GaCouplingSetup& coupling)
		{
			_couplingSetup = coupling;

			// correcting number of offspring that should be produced
			if( _couplingSetup.HasParameters() )
				_couplingSetup.GetParameters().SetNumberOfOffsprings( _populationParameters.GetPermanentSpaceSize() );

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

		// Updates algorithm stub according to new population parameters
		void GaNSGAStubBase::UpdatePopulationParameters(Population::GaPopulationParams& params)
		{
			// correcting number of offspring that should be produced
			if( _couplingSetup.HasParameters() )
				_couplingSetup.GetParameters().SetNumberOfOffsprings( params.GetPermanentSpaceSize() );

			// update setups of the operations stored in flow steps if stub is connected
			if( IsConnected() )
			{
				if( !IsCouplingUsed() )
				{
					// updates number of chromosomes that should be produced by selection operation
					Population::GaSelectionSetup setup = _selectionStep->GetSetup();
					setup.GetParameters().SetSelectionSize( params.GetPermanentSpaceSize() );
					_selectionStep->SetSetup( setup );
				}
				else
				{
					// updates number of chromosomes that should be produced by coupling operation
					_couplingSetup.GetParameters().SetNumberOfOffsprings( params.GetPermanentSpaceSize() );
					_couplingStep->SetSetup( _couplingSetup );
				}
			}

			// store population parameters
			GaBasicStub::UpdatePopulationParameters( params );
		}

		// Updates algorithm stub according to new selection parameters
		void GaNSGAStubBase::UpdateSelectionParameters(Population::GaSelectionParams& params)
		{
			// decide whether the coupling operation should be used
			bool useCoupling =  params.GetCrossoverBuffersTagID() < 0;
			bool changeConnection = useCoupling != IsCouplingUsed();

			// correcting number of offspring that should be produced
			if( !useCoupling )
				params.SetSelectionSize( _populationParameters.GetPermanentSpaceSize() );

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
		}

		// Connects stub to the workflow
		void GaNSGAStubBase::Connected()
		{
			GaBasicStub::Connected();

			GaCachedPopulation population( GetWorkflowStorage(), _populationID );

			// register statistics trackers required by SPEA
			population.GetData().RegisterTracker( Population::GaPopulationSizeTracker::TRACKER_ID, &_sizeTracker );
			population.GetData().RegisterTracker( Population::GaScaledFitnessTracker::TRACKER_ID, &_scaledTracker );

			Population::GaChromosomeGroup* selectionOutput = new Population::GaChromosomeGroup();
			Population::GaChromosomeGroup* coulingOutput = new Population::GaChromosomeGroup();

			// create data objects required by the NSGA and insert them to workflow data storage 
			Common::Workflows::GaDataStorage* bgStorage = _brachGroup->GetData();
			bgStorage->AddData( new Common::Workflows::GaDataEntry<Population::GaChromosomeGroup>( GADID_SELECTION_OUTPUT, selectionOutput ), Common::Workflows::GADSL_BRANCH_GROUP );
			bgStorage->AddData( new Common::Workflows::GaDataEntry<Population::GaChromosomeGroup>( GADID_COUPLING_OUTPUT, coulingOutput ), Common::Workflows::GADSL_BRANCH_GROUP );

			// create flow steps that 

			_checkStep = new GaCheckPopulationStep( GetWorkflowStorage(), _populationID );
			_initStep = new Common::Workflows::GaSimpleMethodExecStep<Population::GaPopulation, Common::Workflows::GaMethodExecIgnoreBranch<Population::GaPopulation> >
				( &Population::GaPopulation::Initialize, GetWorkflowStorage(), _populationID );

			_selectionStep = new Population::GaSelectionStep( Population::GaSelectionSetup(), GetWorkflowStorage(), _populationID, bgStorage, GADID_SELECTION_OUTPUT );

			_couplingStep = new Population::GaCouplingStep( _couplingSetup, bgStorage, GADID_SELECTION_OUTPUT, bgStorage, GADID_COUPLING_OUTPUT );

			_replacementStep = new Population::GaReplacementStep( Population::GaReplacementSetup(), 
				bgStorage, IsCouplingUsed() ? GADID_COUPLING_OUTPUT : GADID_SELECTION_OUTPUT, GetWorkflowStorage(), _populationID );

			_nopStep = new Common::Workflows::GaNopStep();

			_fitnessStep = new Population::GaPopulationFitnessStep( GetWorkflowStorage(), _populationID );

			_scalingStep = new Population::GaScalingStep( Population::GaScalingSetup(), GetWorkflowStorage(), _populationID );

			_sortStep = new Population::GaSortPopulationStep<Population::GaFitnessComparatorSortingCriteria>( GetWorkflowStorage(), _populationID, 
				Population::GaFitnessComparatorSortingCriteria( Fitness::GaFitnessComparatorSetup(), Population::GaChromosomeStorage::GAFT_SCALED ) );

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
			
			// do not use fitness step if population does not have to be re-evaluated in each generation
			_fitnessConnection = UseFitnessStep() ? flow->ConnectSteps( _nopStep, _fitnessStep, 0 ) : flow->ConnectSteps( _nopStep, _scalingStep, 0 );

			flow->ConnectSteps( _checkStep, _initStep, 0 );
			flow->ConnectSteps( _initStep, _nopStep, 0 );
		}

		// Disconnects stub from the workflow
		void GaNSGAStubBase::Disconnecting()
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
		void GaNSGAStubBase::Clear()
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

		// Initializes NSGA stub
		GaNSGAStub::GaNSGAStub(int populationID,
			int statisticsID,
			const Chromosome::GaInitializatorSetup& initializator,
			const Population::GaPopulationFitnessOperationSetup& fitnessOperation,
			const Fitness::GaFitnessComparatorSetup& comparator,
			const Population::GaPopulationParams& populationParameters,
			const Chromosome::GaMatingSetup& mating,
			const Population::GaCouplingSetup& coupling,
			const Population::SelectionOperations::GaDuplicatesSelectionParams& selectionParameters,
			const Chromosome::GaChromosomeComparatorSetup& chromosomeComparator,
			const Multiobjective::NSGA::GaNSGAParams& nsgaParameters) : GaNSGAStubBase(populationID, statisticsID, initializator, fitnessOperation, comparator, mating, coupling)
		{
			// stores operations and their parameters
			SetPopulationParameters( populationParameters );
			SetSelectionParameters( selectionParameters );
			SetChromosomeComparator( chromosomeComparator );
			SetNSGAParameters( nsgaParameters );
		}

		// Sets population parameters
		void GaNSGAStub::SetPopulationParameters(Population::GaPopulationParams params)
		{
			params.SetCrowdingSize( 0 );
			params.GetFillOptions().CopyAllFlags( Population::GaPopulationParams::GAPFO_FILL_ON_INIT );

			// correcting number of offspring that should be produced
			if( !IsCouplingUsed() )
				_selectionParameters.SetSelectionSize( params.GetPermanentSpaceSize() );

			// update setup of replacement operation stored in flow steps if stub is connected
			if( IsConnected() )
			{
				Population::GaReplacementSetup setup = _replacementStep->GetSetup();
				setup.GetParameters().SetReplacementSize( params.GetPermanentSpaceSize() );
				_replacementStep->SetSetup( setup );
			}

			// store population parameters
			GaNSGAStubBase::UpdatePopulationParameters( params );
		}

		// Sets chromosome comparator
		void GaNSGAStub::SetChromosomeComparator(const Chromosome::GaChromosomeComparatorSetup& comparator)
		{
			_chromosomeComparator = comparator;

			// update NSGA setup in flow steps if stub is connected
			if( IsConnected() )
			{
				Population::GaScalingSetup setup = _scalingStep->GetSetup();
				( (Population::ScalingOperations::GaShareFitnessScalingConfig&)setup.GetConfiguration() ).SetComparator( comparator );
				_scalingStep->SetSetup( setup );
			}
		}

		// Sets NSGA parameters
		void GaNSGAStub::SetNSGAParameters(const Multiobjective::NSGA::GaNSGAParams& params)
		{
			_nsgaParameters = params;

			// update operation setup stored in flow step if stub is connected
			if( IsConnected() )
			{
				Population::GaScalingSetup setup = _scalingStep->GetSetup();
				setup.SetParameters( &_nsgaParameters );
				_scalingStep->SetSetup( setup );
			}
		}

		// Connects stub to the workflow
		void GaNSGAStub::Connected()
		{
			GaNSGAStubBase::Connected();

			// stores operation setups to flow steps

			_selectionStep->SetSetup( Population::GaSelectionSetup( &_selectionOperation, &_selectionParameters, &Population::GaCouplingConfig( _mating ) ) );

			_replacementStep->SetSetup( Population::GaReplacementSetup( &_replacementOperation,
				&Population::ReplacementOperations::GaElitismReplacementParams( _populationParameters.GetPermanentSpaceSize(), 0 ), &Population::GaReplacementConfig() ) );

			_scalingStep->SetSetup(
				Population::GaScalingSetup( &_nsgaOperation, &_nsgaParameters, &Population::ScalingOperations::GaShareFitnessScalingConfig( NULL, _chromosomeComparator ) ) );

			_sortStep->SetCriteria( Population::GaFitnessComparatorSortingCriteria(
				Fitness::GaFitnessComparatorSetup( &_scaledFitnessComparator, &Fitness::Comparators::GaSimpleComparatorParams(), NULL ), Population::GaChromosomeStorage::GAFT_SCALED ) );

			// connect created flow steps 

			_brachGroup->GetBranchGroupFlow()->ConnectSteps( _sortStep, _nextGenStep, 0 );
		}

		// Initializes NSGA-II stub
		GaNSGA2Stub::GaNSGA2Stub(int populationID,
			int statisticsID,
			const Chromosome::GaInitializatorSetup& initializator,
			const Population::GaPopulationFitnessOperationSetup& fitnessOperation,
			const Fitness::GaFitnessComparatorSetup& comparator,
			const Population::GaPopulationParams& populationParameters,
			const Chromosome::GaMatingSetup& mating,
			const Population::GaCouplingSetup& coupling,
			const Population::SelectionOperations::GaTournamentSelectionParams& selectionParameters,
			const Multiobjective::NSGA::GaNSGA2Params& nsgaParameters) : GaNSGAStubBase(populationID, statisticsID, initializator, fitnessOperation, comparator, mating, coupling),
				_trimStep(NULL)
		{
			SetPopulationParameters( populationParameters );
			SetSelectionParameters( selectionParameters );
			SetNSGAParameters( nsgaParameters );
		}

		// Sets population parameters
		void GaNSGA2Stub::SetPopulationParameters(Population::GaPopulationParams params)
		{
			// use half of the population as crowding space
			if( params.GetPopulationSize() % 2 != 0 )
				params.SetPopulationSize( params.GetPopulationSize() + 1 );
			params.SetCrowdingSize( params.GetPopulationSize() / 2 );

			params.GetFillOptions().CopyAllFlags( Population::GaPopulationParams::GAPFO_FILL_ON_INIT );

			// correcting number of offspring that should be produced

			if( !IsCouplingUsed() )
				_selectionParameters.SetSelectionSize( params.GetPermanentSpaceSize() );

			// update setup of replacement operation stored in flow steps if stub is connected
			if( IsConnected() )
			{
				Population::GaReplacementSetup setup = _replacementStep->GetSetup();
				setup.GetParameters().SetReplacementSize( params.GetCrowdingSize() );
				_replacementStep->SetSetup( setup );
			}

			// store population parameters
			GaNSGAStubBase::UpdatePopulationParameters( params );
		}

		// Sets NSGA parameters
		void GaNSGA2Stub::SetNSGAParameters(const Multiobjective::NSGA::GaNSGA2Params& params)
		{
			_nsgaParameters = params;

			// update operation setup stored in flow step if stub is connected
			if( IsConnected() )
			{
				Population::GaScalingSetup setup = _scalingStep->GetSetup();
				setup.SetParameters( &_nsgaParameters );
				_scalingStep->SetSetup( setup );
			}
		}

		// Connects stub to the workflow
		void GaNSGA2Stub::Connected()
		{
			GaNSGAStubBase::Connected();

			// stores operation setups to flow steps

			_selectionStep->SetSetup( Population::GaSelectionSetup( &_selectionOperation, &_selectionParameters, &Population::SelectionOperations::GaTournamentSelectionConfig(
				Fitness::GaFitnessComparatorSetup( &_scaledFitnessComparator, &Fitness::Comparators::GaSimpleComparatorParams(), NULL ), _mating ) ) );

			_replacementStep->SetSetup( Population::GaReplacementSetup(
				&_replacementOperation, &Population::GaReplacementParams( _populationParameters.GetCrowdingSize() ), &Population::GaReplacementConfig() ) );

			_scalingStep->SetSetup( Population::GaScalingSetup( &_nsgaOperation, &_nsgaParameters, &Population::GaScalingConfig( &Fitness::Representation::GaMVFitnessParams( 2 ) ) ) );

			_sortStep->SetCriteria( Population::GaFitnessComparatorSortingCriteria(
				Fitness::GaFitnessComparatorSetup( &_scaledFitnessComparator, &Fitness::Comparators::GaSimpleComparatorParams(), NULL ), Population::GaChromosomeStorage::GAFT_SCALED ) );

			_trimStep = new Common::Workflows::GaSimpleMethodExecStep<Population::GaPopulation, Common::Workflows::GaMethodExecIgnoreBranch<Population::GaPopulation> >
				( &Population::GaPopulation::Trim, GetWorkflowStorage(), _populationID );

			Common::Workflows::GaBranchGroupFlow* flow = _brachGroup->GetBranchGroupFlow();

			// connect created flow steps 

			flow->ConnectSteps( _sortStep, _trimStep, 0 );
			flow->ConnectSteps( _trimStep, _nextGenStep, 0 );
		}

		// Disconnects stub from the workflow
		void GaNSGA2Stub::Disconnecting()
		{
			// disconnect and destroy flow steps created by the stub
			_brachGroup->GetBranchGroupFlow()->RemoveStep( _trimStep, true, true );

			// clear internal bookkeeping
			_trimStep = NULL;

			GaNSGAStubBase::Disconnecting();
		}

	} // Stubs
} // Algorithm


#include "AlgorithmStub.h"

namespace Algorithm
{

	// Connects algorithm stub to a workflow
	void GaAlgorithmStub::Connect(Common::Workflows::GaWorkflowBarrier* fork,
		Common::Workflows::GaWorkflowBarrier* join)
	{
		GA_ARG_ASSERT( Common::Exceptions::GaNullArgumentException, fork != NULL, "fork", "Barrier step that forks branch group to execute stub must be specified.", "Algorithms" );
		GA_ARG_ASSERT( Common::Exceptions::GaNullArgumentException, join != NULL, "join", "Barrier step that joins branch group that executes stub must be specified.", "Algorithms" );

		// make branch group, connect it to workflow barriers and set number of branches
		_brachGroup = (Common::Workflows::GaBranchGroup*)fork->GetFlow()->ConnectSteps( fork, join, 0 );
		_brachGroup->SetBranchCount( _branchCount );

		// preparation for stub execution
		Connected();
	}

	// Disconnects algorithm stub from workflow
	void GaAlgorithmStub::Disconnect()
	{
		GA_ASSERT( Common::Exceptions::GaInvalidOperationException, _brachGroup != NULL, "Stab is not connected to any workflow.", "Algorithms" );

		// stub clean-up
		Disconnecting();

		// disconects branch group and distroys it
		_brachGroup->GetWorkflow()->RemoveConnection( _brachGroup, true );
		_brachGroup = NULL;
	}

	// Sets number of branches that will execute algorithm stub
	void GaAlgorithmStub::SetBranchCount(int branchCount)
	{
		if( IsConnected() )
			_brachGroup->SetBranchCount( branchCount );

		_branchCount = branchCount;
	}

	// Prepares workflow for stub execution
	void GaBasicStub::Connected()
	{
		Population::GaPopulation* population = new Population::GaPopulation( _populationParameters, _initializator, _fitnessOperation, _comparator );

		// add population and statistics to workflow data storage
		GetWorkflowStorage()->AddData( new Common::Workflows::GaDataEntry<Population::GaPopulation>( _populationID, population ), Common::Workflows::GADSL_WORKFLOW );
		GetWorkflowStorage()->AddData( new Common::Workflows::GaDataEntry<Statistics::GaStatistics>( _statisticsID, 
			Common::Memory::GaAutoPtr<Statistics::GaStatistics>( &population->GetStatistics(), Common::Memory::GaNoDeletionPolicy<Statistics::GaStatistics>::GetInstance() ) ),
			Common::Workflows::GADSL_WORKFLOW );
	}

	// Cleans workflow
	void GaBasicStub::Disconnecting()
	{
		// remove population and statistics from workflow storage
		GetWorkflowStorage()->RemoveData( _statisticsID, Common::Workflows::GADSL_WORKFLOW );
		GetWorkflowStorage()->RemoveData( _populationID, Common::Workflows::GADSL_WORKFLOW );
	}

} // Algorithm

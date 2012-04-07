
/*! \file Workflows.cpp
	\brief This file contains implementation of classes of workflow framework.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#include "Workflows.h"

namespace Common
{
	namespace Workflows
	{

		// Initializes data storage
		GaDataStorage::GaDataStorage(GaDataStorageLevel level, ...) : _level(level) 
		{
			va_list args;
			va_start( args, level );

			int i = 0;
			// initialize table of higher level objects
			for( ; i < (int)_level; i++ )
				_levelTable[ i ] = va_arg( args, GaDataStorage* );

			for( ; i < GADSL_NUMBER_OF_LEVELS; i++ )
				_levelTable[ i ] = NULL;

			va_end( args );
		}

		// Fress used resources
		GaDataStorage::~GaDataStorage()
		{
			for( STLEXT::hash_map<int, GaDataEntryBase*>::iterator it = _data.begin(); it != _data.end(); ++it )
				delete it->second;
		}

		// Sets pointer to data storage of higher level
		void GaDataStorage::SetHigherLevelStorage(GaDataStorageLevel level,
			GaDataStorage* storage)
		{
			GA_ARG_ASSERT( Exceptions::GaArgumentException, level < _level, "level", "Trying to set storage object for level below this object's level.", "Workflows" );

			GA_LOCK_THIS_OBJECT( lock );
			_levelTable[ level ] = storage;
		}

		// Finds data with specified ID in defined range of levels
		GaDataEntryBase* GaDataStorage::FindData(GaDataStorageLevel startStorageLevel,
			GaDataStorageLevel maxStorageDepth,
			int dataID)
		{
			GA_ARG_ASSERT( Exceptions::GaArgumentException, startStorageLevel >= maxStorageDepth,
				"startStorageLevel", "Start level of the search is higher then specified maximal level.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, startStorageLevel <= _level,
				"startStorageLevel", "Trying to query data below level of this storage object.", "Workflows" );

			GA_LOCK_THIS_OBJECT( lock );

			for( int i = startStorageLevel; i <= maxStorageDepth; i-- )
			{
				if( i == _level )
				{
					// current search level is equal to this object's level, try getting data from this object
					STLEXT::hash_map<int, GaDataEntryBase*>::iterator it = _data.find( dataID );
					if( it != _data.end() )
						return ( *it ).second;
				}
				else if( _levelTable[ i ] )
				{
					// try getting data from the storage level of the current search level
					GaDataEntryBase* data = _levelTable[ i ]->GetData( (GaDataStorageLevel)i, dataID );
					if( data )
						return data;
				}
			}

			// data has not been found
			return NULL;
		}

		// Returns pointer to data with specified ID at specified level
		GaDataEntryBase* GaDataStorage::GetData(GaDataStorageLevel level,
			int dataID)
		{ 
			GA_ARG_ASSERT( Exceptions::GaArgumentException, level <= _level, "storageLevel", "Trying to query data below level of this storage object.", "Workflows" );

			GA_LOCK_THIS_OBJECT( lock );

			// trying to get data from level which is equal to level of this object
			if( level == _level )
			{
				STLEXT::hash_map<int, GaDataEntryBase*>::iterator it = _data.find( dataID );
				if( it != _data.end() )
				{
					it->second->AddReference();
					return it->second;
				}
				
				return NULL;
			}

			// forward query to higher level object
			return _levelTable[ level ] ? _levelTable[ level ]->GetData( dataID ) : NULL;
		}

		// Inserts data in the storage object
		void GaDataStorage::AddData(GaDataEntryBase* entry,
			GaDataStorageLevel level)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, entry != NULL, "entry", "New entry must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, level <= _level, "storageLevel", "Trying to add data below level of this storage object.", "Workflows" );

			GA_LOCK_THIS_OBJECT( lock );

			// trying to insert data at level which is equal to level of this object
			if( level == _level )
			{
				GA_ARG_ASSERT( Exceptions::GaArgumentException, _data.find( entry->GetDataID() ) == _data.end(),
					"dataID", "Data with the ID already exists in the storage object.", "Workflows" );

				// store entry
				entry->SetDataStorage( this );
				_data[ entry->GetDataID() ] = entry;
			}
			else
			{
				// forward data insertion to higher level object
				if( _levelTable[ level ] )
					_levelTable[ level ]->AddData( entry, level );
			}
		}

		// Removes data from the storage object
		void GaDataStorage::RemoveData(int dataID,
			GaDataStorageLevel level)
		{
			GA_ARG_ASSERT( Exceptions::GaArgumentException, level <= _level, "storageLevel", "Trying to remove data from level below this storage object.", "Workflows" );

			GA_LOCK_THIS_OBJECT( lock );

			// trying to remove data from level which is equal to level of this object
			if( level == _level )
			{
				STLEXT::hash_map<int, GaDataEntryBase*>::iterator it = _data.find( dataID );
				if( it != _data.end() )
				{
					GaDataEntryBase* data = it->second;
					GA_ASSERT( Exceptions::GaInvalidOperationException, data->GetReferenceCount() == 0,
						"Entry with valid references to it cannot be removed from the storage.", "Workflows" );

					_data.erase( it );

					GA_UNLOCK( lock );

					delete data;
				}
			}
			else
			{
				// forward deletion to higher level object
				if( _levelTable[ level ] )
					_levelTable[ level ]->RemoveData( dataID, level );
			}
		}

		// Detach all outbound connections
		void GaFlowStep::DisconnectAllOutboundConnections()
		{
			// copy set of outbound connections because DisconnectOutboundStep method affects
			std::set<GaFlowConnection*> connections = _outboundConnections;
			for( std::set<GaFlowConnection*>::iterator it = connections.begin(); it != connections.end(); ++it )
				( *it )->DisconnectOutboundStep();
		}

		// Detach all inbound connections
		void GaFlowStep::DisconnectAllInboundConnections()
		{
			// copy set of inbound connections because DisconnectInboundStep method affects
			std::set<GaFlowConnection*> connections = _inboundConnections;
			for( std::set<GaFlowConnection*>::iterator it = connections.begin(); it != connections.end(); ++it )
				( *it )->DisconnectInboundStep();
		}

		// Attach inbound connection
		void GaBasicStep::AttachPreviousStep(GaFlowConnection* previousStep)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, previousStep != NULL, "previousStep", "Connection object must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, previousStep->GetInboundStep() == this && !HasInboundConnection( previousStep ),
				"previousStep", "Wrong connection object.", "Workflows" );

			AddInboundConnection( previousStep );
		}

		// Detach inbound connection
		void GaBasicStep::DetachPreviousStep(GaFlowConnection* previousStep)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, previousStep != NULL, "previousStep", "Connection object must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, HasInboundConnection( previousStep ), "previousStep", "Wrong connection object.", "Workflows" );

			RemoveInboundConnection( previousStep );
		}

		// Connects two steps
		void GaFlowConnection::ConnectSteps(GaFlowStep* outboundStep,
			GaFlowStep* inboundStep)
		{
			// set and attach connection to outbound and inbound steps
			ConnectOutboundStep( outboundStep );
			ConnectInboundStep( inboundStep );
		}

		// Attaches connection to outbound step
		void GaFlowConnection::ConnectOutboundStep(GaFlowStep* outboundStep)
		{	
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, outboundStep != NULL, "outboundStep", "Outbound step must be specified.", "Workflows" );
			GA_ASSERT( Exceptions::GaInvalidOperationException, _outboundStep == NULL, "Connection is already connected to outbound step.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, !_inboundStep || CheckConnectionValidity( outboundStep, _inboundStep ),
				"outboundStep", "The connection is not valid.", "Workflows" );

			try
			{
				// store outbound step and attach connection to putbound and inboud steps if both are available
				_outboundStep = outboundStep;
				_outboundStep->AttachNextStep( this );
			}
			catch( ... )
			{
				_outboundStep = NULL;
				throw;
			}
		}

		// Attaches connection to inbound step
		void GaFlowConnection::ConnectInboundStep(GaFlowStep* inboundStep)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, inboundStep != NULL, "inboundStep", "Outbound step must be specified.", "Workflows" );
			GA_ASSERT( Exceptions::GaInvalidOperationException, _inboundStep == NULL, "Connection is already inbound to outbound step.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, !_outboundStep || CheckConnectionValidity( inboundStep, _outboundStep ),
				"inboundStep", "The connection is not valid.", "Workflows" );

			try
			{
				// store inbound step and attach connection to outbound and inboud steps if both are available
				_inboundStep = inboundStep;
				_inboundStep->AttachPreviousStep( this );
			}
			catch( ... )
			{
				_inboundStep = NULL;
				throw;
			}
		}

		// Detaches connection from inbound and outbound steps
		void GaFlowConnection::DisconnectSteps()
		{
			DisconnectOutboundStep();
			DisconnectInboundStep();
		}

		// Detaches connection from outbound step
		void GaFlowConnection::DisconnectOutboundStep()
		{
			if( _outboundStep )
			{
				_outboundStep->DetachNextStep( this );
				_outboundStep = NULL;
			}
		}

		// Detaches connection from inbound step
		void GaFlowConnection::DisconnectInboundStep()
		{
			if( _inboundStep )
			{
				_inboundStep->DetachPreviousStep( this );
				_inboundStep = NULL;
			}
		}

		// Changes ID of the connection
		void GaFlowConnection::SetConnectionID(int id)
		{
			int oldConnectionID = _connectionID;
			_connectionID = id;

			// notify steps only when the connection attached to both step
			if( _outboundStep && _inboundStep )
			{
				// notify outbound step that connection ID has changed
				_outboundStep->ConnectionIDChanged( this, oldConnectionID );

				try
				{
					// notify outbound step that connection ID has changed
					_inboundStep->ConnectionIDChanged( this, oldConnectionID );
				}
				catch( Exceptions::GaException& ex1 )
				{
					// restore old connection
					_connectionID = oldConnectionID;

					try
					{
						// try to restore connection ID change performed on outbound step
						_outboundStep->ConnectionIDChanged( this, id );
					}
					catch( Exceptions::GaException& ex2 )
					{
						// store previous exception
						ex2.SetInnerException( &ex1 );
						throw;
					}

					throw;
				}
			}
		}

		// Deletes step and connection objects
		GaBranchGroupFlow::~GaBranchGroupFlow()
		{
			for( std::set<GaFlowConnection*>::iterator it = _connections.begin(); it != _connections.end(); ++it )
				delete *it;

			for( std::set<GaFlowStep*>::iterator it = _steps.begin(); it != _steps.end(); ++it )
				delete *it;
		}

		// Inserts step to the flow
		void GaBranchGroupFlow::AddStep(GaFlowStep* step)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, step != NULL, "step", "Step must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, !step->HasConnections(), "step", "Step is connected to the other steps which might not be members of this flow.", "Workflows" );

			// mark the step as a memeber of this flow and insert it
			step->BindToFlow( this );
			_steps.insert( step );
		}

		// Removes step from the flow
		void GaBranchGroupFlow::RemoveStep(GaFlowStep* step,
			bool disconnect,
			bool destroy)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, step != NULL, "step", "Step must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, step->GetFlow() == this, "step", "Step does not belong to this flow.", "Workflows" );
			GA_ASSERT( Exceptions::GaInvalidOperationException, disconnect || !step->HasConnections(),
				"Trying to remove step which is connected to the other steps of this flow without disconnecting it.", "Workflows" );

			// disconnect step from all other steps
			if( disconnect )
				step->DisconnectAllConnections();

			// clear mark that the step is a memeber of this flow and removes it
			step->UnbindFromFlow();
			_steps.erase( step );
			if( destroy )
				delete step;
		}

		// Connects two step of the flow
		GaFlowConnection* GaBranchGroupFlow::ConnectSteps(GaFlowStep* outboundStep,
			GaFlowStep* inboundStep,
			int connectionID)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, outboundStep != NULL, "outboundStep", "Outbound step must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, inboundStep != NULL, "inboundStep", "Inbound step must be specified.", "Workflows" );

			// add step to flow it is not member of any other flow
			if( !outboundStep->GetFlow() )
				AddStep( outboundStep );

			// add step to flow it is not member of any other flow
			if( !inboundStep->GetFlow() )
				AddStep( inboundStep );

			GA_ARG_ASSERT( Exceptions::GaArgumentException, outboundStep->GetFlow() == this && inboundStep->GetFlow() == this,
				"outboundStep or inboundStep", "Specified steps do not belong to this flow.", "Workflows" );

			// make connection object
			Memory::GaAutoPtr<GaFlowConnection> connection = new GaBrachGroupFlowConnection( connectionID );

			// connect steps and add it to the flow
			connection->ConnectSteps( outboundStep, inboundStep );
			_connections.insert( connection.GetRawPtr() );

			return connection.DetachPointer();
		}

		// Disconnects steps connected with specified connection object and removes it from the flow
		void GaBranchGroupFlow::RemoveConnection(GaFlowConnection* connection,
			bool destory)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, connection != NULL, "connection", "Connection object must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, _connections.find( connection ) != _connections.end(),
				"connection", "This connection does not belong to this flow.", "Workflows" );

			// disconnect attached steps and removes connection from the flow
			connection->DisconnectSteps();
			_connections.erase( connection );
			if( destory )
				delete connection;
		}

		// Sets the first step which is executed in the flow
		void GaBranchGroupFlow::SetFirstStep(GaFlowStep* step)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, step != NULL, "step", "Step must be specified.", "Workflows" );

			// add step to flow it is not member of any other flow
			if( !step->GetFlow() )
				AddStep( step );

			GA_ARG_ASSERT( Exceptions::GaArgumentException, step->GetFlow() == this, "step", "Specified step does not belong to this flow.", "Workflows" );
			_firstStep = step;
		}

		// Disallows specified branch
		void GaBranchFilterInfo::ClearBranchMask(int branchID)
		{
			GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, branchID >= 0 && branchID < GetSize(), "branchID", "Branch ID is out of range.", "Workflows" );

			GaFilterEntry& entry = _filter[ branchID ];

			// is it already disallowed?
			if( entry._allowed )
			{
				// disallow
				entry._allowed = false;
				_count--;

				// update raw to filtered ID converter
				for( int i = GetSize() - 1; i > branchID; i-- )
					_filter[ i ]._filteredID--;
			}
		}

		// Allows specified branch
		void GaBranchFilterInfo::SetBranchMask(int branchID)
		{
			GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, branchID >= 0 && branchID < GetSize(), "branchID", "Branch ID is out of range.", "Workflows" );

			GaFilterEntry& entry = _filter[ branchID ];

			// is it already allowed?
			if( !entry._allowed )
			{
				// allow
				entry._allowed = true;
				_count++;

				// update raw to filtered ID converter
				for( int i = GetSize() - 1; i > branchID; i-- )
					_filter[ i ]._filteredID--;
			}
		}

		// Disallows all branches
		void GaBranchFilterInfo::ClearAll()
		{
			// disallow all and update raw to filtered ID converter
			for( int i = GetSize() - 1; i > 0; i-- )
			{
				_filter[ i ]._allowed = false;
				_filter[ i ]._filteredID = 0;
			}

			_count = 0;
		}

		// Allows all branches
		void GaBranchFilterInfo::SetAll()
		{
			// allow all and update raw to filtered ID converter
			for( int i = GetSize() - 1; i > 0; i-- )
			{
				_filter[ i ]._allowed = true;
				_filter[ i ]._filteredID = i;
			}

			_count = GetSize();
		}

		// Sets size of the filter
		void GaBranchFilterInfo::SetSize(int size)
		{
			int oldSize = GetSize();
			_filter.resize( size );

			// allow all new banches and update raw to filtered ID converter
			for( int i = oldSize; i < size; i++ )
			{
				_filter[ i ]._allowed = true;
				_filter[ i ]._filteredID = _count++;
			}
		}

		// Initializes branch
		GaBranch::GaBranch(GaBranchGroup* branchGroup,
			int branchID) :  _branchGroup(branchGroup),
			_branchID(branchID),
			_currentFilter(NULL),
			_data(GADSL_BRANCH, _branchGroup->GetWorkflow()->GetGlobalData(), _branchGroup->GetWorkflow()->GetWorkflowData(), _branchGroup->GetData()),
			_previousStep(NULL),
			_lastDecision(-1),
			_executingLastStep(false) { }

		// Executes steps
		void GaBranch::BranchFlow(GaFlowStep* firstStep)
		{
			_previousStep = NULL;
			_executingLastStep = false;

			// get step by step, exit loop when there is no next step
			for( GaFlowStep* step = firstStep; step; step = step->GetNextStep( this ) )
			{
				// continue branch execution only if the workflow is in running state
				if( !_branchGroup->GetWorkflow()->CheckWorkflowState() )
					break;

				// execute step
				if( step->Enter( this ) )
				{
					( *step )( this );
					step->Exit( this );
				}
				
				_previousStep = step;

				// clear filter from previous step
				_currentFilter = NULL;
			}

			// executes last step (barrier) at the end of the branch and notify branch group that execution of the branch has finished
			_executingLastStep = true;
			_branchGroup->ExecuteBranchLastStep( this );
		}

		// Starts branch
		void GaBranch::StartBranch(GaFlowStep* firstStep)
		{
			// notifies workflow that group is queued for start
			_branchGroup->GetWorkflow()->BranchStart();
			Threading::GaThreadPool::GetInstance().ExecuteWork( &Threading::GaMethodWorkItem<void, GaBranch, GaFlowStep*>( this, &GaBranch::BranchFlow, firstStep ), false );
		}

		// Calculate distribution of workload among branches
		void GaBranch::SplitWork(int totalWork,
			int& branchWork)
		{
			int branchCount, branchID = _branchID;

			// filter branches
			if( _currentFilter )
			{
				branchCount = _currentFilter->GetBranchCount();
				branchID = _currentFilter->GetFilteredID( branchID );
			}
			else
				branchCount = _branchGroup->GetBranchCount();

			// branch should execute work?
			if( branchID >= 0 )
			{
				// eqaul distribution
				branchWork = totalWork / branchCount;

				// if the work cannot be equally distributed, some threads will do a little more
				if( branchID < totalWork % branchCount )
					++branchWork;
			}
			else
				branchWork = 0;
		}

		// Calculate distribution of workload among branches
		void GaBranch::SplitWork(int totalWork,
			int& branchWork,
			int& branchStart)
		{
			int branchCount, branchID = _branchID;

			// filter branches
			if( _currentFilter )
			{
				branchCount = _currentFilter->GetBranchCount();
				branchID = _currentFilter->GetFilteredID( branchID );
			}
			else
				branchCount = _branchGroup->GetBranchCount();

			// branch should execute work?
			if( branchID >= 0 )
			{
				// eqaul distribution
				branchWork = totalWork / branchCount;
				int left = totalWork % branchCount;

				// if the work cannot be equally distributed, some threads will do a little more
				branchStart = branchID < left ? ++branchWork * branchID : branchWork * branchID + left;
			}
			else
				branchWork = branchStart = 0;
		}

		// Returns barrier that should be used for synchonizing branches
		Threading::GaBarrier& GaBranch::GetBarrier() { return _currentFilter && _currentFilter->IsActive() ? _currentFilter->GetBarrier() : _branchGroup->GetBarrier(); }

		// Returns number of branches that must reach barrier
		int GaBranch::GetBarrierCount() const { return _currentFilter ? _currentFilter->GetBranchCount() : _branchGroup->GetBranchCount(); }

		// Returns filtered branch ID
		int GaBranch::GetFilteredID() const { return _currentFilter ? _currentFilter->GetFilteredID( _branchID ) : _branchID; }

		// Starts waiting process of the barrier
		bool GaAbstractBarrier::Enter(GaBranch* branch)
		{
			// only the last branch that hits this barrier can start outgoing branch groups
			if( ++_branchGroupHitCount == NumberOfInboundBranchGroups() )
			{
				_branchGroupHitCount = 0;

				GaWorkflow* flow = branch ? branch->GetBranchGroup().GetWorkflow() : (GaWorkflow*)_flow ;

				// start branch groups only if the workflow is in running state
				if( flow->CheckWorkflowState() )
					return true;
			}

			return false;
		}

		// Initializes branch group
		GaBranchGroup::GaBranchGroup(GaWorkflow* workflow,
			int numberOfBranches) : GaFlowConnection(0),
			_workflow(workflow),
			_data(GADSL_BRANCH_GROUP, workflow->GetGlobalData(), workflow->GetWorkflowData()),
			_branchGroupFlow(this),
			_lastStep(NULL) { SetBranchCount( numberOfBranches ); }

		// Closes all branches and frees used resources
		GaBranchGroup::~GaBranchGroup()
		{
			// delete branch objects
			for( std::vector<GaBranch*>::iterator it = _branches.begin(); it != _branches.end(); ++it )
				delete *it;
		}

		// Starts all branches
		void GaBranchGroup::StartBranches(GaFlowStep* firstStep/* = NULL */)
		{
			// use default first step if user has not specified the first step
			if( !firstStep )
				firstStep = _branchGroupFlow.GetFirstStep();
			// check whether the specified first step belongs to flow of this branch group
			else if( firstStep->GetFlow() != &_branchGroupFlow )
				firstStep = NULL;

			// restart branch group state
			_numberOfActiveBranches = (int)_branches.size();
			_lastStep = NULL;

			// start all branches
			for( std::vector<GaBranch*>::iterator it = _branches.begin(); it != _branches.end(); ++it )
				( *it )->StartBranch( firstStep );
		}

		// Called by the branch when it finish execution of the flow and it has to execute last step
		void GaBranchGroup::ExecuteBranchLastStep(GaBranch* branch)
		{
			// decrement number of running branches
			// the last branch of the group executs exit barrier
			if( --_numberOfActiveBranches == 0 ) 
			{
				// retrive exit barrier and execute it
				GaAbstractBarrier* last = _lastStep ? _lastStep : (GaAbstractBarrier*)_inboundStep;
				if( last )
					( *last )( branch );
			}

			// nofify workflow that branch has finished
			_workflow->BranchEnd();
		}

		// Sets new number of brances in the group
		void GaBranchGroup::SetBranchCount(int count)
		{
			if( count < 1 )
				count = 1;

			// remove or add branches?
			int diff = count - (int)_branches.size();
			if( diff < 0 )
			{
				// new number of branches is lower then pevious
				// delete unneeded branches
				for( int i = (int)_branches.size() - 1; i >= count; i-- )
					delete _branches[ i ];

				_branches.resize( count );
			}
			else
			{
				// new number of branches is greater then pevious
				// add new branches
				for( int i = diff; i > 0; i-- )
					_branches.push_back( new GaBranch( this, (int)_branches.size() ) );
			}

			// notify steps that branch count has been changed
			std::set<GaFlowStep*>& steps = _branchGroupFlow.GetFlowSteps();
			for( std::set<GaFlowStep*>::iterator it = steps.begin(); it != steps.end(); ++it )
				( *it )->FlowUpdated();
		}

		// Checks whether the parent branches of this branch is compatible with specified branch
		bool GaBranchGroup::CheckCompatibilityUp(const GaBranchGroup* compatibleGroup,
			const GaBranchGroup* ignoreGroup) const
		{
			// this branch is compatible with specified branch
			if( this == compatibleGroup )
				return true;

			// this branch group has no compatible parents 
			if( !_outboundStep || !_outboundStep->HasInboundConnections() ||
				( _outboundStep->GetInboundConnections().size() == 1 && *_outboundStep->GetInboundConnections().begin() == ignoreGroup ) )
				// it group is not compatible with specified group
				return false;

			// check compatibility of parent branches
			const std::set<GaFlowConnection*>& parents = _outboundStep->GetInboundConnections();
			for( std::set<GaFlowConnection*>::const_iterator it = parents.begin(); it != parents.end(); ++it )
			{
				if( compatibleGroup != ignoreGroup && !( (GaBranchGroup*)*it )->CheckCompatibilityUp( compatibleGroup, ignoreGroup ) )
					return false;
			}

			// this group is compatible if all parent branches is compatible
			return true;
		}

		// Checks whether the child branches of this branch is compatible with specified branch
		bool GaBranchGroup::CheckCompatibilityDown(const GaBranchGroup* compatibleGroup,
			const GaBranchGroup* ignoreGroup) const
		{
			// this branch is compatible with specified branch
			if( this == compatibleGroup )
				return true;

			// this branch group has no compatible parents 
			if( !_inboundStep || !_inboundStep->HasOutboundConnections() ||
				( _inboundStep->GetOutboundConnections().size() == 1 && *_inboundStep->GetOutboundConnections().begin() == ignoreGroup ) )
				// it group is not compatible with specified group
				return false;

			// check compatibility of child branches
			const std::set<GaFlowConnection*>& children = _inboundStep->GetOutboundConnections();
			for( std::set<GaFlowConnection*>::const_iterator it = children.begin(); it != children.end(); ++it )
			{
				if( compatibleGroup != ignoreGroup && !( (GaBranchGroup*)*it )->CheckCompatibilityDown( compatibleGroup, ignoreGroup ) )
					return false;
			}

			// this group is compatible if all child branches is compatible
			return true;
		}

		// Attaches next step
		void GaSimpleWorkStep::AttachNextStep(GaFlowConnection* nextStep)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, nextStep != NULL, "nextStep", "Connection object must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, nextStep->GetOutboundStep() == this, "nextStep", "Wrong connection object.", "Workflows" );
			GA_ASSERT( Exceptions::GaInvalidOperationException, !HasOutboundConnections(), "Outbound point of the step is already connected.", "Workflows" );

			// attach and add to list of outbound connections
			AddOutboundConnection( nextStep );
		}

		// Detaches next step
		void GaSimpleWorkStep::DetachNextStep(GaFlowConnection* nextStep)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, nextStep != NULL, "nextStep", "Connection object must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, HasOutboundConnection( nextStep ), "nextStep", "Wrong connection object.", "Workflows" );

			// detach and remove froma to list of outbound connections
			RemoveOutboundConnection( nextStep );
		}

		// Returns next step that should be executed by the branch
		GaFlowStep* GaBinaryDecision::GetNextStep(GaBranch* branch)
		{ 
			GaFlowConnection* next = _branches[ branch->GetLastDecision() ];
			return  next ? next->GetInboundStep() : NULL;
		}

		// Attaches next step
		void GaBinaryDecision::AttachNextStep(GaFlowConnection* nextStep)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, nextStep != NULL, "nextStep", "Connection object must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, nextStep->GetOutboundStep() == this, "nextStep", "Wrong connection object.", "Workflows" );

			int branch = nextStep->GetConnectionID() ? 1 : 0;

			GA_ASSERT( Exceptions::GaInvalidOperationException, _branches[ branch ] == NULL, "Outbound point required by the step is already connected.", "Workflows" );

			// add connection to decision table
			_branches[ branch ] = nextStep;
			AddOutboundConnection( nextStep );
		}

		// Detaches next step
		void GaBinaryDecision::DetachNextStep(GaFlowConnection* nextStep)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, nextStep != NULL, "nextStep", "Connection object must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, HasOutboundConnection( nextStep ), "nextStep", "Wrong connection object.", "Workflows" );

			// remove connection from decision table
			_branches[ nextStep->GetConnectionID() ? 1 : 0 ] = NULL;
			RemoveOutboundConnection( nextStep );
		}

		// Updates decision table when ID of connection object is changed
		void GaBinaryDecision::ConnectionIDChanged(GaFlowConnection* connection,
			int oldConnectionID)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, connection != NULL, "connection", "Connection object must be specified.", "Workflows" );

			if( !HasOutboundConnection( connection ) )
			{
				if( HasInboundConnection( connection ) )
					return;

				GA_ARG_THROW( Exceptions::GaArgumentException, "connection", "Wrong connection object.", "Workflows" );
			}

			int oldBranch = oldConnectionID ? 1 : 0;
			int newBranch = connection->GetConnectionID() ? 1 : 0;

			GA_ARG_ASSERT( Exceptions::GaArgumentException, _branches[ oldBranch ] == connection, "connection", "Wrong connection object.", "Workflows" );
			GA_ASSERT( Exceptions::GaInvalidOperationException, _branches[ newBranch ] == NULL, "Outbound point required by the step is already connected.", "Workflows" );

			// update decision table
			_branches[ newBranch ] = _branches[ oldBranch ];
			_branches[ oldBranch ] = NULL;
		}

		// Returns next step that should be executed by the branch
		GaFlowStep* GaDecision::GetNextStep(GaBranch* branch)
		{
			STLEXT::hash_map<int, GaFlowConnection*>::iterator it = _branches.find( branch->GetLastDecision() );
			return it != _branches.end() ? it->second->GetInboundStep() : NULL;
		}

		// Attaches next step
		void GaDecision::AttachNextStep(GaFlowConnection* nextStep)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, nextStep != NULL, "nextStep", "Connection object must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, nextStep->GetOutboundStep() == this, "nextStep", "Wrong connection object.", "Workflows" );

			int branch = nextStep->GetConnectionID();
			GA_ASSERT( Exceptions::GaInvalidOperationException, _branches.find( branch ) == _branches.end(), "Outbound point of the step is already connected.", "Workflows" );

			// add connection to decision table
			_branches[ branch ] = nextStep;
			AddOutboundConnection( nextStep );
		}

		// Detaches next step
		void GaDecision::DetachNextStep(GaFlowConnection* nextStep)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, nextStep != NULL, "nextStep", "Connection object must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, HasOutboundConnection( nextStep ), "nextStep", "Wrong connection object.", "Workflows" );

			// remove connection from decision table
			_branches.erase( nextStep->GetConnectionID() );
			RemoveOutboundConnection( nextStep );
		}

		// Updates decision table when ID of connection object is changed
		void GaDecision::ConnectionIDChanged(GaFlowConnection* connection,
			int oldConnectionID)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, connection != NULL, "connection", "Connection object must be specified.", "Workflows" );

			if( !HasOutboundConnection( connection ) )
			{
				if( HasInboundConnection( connection ) )
					return;

				GA_ARG_THROW( Exceptions::GaArgumentException, "connection", "Wrong connection object.", "Workflows" );
			}

			int newBranch = connection->GetConnectionID();

			GA_ARG_ASSERT( Exceptions::GaArgumentException, _branches[ oldConnectionID ] == connection, "connection", "Wrong connection object.", "Workflows" );
			GA_ASSERT( Exceptions::GaInvalidOperationException, _branches.find( newBranch ) == _branches.end(), "Outbound point of the step is already connected.", "Workflows" );

			// update decision table
			_branches[ newBranch ] = _branches[ oldConnectionID ];
			_branches.erase( oldConnectionID );
		}

		// Verifies whether the branch can pass step's filter
		bool GaFilteredStep::Enter(GaBranch* branch)
		{
			// is this branch filtered out?
			if( _branchFilter.CanExecute( branch->GetBranchID() ) )
			{
				branch->SetCurrentFilter( &_branchFilter );
				return true;
			}

			return false;
		}

		// Synchronizes branches when step finishes its work if needed
		void GaFilteredStep::Exit(GaBranch* branch)
		{
			if( _syncOnExit )
				branch->GetBarrier().Enter( true, branch->GetBarrierCount() );
		}

		// Transfers control to outgoing branch group 
		void GaBranchGroupTransition::operator ()(GaBranch* branch) 
		{ 
			if( !_outboundConnections.empty() )
			{
				GaFlowStep* step = ( *_outboundConnections.begin() )->GetInboundStep();
				( (GaBranchGroupFlow*)step->GetFlow() )->GetBranchGroup().StartBranches( step );
			}
		}

		// Sets last step for all branches of the group and synchronize them
		bool GaBranchGroupTransition::Enter(GaBranch* branch)
		{
			// synchronize all branches at the end
			if( branch->IsExecutingLastStep() )
				return GaAbstractBarrier::Enter( branch );

			// set this step as last step for all branches in the group if it's not already set
			branch->GetBranchGroup().SetLastStep( this );
			return false;
		}

		// Attaches outgoing branch group
		void GaBranchGroupTransition::AttachNextStep(GaFlowConnection* nextStep)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, nextStep != NULL, "nextStep", "Connection object must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, nextStep->GetOutboundStep() == this, "nextStep", "Wrong connection object.", "Workflows" );
			GA_ASSERT( Exceptions::GaInvalidOperationException, !HasOutboundConnections(), "Outbound point of the step is already connected.", "Workflows" );

			// attach outgoing branch group
			AddOutboundConnection( nextStep );
		}

		// Detaches outgoing branch group
		void GaBranchGroupTransition::DetachNextStep(GaFlowConnection* nextStep)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, nextStep != NULL, "nextStep", "Connection object must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, HasOutboundConnection( nextStep ), "nextStep", "Wrong connection object.", "Workflows" );

			// detach outgoing branch group
			RemoveOutboundConnection( nextStep );
		}

		// Starts all outgoing branch groups
		void GaWorkflowBarrier::operator ()(GaBranch* branch)
		{
			// restar branch group counter
			_branchGroupHitCount = 0;

			for( std::set<GaFlowConnection*>::iterator it = _outboundConnections.begin(); it != _outboundConnections.end(); ++it )
				( (GaBranchGroup*)*it )->StartBranches();
		}

		// Attaches outgoing branch group
		void GaWorkflowBarrier::AttachNextStep(GaFlowConnection* nextStep)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, nextStep != NULL, "nextStep", "Connection object must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, nextStep->GetOutboundStep() == this && !HasOutboundConnection( nextStep ), "nextStep", "Wrong connection object.", "Workflows" );

			// attach outgoing branch group
			AddOutboundConnection( nextStep );
		}

		// Detaches outgoing branch group
		void GaWorkflowBarrier::DetachNextStep(GaFlowConnection* nextStep)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, nextStep != NULL, "nextStep", "Connection object must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, HasOutboundConnection( nextStep ), "nextStep", "Wrong connection object.", "Workflows" );

			// detach outgoing branch group
			RemoveOutboundConnection( nextStep );
		}

		// Initializes workflow
		GaWorkflow::GaWorkflow(GaDataStorage* globalData) : _globalData(globalData),
			_workflowData(GADSL_WORKFLOW, globalData),
			_state(GAWS_STOPPED)
		{
			// make synchronization event objects
			_stateChangeEvent = Threading::GaEventPool::GetInstance()->GetEventWithAutoPtr( Threading::GaEvent::GAET_AUTO );
			_pauseEvent = Threading::GaEventPool::GetInstance()->GetEventWithAutoPtr( Threading::GaEvent::GAET_MANUAL );
			_endEvent = Threading::GaEventPool::GetInstance()->GetEventWithAutoPtr( Threading::GaEvent::GAET_MANUAL );

			// make the first and the last step of the workflow and connect them with branch group
			_firstStep = new GaStartStep();
			_lastStep = new GaFinishStep();
			AddStep( _firstStep );
			AddStep( _lastStep );
			ConnectSteps( _firstStep, _lastStep, 0 );
		}

		// Frees used resources
		GaWorkflow::~GaWorkflow()
		{
			// it is required to destory branch group transition connection object first
			for( std::set<GaFlowConnection*>::iterator it = _branchGroupTransitions.begin(); it != _branchGroupTransitions.end(); ++it )
				delete *it;

			// destory branch groups
			for( std::set<GaFlowConnection*>::iterator it = _branchGroups.begin(); it != _branchGroups.end(); ++it )
				delete *it;

			// destory workflow barriers
			for( std::set<GaAbstractBarrier*>::iterator it = _steps.begin(); it != _steps.end(); ++it )
				delete *it;
		}

		// Inserts step to the workflow
		void GaWorkflow::AddStep(GaFlowStep* step)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, step != NULL, "step", "Step must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, !step->HasConnections(), "step", "Step is connected to the other steps which might not be members of this flow.", "Workflows" );

			// mark the step as a memeber of this flow and insert it
			step->BindToFlow( this );
			_steps.insert( (GaAbstractBarrier*)step );
		}

		// Removes step from the workflow
		void GaWorkflow::RemoveStep(GaFlowStep* step,
			bool disconnect,
			bool destroy)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, step != NULL, "step", "Step must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaArgumentException, step->GetFlow() == this, "step", "Step does not belong to this flow.", "Workflows" );
			GA_ASSERT( Exceptions::GaInvalidOperationException, step != _firstStep && step != _lastStep, "Cannot remove start and finish steps of the workflow.", "Workflows" );
			GA_ASSERT( Exceptions::GaInvalidOperationException, disconnect || !step->HasConnections(),
				"Trying to remove step which is connected to the other steps of this flow without disconnecting it.", "Workflows" );

			// disconnect step from all other steps
			if( disconnect )
				step->DisconnectAllConnections();

			// disconnect step from all other steps
			step->UnbindFromFlow();
			_steps.erase( (GaAbstractBarrier*)step );
			if( destroy )
				delete step;
		}

		// Connects two step of the workflow
		GaFlowConnection* GaWorkflow::ConnectSteps(GaFlowStep* outboundStep,
			GaFlowStep* inboundStep,
			int connectionID)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, outboundStep != NULL, "outboundStep", "Outbound step must be specified.", "Workflows" );
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, inboundStep != NULL, "inboundStep", "Inbound step must be specified.", "Workflows" );

			// add step to flow it is not member of any other flow
			if( !outboundStep->GetFlow() )
				AddStep( outboundStep );
			
			// add step to flow it is not member of any other flow
			if( !inboundStep->GetFlow() )
				AddStep( inboundStep );

			// check when connectiong steps with branch group tranistion connection
			GA_ARG_ASSERT( Exceptions::GaArgumentException, connectionID != 0 || ( outboundStep->GetFlow() == this && inboundStep->GetFlow() == this ),
				"outboundStep or inboundStep", "Specified steps do not belong to this workflow.", "Workflows" );

			// check when connectiong steps with check for branch group
			GA_ARG_ASSERT( Exceptions::GaArgumentException, connectionID == 0 || ( ( (GaBranchGroupFlow*)outboundStep->GetFlow() )->GetBranchGroup().GetWorkflow() == this && 
				( (GaBranchGroupFlow*)inboundStep->GetFlow() )->GetBranchGroup().GetWorkflow() == this ),
				"outboundStep or inboundStep", "Specified steps do not belong to this workflow.", "Workflows" );

			// make connection object depending on required type of connection
			Memory::GaAutoPtr<GaFlowConnection> connection = connectionID ? (GaFlowConnection*)new GaBranchGroupTransitionConnection() : (GaFlowConnection*)new GaBranchGroup( this, 1 );

			// connect steps
			connection->ConnectSteps( outboundStep, inboundStep );
			
			if( connectionID )
				// add branch group transition to the workflow
				_branchGroupTransitions.insert( connection.GetRawPtr() );
			else
			{
				// add branch group to the workflow
				_branchGroups.insert( connection.GetRawPtr() );

				// check whether the adding of the new branch group has broken validity of branch group transitions
				if( !CheckBranchGroupTransitionConnections() )
				{
					// remove branch group
					connection->DisconnectSteps();
					_branchGroups.erase( connection.GetRawPtr() );

					GA_THROW( Exceptions::GaInvalidOperationException, "Connecting specified steps can make some of the branch group transition connections invalid.", "Workflows" );
				}
			}

			return connection.DetachPointer();
		}

		// Disconnects steps connected with specified connection object and removes it from the workflow
		void GaWorkflow::RemoveConnection(GaFlowConnection* connection,
			bool destory)
		{
			GA_ARG_ASSERT( Exceptions::GaNullArgumentException, connection != NULL, "connection", "Connection object must be specified.", "Workflows" );

			// is this connection branch group?
			if( _branchGroups.find( connection ) != _branchGroups.end() )
			{
				// check whether the removing of the branch group has breaken validity of branch group transitions
				GA_ASSERT( Exceptions::GaInvalidOperationException, CheckBranchGroupTransitionConnections( (GaBranchGroup*)connection ),
					"Disconnecting specified steps can make some of the branch group transition connections invalid.", "Workflows" );

				// disconnect attached steps and removes branch group from the workflow
				connection->DisconnectSteps();
				_branchGroups.erase( connection );
				if( destory )
					delete connection;
			}
			// is this connection branch group transition?
			else if( _branchGroupTransitions.find( connection ) != _branchGroupTransitions.end() )
			{
				// disconnect attached steps and removes branch group transition connection from the workflow
				connection->DisconnectSteps();
				_branchGroupTransitions.erase( connection );
				if( destory )
					delete connection;
			}
			else
				GA_ARG_THROW( Exceptions::GaArgumentException, "connection", "Specified connection does not belong to this flow.", "Workflows" );
		}

		// Starts execution of the workflow
		void GaWorkflow::Start()
		{
			GA_LOCK_THIS_OBJECT( lock );

			// start workflow only if it was stopped
			if( Threading::GaAtomicOps<GaWorkflowState>::CmpXchg( &_state, GAWS_STOPPED, GAWS_RUNNING ) )
			{
				_stateChangeEvent->Reset();
				_endEvent->Reset();

				// start execution
				(*_firstStep)( NULL );
			}
			else
				GA_THROW( Exceptions::GaInvalidOperationException, "Workflow is already running or it is paused.", "Workflows" );
		}

		// Resumes execution of the workflow
		void GaWorkflow::Resume()
		{
			GA_LOCK_THIS_OBJECT( lock );

			// resume execution of the workflow only if it was paused
			if( Threading::GaAtomicOps<GaWorkflowState>::CmpXchg( &_state, GAWS_PAUSED, GAWS_RUNNING ) )
			{
				// release all active branch groups and barriers
				_pauseEvent->Signal();

				// wait for all barriers and branch groups to resume their execution
				_stateChangeEvent->Wait();
			}
			else
				GA_THROW( Exceptions::GaInvalidOperationException, "Cannot resume workflow which is not paused.", "Workflows" );
		}

		// Pauses execution of the workflow
		void GaWorkflow::Pause()
		{
			GA_LOCK_THIS_OBJECT( lock );

			// pause workflow only if it was running
			if( Threading::GaAtomicOps<GaWorkflowState>::CmpXchg( &_state, GAWS_RUNNING, GAWS_PAUSED ) )
				// wait for all barriers and branch groups to pause their execution
				_stateChangeEvent->Wait();
			else
				GA_THROW( Exceptions::GaInvalidOperationException, "Workflow is already paused or it is stopped.", "Workflows" );
		}

		// Stops execution of the workflow
		void GaWorkflow::Stop()
		{
			GA_LOCK_THIS_OBJECT( lock );

			// stop workflow only if it was running
			if( Threading::GaAtomicOps<GaWorkflowState>::CmpXchg( &_state, GAWS_RUNNING, GAWS_STOPPED ) ) 
				// wait for all barriers and branches to finish their execution 
				_stateChangeEvent->Wait();
			// or if it was paused
			else if( Threading::GaAtomicOps<GaWorkflowState>::CmpXchg( &_state, GAWS_PAUSED, GAWS_STOPPED ) )
			{
				// release suspended branches and barriers and wait for them to finish their execution
				_pauseEvent->Signal();
				_stateChangeEvent->Wait();
			}
		}

		// Decrements number of active branches and stops workflow if there is no more active branches
		void GaWorkflow::BranchEnd()
		{
			// decrement barrier counter
			if( --_counters[ GACT_ACTIVE_BRANCHES ] == 0 )
			{
				// no more active branches left, change state to stopped state, signal that change
				// and wake threads that waits for workflow to finish execution
				_state = GAWS_STOPPED;
				_endEvent->Signal();
				_stateChangeEvent->Signal();
			}
		}

		// Checks current state of the workflow
		bool GaWorkflow::CheckWorkflowState()
		{
			// user paused workflow
			if( _state == GAWS_PAUSED )
			{
				// increment number of paused branches
				if( ++_counters[ GACT_PAUSED_BRANCHES ] == _counters[ GACT_ACTIVE_BRANCHES ] )
					// if all branches are paused - signal successful state change
					_stateChangeEvent->Signal();

				// wait for user to resume
				_pauseEvent->Wait();

				// decrement number of paused branches
				if( --_counters[ GACT_PAUSED_BRANCHES ] == 0 && _state == GAWS_RUNNING )
				{
					// if all branches are released - signal successful state change
					_pauseEvent->Reset();
					_stateChangeEvent->Signal();
				}
			}

			// user stopped workflow
			if( _state == GAWS_STOPPED )
				return false;

			return true;
		}

		// Checks validity of branch group transition connections
		bool GaWorkflow::CheckBranchGroupTransitionConnections(const GaBranchGroup* ignoreConnection/* = NULL*/)
		{
			for( std::set<GaFlowConnection*>::iterator it = _branchGroupTransitions.begin(); it != _branchGroupTransitions.end(); ++it )
			{
				// check compatiblility of branch groups connected eith branch group tranisition connection
				if( !( (GaBranchGroupFlow*)( *it )->GetInboundStep()->GetFlow() )->GetBranchGroup().CheckBranchGroupCompatibility(
					&( (GaBranchGroupFlow*)( *it )->GetOutboundStep()->GetFlow() )->GetBranchGroup(), ignoreConnection ) )
					return false;
			}

			return true;
		}

	} // Workflows
} // Common

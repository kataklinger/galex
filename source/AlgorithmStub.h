
/*! \file AlgorithmStub.h
	\brief This file declares basic classes and datatypes for algorithm stubs.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_ALGORITHM_STUB_H__
#define __GA_ALGORITHM_STUB_H__

#include "Workflows.h"
#include "Population.h"

namespace Algorithm
{

	/// <summary><c>GaBranchGroupDataIDs</c> contains workflow data storge IDs reserved for usage by builtin algorithm stubs.</summary>
	enum GaBranchGroupDataIDs
	{

		/// <summary>Data storage ID of selection operation output.</summary>
		GADID_SELECTION_OUTPUT = 0x8001,

		/// <summary>Data storage ID of coupling operation output.</summary>
		GADID_COUPLING_OUTPUT,

	};

	/// <summary><c>GaAlgorithmStub</c> is base class for algorithm stubs. Stub is independent part of genetic algorithm contained in a single branch group.
	///
	/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaAlgorithmStub
	{

	protected:

		/// <summary>Branch group that contains and executes stub.</summary>
		Common::Workflows::GaBranchGroup* _brachGroup;

		/// <summary>Number of branches that branch group has.</summary>
		int _branchCount;

	public:

		/// <summary>Initailizes disconnected algorithm stub.</summary>
		GaAlgorithmStub() : _brachGroup(NULL),
			_branchCount(1) { }

		/// <summary>Virtual destructor must be defined because this is base class.</summary>
		virtual ~GaAlgorithmStub() { }

		/// <summary><c>Connect</c> method connects algorithm stub to existing workflow.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="fork">workflow barrier that will fork new branch group that will execute algorithm stub.</param>
		/// <param name="join">workflow barrier that will join branch group that executes algorithm stub with the reset of the workflow.</param>
		/// <exception cref="GaNullArgumentException" />Thrown if <c>fork</c> or <c>join</c> is set to <c>NULL</c>.</exception>
		GAL_API
		void GACALL Connect(Common::Workflows::GaWorkflowBarrier* fork,
			Common::Workflows::GaWorkflowBarrier* join);

		/// <summary><c>Disconnect</c> method disconnects algorithm stub from the workflow.
		///
		/// This method is not thread-safe.</summary>
		/// <exception cref="GaInvalidOperationException" />Thrown if stub is not connected to any workflow.</exception>
		GAL_API
		void GACALL Disconnect();

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to flow that contains steps executed by branch group which represents algorithm stub.</returns>
		inline Common::Workflows::GaBranchGroupFlow& GetStubFlow() { return *_brachGroup->GetBranchGroupFlow(); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to flow that contains steps executed by branch group which represents algorithm stub.</returns>
		inline const Common::Workflows::GaBranchGroupFlow& GetStubFlow() const { return *_brachGroup->GetBranchGroupFlow(); }

		/// <summary><c>SetBranchCount</c> method sets number of branches that will execute algorithm stub.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="branchCount">number of branches that will execute stub.</param>
		GAL_API
		void GACALL SetBranchCount(int branchCount);

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns number of branches that execute algorithm stub.</returns>
		inline int GACALL GetBranchCount() const { return _branchCount; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if the algorithm stub is attached to a workflow.</returns>
		inline bool GACALL IsConnected() const { return _brachGroup != NULL; }

	protected:

		/// <summary><c>Connected</c> method is called after the stub is successfully attached to a workflow.
		/// This method should be overidden by inherited classes to prepare workflow and stub for its execution.</summary>
		virtual void GACALL Connected() = 0;

		/// <summary><c>Connected</c> method is called after the stub is successfully attached to a workflow.
		/// This method should be overidden by inherited classes to prepare workflow and stub for its execution.</summary>
		virtual void GACALL Disconnecting() = 0;

	};

	/// <summary>Defines workflow storage cache for <c>GaPopulation</c> type.</summary>
	typedef Common::Workflows::GaDataCache<Population::GaPopulation> GaCachedPopulation;

	/// <summary><c>GaCheckPopulationStep</c> class represents workflow step that checks whether the population is initialized.
	///
	/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaCheckPopulationStep : public Common::Workflows::GaBinaryDecision
	{

	protected:

		/// <summary>Population that should be checked.</summary>
		GaCachedPopulation _population;

	public:

		/// <summary>Initializes check step for specific population.</summary>
		/// <param name="dataStorage">stoage that contains population which chould be checked.</param>
		/// <param name="dataID">ID of the population object.</param>
		GaCheckPopulationStep(Common::Workflows::GaDataStorage* dataStorage,
			int dataID) { SetPopulation( dataStorage, dataID ); }

		/// <summary>Initializes check step without specifying population.</summary>
		GaCheckPopulationStep() { }

		/// <summary><c>SetPopulation</c> method sets population that should be checked by this step.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="dataStorage">stoage that contains population which chould be checked.</param>
		/// <param name="dataID">ID of the population object.</param>
		inline void GACALL SetPopulation(Common::Workflows::GaDataStorage* dataStorage,
			int dataID) { _population.SetData( dataStorage, dataID ); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns ID of the population that is checked.</returns>
		inline int GACALL GetInputDataID() const { return _population.GetDataID(); }

	protected:

		/// <summary><c>Decision</c> method test population to see if it is initialized.</summary>
		/// <param name="branch">pointer branch that execute the step.</param>
		/// <returns>Method returns result <c>true</c> if the population is initialized.</returns>
		virtual bool GACALL Decision(Common::Workflows::GaBranch* branch) { return _population.GetData().GetFlags().IsFlagSetAny( Population::GaPopulation::GAPF_INITIALIZED ); }

	};

	/// <summary><c>GaBasicStub</c> class represents base class for algorithm stubs which operates on a single population.
	///
	/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaBasicStub : public GaAlgorithmStub
	{

	protected:

		/// <summary>ID of the population on which the algorithm stub operates.</summary>
		int _populationID;

		/// <summary>ID of object that stores statistical information about algorithm.</summary>
		int _statisticsID;

		/// <summary>Parameters of the population on which the stub operates.</summary>
		Population::GaPopulationParams _populationParameters;

		/// <summary>Initialization operation for the population.</summary>
		Chromosome::GaInitializatorSetup _initializator;

		/// <summary>Fitness operation that evaluates chromosomes in the population.</summary>
		Population::GaPopulationFitnessOperationSetup _fitnessOperation;

		/// <summary>Fitness comparator ised by the population.</summary>
		Fitness::GaFitnessComparatorSetup _comparator;

	public:

		/// <summary>This constructor initializes stub with population on which stub will operate and object that will store statistical information.</summary>
		/// <param name="populationID">ID of the population on which the stub will operate.</param>
		/// <param name="statisticsID">ID of the object that whill store statistical information.</param>
		GaBasicStub(int populationID,
			int statisticsID) : _populationID(populationID),
			_statisticsID(statisticsID) { }

		/// <summary>This constructor initializes stub without specifying population on which it will operate.</summary>
		GaBasicStub() : _populationID(-1),
			_statisticsID(-1) { }

		/// <summary><c>SetPopulationID</c> method sets population on which the stub will operate.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="id">ID of the population.</param>
		/// <exception cref="GaInvalidOperationException" />Thrown if stub is connected to a workflow.</exception>
		inline void GACALL SetPopulationID(int id)
		{
			GA_ASSERT( Common::Exceptions::GaInvalidOperationException, !IsConnected(), "ID cannot be changed while stub is in use.", "Algorithms" );
			_populationID = id;
		}

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns ID of the population on which the algorithm stub operates.</returns>
		inline int GACALL GetPopulationID() const { return _populationID; }

		/// <summary><c>SetStatisticsID</c> method sets statistical object that will be used by algorithm stub. 
		///
		/// This method is not thread-safe.</summary>
		/// <param name="id">ID of object that will store statistical information.</param>
		/// <exception cref="GaInvalidOperationException" />Thrown if stub is connected to a workflow.</exception>
		inline void GACALL SetStatisticsID(int id)
		{
			GA_ASSERT( Common::Exceptions::GaInvalidOperationException, !IsConnected(), "ID cannot be changed while stub is in use.", "Algorithms" );
			_statisticsID = id;
		}

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns ID of object that stores statistical information about algorithm.</returns>
		inline int GACALL GetStatisticsID() const { return _statisticsID; }

	protected:

		/// <summary><c>Connected</c> method prepares workflow.</summary>
		GAL_API
		virtual void GACALL Connected();

		/// <summary><c>Disconnecting</c> method cleans workflow.</summary>
		GAL_API
		virtual void GACALL Disconnecting();

		/// <summary><c>UpdateInitializator</c> method sets new population initialization operation.</summary>
		/// <param name="initializator">new initialization operation.</param>
		inline void GACALL UpdateInitializator(const Chromosome::GaInitializatorSetup& initializator)
		{
			_initializator = initializator;

			if( IsConnected() )
				GaCachedPopulation( GetWorkflowStorage(), _populationID ).GetData().SetInitializator( _initializator );
		}

		/// <summary><c>UpdateFitnessOperation</c> method sets new fitness operation for evaluation population's chromosomes.</summary>
		/// <param name="fitnessOperation">new fitness operation.</param>
		inline void GACALL UpdateFitnessOperation(const Population::GaPopulationFitnessOperationSetup& fitnessOperation)
		{
			_fitnessOperation = fitnessOperation;

			if( IsConnected() )
				GaCachedPopulation( GetWorkflowStorage(), _populationID ).GetData().SetFitnessOperation( _fitnessOperation );
		}

		/// <summary><c>UpdateFitnessComparator</c> method sets new fitness comparator used by the population.</summary>
		/// <param name="comparator">new fitness comparator.</param>
		inline void GACALL UpdateFitnessComparator(const Fitness::GaFitnessComparatorSetup& comparator)
		{
			_comparator = comparator;

			if( IsConnected() )
				GaCachedPopulation( GetWorkflowStorage(), _populationID ).GetData().SetFitnessComparator( _comparator );
		}

		/// <summary><c>UpdatePopulationParameters</c> method sets new parameters for the population.</summary>
		/// <param name="parameters">new population parameters.</param>
		inline void GACALL UpdatePopulationParameters(const Population::GaPopulationParams& parameters)
		{
			_populationParameters = parameters;

			if( IsConnected() )
				GaCachedPopulation( GetWorkflowStorage(), _populationID ).GetData().SetPopulationParams( _populationParameters );
		}

		/// <summary><c>GetWorkflowStorage</c> method returns data storage used by the workflow.</summary>
		/// <returns>Method returns pointer to data storage used by the workflow.</returns>
		inline Common::Workflows::GaDataStorage* GetWorkflowStorage() { return _brachGroup->GetWorkflow()->GetWorkflowData(); }

	};

} // Algorithm

#endif // __GA_ALGORITHM_STUB_H__

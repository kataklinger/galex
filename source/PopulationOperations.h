
/*! \file PopulationOperations.h
    \brief This file declares interfaces and classes needed to implement genetic operations which are performed over population.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#ifndef __GA_POPULATION_OPERATIONS_H__
#define __GA_POPULATION_OPERATIONS_H__

#include "Workflows.h"
#include "ChromosomeGroup.h"

namespace Population
{

	class GaPopulation;

	/// <summary><c>GaPopulationFitnessOperation</c> class is interface for fitness operations that evaluates fitness values of chromosomes in population.
	/// This interface defines two type of fitness evaluation: individual-based and population-based.</summary>
	class GaPopulationFitnessOperation : public Chromosome::GaChromosomeFitnessOperation
	{

	public:

		/// <summary>This operator should performs individual-based evaluation of fitness values of specified chromosome.</summary>
		/// <param name="object">reference to chromosome which should be evaluated.</param>
		/// <param name="parameters">reference to parameters of fitness operations.</param>
		/// <param name="fitness">reference to fitness object that will store evaluated fitness value of chromosome.</param>
		virtual void GACALL operator ()(const Chromosome::GaChromosome& object,
			Fitness::GaFitness& fitness,
			const Fitness::GaFitnessOperationParams& parameters) const = 0;

		/// <summary><c>AllowsIndividualEvaluation</c> method returns value that indicates whether the individual-based evaluation of chromosomes
		/// can be performed with this fitness operation.</summary>
		/// <returns>Method returns <c>true</c> if individual based evaluation of chromosomes can be performed with this operation.</returns>
		virtual bool GACALL AllowsIndividualEvaluation() const = 0;

		/// <summary><c>Prepare</c> method prepares population for execution of the fitness operation based on provided parameters and configuration.</summary>
		/// <param name="population">reference to population whose chromosomes are evaluated.</param>
		/// <param name="parameters">reference to parameters of fitness operations.</param>
		/// <param name="configuration">reference to configuration of fitness operations.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		virtual void GACALL Prepare(GaPopulation& population,
			const Fitness::GaFitnessOperationParams& parameters,
			const Common::GaConfiguration& configuration,
			int branchCount) const { }

		/// <summary><c>Clear</c> method reverse changes made by fitness operation to population.</summary>
		/// <param name="population">reference to population whose chromosomes are evaluated.</param>
		/// <param name="parameters">reference to parameters of fitness operations.</param>
		/// <param name="configuration">reference to configuration of fitness operations.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		virtual void GACALL Clear(GaPopulation& population,
			const Fitness::GaFitnessOperationParams& parameters,
			const Common::GaConfiguration& configuration,
			int branchCount) const { }

		/// <summary><c>Update</c> method updates population to reflect changes in the workflow.</summary>
		/// <param name="population">reference to population whose chromosomes are evaluated.</param>
		/// <param name="parameters">reference to parameters of fitness operations.</param>
		/// <param name="configuration">reference to configuration of fitness operations.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		virtual void GACALL Update(GaPopulation& population,
			const Fitness::GaFitnessOperationParams& parameters,
			const Common::GaConfiguration& configuration,
			int branchCount) const { }

		/// <summary>This operator should performs population-based evaluation of fitness values of chromosomes in the specified population.</summary>
		/// <param name="population">reference to population whose chromosomes are evaluated.</param>
		/// <param name="parameters">reference to parameters of fitness operations.</param>
		/// <param name="configuration">reference to configuration of fitness operations.</param>
		/// <param name="branch">pointer to workflow branch that executes fitness operation.</param>
		virtual void GACALL operator ()(GaPopulation& population,
			const Fitness::GaFitnessOperationParams& parameters,
			const Common::GaConfiguration& configuration,
			Common::Workflows::GaBranch* branch) const = 0;

	};

	/// <summary><c>GaPopulationFitnessOperationSetup</c> type is instance of <see cref="GaOperationSetup" /> template class and represents setup of
	/// population fitness operation and its parameters. Detailed description can be found in specification of
	/// <see cref="GaOperationSetup" /> template class.</summary>
	typedef Common::GaOperationSetup<GaPopulationFitnessOperation, Fitness::GaFitnessOperationParams, Fitness::GaFitnessOperationConfig> GaPopulationFitnessOperationSetup;

	/// <summary><c>GaPopulationFitnessStep</c> type is instance of <see cref="GaOperationStep1" /> template class and represents workflow step that performs 
	/// fitness value evaluation of chromosomes in the population. Detailed description can be found in specification of <see cref="GaOperationStep1" /> template class.</summary>
	typedef Common::Workflows::GaOperationStep1<GaPopulationFitnessOperationSetup, GaPopulation> GaPopulationFitnessStep;

	/// <summary><c>GaCombinedFitnessOperation</c> class represents population fitness operation that performes individual-based evaluation of all chromosomes in the population.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaCombinedFitnessOperation : public GaPopulationFitnessOperation
	{

	private:

		/// <summary>Fitness operation used to performe individual-based evaluation of chromosomes.</summary>
		Chromosome::GaChromosomeFitnessOperation* _fitnessOperation;

	public:

		/// <summary>Initializes population fitness operation with fitness operation used to individual-based evaluation of chromosomes.</summary>
		/// <param name="fitnessOperation">pointer to fitness operation used to performe individual-based evaluation of chromosomes.</param>
		GaCombinedFitnessOperation(Chromosome::GaChromosomeFitnessOperation* fitnessOperation) : _fitnessOperation(fitnessOperation) { }

		/// <summary><c>CreateFitnessObject</c> method creates fitness object required by specified fitness operation for individual-based evaluation
		/// using specified fitness parameters.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="params">smart pointer to parameters that will be used by fitness object.</param>
		/// <returns>Method returns pointer to newly created fitness object.</returns>
		virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const
			{ return _fitnessOperation->CreateFitnessObject( params ); }

		/// <summary><c>operator ()</c> performes individual-based evaluation of specified chromosomes in the population using provided fitness operation.
		/// More details are given in specification of <see cref="GaPopulationFitnessOperation::operator ()" /> method.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		virtual void GACALL operator ()(const Chromosome::GaChromosome& object,
			Fitness::GaFitness& fitness,
			const Fitness::GaFitnessOperationParams& parameters) const { ( *_fitnessOperation )( object, fitness, parameters ); }

		/// <summary>More details are given in specification of <see cref="GaPopulationFitnessOperation::AllowsIndividualEvaluation" /> method.
		///
		/// This method is not thread-safe.</summary>
		/// <returns>This method always returns <c>true</c>.</returns>
		virtual bool GACALL AllowsIndividualEvaluation() const { return true; }

		/// <summary><c>operator ()</c> performes individual-based evaluation of all chromosomes in the population using provided fitness operation.
		/// More details are given in specification of <see cref="GaPopulationFitnessOperation::operator ()" /> method.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		virtual void GACALL operator ()(GaPopulation& population,
			const Fitness::GaFitnessOperationParams& parameters,
			const Common::GaConfiguration& configuration,
			Common::Workflows::GaBranch* branch) const;

		/// <summary><c>CreateParameters</c> method creates prameters for underlying chromosome fitness operation.
		/// More details are given in specification of <see cref="GaPopulationFitnessOperation::operator ()" /> method.
		///
		/// This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to created parameters.</returns>
		virtual Common::GaParameters* GACALL CreateParameters() const { return _fitnessOperation->CreateParameters(); }

		/// <summary><c>SetFitnessOperation</c> sets fitness operation that will be used for individual-based evaluation of chromosomes.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="fitnessOperation">pointer to fitness operation used to performe individual-based evaluation of chromosomes.</param>
		inline void GACALL SetFitnessOperation(Chromosome::GaChromosomeFitnessOperation* fitnessOperation) { _fitnessOperation = fitnessOperation; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to fitness operation used to performe individual-based evaluation of chromosomes.</returns>
		inline Chromosome::GaChromosomeFitnessOperation* GACALL GetFitnessOperation() { return _fitnessOperation; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to fitness operation used to performe individual-based evaluation of chromosomes.</returns>
		inline const Chromosome::GaChromosomeFitnessOperation* GACALL GetFitnessOperation() const { return _fitnessOperation; }

	};

	/// <summary><c>GaOperationTime</c> class handles measuring of operation's execution time.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaOperationTime
	{

	protected:

		/// <summary>Population which stores statistics.</summary>
		GaPopulation& _population;

		/// <summary>ID of the statistical value that stores operation execution time.</summary>
		int _operation;

		/// <summary>State of the used stopwatch at the start of operation's execution.</summary>
		long long _startTime;

	public:

		/// <summary>Initializes operation timer storage.</summary>
		/// <param name="population">population which stores statistics.</param>
		/// <param name="operation">ID of the statistical value that stores operation execution time.</param>
		GAL_API
		GaOperationTime(GaPopulation& population,
			int operation);

		/// <summary><c>UpdateStatistics</c> method stores operation execution time to population statistics.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		void GACALL UpdateStatistics();

	};

	/// <summary><c>GaCouplingCounters</c> class handles measuring of operation's execution time and 
	/// tracking number of performed operations that are involved in production of offspring chromosomes.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaCouplingCounters : public GaOperationTime
	{

	protected:

		/// <summary>Number of performed mating operations.</summary>
		int _matingCounter;

		/// <summary>Number of performed crossover operations.</summary>
		int _crossoverCounter;

		/// <summary>Number of performed mutation operations.</summary>
		int _mutationCounter;

		/// <summary>Number of accepted mutations.</summary>
		int _acceptedMutationCounter;

	public:

		/// <summary>Initializes operation timer and counters storage.</summary>
		/// <param name="population">population which stores statistics.</param>
		/// <param name="operation">ID of the statistical value that stores operation execution time.</param>
		GaCouplingCounters(GaPopulation& population,
			int operation) : GaOperationTime(population, operation),
			_matingCounter(0), 
			_crossoverCounter(0),
			_mutationCounter(0),
			_acceptedMutationCounter(0) { }

		/// <summary><c>CollectCrossoverBufferCounters</c> method collects counters from crossover buffer that number of operations performed during single mating operation.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="buffer">crossover buffer that contains counters.</param>
		GAL_API
		void GACALL CollectCrossoverBufferCounters(const Chromosome::GaCrossoverBuffer& buffer);

		/// <summary><c>UpdateStatistics</c> method stores operation execution time and state of counters to population statistics.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		void GACALL UpdateStatistics();

	};

	/// <summary><c>GaCouplingCounters</c> class handles measuring of operation's execution time and tracking number of performed operations during selection operation.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaSelectionCounters : public GaCouplingCounters
	{

	protected:

		/// <summary>Number of performed selections.</summary>
		int _selectionCounter;

	public:

		/// <summary>Initializes operation timer and counters storage.</summary>
		/// <param name="population">population which stores statistics.</param>
		/// <param name="operation">ID of the statistical value that stores operation execution time.</param>
		GaSelectionCounters(GaPopulation& population,
			int operation) : GaCouplingCounters(population, operation),
			_selectionCounter(0) { }

		/// <summary><c>IncSelectionCounter</c> method increments counter that count numbe or fperformed selections.
		///
		/// This method is not thread-safe.</summary>
		inline void GACALL IncSelectionCounter() { _selectionCounter++; }

		/// <summary><c>UpdateStatistics</c> method stores operation execution time and state of counters to population statistics.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		void GACALL UpdateStatistics();

	};

	/// <summary><c>GaCouplingParamsBase</c> class is base for parameters of operations that produce new chromosomes.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaCouplingParamsBase : public Common::GaParameters
	{

	protected:

		/// <summary>ID of the tag that stores crossover buffers that is used if selection operation also produces offspring chromosomes.</summary>
		int _crossoverBuffersTagID;

	public:

		/// <summary>This constructor initializes parameters with ID of the tag that stores crossover buffer.</summary>
		/// <param name="crossoverBuffersTagID">ID of the tag that stores crossover buffers.</param>
		GaCouplingParamsBase(int crossoverBuffersTagID) : _crossoverBuffersTagID(crossoverBuffersTagID) { }

		/// <summary>This constructor initializes parameters with default values</summary>
		GaCouplingParamsBase() : _crossoverBuffersTagID(-1) { }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns ID of the tag that stores crossover buffers that is used for production of offspring chromosomes.</returns>
		inline int GACALL GetCrossoverBuffersTagID() const { return _crossoverBuffersTagID; }

		/// <summary><c>SetCrossoverBuffersTagID</c> method sets ID of the tag that stores crossover buffers that is used for production offspring chromosomes.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="tagID">ID of the tag that stores crossover buffers.</param>
		inline void GACALL SetCrossoverBuffersTagID(int tagID) { _crossoverBuffersTagID = tagID; }

	};

	/// <summary><c>GaSelectionParams</c> class is base for parameters of selection operation.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaSelectionParams : public GaCouplingParamsBase
	{

	protected:

		/// <summary>Number of chromosomes which should be selected.
		/// If selection produces offsprings chromosomes this attribute indicates number of chromosomes that should be produced.</summary>
		int _selectionSize;

	public:

		/// <summary>This constructor initializes selection parameters with user-defined selection size.</summary>
		/// <param name="selectionSize">number of chromosomes which should be selected or produced.</param>
		/// <param name="crossoverBuffersTagID">ID of the tag that stores crossover buffers.
		/// This parameter should be set to negative value if selection operation should not produce offspring chromosomes.</param>
		GaSelectionParams(int selectionSize,
			int crossoverBuffersTagID) : GaCouplingParamsBase(crossoverBuffersTagID) { SetSelectionSize( selectionSize ); }

		/// <summary>This constructor initializes selection parameters with default values. Operation does not produce offspring chromosomes and default selection size is 2.</summary>
		GaSelectionParams() : _selectionSize(2) { }

		/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.</summary>
		virtual Common::GaParameters* GACALL Clone() const { return new GaSelectionParams( *this ); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns number of chromosomes which should be selected or produced.</returns>
		inline int GACALL GetSelectionSize() const { return _selectionSize; }

		/// <summary><c>SetSelectionSize</c> method sets number of chromosomes which should be selected or produced.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="size">new number of chromosomes for selection or produced.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>size</c> is negative value or 0.</exception>
		inline void GACALL SetSelectionSize(int size)
		{
			GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, size > 0, "size", "Number of chromosomes that should be selected must be greater then 0.", "Population" );
			_selectionSize = size;
		}

	};

	/// <summary><c>GaCouplingConfig</c> class represents configuration of genetic operation that produces new offspring chromosomes 
	/// and it contains operations and their parameters that are performed during offspring production.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaCouplingConfig : public Common::GaConfiguration
	{

	protected:

		/// <summary>Mating operation and its parameters.</summary>
		Chromosome::GaMatingSetup _mating;

	public:

		/// <summary>This constructor initializes configuration with operations and their parameters.</summary>
		/// <param name="mating">reference to object that stores mating operation and its parameters and configuration.</param>
		GaCouplingConfig(const Chromosome::GaMatingSetup& mating) : _mating(mating) { }

		/// <summary>Default constructor initializes matin configuration with no specified mating.</summary>
		GaCouplingConfig() { }

		/// <summary>More details are given in specification of <see cref="GaConfiguration::Clone" /> method.
		///
		/// This method is not thread-safe.</summary>
		virtual Common::GaConfiguration* GACALL Clone() const { return new GaCouplingConfig( *this ); }

		/// <summary><c>Mate</c> method performs all genetic operations defined by mating operation to produce produce offspring chromosomes.
		/// The number of chromosomes that will be produced can be queried using <see cref="GetOffspringCount" /> method and number of
		/// required parents can be obtained using <see cref="GetParentCount"/>.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="crossoverBuffer">reference to buffer that contains parent chromosomes and that will store offspring chromosomes.</param>
		inline void GACALL Mate(Chromosome::GaCrossoverBuffer& crossoverBuffer) const { _mating.GetOperation()( _mating.GetConfiguration(), crossoverBuffer, _mating.GetParameters() ); }

		/// <summary><c>SetMating</c> sets mating operation that will be used for producing offspring chromosomes.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="mating">new mating operation.</param>
		void GACALL SetMating(const Chromosome::GaMatingSetup& mating) { _mating = mating; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to mating operation and its parameters.</returns>
		inline Chromosome::GaMatingSetup& GACALL GetMating() { return _mating; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to mating operation and its parameters.</returns>
		inline const Chromosome::GaMatingSetup& GACALL GetMating() const { return _mating; }

	};

	/// <summary>This class is interface for selection operation which selects chromosomes from population which are going to be used as parents in production of new chromosomes.
	/// Selection operation can automatically produce offspring chromosomes if it is required.</summary>
	class GaSelectionOperation : public Common::GaOperation
	{

	public:

		/// <summary><c>Prepare</c> method prepares input and output data object for execution of the selection operation based on provided parameters and configuration.</summary>
		/// <param name="population">reference to population over which the selection is performed.</param>
		/// <param name="output">chromosome that will store selected or offspring chromosomes.</param>
		/// <param name="parameters">parameters of selection operation.</param>
		/// <param name="configuration">reference to configuration of selection operations.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		GAL_API
		virtual void GACALL Prepare(GaPopulation& population,
			GaChromosomeGroup& output,
			const GaSelectionParams& parameters,
			const GaCouplingConfig& configuration,
			int branchCount) const;

		/// <summary><c>Clear</c> method reverse changes made by selection operation to input and output data object.</summary>
		/// <param name="population">reference to population over which the selection is performed.</param>
		/// <param name="output">chromosome that will store selected or offspring chromosomes.</param>
		/// <param name="parameters">parameters of selection operation.</param>
		/// <param name="configuration">reference to configuration of selection operations.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		GAL_API
		virtual void GACALL Clear(GaPopulation& population,
			GaChromosomeGroup& output,
			const GaSelectionParams& parameters,
			const GaCouplingConfig& configuration,
			int branchCount) const;

		/// <summary><c>Update</c> method updates input and output data object to reflect changes in the workflow.</summary>
		/// <param name="population">reference to population over which the selection is performed.</param>
		/// <param name="output">chromosome that will store selected or offspring chromosomes.</param>
		/// <param name="parameters">parameters of selection operation.</param>
		/// <param name="configuration">reference to configuration of selection operations.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		GAL_API
		virtual void GACALL Update(GaPopulation& population,
			GaChromosomeGroup& output,
			const GaSelectionParams& parameters,
			const GaCouplingConfig& configuration,
			int branchCount) const;

		/// <summary><c>operator ()</c> performs selection operation and produces offspring chromosomes as parents are selected.</summary>
		/// <param name="population">reference to population over which the selection is performed.</param>
		/// <param name="output">chromosome that will store selected or offspring chromosomes.</param>
		/// <param name="parameters">parameters of selection operation.</param>
		/// <param name="configuration">reference to configuration of selection operations.</param>
		/// <param name="branch">pointer to workflow barnch that executes operation.</param>
		virtual void GACALL operator ()(GaPopulation& population,
			GaChromosomeGroup& output,
			const GaSelectionParams& parameters,
			const GaCouplingConfig& configuration,
			Common::Workflows::GaBranch* branch) const = 0;

		/// <summary>More details are given in specification of <see cref="GaOperation::CreateConfiguration" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual Common::GaConfiguration* GACALL CreateConfiguration() const { return new GaCouplingConfig(); }

	protected:

		/// <summary><c>GetSelectionCount</c> method calculate number of chromosomes that operation should select.</summary>
		/// <param name="params">parameters of selection operation.</param>
		/// <param name="configuration">pointer to mating configuration if selection operation should also produce offspring chromosomes.
		/// If the operation just selects parent chromosome this parameter should be set to <c>NULL</c>.</param>
		/// <returns>Method returns number fo chromosomes that should be selected.</returns>
		inline int GACALL GetSelectionCount(const GaSelectionParams& params,
			const Chromosome::GaMatingConfig* matingConfig) const
			{ return matingConfig ? params.GetSelectionSize() * matingConfig->GetParentCount() / matingConfig->GetOffspringCount() : params.GetSelectionSize(); }

	};

	/// <summary><c>GaSelectionSetup</c> type is instance of <see cref="GaOperationSetup" /> template class and represents setup of
	/// selection operation and its parameters. Detailed description can be found in specification of
	/// <see cref="GaOperationSetup" /> template class.</summary>
	typedef Common::GaOperationSetup<GaSelectionOperation, GaSelectionParams, GaCouplingConfig> GaSelectionSetup;

	/// <summary><c>GaSelectionCatalogue</c> type is instance of <see cref="GaCatalogue" /> template class and represents catalogue of selection operations.
	/// Detailed description can be found in specification of <see cref="GaCatalogue" /> template class.</summary>
	typedef Common::Data::GaCatalogue<GaSelectionOperation> GaSelectionCatalogue;

	/// <summary><c>GaSelectionStep</c> type is instance of <see cref="GaOperationStep2" /> template class and represents workflow step that performs 
	/// selection operation. Detailed description can be found in specification of <see cref="GaOperationStep2" /> template class.</summary>
	typedef Common::Workflows::GaOperationStep2<GaSelectionSetup, GaPopulation, GaChromosomeGroup> GaSelectionStep;

	/// <summary>This class is base for parameters of coupling operation.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaCouplingParams : public GaCouplingParamsBase
	{

	private:

		/// <summary>Number of chromosomes which should be produces.</summary>
		int _numberOfOffsprings;

	public:

		/// <summary>This constructor initializes coupling parameters with user-defined number of produced offspring size.</summary>
		/// <param name="numberOfOffsprings">number of chromosomes which should be produced.</param>
		/// <param name="crossoverBuffersTagID">ID of the tag that stores crossover buffers.</param>
		GaCouplingParams(int numberOfOffsprings,
			int crossoverBuffersTagID) : GaCouplingParamsBase(crossoverBuffersTagID) { SetNumberOfOffsprings( numberOfOffsprings ); }

		/// <summary>This constructor initializes coupling parameters with default values. Default number of produced offspring is 2.</summary>
		GaCouplingParams() : _numberOfOffsprings(2) { }

		/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.</summary>
		virtual Common::GaParameters* GACALL Clone() const { return new GaCouplingParams( *this ); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns number of chromosomes which should be produced.</returns>
		inline int GACALL GetNumberOfOffsprings() const { return _numberOfOffsprings; }

		/// <summary><c>SetNumberOfOffsprings</c> method sets number of chromosomes which should be produced.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="number">new number of chromosomes which should be produced.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>number</c> is negative value or <c>0</c>.</exception>
		inline void GACALL SetNumberOfOffsprings(int number)
		{
			GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, number > 0, "number",
				"Number of chromosomes that should be produced must be greater then 0.", "Population" );

			_numberOfOffsprings = number;
		}

	};

	/// <summary>This class is interface for coupling operations which produce offspring from selected chromosomes.</summary>
	class GaCouplingOperation : public Common::GaOperation
	{

	public:

		/// <summary><c>Prepare</c> method prepares input and output data object for execution of the coupling operation based on provided parameters and configuration.</summary>
		/// <param name="population">reference to population over which the selection is performed.</param>
		/// <param name="output">chromosome that will store selected or offspring chromosomes.</param>
		/// <param name="parameters">parameters of selection operation.</param>
		/// <param name="configuration">reference to configuration of selection operations.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		GAL_API
		virtual void GACALL Prepare(GaChromosomeGroup& input,
			GaChromosomeGroup& output,
			const GaCouplingParams& parameters,
			const GaCouplingConfig& configuration,
			int branchCount) const;

		/// <summary><c>Clear</c> method reverse changes made by coupling operation to input and output data object.</summary>
		/// <param name="population">reference to population over which the selection is performed.</param>
		/// <param name="output">chromosome that will store selected or offspring chromosomes.</param>
		/// <param name="parameters">parameters of selection operation.</param>
		/// <param name="configuration">reference to configuration of selection operations.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		GAL_API
		virtual void GACALL Clear(GaChromosomeGroup& input,
			GaChromosomeGroup& output,
			const GaCouplingParams& parameters,
			const GaCouplingConfig& configuration,
			int branchCount) const;

		/// <summary><c>Update</c> method updates input and output data object to reflect changes in the workflow.</summary>
		/// <param name="population">reference to population over which the selection is performed.</param>
		/// <param name="output">chromosome that will store selected or offspring chromosomes.</param>
		/// <param name="parameters">parameters of selection operation.</param>
		/// <param name="configuration">reference to configuration of selection operations.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		GAL_API
		virtual void GACALL Update(GaChromosomeGroup& input,
			GaChromosomeGroup& output,
			const GaCouplingParams& parameters,
			const GaCouplingConfig& configuration,
			int branchCount) const;

		/// <summary><c>operator ()</c> performs replacement operation.</summary>
		/// <param name="input">chromosome group that store parents for coupling operation.</param>
		/// <param name="output">chromosome group that is used for storing offspring chromosomes produced by the operation.</param>
		/// <param name="parameters">parameters of the operation.</param>
		/// <param name="configuration">reference to configuration of coupling operations.</param>
		/// <param name="branch">pointer to workflow barnch that executes operation.</param>
		virtual void GACALL operator ()(GaChromosomeGroup& input,
			GaChromosomeGroup& output,
			const GaCouplingParams& parameters,
			const GaCouplingConfig& configuration,
			Common::Workflows::GaBranch* branch) const = 0;

		/// <summary>More details are given in specification of <see cref="GaOperation::CreateConfiguration" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual Common::GaConfiguration* GACALL CreateConfiguration() const { return new GaCouplingConfig(); }

	};

	/// <summary><c>GaCouplingSetup</c> type is instance of <see cref="GaOperationSetup" /> template class and represents setup of
	/// coupling operation and its parameters. Detailed description can be found in specification of
	/// <see cref="GaOperationSetup" /> template class.</summary>
	typedef Common::GaOperationSetup<GaCouplingOperation, GaCouplingParams, GaCouplingConfig> GaCouplingSetup;

	/// <summary><c>GaCouplingCatalogue</c> type is instance of <see cref="GaCatalogue" /> template class and represents catalogue of coupling operations.
	/// Detailed description can be found in specification of <see cref="GaCatalogue" /> template class.</summary>
	typedef Common::Data::GaCatalogue<GaCouplingOperation> GaCouplingCatalogue;

	/// <summary><c>GaCouplingStep</c> type is instance of <see cref="GaOperationStep2" /> template class and represents workflow step that performs 
	/// coupling operation. Detailed description can be found in specification of <see cref="GaOperationStep2" /> template class.</summary>
	typedef Common::Workflows::GaOperationStep2<GaCouplingSetup, GaChromosomeGroup, GaChromosomeGroup> GaCouplingStep;

	/// <summary>This class is base for parameters of replacement operation. 
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaReplacementParams : public Common::GaParameters
	{

	protected:

		/// <summary>Number of chromosomes which should be replaced.</summary>
		int _replacemetSize;

	public:

		/// <summary>This constructor initializes replacement parameters with user-defined replacement size.</summary>
		/// <param name="replacementSize">number of chromosomes which should be replaced.</param>
		GaReplacementParams(int replacementSize) { SetReplacementSize( replacementSize ); }

		/// <summary>This constructor initializes replacement parameters with default values. Default replacement size is 2.</summary>
		GaReplacementParams() : _replacemetSize(0) { }

		/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.</summary>
		virtual Common::GaParameters* GACALL Clone() const { return new GaReplacementParams( *this ); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns number of chromosomes which should be replaced.</returns>
		inline int GACALL GetReplacementSize() const { return _replacemetSize; }

		/// <summary><c>SetReplacementSize</c> method sets number of chromosomes which should be replaced.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="size">new number of chromosomes for replacement.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>size</c> is negative value or 0.</exception>
		inline void GACALL SetReplacementSize(int size)
		{
			GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, size > 0, "size", "Number of chromosomes that should be replaced must be greater then 0.", "Population" );
			_replacemetSize = size;
		}

	};

	/// <summary><c>GaReplacementConfig</c> class is base for parameters of replacement operations.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaReplacementConfig : public Common::GaConfiguration
	{

	protected:

		/// <summary>Comparator that is used for identifying duplicates of population chromosomes in the replacement input set.</summary>
		Chromosome::GaChromosomeComparatorSetup _chromosomeComparator;

	public:

		/// <summary>Thic constructor initializes configuration with comparator that is used for identifying duplicates of population chromosomes in the replacement input set.</summary>
		/// <param name="chromosomeComparator">chromosome comparator.</param>
		GaReplacementConfig(const Chromosome::GaChromosomeComparatorSetup& chromosomeComparator) : _chromosomeComparator(chromosomeComparator) { }

		/// <summary>Thic constructor copies another operation configuration.</summary>
		/// <param name="rhs">configuration that should be copied.</param>
		GaReplacementConfig(const GaReplacementConfig& rhs) : _chromosomeComparator(rhs._chromosomeComparator) { }

		/// <summary>This constructor initializes configuration with no speficied chromosome comparator.</summary>
		GaReplacementConfig() { }

		/// <summary>More details are given in specification of <see cref="GaConfiguration::Clone" /> method.
		///
		/// This method is not thread-safe.</summary>
		virtual Common::GaConfiguration* GACALL Clone() const { return new GaReplacementConfig( *this ); }

		/// <summary><c>SetChromosomeComparator</c> method sets comparator that will be used for identifying duplicates of population chromosomes in the replacement input set.</summary>
		/// <param name="comparator">chromosome comparator.</param>
		inline void GACALL SetChromosomeComparator(const Chromosome::GaChromosomeComparatorSetup& comparator) { _chromosomeComparator = comparator; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns comparator that is used for identifying duplicates of population chromosomes in the replacement input set.</returns>
		inline Chromosome::GaChromosomeComparatorSetup& GetChromosomeComparator() { return _chromosomeComparator; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns comparator that is used for identifying duplicates of population chromosomes in the replacement input set.</returns>
		inline const Chromosome::GaChromosomeComparatorSetup& GetChromosomeComparator() const { return _chromosomeComparator; }

	};

	/// <summary>This class is interface for replacement operations which replace chromosomes of population with new chromosomes.</summary>
	class GaReplacementOperation : public Common::GaOperation
	{

	public:

		/// <summary><c>Prepare</c> method prepares input and output data object for execution of the replacement operation based on provided parameters and configuration.</summary>
		/// <param name="population">reference to population over which the selection is performed.</param>
		/// <param name="output">chromosome that will store selected or offspring chromosomes.</param>
		/// <param name="parameters">parameters of selection operation.</param>
		/// <param name="configuration">reference to configuration of selection operations.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		virtual void GACALL Prepare(GaChromosomeGroup& input,
			GaPopulation& population,
			const GaReplacementParams& parameters,
			const GaReplacementConfig& configuration,
			int branchCount) const { }

		/// <summary><c>Clear</c> method reverse changes made by replacement operation to input and output data object.</summary>
		/// <param name="population">reference to population over which the selection is performed.</param>
		/// <param name="output">chromosome that will store selected or offspring chromosomes.</param>
		/// <param name="parameters">parameters of selection operation.</param>
		/// <param name="configuration">reference to configuration of selection operations.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		virtual void GACALL Clear(GaChromosomeGroup& input,
			GaPopulation& population,
			const GaReplacementParams& parameters,
			const GaReplacementConfig& configuration,
			int branchCount) const { }

		/// <summary><c>Update</c> method updates input and output data object to reflect changes in the workflow.</summary>
		/// <param name="population">reference to population over which the selection is performed.</param>
		/// <param name="output">chromosome that will store selected or offspring chromosomes.</param>
		/// <param name="parameters">parameters of selection operation.</param>
		/// <param name="configuration">reference to configuration of selection operations.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		virtual void GACALL Update(GaChromosomeGroup& input,
			GaPopulation& population,
			const GaReplacementParams& parameters,
			const GaReplacementConfig& configuration,
			int branchCount) const { }

		/// <summary><c>operator ()</c> performs replacement operation.</summary>
		/// <param name="input">new chromosomes that should be inserted into the population</param>
		/// <param name="population">population over which replacement operation is performed.</param>
		/// <param name="parameters">parameters of the operation.</param>
		/// <param name="configuration">reference to configuration of replacement operations.</param>
		/// <param name="branch">pointer to workflow barnch that executes operation.</param>
		virtual void GACALL operator ()(GaChromosomeGroup& input,
			GaPopulation& population,
			const GaReplacementParams& parameters,
			const GaReplacementConfig& configuration,
			Common::Workflows::GaBranch* branch) const = 0;

		/// <summary>More details are given in specification of <see cref="GaOperation::CreateConfiguration" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual Common::GaConfiguration* GACALL CreateConfiguration() const { return NULL; }

	};

	/// <summary><c>GaReplacementSetup</c> type is instance of <see cref="GaOperationSetup" /> template class and represents setup
	/// of replacement operation and its parameters. Detailed description can be found in specification of <see cref="GaOperationSetup" /> template class.</summary>
	typedef Common::GaOperationSetup<GaReplacementOperation, GaReplacementParams, GaReplacementConfig> GaReplacementSetup;

	/// <summary><c>GaReplacementCatalogue</c> type is instance of <see cref="GaCatalogue" /> template class and represents catalogue of replacement operations.
	/// Detailed description can be found in specification of <see cref="GaCatalogue" /> template class.</summary>
	typedef Common::Data::GaCatalogue<GaReplacementOperation> GaReplacementCatalogue;

	/// <summary><c>GaReplacementStep</c> type is instance of <see cref="GaOperationStep2" /> template class and represents workflow step that performs 
	/// replacement operation. Detailed description can be found in specification of <see cref="GaOperationStep2" /> template class.</summary>
	typedef Common::Workflows::GaOperationStep2<GaReplacementSetup, GaChromosomeGroup, GaPopulation> GaReplacementStep;

	/// <summary><c>GaScalingParams</c> class is base for parameters of scaling operations.</summary>
	class GaScalingParams : public Common::GaParameters { };

	/// <summary><c>GaScalingConfig</c> class is base for configuration of scaling operations.</summary>
	class GaScalingConfig : public Fitness::GaFitnessOperationConfig
	{

	public:

		/// <summary>This constructor initializes configuration with fitness parameters.</summary>
		/// <param name="fitnessParams">pointer to fitness parameters.</param>
		GaScalingConfig(const Fitness::GaFitnessParams* fitnessParams) : GaFitnessOperationConfig(fitnessParams) { }

		/// <summary>This constructor creates new configuration and makes copy fitness parameters.</summary>
		/// <params name="rhs">configuration that should be copied.</params>
		GaScalingConfig(const GaFitnessOperationConfig& rhs) : GaFitnessOperationConfig(rhs) { }

		/// <summary>Default configuration initializes configuration without fitness parameters.</summary>
		GaScalingConfig() { }

	};

	/// <summary>This class is interface for scaling operations which transform raw fitness values of chromosomes into fitness values more suitable for selection probability.</summary>
	class GaScalingOperation : public Fitness::GaFitnessOperation
	{

	public:

		/// <summary><c>Prepare</c> method prepares input and output data object for execution of the scaling operation based on provided parameters and configuration.</summary>
		/// <param name="population">reference to population over which the selection is performed.</param>
		/// <param name="parameters">parameters of selection operation.</param>
		/// <param name="configuration">reference to configuration of selection operations.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		GAL_API
		virtual void GACALL Prepare(GaPopulation& population,
			const GaScalingParams& parameters,
			const GaScalingConfig& configuration,
			int branchCount) const;

		/// <summary><c>Clear</c> method reverse changes made by scaling operation to input and output data object.</summary>
		/// <param name="population">reference to population over which the selection is performed.</param>
		/// <param name="parameters">parameters of selection operation.</param>
		/// <param name="configuration">reference to configuration of selection operations.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		GAL_API
		virtual void GACALL Clear(GaPopulation& population,
			const GaScalingParams& parameters,
			const GaScalingConfig& configuration,
			int branchCount) const;

		/// <summary><c>Update</c> method updates input and output data object to reflect changes in the workflow.</summary>
		/// <param name="population">reference to population over which the selection is performed.</param>
		/// <param name="parameters">parameters of selection operation.</param>
		/// <param name="configuration">reference to configuration of selection operations.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		virtual void GACALL Update(GaPopulation& population,
			const GaScalingParams& parameters,
			const GaScalingConfig& configuration,
			int branchCount) const { }

		/// <summary><c>operator ()</c> performs scaling operation over chromosomes in population and stores results.</summary>
		/// <param name="population">population on whose chromosomes fitness scaling is performed.</param>
		/// <param name="parameters">parameters of scaling operation.</param>
		/// <param name="configuration">reference to configuration of replacement operations.</param>
		/// <param name="branch">pointer to workflow barnch that executes operation.</param>
		virtual void GACALL operator ()(GaPopulation& population,
			const GaScalingParams& parameters,
			const GaScalingConfig& configuration,
			Common::Workflows::GaBranch* branch) const = 0;

		/// <summary>More details are given in specification of <see cref="GaOperation::CreateConfiguration" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual Common::GaConfiguration* GACALL CreateConfiguration() const { return NULL; }

	};

	/// <summary><c>GaScalingSetup</c> type is instance of <see cref="GaOperationSetup" /> template class and represents setup of
	/// scaling operation and its parameters. Detailed description can be found in specification of <see cref="GaOperationSetup" /> template class.</summary>
	typedef Common::GaOperationSetup<GaScalingOperation, GaScalingParams, GaScalingConfig> GaScalingSetup;

	/// <summary><c>GaScalingCatalogue</c> type is instance of <see cref="GaCatalogue" /> template class and represents catalogue of scaling operations.
	/// Detailed description can be found in specification of <see cref="GaCatalogue" /> template class.</summary>
	typedef Common::Data::GaCatalogue<GaScalingOperation> GaScalingCatalogue;

	/// <summary><c>GaScalingStep</c> type is instance of <see cref="GaOperationStep1" /> template class and represents workflow step that performs 
	/// scaling operation. Detailed description can be found in specification of <see cref="GaOperationStep1" /> template class.</summary>
	typedef Common::Workflows::GaOperationStep1<GaScalingSetup, GaPopulation> GaScalingStep;

	/// <summary><c>GaSortCriteriaHolder</c> template class stores reference to sort criteria that is used by population soritng workflow steps.
	/// 
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	/// <param name="CRITERIA">type of sort criteria.</param>
	template<typename CRITERIA>
	class GaSortCriteriaHolder
	{

	public:

		/// <summary>Type of sort criteria.</summary>
		typedef CRITERIA GaCriteria;

	private:

		/// <summary>Stored sort criteria.</summary>
		GaCriteria _criteria;

	public:

		/// <summary>Initializes criteria holder.</summary>
		/// <param name="criteria">reference to sort criteria that should be stored.</param>
		GaSortCriteriaHolder(const GaCriteria& criteria) : _criteria(criteria) { }

		/// <summary><c>SetCriteria</c> method stores new sort criteria.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="criteria">reference to sort criteria that should be stored.</param>
		inline void GACALL SetCriteria(const GaCriteria& criteria) { _criteria = criteria; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to stored criteria.</returns>
		inline GaCriteria& GACALL GetCriteria() { return _criteria; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to stored criteria.</returns>
		inline const GaCriteria& GACALL GetCriteria() const { return _criteria; }

	};

	/// <summary>This specialization of <c>GaSortCriteriaHolder</c> template class that stores sort criteria that can be changed at run-time.
	/// 
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	template<>
	class GaSortCriteriaHolder<Common::Sorting::GaMutableSortCriteria<GaChromosomeStorage*> >
	{

	public:

		/// <summary>Type of sort criteria.</summary>
		typedef Common::Sorting::GaMutableSortCriteria<GaChromosomeStorage*> GaCriteria;

	private:

		/// <summary>Stored sort criteria.</summary>
		Common::Memory::GaAutoPtr<GaCriteria> _criteria;

	public:

		/// <summary>Initializes criteria holder.</summary>
		/// <param name="criteria">reference to sort criteria that should be stored.</param>
		GaSortCriteriaHolder(const GaCriteria& criteria) { SetCriteria( criteria ); }

		/// <summary><c>SetCriteria</c> method stores new sort criteria.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="criteria">reference to sort criteria that should be stored.</param>
		inline void GACALL SetCriteria(const GaCriteria& criteria) { _criteria = criteria.Clone(); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to stored criteria.</returns>
		inline GaCriteria& GACALL GetCriteria() { return *_criteria; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to stored criteria.</returns>
		inline const GaCriteria& GACALL GetCriteria() const { return *_criteria; }

	};

	/// <summary><c>GaSortPopulationStep</c> template class represent workflow step that sorts porivided population according to specified sort criteria.
	/// 
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	/// <param name="CRITERIA">type of sort criteria used for sorting population.</param>
	template<typename CRITERIA>
	class GaSortPopulationStep : public Common::Workflows::GaSimpleWorkStep
	{

	public:

		/// <summary>Type of sort criteria used for sorting population.</summary>
		typedef CRITERIA GaCriteria;

	private:

		/// <summary>Population on which the sorting is preformed.</summary>
		Common::Workflows::GaDataCache<GaPopulation> _population;

		/// <summary>Criteria used for sorting population.</summary>
		GaSortCriteriaHolder<GaCriteria> _criteria;

	public:

		/// <summary>Initializes sorting step with population object and sort criteria.</summary>
		/// <param name="dataStorage">pointer to workflow that stores population used by the step.</param>
		/// <param name="populationID">ID of population object in workflow storage.</param>
		/// <param name="criteria">sort criteria that is going to be for sorting population.</param>
		GaSortPopulationStep(Common::Workflows::GaDataStorage* dataStorage,
			int populationID,
			const GaCriteria& criteria) : _population(dataStorage, populationID),
			_criteria(criteria) { }

		/// <summary>Initializes sorting step with sort criteria that should be executed.</summary>
		/// <param name="criteria">sort criteria that is going to be for sorting population.</param>
		GaSortPopulationStep(const GaCriteria& criteria) { SetCriteria( criteria ); }

		/// <summary>Initializes sorting with default sort criteria and without population object.</summary>
		GaSortPopulationStep() { }

		/// <summary><c>SetCriteria</c> method sets criteria that is going to be used for sorting population.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="criteria">sort criteria.</param>
		inline void GACALL SetCriteria(const GaCriteria& criteria) { _criteria.SetCriteria( criteria ); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to sort criteria used for sorting population.</returns>
		inline GaCriteria& GACALL GetCriteria() { return _criteria.GetCriteria(); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to sort criteria used for sorting population.</returns>
		inline const GaCriteria& GACALL GetCriteria() const { return _criteria.GetCriteria(); }

		/// <summary><c>operator ()</c> executes method over specified object.
		///
		/// This operator is not thread-safe.</summary>
		/// <param name="branch">pointer to workflow branch that executes this step.</param>
		virtual void GACALL operator ()(Common::Workflows::GaBranch* branch)
		{
			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
				_population.GetData().Sort( _criteria.GetCriteria() );
		}

		/// <summary><c>SetPopulation</c> method sets population object that should be used by the operation executed by this step.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="dataStorage">pointer to workflow that stores population used by the step.</param>
		/// <param name="populationID">ID of population object in workflow storage.</param>
		inline void GACALL SetPopulation(Common::Workflows::GaDataStorage* dataStorage,
			int populationID) { _population.SetData( dataStorage, populationID ); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns ID of the population object in the wokrflow.</returns>
		inline int GACALL SetPopulation() const { return _population.GetDataID(); }

	};

} // Population

#endif // __GA_POPULATION_OPERATIONS_H__

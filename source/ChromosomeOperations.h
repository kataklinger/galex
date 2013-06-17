
/*! \file ChromosomeOperations.h
    \brief This file contains declaration of interfaces for genetic operations which are performed over chromosomes.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_CHROMOSOME_OPERATIONS_H__
#define __GA_CHROMOSOME_OPERATIONS_H__

#include "Catalogue.h"
#include "Fitness.h"
#include "Chromosome.h"

namespace Chromosome
{

	/// <summary><c>GaInitializatorParams</c> is base class for parameters of chromosomes' initialization operations.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaInitializatorParams : public Common::GaParameters { };

	/// <summary><c>GaInitializatorConfig</c> is base class for configuration classes of chromosome initializators.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaInitializatorConfig : public Common::GaConfiguration
	{

	protected:

		/// <summary>CCB that is used by chromosomes produced with the initialization or crossover operation.</summary>
		Common::Memory::GaSmartPtr<GaChromosomeConfigBlock> _configBlock;

	public:

		/// <summary>This constructor initializes initializator configuration with specified CCB.</summary>
		/// <param name="configBlock">pointer to CCB that will be used by chromosomes produced with the initialization or crossover operation.</param>
		GaInitializatorConfig(const GaChromosomeConfigBlock* configBlock) { SetConfigBlock( configBlock ); }

		/// <summary>This constructor initializes initializator configuration with copy of CCB from another configuration.</summary>
		GaInitializatorConfig(const GaInitializatorConfig& rhs) { SetConfigBlock( rhs._configBlock.GetRawPtr() ); }

		/// <summary>Default constructor intializes chromosome initializator with no specified CCB.</summary>
		GaInitializatorConfig() { }

		/// <summary>Virtual destructor must be defined because this is base class.</summary>
		virtual ~GaInitializatorConfig() { }

		/// <summary>More details are given in specification of <see cref="GaConfiguration::Clone" /> method.
		///
		/// This method is not thread-safe.</summary>
		virtual Common::GaConfiguration* GACALL Clone() const { return new GaInitializatorConfig( *this ); }

		/// <summary><c>SetConfigBlock</c> method sets CCB that will be used by chromosomes produced with the initialization or crossover operation.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="configBlock">pointer to new CCB.</param>
		inline void GACALL SetConfigBlock(const GaChromosomeConfigBlock* configBlock) { _configBlock = configBlock ? configBlock->Clone() : NULL; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns smart pointer to CCB that is used by chromosomes produced with the initialization or crossover operation.</returns>
		inline Common::Memory::GaSmartPtr<GaChromosomeConfigBlock> GACALL GetConfigBlock() { return _configBlock; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns smart pointer to CCB that is used by chromosomes produced with the initialization or crossover operation.</returns>
		inline Common::Memory::GaSmartPtr<const GaChromosomeConfigBlock> GetConfigBlock() const { return _configBlock; }

	};

	/// <summary><c>GaInitializator</c> class is interface for chromosomes' initialization operations.</summary>
	class GaInitializator : public Common::GaOperation
	{

	public:

		/// <summary>This operator makes new chromosome and performs initialization of the chromosome.</summary>
		/// <param name="empty">instruct the initializator to create an empty chromosome without genes.</param>
		/// <param name="parameters">pointer to parameters of initialization operation.</param>
		/// <param name="configBlock">smart pointer to chromosome configuration that will be used by chromosome.</param>
		/// <returns>Method returns smart pointer to chromosome that should be initialized.</returns>
		virtual GaChromosomePtr GACALL operator ()(bool empty,
			const GaInitializatorParams& parameters,
			Common::Memory::GaSmartPtr<GaChromosomeConfigBlock> configBlock) const = 0;

		/// <summary>More details are given in specification of <see cref="GaOperation::CreateConfiguration" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual Common::GaConfiguration* GACALL CreateConfiguration() const { return new GaInitializatorConfig(); }

	};

	/// <summary><c>GaInitializatorSetup</c> type is instance of <see cref="GaOperationSetup" /> template class and represents setup of
	/// chromosome initialization operation and its parameters. Detailed description can be found in specification of
	/// <see cref="GaOperationSetup" /> template class.</summary>
	typedef Common::GaOperationSetup<GaInitializator, GaInitializatorParams, GaInitializatorConfig> GaInitializatorSetup;

	/// <summary><c>GaInitializatorCatalogue</c> type is instance of <see cref="GaCatalogue" /> template class and represents catalogue of
	/// chromosomes initializators. Detailed description can be found in specification of <see cref="GaCatalogue" /> template class.</summary>
	typedef Common::Data::GaCatalogue<GaInitializator> GaInitializatorCatalogue;

	/// <summary><c>GaCrossoverParams</c> is base class for parameters of crossover operations.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaCrossoverParams : public Common::GaOperationProbabilityParams
	{

	protected:

		/// <summary>Number of offspring chromosomes that crossover operation should produce.</summary>
		int _numberOfOffspring;

	public:

		/// <summary>This constructor initializes parameters with user-defined values.</summary>
		/// <param name="crossoverProbability">crossover probability in interval (0, 1).</param>
		/// <param name="numberOfOffspring">number of offspring chromosomes that crossover operation should produce.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown when user tries to set number of chromosomes that should be produced to negative value or to zero.</exception>
		GaCrossoverParams(float crossoverProbability,
			int numberOfOffspring) : GaOperationProbabilityParams(crossoverProbability) { SetNumberOfOffspring( numberOfOffspring ); }

		/// <summary>This constructor initializes parameters with default values.
		/// <br/>1. crossover probability: 80%
		/// <br/>2. number of offspring chromosomes: 2</summary>
		GaCrossoverParams() : GaOperationProbabilityParams(0.8f),
			_numberOfOffspring(2) { }

		/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
		///
		/// This method is not thread-safe.</summary>
		virtual Common::GaParameters* GACALL Clone() const { return new GaCrossoverParams( *this ); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns number of offspring chromosomes that crossover operation should produce.</returns>
		inline int GACALL GetNumberOfOffspring() const { return _numberOfOffspring; }

		/// <summary><c>SetNumberOfOffspring</c> method sets number of offspring chromosomes that crossover operation should produce.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="numberOfOffspring">number of offspring chromosomes that crossover operation should produce.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown when user tries to set number of chromosomes that should be produced to negative value or to zero.</exception>
		inline void GACALL SetNumberOfOffspring(int numberOfOffspring)
		{
			GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, numberOfOffspring > 0,
				"numberOfOffspring", "Number of chromosomes that should be produced must be positive number greater then 0.", "Chromosomes" );

			_numberOfOffspring = numberOfOffspring;
		}

	};

	/// <summary><c>GaCrossoverPointParams</c> is base class for parameters of crossover operations that are based on crossover points.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaCrossoverPointParams : public GaCrossoverParams
	{

	protected:

		/// <summary>Number of crossover points between two parents when crossover operation is performed.</summary>
		int _numberOfCrossoverPoints;

	public:

		/// <summary>This constructor initializes parameters with user-defined values.</summary>
		/// <param name="crossoverProbability">crossover probability in interval (0, 1).</param>
		/// <param name="numberOfOffspring">number of offspring chromosomes that crossover operation should produce.</param>
		/// <param name="numberOfCrossoverPoints">number of crossover points.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown when user tries to set number of crossover points to negative value or to zero.</exception>
		GaCrossoverPointParams(float crossoverProbability,
			int numberOfOffspring,
			int numberOfCrossoverPoints) : GaCrossoverParams(crossoverProbability, numberOfOffspring) { SetNumberOfCrossoverPoints( numberOfCrossoverPoints ); }

		/// <summary>This constructor initializes parameters with default values.
		/// <br/>1. crossover probability: 80%
		/// <br/>2. number of offspring chromosomes: 2
		/// <br/>2. number of crossover points: 1</summary>
		GaCrossoverPointParams() : _numberOfCrossoverPoints(1) { }

		/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
		///
		/// This method is not thread-safe.</summary>
		virtual Common::GaParameters* GACALL Clone() const { return new GaCrossoverPointParams( *this ); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns number of crossover points.</returns>
		inline int GACALL GetNumberOfCrossoverPoints() const { return _numberOfCrossoverPoints; }

		/// <summary><c>SetNumberOfCrossoverPoints</c> method sets number of crossover points. 
		///
		/// This method is not thread-safe.</summary>
		/// <param name="numberOfPoints">new number of crossover points.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown when user tries to set number of crossover points to negative value or to zero.</exception>
		inline void GACALL SetNumberOfCrossoverPoints(int numberOfPoints)
		{
			GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, numberOfPoints > 0,
				"numberOfPoints", "Number of crossover points must be positive number greater then 0.", "Chromosomes" );

			 _numberOfCrossoverPoints = numberOfPoints;
		}

	};

	/// <summary><c>GaCrossoverBuffer</c> is interface for object that provides parent chromosomes for crossover operation and 
	/// stores offspring chromosomes produced by the operation.</summary>
	class GaCrossoverBuffer
	{

	public:

		/// <summary>Virtual destructor must be defined because this is base class.</summary>
		virtual ~GaCrossoverBuffer() { }

		/// <summary><c>CreateOffspringFromPrototype</c> method creates new offspring chromosome based on prototype that crossover buffer requires.</summary>
		/// <returns>Method returns smart pointer to created offspring chromosome.</returns>
		virtual GaChromosomePtr GACALL CreateOffspringFromPrototype() const = 0;

		/// <summary><c>StoreOffspringChromosome</c> method inserts chromosome into the buffer at the first available position.</summary>
		/// <param name="chromosome">smart pointer to offspring chromosome that should be stored.</param>
		/// <param name="parent">identification of chromosome's parent.</param>
		/// <summary>Method returns position at which offspring chromosome is stored in crossover buffer.</summary>
		virtual int GACALL StoreOffspringChromosome(GaChromosomePtr chromosome,
			int parent) = 0;

		/// <summary><c>ReplaceOffspringChromosome</c> method removes chromosome at specified position in the buffere and inserts new at its position.</summary>
		/// <param name="index">index of chromosome in the buffer that should be replaced.</param>
		/// <param name="chromosome">new chromosome that should be inserted insted old one.</param>
		virtual void GACALL ReplaceOffspringChromosome(int index,
			GaChromosomePtr chromosome) = 0;

		/// <summary><c>ReplaceIfBetter</c> method replaces chromosome only if the new one is beter than one at specified position.</summary>
		/// <param name="index">index of chromosome in the buffer that should be replaced.</param>
		/// <param name="newChromosome">new chromosome that should be inserted insted old one.</param>
		/// <returns>Method returns <c>true</c> if replacement was performed (new chromosome was better than old).</returns>
		virtual bool GACALL ReplaceIfBetter(int index,
			GaChromosomePtr newChromosome) = 0;

		/// <summary><c>GetOffspringChromosome</c> method returns chromosome at specified position in the buffer.</summary>
		/// <param name="index">index of queried chromosome.</param>
		/// <returns>Method returns smart pointer to chromosome at specified position in buffer.</returns>
		virtual GaChromosomePtr GACALL GetOffspringChromosome(int index) = 0;

		/// <summary><c>GetParentChromosome</c> method returns parent chromosome at specified position in the buffer.</summary>
		/// <param name="index">index of parent chromosome.</param>
		/// <returns>Method returns smart pointer to chromosome at specified index.</returns>
		virtual GaChromosomePtr GACALL GetParentChromosome(int index) = 0;

	};

	/// <summary><c>GaCrossoverOperation</c> class is interface for crossover operations.</summary>
	class GaCrossoverOperation : public Common::GaOperation
	{

	public:

		/// <summary>This operator performs crossover operation over parent chromosomes and creates offspring.</summary>
		/// <param name="crossoverBuffer">reference to buffer that contains parent chromosomes and that will store offspring chromosomes.</param>
		/// <param name="parameters">reference to parameters of crossover operation.</param>
		virtual void GACALL operator ()(GaCrossoverBuffer& crossoverBuffer,
			const GaCrossoverParams& parameters) const = 0;

		/// <summary><c>ExecutionProbability</c> method determins whether the crossover should be executed over specified chromosome based on crossover probability.
		///
		/// This method is thread-safe.</summary>
		/// <param name="crossoverBuffer">reference to buffer that contains parent chromosomes.</param>
		/// <param name="parameters">paramters of crossover operation.</param>
		/// <returns>Method returns <c>true</c> if the crossover operation should be executed.</returns>
		virtual bool ExecutionProbability(const GaCrossoverBuffer& crossoverBuffer,
			const GaCrossoverParams& parameters) const { return parameters.PerformeOperation(); } 

		/// <summary><c>GetParentCount</c> method calculates number of parent chromosomes required by this operation based on operation's parameters.</summary>
		/// <param name="parameters">reference to crossover parameters that will be used by this operation to produce offspring chromosome.</param>
		/// <summary>Method returns number of parent chromosomes required by this operation.</summary>
		virtual int GACALL GetParentCount(const GaCrossoverParams& parameters) const = 0;

		/// <summary><c>GetOffspringCount</c> method calculates number of chromosomes that this operation will produce based on crossover parameters.</summary>
		/// <param name="parameters">reference to crossover parameters that will be used by this operation to produce offspring chromosome.</param>
		/// <returns>Method returns number of chromosomes that this operation will produce based on the provided parameters.</returns>
		virtual int GACALL GetOffspringCount(const GaCrossoverParams& parameters) const = 0;

		/// <summary>More details are given in specification of <see cref="GaOperation::CreateConfiguration" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual Common::GaConfiguration* GACALL CreateConfiguration() const { return NULL; }

	};

	/// <summary><c>GaCrossoverSetup</c> type is instance of <see cref="GaOperationSetup" /> template class and represents setup of
	/// crossover operation and its parameters. Detailed description can be found in specification of
	/// <see cref="GaOperationSetup" /> template class.</summary>
	typedef Common::GaOperationSetup<GaCrossoverOperation, GaCrossoverParams> GaCrossoverSetup;

	/// <summary><c>GaCrossoverCatalogue</c> type is instance of <see cref="GaCatalogue" /> template class and represents catalogue of crossover operations.
	/// Detailed description can be found in specification of <see cref="GaCatalogue" /> template class.</summary>
	typedef Common::Data::GaCatalogue<GaCrossoverOperation> GaCrossoverCatalogue;

	/// <summary><c>GaMutationParams</c>is base class for parameters of mutation operations.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaMutationParams : public Common::GaOperationProbabilityParams
	{

	protected:

		/// <summary>Indicates whether the framework should accept only the mutations that improve fitness value of chromosome.</summary>
		bool _improvingOnlyMutations;

	public:

		/// <summary>This constructor initializes parameters with user-defined values.</summary>
		/// <param name="mutationProbability">mutation probability in interval (0, 1).</param>
		/// <param name="improvingOnlyMutations">if this parameter is set to <c>true</c>, only mutations that improve fitness should be accepted during production of chromosomes.</param>
		GaMutationParams(float mutationProbability,
			bool improvingOnlyMutations) : GaOperationProbabilityParams(mutationProbability),
			_improvingOnlyMutations(improvingOnlyMutations) { }

		/// <summary>This constructor initializes parameters with default values.
		/// <br/>1. mutation probability: 3%
		/// <br/>2. only improving mutations are accepted: yes.</summary>
		GaMutationParams() : GaOperationProbabilityParams(0.03f),
			_improvingOnlyMutations(true) { }

		/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
		///
		/// This method is not thread-safe.</summary>
		virtual Common::GaParameters* GACALL Clone() const { return new GaMutationParams( *this ); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if only the mutations that improve fitness are accepted. If all mutation are accepted, then this method return <c>false</c>.</returns>
		inline bool GACALL GetImprovingMutationsFlag() const { return _improvingOnlyMutations; }

		/// <summary><c>SetImprovingMutationsFlag</c> method sets "only improving mutation" flag.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="improvingOnly">if this parameter is set to <c>true</c>, only mutations that improve fitness should be accepted during production of chromosomes.</param>
		inline void GACALL SetImprovingMutationsFlag(bool improvingOnly) { _improvingOnlyMutations = improvingOnly; }

	};

	/// <summary><c>GaMutationSizeTypeParams</c> is base class for parameters of mutation operations that require mutation size to be specified.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaMutationSizeParams : public GaMutationParams
	{

	protected:

		/// <summary>Stores mutation size.</summary>
		union
		{

			/// <summary>Interprets mutation size as absolute value.</summary>
			int _absolute;

			/// <summary>Interprets mutation size as relative value.</summary>
			float _relative;

		} _mutationSize;

		/// <summary>Type of mutations size that user set.</summary>
		enum
		{

			/// <summary>Absolute mutation size.</summary>
			ST_ABSOLUTE_SIZE,

			/// <summary>Relative mutation size.</summary>
			ST_RELATIVE_SIZE

		} _sizeType;

	public:

		/// <summary>This constructor initializes parameters with absolute mutation size and other user-defined values.</summary>
		/// <param name="mutationProbability">mutation probability in interval (0, 1).</param>
		/// <param name="improvingOnlyMutations">if this parameter is set to <c>true</c>, only mutations that improve fitness should be accepted during production of chromosomes.</param>
		/// <param name="absoluteSize">number of values of chromosome's code that should be mutated.</param>
		GaMutationSizeParams(float mutationProbability,
			bool improvingOnlyMutations,
			int absoluteSize) : GaMutationParams(mutationProbability, improvingOnlyMutations) { SetAbsoluteMutationSize( absoluteSize ); }

		/// <summary>This constructor initializes parameters with relative mutation size and other user-defined values.</summary>
		/// <param name="mutationProbability">mutation probability in interval (0, 1).</param>
		/// <param name="improvingOnlyMutations">if this parameter is set to <c>true</c>, only mutations that improve fitness should be accepted during production of chromosomes.</param>
		/// <param name="relativeSize">percent of number of values of chromosome's code that should be mutated. Value is in interval (0, 1).</param>
		GaMutationSizeParams(float mutationProbability,
			bool improvingOnlyMutations,
			float relativeSize) : GaMutationParams(mutationProbability, improvingOnlyMutations)  { SetRelativeMutationSize( relativeSize ); }

		/// <summary>This constructor initializes parameters with default values.
		/// <br/>1. mutation probability: 3%
		/// <br/>2. only improving mutations are accepted: yes.</summary>
		/// <br/>3. mutation size is absolute value: 1.</summary>
		GaMutationSizeParams() { SetAbsoluteMutationSize( 1 ); }

		/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
		///
		/// This method is not thread-safe.</summary>
		virtual Common::GaParameters* GACALL Clone() const { return new GaMutationSizeParams( *this ); }

		/// <summary><c>SetAbsoluteMutationSize</c> method sets absolute mutation size.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="size">number of values of chromosome's code that should be mutated.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown when user tries to set mutation size to negative value or zero.</exception>
		inline void GACALL SetAbsoluteMutationSize(int size)
		{
			GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, size >= 0, "size", "Mutation size must be greater then 0.", "Chromosomes" );

			_mutationSize._absolute = size;
			_sizeType = ST_ABSOLUTE_SIZE;
		}

		/// <summary>This method is not tread-safe.</summary>
		/// <returns>Method returns absolute number of values of chromosome's code that should be mutated.</returns>
		/// <exception cref="GaInvalidOperationException" />Thrown when user did not set absolute mutation size.</exception>
		inline int GACALL GetAbsoulteMutationSize()
		{
			GA_ASSERT( Common::Exceptions::GaInvalidOperationException, _sizeType == ST_ABSOLUTE_SIZE, "Parameters does not contain absolute mutation size.", "Chromosomes" );
			return _mutationSize._absolute;
		}

		/// <summary><c>SetRelativeMutationSize</c> method sets relative mutation size.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="size">percent of number of values of chromosome's code that should be mutated. Value is in interval (0, 1).</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown when user tries to set mutation size out of (0, 1) interval.</exception>
		inline void GACALL SetRelativeMutationSize(float size)
		{
			GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, size >= 0 && size <= 1, "size", "Mutation size must be in range (0, 1).", "Chromosomes" );

			_mutationSize._relative = size;
			_sizeType = ST_RELATIVE_SIZE;
		}

		/// <summary>This method is not tread-safe.</summary>
		/// <returns>Method returns percent of number of values of chromosome's code that should be mutated.</returns>
		/// <exception cref="GaInvalidOperationException" />Thrown when user did not set relative mutation size.</exception>
		inline float GACALL GetRelativeMutationSize()
		{
			GA_ASSERT( Common::Exceptions::GaInvalidOperationException, _sizeType == ST_RELATIVE_SIZE, "Parameters does not contain relative mutation size.", "Chromosomes" );
			return _mutationSize._relative;
		}

		/// <summary><c>CalculateMutationSize</c> method calculates number of values of chromosome's code that should be mutated
		/// based on mutation size and length of chromosome's code.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="codeLength">length of chromosome's code.</param>
		/// <returns>Method returns calculated mutation size.</returns>
		inline int GACALL CalculateMutationSize(int codeLength) const
			{ return _sizeType == ST_RELATIVE_SIZE ? (int)( codeLength * _mutationSize._relative ) : ( _mutationSize._absolute <= codeLength ? _mutationSize._absolute : codeLength ); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if mutation size is absolute value.</returns>
		inline bool IsAbsoluteSize() const { return _sizeType == ST_RELATIVE_SIZE; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if mutation size is relative value.</returns>
		inline bool IsRelativeSize() const { return _sizeType == ST_RELATIVE_SIZE; }

	};

	/// <summary><c>GaMutationOperation</c> class is interface for mutation operations.</summary>
	class GaMutationOperation : public Common::GaOperation
	{

	public:

		/// <summary>This operator performs mutation operation.</summary>
		/// <param name="chromosome">reference to chromosome on which the mutation is performed.</param>
		/// <param name="parameters">parameters of mutation operation.</param>
		virtual void GACALL operator ()(GaChromosome& chromosome,
			const GaMutationParams& parameters) const = 0;

		/// <summary><c>ExecutionProbability</c> method determins whether the mutation should be executed over specified chromosome based on mutation probability.
		///
		/// This method is thread-safe.</summary>
		/// <param name="chromosome">reference to chromosome on which the mutation is performed.</param>
		/// <param name="parameters">paramters of mutation operation.</param>
		/// <returns>Method returns <c>true</c> if the mutation operation should be executed.</returns>
		virtual bool ExecutionProbability(const GaChromosome& chromosome,
			const GaMutationParams& parameters) const { return parameters.PerformeOperation(); } 

		/// <summary>More details are given in specification of <see cref="GaOperation::CreateConfiguration" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual Common::GaConfiguration* GACALL CreateConfiguration() const { return NULL; }

	};

	/// <summary><c>GaMutationSetup</c> type is instance of <see cref="GaOperationSetup" /> template class and represents setup of
	/// mutation operation and its parameters. Detailed description can be found in specification of
	/// <see cref="GaOperationSetup" /> template class.</summary>
	typedef Common::GaOperationSetup<GaMutationOperation, GaMutationParams> GaMutationSetup;

	/// <summary><c>GaMutationCatalogue</c> type is instance of <see cref="GaCatalogue" /> template class and represents catalogue of mutation operations.
	/// Detailed description can be found in specification of <see cref="GaCatalogue" /> template class.</summary>
	typedef Common::Data::GaCatalogue<GaMutationOperation> GaMutationCatalogue;

	/// <summary><c>GaMatingParams</c> is base class for parameters of chromosomes' mating operations.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaMatingParams : public Common::GaParameters { };

	/// <summary><c>GaMatingConfig</c> class represents configuration of mating operation and it contains operations and their parameters that are performed during mating.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaMatingConfig : public Common::GaConfiguration
	{

	protected:

		/// <summary>Crossover operation and its parameters.</summary>
		GaCrossoverSetup _crossover;

		/// <summary>Mutation operation and its parameters.</summary>
		GaMutationSetup _mutation;

	public:

		/// <summary>This constructor initializes configuration with crossover and mutation operations and their parameters.</summary>
		/// <param name="crossover">crossover operation setup.</param>
		/// <param name="mutation">mutation operation setup.</param>
		GaMatingConfig(const GaCrossoverSetup& crossover,
			const GaMutationSetup& mutation) : _crossover(crossover),
			_mutation(mutation) { }

		/// <summary>This constructor initializes configuration with crossover operation and its parameters.</summary>
		/// <param name="crossover">crossover operation setup.</param>
		GaMatingConfig(const GaCrossoverSetup& crossover) : _crossover(crossover) { }

		/// <summary>This constructor initializes configuration with mutation operation and its parameters.</summary>
		/// <param name="mutation">mutation operation setup.</param>
		GaMatingConfig(const GaMutationSetup& mutation) : _mutation(mutation) { }

		/// <summary>Default constructor initializes matin configuration with no specified crossover and mutation.</summary>
		GaMatingConfig() { }

		/// <summary>More details are given in specification of <see cref="GaConfiguration::Clone" /> method.
		///
		/// This method is not thread-safe.</summary>
		virtual Common::GaConfiguration* GACALL Clone() const { return new GaMatingConfig( *this ); }

		/// <summary><c>Crossover</c> method performs crossover operation using parent chromosomes and produce offspring.
		/// The number of chromosomes that will be produced can be queried using <see cref="GetOffspringCount" /> method and number of
		/// required parents can be obtained using <see cref="GetParentCount"/>.</summary>
		/// <param name="crossoverBuffer">reference to buffer that contains parent chromosomes and that will store offspring chromosomes.</param>
		inline void GACALL Crossover(GaCrossoverBuffer& crossoverBuffer) const { _crossover.GetOperation()( crossoverBuffer, _crossover.GetParameters() ); }

		/// <summary><c>CrossoverProbability</c> method determins whether the crossover should be executed over specified chromosome based on crossover probability.
		///
		/// This method is thread-safe.</summary>
		/// <param name="crossoverBuffer">reference to buffer that contains parent chromosomes.</param>
		/// <returns>Method returns <c>true</c> if the crossover operation should be executed.</returns>
		inline bool GACALL CrossoverProbability(const GaCrossoverBuffer& crossoverBuffer) const
			{ return _crossover.GetOperation().ExecutionProbability( crossoverBuffer, _crossover.GetParameters() ); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns number of parent chromosomes required by crossover operation to produce offspring.</returns>
		inline int GACALL GetParentCount() const { return _crossover.GetOperation().GetParentCount( _crossover.GetParameters() ); }

		/// <summary><c>GetOffspringCount</c> method is used to determine buffer size for <see cref="Crossover" /> operation.
		///
		/// This method is not thread-safe.</summary>
		/// <returns>Method returns number of offspring chromosomes that crossover operation produces.</returns>
		inline int GACALL GetOffspringCount() const { return _crossover.GetOperation().GetOffspringCount( _crossover.GetParameters() ); }

		/// <summary><c>Mutation</c> method performs mutation operation over specified chromosome.
		///
		/// This method is not thread-safe,</summary>
		/// <param name="chromosome">smart pointer to chromosome over which the operation is performed.</param>
		inline void GACALL Mutation(GaChromosomePtr chromosome) const { _mutation.GetOperation()( *chromosome, _mutation.GetParameters() ); }

		/// <summary><c>MutationProbability</c> method determins whether the mutation should be executed over specified chromosome based on mutation probability.
		///
		/// This method is thread-safe.</summary>
		/// <param name="chromosome">reference to chromosome on which the mutation is performed.</param>
		/// <returns>Method returns <c>true</c> if the mutation operation should be executed.</returns>
		inline bool GACALL MutationProbability(GaChromosomePtr chromosome) const
			{ return _mutation.GetOperation().ExecutionProbability( *chromosome, _mutation.GetParameters() ); }

		/// <summary><c>SetCrossover</c> method sets crossover operation that will be used by mating operation.
		///
		/// This method is thread-safe.</summary>
		/// <param name="operation">crossover operation setup.</param>
		inline void GACALL SetCrossover(const GaCrossoverSetup& operation) { _crossover = operation; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to setup of crossover operation and its parameters.</returns>
		inline GaCrossoverSetup& GACALL GetCrossover() { return _crossover; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to setup of crossover operation and its parameters.</returns>
		inline const GaCrossoverSetup& GACALL GetCrossover() const { return _crossover; }

		/// <summary><c>SetMutation</c> method sets mutation operation that will be used by mating operation.
		///
		/// This method is thread-safe.</summary>
		/// <param name="operation">mutation operation setup.</param>
		inline void GACALL SetMutation(const GaMutationSetup& operation) { _mutation = operation; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to setup of mutation operation and its parameters.</returns>
		inline GaMutationSetup& GACALL GetMutation() { return _mutation; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to setup of mutation operation and its parameters.</returns>
		inline const GaMutationSetup& GACALL GetMutation() const { return _mutation; }

	};

	/// <summary><c>GaMatingOperation</c> class is interface for mating operations that represent complete process of offspring chromosome production.</summary>
	class GaMatingOperation : public Common::GaOperation
	{

	public:

		/// <summary>This operator performs mating operation.</summary>
		/// <param name="operations">reference to object that stores genetic operations and their parameters that are going to be used in mating process.</param>
		/// <param name="crossoverBuffer">reference to buffer that contains parent chromosomes and that will store offspring chromosomes.</param>
		/// <param name="parameters">parameters of mating operation.</param>
		virtual void GACALL operator ()(const GaMatingConfig& operations,
			GaCrossoverBuffer& crossoverBuffer,
			const GaMatingParams& parameters) const = 0;

		/// <summary>More details are given in specification of <see cref="GaOperation::CreateConfiguration" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual Common::GaConfiguration* GACALL CreateConfiguration() const { return new GaMatingConfig(); }

	};

	/// <summary><c>GaMatingSetup</c> type is instance of <see cref="GaOperationSetup" /> template class and represents setup of
	/// mating operation and its parameters. Detailed description can be found in specification of
	/// <see cref="GaOperationSetup" /> template class.</summary>
	typedef Common::GaOperationSetup<GaMatingOperation, GaMatingParams, GaMatingConfig> GaMatingSetup;

	/// <summary><c>GaMatingCatalogue</c> type is instance of <see cref="GaCatalogue" /> template class and represents catalogue of mating operations.
	/// Detailed description can be found in specification of <see cref="GaCatalogue" /> template class.</summary>
	typedef Common::Data::GaCatalogue<GaMatingOperation> GaMatingCatalogue;

	/// <summary><c>GaChromosomeComparatorParams</c> is base class for parameters of chromosome comparators.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaChromosomeComparatorParams : public Common::GaParameters { };

	/// <summary><c>GaChromosomeComparatorConfig</c> is interface class for configuration of chromosome comparators.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaChromosomeComparatorConfig : public Common::GaConfiguration { };

	/// <summary><c>GaChromosomeComparator</c> class is interface for chromosome comparators.</summary>
	class GaChromosomeComparator : public Common::GaOperation
	{

	public:

		/// <summary>This operator compares codes of specified chromosomes.</summary>
		/// <param name="chromosome1">reference to the first chromosomes.</param>
		/// <param name="chromosome2">reference to the second chromosomes.</param>
		/// <param name="parameters">reference to parameters of chromosome comparison operation.</param>
		/// <returns>Operator returns coefficient of similarity of codes in interval (0, 1).</returns>
		virtual float GACALL operator ()(const GaChromosome& chromosome1,
			const GaChromosome& chromosome2,
			const GaChromosomeComparatorParams& parameters) const = 0;

		/// <summary>This operator compares codes of specified chromosomes.</summary>
		/// <param name="chromosome1">reference to the first chromosomes.</param>
		/// <param name="chromosome2">reference to the second chromosomes.</param>
		/// <param name="parameters">reference to parameters of chromosome comparison operation.</param>
		/// <returns>Operator returns <c>true</c> if the codes of both chromosomes are same, otherwise it returns <c>false</c>.</returns>
		virtual bool GACALL Equal(const GaChromosome& chromosome1,
			const GaChromosome& chromosome2,
			const GaChromosomeComparatorParams& parameters) const = 0;

		/// <summary>More details are given in specification of <see cref="GaOperation::CreateConfiguration" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual Common::GaConfiguration* GACALL CreateConfiguration() const { return NULL; }

	};

	/// <summary><c>GaChromosomeComparatorSetup</c> type is instance of <see cref="GaOperationSetup" /> template class and represents setup of
	/// chromosome comparators and its parameters. Detailed description can be found in specification of
	/// <see cref="GaOperationSetup" /> template class.</summary>
	typedef Common::GaOperationSetup<GaChromosomeComparator, GaChromosomeComparatorParams, GaChromosomeComparatorConfig> GaChromosomeComparatorSetup;

	/// <summary><c>GaChromosomeComparatorCatalogue</c> type is instance of <see cref="GaCatalogue" /> template class and represents catalog of 
	/// chromosome comparators. Detailed description can be found in specification of <see cref="GaCatalogue" /> template class.</summary>
	typedef Common::Data::GaCatalogue<GaChromosomeComparator> GaChromosomeComparatorCatalogue;

	/// <summary><c>GaChromosomeFitnessOperation</c> type is instance of <see cref="GaRawFitnessOperation" /> template class that represents
	/// interface for fitness operations of chromosome objects Detailed description can be found in specification of <see cref="GaRawFitnessOperation" /> template class.</summary>
	typedef Fitness::GaRawFitnessOperation<GaChromosome> GaChromosomeFitnessOperation;

	/// <summary><c>GaChromosomeFitnessOperationSetup</c> type is instance of <see cref="GaOperationSetup" /> template class and represents setup of
	/// fitness operation for chromosomes and its parameters. Detailed description can be found in specification of <see cref="GaOperationSetup" /> template class.</summary>
	typedef Common::GaOperationSetup<GaChromosomeFitnessOperation, Fitness::GaFitnessOperationParams, Fitness::GaFitnessOperationConfig> GaChromosomeFitnessOperationSetup;

} // Chromosome

#endif // __GA_CHROMOSOME_OPERATIONS_H__


/*! \file PopulationOperations.cpp
    \brief This file implements classes needed for genetic operations which are performed over population.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#include "Population.h"
#include "PopulationStatistics.h"

namespace Population
{

	/// <summary><c>GaAssignFitnessOp</c> class represents operation that evaluates chromosomes and assign fitness values.</summary>
	class GaAssignFitnessOp
	{

	private:

		/// <summary>Population whose chromosomes is evaluated.</summary>
		GaPopulation& _population;

	public:

		/// <summary>Initializes operation.</summary>
		/// <param name="population">population whose chromosomes is evaluated.</param>
		GaAssignFitnessOp(GaPopulation& population) : _population(population) { }

		/// <summary><c>operator ()</c> evaluates fitness value of the chromosomes.</summary>
		/// <param name="chromosome">storage object of the chromosome that should be evaluated.</param>
		/// <param name="index">index of the chromosomes.</param>
		inline void GACALL operator ()(GaChromosomeStorage& chromosome,
			int index) { _population.CalculateFitness( chromosome ); }

	};

	// Evaluate and assign fitness value to all chromosomes in the population
	void GaCombinedFitnessOperation::operator ()(GaPopulation& population,
		const Fitness::GaFitnessOperationParams& params,
		const Common::GaConfiguration& config,
		Common::Workflows::GaBranch* branch) const
	{
		GaPopulation::GaFlagType& flags = population.GetFlags();

		// re-evaluation required?
		if( flags.IsFlagSetAny( GaPopulation::GAPF_FITNESS_OPERATION_CHANGED ) )
		{
			// re-evaluate fitnesses of the chromosomes
			Common::Workflows::GaParallelExec1<GaPopulation, GaChromosomeStorage> workDist( *branch, population );
			workDist.Execute( GaAssignFitnessOp( population ), false );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
				flags.CopyMaskedFlags( GaPopulation::GAPF_COMPLETE_FITNESS_UPDATE, GaPopulation::GAPF_COMPLETE_FITNESS_UPDATE | GaPopulation::GAPF_FITNESS_OPERATION_CHANGED );
		}
	}

	// Initializes operation timer
	GaOperationTime::GaOperationTime(GaPopulation& population,
		int operation) : _population(population),
		_operation(operation),
		_startTime(0)
	{
		if( _population.IsTrackerRegistered( GaOperationTimeTracker::TRACKER_ID ) )
			_startTime = population.GetStatistics().GetCurrentTimeHighRes();
	}

	// Updates timing statistics of the population
	void GaOperationTime::UpdateStatistics()
	{
		// is the operation time tracked
		if( _population.IsTrackerRegistered( GaOperationTimeTracker::TRACKER_ID ) )
		{
			// get amount of time it took one branch to execute operation
			Statistics::GaStatistics& stats = _population.GetStatistics();
			long long time = stats.GetCurrentTimeHighRes() - _startTime;

			GA_LOCK_OBJECT( lock, &stats );

			// get the longest time of all branches
			if( stats.GetValue<long long>( _operation ).GetCurrentValue() < time )
				stats.GetValue<long long>( _operation ).SetCurrent( time );
		}
	}

	// Stores counters from crossover buffer
	void GaCouplingCounters::CollectCrossoverBufferCounters(const Chromosome::GaCrossoverBuffer& buffer)
	{
		const GaCrossoverStorageBuffer& b = (const GaCrossoverStorageBuffer&)buffer;
		_matingCounter++;

		_crossoverCounter += b.GetOperationCounter( GaCrossoverStorageBuffer::GAOC_CROSSOVER_COUNT );
		_mutationCounter += b.GetOperationCounter( GaCrossoverStorageBuffer::GAOC_MUTATION_COUNT );
		_acceptedMutationCounter += b.GetOperationCounter( GaCrossoverStorageBuffer::GAOC_ACCEPTED_MUTATION_COUNT );
	}

	// Updates operation counter statistics
	void GaCouplingCounters::UpdateStatistics()
	{
		Statistics::GaStatistics& stats = _population.GetStatistics();

		// is the operations count tracked
		if( _population.IsTrackerRegistered( GaOperationCountTracker::TRACKER_ID ) )
		{
			GA_LOCK_OBJECT( lock, &stats );

			// sum mating counters of all branches
			Statistics::GaValueHistory<int>* value = &stats.GetValue<int>( GADV_MATING_COUNT );
			value->SetCurrent( value->GetCurrentValue() + _matingCounter );

			// sum crossover counters of all branches
			value = &stats.GetValue<int>( GADV_CROSSOVER_COUNT );
			value->SetCurrent( value->GetCurrentValue() + _crossoverCounter );

			// sum mutation counters of all branches
			value = &stats.GetValue<int>( GADV_MUTATION_COUNT );
			value->SetCurrent( value->GetCurrentValue() + _mutationCounter );

			// sum accepted mutation counters of all branches
			value = &stats.GetValue<int>( GADV_ACCEPTED_MUTATION_COUNT );
			value->SetCurrent( value->GetCurrentValue() + _acceptedMutationCounter );
		}

		// update time
		GaOperationTime::UpdateStatistics();
	}

	// Updates operation counter statistics
	void GaSelectionCounters::UpdateStatistics()
	{
		Statistics::GaStatistics& stats = _population.GetStatistics();

		// is the operations count tracked
		if( _population.IsTrackerRegistered( GaOperationCountTracker::TRACKER_ID ) )
		{
			GA_LOCK_OBJECT( lock, &stats );

			// sum selection counters of all branches
			Statistics::GaValueHistory<int>* value = &stats.GetValue<int>( GADV_SELECTION_COUNT );
			value->SetCurrent( value->GetCurrentValue() + _selectionCounter );
		}

		// update time and counters
		GaCouplingCounters::UpdateStatistics();
	}

	// Prepares data for selection operation
	void GaSelectionOperation::Prepare(GaPopulation& population,
		GaChromosomeGroup& output,
		const GaSelectionParams& parameters,
		const GaCouplingConfig& configuration,
		int branchCount) const
	{
		int crossoverBufferTagID = parameters.GetCrossoverBuffersTagID();
		const Chromosome::GaMatingConfig& config = configuration.GetMating().GetConfiguration();

		// setup output chromsome group
		output.Clear();
		output.SetSize( GetSelectionCount( parameters, &config ) );
		output.SetPopulation( &population );
		output.SetObjectRecycling( crossoverBufferTagID >= 0 );

		// creating crossover buffers if selection operation should produce offspring chromosomes
		if( crossoverBufferTagID >= 0 )
		{
			int index = population.GetTagManager().AddTag( crossoverBufferTagID, Common::Data::GaSizableTagLifecycle<GaPartialCrossoverBuffer>() );
			population.GetTagByIndex<GaPartialCrossoverBuffer>( index ).SetSize( branchCount,
				GaCrossoverStorageBuffer( &population, config.GetParentCount(), config.GetOffspringCount() ) );
		}
	}

	// Clears changes made by selection operation
	void GaSelectionOperation::Clear(GaPopulation& population,
		GaChromosomeGroup& output,
		const GaSelectionParams& parameters,
		const GaCouplingConfig& configuration,
		int branchCount) const
	{
		output.Clear();
		population.GetTagManager().RemoveTag( parameters.GetCrossoverBuffersTagID() );
	}

	// Updates data used by the operation when workflow has changed
	void GaSelectionOperation::Update(GaPopulation& population,
		GaChromosomeGroup& output,
		const GaSelectionParams& parameters,
		const GaCouplingConfig& configuration,
		int branchCount) const
	{
		int crossoverBufferTagID = parameters.GetCrossoverBuffersTagID();
		if( crossoverBufferTagID >= 0 )
		{
			// setup new chrossover buffers
			const Chromosome::GaMatingConfig& config = configuration.GetMating().GetConfiguration();
			population.GetTagByID<GaPartialCrossoverBuffer>( crossoverBufferTagID ).SetSize( branchCount,
				GaCrossoverStorageBuffer( &population, config.GetParentCount(), config.GetOffspringCount() ) );
		}
	}

	// Prepares data for coupling operation
	void GaCouplingOperation::Prepare(GaChromosomeGroup& input,
		GaChromosomeGroup& output,
		const GaCouplingParams& parameters,
		const GaCouplingConfig& configuration,
		int branchCount) const
	{
		// setup output chromsome group
		output.Clear();
		output.SetSize( parameters.GetNumberOfOffsprings() );
		output.SetObjectRecycling( true );

		Population::GaPopulation* population = input.GetPopulation();

		// creating crossover buffers if selection operation should prodice offspring chromosomes
		const Chromosome::GaMatingConfig& config = configuration.GetMating().GetConfiguration();

		int index = population->GetTagManager().AddTag( parameters.GetCrossoverBuffersTagID(), Common::Data::GaSizableTagLifecycle<GaPartialCrossoverBuffer>() );
		population->GetTagByIndex<GaPartialCrossoverBuffer>( index ).SetSize( branchCount,
			GaCrossoverStorageBuffer( input.GetPopulation(), config.GetParentCount(), config.GetOffspringCount() ) );
	}

	// Clears changes made by coupling operation
	void GaCouplingOperation::Clear(GaChromosomeGroup& input,
		GaChromosomeGroup& output,
		const GaCouplingParams& parameters,
		const GaCouplingConfig& configuration,
		int branchCount) const
	{
		input.GetPopulation()->GetTagManager().RemoveTag( parameters.GetCrossoverBuffersTagID() );
		output.Clear();
	}

	// Updates data used by the operation when workflow has changed
	void GaCouplingOperation::Update(GaChromosomeGroup& input,
		GaChromosomeGroup& output,
		const GaCouplingParams& parameters,
		const GaCouplingConfig& configuration,
		int branchCount) const
	{
		Population::GaPopulation* population = input.GetPopulation();

		// setup new chrossover buffers
		const Chromosome::GaMatingConfig& config = configuration.GetMating().GetConfiguration();
		population->GetTagByID<GaPartialCrossoverBuffer>( parameters.GetCrossoverBuffersTagID() ).SetSize( branchCount,
			GaCrossoverStorageBuffer( population, config.GetParentCount(), config.GetOffspringCount() ) );
	}

	// Prepares data for scaling operation
	void GaScalingOperation::Prepare(GaPopulation& population,
		const GaScalingParams& parameters,
		const GaScalingConfig& configuration,
		int branchCount) const
	{
		// set prototype of fitness object that is going to be used for storeing scaled fitnesses
		Common::Memory::GaAutoPtr<Fitness::GaFitness> newFitnessPrototype = CreateFitnessObject( configuration.GetFitnessParams() );
		population.SetScaledFitnessPrototype( newFitnessPrototype.GetRawPtr() );

		Update( population, parameters, configuration, branchCount );
	}

	// Clears changes made by coupling operation
	void GaScalingOperation::Clear(GaPopulation& population,
		const GaScalingParams& parameters,
		const GaScalingConfig& configuration,
		int branchCount) const { population.SetScaledFitnessPrototype( NULL ); }

} // Population

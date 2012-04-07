
/*! \file Population.cpp
	\brief This file conatins implementation of classes that manages chromosome populations.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#include "PopulationStatistics.h"
#include "Population.h"

namespace Population
{

	// Constructor population
	GaPopulation::GaPopulation(const GaPopulationParams& parameters,
		const Chromosome::GaInitializatorSetup& initializator,
		const GaPopulationFitnessOperationSetup& fitnessOperation,
		const Fitness::GaFitnessComparatorSetup& fitnessComparator) : _parameters(parameters),
		_initializator(initializator),
		_chromosomes(NULL),
		_currentSize(0),
		_sorting(true),
		_chromosomeStoragePool(parameters.GetPopulationSize()),
		_newChromosomes(true, 0, GaChromosomeStorage::GACF_NEW_CHROMOSOME),
		_removedChromosomes(true, 0, 0, this, true)
	{
		_chromosomeStoragePool.GetObjectCreate().SetPopulation( this );
		_chromosomeStoragePool.GetObjectInit().SetPopulation( this );
		_chromosomeStoragePool.GetObjectClean().SetPopulation( this );

		_chromosomeTagManager = new Common::Data::GaTagManager( 4, new GaChromosomeTagManagerUpdate( this ) );

		_tagManager = new Common::Data::GaTagManager( 4, new GaPopulationTagManagerUpdate( this ) );

		SetFitnessOperation( fitnessOperation );
		SetFitnessComparator( fitnessComparator );
	}

	// Initializes population
	void GaPopulation::Initialize()
	{
		GA_ASSERT( Common::Exceptions::GaInvalidOperationException, !_flags.IsFlagSetAll( GAPF_INITIALIZED ), "Population is already initialized.", "Population" );

		_statistics.ResetTime();

		if( !_chromosomes )
		{
			// create new array that will store chromosomes and update sorting algorithm
			_array.SetSize( _parameters.GetPopulationSize() );
			_helper.SetSize( _array.GetSize() );
			_chromosomes = _array.GetArray();
			_sorting.SetArray( _chromosomes, _helper.GetArray(), &_currentSize );
		}

		if( _parameters.GetFillOptions().IsFlagSetAny( GaPopulationParams::GAPFO_FILL_ON_INIT ) )
		{
			// determin number of chromosomes which should be inserted
			int fillSize = _parameters.GetPopulationSize();
			if( !_parameters.GetFillOptions().IsFlagSetAny( GaPopulationParams::GAPFO_FILL_CROWDING_SPACE ) )
				fillSize -= _parameters.GetCrowdingSize();

			// fill population with chromosomes made from prototype if required
			for( int i = fillSize - 1; i >= 0 ; i-- )
			{
				_chromosomes[ i ] = MakeFromPrototype();
				_newChromosomes.Add( _chromosomes[ i ] );
			}

			_currentSize = fillSize;
		}
		else
			_currentSize = 0;

		// mark population as initialized
		_flags.SetFlags( GAPF_INITIALIZED );
	}

	// Clears population
	void GaPopulation::Clear(bool clearStats)
	{
		_newChromosomes.Clear();
		_removedChromosomes.Clear();

		// remove chromosomes from population
		for( _currentSize = _currentSize - 1; _currentSize >= 0; _currentSize-- )
			_chromosomeStoragePool.ReleaseObject( _chromosomes[ _currentSize ] );

		// clear statistics
		if( clearStats )
			_statistics.Clear( false );

		// mark population as uninitialized
		_flags.ClearFlags( GAPF_INITIALIZED );
	}

	// Removes chromosome from the population
	void GaPopulation::Remove(int chromosomeIndex)
	{
		// Update book-keeping
		Remove( _chromosomes[ chromosomeIndex ] );

		_currentSize--;

		// move other chromosomes to fill the gap
		for( int i = chromosomeIndex; i < _currentSize; i++ )
			_chromosomes[ i ] = _chromosomes[ i + 1 ];
	}

	// Removes chromosomes from the population
	void GaPopulation::RemoveSorted(int* chromosomeIndices,
		int count)
	{
		if( count )
		{
			int* end = chromosomeIndices + count;
			for( int i = *chromosomeIndices, j = *chromosomeIndices; i < _currentSize; i++ )
			{
				// chromosome should be removed?
				if( chromosomeIndices == end || i != *chromosomeIndices )
					// this chromosome stays - move it to fill the gap
					_chromosomes[ j++ ] = _chromosomes[ i ];
				else
				{
					// Update book-keeping
					Remove( _chromosomes[ i ] );
					chromosomeIndices++;
				}
			}

			_currentSize -= count;
		}
	}

	// Removes chromosomes from the population
	void GaPopulation::RemoveUnsorted(int* chromosomeIndices,
		int count)
	{
		if( count )
		{
			// removes chromosomes
			for( int i = count - 1; i >= 0; i-- )
			{
				int index = chromosomeIndices[ i ];
				Remove( _chromosomes[ index ] );
				_chromosomes[ index ] = NULL;
			}

			// compact population
			int newSize = 0;
			for( int i = 0; i < _currentSize; i++ )
			{
				if( _chromosomes[ i ] != NULL )
					_chromosomes[ newSize++ ] = _chromosomes[ i ];
			}

			_currentSize = newSize;
		}
	}

	// Remvoes marked chromosomes from population
	void GaPopulation::Remove()
	{
		int newSize = 0;
		for( int i = 0; i < _currentSize; i++ )
		{
			// keep chromosome?
			if( !_chromosomes[ i ]->GetFlags().IsFlagSetAny( GaChromosomeStorage::GACF_REMOVE_CHROMOSOME ) )
				_chromosomes[ newSize++ ] = _chromosomes[ i ];
			else
				Remove( _chromosomes[ i ] );
		}

		_currentSize = newSize;
	}

	// Replaces chromosome with new one
	void GaPopulation::Replace(int oldChromosomeIndex,
		GaChromosomeStorage* newChromosome)
	{
		// move old chromosome to list of removed chromosomes
		GaChromosomeStorage* old = _chromosomes[ oldChromosomeIndex ];
		_removedChromosomes.Add( old );

		// insert new chromosome int population
		_chromosomes[ oldChromosomeIndex ] = newChromosome;
		_newChromosomes.Add( newChromosome );
	}

	// Prepares population for next generation
	void GaPopulation::NextGeneration(Common::Workflows::GaBranch* branch)
	{
		// evaluate population's statistics that has not been updated
		for( STLEXT::hash_map<int, std::pair<bool, GaPopulationStatTracker*> >::iterator it = _registeredTrackers.begin(); it != _registeredTrackers.end(); ++it )
		{
			if( !it->second.first )
			{
				it->second.second->Evaluate( *this, branch );
				it->second.first = false;
			}
		}

		GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
		{
			// prepare object that stores population's statistics for new generation
			_statistics.Next();

			// clear list of chromosomes removed and added during this generation
			_newChromosomes.Clear();
			_removedChromosomes.Clear();

			_events.RaiseEvent( GAPE_NEW_GENERATION, GaPopulationEventData( this ) );

			_flags.ClearFlags( ~GAPF_INITIALIZED );
		}
	}

	// Binds tracker to population's statistics object and adds it to population's statistics trackers
	void GaPopulation::RegisterTracker(int trackerID,
		GaPopulationStatTracker* tracker)
	{
		GA_ARG_ASSERT( Common::Exceptions::GaArgumentException, _registeredTrackers.find( trackerID ) == _registeredTrackers.end(),
			"trackerID", "There is statistics tracker already registered with specified ID.", "Population" );

		// bind tracker to population's statistics object
		tracker->Bind( *this );
		
		// add tracker to the list of population's evaluators
		_registeredTrackers[ trackerID ] = std::make_pair( false, tracker );
	}

	// Unbinds tracker from population's statistics object and removes it from population's statistics trackers
	void GaPopulation::UnregisterTracker(int trackerID)
	{
		STLEXT::hash_map<int, std::pair<bool, GaPopulationStatTracker*> >::iterator it = _registeredTrackers.find( trackerID );

		GA_ARG_ASSERT( Common::Exceptions::GaArgumentException, it != _registeredTrackers.end(),
			"trackerID", "There is no statistics tracker registered with specified ID.", "Population" );

		// unbind from population's statistics and delete tracker
		it->second.second->Unbind( *this );

		// remove it from the list of population's trackers
		_registeredTrackers.erase( it );
	}

	// Updates tracked statistical values
	void GaPopulation::UpdateTracker(int trackerID,
		Common::Workflows::GaBranch* branch)
	{
		STLEXT::hash_map<int, std::pair<bool, GaPopulationStatTracker*> >::iterator it = _registeredTrackers.find( trackerID );

		GA_ARG_ASSERT( Common::Exceptions::GaArgumentException, it != _registeredTrackers.end(),
			"trackerID", "There is no statistics tracker registered with specified ID.", "Population" );

		// evaluates tracked statistical values of the population if it is not updated
		if( !it->second.first )
		{
			it->second.second->Evaluate( *this, branch );
			it->second.first = true;
		}
	}

	// Set populations parameters
	void GaPopulation::SetPopulationParams(const GaPopulationParams& parameters)
	{
		int oldSize = _parameters.GetPopulationSize();
		int newSize = parameters.GetPopulationSize();

		if( _flags.IsFlagSetAll( GAPF_INITIALIZED ) )
		{
			// calculate number of chromosomes that should be added or removed from the population when size change
			int diff = newSize - _currentSize;
			if( !parameters.GetFillOptions().IsFlagSetAny( GaPopulationParams::GAPFO_FILL_CROWDING_SPACE ) )
				diff -= parameters.GetCrowdingSize();

			// do not add new chromosomes if it is not required
			if( parameters.GetFillOptions().IsFlagSetAny( GaPopulationParams::GAPFO_FILL_ON_SIZE_CHANGE ) && diff > 0 )
				diff = 0;

			// remove chromosomes that cannot fit new size
			for( int i = diff; i < 0; i++ )
				_removedChromosomes.Add( _chromosomes[ --_currentSize ] );

			// size changed?
			if( oldSize != newSize )
			{
				// set new size of arrays that stores chromsomes and update sorting algorithm
				_array.SetSize( newSize );
				_helper.SetSize( newSize );
				_sorting.SetArray( _chromosomes, _chromosomes == _array.GetArray() ? _helper.GetArray() : _array.GetArray(), &_currentSize );

				_flags.SetFlags( GAPF_SIZE_CHANGED );
			}

			// store new parameters
			_parameters = parameters;

			// fill additional space with new chromosomes if required
			for( int i = diff; i > 0; i--, _currentSize++ )
			{
				_chromosomes[ _currentSize ] =  MakeFromPrototype();
				_newChromosomes.Add( _chromosomes[ _currentSize ] );
			}
		}
		else
			// store new parameters
			_parameters = parameters;

		_events.RaiseEvent( GAPE_POPULATION_PARAMETERS_CHANGED, GaPopulationEventData( this ) );
	}

	// Sets fitness operation that assigns fitness values to chromosomes in the population
	void GaPopulation::SetFitnessOperation(const GaPopulationFitnessOperationSetup& operation)
	{
		GA_ARG_ASSERT( Common::Exceptions::GaArgumentException, operation.HasOperation(), "operation", "Fitness operation must be specified.", "Population" );

		// sets new operation and create fitness prototype
		_fitnessOperation = operation;
		_fitnessPrototype = _fitnessOperation.GetOperation().CreateFitnessObject( _fitnessOperation.GetConfiguration().GetFitnessParams() );

		UpdateStorageObjects( GaStorageFitnessObjectsUpdate( _fitnessPrototype.GetRawPtr(), GaChromosomeStorage::GAFT_RAW ) );

		_flags.SetFlags( GAPF_FITNESS_OPERATION_CHANGED );
		_events.RaiseEvent( GAPE_FITNESS_OPERATION_CHANGED, GaPopulationEventData( this ) );
	}

	// Sets prototype of fitness object that will be used by scaling operation for storing scaled fitness values
	void GaPopulation::SetScaledFitnessPrototype(const Fitness::GaFitness* prototype)
	{
		_scaledFitnessPrototype = prototype ? prototype->Clone() : NULL;
		UpdateStorageObjects( GaStorageFitnessObjectsUpdate( _scaledFitnessPrototype.GetRawPtr(), GaChromosomeStorage::GAFT_SCALED ) );

		_flags.SetFlags( GAPF_SCALED_FITNESS_PROTOTYPE_CHANGED );
		_events.RaiseEvent( GAPE_SCALED_FITNESS_PROTOTYPE_CHANGED, GaPopulationEventData( this ) );
	}

	// Sets fitness comparator and its parameters
	void GaPopulation::SetFitnessComparator(const Fitness::GaFitnessComparatorSetup& comparator)
	{
		GA_ARG_ASSERT( Common::Exceptions::GaArgumentException, comparator.HasOperation(), "comparator", "Fitness comparator must be specified.", "Population" );

		_fitnessComparator = comparator;

		_flags.SetFlags( GAPF_FITNESS_COMPARATOR_CHANGED );
		_events.RaiseEvent( GAPE_FITNESS_COMPARATOR_CHANGED, GaPopulationEventData( this ) );
	}

	// Updates chromosome storage objects
	void GaPopulation::UpdateStorageObjects(Common::Memory::GaPoolObjectUpdate<GaChromosomeStorage>& update)
	{
		for( int i = _currentSize - 1; i >= 0; i-- )
			update( _chromosomes[ i ] );

		_chromosomeStoragePool.UpdateObjects( update );
	}

	// Inserts chromosome to the group used by crossover operation to store offspring chromosomes
	int GaCrossoverStorageBuffer::StoreOffspringChromosome(Chromosome::GaChromosomePtr chromosome,
		int parent)
	{
		// get storage object for chromosome and stores it
		GaChromosomeStorage* storage = _population->AcquireStorageObject();
		storage->SetChromosome( !chromosome.IsNull() ? chromosome : _parents[ parent ].GetChromosome(), &_parents[ parent ] );

		// evaluate fitness
		_population->CalculateFitness( *storage );

		if ( !chromosome.IsNull() )
			_operationCounters[ GAOC_CROSSOVER_COUNT ] = 1;

		// insert chromosome to offspring group
		return _offspring.Add( storage );
	}

	// Replaces chromosome in the buffer
	void GaCrossoverStorageBuffer::ReplaceOffspringChromosome(int index,
		Chromosome::GaChromosomePtr chromosome)
	{
		GaChromosomeStorage& storage = _offspring[ index ];

		_operationCounters[ GAOC_MUTATION_COUNT ]++;
		_operationCounters[ GAOC_ACCEPTED_MUTATION_COUNT ]++;

		storage.SetChromosome( chromosome, storage.GetParent() );
		_population->CalculateFitness( storage );
	}

	// Replaces chromosome in the buffer only if new chromosome is better
	bool GaCrossoverStorageBuffer::ReplaceIfBetter(int index,
		Chromosome::GaChromosomePtr newChromosome)
	{
		GaChromosomeStorage& storage = _offspring[ index ];

		_operationCounters[ GAOC_MUTATION_COUNT ]++;

		// calculate fitness of new chromosome
		if( _population->CalculateFitness( newChromosome, *_tempFitness ) )
		{
			// new chromosome is better?
			if( _population->CompareFitness( *_tempFitness, storage.GetFitness( GaChromosomeStorage::GAFT_RAW ) ) < 0 )
			{
				// store new chromosome
				_operationCounters[ GAOC_ACCEPTED_MUTATION_COUNT ]++;
				storage.SetChromosome( newChromosome, storage.GetParent() );
				storage.GetFitness( GaChromosomeStorage::GAFT_RAW ) = *_tempFitness;

				return true;
			}
		}
		else
		{
			// evaluation of single chromosome is not supported - always store new chromosome
			_operationCounters[ GAOC_ACCEPTED_MUTATION_COUNT ]++;
			storage.SetChromosome( newChromosome, storage.GetParent() );
			return true;
		}

		return false;
	}

	// Clears crossover buffer
	void GaCrossoverStorageBuffer::Clear()
	{
		_parents.Clear();
		_offspring.Clear();

		// clear counters
		for( int i = GAOC_COUNTERS - 1; i >= 0; i-- )
			_operationCounters[ i ] = 0;
	}

	// Sets population in which the mating will take place.
	void GaCrossoverStorageBuffer::SetPopulation(GaPopulation* population)
	{
		if( population != _population )
		{
			_population = population;

			// prepare output buffer
			_offspring.Clear();
			_offspring.SetPopulation( population );

			_tempFitness = population ? _population->CreateFitnessObject( GaChromosomeStorage::GAFT_RAW ) : NULL;
		}
	}

} // Population

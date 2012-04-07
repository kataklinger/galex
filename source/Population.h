
/*! \file Population.h
	\brief This file conatins declaration of classes that manages chromosome populations.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#ifndef __GA_POPULATION_H__
#define __GA_POPULATION_H__

#include "RandomSequence.h"
#include "Observing.h"
#include "Statistics.h"
#include "PopulationOperations.h"

/// <summary><c>Population</c> namespace contains classes and datatypes that manage chromosome populations.</summary>
namespace Population
{

	/// <summary><c>GaPopulationParams</c> class represents parameters of population.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaPopulationParams : public Common::GaParameters
	{

	public:

		/// <summary>This enum defines fill options of the population.</summary>
		enum GaPopulationFillOptions
		{

			/// <summary>Indicates that population should be filled with chromosomes on initialization.</summary>
			GAPFO_FILL_ON_INIT = 0x1,

			/// <summary>Indicates that additional space after increasing population's size should be filled with chromosomes.</summary>
			GAPFO_FILL_ON_SIZE_CHANGE = 0x2,

			/// <summary>Inidicates that space reserved for crowding should be filled when population is initialized or resized.</summary>
			GAPFO_FILL_CROWDING_SPACE = 0x4

		};

	private:

		/// <summary>Maximal population size (number of chromosomes in population).</summary>
		int _populationSize;

		/// <summary>Size of reserved space of population for temporal crowding of the population.</summary>
		int _crowdingSize;

		/// <summary>Stores fill options of the population.</summary>
		Common::Data::GaFlags8 _fillOptions;

	public:

		/// <summary>This constructor initializes population parameters with values.
		/// Population size: 10
		/// Size of crowding size: 0
		/// Population should be filled on initialization and size change. Crowding space is not filled.</summary>
		GaPopulationParams() : _populationSize(10),
			_crowdingSize(0),
			_fillOptions(GAPFO_FILL_ON_INIT | GAPFO_FILL_ON_SIZE_CHANGE) { }

		/// <summary>This constructor initializes parameters with population size.</summary>
		/// <param name="populationSize">maximal size of population.</param>
		/// <param name="crowdingSize">size of reserved space of population for temporal crowding of the population.</param>
		/// <param name="fillOptions">fill options of the population.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>populationSize</c> or <c>crowdingSize</c> is negative value, 
		/// also this exception is thrown if <c>populationSize</c> is set to 0.</exception>
		GaPopulationParams(int populationSize,
			int crowdingSize,
			Common::Data::GaFlags8::GaDataType fillOptions = 0) : _fillOptions(fillOptions)
		{
			SetPopulationSize( populationSize );
			SetCrowdingSize( crowdingSize );
		}

		/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
		///
		/// This method is not thread-safe.</summary>
		virtual Common::GaParameters* GACALL Clone() const { return new GaPopulationParams( *this ); }

		/// <summary><c>SetPopulationSize</c> method sets maximal population size. If current crowding size is greater then new population size,
		/// size of crowding space is reduced so it can fit new population size.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="populationSize">size of population.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>populationSize</c> is negative value or 0.</exception>
		inline void GACALL SetPopulationSize(int populationSize)
		{
			GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, populationSize > 0, "populationSize", "Size of the population must be greater then 0.", "Population" );

			_populationSize = populationSize;
			if( _crowdingSize >= _populationSize )
				_crowdingSize = _populationSize - 1;
		}

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns maximal population size (number of chromosomes in population).</returns>
		inline int GACALL GetPopulationSize() const { return _populationSize; }

		/// <summary><c>SetCrowdingSize</c> method sets size of reserved space of population for temporal crowding of the population.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="crowdingSize">new size of crowding space.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>crowdingSize</c> is negative value or if it is greter then or equal to population size.</exception>
		inline void GACALL SetCrowdingSize(int crowdingSize)
		{
			GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, crowdingSize >= 0 && crowdingSize < _populationSize,
				"crowdingSize", "Size of crowding space of the population must be positive number or 0 and lower then population size.", "Population" );

			_crowdingSize = crowdingSize;
		}

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns size of reserved space of population for temporal crowding of the population.</returns>
		inline int GACALL GetCrowdingSize() const { return _crowdingSize; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns size of space of population that is used for peramnent storage of chromosomes (population size excluding crowding space).</returns>
		inline int GACALL GetPermanentSpaceSize() const { return _populationSize - _crowdingSize; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to flags structur that stores fill options of the population.</returns>
		inline Common::Data::GaFlags8& GetFillOptions() { return _fillOptions; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to flags structur that stores fill options of the population.</returns>
		inline const Common::Data::GaFlags8& GetFillOptions() const { return _fillOptions; }

	};

	class GaCrossoverStorageBuffer;

	/// <summary><c>GaPopulationStatTracker</c> class is interface for statistic evaluators that evaluates population and produce statistical data.</summary>
	class GaPopulationStatTracker;

	/// <summary><c>GaPopulation</c> class stores chromosomes and track their statistics. Population also defines genetic operations that will be used on chromosomes.
	///
	/// This class has built-in synchronizator so it is allowed to use <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros with instances of this class,
	/// but not all methods are thread-safe.</summary>
	class GaPopulation
	{

	public:

		/// <summary>Defines IDs of population's events.</summary>
		enum GaPopulationEvents
		{

			/// <summary>ID of the event raised when the population starts to new generation.</summary>
			GAPE_NEW_GENERATION,

			/// <summary>ID of the event raised after population parameters has been changed.</summary>
			GAPE_POPULATION_PARAMETERS_CHANGED,

			/// <summary>ID of the event raised after fitness operation has been changed.</summary>
			GAPE_FITNESS_OPERATION_CHANGED,

			/// <summary>ID of the event raised after fitness comparator has been changed.</summary>
			GAPE_FITNESS_COMPARATOR_CHANGED,

			/// <summary>ID of the event raised after prototype of scaled fitness object has bee changed.</summary>
			GAPE_SCALED_FITNESS_PROTOTYPE_CHANGED

		};

		/// <summary><c>GaFlagType</c> defines flag datatype used for storing population's flags.</summary>
		typedef Common::Data::GaFlags16 GaFlagType;

		/// <summary>This enumeration defines well-know population's flags.</summary>
		enum GaPopulationFlags
		{

			/// <summary>Indicates that population is initialized.</summary>
			GAPF_INITIALIZED = 1,

			/// <summary>Indicates that fitness operation has been changed.</summary>
			GAPF_FITNESS_OPERATION_CHANGED = 2,

			/// <summary>Indicates that fitness comparator has been changed.</summary>
			GAPF_FITNESS_COMPARATOR_CHANGED = 4,

			/// <summary>Indicates that prototype of scaled fitness object been changed.</summary>
			GAPF_SCALED_FITNESS_PROTOTYPE_CHANGED = 8,

			/// <summary>Indicates that fitness operation has changed fitness values of all chromosomes in the population.</summary>
			GAPF_COMPLETE_FITNESS_UPDATE = 16,

			/// <summary>Indicates that scaling operation has changed scaled fitness values of all chromosomes in the population population.</summary>
			GAPF_COMPLETE_SCALED_FITNESS_UPDATE = 32,

			/// <summary>Indicates that size of population has been changed.</summary>
			GAPF_SIZE_CHANGED = 64,

		};

		/// <summary><c>GaStorageFitnessObjectsUpdate</c> class implements object pool update policy for chromosome storage objects used for refresing fitness object.</summary>
		class GaStorageFitnessObjectsUpdate : public Common::Memory::GaPoolObjectUpdate<GaChromosomeStorage>
		{

		private:

			/// <summary>Pointer to prototype of new fitness objects.<summary>
			Fitness::GaFitness* _prototype;

			/// <summary>Type of fitness for which the object is created.</summary>
			 GaChromosomeStorage::GaFitnessType _type;

		public:

			/// <summary>This constructor initializes update policy with fitness operation and fitness parameters that will be used for creating new fitness objects.</summary>
			/// <param name="prototype">pointer to prototype of new fitness objects.</param>
			/// <param name="type">type of fitness for which the object is created.</param>
			GaStorageFitnessObjectsUpdate(Fitness::GaFitness* prototype,
				GaChromosomeStorage::GaFitnessType type) : _prototype(prototype),
				_type(type) { }

			/// <summary><c>operator()</c> updates fitness object with object that is used by specified fitness operation.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="object">pointer chromosome storage object which should be updated.</param>
			virtual void GACALL operator ()(GaChromosomeStorage* object) const { object->SetFitnessObject( _prototype ? _prototype->Clone() : NULL, _type ); }

		};

		/// <summary><c>GaAddStorageTag</c> class implements object pool update policy for adding tags to chromosome storage objects.</summary>
		class GaAddStorageTag : public Common::Memory::GaPoolObjectUpdate<GaChromosomeStorage>
		{

		private:

			/// <summary>Index in buffer at which the new tag should be added.</summary>
			int _index;

			/// <summary>Lifecycle manager of new tag.</summary>
			const Common::Data::GaTagLifecycle& _tagLifecycle;

		public:

			/// <summary>Initializes update policy.</summary>
			/// <param name="index">in buffer at which the new tag should be added.</param>
			/// <param name="tagLifecycle">lifecycle manager of new tag.</param>
			GaAddStorageTag(int index,
				const Common::Data::GaTagLifecycle& tagLifecycle) : _index(index),
				_tagLifecycle(tagLifecycle) { }

			/// <summary><c>operator()</c> add tag at specified position using prototype to create new tag.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="object">pointer to chromosome storage object which should be updated.</param>
			virtual void GACALL operator ()(GaChromosomeStorage* object) const { object->GetTags().SetTag( _index, _tagLifecycle.CreateTag() ); }

		};

		/// <summary><c>GaRemoveStorageTag</c> class implements object pool update policy for removing tags from chromosome storage objects.</summary>
		class GaRemoveStorageTag : public Common::Memory::GaPoolObjectUpdate<GaChromosomeStorage>
		{

		private:

			/// <summary>Index of the tag that should be removed.</summary>
			int _index;

		public:

			/// <summary>Initializes update policy.</summary>
			/// <param name="index">index of the tag that should be removed.</param>
			GaRemoveStorageTag(int index) : _index(index) { }

			/// <summary><c>operator()</c> removes tag at specified position.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="object">pointer to chromosome storage object which should be updated.</param>
			virtual void GACALL operator ()(GaChromosomeStorage* object) const { object->GetTags().RemoveTag( _index ); }

		};

		/// <summary><c>GaUpdateStorageTag</c> class implements object pool update policy for updating tags of chromosome storage objects.</summary>
		class GaUpdateStorageTag : public Common::Memory::GaPoolObjectUpdate<GaChromosomeStorage>
		{

		private:

			/// <summary>Index of the tag that should be update.</summary>
			int _index;

			/// <summary>Update operation that is performed over tag.</summary>
			const Common::Data::GaTagUpdate& _update;

		public:

			/// <summary>Initializes update policy.</summary>
			/// <param name="index">index of the tag that should be updated.</param>
			/// <param name="update">update operation that is performed over tag.</param>
			GaUpdateStorageTag(int index,
				const Common::Data::GaTagUpdate& update) : _index(index),
				_update(update) { }

			/// <summary><c>operator()</c> updates tag at specified position.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="object">pointer to chromosome storage object which should be updated.</param>
			virtual void GACALL operator ()(GaChromosomeStorage* object) const { _update( object->GetTags()[ _index ] ); }

		};

		/// <summary><c>GaStorageTagsUpdate</c> class implements object pool update policy for refreshing tag buffers of chromosome storage objects.</summary>
		class GaStorageTagBuffersUpdate : public Common::Memory::GaPoolObjectUpdate<GaChromosomeStorage>
		{

		private:

			/// <summary>New size of tag buffer.</summary>
			int _newSize;

		public:

			/// <summary>This constructor initializes update policy with tag manager responsible for tag buffer and size of buffer.</summary>
			/// <param name="newSize">new size of tag buffer.</param>
			GaStorageTagBuffersUpdate(int newSize) : _newSize(newSize) { }

			/// <summary><c>operator()</c> updates tag buffers used by chromosome storage object.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="object">pointer to chromosome storage object which should be updated.</param>
			virtual void GACALL operator ()(GaChromosomeStorage* object) const { object->GetTags().SetSize( _newSize ); }

		};

		/// <summary><c>GaChromosomeTagManagerUpdate</c> class represent update policy for tag buffers that stores chromosomes' tags.</summary>
		class GaChromosomeTagManagerUpdate : public Common::Data::GaTagBufferUpdate
		{

		private:

			/// <summary>Population that contains chromosomes whose tag buffers are updated.</summary>
			GaPopulation* _population;

		public:

			/// <summary>Initializes update object and bound it to specified population.</summary>
			/// <param name="population">pinter to population that contains chromosomes whose tag buffers are updated.</param>
			GaChromosomeTagManagerUpdate(GaPopulation* population) : _population(population) { }

			/// <summary>Initializes unbound update object.</summary>
			GaChromosomeTagManagerUpdate() : _population(NULL) { }

			/// <summary><c>AddTag</c> method adds tag to buffers of all chromosomes in the population.
			///
			/// This method is thread-safe.</summary>
			/// <param name="index">index at which the tag should be inserted into buffer.</param>
			/// <param name="tagLifecycle">lifecycle manager of new tag.</param>
			virtual void GACALL AddTag(int index,
				const Common::Data::GaTagLifecycle& tagLifecycle) const { _population->UpdateStorageObjects( GaAddStorageTag( index, tagLifecycle ) ); }

			/// <summary><c>RemoveTag</c> method removes tag from the buffers of all chromosomes in the population.
			///
			/// This method is thread-safe.</summary>
			/// <param name="index">index of the tag that should be removed.</param>
			virtual void GACALL RemoveTag(int index) const { _population->UpdateStorageObjects( GaRemoveStorageTag( index ) ); }

			/// <summary><c>UpdateTag</c> method updates tag in buffers of all chromosomes in the population.
			///
			/// This method is thread-safe.</summary>
			/// <param name="index">index of the tag that should be updated.</param>
			/// <param name="update">update operation that is performed over tag.</param>
			virtual void GACALL UpdateTag(int index,
				const Common::Data::GaTagUpdate& update) const { _population->UpdateStorageObjects( GaUpdateStorageTag( index, update ) ); }

			/// <summary><c>SetBufferSize</c> method updates size of tag buffer of all chromosomes in the population.
			///
			/// This method is thread-safe.</summary>
			/// <param name="newSize">new buffer size.</param>
			virtual void GACALL SetBufferSize(int newSize) const { _population->UpdateStorageObjects( GaStorageTagBuffersUpdate( newSize ) ); }

		};

		/// <summary><c>GaPopulationTagManagerUpdate</c> class represent update policy for tag buffers that stores populations' tags.</summary>
		class GaPopulationTagManagerUpdate : public Common::Data::GaTagBufferUpdate
		{

		private:

			/// <summary>Population whose tag buffer is updated.</summary>
			GaPopulation* _population;

		public:

			/// <summary>Initializes update object and bound it to specified population.</summary>
			/// <param name="population">pointer to population whose tag buffer is updated.</param>
			GaPopulationTagManagerUpdate(GaPopulation* population) : _population(population) { }

			/// <summary>Initializes unbound update object.</summary>
			GaPopulationTagManagerUpdate() : _population(NULL) { }

			/// <summary><c>AddTag</c> method adds tag to the population's buffer.
			///
			/// This method is thread-safe.</summary>
			/// <param name="index">index of the tag that should be updated.</param>
			/// <param name="tagLifecycle">lifecycle manager of new tag.</param>
			virtual void GACALL AddTag(int index,
				const Common::Data::GaTagLifecycle& tagLifecycle) const { _population->GetTags().SetTag( index, tagLifecycle.CreateTag() ); }

			/// <summary><c>RemoveTag</c> method removes tag from the population's buffer.
			///
			/// This method is thread-safe.</summary>
			/// <param name="index">index of the tag that should be removed.</param>
			virtual void GACALL RemoveTag(int index) const { _population->GetTags().RemoveTag( index ); }

			/// <summary><c>UpdateTag</c> method updates tag in the population's buffer.
			///
			/// This method is thread-safe.</summary>
			/// <param name="index">index of the tag that should be updated.</param>
			/// <param name="update">update operation that is performed over tag.</param>
			virtual void GACALL UpdateTag(int index,
				const Common::Data::GaTagUpdate& update) const { update( _population->GetTags()[ index ] ); }

			/// <summary><c>SetBufferSize</c> method updates size of population's tag buffer.
			///
			/// This method is thread-safe.</summary>
			/// <param name="newSize">new buffer size.</param>
			virtual void GACALL SetBufferSize(int newSize) const { _population->GetTags().SetSize( newSize ); }

		};

	private:

		/// <summary><c>GaCreateStorage</c> class implements object pool creation policy for chromosome storage objects.</summary>
		class GaCreateStorage
		{

		private:

			/// <summary>Population for which chromosomes storage objects will be created.</summary>
			GaPopulation* _population;

		public:

			/// <summary><c>operator()</c> creates chromosome storage object.
			///
			/// This operator is thread-safe.</summary>
			/// <returns>Returns pointer to newly created object.</returns>
			inline GaChromosomeStorage* GACALL operator() () const
			{ 
				GaChromosomeStorage* storage = new GaChromosomeStorage( _population->CreateFitnessObject( GaChromosomeStorage::GAFT_RAW ),
					_population->CreateFitnessObject( GaChromosomeStorage::GAFT_SCALED ) );

				_population->GetChromosomeTagManager().InitBuffer( storage->GetTags() );

				return storage;
			}

			/// <summary>Sets population for which the chromosomes storage objects are created.<summary>
			/// <param name="population">pointer to the population.</param>
			inline void SetPopulation(GaPopulation* population) { _population = population; }

		};

		/// <summary><c>GaStorageDelete</c> type defines object pool deletion policy for chromosome storage objects.</summary>
		typedef Common::Memory::GaPoolObjectDelete<GaChromosomeStorage> GaDeleteStorage;

		/// <summary><c>GaInitStorage</c> class implements object pool initialization policy for chromosome storage objects.</summary>
		class GaInitStorage
		{

		private:

			/// <summary>Population for which chromosomes storage objects were created.</summary>
			GaPopulation* _population;

		public:

			/// <summary><c>operator()</c> initializes object that stores metadata about chromosome stored in the object.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="object">pointer to chromosome storage object which should be initialized.</param>
			inline void GACALL operator() (GaChromosomeStorage* object) const
			{
				object->Clear();
				_population->GetChromosomeTagManager().PrepareBuffer( object->GetTags() );
			}

			/// <summary>Sets population for which the chromosomes storage objects are created.<summary>
			/// <param name="population">pointer to the population.</param>
			inline void SetPopulation(GaPopulation* population) { _population = population; }

		};

		/// <summary><c>GaCleanStorage</c> class implements object pool cleanup policy  for chromosome storage objects.</summary>
		class GaCleanStorage
		{

		private:

			/// <summary>Population for which chromosomes storage objects were created.</summary>
			GaPopulation* _population;

		public:

			/// <summary><c>operator()</c> removes stored chromosome from storage object.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="object">pointer to chromosome storage object object which should be cleaned.</param>
			virtual void GACALL operator ()(GaChromosomeStorage* object) const
			{
				object->SetChromosome( Chromosome::GaChromosomePtr::NullPtr );
				_population->GetChromosomeTagManager().PrepareBuffer( object->GetTags() );
			}

			/// <summary>Sets population for which the chromosomes storage objects are created.<summary>
			/// <param name="population">pointer to the population.</param>
			inline void SetPopulation(GaPopulation* population) { _population = population; }

		};

		/// <summary>Type of object pool that keep chromosome storage objects used by the population.</summary>
		typedef Common::Memory::GaObjectPool<GaChromosomeStorage, GaCreateStorage, GaDeleteStorage, GaInitStorage, GaCleanStorage> GaChromosomeStoragePool;

		/// <summary>Parameters of the population.</summary>
		GaPopulationParams _parameters;

		/// <summary>Chromosome initialization operation.</summary>
		Chromosome::GaInitializatorSetup _initializator;

		/// <summary>Prototype of the object for storing raw fitness values of chromosomes.</summary>
		Common::Memory::GaAutoPtr<Fitness::GaFitness> _fitnessPrototype;

		/// <summary>Fitness operation and its parameters used for evaluation of fitness values of chromosomes.</summary>
		GaPopulationFitnessOperationSetup _fitnessOperation;

		/// <summary>Default fitness comparator and its parameters used by population.</summary>
		Fitness::GaFitnessComparatorSetup _fitnessComparator;

		/// <summary>Prototype of the object for storing scaled fitness values of chromosomes.</summary>
		Common::Memory::GaAutoPtr<Fitness::GaFitness> _scaledFitnessPrototype;

		/// <summary>Pointer to array that is currently used to store chromosomes.</summary>
		GaChromosomeStorage** _chromosomes;

		/// <summary>Original array used for storing chromosomes.</summary>
		Common::Data::GaSingleDimensionArray<GaChromosomeStorage*> _array;

		/// <summary>Helper array used for sorting.</summary>
		Common::Data::GaSingleDimensionArray<GaChromosomeStorage*> _helper;

		/// <summary>Number of chromosomes currently in the population.</summary>
		int _currentSize;

		/// <summary>Sorting algorithm used for sorting chromosomes in the population.</summary>
		Common::Sorting::GaMergeSortAlgorithm<GaChromosomeStorage*> _sorting;

		/// <summary>Object pool for recycling chromosomes storage objects.</summary>
		GaChromosomeStoragePool _chromosomeStoragePool;

		/// <summary>Group that stores newly added chromosomes in population.</summary>
		GaChromosomeGroup _newChromosomes;

		/// <summary>Group that stores chromosomes removed from the population.</summary>
		GaChromosomeGroup _removedChromosomes;

		/// <summary>Tag manager used by population to handle chromosome's tags.</summary>
		Common::Memory::GaAutoPtr<Common::Data::GaTagManager> _chromosomeTagManager;

		/// <summary>Tag manager that handles population's tags.</summary>
		Common::Memory::GaAutoPtr<Common::Data::GaTagManager> _tagManager;

		/// <summary>Buffer that stores population's tags.</summary>
		Common::Data::GaTagBuffer _tags;

		/// <summary>Stores current states population's flags.</summary>
		GaFlagType _flags;

		/// <summary>Manager of population's flags.</summary>
		Common::Data::GaFlagManager<GaChromosomeStorage::GaFlagType::GaDataType> _chromosomeFlagManager;

		/// <summary>Object that stores statistical information about this population.</summary>
		Statistics::GaStatistics _statistics;

		/// <summary>Event manager of the population.</summary>
		Common::Observing::GaEventManager _events;

		/// <summary>Stores registered statistical trackers of the population.</summary>
		STLEXT::hash_map<int, std::pair<bool, GaPopulationStatTracker*> > _registeredTrackers;

	public:

		/// <summary>Constructs population.</summary>
		/// <param name="parameters">pointer to parameter's parameters.</param>
		/// <param name="initializator">reference to object that stores chromosome initializator, its parameters and configuration .</param>
		/// <param name="fitnessOperation">reference to object that stores fitenss operation, its parameters and configuration.</param>
		/// <param name="fitnessComparator">reference to object that stores fitenss comparator, its parameters and configuration.</param>
		GAL_API
		GaPopulation(const GaPopulationParams& parameters,
			const Chromosome::GaInitializatorSetup& initializator,
			const GaPopulationFitnessOperationSetup& fitnessOperation,
			const Fitness::GaFitnessComparatorSetup& fitnessComparator);

		/// <summary><c>Initialize</c> method prepares population to be used by genetic algorithm. 
		///
		/// This method is not thread-safe.</summary>
		/// <exception cref="GaInvalidOperationException" />Thrown if population is already initialized.</exception>
		GAL_API
		void GACALL Initialize();

		/// <summary><c>Clear</c> method removes all chromosomes from the population and clears statistics. The method also marks population as uninitialized.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="clearStats">if this parameter is set to <c>true</c> this method also clears population's statistics.</param>
		GAL_API
		void GACALL Clear(bool clearStats);

		/// <summary><c>Insert</c> method inserts chromosome into population.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="chromosome">pointer to chromosomes storage object that should be inserted.</param>
		inline void GACALL Insert(GaChromosomeStorage* chromosome)
		{
			_chromosomes[ _currentSize++ ] = chromosome;
			_newChromosomes.Add( chromosome );
		}

		/// <summary><c>Insert</c> method inserts multiple chromosomes into population.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="chromosomes">array of chromosome storage objects that should be inserted.</param>
		/// <param name="count">number of storage objects that should be inserted.</param>
		inline void GACALL Insert(GaChromosomeStorage** chromosomes,
			int count)
		{
			for( count = count - 1; count >= 0; count-- )
				Insert( *( chromosomes++ ) );
		}

		/// <summary><c>Remove</c> method removes chromosome at specified index from the population.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="chromosomeIndex">index of chromosome that should be inserted.</param>
		GAL_API
		void GACALL Remove(int chromosomeIndex);

		/// <summary><c>RemoveSorted</c> method removes multiple chromosomes from the population.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="chromosomeIndices">array that contains indices of chromosomes that should be removed. This array should be sorted in increasing order.</param>
		/// <param name="count">number of chromosomes that should be removed.</param>
		GAL_API
		void GACALL RemoveSorted(int* chromosomeIndices,
			int count);

		/// <summary><c>RemoveUnsorted</c> method removes multiple chromosomes from the population.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="chromosomeIndices">array that contains indices of chromosomes that should be removed. This array does not have to be sorted.</param>
		/// <param name="count">number of chromosomes that should be removed.</param>
		GAL_API
		void GACALL RemoveUnsorted(int* chromosomeIndices,
			int count);

		/// <summary><c>Remove</c> removes marked chromosomes from the population.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		void GACALL Remove();

		/// <summary><c>Trim</c> method removes chromosomes at the bottom of the population.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="trimSize">number of chromosomes that should be removed from the population.</param>
		void GACALL Trim(int trimSize)
		{
			for( ; trimSize > 0; trimSize-- )
				// Update book-keeping
				Remove( _chromosomes[ --_currentSize ] );
		}

		/// <summary><c>Trim</c> method sorts chromosomes according to their fitness and removes all chromosomes
		/// that are at the bottom in reserved space of population for temporal crowding.
		///
		/// This method is not thread-safe.</summary>
		inline void GACALL Trim() { Trim( _currentSize - _parameters.GetPermanentSpaceSize() ); }

		/// <summary><c>Replace</c> method removes chromosome with specified index and inserts new one at its place.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="oldChromosomeIndex">index of chromosome in the population which should be replaced.</param>
		/// <param name="newChromosome">pointer to chromosome storage object that should replace old chromosome.</param>
		GAL_API
		void GACALL Replace(int oldChromosomeIndex,
			GaChromosomeStorage* newChromosome);

		/// <summary><c>Replace</c> method replaces chromosomes with specified indices with new chromosomes.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="chromosomeIndices">array that contains indices of chromosomes that should be removes.</param>
		/// <param name="chromosomes">array of chromosome storage objects that should replace old chromosomes.</param>
		/// <param name="count">number of chromosomes that should be replaced.</param>
		inline void GACALL Replace(int* chromosomeIndices,
			GaChromosomeStorage** chromosomes,
			int count)
		{
			for( count = count - 1; count >= 0; count-- )
				Replace( *( chromosomeIndices++ ), *( chromosomes++ ) );
		}

		/// <summary><c>CalculateFitness</c> method calculate fitness value of chromosome storage object and store that value in specified fitness object.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="chromosome">smart pointer to chromosome whose fitness value should be evaluated.</param>
		/// <param name="fitness">reference to object that will store evaluated fitness value.</param>
		/// <returns>Method returns <c>true</c>if fitness value of chromosome is evaluated. If fitness operation used by population
		/// does not allow individual-based fitness evaluation of chromosomes this method returns <c>false</c></returns>
		inline bool GACALL CalculateFitness(Chromosome::GaChromosomePtr chromosome,
			Fitness::GaFitness& fitness)
		{
			bool calculate = _fitnessOperation.GetOperation().AllowsIndividualEvaluation();
			if( calculate )
				_fitnessOperation.GetOperation()( *chromosome, fitness, _fitnessOperation.GetParameters() );

			return calculate;
		}

		/// <summary><c>CalculateFitness</c> method calculate fitness value of chromosome storage object and store that value.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="storage">reference to storage boject that contains chromosome whose value should be evaluated.</param>
		/// <returns>Method returns <c>true</c>if fitness value of chromosome is evaluated. If fitness operation used by population
		/// does not allow individual-based fitness evaluation of chromosomes this method returns <c>false</c></returns>
		inline bool GACALL CalculateFitness(GaChromosomeStorage& storage) { return CalculateFitness( storage.GetChromosome(), storage.GetFitness( GaChromosomeStorage::GAFT_RAW ) ); }

		/// <summary><c>NextGeneration</c> evaluates population's statistics and prepares population for the next generation.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="branch">pointer to branch that execute operation.</param>
		GAL_API
		void GACALL NextGeneration(Common::Workflows::GaBranch* branch);

		/// <summary><c>Sort</c> method sorts population using provided sorting criteria.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="CRITERIA">type of sorting criteria used for sorting.</param>
		/// <param name="criteria">reference to criteria used for sorting population.</param>
		template<typename CRITERIA>
		inline void GACALL Sort(const CRITERIA& criteria) { _chromosomes = _sorting.Sort( criteria ); }

		/// <summary><c>Sort</c> method sorts chromosomes in specified range using provided sorting criteria.
		/// Method can be executed concurently from different threads only when sorting non-overlapping portions of the array.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="CRITERIA">type of sorting criteria used for sorting.</param>
		/// <param name="begin">index of the first chromosome in the population that should be sorted.</param>
		/// <param name="end">index of the last chromosome in the population that should be sorted.</param>
		/// <param name="criteria">reference to criteria used for sorting population.</param>
		template<typename CRITERIA>
		inline void GACALL Sort(int begin,
			int end,
			const CRITERIA& criteria) { _chromosomes = _sorting.Sort( begin, end, criteria ); }

		/// <summary><c>Shuffle</c> method arranges chromosomes of the group in random order.
		///
		/// This method is not thread-safe.</summary>
		inline void GACALL Shuffle() { Common::Random::GaShuffle( _chromosomes, _currentSize ); }

		/// <summary><c>MakeFromPrototype</c> method produces new chromosome using provided chromosome initializator.
		///
		/// This method is not thread-safe.</summary>
		/// <returns></returns>
		inline GaChromosomeStorage* GACALL MakeFromPrototype()
		{
			GaChromosomeStorage* storage = AcquireStorageObject();
			storage->SetChromosome( _initializator.GetOperation()( _initializator.GetParameters(), _initializator.GetConfiguration().GetConfigBlock() ) );
			CalculateFitness( *storage );

			return storage;
		}

		/// <summary><c>CreateFitnessObject</c> method makes new fitness object for defined fitness operation with defined parameters.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="type">type of fitness for which the object is created.</param>
		/// <returns>Method returns pointer to newly created fitness object.</returns>
		inline Fitness::GaFitness* GACALL CreateFitnessObject(GaChromosomeStorage::GaFitnessType type) const
			{ return type == GaChromosomeStorage::GAFT_RAW ? _fitnessPrototype->Clone() : ( _scaledFitnessPrototype.IsNull() ? NULL : _scaledFitnessPrototype->Clone() ); }

		/// <summary><c>CompareFitness</c> method compares two fitness values using population's default fitness comparator.
		/// 
		/// This method is not thread-safe.</summary>
		/// <param name="fitness1">reference to the first fitness value used for comparison.</param>
		/// <param name="fitness2">reference to the second fitness value used for comparison.</param>
		/// <returns>Method returns:
		/// <br/>a. -1 if the first fitness value if better then the second.
		/// <br/>b.  0 if the fitness values are equal.
		/// <br/>c.  1 if the first fitness value is worse then the second.</returns>
		inline int GACALL CompareFitness(const Fitness::GaFitness& fitness1,
			const Fitness::GaFitness& fitness2) const { return _fitnessComparator.GetOperation()( fitness1, fitness2, _fitnessComparator.GetParameters() ); }

		/// <summary><c>CompareFitness</c> method compare fitness values of two chromosomes using population's default fitness comparator.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="chromosome1">reference to the first chromosome whose fitness value is used for comparison.</param>
		/// <param name="chromosome2">reference to the second chromosome whose fitness value is used for comparison.</param>
		/// <returns>Method returns:
		/// <br/>a. -1 if the first chromosome has better fitness value then the second.
		/// <br/>b.  0 if the fitness values are equal.
		/// <br/>c.  1 if the first chromosome has worse fitness value then the second.</returns>
		inline int GACALL CompareFitness(const GaChromosomeStorage& chromosome1,
			const GaChromosomeStorage& chromosome2) const { return CompareFitness( chromosome1.GetFitness( GaChromosomeStorage::GAFT_RAW ), chromosome2.GetFitness( GaChromosomeStorage::GAFT_RAW ) ); }
		
		/// <summary><c>RegisterTracker</c> method adds tracket to list of population's statistical trackers and bounds it to population's statistics object.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="trackerID">ID of the new evaluator.</param>
		/// <param name="tracker">pointer to statistics evaluator that should be bound.</param>
		/// <exception cref="GaArgumentException" />Thrown if there is already an evaluator registered with specified ID.</exception>
		GAL_API
		void GACALL RegisterTracker(int trackerID,
			GaPopulationStatTracker* tracker);

		/// <summary><c>UnregisterTracker</c> method removes tracker from the list of population's statistical trackers and unbinds it from population's statistics object.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="trackerID">ID of tracker  that should be removed and unbound.</param>
		/// <exception cref="GaArgumentException" />Thrown if there is no registered tracker with specified ID.</exception>
		GAL_API
		void GACALL UnregisterTracker(int trackerID);

		/// <summary><c>UpdateTracker</c> method updates tracked statistical values of the population.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="trackerID">ID of tracker  that should be removed and unbound.</param>
		/// <param name="branch">pointer to branch that execut evaluation.</param>
		/// <exception cref="GaArgumentException" />Thrown if there is no registered tracker with specified ID.</exception>
		GAL_API
		void GACALL UpdateTracker(int trackerID,
			Common::Workflows::GaBranch* branch);

		/// <summary><c>IsTrackerRegistered</c> method checks is there a statistical value tracker with specified ID.
		///
		/// This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if there is registered tracker with specified ID.</returns>
		inline bool GACALL IsTrackerRegistered(int trackerID) { return _registeredTrackers.find( trackerID ) != _registeredTrackers.end(); }

		/// <summary><c>SetPopulationParams</c> method sets new parameters for the population. 
		///
		/// This method is not thread-safe.</summary>
		/// <param name="parameters">reference to new parameters of the population.</param>
		GAL_API
		void GACALL SetPopulationParams(const GaPopulationParams& parameters);

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to parameters of the population.</returns>
		inline const GaPopulationParams& GACALL GetPopulationParams() const { return _parameters; }

		/// <summary><c>SetInitializator</c> method sets chromosome initializator and its parameters.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="operation">new chromosome initializator.</param>
		inline void GACALL SetInitializator(const Chromosome::GaInitializatorSetup& operation) { _initializator = operation; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to object that chromosome initializator and its parameters.</returns>
		inline Chromosome::GaInitializatorSetup& GACALL GetInitializator() { return _initializator; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to object that chromosome initializator and its parameters.</returns>
		inline const Chromosome::GaInitializatorSetup& GACALL GetInitializator() const { return _initializator; }

		/// <summary><c>SetFitnessOperation</c> method sets new fitness operation that will be used for evaluation chromosomes in the population.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="operation">fitness operation setup.</param>
		/// <exception cref="GaArgumentException" />Thrown if setup does not have operation specified.</exception>
		GAL_API
		void GACALL SetFitnessOperation(const GaPopulationFitnessOperationSetup& operation);

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to fitness parameters used by fitness objects in this population.</returns>
		inline Fitness::GaFitnessParams& GACALL GetFitnessParams() { return *_fitnessOperation.GetConfiguration().GetFitnessParams(); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to fitness parameters used by fitness objects in this population.</returns>
		inline const Fitness::GaFitnessParams& GACALL GetFitnessParams() const { return *_fitnessOperation.GetConfiguration().GetFitnessParams(); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if the fitness parameters is specified.</returns>
		inline bool GACALL HasFitnessParams() const { return _fitnessOperation.HasConfiguration() && _fitnessOperation.GetConfiguration().HasFitnessParams(); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to fitness operation used for evaluation chromosomes in the population.</returns>
		inline const GaPopulationFitnessOperationSetup& GACALL GetFitnessOperation() const { return _fitnessOperation; }

		/// <summary><c>SetScaledFitnessPrototype</c> method creates new prototype of fitness object that will be used by scaling operation to store sacled fitness values
		/// of chromosomes in the population.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="prototype">pointer to new prototype.</param>
		GAL_API
		void GACALL SetScaledFitnessPrototype(const Fitness::GaFitness* prototype);

		/// <summary><c>SetFitnessComparator</c> method sets population's default fitness comparator and its parameters.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="comparator">fitness comparator setup.</param>
		/// <exception cref="GaArgumentException" />Thrown if setup does not have operation specified.</exception>
		GAL_API
		void GACALL SetFitnessComparator(const Fitness::GaFitnessComparatorSetup& comparator);

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer setup that stores population's default fitness comparator and its parameters.</returns>
		inline const Fitness::GaFitnessComparatorSetup& GACALL GetFitnessComparator() const { return _fitnessComparator; }

		/// <summary><c>GetAt</c> method returns pointer to chromosomes storage object at specified position in the population.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="index">index of queried object.</param>
		/// <returns>Method returns reference to chromosome storage object located at specified index in population.</returns>
		inline GaChromosomeStorage& GACALL GetAt(int index) { return *_chromosomes[ index ]; }

		/// <summary><c>GetAt</c> method returns pointer to chromosomes storage object at specified position in the population.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="index">index of queried object.</param>
		/// <returns>Method returns reference to chromosome storage object located at specified index in population.</returns>
		inline const GaChromosomeStorage& GACALL GetAt(int index) const { return *_chromosomes[ index ]; }

		/// <summary><c>operator []</c> returns pointer to chromosomes storage object at specified position in the population.
		///
		/// This operator is not thread-safe.</summary>
		/// <param name="index">index of queried object.</param>
		/// <returns>Operator returns pointer to chromosome storage object located at specified index in population.</returns>
		inline GaChromosomeStorage& GACALL operator [](int index) { return *_chromosomes[ index ]; }

		/// <summary><c>operator []</c> returns pointer to chromosomes storage object at specified position in the population.
		///
		/// This operator is not thread-safe.</summary>
		/// <param name="index">index of queried object.</param>
		/// <returns>Operator returns pointer to chromosome storage object located at specified index in population.</returns>
		inline const GaChromosomeStorage& GACALL operator [](int index) const { return *_chromosomes[ index ]; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to array that is used by the population for storing chromosomes.</returns>
		inline GaChromosomeStorage** GACALL GetChromosomes() { return _chromosomes; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to array that is used by the population for storing chromosomes.</returns>
		inline const GaChromosomeStorage* const* GACALL GetChromosomes() const { return _chromosomes; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns number of chromosomes currently stored in the population.</returns>
		inline int GACALL GetCount() const { return _currentSize; }

		/// <summary><c>GetFreeSpaceSize</c> method calculates size of unused space of population (space that does not sotre any chromosomes).
		///
		/// This operator is not thread-safe.<summary>
		/// <param name="inludeCrowdingSpace">when this parameter is <c>true</c> it instructs method to include space reserved for temporal crowding into calculation.</param>
		/// <returns>Method returns number of chromosomes that population can store in unused space.</returns>
		inline int GACALL GetFreeSpaceSize(bool inludeCrowdingSpace) const
			{ return inludeCrowdingSpace ? _parameters.GetPopulationSize() : _parameters.GetPermanentSpaceSize() - _currentSize; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if there are chromosomes stored in space reserved for temporal crowding of the population.</returns>
		inline bool GACALL IsCrowded() const { return _currentSize > _parameters.GetPopulationSize() - _parameters.GetCrowdingSize(); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to group that stores newly added chromosomes to the population.</returns>
		inline GaChromosomeGroup& GACALL GetNewChromosomes() { return _newChromosomes; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to group that stores newly added chromosomes to the population.</returns>
		inline const GaChromosomeGroup& GACALL GetNewChromosomes() const { return _newChromosomes; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to group that stores chromosomes removed from the population.</returns>
		inline GaChromosomeGroup& GACALL GetRemovedChromosomes() { return _removedChromosomes; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to group that stores chromosomes removed from the population.</returns>
		inline const GaChromosomeGroup& GACALL GetRemovedChromosomes() const { return _removedChromosomes; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to tag manager used by population to handle chromosome's tags.</returns>
		inline Common::Data::GaTagManager& GACALL GetChromosomeTagManager() { return *_chromosomeTagManager; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to tag manager used by population to handle chromosome's tags.</returns>
		inline const Common::Data::GaTagManager& GACALL GetChromosomeTagManager() const { return *_chromosomeTagManager; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to tag manager that handles population's tags.</returns>
		inline Common::Data::GaTagManager& GACALL GetTagManager() { return *_tagManager; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to tag manager that handles population's tags.</returns>
		inline const Common::Data::GaTagManager& GACALL GetTagManager() const { return *_tagManager; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to buffer that stores population's tags.</returns>
		inline Common::Data::GaTagBuffer& GACALL GetTags() { return _tags; }

		/// <summary><c>SetTagByID</c> method stores data in tag with specified ID.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="DATA_TYPE">type of data that tag stores.</param>
		/// <param name="id">ID of the tag.</param>
		/// <param name="data">data that should be stored in the tag.</param>
		template<typename DATA_TYPE>
		inline void GACALL SetTagByID(int id,
			const DATA_TYPE& data) { ( (Common::Data::GaTypedTag<DATA_TYPE>&)_tags[ _tagManager->GetTagIndex( id ) ] ).SetData( data ); }

		/// <summary><c>GetTagByID</c> method returns data that is stored by tag with sepcified ID.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="DATA_TYPE">type of data that tag stores.</param>
		/// <param name="id">ID of queried tag.</param>
		/// <returns>Method returns reference to data stored in the tag.</returns>
		template<typename DATA_TYPE>
		inline DATA_TYPE& GACALL GetTagByID(int id) { return ( (Common::Data::GaTypedTag<DATA_TYPE>&)_tags[ _tagManager->GetTagIndex( id ) ] ).GetData(); }

		/// <summary><c>GetTagByID</c> method returns data that is stored by tag with sepcified ID.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="DATA_TYPE">type of data that tag stores.</param>
		/// <param name="id">ID of queried tag.</param>
		/// <returns>Method returns reference to data stored in the tag.</returns>
		template<typename DATA_TYPE>
		inline const DATA_TYPE& GACALL GetTagByID(int id) const { return ( (const Common::Data::GaTypedTag<DATA_TYPE>&)_tags[ _tagManager->GetTagIndex( id ) ] ).GetData(); }

		/// <summary><c>SetTagByIndex</c> method stores data in tag at specified position in tag buffer.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="DATA_TYPE">type of data that tag stores.</param>
		/// <param name="index">index of tag in tag buffer.</param>
		/// <param name="data">data that should be stored in the tag.</param>
		template<typename DATA_TYPE>
		inline void GACALL SetTagByIndex(int index,
			const DATA_TYPE& data) { ( (Common::Data::GaTypedTag<DATA_TYPE>&)_tags[ index ] ).SetData( data ); }

		/// <summary><c>GetTagByIndex</c> method returns data that is stored in tag at specified position in tag buffer.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="DATA_TYPE">type of data that tag stores.</param>
		/// <param name="index">index of tag in tag buffer.</param>
		/// <returns>Method returns reference to data stored in the tag.</returns>
		template<typename DATA_TYPE>
		inline DATA_TYPE& GACALL GetTagByIndex(int index) { return ( (Common::Data::GaTypedTag<DATA_TYPE>&)_tags[ index ] ).GetData(); }

		/// <summary><c>GetTagByIndex</c> method returns data that is stored in tag at specified position in tag buffer.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="DATA_TYPE">type of data that tag stores.</param>
		/// <param name="index">index of tag in tag buffer.</param>
		/// <returns>Method returns reference to data stored in the tag.</returns>
		template<typename DATA_TYPE>
		inline const DATA_TYPE& GACALL GetTagByIndex(int index) const { return ( (const Common::Data::GaTypedTag<DATA_TYPE>&)_tags[ index ] ).GetData(); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to buffer that stores population's tags.</returns>
		inline const Common::Data::GaTagBuffer& GACALL GetTags() const { return _tags; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to flag object that stores current stats of population's flags.</returns>
		inline GaFlagType& GACALL GetFlags() { return _flags; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to flag object that stores current stats of population's flags.</returns>
		inline const GaFlagType& GACALL GetFlags() const { return _flags; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to population's flags manager.</returns>
		inline Common::Data::GaFlagManager<GaChromosomeStorage::GaFlagType::GaDataType>& GACALL GetChromosomeFlagManager() { return _chromosomeFlagManager; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to population's flags manager.</returns>
		inline const Common::Data::GaFlagManager<GaChromosomeStorage::GaFlagType::GaDataType>& GACALL GetChromosomeFlagManager() const { return _chromosomeFlagManager; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to object that stores statistical information about this population.</returns>
		inline Statistics::GaStatistics& GACALL GetStatistics() { return _statistics; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to object that stores statistical information about this population.</returns>
		inline const Statistics::GaStatistics& GACALL GetStatistics() const { return _statistics; }

		/// <summary>This method is thread-safe.</summary>
		/// <returns>Method returns reference to event manager of the population.</returns>
		inline Common::Observing::GaEventManager& GACALL GetEventManager() { return _events; }

		/// <summary>This method is thread-safe.</summary>
		/// <returns>Method returns reference to event manager of the population.</returns>
		inline const Common::Observing::GaEventManager& GACALL GetEventManager() const { return _events; }

		/// <summary><c>AcquireStorageObject</c> method acquires chromosomes storage object from population's pool.
		///
		/// This method is not thread-safe.</summary>
		/// <returns>Method returns pointert to acquired storage object.</returns>
		inline GaChromosomeStorage* GACALL AcquireStorageObject() { return _chromosomeStoragePool.AcquireObject(); }

		/// <summary><c>ReleaseStorageObject</c> method recycles specified chromosome storage object and returns it to population's pool.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="storage">pointer to storage boject that should be returned to the pool.</param>
		inline void GACALL ReleaseStorageObject(GaChromosomeStorage* storage) { _chromosomeStoragePool.ReleaseObject( storage ); }

		/// <summary><c>InvalidateStoragePool</c> method removes all storage object from the object pool that recycles them.
		///
		/// This method is not thread-safe.</summary>
		inline void GACALL InvalidateStoragePool() { _chromosomeStoragePool.Invalidate(); }

		/// <summary><c>UpdateStorageObjects</c> method updates storage objects of the population (both used and currently unused objects).
		///
		/// This method is not thread-safe.</summary>
		/// <param name="update">reference to object that performs update of storage objects individually.</param>
		GAL_API
		void GACALL UpdateStorageObjects(Common::Memory::GaPoolObjectUpdate<GaChromosomeStorage>& update);

	private:

		/// <summary>This method updates book-keeping structures that track chromosomes when specified chromosome is removed from the population.</summary>
		/// <param name="chromosome">pointer to chromosomes which was removed.</param>
		inline void GACALL Remove(GaChromosomeStorage* chromosome)
		{
			// remove from group of new chromosomes
			if( chromosome->GetFlags().IsFlagSetAny( GaChromosomeStorage::GACF_NEW_CHROMOSOME ) )
				_newChromosomes.Remove( chromosome );

			// move specified chromosome to list of removed chromosomes
			_removedChromosomes.Add( chromosome );
		}

	};

	/// <summary><c>GaFitnessSortingCriteria</c> class represent sorting criteria that use population's fitness comparator for comparison.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// Because this genetic operation is stateless all public method are thread-safe.</summary>
	class GaFitnessSortingCriteria : public GaChromosomeSortingCriteria
	{

	private:

		/// <summary>Population whose fitness comparator is used.</summary>
		GaPopulation* _population;

	public:

		/// <summary>This constructors initializes sorting criteria with population whose fitness comparator is used as sorting criteria.</summary>
		/// <param name="population">pointer to population whose fitness comparator is used.</param>
		GaFitnessSortingCriteria(GaPopulation* population) : _population(population) { }

		/// <summary>This operator compares fitness values of two chromosomes.
		///
		/// This method is thread-safe.</summary>
		/// <param name="object1">reference to the first chromosome storage object.</param>
		/// <param name="object2">reference to the second chromosome storage object.</param>
		/// <returns>Method should returns:
		/// <br/>a. -1 if the first chromosome has better fitness value then the second.
		/// <br/>b.  0 if the fitness values are equal.
		/// <br/>c.  1 if the first chromosome has worse fitness value then the second.</returns>
		inline int GACALL operator ()(GaConstType& object1,
			GaConstType& object2) const { return _population->CompareFitness( *object1, *object2 ); }

		/// <summary>This method is thread-safe.</summary>
		/// <returns>Method returns pointer to population whose fitness comparator is used as sorting criteria.</returns>
		inline GaPopulation* GetPopulation() { return _population; }

		/// <summary>This method is thread-safe.</summary>
		/// <returns>Method returns pointer to population whose fitness comparator is used as sorting criteria..</returns>
		inline const GaPopulation* GetPopulation() const { return _population; }

	};

	/// <summary><c>GaFitnessComparatorSortingCriteria</c> class represent criteria that uses chromosomes fitness and fitness comparator for sorting.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// Because this genetic operation is stateless all public method are thread-safe.</summary>
	class GaFitnessComparatorSortingCriteria : public GaChromosomeSortingCriteria
	{

	private:

		/// <summary>Setup that stores fitness comparator and its paramter that is used as sorting criteria.</summary>
		Fitness::GaFitnessComparatorSetup _comparator;

		/// <summary>Type of fitness values used for sorting.</summary>
		GaChromosomeStorage::GaFitnessType _fitnessType;

	public:

		/// <summary>This constructors initializes sorting criteria with fitness comparator used as sorting criteria.</summary>
		/// <param name="comparator">pointer to setup that stores fitness comparator and its paramters that will be used as sorting criteria.</param>
		/// <param name="fitnessType">type of fitness values that will be used for sorting.</param>
		GaFitnessComparatorSortingCriteria(const Fitness::GaFitnessComparatorSetup& comparator,
			GaChromosomeStorage::GaFitnessType fitnessType) : _comparator(comparator),
			_fitnessType(fitnessType) { }

		/// <summary>Initializes sort criteria without specifing fitness comparator.</summary>
		GaFitnessComparatorSortingCriteria() : _fitnessType(GaChromosomeStorage::GAFT_RAW) { }

		/// <summary>More details are given in specification of <see cref="GaChromosomeSortingCriteria::operator ()" /> method.
		///
		/// This method is thread-safe.</summary>
		/// <param name="object1">reference to the first chromosome storage object.</param>
		/// <param name="object2">reference to the second chromosome storage object.</param>
		/// <returns>Method should returns:
		/// <br/>a. -1 if the first chromosome has better fitness value then the second.
		/// <br/>b.  0 if the fitness values are equal.
		/// <br/>c.  1 if the first chromosome has worse fitness value then the second.</returns>
		virtual int GACALL operator ()(GaConstType& object1,
			GaConstType& object2) const { return _comparator.GetOperation()( object1->GetFitness( _fitnessType ), object2->GetFitness( _fitnessType ), _comparator.GetParameters() ); }

		/// <summary>This method is thread-safe.</summary>
		/// <returns>Method returns pointer to setup that stores fitness comparator and its paramter that is used as sorting criteria.</returns>
		inline Fitness::GaFitnessComparatorSetup& GACALL GetComparator() { return _comparator; }

		/// <summary>This method is thread-safe.</summary>
		/// <returns>Method returns pointer to setup that stores fitness comparator and its paramter that is used as sorting criteria.</returns>
		inline const Fitness::GaFitnessComparatorSetup& GACALL GetComparator() const { return _comparator; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns type of fitness values used for sorting.</returns>
		inline GaChromosomeStorage::GaFitnessType GACALL GetFitnessType() const { return _fitnessType; }

	};

	/// <summary><c>GaCrossoverStorageBuffer</c> class represent crossover buffer that uses chromosome groups to store parent chromosomes and offspring chromosomes
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
	/// but some methods are thread-safe.</summary>
	class GaCrossoverStorageBuffer : public Chromosome::GaCrossoverBuffer
	{

	public:

		/// <summary><c>GaOperationCounter</c> defines counters of operations that are being tracked by the crossover buffer.</summary>
		enum GaOperationCounter
		{

			/// <summary>Counter of performed crossover operations.</summary>
			GAOC_CROSSOVER_COUNT,

			/// <summary>Counter of performed mutation operations.</summary>
			GAOC_MUTATION_COUNT,

			/// <summary>Counter of accepted mutations.</summary>
			GAOC_ACCEPTED_MUTATION_COUNT,

			/// <summary>Number of counters.</summary>
			GAOC_COUNTERS

		};

	private:

		/// <summary>Pointer to population in which the mating takes place.</summary>
		GaPopulation* _population;

		/// <summary>Group that stores parent chromosomes.</summary>
		GaChromosomeGroup _parents;

		/// <summary>Group that stores offspring chromosomes.</summary>
		GaChromosomeGroup _offspring;

		/// <summary>Temporary fitness value used for fitness calculations.</summary>
		Common::Memory::GaAutoPtr<Fitness::GaFitness> _tempFitness;

		/// <summary>Counters of performed operations.</summary>
		int _operationCounters[ GAOC_COUNTERS ];

	public:

		/// <summary>This constructor initializes crossover buffer with population in which the chromosomes are produced and its size.</summary>
		/// <param name="population">pointer to population in which the mating takes place.</param>
		/// <param name="parentCount">number of parent chromosomes that buffer can store.</param>
		/// <param name="offspringCount">number of offspring chromosomes that buffer can store.</param>
		/// <exception cref="GaNullArgumentException" />Thrown if <c>population</c> is set to <c>NULL</c>.</exception>
		GaCrossoverStorageBuffer(GaPopulation* population,
			int parentCount,
			int offspringCount) : _population(population),
			_parents(false, parentCount),
			_offspring(false, offspringCount, 0, NULL, true)
		{
			SetPopulation( population );
			Clear();
		}

		/// <summary>This constructor initializes new buffer by copying settings of existing buffer.</summary>
		/// <param name="rhs">buffer that should be copied.</param>
		GaCrossoverStorageBuffer(const GaCrossoverStorageBuffer& rhs) : _parents(false, rhs._parents.GetSize()),
			_offspring(false, rhs._offspring.GetSize(), 0, NULL, true)
		{
			SetPopulation( rhs._population );
			Clear();
		}

		/// <summary>This constructor initializes crossover buffer that is not bound to a population.</summary>
		GaCrossoverStorageBuffer() : _population(NULL) { Clear(); }

		/// <summary>More details are given in specification of <see cref="GaCrossoverBuffer::CreateOffspringFromPrototype" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual Chromosome::GaChromosomePtr GACALL CreateOffspringFromPrototype() const
		{
			Chromosome::GaInitializatorSetup& initializator = _population->GetInitializator();
			return initializator.GetOperation()( initializator.GetParameters(), initializator.GetConfiguration().GetConfigBlock() );
		}

		/// <summary>More details are given in specification of <see cref="GaCrossoverBuffer::StoreOffspringChromosome" /> method.
		///
		/// This method is thread-safe.</summary>
		GAL_API
		virtual int GACALL StoreOffspringChromosome(Chromosome::GaChromosomePtr chromosome,
			int parent);

		/// <summary>More details are given in specification of <see cref="GaCrossoverBuffer::ReplaceOffspringChromosome" /> method.
		///
		/// This method is thread-safe.</summary>
		GAL_API
		virtual void GACALL ReplaceOffspringChromosome(int index,
			Chromosome::GaChromosomePtr chromosome);

		/// <summary>More details are given in specification of <see cref="GaCrossoverBuffer::ReplaceIfBetter" /> method.
		///
		/// This method is thread-safe.</summary>
		GAL_API
		virtual bool GACALL ReplaceIfBetter(int index,
			Chromosome::GaChromosomePtr newChromosome);

		/// <summary><c>Clear</c> method removes all parent and offspring chromosomes from the buffer and prepares buffer for next execution of the operation.
		///
		/// This method is thread-safe.</summary>
		GAL_API
		void GACALL Clear();

		/// <summary>More details are given in specification of <see cref="GaCrossoverBuffer::GetOffspringChromosome" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual Chromosome::GaChromosomePtr GACALL GetOffspringChromosome(int index) { return _offspring[ index ].GetChromosome(); }

		/// <summary>More details are given in specification of <see cref="GaCrossoverBuffer::GetParentChromosome" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual Chromosome::GaChromosomePtr GACALL GetParentChromosome(int index) { return _parents[ index ].GetChromosome(); }

		/// <summary><c>SetPopulation</c> method sets population in which the mating will take place.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="population">pointer to population.</param>
		GAL_API
		void GACALL SetPopulation(GaPopulation* population);

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to population in which the mating takes place.</returns>
		inline GaPopulation* GACALL GetPopulation() { return _population; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to population in which the mating takes place.</returns>
		inline const GaPopulation* GACALL GetPopulation() const { return _population; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to chromosome group that contains parents.</returns>
		inline GaChromosomeGroup& GACALL GetParents() { return _parents; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to chromosome group that contains parents.</returns>
		inline const GaChromosomeGroup& GACALL GetParents() const { return _parents; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to chromosome group that contains offspring.</returns>
		inline GaChromosomeGroup& GACALL GetOffspring() { return _offspring; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to chromosome group that contains offspring.</returns>
		inline const GaChromosomeGroup& GACALL GetOffspring() const { return _offspring; }

		/// <summary><c>GetOperationCounter</c> method queries state of operation counter.
		///
		/// This method is thread-safe.</summary>
		/// <param name="counter">which counter should be queried.</param>
		/// <returns>Method returns state of desired counter.</returns>
		inline int GACALL GetOperationCounter(GaOperationCounter counter) const { return _operationCounters[ counter ]; }

	};

	/// <summary>Tag datatype for storing per-branch crossover buffers.</summary>
	typedef Common::Data::GaSingleDimensionArray<GaCrossoverStorageBuffer> GaPartialCrossoverBuffer;

	/// <summary><c>GaPopulationEventData</c> class base class for storing data of events raised by population of chromosomes.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaPopulationEventData : public Common::Observing::GaEventData
	{

	private:

		/// <summary>Population in which the event is raised.</summary>
		GaPopulation* _population;

	public:

		/// <summary>Initializes event data with pointer to population that raised event.</summary>
		/// <param name="population">population in which the event is raised.</param>
		GaPopulationEventData(GaPopulation* population) : _population(population) { }

		/// <summary>Initializes empty event data object.</summary>
		GaPopulationEventData() : _population(NULL) { }

		/// <summary><c>SetPopulation</c> method sets population which raised event.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="population">population in which the event is raised.</param>
		inline void GACALL SetPopulation(GaPopulation* population) { _population = population; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns population in which the event is raised.</returns>
		inline GaPopulation& GACALL GetPopulation() { return *_population; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns population in which the event is raised.</returns>
		inline const GaPopulation& GACALL GetPopulation() const { return *_population; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if event has valid reference to population object.</returns>
		inline bool GACALL HasPopulation() const { return _population != NULL; }

	};

	/// <summary><c>GaPopulationTagGetter</c> class provides access to typed data of population's tags.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	/// <param name="DATA_TYPE">type of data stored in tag.</param>
	template<typename DATA_TYPE>
	class GaPopulationTagGetter : public Common::Data::GaTagGetter<DATA_TYPE, GaPopulation>
	{

	public:

		/// <summary>Initializes tag getter for specific tag.</summary>
		/// <param name="tagID">tag ID.</param>
		/// <param name="tagManager">pointer to manager that handles tags and tag buffers</param>
		GaPopulationTagGetter(int tagID,
			const Common::Data::GaTagManager& tagManager) : GaTagGetter(tagID, tagManager) { }

		/// <summary>Default constructor.</summary>
		GaPopulationTagGetter() { }

	};

} // Population

#endif // __GA_POPULATION_H__

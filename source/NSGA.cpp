
/*! \file NSGA.cpp
	\brief This file conatins implementation of classes that implement Nondominated Sorting Genetic Algorithm I and II (NSGA-I and NSGA-II).
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#include <limits>
#include "FitnessComparators.h"
#include "PopulationStatistics.h"
#include "Multiobjective.h"
#include "NSGA.h"

namespace Multiobjective
{
	namespace NSGA
	{

		/// <summary><c>GaNSGAStoreOp</c> stores value of scaled fitness to fitness object dedicated fot that purpose in storage object.</summary>
		struct GaNSGAStoreOp
		{

			/// <summary><c> operator ()</c> stores scaled fitness to chromosome storage object.</summary>
			/// <param name="chromosome">chromosome storage object to which the scaled fitness should be stored.</param>
			/// <param name="sum">scaled fitness fitness.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				float sum)
			{
				GaNSGAFitness& fitness = (GaNSGAFitness&)chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_SCALED );
				fitness.SetValue( fitness.GetValue() / sum );
			}

		};

		/// <summary><c>GaNSGADummyStorage</c> struct stores dummy fitness that should be assigned to chromosomes in current pareto set.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		struct GaNSGADummyStorage
		{

			/// <summary>Currently global minimal assigned dummy fitness to a processed chromosome.</summary>
			float _dummyFitness;

			/// <summary>Minimal fitness found by each workflow branch.</summary>
			Common::Data::GaSingleDimensionArray<float> _minBuffer;

			/// <summary><c>UpdateDummyFitness</c> determins global minimal assigned dummy fitness that was assigned by any branch.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL UpdateDummyFitness()
			{
				_dummyFitness = _minBuffer[ 0 ];
				for( int i = _minBuffer.GetSize() - 1; i > 0; i-- )
				{
					if( _dummyFitness > _minBuffer[ i ] )
						_dummyFitness = _minBuffer[ i ];
				}
			}

		};

		/// <summary><c>GaNSGACleanOp</c> class represents operation that cleans chromosome tags used by NSGA and prepares them for execution of the algorithm.</summary>
		class GaNSGACleanOp
		{

		private:

			/// <summary>Chromosome tag that indicates whether the pareto front to which chromosome belongs is already determined.</summary>
			Population::GaChromosomeTagGetter<int> _processed;

			/// <summary>Chromosome tag that contains partially calculated shared fitness.</summary>
			Population::GaChromosomeTagGetter<Common::Data::GaPartialSum> _partialSum;

		public:

			/// <summary>Initializes operaion.</summary>
			/// <param name="processed">chromosome tag that indicates whether the pareto front to which chromosome belongs is already determined.</param>
			/// <param name="partialSum">chromosome tag that contains partially calculated shared fitness.</param>
			GaNSGACleanOp(const Population::GaChromosomeTagGetter<int>& processed,
				const Population::GaChromosomeTagGetter<Common::Data::GaPartialSum>& partialSum) : _processed(processed),
				_partialSum(partialSum) { }

			/// <summary><c>operator ()</c> cleans chromosome's tags.</summary>
			/// <param name="chromosome">chromosome whose tags should be cleaned.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				// clear partial sum
				Common::Data::GaPartialSum& sum = _partialSum( chromosome );
				for( int i = sum.GetSize() - 1; i >= 0; i-- )
					sum[ i ] = 0;

				// clear processed flag
				_processed( chromosome ) = 0;
			}

		};

		/// <summary><c>GaNSGADominanceOp</c> class represents operation that determins dominant chromosome and marks it.</summary>
		class GaNSGADominanceOp
		{

		private:

			/// <summary>Population over whose chromosomes dominance is determined.</summary>
			Population::GaPopulation& _population;

			/// <summary>Chromosome tag that stores information whether the chromosome is dominated by any other chromosome in the population.</summary>
			Population::GaChromosomeTagGetter<int> _dominated;

			/// <summary>Chromosome tag that indicates whether the pareto front to which chromosome belongs is already determined.</summary>
			Population::GaChromosomeTagGetter<int> _processed;

		public:

			/// <summary>Initializes operaion.</summary>
			/// <param name="population">population over whose chromosomes dominance is determined.</param>
			/// <param name="dominated">chromosome tag that stores information whether the chromosome is dominated by any other chromosome in the population.</param>
			/// <param name="processed">chromosome tag that indicates whether the pareto front to which chromosome belongs is already determined.</param>
			GaNSGADominanceOp(Population::GaPopulation& population,
				const Population::GaChromosomeTagGetter<int>& dominated,
				const Population::GaChromosomeTagGetter<int>& processed) : _population(population),
				_dominated(dominated),
				_processed(processed) { }

			/// <summary>Dummy operator to satisfy interface.</summary>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index) { }

			/// <summary><c>operator ()</c> operators determins dominated chromosome and marks it.</summary>
			/// <param name="chromosome1">the first chromosomes that is checked.</param>
			/// <param name="chromosome1">the second chromosomes that is checked.</param>
			/// <param name="index1">index of the first chromosome.</param>
			/// <param name="index2">index of the second chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome1,
				Population::GaChromosomeStorage& chromosome2,
				int index1,
				int index2)
			{
				// skip already chromosomes with determined pareto front and 
				/// there is no need to compare chromosomes if both are already dominanted by other chromosomes
				if( !_processed( chromosome1 ) && !_processed( chromosome2 )
					&& ( !_dominated( chromosome1 ) || !_dominated( chromosome2 ) ) )
				{
					// get dominant chromosome
					int dom = _population.CompareFitness( chromosome1, chromosome2 );

					// is the first one is dominant
					if( dom > 0 )
						// mark domnated
						_dominated( chromosome1 ) = 1;
					// is the second one is dominant
					else if( dom < 0 )
						// mark domnated
						_dominated( chromosome2 ) = 1;
				}
			}

		};

		/// <summary><c>GaNSGADummyFitnessOp</c> class represents operation assigns dummy fitness to chromosomes in the current pareto front.</summary>
		class GaNSGADummyFitnessOp
		{

		private:

			/// <summary>Dummy scaled fitness that should be assigned to chromosome.</summary>
			float _dummyFitness;

			/// <summary>Chromosome tag that stores information whether the chromosome is dominated by any other chromosome in the population.</summary>
			Population::GaChromosomeTagGetter<int> _dominated;

			/// <summary>Chromosome tag that indicates whether the pareto front to which chromosome belongs is already determined.</summary>
			Population::GaChromosomeTagGetter<int> _processed;

			/// <summary>Buffer to which the processed chromosomes are stored.</summary>
			Population::GaChromosomeGroup& _processedBuffer;

		public:

			/// <summary>Initializes operaion.</summary>
			/// <param name="dummyFitness">summy scaled fitness that should be assigned to chromosome.</param>
			/// <param name="dominated">chromosome tag that stores information whether the chromosome is dominated by any other chromosome in the population.</param>
			/// <param name="processed">chromosome tag that indicates whether the pareto front to which chromosome belongs is already determined.</param>
			/// <param name="processedBuffer"></param>
			GaNSGADummyFitnessOp(float dummyFitness,
				const Population::GaChromosomeTagGetter<int>& dominated,
				const Population::GaChromosomeTagGetter<int>& processed,
				Population::GaChromosomeGroup& processedBuffer) : _dummyFitness(dummyFitness),
				_dominated(dominated),
				_processed(processed),
				_processedBuffer(processedBuffer) { }

			/// <summary><c>operator ()</c> assignes specified fitness to chromosome.</summary>
			/// <param name="chromosome">chromosome to which the fitness should be assigned.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				// assign fitness only to chromosomes in current pareto front
				if( !_dominated( chromosome ) && !_processed( chromosome ) )
				{
					// assign fitness
					( (GaNSGAFitness&)chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_SCALED ) ).SetValue( _dummyFitness );

					// store chromosome to processed and mark it
					_processedBuffer.AddAtomic( &chromosome );
					_processed( chromosome ) = 1;
				}

				_dominated( chromosome ) = 0;
			}

		};

		/// <summary><c>GaNSGAMinDummyFitnessOp</c> class represents operation that determins minimal dummy fitness assigned by NSGA.</summary>
		class GaNSGAMinDummyFitnessOp
		{

		private:

			/// <summary>Reference to variable to which the minimal fitness should be stored.</summary>
			float& _min;

		public:

			/// <summary>Initializes operaion.</summary>
			/// <param name="min">reference to variable to which the minimal fitness should be stored.</param>
			GaNSGAMinDummyFitnessOp(float& min) : _min(min) { }

			/// <summary><c>operator ()</c> checks whether the chromosome has lower assigned fitness then previously identified minimum and stores it.</summary>
			/// <param name="chromosome">chromosome whose fitness is checked.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				// get assigned fitness
				float fitness = ( (GaNSGAFitness&)chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_SCALED ) ).GetValue();

				// is chromosome's fitness lower then current minimum?
				if( fitness < _min )
					_min = fitness;
			}

		};

		/// <summary><c>GaNSGAAdjustFitnessOp</c> class represents operation that adjusts assigned fitness to chromosomes by specified value.</summary>
		class GaNSGAAdjustFitnessOp
		{

		private:

			/// <summary>Value by which chromosomes' fitness should be adjusted.</summary>
			float _adjustment;

		public:

			/// <summary>Initializes operaion.</summary>
			/// <param name="adjustment">value by which chromosomes' fitness should be adjusted.</param>
			GaNSGAAdjustFitnessOp(float adjustment) : _adjustment(adjustment) { }

			/// <summary><c>operator ()</c> operator adds adjustment value to assigned fitness value of chromosome.</summary>
			/// <param name="chromosome">chromosome whose fitness value should be adjusted.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				// get current fitness of chromosome and adjust it
				GaNSGAFitness& fitness = (GaNSGAFitness&)chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_SCALED );
				fitness.SetValue( fitness.GetValue() + _adjustment );
			}

		};

		// Prepares population for scaling operation
		void GaNSGA::Prepare(Population::GaPopulation& population,
			const Population::GaScalingParams& parameters,
			const Population::GaScalingConfig& configuration,
			int branchCount) const
		{
			// add required chromosome tags
			population.GetChromosomeTagManager().AddTag( ( (const GaNSGAParams&)parameters ).GetDominatedTagID(), Common::Data::GaTypedTagLifecycle<int>() );
			population.GetChromosomeTagManager().AddTag( ( (const GaNSGAParams&)parameters ).GetProcessedTagID(), Common::Data::GaTypedTagLifecycle<int>() );
			population.GetChromosomeTagManager().AddTag( ( (const GaNSGAParams&)parameters ).GetPartialSumTagID(),
				Common::Data::GaSizableTagLifecycle<Common::Data::GaPartialSum>( branchCount ) );

			// add required population tags
			population.GetTagManager().AddTag( ( (const GaNSGAParams&)parameters ).GetProcessedBufferTagID(), Common::Data::GaTypedTagLifecycle<Population::GaChromosomeGroup>() );
			population.GetTagManager().AddTag( ( (const GaNSGAParams&)parameters ).GetDummyTagID(), Common::Data::GaTypedTagLifecycle<GaNSGADummyStorage>() );

			GaScalingOperation::Prepare( population, parameters, configuration, branchCount );
		}

		// Removes data that was needed by the scaling operation
		void GaNSGA::Clear(Population::GaPopulation& population,
			const Population::GaScalingParams& parameters,
			const Population::GaScalingConfig& configuration,
			int branchCount) const
		{
			// remove used chromosome tags
			population.GetChromosomeTagManager().RemoveTag( ( (const GaNSGAParams&)parameters ).GetDominatedTagID() );
			population.GetChromosomeTagManager().RemoveTag( ( (const GaNSGAParams&)parameters ).GetProcessedTagID() );
			population.GetChromosomeTagManager().RemoveTag( ( (const GaNSGAParams&)parameters ).GetPartialSumTagID() );

			// remove required population tags
			population.GetTagManager().RemoveTag( ( (const GaNSGAParams&)parameters ).GetProcessedBufferTagID() );
			population.GetTagManager().RemoveTag( ( (const GaNSGAParams&)parameters ).GetDummyTagID() );

			GaScalingOperation::Clear( population, parameters, configuration, branchCount );
		}

		// Updates population for scaling operation
		void GaNSGA::Update(Population::GaPopulation& population,
			const Population::GaScalingParams& parameters,
			const Population::GaScalingConfig& configuration,
			int branchCount) const
		{
			// update chromosome tags
			population.GetChromosomeTagManager().UpdateTag( ( (const GaNSGAParams&)parameters ).GetPartialSumTagID(), Common::Data::GaPartialSumSizeUpdate( branchCount ) );
			population.GetTagByID<GaNSGADummyStorage>( ( (const GaNSGAParams&)parameters ).GetDummyTagID() )._minBuffer.SetSize( branchCount );

			GaScalingOperation::Update( population, parameters, configuration, branchCount );
		}

		// Execute scaling operation
		void GaNSGA::Exec(Population::GaPopulation& population,
			const GaNSGAParams& parameters,
			const Population::ScalingOperations::GaShareFitnessScalingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			Population::GaOperationTime timer( population, Population::GADV_SCALING_TIME );

			int branchID = branch->GetFilteredID();
			int branchCount = branch->GetBarrierCount();

			// population tags
			GaNSGADummyStorage& dummyStorage = population.GetTagByID<GaNSGADummyStorage>( parameters.GetDummyTagID() );
			Population::GaChromosomeGroup& processedBuffer = population.GetTagByID<Population::GaChromosomeGroup>( parameters.GetProcessedBufferTagID() );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
			{
				// mark population as re-scaled
				population.GetFlags().SetFlags( Population::GaPopulation::GAPF_COMPLETE_SCALED_FITNESS_UPDATE );

				// update buffer sizes when population size is changed
				processedBuffer.Clear();
				processedBuffer.SetSize( population.GetPopulationParams().GetPopulationSize() );
				dummyStorage._dummyFitness = (float)population.GetCount();
			}

			// chromosome tags
			Population::GaChromosomeTagGetter<Common::Data::GaPartialSum> getSum( parameters.GetPartialSumTagID(), population.GetChromosomeTagManager() );
			Population::GaChromosomeTagGetter<int> getDominated( parameters.GetDominatedTagID(), population.GetChromosomeTagManager() );
			Population::GaChromosomeTagGetter<int> getProcessed( parameters.GetProcessedTagID(), population.GetChromosomeTagManager() );

			// NSGA parameters
			float delta = parameters.GetDelta() * dummyStorage._dummyFitness;
			float cutoff = parameters.GetCutoff();
			float alpha = parameters.GetAlpha();

			Common::Workflows::GaParallelExec1<Population::GaPopulation, Population::GaChromosomeStorage> popWorkDist( *branch, population );
			Common::Workflows::GaParallelExec2<Population::GaPopulation, Population::GaChromosomeStorage> domWorkDist( *branch, population );

			// clear chromosome tags before execution of the algorithm
			popWorkDist.Execute( GaNSGACleanOp( getProcessed, getSum ), true );

			// identify each pareto front
			int orderIndexStart = 0;
			while( orderIndexStart < population.GetCount() )
			{
				// identify nondominated chromosome for the current pareto front and assign dummy fitness value to them 
				domWorkDist.Execute( GaNSGADominanceOp( population, getDominated, getProcessed ), true );
				popWorkDist.Execute( GaNSGADummyFitnessOp( dummyStorage._dummyFitness, getDominated, getProcessed, processedBuffer ), true );

				typedef Common::Workflows::GaLimitedItemProvider1<Population::GaChromosomeGroup, Population::GaChromosomeStorage> GaProvider;
				GaProvider provider( orderIndexStart );

				Common::Workflows::GaParallelExec2<Population::GaChromosomeGroup, Population::GaChromosomeStorage, GaProvider> shareWorkDist( *branch, processedBuffer, provider );
				Common::Workflows::GaParallelExec1<Population::GaChromosomeGroup, Population::GaChromosomeStorage, GaProvider> sumWorkDist( *branch, processedBuffer, provider );

				// perform fitness sharing to assigned values for the current pareto front
				shareWorkDist.Execute( Population::ScalingOperations::GaFitnessSharingOp( getSum, configuration.GetComparator(), cutoff, alpha, branchID ), true );
				sumWorkDist.Execute( Population::ScalingOperations::GaSumFitnessSharingOp<GaNSGAStoreOp>( getSum, GaNSGAStoreOp() ), true );

				Common::Workflows::GaParallelExec1<Population::GaChromosomeGroup, Population::GaChromosomeStorage> procWorkDist( *branch, processedBuffer );

				// get minimal assigned fitness value fot the branch
				dummyStorage._minBuffer[ branchID ] = dummyStorage._dummyFitness;
				procWorkDist.Execute( GaNSGAMinDummyFitnessOp( dummyStorage._minBuffer[ branchID ] ), false );

				// get global minimum os assigned fitness value
				GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
					dummyStorage.UpdateDummyFitness();

				// is adjustment of fitness values assigned to processed chromosomes required
				/// so chromosomes in new front can have positive fitness?
				float probabilityAdj = 0;
				if( dummyStorage._dummyFitness < delta )
				{
					// calculate adjustment and adjust fitness value of processed chromosomes
					probabilityAdj = 2 * delta - dummyStorage._dummyFitness;
					procWorkDist.Execute( GaNSGAAdjustFitnessOp( probabilityAdj ), false );
				}

				// calculate dummy fitness that will be assigned to chromosome in next pareto front
				GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
					dummyStorage._dummyFitness += probabilityAdj - delta;

				orderIndexStart = processedBuffer.GetCount();
			}

			// update operation time statistics
			timer.UpdateStatistics();
		}
		
		/// <summary><c>GaNSGA2Front</c> class handles pareto front for NSGA-II operation.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaNSGA2Front
		{

		private:

			/// <summary>Array that stores chromosomes currently in the front.</summary>
			Common::Data::GaSingleDimensionArray<Population::GaChromosomeStorage*> _front;

			/// <summary>Number of chromosomes currently in the front.</summary>
			Common::Threading::GaAtomic<int> _currentCount;

			/// <summary>Groups that stores chromsomes currently in the front sorted according to each objective.</summary>
			Common::Data::GaSingleDimensionArray<Population::GaChromosomeGroup> _sortedFronts;

			/// <summary>Stores distances between the wors and the best chromosomes for each objective.</summary>
			Common::Data::GaSingleDimensionArray<float> _frontDistances;

			/// <summary>Comparators for each objective used for sorting chromosomes in the front.</summary>
			Common::Data::GaSingleDimensionArray<Fitness::GaFitnessComparatorSetup> _vegaComparators;

			Fitness::Comparators::GaVegaComparator _vega;

		public:

			/// <summary>This constructor initializes empty pareto front.</summary>
			GaNSGA2Front() { }

			/// <summary><c>QueueChromosome</c> method inserts chromosome to pareto front.
			/// Different threads can safely execute this method simultaneously, but no other methods on this object should be executed from other threads at that time.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="chromosome">chromosome that should be inserted.</param>
			inline void GACALL QueueChromosome(Population::GaChromosomeStorage* chromosome) { _front[ _currentCount++ ] = chromosome; }

			/// <summary><c>AcquireQueuedChromosomes</c> method creates chromosome group in the front that is sorted according to specified objective.
			/// Different threads can safely execute this method simultaneously, but only for differen objectives and no other methods on this object
			/// should be executed from other threads at that time.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="objectiveIndex">objective for which the sorted chromosome group should be created.</param>
			/// <param name="sortType">defines how the objective values should be interpeted and in what order chromosomes should be sorted.</param>
			/// <param name="branchID">ID of the branch that execute operation.</param>
			void GACALL AcquireQueuedChromosomes(int objectiveIndex,
				Fitness::Comparators::GaComparisonType sortType,
				int branchID)
			{
				Population::GaChromosomeGroup& group = _sortedFronts[ objectiveIndex ];
				group.Clear();

				// insert chromosomes in group that should be sorted
				for( int i = _currentCount - 1; i >= 0; i-- )
					group.Add( _front[ i ] );

				// prepare sorting comparator
				Fitness::Comparators::GaVegaComparatorParams& params = (Fitness::Comparators::GaVegaComparatorParams&)_vegaComparators[ branchID ].GetParameters();
				params.SetVectorIndex( objectiveIndex );
				params.SetType( sortType );

				// sort chromosomes in the group according to specified objetive value
				group.Sort( Population::GaFitnessComparatorSortingCriteria( _vegaComparators[ branchID ], Population::GaChromosomeStorage::GAFT_RAW ) );

				// calculate the distance between the best and the worst chromosome for the objective value
				_frontDistances[ objectiveIndex ] = ( (GaNSGA2Fitness&)group[ _currentCount - 1 ].GetFitness( Population::GaChromosomeStorage::GAFT_RAW ) ).
					Distance( (GaNSGA2Fitness&)group[ 0 ].GetFitness( Population::GaChromosomeStorage::GAFT_RAW ), objectiveIndex );

				// set crowding distance for most extreme chromosome to infinity to protect them from removing
				( (GaNSGA2Fitness&)group[ 0 ].GetFitness( Population::GaChromosomeStorage::GAFT_SCALED ) ).SetValue( std::numeric_limits<float>::infinity(), 1 );
				( (GaNSGA2Fitness&)group[ _currentCount - 1 ].GetFitness( Population::GaChromosomeStorage::GAFT_SCALED ) ).SetValue( std::numeric_limits<float>::infinity(), 1 );
			}

			/// <summary><c>Clear</c> removes all chromosomes from the front.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL Clear() { _currentCount = 0; }

			/// <summary><c>SetFrontSize</c> method sets size of buffers needed to store chromosomes in pareto front.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="frontSize">number of chromosomes that front should store.</param>
			/// <param name="objectivesCount">number of objective values that raw fitness has.</param>
			/// <param name="branchCount">number of branches that execute operation.</param>
			void GACALL SetFrontSize(int frontSize,
				int objectivesCount,
				int branchCount)
			{
				_front.SetSize( frontSize );
				_currentCount = 0;

				if( _sortedFronts.GetSize() != objectivesCount )
				{
					_sortedFronts.SetSize( objectivesCount, Population::GaChromosomeGroup( true ) );
					_frontDistances.SetSize( objectivesCount );
				}

				if( _vegaComparators.GetSize() != branchCount )
					_vegaComparators.SetSize( branchCount, Fitness::GaFitnessComparatorSetup( &_vega, &Fitness::Comparators::GaVegaComparatorParams(), NULL ) );
			}

			/// <summary><c>GetSortedFront</c> method returns chromosome group that stores chromosomes currently in the front sorted by specified objective.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="objectiveIndex">objective by which the chromosomes are sorted.</param>
			/// <returns>Method returns sorted chromosome group</returns>
			inline Population::GaChromosomeGroup& GACALL GetSortedFront(int objectiveIndex) { return _sortedFronts[ objectiveIndex ]; }

			/// <summary><c>GetFrontDistance</c> method returns distance between the first and the best chromosome in the front for specifed objective.</summary>
			/// <param name="objectiveIndex">objective for which the distance is queried.</param>
			/// <returns>Method returns distance between most exterme chromosomes.</returns>
			inline float GACALL GetFrontDistance(int objectiveIndex) const { return _frontDistances[ objectiveIndex ]; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of chromosomes currently stored in the front.</returns>
			inline int GACALL GetCurrentCount() const { return _currentCount; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of chromosomes that front can store.</returns>
			inline int GACALL GetFrontSize() const { return _front.GetSize(); }

		};

		/// <summary><c>GaNSGA2FirstFrontOp</c> class represents operation that identify chromosomes that belongs to the first pareto front and assigns ranking to the chromosome.</summary>
		class GaNSGA2FirstFrontOp
		{

		private:

			/// <summary>Buffer that stores chromosomes that are identified to belong to the current pareto front.</summary>
			GaNSGA2Front& _front;

			/// <summary>Chromosome tag that stores number of chromosomes that dominates the chromosome.</summary>
			Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > _domCount;

			/// <summary>Rank that chromosomes that belongs to the pareto front should get.</summary>
			int _rank;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="front">buffer that stores chromosomes that are identified to belong to the current pareto front.</param>
			/// <param name="domCount">chromosome tag that stores number of chromosomes that dominates the chromosome.</param>
			/// <param name="rank">rank that chromosomes that belongs to the pareto front should get.</param>
			GaNSGA2FirstFrontOp(GaNSGA2Front& front,
				const Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> >& domCount,
				int rank) : _front(front),
				_domCount(domCount),
				_rank(rank){ }

			/// <summary><c>operator ()</c> checks whether the chromosome is nondominated and insert it to cuurnet pareto front and assign ranking.</summary>
			/// <param name="chromosome">chromosome that should be checked and inserted into pareto front.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				GaNSGA2Fitness& fitness = (GaNSGA2Fitness&)chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_SCALED );

				// clear crowding distance part of scaled fitness value
				fitness.SetValue( 0, 1 );

				// nondominated chromosome?
				if( !_domCount( chromosome ) )
				{
					// yes - add chromosome to pareto front and assign ranking part of fitness value
					fitness.SetValue( (float)_rank, 0 );
					_front.QueueChromosome( &chromosome );
				}
			}

		};

		/// <summary><c>GaNSGA2NextFrontOp</c> class represents operation that identify chromosomes that belongs to next pareto front and assigns ranking to the chromosome.</summary>
		class GaNSGA2NextFrontOp
		{

		private:

			/// <summary>Chromosome tag that stores number of chromosomes that dominates the chromosome.</summary>
			Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > _domCount;

			/// <summary>Chromosome tag that stores partial list of chromosomes that are dominated by the chromosome.</summary>
			Population::GaChromosomeTagGetter<GaDominanceList> _domList;

			/// <summary>Buffer that stores chromosomes that are identified to belong to the current pareto front.</summary>
			GaNSGA2Front& _front;

			/// <summary>Rank that chromosomes that belongs to the pareto front should get.</summary>
			int _rank;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="domCount">chromosome tag that stores number of chromosomes that dominates the chromosome.</param>
			/// <param name="domList">chromosome tag that stores partial list of chromosomes that are dominated by the chromosome</param>
			/// <param name="front">buffer that stores chromosomes that are identified to belong to the current pareto front.</param>
			/// <param name="rank">rank that chromosomes that belongs to the pareto front should get.</param>
			GaNSGA2NextFrontOp(const Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> >& domCount,
				const Population::GaChromosomeTagGetter<GaDominanceList>& domList,
				GaNSGA2Front& front,
				int rank) : _domCount(domCount),
				_domList(domList),
				_front(front),
				_rank(rank) { }

			/// <summary><c>operator ()</c> checks whether the chromosome is nondominated and insert it to cuurnet pareto front and assign ranking.</summary>
			/// <param name="chromosome">chromosome that should be checked and inserted into pareto front.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				GaDominanceList& dominatedList = _domList( chromosome );

				// check all dominated chromosomes
				for( int i = dominatedList.GetSize() - 1; i >= 0; i-- )
				{
					for( Common::Data::GaListNode<Population::GaChromosomeStorage*>* node = dominatedList[ i ].GetHead(); node; node = node->GetNext() )
					{
						Population::GaChromosomeStorage* dominated = node->GetValue();

						// current chromosome is not dominated by any other chromosome except by those already placed in previous pareto fronts?
						if( --_domCount( dominated ) == 0 )
						{
							// yes - insert chromosome to next pareto front and assign ranking
							( (GaNSGA2Fitness&)dominated->GetFitness( Population::GaChromosomeStorage::GAFT_SCALED ) ).SetValue( (float)_rank, 0 );
							_front.QueueChromosome( dominated );
						}
					}

					dominatedList[ i ].Clear();
				}
			}

		};

		/// <summary><c>GaNSGA2DistanceOp</c> class repesents operation that caluclates and assigns crowding distance to chromosomes in current pareto front.</summary>
		class GaNSGA2DistanceOp
		{

		private:

			/// <summary>Group that stores chromosomes that belongs to current pareto front sorted by specified objective value.</summary>
			Population::GaChromosomeGroup& _front;

			/// <summary>Objective for which the crowding distance is caluclated.</summary>
			int _objectiveIndex;

			/// <summary>Distance between the worst and the best chromosome for specified objective.</summary>
			float _maxDistance;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="front">group that stores chromosomes that belongs to current pareto front sorted by specified objective value.</param>
			/// <param name="objectiveIndex">objective for which the crowding distance is caluclated.</param>
			/// <param name="maxDistance">distance between the worst and the best chromosome for specified objective.</param>
			GaNSGA2DistanceOp(Population::GaChromosomeGroup& front,
				int objectiveIndex,
				float maxDistance) : _front(front),
				_objectiveIndex(objectiveIndex),
				_maxDistance(maxDistance) { }

			/// <summary><c>operator ()</c> operator updates crowding distance of chromosome for specified objective.</summary>
			/// <param name="chromosome">chromosome whose crowding distance should be updated.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				GaNSGA2Fitness& fitness = (GaNSGA2Fitness&)chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_SCALED );

				// calculate updated crowding distance of chromosome for specified objective.
				float v = fitness.GetValue( 1 ) + 
					( ( (GaNSGA2Fitness&)_front[ index + 1 ].GetFitness( Population::GaChromosomeStorage::GAFT_RAW ) ).Distance(
					(GaNSGA2Fitness&)_front[ index - 1 ].GetFitness( Population::GaChromosomeStorage::GAFT_RAW ), _objectiveIndex ) ) / _maxDistance;

				// store updated distance
				fitness.SetValue( v, 1 );
			}

		};

		// Prepares population for scaling operation
		void GaNSGA2::Prepare(Population::GaPopulation& population,
			const Population::GaScalingParams& parameters,
			const Population::GaScalingConfig& configuration,
			int branchCount) const
		{
			// add required chromosome tags
			population.GetChromosomeTagManager().AddTag( ( (const GaNSGA2Params&)parameters ).GetDominatedCountTagID(),
				Common::Data::GaTypedTagLifecycle<Common::Threading::GaAtomic<int> >() );
			population.GetChromosomeTagManager().AddTag( ( (const GaNSGA2Params&)parameters ).GetDominatedListTagID(),
				Common::Data::GaSizableTagLifecycle<GaDominanceList>( branchCount ) );

			// add required population tags
			population.GetTagManager().AddTag( ( (const GaNSGA2Params&)parameters ).GetFrontTagID(), Common::Data::GaTypedTagLifecycle<GaNSGA2Front>() );

			GaScalingOperation::Prepare( population, parameters, configuration, branchCount );
		}

		// Removes data that was needed by the scaling operation
		void GaNSGA2::Clear(Population::GaPopulation& population,
			const Population::GaScalingParams& parameters,
			const Population::GaScalingConfig& configuration,
			int branchCount) const
		{
			// remove used chromosome tags
			population.GetChromosomeTagManager().RemoveTag( ( (const GaNSGA2Params&)parameters ).GetDominatedCountTagID() );
			population.GetChromosomeTagManager().RemoveTag( ( (const GaNSGA2Params&)parameters ).GetDominatedListTagID() );

			// remove required population tags
			population.GetTagManager().RemoveTag( ( (const GaNSGA2Params&)parameters ).GetFrontTagID() );

			GaScalingOperation::Clear( population, parameters, configuration, branchCount );
		}

		// Updates population for scaling operation
		void GaNSGA2::Update(Population::GaPopulation& population,
			const Population::GaScalingParams& parameters,
			const Population::GaScalingConfig& configuration,
			int branchCount) const
		{
			// update population tags
			population.GetChromosomeTagManager().UpdateTag( ( (const GaNSGA2Params&)parameters ).GetDominatedListTagID(), GaDominanceListUpdate( branchCount ) );

			GaScalingOperation::Update( population, parameters, configuration, branchCount );
		}

		// Execute scaling operation
		void GaNSGA2::Exec(Population::GaPopulation& population,
			const GaNSGA2Params& parameters,
			const Population::GaScalingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			Population::GaOperationTime timer( population, Population::GADV_SCALING_TIME );

			int branchID = branch->GetFilteredID();
			int branchCount = branch->GetBarrierCount();

			int objectivesCount = ( (Fitness::Representation::GaMVFitnessParams&)population.GetFitnessParams() ).GetValueCount();

			// population tags
			GaNSGA2Front& front = population.GetTagByID<GaNSGA2Front>( parameters.GetFrontTagID() );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
			{
				// mark population as re-scaled
				population.GetFlags().SetFlags( Population::GaPopulation::GAPF_COMPLETE_SCALED_FITNESS_UPDATE );

				// update buffer sizes when population size is changed
				front.SetFrontSize( population.GetPopulationParams().GetPopulationSize(), objectivesCount, branchCount );
			}
			
			// chromosome tags
			Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > getDomCount( parameters.GetDominatedCountTagID(), population.GetChromosomeTagManager() );
			Population::GaChromosomeTagGetter<GaDominanceList> getDomList( parameters.GetDominatedListTagID(), population.GetChromosomeTagManager() );

			Common::Workflows::GaParallelExec1<Population::GaPopulation, Population::GaChromosomeStorage> popWorkDist( *branch, population );
			Common::Workflows::GaParallelExec2<Population::GaPopulation, Population::GaChromosomeStorage> domWorkDist( *branch, population );

			int rank = population.GetCount();

			// identify nondominated chromosome and create the first pareto front
			domWorkDist.Execute( GaDominanceOp( population, getDomCount, getDomList, branchID ), true );
			popWorkDist.Execute( GaNSGA2FirstFrontOp( front, getDomCount, rank ), true );

			Fitness::Comparators::GaComparisonType sortType = ( (Fitness::Comparators::GaSimpleComparatorParams&)population.GetFitnessComparator().GetParameters() ).GetType();

			int count, start;
			branch->SplitWork( objectivesCount, count, start );

			// identify each front and assign fitness
			while( front.GetCurrentCount() )
			{
				// create sorted groups for current front for each objective
				for( int i = start + count - 1; i >= start; i-- )
					front.AcquireQueuedChromosomes( i, sortType, branchID );

				// prepare buffer for next pareto front
				GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
					front.Clear();

				// calculate and assign crowding distance to chromosomes
				for( int objectiveIndex = objectivesCount - 1; objectiveIndex >= 0; objectiveIndex-- )
				{
					// get distance between most extreme chromosomes for current objective
					float diff = front.GetFrontDistance( objectiveIndex );
					if( diff != 0 )
					{
						Population::GaChromosomeGroup& sortedFront = front.GetSortedFront( objectiveIndex );

						typedef Common::Workflows::GaLimitedItemProvider2<Population::GaChromosomeGroup, Population::GaChromosomeStorage> GaProvider;
						Common::Workflows::GaParallelExec1<Population::GaChromosomeGroup, Population::GaChromosomeStorage, GaProvider>
							frontWorkDist( *branch, sortedFront, GaProvider( 1, sortedFront.GetCount() - 2 ) );

						// update crowding distance to chromosomes for current objective
						frontWorkDist.Execute( GaNSGA2DistanceOp( sortedFront, objectiveIndex, diff ), false );
					}
				}

				// identify next pareto front and assign rank to chromosomes
				Common::Workflows::GaParallelExec1<Population::GaChromosomeGroup, Population::GaChromosomeStorage> frontWorkDist( *branch, front.GetSortedFront( 0 ) );
				frontWorkDist.Execute( GaNSGA2NextFrontOp( getDomCount, getDomList, front, --rank ), true );
			}

			// update operation time statistics
			timer.UpdateStatistics();
		}

	} // NSGA
} // Multiobjective

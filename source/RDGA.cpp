
/*! \file RDGA.cpp
	\brief This file conatins implementation of classes that implement Rank-Desity based Genetic Algorithm (RDGA).
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#include "PopulationStatistics.h"
#include "RDGA.h"

namespace Multiobjective
{
	namespace RDGA
	{
		
		/// <summary><c>GaRDGAUnranked</c> class tracks chromosomes that belongs to current pareto front.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but some public methods are thread-safe.</summary>
		class GaRDGAUnranked
		{

		private:

			/// <summary>List that holds unranked chromosomes for current pareto front.</summary>
			Common::Data::GaAtomicList<Population::GaChromosomeStorage, Population::GaChromosomeTagGetter<Population::GaChromosomeStorage*> > _unranked;

			/// <summary>Number of unranked chromosomes in the population chromosomes.</summary>
			Common::Threading::GaAtomic<int> _count;

		public:

			/// <summary>This constructor initializes empty list of chromosomes.</summary>
			GaRDGAUnranked() { }

			/// <summary><c>Dequeue</c> method removes one chromosome from the current pareto front updates count of unranked chromosomes.
			/// If the list is empty method waits for chromosome to be queued.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns pointer to dequeued chromosomes.</returns>
			Population::GaChromosomeStorage* GACALL Dequeue()
			{
				// wait and dequeue chromosome
				Population::GaChromosomeStorage* chromosome;
				while( ( chromosome = _unranked.Pop() ) == NULL )
				{
					if( !_count )
						return NULL;
				}

				// decremen unranked count
				--_count;
				return chromosome;
			}

			/// <summary><c>Queue</c> method inserts chromosome in list of chromosomes of current pareto front.
			///
			/// This method is thread-safe.</summary>
			/// <param name="chromosome">chromosome that should be queued.</param>
			inline void GACALL Queue(Population::GaChromosomeStorage* chromosome) { _unranked.Push( chromosome ); }

			/// <summary><c>SetCount</c> method sets how many unranked chromosomes are ther in population.
			///
			/// This method is thread-safe.</summary>
			/// <param name="count">number of unranked chromosomes/</param>
			inline void GACALL SetCount(int count) { _count = count; }

			/// <summary><c>SetNextInFrontID</c> method sets chromosome tag that references next chromosome in current pareto front.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			/// <param name="tagManager">manager that handles chromosome tags.</param>
			inline void GACALL SetNextInFrontID(int tagID,
				Common::Data::GaTagManager& tagManager) { _unranked.GetNextAccessor().SetTagID( tagID, tagManager ); }

		};

		/// <summary><c>GaRDGAGrid</c> class handles conversation from adaptive grid to fixed hypergrid that can be used to spit hyperspace in hyperboxes.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaRDGAGrid
		{

		private:

			/// <summary>Most extreme points in fitness hyperspace found by each branch.</summary>
			Common::Data::GaSingleDimensionArray<GaFitnessGridBounds> _adaptiveGridBounds;

			/// <summary>Fixed hypergrid created by adaptive grid to accmodate most extreme points in fitness hyperspace.</summary>
			Common::Memory::GaAutoPtr<GaFitnessHyperGrid> _hyperGrid;

			/// <summary>Adaptive grid that created fixed hypergrid.</summary>
			const GaFitnessAdaptiveGrid* _currentAdaptiveGrid;

		public:

			/// <summary>This constructor initializes empty storage object.</summary>
			GaRDGAGrid() : _currentAdaptiveGrid(NULL) { }

			/// <summary>This constructor creates copy of storage object.</summary>
			/// <param name="rhs">object that should be copied.</param>
			GaRDGAGrid(const GaRDGAGrid& rhs) : _adaptiveGridBounds(rhs._adaptiveGridBounds),
				_currentAdaptiveGrid(rhs._currentAdaptiveGrid)
			{
				// creates copy of hypergird
				if( !rhs._hyperGrid.IsNull() )
					_hyperGrid = rhs._hyperGrid->Clone();
			}

			/// <summary><c>SetSize</c> method sets size of sotrage for most extreme points and updates object that stores them.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="size">number of branches will use storage.</param>
			/// <param name="population">population whose extreme points should be stored.</param>
			void GACALL SetSize(int size,
				Population::GaPopulation& population)
			{
				// bound object that stores extreme points should be updated when fitness operation is changed
				if( population.GetFlags().IsFlagSetAny( Population::GaPopulation::GAPF_FITNESS_OPERATION_CHANGED ) )	
				{
					// update all objects
					for( int i = _adaptiveGridBounds.GetSize() - 1; i >= 0; i-- )
					{
						// create new bound object so it can store new type of extreme points
						Common::Memory::GaAutoPtr<Fitness::GaFitness> prototype = population.CreateFitnessObject( Population::GaChromosomeStorage::GAFT_RAW );
						_adaptiveGridBounds[ i ].SetBoundsObjects( *prototype );
					}
				}

				size++;

				// resize storage
				int oldSize = _adaptiveGridBounds.GetSize();
				if( size != oldSize )
				{
					_adaptiveGridBounds.SetSize( size );

					// create new bound objects if new storage size is greater than previous size
					while( --size >= oldSize )
					{
						Common::Memory::GaAutoPtr<Fitness::GaFitness> prototype = population.CreateFitnessObject( Population::GaChromosomeStorage::GAFT_RAW );
						_adaptiveGridBounds[ size ].SetBoundsObjects( *prototype );
					}
				}
			}

			/// <summary><c>SetAdaptiveGrid</c> method sets adaptive grid that is used for creating and updating fixed hypergrid.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="adaptiveGrid">adaptive grid that should manage fixed hypergrid.</param>
			void GACALL SetAdaptiveGrid(const GaFitnessAdaptiveGrid* adaptiveGrid)
			{
				// has adaptive grid been changed?
				if( _currentAdaptiveGrid != adaptiveGrid )
				{
					// destory fixed hypergrid created by previous adaptive grid
					_hyperGrid = NULL;

					_currentAdaptiveGrid = adaptiveGrid;
				}
			}

			/// <summary><c>Update</c> method finds new most extreme points in fitness hyperspace and updates or creates fixed hypergrid so it can accmodate them.
			///
			/// This method is not thread-safe.</summary>
			/// <returns>Method returns reference to object that stores most extreme points in fitness hyperspace.</returns>
			const GaFitnessGridBounds& GACALL Update()
			{
				GaFitnessGridBounds& output = _adaptiveGridBounds[ _adaptiveGridBounds.GetSize() - 1 ];

				output.SetLowerBounds( _adaptiveGridBounds[ 0 ].GetLowerBounds() );
				output.SetUpperBounds( _adaptiveGridBounds[ 0 ].GetUpperBounds() );

				// find the most extreme points among those identified by each branch
				for( int i = _adaptiveGridBounds.GetSize() - 2; i > 0; i-- )
				{
					_currentAdaptiveGrid->UpdateGridBounds( _adaptiveGridBounds[ i ].GetLowerBounds(), output );
					_currentAdaptiveGrid->UpdateGridBounds( _adaptiveGridBounds[ i ].GetUpperBounds(), output );
				}

				// update hypergrid
				_currentAdaptiveGrid->UpdateHyperGrid( output, _hyperGrid );
				return output;
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to fixed hypergird created by adaptive grid.</returns>
			inline GaFitnessHyperGrid& GACALL GetHyperGrid() { return *_hyperGrid; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to fixed hypergird created by adaptive grid.</returns>
			inline const GaFitnessHyperGrid& GACALL GetHyperGrid() const { return *_hyperGrid; }

			/// <summary><c>operator []</c> method provides access to object that stores most extreme points in fitness hyperspace for specified branch.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="branchID">ID of branch whose storage object is queried.</param>
			/// <returns>Method returns reference to object that stores most extreme points in fitness hyperspace for specified branch.</returns>
			inline GaFitnessGridBounds& GACALL operator [](int branchID) { return _adaptiveGridBounds[ branchID ]; }

			/// <summary><c>operator []</c> method provides access to object that stores most extreme points in fitness hyperspace for specified branch.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="branchID">ID of branch whose storage object is queried.</param>
			/// <returns>Method returns reference to object that stores most extreme points in fitness hyperspace for specified branch.</returns>
			inline const GaFitnessGridBounds& GACALL operator [](int branchID) const { return _adaptiveGridBounds[ branchID ]; }

		};

		/// <summary><c>GaRDGAGridOp</c> class represents operation searches for most extreme points in fitness hyperspace and resizes adaptive grid.</summary>
		class GaRDGAGridOp
		{

		private:

			/// <summary>Adaptive grid that should be resized.</summary>
			const GaFitnessAdaptiveGrid& _adaptiveGrid;

			/// <summary>Bounds object that will store most extreme points found by the branch.</summary>
			GaFitnessGridBounds& _gridBounds;

			/// <summary>Chromosome tag that stores chromosome's rank.</summary>
			Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > _rank;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="adaptiveGrid">adaptive grid that should be resized.</param>
			/// <param name="gridBounds">bounds object that will store most extreme points found by the branch.</param>
			/// <param name="rank">chromosome tag that stores chromosome's rank.</param>
			GaRDGAGridOp(const GaFitnessAdaptiveGrid& adaptiveGrid,
				GaFitnessGridBounds& gridBounds,
				const Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> >& rank) : _adaptiveGrid(adaptiveGrid),
				_gridBounds(gridBounds),
				_rank(rank) { }

			/// <summary><c>operator ()</c> tests whether the chromosome's fitness is currently most extreme point in hyperspace and stores its coordinates.</summary>
			/// <param name="chromosome">chromosome whose fitness should be checked.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				_adaptiveGrid.UpdateGridBounds( chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_RAW ), _gridBounds );
				_rank( chromosome ) = 0;
			}

		};

		/// <summary><c>GaRDGAFrontOp</c> class represents operation that finds nondominated chromosomes that belongs to the first pareto front
		/// and identifies hyperboxes to which chromosomes belong and calculates grid density.</summary>
		class GaRDGAFrontOp
		{

		private:

			/// <summary>Hypergrid that splits fitness hyperspace.</summary>
			const GaFitnessHyperGrid& _hyperGrid;

			/// <summary>Chromosome tag that contains coordinates of hyperbox to which the chromosome belongs.</summary>
			Population::GaChromosomeTagGetter<Common::Grid::GaHyperBox> _hyperBox;

			Population::GaChromosomeTagGetter<Population::GaBestChromosomesMatrix::GaCell*> _cell;

			/// <summary>Lowest point that is contained by the grid.</summary>
			Fitness::GaFitness& _gridOrigin;

			/// <summary>List that stores chromosomes which belongs to current pareto front.</summary>
			GaRDGAUnranked& _unranked;

			/// <summary>Chromosome tag that stores number of chromosomes that dominates the chromosome.</summary>
			Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > _domCount;

			/// <summary>Chromosome tag that stores chromosome's rank.</summary>
			Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > _rank;

			/// <summary>Matrix that stores grid density.</summary>
			Population::GaBestChromosomesMatrix& _bestChromosome;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="hyperGrid">hypergrid that splits fitness hyperspace.</param>
			/// <param name="hyperBox">chromosome tag that contains coordinates of hyperbox to which the chromosome belongs.</param>
			/// <param name="gridOrigin">lowest point that is contained by the grid.</param>
			/// <param name="unranked">list that stores chromosomes which belongs to current pareto front.</param>
			/// <param name="domCount">chromosome tag that stores number of chromosomes that dominates the chromosome.</param>
			/// <param name="rank">chromosome tag that stores chromosome's rank.</param>
			GaRDGAFrontOp(const GaFitnessHyperGrid& hyperGrid,
				const Population::GaChromosomeTagGetter<Common::Grid::GaHyperBox>& hyperBox,
				const Population::GaChromosomeTagGetter<Population::GaBestChromosomesMatrix::GaCell*>& cell,
				Fitness::GaFitness& gridOrigin,
				GaRDGAUnranked& unranked,
				const Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> >& domCount,
				const Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> >& rank,
				Population::GaBestChromosomesMatrix& bestChromosome) : _hyperGrid(hyperGrid),
				_hyperBox(hyperBox),
				_cell(cell),
				_gridOrigin(gridOrigin),
				_rank(rank),
				_unranked(unranked),
				_domCount(domCount),
				_bestChromosome(bestChromosome) { }

			/// <summary><c>operator ()</c> determins whehter the chromosome belongs to the first pareto front and coordinates of hyperbox to which it belongs
			/// and updates grid density information.</summary>
			/// <param name="chromosome">chromosome which should be checked.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				// is chromosome nondominated?
				if( !_domCount( chromosome ) )
				{
					// assign rank to chromosome and add it to current pareto front
					_rank( chromosome ) = 1;
					_unranked.Queue( &chromosome );
				}

				// determins coordinates of hyperbox to which chromosome belongs
				Common::Grid::GaHyperBox& hyperBox = _hyperBox( chromosome );
				_hyperGrid.GetHyperBox( chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_RAW ), _gridOrigin, hyperBox );
				_cell( chromosome ) = _bestChromosome.Insert( hyperBox );
			}

		};

		/// <summary><c>GaRDGAMatrixOp</c> class represents operation that finds best ranked chromosomes in each hyperbox and stores them to matrix for each branch.</summary>
		class GaRDGAMatrixOp
		{

		private:

			/// <summary>Matrix that will store best ranked chromosomes for current branch.</summary>
			Population::GaBestChromosomesMatrix::GaItemMatrix::GaMatrix& _matrix;

			/// <summary>Chromosome tag that stores chromosome's rank.</summary>
			Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > _rank;

			/// <summary>Chromosome tag that contains coordinates of hyperbox to which the chromosome belongs.</summary>
			Population::GaChromosomeTagGetter<Common::Grid::GaHyperBox> _hyperBox;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="matrix">matrix that will store best ranked chromosomes  for current branch.</param>
			/// <param name="rank">chromosome tag that stores chromosome's rank.</param>
			/// <param name="hyperBox">chromosome tag that contains coordinates of hyperbox to which the chromosome belongs.</param>
			GaRDGAMatrixOp(Population::GaBestChromosomesMatrix::GaItemMatrix::GaMatrix& matrix,
				const Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> >& rank,
				const Population::GaChromosomeTagGetter<Common::Grid::GaHyperBox>& hyperBox) : _matrix(matrix),
				_rank(rank),
				_hyperBox(hyperBox) { }

			/// <summary><c>operator ()</c> checks whether the chromosome is best ranked chromosome in its hyperbox found so far.</summary>
			/// <param name="chromosome">chromosome that should be checked.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				Population::GaChromosomeStorage*& best = _matrix[ _hyperBox( chromosome ) ];

				// store chromosome if it has better ranking then previously identified chromosome
				if( !chromosome.GetFlags().IsFlagSetAny( Population::GaChromosomeStorage::GACF_REMOVE_CHROMOSOME ) && ( !best || _rank( chromosome ) > _rank( best ) ) )
					best = &chromosome;
			}

		};

		/// <summary><c>GaRDGARemoveOp</c> class represents operation selects child chromosomes in forbidden regisons and those that make population less diffusive 
		/// for removal from the population.</summary>
		class GaRDGARemoveOp
		{

		private:

			/// <summary>Population that should be trimmed.</summary>
			Population::GaPopulation& _population;

			/// <summary>Hypergrid that is used to split fitness hyperspace into hyperboxes.</summary>
			const GaFitnessHyperGrid& _hyperGrid;

			/// <summary>Lowest point that is contained by the grid.</summary>
			Fitness::GaFitness& _gridOrigin;

			/// <summary>Bounds object that is used for storing of hyperspace point that makes border of forbidden region.</summary>
			GaFitnessGridBounds& _bounds;

			/// <summary>Chromosome tag that contains coordinates of hyperbox to which the chromosome belongs.</summary>
			Population::GaChromosomeTagGetter<Common::Grid::GaHyperBox> _hyperBox;

			/// <summary>Chromosome tag that stores chromosome's rank.</summary>
			Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > _rank;

			/// <summary>Chromosome tag that references cell of matrix which stores best chromosomes to which this chromosome belongs.</summary>
			Population::GaChromosomeTagGetter<Population::GaBestChromosomesMatrix::GaCell*> _cell;

			// Half the count of chromosomes that has been inserted into population.
			int _half;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="population">population that should be trimmed.</param>
			/// <param name="hyperGrid">hypergrid that is used to split fitness hyperspace into hyperboxes.</param>
			/// <param name="gridOrigin">lowest point that is contained by the grid.</param>
			/// <param name="bounds">bounds object that is used for storing of hyperspace point that makes border of forbidden region.</param>
			/// <param name="hyperBox">chromosome tag that contains coordinates of hyperbox to which the chromosome belongs.</param>
			/// <param name="rank">chromosome tag that stores chromosome's rank.</param>
			/// <param name="cell">Chromosome tag that references cell of matrix which stores best chromosomes to which this chromosome belongs.</param>
			GaRDGARemoveOp(Population::GaPopulation& population,
				const GaFitnessHyperGrid& hyperGrid,
				Fitness::GaFitness& gridOrigin,
				GaFitnessGridBounds& bounds,
				const Population::GaChromosomeTagGetter<Common::Grid::GaHyperBox>& hyperBox,
				const Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> >& rank,
				const Population::GaChromosomeTagGetter<Population::GaBestChromosomesMatrix::GaCell*>& cell) : _population(population),
				_hyperGrid(hyperGrid),
				_gridOrigin(gridOrigin),
				_bounds(bounds),
				_hyperBox(hyperBox),
				_rank(rank),
				_cell(cell) { _half = population.GetNewChromosomes().GetCount() / 2; }

			/// <summary><c>operator ()</c> marks the offspring chromosome for removal if it is is in forbidden region of if it decreases population diffusion.</summary>
			/// <param name="chromosome">chromosome which should be checked.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{

				// get chromosome's parent and bounds of forbidden region
				Population::GaChromosomeStorage* parent = chromosome.GetParent();
				if( parent )
				{
					_hyperGrid.GetHyperBoxBounds( _hyperBox( parent ), _gridOrigin, _bounds );

					const Fitness::GaFitness& fitness = chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_RAW );

					// remove parent chromosome if child intorduces more diffusion to population and it is not in forbidden region of it has better rank
					if( ( index >= _half && _rank( parent ) > _rank( chromosome ) ) || ( index < _half && _cell( parent )->GetCount() > _cell( chromosome )->GetCount() && 
						!( _population.CompareFitness( fitness, _bounds.GetLowerBounds() ) > 0 && _population.CompareFitness( fitness, _bounds.GetUpperBounds() ) > 0 ) ) )
					{
						parent->GetFlags().SetFlags( Population::GaChromosomeStorage::GACF_REMOVE_CHROMOSOME );
						chromosome.SetParent( NULL );
					}
					// remove child chromosome if it causes drop in diffusion to population
					else
						chromosome.GetFlags().SetFlags( Population::GaChromosomeStorage::GACF_REMOVE_CHROMOSOME );
				}
			}

		};

		// Prepares population for replacement operation
		void GaRDGA::Prepare(Population::GaChromosomeGroup& input,
			Population::GaPopulation& population,
			const Population::GaReplacementParams& parameters,
			const Population::GaReplacementConfig& configuration,
			int branchCount) const
		{
			const GaRDGAParams& params = (const GaRDGAParams&)parameters;

			// add required chromosome tags
			population.GetChromosomeTagManager().AddTag( params.GetDominationListTagID(), Common::Data::GaSizableTagLifecycle<GaDominanceList>( branchCount ) );
			population.GetChromosomeTagManager().AddTag( params.GetDominationCountTagID(), Common::Data::GaTypedTagLifecycle<Common::Threading::GaAtomic<int> >() );
			population.GetChromosomeTagManager().AddTag( params.GetRankTagID(), Common::Data::GaTypedTagLifecycle<Common::Threading::GaAtomic<int> >() );
			population.GetChromosomeTagManager().AddTag( params.GetHyperBoxTagID(), Common::Data::GaSizableTagLifecycle<Common::Grid::GaHyperBox>() );
			population.GetChromosomeTagManager().AddTag( params.GetCellTagID(), Common::Data::GaTypedTagLifecycle<Population::GaBestChromosomesMatrix::GaCell*>() );
			population.GetChromosomeTagManager().AddTag( params.GetNextInFrontTagID(), Common::Data::GaTypedTagLifecycle<Population::GaChromosomeStorage*>() );

			// add required population tags
			population.GetTagManager().AddTag( params.GetBestChromosomesMatrixTagID(), Common::Data::GaTypedTagLifecycle<Population::GaBestChromosomesMatrix>() );
			population.GetTagManager().AddTag( params.GetBranchMatricesTagID(), Common::Data::GaTypedTagLifecycle<Population::GaBestChromosomesMatrix::GaPerBranch>() );

			// add population tag that stores current pareto front
			int index = population.GetTagManager().AddTag( params.GetUnrankedTagID(), Common::Data::GaTypedTagLifecycle<GaRDGAUnranked>() );
			population.GetTagByIndex<GaRDGAUnranked>( index ).SetNextInFrontID( params.GetNextInFrontTagID(), population.GetChromosomeTagManager() );

			// add population tag that stores fixed hypergird
			index = population.GetTagManager().AddTag( params.GetGridTagID(), Common::Data::GaTypedTagLifecycle<GaRDGAGrid>() );
			population.GetTagByIndex<GaRDGAGrid>( index ).SetAdaptiveGrid( &( (const GaRDGAConfig&)configuration ).GetAdaptiveGrid() );
		}

		// Removes data that was needed by the replacement operation
		void GaRDGA::Clear(Population::GaChromosomeGroup& input,
			Population::GaPopulation& population,
			const Population::GaReplacementParams& parameters,
			const Population::GaReplacementConfig& configuration,
			int branchCount) const
		{
			const GaRDGAParams& params = (const GaRDGAParams&)parameters;

			// remove used chromosome tags
			population.GetChromosomeTagManager().RemoveTag( params.GetDominationListTagID() );
			population.GetChromosomeTagManager().RemoveTag( params.GetDominationCountTagID() );
			population.GetChromosomeTagManager().RemoveTag( params.GetRankTagID() );
			population.GetChromosomeTagManager().RemoveTag( params.GetHyperBoxTagID() );
			population.GetChromosomeTagManager().RemoveTag( params.GetCellTagID() );
			population.GetChromosomeTagManager().RemoveTag( params.GetNextInFrontTagID() );

			// remove required population tags
			population.GetTagManager().RemoveTag( params.GetBestChromosomesMatrixTagID() );
			population.GetTagManager().RemoveTag( params.GetUnrankedTagID() );
			population.GetTagManager().RemoveTag( params.GetGridTagID() );
			population.GetTagManager().RemoveTag( params.GetBranchMatricesTagID() );
		}

		// Updates population for replacement operation
		void GaRDGA::Update(Population::GaChromosomeGroup& input,
			Population::GaPopulation& population,
			const Population::GaReplacementParams& parameters,
			const Population::GaReplacementConfig& configuration,
			int branchCount) const
		{
			// update chromosome tags
			population.GetChromosomeTagManager().UpdateTag( ( (const GaRDGAParams&)parameters ).GetDominationListTagID(), GaDominanceListUpdate( branchCount ) );

			// update population tags
			population.GetTagByID<Population::GaBestChromosomesMatrix::GaPerBranch>( ( (const GaRDGAParams&)parameters ).GetBranchMatricesTagID() ).SetSize( branchCount );
		}

		// Execute replacement operation
		void GaRDGA::Exec(Population::GaChromosomeGroup& input,
			Population::GaPopulation& population,
			const GaRDGAParams& parameters,
			const GaRDGAConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			Population::GaOperationTime timer( population, Population::GADV_SCALING_TIME );

			int branchID = branch->GetFilteredID();
			int branchCount = branch->GetBarrierCount();

			const GaFitnessAdaptiveGrid& adaptiveGrid = configuration.GetAdaptiveGrid();

			// population tags
			GaRDGAUnranked& unranked = population.GetTagByID<GaRDGAUnranked>( parameters.GetUnrankedTagID() );
			GaRDGAGrid& grid = population.GetTagByID<GaRDGAGrid>( parameters.GetGridTagID() );
			Population::GaBestChromosomesMatrix& bestChromosomes = population.GetTagByID<Population::GaBestChromosomesMatrix>( parameters.GetBestChromosomesMatrixTagID() );
			Population::GaBestChromosomesMatrix::GaPerBranch& perBranchBest =
				population.GetTagByID<Population::GaBestChromosomesMatrix::GaPerBranch>( parameters.GetBranchMatricesTagID() );

			Population::GaBestChromosomesMatrix::GaItemMatrix& localBest = perBranchBest[ branchID ];

			// clear best ranked chromosomes identified in previous generation
			bestChromosomes.Clear();

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
			{
				// update size of chromosome tag that stores coordinates of hyperbox to which the chromosome belongs
				population.GetChromosomeTagManager().UpdateTag( parameters.GetHyperBoxTagID(),
					Common::Grid::GaHyperBoxUpdate( ( (const Fitness::Representation::GaMVFitnessParams&)population.GetFitnessParams() ).GetValueCount() ) );

				// update buffer sizes when population size is changed
				int populationSize = population.GetPopulationParams().GetPopulationSize();
				grid.SetSize( branchCount, population );

				// insert offspring chromosomes
				input.Trim( parameters.GetReplacementSize() );
				population.Insert( input.GetChromosomes(), input.GetCount() );

				// store number of unranked chromosomes currently in the population
				unranked.SetCount( population.GetCount() );
			}

			// chromosome tags
			Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > getDomCount( parameters.GetDominationCountTagID(), population.GetChromosomeTagManager() );
			Population::GaChromosomeTagGetter<GaDominanceList> getDomList( parameters.GetDominationListTagID(), population.GetChromosomeTagManager() );
			Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > getRank( parameters.GetRankTagID(), population.GetChromosomeTagManager() );
			Population::GaChromosomeTagGetter<Common::Grid::GaHyperBox> getHyperBox( parameters.GetHyperBoxTagID(), population.GetChromosomeTagManager() );
			Population::GaChromosomeTagGetter<Population::GaBestChromosomesMatrix::GaCell*> getCell( parameters.GetCellTagID(), population.GetChromosomeTagManager() );

			Common::Workflows::GaParallelExec2<Population::GaPopulation, Population::GaChromosomeStorage> domWorkDist( *branch, population );
			Common::Workflows::GaParallelExec1<Population::GaPopulation, Population::GaChromosomeStorage> popWorkDist( *branch, population );
			Common::Workflows::GaParallelExec1<Population::GaChromosomeGroup, Population::GaChromosomeStorage> newWorkDist( *branch, population.GetNewChromosomes() );

			GaFitnessGridBounds& gridBounds = grid[ branchID ];

			// restart previously identified bounds
			gridBounds.SetLowerBounds( population[ 0 ].GetFitness( Population::GaChromosomeStorage::GAFT_RAW ) );
			gridBounds.SetUpperBounds( gridBounds.GetLowerBounds() );

			// determine size of fixed hypergird and identify nondominated chromosomes
			domWorkDist.Execute( GaRDGAGridOp( adaptiveGrid, gridBounds, getRank ), GaDominanceOp( population, getDomCount, getDomList, branchID ), false );

			// update or create fixed hypergird according to determined size
			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
				grid.Update();

			// update size of matrix that will store the best ranked chromosome in each hyperbox
			localBest.Update( adaptiveGrid.GetGridSize() );
			if( branchID == 0 )
				bestChromosomes.GetCells().Update( adaptiveGrid.GetGridSize() );

			// identify hyperboxes and chromosomes that belongs to the first pareto front
			popWorkDist.Execute( GaRDGAFrontOp( grid.GetHyperGrid(), getHyperBox, getCell, grid[ branchCount ].GetLowerBounds(), unranked, getDomCount, getRank, bestChromosomes ), true );

			// calculate rank for dominated chromosomes
			Population::GaChromosomeStorage* nondominated;
			while( ( nondominated = unranked.Dequeue() ) != NULL )
			{
				// update rank for chromosomes dominated by the current chromosome
				GaDominanceList& dominatedList = getDomList( *nondominated );
				for( int i = branchCount - 1; i >= 0; i-- )
				{
					for( Common::Data::GaListNode<Population::GaChromosomeStorage*>* it = dominatedList[ i ].GetHead(); it; it = it->GetNext() )
					{
						Population::GaChromosomeStorage& dominated = *it->GetValue();

						// update dominated chromosome's rank
						getRank( dominated ) += getRank( *nondominated );

						// no other chromosomes dominates it chromosome
						if( --getDomCount( dominated ) == 0 )
						{
							// add chromosome to list of current front and performe final update to its rank
							++getRank( dominated );
							unranked.Queue( &dominated );
						}
					}

					// prepare tag for next generation
					dominatedList[ i ].Clear();
				}
			}

			// selects children chromosomes in forbidden regions and those that cause drop in population's diffusion
			newWorkDist.Execute( GaRDGARemoveOp( population, grid.GetHyperGrid(), grid[ branchCount ].GetLowerBounds(), gridBounds, getHyperBox, getRank, getCell ), false );

			// purge parents that were replaced or offsprings that were rejected
			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
				population.Remove();

			// the first pass of detecting the best ranked chromosome in each hyperbox
			popWorkDist.Update();
			popWorkDist.Execute( GaRDGAMatrixOp( localBest.GetMatrix(), getRank, getHyperBox ), true );

			// the second pass of detecting the best ranked chromosome in each hyperbox and storing them to the matrix
			bestChromosomes.CollectBest( perBranchBest, getRank );

			// update operation time statistics
			timer.UpdateStatistics();
		}

	} // RDGA
} // Multiobjective

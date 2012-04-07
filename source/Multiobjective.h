
/*! \file Multiobjective.h
	\brief This file declares basic classes and datatypes required for multiobjective optimization.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_MULTIOBJECTIVE_H__
#define __GA_MULTIOBJECTIVE_H__

#include "Grid.h"
#include "Tags.h"
#include "Population.h"

/// <summary>Contains classes and datatypes that implement multiobjective optimization.</summary>
namespace Multiobjective
{

	/// <summary>Datatype used for storing partial chromosome list in chromosome's tag.</summary>
	typedef Common::Data::GaPartialList<Population::GaChromosomeStorage*>::GaType GaDominanceList;

	/// <summary>Datatype used for updating number of partitions for chromosome's tag that stores partial list of chromosomes.</summary>
	typedef Common::Data::GaPartialListSizeUpdate<Population::GaChromosomeStorage*>::GaType GaDominanceListUpdate;

	/// <summary>Hypergrid divides fitness space.</summary>
	typedef Common::Grid::GaHyperGridBase<Fitness::GaFitness> GaFitnessHyperGrid;

	/// <summary>Adaptive hypergrid divides fitness space.</summary>
	typedef Common::Grid::GaAdaptiveGridBase<Fitness::GaFitness> GaFitnessAdaptiveGrid;

	/// <summary>Bound of hypergrid that divides fitness space.</summary>
	typedef Common::Grid::GaHyperGridBounds<Fitness::GaFitness> GaFitnessGridBounds;

	/// <summary><c>GaFitnessCoordiantesGetter</c> class provides access to values stored in fitness object that can be used as coordinates of hyperspace.</summary>
	/// <param name="COORDINATE">type of values that are stored in fitness object.</param>
	template<typename COORDINATE>
	class GaFitnessCoordiantesGetter
	{

	public:
		
		/// <summary>Type of values that are stored in fitness object.</summary>
		typedef COORDINATE GaCoordinate;

		/// <summary>Type of collection that stores values of fitness object.</summary>
		typedef Common::Data::GaSingleDimensionArray<GaCoordinate> GaCoordinates;

		/// <summary><c>operator ()</c> operator returns collection of values stored in fitness object.
		///
		/// This operator is thread-safe.</summary>
		/// <param name="point">fitness object whose values is queried.</param>
		/// <returns>Method returns reference to collection of values stored in fitness object.</returns>
		inline GaCoordinates& operator ()(Fitness::GaFitness& point) const { return ( (Fitness::Representation::GaMVFitness<GaCoordinate>&)point ).GetTypedValues(); }

		/// <summary><c>operator ()</c> operator returns collection of values stored in fitness object.
		///
		/// This operator is thread-safe.</summary>
		/// <param name="point">fitness object whose values is queried.</param>
		/// <returns>Method returns reference to collection of values stored in fitness object.</returns>
		inline const GaCoordinates& operator ()(const Fitness::GaFitness& point) const { return ( (const Fitness::Representation::GaMVFitness<GaCoordinate>&)point ).GetTypedValues(); }

	};

	/// <summary>Criteria that sorts chromosomes based on hyperboxes in which they are located.</summary>
	typedef Common::Sorting::GaSortCriteriaConverter<Population::GaChromosomeStorage*, Common::Grid::GaHyperBoxSortCriteria, Population::GaChromosomeTagGetter<Common::Grid::GaHyperBox> >
		GaHypeBoxChromosomeSortCriteria;

	/// <summary><c>GaDominanceOp</c> class represents operation that compares and determins dominant chromosome and updates dominance count and dominated chromosome lists.</summary>
	class GaDominanceOp
	{

	private:

		/// <summary>Population over which the dominance operation is performed.</summary>
		Population::GaPopulation& _population;

		/// <summary>Chromosome tag that contains count of dominant chromosome.</summary>
		Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > _domCount;

		/// <summary>Chromosome tag that contains partial list of dominated chromosome.</summary>
		Population::GaChromosomeTagGetter<GaDominanceList> _domList;

		/// <summary>ID of the brach that executes operation.</summary>
		int _branchID;

	public:

		/// <summary>Initalizes operation.</summary>
		/// <param name="population">population over which the dominance operation is performed.</param>
		/// <param name="domCount">chromosome tag that contains partial count of dominant chromosome.</param>
		/// <param name="domList">chromosome tag that contains partial list of dominated chromosome.</param>
		/// <param name="branchID">ID of the brach that executes operation.</param>
		GaDominanceOp(Population::GaPopulation& population,
			const Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> >& domCount,
			const Population::GaChromosomeTagGetter<GaDominanceList>& domList,
			int branchID) : _population(population),
			_domCount(domCount),
			_domList(domList),
			_branchID(branchID) { }

		/// <summary>Dummy operator to satisfy interface.</summary>
		inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
			int index) { }

		/// <summary><c>operator ()</c> determins dominant chromosome and update dominance list and count of chromosomes.</summary>
		/// <param name="chromosome1">the first chromosome.</param>
		/// <param name="chromosome2">the second chromosome.</param>
		/// <param name="index1">index of the the first chromosome.</param>
		/// <param name="index2">index of the the second chromosome.</param>
		inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome1,
			Population::GaChromosomeStorage& chromosome2,
			int index1,
			int index2)
		{
			// get dominant chromosome
			int res = _population.CompareFitness( chromosome1, chromosome2 );

			// is the first chromosome dominant?
			if( res < 0 )
			{
				// increment dominance count and update list of dominated chromosomes
				++_domCount( chromosome2 );
				_domList( chromosome1 )[ _branchID ].InsertTail( &chromosome2 );
			}
			// is the second chromosome dominant?
			else if( res > 0 )
			{
				// increment dominance count and update list of dominated chromosomes
				++_domCount( chromosome1 );
				_domList( chromosome2 )[ _branchID ].InsertTail( &chromosome1 );
			}
		}

	};

	/// <summary><c>GaHyperBoxOp</c> class represents operation which determins coordinates of hyperbox in which the chromosome is located.</summary>
	class GaHyperBoxOp
	{

	private:

		/// <summary>Hypergrid that splits fitness hyperspace.</summary>
		const GaFitnessHyperGrid& _hyperGrid;

		/// <summary>Chromosome tag that contains coordinates of hyperbox to which the chromosome belongs.</summary>
		Population::GaChromosomeTagGetter<Common::Grid::GaHyperBox> _hyperBox;

	public:

		/// <summary>Initalizes operation.</summary>
		/// <param name="hyperGrid">hypergrid that splits fitness hyperspace.</param>
		/// <param name="hyperBox">chromosome tag that contains coordinates of hyperbox to which the chromosome belongs.</param>
		GaHyperBoxOp(const GaFitnessHyperGrid& hyperGrid,
			const Population::GaChromosomeTagGetter<Common::Grid::GaHyperBox>& hyperBox) : _hyperGrid(hyperGrid),
			_hyperBox(hyperBox) { }

		/// <summary><c>operator ()</c> operator determins coordinates of hyperbox to which chromosome belongs and stores results in chromosome's tag.</summary>
		/// <param name="chromosome">chromosome whose location should be determined.</param>
		/// <param name="index">index of the chromosomes.</param>
		inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
			int index) { _hyperGrid.GetHyperBox( chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_RAW ), _hyperBox( chromosome ) ); }

	};

	/// <summary><c>GaGridDensityOp</c> function calculate density of provided collection by counting chromosomes in hyperboxes.</summary>
	/// <param name="COLLECTION">type of collection whose density is calculated.</param>
	/// <param name="collection">collection whose density is calculated.</param>
	/// <param name="infoBuffer">buffer that stores hyperbox info objects.</param>
	/// <param name="hyperBox">chromosome tag that contains coordinates of hyperbox to which the chromosome belongs.</param>
	/// <param name="hyperBoxInfo">chromosome tag that references info object of the hyperbox to which the chromosome belongs.</param>
	template<typename COLLECTION>
	void GACALL GaGridDensityOp(COLLECTION& collection,
		Common::Grid::GaHyperBoxInfoBuffer& infoBuffer,
		const Population::GaChromosomeTagGetter<Common::Grid::GaHyperBox>& hyperBox,
		Population::GaChromosomeTagGetter<Common::Grid::GaHyperBoxInfo*>& hyperBoxInfo)
	{
		// sort chromosomes in the collection according to coordinates of hyperboxes to which they belongs.
		collection.Sort( GaHypeBoxChromosomeSortCriteria( hyperBox ) );

		Common::Grid::GaHyperBoxInfo* current = NULL;
		for( int i = collection.GetCount() - 1; i >= 0; i-- )
		{
			const Common::Grid::GaHyperBox& box = hyperBox( collection[ i ] );

			// current chromosome belongs to new hyperbox?
			if( !current || box != current->GetHyperBox() )
				// add info object for new hyperbox
				current = infoBuffer.Add( i, box );
			else
				// chromosome belongs to existing hyperbox
				current->MoveStart();

			// store infor object to chromosome
			hyperBoxInfo( collection[ i ] ) = current;
		}
	}

	/// <summary><c>GaCrowding</c> stores index of most crowded hyperbox info object.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	struct GaCrowding
	{

		/// <summary>Index of hyperbox info object.</summary>
		int _index;

		/// <summary>Count of chromosomes in the hyperbox.</summary>
		int _count;

		/// <summary>Initailizes storage.</summary>
		GaCrowding() { Clear(); }

		/// <summary><c>Clear</c> method restarts stored crowding information.
		///
		/// This method is not thread-safe.</summary>
		inline void GACALL Clear() { _index = _count = -1; }

		/// <summary><c>Store</c> method stores index of hyperbox info object if it is more crowded (coin of chromosomes is greater) then the one that is already stored.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="index">index of hyperbox info object.</param>
		/// <param name="count">count of chromosomes in hyperbox.</param>
		inline void GACALL Store(int index,
			int count)
		{
			// is new hyperbox more crowded then previous
			if( count > _count )
			{
				// store index
				_count = count;
				_index = index;
			}
		}

	};

	/// <summary><c>GaCrowdingStorage</c> class provides separate hyperbox crowding information storage for each workflow branch that execute operation.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaCrowdingStorage
	{

	private:

		/// <summary>Stores most crowded hyperboxes that were found by each branch.</summary>
		Common::Data::GaSingleDimensionArray<GaCrowding> _maxCrowding;

		/// <summary>Index of info object that represents most crowded hyperbox for all the branches.</summary>
		int _gloablMaxCrowding;

	public:

		/// <summary>Initializes empty storage.</summary>
		/// <param name="size">number of branches that will use storage.</param>
		GaCrowdingStorage(int size = 0) : _maxCrowding(size),
			_gloablMaxCrowding(-1) { }

		/// <summary><c>RequiresUpdate</c> check whether the branch should rescan all info object to find new most crowded hyperbox.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="branchID">ID of the branch that should perform check.</param>
		/// <returns>Method returns <c>true</c> if branch should search for new most crowded hyperbox.</returns>
		inline bool GACALL RequiresUpdate(int branchID) const { return _maxCrowding[ branchID ]._index == _gloablMaxCrowding; }

		/// <summary><c>CalculateGlobalMaxCrowding</c> method finds the most crowded hyperbox among the hyperboxes that each branch have selected.
		///
		/// This method is not thread-safe.</summary>
		/// <returns>Method returns index of info object that repesents the most crowded hyperbox.</returns>
		int GACALL CalculateGlobalMaxCrowding()
		{
			// assume that the first branch have found most crowded
			int max = _maxCrowding[ 0 ]._count;
			_gloablMaxCrowding = _maxCrowding[ 0 ]._index;

			// check all the other branches
			for( int i = _maxCrowding.GetSize() - 1; i > 0; i-- )
			{
				// did current branch find more crowded branch?
				if( _maxCrowding[ i ]._count > max )
				{
					// yes - select this branch
					max = _maxCrowding[ i ]._count;
					_gloablMaxCrowding = _maxCrowding[ i ]._index;
				}
			}

			return _gloablMaxCrowding;
		}

		/// <summary><c>RestartGlobalMaxCrowding</c> method clears previously identified nostcrowded hyperbox.
		///
		/// This method is not thread-safe.</summary>
		inline void GACALL RestartGlobalMaxCrowding() { _gloablMaxCrowding = -1; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns index info object that represents most crowded hyperbox for all the branches.</returns>
		inline int GACALL GetGlobalMaxCrowding() const { return _gloablMaxCrowding; }

		/// <summary><c>SetSize</c> method sets number of branches that storage needs to accommodate.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="size">number of branches that will use storage.</param>
		inline void GACALL SetSize(int size)
		{
			_maxCrowding.SetSize( size );
			_gloablMaxCrowding = -1;
		}

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns number of branches that storage can accommodate.</returns>
		inline int GACALL GetSize() const { return _maxCrowding.GetSize(); }

		/// <summary><c>operator []</c> provides access to object that stores crowding information for each branch.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="branchID">ID of branch whose storage should to be accessed.</param>
		/// <returns>Method returns reference to object that stores crowding information </returns>
		inline GaCrowding& GACALL operator [](int branchID) { return _maxCrowding[ branchID ]; }

		/// <summary><c>operator []</c> provides access to object that stores crowding information for each branch.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="branchID">ID of branch whose storage should to be accessed.</param>
		/// <returns>Method returns reference to object that stores crowding information </returns>
		inline const GaCrowding& GACALL operator [](int branchID) const { return _maxCrowding[ branchID ]; }

	};

	/// <summary><c>GaCrowdingOp</c> class represents operation that searches for the most crowded hyperbox.</summary>
	class GaCrowdingOp
	{

	private:

		/// <summary>Object that stores crowding information for the workflow branch.</summary>
		GaCrowding& _maxCrowd;

	public:

		/// <summary>Initializes operation.</summary>
		/// <param name="maxCrowd">object that stores crowding information for the workflow branch.</param>
		GaCrowdingOp(GaCrowding& maxCrowd) : _maxCrowd(maxCrowd) { }

		/// <summary><c>operator ()</c> check whether the hyperbox is more crowded then the previous and stores index of info object.</summary>
		/// <param name="hyperBoxInfo">hyperbox info object that should be checked.</param>
		/// <param name="index">indef of info object.</param>
		inline void GACALL operator ()(Common::Grid::GaHyperBoxInfo& hyperBoxInfo,
			int index) { _maxCrowd.Store( index, hyperBoxInfo.GetCount() ); }

	};

} // Multiobjective

#endif // __GA_MULTIOBJECTIVE_H__

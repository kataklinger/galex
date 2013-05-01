
/*! \file Grid.h
    \brief This file declares classes and datatypes that represent and handle grids.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_GRID_H__
#define __GA_GRID_H__

#include "Sorting.h"
#include "Tags.h"

namespace Common
{
	/// <summary>Contains interfaces, classes and datatypes that handle grids.</summary>
	namespace Grid
	{

		/// <summary><c>GaHyperBox</c> stores coordinates of hyperbox in hypergrid.</summary>
		typedef Data::GaSingleDimensionArray<int> GaHyperBox;

		/// <summary>Datatype used for updating number of hyperbox coordinates.</summary>
		typedef Common::Data::GaSizableTagUpdate<GaHyperBox, Common::Data::GaTagIgnoreMerge<Common::Grid::GaHyperBox> > GaHyperBoxUpdate;

		/// <summary><c>GaHyperBoxSortCriteria</c> class represents sorting criteria used to sort collections of hyperboxes based on their coordinates.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this object are stateless all public method are thread-safe.</summary>
		class GaHyperBoxSortCriteria : public Sorting::GaSortingCriteria<GaHyperBox>
		{

		public:

			/// <summary><c>operator ()</c> compares coordinates of two hyperboxes.
			///
			/// This operator is thread-safe.</summary>
			/// <param name="object1">the first hyperbox.</param>
			/// <param name="object2">the second hyperbox.</param>
			/// <returns>Method returns:
			/// <br/>a. -1 if the first hyperbox is located lower than the second in coordinate system,
			/// <br/>b.  1 if the second hyperbox is located lower than the first in coordinate system,
			/// <br/>c.  0 if both hyperboxes are located at same position.</returns>
			GAL_API
			int GACALL operator ()(GaConstType& object1,
				GaConstType& object2) const;

		};

		/// <summary><c>GaHyperBoxInfo</c> class stores information about items of a colection splited by hypergrid that belongs to a hyperbox.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaHyperBoxInfo
		{

		private:

			/// <summary>The first index in the collection that can contain item that belong to hyperbox.</summary>
			int _start;

			/// <summary>The last index in the collection that can contain item that belong to hyperbox.</summary>
			int _end;

			/// <summary>Number of items currently in the hyperbox.</summary>
			int _count;

			/// <summary>Coordinates in the hyperbox.</summary>
			GaHyperBox _hyperBox;

		public:

			/// <summary>This constructor initializes empty info object.</summary>
			GaHyperBoxInfo() : _start(-1),
				_end(-1),
				_count(-1) { }

			/// <summary><c>Clear</c> method restarts information stored in the object.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL Clear() { _start = _end = _count = -1; }

			/// <summary><c>Init</c> method sets the first index in the collection that can contain item that belong to hyperbox.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="start">index in the collection.</param>
			/// <param name="hyperBox">coordinates of hyperbox.</param>
			GAL_API
			void GACALL Init(int start,
				const GaHyperBox& hyperBox);

			/// <summary><c>SetStart</c> method sets the first index in the collection that can contain item that belong to hyperbox.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="start">index in the collection.</param>
			inline void GACALL SetStart(int start)
			{
				_start = start;
				_count = _end - start + 1;
			}

			/// <summary><c>MoveStart</c> method moves the first index in the collection that can contain item that belong to hyperbox to one position up.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL MoveStart()
			{
				_start--;
				_count++;
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns the first index in the collection that can contain item that belong to hyperbox.</returns>
			inline int GACALL GetStart() const { return _start; }

			/// <summary><c>SetEnd</c> method sets the last index in the collection that can contain item that belong to hyperbox.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="end">index in the collection.</param>
			inline void GACALL SetEnd(int end)
			{
				_end = end;
				_count = end - _start + 1;
			}

			/// <summary><c>MoveEnd</c> method moves the last index in the collection that can contain item that belong to hyperbox to one position down.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL MoveEnd()
			{
				_end++;
				_count++;
			}

			/// <summary><c>RemoveItem</c> decrements count of items in the hyperbox.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL RemoveItem() { _count--; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns the last index in the collection that can contain item that belong to hyperbox.</returns>
			inline int GACALL GetEnd() const { return _end; }

			/// <summary><c>IsNext</c> method checks if the two hyperboxes are next to each other in items collection.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="info"></param>
			/// <returns>Method returns <c>true</c> if this hyperbox goes right before specified hyperbox.</returns>
			inline bool GACALL IsNext(GaHyperBoxInfo& info) { return _end == info._start; }

			/// <summary><c>IsPrevious</c> method checks if the two hyperboxes are next to each other in items collection.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="info"></param>
			/// <returns>Method returns <c>true</c> if this hyperbox goes right after specified hyperbox.</returns>
			inline bool GACALL IsPrevious(GaHyperBoxInfo& info) { return _start == info._end; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of items currently in the hyperbox.</returns>
			inline int GACALL GetCount() const { return _count; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns coordinates in the hyperbox.</returns>
			inline GaHyperBox& GetHyperBox() { return _hyperBox; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns coordinates in the hyperbox.</returns>
			inline const GaHyperBox& GetHyperBox() const { return _hyperBox; }

		};

		/// <summary><c>GaHyperBoxInfoBuffer</c> class stores and handles hyperbox info objects used when collections are splitted using hypergrids.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaHyperBoxInfoBuffer
		{

		private:

			/// <summary>Hyperbox info objects.</summary>
			Data::GaSingleDimensionArray<GaHyperBoxInfo> _buffer;

			/// <summary>Number of info objects in the buffer.</summary>
			Threading::GaAtomic<int> _count;

		public:

			/// <summary>This constructor initializes empty buffer.</summary>
			GaHyperBoxInfoBuffer() { }

			/// <summary><c>Clear</c> method removes all info objects from the buffer.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL Clear() { _count = 0; }

			/// <summary><c>Add</c> method creates new entry in the buffer for specified hyperbox.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="startIndex">index of the first item </param>
			/// <param name="hyperBox">coordinates of the hyperbox.</param>
			/// <returns>Method returns pointer to infor object which has been added to the buffer to store hyperbox information.</returns>
			GAL_API
			GaHyperBoxInfo* GACALL Add(int startIndex,
				const GaHyperBox& hyperBox);

			/// <summary><c>SetSize</c> method sets the size of buffer.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="size">new size of the buffer.</param>
			/// <param name="branchCount">number of threads/workflow branches that will concurentlly access the buffer.</param>
			GAL_API
			void GACALL SetSize(int size,
				int branchCount);

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of info objects that buffer can store.</returns>
			inline int GACALL GetSize() const { return _buffer.GetSize(); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of info object currently in the buffer.</returns>
			inline int GACALL GetCount() const { return _count; }

			/// <summary><c>operator []</c> queries info object at specified index in the buffer.
			///
			/// This method is not thread-safe</summary>
			/// <param name="index">index of queried object.</param>
			/// <returns>Method returns reference to queried info object.</returns>
			inline GaHyperBoxInfo& GACALL operator [](int index) { return _buffer[ index ]; }

			/// <summary><c>operator []</c> queries info object at specified index in the buffer.
			///
			/// This operator is not thread-safe</summary>
			/// <param name="index">index of queried object.</param>
			/// <returns>Method returns reference to queried info object.</returns>
			inline const GaHyperBoxInfo& GACALL operator [](int index) const { return _buffer[ index ]; }

		};

		/// <summary><c>GaHyperBoxNeighbour</c> class is used for iterationg through hypergrid.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaHyperBoxNeighbour
		{

		private:

			/// <summary>Coordinates of current hyperbox of iteration.</summary>
			Common::Grid::GaHyperBox _coords;

			/// <summary>Max value that coordinates can have.</summary>
			Common::Data::GaSingleDimensionArray<int> _limits;

			/// <summary>Indicates whether the required distance is out of range defined by the coordinates limit.</summary>
			Common::Data::GaSingleDimensionArray<bool> _skip;

			/// <summary>Index of current dimension that is beign visited.</summary>
			int _index;

		public:

			/// <summary>This constructor initializes empty iterator.</summary>
			GaHyperBoxNeighbour() : _index(0) { }

			/// <summary><c>Begin</c> method restarts iterator to the fist hyperbox that shoul be visited.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="center">central hyperbox of iteration.</param>
			/// <param name="limits">max value that coordinates can have.</param>
			/// <param name="level">distance (in number of hyperboxes) of hyperboxes that should be visited from central hyberbox.</param>
			GAL_API
			void GACALL Begin(const Common::Grid::GaHyperBox& center,
				const int* limits,
				int level);

			/// <summary><c>Next</c> moves iterator to next hyperbox.
			///
			/// This method is not thread-safe.</summary>
			/// <returns>Method returns <c>false</c> if the last hyperbox was visited and iteration should stop.</returns>
			GAL_API
			bool GACALL Next();

			/// <summary><c>SetCoordinatesCount</c> method sets number of dimensions that hypergrid has.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="count">number of dimensions.</param>
			GAL_API
			void GACALL SetCoordinatesCount(int count);

			/// <summary><c>SetCoordinate</c> sets single coordinate of the cetral hyperbox.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="coord">coordinate that should be set.</param>
			/// <param name="value">new value for coordinate.</param>
			inline void GACALL SetCoordinate(int coord,
				int value) { _coords[ coord ] = value; }

			/// <summary><c>GetCoordinate</c> method queries single coordinate of current hyperbox.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="coord">coordinate whose value is queried.</param>
			/// <returns>Method returns queried coordinate.</returns>
			inline int GACALL GetCoordinate(int coord) const { return _coords[ coord ]; }

			/// <summary><c>SetCoordinates</c> method sets coordinates of current hyperbox.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="coords">new coordinates.</param>
			inline void GACALL SetCoordinates(const Common::Grid::GaHyperBox& coords) { _coords = coords; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns coordinates of contral hyperbox.</returns>
			inline Common::Grid::GaHyperBox& GACALL GetCoordinates() { return _coords; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns coordinates of contral hyperbox.</returns>
			inline const Common::Grid::GaHyperBox& GACALL GetCoordinates() const { return _coords; }

			/// <summary><c>operator []</c> provides access to a single coordinate of current hyperbox.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="index">required coordinate.</param>
			/// <returns>Method returns reference to varable that stores value of specified coordinate.</returns>
			inline int& GACALL operator [](int index) { return _coords[ index ]; }

			/// <summary><c>operator []</c> provides access to a single coordinate of current hyperbox.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="index">required coordinate.</param>
			/// <returns>Method returns reference to varable that stores value of specified coordinate.</returns>
			inline int GACALL operator [](int index) const { return _coords[ index ]; }

			/// <summary><c>operator =</c> moves iterator to specified hyperbox.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="rhs">coordinates to which the iterator should be moved.</param>
			/// <returns>Method returns reference to <c>this</c> object.</returns>
			inline GaHyperBoxNeighbour& GACALL operator =(const Common::Grid::GaHyperBox& rhs) { _coords = rhs; }

		private:

			/// <summary><c>Find</c> method finds the hyperbox from which the iteration should start or continue and sets iterator to that position.</summary>
			/// <returns>Method returns <c>false</c> if iteration cannot start or continue becaus it reached end or iteration distance from central hyperbox is out of range.</returns>
			GAL_API
			bool GACALL Find();

		};

		/// <summary><c>GaHyperGridBounds</c> class stores and manages bounds of hypergid as points in hyperspace.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <typeparam name="POINT">type of data that represents single point in hyperspace.</typeparam>
		template<typename POINT>
		class GaHyperGridBounds
		{

		public:

			/// <summary>Datatype that represents single point in hyperspace.</summary>
			typedef POINT GaPoint;

		private:

			/// <summary>The lowest point of hypergrid in hyperspace.</summary>
			Memory::GaAutoPtr<GaPoint> _lowerBounds;

			/// <summary>The highest point of hypergrid in hyperspace.</summary>
			Memory::GaAutoPtr<GaPoint> _upperBounds;

		public:

			/// <summary>This constructor initializes empty hypergrid bounds.</summary>
			GaHyperGridBounds() { }

			/// <summary>This constructor crates copy of hypergrid bounds.</summary>
			/// <param name="rhs">bounds which should be copied.</param>
			GaHyperGridBounds(const GaHyperGridBounds& rhs)
			{
				if( !rhs._lowerBounds.IsNull() )
				{
					_lowerBounds = rhs._lowerBounds->Clone();
					_upperBounds = rhs._upperBounds->Clone();
				}
			}

			/// <summary><c>SetBoundsObjects</c> method sets point object that will be used for storing hypergrid bounds in hyperspace.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="object">prototype of point object.</param>
			inline void GACALL SetBoundsObjects(const GaPoint& object)
			{
				_lowerBounds = object.Clone();
				_upperBounds = object.Clone();
			}

			inline void GACALL SetLowerBounds(const GaPoint& bound) { *_lowerBounds = bound; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns the lowest point of hypergrid in hyperspace.</returns>
			inline GaPoint& GACALL GetLowerBounds() { return *_lowerBounds; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns the lowest point of hypergrid in hyperspace.</returns>
			inline const GaPoint& GACALL GetLowerBounds() const { return *_lowerBounds; }

			inline void GACALL SetUpperBounds(const GaPoint& bound) { *_upperBounds = bound; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns the highest point of hypergrid in hyperspace.</returns>
			inline GaPoint& GACALL GetUpperBounds() { return *_upperBounds; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns the highest point of hypergrid in hyperspace.</returns>
			inline const GaPoint& GACALL GetUpperBounds() const { return *_upperBounds; }

		};

		/// <summary><c>GaHyperGridBase</c> template is interface for hypergrids that can divide hyperspace of specified type into hyperboxes of fixed size.</summary>
		/// <typeparam name="POINT">type of data that represents single point in hyperspace.</typeparam>
		template<typename POINT>
		class GaHyperGridBase
		{

		public:

			/// <summary>Datatype that represents single point in hyperspace.</summary>
			typedef POINT GaPoint;

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaHyperGridBase() { }

			/// <summary><c>Clone</c> method creates copy of grid object. Caller is responsible for allocated memory.</summary>
			/// <returns>Method returns pointer to newly created object.</returns>
			virtual GaHyperGridBase<GaPoint>* GACALL Clone() const = 0;

			/// <summary><c>GetHyperBox</c> method calculates coordinates of hyperbox to which the specified point belongs.</summary>
			/// <param name="point">point in hyperspace.</param>
			/// <param name="hyperBox">object that will stores coordinates of hyperbox.</param>
			virtual void GACALL GetHyperBox(const GaPoint& point,
				GaHyperBox& hyperBox) const = 0;

			/// <summary><c>GetHyperBox</c> method calculates coordinates of hyperbox to which the specified point belongs.</summary>
			/// <param name="point">point in hyperspace.</param>
			/// <param name="origin">lowest point that is contained by the grid.</param>
			/// <param name="hyperBox">object that will stores coordinates of hyperbox.</param>
			virtual void GACALL GetHyperBox(const GaPoint& point,
				const GaPoint& origin,
				GaHyperBox& hyperBox) const = 0;

			/// <summary><c>GetHyperBoxBounds</c> method calculates bounds of hypergrid.</summary>
			/// <param name="bounds">reference that object that will store calculated hypergrid bounds.</param>
			/// <param name="origin">lowest point that is contained by the grid.</param>
			/// <param name="hyperBox">coordinates of hyperbox that contains lowest point possible point in hyperspace.</param>
			virtual void GACALL GetHyperBoxBounds(const GaHyperBox& hyperBox,
				const GaPoint& origin,
				GaHyperGridBounds<GaPoint>& bounds) const = 0;

		};

		/// <summary><c>GaHyperGrid</c> template manages hypergrids that can divide hyperspace of specified type into hyperboxes of fixed size.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		/// <typeparam name="POINT">type of data that represents single point in hyperspace.</typeparam>
		/// <typeparam name="COORDINATE">type of single coordinate of a point in hyperspace.</typeparam>
		/// <typeparam name="COORDINATES_GETTER">functor that extracts coordinates of point object.</typeparam>
		template<typename POINT,
			typename COORDINATE,
			typename COORDINATES_GETTER>
		class GaHyperGrid : public GaHyperGridBase<POINT>
		{

		public:

			/// <summary>Type of single coordinate of a point in hyperspace.</summary>
			typedef COORDINATE GaCoordinate;

			/// <summary>Datatype that stores sizes of each dimension of a single hyperbox.</summary>
			typedef Data::GaSingleDimensionArray<GaCoordinate> GaGridSize;

			/// <summary>Functor that extracts coordinates of point object.</summary>
			typedef COORDINATES_GETTER GaCoordinatesGetter;

		private:

			/// <summary>Sizes of each dimension of a single hyperbox.</summary>
			GaGridSize _gridSize;

			/// <summary>Functor that extracts coordinates of point object.</summary>
			GaCoordinatesGetter _coordinatesGetter;

		public:

			/// <summary>This constructor initializes hypergird with hyperbox size and the way of extracting coordinates from hyperspace points.</summary>
			/// <param name="gridSize">sizes of each dimension of a single hyperbox.</param>
			/// <param name="dimensionCount">number of dimensions of hyperspace.</param>
			/// <param name="coordinatesGetter">functor that extracts coordinates of point object.</param>
			GaHyperGrid(const GaCoordinate* gridSize,
				int dimensionCount,
				const GaCoordinatesGetter& coordinatesGetter = GaCoordinatesGetter()) : _gridSize(gridSize, dimensionCount),
				_coordinatesGetter(coordinatesGetter) { }

			/// <summary>This constructor initializes hypergird with hyperbox size and the way of extracting coordinates from hyperspace points.</summary>
			/// <param name="gridSize">sizes of each dimension of a single hyperbox.</param>
			/// <param name="coordinatesGetter">functor that extracts coordinates of point object.</param>
			GaHyperGrid(const GaGridSize& gridSize,
				const GaCoordinatesGetter& coordinatesGetter = GaCoordinatesGetter()) : _gridSize(gridSize),
				_coordinatesGetter(coordinatesGetter) { }

			/// <summary>This constructor initializes hypergird with number of dimensions of hyperspace and the way of extracting coordinates from hyperspace points.</summary>
			/// <param name="dimensionCount">number of dimensions of hyperspace.</param>
			/// <param name="coordinatesGetter">functor that extracts coordinates of point object.</param>
			GaHyperGrid(int dimensionCount,
				const GaCoordinatesGetter& coordinatesGetter = GaCoordinatesGetter()) : _gridSize(dimensionCount),
				_coordinatesGetter(coordinatesGetter) { }

			/// <summary>This constructor initializes hypergird with the way of extracting coordinates from hyperspace points.</summary>
			/// <param name="coordinatesGetter">functor that extracts coordinates of point object.</param>
			GaHyperGrid(const GaCoordinatesGetter& coordinatesGetter = GaCoordinatesGetter()) : _coordinatesGetter(coordinatesGetter) { }

			/// <summary>More details are given in specification of <see cref="GaHyperGridBase::Clone/> method.
			///
			/// This method is thread-safe.</summary>
			virtual GaHyperGridBase<GaPoint>* GACALL Clone() const { return new GaHyperGrid<GaPoint, GaCoordinate, GaCoordinatesGetter>( *this ); }

			/// <summary>More details are given in specification of <see cref="GaHyperGridBase::GetHyperBox/> method.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL GetHyperBox(const GaPoint& point,
				GaHyperBox& hyperBox) const
			{
				// coordinates of the point
				const GaGridSize& f = _coordinatesGetter( point );

				// calculate hyperbox coordinates
				for( int i = _gridSize.GetSize() - 1; i >= 0; i-- )
				{
					int c = (int)( f[ i ] / _gridSize[ i ] );
					hyperBox[ i ] = c == 0 && f[ i ] < 0 ? c - 1 : c;
				}
			}

			/// <summary>More details are given in specification of <see cref="GaHyperGridBase::GetHyperBox/> method.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL GetHyperBox(const GaPoint& point,
				const GaPoint& origin,
				GaHyperBox& hyperBox) const
			{
				const GaGridSize& p = _coordinatesGetter( point );
				const GaGridSize& o = _coordinatesGetter( origin );

				// calculate hyperbox coordinates
				for( int i = hyperBox.GetSize() - 1; i >= 0; i-- )
					hyperBox[ i ] = (int)( ( p[ i ] - o[ i ] ) / _gridSize[ i ] );
			}

			/// <summary>More details are given in specification of <see cref="GaHyperGridBase::GetHyperBoxBounds/> method.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL GetHyperBoxBounds(const GaHyperBox& hyperBox,
				const GaPoint& origin,
				GaHyperGridBounds<GaPoint>& bounds) const
			{
				const GaGridSize& o = _coordinatesGetter( origin );

				GaGridSize& low = _coordinatesGetter( bounds.GetLowerBounds() );
				GaGridSize& up = _coordinatesGetter( bounds.GetUpperBounds() );

				// calculate bounds of hyeprbox
				for( int i = _gridSize.GetSize() - 1; i >= 0; i-- )
				{
					low[ i ] = o[ i ] + hyperBox[ i ] * _gridSize[ i ];
					up[ i ] = low[ i ] + _gridSize[ i ];
				}
			}

			/// <summary><c>SetGridSize</c> method sets size of a single hyperbox that will be used by hypergrid.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="gridSize">sizes of each dimension of a single hyperbox.</param>
			/// <param name="dimensionCount">number of dimensions of hyperspace.</param>
			inline void GACALL SetGridSize(const GaCoordinate* gridSize,
				int dimensionCount) { SetGridSize( GaGridSize( gridSize, dimensionCount ) ); }

			/// <summary><c>SetGridSize</c> method sets size of a single hyperbox that will be used by hypergrid.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="size">sizes of each dimension of a single hyperbox.</param>
			inline void GACALL SetGridSize(const GaGridSize& size) { _gridSize = size; }

			/// <summary><c>SetGridSize</c> method sets size of single dimension of hyperbox.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="size">new size.</param>
			/// <param name="dimension">dimension that should be changed.</param>
			inline void GACALL SetGridSize(const GaCoordinate& size,
				int dimension) { _gridSize[ dimension ] = size; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns sizes of each dimension of a single hyperbox.</returns>
			inline GaGridSize& GACALL GetGridSize() { return _gridSize; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns sizes of each dimension of a single hyperbox.</returns>
			inline const GaGridSize& GACALL GetGridSize() const { return _gridSize; }

			/// <summary><c>GetDimensionSize</c> method queries size of a dimension of hyperbox.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="dimension">queried dimension.</param>
			/// <returns>Method returns size of queried dimension.</returns>
			inline const GaCoordinate& GACALL GetDimensionSize(int dimension) const { return _gridSize[ dimension ]; }

			/// <summary><c>SetDimensionCount</c> method sets number of dimensions that hyeprbox will have.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="count">new number of dimensions.</param>
			inline void GACALL SetDimensionCount(int count) { _gridSize.SetSize( count ); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of dimensions that hypergrid has.</returns>
			inline int GACALL GetDimensionCount() const { return _gridSize.GetSize(); }

			/// <summary><c>SetCoordinatesGetter</c> method sets functor that will be used for extracting coordinates of point object.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="getter">functor for extracting coordinates.</param>
			inline void SetCoordinatesGetter(const GaCoordinatesGetter& getter) { _coordinatesGetter = getter; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns functor that extracts coordinates of point object.</returns>
			inline GaCoordinatesGetter& GetCoordinatesGetter() { return _coordinatesGetter; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns functor that extracts coordinates of point object.</returns>
			inline const GaCoordinatesGetter& GetCoordinatesGetter() const { return _coordinatesGetter; }

			/// <summary><c>operator []</c> providess access to size of singled dimension of hyperbox.
			///
			/// This method is not thread-safe</summary>
			/// <param name="dimension">index of queried dimension.</param>
			/// <returns>Method returns reference to queried dimension.</returns>
			inline GaCoordinate& operator [](int dimension) { return _gridSize[ dimension ]; }

			/// <summary><c>operator []</c> providess access to size of singled dimension of hyperbox.
			///
			/// This method is not thread-safe</summary>
			/// <param name="dimension">index of queried dimension.</param>
			/// <returns>Method returns reference to queried dimension.</returns>
			inline const GaCoordinate& operator [](int dimension) const { return _gridSize[ dimension ]; }

		};

		/// <summary><c>GaAdaptiveGridBase</c> template is base class for hypergrids that can divide hyperspace of specified type into fixed number of hyperboxes.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		/// <typeparam name="POINT">type of data that represents single point in hyperspace.</typeparam>
		template<typename POINT>
		class GaAdaptiveGridBase
		{

		public:

			/// <summary>Datatype that represents single point in hyperspace.</summary>
			typedef POINT GaPoint;

			/// <summary>Datatype that stores number of hyberboxes that each dimension should be divided to.</summary>
			typedef Data::GaSingleDimensionArray<int> GaGridSize;

		protected:

			/// <summary>Number of hyberboxes that each dimension should be divided to.</summary>
			GaGridSize _gridSize;

		public:

			/// <summary>This constructor initializes grid with its size.</summary>
			/// <param name="gridSize">number of hyberboxes that each dimension should be divided to.</param>
			/// <param name="dimensionCount">number of dimension that hyperspace has.</param>
			GaAdaptiveGridBase(const int* gridSize,
				int dimensionCount) : _gridSize(gridSize, dimensionCount) { }

			/// <summary>This constructor initializes grid with its size.</summary>
			/// <param name="gridSize">number of hyberboxes that each dimension should be divided to.</param>
			GaAdaptiveGridBase(const GaGridSize& gridSize) : _gridSize(gridSize) { }

			/// <summary>This constructor initializes empty hypergrid.</summary>
			GaAdaptiveGridBase() { }

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaAdaptiveGridBase() { }

			/// <summary><c>Clone</c> method creates copy of grid object. Caller is responsible for allocated memory.</summary>
			/// <returns>Method returns pointer to newly created object.</returns>
			virtual GaAdaptiveGridBase<GaPoint>* GACALL Clone() const = 0;

			/// <summary><c>UpdateGridBounds</c> method resizes adaptive hypergrid if needed to contain specified point in hyperspace.</summary>
			/// <param name="point">paoint that should be contained by the adaptive hypergrid.</param>
			/// <param name="bounds">object that stores adaptive hypergrid bounds before resise and to which the new size is saved.</param>
			virtual void GACALL UpdateGridBounds(const GaPoint& point,
				GaHyperGridBounds<GaPoint>& bounds) const = 0;

			/// <summary><c>UpdateHyperGrid</c> method updates or creates new fixed hypergrid based on current adaptive hypergird bounds
			/// and number of hyperboxes it should be divided to.</summary>
			/// <param name="bounds">bounds of adaptive hypergrid.</param>
			/// <param name="hyperGrid">smart pointer that store fixed hypergrid that should be updated.
			/// If it is <c>NULL</c> new hypergrid should be created and this paramenter will store pointer to it.</param>
			virtual void GACALL UpdateHyperGrid(const GaHyperGridBounds<GaPoint>& bounds,
				Memory::GaAutoPtr<GaHyperGridBase<GaPoint> >& hyperGrid) const = 0;

			/// <summary><c>SetGridSize</c> sets number of hyberboxes that each dimension should be divided to.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="gridSize">number of hyberboxes.</param>
			/// <param name="dimensionCount">number of dimension that hyperspace has.</param>
			inline void GACALL SetGridSize(const int* gridSize,
				int dimensionCount) { SetGridSize( GaGridSize( gridSize, dimensionCount ) ); }

			/// <summary><c>SetGridSize</c> sets number of hyberboxes that each dimension should be divided to.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="size">number of hyberboxes.</param>
			inline void GACALL SetGridSize(const GaGridSize& size) { _gridSize = size; }

			/// <summary><c>SetGridSize</c> sets number of hyberboxes that specified dimension is going to be divided to.</summary>
			/// <param name="size">new number of hyperboxes.</param>
			/// <param name="dimension">dimension that should be changed.</param>
			inline void GACALL SetGridSize(int size,
				int dimension) { _gridSize[ dimension ] = size; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of hyberboxes that each dimension should be divided to.</returns>
			inline GaGridSize& GACALL GetGridSize() { return _gridSize; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of hyberboxes that each dimension should be divided to.</returns>
			inline const GaGridSize& GACALL GetGridSize() const { return _gridSize; }

			/// <summary><c>GetDimensionSize</c> method queries the number of hyperobxes that a specified dimension is divided to.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="dimension">queried dimension.</param>
			/// <returns>Method returns number of hyperobxes that dimension is divided to.</returns>
			inline int GACALL GetDimensionSize(int dimension) const { return _gridSize[ dimension ]; }

			/// <summary><c>SetDimensionCount</c> method sets number of dimensions that adaptive hypergrid will have.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="count"></param>
			inline void GACALL SetDimensionCount(int count) { _gridSize.SetSize( count ); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of dimensions that adaptive hypergrid has.</returns>
			inline int GACALL GetDimensionCount() const { return _gridSize.GetSize(); }

		};

		/// <summary><c>GaAdaptiveGrid</c> template represents hypergrids that can divide hyperspace of specified type into fixed number of hyperboxes.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		/// <typeparam name="POINT">type of data that represents single point in hyperspace.</typeparam>
		/// <typeparam name="COORDINATE">type of single coordinate of a point in hyperspace.</typeparam>
		/// <typeparam name="COORDINATES_GETTER">functor that extracts coordinates of point object.</typeparam>
		template<typename POINT,
			typename COORDINATE,
			typename COORDINATES_GETTER>
		class GaAdaptiveGrid : public GaAdaptiveGridBase<POINT>
		{

		public:

			/// <summary>Type of single coordinate of a point in hyperspace.</summary>
			typedef COORDINATE GaCoordinate;

			/// <summary></summary>
			typedef Data::GaSingleDimensionArray<GaCoordinate> GaCoordinates;

			/// <summary>Functor that extracts coordinates of point object.</summary>
			typedef COORDINATES_GETTER GaCoordinatesGetter;

		protected:

			/// <summary>Functor that extracts coordinates of point object.</summary>
			GaCoordinatesGetter _coordinatesGetter;

		public:

			/// <summary>This constructor initializes grid with its size and the way of extracting coordinates from hyperspace points..</summary>
			/// <param name="gridSize">number of hyberboxes that each dimension should be divided to.</param>
			/// <param name="dimensionCount">number of dimension that hyperspace has.</param>
			/// <param name="coordinatesGetter">functor that extracts coordinates of point object.</param>
			GaAdaptiveGrid(const int* gridSize,
				int dimensionCount,
				const GaCoordinatesGetter& coordinatesGetter = GaCoordinatesGetter()) : GaAdaptiveGridBase(gridSize, dimensionCount),
				_coordinatesGetter(coordinatesGetter) { }

			/// <summary>This constructor initializes grid with its size and the way of extracting coordinates from hyperspace points..</summary>
			/// <param name="gridSize">number of hyberboxes that each dimension should be divided to.</param>
			/// <param name="coordinatesGetter">functor that extracts coordinates of point object.</param>
			GaAdaptiveGrid(const GaGridSize& gridSize,
				const GaCoordinatesGetter& coordinatesGetter = GaCoordinatesGetter()) : GaAdaptiveGridBase(gridSize),
				_coordinatesGetter(coordinatesGetter) { }

			/// <summary>This constructor initializes hypergird with the way of extracting coordinates from hyperspace points.</summary>
			/// <param name="coordinatesGetter">functor that extracts coordinates of point object.</param>
			GaAdaptiveGrid(const GaCoordinatesGetter& coordinatesGetter = GaCoordinatesGetter()) : _coordinatesGetter(coordinatesGetter) { }

			/// <summary>More details are given in specification of <see cref="GaAdaptiveGridBase::Clone/> method.
			///
			/// This method is thread-safe.</summary>
			virtual GaAdaptiveGridBase<GaPoint>* GACALL Clone() const { return new GaAdaptiveGrid<GaPoint, GaCoordinate, GaCoordinatesGetter>( *this ); }

			/// <summary>More details are given in specification of <see cref="GaAdaptiveGridBase::UpdateGridBounds/> method.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL UpdateGridBounds(const GaPoint& point,
				GaHyperGridBounds<GaPoint>& bounds) const
			{
				const GaCoordinates& in = _coordinatesGetter( point );
				GaCoordinates& low = _coordinatesGetter( bounds.GetLowerBounds() );
				GaCoordinates& up = _coordinatesGetter( bounds.GetUpperBounds() );

				// move bounds of dimensions
				for( int i = in.GetSize() - 1; i >= 0; i-- )
				{
					const GaCoordinate& v = in[ i ];
					// move lower bound if needed
					if( v < low[ i ] )
						low[ i ] = v;
					// move higher bound if needed
					if( v > up[ i ] )
						up[ i ] = v;
				}
			}

			/// <summary>More details are given in specification of <see cref="GaAdaptiveGridBase::UpdateHyperGrid/> method.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL UpdateHyperGrid(const GaHyperGridBounds<GaPoint>& bounds,
				Memory::GaAutoPtr<GaHyperGridBase<GaPoint> >& hyperGrid) const
			{
				int dimensionCount = GetDimensionCount();

				GaHyperGrid<GaPoint, GaCoordinate, GaCoordinatesGetter>* hg = (GaHyperGrid<GaPoint, GaCoordinate, GaCoordinatesGetter>*)hyperGrid.GetRawPtr();

				// hypergrid does not exist?
				if( !hg )
					hyperGrid = hg = new GaHyperGrid<GaPoint, GaCoordinate, GaCoordinatesGetter>( dimensionCount );
				// number of dimensions has changed?
				else if( hg->GetDimensionCount() != dimensionCount )
					hg->SetDimensionCount( dimensionCount );

				const GaCoordinates& low = _coordinatesGetter( bounds.GetLowerBounds() );
				const GaCoordinates& up = _coordinatesGetter( bounds.GetUpperBounds() );

				// calculate sizes for all dimension of hyperbox for fixed hypergrid
				for( int i = dimensionCount - 1; i >= 0; i-- )
					( *hg )[ i ] = ( up[ i ] - low[ i ]  + 2 * ( up[ i ] - low[ i ] ) / _gridSize[ i ] ) / _gridSize[ i ];
			}

			/// <summary><c>SetCoordinatesGetter</c> method sets functor that will be used for extracting coordinates of point object.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="getter">functor for extracting coordinates.</param>
			inline void SetCoordinatesGetter(const GaCoordinatesGetter& getter) { _coordinatesGetter = getter; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns functor that extracts coordinates of point object.</returns>
			inline GaCoordinatesGetter& GetCoordinatesGetter() { return _coordinatesGetter; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns functor that extracts coordinates of point object.</returns>
			inline const GaCoordinatesGetter& GetCoordinatesGetter() const { return _coordinatesGetter; }

		};

		/// <summary><c>GaGridStorageMatrix</c> template class provides storage of specified type of data for each hyperbox of hypergrid.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		/// <typeparam name="DATA_TYPE">type of stored data.</typeparam>
		template<typename DATA_TYPE>
		class GaGridStorageMatrix
		{

		public:

			/// <summary>Type of stored data.</summary>
			typedef DATA_TYPE GaDataType;

			/// <summary>Datatype that providex storage for one data object of specified type per hyperbox.</summary>
			typedef Data::GaMultiDimensionArray<GaDataType> GaMatrix;

		private:

			/// <summary>Matrix that stores data for each hyperbox.</summary>
			GaMatrix _matrix;

		public:

			/// <summary>This constructor initializes storage matrix with specified size.</summary>
			/// <param name="gridSize">dimension sizes of the matrix.</param>
			GaGridStorageMatrix(const Common::Data::GaSingleDimensionArray<int>& gridSize) : _matrix( gridSize ) { }

			/// <summary>This constructor initializes minimal storage matrix.</summary>
			GaGridStorageMatrix() { }

			/// <summary><c>Update</c> method sets size of the matrix (change number of dimensions as well as their sizes).
			///
			/// This method is not thread-safe.</summary>
			/// <param name="gridSize">new dimension sizes.</param>
			void GACALL Update(const Common::Data::GaSingleDimensionArray<int>& gridSize)
			{
				int dimensions = gridSize.GetSize();

				// dimensions are added or removed
				if( _matrix.GetDimensionCount() != dimensions )
				{
					// remove old matrix and create new
					_matrix.Clear();
					_matrix.AddDimension( gridSize );
				}
				else
				{
					// dimensions are just resized

					// resize required dimension 
					for( int i = dimensions - 1; i > 0; i-- )
					{
						int size = gridSize[ i ];
						if( _matrix.GetDimensionSize( i ) != size )
							_matrix.SetDimenstionSize( i, size );
					}
				}
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns matrix that stores data for each hyperbox.</returns>
			inline GaMatrix& GACALL GetMatrix() { return _matrix; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns matrix that stores data for each hyperbox.</returns>
			inline const GaMatrix& GACALL GetMatrix() const { return _matrix; }

		};

		/// <summary><c>GaBestItemsMatrix</c> template class stores information about grid density and best ranked items in the matrix format.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public method are thread-safe.</summary>
		/// <typeparam name="ITEM">type of items stored in the cells.</typeparam>
		template<typename ITEM>
		class GaBestItemsMatrix
		{

		public:

			/// <summary>Type of items stored in the cells.</summary>
			typedef ITEM GaItem;

			/// <summary>Matrix datatype that stores items.</summary>
			typedef Common::Grid::GaGridStorageMatrix<GaItem> GaItemMatrix;

			/// <summary><c>GaCell</c> class stores number of items and best ranked item in the cell of a matrix.
			///
			/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
			/// No public or private methods are thread-safe.</summary>
			class GaCell
			{

				friend class GaBestItemsMatrix<GaItem>;

				GA_DEFINE_ATOM_NODE( GaCell );

			private:

				/// <summary>Number of items located in the cell.</summary>
				Common::Threading::GaAtomic<int> _count;

				/// <summary>The best ranked item in the cell.</summary>
				GaItem _best;

			public:

				/// <summary>This constructor initializes empty cell.</summary>
				GaCell() : _best(NULL) { }

				/// <summary><c>Clear</c> method clears content of the cell.</summary>
				inline void GACALL Clear()
				{
					_count = 0;
					_best = NULL;
				}

				/// <summary>This method is not thread-safe.</summary>
				/// <returns>Method returns number of items located in the cell.</returns>
				inline int GACALL GetCount() const { return _count; }

				/// <summary>This method is not thread-safe.</summary>
				/// <returns>Method returns the best ranked item in the cell.</returns>
				inline GaItem GACALL GetBest() const { return _best; }

			};

			/// <summary>Array of matrices that stores best ranked items identified by separate branchs.</summary>
			typedef Common::Data::GaSingleDimensionArray<GaItemMatrix> GaPerBranch;

			/// <summary>Matrix that stores cell information.</summary>
			typedef Common::Grid::GaGridStorageMatrix<GaCell> GaCellMatrix;

		private:

			/// <summary>Matrix datatype that stores cell information.</summary>
			GaCellMatrix _cells;

			/// <summary>List of occupied cells.</summary>
			Common::Data::GaAtomicList<GaCell> _occupiedCells[ 2 ];

		public:

			/// <summary>This constructor initializes empty matrix.</summary>
			GaBestItemsMatrix() { }

			/// <summary><c>Insert</c> method updates number of items in the cell.
			///
			/// This method is thread-safe.</summary>
			/// <param name="hyperbox">coordinates of cell that should be updated.</param>
			/// <returns>Method returns pointer to cell located at specified coordinates.</returns>
			inline GaCell* GACALL Insert(const Common::Grid::GaHyperBox& hyperbox)
			{
				GaCell* cell = &_cells.GetMatrix()[ hyperbox ];

				// increment number of items
				if( cell->_count++ == 0 )
					// update list of occpied cell if this is the first identified item in the cell
					_occupiedCells[ 0 ].Push( cell );

				return cell;
			}

			/// <summary><c>CollectBest</c> method determins the best ranked items for all occpied cells.
			///
			/// This method is thread-safe.</summary>
			/// <typeparam name="ITEM_RANK_GETTER">type of functor that extracts item's rank</typeparam>
			/// <param name="perBranch">matrices that stores best ranked items identified by separate branchs.</param>
			/// <param name="rank">functor that extracts item's rank.</param>
			template<typename ITEM_RANK_GETTER>
			void GACALL CollectBest(GaPerBranch& perBranch,
				const ITEM_RANK_GETTER& rank)
			{
				int start = perBranch.GetSize() - 1;
				GaCell* base = _cells.GetMatrix().GetArray();

				// determin best ranked items for all occupied cells
				while( GaCell* cell = _occupiedCells[ 0 ].Pop() )
				{
					// index of the cell in multi-dimensional arrary
					int index = cell - base;

					// determin the best items among those identified by the separate branches
					for( int i = start; i >= 0; i-- )
					{
						GaItem& current = perBranch[ i ].GetMatrix()[ index ];

						// is current item better then previously identified?
						if( !cell->_best || ( current && rank( current ) > rank( cell->_best ) ) )
							cell->_best = current;

						current = NULL;
					}

					_occupiedCells[ 1 ].Push( cell );
				}
			}

			/// <summary><c>Clear</c> method clears all occupied cells.
			///
			/// This method is thread-safe.</summary>
			inline void GACALL Clear()
			{
				while( GaCell* cell = _occupiedCells[ 1 ].Pop() )
					cell->Clear();
			}

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns matrix that stores data.</returns>
			inline GaCellMatrix& GACALL GetCells() { return _cells; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns matrix that stores data.</returns>
			inline const GaCellMatrix& GACALL GetCells() const { return _cells; }

		};

	} // Grid
} // Common

#endif // __GA_GRID_H__

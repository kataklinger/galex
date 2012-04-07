
/*! \file SPEA.cpp
    \brief This file conatins implementation of classes that implement Strength Pareto Evolutionary Algorithm I and II (SPEA-I and SPEA-II).
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#include <limits>
#include "PopulationStatistics.h"
#include "Multiobjective.h"
#include "SPEA.h"

namespace Multiobjective
{
	namespace SPEA
	{

		/// <summary><c>GaSPEACluster</c> class stores list of nearby chromosomes that form a cluster.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaSPEACluster
		{

		private:

			/// <summary>Chromosomes that form cluster.</summary>
			Common::Data::GaList<Population::GaChromosomeStorage*> _chromosomes;

		public:

			/// <summary><c>AddChromosome</c> method adds chromosome to the cluster.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="chromosome">chromosome that should be added to cluster.</param>
			inline void GACALL AddChromosome(Population::GaChromosomeStorage* chromosome) { _chromosomes.InsertTail( chromosome ); }

			/// <summary><c>Clear</c> removes all chromosomes from the cluster.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL Clear() { _chromosomes.Clear(); }

			/// <summary><c>Merge</c> method moves all chromosomes from specified cluster to this cluster.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="cluster">cluster whose chromosomes should be moved.</param>
			inline void GACALL Merge(GaSPEACluster& cluster) { _chromosomes.MergeLists( &cluster._chromosomes ); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns chromosome which is closest to the center of cluster.</returns>
			const Population::GaChromosomeStorage* GACALL GetCenter() const
			{
				const Common::Data::GaListNode<Population::GaChromosomeStorage*>* center = _chromosomes.GetHead();
				float minDist = std::numeric_limits<float>::infinity();

				if( _chromosomes.GetCount() > 2 )
				{
					// find chromosome whose average distance to other chromosomes in the cluster is smallest
					for( const Common::Data::GaListNode<Population::GaChromosomeStorage*>* it1 = _chromosomes.GetHead(); it1; it1 = it1->GetNext() )
					{
						// get sum of distances to other chromosomes in cluster for current chromosome
						float totalDist = 0;
						int count = 0;
						for( const Common::Data::GaListNode<Population::GaChromosomeStorage*>* it2 = _chromosomes.GetHead(); it2; it2 = it2->GetNext() )
						{
							if( it1 != it2 )
							{
								// sum distance
								float d = ChromosomeDistance( it1->GetValue(), it2->GetValue() );
								if( d > 0 )
								{
									totalDist += d;
									count++;
								}
							}
						}

						// average distance of current chromosomes to other chromosomes in cluster
						totalDist /= count;

						// is current chromosome closre in average to othe chromosomes then previous one?
						if( totalDist < minDist )
						{
							// current chromosome is new center of the cluster
							minDist = totalDist;
							center = it1;
						}
					}
				}
				// whene there is only two chromosomes - randomly select one
				else if( _chromosomes.GetCount() > 1 )
					center = GaGlobalRandomBoolGenerator->Generate() ? _chromosomes.GetHead() : _chromosomes.GetTail();

				return center ? center->GetValue() : NULL;
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns list of chromosomes that belongs to this cluster.</returns>
			inline Common::Data::GaList<Population::GaChromosomeStorage*>& GACALL GetChromosomes() { return _chromosomes; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns list of chromosomes that belongs to this cluster.</returns>
			inline const Common::Data::GaList<Population::GaChromosomeStorage*>& GACALL GetChromosomes() const { return _chromosomes; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of chromosomes in the cluster.</returns>
			inline int GACALL GetCount() const { return _chromosomes.GetCount(); }

			/// <summary><c>operator -</c> calculates average distance between chromosomes in two cluster.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="rhs">cluster to which the average distance is calculated.</param>
			/// <returns>Operator returns calculated distance.</returns>
			float GACALL operator -(const GaSPEACluster& rhs) const
			{
				float distance = 0;

				// sum all distances between chromosomes
				for( const Common::Data::GaListNode<Population::GaChromosomeStorage*>* it1 = _chromosomes.GetHead(); it1; it1 = it1->GetNext() )
				{
					for( const Common::Data::GaListNode<Population::GaChromosomeStorage*>* it2 = rhs._chromosomes.GetHead(); it2; it2 = it2->GetNext() )
						distance += ChromosomeDistance( it1->GetValue(), it2->GetValue() );
				}

				// calculate average distance
				return distance / ( _chromosomes.GetCount() * rhs._chromosomes.GetCount() );
			}

		protected:

			/// <summary><c>ChromosomeDistance</c> method calculates distance between two chromosomes.</summary>
			/// <param name="chromosome1">the first chromosome.</param>
			/// <param name="chromosome2">the second chromosome.</param>
			/// <returns>Method returns calculated distance.</returns>
			inline float ChromosomeDistance(Population::GaChromosomeStorage* chromosome1,
				Population::GaChromosomeStorage* chromosome2) const
				{ return chromosome1->GetFitness( Population::GaChromosomeStorage::GAFT_RAW ).Distance( chromosome2->GetFitness( Population::GaChromosomeStorage::GAFT_RAW ) ); }

		};

		/// <summary><c>GaSPEAClusterStorage</c> class stores and manages chromosome clusters.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaSPEAClusterStorage
		{

		private:

			/// <summary><c>GaSPEAClusterDistance</c> structure stores distances between chromosome clusters.</summary>
			struct GaSPEAClusterDistance
			{

				/// <summary>The first cluster.</summary>
				GaSPEACluster* _clusterA;

				/// <summary>The second cluster.</summary>
				GaSPEACluster* _clusterB;

				/// <summary>Distance between two clusters.</summary>
				float _distance;

				/// <summary>Initailizes empty distance object.</summary>
				GaSPEAClusterDistance() : _clusterA(NULL),
					_clusterB(NULL),
					_distance(std::numeric_limits<float>::infinity()) { }

				/// <summary><c>Clear</c> method clears data stored in distance object.</summary>
				inline void GACALL Clear()
				{
					_clusterA = _clusterB = NULL;
					_distance = std::numeric_limits<float>::infinity();
				}

			};

			/// <summary>Buffer that stores chromosome clusters.</summary>
			Common::Data::GaSingleDimensionArray<GaSPEACluster> _clusters;

			/// <summary>Number of clusters in the buffer.</summary>
			Common::Threading::GaAtomic<int> _clusterCount;

			/// <summary>Distances to the closest clusters identified by each workflow branch.</summary>
			Common::Data::GaSingleDimensionArray<GaSPEAClusterDistance> _minDistances;

			/// <summary>DIstance to the two closest clusters identified by all branches.</summary>
			GaSPEAClusterDistance* _globalMinDistance;

		public:

			/// <summary>This constructor initializes empty clusters buffer.</summary>
			GaSPEAClusterStorage() : _globalMinDistance(NULL) { }

			/// <summary><c>CreateCluster</c> method creates cluster with only one chromosome and insets is into buffer.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="first">chromosome that should form new cluster.</param>
			inline void GACALL CreateCluster(Population::GaChromosomeStorage* first)
			{
				GaSPEACluster& cluster = _clusters[ _clusterCount++ ];
				cluster.Clear();
				cluster.AddChromosome( first );
			}

			/// <summary><c>StoreDistance</c> method stores distance between two clusters if it is smaller than previously stored distance for the specified branch.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="clusterA">the first cluster.</param>
			/// <param name="clusterB">the second cluster.</param>
			/// <param name="branchID">ID of branch that executes operation.</param>
			inline void GACALL StoreDistance(GaSPEACluster& clusterA,
				GaSPEACluster& clusterB,
				int branchID)
			{
				GaSPEAClusterDistance& minDistance = _minDistances[ branchID ];

				// calculate distance between clusters
				float dist = clusterA - clusterB;

				// is the distance smaller than praviously stored distance?
				if( dist < minDistance._distance )
				{
					// store new distance to storage object
					minDistance._distance = dist;

					minDistance._clusterA = &clusterA;
					minDistance._clusterB = &clusterB;
				}
			}

			/// <summary><c>MergeClosest</c> finds and merges closest clusters.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL MergeClosest()
			{
				// find two closest clusters among the ones that are identified by the branches
				_globalMinDistance = &_minDistances[ 0 ];
				for( int i = _minDistances.GetSize() - 1; i > 0; i-- )
				{
					if( _minDistances[ i ]._distance < _globalMinDistance->_distance )
						_globalMinDistance = &_minDistances[ i ];
				}

				// merge clusters
				if( _globalMinDistance->_clusterA != NULL  )
					_globalMinDistance->_clusterA->Merge( *_globalMinDistance->_clusterB );
			}

			/// <summary><c>ClearDistanceInfo</c> method clears distance inforamtions of closest clusters for specified branch.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="branchID">ID of branch for which the distance information should be cleared.</param>
			inline void GACALL ClearDistanceInfo(int branchID) { _minDistances[ branchID ].Clear(); }

			/// <summary><c>GetMerged</c> method returns pointer to cluster that is formed by merging closest clusters.
			///
			/// This method is not thread-safe.</summary>
			/// <returns></returns>
			inline GaSPEACluster* GACALL GetMerged() { return _globalMinDistance->_clusterA; }

			/// <summary><c>IsMerged</c> method checks whether the merged clusters was identified by specified branch.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="branchID">ID of branch that should be checked.</param>
			/// <returns>Method returns <c>true</c> if merged clusters was identified by specified branch.</returns>
			inline bool GACALL IsMerged(int branchID) const
			{
				const GaSPEAClusterDistance& dist = _minDistances[ branchID ];
				return !_globalMinDistance || 
					dist._clusterA == _globalMinDistance->_clusterA || dist._clusterB == _globalMinDistance->_clusterA || dist._clusterB == _globalMinDistance->_clusterB;
			}

			/// <summary><c>SetSize</c> method sets sizes of buffers that handles chromosome clusters.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="size">number of clusters that buffer should store.</param>
			/// <param name="branchCount">number of branches that will use bufer.</param>
			inline void SetSize(int size,
				int branchCount)
			{
				// set sizes of buffers
				_clusters.SetSize( size );
				_minDistances.SetSize( branchCount );

				// clear buffers
				_clusterCount = 0;
				_globalMinDistance = NULL;
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of clusters currently in the buffer.</returns>
			inline int GACALL GetCount() const { return _clusterCount; }

			/// <summary><c>operator []</c> provides access to chromosome cluster at specified location in the buffer.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="index">index of cluster in the buffer.</param>
			/// <returns>Method returns reference to queried buffer.</returns>
			inline GaSPEACluster& GACALL operator [](int index) { return _clusters[ index ]; }

			/// <summary><c>operator []</c> provides access to chromosome cluster at specified location in the buffer.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="index">index of cluster in the buffer.</param>
			/// <returns>Method returns reference to queried buffer.</returns>
			inline const GaSPEACluster& GACALL operator [](int index) const { return _clusters[ index ]; }

		};

		/// <summary><c>GaSPEADominanceOp</c> class represents operation that compares and determins dominant chromosome and updates dominated chromosome lists.</summary>
		class GaSPEADominanceOp
		{

		private:

			/// <summary>Population over which the dominance operation is performed.</summary>
			Population::GaPopulation& _population;

			/// <summary>Chromosome tag that contains partial list of dominated chromosome.</summary>
			Population::GaChromosomeTagGetter<GaDominanceList> _domList;

			/// <summary>Chromosome tag that stores information whether the chromosome is dominated by any other chromosome in the population.</summary>
			Population::GaChromosomeTagGetter<int> _dominated;

			/// <summary>ID of the brach that executes operation.</summary>
			int _branchID;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="population">population over which the dominance operation is performed.</param>
			/// <param name="domList">chromosome tag that contains partial list of dominated chromosome.</param>
			/// <param name="dominated">chromosome tag that stores information whether the chromosome is dominated by any other chromosome in the population.</param>
			/// <param name="branchID">ID of the brach that executes operation.</param>
			GaSPEADominanceOp(Population::GaPopulation& population,
				const Population::GaChromosomeTagGetter<GaDominanceList>& domList,
				const Population::GaChromosomeTagGetter<int>& dominated,
				int branchID) : _population(population),
				_domList(domList),
				_dominated(dominated),
				_branchID(branchID) { }

			/// <summary>Dummy operator to satisfy interface.</summary>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index) { }

			/// <summary><c>operator ()</c> determins dominant chromosome and update dominance list of chromosomes.</summary>
			/// <param name="chromosome1">the first chromosome.</param>
			/// <param name="chromosome2">the second chromosome.</param>
			/// <param name="index1">index of the the first chromosome.</param>
			/// <param name="index2">index of the the second chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome1,
				Population::GaChromosomeStorage& chromosome2,
				int index1,
				int index2)
			{
				// there is no need to comapre already domnanted chromosome
				if( !_dominated( chromosome1 ) || !_dominated( chromosome2 ) )
				{
					// get dominant chromosome
					int dom = _population.CompareFitness( chromosome1, chromosome2 );

					// is the first chromosome dominant?
					if( dom < 0 )
					{
						// mark dominated chromosome and update dominance list 
						_domList( chromosome1 )[ _branchID ].InsertTail( &chromosome2 );
						_dominated( chromosome2 ) = 1;
					}
					// is the second chromosome dominant?
					else if( dom > 0 )
					{
						// mark dominated chromosome and update dominance list 
						_domList( chromosome2 )[ _branchID ].InsertTail( &chromosome1 );
						_dominated( chromosome1 ) = 1;
					}
				}
			}

		};

		/// <summary><c>GaSPEAStrengthOp</c> class represents operation that assigns strength to chromosomes and identify nondominated chromosomes.</summary>
		class GaSPEAStrengthOp
		{

		private:

			/// <summary>Chromosome tag that stores strength value of the chromosome used to calculate scaled fitness.</summary>
			Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > _strength;

			/// <summary>Chromosome tag that contains partial list of dominated chromosome.</summary>
			Population::GaChromosomeTagGetter<GaDominanceList> _domList;

			/// <summary>Chromosome tag that stores information whether the chromosome is dominated by any other chromosome in the population.</summary>
			Population::GaChromosomeTagGetter<int> _dominated;

			/// <summary>Buffer that stores indentified chromosome clusters.</summary>
			GaSPEAClusterStorage& _clusters;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="strength">chromosome tag that stores strength value of the chromosome used to calculate scaled fitness.</param>
			/// <param name="domList">chromosome tag that contains partial list of dominated chromosome.</param>
			/// <param name="dominated">chromosome tag that stores information whether the chromosome is dominated by any other chromosome in the population.</param>
			/// <param name="clusters">vuffer that stores indentified chromosome clusters.</param>
			GaSPEAStrengthOp(const Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> >& strength,
				const Population::GaChromosomeTagGetter<GaDominanceList>& domList,
				const Population::GaChromosomeTagGetter<int>& dominated,
				GaSPEAClusterStorage& clusters) : _strength(strength),
				_domList(domList),
				_dominated(dominated),
				_clusters(clusters) { }

			/// <summary><c>operator ()</c> calculates strength of chromosome and creates new cluster for it.</summary>
			/// <param name="chromosome">chromosome whose strength should be calculated.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				GaDominanceList& lst = _domList( chromosome );

				if( !_dominated( chromosome ) )
				{
					// get number of chromosomes that are dominated by this chromosome
					int s = 0;
					for( int i = lst.GetSize() - 1; i >= 0; i-- )
						s += lst[ i ].GetCount();

					// assign strength to nondominated chromosome
					_strength( chromosome ) = s;

					// update strength of all dominated chromosomes
					for( int i = lst.GetSize() - 1; i >= 0; i-- )
					{
						for( Common::Data::GaListNode<Population::GaChromosomeStorage*>* it = lst[ i ].GetHead(); it; it = it->GetNext() )
							_strength( it->GetValue() ) += s;

						// clear tag for next genearation
						lst[ i ].Clear();
					}

					// create cluster and place chromosome in it
					_clusters.CreateCluster( &chromosome );
				}
				else
				{
					// clear tag for next genearation
					for( int i = lst.GetSize() - 1; i >= 0; i-- )
						lst[ i ].Clear();
				}
			}

		};

		/// <summary><c>GaSPEAFitnessAssignOp</c> class represents operation that assigns scaled fitness to chromosomes based on their strength.</summary>
		class GaSPEAFitnessAssignOp
		{

		private:

			/// <summary>Chromosome tag that stores strength value of the chromosome used to calculate scaled fitness.</summary>
			Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > _strength;

			/// <summary>Chromosome tag that contains partial list of dominated chromosome.</summary>
			Population::GaChromosomeTagGetter<int> _dominated;

			/// <summary>Number of chromosomes currenlty in the population.</summary>
			int _chromosomeCount;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="strength">chromosome tag that stores strength value of the chromosome used to calculate scaled fitness.</param>
			/// <param name="dominated">chromosome tag that contains partial list of dominated chromosome.</param>
			/// <param name="chromosomeCount">number of chromosomes currenlty in the population.</param>
			GaSPEAFitnessAssignOp(const Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> >& strength,
				const Population::GaChromosomeTagGetter<int>& dominated,
				int chromosomeCount) : _strength(strength),
				_dominated(dominated),
				_chromosomeCount(chromosomeCount) { }

			/// <summary><c>operator ()</c> calculates and assigns scaled fitness to chromosome.</summary>
			/// <param name="chromosome">chromosome whose fitness should be calculated.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				// calculate fitness and assign it to chromosome
				float value = _dominated( chromosome ) ? (float)_chromosomeCount / ( _chromosomeCount + _strength( chromosome ) ): 1.0f + _strength( chromosome );
				( (GaSPEAFitness&)chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_SCALED ) ).SetValue( value );

				// clear chromosome tags for next generation
				_strength( chromosome ) = 0;
				_dominated( chromosome ) = 0;
			}
		};

		/// <summary><c>GaSPEAFindClosestOp</c> class represents operation that searches for closes clusters.</summary>
		class GaSPEAFindClosestOp
		{

		private:

			/// <summary>Buffer that stores indentified chromosome clusters.</summary>
			GaSPEAClusterStorage& _clusters;

			/// <summary>ID of the brach that executes operation.</summary>
			int _branchID;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="clusters">buffer that stores indentified chromosome clusters.</param>
			/// <param name="branchID">ID of the brach that executes operation.</param>
			GaSPEAFindClosestOp(GaSPEAClusterStorage& clusters,
				int branchID) : _clusters(clusters),
				_branchID(branchID) { }

			/// <summary><c>operator ()</c> calculates distance of cluster to other clusters and stores the minimal distance if it is found.</summary>
			/// <param name="cluster">cluster who is compared to other clusters.</param>
			/// <param name="index">index of cluster.</param>
			inline void GACALL operator ()(GaSPEACluster& cluster,
				int index)
			{
				// only for non-empty clusters
				if( cluster.GetCount() )
				{
					// get distances to all other clusters
					for( int j = _clusters.GetCount() - 1; j >= 0; j-- )
					{
						// except for self and other empty clusters
						if( index != j && _clusters[ j ].GetCount() )
							// calcualte distance and store it if it is smaller then previously found minimum
							_clusters.StoreDistance( cluster, _clusters[ j ], _branchID );
					}
				}
			}

		};

		/// <summary><c>GaSPEAUpdateClosestOp</c> class represents operation that updates distance information after clusters has been merged.</summary>
		class GaSPEAUpdateClosestOp
		{

		private:

			/// <summary>Buffer that stores indentified chromosome clusters.</summary>
			GaSPEAClusterStorage& _clusters;

			/// <summary>New cluster tha was formaed by merging closest cluster.</summary>
			GaSPEACluster& _merged;

			/// <summary>ID of the brach that executes operation.</summary>
			int _branchID;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="clusters">buffer that stores indentified chromosome clusters.</param>
			/// <param name="merged">new cluster tha was formaed by merging closest cluster.</param>
			/// <param name="branchID">ID of the brach that executes operation.</param>
			GaSPEAUpdateClosestOp(GaSPEAClusterStorage& clusters,
				GaSPEACluster& merged,
				int branchID) : _clusters(clusters),
				_merged(merged),
				_branchID(branchID) { }

			/// <summary><c>operator ()</c> calculates distance of cluster to merged cluster and stores the minimal distance if it is found.</summary>
			/// <param name="cluster">cluster who is compared to merged clusters.</param>
			/// <param name="index">index of cluster.</param>
			inline void GACALL operator ()(GaSPEACluster& cluster,
				int index) { _clusters.StoreDistance( cluster, _merged, _branchID ); }

		};

		/// <summary><c>GaSPEAPurgeClustersOp</c> class reprsents operation that adds all chromosomes in cluster except the central chromosome to remove buffer.</summary>
		class GaSPEAPurgeClustersOp
		{

		public:

			/// <summary><c>operator ()</c> adds chromosomes that belongs to the cluster to remove buffer.</summary>
			/// <param name="cluster">cluster whose chromosomes should be purged.</param>
			/// <param name="index">indef of cluster.</param>
			inline void GACALL operator ()(GaSPEACluster& cluster,
				int index)
			{
				const Population::GaChromosomeStorage* center = cluster.GetCenter();

				// add all chromosomes in cluster to remove buffer
				for( const Common::Data::GaListNode<Population::GaChromosomeStorage*>* it1 = cluster.GetChromosomes().GetHead(); it1; it1 = it1->GetNext() )
				{
					Population::GaChromosomeStorage* current = it1->GetValue();

					// just keep central chromosome
					if( current != center )
						current->GetFlags().SetFlags( Population::GaChromosomeStorage::GACF_REMOVE_CHROMOSOME );
				}

				// clear cluster entry in the buffer
				cluster.GetChromosomes().Clear();
			}

		};

		// Prepares population for scaling operation
		void GaSPEA::Prepare(Population::GaPopulation& population,
			const Population::GaScalingParams& parameters,
			const Population::GaScalingConfig& configuration,
			int branchCount) const
		{
			const GaSPEAParams& params = (const GaSPEAParams&)parameters;

			// add required chromosome tags
			population.GetChromosomeTagManager().AddTag( params.GetDominanceListTagID(), Common::Data::GaSizableTagLifecycle<GaDominanceList>( branchCount ) );
			population.GetChromosomeTagManager().AddTag( params.GetStrengthTagID(), Common::Data::GaTypedTagLifecycle<Common::Threading::GaAtomic<int> >() );
			population.GetChromosomeTagManager().AddTag( params.GetDominatedTagID(), Common::Data::GaTypedTagLifecycle<int>() );

			// add required population tags
			population.GetTagManager().AddTag( params.GetClusterStorageTagID(), Common::Data::GaTypedTagLifecycle<GaSPEAClusterStorage>() );

			GaScalingOperation::Prepare( population, parameters, configuration, branchCount );
		}

		// Removes data that was needed by the scaling operation
		void GaSPEA::Clear(Population::GaPopulation& population,
			const Population::GaScalingParams& parameters,
			const Population::GaScalingConfig& configuration,
			int branchCount) const
		{
			const GaSPEAParams& params = (const GaSPEAParams&)parameters;

			// remove used chromosome tags
			population.GetChromosomeTagManager().RemoveTag( params.GetDominanceListTagID() );
			population.GetChromosomeTagManager().RemoveTag( params.GetStrengthTagID() );
			population.GetChromosomeTagManager().RemoveTag( params.GetDominatedTagID() );

			// remove required population tags
			population.GetTagManager().RemoveTag( params.GetClusterStorageTagID() );

			GaScalingOperation::Clear( population, parameters, configuration, branchCount );
		}

		// Updates population for scaling operation
		void GaSPEA::Update(Population::GaPopulation& population,
			const Population::GaScalingParams& parameters,
			const Population::GaScalingConfig& configuration,
			int branchCount) const
		{
			// update chromosome tags
			population.GetChromosomeTagManager().UpdateTag( ( (const GaSPEAParams&)parameters ).GetDominanceListTagID(), GaDominanceListUpdate( branchCount ) );

			GaScalingOperation::Update( population, parameters, configuration, branchCount );
		}

		// Execute scaling operation
		void GaSPEA::Exec(Population::GaPopulation& population,
			const GaSPEAParams& parameters,
			const Population::GaScalingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			Population::GaOperationTime timer( population, Population::GADV_SCALING_TIME );

			int branchID = branch->GetFilteredID();
			int branchCount = branch->GetBarrierCount();

			// population tags
			GaSPEAClusterStorage& clusters = population.GetTagByID<GaSPEAClusterStorage>( parameters.GetClusterStorageTagID() );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
			{
				// mark population as re-scaled
				population.GetFlags().SetFlags( Population::GaPopulation::GAPF_COMPLETE_SCALED_FITNESS_UPDATE );

				// update buffer sizes when population size is changed
				int populationSize = population.GetPopulationParams().GetPopulationSize();
				clusters.SetSize( populationSize, branchCount );
			}

			// chromosome tags
			Population::GaChromosomeTagGetter<GaDominanceList> getDomList( parameters.GetDominanceListTagID(), population.GetChromosomeTagManager() );
			Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > getStrength( parameters.GetStrengthTagID(), population.GetChromosomeTagManager() );
			Population::GaChromosomeTagGetter<int> getDominated( parameters.GetDominatedTagID(), population.GetChromosomeTagManager() );

			clusters.ClearDistanceInfo( branchID );

			Common::Workflows::GaParallelExec1<Population::GaPopulation, Population::GaChromosomeStorage> popWorkDist( *branch, population );
			Common::Workflows::GaParallelExec2<Population::GaPopulation, Population::GaChromosomeStorage> domWorkDist( *branch, population );

			// determins dominant chromosomes, assign strength and fitness to chromosomes and create clusters
			domWorkDist.Execute( GaSPEADominanceOp( population, getDomList, getDominated, branchID ), true );
			popWorkDist.Execute( GaSPEAStrengthOp( getStrength, getDomList, getDominated, clusters ), true );
			popWorkDist.Execute( GaSPEAFitnessAssignOp( getStrength, getDominated, population.GetCount() ), false );

			int count, start, clusterCount = clusters.GetCount();
			branch->SplitWork( clusterCount, count, start );
			Common::Workflows::GaParallelExec1<GaSPEAClusterStorage, GaSPEACluster> clustWorkDist( *branch, clusters );

			// merge clusters so they can fit 
			int requiredClusters = population.GetPopulationParams().GetPermanentSpaceSize();
			for( int currentClusterCount = clusterCount; currentClusterCount > requiredClusters; currentClusterCount-- )
			{
				// merge closest clusters
				GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
					clusters.MergeClosest();

				// cluster pair found by current branch has been merged?
				if( clusters.IsMerged( branchID ) )
				{
					// compare all clasters again
					clusters.ClearDistanceInfo( branchID );
					clustWorkDist.Execute( GaSPEAFindClosestOp( clusters, branchID ), false );
				}
				else
					// just check new cluster if it is closer to others
					clustWorkDist.Execute( GaSPEAUpdateClosestOp( clusters, *clusters.GetMerged(), branchID ), false );
			}

			branch->GetBarrier().Enter( true, branchCount );

			// get chromosomes that should be removed
			clustWorkDist.Execute( GaSPEAPurgeClustersOp(), false );

			// update operation time statistics
			timer.UpdateStatistics();
		}

		/// <summary><c>GaSPEA2NeighbourBuffer</c> class stores distances to chromosome's neighbours.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaSPEA2NeighbourBuffer
		{

		private:

			/// <summary>Pointer to array that is currently used to store chromosome's neighbours.</summary>
			int* _currentNeighbours;

			/// <summary>Original array used for chromosome's neighbours.</summary>
			Common::Data::GaSingleDimensionArray<int> _neighbours;

			/// <summary>Helper array used for sorting.</summary>
			Common::Data::GaSingleDimensionArray<int> _helper;

			/// <summary>Array that stores distances to other chromosomes.</summary>
			Common::Data::GaSingleDimensionArray<float> _distances;

			/// <summary>Number of chromosome's neighbours.</summary>
			int _count;

			/// <summary>Algorithm for sorting chromosome's neighbours.</summary>
			Common::Sorting::GaMergeSortAlgorithm<int> _sorting;

			/// <summary><c>GaNeighbourSort</c> class represents criterion for sorting neighbours according to their distance from chromosome.</summary>
			class GaNeighbourSort : public Common::Sorting::GaSortingCriteria<int>
			{

			private:

				/// <summary>Array that stores distances to other chromosomes.</summary>
				const Common::Data::GaSingleDimensionArray<float>& _distances;

			public:

				/// <summary>Initializes sorting criterion.</summary>
				/// <param name="distances">array that stores distances to other chromosomes.</param>
				GaNeighbourSort(const Common::Data::GaSingleDimensionArray<float>& distances) : _distances(distances) { }

				/// <summary>><c>operator ()</c> compares distances of two neighbours returns results of comparison.
				///
				/// This operator is thread-safe.</summary>
				/// <param name="object1">the first neighbour that is being compared.</param>
				/// <param name="object2">the second neighbour that is being compared.</param>
				/// <returns>Method should returns:
				/// <br/>a.  1 if the first neighbour is closer,
				/// <br/>b. -1 if the second neighbour is closer,
				/// <br/>c.  0 if both neighbours are at the same distance.</returns>
				inline int GACALL operator ()(GaConstType& object1,
					GaConstType& object2) const { return _distances[ object1 ] > _distances[ object2 ] ? 1 : (  _distances[ object2 ] > _distances[ object1 ] ? -1 : 0 ); }

			};

		public:

			/// <summary>This constructor initailizes buffer that can store specified number of neighbours.</summary>
			/// <param name="maxSize"></param>
			GaSPEA2NeighbourBuffer(int maxSize) : _neighbours(maxSize),
				_helper(maxSize),
				_distances(maxSize),
				_currentNeighbours(_neighbours.GetArray()),
				_count(0),
				_sorting(_neighbours.GetArray(), _helper.GetArray(), &_count, true) { _currentNeighbours = _neighbours.GetArray(); }

			/// <summary>This constructor creates copy of neighbours buffer.</summary>
			/// <param name="rhs">buffer that should be copied.</param>
			GaSPEA2NeighbourBuffer(const GaSPEA2NeighbourBuffer& rhs) : _neighbours(rhs.GetSize()),
				_helper(rhs.GetSize()),
				_distances(rhs.GetSize()),
				_currentNeighbours(_neighbours.GetArray()),
				_count(rhs._count),
				_sorting(_neighbours.GetArray(), _helper.GetArray(), &_count, true) { _currentNeighbours = _neighbours.GetArray(); }

			/// <summary>This constructor initializes empty neighbours buffer.</summary>
			GaSPEA2NeighbourBuffer() : _currentNeighbours(NULL),
				_count(0),
				_sorting(true) { }

			/// <summary><c>Insert</c> method stores distance to specified neighbour.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="neighbourIndex">neighbour whose distance should be stored.</param>
			/// <param name="distance">distance to neighbour.</param>
			inline void GACALL Insert(int neighbourIndex,
				float distance)
			{
				_currentNeighbours[ neighbourIndex ] = neighbourIndex;
				_distances[ neighbourIndex ] = distance;
			}

			/// <summary><c>Sort</c> method sorts neighbours according to their distances from chromosome.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL Sort() { _currentNeighbours = _sorting.Sort( GaNeighbourSort( _distances ) ); }

			/// <summary><c>Remove</c> method marks neighbour that is removed from the population.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="neighbourIndex">index of neighbour that was removed from population.</param>
			inline void GACALL Remove(int neighbourIndex) { _distances[ neighbourIndex ] = -2.0f; }

			/// <summary><c>GetFirstNeighbour</c> method queries index and distance to closest neighbour.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="index">reference to varaible that will store index of closest neighbour.</param>
			/// <param name="distance">reference to variable that will store distance to closest neighbour.</param>
			inline void GACALL GetFirstNeighbour(int& index,
				float& distance) const
			{
				index = _currentNeighbours[ index ];
				distance = _distances[ index ];

				// use first neighbour from sorted list that is not removed
				for( int m = 2; m < _count && distance < 0; m++ )
				{
					index = _currentNeighbours[ m ];
					distance = _distances[ index ];
				}
			}

			/// <summary><c>SetSize</c> method sets number of neighbours that buffer can store.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="size">number of neighbours that buffer should store.</param>
			void GACALL SetSize(int size)
			{
				// set size of buffers
				_neighbours.SetSize( size );
				_helper.SetSize( size );
				_distances.SetSize( size );
				_count = 0;

				// update sorting algorithm
				_sorting.SetArray( _neighbours.GetArray(), _helper.GetArray(), &_count );
				_currentNeighbours = _neighbours.GetArray();
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of neighbours that buffer can store.</returns>
			inline int GACALL GetSize() const { return _neighbours.GetSize(); }

			/// <summary><c>SetCount</c>> method sets number of neighbour that chromosome has.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="count">number of chromosome's neighbours.</param>
			inline void GACALL SetCount(int count) { _count = count; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Mehtod returns number of neighbours currently in the buffer.</returns>
			inline int GACALL GetCount() const { return _count; }

			/// <summary><c>operator []</c> method returns distance to specified neighbour.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="index">index of queried neighbour.</param>
			/// <returns>Operator returns distance to specified neighbour.</returns>
			inline float GACALL operator [](int index) const { return _distances[ _currentNeighbours[ index ] ]; }

		};

		/// <summary>Datatype used for updating chromosome that when size of chromosome's neighbours buffer should be changed.</summary>
		typedef Common::Data::GaSizableTagUpdate<GaSPEA2NeighbourBuffer, Common::Data::GaTagIgnoreMerge<GaSPEA2NeighbourBuffer> > GaSPEA2NeighbourBufferUpdate;

		/// <summary><c>GaSPEA2Distance</c> struct stores distance between two chromosomes.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		struct GaSPEA2Distance
		{

			/// <summary>Index of the first chromosome.</summary>
			int _indexA;

			/// <summary>Index of the second chromosome.</summary>
			int _indexB;

			/// <summary>Distance between chromosomes.</summary>
			float _distance;

			/// <summary>Chromosome's neighbours buffer from which the distance is fetched.</summary>
			GaSPEA2NeighbourBuffer* _kthNeighbour;

			/// <summary>This chsomosome initializes empty distance object.</summary>
			GaSPEA2Distance() { Clear(); }

			/// <summary><c>Clear</c> method clears stored distance information.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL Clear()
			{
				_indexA = -1;
				_indexB = -1;
				_distance = std::numeric_limits<float>::infinity();
				_kthNeighbour = NULL;
			}

			/// <summary><c>Store</c> method stores distance inforamtion.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="indexA">index of the first chromosome.</param>
			/// <param name="indexB">index of the second chromosome.</param>
			/// <param name="distance">distance between chromosomes.</param>
			/// <param name="kthNeighbour">chromosome's neighbours buffer from which the distance is fetched.</param>
			inline void GACALL Store(int indexA,
				int indexB,
				float distance,
				GaSPEA2NeighbourBuffer* kthNeighbour)
			{
				// store provided inforamtion
				_indexA = indexA;
				_indexB = indexB;
				_distance = distance;
				_kthNeighbour = kthNeighbour;
			}

		};

		/// <summary><c>GaSPEA2DistanceStorage</c> class provides storage for disntance information for closest chromosomes identified by each branch that execute operation.</summary>
		class GaSPEA2DistanceStorage
		{

		private:

			/// <summary>Distance information for closest chromosomes identified by each chromosome.</summary>
			Common::Data::GaSingleDimensionArray<GaSPEA2Distance> _minDistances;

			/// <summary>Index of chromosomes that is identified as closest to its neighbours.</summary>
			int _gloablMinDistance;

		public:

			/// <summary>This constructor initializes storage with its size.</summary>
			/// <param name="size">number of branches that will use storage.</param>
			GaSPEA2DistanceStorage(int size = 0) : _minDistances(size),
				_gloablMinDistance(-1) { }

			/// <summary><<c>RequiresUpdate</c> method checks whether specifed branch identified chromosome that is closest to its neighbours.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="branchID">ID of branch that should be checked.</param>
			/// <returns>Method returns <c>true</c> if specified branch identified the chromosome.</returns>
			inline bool GACALL RequiresUpdate(int branchID) const
				{ return _minDistances[ branchID ]._indexA == _gloablMinDistance || _minDistances[ branchID ]._indexB == _gloablMinDistance; }

			/// <summary><c>CalculateGlobalMinDistance</c> method identifies which chromosome is closest to its neighbours among those identified by each branch.
			///
			/// This method is not thread-safe.</summary>
			/// <returns>Method returns index of identified chromosome.</returns>
			int GACALL CalculateGlobalMinDistance()
			{
				// assume that the first branch found closest chromosome
				float minDist = _minDistances[ 0 ]._distance;
				_gloablMinDistance = _minDistances[ 0 ]._indexA;

				// check if other branches found closer pair
				for( int i = _minDistances.GetSize() - 1; i > 0; i-- )
				{
					// is current pair is closer than previously identified pair
					if( _minDistances[ i ]._distance < minDist )
					{
						// use new pair
						minDist = _minDistances[ i ]._distance;
						_gloablMinDistance = _minDistances[ i ]._indexA;
					}
				}

				return _gloablMinDistance;
			}

			/// <summary><c>RestartGlobalMinDistance</c> method clears previously identified chromosome that is closest to its neighbours.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL RestartGlobalMinDistance() { _gloablMinDistance = -1; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns index of chromosomes that is identified as closest to its neighbours.</returns>
			inline int GACALL GetGlobalMinDistance() const { return _gloablMinDistance; }

			/// <summary><c>SetSize</c> method sets size of storage.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="size">number of branches that will use storage.</param>
			inline void GACALL SetSize(int size) { _minDistances.SetSize( size ); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns size of storage.</returns>
			inline int GACALL GetSize() const { return _minDistances.GetSize(); }

			/// <summary><c>operator []</c> provides acces to storage object for specifed branch.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="branchID">ID of branch whose storage is queried.</param>
			/// <returns>Method returns reference to storage object of specifed branch.</returns>
			inline GaSPEA2Distance& GACALL operator [](int branchID) { return _minDistances[ branchID ]; }

			/// <summary><c>operator []</c> provides acces to storage object for specifed branch.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="branchID">ID of branch whose storage is queried.</param>
			/// <returns>Method returns reference to storage object of specifed branch.</returns>
			inline const GaSPEA2Distance& GACALL operator [](int branchID) const { return _minDistances[ branchID ]; }

		};

		/// <summary><c>GaSPEA2DominanceOp</c> class represents operation that compares and determins dominant chromosome, updates dominated chromosome lists
		/// and calculates distances between chromosomes.</summary>
		class GaSPEA2DominanceOp
		{

		private:

			/// <summary>Population over which the dominance operation is performed.</summary>
			Population::GaPopulation& _population;

			/// <summary>Chromosome tag that contains partial list of dominated chromosome.</summary>
			Population::GaChromosomeTagGetter<GaDominanceList> _domList;

			/// <summary>Chromosome tag that stores information whether the chromosome is dominated by any other chromosome in the population.</summary>
			Population::GaChromosomeTagGetter<int> _dominated;

			/// <summary>Chromosome tag that stores chromosome's neighbour buffer.</summary>
			Population::GaChromosomeTagGetter<GaSPEA2NeighbourBuffer> _kthNeighbour;

			/// <summary>ID of the brach that executes operation.</summary>
			int _branchID;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="population">population over which the dominance operation is performed.</param>
			/// <param name="domList">chromosome tag that contains partial list of dominated chromosome.</param>
			/// <param name="dominated">chromosome tag that stores information whether the chromosome is dominated by any other chromosome in the population.</param>
			/// <param name="kthNeighbour">chromosome tag that stores chromosome's neighbour buffer.</param>
			/// <param name="branchID">ID of the brach that executes operation.</param>
			GaSPEA2DominanceOp(Population::GaPopulation& population,
				const Population::GaChromosomeTagGetter<GaDominanceList>& domList,
				const Population::GaChromosomeTagGetter<int>& dominated,
				const Population::GaChromosomeTagGetter<GaSPEA2NeighbourBuffer> kthNeighbour,
				int branchID) : _population(population),
				_domList(domList),
				_dominated(dominated),
				_kthNeighbour(kthNeighbour),
				_branchID(branchID) { }

			/// <summary><c>operator ()</c> preapres chromosome's neighbour buffer.</summary>
			/// <param name="chromosome">chromosome whose buffer should be prepared.</param>
			/// <param name="index">indx of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				GaSPEA2NeighbourBuffer& neighbours = _kthNeighbour( chromosome );

				// prepare buffer
				neighbours.SetCount( _population.GetCount() );
				neighbours.Insert( index, -1.0f );
			}

			/// <summary><c>operator ()</c> determins dominant chromosome, update dominance list of chromosomes and calculates distance between chromosomes.</summary>
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
				int dom = _population.CompareFitness( chromosome1, chromosome2 );

				// is the first chromosome dominant?
				if( dom < 0 )
				{
					// mark dominated chromosome and update dominance list 
					_domList( chromosome1 )[ _branchID ].InsertTail( &chromosome2 );
					_dominated( chromosome2 ) = 1;
				}
				// is the second chromosome dominant?
				else if( dom > 0 )
				{
					// mark dominated chromosome and update dominance list 
					_domList( chromosome2 )[ _branchID ].InsertTail( &chromosome1 );
					_dominated( chromosome1 ) = 1;
				}

				// calculate and store distance between chromosomes
				float dist = chromosome1.GetFitness( Population::GaChromosomeStorage::GAFT_RAW ).Distance( chromosome2.GetFitness( Population::GaChromosomeStorage::GAFT_RAW ) );
				_kthNeighbour( chromosome1 ).Insert( index2, dist );
				_kthNeighbour( chromosome2 ).Insert( index1, dist );
			}

		};

		/// <summary><c>GaSPEAStrengthOp</c> class represents operation that assigns strength to chromosomes and idnetify clusters.</summary>
		class GaSPEA2StrengthOp
		{

		private:

			/// <summary>Chromosome tag that stores strength value of the chromosome used to calculate scaled fitness.</summary>
			Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > _strength;

			/// <summary>Chromosome tag that contains partial list of dominated chromosome.</summary>
			Population::GaChromosomeTagGetter<GaDominanceList> _domList;

			/// <summary>Chromosome tag that stores information whether the chromosome is dominated by any other chromosome in the population.</summary>
			Population::GaChromosomeTagGetter<int> _dominated;

			/// <summary>Chromosome tag that stores chromosome's neighbour buffer.</summary>
			Population::GaChromosomeTagGetter<GaSPEA2NeighbourBuffer> _kthNeighbour;

			/// <summary>Reference to variable that stores number of nondomnated chromosomes that are identified in the population.</summary>
			Common::Threading::GaAtomic<int>& _nondominatedCount;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="population">population over which the dominance operation is performed.</param>
			/// <param name="domList">chromosome tag that contains partial list of dominated chromosome.</param>
			/// <param name="dominated">chromosome tag that stores information whether the chromosome is dominated by any other chromosome in the population.</param>
			/// <param name="kthNeighbour">chromosome tag that stores chromosome's neighbour buffer.</param>
			/// <param name="nondominatedCount">reference to variable that stores number of nondomnated chromosomes that are identified in the population.</param>
			GaSPEA2StrengthOp(const Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> >& strength,
				const Population::GaChromosomeTagGetter<GaDominanceList>& domList,
				const Population::GaChromosomeTagGetter<int>& dominated,
				const Population::GaChromosomeTagGetter<GaSPEA2NeighbourBuffer> kthNeighbour,
				Common::Threading::GaAtomic<int>& nondominatedCount) : _strength(strength),
				_domList(domList),
				_dominated(dominated),
				_kthNeighbour(kthNeighbour),
				_nondominatedCount(nondominatedCount) { }

			/// <summary><c>operator ()</c> calculates strength of chromosome and counts nondominated chromosomes.</summary>
			/// <param name="chromosome">chromosome whose strength should be calculated.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				// sort chromosome's neighbours according to their distance
				_kthNeighbour( chromosome ).Sort();

				GaDominanceList& lst = _domList( chromosome );

				// get number of chromosomes that are dominated by this chromosome
				int s = 0;
				for( int i = lst.GetSize() - 1; i >= 0; i-- )
					s += lst[ i ].GetCount();

				// update strength of all dominated chromosomes
				for( int i = lst.GetSize() - 1; i >= 0; i-- )
				{
					for( Common::Data::GaListNode<Population::GaChromosomeStorage*>* it = lst[ i ].GetHead(); it; it = it->GetNext() )
						_strength( it->GetValue() ) += s;

					lst[ i ].Clear();
				}

				// count if chromosome is nondominated
				if( !_dominated( chromosome ) )
					++_nondominatedCount;
			}

		};

		/// <summary><c>GaSPEAFitnessAssignOp</c> class represents operation that assigns scaled fitness to chromosomes based on their strength
		/// and distance from their neighbours.</summary>
		class GaSPEA2FitnessAssignOp
		{

		private:

			/// <summary>Chromosome tag that stores strength value of the chromosome used to calculate scaled fitness.</summary>
			Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > _strength;

			/// <summary>Chromosome tag that contains partial list of dominated chromosome.</summary>
			Population::GaChromosomeTagGetter<int> _dominated;

			/// <summary>Chromosome tag that stores chromosome's neighbour buffer.</summary>
			Population::GaChromosomeTagGetter<GaSPEA2NeighbourBuffer> _kthNeighbour;

			/// <summary>Kth nearest neighbour whose distances is incorporated in calculation of scaled fitness.</summary>
			int _k;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="strength">chromosome tag that stores strength value of the chromosome used to calculate scaled fitness.</param>
			/// <param name="dominated">chromosome tag that contains partial list of dominated chromosome.</param>
			/// <param name="kthNeighbour">chromosome tag that stores chromosome's neighbour buffer.</param>
			/// <param name="k">Kth nearest neighbour whose distances is incorporated in calculation of scaled fitness</param>
			GaSPEA2FitnessAssignOp(const Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> >& strength,
				const Population::GaChromosomeTagGetter<int>& dominated,
				const Population::GaChromosomeTagGetter<GaSPEA2NeighbourBuffer> kthNeighbour,
				int k) : _strength(strength),
				_dominated(dominated),
				_kthNeighbour(kthNeighbour),
				_k(k) { }

			/// <summary><c>operator ()</c> calculates and assigns scaled fitness to chromosome.</summary>
			/// <param name="chromosome">chromosome whose fitness should be calculated.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				// get distance to kth neighbour
				float kth = 1 + _kthNeighbour( chromosome )[ _k ];

				// calculate fitness and assign it to chromosome
				float value =  _dominated( chromosome ) ? 1 / ( 1 / kth + _strength( chromosome ) ) : kth;
				//float value =  _dominated( chromosome ) ? kth / ( 1 + _strength( chromosome ) ) : kth;
				( (GaSPEAFitness&)chromosome.GetFitness( Population::GaChromosomeStorage::GAFT_SCALED ) ).SetValue( value );

				// clear chromosome tags for next generation
				_strength( chromosome ) = 0;
				_dominated( chromosome ) = 0;
			}
		};

		/// <summary><c>GaSPEA2FindNearestOp</c> class represents operation that finds pair of nearest chromosomes.</summary>
		class GaSPEA2FindNearestOp
		{

		private:

			/// <summary>Chromosome tag that contains partial list of dominated chromosome.</summary>
			Population::GaChromosomeTagGetter<int> _dominated;

			/// <summary>Chromosome tag that stores chromosome's neighbour buffer.</summary>
			Population::GaChromosomeTagGetter<GaSPEA2NeighbourBuffer> _kthNeighbour;

			/// <summary>Object that stores distance information of pair of nearest chromosomes.</summary>
			GaSPEA2Distance& _minDist;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="dominated">chromosome tag that contains partial list of dominated chromosome.</param>
			/// <param name="kthNeighbour">chromosome tag that stores chromosome's neighbour buffer.</param>
			/// <param name="minDist">object that stores distance information of pair of nearest chromosomes.</param>
			GaSPEA2FindNearestOp(const Population::GaChromosomeTagGetter<int>& dominated,
				const Population::GaChromosomeTagGetter<GaSPEA2NeighbourBuffer> kthNeighbour,
				GaSPEA2Distance& minDist) : _dominated(dominated),
				_kthNeighbour(kthNeighbour),
				_minDist(minDist) { }

			/// <summary><c>operator ()</c> finds distance to its nearest neighbour and store distance if it is smalles distance found so far.</summary>
			/// <param name="chromosome">chromosome whose distance to nearest neighbour should be calculated.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index)
			{
				GaSPEA2NeighbourBuffer& neighbours1 = _kthNeighbour( chromosome );

				// skip nondominated and removed chromosomes
				if( !_dominated( chromosome ) && neighbours1[ 0 ] > -1.5f )
				{
					// get nearest neighbour
					float dist;
					int indexB = 1;
					neighbours1.GetFirstNeighbour( indexB, dist );

					// are they closer then previously identified pair?
					if( dist < _minDist._distance )
						// store current pair
						_minDist.Store( index, indexB, dist, &neighbours1 );
					// pairs has the same distance?
					else if( dist == _minDist._distance )
					{
						// determins the distance to the next neighbours until one pair is closer
						for( int j1 = 1, j2 = 1, count = neighbours1.GetCount(); j1 < count && j2 < count; )
						{
							// get distance to the next neighbour for current pair
							float d1 = neighbours1[ j1 ];

							// skip removed chromosomes
							if( d1 >= 0 )
							{
								// get distance to the next neighbour for previous pair
								float d2 = ( *_minDist._kthNeighbour )[ j2 ];

								// skip removed chromosomes								
								if( d2 >= 0 )
								{
									// is current pair closer?
									if( d2 > d1 )
										// store current pair
										_minDist.Store( index, indexB, dist, &neighbours1 );

									// stop searching if previous pair is closer?
									if( d2 != d1 )
										break;

									j1++;
								}

								j2++;
							}
							else
								j1++;
						}
					}
				}
			}

		};

		/// <summary><c>GaSPEA2NeighbourUpdateOp</c> class represent operation that marks removed chromosome in neighbours buffers of other chromosomes.</summary>
		class GaSPEA2NeighbourUpdateOp
		{

		private:

			/// <summary>Chromosome tag that stores chromosome's neighbour buffer.</summary>
			Population::GaChromosomeTagGetter<GaSPEA2NeighbourBuffer> _kthNeighbour;

			/// <summary>Index of chromosome that is remved.</summary>
			int _removedIndex;

		public:

			/// <summary>Initializes operation.</summary>
			/// <param name="kthNeighbour">chromosome tag that stores chromosome's neighbour buffer.</param>
			/// <param name="removedIndex">index of chromosome that is remved.</param>
			GaSPEA2NeighbourUpdateOp(const Population::GaChromosomeTagGetter<GaSPEA2NeighbourBuffer> kthNeighbour,
				int removedIndex) : _kthNeighbour(kthNeighbour),
				_removedIndex(removedIndex) { }

			/// <summary><c>operator ()</c> updates chromosome neighbours buffer by marking removed chromosome.</summary>
			/// <param name="chromosome">chromosome whose buffer should be updated.</param>
			/// <param name="index">index of chromosome.</param>
			inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
				int index) { _kthNeighbour( chromosome ).Remove( _removedIndex ); }

		};

		// Prepares population for scaling operation
		void GaSPEA2::Prepare(Population::GaPopulation& population,
			const Population::GaScalingParams& parameters,
			const Population::GaScalingConfig& configuration,
			int branchCount) const
		{
			const GaSPEA2Params& params = (const GaSPEA2Params&)parameters;

			// add required chromosome tags
			population.GetChromosomeTagManager().AddTag( params.GetDominanceListTagID(), Common::Data::GaSizableTagLifecycle<GaDominanceList>( branchCount ) );
			population.GetChromosomeTagManager().AddTag( params.GetStrengthTagID(), Common::Data::GaTypedTagLifecycle<Common::Threading::GaAtomic<int> >() );
			population.GetChromosomeTagManager().AddTag( params.GetDominatedTagID(), Common::Data::GaTypedTagLifecycle<int>() );
			population.GetChromosomeTagManager().AddTag( params.GetKthNeighbourTagID(), Common::Data::GaSizableTagLifecycle<GaSPEA2NeighbourBuffer>() );

			// add required population tags
			population.GetTagManager().AddTag( params.GetNondominatedCountTagID(), Common::Data::GaTypedTagLifecycle<Common::Threading::GaAtomic<int> >() );
			population.GetTagManager().AddTag( params.GetDistanceStorageTagID(), Common::Data::GaTypedTagLifecycle<GaSPEA2DistanceStorage>() );

			GaScalingOperation::Prepare( population, parameters, configuration, branchCount );
		}

		// Removes data that was needed by the scaling operation
		void GaSPEA2::Clear(Population::GaPopulation& population,
			const Population::GaScalingParams& parameters,
			const Population::GaScalingConfig& configuration,
			int branchCount) const
		{
			const GaSPEA2Params& params = (const GaSPEA2Params&)parameters;

			// remove used chromosome tags
			population.GetChromosomeTagManager().RemoveTag( params.GetDominanceListTagID() );
			population.GetChromosomeTagManager().RemoveTag( params.GetStrengthTagID() );
			population.GetChromosomeTagManager().RemoveTag( params.GetDominatedTagID() );
			population.GetChromosomeTagManager().RemoveTag( params.GetKthNeighbourTagID() );

			// remove required population tags
			population.GetTagManager().RemoveTag( params.GetNondominatedCountTagID() );
			population.GetTagManager().RemoveTag( params.GetDistanceStorageTagID() );

			GaScalingOperation::Clear( population, parameters, configuration, branchCount );
		}

		// Updates population for scaling operation
		void GaSPEA2::Update(Population::GaPopulation& population,
			const Population::GaScalingParams& parameters,
			const Population::GaScalingConfig& configuration,
			int branchCount) const
		{
			// update chromosome tags
			population.GetChromosomeTagManager().UpdateTag( ( (const GaSPEA2Params&)parameters ).GetDominanceListTagID(), GaDominanceListUpdate( branchCount ) );

			// update population tags
			population.GetTagByID<GaSPEA2DistanceStorage>( ( (const GaSPEA2Params&)parameters ).GetDistanceStorageTagID() ).SetSize( branchCount );

			GaScalingOperation::Update( population, parameters, configuration, branchCount );
		}

		// Execute scaling operation
		void GaSPEA2::Exec(Population::GaPopulation& population,
			const GaSPEA2Params& parameters,
			const Population::GaScalingConfig& configuration,
			Common::Workflows::GaBranch* branch) const
		{
			Population::GaOperationTime timer( population, Population::GADV_SCALING_TIME );

			int branchID = branch->GetFilteredID();
			int branchCount = branch->GetBarrierCount();

			// population tags
			Common::Threading::GaAtomic<int>& nondominatedCount = population.GetTagByID<Common::Threading::GaAtomic<int> >( parameters.GetNondominatedCountTagID() );
			GaSPEA2DistanceStorage& distances = population.GetTagByID<GaSPEA2DistanceStorage>( parameters.GetDistanceStorageTagID() );

			GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
			{
				// mark population as re-scaled
				population.GetFlags().SetFlags( Population::GaPopulation::GAPF_COMPLETE_SCALED_FITNESS_UPDATE );

				// update buffer sizes when population size is changed
				int populationSize = population.GetPopulationParams().GetPopulationSize();
				population.GetChromosomeTagManager().UpdateTag( parameters.GetKthNeighbourTagID(), GaSPEA2NeighbourBufferUpdate( populationSize ) );

				distances.RestartGlobalMinDistance();
				nondominatedCount = 0;
			}

			// chromosome tags
			Population::GaChromosomeTagGetter<GaDominanceList> getDomList( parameters.GetDominanceListTagID(), population.GetChromosomeTagManager() );
			Population::GaChromosomeTagGetter<Common::Threading::GaAtomic<int> > getStrength( parameters.GetStrengthTagID(), population.GetChromosomeTagManager() );
			Population::GaChromosomeTagGetter<int> getDominated( parameters.GetDominanceListTagID(), population.GetChromosomeTagManager() );
			Population::GaChromosomeTagGetter<GaSPEA2NeighbourBuffer> getKthNeighbour( parameters.GetKthNeighbourTagID(), population.GetChromosomeTagManager() );

			GaSPEA2Distance& minDist = distances[ branchID ];
			minDist.Clear();

			Common::Workflows::GaParallelExec1<Population::GaPopulation, Population::GaChromosomeStorage> popWorkDist( *branch, population );
			Common::Workflows::GaParallelExec2<Population::GaPopulation, Population::GaChromosomeStorage> domWorkDist( *branch, population );

			// determins dominant chromosomes, assign strength and fitness to chromosomes and determins distances between chromosomes
			domWorkDist.Execute( GaSPEA2DominanceOp( population, getDomList, getDominated, getKthNeighbour, branchID ), true );
			popWorkDist.Execute( GaSPEA2StrengthOp( getStrength, getDomList, getDominated, getKthNeighbour, nondominatedCount ), true );
			popWorkDist.Execute( GaSPEA2FitnessAssignOp( getStrength, getDominated, getKthNeighbour, parameters.GetKthNeighbourSize() ), false );

			// identify chromosoms that are closest to their neighbours
			int requiredCount = population.GetPopulationParams().GetPermanentSpaceSize();
			for( int currentCount = nondominatedCount; currentCount > requiredCount; currentCount-- )
			{
				// checks whether the pair identified by this branch has been selected as closest in previous round
				if( distances.RequiresUpdate( branchID ) )
				{
					// branch should find another pair
					minDist.Clear();
					popWorkDist.Execute( GaSPEA2FindNearestOp( getDominated, getKthNeighbour, minDist ), false );
				}

				// get closest chromosome among the ones chosen by the branches and add it to remove buffer
				GA_BARRIER_SYNC( lock, branch->GetBarrier(), branchCount )
					population[ distances.CalculateGlobalMinDistance() ].GetFlags().SetFlags( Population::GaChromosomeStorage::GACF_REMOVE_CHROMOSOME );

				// remove selected chromsome for neighbours buffers of other chromosomes
				popWorkDist.Execute( GaSPEA2NeighbourUpdateOp( getKthNeighbour, distances.GetGlobalMinDistance() ), true );
			}

			// update operation time statistics
			timer.UpdateStatistics();
		}

	} // SPEA
} // Multiobjective

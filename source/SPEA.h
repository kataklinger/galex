
/*! \file SPEA.h
    \brief This file conatins declaration of classes that implement Strength Pareto Evolutionary Algorithm I and II (SPEA-I and SPEA-II).
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_SPEA_H__
#define __GA_SPEA_H__

#include "Population.h"
#include "FitnessValues.h"

namespace Multiobjective
{

	/// <summary>Contains classes and datatypes that implement Strength Pareto Evolutionary Algorithm I and II (SPEA-I and SPEA-II).</summary>
	namespace SPEA
	{

		/// <summary><c>GaSPEAParamsBase</c> represents base class for parameters used by implementations of Strength Pareto Evolutionary Algorithm.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaSPEAParamsBase : public Population::GaScalingParams
		{

		protected:

			/// <summary>ID of chromosome tag that stores partial list of chromosomes that are dominated by the chromosome.</summary>
			int _dominanceListTagID;

			/// <summary>ID of chromosome tag that stores strength value of the chromosome used to calculate scaled fitness.</summary>
			int _strengthTagID;

			/// <summary>ID of chromosome tag that indicates whether the chromosome is dominated by any other.</summary>
			int _dominatedTagID;

		public:

			/// <summary>This constructor initializes parameters with used-defined values.</summary>
			/// <param name="dominanceListTagID">ID of chromosome tag that stores partial list of chromosomes that are dominated by the chromosome.</param>
			/// <param name="strengthTagID">ID of chromosome tag that stores strength value of the chromosome used to calculate scaled fitness.</param>
			/// <param name="dominatedTagID">ID of chromosome tag that indicates whether the chromosome is dominated by any other.</param>
			GaSPEAParamsBase(int dominanceListTagID,
				int strengthTagID,
				int dominatedTagID) : _dominanceListTagID(dominanceListTagID),
				_strengthTagID(strengthTagID),
				_dominatedTagID(dominatedTagID) { }

			/// <summary>This constructor initializes parameters with default values.</summary>
			GaSPEAParamsBase() : _dominanceListTagID(-1),
				_strengthTagID(-1),
				_dominatedTagID(-1) { }

			/// <summary><c>SetDominanceListTagID<c/> method sets chromosome tag that stores partial list of chromosomes that are dominated by the chromosome.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetDominanceListTagID(int tagID) { _dominanceListTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of chromosome tag that stores partial list of chromosomes that are dominated by the chromosome.</returns>
			inline int GACALL GetDominanceListTagID() const { return _dominanceListTagID; }

			/// <summary><c>SetStrengthTagID<c/> method sets chromosome tag that stores strength value of the chromosome used to calculate scaled fitness.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetStrengthTagID(int tagID) { _strengthTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of chromosome tag that stores strength value of the chromosome used to calculate scaled fitness.</returns>
			inline int GACALL GetStrengthTagID() const { return _strengthTagID; }

			/// <summary><c>SetDominatedTagID<c/> method sets chromosome tag that indicates whether the chromosome is dominated by any other.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void SetDominatedTagID(int tagID) { _dominatedTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of chromosome tag that indicates whether the chromosome is dominated by any other.</returns>
			inline int GACALL GetDominatedTagID() const { return _dominatedTagID; }

		};

		/// <summary><c>GaSPEAParams</c> class represents parameters for implementation of Strength Pareto Evolutionary Algorithm.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaSPEAParams : public GaSPEAParamsBase
		{

		protected:

			/// <summary>ID of population tag that stores information about chromosome's clusters.</summary>
			int _clusterStorageTagID;

		public:

			/// <summary>This constructor initializes parameters with used-defined values.</summary>
			/// <param name="dominanceListTagID">ID of chromosome tag that stores partial list of chromosomes that are dominated by the chromosome.</param>
			/// <param name="strengthTagID">ID of chromosome tag that stores strength value of the chromosome used to calculate scaled fitness.</param>
			/// <param name="dominatedTagID">ID of chromosome tag that indicates whether the chromosome is dominated by any other.</param>
			/// <param name="clusterStorageTagID">ID of population tag that stores information about chromosome's clusters.</param>
			GaSPEAParams(int dominanceListTagID,
				int strengthTagID,
				int dominatedTagID,
				int clusterStorageTagID) : GaSPEAParamsBase(dominanceListTagID, strengthTagID, dominatedTagID),
				_clusterStorageTagID(clusterStorageTagID) { }

			/// <summary>This constructor initializes parameters with default values.</summary>
			GaSPEAParams() : _clusterStorageTagID(-1) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaSPEAParams( *this ); }

			/// <summary><c>SetClusterStorageTagID<c/> method sets population tag that stores information about chromosome's clusters.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetClusterStorageTagID(int tagID) { _clusterStorageTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of population tag that stores information about chromosome's clusters.</returns>
			inline int GACALL GetClusterStorageTagID() const { return _clusterStorageTagID; }

		};

		/// <summary><c>GaSPEAFitness</c> defines fitness type that SPEA and SPEAII use to scaled fitness. Detailed description can be found in specification of
		/// <see cref="GaMVFitness" /> template class.</summary>
		typedef Fitness::Representation::GaSVFitness<float> GaSPEAFitness;

		/// <summary><c>GaSPEA</c> class repesents scaling operation that implements Strength Pareto Evolutionary Algorithm.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaSPEA : public Population::GaScalingOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::Prepare" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Prepare(Population::GaPopulation& population,
				const Population::GaScalingParams& parameters,
				const Population::GaScalingConfig& configuration,
				int branchCount) const;

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::Clear" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Clear(Population::GaPopulation& population,
				const Population::GaScalingParams& parameters,
				const Population::GaScalingConfig& configuration,
				int branchCount) const;

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::Update" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Update(Population::GaPopulation& population,
				const Population::GaScalingParams& parameters,
				const Population::GaScalingConfig& configuration,
				int branchCount) const;

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL operator ()(Population::GaPopulation& population,
				const Population::GaScalingParams& parameters,
				const Population::GaScalingConfig& configuration,
				Common::Workflows::GaBranch* branch) const { Exec( population, (const GaSPEAParams&)parameters, configuration, branch ); }

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaSPEAParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaSPEAParams(); }

			/// <summary>More details are given in specification of <see cref="GaFitnessOperation::CreateFitnessObject" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaSPEAFitness" /> class.</returns>
			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const
				{ return new GaSPEAFitness( params ); }

		protected:

			/// <summary><c>Exec</c> method executes operation. Paramenters has same meaning that is defined by <c>operator ()</c>.</summary>
			GAL_API
			void GACALL Exec(Population::GaPopulation& population,
				const GaSPEAParams& parameters,
				const Population::GaScalingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

		};

		/// <summary><c>GaSPEA2Params</c> class represents parameters for implementation of Improved Strength Pareto Evolutionary Algorithm.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaSPEA2Params : public GaSPEAParamsBase
		{

		protected:

			/// <summary>ID of population tag that stores distances between closest chromosomes.</summary>
			int _distanceStorageTagID;

			/// <summary>ID of population tag that stores number of nondominated chromosomes in the population.</summary>
			int _nondominatedCountTagID;

			/// <summary>ID of chromosomes tag that stores distances of chromosome to other chromosomes.</summary>
			int _kthNeighbourTagID;

			/// <summary>Chromosome's Kth nearest neighbour whose distances is incorporated in calculation of scaled fitness.</summary>
			int _kthNeighbourSize;

		public:

			/// <summary>This constructor initializes parameters with used-defined values.</summary>
			/// <param name="dominanceListTagID">ID of chromosome tag that stores partial list of chromosomes that are dominated by the chromosome.</param>
			/// <param name="strengthTagID">ID of chromosome tag that stores strength value of the chromosome used to calculate scaled fitness.</param>
			/// <param name="dominatedTagID">ID of chromosome tag that indicates whether the chromosome is dominated by any other.</param>
			/// <param name="distanceStorageTagID">ID of population tag that stores distances between closest chromosomes.</param>
			/// <param name="nondominatedCountTagID">ID of population tag that stores number of nondominated chromosomes in the population.</param>
			/// <param name="kthNeighbourTagID">ID of chromosomes tag that stores distances of chromosome to other chromosomes.</param>
			/// <param name="kthNeighbourSize">chromosome's Kth nearest neighbour whose distances is incorporated in calculation of scaled fitness.</param>
			GaSPEA2Params(int dominanceListTagID,
				int strengthTagID,
				int dominatedTagID,
				int distanceStorageTagID,
				int nondominatedCountTagID,
				int kthNeighbourTagID,
				int kthNeighbourSize) : GaSPEAParamsBase(dominanceListTagID, strengthTagID, dominatedTagID),
				_distanceStorageTagID(distanceStorageTagID),
				_nondominatedCountTagID(nondominatedCountTagID),
				_kthNeighbourTagID(kthNeighbourTagID),
				_kthNeighbourSize(kthNeighbourSize) { }

			/// <summary>This constructor initializes parameters with default values.</summary>
			GaSPEA2Params() : _distanceStorageTagID(-1),
				_nondominatedCountTagID(-1),
				_kthNeighbourTagID(-1),
				_kthNeighbourSize(0) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaSPEA2Params( *this ); }

			/// <summary><c>SetDistanceStorageTagID<c/> method sets population tag that stores distances between closest chromosomes.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetDistanceStorageTagID(int tagID) { _distanceStorageTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of population tag that stores distances between closest chromosomes.</returns>
			inline int GACALL GetDistanceStorageTagID() const { return _distanceStorageTagID; }

			/// <summary><c>SetNondominatedCountTagID<c/> method sets population tag that stores number of nondominated chromosomes in the population.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetNondominatedCountTagID(int tagID) { _nondominatedCountTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of population tag that stores number of nondominated chromosomes in the population.</returns>
			inline int GACALL GetNondominatedCountTagID() const { return _nondominatedCountTagID; }

			/// <summary><c>SetKthNeighbourTagID<c/> method sets chromosomes tag that stores distances of chromosome to other chromosomes.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetKthNeighbourTagID(int tagID) { _kthNeighbourTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of chromosomes tag that stores distances of chromosome to other chromosomes.</returns>
			inline int GACALL GetKthNeighbourTagID() const { return _kthNeighbourTagID; }

			/// <summary><c>SetKthNeighbourSize</c> method sets chromosome's Kth nearest neighbour whose distances is incorporated in calculation of scaled fitness.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="size">Kth chromosome neighbour.</param>
			inline void GACALL SetKthNeighbourSize(int size)
			{
				GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, size >= 0, "size", "Size must be greater or eqaul to 0.", "Scalings" );
				_kthNeighbourSize = size;
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns chromosome's Kth nearest neighbour whose distances is incorporated in calculation of scaled fitness.</returns>
			inline int GACALL GetKthNeighbourSize() const { return _kthNeighbourSize; }

		};

		/// <summary><c>GaSPEA2</c> class repesents scaling operation that implements Improved Strength Pareto Evolutionary Algorithm.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaSPEA2 : public Population::GaScalingOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::Prepare" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Prepare(Population::GaPopulation& population,
				const Population::GaScalingParams& parameters,
				const Population::GaScalingConfig& configuration,
				int branchCount) const;

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::Clear" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Clear(Population::GaPopulation& population,
				const Population::GaScalingParams& parameters,
				const Population::GaScalingConfig& configuration,
				int branchCount) const;

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::Update" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Update(Population::GaPopulation& population,
				const Population::GaScalingParams& parameters,
				const Population::GaScalingConfig& configuration,
				int branchCount) const;

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL operator ()(Population::GaPopulation& population,
				const Population::GaScalingParams& parameters,
				const Population::GaScalingConfig& configuration,
				Common::Workflows::GaBranch* branch) const { Exec( population, (const GaSPEA2Params&)parameters, configuration, branch ); }

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaSPEA2Params" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaSPEA2Params(); }

			/// <summary>More details are given in specification of <see cref="GaFitnessOperation::CreateFitnessObject" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaSPEAFitness" /> class.</returns>
			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const
				{ return new GaSPEAFitness( params ); }

		protected:

			/// <summary><c>Exec</c> method executes operation. Paramenters has same meaning that is defined by <c>operator ()</c>.</summary>
			GAL_API
			void GACALL Exec(Population::GaPopulation& population,
				const GaSPEA2Params& parameters,
				const Population::GaScalingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

		};

	} // SPEA
} // Multiobjective

#endif // __GA_SPEA_H__

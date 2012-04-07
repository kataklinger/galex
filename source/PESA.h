
/*! \file PESA.h
    \brief This file conatins declaration of classes that implement Pareto Archived Evolution Strategy (PAES).
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#ifndef __GA_PESA_H__
#define __GA_PESA_H__

#include "Population.h"
#include "FitnessValues.h"
#include "Multiobjective.h"

namespace Multiobjective
{

	/// <summary>Contains classes and datatypes that implement Pareto Envelope-based Selection Algorithm and Region-based Selection (PESA-I and PESA-II).</summary>
	namespace PESA
	{

		/// <summary><c>GaPESAParams</c> class represents parameters for implementation of Pareto Envelope-based Selection Algorithm and Region-based Selection.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaPESAParams : public Population::GaScalingParams
		{

		private:

			/// <summary>ID of chromosome tag that indicates whether the chromosome is dominated by any other.</summary>
			int _dominatedTagID;

			/// <summary>ID of chromosome tag that stores coordinates of hyperbox to which the chromosome belong.</summary>
			int _hyperBoxTagID;

			/// <summary>ID of chromosome tag that references info object of hyperbox to which the chromosome belongs.</summary>
			int _hyperBoxInfoTagID;

			/// <summary>ID of population tag that stores hyperbox info objects.</summary>
			int _hyperBoxInfoBufferTagID;

			/// <summary>ID of population tag that stores list of nondominated chromosomes.</summary>
			int _nondominatedTagID;

			/// <summary>ID of population tag that stores currently most crowded hyperboxes.</summary>
			int _crowdingStorageTagID;

			/// <summary>Indicates whether the sharing of fitness for a region (hyperbox) should be engaged.</summary>
			bool _regionSharing;

		public:

			/// <summary>This constructor initializes parameters with used-defined values.</summary>
			/// <param name="dominatedTagID">ID of chromosome tag that indicates whether the chromosome is dominated by any other.</param>
			/// <param name="hyperBoxTagID">ID of chromosome tag that stores coordinates of hyperbox to which the chromosome belong.</param>
			/// <param name="hyperBoxInfoTagID">ID of chromosome that that references info object of hyperbox to which the chromosome belongs.</param>
			/// <param name="hyperBoxInfoBufferTagID">ID of population tag that stores hyperbox info objects.</param>
			/// <param name="nondominatedTagID">ID of population tag that stores list of nondominated chromosomes.</param>
			/// <param name="crowdingStorageTagID">ID of population tag that stores currently most crowded hyperboxes.</param>
			/// <param name="regionSharing">if this parameters is set to <c>true</c> chromosomes in same region (hyperbox) share fitness value.</param>
			GaPESAParams(int dominatedTagID,
				int hyperBoxTagID,
				int hyperBoxInfoTagID,
				int hyperBoxInfoBufferTagID,
				int nondominatedTagID,
				int crowdingStorageTagID,
				bool regionSharing) : _dominatedTagID(dominatedTagID),
				_hyperBoxTagID(hyperBoxTagID),
				_hyperBoxInfoTagID(hyperBoxInfoTagID),
				_hyperBoxInfoBufferTagID(hyperBoxInfoBufferTagID),
				_nondominatedTagID(nondominatedTagID),
				_crowdingStorageTagID(crowdingStorageTagID),
				_regionSharing(regionSharing) { }

			/// <summary>This constructor initializes parameters without specifing any tags and engaged sharing of fitness for a region (hyperbox).</summary>
			GaPESAParams() :  _dominatedTagID(-1),
				_hyperBoxTagID(-1),
				_hyperBoxInfoTagID(-1),
				_hyperBoxInfoBufferTagID(-1),
				_nondominatedTagID(-1),
				_crowdingStorageTagID(-1),
				_regionSharing(true) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaPESAParams( *this ); }

			/// <summary><c>SetDominatedTagID</c> method sets chromosome tag that indicates whether the chromosome is dominated by any other.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void SetDominatedTagID(int tagID) { _dominatedTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of chromosome tag that indicates whether the chromosome is dominated by any other.</returns>
			inline int GACALL GetDominatedTagID() const { return _dominatedTagID; }

			/// <summary><c>SetHyperBoxTagID</c> method sets chromosome tag that stores coordinates of hyperbox to which the chromosome belong.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetHyperBoxTagID(int tagID) { _hyperBoxTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of chromosome tag that stores coordinates of hyperbox to which the chromosome belong.</returns>
			inline int GACALL GetHyperBoxTagID() const { return _hyperBoxTagID; }

			/// <summary><c>SetHyperBoxInfoTagID</c> method sets chromosome that that references info object of hyperbox to which the chromosome belongs.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetHyperBoxInfoTagID(int tagID) { _hyperBoxInfoTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of chromosome that that references info object of hyperbox to which the chromosome belongs.</returns>
			inline int GACALL GetHyperBoxInfoTagID() const { return _hyperBoxInfoTagID; }

			/// <summary><c>SetHyperBoxInfoBufferTagID</c> method sets population tag that stores hyperbox info objects.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetHyperBoxInfoBufferTagID(int tagID) { _hyperBoxInfoBufferTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of population tag that stores hyperbox info objects.</returns>
			/// <returns></returns>
			inline int GACALL GetHyperBoxInfoBufferTagID() const { return _hyperBoxInfoBufferTagID; }

			/// <summary><c>SetNondominatedTagID</c> method sets population tag that stores list of nondominated chromosomes.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetNondominatedTagID(int tagID) { _nondominatedTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of population tag that stores list of nondominated chromosomes.</returns>
			inline int GACALL GetNondominatedTagID() const { return _nondominatedTagID; }

			/// <summary><c>SetCrowdingStorageTagID</c> method sets population tag that stores currently most crowded hyperboxes.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetCrowdingStorageTagID(int tagID) { _crowdingStorageTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of population tag that stores currently most crowded hyperboxes.</returns>
			inline int GACALL GetCrowdingStorageTagID() const { return _crowdingStorageTagID; }

			/// <summary><c>SetRegionSharing</c> method sets sharing of fitness for a region (hyperbox)
			///
			/// This method is not thread-safe.</summary>
			/// <param name="sharing">if this parameters is set to <c>true</c> chromosomes in same region (hyperbox) share fitness value.</param>
			inline void GACALL SetRegionSharing(bool sharing) { _regionSharing = sharing; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if chromosomes in same region (hyperbox) share fitness value.</returns>
			inline bool GACALL GetRegionSharing() const { return _regionSharing; }

		};

		/// <summary><c>GaPESAConfig</c> class represents configuration for implementation of Pareto Envelope-based Selection Algorithm and Region-based Selection.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaPESAConfig : public Population::GaScalingConfig
		{

		protected:

			/// <summary>Hypergrid that PESA will use to split raw fitness hyperspace.</summary>
			Common::Memory::GaAutoPtr<GaFitnessHyperGrid> _grid;

		public:

			/// <summary>This constructor initializes configuration with fitness parameters and hypergird.</summary>
			/// <param name="grid">hypergrid that PESA will use to split raw fitness hyperspace.</param>
			/// <param name="fitnessParams">pointer to fitness parameters of the scaled fitness values.</param>
			GaPESAConfig(const GaFitnessHyperGrid& grid,
				const Fitness::GaFitnessParams* fitnessParams) : GaScalingConfig(fitnessParams) { SetGrid( grid ); }

			/// <summary>This constructor creates new configuration and makes copy fitness parameters and hypergrid.</summary>
			/// <params name="rhs">configuration that should be copied.</params>
			GaPESAConfig(const GaPESAConfig& rhs)
			{
				if( !rhs._grid.IsNull() )
					_grid = rhs._grid->Clone();
			}

			/// <summary>More details are given in specification of <see cref="GaConfiguration::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaConfiguration* GACALL Clone() const { return new GaPESAConfig( *this ); }

			/// <summary><c>SetGrid</c> method sets hypergrid that PESA will use to split raw fitness hyperspace.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="grid">new hypergrid.</param>
			inline void GACALL SetGrid(const GaFitnessHyperGrid& grid) { _grid = grid.Clone(); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to hypergrid that PESA uses to split raw fitness hyperspace.</returns>
			inline GaFitnessHyperGrid& GACALL GetGrid() { return *_grid; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to hypergrid that PESA uses to split raw fitness hyperspace.</returns>
			inline const GaFitnessHyperGrid& GACALL GetGrid() const { return *_grid; }

		};

		/// <summary><c>GaPESAFitness</c> defines fitness type that PESA uses to scaled fitness. Detailed description can be found in specification of
		/// <see cref="GaSVFitness" /> template class.</summary>
		typedef Fitness::Representation::GaSVFitness<float> GaPESAFitness;

		/// <summary><c>GaPESA</c> class repesents scaling operation that implements Pareto Envelope-based Selection Algorithm and Region-based Selection.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaPESA : public Population::GaScalingOperation
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
				Common::Workflows::GaBranch* branch) const { Exec( population, (const GaPESAParams&)parameters, (const GaPESAConfig&)configuration, branch ); }

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaPESAParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaPESAParams(); }

			/// <summary>More details are given in specification of <see cref="GaFitnessOperation::CreateFitnessObject" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaPESAFitness" /> class.</returns>
			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const
				{ return new GaPESAFitness( params ); }

		protected:

			/// <summary><c>Exec</c> method executes operation. Paramenters has same meaning that is defined by <c>operator ()</c>.</summary>
			GAL_API
			void GACALL Exec(Population::GaPopulation& population,
				const GaPESAParams& parameters,
				const GaPESAConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

		};

	} // PESA
} // Multiobjective

#endif // __GA_PESA_H__

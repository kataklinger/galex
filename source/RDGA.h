
/*! \file RDGA.h
    \brief This file conatins declaration of classes that implement Rank-Desity based Genetic Algorithm (RDGA).
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_RDGA_H__
#define __GA_RDGA_H__

#include "Grid.h"
#include "Multiobjective.h"
#include "FitnessValues.h"
#include "Population.h"

namespace Multiobjective
{

	/// <summary>Contains classes and datatypes that implement Rank-Desity based Genetic Algorithm (RDGA).</summary>
	namespace RDGA
	{

		/// <summary><c>GaRDGAParams</c> class represents parameters for implementation of Rank-Density based Genetic Algorithm.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaRDGAParams : public Population::GaReplacementParams
		{

		protected:

			/// <summary>ID of chromosome tag that stores number of chromosomes that dominates the chromosome.</summary>
			int _dominationCountTagID;

			/// <summary>ID of chromosome tag that stores partial list of chromosomes that are dominated by the chromosome.</summary>
			int _dominationListTagID;

			/// <summary>ID of chromosome tag that stores chromosome's rank.</summary>
			int _rankTagID;

			/// <summary>ID of chromosome tag that stores coordinates of hyperbox to which the chromosome belong.</summary>
			int _hyperBoxTagID;

			/// <summary>ID of chromosome tag that references cell of matrix which stores best chromosomes to which this chromosome belongs.</summary>
			int _cellTagID;

			/// <summary>ID of chromosome tag that references next chromosome in current pareto front.</summary>
			int _nextInFrontTagID;

			/// <summary>ID of population tag that stores best ranked chromosome for each hyperbox in matrix format.</summary>
			int _bestChromosomesMatrixTagID;

			/// <summary>ID of population tag that stores chromosomes that have not been ranked yet.</summary>
			int _unrankedTagID;

			/// <summary>ID of population tag that stores hypergrid maintaned by adaptive grid that splits raw fitness hyperspace.</summary>
			int _gridTagID;

			/// <summary>ID of population tag that stores matrix of best chromosomes hyperbox identified bt each branch.</summary>
			int _branchMatricesTagID;

		public:

			/// <summary>This constructor initializes parameters with used-defined values.</summary>
			/// <param name="replacementSize">number of chromosomes which should be replaced.</param>
			/// <param name="dominationCountTagID">ID of chromosome tag that stores number of chromosomes that dominates the chromosome.</param>
			/// <param name="dominationListTagID">ID of chromosome tag that stores partial list of chromosomes that are dominated by the chromosome.</param>
			/// <param name="rankTagID">ID of chromosome tag that stores chromosome's rank.</param>
			/// <param name="hyperBoxTagID">>ID of chromosome tag that stores coordinates of hyperbox to which the chromosome belong.</param>
			/// <param name="cellTagID">ID of chromosome tag that references cell of matrix which stores best chromosomes to which this chromosome belongs.</param>
			/// <param name="nextInFrontTagID">ID of chromosome tag that references next chromosome in current pareto front.</param>
			/// <param name="bestChromosomesMatrixTagID">ID of population tag that stores best ranked chromosome for each hyperbox in matrix format.</param>
			/// <param name="unrankedTagID">ID of population tag that stores chromosomes that have not been ranked yet.</param>
			/// <param name="gridTagID">ID of population tag that stores hypergrid maintaned by adaptive grid that splits raw fitness hypersapce.</param>
			/// <param name="branchMatricesTagID">ID of population tag that stores matrix of best chromosomes hyperbox identified bt each branch.</param>
			GaRDGAParams(int replacementSize,
				int dominationCountTagID,
				int dominationListTagID,
				int rankTagID,
				int hyperBoxTagID,
				int cellTagID,
				int nextInFrontTagID,
				int bestChromosomesMatrixTagID,
				int unrankedTagID,
				int gridTagID,
				int branchMatricesTagID) : GaReplacementParams(replacementSize),
				_dominationCountTagID(dominationCountTagID),
				_dominationListTagID(dominationListTagID),
				_rankTagID(rankTagID),
				_hyperBoxTagID(hyperBoxTagID),
				_cellTagID(cellTagID),
				_nextInFrontTagID(nextInFrontTagID),
				_bestChromosomesMatrixTagID(bestChromosomesMatrixTagID),
				_unrankedTagID(unrankedTagID),
				_gridTagID(gridTagID),
				_branchMatricesTagID(branchMatricesTagID) { }

			/// <summary>This constructor initializes parameters with default values.</summary>
			GaRDGAParams() : _dominationCountTagID(-1),
				_dominationListTagID(-1),
				_rankTagID(-1),
				_hyperBoxTagID(-1),
				_cellTagID(-1),
				_nextInFrontTagID(-1),
				_bestChromosomesMatrixTagID(-1),
				_unrankedTagID(-1),
				_gridTagID(-1),
				_branchMatricesTagID(-1) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaRDGAParams( *this ); }

			/// <summary><c>SetDominationCountTagID</c> method sets chromosome tag that stores number of chromosomes that dominates the chromosome.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetDominationCountTagID(int tagID) { _dominationCountTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of chromosome tag that stores number of chromosomes that dominates the chromosome.</returns>
			inline int GACALL GetDominationCountTagID() const { return _dominationCountTagID; }

			/// <summary><c>SetDominationListTagID</c> method sets chromosome tag that stores partial list of chromosomes that are dominated by the chromosome.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetDominationListTagID(int tagID) { _dominationListTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of chromosome tag that stores partial list of chromosomes that are dominated by the chromosome.</returns>
			inline int GACALL GetDominationListTagID() const { return _dominationListTagID; }

			/// <summary><c>SetRankTagID</c> method sets chromosome tag that stores chromosome's rank.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetRankTagID(int tagID) { _rankTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of chromosome tag that stores chromosome's rank.</returns>
			inline int GACALL GetRankTagID() const { return _rankTagID; }

			/// <summary><c>SetHyperBoxTagID</c> method sets chromosome tag that stores coordinates of hyperbox to which the chromosome belong.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetHyperBoxTagID(int tagID) { _hyperBoxTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of chromosome tag that stores coordinates of hyperbox to which the chromosome belong.</returns>
			inline int GACALL GetHyperBoxTagID() const { return _hyperBoxTagID; }

			/// <summary><c>SetCellTagID</c> method sets chromosome tag that references cell of matrix which stores best chromosomes to which this chromosome belongs..
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetCellTagID(int tagID) { _cellTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of chromosome tag that references cell of matrix which stores best chromosomes to which this chromosome belongs.</returns>
			inline int GACALL GetCellTagID() const { return _cellTagID; }

			/// <summary><c>SetNextInFrontTagID</c> method sets chromosome tag that references next chromosome in current pareto front.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetNextInFrontTagID(int tagID) { _nextInFrontTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of chromosome tag that references next chromosome in current pareto front.</returns>
			inline int GACALL GetNextInFrontTagID() const { return _nextInFrontTagID; }

			/// <summary><c>SetBestChromosomesMatrixTagID</c> method sets population tag that stores best ranked chromosome for each hyperbox in matrix format.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetBestChromosomesMatrixTagID(int tagID) { _bestChromosomesMatrixTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of population tag that stores best ranked chromosome for each hyperbox in matrix format.</returns>
			inline int GACALL GetBestChromosomesMatrixTagID() const { return _bestChromosomesMatrixTagID; }

			/// <summary><c>SetUnrankedTagID</c> method sets population tag that stores chromosomes that have not been ranked yet.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetUnrankedTagID(int tagID) { _unrankedTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of population tag that stores chromosomes that have not been ranked yet.</returns>
			inline int GACALL GetUnrankedTagID() const { return _unrankedTagID; }

			/// <summary><c>SetGridTagID</c> method sets population tag that stores hypergrid maintaned by adaptive grid that splits raw fitness hyperspace.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetGridTagID(int tagID) { _gridTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of population tag that stores hypergrid maintaned by adaptive grid that splits raw fitness hyperspace.</returns>
			inline int GACALL GetGridTagID() const { return _gridTagID; }

			/// <summary><c>SetBranchMatricesTagID</c> method sets population tag that stores matrix of best chromosomes hyperbox identified bt each branch.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetBranchMatricesTagID(int tagID) { _branchMatricesTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of population tag that stores matrix of best chromosomes hyperbox identified bt each branch.</returns>
			inline int GACALL GetBranchMatricesTagID() const { return _branchMatricesTagID; }

		};

		/// <summary><c>GaRDGAConfig</c> class represents configuration for implementation of Rank-Density based Genetic Algorithm.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaRDGAConfig : public Population::GaReplacementConfig
		{

		protected:

			/// <summary>Adaptive hypergrid that RDGA will use to split raw fitness hyperspace.</summary>
			Common::Memory::GaAutoPtr<GaFitnessAdaptiveGrid> _adaptiveGrid;

		public:

			/// <summary>This constructor initializes configuration with fitness parameters and adaptive hypergird.</summary>
			/// <param name="grid">adaptive hypergrid that RDGA will use to split raw fitness hyperspace..</param>
			GaRDGAConfig(const GaFitnessAdaptiveGrid& adaptiveGrid) { SetAdaptiveGrid( adaptiveGrid ); }

			/// <summary>This constructor creates new configuration and makes copy fitness parameters and adaptive hypergrid.</summary>
			/// <params name="rhs">configuration that should be copied.</params>
			GaRDGAConfig(const GaRDGAConfig& rhs)
			{
				if( !rhs._adaptiveGrid.IsNull() )
					_adaptiveGrid = rhs._adaptiveGrid->Clone();
			}

			/// <summary>More details are given in specification of <see cref="GaConfiguration::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaConfiguration* GACALL Clone() const { return new GaRDGAConfig( *this ); }

			/// <summary><c>SetGrid</c> method sets adaptive hypergrid that RDGA will use to split raw fitness hyperspace.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="grid">new adaptive hypergrid.</param>
			inline void GACALL SetAdaptiveGrid(const GaFitnessAdaptiveGrid& grid) { _adaptiveGrid = grid.Clone(); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to adaptive hypergrid that RDGA uses to split raw fitness hyperspace.</returns>
			inline GaFitnessAdaptiveGrid& GACALL GetAdaptiveGrid() { return *_adaptiveGrid; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to adaptive hypergrid that RDGA uses to split raw fitness hyperspace.</returns>
			inline const GaFitnessAdaptiveGrid& GACALL GetAdaptiveGrid() const { return *_adaptiveGrid; }

		};

		/// <summary><c>GaRDGA</c> class repesents replacement operation that implements Rank-Density based Genetic Algorithm.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaRDGA : public Population::GaReplacementOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaReplacementOperation::Prepare" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Prepare(Population::GaChromosomeGroup& input,
				Population::GaPopulation& population,
				const Population::GaReplacementParams& parameters,
				const Population::GaReplacementConfig& configuration,
				int branchCount) const;

			/// <summary>More details are given in specification of <see cref="GaReplacementOperation::Clear" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Clear(Population::GaChromosomeGroup& input,
				Population::GaPopulation& population,
				const Population::GaReplacementParams& parameters,
				const Population::GaReplacementConfig& configuration,
				int branchCount) const;

			/// <summary>More details are given in specification of <see cref="GaReplacementOperation::Update" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Update(Population::GaChromosomeGroup& input,
				Population::GaPopulation& population,
				const Population::GaReplacementParams& parameters,
				const Population::GaReplacementConfig& configuration,
				int branchCount) const;

			/// <summary>More details are given in specification of <see cref="GaReplacementOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL operator ()(Population::GaChromosomeGroup& input,
				Population::GaPopulation& population,
				const Population::GaReplacementParams& parameters,
				const Population::GaReplacementConfig& configuration,
				Common::Workflows::GaBranch* branch) const { Exec( input, population, (const GaRDGAParams&)parameters, (const GaRDGAConfig&)configuration, branch ); }

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaRDGAParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaRDGAParams(); }

		protected:

			/// <summary><c>Exec</c> method executes operation. Paramenters has same meaning that is defined by <c>operator ()</c>.</summary>
			GAL_API
			void GACALL Exec(Population::GaChromosomeGroup& input,
				Population::GaPopulation& population,
				const GaRDGAParams& parameters,
				const GaRDGAConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

		};

	} // RDGA
} // Multiobjective

#endif // __GA_RDGA_H__

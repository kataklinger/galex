
/*! \file CellularCoupling.h
    \brief This file declares classes that implement cellular coupling.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_CELLULAR_COUPLING_H__
#define __GA_CELLULAR_COUPLING_H__

#include "Population.h"

namespace Population
{
	namespace CouplingOperations
	{

		/// <summary><c>GaCellularCouplingParams</c> class represent paramenters for cellulcar coupling operation.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaCellularCouplingParams : public GaCouplingParams
		{

		protected:

			/// <summary>ID of tag that stores rank of chromosome upon which the best chromosome in cell is choosen.</summary>
			int _rankTagID;

			/// <summary>ID of tag that stores hyperbox in which the chromosome is located.</summary>
			int _hyperBoxTagID;

			/// <summary>ID of tag that conatins matrix which stores the best chromosome in each cell.</summary>
			int _bestIndividualsMatrixTagID;

			/// <summary>ID of tag that conatins iterator for walking the neighbour cells.</summary>
			int _neighbourHyperBoxTagID;

		public:

			/// <summary>This constructor initializes parameters with user-defined value.</summary>
			/// <param name="rankTagID">>ID of tag that stores rank of chromosome upon which the best chromosome in cell is choosen.</param>
			/// <param name="hyperBoxTagID">ID of tag that stores hyperbox in which the chromosome is located.</param>
			/// <param name="bestIndividualsMatrixTagID">ID of tag that conatins matrix which stores the best chromosome in each cell.</param>
			/// <param name="neighbourHyperBoxTagID">ID of tag that conatins iterator for walking the neighbour cells.</param>
			/// <param name="numberOfOffsprings">number of chromosomes which should be produced.</param>
			/// <param name="crossoverBuffersTagID">ID of the tag that stores crossover buffers.</param>
			GaCellularCouplingParams(int rankTagID,
				int hyperBoxTagID,
				int bestIndividualsMatrixTagID,
				int neighbourHyperBoxTagID,
				int numberOfOffsprings,
				int crossoverBuffersTagID) : GaCouplingParams(numberOfOffsprings, crossoverBuffersTagID),
				_rankTagID(rankTagID),
				_hyperBoxTagID(hyperBoxTagID),
				_bestIndividualsMatrixTagID(bestIndividualsMatrixTagID),
				_neighbourHyperBoxTagID(neighbourHyperBoxTagID) { }

			/// <summary>This constructor initializes parameters with default. Default number of produced offspring is 2.</summary>
			GaCellularCouplingParams() : _rankTagID(-1),
				_hyperBoxTagID(-1),
				_bestIndividualsMatrixTagID(-1),
				_neighbourHyperBoxTagID(-1) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaCellularCouplingParams( *this ); }

			/// <summary><c>SetRankTagID</c> method sets tag that will store rank of chromosome upon which the best chromosome in cell is choosen.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetRankTagID(int tagID) { _rankTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method return ID of tag that stores rank of chromosome upon which the best chromosome in cell is choosen.</returns>
			inline int GACALL GetRankTagID() const { return _rankTagID; }

			/// <summary><c>SetHyperBoxTagID</c> method sets tag that will store hyperbox in which the chromosome is located.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetHyperBoxTagID(int tagID) { _hyperBoxTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of tag that stores hyperbox in which the chromosome is located..</returns>
			inline int GACALL GetHyperBoxTagID() const { return _hyperBoxTagID; }

			/// <summary><c>SetBestIndividualsMatrixTagID</c> method sets tag that will conatin matrix which will store the best chromosome in each cell
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetBestIndividualsMatrixTagID(int tagID) { _bestIndividualsMatrixTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of tag that conatins matrix which stores the best chromosome in each cell</returns>
			inline int GACALL GetBestIndividualsMatrixTagID() const { return _bestIndividualsMatrixTagID; }

			/// <summary><c>SetNeighbourHyperBoxTagID</c> method sets tag will store iterator for walking the neighbour cells.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetNeighbourHyperBoxTagID(int tagID) { _neighbourHyperBoxTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of tag that conatins iterator for walking the neighbour cells.</returns>
			inline int GACALL GetNeighbourHyperBoxTagID() const { return _neighbourHyperBoxTagID; }

		};

		/// <summary><c>GaCellularCoupling</c> class represent coupling operation that selects the best chromosomes in neighbour cells
		/// of chromosomes in selection result set as mates for producing offspring.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaCellularCoupling : public GaCouplingOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaCouplingOperation::Prepare" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Prepare(GaChromosomeGroup& input,
				GaChromosomeGroup& output,
				const GaCouplingParams& parameters,
				const GaCouplingConfig& configuration,
				int branchCount) const;

			/// <summary>More details are given in specification of <see cref="GaCouplingOperation::Clear" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Clear(GaChromosomeGroup& input,
				GaChromosomeGroup& output,
				const GaCouplingParams& parameters,
				const GaCouplingConfig& configuration,
				int branchCount) const;

			/// <summary>More details are given in specification of <see cref="GaCouplingOperation::Update" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Update(GaChromosomeGroup& input,
				GaChromosomeGroup& output,
				const GaCouplingParams& parameters,
				const GaCouplingConfig& configuration,
				int branchCount) const;

			/// <summary>More details are given in specification of <see cref="GaCouplingOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL operator ()(GaChromosomeGroup& input,
				GaChromosomeGroup& output,
				const GaCouplingParams& parameters,
				const GaCouplingConfig& configuration,
				Common::Workflows::GaBranch* branch) const { Exec( input, output, (const GaCellularCouplingParams&)parameters, configuration, branch ); }

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaCellularCouplingParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaCellularCouplingParams(); }

		protected:

			/// <summary><c>Exec</c> method executes operation. Paramenters has same meaning that is defined by <c>operator ()</c>.</summary>
			GAL_API
			void GACALL Exec(GaChromosomeGroup& input,
				GaChromosomeGroup& output,
				const GaCellularCouplingParams& parameters,
				const GaCouplingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

		};

	} // CouplingOperations
} // Population

#endif // __GA_CELLULAR_COUPLING_H__

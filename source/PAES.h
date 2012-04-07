
/*! \file PAES.h
    \brief This file conatins declaration of classes that implement Pareto Envelope-based Selection Algorithm and Region-based Selection (PESA-I and PESA-II).
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#ifndef __GA_PAES_H__
#define __GA_PAES_H__

#include "Population.h"
#include "FitnessValues.h"
#include "Multiobjective.h"

namespace Multiobjective
{
	namespace PAES
	{

		/// <summary><c>GaPAESCrossover</c> represents dummy crossover operation that performs no actions.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaPAESCrossover : public Chromosome::GaCrossoverOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaCrossoverOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL operator ()(Chromosome::GaCrossoverBuffer& crossoverBuffer,
				const Chromosome::GaCrossoverParams& parameters) const { }

			/// <summary>More details are given in specification of <see cref="GaCrossoverOperation::GetParentCount" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method always returns <c>1</c>.</returns>
			virtual int GACALL GetParentCount(const Chromosome::GaCrossoverParams& parameters) const { return 1; }

			/// <summary>More details are given in specification of <see cref="GaCrossoverOperation::GetOffspringCount" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method always returns <c>1</c>.</returns>
			virtual int GACALL GetOffspringCount(const Chromosome::GaCrossoverParams& parameters) const { return 1; }

			/// <summary>More details are given in specification of <see cref="GaCrossoverOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method always returns <c>NULL</c>.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

		};

		/// <summary><c>GaPAESMating</c> class represents mating operation that produces single offspring chromosome by performing mutation operation on copy of its parent.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaPAESMating : public Chromosome::GaMatingOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaMatingOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(const Chromosome::GaMatingConfig& operations,
				Chromosome::GaCrossoverBuffer& crossoverBuffer,
				const Chromosome::GaMatingParams& parameters) const;

			/// <summary>More details are given in specification of <see cref="GaMatingOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method always returns <c>NULL</c>.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

		};

		/// <summary><c>GaPAESSelectionParams</c> class represents parameters for selection  operation that produces new solutions for Pareto Archived Evolution Strategy.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaPAESSelectionParams : public Population::GaSelectionParams
		{

		private:

			/// <summary>ID of population tag that stores chromosome that represents current solution used to continue the search.</summary>
			int _currentSolutionTagID;

		public:

			/// <summary>This constructor initializes parameters with used-defined values.</summary>
			/// <param name="currentSolutionTagID">ID of population tag that stores chromosome that represents current solution used to continue the search.</param>
			/// <param name="crossoverBuffersTagID">ID of the tag that stores crossover buffers.
			/// This parameter should be set to negative value if selection operation should not produce offspring chromosomes.</param>
			GaPAESSelectionParams(int currentSolutionTagID,
				int crossoverBuffersTagID) : GaSelectionParams(1, crossoverBuffersTagID),
				_currentSolutionTagID(currentSolutionTagID) { }

			/// <summary>This constructor initializes parameters without specifing any tags.</summary>
			GaPAESSelectionParams() : _currentSolutionTagID(-1) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaPAESSelectionParams( *this ); }

			/// <summary><c>SetCurrentSolutionTagID</c> method sets population tag that stores chromosome that represents current solution used to continue the search.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetCurrentSolutionTagID(int tagID) { _currentSolutionTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of population tag that stores chromosome that represents current solution used to continue the search.</returns>
			inline int GACALL GetCurrentSolutionTagID() const { return _currentSolutionTagID; }

		};

		/// <summary><c>GaPAESSelection</c> class represents selection operation that produces new solutions for Pareto Archived Evolution Strategy.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaPAESSelection : public Population::GaSelectionOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaSelectionOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(Population::GaPopulation& population,
				Population::GaChromosomeGroup& output,
				const Population::GaSelectionParams& parameters,
				const Population::GaCouplingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

			/// <summary>More details are given in specification of <see cref="GaSelectionOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaPAESSelectionParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaPAESSelectionParams(); }

		};

		/// <summary><c>GaPAESParams</c> class represents parameters for implementation of Pareto Archived Evolution Strategy.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaPAESParams : public Population::GaReplacementParams
		{

		private:

			/// <summary>ID of chromosome tag that indicates dominance status of chromosome compared to others.</summary>
			int _dominanceTagID;

			/// <summary>ID of chromosome tag that stores coordinates of hyperbox to which the chromosome belong.</summary>
			int _hyperBoxTagID;

			/// <summary>ID of chromosome tag that references info object of hyperbox to which the chromosome belongs.</summary>
			int _hyperBoxInfoTagID;

			/// <summary>ID of population tag that stores hyperbox info objects.</summary>
			int _hyperBoxInfoBufferTagID;

			/// <summary>ID of population tag that stores chromosome that represents current solution used to continue the search.</summary>
			int _currentSolutionTagID;

			/// <summary>ID of population tag that stores currently most crowded hyperboxes.</summary>
			int _crowdingStorageTagID;

		public:

			/// <summary>This constructor initializes parameters with used-defined values.</summary>
			/// <param name="dominanceTagID">ID of chromosome tag that indicates dominance status of chromosome compared to others.</param>
			/// <param name="hyperBoxTagID">ID of chromosome tag that stores coordinates of hyperbox to which the chromosome belong.</param>
			/// <param name="hyperBoxInfoTagID">ID of chromosome that that references info object of hyperbox to which the chromosome belongs.</param>
			/// <param name="hyperBoxInfoBufferTagID">ID of population tag that stores hyperbox info objects.</param>
			/// <param name="currentSolutionTagID">ID of population tag that stores chromosome that represents current solution used to continue the search</param>
			/// <param name="crowdingStorageTagID">ID of population tag that stores currently most crowded hyperboxes.</param>
			GaPAESParams(int dominanceTagID,
				int hyperBoxTagID,
				int hyperBoxInfoTagID,
				int hyperBoxInfoBufferTagID,
				int currentSolutionTagID,
				int crowdingStorageTagID) : _dominanceTagID(dominanceTagID),
				_hyperBoxTagID(hyperBoxTagID),
				_hyperBoxInfoTagID(hyperBoxInfoTagID),
				_hyperBoxInfoBufferTagID(hyperBoxInfoBufferTagID),
				_currentSolutionTagID(currentSolutionTagID),
				_crowdingStorageTagID(crowdingStorageTagID) { }

			/// <summary>This constructor initializes parameters without specifing any tags.</summary>
			GaPAESParams() : _dominanceTagID(-1),
				_hyperBoxTagID(-1),
				_hyperBoxInfoTagID(-1),
				_hyperBoxInfoBufferTagID(-1),
				_currentSolutionTagID(-1),
				_crowdingStorageTagID(-1) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaPAESParams( *this ); }

			/// <summary><c>SetDominanceTagID</c> method sets chromosome tag that indicates dominance status of chromosome compared to others.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void SetDominanceTagID(int tagID) { _dominanceTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of chromosome tag that indicates dominance status of chromosome compared to others.</returns>
			inline int GACALL GetDominanceTagID() const { return _dominanceTagID; }

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

			/// <summary><c>SetCurrentSolutionTagID</c> method sets population tag that stores chromosome that represents current solution used to continue the search.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetCurrentSolutionTagID(int tagID) { _currentSolutionTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of population tag that stores chromosome that represents current solution used to continue the search.</returns>
			inline int GACALL GetCurrentSolutionTagID() const { return _currentSolutionTagID; }

			/// <summary><c>SetCrowdingStorageTagID</c> method sets population tag that stores currently most crowded hyperboxes.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetCrowdingStorageTagID(int tagID) { _crowdingStorageTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of population tag that stores currently most crowded hyperboxes.</returns>
			inline int GACALL GetCrowdingStorageTagID() const { return _crowdingStorageTagID; }

		};

		/// <summary><c>GaPAESConfig</c> class represents configuration for implementation of Pareto Archived Evolution Strategy.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaPAESConfig : public Population::GaReplacementConfig
		{

		protected:

			/// <summary>Hypergrid that PAES will use to split raw fitness hyperspace.</summary>
			Common::Memory::GaAutoPtr<GaFitnessHyperGrid> _grid;

		public:

			/// <summary>This constructor initializes configuration with hypergird.</summary>
			/// <param name="grid">hypergrid that PAES will use to split raw fitness hyperspace.</param>
			GaPAESConfig(const GaFitnessHyperGrid& grid) { SetGrid( grid ); }

			/// <summary>This constructor creates new configuration and makes of hypergrid.</summary>
			/// <params name="rhs">configuration that should be copied.</params>
			GaPAESConfig(const GaPAESConfig& rhs)
			{
				if( !rhs._grid.IsNull() )
					_grid = rhs._grid->Clone();
			}

			/// <summary>More details are given in specification of <see cref="GaConfiguration::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaConfiguration* GACALL Clone() const { return new GaPAESConfig( *this ); }

			/// <summary><c>SetGrid</c> method sets hypergrid that PAES will use to split raw fitness hyperspace.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="grid">new hypergrid.</param>
			inline void GACALL SetGrid(const GaFitnessHyperGrid& grid) { _grid = grid.Clone(); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to hypergrid that PAES uses to split raw fitness hyperspace.</returns>
			inline GaFitnessHyperGrid& GACALL GetGrid() { return *_grid; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to hypergrid that PAES uses to split raw fitness hyperspace.</returns>
			inline const GaFitnessHyperGrid& GACALL GetGrid() const { return *_grid; }

		};

		/// <summary><c>GaPAES</c> class repesents replacement operation that implements Pareto Archived Evolution Strategy.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaPAES : public Population::GaReplacementOperation
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
				Common::Workflows::GaBranch* branch) const { Exec( input, population, (const GaPAESParams&)parameters, (const GaPAESConfig&)configuration, branch ); }

			/// <summary>More details are given in specification of <see cref="GaReplacementOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaPESAParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaPAESParams(); }

		protected:

			/// <summary><c>Exec</c> method executes operation. Paramenters has same meaning that is defined by <c>operator ()</c>.</summary>
			GAL_API
			void GACALL Exec(Population::GaChromosomeGroup& input,
				Population::GaPopulation& population,
				const GaPAESParams& parameters,
				const GaPAESConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

		};

	} // PAES
} // Multiobjective

#endif // __GA_PAES_H__

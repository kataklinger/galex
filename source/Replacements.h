
/*! \file Replacements.h
    \brief This file declares classes that represent replacement operations.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_REPLACEMENT_H__
#define __GA_REPLACEMENT_H__

#include "PopulationOperations.h"

namespace Population
{
	/// <summary>Contains built-in replacement operations.</summary>
	namespace ReplacementOperations
	{

		/// <summary><c>GaWorstReplacement</c> class repesents replacement operation that replaces worst chromosomes in the population with offspring.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaWorstReplacement : public GaReplacementOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaReplacementOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaChromosomeGroup& input,
				GaPopulation& population,
				const GaReplacementParams& parameters,
				const GaReplacementConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaReplacementParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaReplacementParams(); }

		};

		/// <summary><c>GaCrowdingReplacement</c> class repesents replacement operation that inserts offspring chromosomes in crowding area of the population,
		/// but without removing existing chromosomes from population. Population should be trimmed later.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaCrowdingReplacement : public GaReplacementOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaReplacementOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaChromosomeGroup& input,
				GaPopulation& population,
				const GaReplacementParams& parameters,
				const GaReplacementConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaReplacementParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaReplacementParams(); }

		};

		/// <summary><c>GaElitismReplacementParams</c> class represents parametners for replacement operations that can protect the best chromosomes from being removed from population.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaElitismReplacementParams : public GaReplacementParams
		{

		protected:

			/// <summary>Number of the best chromosomes in the population that should be perserved.</summary>
			int _elitism;

		public:

			/// <summary>This constructor initializes replacement paramentes with user-defined values.</summary>
			/// <param name="replacementSize">number of chromosomes which should be replaced.</param>
			/// <param name="elitism">number of the best chromosomes in the population that should be perserved.</param>
			GaElitismReplacementParams(int replacementSize,
				int elitism) : GaReplacementParams(replacementSize),
				_elitism(elitism) { }

			/// <summary>This constructor initializes replacement paramentes with default values.
			/// Default replacement size is <c>2</c> and number of perserved chromosomes is <c>0</c>.</summary>
			GaElitismReplacementParams() : _elitism(0) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaElitismReplacementParams( *this ); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of the best chromosomes in the population that is perserved during replacement.</returns>
			inline int GACALL GetElitism() const { return _elitism; }

			/// <summary><c>SetElitism</c> method sets the number of the best chromosomes in the population that should be perserved.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="elitism">number of perserved chromosomes.</param>
			inline void GACALL SetElitism(int elitism) { _elitism = elitism; }

		};

		/// <summary><c>GaRandomReplacementParams</c> class represents parametners for replacement operations that replaces random chromosomes.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaRandomReplacementParams : public GaElitismReplacementParams
		{

		protected:

			/// <summary>ID of the population tag that will store helper array of IDs of chromosomes that will be replaced.</summary>
			int _indicesBufferTagID;

		public:

			/// <summary>This constructor initializes replacement paramentes with user-defined values.</summary>
			/// <param name="replacementSize">number of chromosomes which should be replaced.</param>
			/// <param name="elitism">number of the best chromosomes in the population that should be perserved.</param>
			/// <param name="indicesBufferTagID">ID of the population tag that will store helper array of IDs of chromosomes that will be replaced.</param>
			GaRandomReplacementParams(int replacementSize,
				int elitism,
				int indicesBufferTagID) : GaElitismReplacementParams(replacementSize, elitism), 
				_indicesBufferTagID(indicesBufferTagID) { }

			/// <summary>This constructor initializes replacement paramentes with default values.
			/// Default replacement size is <c>2</c>, number of perserved chromosomes is <c>0</c> and tag for array of removed chromosomes is not specified.</summary>
			GaRandomReplacementParams() : _indicesBufferTagID(-1) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaRandomReplacementParams( *this ); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of the population tag that will store helper array of IDs of chromosomes that will be replaced.</returns>
			inline int GACALL GetIndicesBufferTagID() const { return _indicesBufferTagID; }

			/// <summary><c>SetIndicesBufferTagID</c> method sets the tag that will store helper array of IDs of chromosomes that will be replaced.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">new tag ID.</param>
			inline void GACALL SetIndicesBufferTagID(int tagID) { _indicesBufferTagID = tagID; }

		};

		/// <summary><c>GaRandomReplacement</c> class repesents replacement operation that removes random chromosomes from the population and inserts offspring chromosomes.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaRandomReplacement : public GaReplacementOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaReplacementOperation::Prepare" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Prepare(GaChromosomeGroup& input,
				GaPopulation& population,
				const GaReplacementParams& parameters,
				const GaReplacementConfig& configuration,
				int branchCount) const;

			/// <summary>More details are given in specification of <see cref="GaReplacementOperation::Clear" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Clear(GaChromosomeGroup& input,
				GaPopulation& population,
				const GaReplacementParams& parameters,
				const GaReplacementConfig& configuration,
				int branchCount) const;

			/// <summary>More details are given in specification of <see cref="GaReplacementOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL operator ()(GaChromosomeGroup& input,
				GaPopulation& population,
				const GaReplacementParams& parameters,
				const GaReplacementConfig& configuration,
				Common::Workflows::GaBranch* branch) const { Exec( input, population, (const GaRandomReplacementParams&)parameters, configuration, branch ); }

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaRandomReplacementParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaRandomReplacementParams(); }

		protected:

			/// <summary><c>Exec</c> method executes operation. Paramenters has same meaning that is defined by <c>operator ()</c>.</summary>
			GAL_API
			void GACALL Exec(GaChromosomeGroup& input,
				GaPopulation& population,
				const GaRandomReplacementParams& parameters,
				const GaReplacementConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

		};

		/// <summary><c>GaParentReplacement</c> class repesents replacement operation that replaces parents with offspring chromosomes.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaParentReplacement : public GaReplacementOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaReplacementOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaChromosomeGroup& input,
				GaPopulation& population,
				const GaReplacementParams& parameters,
				const GaReplacementConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaReplacementParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaReplacementParams(); }

		};

		/// <summary><c>GaPopulationReplacement</c> class repesents replacement operation that replaces whole population of chromosomes with offspring population.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaPopulationReplacement : public GaReplacementOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaReplacementOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL operator ()(GaChromosomeGroup& input,
				GaPopulation& population,
				const GaReplacementParams& parameters,
				const GaReplacementConfig& configuration,
				Common::Workflows::GaBranch* branch) const { Exec( input, population, (const GaElitismReplacementParams&)parameters, configuration, branch ); }

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaElitismReplacementParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaElitismReplacementParams(); }

		protected:

			/// <summary><c>Exec</c> method executes operation. Paramenters has same meaning that is defined by <c>operator ()</c>.</summary>
			GAL_API
			void GACALL Exec(GaChromosomeGroup& input,
				GaPopulation& population,
				const GaElitismReplacementParams& parameters,
				const GaReplacementConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

		};

	} // ReplacementOperations
} // Population

#endif // __GA_SELECTION_H__


/*! \file Mutations.h
    \brief This file declares classes that represent mutation operations.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_MUTATIONS_H__
#define __GA_MUTATIONS_H__

#include "ChromosomeOperations.h"

namespace Chromosome
{

	/// <summary>Contains built-in mutation operations.</summary>
	namespace MutationOperations
	{

		/// <summary><c>GaSwapArrayNodesMutation</c> class represent mutation operation that swaps positions of randomly selected genes of chromosomes that are repesented by arrays.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaSwapArrayNodesMutation : public GaMutationOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaMutationOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaChromosome& chromosome,
				const GaMutationParams& parameters) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaMutationSizeParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaMutationSizeParams(); }

		};

		/// <summary><c>GaSwapListNodesMutation</c> class represent mutation operation that swaps positions of randomly selected genes of chromosomes that are repesented by linked lists.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaSwapListNodesMutation : public GaMutationOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaMutationOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaChromosome& chromosome,
				const GaMutationParams& parameters) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaMutationSizeParams" /> class.</returns>
			Common::GaParameters* GACALL CreateParameters() const { return new GaMutationSizeParams(); }

		};

		/// <summary><c>GaMoveListNodesMutation</c> class represent mutation operation that moves around randomly selected genes of chromosomes that are repesented by linked lists.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaMoveListNodesMutation : public GaMutationOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaMutationOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaChromosome& chromosome,
				const GaMutationParams& parameters) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaMutationSizeParams" /> class.</returns>
			virtual Common::GaParameters* GACALL Clone() const { return new GaMutationSizeParams(); }

		};

		/// <summary><c>GaTreeMutationParams</c> class represents paramenters for mutation operation performend on chromosomes represented by tree.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaTreeMutationParams : public GaMutationSizeParams
		{

		public:

			/// <summary>This enum defines behavior of tree mutation operation.</summary>
			enum GaTreeMutationOptions
			{

				/// <summary>Indicates that mutation operation should only swap values stored in selected tree nodes.</summary>
				GATMO_SWAP_VALUES,

				/// <summary>Indicates that mutation operation should swap whole sub-tree whose roots are selected tree nodes.</summary>
				GATMO_SWAP_TREES,

				/// <summary>Indicates that mutation operation should swap children nodes of selected tree nodes.</summary>
				GATMO_SWAP_CHILDREN,

				/// <summary>Indicates that mutation operation should move the first tree node and it's children to the second tree node.</summary>
				GATMO_MOVE_TREE,

				/// <summary>Indicates that mutation operation should move the first tree node's children to the second tree node.</summary>
				GATMO_MOVE_CHILDREN

			};

		protected:

			/// <summary>Mutation operation behavior.</summary>
			GaTreeMutationOptions _options;

		public:

			/// <summary>Initailizes paraemters with user-defined values.
			///
			/// More details are given in specification of <see cref="GaMutationSizeParams::Clone" /> constructor.</summary>
			/// <param name="options">mutation operation behavior.</param>
			GaTreeMutationParams(float mutationProbability,
				bool improvingOnlyMutations,
				int absoluteSize,
				GaTreeMutationOptions options) : GaMutationSizeParams(mutationProbability, improvingOnlyMutations, absoluteSize),
				_options(options) { }

			/// <summary>Initailizes paraemters with user-defined values.
			///
			/// More details are given in specification of <see cref="GaMutationSizeParams::Clone" /> constructor.</summary>
			/// <param name="options">mutation operation behavior.</param>
			GaTreeMutationParams(float mutationProbability,
				bool improvingOnlyMutations,
				float relativeSize,
				GaTreeMutationOptions options) : GaMutationSizeParams(mutationProbability, improvingOnlyMutations, relativeSize),
				_options(options) { }

			/// <summary>Initializes parameters with default values. Default behavior operation is swapping sub-trees.</summary>
			GaTreeMutationParams() : _options(GATMO_SWAP_TREES) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaTreeMutationParams( *this ); }

			/// <summary><c>SetOptions</c> method sets the behavior of the mutation operation.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="options">mutation operation behavior.</param>
			inline void SetOptions(GaTreeMutationOptions options) { _options = options; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns the way the mutation operation should behave.</returns>
			inline GaTreeMutationOptions GetOptions() const { return _options; }

		};

		/// <summary><c>GaTreeMutation</c> class represent mutation operation that swaps or moves around randomly selected genes of chromosomes that are repesented by trees.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaTreeMutation : public GaMutationOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaMutationOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaChromosome& chromosome,
				const GaMutationParams& parameters) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaTreeMutationParams" /> class.</returns>
			virtual Common::GaParameters* GACALL Clone() const { return new GaTreeMutationParams(); }

		};

	} // MutationOperations
} // Chromosome

#endif // __GA_MUTATIONS_H__

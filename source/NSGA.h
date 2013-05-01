
/*! \file NSGA.h
    \brief This file conatins declaration of classes that implement Nondominated Sorting Genetic Algorithm I and II (NSGA-I and NSGA-II).
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_NSGA_H__
#define __GA_NSGA_H__

#include "FitnessSharing.h"

namespace Multiobjective
{

	/// <summary>Contains classes and datatypes that implement Nondominated Sorting Genetic Algorithm I and II (NSGA-I and NSGA-II).</summary>
	namespace NSGA
	{

		/// <summary><c>GaNSGAParams</c> class represents parameters for implementation of Nondominated Sorting Genetic Algorithm.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaNSGAParams : public Population::ScalingOperations::GaShareFitnessParams
		{

		protected:

			/// <summary>Difference between scaled fitness values of chromosomes in different pareto fronts.</summary>
			float _delta;

			/// <summary>ID of chromosome tag that indicates whether the chromosome is dominated by any other.</summary>
			int _dominatedTagID;

			/// <summary>ID of chromosome tag that indicates whether the pareto front to which chromosome belongs is already determined.</summary>
			int _processedTagID;

			/// <summary>ID of population tag stores dummy value for adjusting scaled fitness value.</summary>
			int _dummyTagID;

			/// <summary>ID of population tag that stores chromosomes after pareto front to which they belong is determined.</summary>
			int _processedBufferTagID;

		public:

			/// <summary>This constructor initializes parameters with used-defined values.</summary>
			/// <param name="delta">difference between scaled fitness values of chromosomes in different pareto fronts.</param>
			/// <param name="dominatedTagID">ID of chromosome tag that indicates whether the chromosome is dominated by any other.</param>
			/// <param name="processedTagID">ID of chromosome tag that indicates whether the pareto front to which chromosome belongs is already determined.</param>
			/// <param name="dummyTagID">ID of population tag stores dummy value for adjusting scaled fitness value.</param>
			/// <param name="processedBufferTagID">ID of population tag that stores chromosomes after pareto front to which they belong is determined.</param>
			/// <param name="cutoff">distance cutoff.</param>
			/// <param name="alpha">curvature of sharing function.</param>
			/// <param name="partialSumTagID">ID of the chromosome that that contains partially calculated shared fitness.</param>
			GaNSGAParams(float delta,
				int dominatedTagID,
				int processedTagID,
				int dummyTagID,
				int processedBufferTagID,
				float cutoff,
				float alpha,
				int partialSumTagID) : GaShareFitnessParams(cutoff, alpha, partialSumTagID),
				_dominatedTagID(dominatedTagID),
				_processedTagID(processedTagID),
				_dummyTagID(dummyTagID),
				_processedBufferTagID(processedBufferTagID) { SetDelta( delta ); }

			/// <summary>This constructor initializes parameters with default values.</summary>
			GaNSGAParams() : _delta(0.1f),
				_dominatedTagID(-1),
				_processedTagID(-1),
				_dummyTagID(-1),
				_processedBufferTagID(-1) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaNSGAParams( *this ); }

			/// <summary><c>SetDelta</c> method sets difference between scaled fitness values of chromosomes in different pareto fronts.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="delta">difference between different pareto fronts.</param>
			inline void GACALL SetDelta(float delta)
			{
				GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, delta >= 0, "delta", "Value cannot be negative.", "Multiobjective" );
				_delta = delta;
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns difference between scaled fitness values of chromosomes in different pareto fronts.</returns>
			inline float GACALL GetDelta() const { return _delta; }

			/// <summary><c>SetDominatedTagID</c> method sets chromosome tag that indicates whether the chromosome is dominated by any other.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetDominatedTagID(int tagID) { _dominatedTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of chromosome tag that indicates whether the chromosome is dominated by any other.</returns>
			inline int GACALL GetDominatedTagID() const { return _dominatedTagID; }

			/// <summary><c>SetProcessedTagID</c> method sets chromosome tag that indicates whether the pareto front to which chromosome belongs is already determined.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetProcessedTagID(int tagID) { _processedTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of chromosome tag that indicates whether the pareto front to which chromosome belongs is already determined.</returns>
			inline int GACALL GetProcessedTagID() const { return _processedTagID; }

			/// <summary><c>SetDummyTagID</c> method sets population tag stores dummy value for adjusting scaled fitness value.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetDummyTagID(int tagID) { _dummyTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of population tag stores dummy value for adjusting scaled fitness value.</returns>
			inline int GACALL GetDummyTagID() const { return _dummyTagID; }

			/// <summary><c>SetProcessedBufferTagID</c> method sets population tag that stores chromosomes after pareto front to which they belong is determined.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetProcessedBufferTagID(int tagID) { _processedBufferTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of population tag that stores chromosomes after pareto front to which they belong is determined.</returns>
			inline int GACALL GetProcessedBufferTagID() const { return _processedBufferTagID; }

		};

		/// <summary><c>GaNSGAFitness</c> defines fitness type that NSGA uses to scaled fitness. Detailed description can be found in specification of
		/// <see cref="GaSVFitness" /> template class.</summary>
		typedef Fitness::Representation::GaSVFitness<float> GaNSGAFitness;

		/// <summary><c>GaNSGA</c> class repesents scaling operation that implements Nondominated Sorting Genetic Algorithm.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaNSGA : public Population::GaScalingOperation
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
				Common::Workflows::GaBranch* branch) const
				{ Exec( population, (const GaNSGAParams&)parameters, (const Population::ScalingOperations::GaShareFitnessScalingConfig&)configuration, branch ); }

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaNSGAParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaNSGAParams(); }

			/// <summary>More details are given in specification of <see cref="GaFitnessOperation::CreateFitnessObject" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaNSGAFitness" /> class.</returns>
			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const
				{ return new GaNSGAFitness( params ); }

		protected:

			/// <summary><c>Exec</c> method executes operation. Paramenters has same meaning that is defined by <c>operator ()</c>.</summary>
			GAL_API
			void GACALL Exec(Population::GaPopulation& population,
				const GaNSGAParams& parameters,
				const Population::ScalingOperations::GaShareFitnessScalingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

		};

		/// <summary><c>GaNSGA2Params</c> class represents parameters for implementation of Fast Elitist Nondominated Sorting Genetic Algorithm.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaNSGA2Params : public Population::GaScalingParams
		{

		protected:

			/// <summary>ID of chromosome tag that stores number of chromosomes that dominates the chromosome.</summary>
			int _dominatedCountTagID;

			/// <summary>ID of chromosome tag that stores partial list of chromosomes that are dominated by the chromosome.</summary>
			int _dominatedListTagID;

			/// <summary>ID of population tag that stores chromosome that belongs to currently processed pareto front.</summary>
			int _frontTagID;

		public:

			/// <summary>This constructor initializes parameters with used-defined values.</summary>
			/// <param name="dominatedCountTagID">ID of chromosome tag that stores number of chromosomes that dominates the chromosome.</param>
			/// <param name="dominatedListTagID">ID of chromosome tag that stores partial list of chromosomes that are dominated by the chromosome.</param>
			/// <param name="frontTagID">ID of population tag that stores chromosome that belongs to currently processed pareto front.</param>
			GaNSGA2Params(int dominatedCountTagID,
				int dominatedListTagID,
				int frontTagID) : _dominatedCountTagID(dominatedCountTagID),
				_dominatedListTagID(dominatedListTagID),
				_frontTagID(frontTagID) { }

			/// <summary>This constructor initializes parameters with default values.</summary>
			GaNSGA2Params() : _dominatedCountTagID(-1),
				_dominatedListTagID(-1),
				_frontTagID(-1) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaNSGA2Params( *this ); }

			/// <summary><c>SetDominatedTagID</c> method sets chromosome tag that stores number of chromosomes that dominates the chromosome.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetDominatedCountTagID(int tagID) { _dominatedCountTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of chromosome tag that stores number of chromosomes that dominates the chromosome.</returns>
			inline int GACALL GetDominatedCountTagID() const { return _dominatedCountTagID; }

			/// <summary><c>SetDominatedTagID</c> method sets chromosome tag that stores partial list of chromosomes that are dominated by the chromosome.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetDominatedListTagID(int tagID) { _dominatedListTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of chromosome tag that stores partial list of chromosomes that are dominated by the chromosome.</returns>
			inline int GACALL GetDominatedListTagID() const { return _dominatedListTagID; }

			/// <summary><c>SetDominatedTagID</c> method sets population tag that stores chromosome that belongs to currently processed pareto front.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetFrontTagID(int tagID) { _frontTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of population tag that stores chromosome that belongs to currently processed pareto front.</returns>
			inline int GACALL GetFrontTagID() const { return _frontTagID; }

		};

		/// <summary><c>GaNSGAFitness</c> defines fitness type that NSGA uses to scaled fitness. Detailed description can be found in specification of
		/// <see cref="GaMVFitness" /> template class.</summary>
		typedef Fitness::Representation::GaMVFitness<float> GaNSGA2Fitness;

		/// <summary><c>GaNSGA2</c> class repesents scaling operation that implements Fast Elitist Nondominated Sorting Genetic Algorithm.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaNSGA2 : public Population::GaScalingOperation
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
				Common::Workflows::GaBranch* branch) const { Exec( population, (const GaNSGA2Params&)parameters, configuration, branch ); }

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaNSGA2Params" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaNSGA2Params(); }

			/// <summary>More details are given in specification of <see cref="GaFitnessOperation::CreateFitnessObject" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaNSGA2Fitness" /> class.</returns>
			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const
				{ return new GaNSGA2Fitness( params ); }

		protected:

			/// <summary><c>Exec</c> method executes operation. Paramenters has same meaning that is defined by <c>operator ()</c>.</summary>
			GAL_API
			void GACALL Exec(Population::GaPopulation& population,
				const GaNSGA2Params& parameters,
				const Population::GaScalingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

		};

	} // NSGA
} // Multiobjective

#endif // __GA_NSGA_H__

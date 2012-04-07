
/*! \file FitnessSharing.h
    \brief This file implements classes that implements fitness sharing scaling.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_FITNESS_SHARING_H__
#define __GA_FITNESS_SHARING_H__

#include "Population.h"
#include "FitnessValues.h"

namespace Population
{
	namespace ScalingOperations
	{

		/// <summary><c>GaShareFitnessParams</c> class represent paramenters for fitness sharing scaling operation.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaShareFitnessParams : public GaScalingParams
		{

		protected:

			/// <summary>Distance cutoff.</summary>
			float _cutoff;

			/// <summary>Curvature of sharing function.</summary>
			float _alpha;

			/// <summary>ID of the chromosome that that contains partially calculated shared fitness.</summary>
			int _partialSumTagID;

		public:

			/// <summary>This constructor initializes scaling paramenters with user-defined value.</summary>
			/// <param name="cutoff">distance cutoff.</param>
			/// <param name="alpha">curvature of sharing function.</param>
			/// <param name="partialSumTagID">ID of the chromosome that that contains partially calculated shared fitness.</param>
			GaShareFitnessParams(float cutoff,
				float alpha,
				int partialSumTagID) : _cutoff(cutoff),
				_alpha(alpha),
				_partialSumTagID(partialSumTagID) { }

			/// <summary>This constructor initializes scaling paramenters with default values. 
			/// Cutoff is 1 as well as curvature of sharing function and tag that should contain partially calculated fitness is not specified.</summary>
			GaShareFitnessParams() : _cutoff(1),
				_alpha(1),
				_partialSumTagID(-1) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaShareFitnessParams( *this ); }

			/// <summary><c>SetCutoff</c> method sets distance cutoff that will be used by scaling operation.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="cutoff">distance cutoff</param>
			inline void GACALL SetCutoff(float cutoff) { _cutoff = cutoff; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns distance cutoff.</returns>
			inline float GACALL GetCutoff() const { return _cutoff; }

			/// <summary><c>SetAlpha</c> method sets curvature of sharing function.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="alpha">curvature factor.</param>
			inline void GACALL SetAlpha(float alpha) { _alpha = alpha; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns curvature of sharing function.</returns>
			inline float GACALL GetAlpha() const { return _alpha; }

			/// <summary><c>SetPartialSumTagID</c> method sets chromosome tag that will store partially calculated shared fitness.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline void GACALL SetPartialSumTagID(int tagID) { _partialSumTagID = tagID; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of the chromosome that that contains partially calculated shared fitness.</returns>
			inline int GACALL GetPartialSumTagID() const { return _partialSumTagID; }

		};

		/// <summary><c>GaShareFitnessScalingConfig</c> class represent configuration for fitness sharing scaling operation.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaShareFitnessScalingConfig : public Population::GaScalingConfig
		{

		protected:

			/// <summary>Chromosome comparator and its parameters.</summary>
			Chromosome::GaChromosomeComparatorSetup _comparator;

		public:

			/// <summary>This constructor initializes scaling configuration with chromosome comparator and its parameters.</summary>
			/// <param name="fitnessParams">pointer to fitness parameters of the scaled fitness values.</param>
			/// <param name="comparator">chromosome comparator setup.</param>
			GaShareFitnessScalingConfig(const Fitness::GaFitnessParams* fitnessParams,
				const Chromosome::GaChromosomeComparatorSetup& comparator) : GaScalingConfig(fitnessParams),
				_comparator(comparator) { }

			/// <summary>This constructor creates new configuration and makes copy fitness parameters and chromosome comparator setup.</summary>
			/// <params name="rhs">configuration that should be copied.</params>
			GaShareFitnessScalingConfig(const GaShareFitnessScalingConfig& rhs) : GaScalingConfig(rhs),
				_comparator(rhs._comparator) { }

			/// <summary>Default constructor initializes matin configuration with no specified chromosome comparator.</summary>
			GaShareFitnessScalingConfig() { }

			/// <summary>More details are given in specification of <see cref="GaConfiguration::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaConfiguration* GACALL Clone() const { return new GaShareFitnessScalingConfig( *this ); }

			/// <summary><c>CompareChromosomes</c> method compares two chromosomes.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="chromosome1">reference to the first chromosomes.</param>
			/// <param name="chromosome2">reference to the second chromosomes.</param>
			/// <returns>Method returns how much the two chromosomes are equal. This number is in interval (0, 1).</returns>
			inline float GACALL CompareChromosomes(const Chromosome::GaChromosome& chromosome1,
				const Chromosome::GaChromosome& chromosome2) const { return _comparator.GetOperation()( chromosome1, chromosome2, _comparator.GetParameters() ); }

			/// <summary><c>CompareChromosomesExact</c> method compares two chromosomes.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="chromosome1">reference to the first chromosomes.</param>
			/// <param name="chromosome2">reference to the second chromosomes.</param>
			/// <returns>Method returns <c>true</c> if the two chromosomes are equal.</returns>
			inline bool GACALL CompareChromosomesExact(const Chromosome::GaChromosome& chromosome1,
				const Chromosome::GaChromosome& chromosome2) const { return _comparator.GetOperation().Equal( chromosome1, chromosome2, _comparator.GetParameters() ); }

			/// <summary><c>SetCompartor</c> method sets setup of chromosome comparator.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="comparator">chromosome comparator setup.</param>
			inline void GACALL SetComparator(const Chromosome::GaChromosomeComparatorSetup& comparator) { _comparator = comparator; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to setup of chromosome comparator and its parameters.</returns>
			inline Chromosome::GaChromosomeComparatorSetup& GACALL GetComparator() { return _comparator; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to setup of chromosome comparator and its parameters.</returns>
			inline const Chromosome::GaChromosomeComparatorSetup& GACALL GetComparator() const { return _comparator; }

		};

		/// <summary><c>GaCalculateSharing</c> function calculates sharing factor of two chromosomes.</summary>
		/// <param name="comparator">comparator used for calculating difference between two chromosomes.</param>
		/// <param name="chromosome1">the first chromosome.</param>
		/// <param name="chromosome2">the second chromosome.</param>
		/// <param name="cutoff">distance cutoff.</param>
		/// <param name="alpha">curvature of sharing function.</param>
		/// <returns>Method returns calculated sharing factor.</returns>
		inline float GACALL GaCalculateSharing(const Chromosome::GaChromosomeComparatorSetup& comparator,
			const Chromosome::GaChromosome& chromosome1,
			const Chromosome::GaChromosome& chromosome2,
			float cutoff,
			float alpha)
		{
			// get difference between the chromosomes
			float diff = comparator.GetOperation()( chromosome1, chromosome2, comparator.GetParameters() );
			return diff < cutoff ? 1 - pow( diff / cutoff, alpha ) : 0;
		}

		/// <summary><c>GaFitnessSharingOp</c> class represents operation that calculates sharing factor of two chromosomes.</summary>
		class GaFitnessSharingOp
		{

		private:

			/// <summary>Chromosome that that contains partially calculated shared fitness.</summary>
			GaChromosomeTagGetter<Common::Data::GaPartialSum> _partialSum;

			/// <summary>Comparator used for calculating difference between two chromosomes.</summary>
			const Chromosome::GaChromosomeComparatorSetup& _comparator;

			/// <summary>Dinstance cutoff.</summary>
			float _cutoff;

			/// <summary>Curvature of sharing function.</summary>
			float _alpha;

			/// <summary>ID of the brach that executes operation.</summary>
			int _branchID;

		public:

			/// <summary>Initalizes operation.</summary>
			/// <param name="partialSum">chromosome that that contains partially calculated shared fitness.</param>
			/// <param name="comparator">comparator used for calculating difference between two chromosomes.</param>
			/// <param name="cutoff">dinstance cutoff.</param>
			/// <param name="alpha">curvature of sharing function.</param>
			/// <param name="branchID">ID of the brach that executes operation.</param>
			GaFitnessSharingOp(const GaChromosomeTagGetter<Common::Data::GaPartialSum>& partialSum,
				const Chromosome::GaChromosomeComparatorSetup& comparator,
				float cutoff,
				float alpha,
				int branchID) : _partialSum(partialSum),
				_comparator(comparator),
				_cutoff(cutoff),
				_alpha(alpha),
				_branchID(branchID) { }

			/// <summary>Dummy operator to satisfy interface.</summary>
			inline void GACALL operator ()(GaChromosomeStorage& chromosome,
				int index) { }

			/// <summary><c>operator ()</c> calculates distance between two chromosomes and saves results in chromosomes' tag for storing partial shared fitness.</summary>
			/// <param name="chromosome1">the first chromosome.</param>
			/// <param name="chromosome2">the second chromosome.</param>
			/// <param name="index1">index of the the first chromosome.</param>
			/// <param name="index2">index of the the second chromosome.</param>
			inline void GACALL operator ()(GaChromosomeStorage& chromosome1,
				GaChromosomeStorage& chromosome2,
				int index1,
				int index2)
			{
				// calculate sharing factor
				float distance = GaCalculateSharing( _comparator, *chromosome1.GetChromosome(), *chromosome2.GetChromosome(), _cutoff, _alpha );

				// update partial sharing fitness
				_partialSum( chromosome1 )[ _branchID ] += distance;
				_partialSum( chromosome2 )[ _branchID ] += distance;
			}

		};

		/// <summary><c>GaSumFitnessSharingOp</c> class represents operation that calculates shared fitness.</summary>
		/// <param name="STOR_OPERATION">operation that defines the way the shared fitness is stored to chromosome storage object.</param>
		template<typename STOR_OPERATION>
		class GaSumFitnessSharingOp
		{

		public:

			/// <summary>Operation that defines the way the shared fitness is stored to chromosome storage object.</summary>
			typedef STOR_OPERATION GaStoreOperation;

		private:

			/// <summary>Chromosome that that contains partially calculated shared fitness.</summary>
			GaChromosomeTagGetter<Common::Data::GaPartialSum> _partialSum;

			/// <summary>Operation that defines the way the shared fitness is stored to chromosome storage object.</summary>
			GaStoreOperation _storeOpertaion;

		public:

			/// <summary></summary>
			/// <param name="partialSum">chromosome that that contains partially calculated shared fitness.</param>
			/// <param name="storeOpertaion">operation that defines the way the shared fitness is stored to chromosome storage object.</param>
			GaSumFitnessSharingOp(const GaChromosomeTagGetter<Common::Data::GaPartialSum>& partialSum,
				const GaStoreOperation& storeOpertaion) : _partialSum(partialSum),
				_storeOpertaion(storeOpertaion) { }

			/// <summary><c>operator ()</c> sums partial shared fitness and stores the results using provided operation.</summary>
			/// <param name="chromosome"></param>
			/// <param name="index"></param>
			inline void GACALL operator ()(GaChromosomeStorage& chromosome,
				int index)
			{
				float sum = 1;

				// sum partial fitness
				Common::Data::GaPartialSum& partial = _partialSum( chromosome );
				for( int i = partial.GetSize() - 1; i >= 0; i-- )
					sum += partial[ i ];

				// store result
				_storeOpertaion( chromosome, sum );
			}

		};

		/// <summary><c>GaShareFitnessScaling</c> class represent 
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaShareFitnessScaling : public GaScalingOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::Prepare" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Prepare(GaPopulation& population,
				const GaScalingParams& parameters,
				const GaScalingConfig& configuration,
				int branchCount) const;

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::Clear" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Clear(GaPopulation& population,
				const GaScalingParams& parameters,
				const GaScalingConfig& configuration,
				int branchCount) const;

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::Update" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Update(GaPopulation& population,
				const GaScalingParams& parameters,
				const GaScalingConfig& configuration,
				int branchCount) const;

			/// <summary>More details are given in specification of <see cref="GaScalingOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL operator ()(GaPopulation& population,
				const GaScalingParams& parameters,
				const GaScalingConfig& configuration,
				Common::Workflows::GaBranch* branch) const { Exec( population, (const GaShareFitnessParams&)parameters, (const GaShareFitnessScalingConfig&)configuration, branch ); }

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaShareFitnessParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaShareFitnessParams(); }

			/// <summary>More details are given in specification of <see cref="GaFitnessOperation::CreateFitnessObject" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaSVFitness&lt;float&gt;" /> class.</returns>
			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const
				{ return new Fitness::Representation::GaSVFitness<float>( params ); }

		protected:

			/// <summary><c>Exec</c> method executes operation. Paramenters has same meaning that is defined by <c>operator ()</c>.</summary>
			GAL_API
			void GACALL Exec(GaPopulation& population,
				const GaShareFitnessParams& parameters,
				const GaShareFitnessScalingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

		};

	} // ScalingOperations
} // Population

#endif // __GA_FITNESS_SHARING_H__

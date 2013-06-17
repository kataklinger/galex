
#ifndef __PR_F1_H__
#define __PR_F1_H__

#include "..\source\ChromosomeOperations.h"
#include "..\source\Representation.h"
#include "..\source\FitnessValues.h"
#include "..\source\FitnessComparators.h"

namespace Problems
{
	namespace F1
	{

		typedef Chromosome::Representation::GaSingleGeneChromosome<float> F1Chromosome;

		class F1Crossover : public Chromosome::GaCrossoverOperation
		{

		public:

			virtual void GACALL operator()(Chromosome::GaCrossoverBuffer& crossoverBuffer,
				const Chromosome::GaCrossoverParams& parameters) const;

			virtual int GACALL GetParentCount(const Chromosome::GaCrossoverParams& parameters) const { return 2; }

			virtual int GACALL GetOffspringCount(const Chromosome::GaCrossoverParams& parameters) const { return 2; }

			virtual Common::GaParameters* GACALL CreateParameters() const { return new Chromosome::GaCrossoverParams(); }

		};

		class F1Mutation : public Chromosome::GaMutationOperation
		{

		public:

			virtual void GACALL operator ()(Chromosome::GaChromosome& chromosome,
				const Chromosome::GaMutationParams& parameters) const;

			virtual Common::GaParameters* GACALL CreateParameters() const { return new Chromosome::GaMutationParams(); }

		};

		typedef Fitness::Representation::GaMVFitness<float> F1Fitness;

		class F1FitnessOperation : public Chromosome::GaChromosomeFitnessOperation
		{

		public:

			virtual void GACALL operator ()(const Chromosome::GaChromosome& object,
				Fitness::GaFitness& fitness,
				const Fitness::GaFitnessOperationParams& operationParams) const;

			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const { return new F1Fitness( params ); }

			virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

		};

		typedef Fitness::Comparators::GaDominanceComparator F1FitnessComparator;

		class F1Initializator : public Chromosome::GaInitializator
		{

		public:

			virtual Chromosome::GaChromosomePtr GACALL operator ()(bool empty,
				const Chromosome::GaInitializatorParams& parameters,
				Common::Memory::GaSmartPtr<Chromosome::GaChromosomeConfigBlock> configBlock) const;

			virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

		};

		class F1ChromosomeComparator : public Chromosome::GaChromosomeComparator
		{

		public:

			virtual float GACALL operator ()(const Chromosome::GaChromosome& chromosome1,
				const Chromosome::GaChromosome& chromosome2,
				const Chromosome::GaChromosomeComparatorParams& parameters) const { return abs( ( (F1Chromosome&)chromosome1 ).GetGene() - ( (F1Chromosome&)chromosome2 ).GetGene() ); }

			virtual bool GACALL Equal(const Chromosome::GaChromosome& chromosome1,
				const Chromosome::GaChromosome& chromosome2,
				const Chromosome::GaChromosomeComparatorParams& parameters) const { return ( (F1Chromosome&)chromosome1 ).GetGene() == ( (F1Chromosome&)chromosome2 ).GetGene(); }

			virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

		};

	} // F1
} // Problems

#endif // __PR_F1_H__

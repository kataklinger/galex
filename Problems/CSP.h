
#ifndef __PR_CSP_H__
#define __PR_CSP_H__

#include "..\source\FitnessValues.h"
#include "..\source\ChromosomeOperations.h"
#include "..\source\Representation.h"

namespace Problems
{
	namespace BPP
	{

		class CspConfigBlock : public Chromosome::GaChromosomeConfigBlock
		{

		public:

			virtual GaChromosomeConfigBlock* GACALL Clone() const { return new CspConfigBlock( *this ); }

		};

		class CspInitializator : public Chromosome::GaInitializator
		{

		public:

			virtual Chromosome::GaChromosomePtr GACALL operator ()(bool empty,
				const Chromosome::GaInitializatorParams& parameters,
				Common::Memory::GaSmartPtr<Chromosome::GaChromosomeConfigBlock> configBlock) const;

			virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

		};

		typedef Fitness::Representation::GaSVFitness<float> CspFitness;

		class CspFitnessOperation : public Chromosome::GaChromosomeFitnessOperation
		{

		public:

			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const { return new CspFitness( params ); }

			virtual void GACALL operator ()(const GaObjectType& object,
				Fitness::GaFitness& fitness,
				const Fitness::GaFitnessOperationParams& operationParams) const;

			virtual Common::GaParameters* GACALL CreateParameters() const  { return NULL; }

		};

		class CspCrossoverOperation : public Chromosome::GaCrossoverOperation
		{

		public:

			virtual void GACALL operator ()(Chromosome::GaCrossoverBuffer& crossoverBuffer,
				const Chromosome::GaCrossoverParams& parameters) const;

			virtual int GACALL GetParentCount(const Chromosome::GaCrossoverParams& parameters) const { return 2; }

			virtual int GACALL GetOffspringCount(const Chromosome::GaCrossoverParams& parameters) const { return 2; }

			virtual Common::GaParameters* GACALL CreateParameters() const { return new Chromosome::GaCrossoverParams(); }

		};

		class CspMutationOperation : public Chromosome::GaMutationOperation
		{

		public:

			virtual void GACALL operator ()(Chromosome::GaChromosome& chromosome,
				const Chromosome::GaMutationParams& parameters) const;

			virtual Common::GaParameters* GACALL CreateParameters() const  { return new Chromosome::GaMutationSizeParams(); }

		};

	}
}

#endif // __PR_CSP_H__

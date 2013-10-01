
#include "CSP.h"

namespace Problems
{
	namespace BPP
	{

		Chromosome::GaChromosomePtr CspInitializator::operator ()(bool empty,
			const Chromosome::GaInitializatorParams& parameters,
			Common::Memory::GaSmartPtr<Chromosome::GaChromosomeConfigBlock> configBlock) const
		{
			return Chromosome::GaChromosomePtr();
		}

		void CspFitnessOperation::operator ()(const CspFitnessOperation::GaObjectType& object,
			Fitness::GaFitness& fitness,
			const Fitness::GaFitnessOperationParams& operationParams) const
		{
		}

		void CspCrossoverOperation::operator ()(Chromosome::GaCrossoverBuffer& crossoverBuffer,
			const Chromosome::GaCrossoverParams& parameters) const
		{
		}

		void CspMutationOperation::operator ()(Chromosome::GaChromosome& chromosome,
			const Chromosome::GaMutationParams& parameters) const
		{
		}

	}
}

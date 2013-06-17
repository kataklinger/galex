
#include "F1.h"

namespace Problems
{
	namespace F1
	{

		void F1Crossover::operator()(Chromosome::GaCrossoverBuffer& crossoverBuffer,
				const Chromosome::GaCrossoverParams& parameters) const
		{
			Chromosome::GaChromosomePtr offspring1 = crossoverBuffer.CreateOffspringFromPrototype();
			Chromosome::GaChromosomePtr offspring2 = crossoverBuffer.CreateOffspringFromPrototype();

			float source1 = ( (F1Chromosome&)*crossoverBuffer.GetParentChromosome( 0 ) ).GetGene();
			float source2 = ( (F1Chromosome&)*crossoverBuffer.GetParentChromosome( 1 ) ).GetGene();

			float destination1, destination2, b1;

			switch( GaGlobalRandomIntegerGenerator->Generate( 0, 1 ) )
			{

			case 0:

				b1 = GaGlobalRandomFloatGenerator->Generate( 0, 1 );

				destination1 = b1 * source1 + ( 1 - b1 ) * source2;
				destination2 = b1 * source2 + ( 1 - b1 ) * source1;

				break;

			case 1:

				destination1 = source1 + source2;
				destination2 = source1 - source2;

				break;

			}

			( (F1Chromosome&)*offspring1 ).SetGene( destination1 );
			( (F1Chromosome&)*offspring2 ).SetGene( destination2 );

			crossoverBuffer.StoreOffspringChromosome( offspring1, 0 );
			crossoverBuffer.StoreOffspringChromosome( offspring2, 1 );
		}

		void GACALL F1Mutation::operator ()(Chromosome::GaChromosome& chromosome,
				const Chromosome::GaMutationParams& parameters) const
		{
			( (F1Chromosome&)chromosome ).SetGene( GaGlobalRandomFloatGenerator->Generate( -10, 10 ) );
		}

		void F1FitnessOperation::operator ()(const Chromosome::GaChromosome& object,
			Fitness::GaFitness& fitness,
			const Fitness::GaFitnessOperationParams& operationParams) const
		{
			float x = ( (F1Chromosome&)object ).GetGene();

			Common::Data::GaSingleDimensionArray<float>& values = ( (F1Fitness&)fitness ).GetTypedValues();

			values[ 0 ] = x * x;

			x -= 2;
			values[ 1 ] = x * x;
		}

		Chromosome::GaChromosomePtr F1Initializator::operator ()(bool empty,
			const Chromosome::GaInitializatorParams& parameters,
			Common::Memory::GaSmartPtr<Chromosome::GaChromosomeConfigBlock> configBlock) const
		{
			F1Chromosome* newChromosome = new F1Chromosome( configBlock );
			newChromosome->SetGene( GaGlobalRandomFloatGenerator->Generate( -10, 10 ) );

			return newChromosome;
		}

	} // F1
} // Problems

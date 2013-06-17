
#include "XKCD.h"
#include "..\source\RandomSequence.h"


namespace Problems
{
	namespace XKCD
	{

		void XkcdConfigBlock::SetAppetizers(const Common::Data::GaSingleDimensionArray<Appetizer>& appetizers)
		{
			_appetizers = appetizers;

			_interval.SetBounds(Chromosome::Representation::GaValueIntervalBounds<int>( 0, appetizers.GetSize() - 1 ),
				Chromosome::Representation::GaValueIntervalBounds<int>( 0, appetizers.GetSize() - 1));
		}

		Chromosome::GaChromosomePtr XkcdInitializator::operator ()(bool empty,
			const Chromosome::GaInitializatorParams& parameters,
			Common::Memory::GaSmartPtr<Chromosome::GaChromosomeConfigBlock> configBlock) const
		{
			XkcdConfigBlock& b = ( (XkcdConfigBlock&)( *configBlock ) );
			XkcdChromosome* chromosome = new XkcdChromosome( configBlock );

			if( !empty )
			{
				int limit = b.GetAppetizers().GetSize() - 1;
				for( int i = GaGlobalRandomIntegerGenerator->Generate( limit ); i >= 0; i-- )
					chromosome->GetGenes().InsertTail( XkcdGene( &b.GetInterval() ) );
			}

			return chromosome;
		}

		void XkcdFitnessOperation::operator ()(const GaObjectType& object,
				Fitness::GaFitness& fitness,
				const Fitness::GaFitnessOperationParams& operationParams) const
		{
			const XkcdChromosome& c = ( (const XkcdChromosome&)object );
			XkcdFitness& f = (XkcdFitness&)fitness;

			float target = ( (const XkcdFitnessOperationParams&) operationParams ).GetTargetPrice();
			float price = 0;
			float time = 0;

			const Common::Data::GaSingleDimensionArray<XkcdConfigBlock::Appetizer>& appetizers = ( (XkcdConfigBlock&)( *c.GetConfigBlock() ) ).GetAppetizers();

			const XkcdGeneList& genes = c.GetGenes();
			for( const XkcdGeneList::GaNodeType* gene = genes.GetHead(); gene != NULL; gene = gene->GetNext() )
			{
				gene->GetValue();

				price += appetizers[ gene->GetValue().GetValue() ]._price;
				time += appetizers[ gene->GetValue().GetValue() ]._time;
			}

			f.SetValue( 1.0f / ( 1 + abs( price - target ) ), 0 );
			f.SetValue( 1.0f / time, 1 );
		}

		void XkcdMutationOperation::operator ()(Chromosome::GaChromosome& chromosome,
			const Chromosome::GaMutationParams& parameters) const
		{
			const Chromosome::GaMutationSizeParams& p = (const Chromosome::GaMutationSizeParams&)parameters;

			XkcdChromosome& c = ( (XkcdChromosome&)chromosome );

			int size = p.CalculateMutationSize(c.GetGenes().GetCount());

			Common::Memory::GaAutoPtr<int> points( new int[ size ], Common::Memory::GaArrayDeletionPolicy<int>::GetInstance() );
			Common::Random::GaGenerateRandomSequenceAsc(0, c.GetGenes().GetCount() - 1, size, true, points.GetRawPtr() );

			int i = 0, j = 0;
			for( XkcdGeneList::GaNodeType* node = c.GetGenes().GetHead(); node != NULL && j < size; node = node->GetNext() )
			{
				if( i == points[ j ] )
				{
					node->GetValue().Flip();
					j++;
				}
			}
		}

	}
}

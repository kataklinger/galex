
/*! \file Crossovers.cpp
    \brief This file implements classes that represent crossover operations.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#include "RandomSequence.h"
#include "Crossovers.h"
#include "Representation.h"

namespace Chromosome
{
	namespace CrossoverOperations
	{

		// Array chromosomes crossover
		void GaArrayMultipointCrossover::operator ()(GaCrossoverBuffer& crossoverBuffer,
			const GaCrossoverParams& parameters) const
		{
			// get chromosomes' representations
			Common::Data::GaArrayBase* source1 = &( (Representation::GaArrayStructureChromosome&)*crossoverBuffer.GetParentChromosome( 0 ) ).GetStructure();
			Common::Data::GaArrayBase* source2 = &( (Representation::GaArrayStructureChromosome&)*crossoverBuffer.GetParentChromosome( 1 ) ).GetStructure();

			// reserve memory for storing crossover points
			int count = ( (const GaCrossoverPointParams&)parameters ).GetNumberOfCrossoverPoints() + 1;
			Common::Memory::GaAutoPtr<int> points( new int[ count ], Common::Memory::GaArrayDeletionPolicy<int>::GetInstance() );

			// create required number of offspring chromosomes
			for( int i = ( (const GaCrossoverPointParams&)parameters ).GetNumberOfOffspring() - 1; i >= 0; i -= 2 )
			{
				// create required number of offspring chromosomes
				GaChromosomePtr offspring2, offspring1 = crossoverBuffer.CreateOffspringFromPrototype();
				Common::Data::GaArrayBase* destination1 = &( (Representation::GaArrayStructureChromosome&)*offspring1 ).GetStructure();

				// create the second offspring chromosome if required
				Common::Data::GaArrayBase* destination2 = NULL;
				if( i > 0 )
				{
					offspring2 = crossoverBuffer.CreateOffspringFromPrototype();
					destination2 = &( (Representation::GaArrayStructureChromosome&)*offspring2 ).GetStructure();
				}

				// number of crossover points cannot be larger then smallest chromosome
				if( count > destination1->GetSize() )
					count  = destination1->GetSize();

				// generate crossover points
				if( count > 1 )
					Common::Random::GaGenerateRandomSequenceAsc( 1, destination1->GetSize() - 1, count - 1, true, points.GetRawPtr() );

				// alternately copy genes from parents to offspring chromosomes
				points[ count - 1 ] = source1->GetSize();
				for( int j = 0, s = 0; j < count ; s = points[ j++ ] )
				{
					// copy genes to the first offspring
					destination1->Copy( source1, s, s, points[ j ] - s );

					// copy genes to the second offspring if it was created
					if( destination2 )
						destination2->Copy( source2, s, s, points[ j ] - s );

					// swap sources
					Common::Data::GaArrayBase* t = source1;
					source1 = source2;
					source2 = t;
				}

				// store the first offspring
				crossoverBuffer.StoreOffspringChromosome( offspring1, 0 );

				// store the second offspring if it was created
				if( !offspring2.IsNull() )
					crossoverBuffer.StoreOffspringChromosome( offspring2, 1 );
			}
		}

		// Linked list chromosomes crossover
		void GaListMultipointCrossover::operator ()(GaCrossoverBuffer& crossoverBuffer,
			const GaCrossoverParams& parameters) const
		{
			// get chromosomes' representations
			Common::Data::GaListBase* source1 = &( (Representation::GaListStructureChromosome&)*crossoverBuffer.GetParentChromosome( 0 ) ).GetStructure();
			Common::Data::GaListBase* source2 = &( (Representation::GaListStructureChromosome&)*crossoverBuffer.GetParentChromosome( 1 ) ).GetStructure();

			// reserve memory for storing crossover points
			int maxCount = ( (const GaCrossoverPointParams&)parameters ).GetNumberOfCrossoverPoints() + 1;
			Common::Memory::GaAutoPtr<int> points1( new int[ maxCount ], Common::Memory::GaArrayDeletionPolicy<int>::GetInstance() );
			Common::Memory::GaAutoPtr<int> points2( new int[ maxCount ], Common::Memory::GaArrayDeletionPolicy<int>::GetInstance() );

			// create required number of offspring chromosomes
			for( int i = ( (const GaCrossoverPointParams&)parameters ).GetNumberOfOffspring() - 1; i >= 0; i -= 2 )
			{
				// create the first offspring chromosome
				GaChromosomePtr offspring2, offspring1 = crossoverBuffer.CreateOffspringFromPrototype();
				Common::Data::GaListBase* destination1 = &( (Representation::GaListStructureChromosome&)*offspring1 ).GetStructure();

				// create the second offspring chromosome if required
				Common::Data::GaListBase* destination2 = NULL;
				if( i > 0 )
				{
					offspring2 = crossoverBuffer.CreateOffspringFromPrototype();
					destination2 = &( (Representation::GaListStructureChromosome&)*offspring2 ).GetStructure();
				}

				// number of crossover points cannot be larger then smallest chromosome
				int count = maxCount;
				if( count > source1->GetCount() )
					count = source1->GetCount();
				if( count > source2->GetCount() )
					count = source2->GetCount();

				// generate crossover points
				if( count > 1 )
				{
					Common::Random::GaGenerateRandomSequenceAsc( 1, source1->GetCount() - 1, count - 1, true, points1.GetRawPtr() );
					Common::Random::GaGenerateRandomSequenceAsc( 1, source2->GetCount() - 1, count - 1, true, points2.GetRawPtr() );
				}

				Common::Data::GaListNodeBase* sourceNode1 = source1->GetHead();
				Common::Data::GaListNodeBase* sourceNode2 = source2->GetHead();

				points1[ count - 1 ] = source1->GetCount();
				points2[ count - 1 ] = source2->GetCount();

				// alternately copy genes from parents to offspring chromosomes
				for( int j = 0, s1 = 0, s2 = 0; j < count ; j++ )
				{
					int e1 = points1[ j ];
					int e2 = points2[ j ];

					// copy portion of the first parent to destination offspring if it exists
					if( destination1 )
					{
						for( int k = s1; k < e1; k++, sourceNode1 = sourceNode1->GetNext() )
							destination1->InsertTail( (Common::Data::GaListNodeBase*)sourceNode1->Clone() );
					}

					// copy portion of the second parent to destination offspring if it exists
					if( destination2 )
					{
						for( int k = s2; k < e2; k++, sourceNode2 = sourceNode2->GetNext() )
							destination2->InsertTail( (Common::Data::GaListNodeBase*)sourceNode2->Clone() );
					}

					// swap destination chromosomes
					Common::Data::GaListBase* t = destination1;
					destination1 = destination2;
					destination2 = t;

					s1 = e1;
					s2 = e2;
				}

				if( destination1->GetCount() == 0 || destination2->GetCount() == 0 )
				{
					destination1 = destination1;
				}

				// store the first offspring
				crossoverBuffer.StoreOffspringChromosome( offspring1, 0 );

				// store the second offspring if it was created
				if( !offspring2.IsNull() )
					crossoverBuffer.StoreOffspringChromosome( offspring2, 1 );
			}
		}

		// Tree chromosomes crossover
		void GaTreeSinglepointCrossover::operator ()(GaCrossoverBuffer& crossoverBuffer,
			const GaCrossoverParams& parameters) const
		{
			// get chromosomes' representations
			Representation::GaTreeStructureChromosome& source1 = (Representation::GaTreeStructureChromosome&)*crossoverBuffer.GetParentChromosome( 0 );
			Representation::GaTreeStructureChromosome& source2 = (Representation::GaTreeStructureChromosome&)*crossoverBuffer.GetParentChromosome( 1 );

			// create required number of offspring chromosomes
			for( int i = ( (GaCrossoverPointParams&)parameters ).GetNumberOfOffspring() - 1; i >= 0; i -= 2 )
			{
				// create the first offspring chromosome
				GaChromosomePtr offspring1 = source1.Clone();
				Common::Data::GaTreeBase* destination1 = &( (Representation::GaTreeStructureChromosome&)*offspring1 ).GetStructure();

				// create the second offspring chromosome
				GaChromosomePtr offspring2 = source2.Clone();
				Common::Data::GaTreeBase* destination2 = &( (Representation::GaTreeStructureChromosome&)*offspring2 ).GetStructure();

				// generate crossover point for the first offspring chromosome
				Common::Data::GaTreeNodeBase* node1 = destination1->GetRandomNode( false );
				Common::Data::GaTreeNodeBase* parent1 = node1 ? node1->GetParent() : NULL;
				int position1 = parent1 ? parent1->GetChildren()->GetPosition( node1 ) : 0;

				// generate crossover point for the second offspring chromosome
				Common::Data::GaTreeNodeBase* node2 = destination2->GetRandomNode( false );
				Common::Data::GaTreeNodeBase* parent2 = node2 ? node2->GetParent() : NULL;
				int position2 = parent2 ? parent2->GetChildren()->GetPosition( node2 ) : 0;

				// crossover the first offspring chromosome
				if( node1 )
				{
					destination1->DetachNode( node1 );
					destination2->Insert( node1, parent1, position1 );
				}

				// crossover the second offspring chromosome
				if( node2 )
				{
					destination2->DetachNode( node2 );
					destination1->Insert( node2, parent2, position2 );
				}

				// store the first offspring
				crossoverBuffer.StoreOffspringChromosome( offspring1, 0 );

				// store the second offspring if it is required
				if( i > 0 )
					crossoverBuffer.StoreOffspringChromosome( offspring2, 1 );
			}
		}

	} // CrossoverOperations
} // Chromosome

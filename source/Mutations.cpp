
/*! \file Mutations.cpp
    \brief This file implements classes that represent mutation operations.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#include "Mutations.h"
#include "Representation.h"

namespace Chromosome
{
	namespace MutationOperations
	{

		// Swap array values mutation
		void GaSwapArrayNodesMutation::operator ()(GaChromosome& chromosome,
			const GaMutationParams& parameters) const
		{
			// get representation
			Common::Data::GaArrayBase& values = ( (Representation::GaArrayStructureChromosome&)chromosome ).GetStructure();
			int size = values.GetSize();

			if( size < 2 )
				return;

			// swap values
			int mutationSize = GaGlobalRandomIntegerGenerator->Generate( 1, ( (const GaMutationSizeParams&)parameters ).CalculateMutationSize( size ) );
			for( int i = mutationSize; i > 0; i-- )
				values.Swap( GaGlobalRandomIntegerGenerator->Generate( 0, size - 1 ), GaGlobalRandomIntegerGenerator->Generate( 0, size - 1 ) );
		}

		// Swap list nodes mutation
		void GaSwapListNodesMutation::operator ()(GaChromosome& chromosome,
			const GaMutationParams& parameters) const
		{
			// get representation
			Common::Data::GaListBase& list = ( (Representation::GaListStructureChromosome&)chromosome ).GetStructure();
			int size = list.GetCount();

			if( size < 2 )
				return;

			int mutationSize = GaGlobalRandomIntegerGenerator->Generate( 2, ( (const GaMutationSizeParams&)parameters ).CalculateMutationSize( size ) );

			// swap nodes
			for( int i = mutationSize; i > 0; i -= 2 )
				list.GetAt( GaGlobalRandomIntegerGenerator->Generate( 0, size - 1 ) )->SwapData( list.GetAt( GaGlobalRandomIntegerGenerator->Generate( 0, size - 1 ) ) );
		}

		// Move list nodes mutation
		void GaMoveListNodesMutation::operator ()(GaChromosome& chromosome,
			const GaMutationParams& parameters) const
		{
			// get representation
			Common::Data::GaListBase& list = ( (Representation::GaListStructureChromosome&)chromosome ).GetStructure();
			int size = list.GetCount();

			if( size < 2 )
				return;

			// move nodes
			int mutationSize = GaGlobalRandomIntegerGenerator->Generate( 1, ( (const GaMutationSizeParams&)parameters ).CalculateMutationSize( size ) );
			for( int i = mutationSize; i > 0; i-- )
				list.Move( list.GetAt( GaGlobalRandomIntegerGenerator->Generate( 0, size - 1 ) ), GaGlobalRandomIntegerGenerator->Generate( 0, size - 1 ) );
		}

		// Tree mutation
		void GaTreeMutation::operator ()(GaChromosome& chromosome,
			const GaMutationParams& parameters) const
		{
			// get representation
			Common::Data::GaTreeBase& tree = ( (Representation::GaTreeStructureChromosome&)chromosome ).GetStructure();
			int size = tree.GetCount();

			if( size < 2 )
				return;

			// number of genes that should be mutated
			int mutationSize = GaGlobalRandomIntegerGenerator->Generate( 1, ( (const GaMutationSizeParams&)parameters ).CalculateMutationSize( size ) );
			for( int i = mutationSize; i > 0; )
			{
				Common::Data::GaTreeNodeBase* node1 = tree.GetRandomNode( true );

				switch( ( (const GaTreeMutationParams&)parameters ).GetOptions() )
				{

				case GaTreeMutationParams::GATMO_SWAP_VALUES:
					{
						node1->SwapData( tree.GetRandomNode( true ) );

						i -= 2;
						break;
					}

				case GaTreeMutationParams::GATMO_SWAP_TREES:
					{
						Common::Data::GaTreeNodeBase* node2 = tree.GetRandomNode( true );
						if( node1->IsConnected( node2 ) )
							break;

						tree.SwapNodes( node1, node2 );

						i -= 2;
						break;
					}

				case GaTreeMutationParams::GATMO_SWAP_CHILDREN:
					{
						Common::Data::GaTreeNodeBase* node2 = tree.GetRandomNode( true );
						if( node1->IsConnected( node2 ) )
							break;

						tree.SwapChildren( node1, node2 );

						i -= 2;
						break;
					}

				case GaTreeMutationParams::GATMO_MOVE_TREE:
					{
						Common::Data::GaTreeNodeBase* node2 = tree.GetRandomNode( true );
						if( node1->IsConnected( node2 ) )
							break;

						tree.MoveNode( node1, node2, -1 );

						i--;
						break;
					}

				case GaTreeMutationParams::GATMO_MOVE_CHILDREN:
					{
						Common::Data::GaTreeNodeBase* node2 = tree.GetRandomNode( true );
						if( node1->IsConnected( node2 ) )
							break;

						tree.MoveChildren( node1, node2 );

						i--;
						break;
					}
				}
			}
		}

	} // MutationOperations
} // Chromosome


#include "BPP.h"
#include "..\source\RandomSequence.h"

#include <iostream>

namespace Problems
{
	namespace BPP
	{

		void BinConfigBlock::SetItems(const Common::Data::GaSingleDimensionArray<Item>& items)
		{
			_items = items;
			_indices.SetSize( items.GetSize() );
			for( int i = _indices.GetSize() - 1; i >= 0; i-- )
				_indices[ i ] = i;
		}

		void Bin::MoveItems(ItemList& dest)
		{
			dest.MergeLists( &_items );
			_fill = 0;
		}

		void Bin::MoveItems(ItemList& dest,
			const Common::Data::GaSingleDimensionArray<bool>& skip)
		{
			for( ItemList::GaNodeType* c = _items.GetHead(); c != NULL;  )
			{
				ItemList::GaNodeType* n = c->GetNext();

				if( !skip[ c->GetValue() ] )
					dest.InsertTail( _items.DetachNode( c ) );

				c = n;
			}

			_items.Clear();
			_fill = 0;
		}

		bool Bin::TryAdd(int item,
			float size)
		{
			if( _fill + size > _capacity )
				return false;

			_items.InsertTail( item );
			_fill += size;

			return true;
		}

		bool Bin::TryReplace(ItemList& unassigned,
			ItemList::GaNodeType* current,
			const Common::Data::GaSingleDimensionArray<BinConfigBlock::Item>& items)
		{
			bool replaced = false;

			float s0 = items[ current->GetValue() ]._size;
			for( ItemList::GaNodeType* n1 = _items.GetHead(); n1 != NULL && !replaced; n1 = n1->GetNext() )
			{
				float s1 = items[ n1->GetValue() ]._size;
				for( ItemList::GaNodeType* n2 = n1->GetNext(); n2 != NULL && !replaced; n2 = n2->GetNext() )
				{
					float s2 = s1 + items[ n2->GetValue() ]._size;
					for( ItemList::GaNodeType* n3 = n2->GetNext(); n3 != NULL && !replaced; n3 = n3->GetNext() )
					{
						float s3 = s2 + items[ n3->GetValue() ]._size;

						if( s0 > s3 && ( _fill - s3 + s0 ) < _capacity )
						{
							_fill += s0 - s3;

							unassigned.InsertTail( _items.DetachNode( n1 ) );
							unassigned.InsertTail( _items.DetachNode( n2 ) );
							unassigned.InsertTail( _items.DetachNode( n3 ) );
							replaced = true;
						}
					}

					if( !replaced && s0 > s2 && ( _fill - s2 + s0 ) < _capacity )
					{
						_fill += s0 - s2;

						unassigned.InsertTail( _items.DetachNode( n1 ) );
						unassigned.InsertTail( _items.DetachNode( n2 ) );
						replaced = true;
					}
				}

				if( !replaced && s0 > s1 && ( _fill - s1 + s0 ) < _capacity )
				{
					_fill += s0 - s1;

					unassigned.InsertTail( _items.DetachNode( n1 ) );
					replaced = true;
				}
			}

			if( replaced )
				_items.InsertTail( unassigned.DetachNode( current ) );

			return replaced;
		}

		Chromosome::GaChromosomePtr BinInitializator::operator ()(bool empty,
			const Chromosome::GaInitializatorParams& parameters,
			Common::Memory::GaSmartPtr<Chromosome::GaChromosomeConfigBlock> configBlock) const
		{
			BinChromosome* chromosome = new BinChromosome( configBlock );

			if( !empty )
			{
				BinConfigBlock& b = ( (BinConfigBlock&)( *configBlock ) );

				const Common::Data::GaSingleDimensionArray<BinConfigBlock::Item>& items = b.GetItems();
				Common::Data::GaSingleDimensionArray<int> shuffled( b.GetIndices().GetArray(), items.GetSize() );
				Common::Random::GaShuffle( shuffled.GetArray(), items.GetSize() );

				Bin* bin = NULL;
				BinList& bins = chromosome->GetGenes();

				for( int i = items.GetSize() - 1; i >= 0;  )
				{
					if( bin != NULL && bin->TryAdd( shuffled[ i ], items[ shuffled[ i ] ]._size ) )
						i--;
					else
					{
						bins.InsertTail( Bin( b.GetBinCapacity() ) );
						bin = &bins.GetTail()->GetValue();
					}
				}
			}

			return chromosome;
		}

		void BinFitnessOperation::operator ()(const BinFitnessOperation::GaObjectType& object,
			Fitness::GaFitness& fitness,
			const Fitness::GaFitnessOperationParams& operationParams) const
		{
			const BinChromosome& c = ( (const BinChromosome&)object );
			BinFitness& f = (BinFitness&)fitness;

			float kParam = ( (const BinFitnessOperationParams&) operationParams ).GetKParam();

			float binCapacity = ( (BinConfigBlock&)( *c.GetConfigBlock() ) ).GetBinCapacity();
			const Common::Data::GaSingleDimensionArray<BinConfigBlock::Item>& items = ( (BinConfigBlock&)( *c.GetConfigBlock() ) ).GetItems();

			float val = 0;
			for( const BinList::GaNodeType* bNode = c.GetGenes().GetHead(); bNode != NULL; bNode = bNode->GetNext() )
				val += pow( (float)bNode->GetValue().GetFill() / binCapacity, kParam );

			f.SetValue( val / c.GetGenes().GetCount() );
		}

		void AdoptBins(BinList& bins, Bin::ItemList& unassigned, const Common::Data::GaSingleDimensionArray<BinConfigBlock::Item>& items)
		{
			for( Bin::ItemList::GaNodeType* u = unassigned.GetHead(); u != NULL;  )
			{
				bool replaced = false;
				for( BinList::GaNodeType* b = bins.GetHead(); b != NULL && !replaced; b = b->GetNext() )
				{
					Bin::ItemList::GaNodeType* t = u->GetNext();
					if( b->GetValue().TryReplace( unassigned, u, items ) )
					{
						replaced = true;
						u = t;
					}
				}

				if( !replaced )
					u = u->GetNext();
			}

			if( unassigned.GetCount() > 1 )
			{
				for( Bin::ItemList::GaNodeType* i = unassigned.GetHead(); i->GetNext() != NULL; )
				{
					Bin::ItemList::GaNodeType* m = i->GetNext();
					float mSize = items[ m->GetValue() ]._size;

					for( Bin::ItemList::GaNodeType* j = i; ; j = j->GetPrevious() )
					{
						if( j == NULL )
						{
							unassigned.InsertHead( unassigned.DetachNode( m ) );

							break;
						}
						else if( mSize <= items[ j->GetValue() ]._size )
						{
							if( i != j )
								unassigned.InsertAfter( j, unassigned.DetachNode( m ) );
							else
								i = m;

							break;
						}
					}
				}
			}

			for( Bin::ItemList::GaNodeType* c = unassigned.GetHead(); c != NULL;  c = c->GetNext() )
			{
				int item = c->GetValue();
				float size = items[ item ]._size;

				bool inserted = false;
				for( BinList::GaNodeType* b = bins.GetHead(); b != NULL && !inserted; b = b->GetNext() )
				{
					if( b->GetValue().TryAdd( item, size ) )
						inserted = true;
				}

				if( !inserted )
				{
					bins.InsertTail( Bin( bins.GetHead()->GetValue().GetCapacity() ) );
					bins.GetTail()->GetValue().TryAdd( item, size );
				}
			}
		}

		void BinCrossoverOperation::operator ()(Chromosome::GaCrossoverBuffer& crossoverBuffer,
			const Chromosome::GaCrossoverParams& parameters) const
		{
			const BinConfigBlock& ccb = ( (const BinConfigBlock&)*crossoverBuffer.GetParentChromosome( 0 )->GetConfigBlock() );

			int points[2][2];
			for( int i = 0; i < 2; i++ )
			{
				int cSize = ( (const BinChromosome&)*crossoverBuffer.GetParentChromosome( i ) ).GetStructure().GetCount();

				int sSize = GaGlobalRandomIntegerGenerator->Generate( 1, cSize - 1 );
				points[ i ][ 0 ] = GaGlobalRandomIntegerGenerator->Generate( 0, cSize - sSize - 1 );
				points[ i ][ 1 ] = points[ i ][ 0 ] + sSize;
			}

			int count = ccb.GetItems().GetSize();
			Common::Data::GaSingleDimensionArray<Bin*> member( count );
			Common::Data::GaSingleDimensionArray<bool> duplicate( count );

			for( int i = 0; i < 2; i++ )
			{
				const BinChromosome& src = ( (const BinChromosome&)*crossoverBuffer.GetParentChromosome( i ) );

				crossoverBuffer.StoreOffspringChromosome( crossoverBuffer.GetParentChromosome( 1 - i )->Clone(), i );
				BinChromosome& dst = ( (BinChromosome&)*crossoverBuffer.GetOffspringChromosome( i ) );

				for( BinList::GaNodeType* bNode = (BinList::GaNodeType*)dst.GetGenes().GetHead(); bNode != NULL; bNode = bNode->GetNext() )
				{
					for( const Bin::ItemList::GaNodeType* iNode = bNode->GetValue().GetItems().GetHead(); iNode != NULL; iNode = iNode->GetNext() )
					{
						int item = iNode->GetValue();

						duplicate[ item ] = false;
						member[ item ] = &bNode->GetValue();
					}
				}

				int j = points[ i ][ 0 ];
				for( BinList::GaNodeType *dNode = (BinList::GaNodeType*)dst.GetGenes().GetAt( points[ 1 - i ][ i ] ), *sNode = (BinList::GaNodeType*)src.GetGenes().GetAt( j );
					j < points[ i ][ 1 ]; j++, dNode = dNode->GetNext(), sNode = sNode->GetNext() )
				{
					dst.GetGenes().InsertAfter( dNode, (BinList::GaNodeType*)sNode->Clone() );

					for( const Bin::ItemList::GaNodeType* iNode = sNode->GetValue().GetItems().GetHead(); iNode != NULL; iNode = iNode->GetNext() )
						duplicate[ iNode->GetValue() ] = true;
				}


				Bin::ItemList unassigned;

				for( int k = count - 1; k >= 0; k-- )
				{
					if( duplicate[ k ] )
						member[ k ]->MoveItems( unassigned, duplicate );
				}
				
				for( BinList::GaNodeType* b = (BinList::GaNodeType*)dst.GetGenes().GetHead(); b != NULL;  )
				{
					BinList::GaNodeType* n = b->GetNext();
					if( b->GetValue().IsEmpty() )
						dst.GetGenes().Remove( b );

					b = n;
				}

				AdoptBins( dst.GetGenes(), unassigned, ccb.GetItems() );
			}
		}

		void BinMutationOperation::operator ()(Chromosome::GaChromosome& chromosome,
			const Chromosome::GaMutationParams& parameters) const
		{
			BinChromosome& c = ( (BinChromosome&)chromosome );

			int cSize = c.GetStructure().GetCount();
			int mSize = ( (const Chromosome::GaMutationSizeParams&)parameters ).CalculateMutationSize( cSize );
			double prob = (double) mSize/ cSize;

			Bin::ItemList removed;

			while( mSize > 0 )
			{
				for( BinList::GaNodeType* n = c.GetGenes().GetHead(); n != NULL && mSize > 0;  )
				{
					BinList::GaNodeType* r = n;
					n = n->GetNext();

					if( GaGlobalRandomBoolGenerator->Generate( prob ) )
					{
						mSize--;

						r->GetValue().MoveItems( removed );
					}
				}
			}

			AdoptBins( c.GetGenes(), removed, ( (const BinConfigBlock&)*c.GetConfigBlock() ).GetItems() );
		}

	}
}


#include "TNG.h"
#include "..\source\RandomSequence.h"

namespace Problems
{
	namespace TNG
	{
		int GACALL TngAdd(int a, int b) { return a + b; }

		int GACALL TngSub(int a, int b) { return a - b; }

		int GACALL TngMul(int a, int b) { return a * b; }

		int GACALL TngDiv(int a, int b) { return !b ? a : a / b; }

		typedef int (GACALL *TngOp)(int, int);

		TngOp TngOps[] = { TngAdd, TngSub, TngMul, TngDiv };

		inline int GACALL TngOpExec(TngNodeType nodeType,
			int a,
			int b) { return TngOps[ nodeType - 1 ]( a, b ); }

		char TngOpsChar[] = { '+', '-', '*', '/' };

		void PrintHelper(const Common::Data::GaTreeNode<TngNode>* node,
			const int* values)
		{
			TngNodeType type = node->GetValue()._type;
			const Common::Data::GaTreeNode<TngNode>* parent =  node->GetParent();

			if( type == TNT_NUMBER )
			{
				printf( "%d", values[ node->GetValue()._value ] );
				return;
			}

			// bool printParent = ( type == TNT_PLUS || type == TNT_MINUS ) && parent && ( parent->GetValue()._type == TNT_TIMES || parent->GetValue()._type == TNT_OVER );

			printf( "(" );
			PrintHelper( (const Common::Data::GaTreeNode<TngNode>*)node->GetChildren()->GetHead()->GetValue(), values );
			printf( "%c", TngOpsChar[ type - 1 ] );
			PrintHelper( (const Common::Data::GaTreeNode<TngNode>*)node->GetChildren()->GetTail()->GetValue(), values );
			printf( ")" );
		}

		void GACALL TngPrintChromosome(const TngChromosome& chromosome)
		{
			const Common::Data::GaTreeNode<TngNode>* root = chromosome.GetGenes().GetRoot();
			if( root )
				PrintHelper( root, ( (const TngConfigBlock&)*chromosome.GetConfigBlock() ).GetNumbers() );

			printf( "\n" );
		}
		
		class TngNodeValueBuffer
		{

		public:

			struct TngNodeValue
			{
				int _value;
				Common::Data::GaTreeNode<TngNode>* _node;
			};

		private:

			TngNodeValue _buffer[ 2 * TNG_NUMBER_COUNT - 1 ];

			int _count;

		public:

			TngNodeValueBuffer() : _count(0) { }

			inline void GACALL Add(Common::Data::GaTreeNode<TngNode>* node,
				int value,
				bool sort)
			{
				int index = _count++ - 1;

				if( sort )
				{
					for( ; index >= 0 && _buffer[ index ]._value > value; index-- )
						_buffer[ index + 1 ] = _buffer[ index ];
				}

				TngNodeValue& entry = _buffer[ index + 1 ];
				entry._value = value;
				entry._node = node;
			}

			inline void GACALL Add(const TngNodeValueBuffer& buffer)
			{
				for( int i = buffer._count - 1; i >= 0; i-- )
					_buffer[ _count++ ] = buffer._buffer[ i ];
			}

			inline TngNodeValue& GetLast() { return _buffer[ _count - 1 ]; }

			inline int GetCount() const { return _count; }

			inline TngNodeValue& operator [](int index) { return _buffer[ index ]; }

		};

		Common::Data::GaTreeNode<TngNode>* GACALL TngReduceTree(Common::Data::GaTreeNode<TngNode>* node,
			TngNodeValueBuffer& parentBuffer,
			const int* numbers)
		{
			if( node->GetValue()._type == TNT_NUMBER )
			{
				parentBuffer.Add( node, numbers[ node->GetValue()._value ], false );
				return NULL;
			}

			TngNodeValueBuffer buffer;

			Common::Data::GaTreeNode<TngNode>* replacement = NULL;

			replacement = TngReduceTree( (Common::Data::GaTreeNode<TngNode>*)node->GetChildren()->GetHead()->GetValue(), buffer, numbers );
			if( replacement )
			{
				Common::Data::GaTreeNodeBase* remove = node->GetChildren()->GetHead()->GetValue();

				node->InsertChild( replacement, 0 );

				remove->Detach();
				delete remove;
			}

			int left = buffer.GetLast()._value;

			replacement = TngReduceTree( (Common::Data::GaTreeNode<TngNode>*)node->GetChildren()->GetTail()->GetValue(), buffer, numbers );
			if( replacement )
			{
				Common::Data::GaTreeNodeBase* remove = node->GetChildren()->GetTail()->GetValue();

				node->InsertChild( replacement );

				remove->Detach();
				delete remove;
			}

			int right = buffer.GetLast()._value;

			int result = TngOpExec( node->GetValue()._type, left, right );

			for( int i = buffer.GetCount() - 1; i >= 0; i-- )
			{
				if( buffer[ i ]._value == result )
				{
					TngReduceTree( buffer[ i ]._node, parentBuffer, numbers );
					return buffer[ i ]._node;
				}
			}

			parentBuffer.Add( buffer );
			parentBuffer.Add( node, result, false );

			return NULL;
		}

		void GACALL TngReduceTree(Common::Data::GaTree<TngNode>* tree,
			const int* numbers)
		{
			TngNodeValueBuffer buffer;
			Common::Data::GaTreeNode<TngNode>* newRoot = TngReduceTree( tree->GetRoot(), buffer, numbers );
			if( newRoot )
			{
				tree->DetachNode( newRoot );
				tree->Clear();
				tree->Insert( newRoot, NULL );
			}
			else
			{
				Common::Data::GaTreeNodeBase* root = tree->GetRoot();
				tree->DetachNode( root );
				tree->Insert( root, NULL );
			}
		}

		void GACALL TngNormalizeTree(Common::Data::GaTreeNode<TngNode>* node,
			TngNodeValueBuffer& parentBuffer,
			int previousOp,
			const int* numbers)
		{
			TngNodeValueBuffer buffer;
			bool sorted = previousOp == TNT_TIMES || previousOp == TNT_PLUS;

			switch( node->GetValue()._type )
			{

			case TNT_NUMBER:
					parentBuffer.Add( node->Clone(), numbers[ node->GetValue()._value ], sorted );
					break;

			case TNT_TIMES:
			case TNT_PLUS:

				if( previousOp == node->GetValue()._type )
				{
					TngNormalizeTree( (Common::Data::GaTreeNode<TngNode>*)node->GetChildren()->GetHead()->GetValue(), parentBuffer, node->GetValue()._type, numbers );
					TngNormalizeTree( (Common::Data::GaTreeNode<TngNode>*)node->GetChildren()->GetTail()->GetValue(), parentBuffer, node->GetValue()._type, numbers );
				}
				else
				{
					TngNormalizeTree( (Common::Data::GaTreeNode<TngNode>*)node->GetChildren()->GetHead()->GetValue(), buffer, node->GetValue()._type, numbers );
					TngNormalizeTree( (Common::Data::GaTreeNode<TngNode>*)node->GetChildren()->GetTail()->GetValue(), buffer, node->GetValue()._type, numbers );

					int value;
					Common::Data::GaTreeNode<TngNode> *parent = NULL, *root = NULL;
					for( int i = buffer.GetCount() - 1; i > 0; i-- )
					{
						Common::Data::GaTreeNode<TngNode>* newNode = node->Clone();

						if( parent )
						{
							parent->InsertChild( newNode, 0 );
							value = TngOpExec( node->GetValue()._type, value, buffer[ i ]._value );
						}
						else
						{
							root = newNode;
							value = buffer[ i ]._value;
						}

						newNode->InsertChild( buffer[ i ]._node );

						parent = newNode;
					}

					parent->InsertChild( buffer[ 0 ]._node, 0 );
					parentBuffer.Add( root, TngOpExec( node->GetValue()._type, value, buffer[ 0 ]._value ), sorted );
				}

				break;

			case TNT_MINUS:
			case TNT_OVER:

				TngNormalizeTree( (Common::Data::GaTreeNode<TngNode>*)node->GetChildren()->GetHead()->GetValue(), buffer, node->GetValue()._type, numbers );
				TngNormalizeTree( (Common::Data::GaTreeNode<TngNode>*)node->GetChildren()->GetTail()->GetValue(), buffer, node->GetValue()._type, numbers );

				Common::Data::GaTreeNode<TngNode>* newNode = node->Clone();

				newNode->InsertChild( buffer[ 0 ]._node );
				newNode->InsertChild( buffer[ 1 ]._node );

				parentBuffer.Add( newNode, TngOpExec( node->GetValue()._type, buffer[ 0 ]._value, buffer[ 1 ]._value ), sorted );

				break;
			}
		}

		void GACALL TngNormalizeTree(Common::Data::GaTree<TngNode>* tree,
			const int* numbers)
		{
			TngNodeValueBuffer buffer;
			TngNormalizeTree( (Common::Data::GaTreeNode<TngNode>*)tree->GetRoot(), buffer, -1, numbers );

			tree->Clear();
			tree->Insert( buffer[ 0 ]._node, NULL );
		}

		void GACALL TngIdentifyFreeValues(Common::Data::GaSingleDimensionArray<bool>& freeValues,
			Common::Data::GaTreeNode<TngNode>* node,
			Common::Data::GaTreeNode<TngNode>* removed)
		{
			if( node == removed )
				return;

			if( node->GetValue()._type != TNT_NUMBER )
			{
				TngIdentifyFreeValues( freeValues, (Common::Data::GaTreeNode<TngNode>*)node->GetChildren()->GetHead()->GetValue(), removed );
				TngIdentifyFreeValues( freeValues, (Common::Data::GaTreeNode<TngNode>*)node->GetChildren()->GetTail()->GetValue(), removed );
			}
			else
				freeValues[ node->GetValue()._value ] = false;
		}

		void GACALL TngIdentifyDuplicateValues(Common::Data::GaSingleDimensionArray<bool>& freeValues,
			Common::Data::GaSingleDimensionArray<Common::Data::GaTreeNode<TngNode>*>& duplicateValue,
			Common::Data::GaTreeNode<TngNode>* node,
			int* count)
		{
			if( node->GetValue()._type != TNT_NUMBER )
			{
				TngIdentifyDuplicateValues( freeValues, duplicateValue, (Common::Data::GaTreeNode<TngNode>*)node->GetChildren()->GetHead()->GetValue(), count );
				TngIdentifyDuplicateValues( freeValues, duplicateValue, (Common::Data::GaTreeNode<TngNode>*)node->GetChildren()->GetTail()->GetValue(), count );
			}
			else
			{
				bool& free = freeValues[ node->GetValue()._value ];
				if( !free )
					duplicateValue[ ( *count )++ ] = node;

				free = false;
			}
		}

		void GACALL TngAdoptDuplicateValues(Common::Data::GaSingleDimensionArray<bool>& freeValues,
			Common::Data::GaSingleDimensionArray<Common::Data::GaTreeNode<TngNode>*>& duplicateValue,
			int count)
		{
			for( int j = count - 1; j >= 0; j-- )
			{
				int original = duplicateValue[ j ]->GetValue()._value;
				for( int i = TNG_NUMBER_COUNT - 1; i >= 0; i-- )
				{
					if( freeValues[ i ] )
					{
						duplicateValue[ j ]->GetValue()._value = i;
						freeValues[ i ] = false;

						break;
					}
				}
			}
		}

		void GACALL TngReplaceNode(Common::Data::GaTree<TngNode>& tree,
			Common::Data::GaTreeNode<TngNode>* oldNode,
			Common::Data::GaTreeNode<TngNode>* newNode)
		{
			Common::Data::GaSingleDimensionArray<bool> freeValue( TNG_NUMBER_COUNT, true );
			TngIdentifyFreeValues( freeValue, tree.GetRoot(), oldNode );

			int count = 0;
			Common::Data::GaSingleDimensionArray<Common::Data::GaTreeNode<TngNode>*> duplicateValue( TNG_NUMBER_COUNT );
			TngIdentifyDuplicateValues( freeValue, duplicateValue, newNode, &count );
			TngAdoptDuplicateValues( freeValue, duplicateValue, count );

			Common::Data::GaTreeNode<TngNode>* parent = oldNode->GetParent();
			int insertPos = parent ? parent->GetChildren()->GetPosition( oldNode ): 0;

			tree.Remove( oldNode );
			tree.Insert( newNode, parent, insertPos );
		}

		void GACALL TngSelectNodes(Common::Data::GaTree<TngNode>& tree1,
			Common::Data::GaTree<TngNode>& tree2,
			Common::Data::GaTreeNode<TngNode>** oldNode,
			Common::Data::GaTreeNode<TngNode>** newNode)
		{
			Common::Data::GaTreeNode<TngNode> *node1 = NULL, *node2 = NULL;
			int freeCount1 = TNG_NUMBER_COUNT - ( tree1.GetCount() + 1 ) / 2, freeCount2 = TNG_NUMBER_COUNT - ( tree2.GetCount() + 1 ) / 2;
			int count1 = 0, count2 = 0;

			do
			{
				node1 = tree1.GetRandomNode();
				node2 = tree2.GetRandomNode();
			} while( freeCount1 + ( node1->GetCount() + 1 ) / 2 < ( node2->GetCount() + 1 ) / 2 );

			*oldNode = node1;
			*newNode = (Common::Data::GaTreeNode<TngNode>*)node2->CopySubtree();
		}

		void TngCrossover::operator()(Chromosome::GaCrossoverBuffer& crossoverBuffer,
			const Chromosome::GaCrossoverParams& parameters) const
		{
			TngTree& source1 = ( (TngChromosome&)*crossoverBuffer.GetParentChromosome( 0 ) ).GetGenes();
			Chromosome::GaChromosomePtr offspring1 = crossoverBuffer.GetParentChromosome( 0 )->Clone();
			TngTree& destination1 = ( (TngChromosome&)*offspring1 ).GetGenes();

			TngTree& source2 = ( (TngChromosome&)*crossoverBuffer.GetParentChromosome( 0 ) ).GetGenes();
			Chromosome::GaChromosomePtr offspring2 = crossoverBuffer.GetParentChromosome( 1 )->Clone();
			TngTree& destination2 = ( (TngChromosome&)*offspring2 ).GetGenes();

			const int* numbers = ( (TngConfigBlock&)*( (TngChromosome&)*offspring1 ).GetConfigBlock() ).GetNumbers();

			Common::Data::GaTreeNode<TngNode> *oldNode, *newNode;

			TngSelectNodes( destination1, source2, &oldNode, &newNode );
			TngReplaceNode( destination1, oldNode, newNode );
			TngReduceTree( &destination1, numbers );
			TngNormalizeTree( &destination1, numbers );

			TngSelectNodes( destination2, source1, &oldNode, &newNode );
			TngReplaceNode( destination2, oldNode, newNode );
			TngReduceTree( &destination2, numbers );
			TngNormalizeTree( &destination2, numbers );

			crossoverBuffer.StoreOffspringChromosome( offspring1, 0 );
			crossoverBuffer.StoreOffspringChromosome( offspring2, 1 );
		}

		void GACALL TngMutation::operator ()(Chromosome::GaChromosome& chromosome,
			const Chromosome::GaMutationParams& parameters) const
		{
			const int* numbers = ( (TngConfigBlock&)*chromosome.GetConfigBlock() ).GetNumbers();
			TngTree& tree = ( (TngChromosome&)chromosome).GetGenes();

			if( GaGlobalRandomBoolGenerator->Generate( 0.5 ) || tree.GetCount() < 2 )
			{
				Common::Data::GaTreeNode<TngNode>* node =  tree.GetRandomNode();
				if( node->GetValue()._type == TNT_NUMBER )
				{
					Common::Data::GaSingleDimensionArray<bool> freeValue( TNG_NUMBER_COUNT, true );
					TngIdentifyFreeValues( freeValue, tree.GetRoot(), node );

					for( int i = TNG_NUMBER_COUNT - 1; i >= 0; i-- )
					{
						if( freeValue[ i ] && node->GetValue()._value != i )
						{
							node->GetValue()._value = i;
							break;
						}
					}
				}
				else
					node->GetValue()._type = (TngNodeType)GaGlobalRandomIntegerGenerator->Generate( TNT_PLUS, TNT_OVER );
			}
			else
			{
				Common::Data::GaTreeNode<TngNode>* node1 = NULL;
				Common::Data::GaTreeNode<TngNode>* node2 = NULL;

				do
				{
					node1 = tree.GetRandomNode( true );
					node2 = tree.GetRandomNode( true );
				} while( node1 == node2 || node1->IsConnected( node2 ) );

				tree.SwapNodes( node1, node2 );
			}
			
			TngReduceTree( &tree, numbers );
			TngNormalizeTree( &tree, numbers );
		}

		int GACALL TngCalculateValue(const Common::Data::GaTreeNode<TngNode>* node,
			const int* values)
		{
			if( node->GetValue()._type == TNT_NUMBER )
				return values[ node->GetValue()._value ];

			int result1 = TngCalculateValue( (const Common::Data::GaTreeNode<TngNode>*)node->GetChildren()->GetHead()->GetValue(), values );
			int result2 = TngCalculateValue( (const Common::Data::GaTreeNode<TngNode>*)node->GetChildren()->GetTail()->GetValue(), values );
			return TngOpExec( node->GetValue()._type, result1, result2 );
		}

		void TngFitnessOperation::operator ()(const Chromosome::GaChromosome& object,
			Fitness::GaFitness& fitness,
			const Fitness::GaFitnessOperationParams& operationParams) const
		{
			const TngConfigBlock& block = (const TngConfigBlock&)*object.GetConfigBlock();

			int value = abs( block.GetTargetNumber() - TngCalculateValue( ( (const TngChromosome&)object ).GetGenes().GetRoot(), block.GetNumbers() ) );
			( (TngFitness&)fitness ).SetValue( 1.0f / ( 1 + (float)( value ) ) );
		}

		Chromosome::GaChromosomePtr TngInitializator::operator ()(const Chromosome::GaInitializatorParams& parameters,
			Common::Memory::GaSmartPtr<Chromosome::GaChromosomeConfigBlock> configBlock) const
		{
			int count = GaGlobalRandomIntegerGenerator->Generate( 1, TNG_NUMBER_COUNT );

			int selected[ TNG_NUMBER_COUNT ];
			Common::Data::GaTreeNode<TngNode>* freeNodes[ TNG_NUMBER_COUNT ];

			Common::Random::GaGenerateRandomSequence( 0, TNG_NUMBER_COUNT - 1, count, selected );
			for( int i = count - 1; i >= 0; i-- )
				freeNodes[ i ] = new Common::Data::GaTreeNode<TngNode>( TngNode( TNT_NUMBER, i ) );

			while( count > 1 )
			{
				int merge[ 2 ];
				Common::Random::GaGenerateRandomSequenceAsc( 0, count - 1, 2, merge );

				Common::Data::GaTreeNode<TngNode>* merged = new Common::Data::GaTreeNode<TngNode>( TngNode( (TngNodeType)GaGlobalRandomIntegerGenerator->Generate( TNT_PLUS, TNT_OVER ) ) );
				merged->InsertChild( freeNodes[ merge[ 0 ] ] );
				merged->InsertChild( freeNodes[ merge[ 1 ] ] );

				freeNodes[ merge[ 0 ] ] = merged;
				for( int i = merge[ 1 ]; i < count - 1; i++ )
					freeNodes[ i ] = freeNodes[ i + 1 ];

				count--;
			}

			TngChromosome* newChromosome = new TngChromosome( configBlock );
			newChromosome->GetGenes().Insert( freeNodes[ 0 ], NULL );

			const int* numbers = ( (TngConfigBlock&)*configBlock ).GetNumbers();
			TngReduceTree( &newChromosome->GetGenes(), numbers );
			TngNormalizeTree( &newChromosome->GetGenes(), numbers );

			return newChromosome;
		}

		bool CompareTngNodes(const Common::Data::GaTreeNode<TngNode>* n1,
			const Common::Data::GaTreeNode<TngNode>* n2,
			const int* numbers)
		{
			if( n1->GetValue()._type != n2->GetValue()._type || n1->GetChildren()->GetCount() != n2->GetChildren()->GetCount() )
				return false;

			if( n1->GetValue()._type == TNT_NUMBER )
				return numbers[ n1->GetValue()._value ] == numbers[ n2->GetValue()._value ];

			for( const Common::Data::GaListNode<Common::Data::GaTreeNodeBase*> *c1 = n1->GetChildren()->GetHead(), *c2 = n2->GetChildren()->GetHead(); c1 ;
				c1 = c1->GetNext(), c2 = c2->GetNext() )
			{
				if( !CompareTngNodes( (Common::Data::GaTreeNode<TngNode>*)c1->GetValue(), (Common::Data::GaTreeNode<TngNode>*)c2->GetValue(), numbers ) )
					return false;
			}

			return true;
		}

		bool TngChromosomeComparator::Equal(const Chromosome::GaChromosome& chromosome1,
			const Chromosome::GaChromosome& chromosome2,
			const Chromosome::GaChromosomeComparatorParams& parameters) const
		{
			return CompareTngNodes( ( (const TngChromosome&)chromosome1 ).GetGenes().GetRoot(),
				( (const TngChromosome&)chromosome2 ).GetGenes().GetRoot(),
				( (TngConfigBlock&)*chromosome1.GetConfigBlock() ).GetNumbers() );
		}

	}
} // Problems

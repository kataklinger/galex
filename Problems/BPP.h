
#ifndef __PR_BPP_H__
#define __PR_BPP_H__

#include "..\source\FitnessValues.h"
#include "..\source\ChromosomeOperations.h"
#include "..\source\Representation.h"

namespace Problems
{
	namespace BPP
	{

		class BinConfigBlock : public Chromosome::GaChromosomeConfigBlock
		{

		public:

			struct Item
			{
				
				std::string _label;

				float _size;

				Item() : _size(0) { }

				Item(const std::string& label,
					float size) : _label(label),
						_size(size) { }

				Item(const Item& src) : _label(src._label),
					_size(src._size) { }

				inline Item& operator =(const Item& rhs)
				{
					_label = rhs._label;
					_size = rhs._size;

					return *this;
				}
			};

		private:

			Common::Data::GaSingleDimensionArray<Item> _items;

			Common::Data::GaSingleDimensionArray<int> _indices;

			float _binCapacity;

		public:

			BinConfigBlock(const Common::Data::GaSingleDimensionArray<Item>& items, float binCapacity) : _binCapacity(binCapacity) { SetItems( items ); }

			BinConfigBlock(const BinConfigBlock& rhs) : GaChromosomeConfigBlock(rhs),
				_binCapacity(rhs._binCapacity) { SetItems( rhs._items ); }

			virtual GaChromosomeConfigBlock* GACALL Clone() const { return new BinConfigBlock( *this ); }

			inline const Common::Data::GaSingleDimensionArray<Item>& GACALL GetItems() const { return _items; }

			void GACALL SetItems(const Common::Data::GaSingleDimensionArray<Item>& items);

			inline const Common::Data::GaSingleDimensionArray<int>& GACALL GetIndices() const { return _indices; }

			inline float GACALL GetBinCapacity() const { return _binCapacity; }

			inline void GACALL SetBinCapacity(float binCapacity) { _binCapacity = binCapacity; }

		};

		class Bin
		{

		public:

			typedef Common::Data::GaList<int> ItemList;

		private:

			ItemList _items;

			float _capacity;

			float _fill;

		public:

			Bin(float capacity) : _capacity(capacity),
				_fill(0) { }

			Bin(const Bin& src) : _items(src._items),
				_capacity(src._capacity),
				_fill(src._fill) { }

			void MoveItems(ItemList& dest);

			void MoveItems(ItemList& dest,
				const Common::Data::GaSingleDimensionArray<bool>& skip);

			bool TryAdd(int item,
				float size);

			bool TryReplace(ItemList& unassigned,
				ItemList::GaNodeType* current,
				const Common::Data::GaSingleDimensionArray<BinConfigBlock::Item>& items);

			inline float GetCapacity() const { return _capacity; }

			inline float GetFill() const { return _fill; }

			inline float GetFree() const { return _capacity - _fill; }

			inline bool IsEmpty() const { return _items.GetCount() == 0; }

			inline ItemList& GetItems() { return _items; }

			inline const ItemList& GetItems() const { return _items; }

			inline bool operator ==(const Bin& rhs) const { return _items == rhs._items; }

			inline bool operator !=(const Bin& rhs) const { return _items != rhs._items; }

		};

		typedef Common::Data::GaList<Bin> BinList;
		typedef Chromosome::Representation::GaListChromosome<Bin>::GaType BinChromosome;

		class BinInitializator : public Chromosome::GaInitializator
		{

		public:

			virtual Chromosome::GaChromosomePtr GACALL operator ()(bool empty,
				const Chromosome::GaInitializatorParams& parameters,
				Common::Memory::GaSmartPtr<Chromosome::GaChromosomeConfigBlock> configBlock) const;

			virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

		};

		typedef Fitness::Representation::GaSVFitness<float> BinFitness;

		class BinFitnessOperationParams : public Fitness::GaFitnessOperationParams
		{

		private:

			float _kParam;

		public:

			BinFitnessOperationParams() : _kParam(2) { }

			BinFitnessOperationParams(float kParam) : _kParam(kParam) { }

			BinFitnessOperationParams(const BinFitnessOperationParams& params) : _kParam(params._kParam) { }

			virtual GaParameters* GACALL Clone() const { return new BinFitnessOperationParams( *this ); }

			inline float GACALL GetKParam() const { return _kParam; }

			inline void GACALL GetKParam(float kParam) { _kParam = kParam; }

		};

		class BinFitnessOperation : public Chromosome::GaChromosomeFitnessOperation
		{

		public:

			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const { return new BinFitness( params ); }

			virtual void GACALL operator ()(const GaObjectType& object,
				Fitness::GaFitness& fitness,
				const Fitness::GaFitnessOperationParams& operationParams) const;

			virtual Common::GaParameters* GACALL CreateParameters() const  { return new BinFitnessOperationParams(); }

		};

		class BinCrossoverOperation : public Chromosome::GaCrossoverOperation
		{

		public:

			virtual void GACALL operator ()(Chromosome::GaCrossoverBuffer& crossoverBuffer,
				const Chromosome::GaCrossoverParams& parameters) const;

			virtual int GACALL GetParentCount(const Chromosome::GaCrossoverParams& parameters) const { return 2; }

			virtual int GACALL GetOffspringCount(const Chromosome::GaCrossoverParams& parameters) const { return 2; }

			virtual Common::GaParameters* GACALL CreateParameters() const { return new Chromosome::GaCrossoverParams(); }

		};

		class BinMutationOperation : public Chromosome::GaMutationOperation
		{

		public:

			virtual void GACALL operator ()(Chromosome::GaChromosome& chromosome,
				const Chromosome::GaMutationParams& parameters) const;

			virtual Common::GaParameters* GACALL CreateParameters() const  { return new Chromosome::GaMutationSizeParams(); }

		};

	}
}

#endif // __PR_BPP_H__

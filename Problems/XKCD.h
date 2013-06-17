
#ifndef __PR_XKCD_H__
#define __PR_XKCD_H__

#include "..\source\FitnessValues.h"
#include "..\source\ChromosomeOperations.h"
#include "..\source\Representation.h"

namespace Problems
{
	namespace XKCD
	{

		class XkcdConfigBlock : public Chromosome::GaChromosomeConfigBlock
		{

		public:

			struct Appetizer
			{
				
				std::string _name;

				float _price;

				float _time;

				Appetizer() : _price(0),
					_time(0) { }

				Appetizer(const std::string& name,
					float price,
					float time) : _name(name),
						_price(price),
						_time(time) { }

			};

		private:

			Common::Data::GaSingleDimensionArray<Appetizer> _appetizers;

			Chromosome::Representation::GaIntervalAlleleSet<int> _interval;

		public:

			XkcdConfigBlock(const Common::Data::GaSingleDimensionArray<Appetizer>& appetizers) : _appetizers( appetizers ),
				_interval(Chromosome::Representation::GaValueIntervalBounds<int>( 0, appetizers.GetSize() - 1 ),
				Chromosome::Representation::GaValueIntervalBounds<int>( 0, appetizers.GetSize() - 1 ), GaGlobalRandomIntegerGenerator) { }

			XkcdConfigBlock(const XkcdConfigBlock& rhs) : GaChromosomeConfigBlock(rhs),
				_appetizers(rhs._appetizers),
				_interval(rhs._interval) { }

			virtual GaChromosomeConfigBlock* GACALL Clone() const { return new XkcdConfigBlock( *this ); }

			inline const Common::Data::GaSingleDimensionArray<Appetizer>& GACALL GetAppetizers() const { return _appetizers; }

			void GACALL SetAppetizers(const Common::Data::GaSingleDimensionArray<Appetizer>& appetizers);

			inline Chromosome::Representation::GaIntervalAlleleSet<int>& GetInterval() { return _interval; }

		};

		typedef Chromosome::Representation::GaAlleleGene<int> XkcdGene;
		typedef Common::Data::GaList<XkcdGene> XkcdGeneList;

		typedef Chromosome::Representation::GaAdvanceListChromosome<int, Chromosome::Representation::GaAlleleGene>::GaType XkcdChromosome;

		class XkcdChromosomeComparator : public Chromosome::GaChromosomeComparator
		{

		public:

			virtual float GACALL operator ()(const Chromosome::GaChromosome& chromosome1,
				const Chromosome::GaChromosome& chromosome2,
				const Chromosome::GaChromosomeComparatorParams& parameters) const { return Equal( chromosome1, chromosome2, parameters ) ? 0.0f : 1.0f; }

			virtual bool GACALL Equal(const Chromosome::GaChromosome& chromosome1,
				const Chromosome::GaChromosome& chromosome2,
				const Chromosome::GaChromosomeComparatorParams& parameters) const
				{ return ( (const XkcdChromosome&)chromosome1 ).GetGenes() == ( (const XkcdChromosome&)chromosome2 ).GetGenes(); }

			virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

		};

		class XkcdInitializator : public Chromosome::GaInitializator
		{

		public:

			virtual Chromosome::GaChromosomePtr GACALL operator ()(bool empty,
				const Chromosome::GaInitializatorParams& parameters,
				Common::Memory::GaSmartPtr<Chromosome::GaChromosomeConfigBlock> configBlock) const;

			virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

		};

		typedef Fitness::Representation::GaWeightedFitness<float, float> XkcdFitness;

		class XkcdFitnessOperationParams : public Fitness::GaFitnessOperationParams
		{

		private:

			float _targetPrice;

		public:

			XkcdFitnessOperationParams() : _targetPrice(0) { }

			XkcdFitnessOperationParams(float targetPrice) : _targetPrice(targetPrice) { }

			XkcdFitnessOperationParams(const XkcdFitnessOperationParams& params) : _targetPrice(params._targetPrice) { }

			virtual GaParameters* GACALL Clone() const { return new XkcdFitnessOperationParams( *this ); }

			inline float GACALL GetTargetPrice() const { return _targetPrice; }

			inline void GACALL SetTargetPrice(float price) { _targetPrice = price; }

		};

		class XkcdFitnessOperation : public Chromosome::GaChromosomeFitnessOperation
		{

		public:

			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const { return new XkcdFitness( params ); }

			virtual void GACALL operator ()(const GaObjectType& object,
				Fitness::GaFitness& fitness,
				const Fitness::GaFitnessOperationParams& operationParams) const;

			virtual Common::GaParameters* GACALL CreateParameters() const  { return new XkcdFitnessOperationParams(); }

		};

		class XkcdMutationOperation : public Chromosome::GaMutationOperation
		{

		public:

			virtual void GACALL operator ()(Chromosome::GaChromosome& chromosome,
				const Chromosome::GaMutationParams& parameters) const;

			virtual Common::GaParameters* GACALL CreateParameters() const  { return new Chromosome::GaMutationSizeParams(); }

		};

	}
}

#endif //__PR_TNG_H__

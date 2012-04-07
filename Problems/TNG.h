
#ifndef __PR_TNG_H__
#define __PR_TNG_H__

#include "..\source\Flags.h"
#include "..\source\ChromosomeOperations.h"
#include "..\source\Representation.h"
#include "..\source\FitnessValues.h"
#include "..\source\FitnessComparators.h"

namespace Problems
{
	namespace TNG
	{

		const int TNG_NUMBER_COUNT = 6;

		enum TngNodeType
		{
			TNT_NUMBER,
			TNT_PLUS,
			TNT_MINUS,
			TNT_TIMES,
			TNT_OVER
		};

		struct TngNode
		{

			TngNodeType _type;

			int _value;

			TngNode() : _type(TNT_NUMBER),
				_value(-1) { }

			TngNode(TngNodeType type) : _type(type),
				_value(-1) { }

			TngNode(TngNodeType type,
				int value) : _type(type),
				_value(value) { }

			inline bool GACALL operator ==(const TngNode& rhs) const { return _type == rhs._type && _value == rhs._value; }

			inline bool GACALL operator !=(const TngNode& rhs) const { return !operator ==( rhs ); }

		};

		class TngConfigBlock : public Chromosome::GaChromosomeConfigBlock
		{

		private:

			int _numbers[ TNG_NUMBER_COUNT ];

			int _targetNumber;

		public:

			TngConfigBlock(const Chromosome::GaChromosomeParams* chromosomeParams) : GaChromosomeConfigBlock(chromosomeParams) { }

			TngConfigBlock(int* numbers,
				int targetNumber,
				const Chromosome::GaChromosomeParams* chromosomeParams) : GaChromosomeConfigBlock(chromosomeParams),
				_targetNumber(targetNumber) { SetNumbers( numbers ); }

			TngConfigBlock(const TngConfigBlock& rhs) : GaChromosomeConfigBlock(rhs),
				_targetNumber(rhs._targetNumber) { SetNumbers( rhs._numbers ); }

			virtual Chromosome::GaChromosomeConfigBlock* GACALL Clone() const { return new TngConfigBlock( *this ); }

			inline void GACALL SetNumbers(const int* numbers)
			{
				for( int i = TNG_NUMBER_COUNT - 1; i >= 0; i-- )
					_numbers[ i ] = numbers[ i ];
			}

			inline int* GetNumbers() { return _numbers; }

			inline const int* GetNumbers() const { return _numbers; }

			inline void GACALL SetTargetNumber(int number) { _targetNumber = number; }

			inline int GACALL GetTargetNumber() const { return _targetNumber; }

		};

		typedef Common::Data::GaTree<TngNode> TngTree;

		typedef Chromosome::Representation::GaTreeChromosome<TngNode>::GaType TngChromosome;

		void GACALL TngPrintChromosome(const TngChromosome& chromosome);

		class TngCrossover : public Chromosome::GaCrossoverOperation
		{

		public:

			virtual void GACALL operator()(Chromosome::GaCrossoverBuffer& crossoverBuffer,
				const Chromosome::GaCrossoverParams& parameters) const;

			virtual int GACALL GetParentCount(const Chromosome::GaCrossoverParams& parameters) const { return 2; }

			virtual int GACALL GetOffspringCount(const Chromosome::GaCrossoverParams& parameters) const { return 2; }

			virtual Common::GaParameters* GACALL CreateParameters() const { return new Chromosome::GaCrossoverParams(); }

		};

		class TngMutation : public Chromosome::GaMutationOperation
		{

		public:

			virtual void GACALL operator ()(Chromosome::GaChromosome& chromosome,
				const Chromosome::GaMutationParams& parameters) const;

			virtual Common::GaParameters* GACALL CreateParameters() const { return new Chromosome::GaMutationParams(); }

		};

		int GACALL TngCalculateValue(const Common::Data::GaTreeNode<TngNode>* node,
			const int* values);

		typedef Fitness::Representation::GaSVFitness<float> TngFitness;

		class TngFitnessOperation : public Chromosome::GaChromosomeFitnessOperation
		{

		public:

			virtual void GACALL operator ()(const Chromosome::GaChromosome& object,
				Fitness::GaFitness& fitness,
				const Fitness::GaFitnessOperationParams& operationParams) const;

			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const { return new TngFitness( params ); }

			virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

		};

		typedef Fitness::Comparators::GaSimpleComparator TngFitnessComparator;

		class TngInitializator : public Chromosome::GaInitializator
		{

		public:

			virtual Chromosome::GaChromosomePtr GACALL operator ()(const Chromosome::GaInitializatorParams& parameters,
				Common::Memory::GaSmartPtr<Chromosome::GaChromosomeConfigBlock> configBlock) const;

			virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

		};

		class TngChromosomeComparator : public Chromosome::GaChromosomeComparator
		{

		public:

			virtual float GACALL operator ()(const Chromosome::GaChromosome& chromosome1,
				const Chromosome::GaChromosome& chromosome2,
				const Chromosome::GaChromosomeComparatorParams& parameters) const { return Equal( chromosome1, chromosome2, parameters ) ? 0.0f : 1.0f; }

			virtual bool GACALL Equal(const Chromosome::GaChromosome& chromosome1,
				const Chromosome::GaChromosome& chromosome2,
				const Chromosome::GaChromosomeComparatorParams& parameters) const;

			virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

		};

	}
} // Problems

#endif // __PR_TNG_H__


#include "CSP.h"
#include "..\source\RandomSequence.h"

#include <algorithm>

namespace Problems
{
	namespace CSP
	{
		Point& Point::operator =(const Point& rhs)
		{
			_x = rhs._x;
			_y = rhs._y;

			return *this;
		}

		Point& Point::operator +=(const Size& rhs)
		{
			_x += rhs.GetWidth();
			_y += rhs.GetHeight();

			return *this;
		}

		Point& Point::operator -=(const Size& rhs)
		{
			_x -= rhs.GetWidth();
			_y -= rhs.GetHeight();

			return *this;
		}

		int Size::FitFirst(Size& size) const
		{
			int fit = FitOriginal( size );
			if( fit < 0 )
			{
				Size rotated = size.GetRotated();
				fit = FitOriginal( rotated );

				if( fit >= 0 )
					size = rotated;
			}

			return fit;
		}

		int Size::FitBest(Size& size) const
		{
			int fit = FitOriginal( size );

			Size rotated = size.GetRotated();

			int rotatedFit = FitOriginal( rotated );
			if( rotatedFit >= 0 && ( fit < 0 || rotatedFit < fit ) )
			{
				size = rotated;
				fit = rotatedFit;
			}

			return fit;
		}

		Size& Size::operator +=(const Size& rhs)
		{
			_width += rhs._width;
			_height += rhs._height;

			return *this;
		}

		Size& Size::operator -=(const Size& rhs)
		{
			_width -= rhs._width;
			_height -= rhs._height;

			return *this;
		}

		Size& Size::operator =(const Size& rhs)
		{
			_width = rhs._width;
			_height = rhs._height;

			return *this;
		}

		void AddSlot(std::vector<Slot>& slots, const Slot& slot)
		{
			for( std::vector<Slot>::iterator it = slots.begin(); it != slots.end(); ++it )
			{
				if( it->Merge( slot ) )
					return;
			}

			slots.push_back( slot );
		}

		void Slot::Place(const Placement& placement, std::vector<Slot>& slots)
		{
			if( _area.IsOverlapping( placement.GetArea() ) )
			{
				int distance;
				const Rectangle& area = placement.GetArea();

				distance = area.GetPosition().GetX() - _area.GetPosition().GetX();
				if( distance > 0 )
					AddSlot( slots, Slot( _area.GetPosition(), Size( distance, _area.GetSize().GetHeight() ) ) );

				distance = _area.GetLimit().GetX() - area.GetLimit().GetX();
				if( distance > 0 )
					AddSlot( slots, Slot( Point( area.GetLimit().GetX(), _area.GetPosition().GetY() ), Size( distance, _area.GetSize().GetHeight() ) ) );

				distance = area.GetPosition().GetY() - _area.GetPosition().GetY();
				if( distance > 0 )
					AddSlot( slots, Slot( _area.GetPosition(), Size( _area.GetSize().GetWidth(), distance ) ) );

				distance = _area.GetLimit().GetY() - area.GetLimit().GetY();
				if( distance > 0 )
					AddSlot( slots, Slot( Point( _area.GetPosition().GetX(), area.GetLimit().GetY() ), Size( _area.GetSize().GetWidth(), distance ) ) );
			}
			else
				AddSlot( slots, *this );
		}

		bool Slot::Merge(const Slot& slot)
		{
			if( _area.GetSize().GetWidth() == slot._area.GetSize().GetWidth() && _area.GetPosition().GetX() == slot._area.GetPosition().GetX() )
			{
				if( ( slot._area.GetPosition().GetY() >= _area.GetPosition().GetY() && slot._area.GetPosition().GetY() <= _area.GetLimit().GetY() ) ||
					( slot._area.GetLimit().GetY() >= _area.GetPosition().GetY() && slot._area.GetLimit().GetY() <= _area.GetLimit().GetY() ) )
				{
					int y1 = std::min( slot._area.GetPosition().GetY(), _area.GetPosition().GetY() );
					int y2 = std::max( slot._area.GetLimit().GetY(), _area.GetLimit().GetY() );

					_area = Rectangle( Point( slot._area.GetPosition().GetX(), y1 ), Size( _area.GetSize().GetWidth(), y2 - y1 ) );
					return true;
				}
			}
			else if ( _area.GetSize().GetHeight() == slot._area.GetSize().GetHeight() && _area.GetPosition().GetY() == slot._area.GetPosition().GetY() )
			{
				if( ( slot._area.GetPosition().GetX() >= _area.GetPosition().GetX() && slot._area.GetPosition().GetX() <= _area.GetLimit().GetX() ) ||
					( slot._area.GetLimit().GetX() >= _area.GetPosition().GetX() && slot._area.GetLimit().GetX() <= _area.GetLimit().GetX() ) )
				{
					int x1 = std::min( slot._area.GetPosition().GetX(), _area.GetPosition().GetX() );
					int x2 = std::max( slot._area.GetLimit().GetX(), _area.GetLimit().GetX() );

					_area = Rectangle( Point( x1, slot._area.GetPosition().GetY() ), Size( x2 - x1, _area.GetSize().GetHeight() ) );
					return true;
				}
			}

			return false;
		}

		void Sheet::Place(const Placement& placement)
		{
			_placements.push_back( placement );

			std::vector<Slot> slots;
			for( std::vector<Slot>::iterator it = _slots.begin(); it != _slots.end(); ++it )
				it->Place( placement, slots );

			_slots = slots;
		}

		void Sheet::Remove(Common::Data::GaSingleDimensionArray<int>& removed)
		{
			std::vector<Placement> oldPlacements( _placements );

			_placements.clear();
			_slots.clear();

			_slots.push_back( Slot( Point(), _size ) );

			int i = 0, j = 0;
			for( std::vector<Placement>::iterator it = oldPlacements.begin(); it != oldPlacements.end(); ++it )
			{
				if( i != removed[ j ] )
					Place( *it );
				else
					removed[ j++ ] = it->GetItem().GetIndex();

				i++;
			}
		}

		void Sheet::Clear()
		{
			_placements.clear();
			_slots.clear();
		}

		Sheet& Sheet::operator =(const Sheet& rhs)
		{
			_size = rhs._size;
			_placements = rhs._placements;
			_slots = rhs._slots;

			return *this;
		}

		struct ClosestDistanceHeuristic
		{

		private:
			
			Point _original;

		public:
			
			ClosestDistanceHeuristic(const Point& original) : _original(original) { }

			bool operator()(Placement& placement, Size orientation, bool rotation, const std::vector<Slot>& slots) const
			{
				double distance = 0;
				bool placed = false;
				for( std::vector<Slot>::const_iterator it = slots.begin(); it != slots.end(); ++it )
				{
					if( it->GetArea().GetSize().FitBest( orientation ) >= 0 )
					{
						double d = it->GetArea().GetPosition().Distance(_original);
						if( !placed || d < distance )
						{
							placement.SetArea( it->GetArea().GetPosition(), orientation );

							distance = d;
							placed = true;
						}
					}
				}

				return placed;
			}

		};

		bool LowestPositionHeuristic(Placement& placement, Size orientation, bool rotation, const std::vector<Slot>& slots)
		{
			bool placed = false;
			for( std::vector<Slot>::const_iterator it = slots.begin(); it != slots.end(); ++it )
			{
				if( it->GetArea().GetSize().FitBest( orientation ) >= 0 )
				{
					if( !placed || ( it->GetArea().GetPosition().GetX() <= placement.GetArea().GetPosition().GetX() &&
						it->GetArea().GetPosition().GetY() <= placement.GetArea().GetPosition().GetY() ) )
					{
						placement.SetArea( it->GetArea().GetPosition(), orientation );

						placed = true;
					}
				}
			}

			return placed;
		}

		bool BestFitHeuristic(Placement& placement, Size orientation, bool rotation, const std::vector<Slot>& slots)
		{
			int fit = 0;
			bool placed = false;
			for( std::vector<Slot>::const_iterator it = slots.begin(); it != slots.end(); ++it )
			{
				int f = it->GetArea().GetSize().FitBest( orientation );
				if( f >= 0 )
				{
					if( !placed || f < fit )
					{
						placement.SetArea( it->GetArea().GetPosition(), orientation );

						fit = f;
						placed = true;
					}
				}
			}

			return placed;
		}

		Chromosome::GaChromosomePtr CspInitializator::operator ()(bool empty,
			const Chromosome::GaInitializatorParams& parameters,
			Common::Memory::GaSmartPtr<Chromosome::GaChromosomeConfigBlock> configBlock) const
		{
			CspChromosome* chromosome = new CspChromosome( configBlock );

			if( !empty )
			{
				CspConfigBlock& b = ( (CspConfigBlock&)( *configBlock ) );

				const Common::Data::GaSingleDimensionArray<Item>& items = b.GetItems();

				chromosome->GetGenes().Copy( &b.GetItems() );
				Common::Random::GaShuffle( chromosome->GetGenes().GetArray(), chromosome->GetGenes().GetSize() );
			}

			return chromosome;
		}

		void CspFitnessOperation::operator ()(const CspFitnessOperation::GaObjectType& object,
			Fitness::GaFitness& fitness,
			const Fitness::GaFitnessOperationParams& operationParams) const
		{
			const CspChromosome& c = ( (const CspChromosome&)object );
			CspFitness& f = (CspFitness&)fitness;

			const Size& sheetSize = ( (CspConfigBlock&)( *c.GetConfigBlock() ) ).GetSheetSize();
			const Common::Data::GaSingleDimensionArray<Item>& items = ( (CspConfigBlock&)( *c.GetConfigBlock() ) ).GetItems();
			const Common::Data::GaSingleDimensionArray<int>& order = c.GetGenes();

			Sheet sheet( sheetSize );
			for( int i = order.GetSize() - 1; i >= 0; i-- )
				sheet.Place(BestFitHeuristic, items[ order[ i ] ], true );

			int savedArea = 0;
			const std::vector<Slot>& slots = sheet.GetSlots();
			for( std::vector<Slot>::const_iterator it = slots.begin(); it != slots.end(); ++it )
			{
				int a = it->GetArea().GetSize().GetArea();
				if( a > savedArea )
					savedArea = a;
			}

			const std::vector<Placement>& placements = sheet.GetPlacements();
			f.SetValue( (float)placements.size() / items.GetSize() * savedArea / sheetSize.GetArea() );
		}

		void Correction(Common::Data::GaSingleDimensionArray<int>& genes)
		{
			Common::Data::GaSingleDimensionArray<int> counter( genes.GetSize() );
			for( int i = genes.GetSize(); i >= 0; i++ )
			{
				if( ++counter[ genes[ i ] ] > 1 )
					genes[ i ] = -1;
			}

			for( int i = counter.GetSize(); i >= 0; i++ )
			{
				if( genes[ i ] < 0 )
				{
					for( int j = counter.GetSize(); j >= 0; j++ )
					{
						if( counter[ j ] == 0 )
						{
							counter[ j ] = 1;
							genes[ i ] = j;
						}
					}
				}
			}
		}

		void CspCrossoverOperation::operator ()(Chromosome::GaCrossoverBuffer& crossoverBuffer,
			const Chromosome::GaCrossoverParams& parameters) const
		{
			Common::Data::GaSingleDimensionArray<int>* source1 = &( (CspChromosome&)*crossoverBuffer.GetParentChromosome( 0 ) ).GetGenes();
			Common::Data::GaSingleDimensionArray<int>* source2 = &( (CspChromosome&)*crossoverBuffer.GetParentChromosome( 1 ) ).GetGenes();

			int count = ( (const Chromosome::GaCrossoverPointParams&)parameters ).GetNumberOfCrossoverPoints() + 1;
			Common::Memory::GaAutoPtr<int> points( new int[ count ], Common::Memory::GaArrayDeletionPolicy<int>::GetInstance() );

			for( int i = ( (const Chromosome::GaCrossoverPointParams&)parameters ).GetNumberOfOffspring() - 1; i >= 0; i -= 2 )
			{
				Chromosome::GaChromosomePtr offspring2, offspring1 = crossoverBuffer.CreateOffspringFromPrototype();
				Common::Data::GaSingleDimensionArray<int>* destination1 = &( (CspChromosome&)*offspring1 ).GetGenes();

				Common::Data::GaSingleDimensionArray<int>* destination2 = NULL;
				if( i > 0 )
				{
					offspring2 = crossoverBuffer.CreateOffspringFromPrototype();
					destination2 = &( (CspChromosome&)*offspring2 ).GetGenes();
				}

				if( count > destination1->GetSize() )
					count  = destination1->GetSize();

				if( count > 1 )
					Common::Random::GaGenerateRandomSequenceAsc( 1, destination1->GetSize() - 1, count - 1, true, points.GetRawPtr() );

				points[ count - 1 ] = source1->GetSize();
				for( int j = 0, s = 0; j < count ; s = points[ j++ ] )
				{
					destination1->Copy( source1, s, s, points[ j ] - s );

					if( destination2 )
						destination2->Copy( source2, s, s, points[ j ] - s );

					Common::Data::GaSingleDimensionArray<int>* t = source1;
					source1 = source2;
					source2 = t;
				}

				Correction( *destination1 );
				crossoverBuffer.StoreOffspringChromosome( offspring1, 0 );

				if( !offspring2.IsNull() )
				{
					Correction( *destination2 );
					crossoverBuffer.StoreOffspringChromosome( offspring2, 1 );
				}
			}
		}

	}
}

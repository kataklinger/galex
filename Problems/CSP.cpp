
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

		void CspChromosome::MutationEvent(GaChromosome::GaMuataionEvent e)
		{
			switch( e )
			{
			case Chromosome::GaChromosome::GAME_PREPARE: _backup = _sheet; break;
			case Chromosome::GaChromosome::GAME_ACCEPT: _backup.Clear(); break;
			case Chromosome::GaChromosome::GAME_REJECT: _sheet = _backup; break;
			}
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
				Common::Data::GaSingleDimensionArray<int> shuffled( items.GetSize() );
				Common::Random::GaGenerateRandomSequence( 0, items.GetSize() - 1, shuffled.GetArray() );

				Sheet& sheet = chromosome->GetSheet();
				for( int i = items.GetSize() - 1; i >= 0; i-- )
					sheet.Place(BestFitHeuristic, items[ shuffled[ i ] ], items[ shuffled[ i ] ].GetSize(), true );
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

			//int area = 0;
			//Size placementSize;

			//
			//for( std::vector<Placement>::const_iterator it = placements.begin(); it != placements.end(); ++it )
			//{
			//	area += it->GetArea().GetSize().GetArea();
			//	if( it->GetArea().GetLimit().GetX() > placementSize.GetWidth() )
			//		placementSize.SetWidth( it->GetArea().GetLimit().GetX() );

			//	if( it->GetArea().GetLimit().GetY() > placementSize.GetHeight() )
			//		placementSize.SetHeight( it->GetArea().GetLimit().GetY() );
			//}

			int savedArea = 0;
			const std::vector<Slot>& slots = c.GetSheet().GetSlots();
			for( std::vector<Slot>::const_iterator it = slots.begin(); it != slots.end(); ++it )
			{
				int a = it->GetArea().GetSize().GetArea();
				if( a > savedArea )
					savedArea = a;
			}

			const std::vector<Placement>& placements = c.GetSheet().GetPlacements();
			f.SetValue( (float)placements.size() / items.GetSize() * savedArea / sheetSize.GetArea() );
		}

		void CspCrossoverOperation::operator ()(Chromosome::GaCrossoverBuffer& crossoverBuffer,
			const Chromosome::GaCrossoverParams& parameters) const
		{
			typedef int (Point::*GetLengthPtr)() const;

			const CspConfigBlock& ccb = ( (const CspConfigBlock&)*crossoverBuffer.GetParentChromosome( 0 )->GetConfigBlock() );

			const Common::Data::GaSingleDimensionArray<Item>& items = ccb.GetItems();

			Point limit = ccb.GetSheetSize();
			int count = items.GetSize();

			GetLengthPtr getLength = GaGlobalRandomBoolGenerator->Generate() ? &Point::GetX : &Point::GetY;
			int point = GaGlobalRandomIntegerGenerator->Generate( 0, ( limit.*getLength )() );

			Common::Data::GaSingleDimensionArray<bool> processed( count );

			for( int i = 0; i < 2; i++ )
			{
				const Sheet& src1 = ( (const CspChromosome&)*crossoverBuffer.GetParentChromosome( i ) ).GetSheet();
				const Sheet& src2 = ( (const CspChromosome&)*crossoverBuffer.GetParentChromosome( 1 - i ) ).GetSheet();

				Chromosome::GaChromosomePtr offspring = new CspChromosome( crossoverBuffer.GetParentChromosome( 0 )->GetConfigBlock() );
				Sheet& dst = ( (CspChromosome&)*offspring ).GetSheet();

				for( std::vector<Placement>::const_iterator it = src1.GetPlacements().begin(); it != src1.GetPlacements().end(); ++it )
				{
					if( ( it->GetArea().GetLimit().*getLength )() > point )
					{
						dst.Place( *it );
						processed[ it->GetItem().GetIndex() ] = true;
					}
				}

				for( std::vector<Placement>::const_iterator it = src2.GetPlacements().begin(); it != src2.GetPlacements().begin(); ++it )
				{
					int j = it->GetItem().GetIndex();
					if( ( it->GetArea().GetLimit().*getLength )() <= point && !processed[ j ] )
					{
						dst.Place( ClosestDistanceHeuristic( it->GetArea().GetPosition() ), it->GetItem(), it->GetArea().GetSize(), false );

						processed[ j ] = true;
					}
				}

				for( int j = count - 1; j >= 0; j-- )
				{
					if( !processed[ j ] )
						dst.Place( LowestPositionHeuristic, items[ j ], items[ j ].GetSize(), true );

					processed[ j ] = false;
				}

				crossoverBuffer.StoreOffspringChromosome( offspring, i );
			}
		}

		void CspMutationOperation::operator ()(Chromosome::GaChromosome& chromosome,
			const Chromosome::GaMutationParams& parameters) const
		{
			const Common::Data::GaSingleDimensionArray<Item>& items = ( (const CspConfigBlock&)*chromosome.GetConfigBlock() ).GetItems();

			Sheet& sheet = ( (CspChromosome&)chromosome ).GetSheet();

			const std::vector<Placement>& placements = sheet.GetPlacements();
			int cSize = placements.size();
			int mSize = ( (const Chromosome::GaMutationSizeParams&)parameters ).CalculateMutationSize( cSize );

			Common::Data::GaSingleDimensionArray<int> removed( mSize );
			Common::Random::GaGenerateRandomSequenceAsc( 0, cSize - 1, mSize, true, removed.GetArray() );

			sheet.Remove( removed );

			for( int i = mSize - 1; i >= 0; i-- )
			{
				const Item& item = items[ removed[ i ] ];
				sheet.Place( LowestPositionHeuristic, item, item.GetSize(), true );
			}
		}

	}
}

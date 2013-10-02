
#include "CSP.h"

#include <algorithm>

namespace Problems
{
	namespace BPP
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
				const Rectangle& space = placement.GetArea();

				distance = space.GetPosition().GetX() - _area.GetPosition().GetX();
				if( distance > 0 )
					AddSlot( slots, Slot( _area.GetPosition(), Size( distance, _area.GetSize().GetHeight() ) ) );

				distance = _area.GetLimit().GetX() - space.GetLimit().GetX();
				if( distance > 0 )
					AddSlot( slots, Slot( Point( space.GetLimit().GetX(), _area.GetPosition().GetY() ), Size( distance, _area.GetSize().GetHeight() ) ) );

				distance = space.GetPosition().GetY() - _area.GetPosition().GetY();
				if( distance > 0 )
					AddSlot( slots, Slot( _area.GetPosition(), Size( _area.GetSize().GetWidth(), distance ) ) );

				distance = _area.GetLimit().GetY() - space.GetLimit().GetY();
				if( distance > 0 )
					AddSlot( slots, Slot( Point( _area.GetPosition().GetX(), space.GetLimit().GetY() ), Size( _area.GetSize().GetWidth(), distance ) ) );
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


		bool Sheet::Place(const Item& item)
		{
			Placement placement( item );

			double distance = 0;
			bool placed = false;
			for( std::vector<Slot>::iterator it = _slots.begin(); it != _slots.end(); ++it )
			{
				if( it->GetArea().GetSize().Fits( item.GetSize() ) )
				{
					double d = std::sqrt( std::pow( it->GetArea().GetPosition().GetX(), 2 ) + std::pow( it->GetArea().GetPosition().GetY(), 2 ) );
					if( !placed || d < distance
						/* it->GetArea().GetPosition().GetX() <= placement.GetArea().GetPosition().GetX() &&
						   it->GetArea().GetPosition().GetY() <= placement.GetArea().GetPosition().GetY() */ )
					{
						placement.SetArea( it->GetArea().GetPosition(), item.GetSize() );

						distance = d;
						placed = true;
					}
				}
			}

			if( placed )
			{
				_placements.push_back( placement );

				std::vector<Slot> slots;
				for( std::vector<Slot>::iterator it = _slots.begin(); it != _slots.end(); ++it )
					it->Place( placement, slots );

				_slots = slots;
			}

			return placed;
		}

		Chromosome::GaChromosomePtr CspInitializator::operator ()(bool empty,
			const Chromosome::GaInitializatorParams& parameters,
			Common::Memory::GaSmartPtr<Chromosome::GaChromosomeConfigBlock> configBlock) const
		{
			return Chromosome::GaChromosomePtr();
		}

		void CspFitnessOperation::operator ()(const CspFitnessOperation::GaObjectType& object,
			Fitness::GaFitness& fitness,
			const Fitness::GaFitnessOperationParams& operationParams) const
		{
		}

		void CspCrossoverOperation::operator ()(Chromosome::GaCrossoverBuffer& crossoverBuffer,
			const Chromosome::GaCrossoverParams& parameters) const
		{
		}

		void CspMutationOperation::operator ()(Chromosome::GaChromosome& chromosome,
			const Chromosome::GaMutationParams& parameters) const
		{
		}

	}
}

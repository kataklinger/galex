
#ifndef __PR_CSP_H__
#define __PR_CSP_H__

#include "..\source\FitnessValues.h"
#include "..\source\ChromosomeOperations.h"
#include "..\source\Representation.h"

namespace Problems
{
	namespace BPP
	{

		class Size;

		class Point
		{

		private:

			int _x;

			int _y;

		public:

			Point(int x, int y) : _x(x),
				_y(y) { }

			Point(const Point& src) : _x(src._x),
				_y(src._y) { }

			Point() : _x(0),
				_y(0) { }

			inline void SetX(int x) { _x = x; }

			inline int GetX() const { return _x; }

			inline void SetY(int y) { _y = y; }

			inline int GetY() const { return _y; }

			Point& operator +=(const Size& rhs);

			Point& operator -=(const Size& rhs);

			Point& operator =(const Point& rhs);

			friend Point operator +(const Point& lhs, const Size& rhs);

			friend Size operator -(const Point& lhs, const Point& rhs);

			friend Point operator -(const Point& lhs, const Size& rhs);

			friend bool operator ==(const Point& lhs, const Point& rhs);

			friend bool operator !=(const Point& lhs, const Point& rhs);

		};

		class Size
		{

		private:

			int _width;

			int _height;

		public:

			Size(int width, int height) : _width(width),
				_height(height) { }

			Size(const Size& src) : _width(src._width),
				_height(src._height) { }

			Size() : _width(0),
				_height(0) { }

			inline void Rotate() { std::swap( _width, _height ); }

			inline void SetWidth(int width) { _width = width; }

			inline int GetWidth() const { return _width; }

			inline void SetHeight(int height) { _height = height; }

			inline int GetHeight() const { return _height; }

			inline int GetArea() const { return _width * _height; }

			inline Size GetRotated() const { return Size( _height, _width ); }

			inline bool Fits(const Size& size) const { return _width >= size._width && _height >= size._height; }

			Size& operator +=(const Size& rhs);

			Size& operator -=(const Size& rhs);

			Size& operator =(const Size& rhs);

			friend Size operator +(const Size& lhs, const Size& rhs);

			friend Point operator +(const Point& lhs, const Size& rhs);

			friend Size operator -(const Size& lhs, const Size& rhs);

			friend Point operator -(const Point& lhs, const Size& rhs);

			friend bool operator ==(const Size& lhs, const Size& rhs);

			friend bool operator !=(const Size& lhs, const Size& rhs);

		};

		inline Point operator +(const Point& lhs, const Size& rhs) { return Point( lhs._x + rhs._width, lhs._y + rhs._height ); }

		inline Size operator -(const Point& lhs, const Point& rhs) { return Size( lhs._x - rhs._x, lhs._y - rhs._y ); }

		inline Point operator -(const Point& lhs, const Size& rhs) { return Point( lhs._x - rhs._width, lhs._y - rhs._height ); }

		inline bool operator ==(const Point& lhs, const Point& rhs) { return lhs._x == rhs._x && lhs._y == rhs._y; }

		inline bool operator !=(const Point& lhs, const Point& rhs) { return lhs._x != rhs._x || lhs._y != rhs._y; }

		inline Size operator +(const Size& lhs, const Size& rhs) { return Size( lhs._width + rhs._width, lhs._height + rhs._height ); }

		inline Size operator -(const Size& lhs, const Size& rhs) { return Size( lhs._width - rhs._width, lhs._height - rhs._height ); }

		inline bool operator ==(const Size& lhs, const Size& rhs) { return lhs._width == rhs._width && lhs._height == rhs._height; }

		inline bool operator !=(const Size& lhs, const Size& rhs) { return lhs._width != rhs._width || lhs._height != rhs._height; }

		class Rectangle
		{

		private:

			Point _position;

			Size _size;

			Point _limit;

		public:

			Rectangle() { }

			Rectangle(const Point& position, const Size& size) : _position(position),
				_size(size),
				_limit(_position + _size)  { }

			inline void GetPosition(const Point& position) { _limit = ( _position = position ) + _size; }

			inline const Point& GetPosition() const { return _position; }

			inline void SetSize(const Size& size) { _limit = _position + ( _size = size ); }

			inline const Size& GetSize() const { return _size; }

			inline const Point& GetLimit() const { return _limit; }

			inline bool IsOverlapping(const Rectangle& r)
				{ return _position.GetX() < r._limit.GetX() && _limit.GetX() > r._position.GetX() && _position.GetY() < r._limit.GetY() && _limit.GetY() > r._position.GetY(); }

		};

		class Item
		{

		protected:

			Size _size;

			std::string _label;

		public:

			Item(const Size& size, const std::string& label) : _size(size),
				_label(label) { }

			Item() { }

			inline const Size& GetSize() const { return _size; }

			inline void SetSize(const Size& size) { _size = size; }

			inline const std::string& GetLabel() const { return _label; }

			inline void SetLabel(const std::string& label) { _label = label; }

		};

		class Placement
		{

		private:

			const Item& _item;

			Rectangle _area;

		public:

			Placement(const Item& item) : _item(item) { }

			Placement(const Item& item, const Point& position, const Size& rotation) : _item(item),
				_area(position, rotation) { }

			inline const Item& GetItem() const { return _item; }

			void SetArea(const Point& position, const Size& rotation) { _area = Rectangle( position, rotation ); }

			inline const Rectangle& GetArea() const { return _area; }

		};

		class Slot
		{

		private:

			Rectangle _area;

		public:

			Slot(const Point& position, const Size& size) : _area(position, size) { }

			inline const Rectangle& GetArea() const { return _area; }

			void Place(const Placement& placement, std::vector<Slot>& slots);

			bool Merge(const Slot& slot);

		};

		class Sheet
		{

		private:

			Size _size;

			std::vector<Placement> _placements;

			std::vector<Slot> _slots;

		public:

			Sheet(const Size& size) : _size(size) { _slots.push_back( Slot( Point(), _size ) ); }

			bool Place(const Item& item);

			inline const Size& GetSize() const { return _size; }

			inline const std::vector<Placement> GetPlacements() const { return _placements; }

			inline const std::vector<Slot> GetSlot() const { return _slots; }

		};

		class CspConfigBlock : public Chromosome::GaChromosomeConfigBlock
		{

		public:

			virtual GaChromosomeConfigBlock* GACALL Clone() const { return new CspConfigBlock( *this ); }

		};

		class CspInitializator : public Chromosome::GaInitializator
		{

		public:

			virtual Chromosome::GaChromosomePtr GACALL operator ()(bool empty,
				const Chromosome::GaInitializatorParams& parameters,
				Common::Memory::GaSmartPtr<Chromosome::GaChromosomeConfigBlock> configBlock) const;

			virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

		};

		typedef Fitness::Representation::GaSVFitness<float> CspFitness;

		class CspFitnessOperation : public Chromosome::GaChromosomeFitnessOperation
		{

		public:

			virtual Fitness::GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const Fitness::GaFitnessParams> params) const { return new CspFitness( params ); }

			virtual void GACALL operator ()(const GaObjectType& object,
				Fitness::GaFitness& fitness,
				const Fitness::GaFitnessOperationParams& operationParams) const;

			virtual Common::GaParameters* GACALL CreateParameters() const  { return NULL; }

		};

		class CspCrossoverOperation : public Chromosome::GaCrossoverOperation
		{

		public:

			virtual void GACALL operator ()(Chromosome::GaCrossoverBuffer& crossoverBuffer,
				const Chromosome::GaCrossoverParams& parameters) const;

			virtual int GACALL GetParentCount(const Chromosome::GaCrossoverParams& parameters) const { return 2; }

			virtual int GACALL GetOffspringCount(const Chromosome::GaCrossoverParams& parameters) const { return 2; }

			virtual Common::GaParameters* GACALL CreateParameters() const { return new Chromosome::GaCrossoverParams(); }

		};

		class CspMutationOperation : public Chromosome::GaMutationOperation
		{

		public:

			virtual void GACALL operator ()(Chromosome::GaChromosome& chromosome,
				const Chromosome::GaMutationParams& parameters) const;

			virtual Common::GaParameters* GACALL CreateParameters() const  { return new Chromosome::GaMutationSizeParams(); }

		};

	}
}

#endif // __PR_CSP_H__

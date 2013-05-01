
/*! \file Alleles.h
    \brief This file declares and implements classes that handles chromosomes alleles.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_ALLELES_H__
#define __GA_ALLELES_H__

#include "GlobalRandomGenerator.h"
#include "Array.h"

namespace Chromosome
{
	namespace Representation
	{
		/// <summary><c>GaAlleleSet</c> template class is interface for value sets that defines possible values of chromosome's gene.
		/// 
		/// This class has built-in synchronizator so it is allowed to use <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros
		/// with instances of this class, but no public or private methods are thread-safe.</summary>
		/// <typeparam name="VALUE_TYPE">type of values stored in the set.</typeparam>
		template<typename VALUE_TYPE>
		class GaAlleleSet
		{

			GA_SYNC_CLASS

		public:

			/// <summary>Type of values stored in the set.</summary>
			typedef VALUE_TYPE GaType;

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaAlleleSet() { }

			/// <summary>This method generates random value from the set.</summary>
			/// <returns>Method returns randomly chosen value from the set.</returns>
			virtual void GACALL Generate(GaType& value) const = 0;

			/// <summary><c>Inverse</c> method finds inverted value and stores it into inverted.</summary>
			/// <param name="value">reference to variable whose value should be inverted.</param>
			/// <returns>Method returns <c>true</c> if inversion is successful. If specified value is not in set or it cannot be inverted it returns <c>false</c>.</returns>
			virtual bool GACALL Inverse(GaType& value) const = 0;

			/// <summary><c>Belongs</c> method checks membership of the value in this value set.</summary>
			/// <param name="value">value which is checked.</param>
			/// <returns>Method returns <c>true</c> if the value is member of the set.</returns>
			virtual bool GACALL Belongs(const GaType& value) const = 0;

			/// <summary><c>ClosestValue</c> method returns nearest value which can be found in value set to specified value.</summary>
			/// <param name="value">on input reference variable stores value whose nearest element in the set should be found.
			/// On output this variable will store closest value to the original that can be found in the set.</param>
			virtual void GACALL ClosestValue(GaType& value) const = 0;

			/// <summary><c>GetCount</c> method queries size of the value set.</summary>
			/// <returns>Method returns number of values in the set.</returns>
			virtual long long GACALL GetCount() const = 0;

		};

		/// <summary><c>AbsDiff</c> method calculates absolute value of diffrence between two provided values.</summary>
		/// <typeparam name="VALUE_TYPE">type of values.</typeparam>
		/// <param name="value1">the first value.</param>
		/// <param name="value2">the seconf value.</param>
		/// <returns>Method returns the diferrence.</returns>
		template<typename VALUE_TYPE>
		inline VALUE_TYPE GACALL AbsDiff(const VALUE_TYPE& value1,
			const VALUE_TYPE& value2) { return value1 < value2 ? value2 - value1 : value1 - value2; }

		/// <summary><c>GaUnrestrictedAlleleSet</c> template class represents value set with no additional limitations, except limitation introduced by the type and
		/// random generator used to generate values. 
		/// 
		/// This class has built-in synchronizator so it is allowed to use <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros
		/// with instances of this class, but no public or private methods are thread-safe.</summary>
		/// <typeparam name="VALUE_TYPE">type of values stored in the set. This type must support unary <c>operators -</c>.</typeparam>
		template<typename VALUE_TYPE>
		class GaUnrestrictedAlleleSet : public GaAlleleSet<VALUE_TYPE>
		{

		protected:

			/// <summary>Random generator which is used for generating of random values.</summary>
			Common::Random::GaRandom<GaType>* _randomGenerator;

		public:

			/// <summary>This constructor initializes value set with random nuber generator.</summary>
			/// <param name="randomGenerator">random generator which is used for generating of random values.</param>
			GaUnrestrictedAlleleSet(Common::Random::GaRandom<GaType>* randomGenerator) : _randomGenerator(randomGenerator) { }

			/// <summary>This constructor initializes value set without random nuber generator.</summary>
			GaUnrestrictedAlleleSet() : _randomGenerator(NULL) { }

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::Generate" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Generate(GaType& value) const { value = _randomGenerator->Generate(); }

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::Inverse" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual bool GACALL Inverse(GaType& value) const
			{
				value = -value;
				return true;
			}

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::Belongs" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual bool GACALL Belongs(const GaType& value) const { return true; }

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::ClosestValue" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL ClosestValue(GaType& value) const { }

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::GetCount" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual long long GACALL GetCount() const { return _randomGenerator->GetCount(); }

			/// <summary><c>SetRandomGenerator</c> method sets random benerato that will be used for generating of random values.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="generator">random generator.</param>
			inline void GACALL SetRandomGenerator(Common::Random::GaRandom<GaType>* generator) { _randomGenerator = generator; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to random generator which is used for generating of random values.</returns>
			inline const Common::Random::GaRandom<GaType>* GACALL GetRandomGenerator() const { return _randomGenerator; }

		};

		/// <summary><c>GaSinglePairAlleleSet</c> template represents value set with only one value and its counterpart (inverted value).
		/// 
		/// This class has built-in synchronizator so it is allowed to use <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros
		/// with instances of this class, but no public or private methods are thread-safe.</summary>
		/// <typeparam name="VALUE_TYPE">type of values stored in the set. This type must support unary <c>operators -</c>.</typeparam>
		template<typename VALUE_TYPE>
		class GaSinglePairAlleleSet : public GaAlleleSet<VALUE_TYPE>
		{

		protected:

			/// <summary>Original value of the set.</summary>
			GaType _original;

			/// <summary><see cref="_value" />'s counterpart (inverted value) in the set.</summary>
			GaType _inverted;

		public:

			/// <summary>This constructor initializes value set with default values or the original and its counterpart.</summary>
			GaSinglePairAlleleSet() : _original(),
				_inverted() { }

			/// <summary>This constructor initializes value set with value and its counterpart.</summary>
			/// <param name="original">original value.</param>
			/// <param name="inverted">inverted value.</param>
			GaSinglePairAlleleSet(const GaType& original,
				const GaType& inverted) : _original(original),
				_inverted(inverted) { }

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::Generate" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Generate(GaType& value) const { value = GaGlobalRandomBoolGenerator->Generate() ? _original : _inverted ; }

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::Inverse" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual bool GACALL Inverse(GaType& value) const
			{
				// invert
				if( value == _original )
					value = _inverted;
				else if( value == _inverted )
					value = _original;
				else
					// provided value does not belong to the set
					return false;

				return true;
			}

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::Belongs" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual bool GACALL Belongs(const GaType& value) const { return value == _original || value == _inverted; }

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::ClosestValue" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL ClosestValue(GaType& value) const { value = AbsDiff( _original, value ) < AbsDiff( _inverted, value ) ? _original : _inverted; }

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::GetCount" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual long long GACALL GetCount() const { return 2; }

			/// <summary><c>SetValues</c> method sets original value and its counterpart.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="original">new original value.</param>
			/// <param name="inverted">new inverted value.</param>
			inline void GACALL SetValues(const GaType& original,
				const GaType& inverted)
			{
				_original = original;
				_inverted = inverted;
			}

			/// <summary><c>SetOriginal</c> method sets original value.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">new original value.</param>
			inline void GACALL SetOriginal(const GaType& value) { _original = value; }

			/// <summary><c>SetInverted</c> method sets inverted value.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">new inverted value.</param>
			inline void GACALL SetInverted(const GaType& value) { _inverted = value; }

			/// <summary><c>GetValues</c> method returns original value and its counterpart.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="original">reference to variable that will store original value.</param>
			/// <param name="inverted">>reference to variable that will store inverted value.</param>
			inline void GACALL GetValues(GaType& original,
				GaType& inverted)
			{
				original = _original;
				inverted = _inverted;
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns original value.</returns>
			inline const GaType& GACALL GetOriginal() const { return _original; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns inverted value of the original.</returns>
			inline const GaType& GACALL GetInverted() const { return _inverted; }

		};

		/// <summary><c>GaMultiPairsAlleleSet</c> template represents value set with multiple values and their counterpart (inverted values).
		/// 
		/// This class has built-in synchronizator so it is allowed to use <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros
		/// with instances of this class, but no public or private methods are thread-safe.</summary>
		/// <typeparam name="VALUE_TYPE">type of values stored in the set. This type must support unary <c>operators -</c>.</typeparam>
		template<typename VALUE_TYPE>
		class GaMultiPairsAlleleSet : public GaAlleleSet<VALUE_TYPE>
		{

		protected:

			/// <summary>Original values of the set.</summary>
			Common::Data::GaSingleDimensionArray<GaType> _original;

			/// <summary>Inverted values of the set.</summary>
			Common::Data::GaSingleDimensionArray<GaType> _inverted;

		public:

			/// <summary>Initializes empty value set.</summary>
			GaMultiPairsAlleleSet() { }

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::Generate" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Generate(GaType& value) const
			{
				int pair = GaGlobalRandomIntegerGenerator->Generate( _original.GetSize() - 1 );
				value = GaGlobalRandomBoolGenerator->Generate() ? _original[ pair ] : _inverted[ pair ];
			}

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::Inverse" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual bool GACALL Inverse(GaType& value) const
			{
				// find value
				for( int i = _original.GetSize() - 1; i >= 0; i-- )
				{
					// value belongs to set of originals
					if( _original[ i ] == value )
					{
						// get inverted value
						value = _inverted[ value ];
						return true;
					}

					// value belongs to set of inverted values
					if( _inverted[ i ] == value )
					{
						// get original value
						value = _original[ value ];
						return true;
					}
				}

				// value does not belong to the set
				return false;
			}

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::Belongs" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual bool GACALL Belongs(const GaType& value) const
			{
				for( int i = _original.GetSize() - 1; i >= 0; i-- )
				{
					if( _original[ i ] == value || _inverted[ i ] == value )
						return true;
				}
			}

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::ClosestValue" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL ClosestValue(GaType& value) const
			{
				const GaType* closest = &_original[ 0 ];
				GaType closestDiff = AbsDiff( *closest, value );

				// probe each element in set to find closest
				for( int i = _original.GetSize() - 1; i >= 0; i-- )
				{
					if( _original[ i ] == value || _inverted[ i ] == value )
						return;

					// is it closer to current original
					GaType diff = AbsDiff( _original[ i ], value );
					if( diff < closestDiff )
					{
						closestDiff = diff;
						closest = &_original[ i ];
					}

					// is it closer to inverted value of current original
					diff = AbsDiff( _inverted[ i ], value );
					if( diff < closestDiff )
					{
						closestDiff = diff;
						closest = &_inverted[ i ];
					}
				}
			}

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::GetCount" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual long long GACALL GetCount() const { return _original.GetSize() + _inverted.GetSize(); }

			/// <summary><c>Add</c> method inserts new value and its counterpart to the value set.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="original">original value.</param>
			/// <param name="inverted">inverted value of the original.</param>
			/// <returns>Method returns position at which the value is inserted.</returns>
			int GACALL Add(const GaType& original,
				const GaType& inverted)
			{
				int pos = _original.GetSize();

				// resize set
				_original.SetSize( pos + 1 );
				_inverted.SetSize( pos + 1 );

				// add
				_original[ pos ] = original;
				_inverted[ pos ] = inverted;

				return pos;
			}

			/// <summary><c>Remove</c> method removes specified value and its counterpart from the set. Both sets, originals and inverted values, are searched.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">value that should be removed</param>
			void GACALL Remove(const GaType& value)
			{
				int i = 0, j = 0, last = _original.GetSize();

				// move remaining elements to fill gap made by removing value from the set
				for( ; j < last; j++ )
				{
					if( value != _original[ j ] && value != _inverted[ j ] )
					{
						_original[ i ] = _original[ j ];
						_inverted[ i ] = _inverted[ j ];
						i++;
					}
				}

				// resize set
				_original.SetSize( i );
				_inverted.SetSize( i );
			}

			/// <summary><c>RemoveAt</c> method removes value from both sets at the specified position.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="position">position of the value that should be removed.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>position</c> is less then <c>0</c> or greater then size of allele.</exception>
			void GACALL RemoveAt(int position)
			{
				int last = _original.GetSize() - 1;

				GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, position >= 0 && position <= last, 
					"position", "Position cannot be negative value and greater then size of allele.", "Representation" );

				// move remaining elements to fill gap made by removing value from the set
				for( int i = position; i < last; i++ )
				{
					_original[ i ] = _original[ i + 1 ];
					_inverted[ i ] = _inverted[ i + 1 ];
				}

				// resize set
				_original.SetSize( last );
				_inverted.SetSize( last );
			}

			/// <summary><c>SetValue</c> method sets original and inverted value in the set at specified position.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="original">new original value.</param>
			/// <param name="inverted">new inverted value.</param>
			/// <param name="position">position of the values in the set.</param>
			inline void GACALL SetValue(const GaType& original,
				const GaType& inverted,
				int position)
			{
				_original[ position ] = original;
				_inverted[ position ] = inverted;
			}

			/// <summary><c>SetOriginal</c> method sets value of the original at specified position in the set.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">new original value.</param>
			/// <param name="position">position of the value in the set.</param>
			inline void GACALL SetOriginal(const GaType& value,
				int position) { _original[ position ] = value; }

			/// <summary><c>SetInverted</c> method sets inverted value at specified position in the set.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">new inverted value.</param>
			/// <param name="position">position of the value in the set.</param>
			inline void GACALL SetInverted(const GaType& value,
				int position) { _inverted[ position ] = value; }

			/// <summary><c>GetValues</c> method returns value and its counterpart at given position.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="original">reference to variable that will store original value.</param>
			/// <param name="inverted">reference to variable that will store inverted value.</param>
			/// <param name="position">position of the value in the set.</param>
			inline void GACALL GetValues(GaType& original,
				GaType& inverted,
				int position)
			{
				original = _original[ position ];
				inverted = _inverted[ position ];
			}

			/// <summary><c>GetOriginal</c> method queries original value at specified location in the set.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="position">position of the value in the set.</param>
			/// <returns>Method returns the value.</returns>
			inline const GaType& GACALL GetOriginal(int position) const { return _original[ position ]; }

			/// <summary><c>GetInverted</c> method queries inverted value at specified location in the set.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="position">position of the value in the set.</param>
			/// <returns>Method returns the value.</returns>
			inline const GaType& GACALL GetInverted(int position) const { return _inverted[ position ]; }

		};

		/// <summary><c>GaValueIntervalBounds</c> template class represents bounds of interval value set.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of bounds. This type must support operators &gt;, &gt;=, &lt; and &lt;=.</param>
		template <typename VALUE_TYPE>
		class GaValueIntervalBounds
		{

		public:

			/// <summary>Type of bounds.</summary>
			typedef VALUE_TYPE GaType;

		private:

			/// <summary>Lower bound.</summary>
			GaType _lower;

			/// <summary>Higher bound.</summary>
			GaType _higher;

		public:

			/// <summary>This constructor initializes bounds with user-defined values. Bounds are automatically sorted.</summary>
			/// <param name="lower">value of lower bound.</param>
			/// <param name="higher">value of higher bound.</param>
			GaValueIntervalBounds(const GaType& lower,
				const GaType& higher) { SetBounds( lower, higher ); }

			/// <summary>This constructor creates undefined bounds.</summary>
			GaValueIntervalBounds() { }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns lower bound.</returns>
			inline const GaType& GACALL GetLowerBound() const { return _lower; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns higher bound.</returns>
			inline const GaType& GACALL GetHigherBound() const { return _higher; }

			/// <summary><c>GetBounds</c> method returns both bounds.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="lower">reference to variable that will store lower bound.</param>
			/// <param name="higher">reference to variable that will store higher bound.</param>
			inline void GACALL GetBounds(GaType& lower,
				GaType& higher)
			{
				lower = _lower;
				higher = _higher;
			}

			/// <summary><c>SetLowerBound</c> method sets new lower bound. If new value for lower bound is higher then higher bound, call is ignored.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="lower">new value for lower bound.</param>
			inline void GACALL SetLowerBound(const GaType& lower)
			{
				if( lower <= _higher )
					_lower = lower;
			}

			/// <summary><c>SetHigherBound</c> method sets new higher bound. If new value for higher bound is lower then lower bound, call is ignored.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="higher">new value for higher bound.</param>
			inline void GACALL SetHigherBound(const GaType& higher)
			{
				if( _lower <= higher )
					_higher = higher;
			}

			/// <summary><c>SetBound</c> method sets both bounds. Bounds are automatically sorted.
			/// 
			/// This method is not thread-safe.</summary>
			/// <param name="lower">new value for lower bound.</param>
			/// <param name="higher">new value for higher bound.</param>
			void GACALL SetBounds(const GaType& lower,
				const GaType& higher)
			{
				// bounds already sorted
				if( lower <= higher )
				{
					_lower = lower;
					_higher = higher;
				}
				else
				{
					// resort bounds
					_lower = higher;
					_higher = lower;
				}
			}

			/// <summary><c>InBounds</c> method checks value to see if it is in bounds of the interval.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">value which is checked.</param>
			/// <returns>Method returns <c>true</c> if value is in bound.</returns>
			inline bool GACALL InBounds(const GaType& value) const  { return value >= _lower && value <= _higher; }

		};

		/// <summary><c>GaIntervalAlleleSet</c> template class represents value set which has interval of values. Intervals as specified by theirs bounds.
		/// This value set uses user-specified random generator to generate values within the interval. 
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of values stored in the set. This type must support operators &gt;, &gt;=, &lt; and &lt;=.</param>
		template <typename VALUE_TYPE>
		class GaIntervalAlleleSet : public GaAlleleSet<VALUE_TYPE>
		{

		private:

			/// <summary>Bounds of interval of original values. </summary>
			GaValueIntervalBounds<GaType> _original;

			/// <summary>Bounds of interval of inverted values.</summary>
			GaValueIntervalBounds<GaType> _inverted;

			/// <summary>Random generator which is used for generating of random values within interval.</summary>
			Common::Random::GaRandom<GaType>* _randomGenerator;

		public:

			/// <summary>This constructor initializes value set with bounds and random generator.</summary>
			/// <param name="original">bounds of interval of original values.</param>
			/// <param name="inverted">bounds of interval of inverted values.</param>
			/// <param name="randomGenerator">pointer to random generator.</param>
			GaIntervalAlleleSet(const GaValueIntervalBounds<GaType>& original,
				const GaValueIntervalBounds<GaType>& inverted,
				Common::Random::GaRandom<GaType>* randomGenerator) : _original(original),
				_inverted(inverted),
				_randomGenerator(randomGenerator) { }

			/// <summary>This constructor initializes value set with random generator, but doesn't define bounds of intervals.</summary>
			/// <param name="randomGenerator">pointer to random generator.</param>
			GaIntervalAlleleSet(Common::Random::GaRandom<GaType>* randomGenerator) : _randomGenerator(randomGenerator) { }

			/// <summary>This constructor initializes empty value set.</summary>
			GaIntervalAlleleSet() : _randomGenerator(NULL) { }

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::Generate" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Generate(GaType& value) const
			{
				value = GaGlobalRandomBoolGenerator->Generate()
					? _randomGenerator->Generate( _original.GetLowerBound(), _original.GetHigherBound() )
					: _randomGenerator->Generate( _inverted.GetLowerBound(), _inverted.GetHigherBound() );
			}

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::Inverse" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual bool GACALL Inverse(GaType& value) const
			{				
				if( _original.InBounds( value ) || _inverted.InBounds( value ) )
				{
					value = _inverted.GetLowerBound() + _original.GetHigherBound() - value;
					return true;
				}

				return false;
			}

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::Belongs" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual bool GACALL Belongs(const GaType& value) const { return _original.InBounds( value ) || _inverted.InBounds( value ); }

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::ClosestValue" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL ClosestValue(GaType& value) const
			{
				if( !_original.InBounds( value ) && !_inverted.InBounds( value ) )
				{
					// get bounds
					const GaType* bounds[ 4 ];
					bounds[ 0 ] = &_original.GetLowerBound();
					bounds[ 1 ] = &_original.GetHigherBound();
					bounds[ 2 ] = &_inverted.GetLowerBound();
					bounds[ 3 ] = &_inverted.GetHigherBound();

					// start of search
					int closest = 3;
					GaType closestDiff = AbsDiff( *bounds[ closest ], value );

					// find closes bounds
					for( int i = closest - 1; i >= 0; i-- )
					{
						// calculate distance
						GaType diff = AbsDiff( *bounds[ i ], value );

						// closer
						if( diff < closestDiff )
						{
							closest = i;
							closestDiff = diff;
						}
					}

					value = *bounds[ closest ];
				}
			}

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::GetCount" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual long long GACALL GetCount() const
			{
				return _randomGenerator->GetCount( _original.GetLowerBound(), _original.GetHigherBound() ) +
					_randomGenerator->GetCount( _inverted.GetLowerBound(), _inverted.GetHigherBound() );
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns bounds of interval of original values.</returns>
			inline const GaValueIntervalBounds<GaType>& GACALL GetValueBounds() const { return _original; }

			/// <summary><c>SetValueBounds</c> method sets bounds of interval of original values.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="bounds">new bound of interval.</param>
			inline void GACALL SetValueBounds(const GaValueIntervalBounds<GaType>& bounds) { _original = bounds; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns bounds of interval of inverted values.</returns>
			inline const GaValueIntervalBounds<GaType>& GACALL GetInvertedValueBounds() const { return _inverted; }

			/// <summary><c>SetInvertedValueBounds</c> method sets bounds of interval of inverted values.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="bounds">new bound of interval.</param>
			inline void GACALL SetInvertedValueBounds(const GaValueIntervalBounds<GaType>& bounds) { _inverted = bounds; }

			/// <summary><c>GetBounds</c> method returns bounds of both intervals.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="original">reference to object that will store bounds of original values interval.</param>
			/// <param name="inverted">reference to variable that will store bounds of inverted values interval.</param>
			inline void GACALL GetBounds(GaValueIntervalBounds<GaType>& original,
				GaValueIntervalBounds<GaType>& inverted) const
			{
				original = _original;
				inverted = _inverted;
			}

			/// <summary><c>SetBounds</c> method sets bounds of both intervals.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="original">new bounds of interval of original values.</param>
			/// <param name="inverted">new bounds of interval of inverted values.</param>
			inline void GACALL SetBounds(const GaValueIntervalBounds<GaType>& original,
				const GaValueIntervalBounds<GaType>& inverted)
			{
				_original = original;
				_inverted = inverted;
			}

			/// <summary><c>SetRandomGenerator</c> method sets random generator which is used for generating of random values within interval.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="generator">random generator.</param>
			inline void GACALL SetRandomGenerator(Common::Random::GaRandom<GaType>* generator) { _randomGenerator = generator; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to random generator which is used for generating of random values within interval.</returns>
			inline const Common::Random::GaRandom<GaType>* GACALL GetRandomGenerator() const { return _randomGenerator; }

		};

		/// <summary> <c>GaCombinedAlleleSet</c> template represents value set consisted of different value sets.
		/// 
		/// This class has built-in synchronizator so it is allowed to use <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros
		/// with instances of this class, but no public or private methods are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of values stored in the set.</param>
		template <typename VALUE_TYPE>
		class GaCombinedAlleleSet : public GaAlleleSet<VALUE_TYPE>
		{

		private:

			/// <summary>Register value sets.</summary>
			Common::Data::GaSingleDimensionArray<GaAlleleSet<GaType>*> _alleles;

		public:

			/// <summary>This constructor initializes empty value set.</summary>
			GaCombinedAlleleSet() { }

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::Generate" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Generate(GaType& value) const { _alleles[ GaGlobalRandomIntegerGenerator->Generate( _alleles.GetSize() - 1 ) ]->Generate( value ); }

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::Inverse" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual bool GACALL Inverse(GaType& value) const
			{
				// search all registered value sets
				for( int i = _alleles.GetSize() - 1; i >= 0; i-- )
				{
					if( _alleles[ i ]->Inverse( value ) )
						return true;
				}

				return false;
			}

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::Belongs" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual bool GACALL Belongs(const GaType& value) const
			{
				// search all registered value sets
				for( int i = _alleles.GetSize() - 1; i >= 0; i-- )
				{
					if( _alleles[ i ]->Belongs( value ) )
						return true;
				}

				return false;
			}

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::ClosestValue" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL ClosestValue(GaType& value) const
			{
				GaType closest = value;
				_alleles[ 0 ]->ClosestValue( closest );

				GaType closesDiff = AbsDiff( closest, value );

				for( int i = _alleles.GetSize() - 1; i > 0; i-- )
				{
					GaType v = value;
					_alleles[ i ]->ClosestValue( v );

					GaType diff = AbsDiff( v, value );
					if( diff < closesDiff )
					{
						closest = v;
						closesDiff = diff;
					}
				}
			}

			/// <summary>More details are given in specification of <see cref="GaAlleleSet::GetCount" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual long long GACALL GetCount() const
			{
				// sum size of all register value sets
				long long count = 0;
				for( int i = _alleles.GetSize() - 1; i >= 0; i-- )
					count += _alleles[ i ]->GetCount();

				return count;
			}

			/// <summary><c>Add</c> method registers new value set.
			///
			/// This method is not thread safe.</summary>
			/// <param name="allele">value set that should be registerd.</param>
			/// <returns>Method returns position at which the value set is inserted.</returns>
			/// <exception cref="GaNullArgumentException" />Thrown if <c>allele</c> is set to <c>NULL</c>.</exception>
			int GACALL Add(GaAlleleSet<GaType>* allele)
			{
				GA_ARG_ASSERT( Common::Exceptions::GaNullArgumentException, allele != NULL, "allele", "AlleleSet must be specified.", "Representation" );

				// resize set
				int pos = _alleles.GetSize();
				_alleles.SetSize( pos + 1 );

				// add value set
				_alleles[ pos ] = allele;

				return pos;
			}

			/// <summary><c>Remove</c> method unregister value set.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="allele">value set that should be unregistered.</param>
			void GACALL Remove(GaAlleleSet<GaType>* allele)
			{
				int i = 0, j = 0, last = _alleles.GetSize();

				// move entires to fill the gap made by removing value set
				for( ; j < last; j++ )
				{
					if( allele != _alleles[ j ] )
						_alleles[ i++ ] = _alleles[ j ];
				}

				// reizse set
				_alleles.SetSize( i );
			}

			/// <summary><c>Remove</c> method removes value set at specified position.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="position">position of the value set which should be removed.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>position</c> is less then <c>0</c> or greater then size of allele.</exception>
			void GACALL Remove(int position)
			{
				int last = _alleles.GetSize() - 1;

				GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, position >= 0 && position <= last,
					"position", "Position cannot be negative value and greater then size of allele.", "Representation" );

				// move remaining elements to fill gap made by removing value from the set
				for( int i = position; i < last; i++ )
					_alleles[ i ] = _alleles[ i + 1 ];

				// resze set
				_alleles.SetSize( last );
			}

			/// <summary>This method is not thread safe.</summary>
			/// <returns>Method returns number of value sets in this combined value set.</returns>
			inline int GACALL GetAlleleSetsCount() const { return _alleles.GetSize(); }

			/// <summary><c>operator []</c> returns reference to value set at specified position.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="pos">position of value set.</param>
			/// <returns>Operator returns reference to value set at specified position.</returns>
			inline GaAlleleSet<GaType>& GACALL operator [](int pos) { return *_alleles[ pos ]; }

			/// <summary><c>operator []</c> returns reference to value set at specified position.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="pos">position of value set.</param>
			/// <returns>Operator returns reference to value set at specified position.</returns>
			inline const GaAlleleSet<GaType>& GACALL operator [](int pos) const { return *_alleles[ pos ]; }

		};

	} // Representation
} // Chromosome

#endif // __GA_ALLELES_H__

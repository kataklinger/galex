
/*! \file FitnessValues.h
    \brief This file declares and implements classes that stores fitness values.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_FITNESS_VALUES_H__
#define __GA_FITNESS_VALUES_H__

//#include <math.h>
#include "Array.h"
#include "Fitness.h"

namespace Fitness
{

	/// <summary>Contains built-in fitness values.</summary>
	namespace Representation
	{

		/// <summary><c>GaSVFitnessBase</c> is base class for fitness objects that contain only one value.</summary>
		class GaSVFitnessBase : public GaFitness
		{

		public:

			/// <summary>This constructor initializes fitness object with fitness parameters that will be used.</summary>
			/// <param name="params">fitness parameters.</param>
			GaSVFitnessBase(Common::Memory::GaSmartPtr<const GaFitnessParams> params) : GaFitness(params) { }

			/// <summary><c>CompareValues</c> method compares values stored in fitness objects.</summary>
			/// <param name="fitness">reference to fitness object that contains second value for comparison.</param>
			/// <returns>Method returns:
			/// <br/>a. -1 if the value of first fitness object is better according to comparison citerion.
			/// <br/>b.  0 if the values are equal.
			/// <br/>c.  1 if the value of first fitness object is worse.</returns>
			virtual int GACALL CompareValues(const GaSVFitnessBase& fitness) const = 0;

		};

		/// <summary><c>GaSVFitness</c> template class represent fitness objecst which contain only one value.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <typeparam name="VALUE_TYPE">type of value that stores fitness.</typeparam>
		template<typename VALUE_TYPE>
		class GaSVFitness : public GaSVFitnessBase
		{

		public:

			/// <summary>Type of value that stores fitness.</summary>
			typedef VALUE_TYPE GaValueType;

		protected:

			/// <summary>Fitness value.</summary>
			GaValueType _value;

		public:

			/// <summary>This constructor initializes fitness object with fitness parameters that will be used and stored value in fitness object.</summary>
			/// <param name="value">value that should be stored in fitness object.</param>
			/// <param name="params">fitness parameters.</param>
			GaSVFitness(GaValueType value,
				Common::Memory::GaSmartPtr<const GaFitnessParams> params) : GaSVFitnessBase(params),
				_value(value) { }

			/// <summary>This constructor initializes fitness object with fitness parameters that will be used.</summary>
			/// <param name="params">fitness parameters.</param>
			GaSVFitness(Common::Memory::GaSmartPtr<const GaFitnessParams> params) : GaSVFitnessBase(params),
				_value() { }

			/// <summary>More details are given in specification of <see cref="GaFitness::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaFitness* GACALL Clone() const { return new GaSVFitness<GaValueType>( *this ); }

			/// <summary>More details are given in specification of <see cref="GaFitness::Clear" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Clear() { _value = GaValueType(); }

			/// <summary>More details are given in specification of <see cref="GaFitness::GetProbabilityBase" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual float GACALL GetProbabilityBase() const { return (float)_value; }

			/// <summary>More details are given in specification of <see cref="GaFitness::GetProgress" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaFitness* GACALL GetProgress(const GaFitness& previous) const 
				{ return new GaSVFitness<GaValueType>( _value - ( (const GaSVFitness<GaValueType>&)previous )._value, _parameters ); }

			/// <summary>More details are given in specification of <see cref="GaFitness::GetRelativeProgress" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual float GACALL GetRelativeProgress(const GaFitness& previous) const { return abs( ( (float)_value - ( (const GaSVFitness<GaValueType>&)previous )._value ) / _value ); }

			/// <summary>More details are given in specification of <see cref="GaSVFitnessBase::Distance" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual float GACALL Distance(const GaFitness& second) const { return (float)abs( _value - ( (const GaSVFitness<GaValueType>&)second )._value ); }

			/// <summary>More details are given in specification of <see cref="GaFitness::Distance" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual int GACALL CompareValues(const GaSVFitnessBase& fitness) const
			{
				GaValueType value = ( (const GaSVFitness<GaValueType>&)fitness )._value;
				return _value > value ? -1 : ( value > _value ? 1 : 0 );
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::operator +" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Statistics::GaValue<GaFitness> GACALL operator +(const GaFitness& rhs) const
				{ return GaSVFitness<GaValueType>( _value + ( (const GaSVFitness<GaValueType>&)rhs )._value, _parameters ); }

			/// <summary>More details are given in specification of <see cref="GaFitness::operator -" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Statistics::GaValue<GaFitness> GACALL operator -(const GaFitness& rhs) const
				{ return GaSVFitness<GaValueType>( _value - ( (const GaSVFitness<GaValueType>&)rhs )._value, _parameters ); }

			/// <summary>More details are given in specification of <see cref="GaFitness::operator /" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Statistics::GaValue<GaFitness> GACALL operator /(int rhs) const
				{ return GaSVFitness<GaValueType>( _value / rhs, _parameters ); }

			/// <summary>More details are given in specification of <see cref="GaFitness::operator +=" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaFitness& GACALL operator +=(const GaFitness& rhs) { _value += ( (const GaSVFitness<GaValueType>&)rhs )._value; return *this; }

			/// <summary>More details are given in specification of <see cref="GaFitness::operator -=" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaFitness& GACALL operator -=(const GaFitness& rhs) { _value -= ( (const GaSVFitness<GaValueType>&)rhs )._value; return *this; }

			/// <summary>More details are given in specification of <see cref="GaFitness::operator /=" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaFitness& GACALL operator /=(int rhs) { _value /= rhs; return *this; }

			/// <summary>More details are given in specification of <see cref="GaFitness::operator =" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaFitness& GACALL operator =(const GaFitness& rhs) { _value = ( (const GaSVFitness<GaValueType>&)rhs )._value; return *this; }

			/// <summary>More details are given in specification of <see cref="GaFitness::operator ==" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual bool GACALL operator ==(const GaFitness& rhs) const { return _value == ( (const GaSVFitness<GaValueType>&)rhs )._value; }

			/// <summary>More details are given in specification of <see cref="GaFitness::operator !=" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual bool GACALL operator !=(const GaFitness& rhs) const { return _value != ( (const GaSVFitness<GaValueType>&)rhs )._value; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns value stored in fitness object.</returns>
			inline const GaValueType& GACALL GetValue() const { return _value; }

			/// <summary><c>SetValue</c> method stores value in fitness object.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">value that should be stored.</param>
			inline void GACALL SetValue(const GaValueType& value) { _value = value; }

		};

		/// <summary><c>GaMVFitnessBase</c> is base class for fitness objects that contain multiple values.</summary>
		class GaMVFitnessBase : public GaFitness
		{

		public:

			/// <summary>This constructor initializes fitness object with fitness parameters that will be used.</summary>
			/// <param name="params">fitness parameters.</param>
			GaMVFitnessBase(Common::Memory::GaSmartPtr<const GaFitnessParams> params) : GaFitness(params) { }

			/// <summary><c>CompareValues</c> methods compares values at same index in two different fitness objects.</summary>
			/// <param name="fitness">fitness object that contains second value.</param>
			/// <param name="index">index of values.</param>
			/// <returns>Method returns:
			/// <br/>a. -1 if the value of first fitness object is better according to comparison citerion.
			/// <br/>b.  0 if the values are equal.
			/// <br/>c.  1 if the value of first fitness object is worse.</returns>
			virtual int GACALL CompareValues(const GaMVFitnessBase& fitness,
				int index) const = 0;

			/// <summary><c>Distance</c> method calculates distance between values at same index in two different fitness objects.</summary>
			/// <param name="fitness">fitness object that contains second value.</param>
			/// <param name="index">index of values.</param>
			virtual float GACALL Distance(const GaMVFitnessBase& fitness,
				int index) const = 0;

			/// <summary><c>GetValues</c> method queries values stored in fitness object.</summary>
			/// <returns>Method returns array of values stored in fitness object.</returns>
			virtual Common::Data::GaArrayBase& GACALL GetValues() = 0;

			/// <summary><c>GetValues</c> method queries values stored in fitness object.</summary>
			/// <returns>Method returns array of values stored in fitness object.</returns>
			virtual const Common::Data::GaArrayBase& GACALL GetValues() const = 0;

		};

		/// <summary><c>GaMVFitnessParams</c> class represents parameters for fitness objects that contain multiple values.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaMVFitnessParams : public GaFitnessParams
		{

		protected:

			/// <summary>Number of values stored in fitness object.</summary>
			int _valueCount;

			/// <summary>Index of value in fitness object that is used for calculating probability base.</summary>
			int _probabilityBaseIndex;

		public:

			/// <summary>This constructor initializes parameters with number of values that fitness object should store and index of value used as probability base.</summary>
			/// <param name="valueCount">number of values stored in fitness object.</param>
			/// <param name="probabilityBaseIndex">index of value in fitness object that is used for calculating probability base.</param>
			GaMVFitnessParams(int valueCount,
				int probabilityBaseIndex = 0)
			{
				SetValueCount( valueCount );
				SetProbabilityBaseIndex( probabilityBaseIndex );
			}

			/// <summary>This constructor initializes parameters with default values.
			/// Number of values is 1 and the value is also used as probability base.</summary>
			GaMVFitnessParams() : _valueCount(1),
				_probabilityBaseIndex(0) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaMVFitnessParams( _valueCount ); }

			/// <summary><c>NextProbabilityIndex</c> method moves the index of value that will be used for calculating fitness value.
			/// If the last value is reached, index will be moved to the first value.
			///
			/// This method is not thread-safe.</summary>
			inline void NextProbabilityIndex() { _probabilityBaseIndex = ( _probabilityBaseIndex + 1 ) % _valueCount; }

			/// <summary><c>SetValueCount</c> method sets number of values that will be stored by fitness objects.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="valueCount"></param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>valueCount</c> is negative value or <c>0</c>.</exception>
			inline void GACALL SetValueCount(int valueCount)
			{
				GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, valueCount > 0, "valueCount",
					"Number of values in fitness must be greater then or equal to 1.", "Fitness" );

				_valueCount = valueCount;
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of values stored in fitness object.</returns>
			inline int GACALL GetValueCount() const { return _valueCount; }

			/// <summary><c>SetProbabilityBaseIndex</c> method sets index of the value that is going to be used for calculating probability base.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="index">index of used value.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>index</c> is negative value greater than number of fitness values.</exception>
			inline void GACALL SetProbabilityBaseIndex(int index)
			{
				GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, index >= 0 && index < _valueCount,
					"index", "Index of the objective that will be used as probability base cannot be negative or larger than fitness size.", "Fitness" );

				_probabilityBaseIndex = index;
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns index of value in fitness object that is used for calculating probability base.</returns>
			inline int GACALL GetProbabilityBaseIndex() const { return _probabilityBaseIndex; }

		};

		/// <summary><c>GaMVFitness</c> template class represent fitness objects which contain multiple values.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <typeparam name="VALUE_TYPE">type of values that store fitness.</typeparam>
		template<typename VALUE_TYPE>
		class GaMVFitness : public GaMVFitnessBase
		{

		public:

			/// <summary>Type of values that store fitness.</summary>
			typedef VALUE_TYPE GaValueType;

		protected:

			/// <summary>Array that stores fitness values.</summary>
			Common::Data::GaSingleDimensionArray<GaValueType> _values;

		public:

			/// <summary>This constructor initializes fitness object with fitness parameters that will be used.</summary>
			/// <param name="params">fitness parameters.</param>
			GaMVFitness(Common::Memory::GaSmartPtr<const GaFitnessParams> params) : GaMVFitnessBase(params)
				{ _values.SetSize( ( (const GaMVFitnessParams&)( *params ) ).GetValueCount() ); }

			/// <summary>More details are given in specification of <see cref="GaFitness::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaFitness* GACALL Clone() const { return new GaMVFitness<GaValueType>( *this ); }

			/// <summary>More details are given in specification of <see cref="GaFitness::Clear" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Clear()
			{
				for( int i = _values.GetSize() - 1; i >= 0; i-- )
					_values[ i ] = GaValueType();
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::GetProbabilityBase" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual float GACALL GetProbabilityBase() const { return (float)_values[ ( (const GaMVFitnessParams&)*_parameters ).GetProbabilityBaseIndex() ]; }

			/// <summary>More details are given in specification of <see cref="GaFitness::GetProgress" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaFitness* GACALL GetProgress(const GaFitness& previous) const
			{
				GaMVFitness<GaValueType>* progress = new GaMVFitness<GaValueType>( *this );
				*progress -= previous;

				return progress;
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::GetRelativeProgress" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual float GACALL GetRelativeProgress(const GaFitness& previous) const
			{
				const Common::Data::GaSingleDimensionArray<GaValueType>& previousValues = ( (const GaMVFitness<GaValueType>&)previous )._values;

				// sum relative progress of all values
				float progress = 0;
				for( int i = _values.GetSize() - 1; i >= 0; i-- )
					progress += abs( ( (float)_values[ i ] - previousValues[ i ] ) / _values[ i ] );

				return progress;
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::Distance" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual float GACALL Distance(const GaFitness& second) const
			{
				const Common::Data::GaSingleDimensionArray<GaValueType>& values = ( (const GaMVFitness<GaValueType>&)second )._values;

				// calculate distance between points in multidimensional space
				float distance = 0;
				for( int i = _values.GetSize() - 1; i >= 0; i-- )
					distance += (float)pow( _values[ i ] - values[ i ], 2 );

				return sqrt( distance );
			}

			/// <summary>More details are given in specification of <see cref="GaMVFitnessBase::CompareValues" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual int GACALL CompareValues(const GaMVFitnessBase& fitness,
				int index) const
			{
				const GaValueType& value1 = _values[ index ];
				const GaValueType& value2 = ( (const GaMVFitness<GaValueType>&)fitness )._values[ index ];
				return value1 > value2 ? -1 : ( value2 > value1 ? 1 : 0 );
			}

			/// <summary>More details are given in specification of <see cref="GaMVFitnessBase::Distance" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual float GACALL Distance(const GaMVFitnessBase& fitness,
				int index) const { return (float)( _values[ index ] - ( (const GaMVFitness<GaValueType>&)fitness )._values[ index ] ); }

			/// <summary></summary>
			virtual Common::Data::GaArrayBase& GACALL GetValues() { return _values; }

			/// <summary></summary>
			virtual const Common::Data::GaArrayBase& GACALL GetValues() const { return _values; }

			/// <summary>More details are given in specification of <see cref="GaFitness::operator +" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Statistics::GaValue<GaFitness> GACALL operator +(const GaFitness& rhs) const
			{
				GaMVFitness<GaValueType> temp( *this );
				temp += rhs;

				return temp;
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::operator -" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Statistics::GaValue<GaFitness> GACALL operator -(const GaFitness& rhs) const
			{
				GaMVFitness<GaValueType> temp( *this );
				temp -= rhs;

				return temp;
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::operator /" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Statistics::GaValue<GaFitness> GACALL operator /(int rhs) const
			{
				GaMVFitness<GaValueType> temp( *this );
				temp /= rhs;

				return temp;
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::operator +=" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaFitness& GACALL operator +=(const GaFitness& rhs)
			{
				const Common::Data::GaSingleDimensionArray<GaValueType>& rhsValues = ( (const GaMVFitness<GaValueType>&)rhs )._values;

				// perform addition for all values
				for( int i = _values.GetSize() - 1; i >= 0; i-- )
					_values[ i ] += rhsValues[ i ];

				return *this;
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::operator -=" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaFitness& GACALL operator -=(const GaFitness& rhs)
			{
				const Common::Data::GaSingleDimensionArray<GaValueType>& rhsValues = ( (const GaMVFitness<GaValueType>&)rhs )._values;

				// perform substration for all values
				for( int i = _values.GetSize() - 1; i >= 0; i-- )
					_values[ i ] -= rhsValues[ i ];

				return *this;
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::operator /=" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaFitness& GACALL operator /=(int rhs)
			{
				// perform division for all values
				for( int i = _values.GetSize() - 1; i >= 0; i-- )
					_values[ i ] /= rhs;

				return *this;
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::operator =" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaFitness& GACALL operator =(const GaFitness& rhs) { _values = ( (const GaMVFitness<GaValueType>&)rhs )._values; return *this; }

			/// <summary>More details are given in specification of <see cref="GaFitness::operator ==" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual bool GACALL operator ==(const GaFitness& rhs) const { return _values == ( (const GaMVFitness<GaValueType>&)rhs )._values; }

			/// <summary>More details are given in specification of <see cref="GaFitness::operator !=" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual bool GACALL operator !=(const GaFitness& rhs) const { return _values == ( (const GaMVFitness<GaValueType>&)rhs )._values; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns typed array of stored values in fitness object.</returns>
			inline Common::Data::GaSingleDimensionArray<GaValueType>& GACALL GetTypedValues() { return _values; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns typed array of stored values in fitness object.</returns>
			inline const Common::Data::GaSingleDimensionArray<GaValueType>& GACALL GetTypedValues() const { return _values; }

			/// <summary><c>SetValue</c> method stores new value at specifed position in fitness object.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">new value.</param>
			/// <param name="index">index at which the new value should be stored.</param>
			inline void GACALL SetValue(const GaValueType& value,
				int index) { _values[ index ] = value; }

			/// <summary><c>GetValue</c> method value stored at specified index in fitness object.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="index">index of queried value.</param>
			/// <returns>Method returns value stored at specified index.</returns>
			inline const GaValueType& GACALL GetValue(int index) const { return _values[ index ]; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of stored values.</returns>
			inline int GACALL GetSize() const { return _values.GetSize(); }

			/// <summary><c>operator []</c> queries value stored at specified index in fitness object.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="index">index of queried value.</param>
			/// <returns>Operator returns value stored at specified index.</returns>
			inline GaValueType& GACALL operator [](int index) { return _values[ index ]; }

			/// <summary><c>operator []</c> queries value stored at specified index in fitness object.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="index">index of queried value.</param>
			/// <returns>Operator returns value stored at specified index.</returns>
			inline const GaValueType& GACALL operator [](int index) const { return _values[ index ]; }

		};

		/// <summary><c>GaWeightedFitnessParams</c> class stores parameters for weighted fitness values.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <typeparam name="WEIGHT_TYPE">type of value weights.</typeparam>
		template<typename WEIGHT_TYPE>
		class GaWeightedFitnessParams : public GaMVFitnessParams
		{

		public:

			/// <summary>Type of value weights.</summary>
			typedef WEIGHT_TYPE GaWeightType;

		protected:

			/// <summary>Weights assigned to fitness values.</summary>
			Common::Data::GaSingleDimensionArray<GaWeightType> _weights;

		public:

			/// <summary>This constructor initializes parameters with number of stored values and their weights.</summary>
			/// <param name="weights">array that stores weights.</param>
			/// <param name="valueCount">number of stored values.</param>
			GaWeightedFitnessParams(const GaWeightType* weights,
				int valueCount) : GaMVFitnessParams(valueCount),
				_weights(weights, valueCount) { }

			/// <summary>More details are given in specification of <see cref="GaFitness::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaWeightedFitnessParams( _valueCount ); }

			/// <summary><c>SetWeight</c> method sets weight of fitness value at specified index.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="index">index of the value.</param>
			/// <param name="weight">new weight  of the value.</param>
			inline void GACALL SetWeight(int index,
				GaWeightType weight) { return *_weights[ index ] = weight; }

			/// <summary><c>GetWeight</c> method returns weight of fitness value at specified index.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="index">index of queried value.</param>
			/// <returns>Method returns weight of fitness value.</returns>
			inline const GaWeightType& GACALL GetWeight(int index) const { return _weights[ index ]; }

		};

		/// <summary><c>GaWeightedFitness</c> template class represent fitness object which contain multiple values with assigned weights
		/// and final fitness value is sum of those values multiplied by their weights
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <typeparam name="VALUE_TYPE">type of values that store fitness.</typeparam>
		/// <typeparam name="WEIGHT_TYPE">type of value weights.</typeparam>
		template<typename VALUE_TYPE,
			typename WEIGHT_TYPE>
		class GaWeightedFitness : public GaSVFitnessBase, private GaMVFitness<VALUE_TYPE>
		{

		public:

			/// <summary>Type of values that store fitness.</summary>
			typedef VALUE_TYPE GaValueType;

			/// <summary>Type of value weights.</summary>
			typedef WEIGHT_TYPE GaWeightType;

		protected:

			/// <summary>Sum of stored values multiplied by their weights.</summary>
			GaWeightType _weightedSum;

		public:

			/// <summary>This constructor initializes fitness object with fitness parameters that will be used.</summary>
			/// <param name="params">fitness parameters.</param>
			GaWeightedFitness(Common::Memory::GaSmartPtr<const GaFitnessParams> params) : GaMVFitness(params),
				_weightedSum() { }

			/// <summary>More details are given in specification of <see cref="GaFitness::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaFitness* GACALL Clone() const { return new GaWeightedFitness<GaValueType>( *this ); }

			/// <summary>More details are given in specification of <see cref="GaFitness::Clear" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Clear()
			{
				GaMVFitness<GaValueType>::Clear();
				_weightedSum = GaWeightType();
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::GetProgress" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaFitness* GACALL GetProgress(const GaFitness& previous) const
			{
				GaWeightedFitness<GaValueType>* progress = new GaWeightedFitness<GaValueType>( *this );
				progress -= previous;

				return progress;
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::GetRelativeProgress" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual float GACALL GetRelativeProgress(const GaFitness& previous) const
				{ return abs( ( (float)_weightedSum - ( (const GaWeightedFitness<GaValueType>&)previous )._weightedSum ) / _weightedSum ); }

			/// <summary>More details are given in specification of <see cref="GaSVFitnessBase::CompareValues" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual int GACALL CompareValues(const GaSVFitnessBase& fitness) const
			{
				const GaWeightType& weightedSum = ( (const GaWeightedFitness<GaValueType>&)fitness )._sum;
				return _weightedSum > weightedSum ? -1 : ( weightedSum > _weightedSum ? 1 : 0 );
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::operator +" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Statistics::GaValue<GaFitness> GACALL operator +(const GaFitness& rhs) const
			{
				GaWeightedFitness<GaValueType> temp( *this );
				temp += rhs;

				return temp;
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::operator -" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Statistics::GaValue<GaFitness> GACALL operator -(const GaFitness& rhs) const
			{
				GaWeightedFitness<GaValueType> temp( *this );
				temp -= rhs;

				return temp;
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::operator /" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Statistics::GaValue<GaFitness> GACALL operator /(int rhs) const
			{
				GaWeightedFitness<GaValueType> temp( *this );
				temp /= rhs;

				return temp;
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::operator +=" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaFitness& GACALL operator +=(const GaFitness& rhs)
			{
				GaMVFitness<GaValueType>::operator +=( rhs );
				CalculateWeightedSum();

				return *this;
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::operator -=" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaFitness& GACALL operator -=(const GaFitness& rhs)
			{
				GaMVFitness<GaValueType>::operator -=( rhs );
				CalculateWeightedSum();

				return *this;
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::operator /=" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaFitness& GACALL operator /=(int rhs)
			{
				GaMVFitness<GaValueType>::operator /=( rhs );
				CalculateWeightedSum();

				return *this;
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::operator =" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaFitness& GACALL operator =(const GaFitness& rhs)
			{
				GaMVFitness<GaValueType>::operator =( rhs );
				_weightedSum = ( (const GaWeightedFitness<GaValueType>&)rhs )._weightedSum;

				return *this;
			}

			/// <summary>More details are given in specification of <see cref="GaFitness::operator ==" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual bool GACALL operator ==(const GaFitness& rhs) const { return _weightedSum == ( (const GaWeightedFitness<GaValueType>&)rhs )._weightedSum; }

			/// <summary>More details are given in specification of <see cref="GaFitness::operator !=" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual bool GACALL operator !=(const GaFitness& rhs) const { return _weightedSum != ( (const GaWeightedFitness<GaValueType>&)rhs )._weightedSum; }


			/// <summary><c>SetValue</c> method stores new value at specifed position in fitness object.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">new value.</param>
			/// <param name="index">index at which the new value should be stored.</param>
			inline void GACALL SetValue(const GaValueType& value,
				int index)
			{
				GaWeightType old = GetWeightedValue( index );

				_values[ index ] = value;
				_weightedSum = _weightedSum - old + GetWeightedValue( index );
			}

			/// <summary><c>GetValue</c> method value stored at specified index in fitness object.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="index">index of queried value.</param>
			/// <returns>Method returns value stored at specified index.</returns>
			inline const GaValueType& GACALL GetValue(int index) const { return _values[ index ]; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns typed array of stored values in fitness object.</returns>
			inline Common::Data::GaSingleDimensionArray<GaValueType>& GACALL GetTypedValues() { return _values; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns typed array of stored values in fitness object.</returns>
			inline const Common::Data::GaSingleDimensionArray<GaValueType>& GACALL GetTypedValues() const { return _values; }

			/// <summary><c>GetWeightedValue</c> method calculates weighted value at specified index.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="index">index of queried value.</param>
			/// <returns>Method returns weighted value.</returns>
			inline GaValueType GACALL GetWeightedValue(int index) const { return _values[ index ] * ( (GaWeightedFitnessParams<GaWeightType>&)*_parameters ).GetWeight( index ); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns weighted sum of stored fitness values.</returns>
			inline const GaWeightType& GACALL GetWeightedSum() const { return _weightedSum; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of stored values.</returns>
			inline int GACALL GetSize() const { return _values.GetSize(); }

			/// <summary><c>operator []</c> queries value stored at specified index in fitness object.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="index">index of queried value.</param>
			/// <returns>Operator returns value stored at specified index.</returns>
			inline GaValueType& GACALL operator [](int index) { return _values[ index ]; }

			/// <summary><c>operator []</c> queries value stored at specified index in fitness object.
			///
			/// This operator is not thread-safe.</summary>
			/// <param name="index">index of queried value.</param>
			/// <returns>Operator returns value stored at specified index.</returns>
			inline const GaValueType& GACALL operator [](int index) const { return _values[ index ]; }

		protected:

			/// <summary>This method calculates and stores weighted sum of stored values.</summary>
			inline void GACALL CalculateWeightedSum()
			{
				_weightedSum = GaWeightType();

				for( int i = _values.GetSize() - 1; i >= 0; i-- )
					_weightedSum += GetWeightedValue( i );
			}

		};

	} // Representation
} // Fitness

#endif //__GA_FITNESS_VALUES_H__

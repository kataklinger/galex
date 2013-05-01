
/*! \file Statistics.h
    \brief This file contains declaration of classes and datatypes needed for tracking statistical information about populations and genetic algorithms.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_STATISTICS_H__
#define __GA_STATISTICS_H__

#include <cmath>
#include <list>
#include <set>
#include "HashMap.h"
#include "Exceptions.h"
#include "SmartPtr.h"
#include "Timing.h"

/// <summary>Contains set of classes and datatypes that handles statistical information.</summary>
namespace Statistics
{

	/// <summary><c>GaValue</c> template class wraps values of specified type so thay can be used to store and calculate statistical information.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	/// <typeparam name="TYPE">type of stored value.</typeparam>
	template<typename TYPE>
	class GaValue
	{

	public:

		/// <summary>Type of stored value.</summary>
		typedef TYPE GaType;

		/// <summary>Constant type of stored value.</summary>
		typedef const TYPE GaConstType;

	private:

		/// <summary>Value that is stored in this object.</summary>
		GaType _value;

		/// <summary>Inidicates whether the object contains value.</summary>
		bool _empty;

	public:

		/// <summary>This constructor initializes empty object.</summary>
		GaValue() : _value(0),
			_empty(true) { }

		/// <summary>This constructor stores specified value in the object.</summary>
		/// <param name="value">value that should be stored in this object.</param>
		GaValue(const GaType& value) : _value(value),
			_empty(false) { }

		/// <summary>This is copy constructor.</summary>
		/// <param name="rhs">source object whose value is copied.</param>
		GaValue(const GaValue<GaType>& rhs) : _value(rhs._value),
			_empty(rhs._empty) { }

		/// <summary><c>Clear</c> method clears previously stored value.
		///
		/// This method is not thread-safe.</summary>
		inline void GACALL Clear()
		{
			_value = GaType();
			_empty = true;
		}

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to stored value.</returns>
		inline GaType& GACALL GetValue() { return _value; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to stored value.</returns>
		inline GaConstType& GACALL GetValue() const { return _value; }

		/// <summary><c>GetRelativeProgress</c> calculates relative progress of the stored value based on value which is specified by the parameters of this method.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="previous">reference to value on which the progress calculation is based.</param>
		/// <returns>Method returns calculated relative progress.</returns>
		inline float GACALL GetRelativeProgress(GaConstType& previous) const { return abs( ( (float)_value - previous ) / previous ); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if this object is empty and it contains no value.</returns>
		inline bool GACALL IsEmpty() const { return _empty; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if this object is not empty.</returns>
		inline bool GACALL HasValue() const { return !_empty; }

		/// <summary>This operator is not thread-safe.</summary>
		/// <returns>Opertor returns reference to stored value.</returns>
		inline GACALL operator GaType&() { return _value; }

		/// <summary>This operator is not thread-safe.</summary>
		/// <returns>Opertor returns reference to stored value.</returns>
		inline GACALL operator const GaType&() const { return _value; }

		/// <summary><c>operator =</c> operator copies value from another object.
		///
		/// This operator is not thread-safe.</summary>
		/// <param name="rhs">source object whose value is copied.</param>
		/// <returns>Method returns reference to this object.</returns>
		inline GaValue<GaType>& GACALL operator =(const GaValue<GaType>& rhs)
		{
			_value = rhs._value;
			_empty = rhs._empty;
			return *this;
		}

		/// <summary><c>operator =</c> operator stores specified value in this object.
		///
		/// This operator is not thread-safe.</summary>
		/// <param name="rhs">value that should be stored in this object.</param>
		/// <returns>Method returns reference to this object.</returns>
		inline GaValue<GaType>& GACALL operator =(GaConstType& rhs)
		{
			_value = rhs;
			_empty = false;
			return *this;
		}

	};

	/// <summary>Addition operatotor for stored statistical values.</summary>
	/// <typeparam name="T">type of value.</typeparam>
	/// <param name="lhs">reference to object that stores the first operand.</param>
	/// <param name="rhs">reference to the second operand.</param>
	/// <returns>Operator returns new object that stores results of addition.</returns>
	template<typename T>
	inline GaValue<T> GACALL operator +(const GaValue<T>& lhs,
		typename GaValue<T>::GaConstType& rhs) { return lhs.GetValue() + rhs; }

	/// <summary>Addition operatotor for stored statistical values.</summary>
	/// <typeparam name="T">type of value.</typeparam>
	/// <param name="lhs">reference to object that is destination of the operation.</param>
	/// <param name="rhs">reference to the second operand.</param>
	/// <returns>Operator returns reference to object that stores destination operand.</returns>
	template<typename T>
	inline GaValue<T>& GACALL operator +=(GaValue<T>& lhs,
		typename GaValue<T>::GaConstType& rhs) { lhs.GetValue() += rhs; return lhs; }

	/// <summary>Subtraction operatotor for stored statistical values.</summary>
	/// <typeparam name="T">type of value.</typeparam>
	/// <param name="lhs">reference to object that stores the first operand.</param>
	/// <param name="rhs">reference to the second operand.</param>
	/// <returns>Operator returns new object that stores results of subtraction.</returns>
	template<typename T>
	inline GaValue<T> GACALL operator -(const GaValue<T>& lhs,
		typename GaValue<T>::GaConstType& rhs) { return lhs.GetValue() - rhs; }

	/// <summary>Subtraction operatotor for stored statistical values.</summary>
	/// <typeparam name="T">type of value.</typeparam>
	/// <param name="lhs">reference to object that is destination of the operation.</param>
	/// <param name="rhs">reference to the second operand.</param>
	/// <returns>Operator returns reference to object that stores destination operand.</returns>
	template<typename T>
	inline GaValue<T>& GACALL operator -=(GaValue<T>& lhs,
		typename GaValue<T>::GaConstType& rhs) { lhs.GetValue() -= rhs; return lhs; }

	/// <summary>Multiplication operatotor for stored statistical values.</summary>
	/// <typeparam name="T">type of value.</typeparam>
	/// <param name="lhs">reference to object that stores the first operand.</param>
	/// <param name="rhs">reference to the second operand.</param>
	/// <returns>Operator returns new object that stores results of multiplication.</returns>
	template<typename T>
	inline GaValue<T> GACALL operator *(const GaValue<T>& lhs,
		typename GaValue<T>::GaConstType& rhs) { return lhs.GetValue() * rhs; }

	/// <summary>Multiplication operatotor for stored statistical values.</summary>
	/// <typeparam name="T">type of value.</typeparam>
	/// <param name="lhs">reference to object that is destination of the operation.</param>
	/// <param name="rhs">reference to the second operand.</param>
	/// <returns>Operator returns reference to object that stores destination operand.</returns>
	template<typename T>
	inline GaValue<T>& GACALL operator *=(GaValue<T>& lhs,
		typename GaValue<T>::GaConstType& rhs) { lhs.GetValue() *= rhs; return lhs; }

	/// <summary>Division operatotor for stored statistical values.</summary>
	/// <typeparam name="T">type of value.</typeparam>
	/// <param name="lhs">reference to object that stores the first operand.</param>
	/// <param name="rhs">reference to the second operand.</param>
	/// <returns>Operator returns new object that stores results of division.</returns>
	template<typename T>
	inline GaValue<T> GACALL operator /(const GaValue<T>& lhs,
		typename GaValue<T>::GaConstType& rhs) { return lhs.GetValue() / rhs; }

	/// <summary>Division operatotor for stored statistical values.</summary>
	/// <typeparam name="T">type of value.</typeparam>
	/// <param name="lhs">reference to object that is destination of the operation.</param>
	/// <param name="rhs">reference to the second operand.</param>
	/// <returns>Operator returns reference to object that stores destination operand.</returns>
	template<typename T>
	inline GaValue<T>& GACALL operator /=(GaValue<T>& lhs,
		typename GaValue<T>::GaConstType& rhs) { lhs.GetValue() /= rhs; return lhs; }

	/// <summary>This operator divides statistical value with an integer number.</summary>
	/// <typeparam name="T">type of value.</typeparam>
	/// <param name="lhs">reference to object that is destination of the operation.</param>
	/// <param name="rhs">reference to the second operand.</param>
	/// <returns>Operator returns reference to object that stores destination operand.</returns>
	template<typename T>
	inline GaValue<T> GACALL operator /(const GaValue<T>& lhs,
		int rhs) { return lhs.GetValue() / rhs; }

	/// <summary><c>operator ==</c> compares to values equality.</summary>
	/// <typeparam name="T">type of value.</typeparam>
	/// <param name="lhs">reference to object that stores the first operand.</param>
	/// <param name="rhs">reference to the second operand.</param>
	/// <returns>This method returns <c>true</c> if the values are equal.</returns>
	template<typename T>
	inline bool GACALL operator ==(const GaValue<T>& lhs,
		typename GaValue<T>::GaConstType& rhs) { return lhs.GetValue() == rhs; }

	/// <summary><c>operator !=</c> compares to values inequality.</summary>
	/// <typeparam name="T">type of value.</typeparam>
	/// <param name="lhs">reference to object that stores the first operand.</param>
	/// <param name="rhs">reference to the second operand.</param>
	/// <returns>This method returns <c>true</c> if the values are not equal.</returns>
	template<typename T>
	inline bool GACALL operator !=(const GaValue<T>& lhs,
		typename GaValue<T>::GaConstType& rhs) { return lhs.GetValue() != rhs; }

	class GaValueHistoryBase;

	/// <summary><c>GaValueCombiner</c> is interface for combiners used for combining statistical values.</summary>
	class GaValueCombiner
	{

	public:

		/// <summary><c>Combine</c> method combines two statistical values and stores result into the first value.</summary>
		/// <param name="value1">the first value for the combine operation. It is also the destionation of the operation.</param>
		/// <param name="value2">the second value for the combine operation.</param>
		virtual void GACALL Combine(GaValueHistoryBase& value1,
			const GaValueHistoryBase& value2) const = 0;

	};

	/// <summary><c>GaValueEvaluator</c> is interface for evaluator used for evaluation of statistical values.
	/// When a value is bound it is automatically updated each time the underlying values are changed.</summary>
	class GaValueEvaluator
	{

	public:

		/// <summary><c>Bind</c> method binds specified value that will store results with values used in evaluation process.</summary>
		/// <param name="value">value that should be bound.</param>
		virtual void GACALL Bind(GaValueHistoryBase* value) const = 0;

		/// <summary><c>Evaluate</c> method calculates value and stores result to the specified value.</summary>
		/// <param name="value">statistical value to which the result should be stored.</param>
		virtual void GACALL Evaluate(GaValueHistoryBase& value) const = 0;

	};

	class GaStatistics;

	/// <summary><c>GaValueHistoryBase</c> is base class that provide interface for tracking and storing statistical values. It also handles combining and evaluation
	/// of the values. Values can be combined using combiners (<see cref="GaValueCombiner">) or evaluated using evaluators (<see cref="GaValueCombiner">).
	/// Evaluated values are are automatically calculated using other statistical values (defined by evaluator) and they are automatically update each time
	/// some of the underlying values are changed. Evaluated values cannot be destination of combiner.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaValueHistoryBase
	{

	protected:

		/// <summary>Pointer to statistics object that owns this value.</summary>
		GaStatistics* _owner;

		/// <summary>If this attribute is set to <c>true</c>, history depth of this value does not depend on history depth specified by statistics object.</summary>
		bool _freeHistoryDepth;

		/// <summary>Set of values that are evaluated based on this value.</summary>
		std::set<GaValueHistoryBase*> _dependants;

		/// <summary>Set of values on which are evaluation of this value is based.</summary>
		std::set<GaValueHistoryBase*> _dependencies;

		/// <summary>Pointer to object that performs combining of this value with another statistical value.</summary>
		GaValueCombiner* _combiner;

		/// <summary>Pointer to object that performes evaluation of this value based on other statistical values.</summary>
		GaValueEvaluator* _evaluator;

		/// <summary>Tracks the number of generations that have passed since the last change of the value.</summary>
		int _lastChange;

	public:

		/// <summary>This constructor creates independant statistical value.</summary>
		/// <param name="owner">pointer to statistics object that owns value.</param>
		/// <param name="freeHistoryDepth">if this attribute is set to <c>true</c> history depth of this value does not depend
		/// on history depth specified by parent statistics object.</param>
		/// <param name="combiner">pointer to object that performs combining of this value with another statistical value.</param>
		GaValueHistoryBase(GaStatistics* owner,
			bool freeHistoryDepth,
			GaValueCombiner* combiner) : _owner(owner),
			_freeHistoryDepth(freeHistoryDepth),
			_combiner(combiner),
			_evaluator(NULL),
			_lastChange(0) { }

		/// <summary>This constructor creates statistical value that is evaluated based on other values.</summary>
		/// <param name="owner">pointer to statistics object that owns value.</param>
		/// <param name="freeHistoryDepth">if this attribute is set to <c>true</c> history depth of this value does not depend
		/// on history depth specified by parent statistics object.</param>
		/// <param name="evaluator">pointer to object that performes evaluation of this value.</param>
		GaValueHistoryBase(GaStatistics* owner,
			bool freeHistoryDepth,
			GaValueEvaluator* evaluator) : _owner(owner),
			_freeHistoryDepth(freeHistoryDepth),
			_combiner(NULL),
			_lastChange(0) { SetEvaluator( evaluator ); }

		/// <summary>Virtual destructor must be defined because this is base class.</summary>
		virtual ~GaValueHistoryBase() { }

		/// <summary><c>Combine</c> method combines this value with another statistical value and stores results in this value.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="value">statistical value which should be combined with this value.</param>
		/// <exception cref="GaInvalidOperationException" />Thrown if the combiner object is not defined.</exception>
		inline void GACALL Combine(const GaValueHistoryBase& value)
		{
			GA_ASSERT( Common::Exceptions::GaInvalidOperationException, _combiner != NULL,  "Value combiner is not defined.", "Statistics" );
			_combiner->Combine( *this, value );
		}

		/// <summary><c>Evaluate</c> method performs evaluation of the value using provided evaluator object.
		///
		/// This method is not thread-safe.</summary>
		/// <exception cref="GaInvalidOperationException" />Thrown if the evaluator object is not defined.</exception>
		inline void GACALL Evaluate()
		{
			GA_ASSERT( Common::Exceptions::GaInvalidOperationException, _evaluator != NULL,  "Value evaluator is not defined.", "Statistics" );
			_evaluator->Evaluate( *this );
		}

		/// <summary><c>Next</c> method moves current value to history buffer. If the buffer is full, the oldes value stored in the buffer is discharged.</summary>
		virtual void GACALL Next() = 0;

		/// <summary><c>Clear</c> method clears history buffer and the current value.</summary>
		virtual void GACALL GACALL Clear() = 0;

		/// <summary><c>ClearCurrent</c> method clears only the current value but keeps history buffer.</summary>
		virtual void GACALL GACALL ClearCurrent() = 0;

		/// <summary><c>SetHistoryDepth</c> method sets depth of history buffer.</summary>
		/// <param name="depth">new history depth.</param>
		virtual void GACALL GACALL SetHistoryDepth(int depth) = 0;

		/// <summary><c>GetHistoryDepth</c> method returns maximal number of values that are kept in history buffer.</summary>
		/// <returns>Method returns maximal depth of history buffer.</returns>
		virtual int GACALL GACALL GetHistoryDepth() = 0;

		/// <summary><c>GetHistoryDepth</c> method returns current number of entries in history buffer.</summary>
		/// <returns>Method returns current depth.</returns>
		virtual int GACALL GetCurrentHistoryDepth() const = 0;

		/// <summary><c>IsHistoryFull</c> method returns state of history buffer.</summary>
		/// <returns>Method returns <c>true</c> if the history buffer is full.</returns>
		virtual bool GACALL IsHistoryFull() const = 0;

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns number of generations that have passed since the last change of the value.</returns>
		inline int GACALL GetLastChange() const { return _lastChange; }

		/// <summary><c>GetRelativeProgress</c> method calculates relative progress of current value compared to specified value store in the history buffer.</summary>
		/// <param name="depth">position of value in the history buffer on which the progress calculation is based. If this parameter is <c>-1</c>
		/// method calcualtes progress based on oldes value from the history buffer.</param>
		/// <returns>Method returns calculated relative progress of the value.</returns>
		virtual float GACALL GetRelativeProgress(int depth = 1) const = 0;

		/// <summary><c>SetFreeHistoryDepth</c> sets dependency of this value's history depth on history depth specified by statistics object.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="free">if this parameter is set to <c>true</c>, history depth of this value does not depend on history depth specified by statistics object.</param>
		GAL_API
		void GACALL SetFreeHistoryDepth(bool free);

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if the history depth of this value does not depend on history depth specified by the statistics object.</returns>
		inline bool GACALL IsFreeHistoryDepth() const { return _freeHistoryDepth; }

		/// <summary><c>AddDependant</c> method inserts specified value from list of dependant values for this value.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="value">pointer to value which should be inserted to list of dependant values.</param>
		/// <exception cref="GaNullArgumentException" />Thrown if the <c>value</c> is set to <c>NULL</c>.</exception>
		/// <exception cref="GaArgumentException" />Thrown if the specified value is already in the list.</exception>
		GAL_API
		void GACALL AddDependant(GaValueHistoryBase* value);

		/// <summary><c>RemoveDependant</c> method removes specified value from list of dependant values for this value.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="value">pointer to value which should be removed of dependant values.</param>
		/// <exception cref="GaNullArgumentException" />Thrown if the <c>value</c> is set to <c>NULL</c>.</exception>
		GAL_API
		void GACALL RemoveDependant(GaValueHistoryBase* value);

		/// <summary><c>IsDependant</c> method check dependants values of this statistical value.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="value">pointer to statistical value which is checked against list of dependants values.</param>
		/// <returns>Method returns <c>true</c> if the specified value is dependant on this value.</returns>
		inline bool GACALL IsDependant(GaValueHistoryBase* value) const { return _dependants.find( value ) != _dependants.end(); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to set of values that are evaluated based on this value.</returns>
		inline std::set<GaValueHistoryBase*>& GACALL GetDependants() { return _dependants; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to set of values that are evaluated based on this value.</returns>
		inline const std::set<GaValueHistoryBase*>& GACALL GetDependants() const { return _dependants; }

		/// <summary><c>AddDependency</c> method inserts specified value from list of dependencies for this value.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="value">pointer to value which should be inserted to dependencies.</param>
		/// <exception cref="GaNullArgumentException" />Thrown if the <c>value</c> is set to <c>NULL</c>.</exception>
		/// <exception cref="GaArgumentException" />Thrown if the specified value is already in the list.</exception>
		GAL_API
		void GACALL AddDependency(GaValueHistoryBase* value);

		/// <summary><c>RemoveDependency</c> method removes specified value from list of dependencies for this value.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="value">pointer to value which should be removed from dependencies.</param>
		/// <exception cref="GaNullArgumentException" />Thrown if the <c>value</c> is set to <c>NULL</c>.</exception>
		GAL_API
		void GACALL RemoveDependency(GaValueHistoryBase* value);

		/// <summary><c>RemoveAllDependencies</c> method removes all dependencies for this value.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		void GACALL RemoveAllDependencies();

		/// <summary><c>IsDependency</c> method check dependencies of this statistical value.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="value">pointer to statistical value which is checked against dependencies of this value.</param>
		/// <returns>Method returns <c>true</c> if the specified value is dependency for this value.</returns>
		inline bool GACALL IsDependency(GaValueHistoryBase* value) const { return _dependencies.find( value ) != _dependencies.end(); }

		/// <summary>This method is thread-safe.</summary>
		/// <returns>Method returns pointer to set of values on which are evaluation of this value is based.</returns>
		inline std::set<GaValueHistoryBase*>& GACALL GetDependencies() { return _dependencies; }

		/// <summary>This method is thread-safe.</summary>
		/// <returns>Method returns pointer to set of values on which are evaluation of this value is based.</returns>
		inline const std::set<GaValueHistoryBase*>& GACALL GetDependencies() const { return _dependencies; }

		/// <summary><c>SetCombiner</c> method sets combiner object.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="combiner"></param>
		/// <exception cref="GaInvalidOperationException" />Thrown if the value is created as evaluated value.</exception>
		/// <exception cref="GaNullArgumentException" />Thrown if the <c>combiner</c> is set to <c>NULL</c>.</exception>
		GAL_API
		void GACALL SetCombiner(GaValueCombiner* combiner);

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to object that performs combining of this value with another statistical value.</returns>
		inline GaValueCombiner* GACALL GetCombiner() { return _combiner; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to object that performs combining of this value with another statistical value.</returns>
		inline const GaValueCombiner* GACALL GetCombiner() const { return _combiner; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if the value can be combined with other values.</returns>
		inline bool GACALL IsCombinable() { return _combiner != NULL; }

		/// <summary><c>SetEvaluator</c> method sets evaluator object used to evaluate this value.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="evaluator">pointer ot ne evaluator object.</param>
		/// <exception cref="GaInvalidOperationException" />Thrown if the value is created as independent.</exception>
		/// <exception cref="GaNullArgumentException" />Thrown if the <c>evaluator</c> is set to <c>NULL</c>.</exception>
		GAL_API
		void GACALL SetEvaluator(GaValueEvaluator* evaluator);

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to object that performes evaluation of this value based on other statistical values.</returns>
		inline GaValueEvaluator* GACALL GetEvaluator() { return _evaluator; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to object that performes evaluation of this value based on other statistical values.</returns>
		inline const GaValueEvaluator* GACALL GetEvaluator() const { return _evaluator; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if this value is evalualted based on other statistical values.</returns>
		inline bool GACALL IsEvaluated() { return _evaluator != NULL; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to statistics object that owns this value.</returns>
		inline GaStatistics* GACALL GetOwner() { return _owner; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to statistics object that owns this value.</returns>
		inline const GaStatistics* GACALL GetOwner() const { return _owner; }

	protected:

		/// <summary>This method updates (reevaluates) all values that depends on this statistical value.</summary>
		inline void GACALL UpdateDependants()
		{
			for( std::set<GaValueHistoryBase*>::iterator it = _dependants.begin(); it != _dependants.end(); ++it )
				( *it )->Evaluate();
		}

	};

	/// <summary><c>GaValueHistory</c> template class provide storeage for current value and implements history tracking for specified type of statistical values.
	/// It also provides methods that calculates progress of the value.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	/// <param name="VALUE_TYPE">type of tracked value.</param>
	template <typename VALUE_TYPE>
	class GaValueHistory : public GaValueHistoryBase
	{

	public:

		/// <summary>Type of tracked value.</summary>
		typedef VALUE_TYPE GaType;

		/// <summary>Type of objects that store values.</summary>
		typedef GaValue<GaType> GaStorageType;

		/// <summary>This structure represents block of entries in history buffer. Block groups multiple entries with same value that has been
		/// inserted successively into the history buffer.</summary>
		struct GaHistoryEntry
		{

			friend class GaValueHistory;

		private:

			/// <summary>Value object of the block's entries.</summary>
			GaStorageType _value;

			/// <summary>Number of entries this value takes in history buffer.</summary>
			int _blockSize;

		public:

			/// <summary>This constructor initializes block with the specified value.</summary>
			/// <param name="value">value that is stored in this block.</param>
			GaHistoryEntry(const GaStorageType& value) : _value(value),
				_blockSize(1) { }

			/// <summary><c>GetValue</c> method returns pointer to value stored in this block.</summary>
			/// <returns>Method returns reference to value stored in this block.</returns>
			inline GaStorageType& GACALL GetValue() { return _value; }

			/// <summary><c>GetValue</c> method returns pointer to value stored in this block.</summary>
			/// <returns>Method returns reference to value stored in this block.</returns>
			inline const GaStorageType& GACALL GetValue() const { return _value; }

			/// <summary><c>GetBlockSize</c> method returns number of entries this value takes in history buffer.</summary>
			/// <returns>Method returns number of entries this value takes in history buffer.</returns>
			inline int GACALL GetBlockSize() const { return _blockSize; }

		};

	private:

		/// <summary>Current value.</summary>
		GaStorageType _current;

		/// <summary>History buffer.</summary>
		std::list<GaHistoryEntry*> _history;

		/// <summary>History buffer depth (number of entries in the buffer).</summary>
		int _maxHistoryDepth;

		/// <summary>Current history depth.</summary>
		int _currentHistoryDepth;

		/// <summary>Sum of all values ever inserted into the history buffer.</summary>
		GaStorageType _accumulated;

		/// <summary>Indicates whether the values that are inserted into the history buffer are summed into a single value.</summary>
		bool _accumulating;

	public:

		/// <summary>This constructor creates independant statistical value with specified history depth that is not bound to history depth of statistics object.</summary>
		/// <param name="owner">pointer to statistics object that owns value.</param>
		/// <param name="combiner">pointer to object that performs combining of this value with another statistical value.</param>
		/// <param name="historyDepth">history depth.</param>
		/// <param name="accumulating">if this parameter is <c>true</c>, values that are inserted into the history buffer are summed into a single value.</param>
		GaValueHistory(GaStatistics* owner,
			GaValueCombiner* combiner,
			int historyDepth,
			bool accumulating) : GaValueHistoryBase(owner, false, combiner),
			_maxHistoryDepth(historyDepth),
			_currentHistoryDepth(0),
			_accumulating(accumulating) { }

		/// <summary>This constructor creates independant statistical value with history depth that is bound to history depth of statistics object.</summary>
		/// <param name="owner">pointer to statistics object that owns value.</param>
		/// <param name="combiner">pointer to object that performs combining of this value with another statistical value.</param>
		/// <param name="accumulating">if this parameter is <c>true</c>, values that are inserted into the history buffer are summed into a single value.</param>
		GaValueHistory(GaStatistics* owner,
			GaValueCombiner* combiner,
			bool accumulating) : GaValueHistoryBase(owner, true, combiner),
			_currentHistoryDepth(0),
			_accumulating(accumulating) { _maxHistoryDepth = owner->GetHistoryDepth(); }

		/// <summary>This constructor creates statistical value that is evaluated based on other values with specified history depth 
		/// that is not bound to history depth of statistics object.</summary>
		/// <param name="owner">pointer to statistics object that owns value.</param>
		/// <param name="evaluator">pointer to object that performes evaluation of this value.</param>
		/// <param name="historyDepth">history depth.</param>
		/// <param name="accumulating">if this parameter is <c>true</c>, values that are inserted into the history buffer are summed into a single value.</param>
		GaValueHistory(GaStatistics* owner,
			GaValueEvaluator* evaluator,
			int historyDepth,
			bool accumulating) : GaValueHistoryBase(owner, false, evaluator),
			_maxHistoryDepth(historyDepth),
			_currentHistoryDepth(0),
			_accumulating(accumulating) { }

		/// <summary>This constructor creates statistical value that is evaluated based on other values with history depth that is bound to history depth of statistics object.</summary>
		/// <param name="owner">pointer to statistics object that owns value.</param>
		/// <param name="evaluator">pointer to object that performes evaluation of this value.</param>
		/// <param name="accumulating">if this parameter is <c>true</c>, values that are inserted into the history buffer are summed into a single value.</param>
		GaValueHistory(GaStatistics* owner,
			GaValueEvaluator* evaluator,
			bool accumulating) : GaValueHistoryBase(owner, true, evaluator),
			_currentHistoryDepth(0),
			_accumulating(accumulating) { _maxHistoryDepth = owner->GetHistoryDepth(); }

		/// <summary><c>Next</c> method moves current value to history buffer. If the buffer is full, the oldes value stored in the buffer is discharged.
		///
		/// This method is not thread-safe.</summary>
		/// <exception cref="GaInvalidOperationException" />Thrown if the current value is empty.</exception>
		virtual void GACALL Next()
		{
			GA_ASSERT( Common::Exceptions::GaInvalidOperationException, !_current.IsEmpty(), "Cannot move current value to history buffer because it is empty.", "Statistics" );

			GaHistoryEntry* add = NULL;

			if( _accumulating )
				_accumulated.IsEmpty() ? _accumulated = _current : _accumulated.GetValue() += _current.GetValue();

			// history tracking is enabled?
			if( _maxHistoryDepth )
			{
				// empty history buffer?
				if( !_currentHistoryDepth )
				{
					// just add new block to the buffer
					add = new GaHistoryEntry( _current );
					_currentHistoryDepth++;
				}
				else
				{
					GaHistoryEntry* last = _history.back();

					// should the new value be merged into the same block as previous value
					bool change = last->_value.GetValue() != _current.GetValue();

					if( !change )
					{
						// merge value with previous
						last->_blockSize++;

						// update generation change tracker
						_lastChange++;
					}
					else
						// update generation change tracker
						_lastChange = 0;

					// is the history buffer full?
					if( _currentHistoryDepth < _maxHistoryDepth )
					{
						// buffer is not full

						// make new block that should be inserted into the buffer and store new value there
						if( change )
							add = new GaHistoryEntry( _current );

						_currentHistoryDepth++;
					}
					else
					{
						// buffer is full

						GaHistoryEntry* first = _history.front();
						// oldest block contains only one value?
						if( first->_blockSize == 1 )
						{
							// remove block from the buffer
							_history.pop_front();

							if( change )
							{
								// new value is not merged - reuse removed block to store new value
								first->_value = _current;
								add = first;
							}
							else
								// new value is merged - delete removed block
								delete first;
						}
						else
						{
							// oldest block contains more than one value

							// remove single value from the block
							first->_blockSize--;

							// make new block that should be inserted into the buffer and store new value there
							if( change )
								add = new GaHistoryEntry( _current );
						}
					}
				}

				// add block to the buffer if new value is not merged
				if( add )
					_history.push_back( add );
			}
		}

		/// <summary><c>Clear</c> method clears history buffer and the current value.
		///
		/// This method is not thread-safe.</summary>
		virtual void GACALL GACALL Clear()
		{
			// free memory used by the history buffer and clear it
			_currentHistoryDepth = 0;
			_lastChange = 0;
			for( std::list<GaHistoryEntry*>::iterator it = _history.begin(); it != _history.end(); ++it )
				delete *it;
			_history.clear();

			_current.Clear();
		}

		/// <summary><c>ClearCurrent</c> method clears only the current value but keeps history buffer.
		///
		/// This method is not thread-safe.</summary>
		virtual void GACALL ClearCurrent() { _current.Clear(); }

		/// <summary><c>SetHistoryDepth</c> method sets depth of history buffer.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="depth">new history depth.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>depth</c> is negative value.</exception>
		virtual void GACALL SetHistoryDepth(int depth)
		{
			GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, depth >= 0, "depth", "Depth cannot be negative value.", "Statistics" );

			if( depth < _currentHistoryDepth )
			{
				// current depth is deeper then new - oldest values should be removed

				int current = _currentHistoryDepth;

				// find the first block that does not exceed new depth starting from the oldest
				std::list<GaHistoryEntry*>::iterator it1 = _history.begin();
				for( ; it1 != _history.end(); ++it1 )
				{
					// new buffer size when the current block is removed
					current -= ( *it1 )->_blockSize;

					// current block is the last block that exceed new depth
					if( current == depth )
					{
						++it1;
						break;
					}
					// curent block only partaly exceeds new depth
					else if( current < depth )
					{
						// shrink block
						( *it1 )->_blockSize -= depth - current;
						break;
					}
				}

				// remove oldest blocks of values that exceeds new depth
				for( std::list<GaHistoryEntry*>::iterator it2 = _history.begin(); it2 != it1; ++it2 )
					delete *it2;
				_history.erase( _history.begin(), it1 );

				_currentHistoryDepth = depth;
			}

			_maxHistoryDepth = depth;
		}

		/// <summary>See specification of <see cref="GaValueHistoryBase::GetHistoryDepth" /> method for more details.
		///
		/// This method is not thread-safe.</summary>
		virtual int GACALL GetHistoryDepth() { return _maxHistoryDepth; }

		/// <summary>See specification of <see cref="GaValueHistoryBase::GetCurrentHistoryDepth" /> method for more details.
		///
		/// This method is not thread-safe.</summary>
		virtual int GACALL GetCurrentHistoryDepth() const { return _currentHistoryDepth; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if the history buffer is full.</returns>
		virtual bool GACALL IsHistoryFull() const { return _currentHistoryDepth == _maxHistoryDepth; }

		/// <summary>See specification of <see cref="GaValueHistoryBase::GetRelativeProgress" /> method for more details.
		///
		/// This method is not thread-safe.</summary>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>depth</c> is deeper then depth of history buffer.</exception>
		virtual float GACALL GetRelativeProgress(int depth = 1) const { return _current.GetRelativeProgress( GetPrevious( depth ) ); }

		/// <summary><c>GetProgress</c> method calculates absolute progress of current value compared to specified value store in the history buffer.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="depth">position of value in the history buffer on which the progress calculation is based. If this parameter is <c>-1</c>
		/// method calcualtes progress based on oldes value from the history buffer.</param>
		/// <returns>Method returns calculated absolute progress of the value.</returns>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>depth</c> is deeper then depth of history buffer.</exception>
		inline GaStorageType GACALL GetProgress(int depth = 1) const { return GetCurrentValue() - GetPrevious( depth ); }

		/// <summary><c>SetCurrent</c> method change current value.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="value">new value that should be set as current.</param>
		inline void GACALL SetCurrent(const GaType& value)
		{
			_current = value;
			UpdateDependants();
		}

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to object that stores current value.</returns>
		inline GaValue<GaType>& GACALL GetCurrent() { return _current; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to object that stores current value.</returns>
		inline const GaValue<GaType>& GACALL GetCurrent() const { return _current; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to current value.</returns>
		inline GaType& GACALL GetCurrentValue() { return _current.GetValue(); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to current value.</returns>
		inline const GaType& GACALL GetCurrentValue() const { return _current.GetValue(); }

		/// <summary><c>GetPrevious</c> method returns value stored in history buffer.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="depth">position of the queried value in history buffer. If this parameter is <c>0</c> method returns curretn value.
		/// If it is <c>-1</c> method returns oldes value from the history buffer.</param>
		/// <returns>Method returns reference to value stored at specified position in history buffer.</returns>
		GaType& GACALL GetPrevious(int depth = 1)
		{
			if( depth == 0 )
				// return current value
				return GetCurrentValue();
			else if( depth < 0 || depth > _currentHistoryDepth )
				// return oldest value from the buffer
				return _history.front()->_value.GetValue();

			// find value at specified position starting from the newest value
			std::list<GaHistoryEntry*>::reverse_iterator it = _history.rbegin();
			for( int current = 0; it != _history.rend(); ++it )
			{
				// move current position in the buffer
				current += ( *it )->_blockSize;

				// found?
				if( depth <= current )
					break;
			}

			return ( *it )->_value.GetValue();
		}

		/// <summary><c>GetPrevious</c> method returns value stored in history buffer.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="depth">position of the queried value in history buffer. If this parameter is <c>0</c> method returns curretn value.
		/// If it is <c>-1</c> method returns oldes value from the history buffer.</param>
		/// <returns>Method returns reference to value stored at specified position in history buffer.</returns>
		const GaType& GACALL GetPrevious(int depth = 1) const
		{
			if( depth == 0 )
				// return current value
				return GetCurrentValue();
			else if( depth < 0 || depth > _currentHistoryDepth )
				// return oldest value from the buffer
				return _history.front()->_value.GetValue();

			// find value at specified position starting from the newest value
			std::list<GaHistoryEntry*>::const_reverse_iterator it = _history.rbegin();
			for( int current = 0; it != _history.rend(); ++it )
			{
				// move current position in the buffer
				current += ( *it )->_blockSize;

				// found?
				if( depth <= current )
					break;
			}

			return ( *it )->_value.GetValue();
		}

		/// <summary><c>IsChanged</c> method chechs whether the value has changed sompared to specified value from the history buffer.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="depth">position of the value against which the current value is compared.
		/// If it is <c>-1</c> method compares current value against oldes value from the history buffer.</param>
		/// <returns>Method returns <c>true</c> if the value is changed.</returns>
		bool GACALL IsChanged(int depth = 1) const
		{
			if( depth == 0 )
				/// compare to current - nothing changed
				return false;
			else if( depth < 0 || depth > _currentHistoryDepth )
				// compare to oldest value
				return _history.front()->_value != _history.back()->_value;

			// find value at specified position starting from the newest value
			std::list<GaHistoryEntry*>::const_reverse_iterator it = _history.rbegin();
			for( int current = 0; it != _history.rend(); ++it )
			{
				// move current position in the buffer
				current += ( *it )->_blockSize;

				// desired value is found
				if( depth <= current )
					break;
			}

			// is the value changed
			return ( *it )->_value != _history.back()->_value;
		}

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to history buffer.</returns>
		inline const std::list<GaHistoryEntry*>& GACALL GetHistory() const { return _history; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if the history tracking is enabled (history depth is greter then <c>0</c>).</returns>
		inline bool GACALL IsTrackingEnabled() const { return _maxHistoryDepth > 0; }

		/// <summary><c>EnableAccumulation</c> method enables or disables value accumulation.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="enabled">if this parameter is <c>true</c>, values that are inserted into the history buffer are summed into a single value.</param>
		inline void GACALL EnableAccumulation(bool enabled)
		{
			if( _accumulating != enabled )
			{
				if( !enabled )
					_accumulated.Clear();

				_accumulating = enabled;
			}
		}

		/// <summary>This method is not thread-safe.</summary>
		/// <summary>Method returns <c>true</c> if the values that are inserted into the history buffer are summed into a single value.</summary>
		inline bool GACALL IsAccumulationEnabled() const { return _accumulating; }

	private:

		/// <summary>Disables copy constructor.</summary>
		GaValueHistory(const GaValueHistory<GaType>& rhs) { }

		/// <summary>Disables assign operator.</summary>
		inline GaValueHistory<GaType>& GACALL operator =(const GaValueHistory<GaType>& rhs) { }

	};

	/// <summary><c>GaStatistics</c> class stores and tracks group of statistical values. Each value can be accessed its unique ID.
	///
	/// This class has built-in synchronizator so it is allowed to use <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros with instances of this class.
	/// All public methods are thread-safe.</summary>
	class GaStatistics
	{

		GA_SYNC_CLASS

	private:

		/// <summary>Generation counter that track number of time that current values has been moved to history buffers.</summary>
		int _currentGeneration;

		/// <summary>Preferred hitory buffer size for statistical values.</summary>
		int _historyDepth;

		/// <summary>Statistical values </summary>
		STLEXT::hash_map<int, GaValueHistoryBase*> _values;
		
		/// <summary>Low resolution stopwatch.</summary>
		Common::Timing::GaStopwatchLowRes _lowResStopwatch;

		/// <summary>High resolution stopwatch.</summary>
		Common::Timing::GaStopwatchHiRes _highResStopwatch;

	public:

		/// <summary>This constructor initializes empty group.</summary>
		GaStatistics() : _currentGeneration(0),
			_historyDepth(10) { }

		/// <summary><c>AddValue</c> method inserts new statistical value to the group.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="id">ID of the new value.</param>
		/// <param name="value">pointer to value that should be inserted.</param>
		/// <exception cref="GaNullArgumentException" />Thrown if <c>value</c> is <c>NULL</c>.</exception>
		/// <exception cref="GaArgumentException" />Thrown if there is already a value with specified ID in this group.</exception>
		GAL_API
		void GACALL AddValue(int id,
			GaValueHistoryBase* value);

		/// <summary><c>RemoveValue</c> method removes statistical value from the group.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="id">ID of the value that should be removed.</param>
		GAL_API
		void GACALL RemoveValue(int id);

		/// <summary><c>BindValues</c> method binds value to its evaluation dependency.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="value">pointer to value that should be bound.</param>
		/// <param name="dependencyID">ID of the dependency value.</param>
		/// <exception cref="GaNullArgumentException" />Thrown if <c>value</c> is <c>NULL</c>.</exception>
		/// <exception cref="GaArgumentException" />Thrown if there is no value with specified ID in this group.</exception>
		GAL_API
		void GACALL BindValues(GaValueHistoryBase* value,
			int dependencyID);

		/// <summary><c>Next</c> method moves all current values to history buffers and increments generation counter.
		///
		/// This method is not thread-safe.</summary>
		inline void GACALL Next()
		{
			for( STLEXT::hash_map<int, GaValueHistoryBase*>::iterator it = _values.begin(); it != _values.end(); ++it )
				it->second->Next();

			_currentGeneration++;
		}

		/// <summary><c>Clear</c> method clears all stored statistical values.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="keepGenerationCounter">if this parameter is set to <c>true</c> method also restarts generation counter.</param>
		inline void GACALL Clear(bool keepGenerationCounter)
		{
			// clear history buffer and current statistical values
			for( STLEXT::hash_map<int, GaValueHistoryBase*>::iterator it = _values.begin(); it != _values.end(); ++it )
				it->second->Clear();

			if( !keepGenerationCounter )
				_currentGeneration = 0;
		}

		/// <summary><c>ClearCurrent</c> method clears all current statistical values stored in this group.
		///
		/// This method is not thread-safe.</summary>
		inline void GACALL ClearCurrent()
		{
			for( STLEXT::hash_map<int, GaValueHistoryBase*>::iterator it = _values.begin(); it != _values.end(); ++it )
				it->second->ClearCurrent();
		}

		/// <summary><c>SetHistoryDepth</c> method sets depth of history buffers for all values in the group that are bound to to preferred history depth.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="depth">new history depth.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>depth</c> is negative value.</exception>
		GAL_API
		void GACALL SetHistoryDepth(int depth);

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns preferred depth of history buffer for values in the group.</returns>
		inline int GACALL GetHistoryDepth() const { return _historyDepth; }

		/// <summary><c>Combine</c> method combines all combinable statistical values of this group with values of another group. Results overwrties current values in this group.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="stats">reference to statistics object whose values are combined with values of this group.</param>
		inline void GACALL Combine(const GaStatistics& stats)
		{
			for( STLEXT::hash_map<int, GaValueHistoryBase*>::iterator it = _values.begin(); it != _values.end(); ++it )
			{
				if( it->second->IsCombinable() )
					it->second->Combine( stats[ it->first ] );
			}
		}

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns generation counter.</returns>
		inline int GACALL GetCurrentGeneration() const { return _currentGeneration; }

		/// <summary><c>GetValue</c> method returns statistical value with specified ID that is stored in this group.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="id">ID of the queried value.</param>
		/// <returns>Method returns reference to queried statistical value.</returns>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if there is no value with specified ID.</exception>
		inline GaValueHistoryBase& GACALL GetValue(int id)
		{
			STLEXT::hash_map<int, GaValueHistoryBase*>::iterator it = _values.find( id );
			GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, it != _values.end(), "id", "Statistical value with specified ID does not exist.", "Statistics" );

			return *it->second;
		}

		/// <summary><c>GetValue</c> method returns statistical value with specified ID that is stored in this group.
		///
		/// This method is not thread-safe.</summary>
		/// <typeparam name="VALUE_TYPE">type of value.</typeparam>
		/// <param name="id">ID of the queried value.</param>
		/// <returns>Method returns reference to queried statistical value.</returns>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if there is no value with specified ID.</exception>
		template<typename VALUE_TYPE>
		inline GaValueHistory<VALUE_TYPE>& GetValue(int id) { return (GaValueHistory<VALUE_TYPE>&)GetValue( id ); }

		/// <summary><c>GetValue</c> method returns statistical value with specified ID that is stored in this group.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="id">ID of the queried value.</param>
		/// <returns>Method returns reference to queried statistical value.</returns>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if there is no value with specified ID.</exception>
		inline const GaValueHistoryBase& GACALL GetValue(int id) const
		{
			STLEXT::hash_map<int, GaValueHistoryBase*>::const_iterator it = _values.find( id );
			GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, it != _values.end(), "id", "Statistical value with specified ID does not exist.", "Statistics" );

			return *it->second;
		}

		/// <summary><c>GetValue</c> method returns statistical value with specified ID that is stored in this group.
		///
		/// This method is not thread-safe.</summary>
		/// <typeparam name="VALUE_TYPE">type of value.</typeparam>
		/// <param name="id">ID of the queried value.</param>
		/// <returns>Method returns reference to queried statistical value.</returns>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown if there is no value with specified ID.</exception>
		template<typename VALUE_TYPE>
		inline const GaValueHistory<VALUE_TYPE>& GetValue(int id) const { return (GaValueHistory<VALUE_TYPE>&)GetValue( id ); }

		/// <summary><c>IsExist</c> method checks existance of value with specified ID in the group.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="id">ID of the queried value.</param>
		/// <returns>Method returns <c>true</c> if there is value with specified ID in this group.</returns>
		inline bool GACALL IsExist(int id) const { return _values.find( id ) != _values.end(); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns number of stored statistical values in this group.</returns>
		inline int GACALL GetNumberOfValues() const { return (int)_values.size(); }

		/// <summary><c>ResetTime</c> method restarts stopwatches.
		///
		/// This method is not thread-safe.</summary>
		inline void GACALL ResetTime()
		{
			_lowResStopwatch.Restart();
			_highResStopwatch.Restart();
		};

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns current time of low resolution stopwatch.</returns>
		inline long long GACALL GetCurrentTimeLowRes() { return _lowResStopwatch.GetClock(); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns current time of high resolution stopwatch.</returns>
		inline long long GACALL GetCurrentTimeHighRes() { return _highResStopwatch.GetClock(); }

		/// <summary><c>operator []</c> returns statistical value with specified ID that is stored in this group.
		///
		/// This operator is not thread-safe.</summary>
		/// <param name="id">ID of the queried value.</param>
		/// <returns>operator returns reference to queried statistical value or <c>NULL</c> if the value does not exist.</returns>
		inline GaValueHistoryBase& GACALL operator [](int id) { return GetValue( id ); }

		/// <summary><c>operator []</c> returns statistical value with specified ID that is stored in this group.
		///
		/// This operator is not thread-safe.</summary>
		/// <param name="id">ID of the queried value.</param>
		/// <returns>operator returns reference to queried statistical value or <c>NULL</c> if the value does not exist.</returns>
		inline const GaValueHistoryBase& GACALL operator [](int id) const { return GetValue( id ); }

	};

	/// <summary>This enum defines possible value selection modes of combiner.</summary>
	enum GaSelectionCombinerType
	{

		/// <summary>Selection mode that selects greater statistical value.</summary>
		GASCT_GREATER,

		/// <summary>Selection mode that selects lower statistical value.</summary>
		GASCT_LOWER

	};

	/// <summary><c>GaSelectionCombiner</c> class represent interface for statistical value combiners that produces result by choosing greater or lower statistical value
	/// depending on configuration of the combiner.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// Because this combiner is stateless all public method are thread-safe.</summary>
	/// <param name="VALUE_TYPE">type of value on which the combiner operates.</param>
	template <typename VALUE_TYPE>
	class GaSelectionCombiner : public GaValueCombiner
	{

	public:

		/// <summary>Type of values on which this combiner operates.</summary>
		typedef VALUE_TYPE GaType;

		/// <summary>Type of object that tracks value history.</summary>
		typedef GaValueHistory<GaType> GaHistoryType;

	protected:

		/// <summary>Combiner's selection mode.</summary>
		GaSelectionCombinerType _selectionType;

	public:

		/// <summary>This constructor initializes combiner with type of selection.</summary>
		/// <param name="selectionType">combiner's selection mode.</param>
		GaSelectionCombiner(GaSelectionCombinerType selectionType) : _selectionType(selectionType) { }

		/// <summary>More details are given in specification of <see cref="GaValueCombiner::Combine" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual void GACALL Combine(Statistics::GaValueHistoryBase& value1,
			const Statistics::GaValueHistoryBase& value2) const
		{
			GaValue<GaType>& v1 = ( (GaHistoryType&)value1 ).GetCurrent();
			const GaValue<GaType>& v2 = ( (const GaHistoryType&)value2 ).GetCurrent();

			if( v1.HasValue() && v2.HasValue() )
			{
				// compare to value and determin which one should be selected
				bool move = IsGreater( v2.GetValue(), v1.GetValue() );
				if( _selectionType == GASCT_LOWER )
					move= !move;

				if( move )
					// the second value is selected and it should be copied to destination
					v1 = v2;
			}
			else if( !v2.IsEmpty() )
				// destination value is empty - just copy second value
				v1 = v2;
		}

		/// <summary>This method is thread-safe.</summary>
		/// <returns>Method returns selection mode of the combiner.</returns>
		inline GaSelectionCombinerType GACALL GetSelectionType() const { return _selectionType; }

	protected:

		/// <summary><c>IsGreater</c> method comares two values and returns <c>true</c> if the first value is freater then the second.</summary>
		/// <param name="value1">reference to the first value used in comparison.</param>
		/// <param name="value2">reference to the second value used in comparison.</param>
		/// <returns>Method returns <c>true</c> if the first value is greater then the second.</returns>
		virtual bool GACALL IsGreater(const GaType& value1,
			const GaType& value2) const = 0;

	};

	/// <summary><c>GaDefaultSelectionCombiner</c> class represent default statistical value combiners that produces result by choosing greater or lower statistical value
	/// depending on configuration of the combiner.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// Because this combiner is stateless all public method are thread-safe.</summary>
	/// <param name="VALUE_TYPE">type of value on which the combiner operates.</param>
	template <typename VALUE_TYPE>
	class GaDefaultSelectionCombiner : public GaSelectionCombiner<VALUE_TYPE>
	{

	public:

		/// <summary>This constructor initializes combiner with selection type.</summary>
		/// <param name="selectionType">combiner's selection mode.</param>
		GaDefaultSelectionCombiner(GaSelectionCombinerType selectionType) : GaSelectionCombiner(selectionType) { }

	protected:

		/// <summary>More details are given in specification of <see cref="GaDefaultSelectionCombiner::IsGreater" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual bool GACALL IsGreater(const GaType& value1,
			const GaType& value2) const { return value1 > value2; }

	};

	/// <summary><c>GaSumValueCombiner</c> class represent statistical value combiner that produces result by summing twho values.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// Because this combiner is stateless all public method are thread-safe.</summary>
	/// <param name="VALUE_TYPE">type of value on which the combiner operates.</param>
	template <typename VALUE_TYPE>
	class GaSumValueCombiner : public GaValueCombiner
	{

	public:

		/// <summary>Type of values on which this combiner operates.</summary>
		typedef VALUE_TYPE GaType;

		/// <summary>Type of object that tracks value history.</summary>
		typedef GaValueHistory<GaType> GaHistoryType;

		/// <summary>More details are given in specification of <see cref="GaValueCombiner::Combine" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual void GACALL Combine(GaValueHistoryBase& value1,
			const GaValueHistoryBase& value2) const
		{
			GaValue<GaType>& v1 = ( (GaHistoryType&)value1 ).GetCurrent();
			const GaValue<GaType>& v2 = ( (const GaHistoryType&)value2 ).GetCurrent();

			if( v1.HasValue() && v2.HasValue() )
				// sum and store result
				v1 += v2;
			else if( !v2.IsEmpty() )
				v1 = v2;
		}

	};

	/// <summary><c>GaDivEvaluator</c> calculates quotient of the two input statistical values (<c>outputData = inputData1 / inputData2</c>).
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// Because this evaluator is stateless all public method are thread-safe.</summary>
	/// <typeparam name="INPUT_VALUE_TYPE1">type of the first operand.</typeparam>
	/// <typeparam name="INPUT_VALUE_TYPE2">type of the the operand.</typeparam>
	/// <typeparam name="OUTPUT_VALUE_TYPE">type of result.</typeparam>
	template<typename INPUT_VALUE_TYPE1,
		typename INPUT_VALUE_TYPE2,
		typename OUTPUT_VALUE_TYPE>
	class GaDivEvaluator : public GaValueEvaluator
	{

	public:

		/// <summary>Type of the first operand.</summary>
		typedef INPUT_VALUE_TYPE1 GaInputType1;

		/// <summary>Type of object that tracks history of the first operand.</summary>
		typedef GaValueHistory<GaInputType1> GaInputHistoryType1;

		/// <summary>Type of the second operand.</summary>
		typedef INPUT_VALUE_TYPE2 GaInputType2;

		/// <summary>Type of object that tracks history of the second operand.</summary>
		typedef GaValueHistory<GaInputType2> GaInputHistoryType2;

		/// <summary>Type of result.</summary>
		typedef OUTPUT_VALUE_TYPE GaOutputType;

		/// <summary>Type of object that tracks history of the result.</summary>
		typedef GaValueHistory<GaOutputType> GaOutputHistoryType;

	private:

		/// <summary>ID of the statistical value used as the first operand.</summary>
		int _inputData1;

		/// <summary>ID of the statistical value used as the second operand.</summary>
		int _inputData2;

		/// <summary>ID of the statistical value used as storage for the resutls.</summary>
		int _outputData;

	public:

		/// <summary>This constructor initializes evaluator with ID of data used for evaluation.</summary>
		/// <param name="inputData1">ID of the statistical value used as the first operand.</param>
		/// <param name="inputData2">ID of the statistical value used as the second operand.</param>
		/// <param name="outputData">ID of the statistical value used as storage for the resutls.</param>
		GaDivEvaluator(int inputData1,
			int inputData2,
			int outputData) : _inputData1(inputData1),
			_inputData2(inputData2),
			_outputData(outputData) { }

		/// <summary>More details are given in specification of <see cref="GaValueEvaluator::Bind" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual void GACALL Bind(Statistics::GaValueHistoryBase* value) const
		{
			try
			{
				// bind output statistical value to input operands
				value->GetOwner()->BindValues( value, _inputData1 );
				value->GetOwner()->BindValues( value, _inputData2 );
			}
			catch(...)
			{
				// remove introduced bindings if the value cannot be bound to some of the dependencies
				value->RemoveAllDependencies();
				throw;
			}
		}

		/// <summary>More details are given in specification of <see cref="GaValueEvaluator::Evaluate" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual void GACALL Evaluate(Statistics::GaValueHistoryBase& value) const
		{
			GaValue<GaOutputType>& result = ( (GaOutputHistoryType&)value ).GetCurrent();
			GaValue<GaInputType1>& operand1 = value.GetOwner()->GetValue<GaInputType1>( _inputData1 ).GetCurrent();
			GaValue<GaInputType2>& operand2 = value.GetOwner()->GetValue<GaInputType2>( _inputData2 ).GetCurrent();

			if( operand1.HasValue() && operand2.HasValue() )
				// calculate and store results
				result = operand1 / operand2;
		}

	};

	/// <summary><c>SqrtEvaluator</c> calculates square root of theinput value (<c>outputData = sqrt( inputData )</c>).
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// Because this evaluator is stateless all public method are thread-safe.</summary>
	/// <typeparam name="INPUT_VALUE_TYPE">type of source operand.</typeparam>
	/// <typeparam name="OUTPUT_VALUE_TYPE">type of output result.</typeparam>
	template<typename INPUT_VALUE_TYPE,
		typename OUTPUT_VALUE_TYPE>
	class SqrtEvaluator : public GaValueEvaluator
	{

	public:

		/// <summary>Type of the source operand.</summary>
		typedef INPUT_VALUE_TYPE GaInputType;

		/// <summary>Type of object that tracks history of the source operand.</summary>
		typedef GaValueHistory<GaInputType> GaInputHistoryType;

		/// <summary>Type of result.</summary>
		typedef OUTPUT_VALUE_TYPE GaOutputType;

		/// <summary>Type of object that tracks history of the result.</summary>
		typedef GaValueHistory<GaOutputType> GaOutputHistoryType;

	private:

		/// <summary>ID of the statistical value used as source operand.</summary>
		int _inputData;

		/// <summary>ID of the statistical value used as storage for the resutls.</summary>
		int _outputData;

	public:

		/// <summary>This constructor initializes evaluator with ID of data used for evaluation.</summary>
		/// <param name="inputData">ID of the statistical value used as source operand.</param>
		/// <param name="outputData">ID of the statistical value used as storage for the resutls.</param>
		SqrtEvaluator(int inputData,
			int outputData) : _inputData(inputData),
			_outputData(outputData) { }

		/// <summary>More details are given in specification of <see cref="GaValueEvaluator::Bind" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual void GACALL Bind(Statistics::GaValueHistoryBase* value) const
		{
			try
			{
				// bind output statistical value to input operands
				value->GetOwner()->BindValues( value, _inputData );
			}
			catch(...)
			{
				// remove introduced bindings if the value cannot be bound to some of the dependencies
				value->RemoveAllDependencies();
				throw;
			}
		}

		/// <summary>More details are given in specification of <see cref="GaValueEvaluator::Evaluate" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual void GACALL Evaluate(GaValueHistoryBase& value) const
		{
			GaValue<GaInputType>& operand = value.GetOwner()->GetValue<GaInputType>( _inputData ).GetCurrent();

			if( operand.HasValue() )
				// calculate and store results
				( (GaOutputHistoryType&)value ).SetCurrent( std::sqrt( operand.GetValue() ) );
		}

	};

	/// <summary><c>GaValueHistoryFactory</c> is interface class for factories of statistical values. Value factory abstract creation of statistical value.</summary>
	class GaValueHistoryFactory
	{

	public:

		/// <summary><c>CreateValue</c> creates new statistical value with default combiner or evaluator.</summary>
		/// <param name="statistics">reference to statistics object into which the new value should be inserted.</param>
		/// <param name="valueID">ID of the value.</param>
		/// <param name="historyDepth">history depth for the new value. If this parameter is set to <c>NULL</c> history depth will be bound to 
		/// preferred history depth of statistic object.</param>
		/// <returns>Method returns pointer to newly created value.</returns>
		GAL_API
		virtual Statistics::GaValueHistoryBase* GACALL CreateValue(GaStatistics& statistics,
			int valueID,
			int historyDepth = -1) const = 0;

		/// <summary><c>CreateValue</c> creates new statistical value with defined ID and characteristics and inserts it to statistics object. Created value is independant value.</summary>
		/// <param name="statistics">reference to statistics object into which the new value should be inserted.</param>
		/// <param name="valueID">ID of the value.</param>
		/// <param name="combiner">pointer to combiner object that will be used by new value.
		/// If this parameter is set to <c>NULL</c> method will set default combiner for specified value ID.</param>
		/// <param name="historyDepth">history depth for the new value. If this parameter is set to <c>NULL</c> history depth will be bound to 
		/// preferred history depth of statistic object.</param>
		/// <returns>Method returns pointer to newly created value.</returns>
		virtual GaValueHistoryBase* GACALL CreateValue(GaStatistics& statistics,
			int valueID,
			GaValueCombiner* combiner,
			int historyDepth = -1) const = 0;

		/// <summary><c>CreateValue</c> creates new statistical value with defined ID and characteristics and inserts it to statistics object.  Created value is evaluated value.</summary>
		/// <param name="statistics">reference to statistics object into which the new value should be inserted.</param>
		/// <param name="valueID">ID of the value.</param>
		/// <param name="evaluator">pointer to evaluator object that will be used by new value.
		/// If this parameter is set to <c>NULL</c> method will set default evaluator for specified value ID.</param>
		/// <param name="historyDepth">history depth for the new value. If this parameter is set to <c>NULL</c> history depth will be bound to 
		/// preferred history depth of statistic object.</param>
		/// <returns>Method returns pointer to newly created value.</returns>
		virtual GaValueHistoryBase* GACALL CreateValue(GaStatistics& statistics,
			int valueID,
			GaValueEvaluator* evaluator,
			int historyDepth = -1) const = 0;

	};

} // Statistics

#endif //__GA_STATISTICS_H__

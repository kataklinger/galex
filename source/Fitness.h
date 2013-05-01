
/*! \file Fitness.h
    \brief This file decalres interfaces and classes that handle fitness values.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_FITNESS_H__
#define __GA_FITNESS_H__

#include "Operation.h"
#include "Catalogue.h"
#include "Statistics.h"

/// <summary>Contains interfaces, classes datatypes and functions for handling and storing fitness value.</summary>
namespace Fitness
{

	/// <summary><c>GaFitnessParams</c> is interface for parameters of fitness value objects.</summary>
	class GaFitnessParams : public Common::GaParameters { };

	/// <summary><c>GaFitness</c> is interface for fitness values of chromosomes.</summary>
	class GaFitness
	{

	protected:

		/// <summary>Parameters of fitness value.</summary>
		Common::Memory::GaSmartPtr<const GaFitnessParams> _parameters;

	public:

		/// <summary>This constructor initializes fitness value with parameters and type.</summary>
		/// <param name="parameters">smart pointer to parameters that will be used by the fitness value.</param>
		GaFitness(Common::Memory::GaSmartPtr<const GaFitnessParams> parameters) : _parameters(parameters) { }

		/// <summary>Copy constructor.</summary>
		/// <param name="rhs">source fitness value to be copied.</param>
		GaFitness(const GaFitness& rhs) : _parameters(rhs._parameters) { }

		/// <summary>Virtual destructor must be defined because this is base class.</summary>
		virtual ~GaFitness() { }

		/// <summary><c>SetParameters</c> metho sets parameters for the fitness value.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="parameters">smart pointer to parameters of the fitness value.</param>
		inline void GACALL SetParameters(Common::Memory::GaSmartPtr<const GaFitnessParams> parameters) { _parameters = parameters; }

		/// <summary>This method is thread-safe.</summary>
		/// <returns>Method returns smart pointer to parameters of fitness value.</returns>
		inline Common::Memory::GaSmartPtr<const GaFitnessParams> GACALL GetParameters() const { return _parameters; }

		/// <summary>This method produces copy of fitness value object. It can create exact copy or it can empty object of the same type.
		/// Caller is responsible for allocated memory of new fitness value object.</summary>
		/// <returns>Method returns pointer to newly created fitness value object.</returns>
		virtual GaFitness* GACALL Clone() const = 0;

		/// <summary>Restarts fitness object to default value.</summary>
		virtual void GACALL Clear() = 0;

		/// <summary><c>GetProbabilityBase</c> method calcuates selection probability for the fitness value.</summary>
		/// <returns>Method returns scalcualted selection probability.</returns>
		virtual float GACALL GetProbabilityBase() const = 0;

		/// <summary><c>GetProgress</c> method calculates progress of the fitness comapred to provided fitness object.</summary>
		/// <param name="previous">reference to fitness object to which the progress is calculated.</param>
		/// <returns>Operator returns progress of the fitness comared to provided value.</returns>
		virtual GaFitness* GACALL GetProgress(const GaFitness& previous) const = 0;

		/// <summary><c>GetRelativeProgress</c> method calculates relative progress of the fitness comapred to provided fitness object.</summary>
		/// <param name="previous">reference to fitness object to which the progress is calculated.</param>
		/// <returns>Operator returns relative progress of the fitness comared to provided value.</returns>
		virtual float GACALL GetRelativeProgress(const GaFitness& previous) const = 0;

		/// <summary><c>Distance</c> method calculate euclidean distance between two fitness values.</summary>
		/// <param name="second">reference to fitness object to which the distance is calculated</param>
		/// <returns>Method returns euclidian distance between two fitness values.</returns>
		virtual float GACALL Distance(const GaFitness& second) const = 0;

		/// <summary>This operatotor adds values of one fitness object to this object and makes new fitness object.</summary>
		/// <param name="rhs">reference to fitness object which should be added to this fitness object.</param>
		/// <returns>Operator returns value object that wraps new fitness object.</returns>
		virtual Statistics::GaValue<GaFitness> GACALL operator +(const GaFitness& rhs) const = 0;

		/// <summary>This operatotor subtracts values of one fitness object from this object and makes new fitness object.</summary>
		/// <param name="rhs">reference to fitness object which should be subtracted from this fitness object.</param>
		/// <returns>Operator returns value object that wraps new fitness object.</returns>
		virtual Statistics::GaValue<GaFitness> GACALL operator -(const GaFitness& rhs) const = 0;

		/// <summary>This operatotor divides fitness values in this fitness object with integer number and makes new fitness object.</summary>
		/// <param name="rhs">value which divides fitness value.</param>
		/// <returns>Operator returns value object that wraps new fitness object.</returns>
		virtual Statistics::GaValue<GaFitness> GACALL operator /(int rhs) const = 0;

		/// <summary>This operatotor adds values of one fitness object to <c>this</c> fitness object.</summary>
		/// <param name="rhs">reference to fitness object which should be added to this fitness object.</param>
		/// <returns>Operator returns reference to <c>this</c> object.</returns>
		virtual GaFitness& GACALL operator +=(const GaFitness& rhs) = 0;

		/// <summary>This operatotor subtracts values of one fitness object from <c>this</c> fitness object.</summary>
		/// <param name="rhs">reference to fitness object which should be subtracted from this fitness object.</param>
		/// <returns>Operator returns reference to <c>this</c> object.</returns>
		virtual GaFitness& GACALL operator -=(const GaFitness& rhs) = 0;

		/// <summary>This operatotor divides fitness values in this fitness object with integer number and makes new fitness object.</summary>
		/// <param name="rhs">value which divides fitness value.</param>
		/// <returns>Operator returns reference to <c>this</c> object.</returns>
		virtual GaFitness& GACALL operator /=(int rhs) = 0;
		
		/// <summary>This operator copies fitness values from another fitness object.</summary>
		/// <param name="rhs">reference to fitness object which should be copied to <c>this</c> object.</param>
		/// <returns>Operator returns reference to <c>this</c> object.</returns>
		virtual GaFitness& GACALL operator =(const GaFitness& rhs) = 0;

		/// <summary>This operator tests two fitness object for equality.</summary>
		/// <param name="rhs">reference to fitness object which is compared to this object.</param>
		/// <returns>Operator returns <c>true</c> if the two fitness objects are equal.</returns>
		virtual bool GACALL operator ==(const GaFitness& rhs) const = 0;

		/// <summary>This operator tests two fitness object for inequality.</summary>
		/// <param name="rhs">reference to fitness object which is compared to this object.</param>
		/// <returns>Operator returns <c>true</c> if the two fitness objects are not equal.</returns>
		virtual bool GACALL operator !=(const GaFitness& rhs) const = 0;

	};

	/// <summary><c>GaFitnessComparatorParams</c> is interface class for parameters of fitness comparators.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaFitnessComparatorParams : public Common::GaParameters { };

	/// <summary><c>GaFitnessComparatorConfig</c> is interface class for configuration of fitness comparators.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaFitnessComparatorConfig : public Common::GaConfiguration { };

	/// <summary><c>GaFitnessComparator</c> class is interface for fitness comparators.</summary>
	class GaFitnessComparator : public Common::GaOperation
	{

	public:

		/// <summary>This operator compares fitness values.</summary>
		/// <param name="fitness1">reference to fitness value of the first chromosome.</param>
		/// <param name="fitness2">reference to fitness value of the second chromosome.</param>
		/// <param name="parameters">reference to parameters of fitness comparison operation.</param>
		/// <returns>Method returns:
		/// <br/>a. -1 if the first fitness value lower then the second.
		/// <br/>b.  0 if the fitness values are equal.
		/// <br/>c.  1 if the first fitness value is greater then the second.</returns>
		virtual int GACALL operator ()(const GaFitness& fitness1,
			const GaFitness& fitness2,
			const GaFitnessComparatorParams& parameters) const = 0;

		/// <summary>More details are given in specification of <see cref="GaOperation::CreateConfiguration" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual Common::GaConfiguration* GACALL CreateConfiguration() const { return NULL; }

	};

	/// <summary><c>GaFitnessComparatorSetup</c> type is instance of <see cref="GaOperationSetup" /> template class and represents setup of
	/// fitness comparators and its parameters. Detailed description can be found in specification of
	/// <see cref="GaOperationSetup" /> template class.</summary>
	typedef Common::GaOperationSetup<GaFitnessComparator, GaFitnessComparatorParams, GaFitnessComparatorConfig> GaFitnessComparatorSetup;

	/// <summary><c>GaFitnessComparatorCatalogue</c> type is instance of <see cref="GaCatalogue" /> template class and represents catalogue of fitness comparators.
	/// Detailed description can be found in specification of <see cref="GaCatalogue" /> template class.</summary>
	typedef Common::Data::GaCatalogue<GaFitnessComparator> GaFitnessComparatorCatalogue;

	/// <summary><c>GaFitnessOperationParams</c> is interface for parameters of fitness operations.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaFitnessOperationParams : public Common::GaParameters { };

	/// <summary><c>GaFitnessOperationConfig</c> is base class for configuration classes of fitness operations.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaFitnessOperationConfig : public Common::GaConfiguration
	{

	protected:

		/// <summary>Fitness parameters.</summary>
		Common::Memory::GaSmartPtr<GaFitnessParams> _fitnessParams;

	public:

		/// <summary>This constructor initializes configuration with fitness parameters.</summary>
		/// <param name="fitnessParams">pointer to fitness parameters.</param>
		GaFitnessOperationConfig(const GaFitnessParams* fitnessParams) { SetFitnessParams( fitnessParams ); }

		/// <summary>This constructor creates new configuration and makes copy fitness parameters.</summary>
		/// <param name="rhs">configuration that should be copied.</param>
		GaFitnessOperationConfig(const GaFitnessOperationConfig& rhs) { SetFitnessParams( rhs._fitnessParams.GetRawPtr() ); }

		/// <summary>Default configuration initializes configuration without fitness parameters.</summary>
		GaFitnessOperationConfig() { }

		/// <summary>More details are given in specification of <see cref="GaConfiguration::Clone" /> method.
		///
		/// This method is not thread-safe.</summary>
		virtual Common::GaConfiguration* GACALL Clone() const { return new GaFitnessOperationConfig( *this ); }

		/// <summary><c>SetFitnessParams</c> method stores new fitness parameter in the configuration.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="fitnessParams">pointer to fitness parameters.</param>
		inline void GACALL SetFitnessParams(const GaFitnessParams* fitnessParams) { _fitnessParams = fitnessParams ? (GaFitnessParams*)fitnessParams->Clone() : NULL; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns smart pointer to fitness parameters stored in the configuration.</summary>
		inline Common::Memory::GaSmartPtr<GaFitnessParams> GACALL GetFitnessParams() { return _fitnessParams; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns smart pointer to fitness parameters stored in the configuration.</summary>
		inline Common::Memory::GaSmartPtr<const GaFitnessParams> GACALL GetFitnessParams() const { return _fitnessParams; }

		/// <summary>This method is not thread-safe.</summary>
		/// <summary>Method returns <c>true</c> if the fitness parameters are specified in the configuration.</summary>
		inline bool GACALL HasFitnessParams() const { return !_fitnessParams.IsNull(); }

	};

	/// <summary><c>GaFitnessOperation</c> class is interface for operations that need to create fitness value objects.</summary>
	class GaFitnessOperation : public Common::GaOperation
	{

	public:

		/// <summary><c>CreateFitnessObject</c> method creates fitness value object based on provided parameters.</summary>
		/// <param name="params">smart pointer to fitness parameters that are used to create fitness object.</param>
		/// <returns>Method returns pointer to newly created fitness object.</returns>
		virtual GaFitness* GACALL CreateFitnessObject(Common::Memory::GaSmartPtr<const GaFitnessParams> params) const = 0;

		/// <summary>More details are given in specification of <see cref="GaOperation::CreateConfiguration" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual Common::GaConfiguration* GACALL CreateConfiguration() const { return new GaFitnessOperationConfig(); }

	};

	/// <summary><c>GaFitnessOperation</c> class is interface for operations that calculates raw fitness values of an object of specified type.</summary>
	/// <typeparam name="OBJECT_TYPE">type of object whose fitness value should be calculated.</typeparam>
	template<typename OBJECT_TYPE>
	class GaRawFitnessOperation : public GaFitnessOperation
	{

	public:

		/// <summary>Type of object whose fitness value should be calculated.</summary>
		typedef OBJECT_TYPE GaObjectType;

		/// <summary>This operator calculates fitness value of the chromosome.</summary>
		/// <param name="object">reference to object whose fitness is calculated.</param>
		/// <param name="fitness">reference to fitness object to which teh evaluated fitness value is stored.</param>
		/// <param name="operationParams">reference to parameters of fitness operation.</param>
		virtual void GACALL operator ()(const GaObjectType& object,
			GaFitness& fitness,
			const GaFitnessOperationParams& operationParams) const = 0;

		/// <summary>More details are given in specification of <see cref="GaOperation::CreateConfiguration" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual Common::GaConfiguration* GACALL CreateConfiguration() const { return NULL; }

	};

	/// <summary><c>GaFitnessSelectionCombiner</c> class represent default statistical value combiners that produces result by choosing better or worse fitness value
	/// depending on configuration of the combiner.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// Because this combiner is stateless all public method are thread-safe.</summary>
	class GaFitnessSelectionCombiner : public Statistics::GaSelectionCombiner<GaFitness>
	{

	private:

		/// <summary>Pointer to fitness comparator and its parameters that is used for comparing fitness values to make selection.</summary>
		GaFitnessComparatorSetup* _fitnessComparator;

	public:

		/// <summary>This constructor initializes combiner with mode and fitness comaprator.</summary>
		/// <param name="selectionType">combiner's selection mode.</param>
		/// <param name="fitnessComparator">pointer to fitness comparator and its parameters that is used for comparing fitness values.</param>
		GaFitnessSelectionCombiner(Statistics::GaSelectionCombinerType selectionType,
			Fitness::GaFitnessComparatorSetup* fitnessComparator) : GaSelectionCombiner(selectionType),
			_fitnessComparator(fitnessComparator) { }

		/// <summary>This method is thread-safe.</summary>
		/// <returns>Returns pointer to fitness comparator setup used by this combiner.</returns>
		inline Fitness::GaFitnessComparatorSetup* GACALL GetFitnessComparator() { return _fitnessComparator; }

		/// <summary>This method is thread-safe.</summary>
		/// <returns>Returns pointer to fitness comparator setup used by this combiner.</returns>
		inline const Fitness::GaFitnessComparatorSetup* GACALL GetFitnessComparator() const { return _fitnessComparator; }

	protected:

		/// <summary>More details are given in specification of <see cref="GaDefaultSelectionCombiner::IsGreater" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual bool GACALL IsGreater(const GaFitness* value1,
			const GaFitness* value2) const { return _fitnessComparator->GetOperation()( *value1, *value2, _fitnessComparator->GetParameters() ) > 0; }

	};

} // Fitness

namespace Statistics
{

	/// <summary>This class is specialization of <see cref="GaValue" /> template class that handles chromosome fitness values (<see cref="Fitness::GaFitness" />).
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	template<>
	class GaValue<Fitness::GaFitness>
	{

	public:

		/// <summary>Type of stored value.</summary>
		typedef Fitness::GaFitness GaType;

		/// <summary>Constant type of stored value.</summary>
		typedef const Fitness::GaFitness GaConstType;

	private:

		/// <summary>Pointer to fitness handled by this object.</summary>
		Common::Memory::GaAutoPtr<GaType> _value;

		/// <summary>Inidicates whether the object contains value.</summary>
		bool _empty;

	public:

		/// <summary>This constructor initializes empty object.</summary>
		GaValue() : _value(NULL),
			_empty(true) { }

		/// <summary>This constructor stores fitness value to this object by copying provided fitness value.</summary>
		/// <param name="value">fitness value whose copy should be stored in this object.</param>
		GaValue(GaConstType& value) : _empty(false) { _value = value.Clone(); }

		/// <summary>This is copy constructor.</summary>
		/// <param name="rhs">source object whose value is copied.</param>
		GaValue(const GaValue<GaType>& rhs) : _empty(rhs._empty)
		{
			if( !rhs._value.IsNull() )
				_value = rhs._value->Clone();
		}

		/// <summary><c>Clear</c> method clears previously stored value.
		///
		/// This method is not thread-safe.</summary>
		inline void GACALL Clear() { _value = NULL; _empty = true; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to stored value or <c>NULL</c> if this object is empty.</returns>
		inline GaType& GACALL GetValue() { return *_value.GetRawPtr(); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to stored value or <c>NULL</c> if this object is empty.</returns>
		inline GaConstType& GACALL GetValue() const { return *_value.GetRawPtr(); }

		/// <summary><c>GetRelativeProgress</c> calculates relative progress of the stored value based on value which is specified by the parameters of this method.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="previous">reference to value on which the progress calculation is based.</param>
		/// <returns>Method returns calculated relative progress.</returns>
		inline float GACALL GetRelativeProgress(GaConstType& previous) const { return _value->GetRelativeProgress( previous ); }
		
		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if this object is empty and it contains no value.</returns>
		inline bool GACALL IsEmpty() const { return _empty; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if this object is not empty.</returns>
		inline bool GACALL HasValue() const { return !_empty; }

		/// <summary>This operator is not thread-safe.</summary>
		/// <returns>Opertor returns reference to stored value.</returns>
		inline GACALL operator GaType&() { return *_value; }

		/// <summary>This operator is not thread-safe.</summary>
		/// <returns>Opertor returns reference to stored value.</returns>
		inline GACALL operator const GaType&() const { return *_value; }

		/// <summary><c>operator =</c> operator copies value from another object.
		///
		/// This operator is not thread-safe.</summary>
		/// <param name="rhs">source object whose value is copied.</param>
		/// <returns>Method returns reference to this object.</returns>
		inline GaValue<GaType>& GACALL operator =(const GaValue<GaType>& rhs)
		{
			if( !rhs._empty )
			{
				if( _value )
					*_value = *rhs._value;
				else
					_value = rhs._value->Clone();
			}
			else if( _value )
				_value->Clear();

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
			if( _value )
				*_value = rhs;
			else
				_value = rhs.Clone();

			_empty = false;

			return *this;
		}

	};

} // Statistics

#endif // __GA_FITNESS_H__

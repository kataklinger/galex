
/*! \file Operation.h
	\brief This file declares interface for genetic operations and parameters. It also delcares and implements datatype which store operation-parameters setup. 
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_OPERATION_H__
#define __GA_OPERATION_H__

#include "SmartPtr.h"
#include "GlobalRandomGenerator.h"

namespace Common
{

	/// <summary>Interface for parameters of genetic operations.</summary>
	class GaParameters
	{

	public:

		/// <summary>Virtual destructor must be defined because this is base class.</summary>
		virtual ~GaParameters() { }

		/// <summary><c>Clone</c> method creates copy of parameters object. Caller is responsible for allocated memory.</summary>
		/// <returns>Method returns pointer to newly created object.</returns>
		virtual GaParameters* GACALL Clone() const = 0;

	};

	/// <summary><c>GaOperationProbabilityParams</c> is base class for parameters that defines probability of operation's occurrence.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaOperationProbabilityParams : public GaParameters
	{

	protected:

		/// <summary>Operation's occurrence probability in interval (0, 1).</summary>
		float _probability;

	public:

		/// <summary>This constructor initializes propability with user defined value.</summary>
		/// <param name="probability">occurrence probability. Value is in interval (0, 1).</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown when user tries to set probability to value that is not in (0, 1) range.</exception>
		GaOperationProbabilityParams(float probability) { SetProbability( probability ); }

		/// <summary>This constructor initializes propability to 1.</summary>
		GaOperationProbabilityParams() : _probability(1.0f) { }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns operation's occurrence probability.</returns>
		inline float GACALL GetProbability() const { return _probability; }

		/// <summary><c>SetProbability</c> method sets operation's occurrence probability.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="probability">new occurrence probability. Value is in interval (0, 1).
		///
		/// This method is not thread-safe.</param>
		/// <exception cref="GaArgumentOutOfRangeException" />Thrown when user tries to set probability to value that is not in (0, 1) range.</exception>
		inline void GACALL SetProbability(float probability)
		{
			GA_ARG_ASSERT( Exceptions::GaArgumentOutOfRangeException, probability >= 0 && probability <= 1, "probability", "Probability must be in range (0, 1).", "Common" );
			_probability = probability;
		}

		/// <summary><c>PerformeOperation</c> method check occurrence propability of the operation.</summary>
		/// <returns>Method returns <c>true</c> if the operation should be performed.</returns>
		inline bool GACALL PerformeOperation() const { return GaGlobalRandomBoolGenerator->Generate( _probability ); }

	};

	/// <summary><c>GaConfiguration</c> is interface for operation configurations.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaConfiguration
	{

	public:

		/// <summary>Virtual destructor must be defined because this is base class.</summary>
		virtual ~GaConfiguration() { }

		/// <summary><c>Clone</c> method creates copy of configuration object. Caller is responsible for allocated memory.</summary>
		/// <returns>Method returns pointer to newly created object.</returns>
		virtual GaConfiguration* GACALL Clone() const = 0;

	};

	/// <summary>Interface for genetic operations.</summary>
	class GaOperation
	{

	public:
		
		/// <summary><c>CreateParameters</c> method makes new object that stores parameters required by the operation. Caller is responsible for allocated memory.</summary>
		/// <returns>Method returns pointer to newly created object.</returns>
		virtual GaParameters* GACALL CreateParameters() const = 0;

		/// <summary><c>CreateConfiguration</c> method makes new object that stores configuration of operation. Caller is responsible for allocated memory.</summary>
		/// <returns>Method returns pointer to newly created object.</returns>
		virtual GaConfiguration* GACALL CreateConfiguration() const = 0;

	};

	/// <summary><c>GaOperationSetup</c> template class is used for storing genetic operations, their parameters and configurations.
	/// Instances of this class stores copies of specified operation parameters and configuration.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	/// <param name="OPERATION_TYPE">Type of operation.</param>
	/// <param name="PARAMETERS_TYPE">Type of operation's parameters.</param>
	/// <param name="CONFIGURATION_TYPE">Type of operation's configuration.</param>
	template <typename OPERATION_TYPE,
		typename PARAMETERS_TYPE,
		typename CONFIGURATION_TYPE = GaConfiguration>
	class GaOperationSetup
	{

	public:

		/// <summary>Type of operation.</summary>
		typedef OPERATION_TYPE GaOperationType;

		/// <summary>Type of operation parameters.</summary>
		typedef PARAMETERS_TYPE GaParametersType;

		/// <summary>Type of operation configuration.</summary>
		typedef CONFIGURATION_TYPE GaConfigurationType;

	private:

		/// <summary>Pointer to operation.</summary>
		const GaOperationType* _operation;

		/// <summary>Smart pointer to operation parameters.</summary>
		Memory::GaAutoPtr<GaParametersType> _parameters;

		/// <summary>Smart pointer to operation configuration.</summary>
		Memory::GaAutoPtr<GaConfigurationType> _configuration;

	public:

		/// <summary>This constructor initializes operation and its parameters and configuration.</summary>
		/// <param name="operation">pointer to operation.</param>
		/// <param name="parameters">pointer to parameters of the operation.</param>
		/// <param name="configuration">pointer to configuration of the operation.</param>
		GaOperationSetup(const GaOperationType* operation,
			const GaParametersType* parameters,
			const GaConfigurationType* configuration) { SetOperation( operation, parameters, configuration ); }

		/// <summary>This constructor initializes pointers to operation and it sparameters and configuration to <c>NULL</c>.</summary>
		GaOperationSetup() : _operation(NULL) { }

		/// <summary>Copy constructor.</summary>
		/// <param name="rhs">reference to setup object which is being copied.</param>
		GaOperationSetup(const GaOperationSetup& rhs) { SetOperation( rhs._operation, rhs._parameters.GetRawPtr(), rhs._configuration.GetRawPtr() ); }

		/// <summary>Destructor frees memory allocated by parameters and configuration objects.</summary>
		~GaOperationSetup() { }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to operation.</returns>
		inline const GaOperationType& GACALL GetOperation() const { return *_operation; }

		/// <summary><c>SetOperation</c> method stores pointer to operation and sets new parameters and configuration.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="operation">pointer to new operation.</param>
		/// <param name="parameters">pointer to parameters of the new operation.</param>
		/// <param name="configuration">pointer to configuration of the new operation.</param>
		inline void GACALL SetOperation(const GaOperationType* operation,
			const GaParametersType* parameters,
			const GaConfigurationType* configuration)
		{
			_operation = operation;
			SetParameters( parameters );
			SetConfiguration( configuration );
		}

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to operation parameters.</returns>
		inline GaParametersType& GACALL GetParameters() { return *_parameters; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to operation parameters.</returns>
		inline const GaParametersType& GACALL GetParameters() const { return *_parameters; }

		/// <summary><c>SetParameters</c> method sets new parameters that will be used by operation.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="parameters">pointer to new operation parameters.</param>
		inline void GACALL SetParameters(const GaParametersType* parameters) { _parameters = parameters ? (GaParametersType*)parameters->Clone() : NULL; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to operation configuration.</returns>
		inline GaConfigurationType& GACALL GetConfiguration() { return *_configuration; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to operation configuration.</returns>
		inline const GaConfigurationType& GACALL GetConfiguration() const { return *_configuration; }

		/// <summary><c>SetConfiguration</c> method sets new configuration of the operation.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="configuration">pointer to new configuration.</param>
		inline void GACALL SetConfiguration(const GaConfigurationType* configuration) { _configuration = configuration ? (GaConfigurationType*)configuration->Clone() : NULL; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if operation is specified.</returns>
		inline bool GACALL HasOperation() const { return _operation != NULL; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if parameters is specified.</returns>
		inline bool GACALL HasParameters() const { return !_parameters.IsNull(); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns <c>true</c> if configuration of operation is specified.</returns>
		inline bool GACALL HasConfiguration() const { return !_configuration.IsNull(); }

		/// <summary>This operator copies operation pointer and clones parameters from source setup.
		///
		/// This operator is not thread-safe.</summary>
		/// <param name="rhs">reference to source object.</param>
		/// <returns>Operator returns reference to <c>this</c> object.</returns>
		inline GaOperationSetup& GACALL operator =(const GaOperationSetup& rhs)
		{
			SetOperation( rhs._operation, rhs._parameters.GetRawPtr(), rhs._configuration.GetRawPtr() );
			return *this;
		}

	};

} // Common

#endif //__GA_OPERATION_H__

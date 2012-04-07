
/*! \file Exceptions.h
    \brief This file contains declaration of classes that represent most common exception.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#ifndef __GA_EXCEPTIONS_H__
#define __GA_EXCEPTIONS_H__

#include "Platform.h"
#include <string>

#if defined(GAL_PLATFORM_WIN)

#include <windows.h>

#endif

namespace Common
{
	/// <summary>Contains classes and datatypes for error and handling and defines basic exceptions.</summary>
	namespace Exceptions
	{

		#ifdef __GAL_DOCUMENTATION__

			/// <summary>Wraps compiler specific macro for getting function name at compile time.</summary>
			#define GA_FUNCTION_NAME _SYSTEM_OR_COMPILER_SPECIFIC_

			/// <summary>Wraps system dependant method that returns error code of the last operation.</summary>
			#define GA_LAST_SYS_ERROR _SYSTEM_OR_COMPILER_SPECIFIC_

			/// <summary>Wraps system dependant status value which indicates that system operation is successful.</summary>
			#define GA_NO_SYS_ERROR _SYSTEM_OR_COMPILER_SPECIFIC_

			/// <summary>Wraps system specific data type used to store error codes.</summary>
			typedef _SYSTEM_OR_COMPILER_SPECIFIC_ SysErrorCode;

		#else

		#if defined(GAL_PLATFORM_WIN)

			#define GA_LAST_SYS_ERROR	GetLastError()
			#define GA_NO_SYS_ERROR		0

			typedef DWORD SysErrorCode;

		#elif defined(GAL_PLATFORM_NIX)

			#define GA_LAST_SYS_ERROR	errno()
			#define GA_NO_SYS_ERROR		0

			typedef int SysErrorCode;

		#endif

		#if defined(GAL_SYNTAX_MSVC)

			#define GA_FUNCTION_NAME __FUNCSIG__

		#elif defined(GAL_SYNTAX_GNUC)

			#define GA_FUNCTION_NAME __PRETTY_FUNCTION__ 

		#elif defined(GAL_SYNTAX_SUNC)

			#define GA_FUNCTION_NAME __func__

		#elif defined(GAL_SYNTAX_BCC)

			#define GA_FUNCTION_NAME __FUNCTION__

		#endif

		#endif

		#define GA_CONVERT_TO_STR_HELPER(x) #x

		/// <summary>This macro stringize parameter <c>x</c>.</summary>
		/// <param name="x">parameter to convert to string literals.</param>
		#define GA_CONVERT_TO_STR(x) GA_CONVERT_TO_STR_HELPER(x)

		/// <summary><c>GA_LOCATION</c> macro makes string which contains function name, file name and line number at which the macro is placed.
		/// Depending on compiler, this macro can provide full function or method signature or it can provide only its name.</summary>
		#define GA_LOCATION GA_FUNCTION_NAME "; " __FILE__  " : " GA_CONVERT_TO_STR( __LINE__ )

		/// <summary><c>GA_ASSERT_C</c> macro test <c>CONDITION</c> expression and if it is not satisfied <c>EXCEPTION</c> is raised.
		/// This macro provides fine control to enable or disable assertation by specifying to which group of assertations it belongs in <c>CATEGORY</c> parameter.</summary>
		/// <param name="EXCEPTION">type of the excetion that is raised if condition is not satisfied.</param>
		/// <param name="CATEGORY">category to which assertation belongs. Used for fine tuned asserting.
		/// If the expression is evaluated to <c>false</c> or 0 asseration will be disabled.</param>
		/// <param name="CONDITION">condition which should be satisfied.</param>
		/// <param name="MESSAGE">message text of the raised exception.</param>
		/// <param name="SOURCE">source of the exception that is raised.</param>
		#define GA_ASSERT_C(EXCEPTION, CATEGORY, CONDITION, MESSAGE, SOURCE) \
			if( ( CATEGORY ) && !( CONDITION ) ) \
				throw EXCEPTION( MESSAGE, SOURCE, GA_LOCATION );

		/// <summary><c>GA_ASSERT</c> macro test <c>CONDITION</c> expression and if it is not satisfied <c>EXCEPTION</c> is raised.</summary>
		/// <param name="EXCEPTION">type of the excetion that is raised if condition is not satisfied.</param>
		/// <param name="CONDITION">condition which should be satisfied.</param>
		/// <param name="MESSAGE">message text of the raised exception.</param>
		/// <param name="SOURCE">source of the exception that is raised.</param>
		#define GA_ASSERT(EXCEPTION, CONDITION, MESSAGE, SOURCE) \
			if( !( CONDITION ) ) \
				throw EXCEPTION( MESSAGE, SOURCE, GA_LOCATION );

		/// <summary><c>GA_ARG_ASSERT</c> macro test <c>CONDITION</c> expression and if it is not satisfied <c>EXCEPTION</c> is raised.
		/// This macro provides fine control to enable or disable assertation by specifying to which group of assertations it belongs in <c>CATEGORY</c> parameter.
		/// <c>GA_ARG_ASSERT</c> macro should be used for argument checking.</summary>
		/// <param name="EXCEPTION">type of the excetion that is raised if condition is not satisfied.</param>
		/// <param name="CATEGORY">category to which assertation belongs. Used for fine tuned asserting.
		/// If the expression is evaluated to <c>false</c> or 0 asseration will be disabled.</param>
		/// <param name="CONDITION">condition which should be satisfied.</param>
		/// <param name="ARGUMENT">name of the argument that is being checked.</param>
		/// <param name="MESSAGE">message text of the raised exception.</param>
		/// <param name="SOURCE">source of the exception that is raised.</param>
		#define GA_ARG_ASSERT_C(EXCEPTION, CATEGORY, CONDITION, ARGUMENT, MESSAGE, SOURCE) \
			if( ( CATEGORY ) && !( CONDITION ) ) \
				throw EXCEPTION( ARGUMENT, MESSAGE, SOURCE, GA_LOCATION );

		/// <summary><c>GA_ARG_ASSERT</c> macro test <c>CONDITION</c> expression and if it is not satisfied <c>EXCEPTION</c> is raised.
		/// This macro should be used for argument checking.</summary>
		/// <param name="EXCEPTION">type of the excetion that is raised if condition is not satisfied.</param>
		/// <param name="CONDITION">condition which should be satisfied.</param>
		/// <param name="ARGUMENT">name of the argument that is being checked.</param>
		/// <param name="MESSAGE">message text of the raised exception.</param>
		/// <param name="SOURCE">source of the exception that is raised.</param>
		#define GA_ARG_ASSERT(EXCEPTION, CONDITION, ARGUMENT, MESSAGE, SOURCE) \
			if( !( CONDITION ) ) \
				throw EXCEPTION( ARGUMENT, MESSAGE, SOURCE, GA_LOCATION );

		/// <summary><c>GA_SYS_ASSERT_C</c> macro test <c>CONDITION</c> expression and if it is not satisfied raises system exception.
		/// It also stores error code obtained using <c>GA_LAST_SYS_ERROR</c> macro into exception object.
		/// This macro provides fine control to enable or disable assertation by specifying to which group of assertations it belongs in <c>CONDITION</c> parameter.</summary>
		/// <param name="CATEGORY">category to which assertation belongs. Used for fine tuned asserting.
		/// If the expression is evaluated to <c>false</c> or 0 asseration will be disabled.</param>
		/// <param name="CONDITION">condition which should be satisfied.</param>
		/// <param name="SOURCE">source of the exception that is raised.</param>
		#define GA_SYS_ASSERT_C(CATEGORY, CONDITION, SOURCE) \
			if( ( CATEGORY ) && !( CONDITION ) ) \
				throw Common::Exceptions::GaSystemException( GA_LAST_SYS_ERROR, SOURCE, GA_LOCATION );

		/// <summary><c>GA_SYS_ASSERT</c> macro test <c>CONDITION</c> expression and if it is not satisfied raises system exception.
		/// It also stores error code obtained using <c>GA_LAST_SYS_ERROR</c> macro into exception object.</summary>
		/// <param name="CONDITION">condition which should be satisfied.</param>
		/// <param name="SOURCE">source of the exception that is raised.</param>
		#define GA_SYS_ASSERT(CONDITION, SOURCE) \
			if( !( CONDITION ) ) \
				throw Common::Exceptions::GaSystemException( GA_LAST_SYS_ERROR, SOURCE, GA_LOCATION );

		/// <summary><c>GA_SYS_ASSERT_CODE_C</c> macro test <c>CODE</c> against <c>GA_NO_SYS_ERROR</c>, if they are not equal raises system exception and
		/// stores error code in exception object. This macro provides fine control to enable or disable assertation by specifying to which group of assertations it belongs
		/// in <c>CONDITION</c> parameter.</summary>
		/// <param name="CATEGORY">category to which assertation belongs. Used for fine tuned asserting.
		/// If the expression is evaluated to <c>false</c> or 0 asseration will be disabled.</param>
		/// <param name="CODE">error code of the operation.</param>
		/// <param name="SOURCE">source of the exception that is raised.</param>
		#define GA_SYS_ASSERT_CODE_C(CATEGORY, CODE, SOURCE) \
			if( ( CATEGORY ) && CODE != GA_NO_SYS_ERROR ) \
				throw Common::Exceptions::GaSystemException( CODE, SOURCE, GA_LOCATION );

		/// <summary><c>GA_SYS_ASSERT_CODE_C</c> macro test <c>CODE</c> against <c>GA_NO_SYS_ERROR</c>, if they are not equal raises system exception and
		/// stores error code in exception object.</summary>
		/// <param name="CODE">error code of the operation.</param>
		/// <param name="SOURCE">source of the exception that is raised.</param>
		#define GA_SYS_ASSERT_CODE(CODE, SOURCE) \
			if( CODE != GA_NO_SYS_ERROR ) \
				throw Common::Exceptions::GaSystemException( CODE, SOURCE, GA_LOCATION );

		/// <summary>This macro unconditionally throws exception of the specified type.</summary>
		/// <param name="EXCEPTION">type of the excetion that is raised.</param>
		/// <param name="MESSAGE">message text of the raised exception.</param>
		/// <param name="SOURCE">source of the exception that is raised.</param>
		#define GA_THROW(EXCEPTION, MESSAGE, SOURCE) throw EXCEPTION( MESSAGE, SOURCE, GA_LOCATION )

		/// <summary><c>GA_ARG_THROW</c> macro unconditionally throws exception of the specified type.
		/// This macro should be used is an argument caused exception.</summary>
		/// <param name="EXCEPTION">type of the excetion that is raised.</param>
		/// <param name="ARGUMENT">name of the argument that caused exception.</param>
		/// <param name="MESSAGE">message text of the raised exception.</param>
		/// <param name="SOURCE">source of the exception that is raised.</param>
		#define GA_ARG_THROW(EXCEPTION, ARGUMENT, MESSAGE, SOURCE) throw EXCEPTION( ARGUMENT, MESSAGE, SOURCE, GA_LOCATION )

		/// <summary><c>GA_SYS_THROW</c> macro unconditionally raises system exception and stores error code obtained using <c>GA_LAST_SYS_ERROR</c> macro into exception object.</summary>
		/// <param name="SOURCE">source of the exception that is raised.</param>
		#define GA_SYS_THROW(SOURCE) throw Common::Exceptions::GaSystemException( GA_LAST_SYS_ERROR, SOURCE, GA_LOCATION )

		/// <summary><c>GA_SYS_THROW_CODE</c> macro unconditionally raises system exception and stores specified error code into exception object.</summary>
		/// <param name="CODE">error code of the operation.</param>
		/// <param name="SOURCE">source of the exception that is raised.</param>
		#define GA_SYS_THROW_CODE(CODE, SOURCE) throw Common::Exceptions::GaSystemException( CODE, SOURCE, GA_LOCATION )

		/// <summary><c>GaException</c> is base class for all exceptions in the library. This class provides basic information about exception
		/// such as error message, source and location of the error.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaException
		{

		protected:
			
			// Indentation size of the inner exception
			static const int IDENT_SIZE = 2;

			// Number of places reserved for source of the exception
			static const int SOURCE_SIZE = 10;

			/// <summary>Exception message.</summary>
			const char* _message;

			/// <summary>Source of the exception.</summary>
			const char* _source;

			/// <summary>Location where the exception occurred.</summary>
			const char* _location;

			/// <summary>Pointer to excpetion that caused this excpetion.</summary>
			GaException* _innerException;

		public:

			/// <summary>This constructor initializes exception object with user defined message, source, location and inner exception.</summary>
			/// <param name="message">exception message.</param>
			/// <param name="source">source of the exception.</param>
			/// <param name="location">location where the exception has occurred. User can use <see cref="GA_LOCATION" /> macro to obtain exception location.</param>
			/// <param name="innerException">pointer to excpetion thta caused this excpetion.</param>
			GaException(const char* message,
				const char* source = NULL,
				const char* location = NULL,
				const GaException* innerException = NULL) : _message(message),
				_source(source),
				_location(location),
				_innerException(NULL) { SetInnerException( innerException ); }

			/// <summary>Copy constructor.</summary>
			/// <param name="rhs">reference to exception object that should be copied.</param>
			GaException(const GaException& rhs) : _message(rhs._message),
				_source(rhs._source),
				_location(rhs._location),
				_innerException(NULL) { SetInnerException( rhs._innerException ); }

			/// <summary>The destructor frees inner exception object if it exists.</summary>
			virtual ~GaException()
			{
				if( _innerException )
					delete _innerException;
			}

			/// <summary><c>Clone</c> method creates copy of exception object.
			///
			/// This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to newly created copy of exception object.</returns>
			virtual GaException* GACALL Clone() const { return new GaException( *this ); }

			/// <summary><c>Throw</c> method throws copy of this exception object (polymorphic throw operation).
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Throw() const { throw *this; }

			/// <summary><c>GetFormatedMessage</c> method stores formatted message to provided string object. The method stores all available information about eexception
			/// including formatted messages of all inner exceptions.
			///
			/// This method is thread-safe.</summary>
			/// <param name="outStr">string object to which message is stored.</param>
			/// <param name="level">depth of inner exception.</param>
			GAL_API
			void GACALL GetFormatedMessage(std::string& outStr,
				int level = 0) const;

			/// <summary><c>SetMessage</c> method sets exception message.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="message">string which contains exception message.</param>
			inline void GACALL SetMessage(const char* message) { _message = message; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to string which contains exception message.</returns>
			inline const char* GACALL GetMessage() const { return _message; }

			/// <summary><c>SetSource</c> method sets source of the exception.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="source">string which contains source of the exception.</param>
			inline void GACALL SetSource(const char* source) { _source = source; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to string which contains source of the exception.</returns>
			inline const char* GACALL GetSource() const { return _source; }

			/// <summary><c>SetLocation</c> method sets location where the exception has occurred.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="location">string which contaions location of the exception.
			/// User can use <see cref="GA_LOCATION" /> macro to obtain exception location.</param>
			inline void GACALL SetLocation(const char* location) { _location = location; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to string which contains location of the exception.</returns>
			inline const char* GACALL GetLocation() const { return _location; }

			/// <summary><c>SetInnerException</c> method sets inner exception.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="exception">pointer to the inner exception.</param>
			inline void GACALL SetInnerException(const GaException* exception)
			{
				if( _innerException )
					delete _innerException;

				if( exception )
					_innerException = exception->Clone();
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to inner exception that caused this exception. If there is no inner exception,
			/// this method returns <c>NULL</c>.</returns>
			inline GaException* GACALL GetInnerException() { return _innerException; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to inner exception that caused this exception. If there is no inner exception,
			/// this method returns <c>NULL</c>.</returns>
			inline const GaException* GACALL GetInnerException() const { return _innerException; }

			/// <summary><c>operator =</c> copies data from source exception object.
			//
			/// This operator is not thread-safe.</summary>
			/// <param name="rhs">reference to source exception object.</param>
			/// <returns>Operator returns reference to this object.</returns>
			GaException& operator =(const GaException& rhs)
			{
				_message = rhs._message;
				_source = rhs._source;
				_location = rhs._location;
				SetInnerException( rhs._innerException );
			}
			
		protected:

			/// <summary><c>AddMessageLine</c> method appends single line (with indentation) to formatted exception message.</summary>
			/// <param name="outStr">string object to which the line is appended.</param>
			/// <param name="inStr">string object which contains text of the line.</param>
			/// <param name="level">depth of inner exception.</param>
			/// <param name="firstLine">this parameter should be set to <c>true</c> if </param>
			GAL_API
			void GACALL AddMessageLine(std::string& outStr,
				const std::string& inStr,
				int level,
				bool firstLine = false) const;

			/// <summary><c>AddAdditionalInformation</c> method appends addidional information to formatted exception message which depends on type of the exception.
			/// This method should be overridden by inherited classes to provide specific informations for the exception type.</summary>
			/// <param name="outStr">string object to which additiona information is appended.</param>
			/// <param name="level">depth of inner exception.</param>
			virtual void GACALL AddAdditionalInformation(std::string& outStr,
				int level) const { }

		};

		/// <summary>This macro injects standard constructor for exceptions derived from <see cref="GaException" /> class.</summary>
		/// <param name="CLASS">name of the class to which the constructors are injected.</param>
		/// <param name="BASE_CLASS">name of base class.</param>
		#define GA_STD_EXCEPTION_CTORS(CLASS, BASE_CLASS) \
			public: \
				CLASS(const char* message, \
					const char* source = NULL, \
					const char* location = NULL) : BASE_CLASS(message, source, location) { } \

		/// <summary>This macro injects virtual copy constructor (<see cref="GaException::Clone" /> method) and
		/// polymorphic throw operation (<see cref="GaException::Throw" /> method).</summary>
		/// <param name="CLASS">name of the class to which the methods are injected.</param>
		#define GA_STD_EXCEPTION_METHODS(CLASS) \
			public: \
				virtual GaException* GACALL Clone() const { return new CLASS( *this ); } \
				virtual void GACALL Throw() const { throw *this; } \

		/// <summary><c>GaSystemException</c> class is used for exceptions caused by errors during interaction of the library with the operating systems
		/// or environment. This class wraps error code returned by the failed system call and automatically retrive error message.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaSystemException : public GaException
		{

		protected:

			/// <summary>Error code returned by system call.</summary>
			SysErrorCode _errorCode;

			/// <summary>Memory buffer that stores message system message.</summary>
			char* _systemMessage;

		public:
			
			/// <summary>This constructor initializes exception object with error code, source, location of the error and it loads error message.</summary>
			/// <param name="errorCode">error code returned by system call.</param>
			/// <param name="source">source of the exception.</param>
			/// <param name="location">location where the exception has occurred.
			/// User can use <see cref="GA_LOCATION" /> macro to obtain exception location.</param>
			GaSystemException(SysErrorCode errorCode,
				const char* source = NULL,
				const char* location = NULL) : GaException(NULL, source, location),
				_errorCode(errorCode),
				_systemMessage(NULL) { LoadErrorMessage(); }

			/// <summary>Frees buffer that stores system message.</summary>
			virtual ~GaSystemException()
			{
				#if defined(GAL_PLATFORM_WIN)

				if( _systemMessage )
					::LocalFree( _systemMessage );

				#endif
			}

			/// <summary>This constructor initializes exception object with error code, location of the error and it loads error message.</summary>
			/// <param name="errorCode">error code returned by system call.</param>
			/// <param name="location">location where the exception has occurred.
			/// User can use <see cref="GA_LOCATION" /> macro to obtain exception location.</param>
			GaSystemException(SysErrorCode errorCode,
				const char* location) : GaException(NULL, "System", location, NULL),
				_errorCode(errorCode) { LoadErrorMessage(); }

			/// <summary>This constructor initializes exception object with error code and loads error message.</summary>
			/// <param name="errorCode">error code returned by system call.</param>
			GaSystemException(SysErrorCode errorCode) : GaException(NULL, "System", NULL, NULL),
				_errorCode(errorCode) { LoadErrorMessage(); }

			/// <summary>This constructor initializes exception object with no error code.</summary>
			GaSystemException() : GaException(NULL),
				_errorCode(0) { }

			/// <summary>Copy constructor.</summary>
			/// <param name="rhs">reference to exception object that should be copied.</param>
			GaSystemException(const GaSystemException& rhs) : GaException(rhs),
				_errorCode(rhs._errorCode) { }

			GA_STD_EXCEPTION_METHODS( GaSystemException )

			/// <summary><c>SetErrorCode</c> method sets error code returned by system call and retrives error message.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="errorCode">error code returned by system call.</param>
			inline void GACALL SetErrorCode(SysErrorCode errorCode)
			{ 
				_errorCode = errorCode;

				LoadErrorMessage();
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns></returns>
			inline SysErrorCode GACALL GetErrorCode() const { return _errorCode; }

			/// <summary><c>operator =</c> copies data from source exception object.
			//
			/// This operator is not thread-safe.</summary>
			/// <param name="rhs">reference to source exception object.</param>
			/// <returns>Operator returns reference to this object.</returns>
			inline GaSystemException& operator =(const GaSystemException& rhs)
			{
				_errorCode = rhs._errorCode;
				GaException::operator =( rhs );
			}

		protected:

			/// <summary>This method appends error code returned by the system to the formatted message.
			/// More information are given in the specification of the <see cref="GaException::AddAdditionalInformation"/> method.</summary>
			GAL_API
			virtual void GACALL AddAdditionalInformation(std::string& outStr,
				int level) const;

			/// <summary>This method uses provided error code to load error message.</summary>
			GAL_API
			void GACALL LoadErrorMessage();

		};

		/// <summary><c>GaArgumentException</c> is base class for exception caused by invalid arguments passed to methods or functions.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaArgumentException : public GaException
		{

		protected:

			/// <summary>Name of the argument which caused the exception.</summary>
			const char* _argumentName;

		public:

			/// <summary>This constructor initializes exception object with user defined argument name, message, source and exception location.</summary>
			/// <param name="argumentName">name of the argument which caused the exception.</param>
			/// <param name="message">exception message.</param>
			/// <param name="source">source of the exception.</param>
			/// <param name="location">location where the exception has occurred. User can use <see cref="GA_LOCATION" /> macro to obtain exception location.</param>
			GaArgumentException(const char* argumentName,
				const char* message,
				const char* source = NULL,
				const char* location = NULL) : GaException(message, source, location),
				_argumentName(argumentName) { }

			/// <summary>Copy constructor.</summary>
			/// <param name="rhs">reference to exception object that should be copied.</param>
			GaArgumentException(const GaArgumentException& rhs) : GaException(rhs),
				_argumentName(rhs._argumentName) { }

			GA_STD_EXCEPTION_METHODS( GaArgumentException )

			/// <summary><c>SetArgumentName</c> method sets name of the argument which caused the exception.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="argumentName"></param>
			inline void GACALL SetArgumentName(const char* argumentName) { _argumentName = argumentName; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to string which contains name of the argument which caused the exception.</returns>
			inline const char* GACALL GetArgumentName() const { return _argumentName; }

			/// <summary><c>operator =</c> copies data from source exception object.
			//
			/// This operator is not thread-safe.</summary>
			/// <param name="rhs">reference to source exception object.</param>
			/// <returns>Operator returns reference to this object.</returns>
			inline GaArgumentException& operator =(const GaArgumentException& rhs)
			{
				_argumentName = rhs._argumentName;
				GaException::operator =( rhs );
			}

		protected:

			/// <summary>This method appends argument name which caused exception the formatted message.
			/// More information are given in the specification of the <see cref="GaException::AddAdditionalInformation"/> method.</summary>
			GAL_API
			virtual void GACALL AddAdditionalInformation(std::string& outStr,
				int level) const;

		};

		/// <summary>This macro injects standard constructor for exceptions derived from <see cref="GaArgumentException" /> class.</summary>
		/// <param name="CLASS">name of the class to which the constructors are injected.</param>
		/// <param name="BASE_CLASS">name of base class.</param>
		#define GA_ARG_EXCEPTION_CTORS(CLASS, BASE_CLASS) \
			public: \
				CLASS(const char* argumentName, \
					const char* message, \
					const char* source = NULL, \
					const char* location = NULL) : BASE_CLASS(argumentName, message, source, location) { } \
				CLASS(const CLASS& rhs) : BASE_CLASS(rhs) { }

		/// <summary><c>GaNullArgumentException</c> class represnts exceptions caused by <c>NULL</c> arguments passed to methods or functions.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaNullArgumentException : public GaArgumentException
		{
			GA_ARG_EXCEPTION_CTORS( GaNullArgumentException, GaArgumentException )
			GA_STD_EXCEPTION_METHODS( GaNullArgumentException )
		};

		/// <summary><c>GaArgumentOutOfRangeException</c> class represents exceptions caused by arguments passed to methods or functions that are out of the expected range.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaArgumentOutOfRangeException : public GaArgumentException
		{
			GA_ARG_EXCEPTION_CTORS( GaArgumentOutOfRangeException, GaArgumentException )
			GA_STD_EXCEPTION_METHODS( GaArgumentOutOfRangeException )
		};

		/// <summary><c>GaInvalidOperationException</c> class that represents exception caused by peroforming operation wich is not permitted.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaInvalidOperationException : public GaException
		{
			GA_STD_EXCEPTION_CTORS( GaInvalidOperationException, GaException )
			GA_STD_EXCEPTION_METHODS( GaInvalidOperationException )
		};

		/// <summary><c>GaExceptionHandler</c> class is interface for generic exception handlers.</summary>
		class GaExceptionHandler
		{

		public:

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaExceptionHandler() { }

			/// <summary><c>OnException</c> method should handles caught exceptions.</summary>
			/// <param name="exception">reference to caught exception that should be handled.</param>
			/// <returns>Method returns <c>true</c> if it handled provided exception.</returns>
			virtual bool GACALL OnException(const GaException& exception) = 0;

		};

	} // Exceptions
} // Common

#endif // __GA_EXCEPTIONS_H__

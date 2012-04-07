
/*! \file Exceptions.cpp
    \brief This file contains implemenation of classes that represent most common exception.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#include "Exceptions.h"

namespace Common
{
	namespace Exceptions
	{

		void GaException::GetFormatedMessage(std::string& outStr,
			int level/* = 0*/) const
		{
			// add exception message to formated output
			AddMessageLine( outStr, _message, level, true );

			// include additional information about exception dependiong on type of the exception
			AddAdditionalInformation( outStr, level );

			// add location where the exception occurred
			AddMessageLine( outStr, _location, level );

			// add inner exception's information
			if( _innerException )
				_innerException->GetFormatedMessage( outStr, level + 1 );
		}

		// Adds one line of text to formatted output message
		void GaException::AddMessageLine(std::string& outStr,
			const std::string& inStr,
			int level,
			bool firstLine/* = false*/) const 
		{
			// append indentation
			outStr.insert( outStr.size(), level * IDENT_SIZE, ' ' );

			int sourceLen = (int)strlen( _source );
			if( firstLine )
			{
				// add source for the first line of the message
				outStr += "[";
				outStr += _source;
				outStr.insert( outStr.size(), sourceLen <= SOURCE_SIZE ? SOURCE_SIZE - sourceLen : 0, ' ' );
				outStr += "] : ";
			}
			else
			{
				// align line text with the first line of the message
				outStr.insert( outStr.size(), ( sourceLen <= SOURCE_SIZE ? SOURCE_SIZE : sourceLen ) + 2, ' ' );
				outStr += " : ";
			}

			// append line text
			outStr += inStr;
			outStr += "\n";
		}

		// Appends error code returned by the operating system to formatted output message
		void GaSystemException::AddAdditionalInformation(std::string& outStr,
			int level) const
		{
			char buffer[ 15 ];

			// convert error code to ascii

			#if defined(GAL_PLATFORM_WIN)

			_itoa( (int)_errorCode, buffer, 10 );

			#else

			sprintf( buffer, "%d", (int)_errorCode );

			#endif

			std::string str = "Error code = ";
			str += buffer;

			// append error code
			AddMessageLine( outStr, str, level );
		}

		// Loads error message using error code provided by the operating system
		void GaSystemException::LoadErrorMessage()
		{
			#if defined(GAL_PLATFORM_WIN)

			// free buffer if it was in use
			if( _systemMessage )
			{
				::LocalFree( _systemMessage );
				_systemMessage = NULL;
			}

			// get message for specified error code
			DWORD msgLen = ::FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, _errorCode, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPSTR)&_systemMessage, 1, NULL );

			_message = _systemMessage;

			#endif
		}

		// Appends argument name which caused exception to formatted output message
		void GaArgumentException::AddAdditionalInformation(std::string& outStr,
			int level) const
		{
			std::string str = "Argument name = ";
			str += _argumentName;

			AddMessageLine( outStr, str, level ); 
		}

	} // Exceptions
} // Common

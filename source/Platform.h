
/*! \file Platform.h
	\brief This file detects used compiler and platform and defines macros that customize compilation.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#ifdef __GAL_DOCUMENTATION__

	/// <summary>This macro is defined when Intel C++ compiler is detected. It is used to handle compiler specific syntax.</summary>
	#define GAL_SYNTAX_INTL

	/// <summary>This macro is defined when Microsoft C/C++ compiler is detected. It is used to handle compiler specific syntax.</summary>
	#define GAL_SYNTAX_MSVC

	/// <summary>This macro is defined when GNU G++ compiler is detected. It is used to handle compiler specific syntax.</summary>
	#define GAL_SYNTAX_GNUC

	/// <summary>This macro is defined when Sun Studio C++ compiler is detected. It is used to handle compiler specific syntax.</summary>
	#define GAL_SYNTAX_SUNC

	/// <summary>This macro is defined when Borland C++ compiler is detected. It is used to handle compiler specific syntax.</summary>
	#define GAL_SYNTAX_BCC

	/// <summary>This macro is defined when Windows operating system is detected. It is used to handle operating system specific calls.</summary>
	#define GAL_PLATFORM_WIN

	/// <summary>This macro is defined when Unix-like operating system (Linux, Mac OS X, BSD) is detected.
	/// It is used to handle operating system specific calls.</summary>
	#define GAL_PLATFORM_NIX

	/// <summary>This macro is defined when Mac OS X operating system is detected. It is used to handle operating system specific calls.</summary>
	#define GAL_PLATFORM_MACOS

	/// <summary>This macro is defined when Solaris operating system is detected. It is used to handle operating system specific calls.</summary>
	#define GAL_PLATFORM_SOL

	/// <summary>This macro is defined when Microsoft STL extensions are detected. It is used to handle different implementations of STL extensions.</summary>
	#define GAL_STL_EXT_MSVC

	/// <summary>This macro is defined when GNU STL extensions are detected. It is used to handle different implementations of STL extensions.</summary>
	#define GAL_STL_EXT_GNUC

	/// <summary>This macro is defined when STLport (Digital Mars and Sun C++) is detected. It is used to handle different implementations of STL extensions.</summary>
	#define GAL_STL_EXT_STLPORT

	/// <summary>Defines calling convention used by run-time environment and operating system API.
	/// On all supported systems and compilers this macro is set to <c>cdecl</c> except for Sun C++ compiler which ignores this macro.</summary>
	#define APICALL

	/// <summary>Defines calling convention used by GAL.
	/// For all supported systems and compilers except Borland C++ on Windows and Intel C++ compiler on Linux and Mac OS this macro is set to <c>fastcall</c>.
	/// Borland C++ on Windows and Intel C++ compilers on Linux and Mac OS this macro is set to <c>cdecl</c>. Sun C++ compiler ignores this macro.</summary>
	#define GACALL

	/// <summary>This macro marks function/methods or global variable as exported symbol when building library. When it used from another
	/// project which use the library it marks function/method or global variable as imported symbol. It should only be specified in
	/// function/method declaration, not in definition. This macro is set to nothing on all systems except on Windows.</summary>
	#define GAL_API

	/// <summary>This macro is defined when the library is compiled. This macro is not defined for projects that imports the library.</summary>
	#define GAL_API_EXPORTING

#else

#if defined(__INTEL_COMPILER)

	#define GAL_SYNTAX_INTL

	#if defined(_MSC_VER)

		#define GAL_STL_EXT_MSVC
		#define GAL_SYNTAX_MSVC
		#define GAL_PLATFORM_WIN

	#elif defined(__GNUC__)

		#define GAL_STL_EXT_GNUC
		#define GAL_SYNTAX_GNUC
		#define GAL_PLATFORM_NIX
		
		#if defined(__APPLE__)

			#define GAL_PLATFORM_MACOS

		#endif

	#endif

#elif defined(_MSC_VER)

	#define GAL_STL_EXT_MSVC
	#define GAL_SYNTAX_MSVC
	#define GAL_PLATFORM_WIN

#elif defined(__CYGWIN__)

	#define GAL_STL_EXT_GNUC
	#define GAL_SYNTAX_GNUC
	#define GAL_PLATFORM_WIN

#elif defined(__MINGW32__)

	#define GAL_STL_EXT_GNUC
	#define GAL_SYNTAX_GNUC
	#define GAL_PLATFORM_WIN

#elif defined(__DMC__)

	#define GAL_STL_EXT_STLPORT
	#define GAL_STL_EXT_GNUC
	#define GAL_SYNTAX_MSVC
	#define GAL_PLATFORM_WIN

#elif defined(__BORLANDC__)

	#define GAL_STL_EXT_STLPORT
	#define GAL_SYNTAX_BCC
	#define GAL_PLATFORM_WIN

#elif defined(__GNUC__)

	#define GAL_STL_EXT_GNUC
	#define GAL_SYNTAX_GNUC
	#define GAL_PLATFORM_NIX
	
	#if defined(__APPLE__)

		#define GAL_PLATFORM_MACOS

	#endif

	#if defined(__sun)

		#define GAL_PLATFORM_SOL

	#endif

#elif defined(__SUNPRO_CC)

	#define GAL_STL_EXT_STLPORT
	#define GAL_SYNTAX_SUNC
	#define GAL_PLATFORM_NIX

	#if defined(__sun)

		#define GAL_PLATFORM_SOL

	#endif

#endif

#if defined(GAL_PLATFORM_WIN)

	#define APICALL	__cdecl

	#if !defined(GAL_SYNTAX_BCC)

		#define GACALL	__fastcall

	#else

		#define GACALL __cdecl

	#endif

#elif defined(GAL_PLATFORM_NIX)

	#if defined (GAL_SYNTAX_SUNC)

		#define APICALL
		#define GACALL

	#else

		#define APICALL __attribute__((cdecl))

		#if defined(GAL_SYNTAX_INTL)

			#define GACALL __attribute__((cdecl))

		#else

			#define GACALL __attribute__((fastcall))

		#endif

	#endif

#endif

#ifdef GENETICLIBRARY_EXPORTS
	
	#if defined(GAL_PLATFORM_WIN)

		#if defined(GENETICLIBRARY_STATIC)

			#define GAL_API

		#else

			#define GAL_API __declspec(dllexport)

		#endif

	#elif defined(GAL_PLATFORM_NIX)
	
		#define GAL_API

	#endif

	#define GAL_API_EXPORTING

#else

	#if defined(GAL_PLATFORM_WIN)

		#if defined(GENETICLIBRARY_STATIC)

			#define GAL_API

		#else

			#define GAL_API __declspec(dllimport)

		#endif

	#elif defined(GAL_PLATFORM_NIX)	

		#define GAL_API

	#endif

#endif

#endif

#ifndef NULL

	#define NULL 0

#endif

/// <summary>Defines data type to which the pointer should be converted for performing integer operation.</summary>
/// <param name="PTR_SIZE">size of pointer in bytes.</param>
template<int PTR_SIZE> struct GaIntPtr { };

#ifndef __GAL_DOCUMENTATION__

	template<> struct GaIntPtr<4> { typedef unsigned long GaType; };

	template<> struct GaIntPtr<8> { typedef unsigned long long GaType; };

#endif

#endif // __PLATFORM_H__

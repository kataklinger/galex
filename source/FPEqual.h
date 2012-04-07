
/*! \file FPEqual.h
	\brief This file contains declaration and implementation of functions that check floating point numbers for equalty.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __FPEQUAL_H__
#define __FPEQUAL_H__

#include <stdlib.h>
#include "Platform.h"

/// <summary>This macro interprets <c>float</c> as <c>int</c> for easier bitwies access.</summary>
#define FLOAT_AS_INT(x) ( ( *(int*)&( x ) ) )

/// <summary>This macro interprets <c>double</c> as <c>long long</c> for easier bitwies access.</summary>
#define DOUBLE_AS_LONGLONG(x) ( ( *(long long*)&( x ) ) )

/// <summary>Extracts sign from single precision floating point number.</summary>
#define FLOAT_SIGN 0x80000000UL

/// <summary>Extracts sign from double precision floating point number.</summary>
#define DOUBLE_SIGN 0x8000000000000000ULL

/// <summary>Contains common classes, functions, datatypes and objects used by the library.</summary>
namespace Common
{

	/// <summary>This method compares two single precision floating point numbers for equality with defined level of tolerance.</summary>
	/// <param name="x">the first number</param>
	/// <param name="y">the second number</param>
	/// <param name="ulpTolerance">maximum difference between these two numbers  in ULPs that is tolerated for almost equality. Default value is 10.</param>
	/// <returns>Returns <c>true</c> if the two values are almost equal.</returns>
	inline bool GACALL FloatsEqual(float x,
		float y,
		int ulpTolerance = 10)
	{
		// to avoid equality of small numbers with opposite signs
		if( ( FLOAT_AS_INT( x ) & FLOAT_SIGN ) != ( FLOAT_AS_INT( y ) & FLOAT_SIGN ) )
			return x == y;

		// make x lexicographically ordered as a twos-complement
		if( FLOAT_AS_INT( x ) < 0 )
			FLOAT_AS_INT( x ) = 0x80000000 - FLOAT_AS_INT( x );

		// make y lexicographically ordered as a twos-complement
		if( FLOAT_AS_INT( y ) < 0 )
			FLOAT_AS_INT( y ) = 0x80000000 - FLOAT_AS_INT( y );

		// comparing x and y
		return abs( FLOAT_AS_INT( x ) - FLOAT_AS_INT( y ) ) <= ulpTolerance;
	}

	/// <summary>This method compares two double precision floating point numbers for equality with defined level of tolerance.</summary>
	/// <param name="x">the first number</param>
	/// <param name="y">the second number</param>
	/// <param name="ulpTolerance">maximum difference between these two numbers  in ULPs that is tolerated for almost equality. Default value is 10.</param>
	/// <returns>Returns <c>true</c> if the two values are almost equal.</returns>
	inline bool GACALL DoublesEqual(double x,
		double y,
		int ulpTolerance = 10)
	{
		// to avoid equality of small numbers with opposite signs
		if ( ( DOUBLE_AS_LONGLONG( x ) & DOUBLE_SIGN ) != ( DOUBLE_AS_LONGLONG( y ) & DOUBLE_SIGN ) )
			return x == y;

		// make x lexicographically ordered as a twos-complement
		if( DOUBLE_AS_LONGLONG( x ) < 0 )
			DOUBLE_AS_LONGLONG( x ) = 0x80000000 - DOUBLE_AS_LONGLONG( x );

		// make y lexicographically ordered as a twos-complement
		if( DOUBLE_AS_LONGLONG( y ) < 0 )
			DOUBLE_AS_LONGLONG( y ) = 0x80000000 - DOUBLE_AS_LONGLONG( y );

		// comparing x and y
		long long r = DOUBLE_AS_LONGLONG( x ) - DOUBLE_AS_LONGLONG( y );
		return ( r < 0 ? -r : r ) <= ulpTolerance;
	}

} // Common


#endif // __FPEQUAL_H__

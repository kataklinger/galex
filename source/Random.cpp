
/*! \file Random.cpp
    \brief This file contains implementation of random number generators.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#include "Random.h"

#if defined (GAL_SYNTAX_GNUC) || defined (GAL_SYNTAX_SUNC)

inline unsigned int _lrotl(unsigned int x, int r) { return ( x<< r ) | ( x >> ( sizeof( x ) * 8 - 1 ) ); }

#endif

namespace Common
{
	namespace Random
	{

		// Global instance of random generator
		GaRandomGenerator GaRandomGenerator::_instance;

		// Generator of random bits
		unsigned int GaRandomGenerator::Generate()
		{
			GaState newState, oldState;
			do
			{
				oldState = newState = _currentState;

				// calculate new state of the generator
				newState._z = 0x9069 * ( newState._z & 0xffff ) + ( newState._z >> 16 );
				newState._w = 0x4650 * ( newState._w & 0xffff ) + ( newState._w >> 16 );

				// assure that another thread have not modified the state
			} while( !Threading::GaCmpXchg2<GaState>::Op( &_currentState, &oldState, newState ) );

			// generate new random value
			//return ( newState._w << 16 ) + (unsigned short)newState._z;
			return ( newState._z << 16 ) + newState._w;
		}

		// Generate random single precision floating point number in interval 0..1
		float GaRandomGenerator::GenerateFloat()
		{
			GaUnsignedIntToFloat converter;

			// generate random bits
			converter.bits = Generate();

			// covert to double
			converter.bits = ( converter.bits & 0x007FFFFF ) | 0x3F800000;

			return converter.number - 1;
		}

		// Generate random double precision floating point number in interval 0..1
		double GaRandomGenerator::GenerateDouble()
		{
			GaUnsignedIntToDouble converter;

			// generate random bits
			converter.bits[ 0 ] = Generate();
			converter.bits[ 1 ] = Generate();

			// covert to double
			converter.bits[ _littleEndian ] = ( converter.bits[ 0 ] & 0x000FFFFF ) | 0x3FF00000;

			return converter.number - 1;
		}

		// Initialization of random generator
		void GaRandomGenerator::Initalization(unsigned int seed1,
			unsigned int seed2)
		{
			// initialize seed
			_currentState._w = seed1 ? seed1 : 0x1f123bb5;
			_currentState._z = seed2 ? seed2 : 0x159a55e5;

			// detecting big or little endian
			GaUnsignedIntToDouble converter;
			converter.number = 1;
			_littleEndian = converter.bits[ 1 ] == 0x3FF00000;
		}

	} // Random
} // Common


/*! \file GlobalRandomGenerator.cpp
    \brief This file defines global random number generator objects.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#include "Random.h"

GAL_API Common::Random::GaRandom<int>* GaGlobalRandomIntegerGenerator = 0;
GAL_API Common::Random::GaRandom<long long>* GaGlobalRandomLongIntegerGenerator = 0;
GAL_API Common::Random::GaRandom<float>* GaGlobalRandomFloatGenerator = 0;
GAL_API Common::Random::GaRandom<double>* GaGlobalRandomDoubleGenerator = 0;
GAL_API Common::Random::GaRandom<bool>* GaGlobalRandomBoolGenerator = 0;

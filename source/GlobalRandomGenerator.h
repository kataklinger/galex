
/*! \file GlobalRandomGenerator.h
    \brief This file contains declaration of global random number generator objects.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#include "Random.h"

#if defined(GAL_API_EXPORTING)

/// <summary>Global object used by GAL to generate random integer numbers. It cannot be used prior calling <see cref="GaInitialize" />
/// and after calling <see cref="GaFinalize" />. For more details see specialization of <see cref="GaRandom" /> template class for <c>int</c> type.</summary>
extern Common::Random::GaRandom<int>* GaGlobalRandomIntegerGenerator;

/// <summary>Global object used by GAL to generate random large integer numbers (64-bit). It cannot be used prior calling <see cref="GaInitialize" />
/// and after calling <see cref="GaFinalize" />. For more details see specialization of <see cref="GaRandom" /> template class for <c>int</c> type.</summary>
extern Common::Random::GaRandom<long long>* GaGlobalRandomLongIntegerGenerator;

/// <summary>Global object used by GAL to generate random single precision floating point numbers. It cannot be used prior calling <see cref="GaInitialize" />
/// and after calling <see cref="GaFinalize" />. For more details see specialization of <see cref="GaRandom" /> template class for <c>float</c> type.</summary>
extern Common::Random::GaRandom<float>* GaGlobalRandomFloatGenerator;

/// <summary>Global object used by GAL to generate random double precision floating point numbers. It cannot be used prior calling <see cref="GaInitialize" />
/// and after calling <see cref="GaFinalize" />. For more details see specialization of <see cref="GaRandom" /> template class for <c>double</c> type.</summary>
extern Common::Random::GaRandom<double>* GaGlobalRandomDoubleGenerator;

/// <summary>Global object used by GAL to generate random Boolean values. It cannot be used prior calling <see cref="GaInitialize" />
/// and after calling <see cref="GaFinalize" />. For more details see specialization of <see cref="GaRandom" /> template class for <c>bool</c> type.</summary>
extern Common::Random::GaRandom<bool>* GaGlobalRandomBoolGenerator;

#else

#if defined(GAL_PLATFORM_WIN)

GAL_API Common::Random::GaRandom<int>* GaGlobalRandomIntegerGenerator;
GAL_API Common::Random::GaRandom<long long>* GaGlobalRandomLongIntegerGenerator;
GAL_API Common::Random::GaRandom<float>* GaGlobalRandomFloatGenerator;
GAL_API Common::Random::GaRandom<double>* GaGlobalRandomDoubleGenerator;
GAL_API Common::Random::GaRandom<bool>* GaGlobalRandomBoolGenerator;

#elif defined(GAL_PLATFORM_NIX)

extern Common::Random::GaRandom<int>* GaGlobalRandomIntegerGenerator;
extern Common::Random::GaRandom<long long>* GaGlobalRandomLongIntegerGenerator;
extern Common::Random::GaRandom<float>* GaGlobalRandomFloatGenerator;
extern Common::Random::GaRandom<double>* GaGlobalRandomDoubleGenerator;
extern Common::Random::GaRandom<bool>* GaGlobalRandomBoolGenerator;

#endif

#endif

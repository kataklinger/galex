
/*! \file Initialization.cpp
    \brief This file contains declaration of functions for initialization and finalization of the library.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#ifndef __INITIALIZATION_H__
#define __INITIALIZATION_H__

#include "Platform.h"

/// <summary>This function initializes GAL.
/// Main purpose is to initialize global random generators and operation catalogues.</summary>
/// <remarks>No library feature should be called before calling <c>GaInitialize</c>.</remarks>
GAL_API
void GACALL GaInitialize();

/// <summary><c>GaFinalize</c> should be call before exiting application, and after work with GAL is done.
/// It releases all acquired memory and resources needed for library.</summary>
/// <remarks>Objects which were instantiated by the user,
/// except those objects which was added to global operation catalogues, must be destroyed prior calling <c>GaFinalize</c>.
/// Note that any object of operation which was added to global catalogues will be deleted after <c>GaFinalize</c> call.</remarks>
GAL_API
void GACALL GaFinalize();

#endif // __INITIALIZATION_H__

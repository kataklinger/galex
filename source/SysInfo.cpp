
/*! \file SysInfo.h
	\brief This file contains implementations of classes and functions that provides various system information.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#include "SysInfo.h"

#if defined( GAL_PLATFORM_WIN )

	#define NOMINMAX
	#include <windows.h>

	int GACALL GetCacheLineSize()
	{
		int cachLineSize = 0;

		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
		DWORD bufferSize = 0;

		// get number of logical processors
		if( ::GetLogicalProcessorInformation( buffer, &bufferSize ) == FALSE && GetLastError() == ERROR_INSUFFICIENT_BUFFER )
		{
			buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc( bufferSize );

			// get information about all logical processors
			if( ::GetLogicalProcessorInformation( buffer, &bufferSize ) != FALSE )
			{
				DWORD count = bufferSize / sizeof( SYSTEM_LOGICAL_PROCESSOR_INFORMATION );

				for( int i = count - 1; i >= 0; i-- )
				{
					if( buffer[ i ].Relationship == RelationCache )
					{
						CACHE_DESCRIPTOR& c =  buffer[ i ].Cache;

						// only cache that store data is considered
						if( c.Type == CacheData || c.Type == CacheUnified )
							if( c.LineSize > cachLineSize )
								cachLineSize = c.LineSize;
					}
				}
			}

			free( buffer );
		}

		return cachLineSize;
	}

#elif defined( GAL_PLATFORM_NIX )

#endif

// Pointer to global instance of CPU info class
GaCpuInfo* GaCpuInfo::_instance = NULL;

// Collects CPU info
GaCpuInfo::GaCpuInfo() { _cacheLineSize = ::GetCacheLineSize(); }

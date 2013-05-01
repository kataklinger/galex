
/*! \file SysInfo.h
	\brief This file contains declarations of classes and functions that provides various system information.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __SYS_INFO_H__
#define __SYS_INFO_H__

#include "Exceptions.h"

/// <summary><c>GetCacheLineSize</c> method queries CPU cache line size.</summary>
/// <returns>Method returns size of the largest CPU cache line in bytes.</returns>
int GACALL GetCacheLineSize();

/// <summary><c>GaCpuInfo</c> class provides information about installed processor(s).
///
/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
/// but some public methods are thread-safe.</summary>
class GaCpuInfo
{

private:

		/// <summary>Pointer to global instance of CPU info class.</summary>
		GAL_API
		static GaCpuInfo* _instance;

public:

		/// <summary><c>GetInstance</c> method return pointer to global instance of CPU info class.</summary>
		/// <returns>Method returns pointer to global instance of CPU info classl.</returns>
		static inline GaCpuInfo& GACALL GetInstance() { return *_instance; }

		/// <summary><c>MakeInstance</c> method makes global instance of CPU info class if it is not already exist.</summary>
		/// <exception cref="GaInvalidOperationException" />Thrown if global instance already exists.</exception>
		static void GACALL MakeInstance()
		{
			GA_ASSERT( Common::Exceptions::GaInvalidOperationException, _instance == NULL, "Global instance already exists.", "Threading" );
			_instance = new GaCpuInfo();
		}

		/// <summary><c>FreeInstance</c> method deletes global instance of CPU info class and frees used resources..</summary>
		/// <exception cref="GaInvalidOperationException" />Thrown if global instance does not exist.</exception>
		static void GACALL FreeInstance()
		{
			GA_ASSERT( Common::Exceptions::GaInvalidOperationException, _instance != NULL, "Global instance does not exists.", "Threading" );
			delete _instance;
		}

private:

	/// <summary>Size of the largest CPU cache line in bytes.</summary>
	int _cacheLineSize;

public:

	/// <summary>Constructor collects required information about processor(s).</summary>
	GAL_API
	GaCpuInfo();

	/// <summary>This method is tread-safe.</summary>
	/// <returns>Method returns size of the largest CPU cache line in bytes.</returns>
	inline int GACALL GetCacheLineSize() const { return _cacheLineSize; }

};

#endif // __SYS_INFO_H__

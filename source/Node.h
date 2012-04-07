
/*! \file Node.h
	\brief This file declares interface for nodes of data structures.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#ifndef __GA_NODE_H__
#define __GA_NODE_H__

#include "Platform.h"

namespace Common
{
	namespace Data
	{

		/// <summary><c>GaNodeBase</c> is interface for nodes used by data structures to store data.</summary>
		class GaNodeBase
		{

		public:

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaNodeBase() { }

			/// <summary><c>Clone()</c> method creates new node and copies data stored in source node. Caller is responsible for allocated memory.</summary>
			/// <returns>Method returns pointer to newly created node.</returns>
			virtual GaNodeBase* GACALL Clone() const = 0;

			/// <summary><c>SwapData</c> method swaps data stored in the nodes.</summary>
			/// <param name="node">node with which the data should be swapped.</param>
			virtual void GACALL SwapData(GaNodeBase* node) = 0;

			/// <summary><c>operator ==</c> method compares data stored in the nodes.</summary>
			/// <param name="node">node which stores data that should be compared.</param>
			/// <returns>Method returns <c>true</c> if data in the nodes are equal.</returns>
			virtual bool GACALL operator ==(const GaNodeBase& node) const = 0;

			/// <summary><c>operator !=</c> method compares data stored in the nodes.</summary>
			/// <param name="node">node which stores data that should be compared.</param>
			/// <returns>Method returns <c>true</c> if data in the nodes are not equal.</returns>
			virtual bool GACALL operator !=(const GaNodeBase& node) const = 0;

		};

	} // Data
} // Common

#endif // __GA_NODE_H__

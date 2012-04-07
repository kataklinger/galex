
/*! \file FitnessComparators.cpp
    \brief This file implements classes that represent fitness comparators.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#include "FitnessComparators.h"
#include "FitnessValues.h"

namespace Fitness
{
	namespace Comparators
	{

		// Comparator for fitness objects that conatin single value
		int GaSimpleComparator::operator ()(const GaFitness& fitness1,
			const GaFitness& fitness2,
			const GaFitnessComparatorParams& parameters) const
		{
			return ( (const GaSimpleComparatorParams&)parameters ).GetType() * ( (Representation::GaSVFitnessBase&)fitness1 ).CompareValues( (Representation::GaSVFitnessBase&)fitness2 );
		}

		int GaPositionalComparator::operator ()(const GaFitness& fitness1,
			const GaFitness& fitness2,
			const GaFitnessComparatorParams& parameters) const
		{
			const Representation::GaMVFitnessBase& f1 = (const Representation::GaMVFitnessBase&)fitness1;
			const Representation::GaMVFitnessBase& f2 = (const Representation::GaMVFitnessBase&)fitness2;

			int type = ( (const GaSimpleComparatorParams&)parameters ).GetType();

			// compare each pair of fitness value until difference is found
			int result = 0, size = f1.GetValues().GetSize();
			for( int i = 0; !result && i < size; i++ )
				result = type * f1.CompareValues( f2, i );

			return result;
		}

		// Comparator that implements pareto dominance for fitness objects that contain multiple values
		int GaDominanceComparator::operator ()(const GaFitness& fitness1,
			const GaFitness& fitness2,
			const GaFitnessComparatorParams& parameters) const
		{
			const Representation::GaMVFitnessBase& f1 = (const Representation::GaMVFitnessBase&)fitness1;
			const Representation::GaMVFitnessBase& f2 = (const Representation::GaMVFitnessBase&)fitness2;

			int type = ( (const GaSimpleComparatorParams&)parameters ).GetType();

			// find dominant fitness
			int result = 0;
			for( int i = f1.GetValues().GetSize() - 1; i >= 0; i-- )
			{
				// find better value in the pair
				int cmp = type * f1.CompareValues( f2, i );

				// is it the first dominant value?
				if( !result )
					result = cmp;
				// both fitness contains values the are better then other
				else if( cmp && result != cmp )
					// no dominant fitness
					return 0;
			}

			return result;
		}

		// Comparator that compare only a single value 
		int GaVegaComparator::operator ()(const GaFitness& fitness1,
			const GaFitness& fitness2,
			const GaFitnessComparatorParams& parameters) const
		{
			const GaVegaComparatorParams& p = (const GaVegaComparatorParams&)parameters;
			return p.GetType() * ( (const Representation::GaMVFitnessBase&)fitness1 ).CompareValues( (const Representation::GaMVFitnessBase&)fitness2, p.GetVectorIndex() );
		}

	} // Comparators
} // Fitness


/*! \file ChromosomeStorage.h
    \brief This file contains declaration of classes that is used for storing chrmosomes and data about chromosomes.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_CHROMOSOME_STORAGE_H__
#define __GA_CHROMOSOME_STORAGE_H__

#include "Flags.h"
#include "Tags.h"
#include "ObjectPool.h"
#include "Sorting.h"
#include "Grid.h"
#include "ChromosomeOperations.h"

namespace Population
{

	/// <summary><c>GaChromosomeStorage</c> class is used by population to store chromosome and other information about it (such as its fitness, tag and flags).
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaChromosomeStorage
	{

		GA_DEFINE_ATOM_NODE( GaChromosomeStorage )

	public:

		/// <summary><c>GaFlagType</c> defines type of flags object used to store chromosome's flag.</summary>
		typedef Common::Data::GaFlags<Common::Threading::GaAtomic<unsigned int> > GaFlagType;

		/// <summary>Define chromosome flags.</summary>
		enum GaChromosomeFlags
		{

			/// <summary>Indicates that chromosome was inserted into the population in current generation.</summary>
			GACF_NEW_CHROMOSOME = 0x1,

			/// <summary>Indicates that chromosome should be removed from population.</summary>
			GACF_REMOVE_CHROMOSOME = 0x2

		};

		/// <summary>Defines fitness types of chromosome.</summary>
		enum GaFitnessType
		{

			/// <summary>Raw fitness produced by fitness operation.</summary>
			GAFT_RAW = 0,

			/// <summary>Scaled fitness produced by scaling operation.</summary>
			GAFT_SCALED,

		};

	private:

		/// <summary>Chromosome stored by this object.</summary>
		Chromosome::GaChromosomePtr _chromosome;

		/// <summary>Fitness value of chromosome evaluated by population's fitness operation.</summary>
		Common::Memory::GaAutoPtr<Fitness::GaFitness> _fitness[ 2 ];

		/// <summary>Chromosome's tags.</summary>
		Common::Data::GaTagBuffer _tagBuffer;

		/// <summary>Chromosome's flags.</summary>
		GaFlagType _flags;

		/// <summary>Parent of the chromosome.</summary>
		GaChromosomeStorage* _parent;

	public:

		/// <summary>This constructor initializes chromosome storage object with fitness object and tag buffer that will be used to store information about chromosome.</summary>
		/// <param name="rawFitness">pointer to fitness object that will be used to store raw fitness value of chromosome. Storage object takse ownership of the object.</param>
		/// <param name="scaledFitness">pointer to fitness object that will be used to store scaled fitness value of chromosome. Storage object takse ownership of the object.</param>
		GaChromosomeStorage(Fitness::GaFitness* rawFitness,
			Fitness::GaFitness* scaledFitness) : _parent(NULL)
		{
			_fitness[ GAFT_RAW ] = rawFitness;
			_fitness[ GAFT_SCALED ] = scaledFitness;
		}

		/// <summary><c>Clear</c> method cleans storage object so it can be used again for new chromosome, but does not clean tag buffer.
		///
		/// This method is not thread-safe.</summary>
		GAL_API
		void GACALL Clear();

		/// <summary><c>SetChromosome</c> method stores new chromosome in the object.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="chromosome">smart pointer to new chromosome that should be stored by this object.</param>
		/// <param name="parent">parent of the chromosome.</param>
		inline void GACALL SetChromosome(Chromosome::GaChromosomePtr chromosome,
			GaChromosomeStorage* parent = NULL)
		{
			_chromosome = chromosome;
			_parent = parent;
		}

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns smart pointer to chromosome stored in the object, or <c>NullPtr</c> if object does not contain chromosome.</returns>
		inline Chromosome::GaChromosomePtr GACALL GetChromosome() { return _chromosome; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns smart pointer to chromosome stored in the object, or <c>NullPtr</c> if object does not contain chromosome.</returns>
		inline Chromosome::GaChromosomeConstPtr GACALL GetChromosome() const { return _chromosome; }

		/// <summary><c>SetFitnessObject</c> method sets new fitness object that should be used by storage object to keep fitness value of the chromosome.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="fitness">pointer to new fitness of the stored chromosome. Storage object takse ownership of the object.</param>
		/// <param name="type">type of fitness that should be stored.</param>
		inline void GACALL SetFitnessObject(Fitness::GaFitness* fitness,
			GaFitnessType type) { _fitness[ type ] = fitness; }

		/// <summary><c>GetFitness</c> method returns reference to fitness of the stored chromosome of the specified type.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="type">type of queried fitness.</param>
		/// <returns>Method returns reference to fitness of the stored chromosome.</returns>
		inline Fitness::GaFitness& GACALL GetFitness(GaFitnessType type) { return *_fitness[ type ]; }

		/// <summary><c>GetFitness</c> method returns reference to fitness of the stored chromosome of the specified type.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="type">type of queried fitness.</param>
		/// <returns>Method returns reference to fitness of the stored chromosome.</returns>
		inline const Fitness::GaFitness& GACALL GetFitness(GaFitnessType type) const { return *_fitness[ type ]; }
		
		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to buffer that contaions chromosome's tags.</summary>
		inline Common::Data::GaTagBuffer& GetTags() { return _tagBuffer; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns reference to buffer that contaions chromosome's tags.</summary>
		inline const Common::Data::GaTagBuffer& GetTags() const { return _tagBuffer; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer object that sotres chromosome's flags.</returns>
		inline GaFlagType& GACALL GetFlags() { return _flags; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer object that sotres chromosome's flags.</returns>
		inline const GaFlagType& GACALL GetFlags() const { return _flags; }

		/// <summary><c>SetParent</c> method sets chromosome's parent.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="parent">parent of the chromosome.</param>
		inline void GACALL SetParent(GaChromosomeStorage* parent) { _parent = parent; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to object that stores chromosome's parent.</returns>
		inline GaChromosomeStorage* GACALL GetParent() { return _parent; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to object that stores chromosome's parent.</returns>
		inline const GaChromosomeStorage* GACALL GetParent() const { return _parent; }

	};

	/// <summary><c>GaChromosomeSortingCriteria</c> type is instance of <see cref="GaSortingCriteria" /> template class and represents interface for sorting criteria of chromosomes.
	/// Detailed description can be found in specification of <see cref="GaOperationSetup" /> template class.</summary>
	typedef Common::Sorting::GaSortingCriteria<GaChromosomeStorage*> GaChromosomeSortingCriteria;

	/// <summary><c>GaChromosomeTagGetter</c> class provides access to typed data of chromosome's tags.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	/// <param name="DATA_TYPE">type of data stored in tag.</param>
	template<typename DATA_TYPE>
	class GaChromosomeTagGetter : public Common::Data::GaTagGetter<DATA_TYPE, GaChromosomeStorage>
	{

	public:

		/// <summary>Initializes tag getter for specific tag.</summary>
		/// <param name="tagID">tag ID.</param>
		/// <param name="tagManager">pointer to manager that handles tags and tag buffers</param>
		GaChromosomeTagGetter(int tagID,
			const Common::Data::GaTagManager& tagManager) : GaTagGetter(tagID, tagManager) { }

		/// <summary>Default constructor.</summary>
		GaChromosomeTagGetter() { }

	};

	/// <summary>Matrix that stores best ranked chromosomes and grid desnity information.</summary>
	typedef Common::Grid::GaBestItemsMatrix<GaChromosomeStorage*> GaBestChromosomesMatrix;

} // Population

#endif // __GA_CHROMOSOME_STORAGE_H__

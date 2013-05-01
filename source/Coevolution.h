
/*! \file Coevolution.h
    \brief This file declares basic classes and datatypes required for coevolutive algorithms.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_COEVOLUTION_H__
#define __GA_COEVOLUTION_H__

#include "Population.h"

/// <summary>Contains interfaces, classes and datatypes required for implementing coevolutive algorithm.</summary>
namespace Coevolution
{

	/// <summary><c>GaMigrationPort</c> class is used for storing chromosomes during their migration to other population.
	///
	/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// Because this genetic operation is stateless all public method are thread-safe.</summary>
	class GaMigrationPort
	{

	private:

		/// <summary>Array that stores migrated chromosomes.</summary>
		Common::Data::GaSingleDimensionArray<Chromosome::GaChromosomePtr> _chromosomes;

		/// <summary>Number of chromosomes currently stored in the port.</summary>
		Common::Threading::GaAtomic<int> _count;

	public:

		/// <summary>Constructor initializes migration port with its size.</summary>
		/// <param name="size">number of chromosomes that port can store.</param>
		GaMigrationPort(int size) : _chromosomes(size),
			_count(0) { }

		/// <summary>Constructor initializes migration port with zero size.</summary>
		GaMigrationPort() : _count(0) { }

		/// <summary><c>Insert</c> stores chromosome in the migration port.
		/// Different threads can safely execute this method simultaneously, but no other methods on this object should be executed from other threads at that time.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="chromosome"></param>
		inline void GACALL Insert(Chromosome::GaChromosomePtr chromosome) { _chromosomes[ _count++ ] = chromosome; }

		/// <summary><c>Clear</c> method removes all chromosomes from the port.
		///
		/// This method is not thread-safe.</summary>
		inline void GACALL Clear()
		{
			for( int i = _count - 1; i >= 0; i-- )
				_chromosomes[ i ] = Chromosome::GaChromosomePtr::NullPtr;

			_count = 0;
		}

		/// <summary><c>SetSize</c> method sets the number of chromosomes that port can store.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="size">new port size.</param>
		inline void GACALL SetSize(int size) { _chromosomes.SetSize( size ); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns number fo chromosomes that migration port can store.</returns>
		inline int GACALL GetSize() const { return _chromosomes.GetSize(); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns number of chromosomes stored in migration port.</returns>
		inline int GACALL GetCount() const { return _count; }

		/// <summary><c>operator []</c> returns pointer to chromosome stored at specified index in migration port.
		///
		/// This operator is thread-safe.</summary>
		/// <param name="index">index of queried chromosome.</param>
		/// <returns>Operator returns pointer to queried chromosome.</returns>
		inline Chromosome::GaChromosomePtr& operator [](int index) { return _chromosomes[ index ]; }

		/// <summary><c>operator []</c> returns pointer to chromosome stored at specified index in migration port.
		///
		/// This operator is thread-safe.</summary>
		/// <param name="index">index of queried chromosome.</param>
		/// <returns>Operator returns pointer to queried chromosome.</returns>
		inline const Chromosome::GaChromosomePtr& operator [](int index) const { return _chromosomes[ index ]; }

	};

	/// <summary><c>GaMigrationParams</c> class is base for parameters of migration operations.</summary>
	class GaMigrationParams : public Common::GaParameters { };

	/// <summary><c>GaMigrationConfig</c> class is base for configuration of migration operations.</summary>
	class GaMigrationConfig : public Common::GaConfiguration { };

	/// <summary>This class is interface for migration operations which selectes chromosomes that should be copied to other populations.</summary>
	class GaMigrationOperation : public Common::GaOperation
	{

	public:

		/// <summary><c>Prepare</c> method prepares input and output data object for execution of the migration operation based on provided parameters and configuration.</summary>
		/// <param name="population">population from which the chromosomes are migrated.</param>
		/// <param name="port">migration port to which the selected chromosomes are stored.</param>
		/// <param name="parameters">paramenters of migration operation.</param>
		/// <param name="configuration">configuration of migration operation.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		virtual void GACALL Prepare(Population::GaPopulation& population,
			GaMigrationPort& port,
			const GaMigrationParams& parameters,
			const GaMigrationConfig& configuration,
			int branchCount) const = 0;

		/// <summary><c>Clear</c> method reverse changes made by migration operation to input and output data object.</summary>
		/// <param name="population">population from which the chromosomes are migrated.</param>
		/// <param name="port">migration port to which the selected chromosomes are stored.</param>
		/// <param name="parameters">paramenters of migration operation.</param>
		/// <param name="configuration">configuration of migration operation.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		virtual void GACALL Clear(Population::GaPopulation& population,
			GaMigrationPort& port,
			const GaMigrationParams& parameters,
			const GaMigrationConfig& configuration,
			int branchCount) const = 0;

		/// <summary><c>Update</c> method updates input and output data object to reflect changes in the workflow.</summary>
		/// <param name="population">population from which the chromosomes are migrated.</param>
		/// <param name="port">migration port to which the selected chromosomes are stored.</param>
		/// <param name="parameters">paramenters of migration operation.</param>
		/// <param name="configuration">configuration of migration operation.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		virtual void GACALL Update(Population::GaPopulation& population,
			GaMigrationPort& port,
			const GaMigrationParams& parameters,
			const GaMigrationConfig& configuration,
			int branchCount) const = 0;

		/// <summary><c>operator ()</c> chooses chromosomes that should be migrated to other populations.</summary>
		/// <param name="population">population from which the chromosomes are migrated.</param>
		/// <param name="port">migration port to which the selected chromosomes are stored.</param>
		/// <param name="parameters">paramenters of migration operation.</param>
		/// <param name="configuration">configuration of migration operation.</param>
		/// <param name="branch">pointer to workflow barnch that executes operation.</param>
		virtual void GACALL operator ()(Population::GaPopulation& population,
			GaMigrationPort& port,
			const GaMigrationParams& parameters,
			const GaMigrationConfig& configuration,
			Common::Workflows::GaBranch* branch) const = 0;

	};

	/// <summary><c>GaMigrationSetup</c> type is instance of <see cref="GaOperationSetup" /> template class and represents setup
	/// of migration operation and its parameters. Detailed description can be found in specification of <see cref="GaOperationSetup" /> template class.</summary>
	typedef Common::GaOperationSetup<GaMigrationOperation, GaMigrationParams, GaMigrationConfig> GaMigrationSetup;

	/// <summary><c>GaMigrationCatalogue</c> type is instance of <see cref="GaCatalogue" /> template class and represents catalogue of migration operations.
	/// Detailed description can be found in specification of <see cref="GaCatalogue" /> template class.</summary>
	typedef Common::Data::GaCatalogue<GaMigrationOperation> GaMigrationCatalogue;

	/// <summary><c>GaMigrationStep</c> type is instance of <see cref="GaOperationStep2" /> template class and represents workflow step that performs 
	/// migration operation. Detailed description can be found in specification of <see cref="GaOperationStep2" /> template class.</summary>
	typedef Common::Workflows::GaOperationStep2<GaMigrationSetup, Population::GaPopulation, GaMigrationPort> GaMigrationStep;

	/// <summary><c>GaAdoptionParams</c> class is base for parameters of adoption operations.</summary>
	class GaAdoptionParams : public Common::GaParameters { };

	/// <summary><c>GaAdoptionConfig</c> class is base for configuration of adoption operations.</summary>
	class GaAdoptionConfig : public Common::GaConfiguration { };

	/// <summary>This class is interface for adoption operations which adopts migrated chromosomes from other populations.</summary>
	class GaAdoptionOperation : public Common::GaOperation
	{

	public:

		/// <summary><c>Prepare</c> method prepares input and output data object for execution of the adoption operation based on provided parameters and configuration.</summary>
		/// <param name="port">migration port that contains chromosomes which should be adopted by the population.</param>
		/// <param name="population">population which adopts migrated chromosome.</param>
		/// <param name="parameters">paramenters of adoption operation.</param>
		/// <param name="configuration">configuration of adoption operation.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		virtual void GACALL Prepare(GaMigrationPort& port,
			Population::GaPopulation& population,
			const GaAdoptionParams& parameters,
			const GaAdoptionConfig& configuration,
			int branchCount) const = 0;

		/// <summary><c>Clear</c> method reverse changes made by adoption operation to input and output data object.</summary>
		/// <param name="port">migration port that contains chromosomes which should be adopted by the population.</param>
		/// <param name="population">population which adopts migrated chromosome.</param>
		/// <param name="parameters">paramenters of adoption operation.</param>
		/// <param name="configuration">configuration of adoption operation.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		virtual void GACALL Clear(GaMigrationPort& port,
			Population::GaPopulation& population,
			const GaAdoptionParams& parameters,
			const GaAdoptionConfig& configuration,
			int branchCount) const = 0;

		/// <summary><c>Update</c> method updates input and output data object to reflect changes in the workflow.</summary>
		/// <param name="port">migration port that contains chromosomes which should be adopted by the population.</param>
		/// <param name="population">population which adopts migrated chromosome.</param>
		/// <param name="parameters">paramenters of adoption operation.</param>
		/// <param name="configuration">configuration of adoption operation.</param>
		/// <param name="branchCount">number of workflow branches that executes operation.</param>
		virtual void GACALL Update(GaMigrationPort& port,
			Population::GaPopulation& population,
			const GaAdoptionParams& parameters,
			const GaAdoptionConfig& configuration,
			int branchCount) const = 0;

		/// <summary><c>operator ()</c> adopts chromosomes stored in migration port to be used by/stored to the new population.</summary>
		/// <param name="port">migration port that contains chromosomes which should be adopted by the population.</param>
		/// <param name="population">population which adopts migrated chromosome.</param>
		/// <param name="parameters">paramenters of adoption operation.</param>
		/// <param name="configuration">configuration of adoption operation.</param>
		/// <param name="branch">pointer to workflow barnch that executes operation.</param>
		virtual void GACALL operator ()(GaMigrationPort& port,
			Population::GaPopulation& population,
			const GaAdoptionParams& parameters,
			const GaAdoptionConfig& configuration,
			Common::Workflows::GaBranch* branch) const = 0;

	};

	/// <summary><c>GaAdoptionSetup</c> type is instance of <see cref="GaOperationSetup" /> template class and represents setup
	/// of adoption operation and its parameters. Detailed description can be found in specification of <see cref="GaOperationSetup" /> template class.</summary>
	typedef Common::GaOperationSetup<GaAdoptionOperation, GaAdoptionParams, GaAdoptionConfig> GaAdoptionSetup;

	/// <summary><c>GaAdoptionCatalogue</c> type is instance of <see cref="GaCatalogue" /> template class and represents catalogue of adoption operations.
	/// Detailed description can be found in specification of <see cref="GaCatalogue" /> template class.</summary>
	typedef Common::Data::GaCatalogue<GaAdoptionOperation> GaAdoptionCatalogue;

	/// <summary><c>GaAdoptionStep</c> type is instance of <see cref="GaOperationStep2" /> template class and represents workflow step that performs 
	/// adoption operation. Detailed description can be found in specification of <see cref="GaOperationStep2" /> template class.</summary>
	typedef Common::Workflows::GaOperationStep2<GaAdoptionSetup, GaMigrationPort, Population::GaPopulation> GaAdoptionStep;

} // Coevolution

#endif // __GA_COEVOLUTION_H__

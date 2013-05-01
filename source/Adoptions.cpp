
/*! \file Adoptions.cpp
    \brief This file implements classes that represent adoption operations.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#include "Adoptions.h"

namespace Coevolution
{

	/// <summary><c>GaFillReplacementInputOp</c> class represent operation which prepares migrated chromosome for insetion into population
	/// and adds it to input buffer of replacement operation.</summary>
	class GaFillReplacementInputOp
	{

	private:

		/// <summary>Input buffer of replacement operation which cntains chromosome that should be inserted into population.</summary>
		Population::GaChromosomeGroup& _input;

	public:

		/// <summary>Initializes operation.</summary>
		/// <param name="input">input buffer of replacement operation which cntains chromosome that should be inserted into population.</param>
		GaFillReplacementInputOp(Population::GaChromosomeGroup& input) : _input(input) { }

		/// <summary><c>operator ()</c> prepares migrated chromosome and adds it to input buffer of replacement operation.</summary>
		/// <param name="chromosome">migrated chromosome that should be inserted into input buffer.</param>
		/// <param name="index">index of chromosome in migration port.</param>
		inline void GACALL operator ()(Chromosome::GaChromosomePtr& chromosome,
			int index)
		{
			Population::GaChromosomeStorage* storage = _input.GetPopulation()->AcquireStorageObject();
			storage->SetChromosome( chromosome );

			_input.AddAtomic( storage );
		}

	};

	// Prepares population for adoption operation
	void GaReplacementAdoption::Prepare(GaMigrationPort& port,
		Population::GaPopulation& population,
		const GaAdoptionParams& parameters,
		const GaAdoptionConfig& configuration,
		int branchCount) const
	{
		// add data needed for replacement operation
		int index = population.GetTagManager().AddTag( ( (const GaReplacementAdoptionParams&)parameters).GetReplacementInputTagID(),
			Common::Data::GaTypedTagLifecycle<Population::GaChromosomeGroup>() );

		// set-up input buffer of replacement operation
		Population::GaChromosomeGroup& input = population.GetTagByIndex<Population::GaChromosomeGroup>( index );
		input.SetPopulation( &population );
		input.SetSize( port.GetSize() );

		// perform preparations for replacement operation
		const Population::GaReplacementSetup& replacement = ( (const GaReplacementAdoptionConfig&)configuration ).GetReplacement();
		replacement.GetOperation().Prepare( input, population, replacement.GetParameters(), replacement.GetConfiguration(), branchCount );
	}

	// Removes data that was needed by the adoption operation
	void GaReplacementAdoption::Clear(GaMigrationPort& port,
		Population::GaPopulation& population,
		const GaAdoptionParams& parameters,
		const GaAdoptionConfig& configuration,
		int branchCount) const
	{
		// perform clean-up of replacement operation
		const Population::GaReplacementSetup& replacement = ( (const GaReplacementAdoptionConfig&)configuration ).GetReplacement();
		replacement.GetOperation().Clear( population.GetTagByID<Population::GaChromosomeGroup>( ( (const GaReplacementAdoptionParams&)parameters).GetReplacementInputTagID() ), population,
			replacement.GetParameters(), replacement.GetConfiguration(), branchCount );

		// remove data needed for replacement operation
		population.GetTagManager().RemoveTag( ( (const GaReplacementAdoptionParams&)parameters).GetReplacementInputTagID() );
	}

	// Updates population for adoption operation
	void GaReplacementAdoption::Update(GaMigrationPort& port,
		Population::GaPopulation& population,
		const GaAdoptionParams& parameters,
		const GaAdoptionConfig& configuration,
		int branchCount) const
	{
		// Updates population for replacement operation
		const Population::GaReplacementSetup& replacement = ( (const GaReplacementAdoptionConfig&)configuration ).GetReplacement();
		replacement.GetOperation().Update( population.GetTagByID<Population::GaChromosomeGroup>( ( (const GaReplacementAdoptionParams&)parameters).GetReplacementInputTagID() ), population,
			replacement.GetParameters(), replacement.GetConfiguration(), branchCount );
	}

	// Replaces chromosome from the population with migrated chromosomes
	void GaReplacementAdoption::Exec(GaMigrationPort& port,
		Population::GaPopulation& population,
		const GaReplacementAdoptionParams& parameters,
		const GaReplacementAdoptionConfig& configuration,
		Common::Workflows::GaBranch* branch) const
	{
		Population::GaChromosomeGroup& input = population.GetTagByID<Population::GaChromosomeGroup>( parameters.GetReplacementInputTagID() );

		// prepares migated chromosomes to be inserted into the population
		Common::Workflows::GaParallelExec1<GaMigrationPort, Chromosome::GaChromosomePtr> workDist( *branch, port );
		workDist.Execute( GaFillReplacementInputOp( input ), true );

		// preform replacement
		const Population::GaReplacementSetup& replacement = configuration.GetReplacement();
		replacement.GetOperation()( input, population, replacement.GetParameters(), replacement.GetConfiguration(), branch );
	}

} // Coevolution

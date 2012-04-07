
/*! \file Migrations.cpp
    \brief This file implements classes that represent migration operations.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#include "Migrations.h"

namespace Coevolution
{

	/// <summary><c>GaFillMigrationPortOp</c> class represents operation which adds chromosome to migration port.</summary>
	class GaFillMigrationPortOp
	{

	private:

		/// <summary>Migaration port to which the chromosomes are added.</summary>
		GaMigrationPort& _port;

	public:

		/// <summary>Initializes operation.</summary>
		/// <param name="port">migration port to which the chromosomes are added.</param>
		GaFillMigrationPortOp(GaMigrationPort& port) : _port(port) { }

		/// <summary><c>operator ()</c> inserts chromosome to migration port.</summary>
		/// <param name="chromosome">storage object that contains chromosome which should be added to migration port.</param>
		/// <param name="index">index of chromosome.</param>
		inline void GACALL operator ()(Population::GaChromosomeStorage& chromosome,
			int index) { _port.Insert( chromosome.GetChromosome() ); }

	};

	// Prepares population for migration operation
	void GaSelectionMigration::Prepare(Population::GaPopulation& population,
		GaMigrationPort& port,
		const GaMigrationParams& parameters,
		const GaMigrationConfig& configuration,
		int branchCount) const
	{
		// add data needed for selection operation
		int index = population.GetTagManager().AddTag( ( (const GaSelectionMigrationParams&)parameters).GetSelectionOutputTagID(),
			Common::Data::GaTypedTagLifecycle<Population::GaChromosomeGroup>() );

		// perform preparations for selection operation
		const Population::GaSelectionSetup& selection = ( (const GaSelectionMigrationConfig&)configuration ).GetSelection();
		selection.GetOperation().Prepare( population, population.GetTagByIndex<Population::GaChromosomeGroup>( index ),
			selection.GetParameters(), selection.GetConfiguration(), branchCount );
	}

	// Removes data that was needed by the migration operation
	void GaSelectionMigration::Clear(Population::GaPopulation& population,
		GaMigrationPort& port,
		const GaMigrationParams& parameters,
		const GaMigrationConfig& configuration,
		int branchCount) const
	{
		// perform clean-up of selection operation
		const Population::GaSelectionSetup& selection = ( (const GaSelectionMigrationConfig&)configuration ).GetSelection();
		selection.GetOperation().Clear( population, population.GetTagByID<Population::GaChromosomeGroup>( ( (const GaSelectionMigrationParams&)parameters).GetSelectionOutputTagID() ),
			selection.GetParameters(), selection.GetConfiguration(), branchCount );

		// remove data needed for selection operation
		population.GetTagManager().RemoveTag( ( (const GaSelectionMigrationParams&)parameters).GetSelectionOutputTagID() );
	}

	// Updates population for migration operation
	void GaSelectionMigration::Update(Population::GaPopulation& population,
		GaMigrationPort& port,
		const GaMigrationParams& parameters,
		const GaMigrationConfig& configuration,
		int branchCount) const
	{
		// Updates population for selection operation
		const Population::GaSelectionSetup& selection = ( (const GaSelectionMigrationConfig&)configuration ).GetSelection();
		selection.GetOperation().Update( population, population.GetTagByID<Population::GaChromosomeGroup>( ( (const GaSelectionMigrationParams&)parameters).GetSelectionOutputTagID() ),
			selection.GetParameters(), selection.GetConfiguration(), branchCount );
	}

	// Selects chromosomes that should be migrated
	void GaSelectionMigration::Exec(Population::GaPopulation& population,
		GaMigrationPort& port,
		const GaSelectionMigrationParams& parameters,
		const GaSelectionMigrationConfig& configuration,
		Common::Workflows::GaBranch* branch) const
	{
		Population::GaChromosomeGroup& selectionOutput = population.GetTagByID<Population::GaChromosomeGroup>( parameters.GetSelectionOutputTagID() );

		// selects chromosomes that should be migrated
		const Population::GaSelectionSetup& selection = configuration.GetSelection();
		selection.GetOperation()( population, selectionOutput, selection.GetParameters(), selection.GetConfiguration(), branch );

		GA_BARRIER_SYNC( lock, branch->GetBarrier(), branch->GetBarrierCount() )
			port.Clear();

		// store selected chromosomes to migration buffer
		Common::Workflows::GaParallelExec1<Population::GaChromosomeGroup, Population::GaChromosomeStorage> workDist( *branch, selectionOutput );
		workDist.Execute( GaFillMigrationPortOp( port ), true );
	}

} // Coevolution

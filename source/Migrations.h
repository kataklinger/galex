
/*! \file Migrations.h
    \brief This file declares classes that represent migration operations.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_MIGRATIONS_H__
#define __GA_MIGRATIONS_H__

#include "Coevolution.h"

namespace Coevolution
{
	
	/// <summary><c>GaSelectionMigrationParams</c> class represent paramenters for migration operations that uses standard selection operations.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaSelectionMigrationParams : public GaMigrationParams
	{

	protected:

		/// <summary>ID of the population tag that stores data required by selection operation.</summary>
		int _selectionOutputTagID;

	public:

		/// <summary>This constructor initializes paramenters with tag that stores data required by selection operation.</summary>
		/// <param name="selectionOutputTagID">ID of the population tag.</param>
		GaSelectionMigrationParams(int selectionOutputTagID) : _selectionOutputTagID(selectionOutputTagID) { }

		/// <summary>This constructor initializes paramenters without specified tag that should store data required by by selection operation.</summary>
		GaSelectionMigrationParams() : _selectionOutputTagID(-1) { }

		/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
		///
		/// This method is not thread-safe.</summary>
		virtual Common::GaParameters* GACALL Clone() const { return new GaSelectionMigrationParams( *this ); }

		/// <summary><c>SetSelectionOutputTagID</c> method sets population tag that will store data required by selection operation.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="tagID">ID of the tag.</param>
		inline void GACALL SetSelectionOutputTagID(int tagID) { _selectionOutputTagID = tagID; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns ID of the population tag that stores data required by selection operation.</returns>
		inline int GACALL GetSelectionOutputTagID() const { return _selectionOutputTagID; }

	};

	/// <summary><c>GaSelectionMigrationConfig</c> class represent configuration for migration operations that uses standard selection operations.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaSelectionMigrationConfig : public GaMigrationConfig
	{

	protected:

		/// <summary>Underlying selection operation which chooses which chromosomes should be migrated.</summary>
		Population::GaSelectionSetup _selection;

	public:

		/// <summary>This constructor initializes migration operation with underlaying selection operation.</summary>
		/// <param name="selection">underlying selection operation which chooses which chromosomes should be migrated.</param>
		GaSelectionMigrationConfig(const Population::GaSelectionSetup& selection) : _selection(selection) { }

		/// <summary>This constructor initializes migration operation without underlaying selection operation.</summary>
		GaSelectionMigrationConfig() { }

		/// <summary>More details are given in specification of <see cref="GaConfiguration::Clone" /> method.
		///
		/// This method is not thread-safe.</summary>
		virtual GaConfiguration* GACALL Clone() const { return new GaSelectionMigrationConfig( *this ); }

		/// <summary><c>SetSelection</c> method sets underlying selection operation that is going to be used to choose which chromosomes should be migrated.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="selection">selection operation.</param>
		inline void GACALL SetSelection(Population::GaSelectionSetup& selection)  { _selection = selection; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns selection operation which chooses which chromosomes should be migrated.</returns>
		inline Population::GaSelectionSetup& GACALL GetSelection() { return _selection; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns selection operation which chooses which chromosomes should be migrated.</returns>
		inline const Population::GaSelectionSetup& GACALL GetSelection() const { return _selection; }

	};

	/// <summary><c>GaSelectionMigration</c> class represent migration operation which uses standard selection operations to choose which chromosomes will be migrated.
	///
	/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// Because this genetic operation is stateless all public method are thread-safe.</summary>
	class GaSelectionMigration : public GaMigrationOperation
	{

	public:

		/// <summary>More details are given in specification of <see cref="GaMigrationOperation::Prepare" /> method.
		///
		/// This method is thread-safe.</summary>
		GAL_API
		virtual void GACALL Prepare(Population::GaPopulation& population,
			GaMigrationPort& port,
			const GaMigrationParams& parameters,
			const GaMigrationConfig& configuration,
			int branchCount) const;

		/// <summary>More details are given in specification of <see cref="GaMigrationOperation::Clear" /> method.
		///
		/// This method is thread-safe.</summary>
		GAL_API
		virtual void GACALL Clear(Population::GaPopulation& population,
			GaMigrationPort& port,
			const GaMigrationParams& parameters,
			const GaMigrationConfig& configuration,
			int branchCount) const;

		/// <summary>More details are given in specification of <see cref="GaMigrationOperation::Update" /> method.
		///
		/// This method is thread-safe.</summary>
		GAL_API
		virtual void GACALL Update(Population::GaPopulation& population,
			GaMigrationPort& port,
			const GaMigrationParams& parameters,
			const GaMigrationConfig& configuration,
			int branchCount) const;

		/// <summary>More details are given in specification of <see cref="GaMigrationOperation::operator ()" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual void GACALL operator ()(Population::GaPopulation& population,
			GaMigrationPort& port,
			const GaMigrationParams& parameters,
			const GaMigrationConfig& configuration,
			Common::Workflows::GaBranch* branch) const
			{ Exec( population, port, (const GaSelectionMigrationParams&)parameters, (const GaSelectionMigrationConfig&)configuration, branch ); }

		/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
		///
		/// This method is thread-safe.</summary>
		/// <returns>Method always returns <c>NULL</c>.</returns>
		virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

		/// <summary>More details are given in specification of <see cref="GaOperation::CreateConfiguration" /> method.
		///
		/// This method is thread-safe.</summary>
		/// <returns>Method returns new instance of <see cref="GaSelectionMigrationConfig" /> class.</returns>
		virtual Common::GaConfiguration* GACALL CreateConfiguration() const { return new GaSelectionMigrationConfig(); }

	protected:

		/// <summary><c>Exec</c> method executes operation. Paramenters has same meaning that is defined by <c>operator ()</c>.</summary>
		GAL_API
		void GACALL Exec(Population::GaPopulation& population,
			GaMigrationPort& port,
			const GaSelectionMigrationParams& parameters,
			const GaSelectionMigrationConfig& configuration,
			Common::Workflows::GaBranch* branch) const;

	};

} // Coevolution

#endif // __GA_MIGRATIONS_H__

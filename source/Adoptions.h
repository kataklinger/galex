
/*! \file Adoptions.h
    \brief This file declares classes that represent adoption operations.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#ifndef __GA_ADOPTIONS_H__
#define __GA_ADOPTIONS_H__

#include "Coevolution.h"

namespace Coevolution
{

	/// <summary><c>GaReplacementAdoptionParams</c> class represent paramenters for daoption operations that uses standard replacement operations.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaReplacementAdoptionParams : public GaAdoptionParams
	{

	protected:

		/// <summary>ID of the population tag that stores data required by replacement operation.</summary>
		int _replacementInputTagID;

	public:

		/// <summary>This constructor initializes paramenters with tag that stores data required by replacement operation.</summary>
		/// <param name="replacementInputTagID">ID of the population tag.</param>
		GaReplacementAdoptionParams(int replacementInputTagID) : _replacementInputTagID(replacementInputTagID) { }

		/// <summary>This constructor initializes paramenters without specified tag that should store data required by by replacment operation.</summary>
		GaReplacementAdoptionParams() : _replacementInputTagID(-1) { }

		/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
		///
		/// This method is not thread-safe.</summary>
		virtual Common::GaParameters* GACALL Clone() const { return new GaReplacementAdoptionParams( *this ); }

		/// <summary><c>SetReplacementInputTagID</c> method sets population tag that will store data required by replacement operation.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="tagID">ID of the tag.</param>
		inline void GACALL SetReplacementInputTagID(int tagID) { _replacementInputTagID = tagID; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns ID of the population tag that stores data required by replacement operation.</returns>
		inline int GACALL GetReplacementInputTagID() const { return _replacementInputTagID; }

	};

	/// <summary><c>GaReplacementAdoptionConfig</c> class represent configuration for adoption operations that uses standard replacement operations.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaReplacementAdoptionConfig : public GaAdoptionConfig
	{

	protected:

		/// <summary>Underlying replacement operation which decides what chromosomes should be replaced with migrated chromosomes.</summary>
		Population::GaReplacementSetup _replacement;

	public:

		/// <summary>This constructor initializes adoption operation with underlaying replacement operation.</summary>
		/// <param name="replacement">underlying replacement operation which decides what chromosomes should be replaced with migrated chromosomes.</param>
		GaReplacementAdoptionConfig(const Population::GaReplacementSetup& replacement) : _replacement(replacement) { }

		/// <summary>This constructor initializes migration operation without underlaying replacement operation.</summary>
		GaReplacementAdoptionConfig() { }

		/// <summary>More details are given in specification of <see cref="GaConfiguration::Clone" /> method.
		///
		/// This method is not thread-safe.</summary>
		virtual GaConfiguration* GACALL Clone() const { return new GaReplacementAdoptionConfig( *this ); }

		/// <summary><c>SetReplacement</c> method sets underlying replacement operation that is going to be used to decides what chromosomes should be replaced with migrated chromosomes.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="replacement">replacement operation.</param>
		inline void GACALL SetReplacement(Population::GaReplacementSetup& replacement)  { _replacement = replacement; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns replacement operation which decides what chromosomes should be replaced with migrated chromosomes.</returns>
		inline Population::GaReplacementSetup& GACALL GetReplacement() { return _replacement; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns replacement operation which decides what chromosomes should be replaced with migrated chromosomes.</returns>
		inline const Population::GaReplacementSetup& GACALL GetReplacement() const { return _replacement; }

	};

	/// <summary><c>GaReplacementAdoption</c> class represent which uses standard replacement operations to decides what chromosomes should be replaced with migrated chromosomes.
	///
	/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// Because this genetic operation is stateless all public method are thread-safe.</summary>
	class GaReplacementAdoption : public GaAdoptionOperation
	{

	public:

		/// <summary>More details are given in specification of <see cref="GaAdoptionOperation::Prepare" /> method.
		///
		/// This method is thread-safe.</summary>
		GAL_API
		virtual void GACALL Prepare(GaMigrationPort& port,
			Population::GaPopulation& population,
			const GaAdoptionParams& parameters,
			const GaAdoptionConfig& configuration,
			int branchCount) const;

		/// <summary>More details are given in specification of <see cref="GaAdoptionOperation::Clear" /> method.
		///
		/// This method is thread-safe.</summary>
		GAL_API
		virtual void GACALL Clear(GaMigrationPort& port,
			Population::GaPopulation& population,
			const GaAdoptionParams& parameters,
			const GaAdoptionConfig& configuration,
			int branchCount) const;

		/// <summary>More details are given in specification of <see cref="GaAdoptionOperation::Update" /> method.
		///
		/// This method is thread-safe.</summary>
		GAL_API
		virtual void GACALL Update(GaMigrationPort& port,
			Population::GaPopulation& population,
			const GaAdoptionParams& parameters,
			const GaAdoptionConfig& configuration,
			int branchCount) const;

		/// <summary>More details are given in specification of <see cref="GaAdoptionOperation::operator ()" /> method.
		///
		/// This method is thread-safe.</summary>
		virtual void GACALL operator ()(GaMigrationPort& port,
			Population::GaPopulation& population,
			const GaAdoptionParams& parameters,
			const GaAdoptionConfig& configuration,
			Common::Workflows::GaBranch* branch) const
			{ Exec( port, population, (const GaReplacementAdoptionParams&)parameters, (const GaReplacementAdoptionConfig&)configuration, branch ); }

		/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
		///
		/// This method is thread-safe.</summary>
		/// <returns>Method always returns <c>NULL</c>.</returns>
		virtual Common::GaParameters* GACALL CreateParameters() const { return NULL; }

		/// <summary>More details are given in specification of <see cref="GaOperation::CreateConfiguration" /> method.
		///
		/// This method is thread-safe.</summary>
		/// <returns>Method returns new instance of <see cref="GaReplacementAdoptionConfig" /> class.</returns>
		virtual Common::GaConfiguration* GACALL CreateConfiguration() const { return new GaReplacementAdoptionConfig(); }

	protected:

		/// <summary><c>Exec</c> method executes operation. Paramenters has same meaning that is defined by <c>operator ()</c>.</summary>
		GAL_API
		void GACALL Exec(GaMigrationPort& port,
			Population::GaPopulation& population,
			const GaReplacementAdoptionParams& parameters,
			const GaReplacementAdoptionConfig& configuration,
			Common::Workflows::GaBranch* branch) const;

	};

} // Coevolution

#endif // __GA_ADOPTIONS_H__

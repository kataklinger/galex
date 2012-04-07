
/*! \file Chromosome.h
    \brief This file declares interfaces, classes and datatypes that are needed to represent chromosomes in the library.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#ifndef __GA_CHROMOSOME_H__
#define __GA_CHROMOSOME_H__

#include "SmartPtr.h"

/// <summary>Contains interfaces, classes and datatypes that are needed to represent chromosomes in the library.</summary>
namespace Chromosome
{

	/// <summary><c>GaChromosomeParams</c> is base class for chromosomes parameters.</summary>
	class GaChromosomeParams : public Common::GaParameters { };

	/// <summary><c>GaChromosomeConfigBlock</c> represents base class for chromosome's configuration block (CCB). This class stores chromosome parameters.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaChromosomeConfigBlock
	{

	protected:

		/// <summary>Auto pointer to chromosome parameters.</summary>
		Common::Memory::GaAutoPtr<GaChromosomeParams> _chromosomeParams;

	public:

		/// <summary>This constructor initializes CCB with pointer to chromosome parameters. The constructor creates and stores copy of provided parameters' object.</summary>
		/// <param name="chromosomeParams">pointer to chromosome parameters.</param>
		GaChromosomeConfigBlock(const GaChromosomeParams* chromosomeParams) { SetChromosomeParams( chromosomeParams ); }

		/// <summary>This constructor initializes empty CCB.</summary>
		GaChromosomeConfigBlock() : _chromosomeParams(NULL) { }

		/// <summary>Copy constructor. The constructor creates and stored copy of chromosome parameters that ar stored in proveded CCB.</summary>
		/// <param name="rhs">source CCB which should be copied.</param>
		GaChromosomeConfigBlock(const GaChromosomeConfigBlock& rhs) { SetChromosomeParams( rhs._chromosomeParams.GetRawPtr() ); }

		/// <summary>Virtual destructor must be defined because this is base class.</summary>
		virtual ~GaChromosomeConfigBlock() { }

		/// <summary><c>Clone</c> method creates copy object of the CCB. Caller is responsible for memory allocated for the new object.
		///
		/// This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to newly created copy of CCB.</returns>
		virtual GaChromosomeConfigBlock* GACALL Clone() const { return new GaChromosomeConfigBlock( *this ); }

		/// <summary><c>SetChromosomeParams</c> method sets new chromosome parameters. The method creates and stored copy of provided parameters.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="chromosomeParams">pointer to chromosomes parameters.</param>
		inline void GACALL SetChromosomeParams(const GaChromosomeParams* chromosomeParams) { _chromosomeParams = chromosomeParams ? (GaChromosomeParams*)chromosomeParams->Clone() : NULL; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to chromosome parameters sotred in CCB.</returns>
		inline GaChromosomeParams* GACALL GetChromosomeParams() { return _chromosomeParams.GetRawPtr(); }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns pointer to chromosome parameters sotred in CCB.</returns>
		inline const GaChromosomeParams* GACALL GetChromosomeParams() const { return _chromosomeParams.GetRawPtr(); }

		/// <summary><c>operator =</c> copies data from provided CCB.
		///
		/// This operator is not thread-safe.</summary>
		/// <param name="rhs">source CCB which should be copied.</param>
		/// <returns>Operator returns reference to <c>this</c> object.</returns>
		virtual GaChromosomeConfigBlock& GACALL operator =(const GaChromosomeConfigBlock& rhs)
		{
			SetChromosomeParams( rhs._chromosomeParams.GetRawPtr() );
			return *this;
		}

	};

	/// <summary><c>GaChromosome</c> is base class for chromosomes.
	///
	/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
	/// No public or private methods are thread-safe.</summary>
	class GaChromosome
	{

	public:

		/// <summary>This enum defines events raised by the framework when it performs mutation operation.</summary>
		enum GaMuataionEvent
		{
			/// <summary>This event is raised before the mutation operation is performed.</summary>
			GAME_PREPARE,

			/// <summary>This event is raised after the mutation operation is performed and when the framework decide to accept the mutation.</summary>
			GAME_ACCEPT,

			/// <summary>This event is raised after the mutation operation is performed and when the framework decide to reject the mutation.</summary>
			GAME_REJECT
		};

	protected:

		/// <summary>Smart pointer to CCB used by the chromosome.</summary>
		Common::Memory::GaSmartPtr<GaChromosomeConfigBlock> _configBlock;

	public:

		/// <summary>This constructor initializes chromosome with CCB that it should use.</summary>
		/// <param name="configBlock">smart pointer to CCB.</param>
		GaChromosome(Common::Memory::GaSmartPtr<GaChromosomeConfigBlock> configBlock) : _configBlock(configBlock) { }

		/// <summary>Copy constructor. The constructor copies reference to CCB from source chromosome.</summary>
		/// <param name="rhs">source chromosome that should be copied.</param>
		GaChromosome(const GaChromosome& rhs) : _configBlock(rhs._configBlock) { }

		/// <summary>This constructor initializes chromosome with empty CCB.</summary>
		GaChromosome() { }

		/// <summary>Virtual destructor must be defined because this is base class.</summary>
		virtual ~GaChromosome() { };

		/// <summary>Virtual copy constructor.</summary>
		/// <returns>Method returns smart pointer to created copy of the chromosome.</returns>
		virtual Common::Memory::GaSmartPtr<GaChromosome> GACALL Clone() const = 0;

		/// <summary><c>SetConfigBlock</c> method sets new CCB that should be used by the chromosome.
		///
		/// This method is not thread-safe.</summary>
		/// <param name="configBlock">pointer to new CCB.</param>
		inline void GACALL SetConfigBlock(Common::Memory::GaSmartPtr<GaChromosomeConfigBlock> configBlock) { _configBlock = configBlock; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns smart pointer to CCB used by the chromosome.</returns>
		inline Common::Memory::GaSmartPtr<GaChromosomeConfigBlock> GACALL GetConfigBlock() { return _configBlock; }

		/// <summary>This method is not thread-safe.</summary>
		/// <returns>Method returns smart pointer to CCB used by the chromosome.</returns>
		inline Common::Memory::GaSmartPtr<const GaChromosomeConfigBlock> GACALL GetConfigBlock() const { return _configBlock; }

		/// <summary><c>operator =</c> copies data from source chromosome.</summary>
		/// <param name="rhs">source chromosome that should be copied.</param>
		/// <returns>Method returns reference to <c>this</c> object.</returns>
		virtual GaChromosome& GACALL operator =(const GaChromosome& rhs)
		{
			_configBlock = rhs._configBlock;
			return *this;
		}

		/// <summary>This method is called by the framework when it performes mutation operation over the chromosome.</summary>
		/// <param name="e">event that was raised.</param>
		virtual void GACALL MutationEvent(GaMuataionEvent e) = 0;

	};

	/// <summary><c>GaChromosomePtr</c> type is instance of <see cref="GaSmartPtr" /> template class and represents smart pointer to a chromosome.
	/// Detailed description can be found in specification of <see cref="GaSmartPtr" /> template class.</summary>
	typedef Common::Memory::GaSmartPtr<GaChromosome> GaChromosomePtr;

	/// <summary><c>GaChromosomeConstPtr</c> type is instance of <see cref="GaSmartPtr" /> template class and represents smart pointer to a constant chromosome.
	/// Detailed description can be found in specification of <see cref="GaSmartPtr" /> template class.</summary>
	typedef Common::Memory::GaSmartPtr<const GaChromosome> GaChromosomeConstPtr;

} // Chromosome

#endif // __GA_CHROMOSOME_H__

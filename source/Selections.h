
/*! \file Selections.h
    \brief This file declares classes that represent selection operations.
*/

/*
 * 
 * website: http://kataklinger.com/
 * contact: me[at]kataklinger.com
 *
 */

#ifndef __GA_SELECTION_H__
#define __GA_SELECTION_H__

#include "PopulationOperations.h"

namespace Population
{
	/// <summary>Contains built-in selection operations.</summary>
	namespace SelectionOperations
	{

		/// <summary><c>GaTopSelection</c> class repesents selection operation that selects the first N chromosomes in the population.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaTopSelection : public GaSelectionOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaSelectionOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaPopulation& population,
				GaChromosomeGroup& output,
				const GaSelectionParams& parameters,
				const GaCouplingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaSelectionParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaSelectionParams(); }

		};

		/// <summary><c>GaBottomSelection</c> class repesents selection operation that selects the last N chromosomes in the population.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaBottomSelection : public GaSelectionOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaSelectionOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL operator ()(GaPopulation& population,
				GaChromosomeGroup& output,
				const GaSelectionParams& parameters,
				const GaCouplingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaSelectionParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaSelectionParams(); }

		};

		/// <summary><c>GaDuplicatesSelectionParams</c> class represents parametners for selection operations that allows user to control whether
		/// the chromosome duplicates are allowed in selection result set.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaDuplicatesSelectionParams : public GaSelectionParams
		{

		protected:

			/// <summary>ID of the tag that stores information whether the chromosome is selected.</summary>
			int _selectedTagID;

		public:

			/// <summary>This constructor initializes selection parameters with user-defined values.</summary>
			/// <param name="selectionSize">number of chromosomes which should be selected or produced.</param>
			/// <param name="crossoverBuffersTagID">ID of the tag that stores crossover buffers.
			/// This parameter should be set to negative value if selection operation should not produce offspring chromosomes.</param>
			/// <param name="selectedTagID">ID of the tag that stores information whether the chromosome is selected.
			/// This parameter should be set to negative value if duplicates in selection result set are allowed.</param>
			GaDuplicatesSelectionParams(int selectionSize,
				int crossoverBuffersTagID,
				int selectedTagID) : GaSelectionParams(selectionSize, crossoverBuffersTagID),
				_selectedTagID(selectedTagID) { }

			/// <summary>This constiructor initializes selection parametenrs with default values.
			/// Operation does not produce offspring chromosomes, selection size is 2 and duplicates are allowed.</summary>
			GaDuplicatesSelectionParams() : _selectedTagID(-1) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaDuplicatesSelectionParams( *this ); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns ID of the tag that stores information whether the chromosome is selected.</returns>
			inline int GACALL GetSelectedTagID() const { return _selectedTagID; }

			/// <summary><c>SetSelectedTagID</c> method sets tag that will be used to store information whether the chromosome is selected.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="tagID">ID of the tag.</param>
			inline int GACALL SetSelectedTagID(int tagID) { _selectedTagID = tagID; }

		};

		/// <summary><c>GaDuplicatesSelection</c> class is base class for selection operations that allows user to control whether
		/// the chromosome duplicates are allowed in selection result set.</summary>
		class GaDuplicatesSelection : public GaSelectionOperation
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaSelectionOperation::Prepare" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Prepare(GaPopulation& population,
				GaChromosomeGroup& output,
				const GaSelectionParams& parameters,
				const GaCouplingConfig& configuration,
				int branchCount) const;

			/// <summary>More details are given in specification of <see cref="GaSelectionOperation::Prepare" /> method.
			///
			/// This method is thread-safe.</summary>
			GAL_API
			virtual void GACALL Clear(GaPopulation& population,
				GaChromosomeGroup& output,
				const GaSelectionParams& parameters,
				const GaCouplingConfig& configuration,
				int branchCount) const;

		};

		/// <summary><c>GaRandomSelection</c> class represent selection operation that selects chromosomes randomly from the population.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaRandomSelection : public GaDuplicatesSelection
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaSelectionOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL operator ()(GaPopulation& population,
				GaChromosomeGroup& output,
				const GaSelectionParams& parameters,
				const GaCouplingConfig& configuration,
				Common::Workflows::GaBranch* branch) const { Exec( population, output, (const GaDuplicatesSelectionParams&)parameters, configuration, branch ); }

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaDuplicatesSelectionParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaDuplicatesSelectionParams(); }

		protected:

			/// <summary><c>Exec</c> method executes operation. Paramenters has same meaning that is defined by <c>operator ()</c>.</summary>
			GAL_API
			void GACALL Exec(GaPopulation& population,
				GaChromosomeGroup& output,
				const GaDuplicatesSelectionParams& parameters,
				const GaCouplingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

		};

		/// <summary><c>GaRouletteWheelSelection</c> class represents selection operation use roulette wheel mechanism.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaRouletteWheelSelection : public GaDuplicatesSelection
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaSelectionOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL operator ()(GaPopulation& population,
				GaChromosomeGroup& output,
				const GaSelectionParams& parameters,
				const GaCouplingConfig& configuration,
				Common::Workflows::GaBranch* branch) const { Exec( population, output, (const GaDuplicatesSelectionParams&)parameters, configuration, branch ); }

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaDuplicatesSelectionParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaDuplicatesSelectionParams(); }

		protected:

			/// <summary><c>Exec</c> method executes operation. Paramenters has same meaning that is defined by <c>operator ()</c>.</summary>
			GAL_API
			void GACALL Exec(GaPopulation& population,
				GaChromosomeGroup& output,
				const GaDuplicatesSelectionParams& parameters,
				const GaCouplingConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

		};

		/// <summary><c>GaTournamentSelectionParams</c> class represent parameters for tournament selection operation.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaTournamentSelectionParams : public GaDuplicatesSelectionParams
		{

		public:

			/// <summary>Underlying selection mechanism.</summary>
			enum GaTournamentSelectionType
			{

				/// <summary>Radnom selection mechanism.</summary>
				GATST_RANDOM_SELECTION,

				/// <summary>Roulette wheel selection mechanism.</summary>
				GATST_ROULETTE_WHEEL_SELECTION

			};

		protected:

			/// <summary>Number of selections that should be performed for single place in result set.</summary>
			int _numberOfSelections;

			/// <summary>Selection mechanism.</summary>
			GaTournamentSelectionType _type;

		public:

			/// <summary>This constoructo initializes selection parameters with user-defined values.</summary>
			/// <param name="selectionSize">number of chromosomes which should be selected or produced.</param>
			/// <param name="crossoverBuffersTagID">ID of the tag that stores crossover buffers.
			/// This parameter should be set to negative value if selection operation should not produce offspring chromosomes.</param>
			/// <param name="selectedTagID">ID of the tag that stores information whether the chromosome is selected.
			/// This parameter should be set to negative value if duplicates in selection result set are allowed.</param>
			/// <param name="numberOfSelections">number of selections that should be performed for single place in result set.</param>
			/// <param name="type">selection mechanism</param>
			GaTournamentSelectionParams(int selectionSize,
				int crossoverBuffersTagID,
				int selectedTagID,
				int numberOfSelections,
				GaTournamentSelectionType type) : GaDuplicatesSelectionParams(selectionSize, crossoverBuffersTagID, selectedTagID),
				_numberOfSelections(numberOfSelections),
				_type(type) { }

			/// <summary>This constructor initiailzes selection paramenters with default values.</summary>
			/// Operation does not produce offspring chromosomes, selection size is 2, duplicates are allowed and roulette wheel mechanism is used.</summary>
			GaTournamentSelectionParams() : _numberOfSelections(2),
				_type(GATST_ROULETTE_WHEEL_SELECTION) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaTournamentSelectionParams( *this ); }

			/// <summary>This method is not thread=safe.</summary>
			/// <returns>Method returns number of selections that should be performed for single place in result set.</returns>
			inline int GACALL GetNumberOfSelections() const { return _numberOfSelections; }

			/// <summary><c>SetNumberOfSelections</c> method sets number of selections that should be performed for single place in result set.
			///
			/// This method is not thread=safe.</summary>
			/// <param name="number">number of selections.</param>
			/// <exception cref="GaArgumentOutOfRangeException" />Thrown if <c>number</c> is negative value or <c>0</c>.</exception>
			inline void GACALL SetNumberOfSelections(int number)
			{
				GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, number > 0, "number",
					"Number of chromosomes that should be produced must be greater then 0.", "Population" );

				_numberOfSelections = number;
			}

			/// <summary>This method is not thread=safe.</summary>
			/// <returns>Method returns underlying selection mechanism.</returns>
			inline GaTournamentSelectionType GACALL GetType() const { return _type; }

			/// <summary><c>SetType</c> method sets underlying selection mechanism.
			///
			/// This method is not thread=safe.</summary>
			/// <param name="type">selection mechanism.</param>
			inline void GACALL SetType(GaTournamentSelectionType type) { _type = type; }

		};

		/// <summary><c>GaTournamentSelectionConfig</c> class represent configuration for tournament selection operation.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaTournamentSelectionConfig : public GaCouplingConfig
		{

		private:

			/// <summary>Fitness comparator used for deciding which chromosome from choosen should enter result set.</summary>
			Fitness::GaFitnessComparatorSetup _fitnessComparator;

		public:

			/// <summary>This constructor initializes configuration object with mating operation and fitness comparator.</summary>
			/// <param name="fitnessComparator">fitness comparator used for deciding which chromosome from choosen should enter result set.</param>
			/// <param name="mating">reference to object that stores mating operation and its parameters and configuration.</param>
			GaTournamentSelectionConfig(const Fitness::GaFitnessComparatorSetup& fitnessComparator,
				const Chromosome::GaMatingSetup& mating) : GaCouplingConfig(mating),
				_fitnessComparator(fitnessComparator) { }

			/// <summary>This constructor initializes configuration with no specific mating operation or fitness comparator.</summary>
			GaTournamentSelectionConfig() { }

			/// <summary>More details are given in specification of <see cref="GaConfiguration::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaConfiguration* GACALL Clone() const { return new GaTournamentSelectionConfig( *this ); }

			/// <summary><c>CompareFitnesses</c> method comapres fitness using provided comparator and returns results.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="fitness1">the first fitness object.</param>
			/// <param name="fitness2">the second fitness object.</param>
			/// <returns>Method returns results obtained using provided fitness comparator. For more details see documentation of the comparator.</returns>
			inline int GACALL CompareFitnesses(const Fitness::GaFitness& fitness1,
				const Fitness::GaFitness& fitness2) const { return _fitnessComparator.GetOperation()( fitness1, fitness2, _fitnessComparator.GetParameters() ); };

			/// <summary><c>SetFitnessComparator</c> method sets comparator that will be used for deciding which chromosome from choosen should enter result set.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="comparator">comparator that will be used.</param>
			inline void GACALL SetFitnessComparator(const Fitness::GaFitnessComparatorSetup& comparator) { _fitnessComparator = comparator; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns fitness comparator used for deciding which chromosome from choosen should enter result set.</returns>
			inline Fitness::GaFitnessComparatorSetup& GACALL GetFitnessComparator() { return _fitnessComparator; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns fitness comparator used for deciding which chromosome from choosen should enter result set.</returns>
			inline const Fitness::GaFitnessComparatorSetup& GACALL GetFitnessComparator() const { return _fitnessComparator; }

		};

		/// <summary><c>GaTournamentSelection</c> class represents selection operation that performs several selection for each place in result set
		/// and then it chooses the best chromosome to enter the results. Underyling selection mechanisam can be random or roulette wheel.
		///
		/// This class has no built-in synchronizator, so <c>LOCK_OBJECT</c> and <c>LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// Because this genetic operation is stateless all public method are thread-safe.</summary>
		class GaTournamentSelection : public GaDuplicatesSelection
		{

		public:

			/// <summary>More details are given in specification of <see cref="GaSelectionOperation::operator ()" /> method.
			///
			/// This method is thread-safe.</summary>
			virtual void GACALL operator ()(GaPopulation& population,
				GaChromosomeGroup& output,
				const GaSelectionParams& parameters,
				const GaCouplingConfig& configuration,
				Common::Workflows::GaBranch* branch) const
				{ Exec( population, output, (const GaTournamentSelectionParams&)parameters, (const GaTournamentSelectionConfig&)configuration, branch ); }

			/// <summary>More details are given in specification of <see cref="GaOperation::CreateParameters" /> method.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns new instance of <see cref="GaTournamentSelectionParams" /> class.</returns>
			virtual Common::GaParameters* GACALL CreateParameters() const { return new GaTournamentSelectionParams(); }

		protected:

			/// <summary><c>Exec</c> method executes operation. Paramenters has same meaning that is defined by <c>operator ()</c>.</summary>
			GAL_API
			void GACALL Exec(GaPopulation& population,
				GaChromosomeGroup& output,
				const GaTournamentSelectionParams& parameters,
				const GaTournamentSelectionConfig& configuration,
				Common::Workflows::GaBranch* branch) const;

		};

	} // SelectionOperations
} // Population

#endif // __GA_SELECTION_H__

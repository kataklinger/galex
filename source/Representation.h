
/*! \file Representation.h
    \brief This file declares and implements classes that handles chromosomes representations.
*/

/*
 * 
 * website: N/A
 * contact: kataklinger@gmail.com
 *
 */

#ifndef __GA_REPRESENTATION_H__
#define __GA_REPRESENTATION_H__

#include "List.h"
#include "Tree.h"
#include "Chromosome.h"
#include "Alleles.h"

namespace Chromosome
{

	/// <summary>Contains built-in chromosome representations.</summary>
	namespace Representation
	{

		/// <summary></summary>
		typedef int GaGeneInterfaceID;

		/// <summary><c>GaGeneVisitorInterface</c> class is interface for iterating through the chromosome's genes.</summary>
		class GaGeneVisitorInterface
		{

		public:

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaGeneVisitorInterface() { }

			/// <summary><c>Next</c> method moves iterator to the next gene.</summary>
			virtual void GACALL Next() = 0;

			/// <summary><c>Previous</c> method moves iterator to the previous gene.</summary>
			virtual void GACALL Previous() = 0;

			/// <summary><c>ToFront</c> method moves iterator to the first gene of the chromosome.</summary>
			virtual void GACALL ToFront() = 0;

			/// <summary><c>ToBack</c> method moves iterator to the lst gene of the chromosome.</summary>
			virtual void GACALL ToBack() = 0;

			/// <summary><c>HasMore</c> method checks whether the chromosome has more genes after the current gene at which the iterator is located.</summary>
			/// <returns>Method returns <c>true</c> if the chromosome has more genes after the current.</returns>
			virtual bool GACALL HasMore() const = 0;

		};

		/// <summary><c>GaGeneVisitorBase</c> is base class for iterators that exposes interface for accessing current gene of the chromosome.</summary>
		/// <param name="GENE_INTERFACE">interface that provides access to chromosome's genes.</param>
		template<typename GENE_INTERFACE>
		class GaGeneVisitorBase : public GaGeneVisitorInterface
		{

		public:

			/// <summary><c>GetInterface</c> method queries interface that provides access to the current gene of the chromosome.</summary>
			/// <returns>Method returns reference that provides access to the current gene of the chromosome.</returns>
			virtual GENE_INTERFACE& GACALL GetInterface() const = 0;

		};

		/// <summary><c>GaGeneVisitor</c> template represent gene iterator for chromosomes which uses array-like collections for storing genes.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="GENE_INTERFACE">interface that provides access to chromosome's genes.</param>
		/// <param name="GENE_TYPE">type of chromosome's genes.</param>
		/// <param name="GENE_SET_TYPE">type of collection used by chromosome to store genes.</param>
		template<typename GENE_INTERFACE,
			typename GENE_TYPE,
			template<typename> class GENE_SET_TYPE>
		class GaGeneVisitor : public GaGeneVisitorBase<GENE_INTERFACE>
		{

		protected:

			/// <summary>Pointer to collection that stores chromosome's genes.</summary>
			GENE_SET_TYPE<GENE_TYPE>* _genes;

			/// <summary>Position of interator in the gene collection.</summary>
			int _currentPosition;

		public:

			/// <summary>This constructor initializes iterator and binds it to the gene collection.</summary>
			/// <param name="genes">gene collection that iterator should iterate through.</param>
			GaGeneVisitor(GENE_SET_TYPE<GENE_TYPE>* genes) : _genes(genes),
				_currentPosition(0) { }

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaGeneVisitor() { }

			/// <summary>More details are given in specification of <see cref="GaGeneVisitorBase::GetInterface" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GENE_INTERFACE& GACALL GetInterface() const { return (*_genes)[ _currentPosition ]; }

			/// <summary>More details are given in specification of <see cref="GaGeneVisitorInterface::Next" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Next() { _currentPosition++; }

			/// <summary>More details are given in specification of <see cref="GaGeneVisitorInterface::Previous" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Previous() { _currentPosition--; }

			/// <summary>More details are given in specification of <see cref="GaGeneVisitorInterface::ToFront" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL ToFront() { _currentPosition = 0; }

			/// <summary>More details are given in specification of <see cref="GaGeneVisitorInterface::ToBack" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL ToBack() { _currentPosition = _genes->GetSize() - 1; }

			/// <summary>More details are given in specification of <see cref="GaGeneVisitorInterface::ToBack" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual bool GACALL HasMore() const { return _currentPosition < _genes->GetSize() && _currentPosition >= 0; }

		};

		/// <summary><c>GaGeneVisitor</c> template specialisation represent gene iterator for chromosomes which uses list for storing genes.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="GENE_INTERFACE">interface that provides access to chromosome's genes.</param>
		/// <param name="GENE_TYPE">type of chromosome's genes.</param>
		template<typename GENE_INTERFACE,
			typename GENE_TYPE>
		class GaGeneVisitor<GENE_INTERFACE, GENE_TYPE, Common::Data::GaList> : public GaGeneVisitorBase<GENE_INTERFACE>
		{

		protected:

			/// <summary>Pointer to list that stores chromosome's genes.</summary>
			Common::Data::GaList<GENE_TYPE>* _genes;

			/// <summary>Node that stores gene at which the iterator is located.</summary>
			Common::Data::GaListNode<GENE_TYPE>* _currentPosition;

		public:

			/// <summary>This constructor initializes iterator and binds it to the gene collection.</summary>
			/// <param name="genes">gene collection that iterator should iterate through.</param>
			GaGeneVisitor(Common::Data::GaList<GENE_TYPE>* genes) : _genes(genes) { _currentPosition = genes->GetHead(); }

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaGeneVisitor() { }

			/// <summary>More details are given in specification of <see cref="GaGeneVisitorBase::GetInterface" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GENE_INTERFACE& GACALL GetInterface() const { return _currentPosition->GetData(); }

			/// <summary>More details are given in specification of <see cref="GaGeneVisitorInterface::Next" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Next() { _currentPosition = _currentPosition->GetNext(); }

			/// <summary>More details are given in specification of <see cref="GaGeneVisitorInterface::Previous" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Previous() { _currentPosition = _currentPosition->GetPrevious(); }

			/// <summary>More details are given in specification of <see cref="GaGeneVisitorInterface::ToFront" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL ToFront() { _currentPosition = _genes->GetHead(); }

			/// <summary>More details are given in specification of <see cref="GaGeneVisitorInterface::ToBack" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL ToBack() { _currentPosition = _genes->GetTail(); }

			/// <summary>More details are given in specification of <see cref="GaGeneVisitorInterface::HasMore" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual bool GACALL HasMore() const { return _currentPosition != NULL; }

		};

		/// <summary><c>GaGeneVisitorCreator</c> template defines interface for factory classes of gene visitors.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="GENE_SET_TYPE">type of collection used by chromosome to store genes.</param>
		/// <param name="VALUE_TYPE">type of value stored in the chromosome's genes.</param>
		/// <param name="GENE_INTERFACE">interface that provides access to chromosome's genes.</param>
		/// <param name="GENE_TYPE">type of chromosome's genes.</param>
		template<template<typename> class GENE_SET_TYPE,
			typename VALUE_TYPE,
			template<typename> class GENE_TYPE,
			typename GENE_INTERFACE>
		struct GaGeneVisitorCreator
		{

			/// <summary><c>Create</c> method creates gene visitor for specified gene collection.
			///
			/// This method is thread-safe.</summary>
			/// <param name="genes">pointer to collection that stores chromosome's gene which should be iterated.</param>
			static inline GaGeneVisitorBase<GENE_INTERFACE>* Create(GENE_SET_TYPE<GENE_TYPE<VALUE_TYPE> >* genes) { return NULL; }
		};

		/// <summary><c>GENE_VISITOR_CREATOR</c> macro generates gene visitor factory class from template.</summary>
		/// <param name="GTYPE">type of genes in the chromosome.</param>
		/// <param name="GINTERFACE">type of interface that provides access to chromosome's genes.</param>
		#define GENE_VISITOR_CREATOR(GTYPE, GINTERFACE) \
			template<template<typename> class GENE_SET_TYPE, \
				typename VALUE_TYPE> \
			struct GaGeneVisitorCreator<GENE_SET_TYPE, VALUE_TYPE, GTYPE, GINTERFACE> { \
				static inline GaGeneVisitorBase<GINTERFACE>* Create(GENE_SET_TYPE<GTYPE<VALUE_TYPE> >* genes) \
				{ return new GaGeneVisitor<GINTERFACE, GTYPE<VALUE_TYPE>, GENE_SET_TYPE>( genes ); } };

		/// <summary><c>GaGene</c> class represent simple chromosome's gene that stores only one value.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of value stored in the gene.</param>
		template<typename VALUE_TYPE>
		class GaGene
		{

		public:

			/// <summary>Type of value stored in the gene.</summary>
			typedef VALUE_TYPE GaValueType;

		protected:

			/// <summary>Value stored in the gene.</summary>
			GaValueType _gValue;

		public:

			/// <summary>This constructor initializes gene with value it should store.</summary>
			/// <param name="gValue">value that gene should store.</param>
			GaGene(const GaValueType& gValue) : _gValue(gValue) { }

			/// <summary>This constructor initializes gene with default value for the type.</summary>
			GaGene() : _gValue() { }

			/// <summary><c>SetValue</c> method stores specified value to the gene.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">value that should be stored.</param>
			inline void GACALL SetValue(const GaValueType& value) { _gValue = value; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns value stored in the gene.</returns>
			inline const GaValueType& GetValue() const { return _gValue; }

			/// <summary><c>operator ==</c> compares values of two genes.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="rhs">reference to the second gene agains whose value the comparision is performed.</param>
			/// <returns>Method returns <c>true</c> if value of the two genes are equal.</returns>
			inline bool GACALL operator ==(const GaGene& rhs) const { return _gValue == rhs._gValue; }

			/// <summary><c>operator ==</c> compares values of two genes.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="rhs">reference to the second gene agains whose value the comparision is performed.</param>
			/// <returns>Method returns <c>true</c> if value of the two genes are not equal.</returns>
			inline bool GACALL operator !=(const GaGene& rhs) const { return _gValue != rhs._gValue; }

		};

		/// <summary><c>GaLocusBase</c> class carries information about function of a gene in a chromosome.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaLocusBase
		{

		protected:

			/// <summary>Function of the gene in the chromosome.</summary>
			int _locus;

		public:

			/// <summary>Unique interface number.</summary>
			static const GaGeneInterfaceID IID = 0x1;

			/// <summary>This constructor initializes gene with its function in the chromosome.</summary>
			/// <param name="locus">function of the gene in the chromosome.</param>
			GaLocusBase(int locus) : _locus(locus) { }

			/// <summary>This constructor initializes gene with default function in the chromosome.</summary>
			GaLocusBase() : _locus(0) { }

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaLocusBase() { }

			/// <summary><c>SetLocus</c> method sets function that gene will in the chromosome.
			///
			/// This method is not tread-safe.</summary>
			/// <param name="locus">function of the gene.</param>
			inline void GACALL SetLocus(int locus) { _locus = locus; }

			/// <summary>This method is not tread-safe.</summary>
			/// <returns>Method returns function of the gene in the chromosome.</returns>
			inline int GACALL GetLocus() const { return _locus; }


		};

		/// <summary><c>GaLocusGene</c> class represent chromosome's genes that beside values also store their functions in a chromosome.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of value stored in the gene.</param>
		template<typename VALUE_TYPE>
		class GaLocusGene : virtual public GaGene<VALUE_TYPE>,
			public GaLocusBase
		{

		public:

			/// <summary>This constructor initializes gene with value it should store and specified gene function in the chromosome.</summary>
			/// <param name="value">value that gene should store.</param>
			/// <param name="locus">function of the gene in the chromosome.</param>
			GaLocusGene(const GaValueType& value,
				int locus) : GaGene(value),
				GaLocusBase(locus) { }

			/// <summary>This constructor initializes gene with its function in the chromosome and stores default value.</summary>
			/// <param name="locus">function of the gene in the chromosome.</param>
			GaLocusGene(int locus) : GaLocusBase(locus) { }

			/// <summary>This constructor initializes gene with default gene function in the chromosome and stores default value.</summary>
			GaLocusGene() { }

			/// <summary><c>operator ==</c> comares two chromosome's genes.
			///
			/// This method is not tread-safe.</summary>
			/// <param name="rhs">gene agains comparison is being performed.</param>
			/// <returns>Method returns <c>true</c> if two genes are equal.</returns>
			inline bool GACALL operator ==(const GaLocusGene& rhs) const { return _locus == rhs._locus && GaGene::operator ==( rhs ); }

			/// <summary><c>operator !=</c> comares two chromosome's genes.
			///
			/// This method is not tread-safe.</summary>
			/// <param name="rhs">gene agains comparison is being performed.</param>
			/// <returns>Method returns <c>true</c> if two genes are not equal.</returns>
			inline bool GACALL operator !=(const GaLocusGene& rhs) const { return _locus != rhs._locus || GaGene::operator !=( rhs ); }

		};

		GENE_VISITOR_CREATOR(GaLocusGene, GaLocusBase)

		/// <summary><c>GaAlleleSetBase</c> interface provides mechanism to manipulate value stored in the gene using allele set.</summary>
		class GaAlleleSetBase
		{

		public:

			/// <summary>Unique interface number.</summary>
			static const GaGeneInterfaceID IID = 0x2;

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaAlleleSetBase() { }

			/// <summary><c>Flip</c> method changes value stored in the gene to a radnom value from allele set.</summary>
			virtual void GACALL Flip() = 0;

			/// <summary><c>Inverse</c> method invert stored value in the chromosome using provided allele set.</summary>
			virtual void GACALL Inverse() = 0;

		};

		/// <summary><c>GaAlleleGene</c> class represent chromosome's genes that beside values also store allele set that defines posible values it can store.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of value stored in the gene.</param>
		template<typename VALUE_TYPE>
		class GaAlleleGene : virtual public GaGene<VALUE_TYPE>,
			public GaAlleleSetBase
		{

		protected:

			/// <summary>Allese set that defines posible values that can be stored by the gene.</summary>
			GaAlleleSet<GaValueType>* _allele;

		public:

			/// <summary>This constructor initializes gene with specified allele set and stores closest value to specified value that exists in the set.</summary>
			/// <param name="value">value that gene should store.</param>
			/// <param name="allele">allele set of the gene.</param>
			GaAlleleGene(const GaValueType& value,
				GaAlleleSet<GaValueType>* allele) : _allele(allele) { _allele->ClosestValue( _gValue ); }

			/// <summary>This constructor initializes gene with specified allele set and stores random value generated by the set.</summary>
			/// <param name="allele">allele set of the gene.</param>
			GaAlleleGene(GaAlleleSet<GaValueType>* allele) : _allele(allele) { _allele->Generate( _gValue ); }

			/// <summary>This constructor initializes gene without allele set and stores default value.</summary>
			GaAlleleGene() : _allele(NULL) { }

			/// <summary>More details are given in specification of <see cref="GaAlleleSetBase::Flip" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Flip() { _allele->Generate( _gValue ); }

			/// <summary>More details are given in specification of <see cref="GaAlleleSetBase::Flip" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Inverse() { _allele->Inverse( _gValue ); }

			/// <summary><c>SetAlleleSet</c> method sets allese set that defines posible values that can be stored by the gene.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="allele">new allele set for the gene.</param>
			inline void GACALL SetAlleleSet(GaAlleleSet<GaValueType>* allele) { _allele = allele; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns to allese set that defines posible values that can be stored by the gene.</returns>
			inline GaAlleleSet<GaValueType>& GetAlleleSet() { return *_allele; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns to allese set that defines posible values that can be stored by the gene.</returns>
			inline const GaAlleleSet<GaValueType>& GetAlleleSet() const { return *_allele; }

			/// <summary><c>operator ==</c> comares two chromosome's genes.
			///
			/// This method is not tread-safe.</summary>
			/// <param name="rhs">gene agains comparison is being performed.</param>
			/// <returns>Method returns <c>true</c> if two genes are equal.</returns>
			inline bool GACALL operator ==(const GaAlleleGene& rhs) const { return _allele == rhs._allele && GaGene::operator ==( rhs ); }

			/// <summary><c>operator !=</c> comares two chromosome's genes.
			///
			/// This method is not tread-safe.</summary>
			/// <param name="rhs">gene agains comparison is being performed.</param>
			/// <returns>Method returns <c>true</c> if two genes are not equal.</returns>
			inline bool GACALL operator !=(const GaAlleleGene& rhs) const { return _allele != rhs._allele || GaGene::operator !=( rhs ); }

		};

		GENE_VISITOR_CREATOR(GaAlleleGene, GaAlleleSetBase)

		/// <summary><c>GaDominanceBase</c> interface provides methods for handling dominant and recessive genes.</summary>
		class GaDominanceBase
		{

		public:

			/// <summary>Unique interface number.</summary>
			static const GaGeneInterfaceID IID = 0x3;

			/// <summary>Virtual destructor must be defined because this is base class.</summary>
			virtual ~GaDominanceBase() { }

			/// <summary><c>Dominate</c> method checks two genes and select dominant one.</summary>
			/// <param name="second">reference to second gene which is compared against this one.</param>
			/// <returns>Method returns:
			/// <br/> a) 0 if none of the genes are selected as dominant,
			/// <br/> b) 1 if the first gene is selected as dominant,
			/// <br/> c) 2 if the seconf gene is selected as dominant.</returns>
			virtual int GACALL Dominate(GaDominanceBase& second) = 0;

		};

		/// <summary><c>GaDoubleDominanceGene</c> class represent chromosome's genes that stores pair of values and implements interface to determine dominance.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of value stored in the gene.</param>
		template<typename VALUE_TYPE>
		class GaDoubleDominanceGene : virtual public GaGene<VALUE_TYPE>,
			public GaDominanceBase
		{

		protected:

			/// <summary>The first value of the dominance pair.</summary>
			GaValueType _dValue1;

			/// <summary>The second value of the dominance pair.</summary>
			GaValueType _dValue2;

		public:

			/// <summary>This constructor initializes gene and store provided values in it.</summary>
			/// <param name="dValue1">the first value of the dominance pair.</param>
			/// <param name="dValue2">the second value of the dominance pair.</param>
			GaDoubleDominanceGene(const GaValueType& dValue1,
				const GaValueType& dValue2) : _dValue1(dValue1),
				_dValue2(_dValue2) { }

			/// <summary>This constructor initializes gene stores default values.</summary>
			GaDoubleDominanceGene() : _dValue1(),
				_dValue2() { }

			/// <summary><c>Dominate</c> method ignores provided parameter and performs dominance comparison on itself.
			/// More details are given in specification of <see cref="GaDominanceBase::Dominate" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual int GACALL Dominate(GaDominanceBase& second)
			{
				// determin dominant gene
				if( _dValue1 >= _dValue2 )
				{
					_gValue = _dValue1;
					return 1;
				}

				_gValue = _dValue2;
				return 2;
			}

			/// <summary><c>SetDValue1</c> method stores specified value as the first value of the dominance pair.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">value that should be stored.</param>
			inline void GACALL SetDValue1(const GaValueType& value) { _dValue1 = value; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns the first value of the dominance pair.</returns>
			inline const GaValueType& GACALL GetDValue1() const { return _dValue1; }

			/// <summary><c>SetDValue2</c> method stores specified value as the second value of the dominance pair.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="value">value that should be stored.</param>
			inline void GACALL SetDValue2(const GaValueType& value) { _dValue2 = value; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns the second value of the dominance pair.</returns>
			inline const GaValueType& GACALL GetDValue2() const { return _dValue2; }

			/// <summary><c>operator ==</c> comares two chromosome's genes.
			///
			/// This method is not tread-safe.</summary>
			/// <param name="rhs">gene agains comparison is being performed.</param>
			/// <returns>Method returns <c>true</c> if two genes are equal.</returns>
			inline bool GACALL operator ==(const GaDoubleDominanceGene& rhs) const { return _dValue1 == rhs._dValue1 && _dValue2 == rhs._dValue2 && GaGene::operator ==( rhs ); }

			/// <summary><c>operator !=</c> comares two chromosome's genes.
			///
			/// This method is not tread-safe.</summary>
			/// <param name="rhs">gene agains comparison is being performed.</param>
			/// <returns>Method returns <c>true</c> if two genes are not equal.</returns>
			inline bool GACALL operator !=(const GaDoubleDominanceGene& rhs) const { return _dValue1 != rhs._dValue1 || _dValue2 != rhs._dValue2 || GaGene::operator !=( rhs ); }

		};

		GENE_VISITOR_CREATOR(GaDoubleDominanceGene, GaDominanceBase)

		/// <summary><c>GaLocusDominance</c> is base class for genes thar stores additional inforamtion whether the they are domiant genes
		/// for the specifed function in the chromosome.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of value stored in the gene.</param>
		class GaLocusDominance : public GaLocusBase
		{

		protected:

			/// <summary>Defines possible states of gene dominance falg.</summary>
			enum GaGeneDominanceState
			{

				/// <summary>Indicates that gene dominance is not yet determined.</summary>
				GAGDS_UNDETERMINED,

				/// <summary>Indicates that gene is recessive.</summary>
				GAGDS_RECESSIVE,

				/// <summary>Indicated that gene is dominant.</summary>
				GAGDS_DOMINANT

			};

			/// <summary>Indicates whether the gene is dominant in the chromosome.</summary>
			GaGeneDominanceState _dominant;

		public:

			/// <summary>This constructor initializes gene with its function in the chromosome.</summary>
			/// <param name="locus">function of the gene in the chromosome.</param>
			GaLocusDominance(int locus) : GaLocusBase(locus),
				_dominant(GAGDS_UNDETERMINED) { }

			/// <summary>This constructor initializes gene with default function in the chromosome and stores default value.</summary>
			/// <param name="locus">function of the gene in the chromosome.</param>
			GaLocusDominance() : _dominant(GAGDS_UNDETERMINED) { }

			/// <summary><c>ClearDominance</c> function sets gene dominance to undetermined state.
			///
			/// This method is not thread-safe.</summary>
			inline void GACALL ClearDominance() { _dominant = GAGDS_UNDETERMINED; }

			/// <summary><c>SetDominace</c> method marks gene as recessive or dominant for the function in chromosome.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="dominant">if this parameter is set to <c>true</c> gene is marked as dominant gene, otherwise it is marked as reccesive.</param>
			inline void GACALL SetDominace(bool dominant) { _dominant = dominant ? GAGDS_DOMINANT : GAGDS_RECESSIVE; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if the gene is marked as dominant for the function in chromosome.</returns>
			inline bool GACALL IsDominant() const { return _dominant == GAGDS_DOMINANT; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if the gene is marked as recessive for the function in chromosome.</returns>
			inline bool GACALL IsRecessive() const { return _dominant == GAGDS_RECESSIVE; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns <c>true</c> if the gene for the function in chromosome has not been determined.</returns>
			inline bool GACALL IsUndetermined() const { return _dominant == GAGDS_UNDETERMINED; }

		};

		/// <summary><c>GaLocusGene</c> class represent chromosome's genes that beside values also store their functions in a chromosome and whether the gene is dominant or recessive.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		template<typename VALUE_TYPE>
		class GaLocusDominanceGene : virtual public GaGene<VALUE_TYPE>,
			public GaLocusDominance,
			public GaDominanceBase
		{

		public:

			/// <summary>This constructor initializes gene with value it should store and specified gene function in the chromosome.</summary>
			/// <param name="value">value that gene should store.</param>
			/// <param name="locus">function of the gene in the chromosome.</param>
			GaLocusDominanceGene(const GaValueType& value,
				int locus) : GaLocusGene(value, locus) { }

			/// <summary>This constructor initializes gene with its function in the chromosome and stores default value.</summary>
			/// <param name="locus">function of the gene in the chromosome.</param>
			GaLocusDominanceGene(int locus) : GaLocusGene(locus) { }

			/// <summary>This constructor initializes gene with default gene function in the chromosome and stores default value.</summary>
			GaLocusDominanceGene()  { }

			/// <summary>More details are given in specification of <see cref="GaDominanceBase::Dominate" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual int GACALL Dominate(GaDominanceBase& second)
			{
				GaLocusDominanceGene& s = (GaLocusDominanceGene&)second;

				// are the both genes represnt the same function in the chromosome?
				if( _locus == s._locus && !( _dominant == GAGDS_RECESSIVE && s._dominant == GAGDS_RECESSIVE ) )
				{
					// determin dominant gene
					GaGeneDominanceState dom = _gValue >= s._gValue ? GAGDS_DOMINANT : GAGDS_RECESSIVE;

					// store dominance results to the first gene
					if( _dominant != GAGDS_RECESSIVE )
						_dominant = dom;

					// store dominance results to the second gene
					if( s._dominant != GAGDS_RECESSIVE )
						s._dominant = dom == GAGDS_DOMINANT ? GAGDS_RECESSIVE : GAGDS_DOMINANT;

					return (int)_dominant;
				}

				return 0;
			}

			/// <summary><c>operator ==</c> comares two chromosome's genes.
			///
			/// This method is not tread-safe.</summary>
			/// <param name="rhs">gene agains comparison is being performed.</param>
			/// <returns>Method returns <c>true</c> if two genes are equal.</returns>
			inline bool GACALL operator ==(const GaLocusDominanceGene& rhs) const { return _dominant == rhs._dominant && _loctus == rhs._loctus && GaGene::operator ==( rhs ); }

			/// <summary><c>operator !=</c> comares two chromosome's genes.
			///
			/// This method is not tread-safe.</summary>
			/// <param name="rhs">gene agains comparison is being performed.</param>
			/// <returns>Method returns <c>true</c> if two genes are not equal.</returns>
			inline bool GACALL operator !=(const GaLocusDominanceGene& rhs) const { return _dominant != rhs._dominant || _loctus != rhs._loctus || GaGene::operator !=( rhs ); }

		};

		GENE_VISITOR_CREATOR(GaLocusDominanceGene, GaLocusBase)
		GENE_VISITOR_CREATOR(GaLocusDominanceGene, GaDominanceBase)

		/// <summary><c>GaDoubleDominanceWithAlleleSetGene</c> class represent chromosome's genes that stores pair of values and implements interface to determine dominance.
		/// and allele set that defines posible values it can store.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of value stored in the gene.</param>
		template<typename VALUE_TYPE>
		class GaDoubleDominanceWithAlleleSetGene : public GaAlleleSetBase,
			public GaDoubleDominanceGene<VALUE_TYPE>
		{

		protected:

			/// <summary>Allese set that defines posible values that can be stored by the gene.</summary>
			GaAlleleSet<GaValueType>* _allele;

		public:

			/// <summary>This constructor initializes gene with specified allele set and stores closest values to specified values that exists in the set.</summary>
			/// <param name="dValue1">the first value of the dominance pair.</param>
			/// <param name="dValue2">the second value of the dominance pair.</param>
			GaDoubleDominanceWithAlleleSetGene(const GaValueType& dValue1,
				const GaValueType& dValue2,
				GaAlleleSet<GaValueType>* allele) : GaDoubleDominanceGene(dValue1, dValue2),
				_allele(allele)
			{
				// find and store closest values to the set
				allele->ClosestValue( _dValue1 );
				allele->ClosestValue( _dValue2 );
			}

			/// <summary>This constructor initializes gene with specified allele set and stores random values generated by the set.</summary>
			/// <param name="allele">allele set of the gene.</param>
			GaDoubleDominanceWithAlleleSetGene(GaAlleleSet<GaValueType>* allele) : _allele(allele)
			{
				// generate random values
				allele->Generate( _dValue1 );
				allele->Generate( _dValue2 );
			}

			/// <summary>This constructor initializes gene without allele set and stores default values.</summary>
			GaDoubleDominanceWithAlleleSetGene() { }

			/// <summary>More details are given in specification of <see cref="GaAlleleSetBase::Flip" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Flip() { GaGlobalRandomBoolGenerator->Generate() ? _allele->Generate( _dValue1 ) : _allele->Generate( _dValue2 ); }

			/// <summary>More details are given in specification of <see cref="GaAlleleSetBase::Inverse" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL Inverse() { GaGlobalRandomBoolGenerator->Generate() ? _allele->Inverse( _dValue1 ) : _allele->Inverse( _dValue2 ); }

			/// <summary><c>operator ==</c> comares two chromosome's genes.
			///
			/// This method is not tread-safe.</summary>
			/// <param name="rhs">gene agains comparison is being performed.</param>
			/// <returns>Method returns <c>true</c> if two genes are equal.</returns>
			inline bool GACALL operator ==(const GaDoubleDominanceWithAlleleSetGene& rhs) const { return _allele == rhs._allele && GaDoubleDominanceGene::operator ==( rhs ); }

			/// <summary><c>operator !=</c> comares two chromosome's genes.
			///
			/// This method is not tread-safe.</summary>
			/// <param name="rhs">gene agains comparison is being performed.</param>
			/// <returns>Method returns <c>true</c> if two genes are not equal.</returns>
			inline bool GACALL operator !=(const GaDoubleDominanceWithAlleleSetGene& rhs) const { return _allele != rhs._allele || GaDoubleDominanceGene::operator !=( rhs ); }

		};

		GENE_VISITOR_CREATOR(GaDoubleDominanceWithAlleleSetGene, GaDominanceBase)
		GENE_VISITOR_CREATOR(GaDoubleDominanceWithAlleleSetGene, GaAlleleSetBase)

		/// <summary><c>GaSingleGeneChromosome</c> class represents chromosome which contains only a single gene.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="GENE_TYPE">type of chromosome's gene.</param>
		template<typename GENE_TYPE>
		class GaSingleGeneChromosome : public GaChromosome
		{

		public:

			/// <summary>Type of chromosome's gene.</summary>
			typedef GENE_TYPE GaGeneType;

		private:

			/// <summary>Chromosome's gene.</summary>
			GaGeneType _gene;

			/// <summary>Backup of chromosome's gene before mutation operation.</summary>
			GaGeneType _backup;

		public:

			/// <summary>This constructor initializes chromosome with CCB that it should use.</summary>
			/// <param name="configBlock">smart pointer to CCB.</param>
			GaSingleGeneChromosome(Common::Memory::GaSmartPtr<GaChromosomeConfigBlock> configBlock) : GaChromosome(configBlock) { }

			/// <summary>Copy constructor. The constructor copies reference to CCB from source chromosome.</summary>
			/// <param name="rhs">source chromosome that should be copied.</param>
			GaSingleGeneChromosome(const GaSingleGeneChromosome& rhs) : GaChromosome(rhs._configBlock),
				_gene(rhs._gene) { }

			/// <summary>More details are given in specification of <see cref="GaChromosome::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::Memory::GaSmartPtr<GaChromosome> GACALL Clone() const { return new GaSingleGeneChromosome( *this ); }

			/// <summary>More details are given in specification of <see cref="GaChromosome::MutationEvent" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL MutationEvent(GaChromosome::GaMuataionEvent e)
			{
				switch( e )
				{

					// save gene before mutation
				case Chromosome::GaChromosome::GAME_PREPARE:
					_backup = _gene;
					break;

				case Chromosome::GaChromosome::GAME_ACCEPT:
					break;

					// restore gene from backup afte failed mutation
				case Chromosome::GaChromosome::GAME_REJECT:
					_gene = _backup;
					break;

				}
			}

			/// <summary><c>SetGene</c> method sets chromosome's gene.
			///
			/// This method is noth thread-safe.</summary>
			/// <param name="gene">new gene.</param>
			inline void GACALL SetGene(const GaGeneType& gene) { _gene = gene; }

			/// <summary>This method is noth thread-safe.</summary>
			/// <returns>Method returns chromosome's gene.</returns>
			inline GaGeneType& GACALL GetGene() { return _gene; }

			/// <summary>This method is noth thread-safe.</summary>
			/// <returns>Method returns chromosome's gene.</returns>
			inline const GaGeneType& GACALL GetGene() const { return _gene; }

		};

		/// <summary><c>GaMultiGeneChromosome</c> class is base class for chromosomes that contains mulitple genes.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaMultiGeneChromosome : public GaChromosome
		{

		public:

			/// <summary>This constructor initializes chromosome with CCB that it should use.</summary>
			/// <param name="configBlock">smart pointer to CCB.</param>
			GaMultiGeneChromosome(Common::Memory::GaSmartPtr<GaChromosomeConfigBlock> configBlock) : GaChromosome(configBlock) { }

			/// <summary>Copy constructor. The constructor copies reference to CCB from source chromosome.</summary>
			/// <param name="rhs">source chromosome that should be copied.</param>
			GaMultiGeneChromosome(const GaMultiGeneChromosome& rhs) : GaChromosome(rhs) { }

			/// <summary><c>QueryGeneVisitorInterface</c> method creates visitor for iterationg through chromosome's genes.</summary>
			/// <param name="iid">ID of the interface that will provide access to the genes.</param>
			/// <returns>Method returns pointer to created visitor or <c>NULL</c> if the visitor cannot be created. Caller is responsible for used memory.</returns>
			virtual GaGeneVisitorInterface* GACALL QueryGeneVisitorInterface(GaGeneInterfaceID iid) = 0;

		};

		/// <summary><c>GaStructuredChromosome</c> should be used as base class for chromosomes that have multiple genes. It provides access to structure of chromosome's gene storage.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="BASE_STRUCTURE">base class of data structure that stores chromosome's genes.</param>
		template<typename BASE_STRUCTURE>
		class GaStructuredChromosome : public GaMultiGeneChromosome
		{

		public:

			/// <summary>Base class of data structure that stores chromosome's genes.</summary>
			typedef BASE_STRUCTURE GaBaseStructure;

			/// <summary>This constructor initializes chromosome with CCB that it should use.</summary>
			/// <param name="configBlock">smart pointer to CCB.</param>
			GaStructuredChromosome(Common::Memory::GaSmartPtr<GaChromosomeConfigBlock> configBlock) : GaMultiGeneChromosome(configBlock) { }

			/// <summary>Copy constructor. The constructor copies reference to CCB from source chromosome.</summary>
			/// <param name="rhs">source chromosome that should be copied.</param>
			GaStructuredChromosome(const GaStructuredChromosome& rhs) : GaMultiGeneChromosome(rhs) { }

			/// <summary><c>GetStructure</c> method provides access to structure of chromosome's genes.</summary>
			/// <returns>Method returns reference to data structure that stores chromosome's genes.</returns>
			virtual GaBaseStructure& GACALL GetStructure() = 0;

			/// <summary><c>GetStructure</c> method provides access to structure of chromosome's genes.</summary>
			/// <returns>Method returns reference to data structure that stores chromosome's genes.</returns>
			virtual const GaBaseStructure& GACALL GetStructure() const = 0;

		};

		/// <summary><c>GaTemplateChromosome</c> template class represent chromosome that has multiple genes of specified type stored in defined data structure.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="GENE_TYPE">type of chromosome's gene.</param>
		/// <param name="STRUCTURE">data structure that stores chromosome's genes.</param>
		/// <param name="BASE_STRUCTURE">base class of data structure that stores chromosome's genes.</param>
		template<typename GENE_TYPE,
			template<typename> class STRUCTURE,
			typename BASE_STRUCTURE = typename STRUCTURE<GENE_TYPE>::GaBaseStructure>
		class GaTemplateChromosome : public GaStructuredChromosome<BASE_STRUCTURE>
		{

		public:

			/// <summary>Type of chromosome's genes.</summary>
			typedef GENE_TYPE GaGeneType;

			/// <summary>Data structure that stores chromosome's genes.</summary>
			typedef STRUCTURE<GENE_TYPE> GaStructure;

		protected:

			/// <summary>Chromosome's genes.</summary>
			GaStructure _genes;

			/// <summary>Backup of chromosome's genes before mutation operation.</summary>
			GaStructure _backup;

		public:

			/// <summary>This constructor initializes chromosome with CCB that it should use.</summary>
			/// <param name="configBlock">smart pointer to CCB.</param>
			GaTemplateChromosome(Common::Memory::GaSmartPtr<GaChromosomeConfigBlock> configBlock) : GaStructuredChromosome(configBlock) { }

			/// <summary>Copy constructor. The constructor copies reference to CCB from source chromosome.</summary>
			/// <param name="rhs">source chromosome that should be copied.</param>
			GaTemplateChromosome(const GaTemplateChromosome& rhs) : GaStructuredChromosome(rhs),
				_genes(rhs._genes) { }

			/// <summary>More details are given in specification of <see cref="GaChromosome::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::Memory::GaSmartPtr<GaChromosome> GACALL Clone() const { return new GaTemplateChromosome( *this ); }

			/// <summary>More details are given in specification of <see cref="GaStructuredChromosome::GetStructure" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaBaseStructure& GACALL GetStructure() { return _genes; }

			/// <summary>More details are given in specification of <see cref="GaStructuredChromosome::GetStructure" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual const GaBaseStructure& GACALL GetStructure() const { return _genes; }

			/// <summary>More details are given in specification of <see cref="GaMultiGeneChromosome::QueryGeneVisitorInterface" /> method.
			///
			/// This method is not thread-safe.</summary>
			/// <returns>Method always returns <c>NULL</c>.</returns>
			virtual GaGeneVisitorInterface* GACALL QueryGeneVisitorInterface(GaGeneInterfaceID iid) { return NULL; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to data structure that stores chromosome's genes.</returns>
			inline GaStructure& GACALL GetGenes() { return _genes; }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns reference to data structure that stores chromosome's genes.</returns>
			inline const GaStructure& GACALL GetGenes() const { return _genes; }

			/// <summary>More details are given in specification of <see cref="GaChromosome::MutationEvent" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual void GACALL MutationEvent(GaChromosome::GaMuataionEvent e)
			{
				switch( e )
				{

					// save gene before mutation
				case Chromosome::GaChromosome::GAME_PREPARE:
					_backup.Copy( &_genes );
					break;

					// clear backup after successful mutation
				case Chromosome::GaChromosome::GAME_ACCEPT:
					_backup.Clear();
					break;

					// restore gene from backup afte failed mutation
				case Chromosome::GaChromosome::GAME_REJECT:
					_genes.Copy( &_backup );
					break;

				}
			}

		};

		/// <summary><c>GaAdvanceTemplateChromosome</c> template class represent chromosome that has multiple genes of specified type stored in defined data structure
		/// and provides advance way for handling chromosome's genes.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		/// <param name="VALUE_TYPE">type of values stored in chromosome's genes.</param>
		/// <param name="GENE_TYPE">gene class that will wrap values.</param>
		/// <param name="STRUCTURE">data structure that stores chromosome's genes.</param>
		/// <param name="BASE_STRUCTURE">base class of data structure that stores chromosome's genes.</param>
		template<typename VALUE_TYPE,
			template<typename> class GENE_TYPE,
			template<typename> class STRUCTURE,
			typename BASE_STRUCTURE = typename STRUCTURE<GENE_TYPE<VALUE_TYPE> >::GaBaseStructure>
		class GaAdvanceTemplateChromosome : public GaTemplateChromosome<GENE_TYPE<VALUE_TYPE>, STRUCTURE, BASE_STRUCTURE>
		{

		public:

			/// <summary>Type of values stored in chromosome's genes.</summary>
			typedef VALUE_TYPE GaValueType;

			/// <summary>This constructor initializes chromosome with CCB that it should use.</summary>
			/// <param name="configBlock">smart pointer to CCB.</param>
			GaAdvanceTemplateChromosome(Common::Memory::GaSmartPtr<GaChromosomeConfigBlock> configBlock) : GaTemplateChromosome(configBlock) { }

			/// <summary>Copy constructor. The constructor copies reference to CCB from source chromosome.</summary>
			/// <param name="rhs">source chromosome that should be copied.</param>
			GaAdvanceTemplateChromosome(const GaTemplateChromosome& rhs) : GaTemplateChromosome(rhs)  { }

			/// <summary>More details are given in specification of <see cref="GaChromosome::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::Memory::GaSmartPtr<GaChromosome> GACALL Clone() const { return new GaAdvanceTemplateChromosome( *this ); }

			/// <summary>More details are given in specification of <see cref="GaMultiGeneChromosome::QueryGeneVisitorInterface" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual GaGeneVisitorInterface* GACALL QueryeneVisitorInterface(GaGeneInterfaceID iid)
			{
				// create appropriate gene visitor
				switch( iid )
				{

				case GaLocusBase::IID:
					return GaGeneVisitorCreator<STRUCTURE, GaValueType, GENE_TYPE, GaLocusBase>::Create( &_genes );

				case GaAlleleSetBase::IID:
					return GaGeneVisitorCreator<STRUCTURE, GaValueType, GENE_TYPE, GaAlleleSetBase>::Create( &_genes );

				case GaDominanceBase::IID:
					return GaGeneVisitorCreator<STRUCTURE, GaValueType, GENE_TYPE, GaDominanceBase>::Create( &_genes );

				}

				return NULL;
			}

		};

		/// <summary><c>GaSDAChromosomeParams</c> class represent parameters for chromosomes that store genes in single-dimensional array.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaSDAChromosomeParams : public GaChromosomeParams
		{

		protected:

			/// <summary>Number of genes that chromosomes should have.</summary>
			int _size;

		public:

			/// <summary>This constructor initializes parameters with number of genes that chromosomes will have.</summary>
			/// <param name="size">number of genes.</param>
			GaSDAChromosomeParams(int size) { SetSize( size ); }

			/// <summary>This constructor initializes parameters where the chromosome size is 0.</summary>
			GaSDAChromosomeParams() : _size(0) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL GACALL Clone() const { return new GaSDAChromosomeParams( *this ); }

			/// <summary><c>SetSize</c> method sets the number of genes that chromosome will have.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="size">number of genes.</param>
			inline void GACALL SetSize(int size)
			{
				GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, size >= 0, "size", "Size cannot be negative.", "Representation" );
				_size = size;
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of genes that chromosomes have.</returns>
			inline int GACALL GetSize() const { return _size; }

		};

		/// <summary>Base class for chromosomes that stores genes in arrays.</summary>
		typedef GaStructuredChromosome<Common::Data::GaArrayBase> GaArrayStructureChromosome;

		/// <summary><c>GaSDAChromosome</c> struct defines type of chromosome that stores genes in single-dimensional array.</summary>
		/// <param name="VALUE_TYPE">type of values stored in chromosome's genes.</param>
		template<typename VALUE_TYPE>
		struct GaSDAChromosome
		{

			/// <summary>Chromosomes that stores genes in single-dimensional array. For more details see specifiecation of <see cref="GaTemplateChromosome" />.</summary>
			typedef GaTemplateChromosome<VALUE_TYPE, Common::Data::GaSingleDimensionArray> GaType;

		};

		/// <summary><c>GaAdvanceSDAChromosome</c> struct defines type of chromosome that stores genes in single-dimensional array with advance access to chromosome's genes.</summary>
		/// <param name="VALUE_TYPE">type of values stored in chromosome's genes.</param>
		/// <param name="GENE_TYPE">gene class that will wrap values.</param>
		template<typename VALUE_TYPE,
			template<typename> class GENE_TYPE>
		struct GaAdvanceSDAChromosome
		{

			/// <summary>Chromosomes that stores genes in single-dimensional array. For more details see specifiecation of <see cref="GaAdvanceTemplateChromosome" />.</summary>
			typedef GaAdvanceTemplateChromosome<VALUE_TYPE, GENE_TYPE, Common::Data::GaSingleDimensionArray> GaType;

		};

		/// <summary><c>GaMDAChromosomeParams</c> class represent parameters for chromosomes that store genes in multi-dimensional array.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaMDAChromosomeParams : public GaChromosomeParams
		{

		protected:

			/// <summary>Number of dimensions that arrays have.</summary>
			int _dimensionCount;

			/// <summary>Sizes of array dimensions.</summary>
			Common::Data::GaSingleDimensionArray<int> _dimensionSizes;

		public:

			/// <summary>This constructor initializes parameters with number of dimensions that arrays will have.</summary>
			/// <param name="dimensionCount">number of dimensions that arrays will have.</param>
			GaMDAChromosomeParams(int dimensionCount) : _dimensionSizes( dimensionCount ) { }

			/// <summary>This constructor initializes parameters with number of dimensions that arrays will have and their sizes.</summary>
			/// <param name="dimensionCount">number of dimensions that arrays have.</param>
			/// <param name="dimensionSizes">sizes of array dimensions.</param>
			GaMDAChromosomeParams(int dimensionCount,
				const int* dimensionSizes)  : _dimensionSizes( dimensionCount ) { SetDimensionSizes( dimensionSizes ); }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaMDAChromosomeParams( *this ); }

			/// <summary><c>SetDimensionCount</c> method sets number of dimensions that arrays will have.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="count">new number of dimensions.</param>
			inline void GACALL SetDimensionCount(int count) { _dimensionSizes.SetSize( count ); }

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns number of dimensions that arrays have.</returns>
			inline int GACALL GetDimensionCount() const { return _dimensionSizes.GetSize(); }

			/// <summary><c>SetDimensionSizes</c> sets sizes of arrays' dimensions.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="sizes">array that contains new sizes of arrays' dimensions.</param>
			void GACALL SetDimensionSizes(const int* sizes)
			{
				GA_ARG_ASSERT( Common::Exceptions::GaNullArgumentException, sizes != NULL, "sizes", "New dimension sizes must be specified.", "Representation" );

				for( int i = _dimensionSizes.GetSize() - 1; i >= 0; i-- )
					GA_ARG_ASSERT( Common::Exceptions::GaArgumentOutOfRangeException, sizes[ i ] >= 0, "sizes", "Contains negative size of a dimension.", "Representation" );

				for( int i = _dimensionSizes.GetSize() - 1; i >= 0; i-- )
					_dimensionSizes[ i ] = sizes[ i ];
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the first element that stores sizes of arrays' dimensions.</returns>
			inline int* GACALL GetDimensionSizes() { return _dimensionSizes.GetArray(); } 

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns pointer to the first element that stores sizes of arrays' dimensions.</returns>
			inline const int* GACALL GetDimensionSizes() const { return _dimensionSizes.GetArray(); } 

			/// <summary><c>SetDimensionSize</c> method sets size of specified dimension.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="dimension">dimension whose size should be set.</param>
			/// <param name="size">new size of the dimension.</param>
			inline void GACALL SetDimensionSize(int dimension,
				int size) { _dimensionSizes[ dimension ] = size; }

			/// <summary><c>GetDimensionSize</c> method queries size of a single diemsnion.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="dimension">dimension whose size is queried.</param>
			/// <returns>Method returns size of specified dimension.</returns>
			inline int GACALL GetDimensionSize(int dimension) const { return _dimensionSizes[ dimension ]; }

		};

		/// <summary><c>GaMDAChromosome</c> struct defines type of chromosome that stores genes in multi-dimensional-array.</summary>
		/// <param name="VALUE_TYPE">type of values stored in chromosome's genes.</param>
		template<typename VALUE_TYPE>
		struct GaMDAChromosome
		{

			/// <summary>Chromosomes that stores genes in multi-dimensional array. For more details see specifiecation of <see cref="GaTemplateChromosome" />.</summary>
			typedef GaTemplateChromosome<VALUE_TYPE, Common::Data::GaMultiDimensionArray> GaType;

		};

		/// <summary><c>GaAdvanceMDAChromosome</c> struct defines type of chromosome that stores genes in multi-dimensional array with advance access to chromosome's genes.</summary>
		/// <param name="VALUE_TYPE">type of values stored in chromosome's genes.</param>
		/// <param name="GENE_TYPE">gene class that will wrap values.</param>
		template<typename VALUE_TYPE,
			template<typename> class GENE_TYPE>
		struct GaAdvanceMDAChromosome
		{

			/// <summary>Chromosomes that stores genes in multi-dimensional array. For more details see specifiecation of <see cref="GaAdvanceTemplateChromosome" />.</summary>
			typedef GaAdvanceTemplateChromosome<VALUE_TYPE, GENE_TYPE, Common::Data::GaMultiDimensionArray> GaType;

		};

		/// <summary><c>GaMDAChromosomeParams</c> class represent parameters for chromosomes that store binary string.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class.
		/// No public or private methods are thread-safe.</summary>
		class GaBinaryChromosomeParams : public GaSDAChromosomeParams
		{

		protected:

			/// <summary>Probability of set state of bit in binary string. Value should be between 0..1.</summary>
			float _stateProbability;

		public:

			/// <summary>This constructor initializes parameters with user-defined probability of set state and string size.</summary>
			/// <param name="stateProbability">probability of set state. Value should be between 0..1.</param>
			/// <param name="size">size of binary string.</param>
			GaBinaryChromosomeParams(float stateProbability,
				int size) : GaSDAChromosomeParams(size) { SetStateProbability( stateProbability ); }

			/// <summary>This constructor initializes parameters with default probability of set state (0.5) and string size of 8.</summary>
			GaBinaryChromosomeParams() : GaSDAChromosomeParams(8),
				_stateProbability(0.5) { }

			/// <summary>More details are given in specification of <see cref="GaParameters::Clone" /> method.
			///
			/// This method is not thread-safe.</summary>
			virtual Common::GaParameters* GACALL Clone() const { return new GaBinaryChromosomeParams( *this ); }

			/// <summary><c>SetStateProbability</c> method sets probability of set state of bit in binary string.
			///
			/// This method is not thread-safe.</summary>
			/// <param name="probability">new probability of set state. Value should be between 0..1.</param>
			inline void GACALL SetStateProbability(float probability)
			{
				if( probability < 0 || probability > 1 )
					return;

				_stateProbability = probability;
			}

			/// <summary>This method is not thread-safe.</summary>
			/// <returns>Method returns probability of set state of bit in binary string.</returns>
			inline float GACALL GetStateProbability() const { return _stateProbability; }

		};

		/// <summary>Chromosomes that stores binary string as single-dimansional array of genes. For more details see specifiecation of <see cref="GaTemplateChromosome" />.</summary>
		typedef GaSDAChromosome<bool> GaBinaryChromosome;

		/// <summary><c>GaAdvanceBinaryChromosome</c> struct defines type of chromosome that stores binary string as single-dimansional array of genes.</summary>
		/// <param name="GENE_TYPE">gene class that will wrap string's bits.</param>
		template<template<typename> class GENE_TYPE>
		struct GaAdvanceBinaryChromosome
		{

			/// <summary>Chromosomes that stores binary string as single-dimansional array of genes.
			/// For more details see specifiecation of <see cref="GaAdvanceTemplateChromosome" />.</summary>
			typedef GaAdvanceTemplateChromosome<bool, GENE_TYPE, Common::Data::GaSingleDimensionArray> GaType;
		
		};

		/// <summary>Base class for chromosomes that stores genes in linked lists.</summary>
		typedef GaStructuredChromosome<Common::Data::GaListBase> GaListStructureChromosome;

		/// <summary><c>GaListChromosome</c> struct defines type of chromosome that stores genes in linked list.</summary>
		/// <param name="VALUE_TYPE">type of values stored in chromosome's genes.</param>
		template<typename VALUE_TYPE>
		struct GaListChromosome
		{

			/// <summary>Chromosomes that stores genes in linked list. For more details see specifiecation of <see cref="GaTemplateChromosome" />.</summary>
			typedef typename GaTemplateChromosome<VALUE_TYPE, Common::Data::GaList> GaType;

		};

		/// <summary><c>GaAdvanceListChromosome</c> struct defines type of chromosome that stores genes in linked list with advance access to chromosome's genes.</summary>
		/// <param name="VALUE_TYPE">type of values stored in chromosome's genes.</param>
		/// <param name="GENE_TYPE">gene class that will wrap values.</param>
		template<typename VALUE_TYPE,
			template<typename> class GENE_TYPE>
		struct GaAdvanceListChromosome
		{
			
			/// <summary>Chromosomes that stores genes in linked list. For more details see specifiecation of <see cref="GaAdvanceTemplateChromosome" />.</summary>
			typedef GaAdvanceTemplateChromosome<VALUE_TYPE, GENE_TYPE, Common::Data::GaList> GaType;
		
		};

		/// <summary>Base class for chromosomes that stores genes in tree data structures.</summary>
		typedef GaStructuredChromosome<Common::Data::GaTreeBase> GaTreeStructureChromosome;

		/// <summary><c>GaTreeChromosome</c> struct defines type of chromosome that stores genes in  tree data structure.</summary>
		/// <param name="VALUE_TYPE">type of values stored in chromosome's genes.</param>
		template<typename VALUE_TYPE>
		struct GaTreeChromosome
		{

			/// <summary>Chromosomes that stores genes in tree data structure. For more details see specifiecation of <see cref="GaTemplateChromosome" />.</summary>
			typedef typename GaTemplateChromosome<VALUE_TYPE, Common::Data::GaTree> GaType;

		};

		/// <summary><c>GaAdvanceTreeChromosome</c> struct defines type of chromosome that stores genes in tree data structure with advance access to chromosome's genes.</summary>
		/// <param name="VALUE_TYPE">type of values stored in chromosome's genes.</param>
		/// <param name="GENE_TYPE">gene class that will wrap values.</param>
		template<typename VALUE_TYPE,
			template<typename> class GENE_TYPE>
		struct GaAdvanceTreeChromosome
		{
			
			/// <summary>Chromosomes that stores genes in tree data structure. For more details see specifiecation of <see cref="GaAdvanceTemplateChromosome" />.</summary>
			typedef GaAdvanceTemplateChromosome<VALUE_TYPE, GENE_TYPE, Common::Data::GaTree> GaType;
		
		};

	} // Representation
} // Chromosome

#endif // __GA_REPRESENTATION_H__

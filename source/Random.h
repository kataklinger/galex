
/*! \file Random.h
    \brief This file contains declaration of interfaces, classes and datatypes of random number generators.
*/

/*
 * 
 * website: http://www.coolsoft-sd.com/
 * contact: support@coolsoft-sd.com
 *
 */

#ifndef __GA_RANDOM_H__
#define __GA_RANDOM_H__

#include <time.h>

#include "Synchronization.h"

namespace Common
{
	/// <summary>Contains classes and types used for random number generation.</summary>
	namespace Random
	{

		/// <summary>This union is used for conversation from 32-bits long integer random number
		/// to single precision floating point number in interval (0, 1).</summary>
		union GaUnsignedIntToFloat
		{

			/// <summary>This field is used to store 32-bit long integer number.</summary>
			unsigned int bits;

			/// <summary>field is used to read 32-bit long integer number as mantissa of single precision floating point number.</summary>
			float number;

		};

		/// <summary>This union is used for conversation from 64-bits long integer random number
		/// to double precision floating point number in interval (0, 1).</summary>
		union GaUnsignedIntToDouble
		{

			/// <summary>This field is used to store 64-bit long integer number.</summary>
			unsigned int bits[ 2 ];

			/// <summary>This field is used to read 64-bit long integer number as mantissa of single precision floating point number.</summary>
			double number;

		};

		/// <summary><c>GaRandomGenerator</c> class implements algorithm for generating 32-bit wide random unsigned integers and floating-point numbers.
		/// It takes care of architecture's endianness, but underlying CPU architecture must support floating-point by IEEE 754 standard.
		/// Primary purpose of this class is to provide service for generating random numbers for <see cref="GaRandom" /> template class.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public methods are thread-safe.</summary>
		class GaRandomGenerator
		{

		private:

			/// <summary>Global instance of random generator.</summary>
			static GaRandomGenerator _instance;

			/// <summary>Defines representations of random generator's state.</summary>
			struct GaState
			{

				/// <summary>The first part of random generator state.</summary>
				unsigned int _w;

				/// <summary>The second part of random generator state.</summary>
				unsigned int _z;

			};
			
			/// <summary>Current state of random generator.</summary>
			GaState _currentState;

			/// <summary>This attribute indicates endianness of architecture. If it is set to <c>true</c>, the architecture is little-endian,
			/// if the architecture is big-endian this attribute is set to <c>false</c>.</summary>
			bool _littleEndian;

		public:

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Method returns pointer to global instance of random generator.</returns>
			static inline GaRandomGenerator* GACALL GetInstance() { return &_instance; }

			/// <summary>This constructor initialize random generator with current time as seed.</summary>
			GaRandomGenerator()
			{
				unsigned long long x = (unsigned long long)time( NULL );
				Initalization( (unsigned int)( x >> 16 ), (unsigned int)x );
			}

			/// <summary>This constructor initialize random generator with user-defined seed.</summary>
			/// <param name="seed">user-defined seed.</param>
			GaRandomGenerator(unsigned int seed) { Initalization( seed, 0 ); }

			/// <summary><c>Generate</c> method generates and returns 32-bit wide unsigned integer.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns generated number.</returns>
			GAL_API
			unsigned int GACALL Generate();

			/// <summary><c>GeneratrFloat</c> method generates single precision floating point number in interval (0, 1).
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns generated number.</returns>
			GAL_API
			float GACALL GenerateFloat();

			/// <summary><c>GeneratrFloat</c> method generates double precision floating point number in interval (0, 1).
			///
			/// This method is thread-safe.</summary>
			/// <returns>Method returns generated number.</returns>
			GAL_API
			double GACALL GenerateDouble();

		private:

			/// <summary>Initializes random generator with specified seed. <c>Initialization</c> method is called by constructor.</summary>
			/// <param name="seed1">seed used to initialize the first part of generator's state.</param>
			/// <param name="seed2">seed used to initialize the second part of generator's state.</param>
			GAL_API
			void GACALL Initalization(unsigned int seed1,
				unsigned int seed2);

		};

		/// <summary>Template class for generating random values.</summary>
		/// <param name="TYPE">type of generated values.</param>
		template <typename TYPE>
		class GaRandom
		{

		public:

			/// <summary>This method generates random values of <c>TYPE</c> with no specific range.</summary>
			/// <returns>Returns generate random value.</returns>
			TYPE GACALL Generate();

			/// <summary>This method generates random value of <c>TYPE</c> with specified maximum.</summary>
			/// <param name="max">maximal value which can be generated.</param>
			/// <returns>Returns generate random value.</returns>
			TYPE GACALL Generate(const TYPE& max);

			/// <summary>This method generates random value of <c>TYPE</c> within specified range of values.</summary>
			/// <param name="min">minimal value which can be generated.</param>
			/// <param name="max">maximal value which can be generated.</param>
			/// <returns>Returns generate random value.</returns>
			TYPE GACALL Generate(const TYPE& min,
				const TYPE& max);

			/// <summary><c>GetCount</c> method returns number of different values that generator can produce.</summary>
			/// <returns>Returns number of different values that generator can produce.</returns>
			inline long long GACALL GetCount() const;

			/// <summary><c>GetCount</c> method returns number of different values that generator can produce in defined range.</summary>
			/// <param name="min">minimal value of the range.</param>
			/// <param name="max">maximal value of the range.</param>
			/// <returns>Returns number of different values that generator can produce.</returns>
			inline long long GACALL GetCount(const TYPE& min,
				const TYPE& max) const;

		};

		/// <summary>This specialization of <c>GaRandom</c> template class generates random 32-bits wide integer numbers.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public methods are thread-safe.</summary>
		template<>
		class GaRandom<int>
		{

		private:

			/// <summary>Instance of algorithm for generating random numbers.</summary>
			GaRandomGenerator* _generator;

			/// <summary>Bit mask for removing sign when converting from <c>unsigned int</c> number to <c>int</c>.</summary>
			static const unsigned int _mask = ( 1U << ( sizeof( int ) * 8 - 1 ) ) - 1;

		public:

			/// <summary>Initializes number generator with default random generator.</summary>
			GaRandom() { _generator = GaRandomGenerator::GetInstance(); }

			/// <summary>Initializes number generator to use specified random generator.</summary>
			/// <param name="generator">pointer to random generator that is used as source for generating numbers.</param>
			GaRandom(GaRandomGenerator* generator) : _generator(generator) { }

			/// <summary>This method generates random values in interval(0, 2^31).
			///
			/// This method is thread-safe.</summary>
			/// <returns>Returns generate random value.</returns>
			inline int GACALL Generate() { return (int)( _generator->Generate() & _mask ); }

			/// <summary>This method generates random values in interval(0, <c>max</c>).
			///
			/// This method is thread-safe.</summary>
			/// <param name="max">maximal value which can be generated.</param>
			/// <returns>Returns generate random value.</returns>
			inline int GACALL Generate(const int& max) { return (int)( ( max + 1 ) * _generator->GenerateDouble() ) % ( max + 1 ); }

			/// <summary>This method generates random values in interval(<c>min</c>, <c>max</c>).
			///
			/// This method is thread-safe.</summary>
			/// <param name="min">minimal value which can be generated.</param>
			/// <param name="max">maximal value which can be generated.</param>
			/// <returns>Returns generate random value.</returns>
			inline int GACALL Generate(const int& min,
				const int& max) { return min + Generate( max - min ); }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Returns number of different values that generator can produce.</returns>
			inline long long GACALL GetCount() const { return _mask; }

			/// <summary><c>GetCount</c> method returns number of different values that generator can produce in defined range.</summary>
			/// <param name="min">minimal value of the range.</param>
			/// <param name="max">maximal value of the range.</param>
			/// <returns>Returns number of different values that generator can produce.</returns>
			inline long long GACALL GetCount(const int& min,
				const int& max) const { return max - min + 1; }

		};

		/// <summary>This specialization of <c>GaRandom</c> template class generates random 64-bits wide integer numbers.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public methods are thread-safe.</summary>
		template<>
		class GaRandom<long long>
		{

		private:

			/// <summary>Instance of algorithm for generating random numbers.</summary>
			GaRandomGenerator* _generator;

			/// <summary>Bit mask for removing sign when converting from <c>unsigned long long</c> number to <c>long long</c>.</summary>
			static const unsigned long long _mask = ( 1ULL << ( sizeof( long long ) * 8 - 1 ) ) - 1;

		public:

			/// <summary>Initializes number generator with default random generator.</summary>
			GaRandom() { _generator = GaRandomGenerator::GetInstance(); }

			/// <summary>Initializes number generator to use specified random generator.</summary>
			/// <param name="generator">pointer to random generator that is used as source for generating numbers.</param>
			GaRandom(GaRandomGenerator* generator) : _generator(generator) { }

			/// <summary>This method generates random values in interval(0, 2^63).
			///
			/// This method is thread-safe.</summary>
			/// <returns>Returns generate random value.</returns>
			inline long long GACALL Generate() { return ( ( ( (long long)_generator->Generate() ) << 32 ) + _generator->Generate() ) & _mask; }

			/// <summary>This method generates random values in interval(0, <c>max</c>).
			///
			/// This method is thread-safe.</summary>
			/// <param name="max">maximal value which can be generated.</param>
			/// <returns>Returns generate random value.</returns>
			inline long long GACALL Generate(const long long& max) { return (long long)( ( max + 1 ) * _generator->GenerateDouble() ) % ( max + 1 ); }

			/// <summary>This method generates random values in interval(<c>min</c>, <c>max</c>).
			///
			/// This method is thread-safe.</summary>
			/// <param name="min">minimal value which can be generated.</param>
			/// <param name="max">maximal value which can be generated.</param>
			/// <returns>Returns generate random value.</returns>
			inline long long GACALL Generate(const long long& min,
				const long long& max) { return min + Generate( max - min ); }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Returns number of different values that generator can produce.</returns>
			inline long long GACALL GetCount() const { return _mask; }

			/// <summary><c>GetCount</c> method returns number of different values that generator can produce in defined range.</summary>
			/// <param name="min">minimal value of the range.</param>
			/// <param name="max">maximal value of the range.</param>
			/// <returns>Returns number of different values that generator can produce.</returns>
			inline long long GACALL GetCount(const long long& min,
				const long long& max) const { return max - min + 1; }

		};

		/// <summary>This specialization of <c>GaRandom</c> template class generates random single  precision floating-point numbers. 
		/// Targeted architecture must support IEEE 754 standard.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c>
		/// macros cannot be used with instances of this class, but all public methods are thread-safe.</summary>
		template<>
		class GaRandom<float>
		{

		private:

			/// <summary>Instance of algorithm for generating random numbers.</summary>
			GaRandomGenerator* _generator;

		public:

			/// <summary>Initializes number generator with default random generator.</summary>
			GaRandom() { _generator = GaRandomGenerator::GetInstance(); }

			/// <summary>Initializes number generator to use specified random generator.</summary>
			/// <param name="generator">pointer to random generator that is used as source for generating numbers.</param>
			GaRandom(GaRandomGenerator* generator) : _generator(generator) { }

			/// <summary>This method generates random values in interval(0, 1).
			///
			/// This method is thread-safe.</summary>
			/// <returns>Returns generate random value.</returns>
			inline float GACALL Generate() { return _generator->GenerateFloat(); }

			/// <summary>This method generates random values in interval(0, <c>max</c>).
			///
			/// This method is thread-safe.</summary>
			/// <param name="max">maximal value which can be generated.</param>
			/// <returns>Returns generate random value.</returns>
			inline float GACALL Generate(const float& max) { return max * _generator->GenerateFloat(); }

			/// <summary>This method generates random values in interval(<c>min</c>, <c>max</c>).
			///
			/// This method is thread-safe.</summary>
			/// <param name="min">minimal value which can be generated.</param>
			/// <param name="max">maximal value which can be generated.</param>
			/// <returns>Returns generate random value.</returns>
			inline float GACALL Generate(const float& min,
							 const float& max) { return min + Generate( max - min ); }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Returns number of different values that generator can produce.</returns>
			inline long long GACALL GetCount() const { return ( 1 << 23 ) - 1; }

			/// <summary><c>GetCount</c> method returns number of different values that generator can produce in defined range.</summary>
			/// <param name="min">minimal value of the range.</param>
			/// <param name="max">maximal value of the range.</param>
			/// <returns>Returns number of different values that generator can produce.</returns>
			inline long long GACALL GetCount(const float& min,
				const float& max) const { return GetCount(); }

		};

		/// <summary>This specialization of <c>GaRandom</c> template class generates random double precision floating-point numbers.
		/// Class takes care about endianness of the architecture. Targeted architecture must support IEEE 754 standard.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class
		/// but all public methods are thread-safe.</summary>
		template<>
		class GaRandom<double>
		{

		private:

			/// <summary>Instance of algorithm for generating random numbers.</summary>
			GaRandomGenerator* _generator;

		public:

			/// <summary>Initializes number generator with default random generator.</summary>
			GaRandom() { _generator = GaRandomGenerator::GetInstance(); }

			/// <summary>Initializes number generator to use specified random generator.</summary>
			/// <param name="generator">pointer to random generator that is used as source for generating numbers.</param>
			GaRandom(GaRandomGenerator* generator) : _generator(generator) { }

			/// <summary>This method generates random values in interval(0, 1).
			///
			/// This method is thread-safe.</summary>
			/// <returns>Returns generate random value.</returns>
			inline double GACALL Generate() { return _generator->GenerateDouble(); }

			/// <summary>This method generates random values in interval(0, <c>max</c>).
			///
			/// This method is thread-safe.</summary>
			/// <param name="max">maximal value which can be generated.</param>
			/// <returns>Returns generate random value.</returns>
			inline double GACALL Generate(const double& max) { return max * _generator->GenerateDouble(); }

			/// <summary>This method generates random values in interval(<c>min</c>, <c>max</c>).
			///
			/// This method is thread-safe.</summary>
			/// <param name="min">minimal value which can be generated.</param>
			/// <param name="max">maximal value which can be generated.</param>
			/// <returns>Returns generate random value.</returns>
			inline double GACALL Generate(const double& min,
				const double& max) { return min + Generate( max - min ); }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Returns number of different values that generator can produce.</returns>
			inline long long GACALL GetCount() const { return ( 1ULL << 52 ) - 1; }

			/// <summary><c>GetCount</c> method returns number of different values that generator can produce in defined range.</summary>
			/// <param name="min">minimal value of the range.</param>
			/// <param name="max">maximal value of the range.</param>
			/// <returns>Returns number of different values that generator can produce.</returns>
			inline long long GACALL GetCount(const double& min,
				const double& max) const { return GetCount(); }

		};

		/// <summary>This specialization of <c>GaRandom</c> template class generates random boolean values.
		/// It supports generating boolean with defined probabilities of  <c>true</c> and <c>false</c> states.
		///
		/// This class has no built-in synchronizator, so <c>GA_LOCK_OBJECT</c> and <c>GA_LOCK_THIS_OBJECT</c> macros cannot be used with instances of this class,
		/// but all public methods are thread-safe.</summary>
		template<>
		class GaRandom<bool>
		{

		private:

			/// <summary>Instance of algorithm for generating random numbers.</summary>
			GaRandomGenerator* _generator;

		public:

			/// <summary>Initializes number generator with default random generator.</summary>
			GaRandom() { _generator = GaRandomGenerator::GetInstance(); }

			/// <summary>Initializes number generator to use specified random generator.</summary>
			/// <param name="generator">pointer to random generator that is used as source for generating numbers.</param>
			GaRandom(GaRandomGenerator* generator) : _generator(generator) { }

			/// <summary>This method generates random Boolean values.
			///
			/// This method is thread-safe.</summary>
			/// <returns>Returns generate random value.</returns>
			inline bool GACALL Generate() { return ( _generator->Generate() & 1 ) == 1; }

			/// <summary>This method generates random Boolean values.
			///
			/// This method is thread-safe.</summary>
			/// <param name="max">this parameter is ignored.</param>
			/// <returns>Returns generate random value.</returns>
			inline bool GACALL Generate(const bool& max) { return Generate(); }

			/// <summary>This method generates random Boolean values.
			///
			/// This method is thread-safe.</summary>
			/// <param name="min">this parameter is ignored.</param>
			/// <param name="max">this parameter is ignored.</param>
			/// <returns>Returns generate random value.</returns>
			inline bool GACALL Generate(const bool& min,
				const bool& max) { return Generate(); }

			// Generates boolean with p probability of TRUE and 1-p probability of FALSE
			/// <summary>This method generates Boolean value with <c>p</c> probability of <c>true</c> value.
			///
			/// This method is thread safe.</summary>
			/// <param name="p">probability of <c>true</c> value (0, 1).</param>
			/// <returns>Returns generate random value.</returns>
			inline bool GACALL Generate(double p) { return _generator->GenerateFloat() < p; }

			// Generates boolean with p probability of TRUE and 100-p probability of FALSE. p is expressed in 
			/// <summary>This method generates Boolean value with <c>p</c> probability of <c>true</c> and <c>100-p</c> of <c>false</c> value.
			///
			/// This method is thread safe.</summary>
			/// <param name="p">probability in percents of <c>true</c> value (0 - 100).</param>
			/// <returns>Returns generate random value.</returns>
			inline bool GACALL Generate(int p) { return (int)( _generator->GenerateDouble() * 100 ) < p; }

			/// <summary>This method is thread-safe.</summary>
			/// <returns>Returns number of different values that generator can produce.</returns>
			inline long long GACALL GetCount() const { return 2; }

			/// <summary><c>GetCount</c> method returns number of different values that generator can produce in defined range.</summary>
			/// <param name="min">minimal value of the range.</param>
			/// <param name="max">maximal value of the range.</param>
			/// <returns>Returns number of different values that generator can produce.</returns>
			inline long long GACALL GetCount(const bool& min,
				const bool& max) const { return GetCount(); }

		};

	} // Random
} // Common

#endif // __GA_RANDOM_H__

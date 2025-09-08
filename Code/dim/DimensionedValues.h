//	trax track library
//	AD 2021 
//
//  "Jesus died for somebody's sins, but not mine"
//
//							Patti Smith
//
// Copyright (c) 2025 Trend Redaktions- und Verlagsgesellschaft mbH
// Copyright (c) 2019 Marc-Michael Horstmann
//
// Permission is hereby granted to any person obtaining a copy of this software 
// and associated source code (the "Software"), to use, view, and study the 
// Software for personal or internal business purposes, subject to the following 
// conditions:
//
// 1. Redistribution, modification, sublicensing, or commercial use of the 
// Software is NOT permitted without prior written consent from the copyright 
// holder.
//
// 2. The Software is provided "AS IS", without warranty of any kind, express 
// or implied.
//
// 3. All copies of the Software must retain this license notice.
//
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include <cmath>
#include <limits>
#include <utility>

/// \brief The namespace provides classes and methods for the dim library
namespace dim
{
	constexpr int DIM_VERSION_MAJOR = 3;
	constexpr int DIM_VERSION_MINOR = 0;
	constexpr int DIM_VERSION_PATCH = 0;

#if defined( DIM_SINGLE )
	typedef float Real; ///< Underlying floating point type to be used with the dim library.
#elif defined( DIM_DOUBLE )
	typedef double Real; ///< Underlying floating point type to be used with the dim library.
#elif defined( DIM_LONG_DOUBLE )
	typedef long double Real; ///< Underlying floating point type to be used with the dim library.
#else
	typedef float Real; ///< Underlying floating point type to be used with the dim library.
#endif


	/// \name Length and Mass and Time Units.
	///@{
	constexpr Real kilograms_per_unit	= 1; ///< How many kilograms will make up one unit.
	constexpr Real meters_per_unit		= 1; ///< How many meters will make up one unit.
	constexpr Real seconds_per_unit		= 1; ///< How many seconds will make up one unit.


	// derived units:
	constexpr Real newtons_per_unit = kilograms_per_unit * meters_per_unit / seconds_per_unit / seconds_per_unit;
	constexpr Real pascal_per_unit = newtons_per_unit / (meters_per_unit * meters_per_unit);
	constexpr Real watts_per_unit = kilograms_per_unit * meters_per_unit * meters_per_unit;

	constexpr Real units_per_kilogram = 1 / kilograms_per_unit;														///< How many units will make up one kilogram.	
	constexpr Real units_per_meter = 1 / meters_per_unit;															///< How many units will make up one meter.	
	constexpr Real units_per_second = 1 / seconds_per_unit;															///< How many units will make up one second.	
	constexpr Real units_per_newton = units_per_kilogram * units_per_meter / seconds_per_unit / seconds_per_unit;	///< How many units will make up one newton.
	constexpr Real units_per_pascal = units_per_newton / (units_per_meter * units_per_meter);						///< How many units will make up one pascal.
	constexpr Real units_per_newtonmeter = units_per_newton * units_per_meter;	///< How many units will make up one newtonmeter (unit of torque).	
	constexpr Real units_per_joule = units_per_newton * units_per_meter;		///< How many units will make up one joule.	
	constexpr Real units_per_watt = 1 / watts_per_unit;
	///@}


	/// \name Dimensionated value
	/// \brief Interface for secure handling of physical values and their units.
	/// 
	/// \todo Implement all interfaces in terms of dimensionated values.
	///@{
	///\brief Type selection for class Value
	template<int L, int M, int T>
	struct Dimension {
		enum {
			length = L, ///< spatial dimension
			mass = M,	///< inertial mass dimension
			time = T	///< time dimension
		};
	};
	/// \brief Value type, dependend from dimensions.
	template<typename Dimension>
	class Value {
		Real starkNakedNumber{ 0 };

	public:
		typedef Real real_type;

		/// \name Construction
		/// Constructors declared explicit to prevent silent conversions
		///@{
		inline constexpr Value() noexcept = default;

		inline explicit constexpr Value( const float units ) noexcept
			: starkNakedNumber{ static_cast<Real>(units) }
		{}
		inline explicit constexpr Value( const double units ) noexcept
			: starkNakedNumber( static_cast<Real>(units) )
		{}
		inline explicit constexpr Value( const int units ) noexcept
			: starkNakedNumber{ static_cast<Real>(units) }
		{}
		///@}

		/// For internal use only. Try _X() to get a Real number in
		/// units of 'X'.
		/// \returns the value as used with internal units.
		inline constexpr Real Units() const noexcept {
			return starkNakedNumber;
		}
		/// \brief Explicit conversion to Real for static_cast<Real> and operator (Real)
		///
		/// This operator is provided mainly for internal type conversion to the underlying 
		/// physics engine (which clearly uses the same units as trax) and in some template 
		/// metaprogramming situations; you should not use it elsewhere. Try _xy(Value) instead 
		/// if you know the expected unit xy; at least use Value::Units() but be aware that 
		/// the library can change the units it actually uses.
		/// Uncomment the deprecated clause to get compile time messages.
		//[[deprecated("Try to not use this. Use _xy(Value) or Value::Units() instead.")]] 
		inline explicit constexpr operator Real() const noexcept {
			return starkNakedNumber;
		}
		/// \brief Explicit conversion to bool for logical expressions like if( !value ) etc.
		inline explicit constexpr operator bool() const noexcept {
			return starkNakedNumber != 0 ? true : false;
		}
		/// \returns a const reference to the underlying value.
		inline constexpr const Real& Ref() const noexcept {
			return starkNakedNumber;
		}
	};

	/// \brief Specialization for dimension one. Implicit conversions with undimensionated values.
	template<>
	class Value<Dimension<0, 0, 0>> {
		Real starkNakedNumber{ 0 };

	public:
		typedef Real real_type;

		/// \name Construction
		/// Constructors for implicit conversion from Real and integer.
		///@{
		inline constexpr Value() noexcept = default;

		inline constexpr Value( const float value ) noexcept
			: starkNakedNumber{ static_cast<Real>(value) }
		{}
		inline constexpr Value( const double value ) noexcept
			: starkNakedNumber{ static_cast<Real>(value) }
		{}
		inline constexpr Value( const int value ) noexcept
			: starkNakedNumber{ static_cast<Real>(value) }
		{}
		///@}

		/// For internal use only. Try _X() to get a Real number in
		/// units of 'X'.
		/// \returns the value as used with internal units.
		inline constexpr Real Units() const noexcept {
			return starkNakedNumber;
		}
		/// \brief Implicit conversion to Real.
		inline constexpr operator Real() const noexcept {
			return starkNakedNumber;
		}
		/// \returns a const reference to the underlying value.
		inline constexpr const Real& Ref() const noexcept {
			return starkNakedNumber;
		}
	};
	///@}


	/// \name Dimensionated Values Known Qualities
	///@{
	using One = Value<Dimension<0, 0, 0>>;					///< Dimensionless value.
	using Angle = Value<Dimension<0, 0, 0>>;				///< Angle in radians.
	using AnglePerLength = Value<Dimension<-1, 0, 0>>;		///< Angle per length.
	using PixelPerLength = Value<Dimension<-1, 0, 0>>;		///< Pixel per length.
	using AngularVelocity = Value<Dimension<0, 0, -1>>;		///< Angular velocity.
	using Length = Value<Dimension<1, 0, 0>>;				///< Length.
	using Area = Value<Dimension<2, 0, 0>>;					///< Area.
	using Volume = Value<Dimension<3, 0, 0>>;				///< Volume.
	using Mass = Value<Dimension<0, 1, 0>>;					///< Mass.
	using Density = Value<Dimension<-3, 1, 0>>;				///< Density.
	using Time = Value<Dimension<0, 0, 1>>;					///< Time.
	using Frequency = Value<Dimension<0, 0, -1>>;			///< Frequency.
	using Velocity = Value<Dimension<1, 0, -1>>;			///< Velocity.
	using Acceleration = Value<Dimension<1, 0, -2>>;		///< Acceleration.
	using Force = Value<Dimension<1, 1, -2>>;				///< Force.
	using Pressure = Value<Dimension<-1, 1, -2>>;			///< Pressure.
	using Momentum = Value<Dimension<1, 1, -1>>;			///< Momentum.
	using AngularMomentum = Value<Dimension<2, 1, -1>>;		///< Angular momentum.
	using Torque = Value<Dimension<2, 1, -2>>;				///< Torque.
	using Energy = Value<Dimension<2, 1, -2>>;				///< Energy.
	using Power = Value<Dimension<2, 1, -3>>;				///< Power.
	///@}


	/// \brief Marginal difference in calculations.
	constexpr Real epsilon = std::numeric_limits<Real>::epsilon();
	static_assert(epsilon > 0, "Too small an epsilon!");


	/// \brief Positive infinity value
	constexpr Real infinite = std::numeric_limits<Real>::infinity();

	/// \name Positive infinity values
	///@{
	constexpr One				infinite__one{ infinite };
	constexpr Angle				infinite__angle{ infinite };
	constexpr AnglePerLength	infinite__anglePerLength{ infinite };
	constexpr AngularVelocity	infinite__angularVelocity{ infinite };
	constexpr Length			infinite__length{ infinite };
	constexpr Area				infinite__area{ infinite };
	constexpr Volume			infinite__volume{ infinite };
	constexpr Mass				infinite__mass{ infinite };
	constexpr Density			infinite__density{ infinite };
	constexpr Time				infinite__time{ infinite };
	constexpr Frequency			infinite__frequency{ infinite };
	constexpr Velocity			infinite__velocity{ infinite };
	constexpr Acceleration		infinite__accelearation{ infinite };
	constexpr Force				infinite__force{ infinite };
	constexpr Pressure			infinite__pressure{ infinite };
	constexpr Momentum			infinite__momentum{ infinite };
	constexpr AngularMomentum	infinite__angularMomentum{ infinite };
	constexpr Torque			infinite__torque{ infinite };
	constexpr Energy			infinite__energy{ infinite };
	constexpr Power				infinite__power{ infinite };
	///@}
} // namespace dim

namespace std {

	/// \brief Dimensionated Values Numeric Limits
	template<int L, int M, int T>
	class numeric_limits<dim::Value<dim::Dimension<L, M, T>>>
	{	// limits for type Value<Dimension<L,M,T>>
	public:
		/// \name std::numeric_limits implementation
		///@{
		static constexpr dim::Value<dim::Dimension<L, M, T>> (min)() noexcept
		{	// return minimum value
			return dim::Value<dim::Dimension<L, M, T>>{(std::numeric_limits<dim::Real>::min)()};
		}

		static constexpr dim::Value<dim::Dimension<L, M, T>> (max)() noexcept
		{	// return maximum value
			return dim::Value<dim::Dimension<L, M, T>>{(std::numeric_limits<dim::Real>::max)()};
		}

		static constexpr dim::Value<dim::Dimension<L, M, T>> lowest() noexcept
		{	// return most negative value
			return dim::Value<dim::Dimension<L, M, T>>{std::numeric_limits<dim::Real>::lowest()};
		}

		static constexpr dim::Value<dim::Dimension<L, M, T>> epsilon() noexcept
		{	// return smallest effective increment from 1.0
			return dim::Value<dim::Dimension<L, M, T>>{std::numeric_limits<dim::Real>::epsilon()};
		}

		static constexpr dim::Value<dim::Dimension<L, M, T>> round_error() noexcept
		{	// return largest rounding error
			return dim::Value<dim::Dimension<L, M, T>>{std::numeric_limits<dim::Real>::round_error()};
		}

		static constexpr dim::Value<dim::Dimension<L, M, T>> denorm_min() noexcept
		{	// return minimum denormalized value
			return dim::Value<dim::Dimension<L, M, T>>{std::numeric_limits<dim::Real>::denorm_min()};
		}

		static constexpr dim::Value<dim::Dimension<L, M, T>> infinity() noexcept
		{	// return positive infinity
			return dim::Value<dim::Dimension<L, M, T>>{std::numeric_limits<dim::Real>::infinity()};
		}

		static constexpr dim::Value<dim::Dimension<L, M, T>> quiet_NaN() noexcept
		{	// return non-signaling NaN
			return dim::Value<dim::Dimension<L, M, T>>{std::numeric_limits<dim::Real>::quiet_NaN()};
		}

		static constexpr dim::Value<dim::Dimension<L, M, T>> signaling_NaN() noexcept
		{	// return signaling NaN
			return dim::Value<dim::Dimension<L, M, T>>{std::numeric_limits<dim::Real>::signaling_NaN()};
		}

		static constexpr int digits = std::numeric_limits<dim::Real>::digits;
		static constexpr int digits10 = std::numeric_limits<dim::Real>::digits10;
		static constexpr int max_digits10 = std::numeric_limits<dim::Real>::max_digits10;
		static constexpr int max_exponent = std::numeric_limits<dim::Real>::max_exponent;
		static constexpr int max_exponent10 = std::numeric_limits<dim::Real>::max_exponent10;
		static constexpr int min_exponent = std::numeric_limits<dim::Real>::min_exponent;
		static constexpr int min_exponent10 = std::numeric_limits<dim::Real>::min_exponent10;

#if __cplusplus <= 202002L
		static constexpr float_denorm_style has_denorm = std::numeric_limits<dim::Real>::has_denorm;
#endif
		static constexpr bool has_infinity = std::numeric_limits<dim::Real>::has_infinity;
		static constexpr bool has_quiet_NaN = std::numeric_limits<dim::Real>::has_quiet_NaN;
		static constexpr bool has_signaling_NaN = std::numeric_limits<dim::Real>::has_signaling_NaN;
		static constexpr bool is_bounded = std::numeric_limits<dim::Real>::is_bounded;
		static constexpr bool is_iec559 = std::numeric_limits<dim::Real>::is_iec559;
		static constexpr bool is_signed = std::numeric_limits<dim::Real>::is_signed;
		static constexpr bool is_specialized = std::numeric_limits<dim::Real>::is_specialized;
		static constexpr float_round_style round_style = std::numeric_limits<dim::Real>::round_style;
		static constexpr int radix = std::numeric_limits<dim::Real>::radix;

#if __cplusplus <= 202002L
		static constexpr bool has_denorm_loss = std::numeric_limits<dim::Real>::has_denorm_loss;
#endif
		static constexpr bool is_exact = std::numeric_limits<dim::Real>::is_exact;
		static constexpr bool is_integer = std::numeric_limits<dim::Real>::is_integer;
		static constexpr bool is_modulo = std::numeric_limits<dim::Real>::is_modulo;
		static constexpr bool tinyness_before = std::numeric_limits<dim::Real>::tinyness_before;
		static constexpr bool traps = std::numeric_limits<dim::Real>::traps;
		///@}
	};
}

namespace dim{

	/// \name Dimensionated Values Operators
	///@{
	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>> operator+( const Value<Dimension<L,M,T>>& a, const Value<Dimension<L,M,T>>& b ) noexcept{
		return Value<Dimension<L,M,T>>{ a.Units() + b.Units() };
	}
	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>>& operator+=( Value<Dimension<L,M,T>>& a, const Value<Dimension<L,M,T>>& b ) noexcept{
		a = Value<Dimension<L,M,T>>{ a.Units() + b.Units() };
		return a;
	}
	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>> operator+( const Value<Dimension<L,M,T>>& a ) noexcept{
		return Value<Dimension<L,M,T>>{ +a.Units() };
	}

	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>> operator-( const Value<Dimension<L,M,T>>& a, const Value<Dimension<L,M,T>>& b ) noexcept{
		return Value<Dimension<L,M,T>>{ a.Units() - b.Units() };
	}
	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>>& operator-=( Value<Dimension<L,M,T>>& a, const Value<Dimension<L,M,T>>& b ) noexcept{
		a = Value<Dimension<L,M,T>>{ a.Units() - b.Units() };
		return a;
	}
	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>> operator-( const Value<Dimension<L,M,T>>& a ) noexcept{
		return Value<Dimension<L,M,T>>{ -a.Units() };
	}

	template<int La,int Ma,int Ta,int Lb,int Mb,int Tb>
	constexpr Value<Dimension<La+Lb,Ma+Mb,Ta+Tb>> operator*( const Value<Dimension<La,Ma,Ta>>& a, const Value<Dimension<Lb,Mb,Tb>>& b ) noexcept{
		return Value<Dimension<La+Lb,Ma+Mb,Ta+Tb>>{ a.Units() * b.Units() };
	}

	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>> operator*( const Value<Dimension<L,M,T>>& a, float b ) noexcept{
		return Value<Dimension<L,M,T>>{ a.Units() * b };
	}

	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>> operator*( const Value<Dimension<L,M,T>>& a, double b ) noexcept{
		return Value<Dimension<L,M,T>>{ a.Units() * b };
	}

	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>> operator*( const Value<Dimension<L,M,T>>& a, int b ) noexcept{
		return Value<Dimension<L,M,T>>{ a.Units() * static_cast<Real>(b) };
	}

	template<int L, int M, int T> inline
	constexpr Value<Dimension<L, M, T>> operator*( const Value<Dimension<L, M, T>>& a, size_t b ) noexcept{
		return Value<Dimension<L, M, T>>{ a.Units() * static_cast<Real>(b) };
	}

	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>> operator*( float a, const Value<Dimension<L,M,T>>& b ) noexcept{
		return b * a;
	}

	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>> operator*( double a, const Value<Dimension<L,M,T>>& b ) noexcept{
		return b * a;
	}

	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>> operator*( int a, const Value<Dimension<L,M,T>>& b ) noexcept{
		return b * a;
	}

	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>>& operator*=( Value<Dimension<L,M,T>>& a, float b ) noexcept{
		a = Value<Dimension<L,M,T>>{ a.Units() * b };
		return a;
	}

	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>>& operator*=( Value<Dimension<L,M,T>>& a, double b ) noexcept{
		a = Value<Dimension<L,M,T>>{ a.Units() * b };
		return a;
	}

	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>>& operator*=( Value<Dimension<L,M,T>>& a, int b ) noexcept{
		a = Value<Dimension<L,M,T>>{ a.Units() * static_cast<Real>(b) };
		return a;
	}

	template<int L, int M, int T> inline
	constexpr Value<Dimension<L, M, T>>& operator*=( Value<Dimension<L, M, T>>& a, size_t b ) noexcept{
		a = Value<Dimension<L, M, T>>{ a.Units() * static_cast<Real>(b) };
		return a;
	}

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable: 4723) //disable: 4723, potential divide by 0
#endif
	template<int La,int Ma,int Ta,int Lb,int Mb,int Tb>
	constexpr Value<Dimension<La-Lb,Ma-Mb,Ta-Tb>> operator/( const Value<Dimension<La,Ma,Ta>>& a, const Value<Dimension<Lb,Mb,Tb>>& b ) noexcept{
		return Value<Dimension<La-Lb,Ma-Mb,Ta-Tb>>{ a.Units() / b.Units() };
	}
#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>> operator/( const Value<Dimension<L,M,T>>& a, float b ) noexcept{
		return Value<Dimension<L,M,T>>{ a.Units() / b };
	}

	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>> operator/( const Value<Dimension<L,M,T>>& a, double b ) noexcept{
		return Value<Dimension<L,M,T>>{ a.Units() / b };
	}

	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>> operator/( const Value<Dimension<L,M,T>>& a, int b ) noexcept{
		return Value<Dimension<L,M,T>>{ a.Units() / static_cast<Real>(b) };
	}

	template<int L, int M, int T> inline
	constexpr Value<Dimension<L, M, T>> operator/( const Value<Dimension<L, M, T>>& a, size_t b ) noexcept{
		return Value<Dimension<L, M, T>>{ a.Units() / static_cast<Real>(b) };
	}

	template<int L,int M,int T> inline
	constexpr Value<Dimension<-L,-M,-T>> operator/( float a, const Value<Dimension<L,M,T>>& b ) noexcept{
		return Value<Dimension<-L,-M,-T>>{ a / b.Units() };
	}

	template<int L,int M,int T> inline
	constexpr Value<Dimension<-L,-M,-T>> operator/( double a, const Value<Dimension<L,M,T>>& b ) noexcept{
		return Value<Dimension<-L,-M,-T>>{ a / b.Units() };
	}

	template<int L,int M,int T> inline
	constexpr Value<Dimension<-L,-M,-T>> operator/( int a, const Value<Dimension<L,M,T>>& b ) noexcept{
		return Value<Dimension<-L,-M,-T>>{ static_cast<Real>(a) / b.Units() };
	}

	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>>& operator/=( Value<Dimension<L,M,T>>& a, float b ) noexcept{
		a = Value<Dimension<L,M,T>>{ a.Units() / b };
		return a;
	}

	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>>& operator/=( Value<Dimension<L,M,T>>& a, double b ) noexcept{
		a = Value<Dimension<L,M,T>>{ a.Units() / b };
		return a;
	}

	template<int L,int M,int T> inline
	constexpr Value<Dimension<L,M,T>>& operator/=( Value<Dimension<L,M,T>>& a, int b ) noexcept{
		a = Value<Dimension<L,M,T>>{ a.Units() / static_cast<Real>(b) };
		return a;
	}

	template<int L, int M, int T> inline
	constexpr Value<Dimension<L, M, T>>& operator/=( Value<Dimension<L, M, T>>& a, size_t b ) noexcept{
		a = Value<Dimension<L, M, T>>{ a.Units() / static_cast<Real>(b) };
		return a;
	}

	template<int L,int M,int T> inline
	constexpr bool operator==( const Value<Dimension<L,M,T>>& a, const Value<Dimension<L,M,T>>& b ) noexcept{
		return a.Units() == b.Units();
	}

	template<int L,int M,int T> inline
	constexpr bool operator!=( const Value<Dimension<L,M,T>>& a, const Value<Dimension<L,M,T>>& b ) noexcept{
		return a.Units() != b.Units();
	}

	template<int L,int M,int T> inline
	constexpr bool operator<( const Value<Dimension<L,M,T>>& a, const Value<Dimension<L,M,T>>& b ) noexcept{
		return a.Units() < b.Units();
	}

	template<int L,int M,int T> inline
	constexpr bool operator>( const Value<Dimension<L,M,T>>& a, const Value<Dimension<L,M,T>>& b ) noexcept{
		return a.Units() > b.Units();
	}

	template<int L,int M,int T> inline
	constexpr bool operator<=( const Value<Dimension<L,M,T>>& a, const Value<Dimension<L,M,T>>& b ) noexcept{
		return a.Units() <= b.Units();
	}
	template<int L,int M,int T> inline
	constexpr bool operator>=( const Value<Dimension<L,M,T>>& a, const Value<Dimension<L,M,T>>& b ) noexcept{
		return a.Units() >= b.Units();
	}
	///@}


	/// \name Dimensionated Values Math Functions
	///@{
	template<int L,int M,int T>
	constexpr Value<Dimension<L/2,M/2,T/2>> sqrt( Value<Dimension<L,M,T>> a ) noexcept{
		return Value<Dimension<L/2,M/2,T/2>>{ std::sqrt(a.Units()) };
	}

	template<int Y,int L,int M,int T>
	constexpr Value<Dimension<Y*L,Y*M,Y*T>> pow( Value<Dimension<L,M,T>> x ) noexcept{
		return Value<Dimension<Y*L,Y*M,Y*T>>{ std::pow(x.Units(),static_cast<Real>(Y)) };
	}

	template<int L,int M,int T>
	constexpr Value<Dimension<L,M,T>> abs( Value<Dimension<L,M,T>> x ) noexcept{
		return Value<Dimension<L,M,T>>{ std::abs(x.Units()) };
	}

	template<int L,int M,int T>
	constexpr Value<Dimension<L,M,T>> fabs( Value<Dimension<L,M,T>> x ) noexcept{
		return Value<Dimension<L,M,T>>{ std::fabs(x.Units()) };
	}

	template<int L,int M,int T>
	constexpr Value<Dimension<L,M,T>> round( Value<Dimension<L,M,T>> x ) noexcept{
		return Value<Dimension<L,M,T>>{ std::round(x.Units()) };
	}

	constexpr inline Value<Dimension<0,0,0>> sin( Value<Dimension<0,0,0>> a ) noexcept{
		return Value<Dimension<0,0,0>>{ std::sin(a.Units()) };
	}

	constexpr inline Value<Dimension<0,0,0>> cos( Value<Dimension<0,0,0>> a ) noexcept{
		return Value<Dimension<0,0,0>>{ std::cos(a.Units()) };
	}

	constexpr inline Value<Dimension<0,0,0>> tan( Value<Dimension<0,0,0>> a ) noexcept{
		return Value<Dimension<0,0,0>>{ std::tan(a.Units()) };
	}

	constexpr inline Value<Dimension<0,0,0>> asin( Value<Dimension<0,0,0>> a ) noexcept{
		return Value<Dimension<0,0,0>>{ std::asin(a.Units()) };
	}

	constexpr inline Value<Dimension<0,0,0>> acos( Value<Dimension<0,0,0>> a ) noexcept{
		return Value<Dimension<0,0,0>>{ std::acos(a.Units()) };
	}

	constexpr inline Value<Dimension<0,0,0>> atan( Value<Dimension<0,0,0>> a ) noexcept{
		return Value<Dimension<0,0,0>>{ std::atan(a.Units()) };
	}

	template<int L,int M,int T>
	constexpr Value<Dimension<0,0,0>> atan2( Value<Dimension<L,M,T>> y, Value<Dimension<L,M,T>> x ) noexcept{
		return Value<Dimension<0,0,0>>{ std::atan2(y.Units(),x.Units()) };
	}

	template<int L,int M,int T>
	constexpr bool isnan( Value<Dimension<L,M,T>> x ){
		return std::isnan(x.Units());
	}

	template<int L,int M,int T>
	constexpr Value<Dimension<L,M,T>> nextafter( Value<Dimension<L,M,T>> x, float y ) noexcept{
		return Value<Dimension<L,M,T>>{ std::nextafter(x.Units(),y) };
	}

	template<int L,int M,int T>
	constexpr Value<Dimension<L,M,T>> nexttoward( Value<Dimension<L,M,T>> x, long double y ) noexcept{
		return Value<Dimension<L,M,T>>{ std::nexttoward(x.Units(),y) };
	}

	template<int L,int M,int T>
	constexpr Value<Dimension<L,M,T>> ldexp( Value<Dimension<L,M,T>> x, int exp ){
		return Value<Dimension<L,M,T>>{ std::ldexp(x.Units(),exp) };
	}
	///@}

	inline namespace literals{

		/// \name Dimensioned Values Literals
		///@{
		constexpr Value<Dimension<-1,0,0>> operator"" _1Ikm( const unsigned long long int value ){
			return Value<Dimension<-1,0,0>>{ static_cast<Real>(value) * meters_per_unit / 1000 };
		}

		constexpr Value<Dimension<-1,0,0>> operator"" _1Ikm( const long double value ){
			return Value<Dimension<-1,0,0>>{ static_cast<Real>(value) * meters_per_unit / 1000 };
		}

		constexpr Value<Dimension<-1,0,0>> operator"" _1Im( const unsigned long long int value ){
			return Value<Dimension<-1,0,0>>{ static_cast<Real>(value) * meters_per_unit };
		}

		constexpr Value<Dimension<-1,0,0>> operator"" _1Im( const long double value ){
			return Value<Dimension<-1,0,0>>{ static_cast<Real>(value) * meters_per_unit };
		}

		constexpr Value<Dimension<-1,0,0>> operator"" _1Icm( const unsigned long long int value ){
			return Value<Dimension<-1,0,0>>{ static_cast<Real>(value) * meters_per_unit * 100 };
		}

		constexpr Value<Dimension<-1,0,0>> operator"" _1Icm( const long double value ){
			return Value<Dimension<-1,0,0>>{ static_cast<Real>(value) * meters_per_unit * 100 };
		}

		constexpr Value<Dimension<-2,0,0>> operator"" _1Im2( const unsigned long long int value ){
			return Value<Dimension<-2,0,0>>{ static_cast<Real>(value) * meters_per_unit * meters_per_unit };
		}

		constexpr Value<Dimension<-2,0,0>> operator"" _1Im2( const long double value ){
			return Value<Dimension<-2,0,0>>{ static_cast<Real>(value) * meters_per_unit * meters_per_unit };
		}

		constexpr Value<Dimension<0,0,-1>> operator"" _1Is( const unsigned long long int value ){
			return Value<Dimension<0,0,-1>>{ static_cast<Real>(value) / units_per_second };
		}

		constexpr Value<Dimension<0,0,-1>> operator"" _1Is( const long double value ){
			return Value<Dimension<0,0,-1>>{ static_cast<Real>(value) / units_per_second };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _mm( const unsigned long long int value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * units_per_meter / 1000 };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _mm( const long double value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * units_per_meter / 1000 };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _cm( const unsigned long long int value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * units_per_meter / 100 };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _cm( const long double value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * units_per_meter / 100 };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _dm( const unsigned long long int value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * units_per_meter / 10 };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _dm( const long double value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * units_per_meter / 10 };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _m( const unsigned long long int value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * units_per_meter };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _m( const long double value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * units_per_meter };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _km( const unsigned long long int value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * 1000 * units_per_meter };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _km( const long double value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * 1000 * units_per_meter };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _mi( const unsigned long long int value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * 1000 * units_per_meter * 25146/15625 };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _mi( const long double value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * 1000 * units_per_meter * 25146/15625 };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _nmi( const unsigned long long int value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * units_per_meter * 1852 };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _nmi( const long double value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * units_per_meter * 1852 };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _yd( const unsigned long long int value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * units_per_meter * 0.3048f * 3 };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _yd( const long double value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * units_per_meter * 0.3048f * 3 };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _ft( const unsigned long long int value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * units_per_meter * 0.3048f };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _ft( const long double value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * units_per_meter * 0.3048f  };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _in( const unsigned long long int value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * units_per_meter * 0.3048f/12 };
		}

		constexpr Value<Dimension<1,0,0>> operator"" _in( const long double value ){
			return Value<Dimension<1,0,0>>{ static_cast<Real>(value) * units_per_meter * 0.3048f/12  };
		}

		constexpr Value<Dimension<2,0,0>> operator"" _cm2( const unsigned long long int value ){
			return Value<Dimension<2,0,0>>{ static_cast<Real>(value) * units_per_meter / 100 * units_per_meter / 100 };
		}

		constexpr Value<Dimension<2,0,0>> operator"" _cm2( const long double value ){
			return Value<Dimension<2,0,0>>{ static_cast<Real>(value) * units_per_meter / 100 * units_per_meter / 100 };
		}

		constexpr Value<Dimension<2,0,0>> operator"" _m2( const unsigned long long int value ){
			return Value<Dimension<2,0,0>>{ static_cast<Real>(value) * units_per_meter * units_per_meter };
		}

		constexpr Value<Dimension<2,0,0>> operator"" _m2( const long double value ){
			return Value<Dimension<2,0,0>>{ static_cast<Real>(value) * units_per_meter * units_per_meter };
		}

		constexpr Value<Dimension<3,0,0>> operator"" _m3( const unsigned long long int value ){
			return Value<Dimension<3,0,0>>{ static_cast<Real>(value) * units_per_meter * units_per_meter * units_per_meter };
		}

		constexpr Value<Dimension<3,0,0>> operator"" _m3( const long double value ){
			return Value<Dimension<3,0,0>>{ static_cast<Real>(value) * units_per_meter * units_per_meter * units_per_meter };
		}

		constexpr Value<Dimension<0,1,0>> operator"" _g( const unsigned long long int value ){
			return Value<Dimension<0,1,0>>{ static_cast<Real>(value) * units_per_kilogram / 1000 };
		}

		constexpr Value<Dimension<0,1,0>> operator"" _g( const long double value ){
			return Value<Dimension<0,1,0>>{ static_cast<Real>(value) * units_per_kilogram / 1000 };
		}

		constexpr Value<Dimension<0,1,0>> operator"" _kg( const unsigned long long int value ){
			return Value<Dimension<0,1,0>>{ static_cast<Real>(value) * units_per_kilogram };
		}

		constexpr Value<Dimension<0,1,0>> operator"" _kg( const long double value ){
			return Value<Dimension<0,1,0>>{ static_cast<Real>(value) * units_per_kilogram };
		}

		constexpr Value<Dimension<0,1,0>> operator"" _t( const unsigned long long int value ){
			return Value<Dimension<0,1,0>>{ static_cast<Real>(value) * 1000 * units_per_kilogram };
		}

		constexpr Value<Dimension<0,1,0>> operator"" _t( const long double value ){
			return Value<Dimension<0,1,0>>{ static_cast<Real>(value) * 1000 * units_per_kilogram };
		}

		constexpr Value<Dimension<-3,1,0>> operator"" _gIcm3( const unsigned long long int value ){
			return Value<Dimension<-3,1,0>>{ static_cast<Real>(value) * 1000 * units_per_kilogram / units_per_meter / units_per_meter / units_per_meter };
		}

		constexpr Value<Dimension<-3,1,0>> operator"" _gIcm3( const long double value ){
			return Value<Dimension<-3,1,0>>{ static_cast<Real>(value) * 1000 * units_per_kilogram / units_per_meter / units_per_meter / units_per_meter };
		}

		constexpr Value<Dimension<-3,1,0>> operator"" _kgIm3( const unsigned long long int value ){
			return Value<Dimension<-3,1,0>>{ static_cast<Real>(value) * units_per_kilogram / units_per_meter / units_per_meter / units_per_meter };
		}

		constexpr Value<Dimension<-3,1,0>> operator"" _kgIm3( const long double value ){
			return Value<Dimension<-3,1,0>>{ static_cast<Real>(value) * units_per_kilogram / units_per_meter / units_per_meter / units_per_meter };
		}

		constexpr Value<Dimension<-3,1,0>> operator"" _tIm3( const unsigned long long int value ){
			return Value<Dimension<-3,1,0>>{ static_cast<Real>(value) * 1000 * units_per_kilogram / units_per_meter / units_per_meter / units_per_meter };
		}

		constexpr Value<Dimension<-3,1,0>> operator"" _tIm3( const long double value ){
			return Value<Dimension<-3,1,0>>{ static_cast<Real>(value) * 1000 * units_per_kilogram / units_per_meter / units_per_meter / units_per_meter };
		}

		constexpr Value<Dimension<0,0,1>> operator"" _h( const unsigned long long int value ){
			return Value<Dimension<0,0,1>>{ static_cast<Real>(value) * 60 * 60 / seconds_per_unit };
		}

		constexpr Value<Dimension<0,0,1>> operator"" _h( const long double value ){
			return Value<Dimension<0,0,1>>{ static_cast<Real>(value) * 60 * 60 / seconds_per_unit };
		}

		constexpr Value<Dimension<0,0,1>> operator"" _s( const unsigned long long int value ){
			return Value<Dimension<0,0,1>>{ static_cast<Real>(value) / seconds_per_unit };
		}

		constexpr Value<Dimension<0,0,1>> operator"" _s( const long double value ){
			return Value<Dimension<0,0,1>>{ static_cast<Real>(value) / seconds_per_unit };
		}

		constexpr Value<Dimension<0,0,1>> operator"" _ms( const unsigned long long int value ){
			return Value<Dimension<0,0,1>>{ static_cast<Real>(value) / seconds_per_unit / 1000 };
		}

		constexpr Value<Dimension<0,0,1>> operator"" _ms( const long double value ){
			return Value<Dimension<0,0,1>>{ static_cast<Real>(value) / seconds_per_unit / 1000 };
		}

		constexpr Value<Dimension<1,0,-1>> operator"" _cmIs( const unsigned long long int value ){
			return Value<Dimension<1,0,-1>>{ static_cast<Real>(value) / 100 * units_per_meter / units_per_second };
		}

		constexpr Value<Dimension<1,0,-1>> operator"" _cmIs( const long double value ){
			return Value<Dimension<1,0,-1>>{ static_cast<Real>(value) / 100 * units_per_meter / units_per_second };
		}

		constexpr Value<Dimension<1,0,-1>> operator"" _mIs( const unsigned long long int value ){
			return Value<Dimension<1,0,-1>>{ static_cast<Real>(value) * units_per_meter / units_per_second };
		}

		constexpr Value<Dimension<1,0,-1>> operator"" _mIs( const long double value ){
			return Value<Dimension<1,0,-1>>{ static_cast<Real>(value) * units_per_meter / units_per_second };
		}

		constexpr Value<Dimension<1,0,-1>> operator"" _kmIh( const unsigned long long int value ){
			return Value<Dimension<1,0,-1>>{ static_cast<Real>(value) / 3.6f * units_per_meter };
		}

		constexpr Value<Dimension<1,0,-1>> operator"" _kmIh( const long double value ){
			return Value<Dimension<1,0,-1>>{ static_cast<Real>(value) / 3.6f * units_per_meter };
		}

		constexpr Value<Dimension<1,0,-1>> operator"" _miIh( const unsigned long long int value ){
			return Value<Dimension<1,0,-1>>{ static_cast<Real>(value) * 0.44704 * units_per_meter / units_per_second };
		}

		constexpr Value<Dimension<1,0,-1>> operator"" _miIh( const long double value ){
			return Value<Dimension<1,0,-1>>{ static_cast<Real>(value) * 0.44704 * units_per_meter / units_per_second };
		}

		constexpr Value<Dimension<1,0,-1>> operator"" _mph( const unsigned long long int value ){
			return Value<Dimension<1,0,-1>>{ static_cast<Real>(value) * 0.44704 * units_per_meter / units_per_second };
		}

		constexpr Value<Dimension<1,0,-1>> operator"" _mph( const long double value ){
			return Value<Dimension<1,0,-1>>{ static_cast<Real>(value) * 0.44704 * units_per_meter / units_per_second };
		}

		constexpr Value<Dimension<1,0,-2>> operator"" _mIs2( const unsigned long long int value ){
			return Value<Dimension<1,0,-2>>{ static_cast<Real>(value) * units_per_meter / units_per_second / units_per_second};
		}

		constexpr Value<Dimension<1,0,-2>> operator"" _mIs2( const long double value ){
			return Value<Dimension<1,0,-2>>{ static_cast<Real>(value) * units_per_meter / units_per_second / units_per_second };
		}

		constexpr Value<Dimension<1,1,-2>> operator"" _N( const unsigned long long int value ){
			return Value<Dimension<1,1,-2>>{ static_cast<Real>(value) * units_per_newton };
		}

		constexpr Value<Dimension<1,1,-2>> operator"" _N( const long double value ){
			return Value<Dimension<1,1,-2>>{ static_cast<Real>(value) * units_per_newton };
		}

		constexpr Value<Dimension<1,1,-2>> operator"" _kN( const unsigned long long int value ){
			return Value<Dimension<1,1,-2>>{ static_cast<Real>(value) * 1000 * units_per_newton };
		}

		constexpr Value<Dimension<1,1,-2>> operator"" _kN( const long double value ){
			return Value<Dimension<1,1,-2>>{ static_cast<Real>(value) * 1000 * units_per_newton };
		}

		constexpr Value<Dimension<-1,1,-2>> operator"" _Pa( const unsigned long long int value ){
			return Value<Dimension<-1,1,-2>>{ static_cast<Real>(value) * units_per_pascal };
		}

		constexpr Value<Dimension<-1,1,-2>> operator"" _Pa( const long double value ){
			return Value<Dimension<-1,1,-2>>{ static_cast<Real>(value) * units_per_pascal };
		}

		constexpr Value<Dimension<-1, 1, -2>> operator"" _kPa( const unsigned long long int value ){
			return Value<Dimension<-1, 1, -2>>{ static_cast<Real>(value) * 1000 * units_per_pascal };
		}

		constexpr Value<Dimension<-1, 1, -2>> operator"" _kPa( const long double value ){
			return Value<Dimension<-1, 1, -2>>{ static_cast<Real>(value) * 1000 * units_per_pascal };
		}

		constexpr Value<Dimension<-1, 1, -2>> operator"" _bar( const unsigned long long int value ){
			return Value<Dimension<-1, 1, -2>>{ static_cast<Real>(value) * 100 * 1000 * units_per_pascal };
		}

		constexpr Value<Dimension<-1, 1, -2>> operator"" _bar( const long double value ){
			return Value<Dimension<-1, 1, -2>>{ static_cast<Real>(value) * 100 * 1000 * units_per_pascal };
		}

		constexpr Value<Dimension<2,1,-2>> operator"" _Nm( const unsigned long long int value ){
			return Value<Dimension<2,1,-2>>{ static_cast<Real>(value) * units_per_newton * units_per_meter };
		}

		constexpr Value<Dimension<2,1,-2>> operator"" _Nm( const long double value ){
			return Value<Dimension<2,1,-2>>{ static_cast<Real>(value) * units_per_newton * units_per_meter };
		}

		constexpr Value<Dimension<2,1,-2>> operator"" _kNm( const unsigned long long int value ){
			return Value<Dimension<2,1,-2>>{ static_cast<Real>(value) * 1000 * units_per_newton * units_per_meter };
		}

		constexpr Value<Dimension<2,1,-2>> operator"" _kNm( const long double value ){
			return Value<Dimension<2,1,-2>>{ static_cast<Real>(value) * 1000 * units_per_newton * units_per_meter };
		}

		constexpr Value<Dimension<1,1,-1>> operator"" _Ns( const unsigned long long int value ){
			return Value<Dimension<1,1,-1>>{ static_cast<Real>(value) * units_per_newton * units_per_second };
		}

		constexpr Value<Dimension<1,1,-1>> operator"" _Ns( const long double value ){
			return Value<Dimension<1,1,-1>>{ static_cast<Real>(value) * units_per_newton * units_per_second };
		}

		constexpr Value<Dimension<2,1,-1>> operator"" _Nms( const unsigned long long int value ){
			return Value<Dimension<2,1,-1>>{ static_cast<Real>(value) * units_per_newton * units_per_meter * units_per_second };
		}

		constexpr Value<Dimension<2,1,-1>> operator"" _Nms( const long double value ){
			return Value<Dimension<2,1,-1>>{ static_cast<Real>(value) * units_per_newton * units_per_meter * units_per_second };
		}

		constexpr Value<Dimension<2,1,-2>> operator"" _J( const unsigned long long int value ){
			return Value<Dimension<2,1,-2>>{ static_cast<Real>(value) * units_per_kilogram * units_per_meter * units_per_meter / units_per_second / units_per_second };
		}

		constexpr Value<Dimension<2,1,-2>> operator"" _J( const long double value ){
			return Value<Dimension<2,1,-2>>{ static_cast<Real>(value) * units_per_kilogram * units_per_meter * units_per_meter / units_per_second / units_per_second };
		}

		constexpr Value<Dimension<2,1,-3>> operator"" _W( const unsigned long long int value ){
			return Value<Dimension<2,1,-3>>{ static_cast<Real>(value) * units_per_kilogram * units_per_meter * units_per_meter };
		}

		constexpr Value<Dimension<2,1,-3>> operator"" _W( const long double value ){
			return Value<Dimension<2,1,-3>>{ static_cast<Real>(value) * units_per_kilogram * units_per_meter * units_per_meter };
		}

		constexpr Value<Dimension<2,1,-3>> operator"" _kW( const unsigned long long int value ){
			return Value<Dimension<2,1,-3>>{ static_cast<Real>(value) * 1000 * units_per_kilogram * units_per_meter * units_per_meter };
		}

		constexpr Value<Dimension<2,1,-3>> operator"" _kW( const long double value ){
			return Value<Dimension<2,1,-3>>{ static_cast<Real>(value) * 1000 * units_per_kilogram * units_per_meter * units_per_meter };
		}

		constexpr Value<Dimension<0,0,0>> operator"" _1( const unsigned long long int value ){
			return Value<Dimension<0,0,0>>{ static_cast<Real>(value) };
		}

		constexpr Value<Dimension<0,0,0>> operator"" _1( const long double value ){
			return Value<Dimension<0,0,0>>{ static_cast<Real>(value) };
		}

		constexpr Angle operator"" _rad( const unsigned long long int radiant ){
			return Angle{ static_cast<Real>(radiant) };
		}

		constexpr Angle operator"" _rad( const long double radiant ){
			return Angle{ static_cast<Real>(radiant) };
		}

		constexpr AngularVelocity operator"" _radIs( const unsigned long long int value ){
			return AngularVelocity{ static_cast<Real>(value) * seconds_per_unit };
		}

		constexpr AngularVelocity operator"" _radIs( const long double value ){
			return AngularVelocity{ static_cast<Real>(value) * seconds_per_unit };
		}
		///@}
	} // inline namespace literals

	/// \brief Circle number pi.
	constexpr Angle pi = 3.14159265358979323846264338327950288_rad; ///< Ludolph van Ceulen

	/// \brief 2pi, sometimes called tau.
	constexpr Angle tau = 2*pi;

	inline namespace literals{
		/// \name Dimensionated Values Literals
		///@{
		constexpr Angle operator"" _deg( const unsigned long long int degree ){
			return Angle{ static_cast<Real>(degree) * pi/180 };
		}

		constexpr Angle operator"" _deg( const long double degree ){
			return Angle{ static_cast<Real>(degree) * pi/180 };
		}

		constexpr AngularVelocity operator"" _degIs( const unsigned long long int value ){
			return AngularVelocity{ static_cast<Real>(value) * pi/180  * seconds_per_unit };
		}

		constexpr AngularVelocity operator"" _degIs( const long double value ){
			return AngularVelocity{ static_cast<Real>(value) * pi/180  * seconds_per_unit };
		}
		///@}
	} // inline namespace literals


	/// \name Dimensionated Values Units Conversion Functions:
	///@{
	constexpr inline Real _1( One one ) noexcept{
		return one.Units();
	}

	constexpr inline One _1( Real r ) noexcept{
		return One{ r };
	}

	constexpr inline Real _deg( Angle a ) noexcept{
		return a.Units() * 180 / pi;
	}

	constexpr inline Angle _deg( Real l ) noexcept{
		return Angle{ l * pi / 180 };
	}

	constexpr inline Real _rad( Angle a ) noexcept{
		return a.Units();
	}

	constexpr inline Angle _rad( Real l ) noexcept{
		return Angle{ l };
	}

	constexpr inline Real _m( Length l ) noexcept{
		return l.Units() * meters_per_unit;
	}

	constexpr inline Length _m( Real l ) noexcept{
		return Length{ l * units_per_meter };
	}

	constexpr inline Real _mm( Length l ) noexcept{
		return _m( l ) * 1000;
	}

	constexpr inline Length _mm( Real l ) noexcept{
		return _m( l / 1000 );
	}

	constexpr inline Real _cm( Length l ) noexcept{
		return _m( l ) * 100;
	}

	constexpr inline Length _cm( Real l ) noexcept{
		//return _m( l / 100 );
		return _m( l * Real{0.01f} ); // I don't know why it is so, but sometimes the multiplication is faster than dividing  R.Bacza
	}

	constexpr inline Real _dm( Length l ) noexcept{
		return _m( l ) * 10;
	}

	constexpr inline Length _dm( Real l ) noexcept{
		return _m( l / 10 );
	}

	constexpr inline Real _km( Length l ) noexcept{
		return _m( l ) / 1000;
	}

	constexpr inline Length _km( Real l ) noexcept{
		return _m( l * 1000 );
	}

	constexpr inline Real _mi( Length l ) noexcept{
		return _m( l ) / 1000 * 15625/25146;
	}

	constexpr inline Length _mi( Real l ) noexcept{
		return _m( l * 1000 * 25146/15625 );
	}

	constexpr inline Real _nmi( Length l ) noexcept{
		return _m( l ) / 1852;
	}

	constexpr inline Length _nmi( Real l ) noexcept{
		return _m( l * 1852 );
	}

	constexpr inline Real _yd( Length l ) noexcept{
		return _m( l ) / (Real{0.3048f} * 3);
	}

	constexpr inline Length _yd( Real l ) noexcept{
		return _m( l * 0.3048f * 3 );
	}

	constexpr inline Real _ft( Length l ) noexcept{
		return _m( l ) / 0.3048f;
	}

	constexpr inline Length _ft( Real l ) noexcept{
		return _m( l * 0.3048f );
	}

	constexpr inline Real _in( Length l ) noexcept{
		return _m( l ) * 12 / 0.3048f;
	}

	constexpr inline Length _in( Real l ) noexcept{
		return _m( l * 0.3048f / 12 );
	}

	constexpr inline Real _cm2( Area a ) noexcept{
		return a.Units() * 100 * meters_per_unit * 100 * meters_per_unit;
	}

	constexpr inline Area _cm2( Real a ) noexcept{
		return Area{ a * units_per_meter / 100 * units_per_meter / 100 };
	}

	constexpr inline Real _m2( Area a ) noexcept{
		return a.Units() * meters_per_unit * meters_per_unit;
	}

	constexpr inline Area _m2( Real a ) noexcept{
		return Area{ a * units_per_meter * units_per_meter };
	}

	constexpr inline Real _m3( Volume v ) noexcept{
		return v.Units() * meters_per_unit * meters_per_unit * meters_per_unit;
	}

	constexpr inline Volume _m3( Real v ) noexcept{
		return Volume{ v * units_per_meter * units_per_meter * units_per_meter };
	}

	constexpr inline Real _1Is( AngularVelocity v ) noexcept{
		return v.Units() / seconds_per_unit;
	}

	constexpr inline AngularVelocity _1Is( Real v ) noexcept{
		return AngularVelocity{ v / units_per_second };
	}

	constexpr inline Real _degIs( AngularVelocity dads ) noexcept{
		return dads.Units() * 180 / pi / seconds_per_unit;
	}

	constexpr inline AngularVelocity _degIs( Real l ) noexcept{
		return AngularVelocity{ l * pi / 180 / units_per_second };
	}

	constexpr inline Real _radIs( AngularVelocity dads ) noexcept{
		return dads.Units() / seconds_per_unit;
	}

	constexpr inline AngularVelocity _radIs( Real l ) noexcept{
		return AngularVelocity{ l / units_per_second };
	}

	constexpr inline Real _1Im( AnglePerLength a ) noexcept{
		return a.Units() / meters_per_unit;
	}

	constexpr inline AnglePerLength _1Im( Real a ) noexcept{
		return AnglePerLength{ a / units_per_meter };
	}

	constexpr inline Real _1Icm( AnglePerLength a ) noexcept{
		return a.Units() / meters_per_unit / 100;
	}

	constexpr inline AnglePerLength _1Icm( Real a ) noexcept{
		return AnglePerLength{ a / units_per_meter * 100 };
	}

	constexpr inline Real _degIm( AnglePerLength a ) noexcept{
		return a.Units() * 180 / pi / meters_per_unit;
	}

	constexpr inline AnglePerLength _degIm( Real l ) noexcept{
		return AnglePerLength{ l * pi / 180 * meters_per_unit };
	}

	constexpr inline Real _degIcm( AnglePerLength a ) noexcept{
		return a.Units() * 180 / pi / meters_per_unit / 100;
	}

	constexpr inline AnglePerLength _degIcm( Real l ) noexcept{
		return AnglePerLength{ l * pi / 180 * meters_per_unit * 100 };
	}

	constexpr inline Real _1Im2( Value<Dimension<-2,0,0>> a ) noexcept{
		return a.Units() / meters_per_unit / meters_per_unit;
	}

	constexpr inline Value<Dimension<-2,0,0>> _1Im2( Real a ) noexcept{
		return Value<Dimension<-2,0,0>>{ a / units_per_meter / meters_per_unit };
	}

	constexpr inline Real _cmIs( Velocity v ) noexcept{
		return v.Units() * meters_per_unit / seconds_per_unit * 100;
	}

	constexpr inline Velocity _cmIs( Real v ) noexcept{
		return Velocity{ v / 100 * units_per_meter / units_per_second };
	}

	constexpr inline Real _mIs( Velocity v ) noexcept{
		return v.Units() * meters_per_unit / seconds_per_unit;
	}

	constexpr inline Velocity _mIs( Real v ) noexcept{
		return Velocity{ v * units_per_meter / units_per_second };
	}

	constexpr inline Real _kmIh( Velocity v ) noexcept{
		return _mIs( v ) * 3.6f;
	}

	constexpr inline Velocity _kmIh( Real v ) noexcept{
		return _mIs( v / 3.6f );
	}

	constexpr inline Real _mph( Velocity v ) noexcept{
		return _mIs( v ) * 2.236f;
	}

	constexpr inline Velocity _mph( Real v ) noexcept{
		return _mIs( v * 0.44704f );
	}

	constexpr inline Real _kn( Velocity v ) noexcept{
		return _mIs( v ) / 0.514f;
	}

	constexpr inline Velocity _kn( Real v ) noexcept{
		return _mIs( v * 0.514f );
	}

	constexpr inline Real _mIs2( Acceleration a ) noexcept{
		return a.Units() * meters_per_unit / seconds_per_unit / seconds_per_unit;
	}

	constexpr inline Acceleration _mIs2( Real a ) noexcept{
		return Acceleration{ a * units_per_meter / units_per_second / units_per_second };
	}

	constexpr inline Real _m2Is2( decltype(Velocity{}*Velocity{}) v2 ) noexcept{
		return v2.Units() * meters_per_unit * meters_per_unit / seconds_per_unit / seconds_per_unit;
	}

	constexpr inline decltype(Velocity{}*Velocity{}) _m2Is2( Real v2 ) noexcept{
		return decltype(Velocity{}*Velocity{}){ v2 * units_per_meter * units_per_meter / units_per_second / units_per_second };
	}

	constexpr inline Real _kg( Mass m ) noexcept{
		return m.Units() * kilograms_per_unit;
	}

	constexpr inline Mass _kg( Real m ) noexcept{
		return Mass{ m * units_per_kilogram };
	}

	constexpr inline Real _g( Mass m ) noexcept{
		return _kg( m ) * 1000;
	}

	constexpr inline Mass _g( Real m ) noexcept{
		return _kg( m / 1000 );
	}

	constexpr inline Real _t( Mass m ) noexcept{
		return _kg( m ) / 1000;
	}

	constexpr inline Mass _t( Real m ) noexcept{
		return _kg( m * 1000 );
	}

	constexpr inline Real _lb( Mass m ) noexcept{
		return _kg( m ) / 0.45359237f;
	}

	constexpr inline Mass _lb( Real m ) noexcept{
		return _kg( m * 0.45359237f );
	}

	constexpr inline Real _oz( Mass m ) noexcept{
		return _lb( m ) * 16;
	}

	constexpr inline Mass _oz( Real m ) noexcept{
		return _lb( m / 16 );
	}

	constexpr inline Real _long_tons( Mass m ) noexcept{
		return _lb( m ) / 2240;
	}

	constexpr inline Mass _long_tons( Real m ) noexcept{
		return _lb( m * 2240 );
	}

	constexpr inline Real _short_tons( Mass m ) noexcept{
		return _lb( m ) / 2000;
	}

	constexpr inline Mass _short_tons( Real m ) noexcept{
		return _lb( m * 2000 );
	}

	constexpr inline Real _gIcm3( Density d ) noexcept{
		return d.Units() * kilograms_per_unit / meters_per_unit / meters_per_unit / meters_per_unit / 100;
	}

	constexpr inline Density _gIcm3( Real d ) noexcept{
		return Density{ d * units_per_kilogram / units_per_meter / units_per_meter / units_per_meter * 100 };
	}

	constexpr inline Real _kgIm3( Density d ) noexcept{
		return d.Units() * kilograms_per_unit / meters_per_unit / meters_per_unit / meters_per_unit;
	}

	constexpr inline Density _kgIm3( Real d ) noexcept{
		return Density{ d * units_per_kilogram / units_per_meter / units_per_meter / units_per_meter };
	}

	constexpr inline Real _tIm3( Density d ) noexcept{
		return d.Units() * kilograms_per_unit / meters_per_unit / meters_per_unit / meters_per_unit / 1000;
	}

	constexpr inline Density _tIm3( Real d ) noexcept{
		return Density{ d * units_per_kilogram / units_per_meter / units_per_meter / units_per_meter * 1000 };
	}

	constexpr inline Real _ms( Time t ) noexcept{
		return t.Units() * seconds_per_unit * 1000;
	}

	constexpr inline Time _ms( Real t ) noexcept{
		return Time{ t / 1000 * units_per_second };
	}

	constexpr inline Time _ms( const long long int t ) noexcept{
		return Time{ static_cast<Real>(t) / 1000 * units_per_second };
	}

	constexpr inline Real _s( Time t ) noexcept{
		return t.Units() * seconds_per_unit;
	}

	constexpr inline Time _h( Real t ) noexcept{
		return Time{ t * 60 * 60 * units_per_second };
	}

	constexpr inline Real _h( Time t ) noexcept{
		return t.Units() * seconds_per_unit / 60 / 60;
	}

	constexpr inline Time _s( Real t ) noexcept{
		return Time{ t * units_per_second };
	}

	constexpr inline Real _N( Force f ) noexcept{
		return f.Units() * newtons_per_unit;
	}

	constexpr inline Force _N( Real f ) noexcept{
		return Force{ f * units_per_newton };
	}

	constexpr inline Real _kN( Force f ) noexcept{
		return _N( f ) / 1000;
	}

	constexpr inline Force _kN( Real f ) noexcept{
		return _N( f * 1000  );
	}

	constexpr inline Real _kp( Force f ) noexcept{
		return _N( f ) / 9.80665f;
	}

	constexpr inline Force _kp( Real f ) noexcept{
		return _N( f * 9.80665f );
	}

	constexpr inline Real _lbf( Force f ) noexcept{
		return _N( f ) / 4.448222f;
	}

	constexpr inline Force _lbf( Real f ) noexcept{
		return _N( f * 4.448222f );
	}

	constexpr inline Real _long_tons_force( Force f ) noexcept{
		return _lbf( f ) / 2240;
	}

	constexpr inline Force _long_tons_force( Real f ) noexcept{
		return _lbf( f * 2240 );
	}

	constexpr inline Real _short_tons_force( Force f ) noexcept{
		return _lbf( f ) / 2000;
	}

	constexpr inline Force _short_tons_force( Real f ) noexcept{
		return _lbf( f * 2000 );
	}

	constexpr inline Real _Pa( Pressure p ) noexcept{
		return p.Units() * pascal_per_unit;
	}

	constexpr inline Pressure _Pa( Real p ) noexcept{
		return Pressure{ p * units_per_pascal };
	}

	constexpr inline Real _kPa( Pressure p ) noexcept{
		return _Pa( p ) / 1000;
	}

	constexpr inline Pressure _kPa( Real p ) noexcept{
		return _Pa( p * 1000 );
	}

	constexpr inline Real _bar( Pressure p ) noexcept{
		return _kPa( p ) / 100;
	}

	constexpr inline Pressure _bar( Real p ) noexcept{
		return _kPa( p * 100 );
	}

	constexpr inline Real _kgmIs( Momentum m ) noexcept{
		return m.Units() * kilograms_per_unit * meters_per_unit / seconds_per_unit;
	}

	constexpr inline Momentum _kgmIs( Real m ) noexcept{
		return Momentum{ m * units_per_kilogram * units_per_meter / units_per_second };
	}

	constexpr inline Real _tmIs( Momentum m ) noexcept{
		return m.Units() * kilograms_per_unit * meters_per_unit / seconds_per_unit / 1000;
	}

	constexpr inline Momentum _tmIs( Real m ) noexcept{
		return Momentum{ m * units_per_kilogram * units_per_meter / units_per_second *	1000 };
	}

	constexpr inline Real _kgm2Is( AngularMomentum am ) noexcept{
		return am.Units() * kilograms_per_unit * meters_per_unit * meters_per_unit / seconds_per_unit;
	}

	constexpr inline AngularMomentum _kgm2Is( Real am ) noexcept{
		return AngularMomentum{ am * units_per_kilogram * units_per_meter * units_per_meter / units_per_second };
	}

	constexpr inline Real _tm2Is( AngularMomentum am ) noexcept{
		return am.Units() * kilograms_per_unit * meters_per_unit * meters_per_unit / seconds_per_unit / 1000;
	}

	constexpr inline AngularMomentum _tm2Is( Real am ) noexcept{
		return AngularMomentum{ am * units_per_kilogram * units_per_meter * units_per_meter / units_per_second * 1000 };
	}

	constexpr inline Real _Nm( Torque t ) noexcept{
		return t.Units() * newtons_per_unit * meters_per_unit;
	}

	constexpr inline Torque _Nm( Real t ) noexcept{
		return Torque{ t * units_per_newton * units_per_meter };
	}

	constexpr inline Real _kNm( Torque t ) noexcept{
		return _Nm( t ) / 1000;
	}

	constexpr inline Torque _kNm( Real t ) noexcept{
		return _Nm( t * 1000 );
	}

	constexpr inline Real _W( Power p ) noexcept{
		return p.Units() * watts_per_unit;
	}

	constexpr inline Power _W( Real p ) noexcept{
		return Power{ p * units_per_watt };
	}

	constexpr inline Real _kW( Power p ) noexcept{
		return _W( p ) / 1000;
	}

	constexpr inline Power _kW( Real p ) noexcept{
		return _W( p * 1000 );
	}

	constexpr inline Real _PS( Power p ) noexcept{
		return _W( p ) / 735.49875f;
	}

	constexpr inline Power _PS( Real p ) noexcept{
		return _W( p * 735.49875f );
	}

	constexpr inline Real _hp( Power p ) noexcept{
		return _kW( p ) * 1.34102f;
	}

	constexpr inline Power _hp( Real p ) noexcept{
		return _kW( p * 0.74570f );
	}
	///@}

} // namespace dim

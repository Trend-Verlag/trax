//	trax track library
//	AD 2014 
//
//  "hard-boiled eggs and nuts"
//
//					Oliver Norvell Hardy
//
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

#include "Helpers.h"

#include <algorithm>

namespace common{


	/// \brief An interval describes the area between two
	/// numbers. It is understood to contain the near one
	/// and exlude the far.
	template<typename Valtype>
	struct Interval
	{
		//TODO: rename m_Near m_Min and m_Far m_Max
		Valtype m_Near;	///< The 'left' side of the interval; usually the smaller value.
		Valtype m_Far;	///< The 'right' side of the interval; usually the bigger value.

		/// \name Construction
		/// \param val Set the interval at val with no extention
		/// \param nearval lower value of interval.
		/// \param farval lower value of interval
		///@{		
				 constexpr Interval() noexcept = default;			///< Does not initialize the members.
		explicit constexpr Interval( Valtype val ) noexcept;
				 constexpr Interval( Valtype nearval, Valtype farval ) noexcept;
		explicit constexpr Interval( const std::pair<Valtype,Valtype>& pair ) noexcept;
		template<typename Valtype2>
		explicit constexpr Interval( const Interval<Valtype2>& interval ) noexcept;
		///@}


		/// \name Initialisation
		/// \brief Initializes the interval to origin.
		///@{
		Interval& Init() noexcept;

		Interval& Init( Valtype nearval, Valtype length ) noexcept;
		///@}


		/// \returns 1 if the interval is normal, -1 elsewise.
		constexpr auto Sign() const noexcept ->decltype(Valtype{}/Valtype{});


		/// \brief	Is m_Near <= m_Far
		constexpr bool Normal() const noexcept;


		/// \brief	Make m_Near <= m_Far by flipping.
		Interval& Normalize() noexcept;


		/// \brief swap near and far.
		Interval& Flip() noexcept;


		/// \returns The near value of this interval.
		constexpr Valtype Near() const noexcept;


		/// \returns The far value of this interval.
		constexpr Valtype Far() const noexcept;


		/// \brief Sets the near value of this interval.
		Interval& Near( Valtype val ) noexcept;


		/// \brief Sets the far value of this interval.
		Interval& Far( Valtype val ) noexcept;


		/// \returns The maximum of the two limiting values.
		constexpr Valtype Max() const noexcept;


		/// \returns The minimum of the two limiting values.
		constexpr Valtype Min() const noexcept;


		/// \brief Distance from m_Near to m_Far
		constexpr Valtype Length() const noexcept;

		
		/// \brief Sets the length of the interval by centering
		/// it around 0.
		Interval& Length( Valtype length ) noexcept;


		/// \brief gets the value centered in the interval.
		constexpr Valtype Center() const noexcept;


		/// \brief Calculates the union of the two intervals.
		Interval& Union( const Interval& i1, const Interval& i2 ) noexcept;


		/// \brief Calculates the union of this and interval.
		Interval& Union( const Interval& interval ) noexcept;


		/// \brief Calculates the intersection interval of i1 and i2. 
		///
		/// The normal state will be that of intersected; if these rects 
		/// are not in touch, the resulting interval will be {0,0}.
		Interval& Intersection( const Interval& intersected, const Interval& with ) noexcept;


		/// \brief Calculates the intersection interval of this and interval. 
		///
		/// The normal state will be that of this; if these rects are not 
		/// in touch, the resulting interval will be {0,0}.
		Interval& Intersection( const Interval& interval ) noexcept;


		/// \returns True if val is inbetween m_Near and m_Far. 
		/// val == m_Near will return true, val == m_Far false.
		constexpr bool Includes( Valtype val ) const noexcept;


		/// \returns True if interval is completely inbetween m_Near and m_Far. 
		constexpr bool Includes( const Interval& interval ) const noexcept;


		/// \returns True if val is inbetween m_Near and m_Far. 
		/// val == m_Near-epsilon and val == m_Far+epsilon will return true.
		constexpr bool Touches( Valtype val, Valtype epsilon = Valtype{0} ) const noexcept;


		/// \brief Clips the value to the interval.
		/// \returns The clipped value.
		Valtype Clip( Valtype& val ) const noexcept;

		constexpr const Valtype& Clip( const Valtype& val ) const noexcept;


		/// \brief Wraps the value to the interval.
		/// \returns The wrapped value.
		Valtype Wrap( Valtype& val ) const noexcept;


		/// \brief Expands the interval so that it touches (being included or laying on border) val.
		///
		/// If the value is allready included in the interval, no changes are made.
		/// If val becomes m_Far Includes() will not return the value as beeing included.
		Interval& Expand( Valtype val ) noexcept;


		/// \brief Extends the inteval by dx at each end.
		Interval& Inflate( Valtype dx ) noexcept;


		/// \brief Shrinks the interval by dx at each end.
		Interval& Deflate( Valtype dx ) noexcept;


		/// \brief Moves the interval by dx.
		Interval& Move( Valtype dx ) noexcept;


		/// \brief Understands interval as relative to this and transforms it to parent.
		/// \returns transformed interval.
		Interval& ToParent( Interval& interval ) const noexcept;


		/// \brief Understands interval as relative to parent and transforms it to this.
		/// \returns transformed interval.
		Interval& FromParent( Interval& interval ) const noexcept;
		

		/// \brief Comparison
		/// \param interval Interval to compare with.
		/// \param epsilon deviation margin.
		/// \returns true if the two Intervals are equal inside the given margin.
		constexpr bool Equals( const Interval& interval, Valtype epsilon = std::numeric_limits<Valtype>::epsilon() ) const noexcept;


		/// \brief Rounds the inteval's values to a certain digit after the point.
		Interval& Round( int toDigit ) noexcept;
	};


	/// \name Mathematical Operators for Interval
	///@{
	
	/// \brief Interval operator.
	template<typename Valtype> 
	void operator+=( Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept;
	template<typename Valtype> constexpr 
	Interval<Valtype> operator+( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept;
	template<typename Valtype> constexpr 
	Interval<Valtype> operator+( const Interval<Valtype>& i1, Valtype l ) noexcept;
	template<typename Valtype> constexpr 
	Interval<Valtype> operator+( Valtype l, const Interval<Valtype>& i1 ) noexcept;
	template<typename Valtype> constexpr 
	Interval<Valtype> operator+( const Interval<Valtype>& i1 ) noexcept;
	template<typename Valtype> constexpr 
	Interval<Valtype> operator-( const Interval<Valtype>& i1, Valtype l ) noexcept;
	template<typename Valtype> constexpr 
	Interval<Valtype> operator-( Valtype l, const Interval<Valtype>& i1 ) noexcept;
	template<typename Valtype> constexpr 
	Interval<Valtype> operator-( const Interval<Valtype>& i1 ) noexcept;

	template<typename Valtype, typename Valtype2> constexpr
	auto operator*( const Interval<Valtype>& i1, Valtype2 scalar ) noexcept -> Interval<decltype(Valtype{}*Valtype2{})>;

	template<typename Valtype, typename Valtype2> constexpr
	auto operator*( Valtype scalar, const Interval<Valtype2>& i1 ) noexcept -> Interval<decltype(Valtype{}*Valtype2{})>;

	template<typename Valtype, typename Valtype2> constexpr
	Interval<Valtype>& operator*=( Interval<Valtype>& i1, Valtype2 scalar ) noexcept;

	template<typename Valtype, typename Valtype2> constexpr
	auto operator/( const Interval<Valtype>& i1, Valtype2 scalar ) noexcept -> Interval<decltype(Valtype{}/Valtype2{})>;

	template<typename Valtype, typename Valtype2> constexpr
	Interval<Valtype>& operator/=( Interval<Valtype>& i1, Valtype2 scalar ) noexcept;


	/// \returns true if every point in i2 is greater than any point in i1.
	template<typename Valtype> constexpr 
	bool operator<( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept;
	/// \returns true if every point in i2 is lesser than any point in i1.
	template<typename Valtype> constexpr 
	bool operator>( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept;


	/// \returns true if every point in i2 is greater than any point in i1, including borders.
	template<typename Valtype> constexpr 
	bool SmallerClosed( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept;
	/// \returns true if every point in i2 is lesser than any point in i1, excluding borders.
	template<typename Valtype> constexpr 
	bool SmallerOpen( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept;
	/// \returns true if every point in i2 is lesser than any point in i1, including borders.
	template<typename Valtype> constexpr 
	bool GreaterClosed( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept;
	/// \returns true if every point in i2 is lesser than any point in i1, excluding borders.
	template<typename Valtype> constexpr 
	bool GreaterOpen( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept;


	template<typename Valtype> constexpr 
	bool operator== ( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept;
	template<typename Valtype> constexpr 
	bool operator!= ( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept;
	///@}


	/// \returns true if the two intervals have a section in common, including Near() and excluding Far().
	template<typename Valtype> constexpr 
	bool Intersecting( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept;


	/// \returns true if the two intervals have a section in common, including borders.
	template<typename Valtype> constexpr 
	bool IntersectingClosed( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept;


	/// \returns true if the two intervals have a section in common, excluding borders.
	template<typename Valtype> constexpr 
	bool IntersectingOpen( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept;


	/// Two intervals touch, if the distance between them is insignificant (with respect to 
	/// std::numeric_limits<Valtype>::epsilon()). Intersecting intervals always touch.
	/// \returns true if the two intervals have a section in common or are at least adjacent.
	template<typename Valtype> constexpr 
	bool Touching( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept;


	/// \returns The common section as an Interval with the same normal state as i1. 
	/// If the two intervals do not intersect, the result will be {0,0}.
	/// \param intersected Interval that is interseced; governs the normal state of the result.
	/// \param with Interval to intersect with.
	template<typename Valtype> constexpr 
	Interval<Valtype> Intersection( const Interval<Valtype>& intersected, const Interval<Valtype>& with ) noexcept;


	/// \brief Samples an evaluating function inside an interval.
	/// \tparam EvalFuncType can be function pointer, functor or lambda: Valtype(*)( Valtype ). It returns the 
	/// distance for the next sample.
	/// \param range interval to sample
	/// \param Evaluate evaluation function
	template<class Valtype, class EvalFuncType>
	void Sample( const common::Interval<Valtype> range, const EvalFuncType Evaluate );

///////////////////////////////////////
//inlines:
template<typename Valtype> inline constexpr
Interval<Valtype>::Interval( Valtype val ) noexcept
	:	m_Near{val},
		m_Far{val}
{}

template<typename Valtype> inline constexpr
Interval<Valtype>::Interval( Valtype nearval, Valtype farval ) noexcept
	:	m_Near{nearval},
		m_Far{farval}
{}

template<typename Valtype> inline constexpr 
Interval<Valtype>::Interval( const std::pair<Valtype,Valtype>& pair ) noexcept
	:	m_Near{pair.first},
		m_Far{pair.second}
{}

template<typename Valtype>
template<typename Valtype2> inline constexpr
Interval<Valtype>::Interval( const Interval<Valtype2>& interval ) noexcept
	:	m_Near{interval.m_Near},
		m_Far{interval.m_Far}
{}

template<typename Valtype> inline
Interval<Valtype>& Interval<Valtype>::Init() noexcept{
	m_Near = Valtype{0};
	m_Far = Valtype{0};
	return *this;
}

template<typename Valtype> inline 
Interval<Valtype>& Interval<Valtype>::Init( Valtype nearval, Valtype length ) noexcept{
	m_Near = nearval;
	m_Far = nearval + length;
	return *this;
}

template<typename Valtype> inline constexpr
auto Interval<Valtype>::Sign() const noexcept ->decltype(Valtype{}/Valtype{}){
	return static_cast<decltype(Valtype{}/Valtype{})>(m_Near > m_Far ? -1 : +1);
}

template<typename Valtype> inline constexpr
bool Interval<Valtype>::Normal() const noexcept{
	return m_Near <= m_Far;
}

template<typename Valtype> inline 
Interval<Valtype>& Interval<Valtype>::Normalize() noexcept{
	if( !Normal() )
		Flip();
	return *this;
}

template<typename Valtype> inline
Interval<Valtype>& Interval<Valtype>::Flip() noexcept{
	std::swap( m_Near, m_Far );
	return *this;
}

template<typename Valtype> inline constexpr
Valtype Interval<Valtype>::Near() const noexcept{
	return m_Near;
}

template<typename Valtype> inline constexpr
Valtype Interval<Valtype>::Far() const noexcept{
	return m_Far;
}

template<typename Valtype> inline 
Interval<Valtype>& Interval<Valtype>::Near( Valtype val ) noexcept{
	m_Near = val;
	return *this;
}

template<typename Valtype> inline 
Interval<Valtype>& Interval<Valtype>::Far( Valtype val ) noexcept{
	m_Far = val;
	return *this;
}

template<typename Valtype> inline constexpr
Valtype Interval<Valtype>::Max() const noexcept{
	return Normal() ? Far() : Near();
}

template<typename Valtype> inline constexpr
Valtype Interval<Valtype>::Min() const noexcept{
	return Normal() ? Near() : Far();
}

template<typename Valtype> inline constexpr
Valtype Interval<Valtype>::Length() const noexcept{
	if( std::numeric_limits<Valtype>::has_infinity ){
		if( m_Far == std::numeric_limits<Valtype>::infinity() ||
			m_Near == -std::numeric_limits<Valtype>::infinity() )
			return std::numeric_limits<Valtype>::infinity();
	}

	return m_Far - m_Near;
}

template<typename Valtype> inline 
Interval<Valtype>& Interval<Valtype>::Length( Valtype length ) noexcept{
	m_Near = -length/2;
	m_Far = length/2;
	return *this;
}

template<typename Valtype> inline constexpr
Valtype Interval<Valtype>::Center() const noexcept{
	assert( Normal() );
	if( std::numeric_limits<Valtype>::has_infinity ){
		if( m_Far == std::numeric_limits<Valtype>::infinity() )
			return m_Near == -std::numeric_limits<Valtype>::infinity() ? Valtype{0} : std::numeric_limits<Valtype>::infinity();
		if( m_Near == -std::numeric_limits<Valtype>::infinity() )
			return -std::numeric_limits<Valtype>::infinity();
	}

#if __cplusplus >= 201703L //C++17
	if constexpr( std::numeric_limits<Valtype>::is_integer )
		return (m_Far + m_Near) >> 1;
	else 
#endif
	return (m_Far + m_Near) / 2;
}

template<typename Valtype> inline 
Interval<Valtype>& Interval<Valtype>::Union( const Interval& i1, const Interval& i2 ) noexcept{
	m_Near = i1.m_Near< i2.m_Near? i1.m_Near: i2.m_Near;
	m_Far  = i1.m_Far > i2.m_Far ? i1.m_Far : i2.m_Far;
	return *this;
}

template<typename Valtype> inline 
Interval<Valtype>& Interval<Valtype>::Union( const Interval& interval ) noexcept{
	Union( *this, interval );
	return *this;
}

template<typename Valtype> inline 
Interval<Valtype>& Interval<Valtype>::Intersection( const Interval& i1, const Interval& i2 ) noexcept{
	*this = common::Intersection( i1, i2 );
	return *this;
}

template<typename Valtype> inline 
Interval<Valtype>& Interval<Valtype>::Intersection( const Interval& interval ) noexcept{
	*this = common::Intersection( *this, interval );
	return *this;
}

template<typename Valtype> inline constexpr
bool Interval<Valtype>::Includes( Valtype val ) const noexcept{
	return Normal() ? m_Near <= val && val < m_Far : m_Far < val && val <= m_Near;
}

template<typename Valtype> inline constexpr
bool Interval<Valtype>::Includes( const Interval& interval ) const noexcept{
	return	Includes( interval.m_Near ) &&  
			(Includes( interval.m_Far ) || m_Far == interval.m_Far);
}

template<typename Valtype> inline constexpr
bool Interval<Valtype>::Touches( Valtype val, Valtype epsilon_ ) const noexcept{
	if( Normal() )
		return m_Near-epsilon_ <= val && val <= m_Far+epsilon_;
	else
		return m_Far-epsilon_ <= val && val <= m_Near+epsilon_;
}

template<typename Valtype> inline 
Valtype Interval<Valtype>::Clip( Valtype& val ) const noexcept{
	return common::Clip( val, m_Near, m_Far );
}

template<typename Valtype> inline 
constexpr const Valtype& Interval<Valtype>::Clip( const Valtype& val ) const noexcept{
	return Normal() ? common::Clamp( val, m_Near, m_Far ) : common::Clamp( val, m_Far, m_Near );
}

template<typename Valtype> inline 
Valtype Interval<Valtype>::Wrap( Valtype& val ) const noexcept{
	val = common::Wrap( val, m_Near, m_Far );
	return val;
}

template<typename Valtype> inline 
Interval<Valtype>& Interval<Valtype>::Expand( Valtype val ) noexcept{
	if( Normal() ){
		if( m_Near > val )
			m_Near = val;
		if( m_Far < val )
			m_Far = val;
	}
	else{
		if( m_Near < val )
			m_Near = val;
		if( m_Far > val )
			m_Far = val;
	}

	return *this;
}

template<typename Valtype> inline 
Interval<Valtype>& Interval<Valtype>::Inflate( Valtype dx ) noexcept{
	if( Normal() ){
		m_Near-= dx;
		m_Far += dx;
	}
	else{
		m_Near+= dx;
		m_Far -= dx;
	}

	return *this;
}

template<typename Valtype> inline 
Interval<Valtype>& Interval<Valtype>::Deflate( Valtype dx ) noexcept{
	return Inflate( -dx );
}

template<typename Valtype> inline 
Interval<Valtype>& Interval<Valtype>::Move( Valtype dx ) noexcept{
	m_Near+= dx;
	m_Far += dx;
	return *this;
}

template<typename Valtype> inline 
Interval<Valtype>& Interval<Valtype>::ToParent( Interval& interval ) const noexcept{
	if( Normal() )
		interval = Near() + interval;
	else
		interval = Near() - interval;

	return interval;
}

template<typename Valtype> inline 
Interval<Valtype>& Interval<Valtype>::FromParent( Interval& interval ) const noexcept{
	if( Normal() )
		interval = interval - Near();
	else
		interval = Near() - interval;

	return interval;
}

template<typename Valtype> inline 
constexpr bool Interval<Valtype>::Equals( const Interval<Valtype>& interval, Valtype epsilon_ ) const noexcept{
	return	common::Equals( m_Near, interval.m_Near, epsilon_ ) &&
			common::Equals( m_Far, interval.m_Far, epsilon_ );
}

template<typename Valtype> inline 
Interval<Valtype>& Interval<Valtype>::Round( int toDigit ) noexcept{
	m_Near = common::Round( m_Near, toDigit );
	m_Far = common::Round( m_Far, toDigit );
	return *this;
}

template<typename Valtype> inline 
void operator+=( Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept{
	i1.Union( i2 );
}

template<typename Valtype> inline constexpr
Interval<Valtype> operator+( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept{
	Interval<Valtype> retval;
	retval.Union( i1, i2 );
	return retval;
}

template<typename Valtype> inline constexpr
Interval<Valtype> operator+( const Interval<Valtype>& i1, Valtype l ) noexcept{
	return { i1.m_Near + l, i1.m_Far + l };
}

template<typename Valtype> inline constexpr
Interval<Valtype> operator+( Valtype l, const Interval<Valtype>& i1 ) noexcept{
	return i1 + l;
}

template<typename Valtype> inline constexpr
Interval<Valtype> operator+( const Interval<Valtype>& i1 ) noexcept{
	return { +i1.m_Near, +i1.m_Far };
}

template<typename Valtype> inline constexpr
Interval<Valtype> operator-( const Interval<Valtype>& i1, Valtype l ) noexcept{
	return { i1.m_Near - l, i1.m_Far - l };
}

template<typename Valtype> inline constexpr
Interval<Valtype> operator-(Valtype l,const Interval<Valtype>& i1) noexcept{
	return -(i1 - l);
}

template<typename Valtype> inline constexpr
Interval<Valtype> operator-(const Interval<Valtype>& i1) noexcept{
	return { -i1.m_Near, -i1.m_Far };
}

template<typename Valtype, typename Valtype2>
inline constexpr auto operator*( const Interval<Valtype>& i1, Valtype2 scalar ) noexcept -> Interval<decltype(Valtype{}*Valtype2{})>{
	return { i1.m_Near * scalar, i1.m_Far * scalar };
}

template<typename Valtype,typename Valtype2>
inline constexpr auto operator*(Valtype scalar,const Interval<Valtype2>& i1) noexcept -> Interval<decltype(Valtype{}* Valtype2{})>{
	return i1 * scalar;
}

template<typename Valtype, typename Valtype2> inline constexpr
Interval<Valtype>& operator*=( Interval<Valtype>& i1, Valtype2 scalar ) noexcept{
	i1.m_Near *= scalar;
	i1.m_Far *= scalar;
	return i1;
}

template<typename Valtype,typename Valtype2>
inline constexpr auto operator/(const Interval<Valtype>& i1,Valtype2 scalar) noexcept -> Interval<decltype(Valtype{}/ Valtype2{})>{
	return { i1.m_Near / scalar, i1.m_Far / scalar };
}

template<typename Valtype,typename Valtype2>
inline constexpr Interval<Valtype>& operator/=( Interval<Valtype>& i1, Valtype2 scalar ) noexcept{
	i1.m_Near /= scalar;
	i1.m_Far /= scalar;
	return i1;
}

template<typename Valtype> inline constexpr
bool operator<( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept{
	return	i1.Near() < i2.Near() && 
			i1.Near() <= i2.Far() && 
			i1.Far() <= i2.Near() && 
			i1.Far() <= i2.Far();
}

template<typename Valtype> inline constexpr
bool operator>( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept{
	return i2 < i1;
}

template<typename Valtype>
inline constexpr bool SmallerClosed( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept{
	return	i1.Near() < i2.Near() && 
			i1.Near() < i2.Far() && 
			i1.Far() < i2.Near() && 
			i1.Far() < i2.Far();
}

template<typename Valtype>
inline constexpr bool SmallerOpen( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept{
	return	i1.Near() <= i2.Near() && 
			i1.Near() <= i2.Far() && 
			i1.Far() <= i2.Near() && 
			i1.Far() <= i2.Far();
}

template<typename Valtype>
inline constexpr bool GreaterClosed( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept{
	return SmallerClosed( i2, i1 );
}

template<typename Valtype>
inline constexpr bool GreaterOpen( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept{
	return SmallerOpen( i2, i1 );
}

template<typename Valtype> constexpr
bool operator==( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept{
	return i1.m_Near == i2.m_Near && i1.m_Far == i2.m_Far;
}

template<typename Valtype> constexpr
bool operator!= ( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept{
	return i1.m_Near != i2.m_Near || i1.m_Far != i2.m_Far;	
}

template<typename Valtype> inline constexpr
bool Intersecting( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept{
	return !(i1 < i2 || i1 > i2);
}

template<typename Valtype>
inline constexpr bool IntersectingClosed( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept{
	return !(SmallerClosed( i1, i2 ) || GreaterClosed( i1, i2 ));
}

template<typename Valtype>
inline constexpr bool IntersectingOpen( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept{
	return !(SmallerOpen( i1, i2 ) || GreaterOpen( i1, i2 ));
}

template<typename Valtype>
inline constexpr bool Touching( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept{
	return	Intersecting(i1,i2)																					||
			abs(i1.Near() - i2.Near()) * Valtype{1} <= std::numeric_limits<Valtype>::epsilon() * abs(i1.Near())	||
			abs(i1.Near() - i2.Far())  * Valtype{1} <= std::numeric_limits<Valtype>::epsilon() * abs(i1.Near())	||
			abs(i1.Far() - i2.Near())  * Valtype{1} <= std::numeric_limits<Valtype>::epsilon() * abs(i1.Far())	||
			abs(i1.Far() - i2.Far())   * Valtype{1} <= std::numeric_limits<Valtype>::epsilon() * abs(i1.Far());
}

template<typename Valtype> constexpr
Interval<Valtype> Intersection( const Interval<Valtype>& i1, const Interval<Valtype>& i2 ) noexcept{
	if( !Intersecting( i1, i2 ) )
		return {Valtype{0},Valtype{0}};

	if( i1.Normal() ){
		if( i2.Normal() )
			return {(std::max)(i1.Near(),i2.Near()), (std::min)(i1.Far(),i2.Far())};
		else
			return {(std::max)(i1.Near(),i2.Far()), (std::min)(i1.Far(),i2.Near())};
	}
	else{
		if( i2.Normal() )
			return {(std::min)(i1.Near(),i2.Far()), (std::max)(i1.Far(),i2.Near())};
		else
			return {(std::min)(i1.Near(),i2.Near()), (std::max)(i1.Far(),i2.Far())};
	}
}

template<class Valtype, class EvalFuncType>
void Sample( const common::Interval<Valtype> range, const EvalFuncType Evaluate ){
	Valtype s = range.Near();
	if( range.Normal() )
	{
		do{
			s += Evaluate( s );
		}
		while( s < range.Far() - std::numeric_limits<Valtype>::epsilon() );
	}
	else
	{
		do{
			s -= Evaluate( s );
		}
		while( s > range.Far() + std::numeric_limits<Valtype>::epsilon() );
	}

	Evaluate( range.Far() );
}

}


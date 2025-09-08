//	trax track library
//	AD 2013 
//
//  "I am the defector and you're the farmer's daughter"
//
//											Bell X1
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

#include "common/Helpers.h"

#include <cassert>
#include <cmath>
#include <vector>


namespace spat{

	template<typename> struct Position;
	template<typename> struct Position2D;
	template<typename> struct Vector2D;

	/// \brief Implements a 3D - vector in cartesian coordinates.
	///
	/// Specifies the coordinates of a direction in 3D space. For points, like 
	/// e.g. vertices, use the Position type.
	template<typename Valtype>
	struct Vector
	{
		typedef Valtype value_type;

		Valtype dx;	///< cartesian x component.
		Valtype dy;	///< cartesian y component.
		Valtype dz;	///< cartesian z component.


		/// \brief Identifiers for special vectors.
		enum class specials{
			null= -1,	///< Vector with all parameters zeroed.
			ex	= 0,	///< Unit vector in x-direction.
			ey	= 1,	///< Unit vector in y-direction.
			ez	= 2		///< Unit vector in z-direction.
		};


		/// \name Construction
		/// \param dx cartesian x component.
		/// \param dy cartesian y component.
		/// \param dz cartesian z component.
		/// \param s One of the specials values. Vector will be initialized accordingly.	
		/// \param p Position for explicit conversion.
		/// \param pVal	Array of three values.
		///@{
				 constexpr	Vector() noexcept = default;								///< Does not initialize the members.	
				 constexpr	Vector( Valtype dx, Valtype dy, Valtype dz ) noexcept;
		explicit constexpr	Vector( specials s ) noexcept;
		explicit			Vector( const Position<Valtype>& p ) = delete;	///< Conversions between Position and Vector shouldn't be necessary.
		explicit constexpr	Vector( const Vector2D<Valtype>& v2D ) noexcept;
		template<typename Valtype2>
		explicit			Vector( const Valtype2* pVal ) noexcept;
		///@}


		/// \name Assignment
		/// \param pVal Array of three values.
		/// \param vector source vector
		///@{
		template<typename Valtype2>
		Vector& operator=( const Valtype2* pVal ) noexcept;

		template<typename Valtype2>
		Vector& operator=( const Vector2D<Valtype2>& vector ) noexcept;
		///@}


		//template<typename Valtype2>
		//inline auto operator*( Valtype2 f ) const noexcept -> Vector<decltype(Valtype{}*Valtype2{})> {
		//	return { dx * f, dy * f, dz * f };
		//}


		/// \name Subscripting.
		/// \returns Reference to value.
		///@{
		const Valtype& operator[]( size_t index ) const;

		Valtype& operator[]( size_t index );
		///@}


		/// \brief Pointer accessor for direct copying
		Valtype* ptr() noexcept;


		/// \brief Pointer accessor for direct copying
		const Valtype* ptr() const noexcept;


		/// \brief Initializes the vector.
		/// \param s Type of vector to initialize with.
		Vector& Init( specials s = specials::ex ) noexcept;


		/// \brief Test the vector to mach a special vector exactly.
		/// \param s special type of vector to check.
		constexpr bool Is( specials s ) const noexcept;


		/// \brief Calculates the length of the vector
		/// \return Length of vector.
		constexpr Valtype Length() const noexcept;


		using AngleType = decltype(Valtype{}/Valtype{});

		/// \name AircraftPrincipalAxes
		/// \brief Calculate the vector as normal vector from Yaw and Pitch.
		/// 
		/// If the dx,dy are small the yaw will be calculated to be zero.
		/// 
		/// \param yaw The Yaw angle. The angle in the plane.
		/// \param pitch The Pitch angle. The angle vertical to a plane.
		/// \param epsilon length in x/y direction to ignore for yaw.
		/// \returns yaw, pitch, roll in a Position struct.
		///@{
		Vector& AircraftPrincipalAxes( AngleType yaw, AngleType pitch );

		Vector& AircraftPrincipalAxes( const Position2D<AngleType>& angles );

		Position2D<AngleType> AircraftPrincipalAxes( Valtype epsilon = std::numeric_limits<Valtype>::epsilon() ) const;
		///@}


		/// \brief Normalizes the vector to length 1.
		/// \return Previous length.
		auto Normalize() noexcept -> decltype(Valtype{}/Valtype{});


		/// \returns true if the length of the vectr is 1 within the limits of epsilon.
		constexpr inline bool IsNormal( Valtype epsilon_length = 2*std::numeric_limits<Valtype>::epsilon() ) const noexcept;


		///	\brief Rotate this vector around unit vector r with angle in radiants.
		Vector& Rotate( const Vector<AngleType>& r, AngleType angle ) noexcept;


		///	\brief Rotate this vector around r with angle r.Length() in radiants.
		Vector& Rotate( const Vector<AngleType>& r ) noexcept;


		/// \brief Part of this Vector which is parallel to normal.
		///	\param normal Reference vector to meassure this against. Must not be 0.
		/// \return A vector parallel to normal.
		template<typename Valtype2> constexpr
		Vector<Valtype> ParallelProjection( const Vector<Valtype2>& normal ) const noexcept;


		/// \brief Calculate part of this tVector wich is orthogonal to normal.
		///	\param normal Reference vector to meassure this against. Must not be 0.
		/// \return A vector orthogonal to normal.
		template<typename Valtype2> constexpr
		Vector<Valtype> OrthogonalProjection( const Vector<Valtype2>& normal ) const noexcept;


		/// \brief Comparison within some range.
		///
		///	The area within two vectors regarded as equal is a sphere with radius epsilon.
		/// In case of a unit vector epsilon equals the angle between the two vectors, as long
		/// as epsilon is small.
		/// \param v Vector to compare with.
		/// \param epsilon Distance <= wich two vectors are 
		/// regarded as equal.
		/// \return True if positions are nearby each other, else false.
		constexpr bool Equals( const Vector& v, Valtype epsilon = Valtype{0} ) const noexcept;


		/// \brief Rounds the vectors values to a certain digit after the point.
		Vector& Round( int toDigit ) noexcept;
	};

	/// \name Special Vectors
	///@{
	template<typename Valtype>
	constexpr Vector<Valtype> Null{Valtype{0},Valtype{0},Valtype{0}};

	template<typename Valtype>
	constexpr Vector<Valtype> Ex{Valtype{1},Valtype{0},Valtype{0}};

	template<typename Valtype>
	constexpr Vector<Valtype> Ey{Valtype{0},Valtype{1},Valtype{0}};

	template<typename Valtype>
	constexpr Vector<Valtype> Ez{Valtype{0},Valtype{0},Valtype{1}};
	///@}

	/// \brief Cast to Vector with different underlying value type.
	template<typename Valtype,typename Valtype2>
	Vector<Valtype>& spatial_cast( Vector<Valtype2>& from ) noexcept;

	/// \brief Cast to Vector with different underlying value type.
	template<typename Valtype,typename Valtype2>
	const Vector<Valtype>& spatial_cast( const Vector<Valtype2>& from ) noexcept;


	/// \name Operators for Vector
	///
	/// \brief Algebra for Vectors.
	///@{
	template<typename Valtype> constexpr 
	Vector<Valtype> operator+( const Vector<Valtype>& v1, const Vector<Valtype>& v2 ) noexcept;
	template<typename Valtype> 
	Vector<Valtype>& operator+=( Vector<Valtype>& v1, const Vector<Valtype>& v2 ) noexcept;
	template<typename Valtype> constexpr 
	Vector<Valtype> operator+( const Vector<Valtype>& v ) noexcept;
	template<typename Valtype> constexpr 
	Vector<Valtype> operator-( const Vector<Valtype>& v1, const Vector<Valtype>& v2 ) noexcept;
	template<typename Valtype> 
	Vector<Valtype>& operator-=( Vector<Valtype>& v1, const Vector<Valtype>& v2 ) noexcept;
	template<typename Valtype> constexpr 
	Vector<Valtype> operator-( const Vector<Valtype>& v ) noexcept;
	template<typename Valtype,typename Valtype2> constexpr 
	auto operator*( const Vector<Valtype>& v1, const Vector<Valtype2>& v2 ) noexcept -> decltype(Valtype{}*Valtype2{}); ///< Inner product.
	template<typename Valtype> constexpr 
	auto operator*( Valtype f, const Vector<Valtype>& v ) noexcept -> Vector<decltype(Valtype{}*Valtype{})>;
	template<typename Valtype> constexpr 
	auto operator*( const Vector<Valtype>& v, Valtype f ) noexcept -> Vector<decltype(Valtype{}*Valtype{})>;
	template<typename Valtype,typename Valtype2> constexpr 
	auto operator*( Valtype f, const Vector<Valtype2>& v ) noexcept -> Vector<decltype(Valtype{}*Valtype2{})>;
	template<typename Valtype,typename Valtype2> constexpr 
	auto operator*( const Vector<Valtype>& v, Valtype2 f ) noexcept -> Vector<decltype(Valtype{}*Valtype2{})>;
	template<typename Valtype,typename Valtype2> 
	Vector<Valtype>& operator*=( Vector<Valtype>& v, Valtype2 f ) noexcept;
	template<typename Valtype>
	Vector<Valtype>& operator*=( Vector<Valtype>& v1, const Vector<Valtype>& v2 ) noexcept = delete;
	template<typename Valtype,typename Valtype2> constexpr 
	auto operator%( const Vector<Valtype>& v1, const Vector<Valtype2>& v2 ) noexcept -> Vector<decltype(Valtype{}*Valtype2{})>; ///< Cross product.
	template<typename Valtype>
	Vector<Valtype>& operator%=( Vector<Valtype>& v1, const Vector<Valtype>& v2 ) noexcept;
	template<typename Valtype,typename Valtype2> constexpr 
	auto operator/( const Vector<Valtype>&, const Vector<Valtype2>& ) = delete;
	template<typename Valtype,typename Valtype2> constexpr 
	auto operator/( const Vector<Valtype>& v, Valtype2 f ) noexcept -> Vector<decltype(Valtype{}/Valtype2{})>;
	template<typename Valtype,typename Valtype2> constexpr 
	auto operator/( const Valtype& f, const Vector<Valtype2>& v ) = delete;
	template<typename Valtype,typename Valtype2>
	Vector<Valtype>& operator/=( Vector<Valtype>& v, Valtype2 f ) noexcept;
	template<typename Valtype> constexpr 
	bool operator==( const Vector<Valtype>& v1, const Vector<Valtype>& v2 ) noexcept;
	template<typename Valtype> constexpr 
	bool operator!=( const Vector<Valtype>& v1, const Vector<Valtype>& v2 ) noexcept;
	///@}


	/// \name Rotate
	/// \brief Rotates Vectors around a rotational axis.
	/// \param r rotational axis with angle r.Length() in radiants.
	/// \param pVec Pointer to vector array.
	/// \param nVec Number of Vectors in array.
	/// \param vec Reference to Vector to rotate.
	///@{
	template<typename Valtype>
	void Rotate( Vector<Valtype>* pVec, unsigned int nVec, const Vector<Valtype>& r ) noexcept;

	template<typename Valtype>
    void Rotate( std::vector<Vector<Valtype>>& vec, const Vector<Valtype>& r ) noexcept;
	///@}


	/// \brief Comparison within some range.
	///
	///	The area within two vectors regarded as equal is a sphere with radius epsilon.
	/// In case of a unit vector epsilon equals the angle between the two vectors, as long
	/// as epsilon is small.
	/// \param v1 Vector to compare.
	/// \param v2 Vector to compare with.
	/// \param epsilon Distance <= wich two vectors are 
	/// regarded as equal.
	/// \return True if positions are nearby each other, else false.
	template<typename Valtype>
	constexpr bool Equals( const Vector<Valtype>& v1, const Vector<Valtype>& v2, Valtype epsilon = Valtype{0} ) noexcept;


	/// \name Parallelity
	/// \brief Function return true if the parallel codition is met.
	/// 
	/// 'Ortho' means the vectors are parallel and point in the same direction. 'Anti'
	/// means the vectors are parallel but point in different directions. A Null<Valtype> 
	/// vector is orthoparallel to any and antiparallel to none.
	/// \param v1 first vector.
	/// \param v2 vector to be compared with the first vector.
	/// \param epsilon angle between the two vectors to be tolerated as 'parallel'.
	/// \returns True if the two vectors are parallel, false elsewise.
	///@{
	template<typename Valtype> constexpr 
	bool Parallel( const Vector<Valtype>& V1, const Vector<Valtype>& V2, decltype(Valtype{}/Valtype{}) epsilon = std::numeric_limits<decltype(Valtype{}/Valtype{})>::epsilon() ) noexcept;

	template<typename Valtype> constexpr 
	bool OrthoParallel( const Vector<Valtype>& V1, const Vector<Valtype>& v2, decltype(Valtype{}/Valtype{}) epsilon = std::numeric_limits<decltype(Valtype{}/Valtype{})>::epsilon() ) noexcept;

	template<typename Valtype> constexpr 
	bool AntiParallel( const Vector<Valtype>& V1, const Vector<Valtype>& V2, decltype(Valtype{}/Valtype{}) epsilon = std::numeric_limits<decltype(Valtype{}/Valtype{})>::epsilon() ) noexcept;
	///@}


	/// \brief Outer normalizing
	/// \returns The length of the Vector as a dimensionated value and a Vector of unit length with dimension One.
	template<typename Valtype>
	auto Normalize( const Vector<Valtype>& v ) noexcept -> std::pair<Valtype,Vector<decltype(Valtype{}/Valtype{})>>;


	/// \name Slerp
	/// \brief Linear Vector interpolation
	/// \param a, b Vectors to interpolate
	/// \param na, nb normalized vectors to interpolate.
	/// \param t [0,1] interpolation parameter. 0 would be a, 1 would become b.
	///@{
	template<typename Valtype>
	Vector<Valtype> Slerp( const Vector<Valtype>& a, const Vector<Valtype>& b, Valtype t ) noexcept;

	template<typename Valtype>
	Vector<Valtype> SlerpNormals( const Vector<Valtype>& na, const Vector<Valtype>& nb, Valtype t ) noexcept;

	template<typename Valtype>
	std::pair<Vector<Valtype>,Vector<Valtype>> SlerpDNormals( const Vector<Valtype>& na, const Vector<Valtype>& nb, Valtype t ) noexcept;
	///@}


	template<typename Valtype>
	constexpr size_t size( const Vector<Valtype> ) noexcept {
		return 3;
	}


template<typename Valtype>
inline constexpr Vector<Valtype>::Vector( Valtype dx, Valtype dy, Valtype dz ) noexcept
	:	dx{dx}, dy{dy}, dz{dz} 
{}

template<typename Valtype>
inline constexpr Vector<Valtype>::Vector( specials s ) noexcept
	:	dx{s == specials::ex ? Valtype{1} : Valtype{0}}, 
		dy{s == specials::ey ? Valtype{1} : Valtype{0}}, 
		dz{s == specials::ez ? Valtype{1} : Valtype{0}} 
{}

template<typename Valtype>
inline constexpr Vector<Valtype>::Vector( const Vector2D<Valtype>& v2D ) noexcept
	:	dx{v2D.dx}, dy{v2D.dy}, dz{0} 
{}

template<typename Valtype>
template<typename Valtype2>
inline Vector<Valtype>::Vector( const Valtype2* pVal ) noexcept
	:	dx{Valtype{static_cast<Valtype>(pVal[0])}}, 
		dy{Valtype{static_cast<Valtype>(pVal[1])}}, 
		dz{Valtype{static_cast<Valtype>(pVal[2])}} 
{}

template<typename Valtype>
template<typename Valtype2>
inline Vector<Valtype>& Vector<Valtype>::operator=( const Valtype2* pVal ) noexcept{
	dx = Valtype{pVal[0]};
	dy = Valtype{pVal[1]};
	dz = Valtype{pVal[2]};
	return *this;
}

template<typename Valtype>
template<typename Valtype2>
inline Vector<Valtype>& Vector<Valtype>::operator=( const Vector2D<Valtype2>& vector ) noexcept{
	dx = static_cast<Valtype>(vector.dx);
	dy = static_cast<Valtype>(vector.dy);
	return *this;
}

template<typename Valtype>
inline const Valtype& Vector<Valtype>::operator[]( size_t index ) const{
	if( index < 3 )
		return (&dx)[index];

	throw std::out_of_range( "invalid Vector subscript" );
}

template<typename Valtype>
inline Valtype& Vector<Valtype>::operator[]( size_t index ){
	if( index < 3 )
		return (&dx)[index];

	throw std::out_of_range( "invalid Vector subscript" );
}

template<typename Valtype>
inline Valtype* Vector<Valtype>::ptr() noexcept{
	return &dx;
}

template<typename Valtype>
inline const Valtype* Vector<Valtype>::ptr() const noexcept{
	return &dx;
}

template<typename Valtype>
inline Vector<Valtype>& Vector<Valtype>::Init( specials s ) noexcept{
	dx = s == specials::ex ? Valtype{1} : Valtype{0}; 
	dy = s == specials::ey ? Valtype{1} : Valtype{0}; 
	dz = s == specials::ez ? Valtype{1} : Valtype{0};
	return *this;
}

template<typename Valtype>
constexpr bool Vector<Valtype>::Is( specials s ) const noexcept{
	switch(s){
	case specials::null:
		return *this == Null<Valtype>;
	case specials::ex:
		return *this == Ex<Valtype>;
	case specials::ey:
		return *this == Ey<Valtype>;
	case specials::ez:
		return *this == Ez<Valtype>;
	default:
		assert( false );
	}

	return false;
}

template<typename Valtype> constexpr
inline Valtype Vector<Valtype>::Length() const noexcept{	
	return sqrt( dx*dx + dy*dy + dz*dz ); 
}

template<typename Valtype>
inline Vector<Valtype>& Vector<Valtype>::AircraftPrincipalAxes( AngleType yaw, AngleType pitch )
{
	*this = Length() * Ex<AngleType>;
	Vector<AngleType> N = Ey<AngleType>;
	N.Rotate( Ez<AngleType>, yaw );

	return Rotate( Ez<AngleType>, yaw ).Rotate( N, pitch );
}

template<typename Valtype>
inline Vector<Valtype>& Vector<Valtype>::AircraftPrincipalAxes( const Position2D<AngleType>& angles )
{
	return AircraftPrincipalAxes( angles.x, angles.y );
}

template<typename Valtype>
inline Position2D<typename Vector<Valtype>::AngleType> Vector<Valtype>::AircraftPrincipalAxes( Valtype _epsilon ) const
{
	Valtype sqrtdxdxplusdydy = sqrt( dx*dx + dy*dy );
	if( common::Equals( sqrtdxdxplusdydy, Valtype{0}, _epsilon ) ){
		const Vector<Valtype>::AngleType pi__{3.141592654f};

		return { Vector<Valtype>::AngleType{0}, 
				 -common::Sign( dz ) * pi__ / 2 };
	}

	return {
		atan2( dy, dx ),
		atan2( -dz, sqrtdxdxplusdydy )
	};
}

template<typename Valtype>
inline auto Vector<Valtype>::Normalize() noexcept -> decltype(Valtype{}/Valtype{}){	
	decltype(Valtype{}/Valtype{}) L = Length() / Valtype{1};
	if( !L ){
		Init();
		return decltype(Valtype{}/Valtype{}){0};
	}
	*this /= L;
	return L;
}

template<typename Valtype> constexpr
inline bool Vector<Valtype>::IsNormal( Valtype _epsilon_length ) const noexcept{
	return abs(Length() - Valtype{1}) <= _epsilon_length;
}

template<typename Valtype>
Vector<Valtype>& Vector<Valtype>::Rotate( const Vector<AngleType>& r, AngleType angle ) noexcept{
	if( angle == AngleType{ 0 } )
		return *this;
	Vector& v = *this;
	const Vector rxv = r % v;

	Valtype IrxvI = rxv.Length();
	if( IrxvI == Valtype{0} )
		return *this;

	const Vector rvr = (r * v) * r;
	const Vector vmrvr = v - rvr;

	v = rvr + cos( angle ) * vmrvr + sin( angle ) * vmrvr.Length()/IrxvI * rxv;
	return *this;
}

template<typename Valtype>
Vector<Valtype>& Vector<Valtype>::Rotate( const Vector<AngleType>& r ) noexcept{
	Vector<AngleType> nr{r};
	AngleType angle = nr.Normalize();

	Rotate( nr, angle );
	return *this;
}

template<typename Valtype>
template<typename Valtype2> constexpr
inline Vector<Valtype> Vector<Valtype>::ParallelProjection( const Vector<Valtype2>& normal ) const noexcept{
	Valtype2 nl2 = normal * normal;
	if( nl2 == Valtype2{0} )
		return Null<Valtype>;

	return (*this * normal) / nl2 * normal;
}

template<typename Valtype>
template<typename Valtype2> constexpr
inline Vector<Valtype> Vector<Valtype>::OrthogonalProjection( const Vector<Valtype2>& normal ) const noexcept{
	return *this - ParallelProjection( normal );
}

template<typename Valtype>
constexpr bool Vector<Valtype>::Equals( const Vector<Valtype>& v, Valtype epsilon_ ) const noexcept{
	const Vector<Valtype> d{*this - v};
	return d*d <= epsilon_*epsilon_;
}

template<typename Valtype>
inline Vector<Valtype>& Vector<Valtype>::Round( int toDigit ) noexcept{
	dx = common::Round(dx,toDigit);
	dy = common::Round(dy,toDigit);
	dz = common::Round(dz,toDigit);
	return *this;
}

template<typename Valtype,typename Valtype2>
inline Vector<Valtype>& spatial_cast( Vector<Valtype2>& from ) noexcept{
	static_assert( sizeof(Valtype) == sizeof(Valtype2), "Can not spatial_cast with different memory layout" );
	return reinterpret_cast<Vector<Valtype>&>(from);
}

template<typename Valtype,typename Valtype2>
inline const Vector<Valtype>& spatial_cast( const Vector<Valtype2>& from ) noexcept{
	static_assert( sizeof(Valtype) == sizeof(Valtype2), "Can not spatial_cast with different memory layout" );
	return reinterpret_cast<const Vector<Valtype>&>(from);
}

template<typename Valtype> constexpr 
inline Vector<Valtype> operator+( const Vector<Valtype>& v1, const Vector<Valtype>& v2 ) noexcept{
	return { v1.dx + v2.dx,
			 v1.dy + v2.dy,
			 v1.dz + v2.dz };
}

template<typename Valtype>
inline Vector<Valtype>& operator+=( Vector<Valtype>& v1, const Vector<Valtype>& v2 ) noexcept{
	v1.dx += v2.dx;
	v1.dy += v2.dy;
	v1.dz += v2.dz;
	return v1;
}

template<typename Valtype> constexpr 
inline Vector<Valtype> operator+( const Vector<Valtype>& v ) noexcept{
	return { +v.dx, +v.dy, +v.dz };
}

template<typename Valtype> constexpr 
inline Vector<Valtype> operator-( const Vector<Valtype>& v1, const Vector<Valtype>& v2 ) noexcept{
	return { v1.dx - v2.dx,
			 v1.dy - v2.dy,
			 v1.dz - v2.dz };
}

template<typename Valtype>
inline Vector<Valtype>&	operator-=( Vector<Valtype>& v1, const Vector<Valtype>& v2 ) noexcept{
	v1.dx -= v2.dx;
	v1.dy -= v2.dy;
	v1.dz -= v2.dz;
	return v1;
}

template<typename Valtype> constexpr 
inline Vector<Valtype> operator-( const Vector<Valtype>& v ) noexcept{
	return { -v.dx, -v.dy, -v.dz };
}

template<typename Valtype,typename Valtype2> constexpr 
inline auto operator*( const Vector<Valtype>& v1, const Vector<Valtype2>& v2 ) noexcept -> decltype(Valtype{}*Valtype2{}){
	return v1.dx*v2.dx + v1.dy*v2.dy + v1.dz*v2.dz;
}

template<typename Valtype> constexpr 
inline auto operator*( Valtype f, const Vector<Valtype>& v ) noexcept -> Vector<decltype(Valtype{}*Valtype{})>{
	return { v.dx * f,
			 v.dy * f,
			 v.dz * f };
}

template<typename Valtype> constexpr 
inline auto operator*( const Vector<Valtype>& v, Valtype f ) noexcept -> Vector<decltype(Valtype{}*Valtype{})>{
	return f * v;
}

template<typename Valtype,typename Valtype2> constexpr 
inline auto operator*( Valtype f, const Vector<Valtype2>& v ) noexcept -> Vector<decltype(Valtype{}*Valtype2{})>{
	static_assert( !std::is_integral<Valtype2>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	return { v.dx * f,
			 v.dy * f,
			 v.dz * f };
}

template<typename Valtype,typename Valtype2> constexpr 
inline auto operator*( const Vector<Valtype>& v, Valtype2 f ) noexcept -> Vector<decltype(Valtype{}*Valtype2{})>{
	static_assert( !std::is_integral<Valtype>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	return f * v;
}

template<typename Valtype> constexpr 
inline Vector<int> operator*( const Vector<int>& v, Valtype f ) noexcept {
	return { static_cast<int>(static_cast<Valtype>(v.dx) * f),
			 static_cast<int>(static_cast<Valtype>(v.dy) * f),
			 static_cast<int>(static_cast<Valtype>(v.dz) * f) };
}

template<typename Valtype> constexpr 
inline Vector<int> operator*( Valtype f, const Vector<int>& v ) noexcept{
	return v * f;
}

template<typename Valtype,typename Valtype2>
inline Vector<Valtype>& operator*=( Vector<Valtype>& v, Valtype2 f ) noexcept{
	static_assert( !std::is_integral<Valtype>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	v.dx *= f;
	v.dy *= f;
	v.dz *= f;
	return v;
}

template<typename Valtype>
inline Vector<int>& operator*=( Vector<int>& v, Valtype f ) noexcept{
	v.dx = static_cast<int>(static_cast<Valtype>(v.dx) * f);
	v.dy = static_cast<int>(static_cast<Valtype>(v.dy) * f);
	v.dz = static_cast<int>(static_cast<Valtype>(v.dz) * f);
	return v;
}

template<typename Valtype,typename Valtype2> constexpr 
inline auto operator%( const Vector<Valtype>& v1, const Vector<Valtype2>& v2 ) noexcept -> Vector<decltype(Valtype{}*Valtype2{})>{
	return { v1.dy * v2.dz - v1.dz * v2.dy,
			 v1.dz * v2.dx - v1.dx * v2.dz,
			 v1.dx * v2.dy - v1.dy * v2.dx }; 
}

template<typename Valtype>
inline Vector<Valtype>& operator%=( Vector<Valtype>& v1, const Vector<Valtype>& v2 ) noexcept{
	v1 = v1 % v2;
	return v1;
}

template<typename Valtype,typename Valtype2> constexpr 
inline auto operator/( const Vector<Valtype>& v, Valtype2 f ) noexcept -> Vector<decltype(Valtype{}/Valtype2{})>{
	static_assert( !std::is_integral<Valtype>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	return { v.dx / f,
			 v.dy / f,
			 v.dz / f };
}

template<typename Valtype> constexpr 
inline Vector<int> operator/( const Vector<int>& v, Valtype f ) noexcept{
	return { static_cast<int>(static_cast<Valtype>(v.dx) / f),
			 static_cast<int>(static_cast<Valtype>(v.dy) / f),
			 static_cast<int>(static_cast<Valtype>(v.dz) / f) };
}

template<typename Valtype,typename Valtype2>
inline Vector<Valtype>& operator/=( Vector<Valtype>& v, Valtype2 f ) noexcept{
	static_assert( !std::is_integral<Valtype>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	v.dx /= f;
	v.dy /= f;
	v.dz /= f;
	return v;
}

template<typename Valtype>
inline Vector<int>& operator/=( Vector<int>& v, Valtype f ) noexcept{
	v.dx = static_cast<int>(static_cast<Valtype>(v.dx) / f);
	v.dy = static_cast<int>(static_cast<Valtype>(v.dy) / f);
	v.dz = static_cast<int>(static_cast<Valtype>(v.dz) / f);
	return v;
}

template<typename Valtype> constexpr 
inline bool operator==( const Vector<Valtype>& v1, const Vector<Valtype>& v2 ) noexcept{
	return v1.dx == v2.dx && v1.dy == v2.dy && v1.dz == v2.dz;
}

template<typename Valtype> constexpr 
inline bool operator!=( const Vector<Valtype>& v1, const Vector<Valtype>& v2 ) noexcept{
	return !(v1 == v2);
}

template<typename Valtype>
void Rotate( Vector<Valtype>* pVec, unsigned int nVec, const Vector<Valtype>& rot ) noexcept{
	Vector<Valtype> r{rot};
	Valtype angle = r.Normalize();

	for( size_t idx = 0; idx < nVec; ++idx )
		pVec[idx].Rotate( r, angle );
}

template<typename Valtype>
void Rotate( std::vector<Vector<Valtype>>& vec, const Vector<Valtype>& r ) noexcept{
	Vector<Valtype> nr{r};
	Valtype angle = nr.Normalize();

	for( auto iter = vec.begin(); iter != vec.end(); ++iter )
		(*iter).Rotate( nr, angle );
}

template<typename Valtype>
inline constexpr bool Equals(const Vector<Valtype>& v1,const Vector<Valtype>& v2,Valtype epsilon) noexcept{
	return v1.Equals( v2, epsilon );
}

template<typename Valtype> constexpr 
inline bool Parallel( const Vector<Valtype>& V1, const Vector<Valtype>& V2, decltype(Valtype{}/Valtype{}) epsilon ) noexcept{
	Valtype v1 = V1.Length();
	Valtype v2 = V2.Length();
	if( v1 == Valtype{0} || v2 == Valtype{0} )
		return true;

	if( (V1 % V2).Length() / v1 / v2  > epsilon )
		return false;

	return true;
}

template<typename Valtype> constexpr 
inline bool OrthoParallel( const Vector<Valtype>& V1, const Vector<Valtype>& V2, decltype(Valtype{}/Valtype{}) epsilon ) noexcept{
	if( !Parallel( V1, V2, epsilon ) )
		return false;

	return V1 * V2 >= decltype(Valtype{}*Valtype{}){0};
}

template<typename Valtype> constexpr 
inline bool AntiParallel( const Vector<Valtype>& V1, const Vector<Valtype>& V2, decltype(Valtype{}/Valtype{}) epsilon ) noexcept{
	if( !Parallel( V1, V2, epsilon ) )
		return false;

	return V1 * V2 < decltype(Valtype{}*Valtype{}){0};
}

template<typename Valtype>
auto Normalize( const Vector<Valtype>& v ) noexcept -> std::pair<Valtype,Vector<decltype(Valtype{}/Valtype{})>> {
	Vector<decltype(Valtype{}/Valtype{})> V{	v.dx / Valtype{1},
												v.dy / Valtype{1},
												v.dz / Valtype{1} };
	Valtype L = V.Normalize() * Valtype{1};
	return std::make_pair( L, V );
}

template<typename Valtype>
inline Vector<Valtype> Slerp( const Vector<Valtype>& a_, const Vector<Valtype>& b_, Valtype t ) noexcept
{
	t = common::Clamp( t, Valtype{0}, Valtype{1} );
	Vector<Valtype> na = a_;
	const Valtype a = na.Normalize();
	Vector<Valtype> nb = b_;
	const Valtype b = nb.Normalize();

	Vector<Valtype> r = na % nb;
	const Valtype angle = t*asin( r.Normalize() );

	return ((1-t)*a + t*b) * (cos(angle) * na + sin(angle) * (r % na));
}

template<typename Valtype>
inline Vector<Valtype> SlerpNormals( const Vector<Valtype>& na, const Vector<Valtype>& nb, Valtype t ) noexcept
{
	t = common::Clamp( t, Valtype{0}, Valtype{1} );

	Vector<Valtype> r = na % nb;
	const Valtype angle = t*asin( r.Normalize() );

	return cos(angle) * na + sin(angle) * (r % na);
}

template<typename Valtype>
inline std::pair<Vector<Valtype>,Vector<Valtype>> SlerpDNormals( 
	const Vector<Valtype>& na, 
	const Vector<Valtype>& nb, 
	Valtype t ) noexcept
{
	t = common::Clamp( t, Valtype{0}, Valtype{1} );

	Vector<Valtype> r = na % nb;
	const Valtype angle = asin( r.Normalize() );

	return { cos(t*angle) * na + sin(t*angle) * (r % na), angle*(-sin(t*angle) * na + cos(t*angle) * (r % na)) };
}

} // namespace spat

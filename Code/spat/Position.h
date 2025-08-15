// Spatial Coordinates And Vectors Library (spat)
// 
// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2013 
//
//  "I saw a werewolve with a chinese menue in his hand"
//
//								Warren Zevon

#pragma once

#include "common/Helpers.h"


namespace spat{

	template<typename> struct Vector;
	template<typename> struct PositionH;
	template<typename> struct Position2D;
	template<typename,typename> struct VectorBundle;


	/// \brief Implements a 3D - position in cartesian coordinates.
	///
	/// Specifies the coordinates of a point in 3D space. For Vectors, like 
	/// e.g. surface normals, use the Vector type.
	template<typename Valtype>
	struct Position
	{
		typedef Valtype value_type;

		Valtype x;	///< cartesian x coordinate.
		Valtype y;	///< cartesian y coordinate.
		Valtype z;	///< cartesian z coordinate.


		/// \name Construction
		/// \param x cartesian x coordinate.
		/// \param y cartesian y coordinate.
		/// \param z cartesian z coordinate.
		/// \param v Vector for explicit conversion.				
		/// \param p2d initializes x, y. z gets set to zero.
		/// \param pVal Array of three values.
		/// \param sp Spherical coordinates will get transformed to cartesian.
		///@{
				 constexpr Position() noexcept = default;						///< Does not initialize the members.	
				 constexpr Position( Valtype x, Valtype y, Valtype z ) noexcept;
		explicit		   Position( const Vector<Valtype>& v ) = delete;		///< Conversions between Vector and Position shouldn't be necessary and if needed anyhow explicitly specified.
		template<typename Valtype2>
		explicit constexpr Position( const VectorBundle<Valtype,Valtype2>& b ) noexcept;
		explicit constexpr Position( const Position2D<Valtype>& p ) noexcept;
		template<typename Valtype2>
		explicit constexpr Position( const PositionH<Valtype2>& pos ) noexcept;
		template<typename Valtype2>
		explicit		   Position( const Valtype2* pVal ) noexcept;
		///@}


		/// \name Assignment
		/// \param p2d initializes x, y. z gets set to zero.
		/// \param pVal Array of three values.
		/// \param sp Spherical coordinates will get transformed to cartesian.
		///@{
		Position& operator=( const Position2D<Valtype>& p2d ) noexcept;

		template<typename Valtype2>
		Position& operator=( const Valtype2* pVal ) noexcept;
		///@}


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


		/// \brief Initialisation to origin.
		Position& Init() noexcept;


		/// \brief Moves the position by v.
		Position& Transport( const Vector<Valtype>& v ) noexcept;


		/// \brief Rotates a position by axis (length of axis.T make angle in radints)
		/// \tparam Valtype2 template parameter of axis.T
		/// \param axis rotation axis
		template<typename Valtype2>
		Position& Rotate( const VectorBundle<Valtype,Valtype2>& axis ) noexcept;


		/// \brief Rotates a position by axis and angle in radiants.
		/// \tparam Valtype2 template parameter of axis.T and angle
		/// \param axis rotation axis
		/// \param angle in radiants
		template<typename Valtype2>
		Position& Rotate( const VectorBundle<Valtype,Valtype2>& axis, Valtype2 angle ) noexcept;


		/// \brief Comparison within some range.
		///	The area within two positions regarded as equal is a sphere with radius epsilon.
		/// \param p Position to compare to.
		/// \param epsilon Distance <= wich two positions are 
		/// regarded as equal.
		constexpr bool Equals( const Position& p, Valtype epsilon = Valtype{0} ) const noexcept;


		/// \brief Rounds the positions values to a certain digit after the point.
		Position& Round( int toDigit ) noexcept;
	};


	/// \brief Origin of coordinate system.
	template<typename Valtype>
	constexpr Position<Valtype> Origin3D{Valtype{0},Valtype{0},Valtype{0}};

	/// \brief Cast to Position with different underlying value type.
	template<typename Valtype,typename Valtype2>
	Position<Valtype>& spatial_cast( Position<Valtype2>& from ) noexcept;

	/// \brief Cast to Position with different underlying value type.
	template<typename Valtype,typename Valtype2>
	const Position<Valtype>& spatial_cast( const Position<Valtype2>& from ) noexcept;


	/// \name Operators for Position
	///
	/// \brief Algebra for Positions.
	/// 
	/// Most surprises: you ran over a deleted operator? You should try Vector instead. If you really 
	/// think it would make any sense to add the position of Rome and the position of Moscow to come 
	/// out with Peking, there most probably is something wrong with your reasoning.
	///
	/// To transform a Vector into a Position use Origin3D + Vector.
	/// To transform a Position into a Vector use Position - Origin3D.
	/// But try to avoid it.
	///@{
	template<typename Valtype> constexpr
	Position<Valtype> operator+( const Position<Valtype>&, const Position<Valtype>& ) = delete;
	template<typename Valtype> constexpr
	Position<Valtype> operator+( const Position<Valtype>& p, const Vector<Valtype>& v ) noexcept;
	template<typename Valtype> constexpr
	Position<Valtype> operator+( const Vector<Valtype>&, const Position<Valtype>& ) = delete;
	template<typename Valtype>
	Position<Valtype>& operator+=( Position<Valtype>&, const Position<Valtype>& ) = delete;
	template<typename Valtype>
	Position<Valtype>& operator+=( Position<Valtype>& p, const Vector<Valtype>& v ) noexcept;
	template<typename Valtype>
	Vector<Valtype>& operator+=( Vector<Valtype>&, const Position<Valtype>& ) = delete;
	template<typename Valtype> constexpr
	Position<Valtype> operator+( const Position<Valtype>& ) = delete;
	template<typename Valtype> constexpr
	Vector<Valtype> operator-( const Position<Valtype>& p1, const Position<Valtype>& p2 ) noexcept;
	template<typename Valtype> constexpr
	Position<Valtype> operator-( const Position<Valtype>& p, const Vector<Valtype>& v ) noexcept;
	template<typename Valtype> constexpr
	Position<Valtype> operator-( const Vector<Valtype>&, const Position<Valtype>& ) = delete;
	template<typename Valtype>
	Position<Valtype>& operator-=( Position<Valtype>&, const Position<Valtype>& ) = delete;
	template<typename Valtype>
	Position<Valtype>& operator-=( Position<Valtype>& p, const Vector<Valtype>& v ) noexcept;
	template<typename Valtype> constexpr
	Position<Valtype> operator-( const Position<Valtype>& ) = delete;
	template<typename Valtype> constexpr
	Position<Valtype> operator*( const Position<Valtype>& , const Position<Valtype>& ) = delete;
	template<typename Valtype> constexpr
	auto operator*( Valtype f, const Position<Valtype>& p ) noexcept -> Position<decltype(Valtype{}*Valtype{})>;
	template<typename Valtype> constexpr
	auto operator*( const Position<Valtype>& p, Valtype f ) noexcept -> Position<decltype(Valtype{}*Valtype{})>;
	template<typename Valtype,typename Valtype2> constexpr
	auto operator*( Valtype f, const Position<Valtype2>& p ) noexcept -> Position<decltype(Valtype{}*Valtype2{})>;
	template<typename Valtype,typename Valtype2> constexpr
	auto operator*( const Position<Valtype>& p, Valtype2 f ) noexcept -> Position<decltype(Valtype{}*Valtype2{})>;
	template<typename Valtype,typename Valtype2> 
	Position<Valtype>& operator*=( Position<Valtype>& p, Valtype2 f ) noexcept;
	template<typename Valtype> 
	Valtype& operator*=( Valtype&, const Position<Valtype>& ) = delete;
	template<typename Valtype> constexpr
	Position<Valtype> operator%( const Position<Valtype>& , const Position<Valtype>& ) = delete;
	template<typename Valtype>
	Position<Valtype>& operator%=( const Position<Valtype>& , const Position<Valtype>& ) = delete;
	template<typename Valtype> constexpr
	Valtype operator/( const Position<Valtype>& , const Position<Valtype>& ) = delete;
	template<typename Valtype,typename Valtype2> constexpr
	auto operator/( const Position<Valtype>& p, Valtype2 f ) noexcept -> Position<decltype(Valtype{}/Valtype2{})>;
	template<typename Valtype> constexpr
	Position<Valtype> operator/( Valtype f, const Position<Valtype>& p ) = delete;
	template<typename Valtype,typename Valtype2>
	Position<Valtype>& operator/=( Position<Valtype>& p, Valtype2 f ) noexcept;
	template<typename Valtype> constexpr
	bool operator==( const Position<Valtype>& p1, const Position<Valtype>& p2 ) noexcept;
	template<typename Valtype> constexpr
	bool operator!=( const Position<Valtype>& p1, const Position<Valtype>& p2 ) noexcept;
	///@}


	template<typename Valtype>
	constexpr size_t size( const Position<Valtype> ) noexcept {
		return 3;
	}


template<typename Valtype>
inline constexpr Position<Valtype>::Position( Valtype x, Valtype y, Valtype z ) noexcept
	: x{x}, y{y}, z{z} 
{}

template<typename Valtype>
template<typename Valtype2>
inline constexpr Position<Valtype>::Position( const VectorBundle<Valtype,Valtype2>& b ) noexcept
	: x{b.P.x}, y{b.P.y}, z{b.P.z} 
{}

template<typename Valtype>
inline constexpr Position<Valtype>::Position( const Position2D<Valtype>& p ) noexcept
	: x{p.x}, y{p.y}, z{0} 
{}

template<typename Valtype>
template<typename Valtype2>
inline constexpr Position<Valtype>::Position( const PositionH<Valtype2>& p ) noexcept
	: x{p.x/p.w}, y{p.y/p.w}, z{p.z/p.w} 
{}

template<typename Valtype>
template<typename Valtype2>
inline Position<Valtype>::Position( const Valtype2* pVal ) noexcept
	: x{Valtype{static_cast<Valtype>(pVal[0])}}, 
	  y{Valtype{static_cast<Valtype>(pVal[1])}}, 
	  z{Valtype{static_cast<Valtype>(pVal[2])}}
{}

template<typename Valtype>
inline Position<Valtype>& Position<Valtype>::operator=( const Position2D<Valtype>& p2d ) noexcept{
	x = p2d.x; y = p2d.y;
	return *this;
}

template<typename Valtype>
template<typename Valtype2>
inline Position<Valtype>& Position<Valtype>::operator=( const Valtype2* pVal ) noexcept{
	x = Valtype{pVal[0]};
	y = Valtype{pVal[1]};
	z = Valtype{pVal[2]};
	return *this;
}

template<typename Valtype>
template<typename Valtype2>
inline Position<Valtype>& Position<Valtype>::Rotate( const VectorBundle<Valtype,Valtype2>& axis ) noexcept
{
	VectorBundle<Valtype,Valtype2> r(axis);
	Valtype2 angle = r.T.Normalize();

	return Rotate( r, angle );
}

template<typename Valtype>
template<typename Valtype2>
inline Position<Valtype>& Position<Valtype>::Rotate( const VectorBundle<Valtype,Valtype2>& axis, Valtype2 angle ) noexcept
{
	Vector<Valtype> D = (*this - axis.P);
	Vector<Valtype> Dp = D * axis.T * axis.T;
	Vector<Valtype> Do = D - Dp;
	*this = axis.P + Dp + cos(angle) * Do + axis.T % Do * sin(angle);
	return *this;
}

template<typename Valtype>
inline const Valtype& Position<Valtype>::operator[]( size_t index ) const{
	if( index < 3 )
		return (&x)[index];

	throw std::out_of_range( "invalid Position subscript" );
}

template<typename Valtype>
inline Valtype& Position<Valtype>::operator[]( size_t index ){
	if( index < 3 )
		return (&x)[index];

	throw std::out_of_range( "invalid Position subscript" );
}

template<typename Valtype>
inline Valtype* Position<Valtype>::ptr() noexcept{
	return &x;
}

template<typename Valtype>
inline const Valtype* Position<Valtype>::ptr() const noexcept{
	return &x;
}

template<typename Valtype>
inline Position<Valtype>& Position<Valtype>::Init() noexcept{
	*this = Origin3D<Valtype>;
	return *this;
}

template<typename Valtype> 
inline Position<Valtype>& Position<Valtype>::Transport( const Vector<Valtype>& v ) noexcept{
	x += v.dx;
	y += v.dy;
	z += v.dz;
	return *this;
}

template<typename Valtype> 
inline constexpr bool Position<Valtype>::Equals( const Position<Valtype>& p, Valtype epsilon_ ) const noexcept{
	const Vector<Valtype> d{ *this - p };
	return d*d <= epsilon_*epsilon_;
}

template<typename Valtype>
inline Position<Valtype>& Position<Valtype>::Round( int toDigit ) noexcept{
	x = common::Round(x,toDigit);
	y = common::Round(y,toDigit);
	z = common::Round(z,toDigit);
	return *this;
}

template<typename Valtype,typename Valtype2>
inline Position<Valtype>& spatial_cast( Position<Valtype2>& from ) noexcept{
	static_assert( sizeof(Valtype) == sizeof(Valtype2), "Can not spatial_cast with different memory layout" );
	return reinterpret_cast<Position<Valtype>&>(from);
}

template<typename Valtype,typename Valtype2>
inline const Position<Valtype>& spatial_cast( const Position<Valtype2>& from ) noexcept{
	static_assert( sizeof(Valtype) == sizeof(Valtype2), "Can not spatial_cast with different memory layout" );
	return reinterpret_cast<const Position<Valtype>&>(from);
}

template<typename Valtype> constexpr
inline Position<Valtype> operator+( const Position<Valtype>& p, const Vector<Valtype>& v ) noexcept{
	return { p.x + v.dx,
			 p.y + v.dy,
			 p.z + v.dz };
}

template<typename Valtype>
inline Position<Valtype>& operator+=( Position<Valtype>& p, const Vector<Valtype>& v ) noexcept{
	p.Transport( v );
	return p;
}

template<typename Valtype> constexpr
inline Vector<Valtype> operator-( const Position<Valtype>& p1, const Position<Valtype>& p2 ) noexcept{
	return { p1.x - p2.x,
			 p1.y - p2.y,
		     p1.z - p2.z };
}

template<typename Valtype> constexpr
inline Position<Valtype> operator-( const Position<Valtype>& p, const Vector<Valtype>& v ) noexcept{
	return { p.x - v.dx,
			 p.y - v.dy,
			 p.z - v.dz };
}

template<typename Valtype>
inline Position<Valtype>& operator-=( Position<Valtype>& p, const Vector<Valtype>& v ) noexcept{
	p.x -= v.dx;
	p.y -= v.dy;
	p.z -= v.dz;
	return p;
}

template<typename Valtype> constexpr
inline auto operator*( Valtype f, const Position<Valtype>& p ) noexcept -> Position<decltype(Valtype{}*Valtype{})>{
	return { p.x * f, 
			 p.y * f, 
			 p.z * f };	
}
template<typename Valtype> constexpr
inline auto operator*( const Position<Valtype>& p, Valtype f ) noexcept -> Position<decltype(Valtype{}*Valtype{})>{
	return f * p;
}

template<typename Valtype,typename Valtype2> constexpr
inline auto operator*( Valtype f, const Position<Valtype2>& p ) noexcept -> Position<decltype(Valtype{}*Valtype2{})>{
	static_assert( !std::is_integral<Valtype>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	return { p.x * f, 
			 p.y * f, 
			 p.z * f };	
}

template<typename Valtype,typename Valtype2> constexpr
inline auto operator*( const Position<Valtype>& p, Valtype2 f ) noexcept -> Position<decltype(Valtype{}*Valtype2{})>{
	static_assert( !std::is_integral<Valtype>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	return f * p;
}

template<typename Valtype> constexpr
inline Position<int> operator*( const Position<int>& p, Valtype f ) noexcept{
	return { static_cast<int>(static_cast<Valtype>(p.x) * f), 
			 static_cast<int>(static_cast<Valtype>(p.y) * f), 
			 static_cast<int>(static_cast<Valtype>(p.z) * f) };
}

template<typename Valtype> constexpr
inline Position<int> operator*( Valtype f, const Position<int>& p ) noexcept{
	return p * f;
}

template<typename Valtype,typename Valtype2>
inline Position<Valtype>& operator*=( Position<Valtype>& p, Valtype2 f ) noexcept{
	static_assert( !std::is_integral<Valtype>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	p.x *= f;
	p.y *= f;
	p.z *= f;
	return p;
}

template<typename Valtype>
inline Position<int>& operator*=( Position<int>& p, Valtype f ) noexcept{
	p.x = static_cast<int>(static_cast<Valtype>(p.x) * f);
	p.y = static_cast<int>(static_cast<Valtype>(p.y) * f);
	p.z = static_cast<int>(static_cast<Valtype>(p.z) * f);
	return p;
}

template<typename Valtype,typename Valtype2> constexpr
inline auto operator/( const Position<Valtype>& p, Valtype2 f ) noexcept -> Position<decltype(Valtype{}/Valtype2{})>{
	static_assert( !std::is_integral<Valtype>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	return { p.x / f,
			 p.y / f,
			 p.z / f };
}

template<typename Valtype> constexpr
inline Position<int> operator/( const Position<int>& p, Valtype f ) noexcept{
	return { static_cast<int>(static_cast<Valtype>(p.x) / f),
			 static_cast<int>(static_cast<Valtype>(p.y) / f),
			 static_cast<int>(static_cast<Valtype>(p.z) / f) };
}

template<typename Valtype,typename Valtype2>
inline Position<Valtype>& operator/=( Position<Valtype>& p, Valtype2 f ) noexcept{
	static_assert( !std::is_integral<Valtype>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	p.x /= f;
	p.y /= f;
	p.z /= f;
	return p;
}

template<typename Valtype> constexpr
inline Position<int>& operator/=( Position<int>& p, Valtype f ) noexcept{
	p.x = static_cast<int>(static_cast<Valtype>(p.x) / f);
	p.y = static_cast<int>(static_cast<Valtype>(p.y) / f);
	p.z = static_cast<int>(static_cast<Valtype>(p.z) / f);
	return p;
}

template<typename Valtype> constexpr
inline bool operator==( const Position<Valtype>& p1, const Position<Valtype>& p2 ) noexcept{
	return (p1.x == p2.x && p1.y == p2.y && p1.z == p2.z);
}

template<typename Valtype> constexpr
inline bool operator!=( const Position<Valtype>& p1, const Position<Valtype>& p2 ) noexcept{
	return !(p1 == p2);
}

} // namespace spat

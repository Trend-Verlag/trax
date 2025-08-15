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
//  "let me entertain you"
//
//			Robbie Williams

#pragma once

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <utility>


namespace spat{

	template<typename> struct Position2D;
	template<typename> struct Vector;

	/// \brief Implements a 2D - vector in cartesian coordinates.
	///
	/// Specifies the coordinates of a direction in 2D space. For points, like 
	/// e.g. vertices, use the Position2D type.
	template<typename Valtype>
	struct Vector2D
	{
		typedef Valtype value_type;

		Valtype dx; ///< cartesian x component.
		Valtype dy; ///< cartesian y component.


		/// \brief Identifiers for special vectors.
		enum class specials{
			null= -1,	///< Vector with all parameters zeroed
			ex	= 0,	///< Unit vector in x-direction
			ey	= 1		///< Unit vector in y-direction
		};


		/// \name Construction
		/// \param dx cartesian x component.
		/// \param dy cartesian y component.
		/// \param s
		/// \param p
		/// \param pVal
		///@{
				 constexpr Vector2D() noexcept = default;					///< Does not initialize the members.
				 constexpr Vector2D( Valtype dx, Valtype dy ) noexcept;
		explicit constexpr Vector2D( specials s ) noexcept;					
		explicit		   Vector2D( const Position2D<Valtype>& p ) noexcept = delete;		///< Conversions between Position2D and Vector2D shouldn't be necessary.
		explicit constexpr Vector2D( const Vector<Valtype>& v ) noexcept;
		template<typename Valtype2>
		explicit		   Vector2D( const Valtype2* pVal ) noexcept;
		///@}


		/// \name Assignment
		/// \param vector source vector
		///@{
		template<typename Valtype2>
		Vector2D& operator=( const Vector<Valtype2>& vector ) noexcept;
		///@}


		/// \name Subscripting.
		/// \returns Reference to value.
		///@{
		const Valtype& operator[]( size_t index ) const;

		Valtype& operator[]( size_t index );
		///@}


		/// \brief Initializes the Vector2D.
		/// \param s Type of Vector2D to initialize with.
		Vector2D& Init( specials s = specials::ex ) noexcept;


		/// \brief Test the Vector2D to mach a special Vector2D exactly.
		constexpr bool Is( specials s ) const noexcept;


		/// \brief Calculates the length of the Vector2D
		/// \return Length of Vector2D.
		constexpr Valtype Length() const noexcept;


		/// Normalizes the Vector2D to length 1.
		/// \return Previous length.
		auto Normalize() noexcept -> decltype(Valtype{}/Valtype{});
		

		///	\brief Rotate this vector with angle in radiants.
		template<typename Valtype2>
		Vector2D& Rotate( Valtype2 angle ) noexcept;


		/// \brief Part of this tVector wich is parallel to normal.
		///	\param normal Reference vector to meassure this against. Must not be 0.
		/// \return A vector parallel to normal.
		constexpr Vector2D ParallelProjection( const Vector2D& normal ) const noexcept;


		/// \brief Calculate part of this tVector wich is orthogonal to normal.
		///	\param normal Reference vector to meassure this against. Must not be 0.
		/// \return A vector orthogonal to normal.
		constexpr Vector2D OrthogonalProjection( const Vector2D& normal ) const noexcept;


		/// \brief Comparison within some range.
		///	The area within two vectors regarded as equal is a sphere with radius epsilon.
		/// \param v Vector2D to compare with.
		/// \param epsilon Distance <= wich two vectors are 
		/// regarded as equal.
		/// \return True if positions are nearby each other, else false.
		constexpr bool Equals( const Vector2D& v, Valtype epsilon = 0 ) const noexcept;
	};

	/// \name Special Vectors
	///@{
	template<typename Valtype>
	constexpr Vector2D<Valtype> Null2D{Valtype{0},Valtype{0}};

	template<typename Valtype>
	constexpr Vector2D<Valtype> Ex2D{Valtype{1},Valtype{0}};

	template<typename Valtype>
	constexpr Vector2D<Valtype> Ey2D{Valtype{0},Valtype{1}};
	///@}


	/// \brief Cast to Vector with different underlying value type.
	template<typename Valtype,typename Valtype2>
	Vector2D<Valtype>& spatial_cast( Vector2D<Valtype2>& from ) noexcept;

	/// \brief Cast to Vector with different underlying value type.
	template<typename Valtype,typename Valtype2>
	const Vector2D<Valtype>& spatial_cast( const Vector2D<Valtype2>& from ) noexcept;


	/// \name Operators for Vector2D
	///
	/// \brief Algebra for 2D Vectors.
	///@{
	template<typename Valtype> constexpr 
	Vector2D<Valtype> operator+( const Vector2D<Valtype>& v1, const Vector2D<Valtype>& v2 ) noexcept;
	template<typename Valtype> 
	Vector2D<Valtype>& operator+=( Vector2D<Valtype>& v1, const Vector2D<Valtype>& v2 ) noexcept;
	template<typename Valtype> constexpr 
	Vector2D<Valtype> operator+( const Vector2D<Valtype>& v ) noexcept;
	template<typename Valtype> constexpr 
	Vector2D<Valtype> operator-( const Vector2D<Valtype>& v1, const Vector2D<Valtype>& v2 ) noexcept;
	template<typename Valtype>
	Vector2D<Valtype>& operator-=( Vector2D<Valtype>& v1, const Vector2D<Valtype>& v2 ) noexcept;
	template<typename Valtype> constexpr 
	Vector2D<Valtype> operator-( const Vector2D<Valtype>& v ) noexcept;
	template<typename Valtype,typename Valtype2> constexpr 
	auto operator*( const Vector2D<Valtype>& v1, const Vector2D<Valtype2>& v2 ) noexcept -> decltype(Valtype{}*Valtype2{}); ///< Inner product.
	template<typename Valtype,typename Valtype2> constexpr 
	auto operator*( const Vector2D<Valtype>& v, Valtype2 f ) noexcept -> Vector2D<decltype(Valtype{}*Valtype2{})>;
	template<typename Valtype,typename Valtype2> constexpr 
	auto operator*( Valtype f, const Vector2D<Valtype2>& v ) noexcept -> Vector2D<decltype(Valtype{}*Valtype2{})>;
	template<typename Valtype,typename Valtype2> 
	Vector2D<Valtype>& operator*=( Vector2D<Valtype>& v, const Valtype2& f ) noexcept;
	template<typename Valtype>
	Vector2D<Valtype>& operator*=( Vector2D<Valtype>& v1, const Vector2D<Valtype>& v2 ) noexcept = delete;
	template<typename Valtype> constexpr 
	Valtype operator%( const Vector2D<Valtype>& v1, const Vector2D<Valtype>& v2 ) noexcept; ///< Cross product.
	template<typename Valtype>
	Vector2D<Valtype>& operator%=( Vector2D<Valtype>& v1, const Vector2D<Valtype>& v2 ) = delete;
	template<typename Valtype,typename Valtype2> constexpr 
	auto operator/( const Vector2D<Valtype>&, const Vector2D<Valtype2>& ) = delete;
	template<typename Valtype,typename Valtype2> constexpr 
	auto operator/( const Vector2D<Valtype>& v, Valtype2 f ) noexcept -> Vector2D<decltype(Valtype{}/Valtype2{})>;
	template<typename Valtype,typename Valtype2> constexpr 
	auto operator/( const Valtype& f, const Vector2D<Valtype2>& v ) = delete;
	template<typename Valtype,typename Valtype2>
	Vector2D<Valtype>& operator/=( Vector2D<Valtype>& v, Valtype2 f ) noexcept;
	template<typename Valtype> constexpr 
	bool operator==( const Vector2D<Valtype>& v1, const Vector2D<Valtype>& v2 ) noexcept;
	template<typename Valtype> constexpr 
	bool operator!=( const Vector2D<Valtype>& v1, const Vector2D<Valtype>& v2 ) noexcept;
	///@}

	/// \brief Outer normalizing
	/// \returns The length of the Vector as a dimensionated value and a Vector of unit length with dinmensions One.
	template<typename Valtype>
	auto Normalize( const Vector2D<Valtype>& v ) noexcept -> std::pair<Valtype,Vector2D<decltype(Valtype{}/Valtype{})>>;

	template<typename Valtype>
	constexpr size_t size( const Vector2D<Valtype> ) noexcept {
		return 2;
	}

template<typename Valtype> constexpr
inline Vector2D<Valtype>::Vector2D( Valtype dx, Valtype dy ) noexcept
	:	dx{dx}, dy{dy} 
{}

template<typename Valtype> constexpr
inline Vector2D<Valtype>::Vector2D( specials s ) noexcept
	:	dx{s == specials::ex ? Valtype{1} : Valtype{0}}, 
		dy{s == specials::ey ? Valtype{1} : Valtype{0}} 
{}

template<typename Valtype> constexpr
inline Vector2D<Valtype>::Vector2D( const Vector<Valtype>& v ) noexcept
	: dx{v.dx}, dy{v.dy} 
{}

template<typename Valtype>
template<typename Valtype2>
inline Vector2D<Valtype>::Vector2D( const Valtype2* pVal ) noexcept
	:	dx{ Valtype{pVal[0]} }, 
		dy{ Valtype{pVal[1]} }
{}

template<typename Valtype>
template<typename Valtype2>
inline Vector2D<Valtype>& Vector2D<Valtype>::operator=( const Vector<Valtype2>& v ) noexcept{
	dx = Valtype{v.dx};
	dy = Valtype{v.dy};
	return *this;
}

template<typename Valtype>
inline const Valtype& Vector2D<Valtype>::operator[]( size_t index ) const{
	if( index < 2 )
		return (&dx)[index];

	throw std::out_of_range( "invalid Vector2D subscript" );
}

template<typename Valtype>
inline Valtype& Vector2D<Valtype>::operator[]( size_t index ){
	if( index < 2 )
		return (&dx)[index];

	throw std::out_of_range( "invalid Vector2D subscript" );
}

template<typename Valtype>
Vector2D<Valtype>& Vector2D<Valtype>::Init( specials s ) noexcept{
	dx = s == specials::ex ? Valtype{1} : Valtype{0};
	dy = s == specials::ey ? Valtype{1} : Valtype{0};
	return *this;
}

template<typename Valtype>
constexpr bool Vector2D<Valtype>::Is( specials s ) const noexcept{
	switch(s){
	case specials::null:
		return *this == Null2D<Valtype>;
	case specials::ex:
		return *this == Ex2D<Valtype>;
	case specials::ey:
		return *this == Ey2D<Valtype>;
	default:
		assert( false );
	}

	return false;
}

template<typename Valtype> constexpr
inline Valtype Vector2D<Valtype>::Length() const noexcept{	
	return sqrt( dx*dx + dy*dy );
}

template<typename Valtype>
inline auto Vector2D<Valtype>::Normalize() noexcept -> decltype(Valtype{}/Valtype{}){	
	decltype(Valtype{}/Valtype{}) L = Length() / Valtype{1};
	if( !L ){
		Init();
		return decltype(Valtype{}/Valtype{}){0};
	}
	*this /= L;
	return L;
}

template<typename Valtype>
template<typename Valtype2>
Vector2D<Valtype>& Vector2D<Valtype>::Rotate( Valtype2 angle ) noexcept{
	*this = cos( angle ) * *this + sin( angle ) * Vector2D<Valtype>{ -dy, dx };
	return *this;
}

template<typename Valtype> constexpr
inline Vector2D<Valtype> Vector2D<Valtype>::ParallelProjection( const Vector2D& normal ) const noexcept{
	Valtype nl2 = normal * normal;
	if( nl2 == 0 )
		return Null2D<Valtype>;

	return (*this * normal) / nl2 * normal;
}

template<typename Valtype> constexpr
inline Vector2D<Valtype> Vector2D<Valtype>::OrthogonalProjection( const Vector2D& normal ) const noexcept{
	return *this - ParallelProjection( normal );
}

template<typename Valtype>
constexpr bool Vector2D<Valtype>::Equals( const Vector2D<Valtype>& v, Valtype epsilon_ ) const noexcept{
	const Vector2D<Valtype> d{*this - v};
	return d*d <= epsilon_*epsilon_;
}

template<typename Valtype,typename Valtype2>
inline Vector2D<Valtype>& spatial_cast( Vector2D<Valtype2>& from ) noexcept{
	static_assert( sizeof(Valtype) == sizeof(Valtype2), "Can not spatial_cast with different memory layout" );
	return reinterpret_cast<Vector2D<Valtype>&>(from);
}

template<typename Valtype,typename Valtype2>
inline const Vector2D<Valtype>& spatial_cast( const Vector2D<Valtype2>& from ) noexcept{
	static_assert( sizeof(Valtype) == sizeof(Valtype2), "Can not spatial_cast with different memory layout" );
	return reinterpret_cast<const Vector2D<Valtype>&>(from);
}

template<typename Valtype> constexpr 
inline Vector2D<Valtype> operator+( const Vector2D<Valtype>& v1, const Vector2D<Valtype>& v2 ) noexcept{
	return { v1.dx + v2.dx,
			 v1.dy + v2.dy };
}

template<typename Valtype>
inline Vector2D<Valtype>& operator+=( Vector2D<Valtype>& v1, const Vector2D<Valtype>& v2 ) noexcept{
	v1.dx += v2.dx;
	v1.dy += v2.dy;
	return v1;
}

template<typename Valtype> constexpr 
inline Vector2D<Valtype> operator+( const Vector2D<Valtype>& v ) noexcept{
	return { +v.dx, +v.dy };
}

template<typename Valtype> constexpr 
inline Vector2D<Valtype> operator-( const Vector2D<Valtype>& v1, const Vector2D<Valtype>& v2 ) noexcept{
	return { v1.dx - v2.dx,
			 v1.dy - v2.dy };
}

template<typename Valtype>
inline Vector2D<Valtype>& operator-=( Vector2D<Valtype>& v1, const Vector2D<Valtype>& v2 ) noexcept{
	v1.dx -= v2.dx;
	v1.dy -= v2.dy;
	return v1;
}

template<typename Valtype> constexpr 
inline Vector2D<Valtype> operator-( const Vector2D<Valtype>& v ) noexcept{
	return { -v.dx, -v.dy };
}

template<typename Valtype,typename Valtype2> constexpr 
inline auto operator*( const Vector2D<Valtype>& v1, const Vector2D<Valtype2>& v2 ) noexcept -> decltype(Valtype{}*Valtype2{}){
	return v1.dx*v2.dx + v1.dy*v2.dy;
}

template<typename Valtype,typename Valtype2> constexpr 
inline auto operator*( const Vector2D<Valtype>& v, Valtype2 f ) noexcept -> Vector2D<decltype(Valtype{}*Valtype2{})>{
	static_assert( !std::is_integral<Valtype>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	return { v.dx * f,
			 v.dy * f };
}

template<typename Valtype> constexpr 
inline Vector2D<int> operator*( const Vector2D<int>& v, Valtype f ) noexcept {
	return { static_cast<int>(static_cast<Valtype>(v.dx) * f),
			 static_cast<int>(static_cast<Valtype>(v.dy) * f) };
}

template<typename Valtype,typename Valtype2> constexpr 
inline auto operator*( Valtype f, const Vector2D<Valtype2>& v ) noexcept -> Vector2D<decltype(Valtype{}*Valtype2{})>{
	static_assert( !std::is_integral<Valtype2>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	return v * f;
}

template<typename Valtype> constexpr 
inline Vector2D<int> operator*( Valtype f, const Vector2D<int>& v ) noexcept{
	return v * f;
}

template<typename Valtype,typename Valtype2>
inline Vector2D<Valtype>& operator*=( Vector2D<Valtype>& v, const Valtype2& f ) noexcept{
	static_assert( !std::is_integral<Valtype>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	v.dx *= f;
	v.dy *= f;
	return v;
}

template<typename Valtype>
inline Vector2D<int>& operator*=( Vector2D<int>& v, Valtype f ) noexcept{
	v.dx = static_cast<int>(static_cast<Valtype>(v.dx) * f);
	v.dy = static_cast<int>(static_cast<Valtype>(v.dy) * f);
	return v;
}

template<typename Valtype> constexpr 
inline Valtype operator%( const Vector2D<Valtype>& v1, const Vector2D<Valtype>& v2 ) noexcept{
	return v1.dx * v2.dy - v1.dy * v2.dx;
}

template<typename Valtype,typename Valtype2> constexpr 
inline auto operator/( const Vector2D<Valtype>& v, Valtype2 f ) noexcept -> Vector2D<decltype(Valtype{}/Valtype2{})>{
	static_assert( !std::is_integral<Valtype>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	return { v.dx / f,
			 v.dy / f };
}

template<typename Valtype> constexpr 
inline Vector2D<int> operator/( const Vector2D<int>& v, Valtype f ) noexcept{
	return { static_cast<int>(static_cast<Valtype>(v.dx) / f),
			 static_cast<int>(static_cast<Valtype>(v.dy) / f) };
}

template<typename Valtype,typename Valtype2>
inline Vector2D<Valtype>& operator/=( Vector2D<Valtype>& v, Valtype2 f ) noexcept{
	static_assert( !std::is_integral<Valtype>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	v.dx /= f;
	v.dy /= f;
	return v;
}

template<typename Valtype>
inline Vector2D<int>& operator/=( Vector2D<int>& v, Valtype f ) noexcept{
	v.dx = static_cast<int>(static_cast<Valtype>(v.dx) / f);
	v.dy = static_cast<int>(static_cast<Valtype>(v.dy) / f);
	return v;
}

template<typename Valtype> constexpr
inline bool operator==( const Vector2D<Valtype>& v1, const Vector2D<Valtype>& v2 ) noexcept{
	return v1.dx == v2.dx && v1.dy == v2.dy;
}

template<typename Valtype> constexpr
inline bool operator!=( const Vector2D<Valtype>& v1, const Vector2D<Valtype>& v2 ) noexcept{
	return !(v1 == v2);
}

template<typename Valtype>
auto Normalize( const Vector2D<Valtype>& v ) noexcept -> std::pair<Valtype,Vector2D<decltype(Valtype{}/Valtype{})>> {
	Vector2D<decltype(Valtype{}/Valtype{})> V{	v.dx / Valtype{1},
												v.dy / Valtype{1} };
	return std::make_pair( V.Normalize() * Valtype{1}, V );
}

} // namespace spat

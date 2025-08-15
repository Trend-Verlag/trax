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
//  "l'etat c'est moi"
//
//	 Louis Quatorze

#pragma once

#include <stdexcept>
#include <type_traits>


namespace spat{

	template<typename> struct Vector2D;
	template<typename> struct Position;


	/// \brief Implements a 2D - position in cartesian coordinates.
	///
	/// Specifies the coordinates of a point in 2D space. For Vectors, like 
	/// e.g. distances, use the Vector type.
	template<typename Valtype>
	struct Position2D
	{
		typedef Valtype value_type;

		Valtype x;	///< cartesian x coordinate
		Valtype y;	///< cartesian y coordinate

						
		/// \name Construction
		/// \param x cartesian x coordinate.
		/// \param y cartesian y coordinate.
		/// \param pos
		/// \param v
		/// \param pVal
		///@{
				 constexpr Position2D() noexcept = default;						///< Does not initialize the members.	
				 constexpr Position2D( Valtype x, Valtype y ) noexcept;
		explicit		   Position2D( const Vector2D<Valtype>& ) = delete;	///< Conversions between Vector and Position2D shouldn't be necessary.
		explicit constexpr Position2D( const Position<Valtype>& pos ) noexcept;
		template<typename Valtype2>
		explicit		   Position2D( const Valtype2* pVal ) noexcept;
		///@}

		/// \name Assignment
		///@{
		Position2D& operator=( const Position<Valtype>& p ) noexcept;
		///@}


		/// \name Subscripting.
		/// \returns Reference to value.
		///@{
		const Valtype& operator[]( size_t index ) const;

		Valtype& operator[]( size_t index );
		///@}


		/// \brief Initialisation to origin.
		Position2D& Init() noexcept;


		/// \brief Moves the Position2D by v.
		Position2D& Transport	( const Vector2D<Valtype>& v ) noexcept;


		/// \brief Comparison within some range.
		///	The area within two Position2Ds regarded as equal is a sphere with radius epsilon.
		/// \param p Position2D to compare with.
		/// \param epsilon Distance <= wich two Position2Ds are 
		/// regarded as equal.
		constexpr bool Equals( const Position2D& p, Valtype epsilon = 0 ) const noexcept;
	};


	/// \brief Origin of coordinate system.
	template<typename Valtype>
	constexpr Position2D<Valtype> Origin2D{Valtype{0},Valtype{0}};


	/// \name Operators for Position2D
	///
	/// \brief Algebra for Positions.
	/// 
	/// Most surprises: you ran over a deleted operator? You should try Vector instead. If you really 
	/// think it would make any sense to add the position of Rome and the position of Moscow to come 
	/// out with Peking, there most probably is something wrong with your reasoning.
	///
	/// To transform a Vector2D into a Position use Origin2D + Vector2D.
	/// To transform a Position2D into a Vector use Position2D - Origin2D.
	///@{
	template<typename Valtype> constexpr
	Position2D<Valtype> operator+( const Position2D<Valtype>&, const Position2D<Valtype>& ) = delete;
	template<typename Valtype> constexpr
	Position2D<Valtype> operator+( const Position2D<Valtype>& p, const Vector2D<Valtype>& v ) noexcept;
	template<typename Valtype> constexpr
	Position2D<Valtype> operator+( const Vector2D<Valtype>&, const Position2D<Valtype>& ) = delete;
	template<typename Valtype>
	Position2D<Valtype>& operator+=( Position2D<Valtype>&, const Position2D<Valtype>& ) = delete;
	template<typename Valtype>
	Position2D<Valtype>& operator+=( Position2D<Valtype>& p, const Vector2D<Valtype>& v ) noexcept;
	template<typename Valtype>
	Vector2D<Valtype>& operator+=( Vector2D<Valtype>&, const Position2D<Valtype>& ) = delete;
	template<typename Valtype> constexpr
	Position2D<Valtype> operator+( const Position2D<Valtype>& ) = delete;
	template<typename Valtype> constexpr
	Vector2D<Valtype> operator-( const Position2D<Valtype>& p1, const Position2D<Valtype>& p2 ) noexcept;
	template<typename Valtype> constexpr
	Position2D<Valtype> operator-( const Position2D<Valtype>& p, const Vector2D<Valtype>& v ) noexcept;
	template<typename Valtype> constexpr
	Position2D<Valtype> operator-( const Vector2D<Valtype>&, const Position2D<Valtype>& ) = delete;
	template<typename Valtype>
	Position2D<Valtype>& operator-=( Position2D<Valtype>&, const Position2D<Valtype>& ) = delete;
	template<typename Valtype>
	Position2D<Valtype>& operator-=( Position2D<Valtype>& p, const Vector2D<Valtype>& v ) noexcept;
	template<typename Valtype> constexpr
	Position2D<Valtype> operator-( const Position2D<Valtype>& ) = delete;
	template<typename Valtype> constexpr
	Position2D<Valtype> operator*( const Position2D<Valtype>& , const Position2D<Valtype>& ) = delete;
	template<typename Valtype,typename Valtype2> constexpr
	auto operator*( const Position2D<Valtype>& p, Valtype2 f ) noexcept -> Position2D<decltype(Valtype{}*Valtype2{})>;
	template<typename Valtype,typename Valtype2> constexpr
	auto operator*( Valtype f, const Position2D<Valtype2>& p ) noexcept -> Position2D<decltype(Valtype{}*Valtype2{})>;
	template<typename Valtype,typename Valtype2> 
	Position2D<Valtype>& operator*=( Position2D<Valtype>& p, const Valtype2& f ) noexcept;
	template<typename Valtype,typename Valtype2> 
	Valtype2& operator*=( Valtype2&, const Position2D<Valtype>& ) = delete;
	template<typename Valtype> constexpr
	Position2D<Valtype> operator%( const Position2D<Valtype>& , const Position2D<Valtype>& ) = delete;
	template<typename Valtype>
	Position2D<Valtype>& operator%=( const Position2D<Valtype>& , const Position2D<Valtype>& ) = delete;
	template<typename Valtype> constexpr
	Valtype operator/( const Position2D<Valtype>& , const Position2D<Valtype>& ) = delete;
	template<typename Valtype,typename Valtype2> constexpr
	auto operator/( const Position2D<Valtype>& p, Valtype2 f ) noexcept -> Position2D<decltype(Valtype{}/Valtype2{})>;
	template<typename Valtype> constexpr
	Position2D<Valtype> operator/( Valtype f, const Position2D<Valtype>& p ) = delete;
	template<typename Valtype,typename Valtype2>
	Position2D<Valtype>& operator/=( Position2D<Valtype>& p, const Valtype2& f ) noexcept;
	template<typename Valtype> constexpr
	bool operator==( const Position2D<Valtype>& p1, const Position2D<Valtype>& p2 ) noexcept;
	template<typename Valtype> constexpr
	bool operator!=( const Position2D<Valtype>& p1, const Position2D<Valtype>& p2 ) noexcept;
	///@}

	template<typename Valtype>
	constexpr size_t size( const Position2D<Valtype> ) noexcept {
		return 2;
	}

template<typename Valtype>
inline constexpr Position2D<Valtype>::Position2D( Valtype x, Valtype y ) noexcept
	: x{x}, y{y} 
{}

template<typename Valtype>
inline constexpr Position2D<Valtype>::Position2D( const Position<Valtype>& pos ) noexcept
	: x{pos.x}, y{pos.y} 
{}

template<typename Valtype>
template<typename Valtype2>
inline Position2D<Valtype>::Position2D( const Valtype2* pVal ) noexcept
	:	x{Valtype{pVal[0]}}, 
		y{Valtype{pVal[1]}} 
{}

template<typename Valtype>
inline Position2D<Valtype>& Position2D<Valtype>::operator=( const Position<Valtype>& p ) noexcept{
	x = p.x;
	y = p.y;
	return *this;
}

template<typename Valtype>
inline const Valtype& Position2D<Valtype>::operator[]( size_t index ) const{
	if( index < 2 )
		return (&x)[index];

	throw std::out_of_range( "invalid Position2D subscript" );
}

template<typename Valtype>
inline Valtype& Position2D<Valtype>::operator[]( size_t index ){
	if( index < 2 )
		return (&x)[index];

	throw std::out_of_range( "invalid Position2D subscript" );
}

template<typename Valtype>
inline Position2D<Valtype>& Position2D<Valtype>::Init() noexcept{
	*this = Origin2D<Valtype>;
	return *this;
}

template<typename Valtype> 
inline Position2D<Valtype>& Position2D<Valtype>::Transport( const Vector2D<Valtype>& v ) noexcept{
	x += v.dx;
	y += v.dy;
	return *this;
}

template<typename Valtype> 
inline constexpr bool Position2D<Valtype>::Equals( const Position2D<Valtype>& p, Valtype epsilon_ ) const noexcept{
	Vector2D<Valtype> d{ *this - p };
	return d*d <= epsilon_*epsilon_;
}

template<typename Valtype> constexpr
inline Position2D<Valtype> operator+( const Position2D<Valtype>& p, const Vector2D<Valtype>& v ) noexcept{
	return { p.x + v.dx,
			 p.y + v.dy };
}

template<typename Valtype>
inline Position2D<Valtype>& operator+=( Position2D<Valtype>& p, const Vector2D<Valtype>& v ) noexcept{
	p.Transport( v );
	return p;
}

template<typename Valtype> constexpr
inline Vector2D<Valtype> operator-( const Position2D<Valtype>& p1, const Position2D<Valtype>& p2 ) noexcept{
	return { p1.x - p2.x,
			 p1.y - p2.y };
}

template<typename Valtype> constexpr
inline Position2D<Valtype> operator-( const Position2D<Valtype>& p, const Vector2D<Valtype>& v ) noexcept{
	return { p.x - v.dx,
			 p.y - v.dy };
}

template<typename Valtype>
inline Position2D<Valtype>& operator-=( Position2D<Valtype>& p, const Vector2D<Valtype>& v ) noexcept{
	p.x -= v.x;
	p.y -= v.y;
	return p;
}

template<typename Valtype,typename Valtype2> constexpr
inline auto operator*( const Position2D<Valtype>& p, Valtype2 f ) noexcept -> Position2D<decltype(Valtype{}*Valtype2{})>{
	static_assert( !std::is_integral<Valtype>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	return { p.x * f, 
			 p.y * f };
}

template<typename Valtype> constexpr
inline Position2D<int> operator*( const Position2D<int>& p, Valtype f ) noexcept{
	return { static_cast<int>(static_cast<Valtype>(p.x) * f), 
			 static_cast<int>(static_cast<Valtype>(p.y) * f) };
}

template<typename Valtype,typename Valtype2> constexpr
inline auto operator*( Valtype f, const Position2D<Valtype2>& p ) noexcept -> Position2D<decltype(Valtype{}*Valtype2{})>{
	static_assert( !std::is_integral<Valtype>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	return p * f;
}

template<typename Valtype> constexpr
inline Position2D<int> operator*( Valtype f, const Position2D<int>& p ) noexcept{
	return p * f;
}

template<typename Valtype,typename Valtype2>
inline Position2D<Valtype>& operator*=( Position2D<Valtype>& p, const Valtype2& f ) noexcept{
	p.x *= f;
	p.y *= f;
	return p;
}

template<typename Valtype>
inline Position2D<int>& operator*=( Position2D<int>& p, Valtype f ) noexcept{
	p.x = static_cast<int>(static_cast<Valtype>(p.x) * f);
	p.y = static_cast<int>(static_cast<Valtype>(p.y) * f);
	return p;
}

template<typename Valtype,typename Valtype2> constexpr
inline auto operator/( const Position2D<Valtype>& p, Valtype2 f ) noexcept -> Position2D<decltype(Valtype{}/Valtype2{})>{
	static_assert( !std::is_integral<Valtype>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	return { p.x / f,
			 p.y / f };
}

template<typename Valtype> constexpr
inline Position2D<int> operator/( const Position2D<int>& p, Valtype f ) noexcept{
	return { static_cast<int>(static_cast<Valtype>(p.x) / f),
			 static_cast<int>(static_cast<Valtype>(p.y) / f) };
}

template<typename Valtype,typename Valtype2> constexpr
inline Position2D<Valtype>& operator/=( Position2D<Valtype>& p, Valtype2 f ) noexcept{
	static_assert( !std::is_integral<Valtype>::value, "Possible data loss on integral values, proper implementation provided for int only!" );
	p.x /= f;
	p.y /= f;
	return p;
}

template<typename Valtype>
inline Position2D<int>& operator/=( Position2D<int>& p, Valtype f ) noexcept{
	p.x = static_cast<int>(static_cast<Valtype>(p.x) / f);
	p.y = static_cast<int>(static_cast<Valtype>(p.y) / f);
	return p;
}

template<typename Valtype> constexpr
inline bool operator==( const Position2D<Valtype>& p1, const Position2D<Valtype>& p2 ) noexcept{
	return (p1.x == p2.x && p1.y == p2.y );
}

template<typename Valtype> constexpr
inline bool operator!=( const Position2D<Valtype>& p1, const Position2D<Valtype>& p2 ) noexcept{
	return !(p1 == p2);
}

} // namespace spat

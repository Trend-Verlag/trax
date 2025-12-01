//	trax track library
//	AD 2013 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once


namespace spat{

	template<typename> struct Position;
	template<typename> struct Vector;

	/// \brief Implements a 4D - position in homogenous coordinates.
	///
	/// Specifies the coordinates of a point in 3D space. For Vectors, like 
	/// e.g. surface normals, use the Vector type.
	template<typename Valtype>
	struct PositionH
	{
		typedef Valtype value_type;

		Valtype x;	///< x coordinate
		Valtype y;	///< y coordinate
		Valtype z;	///< z coordinate
		Valtype w;	///< w coordinate, homogenous coordinate

						
		/// \name Construction
		/// \param x
		/// \param y
		/// \param z
		/// \param w
		/// \param p
		/// \param v
		///@{
					PositionH() = default;							///< Does not initialize the members.	
					PositionH( Valtype x, Valtype y, Valtype z, Valtype w ) noexcept;
		template<typename Valtype2>
		explicit constexpr PositionH( const Position<Valtype2>& p ) noexcept;
		template<typename Valtype2>
		explicit	PositionH( const Vector<Valtype2>& v ) noexcept;
		template<typename Valtype2>
		explicit	PositionH( const Valtype2* pVal ) noexcept;
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

		/// \brief	Sets the values to (0,0,0,1)
		void Init() noexcept;


		/// \brief Transforms the position so that w == 1
		bool Homogenize() noexcept;


		/// \brief Comparison within some range.
		///	The area within two positions regarded as equal is a sphere with radius epsilon.
		/// \param p PositionH to compare with.
		/// \param epsilon Distance <= wich two positions are 
		/// regarded as equal.
		bool Equals( const PositionH& p, Valtype epsilon = Valtype{0} ) const noexcept;
	};

	/// \name Comparison
	///@{
	template<typename Valtype> constexpr
	bool operator==( const PositionH<Valtype>& p1, const PositionH<Valtype>& p2 ) noexcept;
	template<typename Valtype> constexpr
	bool operator!=( const PositionH<Valtype>& p1, const PositionH<Valtype>& p2 ) noexcept;
	///@}


	template<typename Valtype>
	constexpr size_t size( const PositionH<Valtype> ) noexcept {
		return 4;
	}
// inlines:
template<typename Valtype>
inline PositionH<Valtype>::PositionH( Valtype x, Valtype y, Valtype z, Valtype w ) noexcept
	: x{x}, y{y}, z{z}, w{w}
{}

template<typename Valtype>
template<typename Valtype2>
inline constexpr PositionH<Valtype>::PositionH( const Position<Valtype2>& p ) noexcept
	:	x{ p.x/Valtype2{1} },
		y{ p.y/Valtype2{1} },
		z{ p.z/Valtype2{1} },
		w{ Valtype{1} }
{}

template<typename Valtype>
template<typename Valtype2>
inline PositionH<Valtype>::PositionH( const Vector<Valtype2>& v ) noexcept
	:	x{ v.dx/Valtype2{1} }, 
		y{ v.dy/Valtype2{1} }, 
		z{ v.dz/Valtype2{1} }, 
		w{ Valtype{0} }
{}

template<typename Valtype>
template<typename Valtype2>
inline PositionH<Valtype>::PositionH( const Valtype2* pVal ) noexcept
	:	x{ Valtype{ pVal[0] } },
		y{ Valtype{ pVal[1] } },
		z{ Valtype{ pVal[2] } },
		w{ Valtype{ pVal[3] } }
{}

template<typename Valtype>
inline const Valtype& PositionH<Valtype>::operator[]( size_t index ) const{
	switch( index ){
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	case 3:
		return w;
	default:
		assert(0);
		throw std::out_of_range( "invalid PositionH subscript" );
	}
}

template<typename Valtype>
inline Valtype& PositionH<Valtype>::operator[]( size_t index ){
	switch( index ){
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	case 3:
		return w;
	default:
		assert(0);
		throw std::out_of_range( "invalid PositionH subscript" );
	}
}

template<typename Valtype>
inline Valtype* PositionH<Valtype>::ptr() noexcept{	
	return &x; 
}

template<typename Valtype>
inline const Valtype* PositionH<Valtype>::ptr() const noexcept{	
	return &x; 
}

template<typename Valtype>
inline void PositionH<Valtype>::Init() noexcept{
	x = Valtype{0}; y = Valtype{0}; z = Valtype{0}; w = Valtype{1};
}

template<typename Valtype>
inline bool PositionH<Valtype>::Homogenize() noexcept{
	if( w != Valtype{0} ){
		x /= w;
		y /= w;
		z /= w;
		w = Valtype{1};
		return true;
	}
	return false;
}

template<typename Valtype>
inline bool PositionH<Valtype>::Equals( const PositionH<Valtype>& p, Valtype epsilon_ ) const noexcept{
	return	std::abs(x/w - p.x/p.w) <= epsilon_ &&
			std::abs(y/w - p.y/p.w) <= epsilon_ &&
			std::abs(z/w - p.z/p.w) <= epsilon_;
}

template<typename Valtype>
inline constexpr bool operator==( const PositionH<Valtype>& p1, const PositionH<Valtype>& p2 ) noexcept{
	return	p1.x/p1.w == p2.x/p2.w &&
			p1.y/p1.w == p2.y/p2.w &&
			p1.z/p1.w == p2.z/p2.w;
}

template<typename Valtype>
inline constexpr bool operator!=( const PositionH<Valtype>& p1, const PositionH<Valtype>& p2 ) noexcept{
	return !(p1 == p2);
}

} // namespace spat

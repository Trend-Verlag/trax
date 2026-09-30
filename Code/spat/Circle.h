//	trax track library
//	AD 2026 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//
// Copyright (c) 2026 Trend Redaktions- und Verlagsgesellschaft mbH
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
// For further information, please contact: horstmann@traxlibrary.dev

#pragma once

#include "Position2D.h"

namespace spat{

	template<typename> struct Rect;
	template<typename,typename> struct VectorBundle2D;
	
	/// \brief Circle with center and radius.
	template<typename Valtype>
	struct Circle
	{
		typedef Valtype value_type;

		Position2D<Valtype>	c;	///< Center of the circle
		Valtype				r;	///< Radius of the circle


		/// \name Construction
		/// \param center
		/// \param radius
		///@{		
		Circle() noexcept = default;			///< Does not initialize the members.
		Circle( const Position2D<Valtype>& center, Valtype radius ) noexcept;
		///@}


		/// \brief Initializes the circle to unit circle around origin.
		void Init() noexcept;


		///	\returns The geometric center of the circle.
		Position2D<Valtype> Center() const noexcept;


		/// \returns The radius of the circle.
		Valtype Radius() const noexcept;


		/// \returns True if the point lays inside the circle. 
		///
		/// A point on the surface is not included in the circle
		bool Includes( Valtype x, Valtype y ) const noexcept;


		/// \returns True if the point lays inside the circle. 
		///
		/// A point on the surface is not included in the circle
		bool Includes( const Position2D<Valtype>& pt ) const noexcept;


		/// \brief Gets the circumscribed rectangle of the circle
		Rect<Valtype> ExRect() const noexcept;


		/// \brief Gets the inscribed rectangle of the circle
		Rect<Valtype> InRect() const noexcept;
	};

	/// \name Mathematical Operators for Circle
	///@{
	template<typename Valtype>
	bool	operator==( const Circle<Valtype> a, const Circle<Valtype> b ) noexcept;
	template<typename Valtype>
	bool	operator!=( const Circle<Valtype> a, const Circle<Valtype> b ) noexcept;
	///@}

	/// \brief Do the two circles intersect?
	///@{
	template<typename Valtype>
	bool Intersecting( const Circle<Valtype>& circleA, const Circle<Valtype>& circleB ) noexcept;

	template<typename Valtype,typename ValtypeT>
	bool Intersecting( const Circle<Valtype>& circle, const VectorBundle2D<Valtype,ValtypeT>& ray ) noexcept;
	///@}

///////////////////////////////////////
template<typename Valtype> inline 
Circle<Valtype>::Circle( const Position2D<Valtype>& center, Valtype radius ) noexcept
	:	c{center},
		r{radius}
{}

template<typename Valtype> inline 
void Circle<Valtype>::Init() noexcept{
	c.Init();
	r = static_cast<Valtype>(1);
}

template<typename Valtype> inline 
Position2D<Valtype> Circle<Valtype>::Center() const noexcept{
	return c;
}

template<typename Valtype> inline 
Valtype Circle<Valtype>::Radius() const noexcept{
	return r;
}

template<typename Valtype> inline 
bool Circle<Valtype>::Includes( Valtype x, Valtype y ) const noexcept{
	return Includes( { x, y } );
}

template<typename Valtype> inline 
bool Circle<Valtype>::Includes( const Position2D<Valtype>& pt ) const noexcept{
	return (pt - c).Length() < r;
}

template<typename Valtype> inline 
Rect<Valtype> Circle<Valtype>::ExRect() const noexcept{
	return Rect<Valtype>( 
		c.x - r, c.y - r,
		c.x + r, c.y + r );
}

template<typename Valtype> inline 
Rect<Valtype> Circle<Valtype>::InRect() const noexcept{
	Valtype a = r / std::sqrt(2);
	return Rect<Valtype>( 
		c.x - a, c.y - a,
		c.x + a, c.y + a );
}

template<typename Valtype> inline 
bool operator==( const Circle<Valtype> a, const Circle<Valtype> b ) noexcept{
	return a.Center() == b.Center() && a.Radius() == b.Radius();
}

template<typename Valtype> inline 
bool operator!=( const Circle<Valtype> a, const Circle<Valtype> b ) noexcept{
	return !(a == b);
}

template<typename Valtype>
bool Intersecting( const Circle<Valtype>& circleA, const Circle<Valtype>& circleB ) noexcept{
	return (circleA.Center() - circleB.Center()).Length() < circleA.Radius() + circleB.Radius();
}

template<typename Valtype,typename ValtypeT>
bool Intersecting( const Circle<Valtype>& circle, const VectorBundle2D<Valtype,ValtypeT>& ray ) noexcept{
	const Vector<Valtype> D = circle.Center() - ray.P;
	return (D - (D*ray.T) * ray.T).Length() < circle.Radius();
}

} // namespace spat
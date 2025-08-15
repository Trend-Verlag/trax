// Spatial Coordinates And Vectors Library (spat)
// 
// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2014 
//
//  "Dream On"
//
//			Aerosmith

#pragma once

#include "common/Interval.h"


namespace spat{

	template<typename> struct Position2D;
	template<typename> struct Vector2D;


	/// \brief Axis aligned rectangle.
	template<typename Valtype> 
	struct Rect
	{
		typedef Valtype value_type;

		common::Interval<Valtype>	m_Width;	///< X dimension
		common::Interval<Valtype>	m_Height;	///< Y dimension

		/// \name Construction
		/// \param lefttop
		/// \param rightbottom
		/// \param left
		/// \param top
		/// \param right
		/// \param bottom
		///@{		
					Rect() noexcept = default;			///< Does not initialize the members.
					Rect( Valtype width, Valtype height ) noexcept;
					Rect( const common::Interval<Valtype>& width, const common::Interval<Valtype>& height ) noexcept;
		explicit	Rect( const Vector2D<Valtype>& diagonal ) noexcept;
					Rect( const Position2D<Valtype>& lefttop, const Position2D<Valtype>& rightbottom ) noexcept;
					Rect( Valtype left, Valtype top, Valtype right, Valtype bottom ) noexcept;
		///@}


		/// \name Initialisation
		/// \brief Initializes the interval to origin.
		/// \param left Left edge of the Rect.
		/// \param top Right edge of the Rect.
		/// \param width Width of the Rect.
		/// \param height Height of the Rect.
		///@{
		Rect& Init() noexcept;

		Rect& Init( Valtype left, Valtype top, Valtype width, Valtype height ) noexcept;
		///@}


		/// \brief	is bottom <= top and left <= right
		bool Normal() const noexcept;


		/// \brief	make bottom <= top and left <= right
		Rect& Normalize() noexcept;


		/// \returns The left value of this rect.
		Valtype Left() const noexcept;


		/// \returns The top value of this rect.
		Valtype Top() const noexcept;


		/// \returns The right value of this rect.
		Valtype Right() const noexcept;


		/// \returns The bottom value of this rect.
		Valtype Bottom() const noexcept;


		Rect& Left( Valtype val ) noexcept;	///< \brief Sets left edge of this rect.
		Rect& Top( Valtype val ) noexcept;	///< \brief Sets top edge of this rect.
		Rect& Right( Valtype val ) noexcept;	///< \brief Sets right edge of this rect.
		Rect& Bottom( Valtype val ) noexcept;	///< \brief Sets bottom edge of this rect.

		Position2D<Valtype> LeftTop		() const noexcept;	///< \returns LeftTop of this rect.
		Position2D<Valtype> LeftBottom	() const noexcept;	///< \returns LeftBottom of this rect.
		Position2D<Valtype> RightBottom	() const noexcept;	///< \returns RightBottom of this rect.
		Position2D<Valtype> RightTop	() const noexcept;	///< \returns RightTop of this rect.
		Position2D<Valtype>	Center		() const noexcept;	///< \returns Center of this rect.


		/// \returns Width of rect.
		Valtype Width() const noexcept;


		/// \returns Height of rect.
		Valtype Height() const noexcept;


		/// \returns Area of rect.
		auto Area() const noexcept -> decltype(Valtype{}*Valtype{});


		/// \returns The length of the diagonal.
		Valtype Diagonal() const noexcept;


		/// \brief	Is non zero area?
		bool IsArea() const noexcept;


		/// \brief Calculates the union of the two rects.
		Rect& Union( const Rect& r1, const Rect& r2 ) noexcept;


		/// \brief Calculates the union of this and rect.
		Rect& Union( const Rect& rect ) noexcept;


		/// \brief	Calculates the intersection rect of r1 and r2. 
		///
		/// If these rects are not in touch, the resulting
		/// rect will not be normal.
		Rect& Intersection( const Rect& r1, const Rect& r2 ) noexcept;
		
		
		/// \brief	Calculates the intersection of this and rect. 
		///
		/// If the rects are not in touch, the resulting
		/// rect will not be normal.
		Rect& Intersection( const Rect& rect ) noexcept;


		/// \returns True if the point lays inside the rect. 
		///
		/// The left and top edges are inside the rect; the right 
		/// and bottom edges are not
		bool Includes( Valtype x, Valtype y ) const noexcept;


		/// \returns True if the point lays inside the rect. 
		///
		/// The left and top edges are inside the rect; the right 
		/// and bottom edges are not
		bool Includes( const Position2D<Valtype>& pt ) const noexcept;


		/// \brief Expands the rect so that it touches (being included or laying on border) the point.
		///
		/// If the point is allready included in the rect, no changes are made.
		/// If val becomes m_Far Includes() will not return the value as beeing included.
		Rect& Expand( Valtype x, Valtype y ) noexcept;


		/// \brief Expands the rect so that it touches (being included or laying on border) the point.
		///
		/// If the point is allready included in the rect, no changes are made.
		/// If val becomes m_Far Includes() will not return the value as beeing included.
		Rect& Expand( const Position2D<Valtype>& pt ) noexcept;


		/// \brief Moves the edges to the outside of the rect, thereyby inflating it.
		/// \param dx Distance to move horizontal edges.
		/// \param dy Distance to move vertical edges.
		Rect& Inflate( Valtype dx, Valtype dy ) noexcept;


		/// \brief Moves the edges to the inward side of the rect, thereyby deflating it.
		/// \param dx Distance to move horizontal edges.
		/// \param dy Distance to move vertical edges.
		Rect& Deflate( Valtype dx, Valtype dy ) noexcept;


		/// \name Translation
		/// \brief Moves the edges in one direction, thereby moving the rect.
		/// \param dx Distance to move horizontal edges.
		/// \param dy Distance to move vertical edges.
		/// \param offset Distance to move the rect.
		///@{
		Rect& Move( Valtype dx, Valtype dy ) noexcept;

		Rect& Move( const Vector2D<Valtype>& offset ) noexcept;
		///@}
	};


	/// \name Mathematical Operators for Rect
	///@{
	template<typename Valtype>
	void			operator+=( Rect<Valtype>& r1, const Rect<Valtype>& r2 ) noexcept;
	template<typename Valtype>
	Rect<Valtype>	operator+( const Rect<Valtype>& r1, const Rect<Valtype>& r2 ) noexcept;
	template<typename Valtype> constexpr 
	bool operator==( const Rect<Valtype>& r1, const Rect<Valtype>& r2 ) noexcept;
	template<typename Valtype> constexpr 
	bool operator!=( const Rect<Valtype>& r1, const Rect<Valtype>& r2 ) noexcept;
	///@}



	/// \returns true if the two rects have a nonzero common area.
	template<typename Valtype> 
	bool Intersect( const Rect<Valtype>& r1, const Rect<Valtype>& r2 ) noexcept;

///////////////////////////////////////
//inlines:
template<typename Valtype> inline 
Rect<Valtype>::Rect( Valtype width, Valtype height ) noexcept
	:	m_Width	{-width/2,width/2},
		m_Height{-height/2,height/2}
{}

template<typename Valtype> inline 
Rect<Valtype>::Rect( const common::Interval<Valtype>& width, const common::Interval<Valtype>& height ) noexcept
	:	m_Width	{width},
		m_Height{height}
{}

template<typename Valtype> inline 
Rect<Valtype>::Rect( const Vector2D<Valtype>& diagonal ) noexcept
	:	m_Width	{-diagonal.dx/2,diagonal.dx/2},
		m_Height{-diagonal.dy/2,diagonal.dy/2}
{}

template<typename Valtype> inline 
Rect<Valtype>::Rect( const Position2D<Valtype>& lefttop, const Position2D<Valtype>& rightbottom ) noexcept
	:	m_Width	{lefttop.x,rightbottom.x},
		m_Height{lefttop.y,rightbottom.y}
{}

template<typename Valtype> inline 
Rect<Valtype>::Rect( Valtype left, Valtype top, Valtype right, Valtype bottom ) noexcept
	:	m_Width	{left,right},
		m_Height{top,bottom}
{}

template<typename Valtype> inline
Rect<Valtype>& Rect<Valtype>::Init() noexcept{
	m_Width.Init();
	m_Height.Init();
	return *this;
}

template<typename Valtype> inline
Rect<Valtype>& Rect<Valtype>::Init( Valtype left, Valtype top, Valtype width, Valtype height ) noexcept{
	m_Width.Init( left, width );
	m_Height.Init( top, height );
	return *this;
}

template<typename Valtype> inline
bool Rect<Valtype>::Normal() const noexcept{
	return m_Width.Normal() && m_Height.Normal();
}

template<typename Valtype> inline
Rect<Valtype>& Rect<Valtype>::Normalize() noexcept{
	m_Width.Normalize();
	m_Height.Normalize();
	return *this;
}

template<typename Valtype> inline 
Valtype Rect<Valtype>::Left() const noexcept{
	return m_Width.m_Near;
}

template<typename Valtype> inline 
Valtype Rect<Valtype>::Top() const noexcept{
	return m_Height.m_Near;
}

template<typename Valtype> inline 
Valtype Rect<Valtype>::Right() const noexcept{
	return m_Width.m_Far;
}

template<typename Valtype> inline 
Valtype Rect<Valtype>::Bottom() const noexcept{
	return m_Height.m_Far;
}

template<typename Valtype> inline 
Rect<Valtype>& Rect<Valtype>::Left( Valtype val ) noexcept{
	m_Width.Near(val);
	return *this;
}

template<typename Valtype> inline 
Rect<Valtype>& Rect<Valtype>::Top( Valtype val ) noexcept{
	m_Height.Near(val);
	return *this;
}

template<typename Valtype> inline 
Rect<Valtype>& Rect<Valtype>::Right( Valtype val ) noexcept{
	m_Width.Far(val);
	return *this;
}

template<typename Valtype> inline 
Rect<Valtype>& Rect<Valtype>::Bottom( Valtype val ) noexcept{
	m_Height.Far(val);
	return *this;
}

template<typename Valtype> inline 
Position2D<Valtype> Rect<Valtype>::LeftTop() const noexcept{
	return { m_Width.Near(), m_Height.Near() };
}

template<typename Valtype> inline 
Position2D<Valtype> Rect<Valtype>::LeftBottom() const noexcept{
	return { m_Width.Near(), m_Height.Far() };
}

template<typename Valtype> inline 
Position2D<Valtype> Rect<Valtype>::RightBottom() const noexcept{
	return { m_Width.Far(), m_Height.Far() };
}

template<typename Valtype> inline 
Position2D<Valtype> Rect<Valtype>::RightTop	() const noexcept{
	return { m_Width.Far(), m_Height.Near() };
}

template<typename Valtype> inline 
Position2D<Valtype> Rect<Valtype>::Center() const noexcept{
	return { m_Width.Center(), m_Height.Center() };
}

template<typename Valtype> inline
Valtype Rect<Valtype>::Width() const noexcept{
	return m_Width.Length();
}

template<typename Valtype> inline
Valtype Rect<Valtype>::Height() const noexcept{
	return m_Height.Length();
}

template<typename Valtype> inline
auto Rect<Valtype>::Area() const noexcept -> decltype(Valtype{}*Valtype{}){
	return Width() * Height();
}

template<typename Valtype> inline
Valtype Rect<Valtype>::Diagonal() const noexcept{
	return sqrt( Square(Width()) + Square(Height()) );
}

template<typename Valtype> inline
bool Rect<Valtype>::IsArea() const noexcept{
	return (Normal() && Area() != 0.0f);
}

template<typename Valtype> inline
Rect<Valtype>& Rect<Valtype>::Union( 
		const Rect<Valtype>& r1, const Rect<Valtype>& r2 ) noexcept
{
	m_Width.Union( r1.m_Width, r2.m_Width );
	m_Height.Union( r1.m_Height, r2.m_Height );
	return *this;
}

template<typename Valtype> inline
Rect<Valtype>& Rect<Valtype>::Union( const Rect<Valtype>& rect ) noexcept
{
	m_Width.Union( rect.m_Width );
	m_Height.Union( rect.m_Height );
	return *this;
}

template<typename Valtype> inline
Rect<Valtype>& Rect<Valtype>::Intersection( 
		const Rect<Valtype>& r1, const Rect<Valtype>& r2 ) noexcept
{
	m_Width.Intersection( r1.m_Width, r2.m_Width );
	m_Height.Intersection( r1.m_Height, r2.m_Height );
	return *this;
}

template<typename Valtype> inline
Rect<Valtype>& Rect<Valtype>::Intersection( const Rect& rect ) noexcept
{
	m_Width.Intersection( rect.m_Width );
	m_Height.Intersection( rect.m_Height );
	return *this;
}

template<typename Valtype> inline
bool Rect<Valtype>::Includes( Valtype x, Valtype y ) const noexcept{
	return m_Width.Includes( x ) && m_Height.Includes( y );
}

template<typename Valtype> inline
bool Rect<Valtype>::Includes( const Position2D<Valtype>& pt ) const noexcept{
	return Includes( pt.x, pt.y );
}

template<typename Valtype> inline
Rect<Valtype>& Rect<Valtype>::Expand( Valtype x, Valtype y ) noexcept{
	m_Width.Expand( x );
	m_Height.Expand( y );
	return *this;
}

template<typename Valtype> inline
Rect<Valtype>& Rect<Valtype>::Expand( const Position2D<Valtype>& pt ) noexcept{
	return Expand( pt.x, pt.y );
}

template<typename Valtype> inline
Rect<Valtype>& Rect<Valtype>::Inflate( Valtype dx, Valtype dy ) noexcept{
	m_Width.Inflate( dx );
	m_Height.Inflate( dy );
	return *this;
}

template<typename Valtype> inline
Rect<Valtype>& Rect<Valtype>::Deflate( Valtype dx, Valtype dy ) noexcept{
	return Inflate( -dx, -dy );
}

template<typename Valtype> inline
Rect<Valtype>& Rect<Valtype>::Move( Valtype dx, Valtype dy ) noexcept{
	m_Width.Move( dx );
	m_Height.Move( dy );
	return *this;
}

template<typename Valtype> inline 
Rect<Valtype>& Rect<Valtype>::Move( const Vector2D<Valtype>& offset ) noexcept{
	return Move( offset.dx, offset.dy );
}

template<typename Valtype> inline
void operator+=( Rect<Valtype>& r1, const Rect<Valtype>& r2 ) noexcept{
	r1.Union(r2);
}

template<typename Valtype> inline
Rect<Valtype> operator+( const Rect<Valtype>& r1, const Rect<Valtype>& r2 ) noexcept{
	Rect<Valtype> retval;
	retval.Union( r1, r2 );
	return retval;
}

template<typename Valtype> inline
bool Intersect( const Rect<Valtype>& r1, const Rect<Valtype>& r2 ) noexcept{
	return Intersect( r1.m_Width, r2.m_Width ) && Intersect( r1.m_Height, r2.m_Height );
}

template<typename Valtype> constexpr 
inline bool operator==( const Rect<Valtype>& r1, const Rect<Valtype>& r2 ) noexcept{
	return r1.m_Width == r2.m_Width && r1.m_Height == r2.m_Height;
}

template<typename Valtype> constexpr 
inline bool operator!=( const Rect<Valtype>& r1, const Rect<Valtype>& r2 ) noexcept{
	return !(r1 == r2);
}

} // namespace spat


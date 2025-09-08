//	trax track library
//	AD 2014 
//
// "She's got a box full [?]"
//
//					Jade Bird
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

#include "common/Interval.h"


namespace spat{

	template<typename> struct Position;
	template<typename> struct Sphere;
	template<typename> struct Vector;

	///	\brief Axis aligned box.
	template<typename Valtype>
	struct Box
	{
		typedef Valtype value_type;

		common::Interval<Valtype> m_WidthX;	///< width
		common::Interval<Valtype> m_WidthY;	///< depth
		common::Interval<Valtype> m_WidthZ;	///< height


		/// \name Construction
		/// \param xext
		/// \param yext
		/// \param zext
		/// \param cuboid
		/// \param xnear
		/// \param ynear
		/// \param znear
		/// \param xfar
		/// \param yfar
		/// \param zfar
		/// \param nearCorner
		/// \param farCorner
		///@{
					Box() noexcept = default;				///< Does not initialize the members.
					Box( Valtype xext, Valtype yext, Valtype zext ) noexcept;
		explicit	Box( const Vector<Valtype>& cuboid ) noexcept;
					Box( Valtype xnear, Valtype ynear, Valtype znear, Valtype xfar, Valtype yfar, Valtype zfar ) noexcept;
					Box( const Position<Valtype>& nearCorner, const Position<Valtype>& farCorner ) noexcept;
		///@}


		/// \brief Initializes the interval to origin.
		Box& Init() noexcept;


		/// \brief	Is the box normalized?
		bool Normal() const noexcept;


		///	\brief Normalizes the box by changing the corners, so 
		///	that the near corner has smaller coordinates than 
		///	the far corner.
		Box& Normalize() noexcept;


		///	\returns The position of the near corner.
		const Position<Valtype> Near() const noexcept;


		///	\returns The position of the far corner.
		const Position<Valtype> Far() const noexcept;


		/// \returns The near X dimension value of this Box.
		Valtype NearX() const noexcept;


		/// \returns The near Y dimension value of this Box.
		Valtype NearY() const noexcept;


		/// \returns The near Z dimension value of this Box.
		Valtype NearZ() const noexcept;


		/// \returns The far X dimension value of this Box.
		Valtype FarX() const noexcept;


		/// \returns The far Y dimension value of this Box.
		Valtype FarY() const noexcept;


		/// \returns The far Z dimension value of this Box.
		Valtype FarZ() const noexcept;


		/// \brief Sets the near X dimension value of this Box.
		Box& NearX( Valtype val ) noexcept;


		/// \brief Sets the near Y dimension value of this Box.
		Box& NearY( Valtype val ) noexcept;


		/// \brief Sets the near Z dimension value of this Box.
		Box& NearZ( Valtype val ) noexcept;


		/// \brief Sets the far X dimension value of this Box.
		Box& FarX( Valtype val ) noexcept;


		/// \brief Sets the far Y dimension value of this Box.
		Box& FarY( Valtype val ) noexcept;


		/// \brief Sets the far Z dimension value of this Box.
		Box& FarZ( Valtype val ) noexcept;


		///	\returns The length of the edge aligned with the X-axis.
		Valtype EdgeX() const noexcept;


		/// \brief Sets the width of the edge by centering
		/// it around 0.
		Box& EdgeX( Valtype width ) noexcept;


		///	\returns The length of the edge aligned with the Y-axis.
		Valtype EdgeY() const noexcept;


		/// \brief Sets the width of the edge by centering
		/// it around 0.
		Box& EdgeY( Valtype width ) noexcept;


		///	\returns The length of the edge aligned with the Z-axis.
		Valtype EdgeZ() const noexcept;


		/// \brief Sets the width of the edge by centering
		/// it around 0.
		Box& EdgeZ( Valtype width ) noexcept;


		/// \returns The volume of the box.
		auto Volume() const noexcept -> decltype(Valtype{}*Valtype{}*Valtype{});


		/// \returns The diagonal vector.
		Vector<Valtype> Diagonal() const noexcept;


		///	\returns The geometric center of the box.
		Position<Valtype> Center() const noexcept;


		/// \brief Calculates the union of the two Boxes.
		Box& Union( const Box& b1, const Box& b2 ) noexcept;


		/// \brief Calculates the union of this and rect.
		Box& Union( const Box& box ) noexcept;


		/// \returns True if the point lays inside the box. 
		///
		/// The near edges are inside the box; the far 
		/// edges are not
		bool Includes( Valtype x, Valtype y, Valtype z ) const noexcept;


		/// \returns True if the point lays inside the box. 
		///
		/// The near edges are inside the box; the far 
		/// edges are not
		bool Includes( const Position<Valtype>& pt ) const noexcept;


		/// \brief Expands the box so that it touches (being included or laying on border) the point.
		///
		/// If the point is allready included in the box, no changes are made.
		/// If val becomes m_Far Includes() will not return the value as beeing included.
		Box& Expand( Valtype x, Valtype y, Valtype z ) noexcept;


		/// \brief Expands the box so that it touches (being included or laying on border) the point.
		///
		/// If the point is allready included in the box, no changes are made.
		/// If val becomes m_Far Includes() will not return the value as beeing included.
		Box& Expand( const Position<Valtype>& pt ) noexcept;


		/// \brief Moves the edges to the outside of the box, thereyby inflating it.
		/// \param dx Distance to move x edges.
		/// \param dy Distance to move y edges.
		/// \param dz Distance to move z edges.
		Box& Inflate( Valtype dx, Valtype dy, Valtype dz ) noexcept;


		/// \brief Moves the edges to the inward side of the box, thereyby deflating it.
		/// \param dx Distance to move x edges.
		/// \param dy Distance to move y edges.
		/// \param dz Distance to move z edges.
		Box& Deflate( Valtype dx, Valtype dy, Valtype dz ) noexcept;


		/// \brief Moves the box by dPos.
		Box& Move( const Vector<Valtype>& dPos ) noexcept;


		/// \brief Rounds the boxes values to a certain digit after the point.
		Box& Round( int toDigit ) noexcept;
	};


	/// \name Intersecting
	/// \returns true if the two bodies have a volume in common.
	///@{
	template<typename Valtype>
	bool Intersecting( const Box<Valtype>& box, const Sphere<Valtype>& sphere ) noexcept;

	template<typename Valtype>
	bool Intersecting( const Box<Valtype>& boxA, const Box<Valtype>& boxB ) noexcept;
	///@}

	/// \name Comparison
	///@{
	template<typename Valtype> constexpr 
	bool operator==( const Box<Valtype>& b1, const Box<Valtype>& b2 ) noexcept;
	template<typename Valtype> constexpr 
	bool operator!=( const Box<Valtype>& b1, const Box<Valtype>& b2 ) noexcept;
	///@}


//	///	Transform Box by homogenous transformation matrix
//	template<typename Valtype>
//	Box<Valtype> operator*( const Transformation<Valtype>& mat, const Box<Valtype>& sp );
//
//	///	Smallest geom that contains several points.
//	///	Take an array of vertices. The size counts
//	///	in vertices and the stride is in bytes.
//	template<typename Valtype>
//	Box<Valtype>& SmallestContainingAll(	Box<Valtype>& Result,
//											const void* pPositions,
//											unsigned int numPositions,
//											unsigned short offset = 0,
//											unsigned short stride = sizeof(Position<Valtype>) );
//
//
///////////////////////////////////////
template<typename Valtype> inline 
Box<Valtype>::Box( Valtype xext, Valtype yext, Valtype zext ) noexcept
	:	m_WidthX{-xext/2,xext/2},
		m_WidthY{-yext/2,yext/2},
		m_WidthZ{-zext/2,zext/2}
{}

template<typename Valtype> inline 
Box<Valtype>::Box( const Vector<Valtype>& diagonal ) noexcept
	:	m_WidthX{-diagonal.dx/2,diagonal.dx/2},
		m_WidthY{-diagonal.dy/2,diagonal.dy/2},
		m_WidthZ{-diagonal.dz/2,diagonal.dz/2}
{}

template<typename Valtype> inline 
Box<Valtype>::Box( Valtype xnear, Valtype ynear, Valtype znear, Valtype xfar, Valtype yfar, Valtype zfar ) noexcept
	:	m_WidthX{xnear,xfar},
		m_WidthY{ynear,yfar},
		m_WidthZ{znear,zfar}
{}

template<typename Valtype> inline
Box<Valtype>::Box( const Position<Valtype>& nearCorner, const Position<Valtype>& farCorner ) noexcept
	:	m_WidthX{nearCorner.x,farCorner.x},
		m_WidthY{nearCorner.y,farCorner.y},
		m_WidthZ{nearCorner.z,farCorner.z}
{}

template<typename Valtype> inline
Box<Valtype>& Box<Valtype>::Init() noexcept{
	m_WidthX.Init();
	m_WidthY.Init();
	m_WidthZ.Init();
	return *this;
}

template<typename Valtype> inline
bool Box<Valtype>::Normal() const noexcept{
	return m_WidthX.Normal() && m_WidthY.Normal() && m_WidthZ.Normal();
}

template<typename Valtype> inline 
Box<Valtype>& Box<Valtype>::Normalize() noexcept{
	m_WidthX.Normalize();
	m_WidthY.Normalize();
	m_WidthZ.Normalize();
	return *this;
}

template<typename Valtype> inline 
const Position<Valtype> Box<Valtype>::Near() const noexcept{
	return { NearX(), NearY(), NearZ() };
}

template<typename Valtype> inline 
const Position<Valtype> Box<Valtype>::Far() const noexcept{
	return { FarX(), FarY(), FarZ() };
}

template<typename Valtype> inline 
Valtype Box<Valtype>::NearX() const noexcept{
	return m_WidthX.Near();
}

template<typename Valtype> inline 
Valtype Box<Valtype>::NearY() const noexcept{
	return m_WidthY.Near();
}

template<typename Valtype> inline 
Valtype Box<Valtype>::NearZ() const noexcept{
	return m_WidthZ.Near();
}

template<typename Valtype> inline 
Valtype Box<Valtype>::FarX() const noexcept{
	return m_WidthX.Far();
}

template<typename Valtype> inline 
Valtype Box<Valtype>::FarY() const noexcept{
	return m_WidthY.Far();
}

template<typename Valtype> inline 
Valtype Box<Valtype>::FarZ() const noexcept{
	return m_WidthZ.Far();
}

template<typename Valtype> inline 
Box<Valtype>& Box<Valtype>::NearX( Valtype val ) noexcept{
	m_WidthX.Near(val);
	return *this;
}

template<typename Valtype> inline 
Box<Valtype>& Box<Valtype>::NearY( Valtype val ) noexcept{
	m_WidthY.Near(val);
	return *this;
}

template<typename Valtype> inline 
Box<Valtype>& Box<Valtype>::NearZ( Valtype val ) noexcept{
	m_WidthZ.Near(val);
	return *this;
}

template<typename Valtype> inline 
Box<Valtype>& Box<Valtype>::FarX( Valtype val ) noexcept{
	m_WidthX.Far(val);
	return *this;
}

template<typename Valtype> inline 
Box<Valtype>& Box<Valtype>::FarY( Valtype val ) noexcept{
	m_WidthY.Far(val);
	return *this;
}

template<typename Valtype> inline 
Box<Valtype>& Box<Valtype>::FarZ( Valtype val ) noexcept{
	m_WidthZ.Far(val);
	return *this;
}

template<typename Valtype> inline 
Valtype Box<Valtype>::EdgeX() const noexcept{
	return m_WidthX.Length();
}

template<typename Valtype> inline 
Box<Valtype>& Box<Valtype>::EdgeX( Valtype width ) noexcept{
	m_WidthX.Length(width);
	return *this;
}

template<typename Valtype> inline 
Valtype Box<Valtype>::EdgeY() const noexcept{
	return m_WidthY.Length();
}

template<typename Valtype> inline 
Box<Valtype>& Box<Valtype>::EdgeY( Valtype width ) noexcept{
	m_WidthY.Length(width);
	return *this;
}

template<typename Valtype> inline 
Valtype Box<Valtype>::EdgeZ() const noexcept{
	return m_WidthZ.Length();
}

template<typename Valtype> inline 
Box<Valtype>& Box<Valtype>::EdgeZ( Valtype width ) noexcept{
	m_WidthZ.Length(width);
	return *this;
}

template<typename Valtype> inline 
auto Box<Valtype>::Volume() const noexcept -> decltype(Valtype{}*Valtype{}*Valtype{}){
	return EdgeX() * EdgeY() * EdgeZ();
}

template<typename Valtype> inline 
Vector<Valtype> Box<Valtype>::Diagonal() const noexcept{
	return { EdgeX(), EdgeY(), EdgeZ() };
}

template<typename Valtype> inline 
Position<Valtype> Box<Valtype>::Center() const noexcept{
	return { m_WidthX.Center(), m_WidthY.Center(), m_WidthZ.Center() };
}

template<typename Valtype> inline 
Box<Valtype>& Box<Valtype>::Union( const Box& b1, const Box& b2 ) noexcept{
	m_WidthX.Union( b1.m_WidthX, b2.m_WidthX );
	m_WidthY.Union( b1.m_WidthY, b2.m_WidthY );
	m_WidthZ.Union( b1.m_WidthZ, b2.m_WidthZ );
	return *this;
}


template<typename Valtype> inline 
Box<Valtype>& Box<Valtype>::Union( const Box& box ) noexcept{
	m_WidthX.Union( box.m_WidthX );
	m_WidthY.Union( box.m_WidthY );
	m_WidthZ.Union( box.m_WidthZ );
	return *this;
}

template<typename Valtype> inline 
bool Box<Valtype>::Includes( Valtype x, Valtype y, Valtype z ) const noexcept{
	return m_WidthX.Includes(x) && m_WidthY.Includes(y) && m_WidthZ.Includes(z);
}

template<typename Valtype> inline 
bool Box<Valtype>::Includes( const Position<Valtype>& pt ) const noexcept{
	return Includes( pt.x, pt.y, pt.z );
}

template<typename Valtype> inline 
Box<Valtype>& Box<Valtype>::Expand( Valtype x, Valtype y, Valtype z ) noexcept{
	m_WidthX.Expand(x);
	m_WidthY.Expand(y);
	m_WidthZ.Expand(z);
	return *this;
}

template<typename Valtype> inline 
Box<Valtype>& Box<Valtype>::Expand( const Position<Valtype>& pt ) noexcept{
	return Expand( pt.x, pt.y, pt.z );
}

template<typename Valtype> inline 
Box<Valtype>& Box<Valtype>::Inflate( Valtype dx, Valtype dy, Valtype dz ) noexcept
{
	m_WidthX.Inflate( dx );
	m_WidthY.Inflate( dy );
	m_WidthZ.Inflate( dz );
	return *this;
}

template<typename Valtype> inline 
Box<Valtype>& Box<Valtype>::Deflate( Valtype dx, Valtype dy, Valtype dz ) noexcept
{
	m_WidthX.Deflate( dx );
	m_WidthY.Deflate( dy );
	m_WidthZ.Deflate( dz );
	return *this;
}

template<typename Valtype> inline 
Box<Valtype>& Box<Valtype>::Move( const Vector<Valtype>& dPos ) noexcept{
	m_WidthX.Move( dPos.dx );
	m_WidthY.Move( dPos.dy );
	m_WidthZ.Move( dPos.dz );
	return *this;
}

template<typename Valtype> inline 
Box<Valtype>& Box<Valtype>::Round( int toDigit ) noexcept{
	m_WidthX.Round( toDigit );
	m_WidthY.Round( toDigit );
	m_WidthZ.Round( toDigit );
	return *this;
}

template<typename Valtype> inline
bool Intersecting( const Box<Valtype>& box, const Sphere<Valtype>& sphere ) noexcept{
	// not quite entirely correct
	return Intersecting( box, sphere.ExBox() );
}

template<typename Valtype> inline
bool Intersecting( const Box<Valtype>& boxA, const Box<Valtype>& boxB ) noexcept{
	return	Intersecting( boxA.m_WidthX, boxB.m_WidthX ) &&
			Intersecting( boxA.m_WidthY, boxB.m_WidthY ) &&
			Intersecting( boxA.m_WidthZ, boxB.m_WidthZ );
}

template<typename Valtype> constexpr 
inline bool operator==( const Box<Valtype>& b1, const Box<Valtype>& b2 ) noexcept{
	return b1.m_WidthX == b2.m_WidthX && b1.m_WidthY == b2.m_WidthY && b1.m_WidthZ == b2.m_WidthZ;
}

template<typename Valtype> constexpr 
inline bool operator!=( const Box<Valtype>& b1, const Box<Valtype>& b2 ) noexcept{
	return !(b1 == b2);
}

} // namespace spat


//	trax track library
//	AD 2015
//
//  "There's a truth begging to be told"
//
//					Powderfinger
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

#include "common/support/CommonSupportStream.h"

#include "spat/Box.h"
#include "spat/Frame.h"
#include "spat/Matrix.h"
#include "spat/Rect.h"
#include "spat/Sphere.h"

#include <iostream>
#include <iomanip>

namespace spat{

	/// \name Streaming functions for spat objects
	/// \brief Streams data out and in.
	///@{
	template<typename Valtype>
	std::ostream& operator << ( std::ostream& os, const Position<Valtype>& p );

	template<typename Valtype>
	std::istream& operator >> ( std::istream& is, Position<Valtype>& p );

	template<typename Valtype>
	std::ostream& operator << ( std::ostream& os, const Position2D<Valtype>& p );

	template<typename Valtype>
	std::istream& operator >> ( std::istream& is, Position2D<Valtype>& p );

	template<typename Valtype>
	std::ostream& operator << ( std::ostream& os, const PositionH<Valtype>& p );

	template<typename Valtype>
	std::istream& operator >> ( std::istream& is, PositionH<Valtype>& p );

	template<typename Valtype>
	std::ostream& operator << ( std::ostream& os, const Vector<Valtype>& v );

	template<typename Valtype>
	std::istream& operator >> ( std::istream& is, Vector<Valtype>& v );

	template<typename Valtype>
	std::ostream& operator << ( std::ostream& os, const Vector2D<Valtype>& v );

	template<typename Valtype>
	std::istream& operator >> ( std::istream& is, Vector2D<Valtype>& v );

	template<typename Valtype,typename ValtypeT>
	std::ostream& operator << ( std::ostream& os, const VectorBundle<Valtype,ValtypeT>& vb );

	template<typename Valtype,typename ValtypeT>
	std::istream& operator >> ( std::istream& is, VectorBundle<Valtype,ValtypeT>& vb );

	template<typename Valtype,typename ValtypeT>
	std::ostream& operator << ( std::ostream& os, const VectorBundle2<Valtype,ValtypeT>& vb );

	template<typename Valtype,typename ValtypeT>
	std::istream& operator >> ( std::istream& is, VectorBundle2<Valtype,ValtypeT>& vb );

	template<typename Valtype,typename ValtypeT>
	std::ostream& operator << ( std::ostream& os, const Frame<Valtype,ValtypeT>& f );

	template<typename Valtype,typename ValtypeT>
	std::istream& operator >> ( std::istream& is, Frame<Valtype,ValtypeT>& f );

	template<typename Valtype>
	std::ostream& operator << ( std::ostream& os, const Box<Valtype>& b );

	template<typename Valtype>
	std::istream& operator >> ( std::istream& is, Box<Valtype>& b );

	template<typename Valtype>
	std::ostream& operator << ( std::ostream& os, const Rect<Valtype>& r );

	template<typename Valtype>
	std::istream& operator >> ( std::istream& is, Rect<Valtype>& r );

	template<typename Valtype>
	std::ostream& operator << ( std::ostream& os, const Sphere<Valtype>& s );

	template<typename Valtype>
	std::istream& operator >> ( std::istream& is, Sphere<Valtype>& s );


	template<typename Valtype,typename ValtypeT>
	void OrthogonalityDump( std::ostream& os, const Frame<Valtype,ValtypeT>& f );

	///@}
	/// \name Streaming for Matrix
	///@{
	template<	typename Valtype, 
				const unsigned short nCols, 
				const unsigned short nRows >
	std::ostream& operator << ( std::ostream& os, const Matrix<Valtype,nCols,nRows>& m );

	template<	typename Valtype, 
				const unsigned short nCols, 
				const unsigned short nRows >
	std::istream& operator >> ( std::istream& is, Matrix<Valtype,nCols,nRows>& m );
	///@}


template<typename Valtype>
std::ostream& operator << ( std::ostream& os, const Position<Valtype>& p ){
	os << "Position( " << p.x << ", " << p.y << ", " << p.z << " )";
	return os;
}

template<typename Valtype>
std::istream& operator >> ( std::istream& is, Position<Valtype>& p )
{
	common::StreamInHead( is, "Position" );

	char c;
	is >> p.x;
	is >> c; assert( c == ',' );
	is >> p.y;
	is >> c; assert( c == ',' );
	is >> p.z;
	is >> c; assert( c == ')' );
	return is;
}

template<typename Valtype>
std::ostream& operator << ( std::ostream& os, const Position2D<Valtype>& p ){
	os << "Position2D( " << p.x << ", " << p.y << " )";
	return os;
}

template<typename Valtype>
std::istream& operator >> ( std::istream& is, Position2D<Valtype>& p )
{
	common::StreamInHead( is, "Position2D" );

	char c;
	is >> p.x;
	is >> c; assert( c == ',' );
	is >> p.y;
	is >> c; assert( c == ')' );
	return is;
}

template<typename Valtype>
std::ostream& operator<<( std::ostream& os, const PositionH<Valtype>& p ){
	os << "PositionH( " << p.x << ", " << p.y << ", " << p.z << ", " << p.w << " )";
	return os;
}

template<typename Valtype>
std::istream& operator>>( std::istream& is, PositionH<Valtype>& p )
{
	common::StreamInHead( is, "PositionH" );

	char c;
	is >> p.x;
	is >> c; assert( c == ',' );
	is >> p.y;
	is >> c; assert( c == ',' );
	is >> p.z;
	is >> c; assert( c == ',' );
	is >> p.w;
	is >> c; assert( c == ')' );
	return is;
}

template<typename Valtype>
std::ostream& operator << ( std::ostream& os, const Vector<Valtype>& v ){
	os << "Vector( " << v.dx << ", " << v.dy << ", " << v.dz << " )";
	return os;
}

template<typename Valtype>
std::istream& operator >> ( std::istream& is, Vector<Valtype>& v )
{
	common::StreamInHead( is, "Vector" );

	char c;
	is >> v.dx;
	is >> c; assert( c == ',' );
	is >> v.dy;
	is >> c; assert( c == ',' );
	is >> v.dz;
	is >> c; assert( c == ')' );
	return is;
}

template<typename Valtype>
std::ostream& operator << ( std::ostream& os, const Vector2D<Valtype>& v ){
	os << "Vector2D( " << v.dx << ", " << v.dy << " )";
	return os;
}

template<typename Valtype>
std::istream& operator >> ( std::istream& is, Vector2D<Valtype>& v )
{
	common::StreamInHead( is, "Vector2D" );

	char c;
	is >> v.dx;
	is >> c; assert( c == ',' );
	is >> v.dy;
	is >> c; assert( c == ')' );
	return is;
}

template<typename Valtype,typename ValtypeT>
std::ostream& operator << ( std::ostream& os, const VectorBundle<Valtype,ValtypeT>& vb ){
	os << "VectorBundle( " << vb.P << ", " << vb.T << " )";
	return os;
}

template<typename Valtype,typename ValtypeT>
std::istream& operator >> ( std::istream& is, VectorBundle<Valtype,ValtypeT>& vb )
{
	common::StreamInHead( is, "VectorBundle" );

	char c;
	is >> vb.P;
	is >> c; assert( c == ',' );
	is >> vb.T;
	is >> c; assert( c == ')' );
	return is;
}

template<typename Valtype,typename ValtypeT>
std::ostream& operator << ( std::ostream& os, const VectorBundle2<Valtype,ValtypeT>& vb ){
	os << "VectorBundle2( " << vb.P << ", " << vb.T << ", " << vb.N << " )";
	return os;
}

template<typename Valtype,typename ValtypeT>
std::istream& operator >> ( std::istream& is, VectorBundle2<Valtype,ValtypeT>& vb )
{
	common::StreamInHead( is, "VectorBundle2" );

	char c;
	is >> vb.P;
	is >> c; assert( c == ',' );
	is >> vb.T;
	is >> c; assert( c == ',' );
	is >> vb.N;
	is >> c; assert( c == ')' );
	return is;
}

template<typename Valtype,typename ValtypeT>
std::ostream& operator << ( std::ostream& os, const Frame<Valtype,ValtypeT>& f ){
	os << "Frame( " << f.P << ", " << f.T << ", " << f.N << ", " << f.B << " )";
	return os;
}

template<typename Valtype,typename ValtypeT>
std::istream& operator >> ( std::istream& is, Frame<Valtype,ValtypeT>& f )
{
	common::StreamInHead( is, "Frame" );

	char c;
	is >> f.P;
	is >> c; assert( c == ',' );
	is >> f.T;
	is >> c; assert( c == ',' );
	is >> f.N;
	is >> c; assert( c == ',' );
	is >> f.B;
	is >> c; assert( c == ')' );
	return is;
}

template<typename Valtype>
std::ostream& operator << ( std::ostream& os, const Box<Valtype>& b ){
	os << "Box( " << b.m_WidthX << ", " << b.m_WidthY << ", " << b.m_WidthZ << " )";
	return os;
}

template<typename Valtype>
std::istream& operator >> ( std::istream& is, Box<Valtype>& b )
{
	common::StreamInHead( is, "Box" );

	char c;
	is >> b.m_WidthX;
	is >> c; assert( c == ',' );
	is >> b.m_WidthY;
	is >> c; assert( c == ',' );
	is >> b.m_WidthZ;
	is >> c; assert( c == ')' );
	return is;
}

template<typename Valtype>
std::ostream& operator << ( std::ostream& os, const Rect<Valtype>& r ){
	os << "Rect( " << r.m_Width << ", " << r.m_Height << " )";
	return os;
}

template<typename Valtype>
std::istream& operator >> ( std::istream& is, Rect<Valtype>& r )
{
	common::StreamInHead( is, "Rect" );

	char c;
	is >> r.m_Width;
	is >> c; assert( c == ',' );
	is >> r.m_Height;
	is >> c; assert( c == ')' );
	return is;
}

template<typename Valtype>
std::ostream& operator << ( std::ostream& os, const Sphere<Valtype>& s ){
	os << "Sphere( " << s.c << ", " << s.r << " )";
	return os;
}

template<typename Valtype>
std::istream& operator >> ( std::istream& is, Sphere<Valtype>& s )
{
	common::StreamInHead( is, "Sphere" );

	char c;
	is >> s.c;
	is >> c; assert( c == ',' );
	is >> s.r;
	is >> c; assert( c == ')' );
	return is;
}

template<typename Valtype,typename ValtypeT>
void OrthogonalityDump( std::ostream& os, const Frame<Valtype,ValtypeT>& f ){
	os << "T*T: " << f.T*f.T <<  ", T*N: " << f.T*f.N <<  ", T*B: " << f.T*f.B << std::endl;
	os << "N*T: " << f.N*f.T <<  ", N*N: " << f.N*f.N <<  ", T*B: " << f.N*f.B << std::endl;
	os << "B*T: " << f.B*f.T <<  ", B*N: " << f.B*f.N <<  ", B*B: " << f.B*f.B << std::endl;
}

template<	typename Valtype, 
			const unsigned short  nCols, 
			const unsigned short  nRows >
std::ostream& operator << ( std::ostream& os, const Matrix<Valtype,nCols,nRows>& matrix ){
	os << "<Matrix";
	for( unsigned short i  = 0; i < nRows; ++i ){
		os << "\n";
		for( unsigned short j = 0; j < nCols; ++j ){
			os << "(" << j << "," << i << ") = " << matrix( j, i ) << "\t";
		}
	}
	os << "\n/>";
	return os;
}

template<	typename Valtype, 
			const unsigned short  nCols, 
			const unsigned short  nRows >
std::istream& operator >> ( std::istream& is, Matrix<Valtype,nCols,nRows>& m ){
	// this will need a parser to work properly ...
	throw std::logic_error{ "Not implemented" };
	return is;
}

}
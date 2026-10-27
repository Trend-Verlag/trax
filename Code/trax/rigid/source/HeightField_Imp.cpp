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

#include "HeightField_Imp.h"
#include "trax/rigid/GeomType.h"

#include <algorithm>
#include <cmath>

namespace trax{

	using namespace spat;

///////////////////////////////////////
std::unique_ptr<HeightField> HeightField::Make() noexcept
{
	try{
		return std::make_unique<HeightField_Imp>();
	}
	catch(...){
		return nullptr;
	}
}
///////////////////////////////////////
HeightField_Imp::HeightField_Imp()
	: m_Frame{ Identity<Length,One> }
	, m_pData{ std::make_shared<Data>() }
{}

HeightField_Imp::HeightField_Imp( std::shared_ptr<Data> pData )
	: m_Frame{ Identity<Length,One> }
	, m_pData{ pData }
{
	if( !m_pData )
		throw std::invalid_argument( "HeightField_Imp::HeightField_Imp(): pData is null" );
}

const char* HeightField_Imp::TypeName() const noexcept
{
	return "Memory_HeightField";
}

std::unique_ptr<Geom> HeightField_Imp::Instance() const noexcept{
	return std::unique_ptr<HeightField_Imp>{ new HeightField_Imp{ m_pData } };
}

void HeightField_Imp::SetFrame( const Frame<Length,One>& frame ) noexcept{
	m_Frame = frame;
}

void HeightField_Imp::GetFrame( Frame<Length,One>& frame ) const noexcept{
	frame = m_Frame;
}

Box<Length> HeightField_Imp::GetExtent() const noexcept{
	Box<Length> extent = m_pData->GetExtent();
	m_Frame.ToParent( extent );
	return extent;
}

SquareMatrix<Area,3> HeightField_Imp::SpecificInertiaTensor() const{
	throw std::runtime_error( "HeightField_Imp::SpecificInertiaTensor(): not applicable for static geometry" );
}

Volume HeightField_Imp::GetVolume() const noexcept{
	return 0_m3;
}

GeomType HeightField_Imp::GetGeomType() const noexcept{
	return GeomType::heightField;
}

bool HeightField_Imp::IsOverlapping( const Geom& other ) const noexcept{
	return Intersecting( GetExtent(), other.GetExtent() );
}

bool HeightField_Imp::Create( 
	common::Span<const short> samples, 
	common::Span<const bool> holes, 
	const Vector2D<int>& rasterRect, 
	Length rasterScale, 
	Length heightScale )
{
	if( heightScale <= 0_m )
		return false;
	if( !IsValidData( samples.size, holes.size, rasterRect, rasterScale ) )
		return false;

	m_pData->Create( rasterRect, rasterScale );
	for( size_t i = 0; i < m_pData->m_Samples.size(); ++i ){
		m_pData->m_Samples[i].height = samples[i] * heightScale;
		m_pData->m_Samples[i].bHole = holes[i];
	}
	return true;
}

bool HeightField_Imp::Create( 
	common::Span<const Length> samples, 
	common::Span<const bool> holes, 
	const Vector2D<int>& rasterRect, 
	Length rasterScale )
{
	if( !IsValidData( samples.size, holes.size, rasterRect, rasterScale ) )
		return false;

	m_pData->Create( rasterRect, rasterScale );
	for( size_t i = 0; i < m_pData->m_Samples.size(); ++i ){
		m_pData->m_Samples[i].height = samples[i];
		m_pData->m_Samples[i].bHole = holes[i];
	}
	return true;
}

bool HeightField_Imp::IsValidData( size_t nSamples, size_t nHoles, const Vector2D<int>& rasterRect, Length rasterScale ) noexcept
{
	if( rasterRect.dx <= 0 || rasterRect.dy <= 0 )
		return false;
	if( rasterScale <= 0_m )
		return false;

	const size_t nPoints = rasterRect.dx * rasterRect.dy;
	return nSamples == nPoints && nHoles == nPoints;
}

bool HeightField_Imp::Create( const Vector2D<int>& rasterExtent, const Length rasterScale )
{
	if( rasterExtent.dx <= 0 || rasterExtent.dy <= 0 )
		return false;
	if( rasterScale <= 0_m )
		return false;

	m_pData->Create( rasterExtent, rasterScale );
	return true;
}

void HeightField_Imp::Clear() noexcept
{
	m_pData = std::make_shared<Data>();
}

void HeightField_Imp::Take( HeightField& fromHeightField )
{
	HeightField_Imp* pFrom = dynamic_cast<HeightField_Imp*>( &fromHeightField );
	if( !pFrom || pFrom->m_pData->m_Samples.empty() )
		return;

	m_Frame = pFrom->m_Frame;
	m_pData = pFrom->m_pData;
	pFrom->m_pData = std::make_shared<Data>();
}

Vector2D<int> HeightField_Imp::GetRasterExtent() const noexcept{
	return m_pData->m_RasterExtent.Diagonal();
}

Length HeightField_Imp::GetRasterScale() const noexcept{
	return m_pData->m_RasterScale;
}

Length HeightField_Imp::GetHeight( const Position2D<int>& atRasterPoint ) const
{
	if( !m_pData->m_RasterRect.Includes( atRasterPoint ) )
		throw std::out_of_range( "HeightField_Imp::GetHeight(): point out of range" );

	return m_pData->GetHeight( atRasterPoint );
}

Length HeightField_Imp::GetHeight(const Position<Length>& atShapeLocalPoint ) const noexcept
{
	Position<Length> geomLocalPoint = atShapeLocalPoint;
	m_Frame.FromParent( geomLocalPoint );
	return m_pData->GetHeight( geomLocalPoint );
}

common::Interval<Length> HeightField_Imp::HeightRange() const noexcept{
	return HeightRange( m_pData->m_RasterExtent );
}

common::Interval<Length> HeightField_Imp::HeightRange( const Rect<int>& inRect ) const{
	return m_pData->HeightRange( inRect );
}

void HeightField_Imp::SetHeight( const Position2D<int>& atRasterPoint, Length height )
{
	if( !m_pData->m_RasterRect.Includes( atRasterPoint ) )
		throw std::out_of_range( "HeightField_Imp::GetHeight(): point out of range" );

	m_pData->m_Samples[ atRasterPoint.y * m_pData->m_RasterRect.Width() + atRasterPoint.x ].height = height;
}

void HeightField_Imp::SetHeight( const Rect<int>& atRasterArea, Length height )
{
	if( !m_pData->m_RasterExtent.Includes( atRasterArea ) )
		throw std::out_of_range( "HeightField_Imp::SetHeight(): area out of range" );

	for( Position2D<int> pos = atRasterArea.LeftBottom(); pos.y <= atRasterArea.Top(); ++pos.y )
	{
		for( pos.x = atRasterArea.Left(); pos.x <= atRasterArea.Right(); ++pos.x )
		{
			m_pData->m_Samples[ pos.y * m_pData->m_RasterRect.Width() + pos.x ].height = height;
		}
	}
}

bool HeightField_Imp::IsHole( const Position2D<int>& atRasterPoint ) const
{
	if( !m_pData->m_RasterRect.Includes( atRasterPoint ) )
		throw std::out_of_range( "HeightField_Imp::IsHole(): point out of range" );

	return m_pData->m_Samples[ atRasterPoint.y * m_pData->m_RasterRect.Width() + atRasterPoint.x ].bHole;
}

void HeightField_Imp::SetHole( const Position2D<int>& atRasterPoint, bool hole )
{
	if( !m_pData->m_RasterRect.Includes( atRasterPoint ) )
		throw std::out_of_range( "HeightField_Imp::SetHole(): point out of range" );

	m_pData->m_Samples[ atRasterPoint.y * m_pData->m_RasterRect.Width() + atRasterPoint.x ].bHole = hole;
}

HeightField_Imp::Data::Data()
	: m_Samples		{}
	, m_RasterExtent{ 0, 0 }
	, m_RasterRect	{ 0, 0 }
	, m_RasterScale	{ 1_m }
{}

Box<Length> HeightField_Imp::Data::GetExtent() const noexcept
{
	common::Interval<Length> range = HeightRange( m_RasterRect );
	Box<Length> extent{ 
		{ 0_m, 0_m, range.Near() }, 
		{ m_RasterScale * m_RasterExtent.Width(), m_RasterScale * m_RasterExtent.Height(), range.Far() } };
	return extent;
}

void HeightField_Imp::Data::Create( const Vector2D<int>& rasterRect, const Length rasterScale ) noexcept
{
	m_RasterExtent = m_RasterRect = spat::Rect<int>{ rasterRect };
	--m_RasterExtent.m_Width.m_Far;
	--m_RasterExtent.m_Height.m_Far;
	m_RasterScale = rasterScale;
	m_Samples.resize( m_RasterRect.Area() );
}

Length HeightField_Imp::Data::GetHeight( const Position<Length>& atLocalPoint ) const noexcept
{
	const Real u = static_cast<Real>( atLocalPoint.x / m_RasterScale );
	const Real v = static_cast<Real>( atLocalPoint.y / m_RasterScale );
	const int nCellsX = m_RasterExtent.Width();
	const int nCellsY = m_RasterExtent.Height();

	if( !(u >= 0 && v >= 0 && u <= nCellsX && v <= nCellsY) || nCellsX <= 0 || nCellsY <= 0 )
		return 0_m;

	// cell indices; points on the far edges belong to the last cell
	const int x = std::min( static_cast<int>( std::floor( u ) ), nCellsX - 1 );
	const int y = std::min( static_cast<int>( std::floor( v ) ), nCellsY - 1 );
	const Real fx = u - x;
	const Real fy = v - y;

	// Quad tesselation: [i],[i+nCols+1],[i+1] and [i],[i+nCols],[i+nCols+1]
	// i.e. the diagonal runs from (x,y) to (x+1,y+1).
	const Length h00 = GetHeight( Position2D<int>{ x,		y } );
	const Length h11 = GetHeight( Position2D<int>{ x + 1,	y + 1 } );

	if( fx >= fy ){
		// triangle (x,y),(x+1,y+1),(x+1,y)
		const Length h10 = GetHeight( Position2D<int>{ x + 1, y } );
		return h00 + fx * (h10 - h00) + fy * (h11 - h10);
	}
	else{
		// triangle (x,y),(x,y+1),(x+1,y+1)
		const Length h01 = GetHeight( Position2D<int>{ x, y + 1 } );
		return h00 + fy * (h01 - h00) + fx * (h11 - h01);
	}
}

common::Interval<Length> HeightField_Imp::Data::HeightRange( const Rect<int>& inRect ) const
{
	if( !m_RasterExtent.Includes( inRect ) )
		throw std::out_of_range( "HeightField_Imp::HeightRange(): area out of range" );

	common::Interval<Length> range{ +infinite__length, -infinite__length };
	for( Position2D<int> pos = inRect.LeftBottom(); pos.y <= inRect.Top(); ++pos.y )
	{
		for( pos.x = inRect.Left(); pos.x <= inRect.Right(); ++pos.x )
		{
			range.Expand( m_Samples[ pos.y * m_RasterRect.Width() + pos.x ].height );	
		}
	}

	return range;
}
///////////////////////////////////////
}
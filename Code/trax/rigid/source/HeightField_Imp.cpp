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
	const short* pSamples, 
	const bool* pbHoles, 
	const Vector2D<int>& rasterExtent, 
	Length rasterScale, 
	Length heightScale )
{
	throw std::runtime_error( "HeightField_Imp::Create(): not implemented yet" );
}

bool HeightField_Imp::Create( const Length* pSamples, const bool* pbHoles, const Vector2D<int>& rasterExtent, Length rasterScale )
{
	throw std::runtime_error( "HeightField_Imp::Create(): not implemented yet" );
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

Vector2D<int> HeightField_Imp::GetRasterExtent() const noexcept{
	return m_pData->m_RasterExtent.Diagonal();
}

Length HeightField_Imp::GetRasterScale() const noexcept{
	return m_pData->m_RasterScale;
}

Length HeightField_Imp::GetHeight( const Position2D<int>& atRasterPoint ) const
{
	if( !m_pData->m_RasterExtent.Includes( atRasterPoint ) )
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
	if( !m_pData->m_RasterExtent.Includes( atRasterPoint ) )
		throw std::out_of_range( "HeightField_Imp::GetHeight(): point out of range" );

	m_pData->m_Samples[ atRasterPoint.y * m_pData->m_RasterExtent.Width() + atRasterPoint.x ] = height;
}

void HeightField_Imp::SetHeight( const Rect<int>& atRasterArea, Length height )
{
	if( !m_pData->m_RasterExtent.Includes( atRasterArea ) )
		throw std::out_of_range( "HeightField_Imp::SetHeight(): area out of range" );

	for( Position2D<int> pos = atRasterArea.LeftTop(); pos.y < atRasterArea.Bottom(); ++pos.y )
	{
		for( pos.x = atRasterArea.Left(); pos.x < atRasterArea.Right(); ++pos.x )
		{
			m_pData->m_Samples[ pos.y * m_pData->m_RasterExtent.Width() + pos.x ] = height;
		}
	}
}

bool HeightField_Imp::IsHole( const Position2D<int>& atRasterPoint ) const
{
	if( !m_pData->m_RasterExtent.Includes( atRasterPoint ) )
		throw std::out_of_range( "HeightField_Imp::IsHole(): point out of range" );

	return m_pData->m_Holes[ atRasterPoint.y * m_pData->m_RasterExtent.Width() + atRasterPoint.x ];
}

void HeightField_Imp::SetHole( const Position2D<int>& atRasterPoint, bool hole )
{
	if( !m_pData->m_RasterExtent.Includes( atRasterPoint ) )
		throw std::out_of_range( "HeightField_Imp::SetHole(): point out of range" );

	m_pData->m_Holes[ atRasterPoint.y * m_pData->m_RasterExtent.Width() + atRasterPoint.x ] = hole;
}

HeightField_Imp::Data::Data()
	: m_Samples		{}
	, m_Holes		{}
	, m_RasterExtent{ 0, 0 }
	, m_RasterScale	{ 1_m }
{}

Box<Length> HeightField_Imp::Data::GetExtent() const noexcept
{
	common::Interval<Length> range = HeightRange( m_RasterExtent );
	Box<Length> extent{ 
		0_m, 0_m, range.Near(), 
		m_RasterExtent.Width() * m_RasterScale, m_RasterExtent.Height() * m_RasterScale, range.Far() };
	return extent;
}

void HeightField_Imp::Data::Create( const Vector2D<int>&rasterExtent, const Length rasterScale )
{
	m_RasterExtent = spat::Rect<int>{ {0,0}, Origin2D<int> + rasterExtent };
	m_RasterScale = rasterScale;
	m_Samples.resize( m_RasterExtent.Area() );
	m_Holes.resize( m_RasterExtent.Area() );
}

Length HeightField_Imp::Data::GetHeight( const Position<Length>& atLocalPoint ) const noexcept
{
	Position2D<int> rasterPoint{
		static_cast<int>( atLocalPoint.x / m_RasterScale ),
		static_cast<int>( atLocalPoint.y / m_RasterScale ) };

	if( m_RasterExtent.Includes( rasterPoint ) )
		return GetHeight( rasterPoint );

	return 0_m;
}

common::Interval<Length> HeightField_Imp::Data::HeightRange( const Rect<int>& inRect ) const
{
	if( !m_RasterExtent.Includes( inRect ) )
		throw std::out_of_range( "HeightField_Imp::HeightRange(): area out of range" );

	common::Interval<Length> range{ +infinite__length, -infinite__length };
	for( Position2D<int> pos = inRect.LeftTop(); pos.y < inRect.Bottom(); ++pos.y )
	{
		for( pos.x = inRect.Left(); pos.x < inRect.Right(); ++pos.x )
		{
			range.Expand( m_Samples[ pos.y * m_RasterExtent.Width() + pos.x ] );	
		}
	}

	return range;
}
///////////////////////////////////////
}
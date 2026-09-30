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


#include "Terrain_Imp.h"

#include "trax/rigid/HeightField.h"
#include "trax/Track.h"


namespace trax{

	using namespace spat;

///////////////////////////////////////
std::unique_ptr<Terrain> trax::Terrain::Make() noexcept
{
	try{
		return std::make_unique<Terrain_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
Terrain_Imp::Terrain_Imp()
	: m_Frame		{ Identity<Length, One> }
	, m_TileRange	{ 10, 10 }
	, m_HeightFields{ static_cast<size_t>(m_TileRange.Width() + 1) * static_cast<size_t>(m_TileRange.Height() + 1) }
	, m_TileExtent	{ -1_m }
{
}

const char* Terrain_Imp::TypeName() const noexcept
{
	return "Terrain";
}

void Terrain_Imp::Create( const Rect<int>& tileRange )
{
	Clear();
	m_TileRange = tileRange;
	m_HeightFields.resize( ( m_TileRange.Width() + 1 ) * ( m_TileRange.Height() + 1 ) );
}

void Terrain_Imp::SetFrame( const Frame<Length, One>&frame )
{
	if( !frame.IsOrthoNormal() )
		throw std::invalid_argument( "frame must be orthonormal" );

	m_Frame = frame;
}

const spat::Frame<Length,One>& Terrain_Imp::GetFrame() const noexcept{
	return m_Frame;
}

void Terrain_Imp::Attach( std::unique_ptr<HeightField> pTile, const Position2D<int>& tileCoordinates )
{
	if( !pTile )
		throw std::invalid_argument( "pTile must not be nullptr" );
	if( !m_TileRange.Touches( tileCoordinates ) )
		throw std::out_of_range( "tileCoordinates is out of range" );
	if( pTile->GetRasterExtent().dx != pTile->GetRasterExtent().dy )
		throw std::invalid_argument( "pTile must have a square raster extent" );

	if( m_TileExtent < 0_m ){
		m_TileExtent = pTile->GetRasterScale().dx * pTile->GetRasterExtent().dx;
	}
	else if( abs(m_TileExtent - pTile->GetRasterScale().dx * pTile->GetRasterExtent().dx) > epsilon__length )
		throw std::invalid_argument( "pTile must have the same extent as the other tiles" );

	At( tileCoordinates ) = std::move( pTile );
}

std::unique_ptr<HeightField> Terrain_Imp::Detach( const Position2D<int>& atTileCoordinates ) noexcept
{
	if( !m_TileRange.Touches( atTileCoordinates ) )
		return nullptr;

	return std::move( At( atTileCoordinates ) );
}

void Terrain_Imp::Clear() noexcept
{
	for( auto& pHeightField : m_HeightFields )
		pHeightField.reset();

	m_TileExtent = -1_m;
}

std::unique_ptr<HeightField> Terrain_Imp::SetHeight( const spat::Position<Length>& parameter, Length radius )
{
	return std::unique_ptr<HeightField>();
}

std::unique_ptr<HeightField> Terrain_Imp::SetHeight( const Position2D<Length>& parameter, Length height )
{
	TileSpacePosition tileSpacePosition = TerrainSpaceToTileSpace( *this, parameter );

	if( const std::unique_ptr<HeightField>& pHeightField = At( tileSpacePosition.tileCoordinates ); pHeightField )
	{
		Length rasterScale = pHeightField->GetRasterScale().dx;
		const int left = static_cast<int>( std::floor( tileSpacePosition.tileSpaceParameter.x / rasterScale ) );
		const int bottom = static_cast<int>( std::floor( tileSpacePosition.tileSpaceParameter.y / rasterScale ) );
		const Rect<int> RasterArea{
			left, 
			bottom + 1,
			left + 1,
			bottom };

		pHeightField->SetHeight( RasterArea, height );
	}

	return nullptr;
}

std::unique_ptr<HeightField> Terrain_Imp::SetHeight( const Rect<Length>& area, Length height )
{
	throw std::logic_error( "not implemented" );
}

std::unique_ptr<HeightField> Terrain_Imp::SetHeight( const spat::Circle<Length>& area, Length height )
{
	return std::unique_ptr<HeightField>();
}

std::unique_ptr<HeightField> Terrain_Imp::SetHeight( const HeightField & area, const spat::Position2D<int>& position )
{
	return std::unique_ptr<HeightField>();
}

Length Terrain_Imp::GetHeight( const Position2D<Length>& parameter ) const
{
	TileSpacePosition tileSpacePosition = TerrainSpaceToTileSpace( *this, parameter );

	if( const std::unique_ptr<HeightField>& pHeightField = At( tileSpacePosition.tileCoordinates ); pHeightField )
		return pHeightField->GetHeight( tileSpacePosition.tileSpaceParameter );

	return 0_m;
}

Rect<Length> Terrain_Imp::PunchHole( const Position2D<Length>& parameter, Length radius )
{
	return Rect<Length>();
}

Rect<Length> Terrain_Imp::PunchHole( const Rect<Length>& inRect )
{
	return Rect<Length>();
}

Rect<Length> Terrain_Imp::Solidify( const Rect<Length>& inRect )
{
	return Rect<Length>();
}

bool Terrain_Imp::IsSolid( const Rect<Length>& inRect ) noexcept
{
	return false;
}

Length Terrain_Imp::GetTileExtent() const noexcept{
	return m_TileExtent;
}

Rect<Length> Terrain_Imp::Range() const noexcept{
	return m_TileExtent * m_TileRange;
}

common::Interval<Length> Terrain_Imp::HeightRange() const noexcept
{
	common::Interval<Length> heightRange{ +infinite__length, -infinite__length };

	for( auto& pHeightField : m_HeightFields )
	{
		if( pHeightField )
			heightRange.Union( pHeightField->HeightRange() );
	}

	return heightRange;
}

void Terrain_Imp::Transition( const Position2D<Length>& parameter, Position<Length>& position ) const
{
	throw std::logic_error( "not implemented" );
}

void Terrain_Imp::Transition( const Position2D<Length>&parameter, VectorBundle2<Length, One>&bundle ) const
{
	throw std::logic_error( "not implemented" );
}

void Terrain_Imp::Transition( const Position2D<Length>&parameter, Frame<Length, One>&frame ) const
{
	throw std::logic_error( "not implemented" );
}
///////////////////////////////////////
Rect<Length> BuildRamp( Terrain& atTerrain, const Track& forTrack, Length width, common::Interval<Length> forTrackRange )
{
	const Length ds = 1_m;
	Rect<Length> invalArea{ +infinite__length, -infinite__length };
	forTrackRange.Intersection( forTrack.Range() );

	for( Length s = forTrackRange.Near(); s <= forTrackRange.Far(); s += ds )
	{
		Position<Length> P;
		forTrack.Transition( s, P );
		atTerrain.GetFrame().FromParent( P );
		//invalArea.Expand( atTerrain.SetHeight( P, ds ) );
	}

	return invalArea;
}

Rect<Length> PunchTunnel( Terrain& atTerrain, const SectionTrack & forTrack )
{
	throw std::logic_error( "not implemented" );
}
///////////////////////////////////////
}

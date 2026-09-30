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

#include "trax/Configuration.h"
#include "trax/Units.h"

#include "spat/Box.h"
#include "spat/Circle.h"
#include "spat/Frame.h"
#include "spat/Rect.h"
#include "spat/Vector2D.h"

#include <memory>

namespace trax{

	struct HeightField;
	struct SectionTrack;
	struct Track;

	using namespace dim;


	/// \brief A Terrain is a 2D surface in 3D space that can be used for collision detection.
	///
	/// Tiles of same extent, but 
	/// 
	/// Stub only.
	struct Terrain
	{
		/// \brief Makes a Terrain object.
		static dclspc std::unique_ptr<Terrain> Make() noexcept;


		/// \brief Name for the object type that implements this interface. 
		virtual const char*	TypeName() const noexcept = 0;

		struct TileSpacePosition{
			spat::Position2D<int>		tileCoordinates;
			spat::Position2D<Length>	tileSpaceParameter;
		};

		virtual void Create( const spat::Rect<int>& tileRange ) = 0;

		virtual void SetFrame( const spat::Frame<Length, One>& frame ) = 0;

		virtual const spat::Frame<Length,One>& GetFrame() const noexcept = 0;

		virtual void Attach( std::unique_ptr<HeightField> pTile, const spat::Position2D<int>& tileCoordinates ) = 0;

		virtual std::unique_ptr<HeightField> Detach( const spat::Position2D<int>& atTileCoordinates ) noexcept = 0;

		virtual void Clear() noexcept = 0;
		
		/// \name Height modification
		/// 
		/// \brief Sets the height of an area on the terrain.
		/// \param parameter The center point in terrain space on the terrain to set 
		/// the height for; parameter.z will be the new height.
		/// \param radius The radius around the point to set the height for.
		/// \param height The height to set.
		/// \param area The area in terrain space to set the height for.
		/// \returns The area that was modified in terrain space.
		/// @{
		
		virtual std::unique_ptr<HeightField> SetHeight( const spat::Position<Length>& parameter, Length radius ) = 0;

		virtual std::unique_ptr<HeightField> SetHeight( const spat::Position2D<Length>& parameter, Length height ) = 0;

		virtual std::unique_ptr<HeightField> SetHeight( const spat::Rect<Length>& area, Length height ) = 0;

		virtual std::unique_ptr<HeightField> SetHeight( const spat::Circle<Length>& area, Length height ) = 0;

		virtual std::unique_ptr<HeightField> SetHeight( const HeightField& area, const spat::Position2D<int>& position ) = 0;
		/// @}

		virtual Length GetHeight( const spat::Position2D<Length>& parameter ) const = 0;

		virtual spat::Rect<Length> PunchHole( const spat::Position2D<Length>& parameter, Length radius ) = 0;

		virtual spat::Rect<Length> PunchHole( const spat::Rect<Length>& inRect ) = 0;

		virtual spat::Rect<Length> Solidify( const spat::Rect<Length>& inRect ) = 0;

		virtual bool IsSolid( const spat::Rect<Length>& inRect ) noexcept = 0;


		virtual Length GetTileExtent() const noexcept = 0;

		virtual spat::Rect<Length> Range() const noexcept = 0;

		virtual common::Interval<Length> HeightRange() const noexcept = 0;


		virtual void Transition( const spat::Position2D<Length>& parameter, spat::Position<Length>& position ) const = 0;

		virtual void Transition( const spat::Position2D<Length>& parameter, spat::VectorBundle2<Length,One>& bundle ) const = 0;

		virtual void Transition( const spat::Position2D<Length>& parameter, spat::Frame<Length,One>& frame ) const = 0;
	};


	inline spat::Box<Length> BoundingBox( const Terrain& terrain );


	inline Terrain::TileSpacePosition TerrainSpaceToTileSpace( const Terrain& forTerrain, const spat::Position2D<Length>& terrainPoint );
	inline spat::Position2D<Length> TileSpaceToTerrainSpace( const Terrain& forTerrain, const Terrain::TileSpacePosition& tilePosition );

	spat::Rect<Length> dclspc BuildRamp( Terrain& atTerrain, const Track& forTrack, Length width, common::Interval<Length> forTrackRange = common::Interval<Length>{ 0_m, infinite__length } );
	
	spat::Rect<Length> dclspc PunchTunnel( Terrain& atTerrain, const SectionTrack& forTrack );

///////////////////////////////////////
inline spat::Box<Length> BoundingBox( const Terrain& terrain )
{
	spat::Rect<Length> range = terrain.Range();
	return spat::Box<Length>{ range.m_Width, range.m_Height, terrain.HeightRange() };
}

inline Terrain::TileSpacePosition TerrainSpaceToTileSpace( const Terrain& forTerrain, const spat::Position2D<Length>& terrainPoint )
{
	spat::Position2D<Real> tileCoordinatesf;
	spat::Position2D<Length> tileParameter{
		std::modf( (terrainPoint.x + forTerrain.Range().Width() / 2) / forTerrain.GetTileExtent(), &tileCoordinatesf.x ) * forTerrain.GetTileExtent(),
		std::modf( (terrainPoint.y + forTerrain.Range().Height() / 2) / forTerrain.GetTileExtent(), &tileCoordinatesf.y ) * forTerrain.GetTileExtent() };

	spat::Position2D<int> tileCoordinates = { 
		static_cast<int>( tileCoordinatesf.x - forTerrain.Range().Width() / 2 / forTerrain.GetTileExtent()), 
		static_cast<int>( tileCoordinatesf.y - forTerrain.Range().Height() / 2 / forTerrain.GetTileExtent()) };

	return { tileCoordinates, tileParameter };
}

inline spat::Position2D<Length> TileSpaceToTerrainSpace( const Terrain& forTerrain, const Terrain::TileSpacePosition& tilePosition )
{
	return spat::Position2D<Length>();
}
///////////////////////////////////////
}

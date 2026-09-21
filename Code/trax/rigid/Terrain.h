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

#include "Shape.h"

#include "spat/Rect.h"


namespace trax{

	struct SectionTrack;


	/// \brief A Terrain is a 2D surface in 3D space that can be used for collision detection.
	///
	/// The Terrain creates GeomHeightField patches automatically to provide collision detection.
	/// The exact height of the terrain can be queried at any point and the terrain can be modified 
	/// by setting the height of a point. Special methods create ramps and tunnels for tracks.
	struct Terrain : virtual Shape
	{

		/// \brief Makes a Terrain object.
		static dclspc std::shared_ptr<Terrain> Make() noexcept;


		/// \brief Name for the object type that implements this interface. 
		virtual const char*	TypeName() const noexcept = 0;


		virtual bool Create( const spat::Rect<Length>& rect, Length rasterSize, int patchSize = 16 ) = 0;

		virtual void Clear() noexcept = 0;

		virtual spat::Rect<Length> SetHeight( const spat::Position2D<Length>& parameter, Length height ) = 0;

		virtual spat::Rect<Length> SetHeight( const spat::Position2D<int>& rasterPoint, Length height ) = 0;

		virtual spat::Rect<Length> SetHeight( const spat::Rect<Length>& area, Length height ) = 0;

		virtual spat::Rect<Length> SetHeight( const spat::Rect<int>& rasterPoints, Length height ) = 0;



		virtual Length GetHeight( const spat::Position2D<Length>& parameter ) const = 0;

		virtual Length GetHeight( const spat::Position2D<int>& rasterPoint ) const = 0;

		virtual spat::Rect<Length> PunchHole( const spat::Position2D<Length>& parameter, Length radius ) = 0;

		virtual spat::Rect<Length> PunchHole( const spat::Rect<Length>& inRect ) = 0;

		virtual spat::Rect<Length> Solidify( const spat::Rect<Length>& inRect ) = 0;


		virtual bool IsSolid( const spat::Rect<Length>& inRect ) noexcept = 0;



		virtual spat::Rect<Length> Range() const noexcept = 0;


		virtual common::Interval<Length> HeightRange() const noexcept = 0;


		virtual spat::Rect<Length> BuildRamp( const SectionTrack& forTrack ) = 0;


		virtual spat::Rect<Length> PunchTunnel( const SectionTrack& forTrack ) = 0;


		virtual void Transition( const spat::Position2D<Length>& parameter, spat::Position<Length>& position ) const = 0;

		virtual void Transition( const spat::Position2D<Length>& parameter, spat::VectorBundle2<Length,One>& bundle ) const = 0;

		virtual void Transition( const spat::Position2D<Length>& parameter, spat::Frame<Length,One>& frame ) const = 0;




	};


	inline spat::Box<Length> BoundingBox( const Terrain& terrain );

///////////////////////////////////////
spat::Box<Length> BoundingBox( const Terrain& terrain )
{
	spat::Rect<Length> range = terrain.Range();
	return spat::Box<Length>{ range.m_Width, range.m_Height, terrain.HeightRange() };
}
///////////////////////////////////////
}

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


#include "trax/rigid/Terrain.h"
#include "Shape_Imp.h"

#include "spat/Vector2D.h"


namespace trax{

	class Terrain_Imp : public Terrain,
						public virtual Shape_ImpBase
	{
	public:
		Terrain_Imp();

		const char*	TypeName() const noexcept override;

		bool Create( const spat::Rect<Length>& rect, Length rasterSize, int patchSize = 16 ) override;

		void Clear() noexcept override;

		spat::Rect<Length> SetHeight( const spat::Position2D<Length>& parameter, Length height ) override;

		Length GetHeight( const spat::Position2D<Length>& parameter ) const override;

		spat::Rect<Length> PunchHole( const spat::Position2D<Length>& parameter, Length radius ) override;

		spat::Rect<Length> PunchHole( const spat::Rect<Length>& inRect ) override;

		spat::Rect<Length> Solidify( const spat::Rect<Length>& inRect ) override;


		bool IsSolid( const spat::Rect<Length>& inRect ) noexcept override;

		spat::Rect<Length> Range() const noexcept override;

		common::Interval<Length> HeightRange() const noexcept override;

		spat::Rect<Length> BuildRamp( const SectionTrack& forTrack ) override;

		spat::Rect<Length> PunchTunnel( const SectionTrack& forTrack ) override;

		void Transition( const spat::Position2D<Length>& parameter, spat::Position<Length>& position ) const override;

		void Transition( const spat::Position2D<Length>& parameter, spat::VectorBundle2<Length,One>& bundle ) const override;

		void Transition( const spat::Position2D<Length>& parameter, spat::Frame<Length,One>& frame ) const override;

	private:
		spat::Rect<Length> m_Range;
		spat::Vector2D<int> m_Size;
		Length m_RasterSize;
		int m_PatchSize;






	};

}

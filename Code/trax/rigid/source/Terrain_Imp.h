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

#include "spat/Vector2D.h"
#include "spat/Matrix.h"


namespace trax{

	class Terrain_Imp : public Terrain
	{
	public:
		Terrain_Imp();

		const char*	TypeName() const noexcept override;

		void Create( const spat::Rect<int>& tileRange ) override;

		void SetFrame( const spat::Frame<Length, One>& frame ) override;

		const spat::Frame<Length,One>& GetFrame() const noexcept override;

		void Attach( std::unique_ptr<HeightField> pTile, const spat::Position2D<int>& tileCoordinates ) override;

		std::unique_ptr<HeightField> Detach( const spat::Position2D<int>& atTileCoordinates ) noexcept override;

		void Clear() noexcept override;


		std::unique_ptr<HeightField> SetHeight( const spat::Position<Length>& parameter, Length radius ) override;

		std::unique_ptr<HeightField> SetHeight( const spat::Position2D<Length>& parameter, Length height ) override;

		std::unique_ptr<HeightField> SetHeight( const spat::Rect<Length>& area, Length height ) override;

		std::unique_ptr<HeightField> SetHeight( const spat::Circle<Length>& area, Length height ) override;

		std::unique_ptr<HeightField> SetHeight( const HeightField& area, const spat::Position2D<int>& position ) override;

		Length GetHeight( const spat::Position2D<Length>& parameter ) const override;


		spat::Rect<Length> PunchHole( const spat::Position2D<Length>& parameter, Length radius ) override;

		spat::Rect<Length> PunchHole( const spat::Rect<Length>& inRect ) override;

		spat::Rect<Length> Solidify( const spat::Rect<Length>& inRect ) override;

		bool IsSolid( const spat::Rect<Length>& inRect ) noexcept override;


		Length GetTileExtent() const noexcept override;

		spat::Rect<Length> Range() const noexcept override;

		common::Interval<Length> HeightRange() const noexcept override;


		void Transition( const spat::Position2D<Length>& parameter, spat::Position<Length>& position ) const override;

		void Transition( const spat::Position2D<Length>& parameter, spat::VectorBundle2<Length,One>& bundle ) const override;

		void Transition( const spat::Position2D<Length>& parameter, spat::Frame<Length,One>& frame ) const override;
	private:
		spat::Frame<Length,One> m_Frame;
		spat::Rect<int> m_TileRange;
		std::vector<std::unique_ptr<HeightField>> m_HeightFields;
		Length m_TileExtent;

		inline std::unique_ptr<HeightField>& At( const spat::Position2D<int>& tileCoordinates ) noexcept{
			return m_HeightFields.at( tileCoordinates.y * m_TileRange.Width() + tileCoordinates.x );
		}
		inline const std::unique_ptr<HeightField>& At( const spat::Position2D<int>& tileCoordinates ) const noexcept{
			return m_HeightFields.at( tileCoordinates.y * m_TileRange.Width() + tileCoordinates.x );
		}
	};

}

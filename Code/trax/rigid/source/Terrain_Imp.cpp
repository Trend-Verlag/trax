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


namespace trax{
///////////////////////////////////////
std::shared_ptr<Terrain> trax::Terrain::Make() noexcept
{
	return std::shared_ptr<Terrain>();
}
///////////////////////////////////////
Terrain_Imp::Terrain_Imp()
{}

const char * Terrain_Imp::TypeName() const noexcept
{
	return nullptr;
}

bool Terrain_Imp::Create( const spat::Rect<Length>& rect, Length rasterSize, int patchSize )
{
	Clear();











	return false;
}

void Terrain_Imp::Clear() noexcept
{}

spat::Rect<Length> Terrain_Imp::SetHeight( const spat::Position2D<Length>& parameter, Length height )
{
	return spat::Rect<Length>();
}

Length Terrain_Imp::GetHeight( const spat::Position2D<Length>& parameter ) const
{
	return Length();
}

spat::Rect<Length> Terrain_Imp::PunchHole( const spat::Position2D<Length>& parameter, Length radius )
{
	return spat::Rect<Length>();
}

spat::Rect<Length> Terrain_Imp::PunchHole( const spat::Rect<Length>& inRect )
{
	return spat::Rect<Length>();
}

spat::Rect<Length> Terrain_Imp::Solidify( const spat::Rect<Length>& inRect )
{
	return spat::Rect<Length>();
}

bool Terrain_Imp::IsSolid( const spat::Rect<Length>& inRect ) noexcept
{
	return false;
}

spat::Rect<Length> Terrain_Imp::Range() const noexcept
{
	return spat::Rect<Length>();
}

common::Interval<Length> Terrain_Imp::HeightRange() const noexcept
{
	return common::Interval<Length>();
}

spat::Rect<Length> Terrain_Imp::BuildRamp( const SectionTrack & forTrack )
{
	return spat::Rect<Length>();
}

spat::Rect<Length> Terrain_Imp::PunchTunnel( const SectionTrack & forTrack )
{
	return spat::Rect<Length>();
}

void Terrain_Imp::Transition( const spat::Position2D<Length>& parameter, spat::Position<Length>& position ) const
{}

void Terrain_Imp::Transition( const spat::Position2D<Length>&parameter, spat::VectorBundle2<Length, One>&bundle ) const
{}

void Terrain_Imp::Transition( const spat::Position2D<Length>&parameter, spat::Frame<Length, One>&frame ) const
{}
///////////////////////////////////////
}

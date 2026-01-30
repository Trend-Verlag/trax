//	trax track library
//	AD 2026 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
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
// For further information, please contact: horstmann.marc@trendverlag.de


#include "MovableSectionTrack_Imp.h"
//#include "trax/source/ParallelTrack_Imp.h"

#include <iostream>

namespace trax{
///////////////////////////////////////
std::shared_ptr<TrackBuilder> MovableTrack::Make( TrackType type ) noexcept
{
	try{
		switch( type ){
		case TrackType::unknown:
		case TrackType::none:
			return nullptr;
		case TrackType::standard:
			return std::make_shared<Track_Imp>();
		case TrackType::withGeoms:
			return std::make_shared<SectionTrack_Imp>();
		//case TrackType::parallel:
		//	return std::make_shared<ParallelizableTrack_Imp>();
		case TrackType::movable:
			return std::make_shared<MovableTrack_Imp>();
		case TrackType::movable_withGeoms:
			return std::make_shared<MovableSectionTrack_Imp>();
		default:
			std::cerr << "MovableTrack::Make(): Unsupported track type requested: " << ToString(type) << ". Try more specific Make() method." << std::endl;
			return nullptr;
		}
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
Track::TrackType MovableSectionTrack_Imp::GetTrackType() const noexcept{
	return Track::TrackType::movable_withGeoms;
}

bool MovableSectionTrack_Imp::IsValid() const noexcept
{
	if( !MovableTrack_Imp::IsValid() )
		return false;

	if( !SectionTrack_Imp::IsValid() )
		return false;

	return true;
}

bool MovableSectionTrack_Imp::Diagnose( std::ostream& os ) const noexcept
{
	if( !MovableTrack_Imp::Diagnose( os ) )
		return false;

	if( !SectionTrack_Imp::Diagnose( os ) )
		return false;

	return true;
}
///////////////////////////////////////
} // namespace trax
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


#include "RigidDynamicTrack_Imp.h"
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
		case TrackType::section:
			return std::make_shared<SectionTrack_Imp>();
		case TrackType::withGeoms:
			return std::make_shared<CollidableTrack_Imp>();
		//case TrackType::parallel:
		//	return std::make_shared<ParallelizableTrack_Imp>();
		case TrackType::movable:
			return std::make_shared<MovableTrack_Imp>();
		case TrackType::movable_withGeoms:
			return std::make_shared<DynamicTrack_Imp>();
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
void DynamicTrack_Imp::PropagateAbsoluteFrameToClients() noexcept{
	MovableTrack_Imp::PropagateAbsoluteFrameToClients();
}

Track::TrackType DynamicTrack_Imp::GetTrackType() const noexcept{
	return Track::TrackType::movable_withGeoms;
}

bool DynamicTrack_Imp::IsValid() const noexcept{
	return MovableTrack_Imp::IsValid();
}

bool DynamicTrack_Imp::Diagnose( std::ostream& os ) const noexcept{
	return MovableTrack_Imp::Diagnose( os );
}

void DynamicTrack_Imp::SetBody( std::shared_ptr<Body> pBody ) noexcept
{
	MovableTrack_Imp::SetBody( pBody );
	CollidableTrack_Imp::SetShape( std::dynamic_pointer_cast<Shape>(pBody) );
}

void DynamicTrack_Imp::SetShape( std::shared_ptr<Shape> pShape ) noexcept
{
	CollidableTrack_Imp::SetShape( pShape );
	MovableTrack_Imp::SetBody( std::dynamic_pointer_cast<Body>( pShape ) );
}
///////////////////////////////////////
} // namespace trax
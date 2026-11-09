//	trax track library
//	AD 2026 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
// 
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
// For further information, please contact: horstmann@traxlibrary.dev


#include "trax/TrackType.h"

#include <cassert>

namespace trax{

const char* ToString( TrackType type ) noexcept{
	switch( type ){
	case TrackType::unknown:
		return "unknown";
	case TrackType::none:
		return "none";
	case TrackType::standard:
		return "standard";
	case TrackType::section:
		return "section";
	case TrackType::static_track:
		return "static_track";
	case TrackType::collidable:
		return "collidable";
	case TrackType::movable:
		return "movable";
	case TrackType::dynamic_track:
		return "dynamic_track";
	case TrackType::movable_autoconnecting:
		return "movable_autoconnecting";
	case TrackType::parallelizable:
		return "parallelizable";
	default:
		assert( !"Unknown Track type!" );
		return "unknown";
	}
}

TrackType ToTrackType( const char* type ) noexcept{
	if( std::strcmp( type, "standard" ) == 0 )
		return TrackType::standard;
	else if( std::strcmp( type, "section" ) == 0 )
		return TrackType::section;
	else if( std::strcmp( type, "static_track" ) == 0 )
		return TrackType::static_track;
	else if( std::strcmp( type, "collidable" ) == 0 )
		return TrackType::collidable;
	else if( std::strcmp( type, "movable" ) == 0 )
		return TrackType::movable;
	else if( std::strcmp( type, "dynamic_track" ) == 0 )
		return TrackType::dynamic_track;
	else if( std::strcmp( type, "movable_autoconnecting" ) == 0 )
		return TrackType::movable_autoconnecting;
	else if( std::strcmp( type, "parallelizable" ) == 0 )
		return TrackType::parallelizable;
	else if( std::strcmp( type, "unknown" ) == 0 )
		return TrackType::unknown;
	else if( std::strcmp( type, "none" ) == 0 )
		return TrackType::none;
	else if( std::strcmp( type, "withGeoms" ) == 0 )
		return TrackType::collidable;
	else if( std::strcmp( type, "movable_withGeoms" ) == 0 )
		return TrackType::dynamic_track;
	else if( std::strcmp( type, "parallel" ) == 0 )
		return TrackType::parallelizable;
	else if( std::strcmp( type, "none" ) == 0 )
		return TrackType::none;
	else
		return TrackType::unknown;
}

}
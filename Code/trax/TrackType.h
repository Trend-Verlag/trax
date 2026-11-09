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


#pragma once

#include "Configuration.h"

namespace trax{

	enum class TrackType : char{
		unknown		= -1,
		none		= 0,
		standard,
		 section,
		  static_track,
		  collidable,
		movable,
		 dynamic_track,
		 movable_autoconnecting,
		parallelizable,
	};


	/// \brief Converts a TrackType to readable string.
	dclspc const char* ToString( TrackType type ) noexcept;

	/// \brief Finds TrackType for string.
	dclspc TrackType ToTrackType( const char* type ) noexcept;

	inline TrackType ToTrackType( const std::string& type ) noexcept;

	///////////////////////////////////////
// inlines:
inline TrackType ToTrackType( const std::string& type ) noexcept{
	return ToTrackType( type.c_str() );
}

}

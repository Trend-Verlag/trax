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
// For further information, please contact: horstmann.marc@trendverlag.de

#include "trax/End.h"

#include <stdexcept>

namespace trax{

std::string ToString( EndType end ){
	switch( end ){
	case EndType::none:
		return "none";
	case EndType::north:
		return "north";
	case EndType::south:
		return "south";
	case EndType::any:
		return "any";
	case EndType::both:
		return "both";
	default:
		throw std::invalid_argument( "Unknown EndType!" );
	}
}

EndType ToEndType( const std::string& endtype ){
	if( endtype == "none" )
		return EndType::none;
	else if( endtype == "north" )
		return EndType::north;
	else if( endtype == "south" )
		return EndType::south;
	else if( endtype == "any" )
		return EndType::any;
	else if( endtype == "both" )
		return EndType::both;
	else if( endtype == "front" )
		return EndType::north;
	else if( endtype == "end" )
		return EndType::south;
	else
		throw std::invalid_argument( "Unknown EndType!" );
}

}
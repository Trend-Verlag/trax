//	trax track library
//	AD 2020 
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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#include "Event_Imp.h"
#include <sstream>

namespace trax{
	
std::unique_ptr<EventFilter> EventFilter::Make( EventFilter::Type type ) noexcept{
	return std::make_unique<EventType>( type, "" );
}

std::string EventFilter::From( Type type ){
	switch( type ){
	case Type::wheelframe:
		return "wheelframe";
	case Type::traintip_north:
		return "traintip_north";
	case Type::traintip_south:
		return "traintip_south";
	case Type::traintip_both:
		return "traintip_both";
	case Type::any:
		return "any";
	default:
		std::ostringstream stream;
		stream << "Unknown EventFilter type! (" << static_cast<int>(type) << ")" << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::range_error( stream.str() );
	};
}

EventFilter::Type EventFilter::From( const std::string& type ){
	if( type == "wheelframe" )
		return Type::wheelframe;
	else if( type == "traintip_north" )
		return Type::traintip_north;
	else if( type == "traintip_south" )
		return Type::traintip_south;
	else if( type == "traintip_both" )
		return Type::traintip_both;
	else if( type == "any" )
		return Type::any;

	std::ostringstream stream;
	stream << "Unknown EventFilter type! (" << type << ")" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::range_error( stream.str() );
}

}
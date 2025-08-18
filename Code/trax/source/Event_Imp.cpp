// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2020 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


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
// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#include "trax/Orientation.h"

namespace trax{

std::string ToString( Orientation orient )
{
	if( orient == Orientation::Value::anti )
		return "anti";
	if( orient == Orientation::Value::para )
		return "para";
	if( orient == Orientation::Value::none )
		return "none";

	throw std::invalid_argument( "ToString: invalid Orientation value!" );
}

Orientation ToOrientation( const std::string& orient )
{
	if( orient == "anti" )
		return Orientation::Value::anti;
	if( orient == "para" )
		return Orientation::Value::para;
	if( orient == "none" )
		return Orientation::Value::none;

	throw std::invalid_argument( "ToOrientation: invalid string!" );
}

}
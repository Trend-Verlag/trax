//	trax track library
//	AD 2024 
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
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

#pragma once

#include "trax/collections/TrackSystemParser.h"
#include "trax/source/Parser_Imp.h"


namespace trax{
namespace ptreesupport{

	bool ParseTrackSystem( const boost::property_tree::ptree& pt, TrackSystemParser& callback );


}	// namespace ptreesupport
}	// namespace trax
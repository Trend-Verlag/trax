//	trax track library
//	AD 2014 
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

#include "trax/support/TraxSupportXML.h"

#include "trax/TrackLocation.h"
//#include "trax/TractionForceCharacteristic.h"
//#include "trax/Wheelset.h"

namespace trax{
	using namespace dim;
	using namespace dim::ptreesupport;
	using namespace spat;
	using namespace spat::ptreesupport;

namespace ptreesupport{

void ReadConnection( const boost::property_tree::ptree& pt, Track::End& trackend ) noexcept{
	for( const auto& pair : pt )
	{
		if( pair.first == "TrackEnd" )
			ReadTrackEnd( pair.second, trackend );
	}
}

void ReadTrackEnd( const boost::property_tree::ptree& pt, Track::End& trackend ) noexcept{
	trackend.id		= pt.get( "<xmlattr>.refid", IDType{0} );
	trackend.type	= ToEndType( pt.get( "<xmlattr>.type", "front" ) );
}

void ReadTrackLocation( const boost::property_tree::ptree& pt, TrackLocation& trackLocation ) noexcept{
	trackLocation.parameter		= get( pt, "<xmlattr>.parameter", 0_m, _m );
	trackLocation.orientation	= pt.get( "<xmlattr>.orientation", "para" ) == "anti" ? Orientation::Value::anti : Orientation::Value::para;
}

void ReadTrackLocationRef( const boost::property_tree::ptree& pt, TrackLocationRef& trackLocationRef ) noexcept{
	ReadTrackLocation( pt, trackLocationRef.location );
	trackLocationRef.refid = pt.get( "<xmlattr>.refid", IDType{0} );
}

void ReadTrackRange( const boost::property_tree::ptree& pt, TrackRange& trackRange ) noexcept{
	trackRange.range.Near( get( pt, "<xmlattr>.near", 0_m, _m ) );
	trackRange.range.Far( get( pt, "<xmlattr>.far", 0_m, _m ) );
	trackRange.refid = pt.get( "<xmlattr>.refid", IDType{0} );
}

} // namespace ptreesupport
} // namespace trax
// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#include "Parser_Imp.h"
#include "common/support/CommonSupportXML.h"
#include "dim/support/DimSupportXML.h"
#include "spat/support/SpatSupportXML.h"

namespace trax{

	using namespace dim;

namespace ptreesupport{

void ParseDisplayName( const boost::property_tree::ptree& pt, Parser& callback ) noexcept{
	return callback.DisplayName( pt.get( "<xmlattr>.language", "" ), pt.data() );
}

void ParseDescription( const boost::property_tree::ptree& pt, Parser& callback ) noexcept{
	return callback.Description( pt.get( "<xmlattr>.language", "" ), pt.data() );
}

void ParseInterval( const boost::property_tree::ptree& pt, Parser& callback ) noexcept
{
	common::Interval<Length> interval;
	common::ptreesupport::ReadInterval( pt, interval );
	return callback.Interval( interval );
}

void ParseFrame( const boost::property_tree::ptree& pt, Parser& callback ) noexcept
{
	spat::Frame<Length,One> frame;
	spat::ptreesupport::ReadFrame( pt, frame );
	return callback.Frame( frame );
}

void ParseArea( const boost::property_tree::ptree& pt, Parser& callback ) noexcept
{
	spat::Box<Length> area;
	spat::ptreesupport::ReadArea( pt, area );
	return callback.Area( area );
}

}
}

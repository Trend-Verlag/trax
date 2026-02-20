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
// For further information, please contact: horstmann.marc@trendverlag.de

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

//	trax track library
//	AD 2023 
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

#include <boost/test/unit_test.hpp>
#include "trax/rigid/MovableTrack.h"


using namespace trax;


BOOST_AUTO_TEST_SUITE(TestDynamicTrack)

BOOST_AUTO_TEST_CASE( testCreation )
{
	std::shared_ptr<TrackBuilder> pTrack = MovableTrack::Make( Track::TrackType::movable_withGeoms );
	BOOST_REQUIRE( pTrack );

	BOOST_CHECK( pTrack->GetMovableTrack() );
	BOOST_CHECK( pTrack->GetSectionTrack() );
}

BOOST_AUTO_TEST_SUITE_END() //TestDynamicTrack
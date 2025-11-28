//	trax track library
//	AD 2025 
//
//  "She got you on your knees like a little boy"
//
//								Jade Bird
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

#if defined( WITH_BOOST_TESTS )

#include <boost/test/unit_test.hpp> // NOLINT 

#include "trax/Curve.h"
#include "trax/Signal.h"
#include "trax/Track.h"

#include "trax/support/Fixtures.h"

using namespace common;
using namespace trax;
using namespace spat;

BOOST_AUTO_TEST_SUITE(trax_tests)
BOOST_AUTO_TEST_SUITE(Signal_tests)

BOOST_FIXTURE_TEST_CASE( GetRanges_OffTrack, ThreeTracksInALineFixture )
{
	common::Interval<Length> zeroFront{ 0_m, 0_m };
	common::Interval<Length> zeroEnd{ 10_m, 10_m };
	common::Interval<Length> trackrange{ 0_m, 10_m };
	common::Interval<Length> centerrange{ 3_m, 6_m };

	std::shared_ptr<VelocityControl> pSignal = VelocityControl::Make();
	m_pTrack2->Attach( pSignal, trackrange );

	TestSignalTarget signalTarget;
	BOOST_CHECK_NO_THROW( m_pTrack1->DoSignal( zeroEnd, Orientation::Value::para, signalTarget ) );
	BOOST_CHECK( !signalTarget.m_bNotifyFlag );

	TestSignalTarget signalTarget2;
	BOOST_CHECK_NO_THROW( m_pTrack2->DoSignal( zeroFront, Orientation::Value::para, signalTarget2 ) );
	BOOST_CHECK( signalTarget2.m_bNotifyFlag );

	TestSignalTarget signalTarget3;
	BOOST_CHECK_NO_THROW( m_pTrack2->DoSignal( trackrange, Orientation::Value::para, signalTarget3 ) );
	BOOST_CHECK( signalTarget3.m_bNotifyFlag );

	TestSignalTarget signalTarget4;
	BOOST_CHECK_NO_THROW( m_pTrack2->DoSignal( zeroEnd, Orientation::Value::para, signalTarget4 ) );
	BOOST_CHECK( signalTarget4.m_bNotifyFlag );

	TestSignalTarget signalTarget5;
	BOOST_CHECK_NO_THROW( m_pTrack3->DoSignal( zeroFront, Orientation::Value::para, signalTarget5 ) );
	BOOST_CHECK( !signalTarget5.m_bNotifyFlag );

	TestSignalTarget signalTarget6;
	BOOST_CHECK_NO_THROW( m_pTrack2->DoSignal( centerrange, Orientation::Value::para, signalTarget6 ) );
	BOOST_CHECK( signalTarget6.m_bNotifyFlag );

	m_pTrack2->Detach( *pSignal );

	signalTarget.m_bNotifyFlag = false;
	signalTarget2.m_bNotifyFlag = false;
	signalTarget3.m_bNotifyFlag = false;
	signalTarget4.m_bNotifyFlag = false;
	signalTarget5.m_bNotifyFlag = false;
	signalTarget6.m_bNotifyFlag = false;

	m_pTrack1->DoSignal( zeroEnd, Orientation::Value::para, signalTarget );
	BOOST_CHECK( !signalTarget.m_bNotifyFlag );

	m_pTrack2->DoSignal( zeroFront, Orientation::Value::para, signalTarget2 );
	BOOST_CHECK( !signalTarget2.m_bNotifyFlag );

	m_pTrack2->DoSignal( trackrange, Orientation::Value::para, signalTarget3 );
	BOOST_CHECK( !signalTarget3.m_bNotifyFlag );

	m_pTrack2->DoSignal( zeroEnd, Orientation::Value::para, signalTarget4 );
	BOOST_CHECK( !signalTarget4.m_bNotifyFlag );

	m_pTrack3->DoSignal( zeroFront, Orientation::Value::para, signalTarget5 );
	BOOST_CHECK( !signalTarget5.m_bNotifyFlag );

	m_pTrack2->DoSignal( centerrange, Orientation::Value::para, signalTarget6 );
	BOOST_CHECK( !signalTarget6.m_bNotifyFlag );
}

BOOST_FIXTURE_TEST_CASE( distanceoverseveraltracks2, ThreeTracksInALineFixture )
// Do Signal beyond track the signal was attached to.
{
	std::shared_ptr<VelocityControl> pSignal = VelocityControl::Make();
	common::Interval<Length> spreading{ -5_m, 15_m };
	m_pTrack2->Attach( pSignal, spreading );

	TestSignalTarget signalTarget1;
	m_pTrack3->DoSignal( m_pTrack3->Range(), Orientation::Value::para, signalTarget1 );
	BOOST_CHECK( signalTarget1.m_bNotifyFlag );

	TestSignalTarget signalTarget2;
	m_pTrack1->DoSignal( m_pTrack1->Range(), Orientation::Value::para, signalTarget2 );
	BOOST_CHECK( signalTarget2.m_bNotifyFlag );

	m_pTrack2->Detach( *pSignal );

	signalTarget1.m_bNotifyFlag = false;
	signalTarget2.m_bNotifyFlag = false;

	m_pTrack3->DoSignal( m_pTrack3->Range(), Orientation::Value::para, signalTarget1 );
	BOOST_CHECK( !signalTarget1.m_bNotifyFlag );

	m_pTrack1->DoSignal( m_pTrack1->Range(), Orientation::Value::para, signalTarget2 );
	BOOST_CHECK( !signalTarget2.m_bNotifyFlag );
}

BOOST_AUTO_TEST_SUITE_END() //Switch_tests
BOOST_AUTO_TEST_SUITE_END() //trax_tests
#endif
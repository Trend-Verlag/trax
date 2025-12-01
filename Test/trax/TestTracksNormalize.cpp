//	trax track library
//	AD 2019 
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

#if defined( WITH_BOOST_TESTS )
#include <boost/test/unit_test.hpp>

#include "trax/Track.h"
#include "trax/Curve.h"

using namespace trax;
using namespace spat;

BOOST_AUTO_TEST_SUITE(tests_trax)
BOOST_AUTO_TEST_SUITE(tests_trax_Normalize)

BOOST_AUTO_TEST_CASE( tests_trax_NormalizeArcFlipped )
{
	{
		auto pTrack = TrackBuilder::Make();
		auto pCurve = ArcP::Make();
		auto range = pCurve->Create( {{10_m,0_m,0_m},{0,1,0}}, {-10_m,0_m,0_m} );
		range.Flip();
		pTrack->Attach( std::move(pCurve), range );

		Frame<Length,One> startFrame, midFrame, endFrame, testFrame;
		pTrack->Transition( 0_m, startFrame );
		pTrack->Transition( 10_m, midFrame );
		pTrack->Transition( 20_m, endFrame );

		BOOST_CHECK( Normalize( *pTrack ) );

		pTrack->Transition( 0_m, testFrame );
		BOOST_CHECK( testFrame.P.Equals( startFrame.P, epsilon__length ) );
		BOOST_CHECK( testFrame.T.Equals( startFrame.T, epsilon__angle ) );
		BOOST_CHECK( testFrame.N.Equals( startFrame.N, epsilon__angle ) );
		BOOST_CHECK( testFrame.B.Equals( startFrame.B, epsilon__angle ) );

		pTrack->Transition( 10_m, testFrame );
		BOOST_CHECK( testFrame.P.Equals( midFrame.P, epsilon__length ) );
		BOOST_CHECK( testFrame.T.Equals( midFrame.T, epsilon__angle ) );
		BOOST_CHECK( testFrame.N.Equals( midFrame.N, epsilon__angle ) );
		BOOST_CHECK( testFrame.B.Equals( midFrame.B, epsilon__angle ) );

		pTrack->Transition( 20_m, testFrame );
		BOOST_CHECK( testFrame.P.Equals( endFrame.P, epsilon__length ) );
		BOOST_CHECK( testFrame.T.Equals( endFrame.T, epsilon__angle ) );
		BOOST_CHECK( testFrame.N.Equals( endFrame.N, epsilon__angle ) );
		BOOST_CHECK( testFrame.B.Equals( endFrame.B, epsilon__angle ) );
	}
}

BOOST_AUTO_TEST_CASE( tests_trax_NormalizeArcRotatedFlipped )
{
	{
		auto pTrack = TrackBuilder::Make();
		auto pCurve = ArcP::Make();
		pCurve->Create( {{10_m,0_m,0_m},{0,1,0}}, {-10_m,0_m,0_m} );
		pTrack->Attach( std::move(pCurve), { 0_m, 20_m } );
		Frame<Length,One> startFrame, midFrame, endFrame, testFrame;
		startFrame.Init();
		startFrame.RotateTan( 90_deg );
		pTrack->SetFrame( startFrame );

		pTrack->Transition( 0_m, startFrame );
		pTrack->Transition( 10_m, midFrame );
		pTrack->Transition( 20_m, endFrame );

		pTrack->Flip();
		BOOST_CHECK( !pTrack->GetCurve().second.Normal() );

		pTrack->Transition( 0_m, testFrame );
		BOOST_CHECK( testFrame.P.Equals( endFrame.P, epsilon__length ) );
		BOOST_CHECK( testFrame.T.Equals( -endFrame.T, epsilon__angle ) );
		BOOST_CHECK( testFrame.N.Equals( -endFrame.N, epsilon__angle ) );
		BOOST_CHECK( testFrame.B.Equals( endFrame.B, epsilon__angle ) );

		pTrack->Transition( 10_m, testFrame );
		BOOST_CHECK( testFrame.P.Equals( midFrame.P, epsilon__length ) );
		BOOST_CHECK( testFrame.T.Equals( -midFrame.T, epsilon__angle ) );
		BOOST_CHECK( testFrame.N.Equals( -midFrame.N, epsilon__angle ) );
		BOOST_CHECK( testFrame.B.Equals( midFrame.B, epsilon__angle ) );

		pTrack->Transition( 20_m, testFrame );
		BOOST_CHECK( testFrame.P.Equals( startFrame.P, epsilon__length ) );
		BOOST_CHECK( testFrame.T.Equals( -startFrame.T, epsilon__angle ) );
		BOOST_CHECK( testFrame.N.Equals( -startFrame.N, epsilon__angle ) );
		BOOST_CHECK( testFrame.B.Equals( startFrame.B, epsilon__angle ) );


		BOOST_CHECK( Normalize( *pTrack ) );
		BOOST_CHECK( pTrack->GetCurve().second.Normal() );

		pTrack->Transition( 0_m, testFrame );
		BOOST_CHECK( testFrame.P.Equals( endFrame.P, epsilon__length ) );
		BOOST_CHECK( testFrame.T.Equals( -endFrame.T, epsilon__angle ) );
		BOOST_CHECK( testFrame.N.Equals( -endFrame.N, epsilon__angle ) );
		BOOST_CHECK( testFrame.B.Equals( endFrame.B, epsilon__angle ) );

		pTrack->Transition( 10_m, testFrame );
		BOOST_CHECK( testFrame.P.Equals( midFrame.P, epsilon__length ) );
		BOOST_CHECK( testFrame.T.Equals( -midFrame.T, epsilon__angle ) );
		BOOST_CHECK( testFrame.N.Equals( -midFrame.N, epsilon__angle ) );
		BOOST_CHECK( testFrame.B.Equals( midFrame.B, epsilon__angle ) );

		pTrack->Transition( 20_m, testFrame );
		BOOST_CHECK( testFrame.P.Equals( startFrame.P, epsilon__length ) );
		BOOST_CHECK( testFrame.T.Equals( -startFrame.T, epsilon__angle ) );
		BOOST_CHECK( testFrame.N.Equals( -startFrame.N, epsilon__angle ) );
		BOOST_CHECK( testFrame.B.Equals( startFrame.B, epsilon__angle ) );
	}
}

BOOST_AUTO_TEST_CASE( tests_trax_NormalizeArcRotatedFlippedWithFrozenDirectionalTwist )
{
	{
		auto pTrack = TrackBuilder::Make();
		auto pCurve = ArcP::Make();
		pCurve->Create( {{10_m,0_m,0_m},{0,1,0}}, {-10_m,0_m,0_m} );
		pTrack->Attach( std::move(pCurve), { 0_m, 20_m } );
		pTrack->Attach( DirectionalTwist::Make() );
		pTrack->GetTwist().Freeze();
		Frame<Length,One> startFrame, midFrame, endFrame, testFrame;
		startFrame.Init();
		startFrame.RotateTan( 90_deg );
		pTrack->SetFrame( startFrame );

		pTrack->Transition( 0_m, startFrame );
		pTrack->Transition( 10_m, midFrame );
		pTrack->Transition( 20_m, endFrame );

		pTrack->Flip();
		BOOST_CHECK( !pTrack->GetCurve().second.Normal() );

		pTrack->Transition( 0_m, testFrame );
		BOOST_CHECK( testFrame.P.Equals( endFrame.P, epsilon__length ) );
		BOOST_CHECK( testFrame.T.Equals( -endFrame.T, epsilon__angle ) );
		BOOST_CHECK( testFrame.N.Equals( -endFrame.N, epsilon__angle ) );
		BOOST_CHECK( testFrame.B.Equals( endFrame.B, epsilon__angle ) );

		pTrack->Transition( 10_m, testFrame );
		BOOST_CHECK( testFrame.P.Equals( midFrame.P, epsilon__length ) );
		BOOST_CHECK( testFrame.T.Equals( -midFrame.T, epsilon__angle ) );
		BOOST_CHECK( testFrame.N.Equals( -midFrame.N, epsilon__angle ) );
		BOOST_CHECK( testFrame.B.Equals( midFrame.B, epsilon__angle ) );

		pTrack->Transition( 20_m, testFrame );
		BOOST_CHECK( testFrame.P.Equals( startFrame.P, epsilon__length ) );
		BOOST_CHECK( testFrame.T.Equals( -startFrame.T, epsilon__angle ) );
		BOOST_CHECK( testFrame.N.Equals( -startFrame.N, epsilon__angle ) );
		BOOST_CHECK( testFrame.B.Equals( startFrame.B, epsilon__angle ) );

		BOOST_CHECK( Normalize( *pTrack ) );
		BOOST_CHECK( pTrack->GetCurve().second.Normal() );

		pTrack->Transition( 0_m, testFrame );
		BOOST_CHECK( testFrame.P.Equals( endFrame.P, epsilon__length ) );
		BOOST_CHECK( testFrame.T.Equals( -endFrame.T, epsilon__angle ) );
		BOOST_CHECK( testFrame.N.Equals( -endFrame.N, epsilon__angle ) );
		BOOST_CHECK( testFrame.B.Equals( endFrame.B, epsilon__angle ) );

		pTrack->Transition( 10_m, testFrame );
		BOOST_CHECK( testFrame.P.Equals( midFrame.P, epsilon__length ) );
		BOOST_CHECK( testFrame.T.Equals( -midFrame.T, epsilon__angle ) );
		BOOST_CHECK( testFrame.N.Equals( -midFrame.N, epsilon__angle ) );
		BOOST_CHECK( testFrame.B.Equals( midFrame.B, epsilon__angle ) );

		pTrack->Transition( 20_m, testFrame );
		BOOST_CHECK( testFrame.P.Equals( startFrame.P, epsilon__length ) );
		BOOST_CHECK( testFrame.T.Equals( -startFrame.T, epsilon__angle ) );
		BOOST_CHECK( testFrame.N.Equals( -startFrame.N, epsilon__angle ) );
		BOOST_CHECK( testFrame.B.Equals( startFrame.B, epsilon__angle ) );
	}
}

BOOST_AUTO_TEST_CASE( tests_trax_NormalizeHelix )
{
	{
		auto pTrack = TrackBuilder::Make();
		auto pCurve = HelixP::Make();
		pCurve->Create( {{10_m,0_m,0_m},{0,1,0}}, {-10_m,0_m,10_m} );
		pTrack->Attach( std::move(pCurve), {20_m, 40_m} );
		Frame<Length,One> frame;
		frame.Init();
		frame.P = {1_m,2_m,3_m};
		frame.RotateTan( 10_deg );
		frame.RotateBin( 10_deg );
		pTrack->SetFrame( frame );

		Frame<Length,One> startFrame, midFrame, endFrame, testFrame;
		pTrack->Transition( 0_m, startFrame );
		pTrack->Transition( 10_m, midFrame );
		pTrack->Transition( 20_m, endFrame );

		BOOST_CHECK( Normalize( *pTrack ) );

		pTrack->Transition( 0_m, testFrame );
		BOOST_CHECK( testFrame.Equals( startFrame, epsilon__length, epsilon__angle ) );
		pTrack->Transition( 10_m, testFrame );
		BOOST_CHECK( testFrame.Equals( midFrame, epsilon__length, epsilon__angle ) );
		pTrack->Transition( 20_m, testFrame );
		BOOST_CHECK( testFrame.Equals( endFrame, epsilon__length, epsilon__angle ) );
	}
}

BOOST_AUTO_TEST_CASE( tests_trax_NormalizeHelixFlipped )
{
	{
		auto pTrack = TrackBuilder::Make();
		auto pCurve = HelixP::Make();
		pCurve->Create( {{10_m,0_m,0_m},{0,1,0}}, {-10_m,0_m,10_m} );
		pTrack->Attach( std::move(pCurve), {30_m, 10_m} );

		Frame<Length,One> startFrame, midFrame, endFrame, testFrame;
		pTrack->Transition( 0_m, startFrame );
		pTrack->Transition( 10_m, midFrame );
		pTrack->Transition( 20_m, endFrame );

		BOOST_CHECK( Normalize( *pTrack ) );

		pTrack->Transition( 0_m, testFrame );
		BOOST_CHECK( testFrame.P.Equals( startFrame.P, epsilon__length ) );
		BOOST_CHECK( testFrame.T.Equals( startFrame.T, epsilon__angle ) );
		BOOST_CHECK( testFrame.N.Equals( startFrame.N, epsilon__angle ) );
		BOOST_CHECK( testFrame.B.Equals( startFrame.B, epsilon__angle ) );

		pTrack->Transition( 10_m, testFrame );
		BOOST_CHECK( testFrame.P.Equals( midFrame.P, epsilon__length ) );
		BOOST_CHECK( testFrame.T.Equals( midFrame.T, epsilon__angle ) );
		BOOST_CHECK( testFrame.N.Equals( midFrame.N, epsilon__angle ) );
		BOOST_CHECK( testFrame.B.Equals( midFrame.B, epsilon__angle ) );

		pTrack->Transition( 20_m, testFrame );
		BOOST_CHECK( testFrame.P.Equals( endFrame.P, epsilon__length ) );
		BOOST_CHECK( testFrame.T.Equals( endFrame.T, epsilon__angle ) );
		BOOST_CHECK( testFrame.N.Equals( endFrame.N, epsilon__angle ) );
		BOOST_CHECK( testFrame.B.Equals( endFrame.B, epsilon__angle ) );
	}
}

BOOST_AUTO_TEST_CASE( tests_trax_NormalizeRotator1 )
{
	{
		auto pTrack = TrackBuilder::Make();
		auto pCurve = Rotator::Make();
		pCurve->Create( { 30_deg / 60_m, 0_deg / 60_m, 0_deg, 0_deg } );
		pTrack->Attach( std::move(pCurve), {20_m, 40_m} );

		Frame<Length,One> startFrame, midFrame, endFrame, testFrame;
		pTrack->Transition( 0_m, startFrame );
		pTrack->Transition( 10_m, midFrame );
		pTrack->Transition( 20_m, endFrame );

		BOOST_CHECK( Normalize( *pTrack ) );

		pTrack->Transition( 0_m, testFrame );
		BOOST_CHECK( testFrame.Equals( startFrame, epsilon__length, epsilon__angle ) );
		pTrack->Transition( 10_m, testFrame );
		BOOST_CHECK( testFrame.Equals( midFrame, epsilon__length, epsilon__angle ) );
		pTrack->Transition( 20_m, testFrame );
		BOOST_CHECK( testFrame.Equals( endFrame, epsilon__length, epsilon__angle ) );
	}
}

BOOST_AUTO_TEST_CASE( tests_trax_NormalizeRotator2 )
{
	{
		auto pTrack = TrackBuilder::Make();
		auto pCurve = Rotator::Make();
		pCurve->Create( { 0_deg / 60_m, 20_deg / 60_m, 0_deg, 0_deg } );
		pTrack->Attach( std::move(pCurve), {20_m, 40_m} );

		Frame<Length,One> startFrame, midFrame, endFrame, testFrame;
		pTrack->Transition( 0_m, startFrame );
		pTrack->Transition( 10_m, midFrame );
		pTrack->Transition( 20_m, endFrame );

		BOOST_CHECK( Normalize( *pTrack ) );

		pTrack->Transition( 0_m, testFrame );
		BOOST_CHECK( testFrame.Equals( startFrame, epsilon__length, epsilon__angle ) );
		pTrack->Transition( 10_m, testFrame );
		BOOST_CHECK( testFrame.Equals( midFrame, epsilon__length, epsilon__angle ) );
		pTrack->Transition( 20_m, testFrame );
		BOOST_CHECK( testFrame.Equals( endFrame, epsilon__length, epsilon__angle ) );
	}
}

BOOST_AUTO_TEST_SUITE_END() // tests_trax
BOOST_AUTO_TEST_SUITE_END() // tests_trax_Normalize
#endif // WITH_BOOST_TESTS
//	trax track library
//	AD 2013 
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

#include <boost/test/unit_test.hpp>

#include "trax/Track.h"
#include "trax/ParallelTrack.h"
#include "trax/support/TraxSupportStream.h"
#include "trax/support/Fixtures.h"
#include "../Test/dim/BoostTestDimensionedValuesHelpers.h"
#include "../Test/spat/BoostTestSpatialHelpers.h"

using namespace common;
using namespace trax;
using namespace spat;
using namespace std;


BOOST_AUTO_TEST_SUITE(trax_tests)
BOOST_AUTO_TEST_SUITE(Fixture_tests)
	
BOOST_FIXTURE_TEST_CASE( TraxFixture_test, TrackFixture )
{
	BOOST_CHECK( m_pTrack );
}

BOOST_FIXTURE_TEST_CASE( TrackCircle_test, TrackCircle )
{
	BOOST_CHECK_CLOSE_DIMENSION( m_pTrack1->GetLength(), R*pi/2, 0.001f );
	BOOST_CHECK_CLOSE_DIMENSION( m_pTrack2->GetLength(), R*pi/2, 0.001f );
	BOOST_CHECK_CLOSE_DIMENSION( m_pTrack3->GetLength(), R*pi/2, 0.001f );
	BOOST_CHECK_CLOSE_DIMENSION( m_pTrack4->GetLength(), R*pi/2, 0.001f );

	BOOST_CHECK( m_pTrack1->TransitionEnd( Track::EndType::front ).first == m_pTrack4 );
	BOOST_CHECK( m_pTrack1->TransitionEnd( Track::EndType::front ).second == Track::EndType::end );
	BOOST_CHECK( m_pTrack2->TransitionEnd( Track::EndType::front ).first == m_pTrack1 );
	BOOST_CHECK( m_pTrack2->TransitionEnd( Track::EndType::front ).second == Track::EndType::end );
	BOOST_CHECK( m_pTrack3->TransitionEnd( Track::EndType::front ).first == m_pTrack2 );
	BOOST_CHECK( m_pTrack3->TransitionEnd( Track::EndType::front ).second == Track::EndType::end );
	BOOST_CHECK( m_pTrack4->TransitionEnd( Track::EndType::front ).first == m_pTrack3 );
	BOOST_CHECK( m_pTrack4->TransitionEnd( Track::EndType::front ).second == Track::EndType::end );
}

BOOST_AUTO_TEST_SUITE_END() //Fixture_tests
BOOST_AUTO_TEST_SUITE(Performance_tests)

//BOOST_FIXTURE_TEST_CASE( TrackTransition_test, TrackCircle )
//{
//	Frame<Length,One> frame;
//	for( int i = 0; i < 1000000000; ++i )
//		m_pTrack1->Transition( R*pi/4, frame );
//}

BOOST_AUTO_TEST_SUITE_END() //Performance_tests
BOOST_AUTO_TEST_SUITE(track_creation)

BOOST_AUTO_TEST_CASE( basicTrackTypeTests )
{
	BOOST_CHECK_EQUAL( ToString( Track::EndType::front ), "front" );
	BOOST_CHECK_EQUAL( ToString( Track::EndType::end ), "end" );
}

BOOST_AUTO_TEST_CASE( take_track_length_from_curve )
{
	auto pBuildTrack = TrackBuilder::Make();
	std::shared_ptr<trax::LineP> pCurve = LineP::Make();
	pCurve->Create( {Origin3D<Length>,{1,0,0}}, trax::Up );
	pBuildTrack->Attach(pCurve,{0_m,10_m});

	BOOST_CHECK_EQUAL( pBuildTrack->GetLength(), 10_m );
}

BOOST_AUTO_TEST_CASE( trackSetStartAccuracy )
{
	auto pBuildTrack = TrackBuilder::Make();
	auto pLine = LineP::Make();

	Frame<Length,One> startFrame{ {1_m,2_m,3_m}, {1,1,1}, {-1,1,0}, {0,0,1} }, testStartFrame;
	startFrame.OrthoNormalize();

	pLine->Create( startFrame.P, startFrame.T );
	pBuildTrack->Attach( std::move(pLine) );

	for( int i = 0; i < 10; ++i ){
		pBuildTrack->Transition( 0_m, testStartFrame );
		BOOST_CHECK( testStartFrame.IsOrthoNormal() );
		pBuildTrack->SetFrame( testStartFrame, 0_m );
		BOOST_CHECK( startFrame.Equals( testStartFrame, epsilon__length, epsilon__angle ) );
	}
}

BOOST_AUTO_TEST_CASE( trackSetStartAccuracyRotatorCurve1 )
{
	{
		auto pTrack1 = TrackBuilder::Make();
		auto pCurve = Rotator::Make();
		pCurve->Create( Rotator::Data{ 20_deg / 90_m, 10_deg / 90_m} );
		pTrack1->Attach( std::move(pCurve), {0_m,90_m} );
		pTrack1->Attach( DirectionalTwist::Make() );

		Frame<Length,One> endFrame1;
		pTrack1->Transition( pTrack1->GetLength(), endFrame1 );
		BOOST_CHECK_SMALL( Angle{std::asin(endFrame1.T.dz)} - 10_deg, 0.0001_deg );
		BOOST_CHECK_SMALL( Angle{std::atan2(endFrame1.T.dy,endFrame1.T.dx)} - 20_deg, 0.0001_deg );

		auto pTrack2 = TrackBuilder::Make();
		pCurve = Rotator::Make( Curve::CurveType::RotatorWithOffset );
		pCurve->Create( Rotator::Data{ 20_deg / 60_m, -10_deg / 60_m, 20_deg, 10_deg } );
		pTrack2->Attach( std::move(pCurve), {0_m,60_m} );
		pTrack2->Attach( DirectionalTwist::Make() );

		pTrack2->SetFrame( endFrame1, 0_m );
		Frame<Length,One> startFrame2;
		pTrack2->Transition( 0_m, startFrame2 );
		BOOST_CHECK( startFrame2.Equals( endFrame1, epsilon__length, epsilon__angle ) );

		Vector<One> endTangent2;
		pTrack2->Transition( pTrack2->GetLength(), endTangent2 );

		BOOST_CHECK_SMALL( Angle{std::asin(endTangent2.dz)}, 0.1_deg );
		BOOST_CHECK_SMALL( Angle{std::atan2(endTangent2.dy,endTangent2.dx)} - 40_deg, 0.1_deg );
	}
}

BOOST_AUTO_TEST_CASE( trackSetStartAccuracyRotatorChain0 )
{
	{
		auto pTrack = TrackBuilder::Make();
		auto pCurve = RotatorChain::Make();

		pCurve->Create( RotatorChain::Data{ 
			{45_deg, 10_deg, 90_m } } );
		pTrack->Attach( std::move(pCurve), {0_m,90_m} );

		Frame<Real> startFrame;
		startFrame.Init();
		//pTrack->SetStart( startFrame );
		Vector<One> endTangent;
		pTrack->Transition( pTrack->GetLength(), endTangent );

		Vector<One> shouldBe{ cos(45_deg)*cos(10_deg), sin(45_deg)*cos(10_deg), sin(10_deg) };

		BOOST_CHECK( endTangent.Equals( shouldBe, epsilon__angle ) );
	}
}

BOOST_AUTO_TEST_CASE( trackSetStartAccuracyRotatorChain1 )
{
	{
		auto pTrack = TrackBuilder::Make();
		auto pCurve = RotatorChain::Make();

		pCurve->Create( RotatorChain::Data{ 
			{45_deg, 0_deg, 90_m }, 
			{45_deg, 0_deg, 90_m }, 
			{45_deg, 0_deg, 90_m }, 
			{45_deg, 0_deg, 90_m },
			{45_deg, 0_deg, 90_m }, 
			{45_deg, 0_deg, 90_m }, 
			{45_deg, 0_deg, 90_m }, 
			{45_deg, 0_deg, 90_m } } );
		pTrack->Attach( std::move(pCurve), {0_m,8*90_m} );

		Frame<Length,One> startFrame;
		startFrame.Init();
		pTrack->SetFrame( startFrame, 0_m );
		Position<Length> endPosition;
		pTrack->Transition( pTrack->GetLength(), endPosition );
		BOOST_CHECK( startFrame.P.Equals( endPosition, epsilon__length ) );
	}
}

BOOST_AUTO_TEST_CASE( trackSetStartAccuracyRotatorChain2 )
{
	{
		auto pTrack = TrackBuilder::Make();
		auto pCurve = RotatorChain::Make();

		pCurve->Create( RotatorChain::Data{ 
			{0_deg, 45_deg, 90_m }, 
			{0_deg, 45_deg, 90_m }, 
			{0_deg, 45_deg, 90_m }, 
			{0_deg, 45_deg, 90_m },
			{0_deg, 45_deg, 90_m }, 
			{0_deg, 45_deg, 90_m }, 
			{0_deg, 45_deg, 90_m }, 
			{0_deg, 45_deg, 90_m } } );
		pTrack->Attach( std::move(pCurve), {0_m,8*90_m} );

		Frame<Length,One> startFrame;
		startFrame.Init();
		pTrack->SetFrame( startFrame, 0_m );
		Position<Length> endPosition;
		pTrack->Transition( pTrack->GetLength(), endPosition );
		BOOST_CHECK( startFrame.P.Equals( endPosition, epsilon__length ) );
	}
}

BOOST_AUTO_TEST_CASE( trackSetStartAccuracyRotatorChain3 )
{
	{
		auto pTrack = TrackBuilder::Make();
		auto pCurve = RotatorChain::Make();

		pCurve->Create( RotatorChain::Data{ 
			{20_deg, 10_deg, 90_m }, 
			{20_deg, -10_deg, 60_m }, 
			{-40_deg, 0_deg, 50_m } } );
		pTrack->Attach( std::move(pCurve), {0_m,90_m+60_m+50_m} );

		Frame<Length,One> startFrame;
		startFrame.Init();
		pTrack->SetFrame( startFrame, 0_m );
		Vector<One> endTangent;
		pTrack->Transition( pTrack->GetLength(), endTangent );
		BOOST_CHECK( startFrame.T.Equals( endTangent, epsilon__angle ) );

		startFrame.RotateBin( pi/4 );
		startFrame.RotateNor( -pi/8 );
		pTrack->SetFrame( startFrame, 0_m );
		pTrack->Transition( pTrack->GetLength(), endTangent );
		BOOST_CHECK( startFrame.T.Equals( endTangent, epsilon__angle ) );
	}
}

BOOST_AUTO_TEST_CASE( trackSetStartWithDirectionalTwistExpectedBehaviour )
{
	{
		auto pTrack = TrackBuilder::Make();
		auto pCurve = ArcP::Make();

		pCurve->Create( { { 0_m, 0_m, 0_m }, { 1, 0, 0 }, {0, 1, 0 } }, 45_deg / 60_m );
		pTrack->Attach( std::move(pCurve), {0_m,60_m} );

		auto pCombined = CombinedTwist::Make();
		pCombined->AttachTwist1( DirectionalTwist::Make() );	
		pCombined->AttachTwist2( LinearTwist::Make(-pi/2,-pi/2));
		pTrack->Attach( std::move(pCombined) );

		Frame<Length,One> startFrame;
		startFrame.Init();
		pTrack->SetFrame( startFrame, 0_m );

		VectorBundle<Length,One> endTangent;
		pTrack->Transition( pTrack->GetLength(), endTangent );

		BOOST_CHECK( endTangent.T.Equals( { cos(45_deg), 0, sin(45_deg) }, epsilon__angle ) );
		BOOST_CHECK_SMALL( endTangent.P.y, epsilon__length );
	}
}

BOOST_AUTO_TEST_CASE( trackSetStartWithDirectionalTwistExpectedBehaviour2 )
{
	{
		auto pTrack = TrackBuilder::Make();
		auto pCurve = ArcP::Make();

		pCurve->Create( { { 0_m, 0_m, 0_m }, { 1, 0, 0 }, {0, 1, 0 } }, 45_deg / 60_m );
		pTrack->Attach( std::move(pCurve), {0_m,60_m} );

		auto pCombined = CombinedTwist::Make();
		pCombined->AttachTwist1( DirectionalTwist::Make());	
		pCombined->AttachTwist2( LinearTwist::Make(-pi/2,-pi/2));
		pTrack->Attach( std::move(pCombined) );

		Frame<Length,One> startFrame;
		pTrack->Transition( 0_m, startFrame );
		startFrame.RotateTan( 1_deg );
		pTrack->SetFrame( startFrame, 0_m );

		VectorBundle<Length,One> endTangent;
		pTrack->Transition( pTrack->GetLength(), endTangent );

		BOOST_CHECK_GT( endTangent.P.z, epsilon__length );
	}
}

BOOST_FIXTURE_TEST_CASE( splitTrack, TrackFixture )
{
	//Add some EEP like twist:
	auto pCombinedTwist = CombinedTwist::Make();
	auto pDirectionalTwist = DirectionalTwist::Make();
	auto pLinearTwist = LinearTwist::Make( 10_deg, 30_deg );
	pCombinedTwist->AttachTwist1( std::move(pDirectionalTwist) );
	pCombinedTwist->AttachTwist2( std::move(pLinearTwist) );

	pCombinedTwist->Freeze();
	m_pTrack->Attach( std::move(pCombinedTwist) );

	{
		Length parameterToSplit = 50_m;
		Angle twistAtSplitPoint = 10_deg + (30_deg - 10_deg) / m_pTrack->GetLength() * parameterToSplit;

		// record test data:
		Frame<Length,One> testFrame1, testFrame2, testFrame3;
		m_pTrack->Transition( 0_m, testFrame1 );
		m_pTrack->Transition( parameterToSplit, testFrame2 );
		m_pTrack->Transition( m_pTrack->GetLength(), testFrame3 );


		// clone original track:
		auto pNewTrack = TrackBuilder::Make();
		pNewTrack->SetFrame( m_pTrack->GetFrame() );
		pNewTrack->Attach( m_pTrack->GetCurve() );
		pNewTrack->Attach( m_pTrack->GetTwist().Clone() );

		// new track becomes second part:
		auto curve = pNewTrack->GetCurve();
		curve.second.Near( curve.second.Near() + parameterToSplit );
		pNewTrack->Attach( curve );
		if( LinearTwist* pTwist = FindTwist<LinearTwist>(pNewTrack->GetTwist()) )
			pTwist->From( twistAtSplitPoint );

		// original track becomes first part:
		curve = m_pTrack->GetCurve();
		curve.second.Far( curve.second.Near() + parameterToSplit );
		m_pTrack->Attach( curve );
		if( LinearTwist* pTwist = FindTwist<LinearTwist>(m_pTrack->GetTwist()) )
			pTwist->To( twistAtSplitPoint );

		// make parameter start with 0_m:
		BOOST_CHECK( Normalize( *pNewTrack ) );
		BOOST_CHECK_EQUAL( pNewTrack->GetCurve().second.Near(), 0_m );


		// test geometry:
		Frame<Length,One> testFrame;
		m_pTrack->Transition( 0_m, testFrame );
		BOOST_CHECK( testFrame.Equals( testFrame1, epsilon__length, epsilon__angle ) );

		m_pTrack->Transition( m_pTrack->GetLength(), testFrame );
		BOOST_CHECK( testFrame.Equals( testFrame2, epsilon__length, epsilon__angle ) );

		pNewTrack->Transition( 0_m, testFrame );
		BOOST_CHECK( testFrame.Equals( testFrame2, epsilon__length, epsilon__angle ) );

		pNewTrack->Transition( pNewTrack->GetLength(), testFrame );
		BOOST_CHECK( testFrame.Equals( testFrame3, epsilon__length, epsilon__angle ) );
	}
}

BOOST_FIXTURE_TEST_CASE( splitTrackByDedicatedMethod, TrackFixture )
{
	//Add some EEP like twist:
	auto pCombinedTwist = CombinedTwist::Make();
	auto pDirectionalTwist = DirectionalTwist::Make();
	auto pLinearTwist = LinearTwist::Make( 10_deg, 30_deg );
	pCombinedTwist->AttachTwist1( std::move(pDirectionalTwist) );
	pCombinedTwist->AttachTwist2( std::move(pLinearTwist) );

	m_pTrack->Attach( std::move(pCombinedTwist) );

	{
		constexpr size_t numPieces = 7;


		// get the track pieces
		std::vector<std::shared_ptr<TrackBuilder>> list = SplitTrack( *m_pTrack, numPieces );
		BOOST_CHECK_EQUAL( list.size(), numPieces );

		// test geometry:
		Length parameter = 0_m;
		Length sampleDistance = m_pTrack->GetLength() / numPieces;
		for( size_t i = 0; i < list.size(); ++i ){
			Frame<Length,One> testFrame1;
			m_pTrack->Transition( parameter, testFrame1 );
			Frame<Length,One> testFrame2;
			list[i]->Transition( 0_m, testFrame2 );

			BOOST_CHECK( testFrame1.Equals( testFrame2, epsilon__length, epsilon__angle ) );
			parameter += sampleDistance;
		}


		//try with non zero start:
		auto curve = m_pTrack->GetCurve();
		curve.second.Near( 20_m );
		m_pTrack->Attach( curve );
		list = SplitTrack( *m_pTrack, numPieces );
		BOOST_CHECK_EQUAL( list.size(), numPieces );

		// test geometry:
		parameter = 0_m;
		sampleDistance = m_pTrack->GetLength() / numPieces;
		for( size_t i = 0; i < list.size(); ++i ){
			Frame<Length,One> testFrame1;
			m_pTrack->Transition( parameter, testFrame1 );
			Frame<Length,One> testFrame2;
			list[i]->Transition( 0_m, testFrame2 );

			BOOST_CHECK( testFrame1.Equals( testFrame2, epsilon__length, epsilon__angle ) );
			parameter += sampleDistance;
		}


		// try flipped track:
		m_pTrack->Flip(); 
		list = SplitTrack( *m_pTrack, numPieces );
		BOOST_CHECK_EQUAL( list.size(), numPieces );

		// test geometry:
		parameter = 0_m;
		sampleDistance = m_pTrack->GetLength() / numPieces;
		for( size_t i = 0; i < list.size(); ++i ){
			Frame<Length,One> testFrame1;
			m_pTrack->Transition( parameter, testFrame1 );
			Frame<Length,One> testFrame2;
			list[i]->Transition( 0_m, testFrame2 );

			BOOST_CHECK( testFrame1.Equals( testFrame2, epsilon__length, epsilon__angle ) );
			parameter += sampleDistance;
		}
	}
}

BOOST_AUTO_TEST_CASE( getClosestPointOnHelix )
{
	{
		auto pTrack = TrackBuilder::Make();
		auto pCurve = HelixP::Make();

		pCurve->Create( HelixP::Data{} );
		pTrack->Attach( std::move(pCurve), {0_m,60_m} );

		Frame<Length,One> A;
		pTrack->Transition( 55_m, A );
		A.TransportBin( 1_m );

		Length s = Closest( A.P, *pTrack );
		BOOST_CHECK_CLOSE_DIMENSION( s, 55_m, 1 );

		Frame<Length,One> frame;
		frame.Init();
		frame.P += Vector<Length>{ -93_cm, -1_cm, 20_cm };
		pTrack->SetFrame( frame );

		A.Init();
		A.P = { -222_cm, -987_cm, 457_cm };
		s = Closest( A.P, *pTrack );

		BOOST_CHECK_CLOSE_DIMENSION( s, 46_m, 1 );
	}
}

BOOST_AUTO_TEST_CASE( MakeParallelTrackWithPolygonalChainTest )
{
	Frame<Length,One> frame{
		{ 610_m, 1050.32_m, 31.53_m },
		{ 4.05444131e-39, -9.39521519e-38, 0.999999821 },
		{ -0.0431128815, 0.999070227, 9.40396153e-38 },
		{ -0.999070227, -0.0431128591, 1.23314265e-43 }	
	};

	VectorBundle<Length, One> bundle{
		{ 0_m, 0_m, 0_m },
		{ 1, 0, 0 }
	};

	Vector<One> up{ 0, 0, 1 };

	auto pLine = LineP::Make();
	pLine->Create( LineP::Data{ bundle, up } );

	std::unique_ptr<CombinedTwist> pTwist = CombinedTwist::Make();
	pTwist->AttachTwist1( LinearTwist::Make( -1.52306302e-06, -1.52306302e-06 ) );
	//pTwist->AttachTwist2( DirectionalTwist::Make( { 1.0, 9.40395593e-38, -1.42932443e-43 } ) );

	pTwist->AttachTwist2( DirectionalTwist::Make( {
		1.0,
		common::DealDenormalizedNumbers(One{9.40395593e-38}),
		common::DealDenormalizedNumbers(One{-1.42932443e-43}) }) );
	pTwist->Freeze();

	auto pTrack = TrackBuilder::Make();
	pTrack->SetFrame( frame );
	pTrack->Attach( std::move( pLine ), { 0_m, 2.50_m } );
	pTrack->Attach( std::move( pTwist ) );

	std::shared_ptr<TrackBuilder> pNewTrack = MakeParallelTrackWithPolygonalChain( *pTrack, { 0_m, 2.50_m } );

	BOOST_CHECK( pNewTrack );
	BOOST_CHECK( pNewTrack->IsValid() );
}

BOOST_AUTO_TEST_SUITE_END() //track_creation
BOOST_AUTO_TEST_SUITE(RoadwayTwist_tests)

BOOST_AUTO_TEST_CASE( totalGeometryTest )
{
	auto pBuildTrack = TrackBuilder::Make();
	auto pCubic = Cubic::Make();
	common::Interval<Length> length = pCubic->Create( {{10_m,10_m,10_m},{150_m,0_m,0_m}}, {{10_m,70_m,20_m},{150_m,0_m,0_m}} );

	Frame<Length,One> frame = Identity<Length,One>;
	frame.RotateTan( 45_deg );
	pBuildTrack->SetFrame(frame);

	pBuildTrack->Attach( std::move(pCubic), {length.Near(),length.Far()} );
	auto pTwist = DirectionalTwist::Make();
	pTwist->Freeze();
	pBuildTrack->Attach( std::move(pTwist) );

	Frame<Length,One> restoreFrame = pBuildTrack->GetFrame();
	auto retsoreCurve = pBuildTrack->GetCurve();
	auto restoreTwist = pBuildTrack->GetTwist().Clone();

	auto pBuildTrack2 = TrackBuilder::Make();
	pBuildTrack2->SetFrame( restoreFrame );
	pBuildTrack2->Attach( retsoreCurve );
	pBuildTrack2->Attach( std::move(restoreTwist) );

	Frame<Length,One> frame1, frame2;
	pBuildTrack->Transition( 0_m, frame1 );
	pBuildTrack2->Transition( 0_m, frame2 );
	BOOST_CHECK_CLOSE_SPATIAL2( frame1, frame2, epsilon__length, epsilon__angle );
	pBuildTrack->Transition( length.Length(), frame1 );
	pBuildTrack2->Transition( length.Length(), frame2 );
	BOOST_CHECK_CLOSE_SPATIAL2( frame1, frame2, epsilon__length, epsilon__angle );
}

BOOST_AUTO_TEST_CASE( twistRelatesToTrackParameter )
{
	auto pBuildTrack = TrackBuilder::Make();
	auto pLine = LineP::Make();
	pLine->Create( VectorBundle<Length,One>{{10_m,10_m,10_m},{1,0,0}} );
	pBuildTrack->Attach( std::move(pLine), {10_m,20_m} );
	pBuildTrack->Attach( LinearTwist::Make( 0_deg, 90_deg ) );

	Frame<Length,One> frame;

	pBuildTrack->Transition( 0_m, frame );
	Angle alpha = acos(Up*frame.B);
	BOOST_CHECK_EQUAL( alpha, 0_deg );
	BOOST_CHECK_GT( Up*frame.B, 0 );
	BOOST_CHECK_GT( Ey<One>*frame.N, 0 );
	pBuildTrack->Transition( 10_m, frame );
	alpha = acos(Up*frame.B);
	BOOST_CHECK_EQUAL( alpha, 90_deg );
	BOOST_CHECK_GT( Up*frame.N, 0 );
	BOOST_CHECK_LT( Ey<One>*frame.B, 0 );

	pBuildTrack->Flip();

	pBuildTrack->Transition( 0_m, frame );
	alpha = acos(Up*frame.B);
	BOOST_CHECK_CLOSE_DIMENSION( alpha, 90_deg, 0.1 );
	BOOST_CHECK_LT( Up*frame.N, 0 );
	BOOST_CHECK_LT( Ey<One>*frame.B, 0 );
	pBuildTrack->Transition( 10_m, frame );
	alpha = acos(Up*frame.B);
	BOOST_CHECK_CLOSE_DIMENSION( alpha, 0_deg, 0.1 );
	BOOST_CHECK_GT( Up*frame.B, 0 );
	BOOST_CHECK_LT( Ey<One>*frame.N, 0 );

	pBuildTrack->Flip();
	auto curve = pBuildTrack->DetachCurve();
	curve.second.Flip();
	pBuildTrack->Attach( curve );

	pBuildTrack->Transition( 0_m, frame );
	alpha = acos(Up*frame.B);
	BOOST_CHECK_CLOSE_DIMENSION( alpha, 180_deg, 0.1 );
	BOOST_CHECK_LT( Up*frame.B, 0 );
	BOOST_CHECK_GT( Ey<One>*frame.N, 0 );
	pBuildTrack->Transition( 10_m, frame );
	alpha = acos(Up*frame.B);
	BOOST_CHECK_CLOSE_DIMENSION( alpha, 90_deg, 0.1 );
	BOOST_CHECK_LT( Up*frame.N, 0 );
	BOOST_CHECK_LT( Ey<One>*frame.B, 0 );

	pBuildTrack->Flip();

	pBuildTrack->Transition( 0_m, frame );
	alpha = acos(Up*frame.B);
	BOOST_CHECK_CLOSE_DIMENSION( alpha, 90_deg, 0.1 );
	BOOST_CHECK_GT( Up*frame.N, 0 );
	BOOST_CHECK_LT( Ey<One>*frame.B, 0 );
	pBuildTrack->Transition( 10_m, frame );
	alpha = acos(Up*frame.B);
	BOOST_CHECK_EQUAL( alpha, 180_deg );
	BOOST_CHECK_LT( Up*frame.B, 0 );
	BOOST_CHECK_LT( Ey<One>*frame.N, 0 );
}

BOOST_AUTO_TEST_CASE( directionalTwistAlwaysBringsUp )
{
	auto pBuildTrack = TrackBuilder::Make();
	auto pLine = LineP::Make();
	pLine->Create( VectorBundle<Length,One>{{10_m,10_m,10_m},{1,0,0}} );
	pBuildTrack->Attach( std::move(pLine), {10_m,20_m} );
	pBuildTrack->Attach( DirectionalTwist::Make() );

	Frame<Length,One> frame;

	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );
	pBuildTrack->Transition( 10_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );

	pBuildTrack->Flip();

	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );
	pBuildTrack->Transition( 10_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );

	pBuildTrack->Flip();
	auto curve = pBuildTrack->DetachCurve();
	curve.second.Flip();
	pBuildTrack->Attach( curve );

	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );
	pBuildTrack->Transition( 10_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );

	pBuildTrack->Flip();

	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );
	pBuildTrack->Transition( 10_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );
}

BOOST_AUTO_TEST_CASE( directionalTwistWorksWithParameterRanges )
{
	auto pBuildTrack = TrackBuilder::Make();
	auto pCubic = Cubic::Make();
	common::Interval<Length> length = pCubic->Create( {{10_m,10_m,10_m},{150_m,0_m,0_m}}, {{10_m,70_m,20_m},{150_m,0_m,0_m}} );

	BOOST_CHECK_EQUAL( pCubic->ZeroSet().size(), 1u );
	length /= 2;
	length.Move( length.Length() );
	pBuildTrack->Attach( std::move(pCubic), length );
	pBuildTrack->Attach( DirectionalTwist::Make() );

	Frame<Length,One> frame;

	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );
	pBuildTrack->Transition( length.Length()/2, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );
	pBuildTrack->Transition( length.Length(), frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );

	pBuildTrack->Flip();

	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );
	pBuildTrack->Transition( length.Length()/2, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );
	pBuildTrack->Transition( length.Length(), frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );

	pBuildTrack->Flip();
	auto curve = pBuildTrack->DetachCurve();
	curve.second.Flip();
	pBuildTrack->Attach( curve );

	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );
	pBuildTrack->Transition( length.Length()/4, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );
	pBuildTrack->Transition( length.Length(), frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );
	
	pBuildTrack->Flip();

	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );
	pBuildTrack->Transition( length.Length()/2, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );
	pBuildTrack->Transition( length.Length(), frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( Up*frame.B, 0 );
}

BOOST_AUTO_TEST_CASE( directionalTwist )
{
	auto pBuildTrack = TrackBuilder::Make();
	auto pArc = ArcP::Make();
	pArc->Create( {10_m,10_m,10_m}, {1,0,0}, {0,1,0}, 1.f/10_m );
	pBuildTrack->Attach( std::move(pArc), {0_m,pi/4 * 10_m} );

	pBuildTrack->Attach( DirectionalTwist::Make() );

	Frame<Length,One> frame;
	pBuildTrack->Transition( pBuildTrack->GetLength(), frame );
	BOOST_CHECK( frame.B.Equals( Up, epsilon__angle ) );

	frame = pBuildTrack->GetFrame();
	frame.RotateTan( pi/8 );
	pBuildTrack->SetFrame( frame );

	pBuildTrack->Transition( pBuildTrack->GetLength(), frame );
	BOOST_CHECK_SMALL( frame.N * Up, One{std::cos(epsilon__angle)} );
}

BOOST_AUTO_TEST_CASE( flippedTrackWithCombinedLinearTwists )
{
	auto pBuildTrack = TrackBuilder::Make();
	auto pLine = LineP::Make();
	pLine->Create( VectorBundle<Length,One>{{10_m,10_m,10_m},{1,0,0}} );
	pBuildTrack->Attach( std::move(pLine), {0_m,10_m} );
	auto pCombined = CombinedTwist::Make();
	pCombined->AttachTwist1( LinearTwist::Make(90_deg,0_deg) );
	pCombined->AttachTwist2( LinearTwist::Make(-90_deg,0_deg) );
	pBuildTrack->Attach( std::move(pCombined) );

	Frame<Length,One> frame;
	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	pBuildTrack->Transition( 5_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	pBuildTrack->Transition( 10_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );

	pBuildTrack->Flip();

	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	pBuildTrack->Transition( 5_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	pBuildTrack->Transition( 10_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
}

BOOST_AUTO_TEST_CASE( flippedTrackWithCombinedUpAndLinearTwist )
{
	auto pBuildTrack = TrackBuilder::Make();
	auto pLine = LineP::Make();
	pLine->Create( VectorBundle<Length,One>{{10_m,10_m,10_m},{1,0,0}} );
	pBuildTrack->Attach( std::move(pLine), {0_m,10_m} );
	auto pCombined = CombinedTwist::Make();
	pCombined->AttachTwist1( DirectionalTwist::Make() );
	pCombined->AttachTwist2( LinearTwist::Make(0,0) );
	pBuildTrack->Attach( std::move(pCombined) );

	Frame<Length,One> frame;
	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	pBuildTrack->Transition( 5_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	pBuildTrack->Transition( 10_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );

	pBuildTrack->Flip();

	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	pBuildTrack->Transition( 5_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	pBuildTrack->Transition( 10_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
}

BOOST_AUTO_TEST_CASE( flippedTrackWithCombinedUpAndLinearTwist2 )
{
	auto pBuildTrack = TrackBuilder::Make();
	auto pCubic = Cubic::Make();
	common::Interval<Length> length = pCubic->Create( {{10_m,10_m,10_m},{150_m,0_m,0_m}}, {{70_m,10_m,20_m},{150_m,0_m,0_m}} );
	pBuildTrack->Attach( std::move(pCubic), length );
	auto pCombined = CombinedTwist::Make();
	pCombined->AttachTwist1( DirectionalTwist::Make() );
	pCombined->AttachTwist2( LinearTwist::Make(0,0) );
	pCombined->ZeroFlip(false);
	pBuildTrack->Attach( std::move(pCombined) );

	Frame<Length,One> frame;
	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	pBuildTrack->Transition( length.Length()/2, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	pBuildTrack->Transition( length.Length(), frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );

	pBuildTrack->Flip();

	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	pBuildTrack->Transition( length.Length()/2, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	pBuildTrack->Transition( length.Length(), frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
}

BOOST_AUTO_TEST_CASE( flippedTrackWithCombinedUpAndLinearTwist3 )
{
	auto pBuildTrack = TrackBuilder::Make();
	auto pLine = LineP::Make();
	pLine->Create( VectorBundle<Length,One>{{10_m,10_m,10_m},{1,0,0}} );
	pBuildTrack->Attach( std::move(pLine), {0_m,10_m} );
	auto pCombined = CombinedTwist::Make();
	pCombined->AttachTwist1( DirectionalTwist::Make() );
	auto pLinearTwist = LinearTwist::Make(0_deg,90_deg);
	LinearTwist& linearTwist = *pLinearTwist;
	pCombined->AttachTwist2( std::move(pLinearTwist) );
	pBuildTrack->Attach( std::move(pCombined) );

	Frame<Length,One> frame;
	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	pBuildTrack->Transition( 10_m, frame );
	BOOST_CHECK_SMALL( frame.B * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.N * Up, 0 );

	pBuildTrack->Flip();

	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.B * Up, epsilon__angle );
	BOOST_CHECK_LT( frame.N * Up, 0 );
	BOOST_CHECK_CLOSE_DIMENSION( linearTwist.Twist( 0_m ), -90_deg, 0.1 ); 
	BOOST_CHECK_CLOSE_DIMENSION( linearTwist.From(), -90_deg, 0.1 ); 
	pBuildTrack->Transition( 10_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	BOOST_CHECK_SMALL( linearTwist.Twist( 10_m ), epsilon__angle );
	BOOST_CHECK_SMALL( linearTwist.To(), epsilon__angle );

	linearTwist.From( 0_deg );
	linearTwist.To( 90_deg );
	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	pBuildTrack->Transition( 10_m, frame );
	BOOST_CHECK_SMALL( frame.B * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.N * Up, 0 );
}

BOOST_AUTO_TEST_CASE( flippedTrackSameAsAddingFlippedTwist )
{
	auto pBuildTrack = TrackBuilder::Make();
	std::shared_ptr<LineP> pLine = LineP::Make();
	pLine->Create( VectorBundle<Length,One>{{10_m,10_m,10_m},{1,0,0}} );
	pBuildTrack->Attach( pLine, {0_m,10_m} );
	auto pCombined = CombinedTwist::Make();
	pCombined->AttachTwist1( LinearTwist::Make(0_deg,90_deg) );
	pCombined->AttachTwist2( LinearTwist::Make(0_deg,90_deg) );
	pBuildTrack->Attach( std::move(pCombined) );
	pBuildTrack->Flip();

	auto pBuildTrack2 = TrackBuilder::Make();
	pBuildTrack2->Attach( pLine, {10_m,0_m} );
	auto pCombined2 = CombinedTwist::Make();
	pCombined2->AttachTwist1( LinearTwist::Make(0_deg,90_deg) );
	pCombined2->AttachTwist2( LinearTwist::Make(0_deg,90_deg) );
	pCombined2->Flip();
	pBuildTrack2->Attach( std::move(pCombined2) );

	Frame<Length,One> frame1, frame2;
	pBuildTrack->Transition( 0_m, frame1 );
	pBuildTrack2->Transition( 0_m, frame2 );
	BOOST_CHECK_EQUAL( frame1, frame2 );

	pBuildTrack->Transition( 10_m, frame1 );
	pBuildTrack2->Transition( 10_m, frame2 );
	BOOST_CHECK_EQUAL( frame1, frame2 );
}

BOOST_AUTO_TEST_CASE( flippedTrackWithCombinedUpTwists )
{
	auto pBuildTrack = TrackBuilder::Make();
	auto pCubic = Cubic::Make();
	common::Interval<Length> length = pCubic->Create( {{10_m,10_m,10_m},{150_m,0_m,0_m}}, {{70_m,10_m,20_m},{150_m,0_m,0_m}} );
	pBuildTrack->Attach( std::move(pCubic), length );
	auto pCombined = CombinedTwist::Make();
	pCombined->AttachTwist1( DirectionalTwist::Make() );
	BOOST_CHECK_THROW( pCombined->AttachTwist2( DirectionalTwist::Make() ), std::invalid_argument );
	pBuildTrack->Attach( std::move(pCombined) );

	Frame<Length,One> frame;
	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	pBuildTrack->Transition( length.Length()/2, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	pBuildTrack->Transition( length.Length(), frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );

	pBuildTrack->Flip();

	pBuildTrack->Transition( 0_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	pBuildTrack->Transition( length.Length()/2, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
	pBuildTrack->Transition( length.Length(), frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
}

BOOST_AUTO_TEST_CASE( twistDerivativeAtTrackEnds )
{
	auto pBuildTrack = TrackBuilder::Make();
	auto pCubic = Cubic::Make();
	common::Interval<Length> length = pCubic->Create( {
		{8.90087700_m, -35.9992676_m, 4.30014849_m }, 
		{32.7477417_m, 27.5992641_m, 5.54985237_m },
		{0.000188340622_m, -0.000736197224_m, -0.000148288440_m },
		{2.35119486_m, -9.19926357_m, -1.84985185_m } } );

	pBuildTrack->Attach( std::move(pCubic), length );
	pBuildTrack->Attach( DirectionalTwist::Make() );

	Length ds = epsilon__length / 10000;
	AnglePerLength dwIds1 = (pBuildTrack->GetTwist().Twist( 0_m + ds ) - pBuildTrack->GetTwist().Twist( 0_m )) / ds;
	AnglePerLength dwIds2 = pBuildTrack->GetTwist().D1( 0_m );

	auto k = pBuildTrack->Curvature( 0_m );
	auto set = pBuildTrack->GetCurve().first->ZeroSet();

//	BOOST_CHECK_CLOSE_DIMENSION( dwIds1, dwIds2, 5 );
}

BOOST_AUTO_TEST_CASE( TwistMirror1 )
{
	auto pTrack = TrackBuilder::Make();

	std::shared_ptr<LineP> pLine = LineP::Make();
	pLine->Create( VectorBundle<Length,One>{{10_m,10_m,10_m},{1,0,0}} );
	pTrack->Attach( pLine, {0_m,10_m} );

	Frame<Length,One> frame;
	pTrack->Transition( 5_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );

	BOOST_CHECK( Mirror( *pTrack, { {0_m, 0_m, 0_m }, Ey<One> } ) );

	pTrack->Transition( 5_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
}

BOOST_AUTO_TEST_CASE( TwistMirror2 )
{
	auto pTrack = TrackBuilder::Make();

	std::shared_ptr<LineP> pLine = LineP::Make();
	pLine->Create( VectorBundle<Length,One>{{10_m,10_m,10_m},{1,0,0}} );
	pTrack->Attach( pLine, {0_m,10_m} );
	pTrack->Attach( ConstantTwist::Make() );

	Frame<Length,One> frame;
	pTrack->Transition( 5_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );

	BOOST_CHECK( Mirror( *pTrack, { {0_m, 0_m, 0_m }, Ey<One> } ) );

	pTrack->Transition( 5_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
}

BOOST_AUTO_TEST_CASE( flippedConstantTwistMirror )
{
	auto pTrack = TrackBuilder::Make();

	std::shared_ptr<LineP> pLine = LineP::Make();
	pLine->Create( VectorBundle<Length,One>{{10_m,10_m,10_m},{1,0,0}} );
	pTrack->Attach( pLine, {0_m,10_m} );
	pTrack->Attach( ConstantTwist::Make() );

	Frame<Length,One> frame;
	pTrack->Transition( 5_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );

	pTrack->Flip();
	BOOST_CHECK( Mirror( *pTrack, { {0_m, 0_m, 0_m }, Ey<One> } ) );

	pTrack->Transition( 5_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
}

BOOST_AUTO_TEST_CASE( flippedCombinedTwistMirror )
{
	auto pTrack = TrackBuilder::Make();

	std::shared_ptr<LineP> pLine = LineP::Make();
	pLine->Create( VectorBundle<Length,One>{{10_m,10_m,10_m},{1,0,0}} );
	pTrack->Attach( pLine, {0_m,10_m} );
	pTrack->Attach( CombinedTwist::Make() );

	Frame<Length,One> frame;
	pTrack->Transition( 5_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );

	pTrack->Flip();
	BOOST_CHECK( Mirror( *pTrack, { {0_m, 0_m, 0_m }, Ey<One> } ) );

	pTrack->Transition( 5_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
}

BOOST_AUTO_TEST_CASE( flippedCombinedTwistMirrorZ )
{
	auto pTrack = TrackBuilder::Make();

	std::shared_ptr<LineP> pLine = LineP::Make();
	pLine->Create( VectorBundle<Length,One>{{10_m,10_m,10_m},{1,0,0}} );
	pTrack->Attach( pLine, {0_m,10_m} );
	pTrack->Attach( CombinedTwist::Make() );

	Frame<Length,One> frame;
	pTrack->Transition( 5_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );

	pTrack->Flip();
	BOOST_CHECK( Mirror( *pTrack, { {0_m, 0_m, 0_m }, Ez<One> } ) );

	pTrack->Transition( 5_m, frame );
	BOOST_CHECK_SMALL( frame.N * Up, epsilon__angle );
	BOOST_CHECK_GT( frame.B * Up, 0 );
}

BOOST_AUTO_TEST_SUITE_END() //RoadwayTwist_Tests
BOOST_AUTO_TEST_SUITE(ParallelTrack_Tests)

BOOST_AUTO_TEST_CASE( ParallelLine )
{
	auto pOriginalTrack = ParallelizableTrack::Make();
	auto pLine = LineP::Make();
	pLine->Create( VectorBundle<Length,One>{ spat::Origin3D<Length>, spat::Ex<One> } );
	pOriginalTrack->Attach( std::move(pLine), common::Interval<Length>{ 0_m, 60_m } );

	auto pLinearTwist = LinearTwist::Make();
	pLinearTwist->From( 0_deg );
	pLinearTwist->To( 360_deg );
	pOriginalTrack->Attach( std::move(pLinearTwist) );

	auto pParallelTrack = pOriginalTrack->AddParallel( { 4.5_m, 0_m }, true  );

	BOOST_CHECK_GT( pParallelTrack->Range().Far(), pOriginalTrack->Range().Far() );

	VectorBundle2<Length,One> frameO, frameP;
	Length sParallel = 0_m;

	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( 4.5_m );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );

	sParallel = pParallelTrack->GetLength()/2;
	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( 4.5_m );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );

	sParallel = pParallelTrack->GetLength();
	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( 4.5_m );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
}

BOOST_AUTO_TEST_CASE( ParallelArc )
{
	auto pOriginalTrack = ParallelizableTrack::Make();
	auto pArc = ArcP::Make();
	pArc->Create( spat::Origin3D<Length>,
				  spat::Ex<One>,
				  spat::Ey<One>,
				  1.f/50_m );
	pOriginalTrack->Attach( std::move(pArc), common::Interval<Length>{ 0_m, 60_m } );

	auto pParallelTrack = pOriginalTrack->AddParallel( { 4.5_m, 0_m }, true );

	VectorBundle2<Length,One> frameO, frameP;
	Length sParallel = 0_m;

	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( 4.5_m );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.001f );

	sParallel = pParallelTrack->GetLength()/2;
	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( 4.5_m );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.001f );

	sParallel = pParallelTrack->GetLength();
	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( 4.5_m );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.001f );
}

BOOST_AUTO_TEST_CASE( ParallelHelix )
{
	auto pOriginalTrack = ParallelizableTrack::Make();
	auto pHelix = HelixP::Make();
	HelixP::Data data{};
	pHelix->Create( data );
	BOOST_CHECK_CLOSE_DIMENSION( data.Torsion(), pHelix->Torsion( 30_m ), 1 );
	pOriginalTrack->Attach( std::move(pHelix), common::Interval<Length>{ 0_m, 60_m } );

	auto pParallelTrack = pOriginalTrack->AddParallel( { 4.5_m, 0_m }, true );

	VectorBundle2<Length,One> frameO, frameP;
	Length sParallel = 0_m;

	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( 4.5_m );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.01f );

	sParallel = pParallelTrack->GetLength()/2;
	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( 4.5_m );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.001f );

	sParallel = pParallelTrack->GetLength();
	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( 4.5_m );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.01f );

	HelixP::Data data2{};
	data2.a -= 4.5_m;
	BOOST_CHECK_CLOSE_DIMENSION( pParallelTrack->Curvature( pParallelTrack->GetLength()/2 ), data2.Curvature(), 0.1 );
//	BOOST_CHECK_CLOSE_DIMENSION( pParallelTrack->Torsion( pParallelTrack->GetLength()/2 ), data2.Torsion(), 1 );
}

BOOST_AUTO_TEST_CASE( ParallelCubic )
{
	auto pOriginalTrack = ParallelizableTrack::Make();
	auto pCubic = Cubic::Make();
	common::Interval<Length> range = pCubic->Create( 
		{ {0_m,0_m,0_m},
		  {75_m,0_m,0_m} },
		{ {50_m,0_m,5_m},
		  {75_m,0_m,0_m} }
	);

	pOriginalTrack->Attach( std::move(pCubic), range );

	auto pParallelTrack = pOriginalTrack->AddParallel( { 4.5_m, 0_m }, true );

	VectorBundle2<Length,One> frameO, frameP;
	Length sParallel = 0_m;

	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( 4.5_m );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.001f );

	sParallel = pParallelTrack->GetLength()/2;
	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( 4.5_m );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.001f );

	sParallel = pParallelTrack->GetLength();
	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( 4.5_m );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.001f );
}

BOOST_AUTO_TEST_CASE( ParallelCubic2 )
{
	auto pOriginalTrack = ParallelizableTrack::Make();
	auto pCubic = Cubic::Make();
	common::Interval<Length> range = pCubic->Create( {
		{ -54.0000000_m, -126.599998_m, 0.600000024_m  },
		{ 98.7074509_m, 0.00000000_m, 0.00000000_m  },
		{ -8.72233582_m, 67.7999954_m, 22.2000008_m  },
		{ 5.81489563_m, -45.1999969_m, -14.8000002_m  }
	} );

	pOriginalTrack->Attach( std::move(pCubic), range );

	auto pParallelTrack = pOriginalTrack->AddParallel( { 4.5_m, 0_m }, true );

	VectorBundle2<Length,One> frameO, frameP;
	Length sParallel = 0_m;

	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( 4.5_m );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.001f );

	sParallel = pParallelTrack->GetLength()/2;
	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( 4.5_m );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.001f );

	sParallel = pParallelTrack->GetLength();
	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( 4.5_m );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.001f );
}

BOOST_AUTO_TEST_CASE( ParallelCubic3 )
{
	auto pOriginalTrack = ParallelizableTrack::Make();
	auto pCubic = Cubic::Make();
	common::Interval<Length> range = pCubic->Create( 
		{ { 0_m, 0_m, 0_m  },
		  { 25_m, 0_m, 0_m  } },
		{ { 20_m, 5_m, 0_m  },
		  { 25_m, 0_m, 0_m  } } 
	);

	BOOST_CHECK_CLOSE_DIMENSION( range.Length(), 20.79_m, 1 );
	pOriginalTrack->Attach( std::move(pCubic), range );

	Length h = 4.5_m;
	auto pParallelTrack = pOriginalTrack->AddParallel( { h, 0_m }, true );

	VectorBundle2<Length,One> frameO, frameP;
	Length sParallel = 0_m;

	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( h );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.001f );

	sParallel = pParallelTrack->GetLength()/2;
	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( h );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.001f );

	sParallel = pParallelTrack->GetLength();
	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( h );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.01f );

	sParallel = 1.5_m;
	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( h );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.001f );

	BOOST_CHECK_SMALL( pOriginalTrack->Curvature( pOriginalTrack->GetLength()/2 ), 200*epsilon__curvature );

	BOOST_CHECK_LT( pParallelTrack->Curvature( 20_m ), pOriginalTrack->Curvature( pOriginalTrack->OriginalParameterFrom( 0, 20_m ) ) );
	BOOST_CHECK_LT( pParallelTrack->Curvature( 20.5_m ), pOriginalTrack->Curvature( pOriginalTrack->OriginalParameterFrom( 0, 20.5_m ) ) );
	BOOST_CHECK_LT( pParallelTrack->Curvature( 19_m ), pOriginalTrack->Curvature( pOriginalTrack->OriginalParameterFrom( 0, 19_m ) ) );
	BOOST_CHECK_LT( pParallelTrack->Curvature( 19.5_m ), pOriginalTrack->Curvature( pOriginalTrack->OriginalParameterFrom( 0, 19.5_m ) ) );
	BOOST_CHECK_LT( pParallelTrack->Curvature( 18_m ), pOriginalTrack->Curvature( pOriginalTrack->OriginalParameterFrom( 0, 18_m ) ) );
	BOOST_CHECK_LT( pParallelTrack->Curvature( 18.5_m ), pOriginalTrack->Curvature( pOriginalTrack->OriginalParameterFrom( 0, 18.5_m ) ) );
	BOOST_CHECK_LT( pParallelTrack->Curvature( 17_m ), pOriginalTrack->Curvature( pOriginalTrack->OriginalParameterFrom( 0, 17_m ) ) );
	BOOST_CHECK_LT( pParallelTrack->Curvature( 17.5_m ), pOriginalTrack->Curvature( pOriginalTrack->OriginalParameterFrom( 0, 17.5_m ) ) );


	//for( Length s = 0_m; s < pOriginalTrack->GetLength(); s += 0.5_m ){
	//	Length sp = pOriginalTrack->ParallelParameterFrom( 0, s );
	//	AnglePerLength k = pOriginalTrack->Curvature( s );
	//	AnglePerLength kpc = k / (1-k*h);
	//	AnglePerLength kp = pParallelTrack->Curvature( sp );
	//	//std::cout << s << ", " << sp << std::endl;
	//	//std::cout << k.Units() << ", " << kp.Units() << ", " << kpc.Units() << std::endl;
	//	BOOST_CHECK_CLOSE_DIMENSION( kp, kpc, 1 );
	//}

}

BOOST_AUTO_TEST_CASE( ParallelCubic4 )
{
	auto pOriginalTrack = ParallelizableTrack::Make();
	auto pCubic = Cubic::Make();
	common::Interval<Length> range = pCubic->Create( 
		{ {0_m,0_m,0_m},
		  {75_m,0_m,0_m} },
		{ {100_m,25_m,8_m},
		  {75_m,0_m,0_m} }
	);

	pOriginalTrack->Attach( std::move(pCubic), range );

	Length h = 4.5_m;
	auto pParallelTrack = pOriginalTrack->AddParallel( { h, 0_m }, true );

//	BOOST_CHECK_EQUAL( pParallelTrack->GetCurve().first->ZeroSet().size(), 0 );

	VectorBundle2<Length,One> frameO, frameP;
	Length sParallel = 0_m;

	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( h );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.001f );

	sParallel = pParallelTrack->GetLength()/2;
	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( h );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.001f );

	sParallel = pParallelTrack->GetLength();
	BOOST_CHECK_CLOSE_DIMENSION( pOriginalTrack->ParallelParameterFrom( 0, pOriginalTrack->OriginalParameterFrom( 0, sParallel ) ), sParallel, 0.1f );
	pOriginalTrack->Transition( pOriginalTrack->OriginalParameterFrom( 0, sParallel ), frameO );
	pParallelTrack->Transition( sParallel, frameP );
	frameO.TransportNor( h );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.P, frameP.P, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( frameO.N, frameP.N, 0.001f );

	for( Length s = 0.5_m; s < pOriginalTrack->GetLength() / 3; s += 0.5_m ){
		Length sp = pOriginalTrack->ParallelParameterFrom( 0, s );
		AnglePerLength k = pOriginalTrack->Curvature( s );
		AnglePerLength kp = pParallelTrack->Curvature( sp );
		//std::cout << s << ", " << sp << std::endl;
		//std::cout << k << ", " << kp << ", " << std::endl;
		BOOST_CHECK_GT( kp, k );
	}

	for( Length s = 2 * pOriginalTrack->GetLength() / 3; s < pOriginalTrack->GetLength(); s += 0.5_m ){
		Length sp = pOriginalTrack->ParallelParameterFrom( 0, s );
		AnglePerLength k = pOriginalTrack->Curvature( s );
		AnglePerLength kp = pParallelTrack->Curvature( sp );
		//std::cout << s << ", " << sp << std::endl;
		//std::cout << k << ", " << kp << ", " << std::endl;
		BOOST_CHECK_LT( kp, k );
	}

	std::cout << "Curvature at 50.39m: " << pOriginalTrack->Curvature( pOriginalTrack->OriginalParameterFrom( 0, 50.39_m ) ) << std::endl;
}

BOOST_AUTO_TEST_CASE( PlaneParallelCubic )
{
	auto pOriginalTrack = ParallelizableTrack::Make();
	auto pCubic = Cubic::Make();
	common::Interval<Length> range = pCubic->Create( 
		{ {0_m,0_m,0_m},
		  {75_m,0_m,0_m} },
		{ {100_m,25_m,8_m},
		  {75_m,0_m,0_m} }
	);

	pOriginalTrack->Attach( std::move(pCubic), range );

	Length h = 4.5_m;
	auto pParallelTrack = pOriginalTrack->AddParallel( { h, 0_m }, false );

	BOOST_CHECK_EQUAL( pParallelTrack->GetCurve().first->ZeroSet().size(), 0u );
}

BOOST_AUTO_TEST_CASE( PlaneParallelCubic2 )
{
	auto pOriginalTrack = TrackBuilder::Make();
	auto pCubic = Cubic::Make();
	Cubic::Data data{
		{0_m,0_m,0_m},
		{74.1342316_m,0_m,0_m},
		{77.8333282_m,-143.031998_m,-1.22054998e-05_m},
		{2.49909997_m,96.6319809_m,8.13699626e-06_m} };
	common::Interval<Length> range = pCubic->Create( data );

	Frame<Length,One> frame{
		{-141.699997_m, 90.0000000_m, 0.300000012_m },
		{1.00000000, 0.00000000, 0.00000000 },
		{0.00000000, 0.864785612, -0.502141774 },
		{0.00000000, 0.502141774, 0.864785612 }
	};
	BOOST_CHECK( frame.IsOrthoNormal() );

	std::unique_ptr<CombinedTwist> pTwist = CombinedTwist::Make();
	pTwist->AttachTwist1( DirectionalTwist::Make( {0,0.502141774,0.864785612} ) );
	pTwist->AttachTwist2( LinearTwist::Make( 0.523598790f, 0.523598790f ) );

	pOriginalTrack->SetFrame( frame );
	pOriginalTrack->Attach( std::move(pCubic), range );
	pOriginalTrack->Attach( std::move(pTwist) );
	pOriginalTrack->GetTwist().Freeze();

	Length h = 4.5_m;
	std::shared_ptr<TrackBuilder> pParallel = MakeParallelTrackWithSpline(
			*pOriginalTrack, 
			range,
			{ h, 0_m },
			false );

	BOOST_REQUIRE( pParallel );
	std::shared_ptr<const Spline> pSpline = std::dynamic_pointer_cast<const Spline>(pParallel->GetCurve().first);
	BOOST_CHECK( !pSpline->HasGaps() );
	BOOST_CHECK( !pSpline->HasKinks() );
	BOOST_CHECK_EQUAL( pSpline->GetData().size(), 5u );

	common::Interval<Length> rangeParellel = pParallel->Range();

	spat::Frame<Length,One> frameA, frameB;
	pOriginalTrack->Transition( 0_m, frameA );
	pParallel->Transition( 0_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
	pOriginalTrack->Transition( 10_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 10_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, 2*trax::epsilon__angle );
	pOriginalTrack->Transition( 20_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 20_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
	pOriginalTrack->Transition( 30_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 30_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
	pOriginalTrack->Transition( 40_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 40_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle ); 
	pOriginalTrack->Transition( 50_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 50_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
	pOriginalTrack->Transition( range.Far(), frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * range.Far(), frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
}

BOOST_AUTO_TEST_CASE( PlaneParallelCubic3 )
{
	auto pOriginalTrack = TrackBuilder::Make();
	auto pCubic = Cubic::Make();
	Cubic::Data data{
		{0_m,0_m,0_m},
		{74.1342316_m,0_m,0_m},
		{77.8333282_m,-143.031998_m,-1.22054998e-05_m},
		{2.49909997_m,96.6319809_m,8.13699626e-06_m} };
	common::Interval<Length> range = pCubic->Create( data );

	Frame<Length,One> frame{
		{-141.699997_m, 90.0000000_m, 0.300000012_m },
		{1.00000000, 0.00000000, 0.00000000 },
		{0.00000000, 0.864785612, -0.502141774 },
		{0.00000000, 0.502141774, 0.864785612 }
	};
	BOOST_CHECK( frame.IsOrthoNormal() );

	std::unique_ptr<CombinedTwist> pTwist = CombinedTwist::Make();
	pTwist->AttachTwist1( DirectionalTwist::Make( {0,0.502141774,0.864785612} ) );
	pTwist->AttachTwist2( LinearTwist::Make( 0.523598790f, 0.523598790f ) );

	pOriginalTrack->SetFrame( frame );
	pOriginalTrack->Attach( std::move(pCubic), range );
	pOriginalTrack->Attach( std::move(pTwist) );

	Length h = 4.5_m;
	std::shared_ptr<TrackBuilder> pParallel = MakeParallelTrackWithSpline(
			*pOriginalTrack, 
			range,
			{ h, 0_m },
			false );

	BOOST_REQUIRE( pParallel );
	std::shared_ptr<const Spline> pSpline = std::dynamic_pointer_cast<const Spline>(pParallel->GetCurve().first);
	BOOST_CHECK( !pSpline->HasGaps() );
	BOOST_CHECK( !pSpline->HasKinks() );
	BOOST_CHECK_EQUAL( pSpline->GetData().size(), 5u );


	common::Interval<Length> rangeParellel = pParallel->Range();

	spat::Frame<Length,One> frameA, frameB;
	pOriginalTrack->Transition( 0_m, frameA );
	pParallel->Transition( 0_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
	pOriginalTrack->Transition( 10_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 10_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, 2*trax::epsilon__angle );
	pOriginalTrack->Transition( 20_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 20_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
	pOriginalTrack->Transition( 30_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 30_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
	pOriginalTrack->Transition( 40_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 40_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle ); 
	pOriginalTrack->Transition( 50_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 50_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
	pOriginalTrack->Transition( range.Far(), frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * range.Far(), frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
}

BOOST_AUTO_TEST_CASE( PlaneParallelCubic4 )
{
	auto pOriginalTrack = TrackBuilder::Make();
	auto pCubic = Cubic::Make();
	common::Interval<Length> range = pCubic->Create( 
		{ {0_m,0_m,0_m},
		  {75_m,0_m,0_m} },
		{ {100_m,25_m,20_m},
		  {75_m,0_m,0_m} }
	);

	pOriginalTrack->Attach( std::move(pCubic), range );
	std::unique_ptr<CombinedTwist> pTwist = CombinedTwist::Make();
	pTwist->AttachTwist1( DirectionalTwist::Make() );
	pTwist->AttachTwist2( LinearTwist::Make( 0.f, 0.523598790f ) );
	pOriginalTrack->Attach( std::move(pTwist) );

	Length h = 4.5_m;
	std::shared_ptr<TrackBuilder> pParallel = MakeParallelTrackWithSpline(
			*pOriginalTrack, 
			range,
			{ h, 0_m },
			false );

	BOOST_REQUIRE( pParallel );
	std::shared_ptr<const Spline> pSpline = std::dynamic_pointer_cast<const Spline>(pParallel->GetCurve().first);
	BOOST_CHECK( !pSpline->HasGaps() );
	BOOST_CHECK( !pSpline->HasKinks() );
	BOOST_CHECK_EQUAL( pSpline->GetData().size(), 6u );


	common::Interval<Length> rangeParellel = pParallel->Range();

	spat::Frame<Length,One> frameA, frameB;
	pOriginalTrack->Transition( 0_m, frameA );
	pParallel->Transition( 0_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
	pOriginalTrack->Transition( 10_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 10_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
	pOriginalTrack->Transition( 20_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 20_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
	pOriginalTrack->Transition( 30_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 30_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
	pOriginalTrack->Transition( 40_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 40_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle ); 
	pOriginalTrack->Transition( 50_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 50_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
	pOriginalTrack->Transition( range.Far(), frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * range.Far(), frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );

	Length ds = epsilon__length;
	Length length = 0_m;
	Position<Length> PNew, POld;
	pParallel->Transition( 0_m, POld );
	Vector<One> TNew, TOld;
	pParallel->Transition( 0_m, TOld );

	const auto default_precision{std::cout.precision()};	
	std::cout << std::fixed;
	std::cout << std::setprecision(6);
	for( Length s = rangeParellel.Near(); s < rangeParellel.Far(); s += ds ){
		pParallel->Transition( s, PNew );
		std::pair<Length,Vector<One>> n = Normalize( PNew - POld );
		BOOST_CHECK_LE( n.first.Units(), 10*ds.Units() );
		if( n.first.Units() > 2*ds.Units() ){
			std::cout << "d/ds difference at s: " << s.Units() << std::endl;
		}
		length += n.first;
		TNew = n.second;
		BOOST_CHECK_GT( TOld * TNew, 0_1 );
		if( TOld * TNew < 0_1 ){
			std::cout << "s: " << s.Units() << std::endl;
		}
		POld = PNew;
		TOld = TNew;
	}
	pParallel->Transition( rangeParellel.Far(), PNew );
	std::pair<Length,Vector<One>> n = Normalize( PNew - POld );
	length += n.first;
	std::cout << std::setprecision(default_precision);

	BOOST_CHECK_SMALL( abs(length - rangeParellel.Length()), 5*epsilon__length );
}

BOOST_AUTO_TEST_CASE( PlaneParallelCubic5 )
{
	auto pOriginalTrack = TrackBuilder::Make();

	Frame<Length,One> frame{ {-150.000000_m,-301.000000_m,0.300000012_m}, Ex<One>, Ey<One>, Ez<One> };
	pOriginalTrack->SetFrame( frame );

	auto pCubic = Cubic::Make();
	common::Interval<Length> range = pCubic->Create( 
		{ {-7.00000000_m,15.9999905_m,0_m},
		  {202.639694_m,0_m,0_m},
		  {-391.180389_m,222.301102_m,0_m},
		  {258.740601_m,-164.701004_m,0_m} }
	);
	pOriginalTrack->Attach( std::move(pCubic), range );

	std::unique_ptr<CombinedTwist> pTwist = CombinedTwist::Make();
	pTwist->AttachTwist1( DirectionalTwist::Make() );
	pTwist->AttachTwist2( LinearTwist::Make( 0.f, 0.523598790f ) );
	pOriginalTrack->Attach( std::move(pTwist) );

	Length h = 4.5_m;
	std::shared_ptr<TrackBuilder> pParallel = MakeParallelTrackWithSpline(
			*pOriginalTrack, 
			range,
			{ h, 0_m },
			false );

	BOOST_REQUIRE( pParallel );
	std::shared_ptr<const Spline> pSpline = std::dynamic_pointer_cast<const Spline>(pParallel->GetCurve().first);
	BOOST_CHECK( !pSpline->HasGaps() );
	BOOST_CHECK( !pSpline->HasKinks() );
	BOOST_CHECK_GE( pSpline->GetData().size(), 11u );
	BOOST_CHECK_LE( pSpline->GetData().size(), 12u );


	common::Interval<Length> rangeParellel = pParallel->Range();

	spat::Frame<Length,One> frameA, frameB;
	pOriginalTrack->Transition( 0_m, frameA );
	pParallel->Transition( 0_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
	pOriginalTrack->Transition( 10_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 10_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
	pOriginalTrack->Transition( 20_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 20_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
	pOriginalTrack->Transition( 30_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 30_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, 2*trax::epsilon__angle );
	pOriginalTrack->Transition( 40_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 40_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, 2*trax::epsilon__angle ); 
	pOriginalTrack->Transition( 50_m, frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * 50_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );
	pOriginalTrack->Transition( range.Far(), frameA );
	pParallel->Transition( rangeParellel.Length() / range.Length() * range.Far(), frameB );
	BOOST_CHECK_CLOSE_SPATIAL( frameA.B, frameB.B, trax::epsilon__angle );

	Length ds = epsilon__length;
	Length length = 0_m;
	Position<Length> PNew, POld;
	pParallel->Transition( 0_m, POld );
	Vector<One> TNew, TOld;
	pParallel->Transition( 0_m, TOld );

	const auto default_precision{std::cout.precision()};	
	std::cout << std::fixed;
	std::cout << std::setprecision(6);
	for( Length s = rangeParellel.Near(); s < rangeParellel.Far(); s += ds ){
		pParallel->Transition( s, PNew );
		std::pair<Length,Vector<One>> n = Normalize( PNew - POld );
		BOOST_CHECK_LE( n.first.Units(), 10*ds.Units() );
		if( n.first.Units() > 2*ds.Units() ){
			std::cout << "d/ds difference at s: " << s.Units() << std::endl;
		}
		length += n.first;
		TNew = n.second;
		BOOST_CHECK_GT( TOld * TNew, 0_1 );
		if( TOld * TNew < 0_1 ){
			std::cout << "s: " << s.Units() << std::endl;
		}
		POld = PNew;
		TOld = TNew;
	}
	pParallel->Transition( rangeParellel.Far(), PNew );
	std::pair<Length,Vector<One>> n = Normalize( PNew - POld );
	length += n.first;
	std::cout << std::setprecision(default_precision);

	//for( int idx = 0; idx < pSpline->CountControlPoints(); ++idx ){
	//	Length s = pSpline->GetParameter( idx );
	//	std::cout << "Controlpoint at s: " << s + rangeParellel.Near() << std::endl;
	//}

	BOOST_CHECK_SMALL( abs(length - rangeParellel.Length()), 5*epsilon__length );
}

BOOST_AUTO_TEST_CASE( PlaneParallelCubic6 )
{
	auto pOriginalTrack = TrackBuilder::Make();

	auto pCubic = Cubic::Make();
	common::Interval<Length> range = pCubic->Create( 
		{ {0.00000000_m,0_m,0_m},
		  {249.272202_m,0_m,0_m},
		  {-690.844727_m,-39.4499397_m,0_m},
		  {456.472412_m,0.549949586_m,0_m} }
	);
	pOriginalTrack->Attach( std::move(pCubic), range );

	std::unique_ptr<CombinedTwist> pTwist = CombinedTwist::Make();
	pTwist->AttachTwist1( DirectionalTwist::Make() );
	pTwist->AttachTwist2( LinearTwist::Make( 0.f, 0.f ) );
	pOriginalTrack->Attach( std::move(pTwist) );

	Length h = 4.5_m;
	
	BOOST_REQUIRE_THROW( MakeParallelTrackWithSpline(
			*pOriginalTrack, 
			range,
			{ h, 0_m },
			false ), std::runtime_error );
}

BOOST_AUTO_TEST_SUITE_END() //ParallelTrack_Tests
BOOST_AUTO_TEST_SUITE(Track_Connection_tests)

BOOST_AUTO_TEST_CASE( connect_tests )
{
	{	
		std::shared_ptr<trax::TrackBuilder> pTrack1 = TrackBuilder::Make();
		BOOST_CHECK( pTrack1 != nullptr );
		std::shared_ptr<trax::TrackBuilder> pTrack2 = TrackBuilder::Make();
		BOOST_CHECK( pTrack2 != nullptr );
		std::shared_ptr<trax::TrackBuilder> pTrack3 = TrackBuilder::Make();
		BOOST_CHECK( pTrack3 != nullptr );
		std::shared_ptr<trax::TrackBuilder> pTrack4 = TrackBuilder::Make();
		BOOST_CHECK( pTrack4 != nullptr );


		// this fixture should give a circle with r == 10...
		shared_ptr<trax::ArcP> pArc1 = ArcP::Make();
		pArc1->Create( { Origin3D<Length>, { 1, 0, 0 }, { 0, _m(10_m), 0 } } );
		pTrack1->Attach( pArc1, {0_m,10_m*pi/2} );
		BOOST_CHECK_SMALL( pTrack1->GetLength() - 10_m*pi/2, 1_mm );

		shared_ptr<trax::ArcP> pArc2 = ArcP::Make();
		pArc2->Create( { Origin3D<Length>, { 0, 1, 0 }, { _m(-10_m), 0, 0 } } );
		pTrack2->Attach( pArc2, {0_m,10_m*pi/2} );
		BOOST_CHECK_SMALL( pTrack2->GetLength() - 10_m*pi/2, 1_mm );

		shared_ptr<trax::ArcP> pArc3 = ArcP::Make();
		pArc3->Create( { Origin3D<Length>, { -1, 0, 0 }, { 0, _m(-10_m), 0 } } );
		pTrack3->Attach( pArc3, {0_m,10_m*pi/2} );
		BOOST_CHECK_SMALL( pTrack3->GetLength() - 10_m*pi/2, 1_mm );

		shared_ptr<trax::ArcP> pArc4 = ArcP::Make();
		pArc4->Create( { Origin3D<Length>, { 0, -1, 0 }, { _m(10_m), 0, 0 } } );
		pTrack4->Attach( pArc4, {0_m,10_m*pi/2} );
		BOOST_CHECK_SMALL( pTrack4->GetLength() - 10_m*pi/2, 1_mm );

		pTrack1->Couple( std::make_pair(pTrack1, Track::EndType::end), std::make_pair(pTrack2, Track::EndType::front) );
		pTrack2->Couple( std::make_pair(pTrack2, Track::EndType::end), std::make_pair(pTrack3, Track::EndType::front) );
		pTrack3->Couple( std::make_pair(pTrack3, Track::EndType::end), std::make_pair(pTrack4, Track::EndType::front) );
		pTrack4->Couple( std::make_pair(pTrack4, Track::EndType::end), std::make_pair(pTrack1, Track::EndType::front) );

		BOOST_CHECK( pTrack1->TransitionEnd( Track::EndType::front ).first == pTrack4 );
		BOOST_CHECK( pTrack1->TransitionEnd( Track::EndType::front ).second == Track::EndType::end );
		BOOST_CHECK( pTrack2->TransitionEnd( Track::EndType::front ).first == pTrack1 );
		BOOST_CHECK( pTrack2->TransitionEnd( Track::EndType::front ).second == Track::EndType::end );
		BOOST_CHECK( pTrack3->TransitionEnd( Track::EndType::front ).first == pTrack2 );
		BOOST_CHECK( pTrack3->TransitionEnd( Track::EndType::front ).second == Track::EndType::end );
		BOOST_CHECK( pTrack4->TransitionEnd( Track::EndType::front ).first == pTrack3 );
		BOOST_CHECK( pTrack4->TransitionEnd( Track::EndType::front ).second == Track::EndType::end );

		trax::Location Loc( pTrack1, trax::TrackLocation( 0_m, true ) );

		Frame<Length,One> frame1, frame2;
		Loc.Transition( frame1 );
		Loc.Move( pTrack1->GetLength() + pTrack2->GetLength() + pTrack3->GetLength() + pTrack4->GetLength() );
		Loc.Transition( frame2 );
		BOOST_CHECK( frame1.Equals( frame2, epsilon__length, 0.001f ) );

		Loc.Move( pTrack1->GetLength() + pTrack2->GetLength() + pTrack3->GetLength()/2 );
		shared_ptr<const trax::Track> pTrack = Loc.GetTrack();
		BOOST_CHECK( pTrack == pTrack3 );
		Loc.Transition( frame1 );
		Loc.Move( pTrack1->GetLength() + pTrack2->GetLength() + pTrack3->GetLength() + pTrack4->GetLength() );
		Loc.Transition( frame2 );
		BOOST_CHECK( frame1.Equals( frame2, epsilon__length, 0.001f ) );

		pTrack1->DeCouple();
		pTrack3->DeCouple();
	}
}

BOOST_AUTO_TEST_CASE( move_over_paraconnection_tests )
{
	{
		std::shared_ptr<trax::TrackBuilder> pTrack1 = TrackBuilder::Make();
		BOOST_CHECK( pTrack1 != nullptr );
		std::shared_ptr<trax::TrackBuilder> pTrack2 = TrackBuilder::Make();
		BOOST_CHECK( pTrack2 != nullptr );

		std::shared_ptr<trax::HelixP> pCurve1 = HelixP::Make();
		std::shared_ptr<trax::HelixP> pCurve2 = HelixP::Make();

		spat::VectorBundle2<Length,One> center;
		center.Init();
		center.RotateTan( -pi/2 );
		center.P.y += 10_m;
		pCurve1->Create( center, 20_1Im, 1_1Im );

		center.Rotate( pi * Vector<One>(0,1,0) );
	//	center.TransportBy( -center.T * 40 );
		center.TransportTo( -40_m );
		pCurve2->Create( center, 20_1Im, -1_1Im );

		pTrack1->Attach( pCurve1, {0_m,200_m} );
		pTrack2->Attach( pCurve2, {0_m,200_m} );

		pTrack1->Couple( std::make_pair(pTrack1, Track::EndType::front), std::make_pair(pTrack2, Track::EndType::front) );

		trax::Location Loc{ pTrack1, trax::TrackLocation( 1_m, true ) };

		Frame<Length,One> frame1;
		Loc.Transition( frame1 );

		Loc.Move( -2_m );

		Frame<Length,One> frame2;
		Loc.Transition( frame2 );

		// after transition both vectors should point roughly in the same direction...
		BOOST_CHECK_GT( frame1.T * frame2.T, 0 );

		pTrack1->DeCouple();
	}
}

BOOST_AUTO_TEST_CASE( ConnectTwoTracksOnBothEnds )
{
	{
		std::shared_ptr<trax::TrackBuilder> pTrack1 = TrackBuilder::Make();
		BOOST_CHECK( pTrack1 != nullptr );
		std::shared_ptr<trax::TrackBuilder> pTrack2 = TrackBuilder::Make();
		BOOST_CHECK( pTrack2 != nullptr );

		pTrack1->Couple( std::make_pair(pTrack1, Track::EndType::front), std::make_pair(pTrack2, Track::EndType::end) );
		pTrack1->Couple( std::make_pair(pTrack1, Track::EndType::end), std::make_pair(pTrack2, Track::EndType::front) );

		BOOST_CHECK_EQUAL( pTrack1->TransitionEnd( Track::EndType::front ).first, pTrack2 );
		BOOST_CHECK_EQUAL( pTrack1->TransitionEnd( Track::EndType::front ).second, Track::EndType::end );
		BOOST_CHECK_EQUAL( pTrack1->TransitionEnd( Track::EndType::end ).first, pTrack2 );
		BOOST_CHECK_EQUAL( pTrack1->TransitionEnd( Track::EndType::end ).second, Track::EndType::front );
		BOOST_CHECK_EQUAL( pTrack2->TransitionEnd( Track::EndType::end ).first, pTrack1 );
		BOOST_CHECK_EQUAL( pTrack2->TransitionEnd( Track::EndType::end ).second, Track::EndType::front );
		BOOST_CHECK_EQUAL( pTrack2->TransitionEnd( Track::EndType::front ).first, pTrack1 );
		BOOST_CHECK_EQUAL( pTrack2->TransitionEnd( Track::EndType::front ).second, Track::EndType::end );

		pTrack1->DeCouple();
	}
}

BOOST_AUTO_TEST_CASE( ConnectSelf )
{
	{
		std::shared_ptr<trax::TrackBuilder> pTrack = TrackBuilder::Make();
		BOOST_CHECK( pTrack != nullptr );

		pTrack->Couple( std::make_pair(pTrack, Track::EndType::front), std::make_pair(pTrack, Track::EndType::end) );

		BOOST_CHECK_EQUAL( pTrack->TransitionEnd( Track::EndType::front ).first, pTrack );
		BOOST_CHECK_EQUAL( pTrack->TransitionEnd( Track::EndType::front ).second, Track::EndType::end );
		BOOST_CHECK_EQUAL( pTrack->TransitionEnd( Track::EndType::end ).first, pTrack );
		BOOST_CHECK_EQUAL( pTrack->TransitionEnd( Track::EndType::end ).second, Track::EndType::front );

		pTrack->DeCouple();
	}
}

BOOST_AUTO_TEST_SUITE_END() //connect_tests
BOOST_AUTO_TEST_SUITE(Switch_tests)

BOOST_AUTO_TEST_CASE( normalization_tests )
{	//		 
	//	1		2
	// ----- ------
	//		\3
	//		 \
	//

	shared_ptr<TrackBuilder> pTrack1 = TrackBuilder::Make();
    BOOST_CHECK( pTrack1 != nullptr );
	shared_ptr<LineP> pLine = LineP::Make();
    BOOST_CHECK( pLine != nullptr );
	pLine->Create( Origin3D<Length>, Position<Length>(1_m,0_m,0_m), Vector<One>(0,0,1) );
	pTrack1->Attach( pLine, {0_m,10_m} );

	shared_ptr<TrackBuilder> pTrack2 = TrackBuilder::Make();
    BOOST_CHECK( pTrack2 != nullptr );
	std::shared_ptr<trax::ArcP> pArc1 = ArcP::Make();
	pArc1->Create( { {10_m,0_m,0_m}, {1,0,0} }, {20_m,1_m,0_m} );
	pTrack2->Attach( pArc1, {0_m,1_m} );

	shared_ptr<TrackBuilder> pTrack3 = TrackBuilder::Make();
    BOOST_CHECK( pTrack3 != nullptr );
	std::shared_ptr<trax::ArcP> pArc2 = ArcP::Make();
	pArc2->Create( { {10_m,0_m,0_m}, {1,0,0} }, {20_m,-10_m,0_m} );
	pTrack3->Attach( pArc2, {0_m,1_m} );

	unique_ptr<Switch> pSwitch = Switch::Make();
    BOOST_CHECK( pSwitch != nullptr );
	pSwitch->NarrowTrack( pTrack1, Track::EndType::end );
	pSwitch->StraightTrack( pTrack2, Track::EndType::front );
	pSwitch->DivergedTrack( pTrack3, Track::EndType::front );
	pSwitch->Set( Switch::Status::go );

	BOOST_CHECK( pSwitch->IsNormal() );
	pSwitch->SwapTracks();
	BOOST_CHECK( !pSwitch->IsNormal() );
	pSwitch->SwapTracks();
	BOOST_CHECK( pSwitch->IsNormal() );

	pTrack1->DeCouple();
	pTrack2->DeCouple();
	pTrack3->DeCouple();
}

BOOST_FIXTURE_TEST_CASE( sloReassignmentPropperFreeExisting, SwichFixture )
// Connecting to already connected slot properly disconnects other track.
{	
	unique_ptr<Switch> pSwitch = Switch::Make();
    BOOST_CHECK( pSwitch != nullptr );
	pSwitch->NarrowTrack( m_pTrack1, Track::EndType::end );
	pSwitch->StraightTrack( m_pTrack2, Track::EndType::front );
	pSwitch->DivergedTrack( m_pTrack3, Track::EndType::front );
	pSwitch->Set( Switch::Status::go );

	pSwitch->StraightTrack( m_pTrack3, Track::EndType::front );
	BOOST_CHECK( m_pTrack2->GetConnector(Track::EndType::front) == nullptr );
}

BOOST_FIXTURE_TEST_CASE( flippingTracksConnectedBySwitch, SwichFixture ){
	unique_ptr<Switch> pSwitch = Switch::Make();
	pSwitch->NarrowTrack( m_pTrack1, Track::EndType::end );
	pSwitch->StraightTrack( m_pTrack2, Track::EndType::front );
	pSwitch->DivergedTrack( m_pTrack3, Track::EndType::front );
	pSwitch->Set( Switch::Status::go );

	m_pTrack1->Flip(true);
	BOOST_CHECK_EQUAL( pSwitch->NarrowTrack().first, m_pTrack1 );
	BOOST_CHECK_EQUAL( pSwitch->NarrowTrack().second, Track::EndType::front );

	m_pTrack2->Flip(true);
	BOOST_CHECK_EQUAL( pSwitch->StraightTrack().first, m_pTrack2 );
	BOOST_CHECK_EQUAL( pSwitch->StraightTrack().second, Track::EndType::end );

	m_pTrack3->Flip(true);
	BOOST_CHECK_EQUAL( pSwitch->DivergedTrack().first, m_pTrack3 );
	BOOST_CHECK_EQUAL( pSwitch->DivergedTrack().second, Track::EndType::end );
}

BOOST_FIXTURE_TEST_CASE( NarrowSwitchWithFourOutgoingTracksCreationtest, SwichFixture ){
	unique_ptr<NarrowSwitch> pSwitch = NarrowSwitch::Make( 3 );
	pSwitch->NarrowTrack( m_pTrack1, Track::EndType::end );
	pSwitch->StraightTrack( m_pTrack2, Track::EndType::front );
	pSwitch->DivergedTrack( 2, m_pTrack3, Track::EndType::front );
	pSwitch->Set( Switch::Status::go );

	BOOST_CHECK_EQUAL( pSwitch->NarrowTrack().first, m_pTrack1 );
	BOOST_CHECK_EQUAL( pSwitch->NarrowTrack().second, Track::EndType::end );

	pSwitch->Set( NarrowSwitch::Status::empty );
	BOOST_CHECK_EQUAL( pSwitch->Get(), NarrowSwitch::Status::branch1 );

	BOOST_CHECK_EQUAL( ToNarrowSwitchStatus( "branch3" ), NarrowSwitch::Status::branch3 );
}

BOOST_FIXTURE_TEST_CASE( NarrowSwitchAccesToNonExistingSlot, SwichFixture ){
	unique_ptr<NarrowSwitch> pSwitch = NarrowSwitch::Make( 3 );
	pSwitch->NarrowTrack( m_pTrack1, Track::EndType::end );
	pSwitch->StraightTrack( m_pTrack2, Track::EndType::front );
	BOOST_CHECK_THROW( pSwitch->DivergedTrack( 3, m_pTrack3, Track::EndType::front ), std::out_of_range );
	pSwitch->Set( Switch::Status::go );
}

BOOST_FIXTURE_TEST_CASE( switchShapeTest, SwichFixture ){
	unique_ptr<Switch> pSwitch = Switch::Make();
	pSwitch->NarrowTrack( m_pTrack1, Track::EndType::end );
	pSwitch->StraightTrack( m_pTrack2, Track::EndType::front );
	pSwitch->DivergedTrack( m_pTrack3, Track::EndType::front );
	pSwitch->Set( Switch::Status::go );

	BOOST_CHECK( pSwitch->IsNormal( false, 5_m ) );
	BOOST_CHECK( pSwitch->IsY( false, 5_m ) );
	BOOST_CHECK( pSwitch->IsNormal( false, 50_m ) );
	BOOST_CHECK( pSwitch->IsY( false, 50_m ) );

	std::shared_ptr<trax::ArcP> pArc = ArcP::Make();
	pArc->Create( { {10_m,0_m,0_m}, {1,0,0} }, {20_m,-30_cm,0_m} );
	m_pTrack2->Attach( pArc, {0_m,10_m} );

	BOOST_CHECK( pSwitch->IsNormal( false, 5_m ) );
	BOOST_CHECK( !pSwitch->IsY( false, 5_m ) );
	BOOST_CHECK( pSwitch->IsNormal( false, 50_m ) );
	BOOST_CHECK( !pSwitch->IsY( false, 50_m ) );

	BOOST_CHECK( !pSwitch->BranchLeftOrRight( false, 5_m ) );

	std::shared_ptr<trax::LineP> pLine = LineP::Make();
	pLine->Create( {10_m,0_m,0_m}, {20_m,0_cm,0_m} );
	m_pTrack2->Attach( pLine, {0_m,10_m} );

	BOOST_CHECK( pSwitch->IsNormal( false, 5_m ) );
	BOOST_CHECK( !pSwitch->IsY( false, 5_m ) );
	BOOST_CHECK( pSwitch->IsNormal( false, 50_m ) );
	BOOST_CHECK( !pSwitch->IsY( false, 50_m ) );

	BOOST_CHECK( !pSwitch->BranchLeftOrRight( false, 5_m ) );
}

BOOST_AUTO_TEST_SUITE_END() //Switch_tests
BOOST_AUTO_TEST_SUITE_END() //trax_tests
#endif
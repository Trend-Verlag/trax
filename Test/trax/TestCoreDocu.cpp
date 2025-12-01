//	trax track library
//	AD 2025
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

#include "common/support/CommonSupportStream.h"
#include "dim/DimensionedValues.h"
#include "dim/support/DimSupportStream.h"
#include "spat/Spatial.h"
#include "spat/support/SpatSupportStream.h"

#include "trax/Trax.h"

using namespace common;
using namespace dim;
using namespace spat;
using namespace trax;

BOOST_AUTO_TEST_SUITE(trax_tests)
BOOST_AUTO_TEST_SUITE(CoreDocu)

BOOST_AUTO_TEST_CASE( test_docu_units )
{
	{
		Position<Length> P1 = Origin3D<Length>;		// or any other point in the Frame's local coordinates
		Frame<Length,One> F = Identity<Length,One>; // or any other orthonormal Frame in global coordinates
	
		Position<Length> Pglobal = F.P + P1.x * F.T + P1.y * F.N + P1.z * F.B;
	
		BOOST_CHECK( Pglobal == F.ToParent( P1 ) );
		BOOST_CHECK( Pglobal == F * P1  );
		BOOST_CHECK( P1 == F.FromParent( Pglobal ) );

		Position<Length> P2{ 1_m, 2_m, 3_m };	// pointin the Frame's local coordinates
		Vector<Length> V = P2 - P1;				// distance vector in local coordinates
		Vector<Length> Vglobal = V.dx * F.T + V.dy * F.N + V.dz * F.B;
		
		BOOST_CHECK( Vglobal == F.ToParent( V ) );
		BOOST_CHECK( Vglobal == F * V  );
		BOOST_CHECK( V == F.FromParent( Vglobal ) );
	}

	Mass m = 130_kg;
	Vector<Acceleration> a = G;	// acceleration due to gravity
	Vector<Force> F = m * a;
	std::cout << "the weight of a mass of " << m << " is " << F << std::endl; // prints: "the weight of a mass of 130kg is Vector( -0kN, -0kN, -1.2753kN )"

	Vector<AngularVelocity> w{ Ez<One> * 20_deg / 1_s };
	Vector<Length> d{ Ex<One> * 2.8_m };
	Vector<Velocity> v = w % d;
	std::cout << "the momentary velocity of a point rotating around " << w << " at distance " << d << ": " << v << std::endl; // prints: "[...]  Vector( 0km/h, 3.52km/h, 0km/h )" 

	Position<Length> P1{1_m,2_m,3_m};
	Position<Length> P2{1_m,1_m,1_m};

	Vector<Length> D = P2 - P1;
	BOOST_CHECK( D.Length() == sqrt(5_m2) );

	Vector<Velocity> V = D / 2_s;
	BOOST_CHECK( V.Length() != 2.5_mIs );
}

BOOST_AUTO_TEST_CASE( test_docu_curve )
{
	auto pArc = ArcP::Make();
	Interval<Length> rangeArc = pArc->Create( {{0_m,0_m,0_m},{1,0,0}}, {10_m,10_m,0_m} );
	BOOST_CHECK( rangeArc.Equals( {0_m,pi/2 * 10_m}, epsilon__length ) );

	auto curve = CreateCurve( {{0_m,0_m,0_m},{1,0,0}}, {{10_m,10_m,0_m},{0,1,0}}, spat::Ez<One> );
	BOOST_CHECK( curve.second.Equals( {0_m,pi/2 * 10_m}, epsilon__length ) );
	BOOST_CHECK_EQUAL( curve.first->TypeName(), "Arc" );

	auto pCubic = Cubic::Make();
	spat::VectorBundle<Length,Length> start{ {10_m, 10_m, 10_m}, {250_m,0_m,0_m} };
	spat::VectorBundle<Length,Length> end{ {10_m,70_m,10_m}, {250_m,0_m,0_m} };
	auto rangeCubic = pCubic->Create( start, end );
	std::cout << "Cubic curve created with range: " << rangeCubic << std::endl;
	BOOST_CHECK( pCubic->IsValid() );
}

BOOST_AUTO_TEST_CASE( test_docu_twist )
{
	auto pTrack = TrackBuilder::Make();
	BOOST_CHECK( pTrack );
	auto pCombinedTwist = CombinedTwist::Make();
	pCombinedTwist->AttachTwist1( DirectionalTwist::Make(Up) );	
	pCombinedTwist->AttachTwist2( LinearTwist::Make( 0_deg, 7_deg ) );
	
	pTrack->Attach( std::move(pCombinedTwist) );
}

BOOST_AUTO_TEST_CASE( test_docu_track )
{
	auto pTrack = TrackBuilder::Make();
	std::shared_ptr<RotatorChain> pCurve = RotatorChain::Make();
	pCurve->Create( RotatorChain::Data{ 
		{ 20_deg, 10_deg, 90_m }, 
		{ 20_deg, -10_deg, 60_m }, 
		{ -40_deg, 0_deg, 50_m } } );
	pTrack->Attach( pCurve, { 0_m, 90_m+60_m+50_m} );
	BOOST_CHECK( pTrack->IsValid() );


	spat::Frame<Length,One> myPose{ {100_m,200_m,300_m},
									{0,1,0},
									{0,0,1},
									{1,0,0} };
	pTrack->SetFrame( myPose, 0_m );


	pTrack->SetFrame( myPose, pTrack->Range().Center() );


	auto curve = pTrack->DetachCurve();
	// do something with the curve ...
	pTrack->Attach( curve );


	spat::Frame<Length,One> frame = pTrack->GetFrame();
	curve = pTrack->GetCurve();
	auto pTwist = pTrack->GetTwist().Clone();
	// something else is happening to the track ...
	// ...
	// For some reason restore the track geometry:
	pTrack->Attach( std::move(pTwist) );
	pTrack->Attach( curve );
	pTrack->SetFrame( frame );


	spat::VectorBundle<Length,One> start{ {10_m, 10_m, 10_m}, {250,0,0} };
	spat::VectorBundle<Length,One> end{ {10_m,70_m,10_m}, {250,0,0} };
	Strech( *pTrack, start, end );
	BOOST_CHECK( pTrack->IsValid() );
}

BOOST_AUTO_TEST_CASE( test_docu_location )
{ 
	auto pTrack1 = TrackBuilder::Make();
	auto pTrack2 = TrackBuilder::Make();
	auto pTrack3 = TrackBuilder::Make();
	auto pTrack4 = TrackBuilder::Make();
 	
	// We build a track circle with radius 10_m around the Origin3D from the four pieces:
	auto pArc1 = ArcP::Make();
	pArc1->Create( { Origin3D<Length>, { 1, 0, 0 }, { 0, 10, 0 } } );
	pTrack1->Attach( std::move(pArc1), {0_m,10_m*pi/2} );

	auto pArc2 = ArcP::Make();
	pArc2->Create( { Origin3D<Length>, { 0, 1, 0 }, { -10, 0, 0 } } );
	pTrack2->Attach( std::move(pArc2), {0_m,10_m*pi/2} );

	auto pArc3 = ArcP::Make();
	pArc3->Create( { Origin3D<Length>, { -1, 0, 0 }, { 0, -10, 0 } } );
	pTrack3->Attach( std::move(pArc3), {0_m,10_m*pi/2} );

	auto pArc4 = ArcP::Make();
	pArc4->Create( { Origin3D<Length>, { 0, -1, 0 }, { 10, 0, 0 } } );
	pTrack4->Attach( std::move(pArc4), {0_m, 10_m*pi/2} );
 
	pTrack1->Couple( std::make_pair(pTrack1, Track::EndType::end), std::make_pair(pTrack2, Track::EndType::front) );
	pTrack2->Couple( std::make_pair(pTrack2, Track::EndType::end), std::make_pair(pTrack3, Track::EndType::front) );
	pTrack3->Couple( std::make_pair(pTrack3, Track::EndType::end), std::make_pair(pTrack4, Track::EndType::front) );
	pTrack4->Couple( std::make_pair(pTrack4, Track::EndType::end), std::make_pair(pTrack1, Track::EndType::front) );
 
	Location loc{ pTrack1, { 0_m, Orientation::Value::para } };
 
	Frame<Length,One> frame1, frame2;
	loc.Transition( frame1 );
	loc.Move( pTrack1->GetLength() + pTrack2->GetLength() + pTrack3->GetLength() + pTrack4->GetLength() );
	loc.Transition( frame2 );
	BOOST_CHECK( frame1.Equals( frame2, epsilon__length, 0.001f ) );
 
	loc.Move( pTrack1->GetLength() + pTrack2->GetLength() + pTrack3->GetLength()/2 );
	auto pTrack = loc.GetTrack();
	BOOST_CHECK( pTrack == pTrack3 );
 
	pTrack1->DeCouple(); // Coupled tracks will prevent each others destruction \see TrackSystem
	pTrack3->DeCouple();
}

BOOST_AUTO_TEST_SUITE_END() //CoreDocu
BOOST_AUTO_TEST_SUITE_END() //trax_tests
#endif

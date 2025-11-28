//	trax track library
//	AD 2018
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

#if defined( WITH_BOOST_TESTS )
#include <boost/test/unit_test.hpp> // NOLINT 

#include "trax/Curve.h"
#include "trax/support/TraxSupportStream.h"
#include "trax/support/Fixtures.h"
#include "../Test/dim/BoostTestDimensionedValuesHelpers.h"
#include "../Test/spat/BoostTestSpatialHelpers.h"

using namespace trax;
using namespace spat;

BOOST_AUTO_TEST_SUITE(Curve)

BOOST_AUTO_TEST_CASE( testDimensionMakros )
{
	Velocity v1{20};
	Velocity v2{20.1f};
	//BOOST_CHECK_CLOSE_DIMENSION( v1, v2, 0.49f ); //fails
	BOOST_CHECK_CLOSE_DIMENSION( v1, v2, 0.51f ); //succeeds

	//BOOST_CHECK_SMALL_DIMENSION( v2 - v1, Velocity{0.05f} ); //fails
	BOOST_CHECK_SMALL( v2 - v1, 0.11_mIs ); //succeeds
}

BOOST_AUTO_TEST_CASE( testUpNotEx )
{
	BOOST_CHECK_NE( Up, Ex<One> );
}

BOOST_AUTO_TEST_CASE( lineInUpDirection )
{
	auto pCurve = LineP::Make();
	pCurve->Create( {{0_m,0_m,0_m},Up} );
	BOOST_CHECK( pCurve->IsValid() );

	Frame<Length,One> frame;
	pCurve->Transition( 0_m, frame );
	BOOST_CHECK( frame.IsOrthoNormal() );
	BOOST_CHECK_NE( Up, Ex<One> );
}

BOOST_AUTO_TEST_CASE( arcRoundTrip )
{
	auto pCurve = ArcP::Make();
	BOOST_CHECK_SMALL( pCurve->Create( {{0_m,0_m,0_m},{1,0,0}}, {10_m,10_m,0_m} ).Far() - trax::pi/2 * 10_m, trax::epsilon__length );

	trax::ArcP::Data data{ pCurve->GetData() };
	auto pCurveB = ArcP::Make();
	pCurveB->Create( data );
	trax::ArcP::Data dataB{ pCurveB->GetData() };
	BOOST_CHECK( data.vb2 == dataB.vb2 );
}

BOOST_AUTO_TEST_CASE( initalisationFromData ){
	spat::Position<Length> pos;

	auto pCurve1 = LineP::Make();
	pCurve1->Create( trax::LineP::Data{} );
	BOOST_CHECK_EQUAL( pCurve1->IsValid(), true );
	pCurve1->Transition( 0_m, pos );
	BOOST_CHECK( pos.Equals( spat::Origin3D<Length>, trax::epsilon__length ) );
	pCurve1->Transition( 0.1f * trax::epsilon__length, pos );
	BOOST_CHECK( pos.Equals( spat::Origin3D<Length>, trax::epsilon__length ) );

	auto pCurve2 = ArcP::Make();
	pCurve2->Create( trax::ArcP::Data{} );
	BOOST_CHECK_EQUAL( pCurve2->IsValid(), true );
	pCurve2->Transition( 0_m, pos );
	BOOST_CHECK( pos.Equals( spat::Origin3D<Length>- 10_m * spat::Ey<One>, trax::epsilon__length ) );
	pCurve2->Transition( 0.1f * trax::epsilon__length, pos );
	BOOST_CHECK( pos.Equals( spat::Origin3D<Length>- 10_m * spat::Ey<One>, trax::epsilon__length ) );

	auto pCurve3 = HelixP::Make();
	pCurve3->Create( trax::HelixP::Data{} );
	BOOST_CHECK_EQUAL( pCurve3->IsValid(), true );
	pCurve3->Transition( 0_m, pos );
	BOOST_CHECK( pos.Equals( spat::Origin3D<Length>+10_m*spat::Ex<One>, trax::epsilon__length ) );
	pCurve3->Transition( 0.1f * trax::epsilon__length, pos );
	BOOST_CHECK( pos.Equals( spat::Origin3D<Length>+10_m*spat::Ex<One>, trax::epsilon__length ) );

	auto pCurve4 = Cubic::Make();
	pCurve4->Create( trax::Cubic::Data{} );
	BOOST_CHECK_EQUAL( pCurve4->IsValid(), true );
	pCurve4->Transition( 0_m, pos );
	BOOST_CHECK( pos.Equals( spat::Origin3D<Length>, trax::epsilon__length ) );
	pCurve4->Transition( 0.1f * trax::epsilon__length, pos );
	BOOST_CHECK( pos.Equals( spat::Origin3D<Length>, trax::epsilon__length ) );

	auto pCurve6 = Clothoid::Make();
	pCurve6->Create( trax::Clothoid::Data{} );
	BOOST_CHECK_EQUAL( pCurve6->IsValid(), true );
	pCurve6->Transition( 0_m, pos );
	BOOST_CHECK( pos.Equals( spat::Origin3D<Length>, trax::epsilon__length ) );
	pCurve6->Transition( 0.1f * trax::epsilon__length, pos );
	BOOST_CHECK( pos.Equals( spat::Origin3D<Length>, trax::epsilon__length ) );
}

BOOST_AUTO_TEST_CASE( test_HelixWriteRead )
{
	auto pHelix = HelixP::Make();
	HelixP::Data data{ {{0_m,-36.47577_m,0_m}, {0,1,0}, {1,0,0}}, 36.47577_m, -6.74914_m };
	pHelix->Create( data );

	BOOST_CHECK_EQUAL( pHelix->GetData().center, data.center );
	BOOST_CHECK_EQUAL( pHelix->GetData().a, data.a );
	BOOST_CHECK_EQUAL( pHelix->GetData().b, data.b );
}

BOOST_AUTO_TEST_SUITE(CurveArc)

BOOST_AUTO_TEST_CASE( curveArc_TBN_correctness )
{
	auto pArc1 = ArcP::Make();
	pArc1->Create( {0_m,10_m,0_m}, {1,0,0}, {0,1,0}, 1.0f/10_m );

	spat::Frame<Length,One> frame;
	pArc1->Transition( 5_m, frame );

	BOOST_CHECK_GT( frame.P.y , 10_m ); // Test for correct development
}

BOOST_AUTO_TEST_CASE( curveArcFullCircle )
{
	auto pArc = ArcP::Make();
	pArc->Create( {0_m,10_m,0_m}, {1,0,0}, {0,1,0}, 1.0f/10_m );

	spat::Frame<Length,One> frameA, frameB;
	pArc->Transition( 0_m, frameA );
	pArc->Transition( 10_m * 2*trax::pi, frameB );

	BOOST_CHECK( frameA.Equals( frameB, trax::epsilon__length, trax::epsilon__angle ) ); 
}

BOOST_AUTO_TEST_CASE( arcCreationFromEndpoint1 )
{
	auto pArc = ArcP::Make();

	spat::VectorBundle<Length,One> start{{10_m,0_m,0_m},{0,1,0}};
	spat::Position<Length> end{0_m,10_m,0_m};
	spat::VectorBundle<Length,One> startCalculated;
	spat::Position<Length> endCalculated;

	Length length1 = trax::pi/2*10_m;
	Length length2 = 3*trax::pi/2*10_m;

	BOOST_CHECK_CLOSE_DIMENSION( pArc->Create( start, end ).Far(), length1, 100 * trax::epsilon__length / length1 );
	pArc->Transition( 0_m, startCalculated );
	BOOST_CHECK(startCalculated.Equals( start, trax::epsilon__length ));
	pArc->Transition( length1, endCalculated );
	BOOST_CHECK(endCalculated.Equals( end, trax::epsilon__length ));

	start.T *= -1;
	BOOST_CHECK_CLOSE_DIMENSION( pArc->Create( start, end ).Far(), length2, 100 * trax::epsilon__length  / length2 );
	pArc->Transition( 0_m, startCalculated );
	BOOST_CHECK(startCalculated.Equals( start, trax::epsilon__length ));
	pArc->Transition( length2, endCalculated );
	BOOST_CHECK(endCalculated.Equals( end, trax::epsilon__length ));

	start.P.x = -10_m;
	BOOST_CHECK_CLOSE_DIMENSION( pArc->Create( start, end ).Far(), length2, 100 * trax::epsilon__length  / length2 );
	pArc->Transition( 0_m, startCalculated );
	BOOST_CHECK(startCalculated.Equals( start, trax::epsilon__length ));
	pArc->Transition( length2, endCalculated );
	BOOST_CHECK(endCalculated.Equals( end, trax::epsilon__length ));

	start.T *= -1;
	BOOST_CHECK_CLOSE_DIMENSION( pArc->Create( start, end ).Far(), length1, 100 * trax::epsilon__length / length1  );
	pArc->Transition( 0_m, startCalculated );
	BOOST_CHECK(startCalculated.Equals( start, trax::epsilon__length ));
	pArc->Transition( length1, endCalculated );
	BOOST_CHECK(endCalculated.Equals( end, trax::epsilon__length ));
}

BOOST_AUTO_TEST_CASE( arcCreationFromEndpoint2 )
{
	auto pArc = ArcP::Make();

	spat::Position<Length> start{10_m,0_m,0_m};
	spat::VectorBundle<Length,One> end{{0_m,10_m,0_m},{-1,0,0}};
	spat::Position<Length> startCalculated;
	spat::VectorBundle<Length,One> endCalculated;

	Length length1 = trax::pi/2*10_m;
	Length length2 = 3*trax::pi/2*10_m;

	BOOST_CHECK_CLOSE_DIMENSION( pArc->Create( start, end ).Far(), length1, 100 * trax::epsilon__length / length1 );
	pArc->Transition( 0_m, startCalculated );
	BOOST_CHECK_CLOSE_SPATIAL( startCalculated, start, trax::epsilon__length );
	pArc->Transition( length1, endCalculated );
	BOOST_CHECK_CLOSE_SPATIAL( endCalculated, end, trax::epsilon__length );

	end.T *= -1;
	BOOST_CHECK_CLOSE_DIMENSION( pArc->Create( start, end ).Far(), length2, 100 * trax::epsilon__length  / length2 );
	pArc->Transition( 0_m, startCalculated );
	BOOST_CHECK_CLOSE_SPATIAL( startCalculated, start, trax::epsilon__length );
	pArc->Transition( length2, endCalculated );
	BOOST_CHECK_CLOSE_SPATIAL2( endCalculated, end, trax::epsilon__length, std::numeric_limits<float>::epsilon() );

	start.x = -10_m;
	BOOST_CHECK_CLOSE_DIMENSION( pArc->Create( start, end ).Far(), length1, 100 * trax::epsilon__length / length1  );
	pArc->Transition( 0_m, startCalculated );
	BOOST_CHECK_CLOSE_SPATIAL( startCalculated, start, trax::epsilon__length );
	pArc->Transition( length1, endCalculated );
	BOOST_CHECK_CLOSE_SPATIAL( endCalculated, end, trax::epsilon__length );

	end.T *= -1;
	BOOST_CHECK_CLOSE_DIMENSION( pArc->Create( start, end ).Far(), length2, 100 * trax::epsilon__length  / length2 );
	pArc->Transition( 0_m, startCalculated );
	BOOST_CHECK_CLOSE_SPATIAL( startCalculated, start, trax::epsilon__length );
	pArc->Transition( length2, endCalculated );
	BOOST_CHECK_CLOSE_SPATIAL2( endCalculated, end, trax::epsilon__length, std::numeric_limits<float>::epsilon() );
}
BOOST_AUTO_TEST_SUITE_END() // CurveArc
BOOST_AUTO_TEST_SUITE(CurveHelix)

BOOST_AUTO_TEST_CASE( helixCreateFromEndPoints ){
	auto pHelix = HelixP::Make();

	spat::VectorBundle<Length,One> start{{10_m,0_m,0_m},{0,1,0}};
	spat::Position<Length> end{0_m,10_m,5_m};
	spat::Position<Length> startCalculated;
	spat::Position<Length> endCalculated;

	Length length1 = pHelix->Create( start, end,{0,0,1} ).Far();
	pHelix->Transition( 0_m, startCalculated );
	BOOST_CHECK(startCalculated.Equals( start.P, trax::epsilon__length ));
	pHelix->Transition( length1, endCalculated );
	BOOST_CHECK(endCalculated.Equals( end, trax::epsilon__length ));

	start.T *= -1;
	Length length2 = pHelix->Create( start, end,{0,0,1} ).Far();
	pHelix->Transition( 0_m, startCalculated );
	BOOST_CHECK(startCalculated.Equals( start.P, trax::epsilon__length ));
	pHelix->Transition( length2, endCalculated );
	BOOST_CHECK(endCalculated.Equals( end, trax::epsilon__length ));

	start.P.x = -10_m;
	BOOST_CHECK_EQUAL( pHelix->Create( start, end,{0,0,1} ).Far(), length2 );
	pHelix->Transition( 0_m, startCalculated );
	BOOST_CHECK(startCalculated.Equals( start.P, trax::epsilon__length ));
	pHelix->Transition( length2, endCalculated );
	BOOST_CHECK(endCalculated.Equals( end, trax::epsilon__length ));

	start.T *= -1;
	BOOST_CHECK_EQUAL( pHelix->Create( start, end,{0,0,1} ).Far(), length1 );
	pHelix->Transition( 0_m, startCalculated );
	BOOST_CHECK(startCalculated.Equals( start.P, trax::epsilon__length ));
	pHelix->Transition( length1, endCalculated );
	BOOST_CHECK(endCalculated.Equals( end, trax::epsilon__length ));
}

BOOST_AUTO_TEST_SUITE_END() // CurveHelix
BOOST_AUTO_TEST_SUITE(CurveClothoid)

BOOST_AUTO_TEST_CASE( clothoidSmallEndPoints ){
	using namespace trax;
	auto pCurve = Clothoid::Make();
	Length length = pCurve->Create( 100_m2 ).Far();

	spat::Frame<Length,One> frame;
	pCurve->Transition( 0_m, frame );
	BOOST_CHECK( frame.Equals( spat::Identity<Length,One>, trax::epsilon__length, trax::epsilon__angle ) );

	pCurve->Transition( length, frame );
	BOOST_CHECK( frame.T.Equals( {-1,0,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.N.Equals( {0,-1,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals( {0,0,1}, trax::epsilon__angle ) );
}

BOOST_AUTO_TEST_CASE( clothoidLargeEndPoints ){
	using namespace trax;
	auto pCurve = Clothoid::Make();
	Length length = pCurve->Create( 10000_m2 ).Far();

	spat::Frame<Length,One> frame;
	pCurve->Transition( 0_m, frame );
	BOOST_CHECK( frame.Equals( spat::Identity<Length,One>, trax::epsilon__length, trax::epsilon__angle ) );

	pCurve->Transition( length, frame );
	BOOST_CHECK( frame.T.Equals( {-1,0,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.N.Equals( {0,-1,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals( {0,0,1}, trax::epsilon__angle ) );
}

BOOST_AUTO_TEST_SUITE_END() // CurveClothoid

BOOST_AUTO_TEST_SUITE(CurveCubic)

BOOST_AUTO_TEST_CASE( cubicZeroSet ){

	auto pCurve = Cubic::Make();
	spat::VectorBundle<Length,Length> start{{10_m, 10_m, 10_m},{250_m,0_m,0_m}};
	spat::VectorBundle<Length,Length> end{{10_m,70_m,10_m},{250_m,0_m,0_m}};
	pCurve->Create( start, end );

	std::vector<Length> zeroSet = pCurve->ZeroSet();
	BOOST_CHECK_EQUAL( zeroSet.size(), 1u );
}

BOOST_AUTO_TEST_CASE( straightLineCubic ){
	auto pCurve = Cubic::Make();
	spat::VectorBundle<Length,Length> start{{0_m, 0_m, 0_m},{1_m,0_m,0_m}};
	spat::VectorBundle<Length,Length> end{{10_m,0_m,0_m},{1_m,0_m,0_m}};
	Length length = pCurve->Create( start, end ).Far();
	BOOST_CHECK_SMALL( length - 10_m, trax::epsilon__length );

	std::vector<Length> zeroSet = pCurve->ZeroSet();
	BOOST_CHECK_EQUAL( zeroSet.size(), 0u );
	BOOST_CHECK_SMALL( pCurve->Curvature( length / 2 ), epsilon__curvature );
	BOOST_CHECK_SMALL( pCurve->Torsion( length / 2 ), epsilon__curvature );

	spat::Frame<Length,One> frame;
	pCurve->Transition( 0_m, frame );

	BOOST_CHECK( frame.Equals( spat::Identity<Length,One>, trax::epsilon__length, trax::epsilon__angle ) );
	pCurve->Transition( 10_m, frame );
	BOOST_CHECK( frame.P.Equals( {10_m,0_m,0_m}, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals( {1,0,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.N.Equals( {0,1,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals( {0,0,1}, trax::epsilon__angle ) );
}

BOOST_AUTO_TEST_CASE( straightLineCubic2 ){
	auto pCurve = Cubic::Make();
	spat::VectorBundle<Length,Length> start{{200_m, -100_m, 50_m},{0.7071_m,-0.7071_m,0_m}};
	spat::VectorBundle<Length,Length> end{{300_m,-200_m,50_m},{0.7071_m,-0.7071_m,0_m}};
	Length length = pCurve->Create( start, end ).Far();
	BOOST_CHECK_SMALL( length - 141.42_m, trax::epsilon__length );

	std::vector<Length> zeroSet = pCurve->ZeroSet();
	BOOST_CHECK_EQUAL( zeroSet.size(), 0u );
	BOOST_CHECK_SMALL( pCurve->Curvature( length / 2 ), trax::epsilon__curvature );
	BOOST_CHECK_SMALL( pCurve->Torsion( length / 2 ), trax::epsilon__curvature );

	spat::Frame<Length,One> frame;
	pCurve->Transition( 0_m, frame );
	BOOST_CHECK( frame.P.Equals( start.P, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals( {0.7071f,-0.7071f,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.N.Equals( {0.7071f,0.7071f,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals( {0,0,1}, trax::epsilon__angle ) );

	pCurve->Transition( length/2, frame );
	BOOST_CHECK_CLOSE_SPATIAL( frame.P, start.P + (end.P - start.P) / 2, 2*trax::epsilon__length );
	BOOST_CHECK( frame.T.Equals( {0.7071f,-0.7071f,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.N.Equals( {0.7071f,0.7071f,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals( {0,0,1}, trax::epsilon__angle ) );

	pCurve->Transition( length, frame );
	BOOST_CHECK( frame.P.Equals( end.P, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals( {0.7071f,-0.7071f,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.N.Equals( {0.7071f,0.7071f,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals( {0,0,1}, trax::epsilon__angle ) );
}

BOOST_AUTO_TEST_CASE( straightLineCubic3 ){
	auto pCurve = Cubic::Make();
	spat::VectorBundle<Length,Length> start{{0_m, 0_m, 0_m},{0_m,0_m,10_m}};
	spat::VectorBundle<Length,Length> end{{0_m,0_m,10_m},{0_m,0_m,40_m}};
	Length length = pCurve->Create( start, end ).Far();
	BOOST_CHECK_SMALL( length - 10_m, trax::epsilon__length );

	std::vector<Length> zeroSet = pCurve->ZeroSet();
	BOOST_CHECK_EQUAL( zeroSet.size(), 0u );
	BOOST_CHECK_EQUAL( pCurve->Curvature( length / 2 ), 0_1Im );
	BOOST_CHECK_EQUAL( pCurve->Torsion( length / 2 ), 0_1Im );

	spat::Frame<Length,One> frame;
	pCurve->Transition( length/2, frame );

	BOOST_CHECK( frame.P.Equals( {0_m,0_m,5_m}, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals( {0,0,1}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.N.Equals( {1,0,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals( {0,1,0}, trax::epsilon__angle ) );
}

BOOST_AUTO_TEST_CASE( circleArcQuarterCubic ){
	auto pCurve = Cubic::Make();
	spat::VectorBundle<Length,Length> start{{0_m, 0_m, 0_m},{1_m,0_m,0_m}};
	spat::VectorBundle<Length,Length> end{{5_m,5_m,0_m},{0_m,1_m,0_m}};
	Length length = pCurve->Create( start, end ).Far();

	spat::Frame<Length,One> frame;
	pCurve->Transition( 0_m, frame );
	BOOST_CHECK( frame.P.Equals( {0_m,0_m,0_m}, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals( {1,0,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.N.Equals( {0,1,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals( {0,0,1}, trax::epsilon__angle ) );

	pCurve->Transition( length, frame );
	BOOST_CHECK( frame.P.Equals( {5_m,5_m,0_m}, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals( {0,1,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.N.Equals( {-1,0,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals( {0,0,1}, trax::epsilon__angle ) );
}

BOOST_AUTO_TEST_CASE( circleArcQuarterHugheCubic ){
	auto pCurve = Cubic::Make();
	spat::VectorBundle<Length,Length> start{{0_m, 0_m, 1_m},{150_m,0_m,0_m}};
	spat::VectorBundle<Length,Length> end{{50_m,50_m,1_m},{0_m,150_m,0_m}};
	Length length = pCurve->Create( start, end ).Far();

	spat::Frame<Length,One> frame;
	pCurve->Transition( 0_m, frame );
	BOOST_CHECK( frame.P.Equals( {0_m,0_m,1_m}, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals( {1,0,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.N.Equals( {0,1,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals( {0,0,1}, trax::epsilon__angle ) );

	pCurve->Transition( length, frame );
	BOOST_CHECK( frame.P.Equals( {50_m,50_m,1_m}, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals( {0,1,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.N.Equals( {-1,0,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals( {0,0,1}, trax::epsilon__angle ) );
}

BOOST_AUTO_TEST_CASE( circleArcCubic2 ){
	auto pCurve = Cubic::Make();
	spat::VectorBundle<Length,Length> start{{0_m, 0_m, 0_m},{10_m,0_m,0_m}};
	spat::VectorBundle<Length,Length> end{{0_m,50_m,0_m},{0_m,1_m,0_m}};
	Length length = pCurve->Create( start, end ).Far();

	spat::Frame<Length,One> frame;
	pCurve->Transition( 0_m, frame );
	BOOST_CHECK( frame.P.Equals( {0_m,0_m,0_m}, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals( {1,0,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.N.Equals( {0,1,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals( {0,0,1}, trax::epsilon__angle ) );

	std::vector<Length> zeroSet = pCurve->ZeroSet();
	BOOST_CHECK_EQUAL( zeroSet.size(), 1u );

	pCurve->Transition( length, frame );
	BOOST_CHECK( frame.P.Equals( {0_m,50_m,0_m}, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals( {0,1,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.N.Equals( {1,0,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals( {0,0,-1}, trax::epsilon__angle ) );
}

BOOST_AUTO_TEST_CASE( circleArcHalfCubic ){
	auto pCurve = Cubic::Make();
	spat::VectorBundle<Length,Length> start{{0_m, 0_m, 0_m},{150_m,0_m,0_m}};
	spat::VectorBundle<Length,Length> end{{0_m,10_m,0_m},{-150_m,0_m,0_m}};
	Length length = pCurve->Create( start, end ).Far();

	spat::Frame<Length,One> frame;
	pCurve->Transition( 0_m, frame );
	BOOST_CHECK( frame.P.Equals( {0_m,0_m,0_m}, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals( {1,0,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.N.Equals( {0,1,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals( {0,0,1}, trax::epsilon__angle ) );

	pCurve->Transition( length, frame );
	BOOST_CHECK( frame.P.Equals( {0_m,10_m,0_m}, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals( {-1,0,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.N.Equals( {0,-1,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals( {0,0,1}, trax::epsilon__angle ) );
}

BOOST_AUTO_TEST_CASE( cubicNarrowTurnCurve ){
	auto pCurve = Cubic::Make();
	spat::VectorBundle<Length,Length> start{{-0.7999992_m, -20.1999989_m, 0.299999982_m},{1_m,0_m,0_m}};
	spat::VectorBundle<Length,Length> end{{73.4591599_m,1.86029792_m,5.29999876_m},{-0.925743997_m,0.378151_m,0_m}};
	Length length = pCurve->Create( start, end ).Far();

	spat::VectorBundle<Length,One> fibre;
	pCurve->Transition( 0_m, fibre );
	BOOST_CHECK( fibre.P.Equals( start.P, trax::epsilon__length ) );
	BOOST_CHECK( fibre.T.Equals( start.T / 1_m, trax::epsilon__angle ) );

	pCurve->Transition( length, fibre );
	BOOST_CHECK( fibre.P.Equals( end.P, trax::epsilon__length ) );
	BOOST_CHECK( fibre.T.Equals( end.T / 1_m, trax::epsilon__angle ) );
}

BOOST_AUTO_TEST_CASE( cubicCurvesDifferentStartVectors ){
	const spat::VectorBundle<Length,Length> start{{0_m,0_m,1_m},{0.7071_m,0.7071_m,0_m}};
	const spat::VectorBundle<Length,Length> start1{{0_m,0_m,1_m},{10_m,10_m,0_m}};
	const spat::VectorBundle<Length,Length> start2{{0_m,0_m,1_m},{100_m,100_m,0_m}};
	const spat::VectorBundle<Length,Length> start3{{0_m,0_m,1_m},{500_m,500_m,0_m}};
	const spat::VectorBundle<Length,Length> end{{100_m,0_m,1_m},{0_m,-1_m,0_m}};
	spat::VectorBundle<Length,One> fibre;
	spat::Frame<Length,One> frame;

	auto pCurve1 = Cubic::Make();
	Length length = pCurve1->Create( start1, end ).Far();
	pCurve1->Transition( 0_m, fibre );
	BOOST_CHECK( fibre.P.Equals( start.P, trax::epsilon__length ) );
	BOOST_CHECK( fibre.T.Equals( start.T / 1_m, trax::epsilon__angle ) );
	pCurve1->Transition( length, fibre );
	BOOST_CHECK( fibre.P.Equals( end.P, trax::epsilon__length ) );
	BOOST_CHECK( fibre.T.Equals( end.T / 1_m, trax::epsilon__angle ) );
	std::vector<Length> zeroSet = pCurve1->ZeroSet();
	BOOST_CHECK_EQUAL( zeroSet.size(), 0u );
	pCurve1->Transition( length, frame );
	BOOST_CHECK( frame.N.Equals( {-1,0,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals( {0,0,-1}, trax::epsilon__angle ) );

	auto pCurve2 = Cubic::Make();
	length = pCurve2->Create( start2, end ).Far();
	pCurve2->Transition( 0_m, fibre );
	BOOST_CHECK( fibre.P.Equals( start.P, trax::epsilon__length ) );
	BOOST_CHECK( fibre.T.Equals( start.T / 1_m, trax::epsilon__angle ) );
	pCurve2->Transition( length, fibre );
	BOOST_CHECK( fibre.P.Equals( end.P, trax::epsilon__length ) );
	BOOST_CHECK( fibre.T.Equals( end.T / 1_m, trax::epsilon__angle ) );
	zeroSet = pCurve2->ZeroSet();
	BOOST_CHECK_EQUAL( zeroSet.size(), 0u );
	pCurve2->Transition( length, frame );
	BOOST_CHECK( frame.N.Equals( {-1,0,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals( {0,0,-1}, trax::epsilon__angle ) );


	auto pCurve3 = Cubic::Make();
	length = pCurve3->Create( start3, end ).Far();
	pCurve3->Transition( 0_m, fibre );
	BOOST_CHECK( fibre.P.Equals( start.P, trax::epsilon__length ) );
	BOOST_CHECK( fibre.T.Equals( start.T / 1_m, trax::epsilon__angle ) );
	pCurve3->Transition( length, fibre );
	BOOST_CHECK( fibre.P.Equals( end.P, trax::epsilon__length ) );
	BOOST_CHECK( fibre.T.Equals( end.T / 1_m, trax::epsilon__angle ) );
	zeroSet = pCurve3->ZeroSet();
	BOOST_CHECK_EQUAL( zeroSet.size(), 1u );
	pCurve3->Transition( length, frame );
	BOOST_CHECK( frame.N.Equals( {1,0,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals( {0,0,1}, trax::epsilon__angle ) );
}

BOOST_AUTO_TEST_CASE( cubicExtremeCurves ){
	const spat::VectorBundle<Length,Length> start{{0_m,0_m,1_m},{0.7071_m,0.7071_m,0_m}};
	const spat::VectorBundle<Length,Length> start1{{0_m,0_m,1_m}, {353.6_m,353.6_m,0_m}};
	const spat::VectorBundle<Length,Length> start2{{0_m,0_m,1_m}, {500_m,0_m,0_m}};
	const spat::VectorBundle<Length,Length> start3{{0_m,0_m,1_m}, {353.6_m,-353.6_m,0_m}};
	const spat::VectorBundle<Length,Length> end{{100_m,0_m,1_m}, {0_m,-500_m,0_m}};

	auto pCurve1 = Cubic::Make();
	Length length = pCurve1->Create( start1, end ).Far();
	spat::Frame<Length,One> frame;
	pCurve1->Transition( 0_m, frame );
	BOOST_CHECK( frame.P.Equals(start.P, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals({0.7071f,0.7071f,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals({0,0,1}, trax::epsilon__angle ) );
	std::vector<Length> zeroSet = pCurve1->ZeroSet();
	BOOST_CHECK_EQUAL( zeroSet.size(), 2u );
	pCurve1->Transition( length, frame );
	BOOST_CHECK( frame.P.Equals(end.P, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals({0,-1,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals({0,0,1}, trax::epsilon__angle ) );

	auto pCurve2 = Cubic::Make();
	length = pCurve2->Create( start2, end ).Far();
	pCurve2->Transition( 0_m, frame );
	BOOST_CHECK( frame.P.Equals({0_m,0_m,1_m}, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals({1,0,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals({0,0,1}, trax::epsilon__angle ) );
	zeroSet = pCurve2->ZeroSet();
	BOOST_CHECK_EQUAL( zeroSet.size(), 0u );
	pCurve2->Transition( length, frame );
	BOOST_CHECK( frame.P.Equals(end.P, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals({0,-1,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals({0,0,1}, trax::epsilon__angle ) );

	auto pCurve3 = Cubic::Make();
	length = pCurve3->Create( start3, end ).Far();
	pCurve3->Transition( 0_m, frame );
	BOOST_CHECK( frame.P.Equals({0_m,0_m,1_m}, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals({0.7071f,-0.7071f,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals({0,0,1}, trax::epsilon__angle ) );
	zeroSet = pCurve3->ZeroSet();
	BOOST_CHECK_EQUAL( zeroSet.size(), 1u );
	pCurve3->Transition( length, frame );
	BOOST_CHECK( frame.P.Equals(end.P, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals({0,-1,0}, trax::epsilon__angle ) );
	BOOST_CHECK( frame.B.Equals({0,0,1}, trax::epsilon__angle ) );
}

BOOST_AUTO_TEST_CASE( cubic3DCurves ){
	const spat::VectorBundle<Length,Length> start{{100_m,0_m,1_m}, {200_m,0_m,0_m}};
	const spat::VectorBundle<Length,Length> end1{{200_m,-100_m,50_m}, {141_m,-141_m,0_m}};
	const spat::VectorBundle<Length,Length> end2{{200_m,-100_m,50_m}, {0_m,-200_m,0_m}};

	auto pCurve1 = Cubic::Make();
	Length length = pCurve1->Create( start, end1 ).Far();
	spat::Frame<Length,One> frame;
	pCurve1->Transition( 0_m, frame );
	BOOST_CHECK( frame.P.Equals(start.P, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals({1,0,0}, trax::epsilon__angle ) );
	std::vector<Length> zeroSet = pCurve1->ZeroSet();
	BOOST_CHECK_EQUAL( zeroSet.size(), 0u );
	pCurve1->Transition( length, frame );
	BOOST_CHECK( frame.P.Equals(end1.P, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals({0.7071f,-0.7071f,0}, trax::epsilon__angle ) );

	auto pCurve2 = Cubic::Make();
	length = pCurve2->Create( start, end2 ).Far();
	pCurve2->Transition( 0_m, frame );
	BOOST_CHECK( frame.P.Equals(start.P, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals({1,0,0}, trax::epsilon__angle ) );
	zeroSet = pCurve2->ZeroSet();
	BOOST_CHECK_EQUAL( zeroSet.size(), 0u );
	pCurve2->Transition( length, frame );
	BOOST_CHECK( frame.P.Equals(end2.P, trax::epsilon__length ) );
	BOOST_CHECK( frame.T.Equals({0,-1,0}, trax::epsilon__angle ) );
}

BOOST_AUTO_TEST_CASE( cubicOvershoot ){
	const spat::VectorBundle<Length,Length> start{{100_m,100_m,0_m},{0_m,100_m,10_m}};
	const spat::VectorBundle<Length,Length> end{{0_m,200_m,0_m},{0_m,100_m,10_m}};

	auto pCurve = Cubic::Make();
	auto length = pCurve->Create( start, end );

	spat::VectorBundle<Length,Length> newStart;
	spat::VectorBundle<Length,Length> newEnd;
	pCurve->Transition( length.Near(), newStart.P );
	pCurve->Transition( length.Far(), newEnd.P );
	newStart.T = pCurve->GetData().CubicOvershootAtStart();
	newEnd.T = pCurve->GetData().CubicOvershootAtEnd();

	BOOST_CHECK_CLOSE_SPATIAL2( start, newStart, epsilon__length, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL2( end, newEnd, epsilon__length, epsilon__length );
}

BOOST_AUTO_TEST_CASE( cubicCreateSameFromCubicOvershhoots ){
	const spat::VectorBundle<Length,Length> start{{100_m,100_m,0_m},{0_m,100_m,10_m}};
	const spat::VectorBundle<Length,Length> end{{0_m,200_m,0_m},{0_m,100_m,10_m}};

	auto pCurve = Cubic::Make();
	auto length = pCurve->Create( start, end );

	spat::VectorBundle<Length,Length> newStart;
	spat::VectorBundle<Length,Length> newEnd;
	pCurve->Transition( length.Near(), newStart.P );
	pCurve->Transition( length.Far(), newEnd.P );
	newStart.T = pCurve->GetData().CubicOvershootAtStart();
	newEnd.T = pCurve->GetData().CubicOvershootAtEnd();

	auto length2 = pCurve->Create( newStart, newEnd );
	BOOST_CHECK_CLOSE_DIMENSION( length.Length(), length2.Length(), 100 * epsilon__length / length2.Length()  );

	BOOST_CHECK_CLOSE_SPATIAL( newStart.T, pCurve->GetData().CubicOvershootAtStart(), epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( newEnd.T, pCurve->GetData().CubicOvershootAtEnd(), epsilon__length );
}

BOOST_AUTO_TEST_CASE( cubicShortenCompareIntervalLength ){
	const spat::VectorBundle<Length,Length> start{{100_m,100_m,0_m},{0_m,100_m,10_m}};
	const spat::VectorBundle<Length,Length> end{{0_m,200_m,0_m},{0_m,100_m,10_m}};

	auto pCurve = Cubic::Make();
	auto length = pCurve->Create( start, end );

	common::Interval<Length> newInterval{ length };
	newInterval.Deflate( newInterval.Length() / 4 );
	auto length2 = pCurve->Shorten( newInterval );

	BOOST_CHECK_CLOSE_DIMENSION( length2.Length(), newInterval.Length(), 100 * epsilon__length / newInterval.Length()  );
}

BOOST_AUTO_TEST_CASE( cubicShortenCompareIntervalLength2 ){
	const spat::VectorBundle<Length,Length> start{{0_m,0_m,0_m},	{100_m,0_m,10_m}};
	const spat::VectorBundle<Length,Length> end{{100_m,100_m,0_m},{0_m,100_m,10_m}};

	auto pCurve = Cubic::Make();
	auto length = pCurve->Create( start, end );

	common::Interval<Length> newInterval{ 0.708277941_1 * length.Length(), 1.0_1 * length.Length() };
	auto length2 = pCurve->Shorten( newInterval );

	BOOST_CHECK_CLOSE_DIMENSION( length2.Length(), newInterval.Length(), 100 * epsilon__length / length2.Length()  );
}

BOOST_AUTO_TEST_CASE( cubicShortenCompareTangents ){
	Cubic::Data data{
		{-27.6881409_m,8.77320671_m,0.299999595_m},
		{27.2210236_m,-15.0666008_m,0_m},
		{-12.7011909_m,-10.8370819_m,1.11119175e-06_m},
		{4.08002329_m,5.38771200_m,-7.24327379e-07_m} };

	auto pCurveA = Cubic::Make();
	auto rangeA = pCurveA->Create( data	);
	auto pCurveB = Cubic::Make();
	auto rangeB = pCurveB->Create( data );

	const auto rangeA2 = pCurveA->Shorten( rangeA.Far( rangeA.Center() ) );
	const auto rangeB2 = pCurveB->Shorten( rangeB.Near( rangeB.Center() ) );

	BOOST_CHECK_CLOSE_DIMENSION( rangeA.Length(), rangeA2.Length(), 100 * epsilon__length / rangeA2.Length()  );
	BOOST_CHECK_CLOSE_DIMENSION( rangeB.Length(), rangeB2.Length(), 100 * epsilon__length / rangeB2.Length()  );

	spat::VectorBundle<Length,One> bundleA, bundleB;
	pCurveA->Transition( rangeA2.Far(), bundleA );
	pCurveB->Transition( rangeB2.Near(), bundleB );

	BOOST_CHECK_CLOSE_SPATIAL2( bundleA, bundleB, epsilon__length, epsilon__angle );
}

BOOST_AUTO_TEST_CASE( BezierColocatedControlPoints )
{
	auto pBezier = Cubic::Make();

	const spat::Position<Length> P0{0_m,0_m,0_m};
	const spat::Position<Length> P1{0_m,10_m,0_m};
	const spat::Position<Length> P2{10_m,0_m,0_m}; 
	const spat::Position<Length> P3{10_m,0_m,0_m};

	common::Interval<Length> range = pBezier->CreateBezier( P0, P1, P2, P3 );

	spat::Frame<Length,One> end, beforeEnd;

	pBezier->Transition( range.Near(), end );
	pBezier->Transition( range.Near() + epsilon__length, beforeEnd );
	BOOST_CHECK_CLOSE_SPATIAL2( end, beforeEnd, 10*epsilon__length, epsilon__angle );

	pBezier->Transition( range.Far(), end );
	pBezier->Transition( range.Far() - epsilon__length, beforeEnd );
	BOOST_CHECK_CLOSE_SPATIAL2( end, beforeEnd, 10*epsilon__length, epsilon__angle );
}

BOOST_AUTO_TEST_CASE( ApproximateLine )
{
	auto pLine = LineP::Make();
	pLine->Create( { 0_m, 0_m, 0_m }, { 0_m, 100_m, 0_m } );

	std::shared_ptr<Cubic> pCubic = Cubic::Make();
	common::Interval<Length> rangeApproximation = pCubic->Create( *pLine, {0_m, 100_m } ).first;
	BOOST_CHECK( pCubic->IsValid() );

	BOOST_CHECK( rangeApproximation.Equals( {0_m, 100_m }, epsilon__length ) );
}

BOOST_AUTO_TEST_CASE( ApproximateArc )
{
	auto pArc = ArcP::Make();
	pArc->Create( {0_m,10_m,0_m}, {1,0,0}, {0,1,0}, 1.0f/10_m );

	std::shared_ptr<Cubic> pCubic = Cubic::Make();
	std::pair<common::Interval<Length>,Length> retval = pCubic->Create( *pArc, {0_m, 10_m * 2*trax::pi / 4} );
	BOOST_CHECK( pCubic->IsValid() );

	BOOST_CHECK( retval.first.Equals( {0_m, 10_m * 2*trax::pi / 4}, epsilon__length ) );
	BOOST_CHECK_LE( retval.second, epsilon__length );
}

BOOST_AUTO_TEST_CASE( ApproximateArc2 )
{
	auto pArc = ArcP::Make();
	pArc->Create( {0_m,10_m,0_m}, {1,0,0}, {0,1,0}, 1.0f/10_m );

	std::shared_ptr<Cubic> pCubic = Cubic::Make();
	std::pair<common::Interval<Length>,Length> retval = pCubic->Create( *pArc, {10_m * 2*trax::pi / 2, 10_m * 3*2*trax::pi / 4} );
	BOOST_CHECK( pCubic->IsValid() );

	BOOST_CHECK( retval.first.Equals( {0_m, 10_m * 2*trax::pi / 4}, epsilon__length ) );
	BOOST_CHECK_LE( retval.second, epsilon__length );
}

BOOST_AUTO_TEST_CASE( ApproximateCubicSymmetric )
{
	const spat::VectorBundle<Length,Length> start{{100_m,0_m,1_m}, {200_m,0_m,0_m}};
	const spat::VectorBundle<Length,Length> end{{200_m,-100_m,50_m}, {141_m,-141_m,0_m}};

	std::shared_ptr<Cubic> pCubic = Cubic::Make();
	common::Interval<Length> rangeOriginal = pCubic->Create( start, end );
	BOOST_CHECK( pCubic->IsValid() );

	std::shared_ptr<Cubic> pCubicApprox = Cubic::Make();
	common::Interval<Length> rangeApproximation = pCubicApprox->Create( *pCubic, rangeOriginal, epsilon__length ).first;
	BOOST_CHECK( pCubicApprox->IsValid() );

	BOOST_CHECK( Equals( pCubic->GetData(), pCubicApprox->GetData(), rangeOriginal, epsilon__length ) );
	BOOST_CHECK( rangeApproximation.Equals( rangeOriginal, epsilon__length ) );
}

BOOST_AUTO_TEST_CASE( ApproximateCubicAsymmetric )
{
	const spat::VectorBundle<Length,Length> start{{100_m,0_m,1_m}, {200_m,0_m,0_m}};
	const spat::VectorBundle<Length,Length> end{{200_m,-100_m,50_m}, {70.71_m,-70.71_m,0_m}};

	std::shared_ptr<Cubic> pCubic = Cubic::Make();
	common::Interval<Length> rangeOriginal = pCubic->Create( start, end );
	BOOST_CHECK( pCubic->IsValid() );

	std::shared_ptr<Cubic> pCubicApprox = Cubic::Make();
	common::Interval<Length> rangeApproximation = pCubicApprox->Create( *pCubic, rangeOriginal, epsilon__length ).first;
	BOOST_CHECK( pCubicApprox->IsValid() );

	BOOST_CHECK( Equals( pCubic->GetData(), pCubicApprox->GetData(), rangeOriginal, epsilon__length ) );
	BOOST_CHECK( rangeApproximation.Equals( rangeOriginal, epsilon__length ) );
}

BOOST_AUTO_TEST_CASE( ApproximateCubicAsymmetric2 )
{
	const spat::VectorBundle<Length,Length> start{{0_m,0_m,0_m}, {100_m,0_m,0_m}};
	const spat::VectorBundle<Length,Length> end{{-165.789230_m,-232.241913_m,0_m}, {130.289230_m,195.141907_m,0_m}};

	std::shared_ptr<Cubic> pCubic = Cubic::Make();
	common::Interval<Length> rangeOriginal = pCubic->Create( start, end );
	BOOST_CHECK( pCubic->IsValid() );

	std::shared_ptr<Cubic> pCubicApprox = Cubic::Make();
	common::Interval<Length> rangeApproximation = pCubicApprox->Create( *pCubic, rangeOriginal, epsilon__length ).first;
	BOOST_CHECK( pCubicApprox->IsValid() );

	BOOST_CHECK( Equals( pCubic->GetData(), pCubicApprox->GetData(), rangeOriginal, 2*epsilon__length ) );
	BOOST_CHECK( rangeApproximation.Equals( rangeOriginal, epsilon__length ) );
}

BOOST_AUTO_TEST_SUITE_END()// CurveCubic
BOOST_AUTO_TEST_SUITE(CurveSpline)

BOOST_AUTO_TEST_CASE( splineFlatZeroSet ){
	std::vector<spat::VectorBundle<Length>> controlPoints = { 
		{ {0_m,0_m,0_m},	{100_m,0_m,0_m} }, 
		{ {100_m,100_m,0_m},{0_m,100_m,0_m} }, 
		{ {0_m,200_m,0_m},	{0_m,100_m,0_m} }, 
		{ {100_m,300_m,0_m},{0_m,100_m,0_m} },
		{ {200_m,300_m,0_m},{0_m,100_m,0_m} }
	};

	auto pCurve = Spline::Make();
	auto length = pCurve->Create( controlPoints );

	spat::VectorBundle<Length,One> bundle;
	pCurve->Transition( length.Near(), bundle );
	BOOST_CHECK( bundle.Equals( { {0_m,0_m,0_m},{1,0,0} } ) );
	pCurve->Transition( length.Far(), bundle );
	spat::VectorBundle<Length,One> b{ {200_m,300_m,0_m},{0,1,0} };
	BOOST_CHECK_CLOSE_SPATIAL( bundle, b, epsilon__length );

	std::vector<Length> zeroSet = pCurve->ZeroSet();
	BOOST_CHECK_EQUAL( zeroSet.size(), 4u );
	BOOST_CHECK( pCurve->IsFlat() );
}

BOOST_AUTO_TEST_CASE( splineUnFlatZeroSet ){
	std::vector<spat::VectorBundle<Length>> controlPoints = { 
		{ {0_m,0_m,0_m},	{100_m,0_m,10_m} }, 
		{ {100_m,100_m,0_m},{0_m,100_m,10_m} }, 
		{ {0_m,200_m,0_m},	{0_m,100_m,10_m} }, 
		{ {100_m,300_m,0_m},{0_m,100_m,10_m} }
	};

	auto pCurve = Spline::Make();
	auto length = pCurve->Create( controlPoints );

	spat::Position<Length> pos;
	pCurve->Transition( length.Near(), pos );
	BOOST_CHECK( pos.Equals( { 0_m,0_m,0_m } ) );
	pCurve->Transition( length.Far(), pos );
	spat::Position<Length> p{100_m,300_m,0_m};
	BOOST_CHECK_CLOSE_SPATIAL( pos, p, epsilon__length );

	std::vector<Length> zeroSet = pCurve->ZeroSet();
	BOOST_CHECK_EQUAL( zeroSet.size(), 2u );
	BOOST_CHECK( !pCurve->IsFlat() );
}

BOOST_AUTO_TEST_CASE( splineInterpolateByCatmullRom  ){
	std::vector<spat::Position<Length>> controlPoints = { 
		{ 0_m,	0_m,	0_m}, 
		{ 50_m, 100_m,	30_m}, 
		{ 0_m,	200_m,	0_m}, 
		{ 100_m,300_m,	-10_m},
		{ 150_m,280_m,  0_m}
	};

	auto pCurve = Spline::Make();
	auto length = pCurve->CreateCatmullRom( controlPoints );
	BOOST_CHECK( pCurve->IsValid() );
	BOOST_CHECK_GT( length.Length(), 0_m );

	spat::Position<Length> pos;
	pCurve->Transition( length.Near(), pos );
	BOOST_CHECK_EQUAL( pos, controlPoints[0] );
	pCurve->Transition( length.Far(), pos );
	BOOST_CHECK_CLOSE_SPATIAL( pos, controlPoints[4], epsilon__length );

	for( int idx = 0; idx < pCurve->CountControlPoints(); ++idx ){
		BOOST_CHECK( controlPoints[idx].Equals( pCurve->GetControlPointAndTangent( idx ).P, epsilon__length ) );
		VectorBundle<Length,One> pointAndTangent;
		pCurve->Transition( pCurve->GetParameter( idx ), pointAndTangent );
		BOOST_CHECK( pointAndTangent.P.Equals( pCurve->GetControlPointAndTangent( idx ).P, epsilon__length ) );
		BOOST_CHECK( pointAndTangent.T.Equals( Normalize( pCurve->GetControlPointAndTangent( idx ).T ).second, 10*epsilon ) );
	}
}

BOOST_AUTO_TEST_CASE( splineInterpolateByNaturalC2 ){
	std::vector<spat::Position<Length>> controlPoints = { 
		{ 0_m,	0_m,	0_m}, 
		{ 50_m, 100_m,	30_m}, 
		{ 0_m,	200_m,	0_m}, 
		{ 100_m,300_m,	-10_m},
		{ 150_m,280_m,  0_m}
	};

	auto pCurve = Spline::Make();
	auto length = pCurve->Create( controlPoints );
	BOOST_CHECK( pCurve->IsValid() );
	BOOST_CHECK_GT( length.Length(), 0_m );

	spat::Position<Length> pos;
	pCurve->Transition( length.Near(), pos );
	BOOST_CHECK_EQUAL( pos, controlPoints[0] );
	pCurve->Transition( length.Far(), pos );
	BOOST_CHECK( pos.Equals( controlPoints[4], epsilon__length ) );

	for( int idx = 0; idx < pCurve->CountControlPoints(); ++idx ){
		BOOST_CHECK( controlPoints[idx].Equals( pCurve->GetControlPointAndTangent( idx ).P, epsilon__length ) );
		VectorBundle<Length,One> pointAndTangent;
		pCurve->Transition( pCurve->GetParameter( idx ), pointAndTangent );
		BOOST_CHECK( pointAndTangent.P.Equals( pCurve->GetControlPointAndTangent( idx ).P, epsilon__length ) );
		BOOST_CHECK( pointAndTangent.T.Equals( Normalize( pCurve->GetControlPointAndTangent( idx ).T ).second, 10*epsilon ) );

		if( 0 < idx && idx < pCurve->CountControlPoints() - 1 ){
			VectorBundle2<Length,One> normal1, normal2;
			pCurve->Transition( pCurve->GetParameter( idx ) - epsilon__length / 100, normal1 );
			pCurve->Transition( pCurve->GetParameter( idx ) + epsilon__length / 100, normal2 );
			BOOST_CHECK( normal1.N.Equals( normal2.N, epsilon__angle ) );
		}
	}
}

BOOST_AUTO_TEST_CASE( splineInterpolateByClampedC2MatchEndTangents ){
	std::vector<spat::Position<Length>> controlPoints = { 
		{ 0_m,	 0_m, 0_m}, 
		{ 200_m, 0_m, 0_m}
	};

	auto pCurve = Spline::Make();
	pCurve->Create( controlPoints, true, {100_m,0_m,0_m}, true, {100_m,0_m,0_m} );
	BOOST_CHECK( pCurve->IsValid() );

	VectorBundle<Length,One> pointAndTangent;
	pCurve->Transition( 0_m, pointAndTangent );
	VectorBundle<Length,One> target{ { 0_m, 0_m, 0_m}, {1,0,0} };
	BOOST_CHECK_EQUAL( pointAndTangent, target );

	pCurve->Transition( 200_m, pointAndTangent );
	target = VectorBundle<Length,One>{ { 200_m, 0_m, 0_m}, {1,0,0} };
	BOOST_CHECK_EQUAL( pointAndTangent, target );
}

BOOST_AUTO_TEST_CASE( splineInterpolateByClampedC2MatchEndTangents2 ){
	std::vector<spat::Position<Length>> controlPoints = { 
		{-50_m, -50_m, 0_m },
		{50_m, -50_m, 0_m }
	};
	
	auto pCurve = Spline::Make();
	auto length = pCurve->Create( controlPoints, true, {30_m,0_m,0_m}, true, {30_m,0_m,0_m} );
	BOOST_CHECK( pCurve->IsValid() );

	VectorBundle<Length,One> pointAndTangent;
	pCurve->Transition( length.Near(), pointAndTangent );
	VectorBundle<Length,One> target{ {-50_m, -50_m, 0_m }, {1,0,0} };
	BOOST_CHECK_EQUAL( pointAndTangent.P, target.P );
	BOOST_CHECK_CLOSE_SPATIAL( pointAndTangent.T, target.T, epsilon );

	pCurve->Transition( length.Far(), pointAndTangent );
	target = VectorBundle<Length,One>{ {50_m, -50_m, 0_m }, {1,0,0} };
	BOOST_CHECK_EQUAL( pointAndTangent.P, target.P );
	BOOST_CHECK_CLOSE_SPATIAL( pointAndTangent.T, target.T, epsilon );
}

BOOST_AUTO_TEST_CASE( splineInterpolateByClampedC2MatchEndTangents3 ){
	std::vector<spat::Position<Length>> controlPoints = { 
		{-50_m, 0_m, -50_m },
		{50_m, 0_m, -50_m }
	};
	
	auto pCurve = Spline::Make();
	auto length = pCurve->Create( controlPoints, true, {30_m,0_m,0_m}, true, {30_m,0_m,0_m} );
	BOOST_CHECK( pCurve->IsValid() );

	VectorBundle<Length,One> pointAndTangent;
	pCurve->Transition( length.Near(), pointAndTangent );
	VectorBundle<Length,One> target{ {-50_m, 0_m, -50_m }, {1,0,0} };
	BOOST_CHECK_EQUAL( pointAndTangent.P, target.P );
	BOOST_CHECK_CLOSE_SPATIAL( pointAndTangent.T, target.T, epsilon );

	pCurve->Transition( length.Far(), pointAndTangent );
	target = VectorBundle<Length,One>{ {50_m, 0_m, -50_m }, {1,0,0} };
	BOOST_CHECK_EQUAL( pointAndTangent.P, target.P );
	BOOST_CHECK_CLOSE_SPATIAL( pointAndTangent.T, target.T, epsilon );
}

BOOST_AUTO_TEST_CASE( splineInterpolateByClampedC2 ){
	std::vector<spat::Position<Length>> controlPoints = { 
		{ 0_m,	0_m,	0_m}, 
		{ 50_m, 100_m,	30_m}, 
		{ 0_m,	200_m,	0_m}, 
		{ 100_m,300_m,	-10_m},
		{ 150_m,280_m,  0_m}
	};

	auto pCurve = Spline::Make();
	auto length = pCurve->Create( controlPoints, true, {100_m,0_m,10_m}, true, {0_m,100_m,10_m} );
	BOOST_CHECK( pCurve->IsValid() );
	BOOST_CHECK_GT( length.Length(), 0_m );

	spat::Position<Length> pos;
	pCurve->Transition( length.Near(), pos );
	BOOST_CHECK_EQUAL( pos, controlPoints[0] );
	pCurve->Transition( length.Far(), pos );
	BOOST_CHECK( pos.Equals( controlPoints[4], epsilon__length ) );

	for( int idx = 0; idx < pCurve->CountControlPoints(); ++idx ){
		BOOST_CHECK( controlPoints[idx].Equals( pCurve->GetControlPointAndTangent( idx ).P, epsilon__length ) );
		VectorBundle<Length,One> pointAndTangent;
		pCurve->Transition( pCurve->GetParameter( idx ), pointAndTangent );
		BOOST_CHECK( pointAndTangent.P.Equals( pCurve->GetControlPointAndTangent( idx ).P, epsilon__length ) );
		BOOST_CHECK( pointAndTangent.T.Equals( Normalize( pCurve->GetControlPointAndTangent( idx ).T ).second, 10*epsilon ) );

		if( 0 < idx && idx < pCurve->CountControlPoints() - 1 ){
			VectorBundle2<Length,One> normal1, normal2;
			pCurve->Transition( pCurve->GetParameter( idx ) - epsilon__length / 100, normal1 );
			pCurve->Transition( pCurve->GetParameter( idx ) + epsilon__length / 100, normal2 );
			BOOST_CHECK( normal1.N.Equals( normal2.N, epsilon__angle ) );
		}
	}
}

BOOST_AUTO_TEST_CASE( splineWriteRead ){
	std::vector<spat::VectorBundle<Length>> controlPoints = { 
		{ {0_m,0_m,0_m},	{100_m,0_m,10_m} }, 
		{ {100_m,100_m,0_m},{0_m,100_m,10_m} }, 
		{ {0_m,200_m,0_m},	{0_m,100_m,10_m} }, 
		{ {100_m,300_m,0_m},{0_m,100_m,10_m} }
	};

	auto pCurve = Spline::Make();
	auto length = pCurve->Create( controlPoints );

	Spline::Data data = pCurve->GetData();
	auto pCurve2 = Spline::Make();
	length = pCurve2->Create( controlPoints );
	Spline::Data data2 = pCurve2->GetData();
	BOOST_CHECK_EQUAL( data, data2 );
}

BOOST_AUTO_TEST_CASE( splineVeryFewControlPoints ){
	std::vector<spat::Position<Length>> controlPoints = { 
		{0_m,0_m,0_m},
		{100_m,100_m,0_m}
	};

	auto pCurve = Spline::Make();
	auto length = pCurve->Create( controlPoints, true, {100_m,0_m,10_m}, true, {0_m,100_m,10_m} );
	BOOST_CHECK( pCurve->IsValid() );
	BOOST_CHECK_GT( length.Length(), 0_m );

	length = pCurve->Create( controlPoints );
	BOOST_CHECK( pCurve->IsValid() );
	BOOST_CHECK_GT( length.Length(), 0_m );

	std::vector<spat::VectorBundle<Length>> controlPoints2 = { 
		{ {0_m,0_m,0_m},	{100_m,0_m,10_m} }, 
		{ {100_m,100_m,0_m},{0_m,100_m,10_m} }
	};

	length = pCurve->Create( controlPoints2 );
	BOOST_CHECK( pCurve->IsValid() );
	BOOST_CHECK_GT( length.Length(), 0_m );
}

BOOST_AUTO_TEST_CASE( splineZeroTangent ){
	std::vector<spat::VectorBundle<Length>> controlPoints = { 
		{ {0_m,0_m,0_m},	{100_m,0_m,10_m} }, 
		{ {100_m,100_m,0_m},{0_m,100_m,10_m} }, 
		{ {0_m,200_m,0_m},	{0_m,100_m,10_m} }, 
		{ {100_m,300_m,0_m},{0_m,100_m,10_m} },
		{ {0_m,0_m,0_m},    {0_m,0_m,0_m} }
	};

	auto pCurve = Spline::Make();
	auto length = pCurve->Create( controlPoints );

	spat::Position<Length> pos;
	pCurve->Transition( length.Near(), pos );
	BOOST_CHECK_EQUAL( pos, controlPoints[0].P );
	pCurve->Transition( length.Far(), pos );
	BOOST_CHECK_CLOSE_SPATIAL( pos, controlPoints[4].P, epsilon__length );

	VectorBundle2<Length,One> frame;
	pCurve->Transition( length.Far(), frame );
	BOOST_CHECK( frame.IsOrthoNormal() );
	BOOST_CHECK( !isnan( frame.N.dx ) );
	BOOST_CHECK( !isnan( frame.N.dy ) );
	BOOST_CHECK( !isnan( frame.N.dz ) );
}

BOOST_AUTO_TEST_CASE( splineZeroTangent2 ){
	std::vector<spat::VectorBundle<Length>> controlPoints = { 
		{ {0_m,0_m,0_m},	{100_m,0_m,10_m} }, 
		{ {100_m,100_m,0_m},{0_m,100_m,10_m} }, 
		{ {0_m,200_m,0_m},	{0_m,0_m,0_m} }, 
		{ {100_m,300_m,0_m},{0_m,100_m,10_m} }
	};

	auto pCurve = Spline::Make();
	pCurve->Create( controlPoints );

	VectorBundle2<Length,One> frame;
	pCurve->Transition( pCurve->GetParameter( 2 ), frame );
	BOOST_CHECK( !isnan( frame.N.dx ) );
	BOOST_CHECK( !isnan( frame.N.dy ) );
	BOOST_CHECK( !isnan( frame.N.dz ) );
}

BOOST_AUTO_TEST_CASE( splineShortenNoEffect ){
	std::vector<spat::VectorBundle<Length>> controlPoints = { 
		{ {0_m,0_m,0_m},	{100_m,0_m,10_m} }, 
		{ {100_m,100_m,0_m},{0_m,100_m,10_m} }, 
		{ {0_m,200_m,0_m},	{0_m,100_m,10_m} }, 
		{ {100_m,300_m,0_m},{0_m,100_m,10_m} }
	};

	auto pCurve = Spline::Make();
	auto length = pCurve->Create( controlPoints );

	Vector<One> startT, endT;
	pCurve->Transition( length.Near(), startT );
	pCurve->Transition( length.Far(), endT );

	auto length2 = pCurve->Shorten( length );
	BOOST_CHECK_EQUAL( pCurve->CountControlPoints(), 4 );
	BOOST_CHECK_EQUAL( length, length2 );

	Vector<One> compareT;
	pCurve->Transition( length2.Near(), compareT );
	BOOST_CHECK_CLOSE_SPATIAL( startT, compareT, epsilon__angle );
	pCurve->Transition( length2.Far(), compareT );
	BOOST_CHECK_CLOSE_SPATIAL( endT, compareT, epsilon__angle );
}

BOOST_AUTO_TEST_CASE( splineShortenHalf ){
	std::vector<spat::VectorBundle<Length>> controlPoints = { 
		{ {0_m,0_m,0_m},	{100_m,0_m,10_m} }, 
		{ {100_m,100_m,0_m},{0_m,100_m,10_m} }, 
		{ {0_m,200_m,0_m},	{0_m,100_m,10_m} }, 
		{ {100_m,300_m,0_m},{0_m,100_m,10_m} }
	};

	auto pCurve = Spline::Make();
	common::Interval<Length> length = pCurve->Create( controlPoints );
	common::Interval<Length> newInterval = length;
	newInterval.Deflate( length.Length() / 4 );

	Vector<One> startT, endT;
	pCurve->Transition( newInterval.Near(), startT );
	pCurve->Transition( newInterval.Far(), endT );
	common::Interval<Length> interval1{ newInterval.Near(), pCurve->GetParameter( 1 ) };
	common::Interval<Length> interval2{ pCurve->GetParameter( 1 ), pCurve->GetParameter( 2 ) };
	common::Interval<Length> interval3{ pCurve->GetParameter( 2 ), newInterval.Far() };

	auto length3 = pCurve->Shorten( newInterval );
	BOOST_CHECK_CLOSE_DIMENSION( length3.Length(), newInterval.Length(), 100 * 2*epsilon__length / newInterval.Length() );

	Vector<One> compareT;
	pCurve->Transition( length3.Near(), compareT );
	BOOST_CHECK_CLOSE_SPATIAL( startT, compareT, epsilon__angle );
	pCurve->Transition( length3.Far(), compareT );
	BOOST_CHECK_CLOSE_SPATIAL( endT, compareT, epsilon__angle );

	common::Interval<Length> testInterval1{ pCurve->GetParameter( 0 ), pCurve->GetParameter( 1 ) };
	BOOST_CHECK_CLOSE_DIMENSION( interval1.Length(), testInterval1.Length(), 1 );
	common::Interval<Length> testInterval2{ pCurve->GetParameter( 1 ), pCurve->GetParameter( 2 ) };
	BOOST_CHECK_CLOSE_DIMENSION( interval2.Length(), testInterval2.Length(), 1 );
	common::Interval<Length> testInterval3{ pCurve->GetParameter( 2 ), pCurve->GetParameter( 3 ) };
	BOOST_CHECK_CLOSE_DIMENSION( interval3.Length(), testInterval3.Length(), 1 );
}

BOOST_AUTO_TEST_CASE( splineShortenInsideOneCubic ){
	std::vector<spat::VectorBundle<Length>> controlPoints = { 
		{ {0_m,0_m,0_m},	{100_m,0_m,10_m} }, 
		{ {100_m,100_m,0_m},{0_m,100_m,10_m} }, 
		{ {0_m,200_m,0_m},	{0_m,100_m,10_m} }, 
		{ {100_m,300_m,0_m},{0_m,100_m,10_m} }
	};

	auto pCurve = Spline::Make();
	pCurve->Create( controlPoints );

	common::Interval<Length> newInterval{ pCurve->GetParameter( 1 ), pCurve->GetParameter( 2 ) };
	newInterval.Deflate( newInterval.Length() / 4 );
	auto length2 = pCurve->Shorten( newInterval );

	BOOST_CHECK_EQUAL( pCurve->CountControlPoints(), 2 );
	BOOST_CHECK_CLOSE_DIMENSION( length2.Length(), newInterval.Length(), 100 * epsilon__length / newInterval.Length() );
}

BOOST_AUTO_TEST_CASE( splineShortenExactCubicMatch ){
	std::vector<spat::VectorBundle<Length>> controlPoints = { 
		{ {0_m,0_m,0_m},	{100_m,0_m,10_m} }, 
		{ {100_m,100_m,0_m},{0_m,100_m,10_m} }, 
		{ {0_m,200_m,0_m},	{0_m,100_m,10_m} }, 
		{ {100_m,300_m,0_m},{0_m,100_m,10_m} }
	};

	auto pCurve = Spline::Make();
	pCurve->Create( controlPoints );

	common::Interval<Length> newInterval{ pCurve->GetParameter( 1 ), pCurve->GetParameter( 2 ) };

	Frame<Length,One> frame1;
	pCurve->Transition( newInterval.Center(), frame1 );
	auto length2 = pCurve->Shorten( newInterval );

	BOOST_CHECK_EQUAL( pCurve->CountControlPoints(), 2 );
	BOOST_CHECK_CLOSE_DIMENSION( length2.Length(), newInterval.Length(), 100 * (2*epsilon__length) / newInterval.Length()  );

	Frame<Length,One> frame2;
	pCurve->Transition( length2.Center(), frame2 );
	BOOST_CHECK_CLOSE_SPATIAL2( frame1, frame2, epsilon__length, epsilon__angle );
}

BOOST_AUTO_TEST_CASE( splineShortenZero ){
	std::vector<spat::VectorBundle<Length>> controlPoints = { 
		{ {0_m,0_m,0_m},	{100_m,0_m,10_m} }, 
		{ {100_m,100_m,0_m},{0_m,100_m,10_m} }, 
		{ {0_m,200_m,0_m},	{0_m,100_m,10_m} }, 
		{ {100_m,300_m,0_m},{0_m,100_m,10_m} }
	};

	auto pCurve = Spline::Make();
	common::Interval<Length> length = pCurve->Create( controlPoints );

	common::Interval<Length> newInterval{ length.Center() };
	auto length2 = pCurve->Shorten( newInterval );

	BOOST_CHECK_EQUAL( length2, length );
}

BOOST_AUTO_TEST_CASE( BezierColocatedControlPoints )
{
	std::vector<spat::Position<Length>> controlPoints = { 
		{-54.887764_m, -4.5113762e-7_m, 0_m}, 
		{-54.887764_m, -4.5113762e-7_m, 0_m},
		{17.373306_m, 9.0211023e-7_m, 0_m}, 
		{17.373306_m, 9.0211023e-7_m, 0_m}, 
		{17.373306_m, 9.0211023e-7_m, 0_m}, 
		{22.80922_m, 0.0000036086026_m, 0_m}, 
		{50_m, 0.0000045107713_m, 0_m}, 
		{77.19078_m, 0.00000541294_m, 0_m}, 
		{36.747955_m, -0.000005413051_m, 0_m}, 
		{325.2441_m, -0.000005864129_m, 0_m}, 
		{613.74036_m, -0.000006315207_m, 0_m}, 
		{700_m, 175.64148_m, 0_m}, 
		{700_m, 429.55014_m, 0_m}, 
		{700_m, 683.45874_m, 0_m}, 
		{700_m, 1000_m, 0_m}, 
		{700_m, 1000_m, 0_m} };

	auto pBezier = Spline::Make();
	common::Interval<Length> range = pBezier->CreateBezier( controlPoints );

	spat::Frame<Length,One> end, beforeEnd;

	pBezier->Transition( range.Near(), end );
	pBezier->Transition( range.Near() + epsilon__length, beforeEnd );
	BOOST_CHECK_CLOSE_SPATIAL2( end, beforeEnd, epsilon__length, epsilon__angle );

	pBezier->Transition( pBezier->GetParameter(1), end );
	pBezier->Transition( pBezier->GetParameter(1) - epsilon__length, beforeEnd );
	BOOST_CHECK_CLOSE_SPATIAL2( end, beforeEnd, epsilon__length, epsilon__angle );

	pBezier->Transition( range.Far(), end );
	pBezier->Transition( range.Far() - epsilon__length, beforeEnd );
	BOOST_CHECK_CLOSE_SPATIAL2( end, beforeEnd, epsilon__length, epsilon__angle );
}

BOOST_AUTO_TEST_CASE( ApproximateLine )
{
	auto pLine = LineP::Make();
	pLine->Create( { 0_m, 0_m, 0_m }, { 0_m, 100_m, 0_m } );

	std::shared_ptr<Spline> pSpline = Spline::Make();
	common::Interval<Length> rangeApproximation = pSpline->Create( *pLine, {0_m, 100_m } );
	BOOST_CHECK( pSpline->IsValid() );

	BOOST_CHECK( rangeApproximation.Equals( {0_m, 100_m }, epsilon__length ) );
}

BOOST_AUTO_TEST_CASE( ApproximateArc )
{
	auto pArc = ArcP::Make();
	pArc->Create( {0_m,10_m,0_m}, {1,0,0}, {0,1,0}, 1.0f/10_m );
	common::Interval<Length> rangeArc = {0_m, 10_m * 2*trax::pi};

	std::shared_ptr<Spline> pSpline = Spline::Make();
	common::Interval<Length> rangeSpline = pSpline->Create( *pArc, rangeArc, 0.01_m );
	BOOST_CHECK( pSpline->IsValid() );
	BOOST_CHECK( !pSpline->HasGaps() );
	BOOST_CHECK( !pSpline->HasKinks() );
	BOOST_CHECK( pSpline->GetData().size() == 4u || pSpline->GetData().size() == 5u );

	spat::Frame<Length,One> frameA, frameB;
	pArc->Transition( 10_m, frameA );
	pSpline->Transition( rangeSpline.Length() / rangeArc.Length() * 10_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL2( frameA, frameB, 0.5_m, 5*trax::epsilon__angle ); 
	pArc->Transition( 20_m, frameA );
	pSpline->Transition( rangeSpline.Length() / rangeArc.Length() * 20_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL2( frameA, frameB, 0.5_m, 5*trax::epsilon__angle ); 
	pArc->Transition( 30_m, frameA );
	pSpline->Transition( rangeSpline.Length() / rangeArc.Length() * 30_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL2( frameA, frameB, 0.5_m, 5*trax::epsilon__angle ); 
	pArc->Transition( 40_m, frameA );
	pSpline->Transition( rangeSpline.Length() / rangeArc.Length() * 40_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL2( frameA, frameB, 0.5_m, 10*trax::epsilon__angle ); 
	pArc->Transition( 50_m, frameA );
	pSpline->Transition( rangeSpline.Length() / rangeArc.Length() * 50_m, frameB );
	BOOST_CHECK_CLOSE_SPATIAL2( frameA, frameB, 0.5_m, 10*trax::epsilon__angle );
	pArc->Transition( 10_m * 2*trax::pi, frameA );
	pSpline->Transition( rangeSpline.Length() / rangeArc.Length() * 10_m * 2*trax::pi, frameB );
	BOOST_CHECK_CLOSE_SPATIAL2( frameA, frameB, 0.5_m, 5*trax::epsilon__angle ); 
}

BOOST_AUTO_TEST_CASE( ApproximateCubic )
{
	const spat::VectorBundle<Length,Length> start{{100_m,0_m,10_m}, {200_m,0_m,0_m}};
	const spat::VectorBundle<Length,Length> end{{200_m,-100_m,50_m}, {141_m,-141_m,0_m}};

	std::shared_ptr<Cubic> pCubic = Cubic::Make();
	common::Interval<Length> rangeOriginal = pCubic->Create( start, end );
	BOOST_CHECK( pCubic->IsValid() );

	std::shared_ptr<Spline> pSpline = Spline::Make();
	common::Interval<Length> rangeApproximation = pSpline->Create( *pCubic, rangeOriginal );
	BOOST_CHECK( pSpline->IsValid() );
	BOOST_CHECK_EQUAL( pSpline->GetData().size(), 1u );
	BOOST_CHECK( rangeApproximation.Equals( rangeOriginal, epsilon__length ) );
}

BOOST_AUTO_TEST_SUITE_END()// CurveSpline
BOOST_AUTO_TEST_SUITE(CurvePolygonChain)

BOOST_AUTO_TEST_CASE( MinimumVertexCnt )
{
	std::vector<spat::Position<Length>> data{ {10_m,0_m,0_m}, { 20_m, 10_m, 10_m } };
	auto pPolygonChain = PolygonalChain::Make();
	common::Interval<Length> range = pPolygonChain->Create( data );

	spat::Position<Length> posA, posB;
	pPolygonChain->Transition( range.Near(), posA );
	pPolygonChain->Transition( range.Far(), posB );

	BOOST_CHECK_CLOSE_SPATIAL( data.front(), posA, epsilon__length );
	BOOST_CHECK_CLOSE_SPATIAL( data.back(), posB, epsilon__length );
}

BOOST_AUTO_TEST_CASE( ArcApproximation )
{
	auto pArc = ArcP::Make();
	pArc->Create( ArcP::Data{} );

	common::Interval<Length> range{0_m, 10_m * pi};
	auto pPolygonChain = PolygonalChain::Make();
	pPolygonChain->Create( *pArc, range, epsilon__length, 5_cm );

	Length s = range.Near();
	Length ds = 1_m;
	while( s < range.Far() )
	{
		spat::Frame<Length,One> frameA, frameB;
		pArc->Transition( s, frameA );
		pPolygonChain->Transition( s, frameB );

		BOOST_CHECK( frameA.Equals( frameB, epsilon__length, 2*epsilon__angle ) );

		if( !frameA.Equals( frameB, epsilon__length, 2*epsilon__angle ) ){
			std::cout << "Failed approximation:" << std::endl;
			std::cout << frameA << std::endl;
			std::cout << frameB << std::endl;
		}

		s += ds;
	}

	spat::Frame<Length,One> frameA, frameB;
	pArc->Transition( range.Far(), frameA );
	pPolygonChain->Transition( range.Far(), frameB );

	BOOST_CHECK( frameA.Equals( frameB, epsilon__length, 2*epsilon__angle ) );

	if( !frameA.Equals( frameB, epsilon__length, 2*epsilon__angle ) ){
		std::cout << "Failed approximation:" << std::endl;
		std::cout << frameA << std::endl;
		std::cout << frameB << std::endl;
	}
}

BOOST_AUTO_TEST_CASE( ParallelCubicApproximation )
{
	const spat::VectorBundle<Length,Length> start{{100_m,100_m,0_m},{0_m,100_m,0_m}};
	const spat::VectorBundle<Length,Length> end{{0_m,200_m,0_m},{0_m,100_m,0_m}};

	auto pCurve = Cubic::Make();
	auto range = pCurve->Create( start, end );

	auto pTrack = TrackBuilder::Make();
	pTrack->Attach( std::move(pCurve), range );
	pTrack->Attach( DirectionalTwist::Make() );

	auto pParallel = MakeParallelTrackWithPolygonalChain( 
		*pTrack, 
		range, 
		spat::Vector2D<Length>{ 4.5_m, 0_m }, 
		false );

	Length s = pParallel->Range().Near();
	const Length ds = 1_m;

	while( s < pParallel->Range().Far() ){
		Frame<Length,One> frame;
		pParallel->Transition( s, frame );
		BOOST_CHECK_CLOSE_SPATIAL( frame.B, Up, epsilon__angle );
		s += ds;
	}
}

BOOST_AUTO_TEST_SUITE_END() //CurvePolygonChain
BOOST_AUTO_TEST_SUITE(CurveEEPCurve)
/*
BOOST_FIXTURE_TEST_CASE( eepCurveEndpointCreation1, TraxFixtureBase<trax::Factory::EngineTypes::Standard> ){
	auto pCurve = m_Factory.CreateEEPCurve();
	spat::Frame<Length,One> start{ {0_m,0_m,0_m}, {0,1,0}, {-1,0,0}, {0,0,1} };
	spat::Position<Length> end{-10_m,10_m,0_m};

	Length length = 10_m * trax::pi/2;
	BOOST_CHECK_CLOSE_DIMENSION( pCurve->Create( start, end ).Far(), length, trax::epsilon__length / length * 100 );

	spat::Position<Length> endCreated;
	pCurve->Transition( 10_m * trax::pi/2, endCreated );
	BOOST_CHECK(end.Equals(endCreated, trax::epsilon__length));

	spat::Frame<Length,One> startCreated;
	pCurve->Transition( 0_m, startCreated );
	BOOST_CHECK(start.P.Equals(startCreated.P, trax::epsilon__length));
}

BOOST_FIXTURE_TEST_CASE( eepCurveEndpointCreation2, TraxFixtureBase<trax::Factory::EngineTypes::Standard> ){
	auto pCurve = m_Factory.CreateEEPCurve();
	spat::Frame<Length,One> start{ {0_m,0_m,0_m}, {1,0,0}, {0,1,0}, {0,0,1} };
	spat::Position<Length> end{10_m,10_m,0_m};

	Length length = 10_m * trax::pi/2;
	BOOST_CHECK_CLOSE_DIMENSION( pCurve->Create( start, end ).Far(), length, trax::epsilon__length / length * 100 );

	spat::Position<Length> endCreated;
	pCurve->Transition( 10_m * trax::pi/2, endCreated );
	BOOST_CHECK(end.Equals(endCreated, trax::epsilon__length));

	spat::Frame<Length,One> startCreated;
	pCurve->Transition( 0_m, startCreated );
	BOOST_CHECK(start.P.Equals(startCreated.P, trax::epsilon__length));
}

BOOST_FIXTURE_TEST_CASE( eepCurveEndpointCreation3, TraxFixtureBase<trax::Factory::EngineTypes::Standard> ){
	auto pCurve = m_Factory.CreateEEPCurve();
	spat::Frame<Length,One> start{ {0_m,0_m,0_m}, {0,-1,0}, {1,0,0}, {0,0,1} };
	spat::Position<Length> end{10_m,-10_m,0_m};

	Length length = 10_m * trax::pi/2;
	BOOST_CHECK_CLOSE_DIMENSION( pCurve->Create( start, end ).Far(), length, trax::epsilon__length / length * 100 );

	spat::Position<Length> endCreated;
	pCurve->Transition( 10_m * trax::pi/2, endCreated );
	BOOST_CHECK(end.Equals(endCreated, trax::epsilon__length));

	spat::Frame<Length,One> startCreated;
	pCurve->Transition( 0_m, startCreated );
	BOOST_CHECK(start.P.Equals(startCreated.P, trax::epsilon__length));
}

BOOST_FIXTURE_TEST_CASE( eepCurveEndpointCreation4, TraxFixtureBase<trax::Factory::EngineTypes::Standard> ){
	auto pCurve = m_Factory.CreateEEPCurve();
	spat::Frame<Length,One> start{ {0_m,0_m,0_m}, {-1,0,0}, {0,-1,0}, {0,0,1} };
	spat::Position<Length> end{-10_m,-10_m,0_m};

	Length length = 10_m * trax::pi/2;
	BOOST_CHECK_CLOSE_DIMENSION( pCurve->Create( start, end ).Far(), length, trax::epsilon__length / length * 100 );

	spat::Position<Length> endCreated;
	pCurve->Transition( 10_m * trax::pi/2, endCreated );
	BOOST_CHECK(end.Equals(endCreated, trax::epsilon__length));

	spat::Frame<Length,One> startCreated;
	pCurve->Transition( 0_m, startCreated );
	BOOST_CHECK(start.P.Equals(startCreated.P, trax::epsilon__length));
}

BOOST_FIXTURE_TEST_CASE( eepCurveEndpointCreation5, TraxFixtureBase<trax::Factory::EngineTypes::Standard> ){
	auto pCurve = m_Factory.CreateEEPCurve();
	spat::Frame<Length,One> start{ {0_m,0_m,5_m}, {1,0,0}, {0,1,0}, {0,0,1} };
	spat::Position<Length> end{10_m,10_m,0_m};

	Length length = 16.48_m;
	BOOST_CHECK_CLOSE_DIMENSION( pCurve->Create( start, end ).Far(), length, trax::epsilon__length / length * 100 );

	spat::Position<Length> endCreated;
	pCurve->Transition( 10_m * trax::pi/2, endCreated );
	BOOST_CHECK(end.Equals(endCreated, 100 * trax::epsilon__length)); // quite inaccurate...

	spat::Frame<Length,One> startCreated;
	pCurve->Transition( 0_m, startCreated );
	BOOST_CHECK(start.P.Equals(startCreated.P, trax::epsilon__length));
}
*/
BOOST_AUTO_TEST_SUITE_END() // CurveEEPCurve
BOOST_AUTO_TEST_SUITE(CurveCreation)

BOOST_AUTO_TEST_CASE( CreateCurveStartPositionFixedStartTangentInPlane ){
	spat::VectorBundle<Length,One> start{ {0_m,0_m,0_m}, {1,0,0} }; 
	spat::Position<Length> end{10_m,10_m,10_m};
	spat::Vector<One> startTxUp = start.T % Up;

	auto curve = CreateCurve( start, end, Up );

	BOOST_CHECK( curve.first );
	spat::VectorBundle<Length,One> startResult;
	curve.first->Transition( curve.second.Near(), startResult );
	BOOST_CHECK_EQUAL(start.P,startResult.P);
	BOOST_CHECK_EQUAL( startResult.T * startTxUp, One{0} );

	spat::Position<Length> endResult;
	curve.first->Transition( curve.second.Far(), endResult );
	BOOST_CHECK( (end - endResult).Length() <= epsilon__length );
}

BOOST_AUTO_TEST_CASE( CreateLineCurve_StartPositionFixed_StartTangentFixed_EndInMargin ){
	spat::VectorBundle<Length,One> start{ {0_m,0_m,0_m}, {1,0,0} };
	spat::Position<Length> end{10_m,epsilon__length,0_m};
	auto curve = CreateCurve( start, end, Up );

	BOOST_CHECK( curve.first );
	spat::VectorBundle<Length,One> startResult;
	curve.first->Transition( curve.second.Near(), startResult );
	BOOST_CHECK_EQUAL(start,startResult);

	spat::Position<Length> endResult;
	curve.first->Transition( curve.second.Far(), endResult );
	BOOST_CHECK( (end - endResult).Length() <= epsilon__length );
}

BOOST_AUTO_TEST_CASE( CreateArcCurve_StartPositionFixed_StartTangentFixed_EndInMargin ){
	spat::VectorBundle<Length,One> start{ {0_m,0_m,0_m}, {1,0,0} };
	spat::Position<Length> end{10_m,2*epsilon__length,0_m};
	auto curve = CreateCurve( start, end, Up );

	BOOST_CHECK( curve.first );
	spat::VectorBundle<Length,One> startResult;
	curve.first->Transition( curve.second.Near(), startResult );
	BOOST_CHECK_EQUAL(start,startResult);

	spat::Position<Length> endResult;
	curve.first->Transition( curve.second.Far(), endResult );
	BOOST_CHECK( (end - endResult).Length() <= epsilon__length );
}

BOOST_AUTO_TEST_CASE( CreateArcCurve_StartPositionFixed_StartTangentFixed_EndInMargin2 ){
	spat::VectorBundle<Length,One> start{ {0_m,0_m,0_m}, {1,0,0} };
	spat::Position<Length> end{10_m,10_m,epsilon__length};
	auto curve = CreateCurve( start, end, Up );

	BOOST_CHECK( curve.first );
	spat::VectorBundle<Length,One> startResult;
	curve.first->Transition( curve.second.Near(), startResult );
	BOOST_CHECK_EQUAL(start,startResult);

	spat::Position<Length> endResult;
	curve.first->Transition( curve.second.Far(), endResult );
	BOOST_CHECK( (end - endResult).Length() <= epsilon__length );
}

BOOST_AUTO_TEST_SUITE_END() // CurveCreation
BOOST_AUTO_TEST_SUITE_END() //Curve

#endif 



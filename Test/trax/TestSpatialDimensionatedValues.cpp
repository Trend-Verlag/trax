//	trax track library
//	AD 2020 
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

#include "common/Interval.h"
#include "dim/support/DimSupportStream.h"
#include "dim/DimLimits.h"
#include "../Test/dim/BoostTestDimensionedValuesHelpers.h"
#include "../Test/spat/BoostTestSpatialHelpers.h"
#include "spat/Spatial.h"

using namespace common;
using namespace spat;
using namespace dim;

BOOST_AUTO_TEST_SUITE( DimensionatedInterval )

BOOST_AUTO_TEST_CASE ( testDimensionatedIntervalStreaming )
{
	std::stringstream stream;
	common::Interval<Length> intervalOriginal{ 2_m, 12_m }, intervalCopy;

	stream << intervalOriginal;
	stream >> intervalCopy;

	BOOST_CHECK_EQUAL( intervalOriginal, intervalCopy );
}

BOOST_AUTO_TEST_SUITE_END() //DimensionatedInterval

BOOST_AUTO_TEST_SUITE( DimensionatedSpatials )

BOOST_AUTO_TEST_CASE( test_pod )
{
	// guarantee that data structs copy like plain data:
	BOOST_CHECK( std::is_trivially_copyable<Length>::value );

	BOOST_CHECK( std::is_trivially_copyable<Position<Length>>::value );
	BOOST_CHECK( std::is_trivially_copyable<Position2D<Length>>::value );
	BOOST_CHECK( std::is_trivially_copyable<PositionH<Real>>::value );
	BOOST_CHECK( std::is_trivially_copyable<Vector<Velocity>>::value );
	BOOST_CHECK( std::is_trivially_copyable<Vector2D<Real>>::value );
	BOOST_CHECK( (std::is_trivially_copyable<VectorBundle<Length,Velocity>>::value) );
	BOOST_CHECK( (std::is_trivially_copyable<VectorBundle2<Length,One>>::value) );
	BOOST_CHECK( (std::is_trivially_copyable<Frame<Length,One>>::value) );

	BOOST_CHECK(std::is_standard_layout<Length>::value);

	BOOST_CHECK(std::is_standard_layout<Position<Length>>::value);
	BOOST_CHECK(std::is_standard_layout<Position2D<Length>>::value);
	BOOST_CHECK(std::is_standard_layout<PositionH<Real>>::value);
	BOOST_CHECK(std::is_standard_layout<Vector<Velocity>>::value);
	BOOST_CHECK(std::is_standard_layout<Vector2D<Velocity>>::value);
	BOOST_CHECK((std::is_standard_layout<VectorBundle<Length,Velocity>>::value));
	BOOST_CHECK((std::is_standard_layout<VectorBundle2<Length,One>>::value));
	BOOST_CHECK((std::is_standard_layout<Frame<Length,One>>::value));


	//Language behaviour:
	//Position<Real> pos;
	//BOOST_CHECK( (pos.x || !pos.x) );
	Position<Real> posInt{};
	BOOST_CHECK( !posInt.x );
}

BOOST_AUTO_TEST_CASE ( testDimensionatedPositionStreaming )
{
	std::stringstream stream;
	Position<Length> positionOriginal{ 2_m, 12_m, 112_m }, positionCopy;

	stream << positionOriginal;
	stream >> positionCopy;

	BOOST_CHECK_EQUAL( positionOriginal, positionCopy );
}

BOOST_AUTO_TEST_CASE ( testDimensionatedVectorStreaming1 )
{
	std::istringstream stream{ "Vector( 0, 0, 1 )" };
	Vector<One> vectorOriginal{.0,.0,1.}, vectorCopy;

	stream >> vectorCopy;

	BOOST_CHECK_EQUAL( vectorOriginal, vectorCopy );
}

BOOST_AUTO_TEST_CASE ( testDimensionatedVectorStreaming2 )
{
	std::istringstream stream{ "Vector( 0m, 0m, 1m )" };
	Vector<Length> vectorOriginal{ 0_m, 0_m, 1_m }, vectorCopy;

	stream >> vectorCopy;

	BOOST_CHECK_EQUAL( vectorOriginal, vectorCopy );
}

BOOST_AUTO_TEST_CASE ( testDimensionatedDimVectorRoundtrip1 )
{
	std::stringstream strgstr;
	spat::Vector<Length> vOriginal{ 1.0_m, 0.0_m, 95.0075912_km };
	spat::Vector<Length> vResult;
	
	strgstr << vOriginal;
	strgstr >> vResult;

	BOOST_CHECK_EQUAL( vOriginal, vResult );
}

BOOST_AUTO_TEST_CASE ( testDimensionatedDimVectorRoundtrip2 )
{
	std::stringstream strgstr;
	spat::Vector<Length> vOriginal{ 1.0_m, 0.0_m, 95.0075912_m };
	spat::Vector<Length> vResult;
	
	strgstr << vOriginal;
	strgstr >> vResult;

	BOOST_CHECK_CLOSE_SPATIAL( vOriginal, vResult, epsilon__length );
}

BOOST_AUTO_TEST_CASE ( testDimensionatedDimVectorRoundtrip3 )
{
	std::stringstream strgstr;
	spat::Vector<One> vOriginal{ 0.5, 0.5, 0.5 };
	vOriginal.Normalize();
	spat::Vector<One> vResult;

//	{dx={starkNakedNumber=-0.998710990 } dy={starkNakedNumber=-0.0239610001 } dz={starkNakedNumber=-0.0447370000 } }

	strgstr << vOriginal;
	strgstr >> vResult;
	BOOST_CHECK( vOriginal.IsNormal() );
	BOOST_CHECK( vResult.IsNormal() );
	BOOST_CHECK_CLOSE_SPATIAL( vOriginal, vResult, epsilon );
}

BOOST_AUTO_TEST_CASE ( testDimensions )
{
	BOOST_CHECK_CLOSE_DIMENSION( 10_m, 11_m, 11 );

	Frame<Real> frame;
	frame.Init();
	frame.RotateBin( 20_deg );
	Frame<Length,One> frame2{ frame.ptr() };
	Frame<Real> frame3;
	frame3 = frame2.ptr();
	BOOST_CHECK_EQUAL( frame, frame3 );

	auto e = std::numeric_limits<Length>::epsilon();
	BOOST_CHECK_EQUAL( e.Units(), std::numeric_limits<Real>::epsilon() );


	//Length l = 3_m;
	//Time t = 2_s;
	//auto r = l + t;		// does not compile!
	//auto r2 = l + 4;	// does not compile!




	Vector<Velocity> v1{ 1_mIs, 0_mIs, 0_mIs };
	Vector<Velocity> v2{ -1_mIs, 0_mIs, 0_mIs };

	auto sv = abs( v1*v2 );
	BOOST_CHECK_CLOSE_DIMENSION( sv, 1_mIs*1_mIs, epsilon__velocity * epsilon__velocity * 100  / (1_mIs*1_mIs) );

	BOOST_CHECK_EQUAL( Sign( v2.dx ), -1_1 );

	BOOST_CHECK( !Equals( v1.dx, v2.dx, 0.0001_mIs/*epsilon_velocity*/ ) );

	auto p1 = pow<2>(v1.dx);
	auto p2 = pow<2>(v2.dx);
	BOOST_CHECK_CLOSE_DIMENSION( p1, p2, 0.0001_mIs );


	Velocity v3{0};
	v3 += v1.dx;





	{
	//Acceleration g = 9.81_mIs2;
	Mass m = 10_kg;
	Vector<Velocity> v{ -1_mIs, 2_mIs, 3_mIs };
	Energy E = m * v*v/2;
	std::cout << E << std::endl;
	}

	//BOOST_CHECK( E, 18.708f );
	Mass m = 1.2_g;
	Velocity c = 299792458_mIs;
	Energy E = m * c * c;
	std::cout << E << std::endl; // should print: 1.07851e+11kNm

	constexpr spat::Vector<AngularVelocity> w{ spat::Ez<One> * 20_deg / 1_s };
	constexpr spat::Vector<Length> d{ spat::Ex<One> * 2.8_m };
	constexpr spat::Vector<Velocity> v = w % d; 
	std::cout << "The momentary velocity of a point rotating around " << w << " at distance " << d << " is " << v << std::endl;
}

BOOST_AUTO_TEST_CASE ( testDimensionalEquality )
{
	using namespace common;

	Vector<Velocity> v1{ 1_mIs, 0_mIs, 0_mIs };
	Vector<Velocity> v2{ 0_mIs, 2_mIs, 0_mIs };
	BOOST_CHECK( v1.Equals( v1 ) );
	BOOST_CHECK( !v1.Equals( v2 ) );

	Position<Length> P1{1_m,2_m,3_m};
	Position<Length> P2{1_m,1_m,1_m};
	BOOST_CHECK( P1.Equals( P1 ) );
	BOOST_CHECK( !P1.Equals( P2 ) );
}

BOOST_AUTO_TEST_CASE ( testDimensionalNormalize )
{

	Vector<Velocity> V{ 1_mIs, 1_mIs, 1_mIs };
	auto pair = Normalize( V );

	BOOST_CHECK( V.Equals( pair.first * pair.second, epsilon__velocity ) );
}

BOOST_AUTO_TEST_CASE ( testDimensionalVectorAddition )
{
	Vector<Velocity> v1{ 1_mIs, 0_mIs, 0_mIs };
	Vector<Velocity> v2{ 0_mIs, 2_mIs, 0_mIs };
	Vector<Velocity> v3;
	v3 = v1 + v2;
	BOOST_CHECK_EQUAL( v3.dx, 1_mIs );
	BOOST_CHECK_EQUAL( v3.dy, 2_mIs );
	BOOST_CHECK_EQUAL( v3.dz, 0_mIs );
}

BOOST_AUTO_TEST_CASE ( testDimensionalVectorRotation )
{
	Vector<Velocity> v1{ 1_mIs, 0_mIs, 1_mIs };
	v1.Rotate( Vector<One>{ 0, 0, 1 }, pi/2 ); 

	BOOST_CHECK( v1.Equals( Vector<Velocity>{ 0_mIs, 1_mIs, 1_mIs }, 0.00001_mIs ) );

	Vector<Velocity> v2{ 1_mIs, 0_mIs, 1_mIs };
	v2.Rotate( Vector<Angle>{ 0, 0, 1 }, pi/2 ); 

	BOOST_CHECK( v2.Equals( Vector<Velocity>{ 0_mIs, 1_mIs, 1_mIs }, 0.00001_mIs ) );

	Vector<Velocity> v3{ 1_mIs, 0_mIs, 1_mIs };
	v3.Rotate( Vector<Angle>{ 0, 0, pi/2 } );

	BOOST_CHECK( v2.Equals( Vector<Velocity>{ 0_mIs, 1_mIs, 1_mIs }, 0.00001_mIs ) );
}

BOOST_AUTO_TEST_CASE ( testDimensionalVectorMultiplication )
{
	using namespace spat;

	Vector<One> T{ 1, 0, 0 };
	Length distance = 5_m;

	Vector<Length> Distance = distance * T;
	auto length1 = Distance.Length();
	BOOST_CHECK_CLOSE_DIMENSION( length1, 5_m, 0.001f );

	//Vector<Length> Distance2 = T * distance;
	auto length2 = Distance.Length();
	BOOST_CHECK_CLOSE_DIMENSION( length2, 5_m, 0.001f );

	One f = 5.2f;
	Vector<Length> Distance3 = f * Distance;
	BOOST_CHECK_CLOSE_DIMENSION( Distance3.Length(), 26_m, 0.001f );

	Vector<Length> Distance4 = 5 * Distance;
	BOOST_CHECK_CLOSE_DIMENSION( Distance4.Length(), 25_m, 0.001f );

	Vector<Length> Distance5 = 5.2f * Distance;
	BOOST_CHECK_CLOSE_DIMENSION( Distance5.Length(), 26_m, 0.001f );

	Real r = 5.2f;
	Vector<Length> Distance6 = r * Distance;
	BOOST_CHECK_CLOSE_DIMENSION( Distance6.Length(), 26_m, 0.001f );

	Vector<Length> Distance7 = Distance * 3;
	BOOST_CHECK_CLOSE_DIMENSION( Distance7.Length(), 15_m, 0.001f );

	Vector<Length> Distance8 = -Distance7;
	BOOST_CHECK_CLOSE_DIMENSION( Distance8.Length(), 15_m, 0.001f );

	Area a = Distance6 * Distance7;
	BOOST_CHECK_CLOSE_DIMENSION( a, 390_m2, 0.001f );

	Vector<Area> a2 = Distance6 % Distance7;
	BOOST_CHECK_EQUAL( a2, Null<Area> );

	Distance8 /= -1;
	BOOST_CHECK_EQUAL( Distance8, Distance7 );
	Real m1 = -1.0f;
	Distance8 = Distance8 / m1;
	Distance8 /= m1;
	BOOST_CHECK_EQUAL( Distance8, Distance7 );

	Distance8 *= -1;
	Distance7 *= -1.0f;
	Distance7 /= -1.0f;
	Distance7 /= -1;
	BOOST_CHECK_EQUAL( Distance8, Distance7 );

	Vector<AngularVelocity> w{0_1Is,0_1Is,1_1Is};
	Vector<Velocity> v = w % Distance8;
	BOOST_CHECK_EQUAL( v.dy, -15_mIs );
	v = v / 3;
	BOOST_CHECK_EQUAL( v.dy, -5_mIs );

	AnglePerLength c = 10_deg / 2_m;
	Vector<AnglePerLength> w2 = c * T;
	BOOST_CHECK_EQUAL( w2.dx, c );
}

BOOST_AUTO_TEST_CASE ( testDimensionalPositionAlgebra )
{
	using namespace spat;

	Position<Length> P1{1_m,2_m,3_m};
	Position<Length> P2{1_m,1_m,1_m};

	Vector<Length> V1 = P2 - P1;
	BOOST_CHECK_EQUAL( V1.Length(), sqrt(5_m2) );

	One f = 5.2f;
	Position<Length> P3 = f * P2;
	Real r = 5.2f;
	Position<Length> P4 = r * P2;
	BOOST_CHECK_EQUAL( P3, P4 );

	Position<Length> P5 = P4 / r;
	P4 /= r;
	BOOST_CHECK_EQUAL( P4, P5 );
	BOOST_CHECK_EQUAL( P4, P2 );
}

BOOST_AUTO_TEST_CASE ( testDimensionalFrameAlgebra )
{
	Frame<Length,One> frame;
	frame.Init();

	frame.TransportTan( 10_m );
	frame.OrthoNormalize();
	BOOST_CHECK( frame.IsOrthoNormal() );

	Position<Length> P1{1_m,1_m,1_m};
	frame.FromParent( P1 );
	Position<Length> P2{-9_m,1_m,1_m};
	BOOST_CHECK_EQUAL( P1, P2 );

	Vector<Length> D1{1_m,1_m,1_m};
	frame.FromParent( D1 );
	frame.ToParent( D1 );

	frame.RotateBin( 90_deg );
	Vector<One> V1{1,0,0};
	frame.FromParent( V1 );
	Vector<One> V2{0,-1,0};
	BOOST_CHECK( V1.Equals(V2,epsilon__angle) );

	frame.ToParent( V1 );
	frame.LookTAt( Ex<One> );
	BOOST_CHECK( V1.Equals(frame.T,epsilon__angle) );

	frame.Round( 3 );
	BOOST_CHECK_EQUAL( V1, frame.T );

	Frame<Length,Length> frame2;
	frame2.Init();
	BOOST_CHECK( frame2.IsOrthoNormal() );
}

BOOST_AUTO_TEST_CASE ( testDimensionalFrameTransformation )
{
	Frame<Length,One> frame;
	frame.Init();
	frame.TransportTan( 10_m );
	frame.RotateBin( 90_deg );

	//transform elements with different valtypes:
	Vector<Velocity> V1{Ex<Velocity>};
	frame.FromParent( V1 );
	frame.ToParent( V1 );
	BOOST_CHECK_EQUAL( V1, Ex<Velocity> );

	V1 = frame * V1;
	frame.FromParent( V1 );
	BOOST_CHECK_EQUAL( V1, Ex<Velocity> );

	Vector<Real> V2{Ex<Real>};
	frame.FromParent( V2 );
	frame.ToParent( V2 );
	BOOST_CHECK_EQUAL( V2, Ex<Real> );

	V2 = frame * V2;
	frame.FromParent( V2 );
	BOOST_CHECK_EQUAL( V2, Ex<Real> );

	Position<Real> P0{1,0,0}, P1{1,0,0};
	frame.FromParent( P1 );
	frame.ToParent( P1 );
	BOOST_CHECK_EQUAL( P1, P0 );

	P1 = frame * P1;
	frame.FromParent( P1 );
	BOOST_CHECK( P1.Equals( P0, epsilon ) );
}

BOOST_AUTO_TEST_CASE( testFrameOrthonormalize )
{
	Frame<Length,One> startFrame{ {1_m,2_m,3_m}, {1,1,1}, {-1,1,0}, {0,0,1} };
	startFrame.OrthoNormalize();
	BOOST_CHECK( startFrame.IsOrthoNormal() );
}

BOOST_AUTO_TEST_CASE ( testDimensionalMatrixAlgebra )
{
	Position<Length> P1{1_m,2_m,3_m};
	PositionH<One> H1{ P1 };
	Transformation<One> T1;
	T1.CreateRotation( 10_deg, 20_deg, 0_deg );
	PositionH<One> H2 = T1 * H1;
	T1.Invert();
	H2 = T1 * H2;
	BOOST_CHECK( H1.Equals( H2, 0.0001f ) );
	Position<Length> P2{ H2 };
	BOOST_CHECK( P1.Equals( P2, epsilon__length ) );

	Frame<Length,One> base, start, absolute;
	base.Init();
	start.Init();
	absolute.Init();

	Frame<Length,One> frame{ Transformation<One>{ absolute }.Invert() * 
							 Transformation<One>{ start } * 
							 Transformation<One>{ base }.Invert() };

	BOOST_CHECK( frame.IsOrthoNormal() );
}

BOOST_AUTO_TEST_CASE ( testDimensionatedValueAircraftPrincipalAxes )
{
	Vector<Length> vector{ 10_m, 0_m, 0_m };

	vector.AircraftPrincipalAxes( 10_deg, 20_deg );
	BOOST_CHECK_EQUAL( vector.Length(), 10_m );

	auto angles = vector.AircraftPrincipalAxes();
	BOOST_CHECK_EQUAL( angles.x, 10_deg );
	BOOST_CHECK_EQUAL( angles.y, 20_deg );
}

BOOST_AUTO_TEST_SUITE_END() //DimensionatedSpatials

#endif // WITH_BOOST_TESTS
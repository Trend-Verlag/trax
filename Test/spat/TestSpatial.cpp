//	trax track library
//	AD 2013 
//
//  "Tell me what you want,
//   what you really really want"
//
//					Spice Girls
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

#include "common/Helpers.h"

namespace spat{
	using common::pow;
}
#include "spat/Spatial.h"
#include "common/NarrowCast.h"
#include "BoostTestSpatialHelpers.h"

#include <numeric>
#include <iostream>

#include <type_traits>

using namespace spat;
using namespace common;

using Real = float;

inline constexpr Real pi = static_cast<Real>(3.14159265358979323846264338327950288);

BOOST_AUTO_TEST_SUITE(SpatialCoordinatesAreNotVectors_tests)

BOOST_AUTO_TEST_CASE( test_pod )
{
	// guarantee that data structs copy like plain data:
	BOOST_CHECK( std::is_trivially_copyable<Real>::value );

	BOOST_CHECK( std::is_trivially_copyable<Position<Real>>::value );
	BOOST_CHECK( std::is_trivially_copyable<Position2D<Real>>::value );
	BOOST_CHECK( std::is_trivially_copyable<PositionH<Real>>::value );
	BOOST_CHECK( std::is_trivially_copyable<Vector<Real>>::value );
	BOOST_CHECK( std::is_trivially_copyable<Vector2D<Real>>::value );
	BOOST_CHECK( std::is_trivially_copyable<VectorBundle<Real>>::value );
	BOOST_CHECK( std::is_trivially_copyable<VectorBundle2<Real>>::value );
	BOOST_CHECK( std::is_trivially_copyable<Frame<Real>>::value );

	BOOST_CHECK(std::is_standard_layout<Real>::value);

	BOOST_CHECK(std::is_standard_layout<Position<Real>>::value);
	BOOST_CHECK(std::is_standard_layout<Position2D<Real>>::value);
	BOOST_CHECK(std::is_standard_layout<PositionH<Real>>::value);
	BOOST_CHECK(std::is_standard_layout<Vector<Real>>::value);
	BOOST_CHECK(std::is_standard_layout<Vector2D<Real>>::value);
	BOOST_CHECK(std::is_standard_layout<VectorBundle<Real>>::value);
	BOOST_CHECK(std::is_standard_layout<VectorBundle2<Real>>::value);
	BOOST_CHECK(std::is_standard_layout<Frame<Real>>::value);


	//Language behaviour:
	//Position<Real> pos;
	//BOOST_CHECK( (pos.x || !pos.x) );
	Position<Real> posInt{};
	BOOST_CHECK( !posInt.x );
}

BOOST_AUTO_TEST_SUITE(Position_tests)

BOOST_AUTO_TEST_CASE( transport )
{
	Position<Real> p1( 1,2,3);
	Vector<Real> v1( 1,1,1 );

	p1.Transport( v1 );

	BOOST_CHECK( p1.x == 2 );
	BOOST_CHECK( p1.y == 3 );
	BOOST_CHECK( p1.z == 4 );
}

BOOST_AUTO_TEST_CASE( equals )
{
	Position<Real> p1( 1,2,3);
	Position<Real> p2( 1,2,3.00005f);

	BOOST_CHECK( !p1.Equals( p2, 0.00004f ) );
	p2.z = 3.00003f;
	BOOST_CHECK( p1.Equals( p2, 0.00004f ) );
}

BOOST_AUTO_TEST_CASE( positionIntegerValuesDividedByFraction )
{
	Position<int> p1{ 1, 2, 3 }, p2{ 2, 4, 6 };
	Real f = 0.5f;
	p1 /= f;
	BOOST_CHECK_EQUAL( p1, p2 );
	Position<int> p3 = p1 / f;
	Position<int> p4{ 4, 8, 12 };
	BOOST_CHECK_EQUAL( p3, p4 );

	Position2D<int> p2D1{ 1, 2 }, p2D2{ 2, 4 };
	p2D1 /= f;
	BOOST_CHECK_EQUAL( p2D1, p2D2 );
	Position2D<int> p2D3 = p2D1 / f;
	Position2D<int> p2D4{ 4, 8 };
	BOOST_CHECK_EQUAL( p2D3, p2D4 );
}

BOOST_AUTO_TEST_CASE( positionIntegerValuesMultipliedByFraction )
{
	Position<int> p1{ 4, 8, 12 }, p2{ 2, 4, 6 };
	Real f = 0.5f;
	p1 *= f;
	BOOST_CHECK_EQUAL( p1, p2 );
	Position<int> p3 = p1 * f;
	Position<int> p4{ 1, 2, 3 };
	BOOST_CHECK_EQUAL( p3, p4 );

	Position2D<int> p2D1{ 4, 8 }, p2D2{ 2, 4 };
	p2D1 *= f;
	BOOST_CHECK_EQUAL( p2D1, p2D2 );
	Position2D<int> p2D3 = p2D1 * f;
	Position2D<int> p2D4{ 1, 2 };
	BOOST_CHECK_EQUAL( p2D3, p2D4 );
}

BOOST_AUTO_TEST_SUITE_END() // Position_tests
BOOST_AUTO_TEST_SUITE(Vector_tests)

BOOST_AUTO_TEST_CASE( constexpression )
{
	constexpr Vector<Real> v0 = Null<Real>;
	constexpr Vector<Real> v1 = Ex<Real>;
	constexpr Vector<Real> v2 = Ey<Real>;
	constexpr Vector<Real> v3 = Ez<Real>;

	BOOST_CHECK_EQUAL( v0[0], 0 );
	BOOST_CHECK_EQUAL( v1[0], 1 );
	BOOST_CHECK_EQUAL( v2[1], 1 );
	BOOST_CHECK_EQUAL( v3[2], 1 );

	constexpr Real trace = v1.dx + v2.dy + v3.dz;
	BOOST_CHECK_EQUAL( trace, 3 );
}

BOOST_AUTO_TEST_CASE( equals )
{
	Vector<Real> v1( 1,2,3);
	Vector<Real> v2( 1,2,3.00005f);

	BOOST_CHECK( v1.Equals(v1,0) && v2.Equals(v2,0) );

	BOOST_CHECK( !v1.Equals( v2, 0.00004f ) && ! v2.Equals( v1, 0.00004f ));
	v2.dz = 3.00003f;
	BOOST_CHECK( v1.Equals( v2, 0.00004f ) && v2.Equals( v1, 0.00004f ) );	
}

BOOST_AUTO_TEST_CASE( rotate )
{
	Vector<Real> v(1,0,1);
	v.Rotate( Vector<Real>( 0, 0, pi/2 ) );

	BOOST_CHECK( v.Equals( Vector<Real>( 0, 1, 1 ), 0.00001f ) );
}

BOOST_AUTO_TEST_CASE( mass_rotate )
{
	std::vector<Vector<Real>> vec( 100, Vector<Real>(1,0,1) );

	Rotate( vec, Vector<Real>( 0, 0, pi/2 ) );

	BOOST_CHECK( vec[50].Equals( Vector<Real>( 0, 1, 1 ), 0.00001f ) );
}

BOOST_AUTO_TEST_CASE( subtract_length )
{
	Vector<Real> v1(5434,975,476);
	Vector<Real> v2(6347,984,432);

	BOOST_CHECK_CLOSE( (v2 - v1).Length(), 914.1039328, 0.0001 );
}

BOOST_AUTO_TEST_CASE( vectorIntegerValuesDividedByFraction )
{
	Vector<int> p1{ 1, 2, 3 }, p2{ 2, 4, 6 };
	Real f = 0.5f;
	p1 /= f;
	BOOST_CHECK_EQUAL( p1, p2 );
	Vector<int> p3 = p1 / f;
	Vector<int> p4{ 4, 8, 12 };
	BOOST_CHECK_EQUAL( p3, p4 );

	Vector2D<int> p2D1{ 1, 2 }, p2D2{ 2, 4 };
	p2D1 /= f;
	BOOST_CHECK_EQUAL( p2D1, p2D2 );
	Vector2D<int> p2D3 = p2D1 / f;
	Vector2D<int> p2D4{ 4, 8 };
	BOOST_CHECK_EQUAL( p2D3, p2D4 );
}

BOOST_AUTO_TEST_CASE( vectorIntegerValuesMultipliedByFraction )
{
	Vector<int> p1{ 4, 8, 12 }, p2{ 2, 4, 6 };
	Real f = 0.5f;
	p1 *= f;
	BOOST_CHECK_EQUAL( p1, p2 );
	Vector<int> p3 = p1 * f;
	Vector<int> p4{ 1, 2, 3 };
	BOOST_CHECK_EQUAL( p3, p4 );

	Vector2D<int> p2D1{ 4, 8 }, p2D2{ 2, 4 };
	p2D1 *= f;
	BOOST_CHECK_EQUAL( p2D1, p2D2 );
	Vector2D<int> p2D3 = p2D1 * f;
	Vector2D<int> p2D4{ 1, 2 };
	BOOST_CHECK_EQUAL( p2D3, p2D4 );
}

BOOST_AUTO_TEST_CASE( vector_AircraftPrincipalAxes )
{
	Vector<Real> vector;
	vector.Init();
	Position2D<Real> angles{ pi/8, pi/4 };
	vector.AircraftPrincipalAxes( angles );
	Position2D<Real> results = vector.AircraftPrincipalAxes();
	BOOST_CHECK_CLOSE_SPATIAL( angles, results, 0.001f );

	angles *= -1.f;
	vector.AircraftPrincipalAxes( angles );
	results = vector.AircraftPrincipalAxes();
	BOOST_CHECK_CLOSE_SPATIAL( angles, results, 0.001f );
}

BOOST_AUTO_TEST_CASE( vector_AircraftPrincipalAxesGimbleLock )
{
	Vector<Real> vector;
	vector.Init();
	Position2D<Real> angles{ pi/8, pi/2 };
	vector.AircraftPrincipalAxes( angles );
	Position2D<Real> results = vector.AircraftPrincipalAxes();
	BOOST_CHECK_CLOSE( 0.f, results.x, 0.001f );
	BOOST_CHECK_CLOSE( angles.y, results.y, 0.001f );

	angles *= -1.f;
	vector.AircraftPrincipalAxes( angles );
	results = vector.AircraftPrincipalAxes();
	BOOST_CHECK_CLOSE( 0.f, results.x, 0.001f );
	BOOST_CHECK_CLOSE( angles.y, results.y, 0.001f );
}

BOOST_AUTO_TEST_SUITE_END() //Vector_tests
BOOST_AUTO_TEST_SUITE(Frame_tests)

BOOST_AUTO_TEST_CASE( constexpression )
{
	constexpr Frame<Real> f3{ Identity<Real> };
	constexpr Vector<Real> U = f3.T % f3.N;
	static_assert( U == f3.B );

	BOOST_CHECK_EQUAL( U, f3.B );
}

BOOST_AUTO_TEST_CASE( frame_transform )
{
	Frame<Real> f1(
		Position<Real>(12,4,-8),
		Vector<Real>(1,0,0),
		Vector<Real>(0,1,0),
		Vector<Real>(0,0,1) );
			
	Frame<Real> f2(
		Position<Real>(2,78,90),
		Vector<Real>(0,1,0),
		Vector<Real>(-1,0,0),
		Vector<Real>(0,0,1) );
	Frame<Real> ftest = f2;

	f1.ToParent( f2 );
	f1.FromParent( f2 );

	BOOST_CHECK( ftest.Equals( f2, 0.001f, 0.001f ) );

	Frame<Real> ftest2 = f1;
	f2.ToParent( f1 );
	f2.FromParent( f1 );

	BOOST_CHECK( ftest2.Equals( f1, 0.001f, 0.001f ) );
}

BOOST_AUTO_TEST_CASE( frame_conversion )
{
	Frame<Real> frame(
		Position<Real>(2,78,90),
		Vector<Real>(0,1,0),
		Vector<Real>(-1,0,0),
		Vector<Real>(0,0,1) );

	VectorBundle2<Real> vb2( frame );

	BOOST_CHECK( vb2.P.Equals( frame.P, 0.001f ) );
	BOOST_CHECK( vb2.T.Equals( frame.T, 0.001f ) );
	BOOST_CHECK( vb2.N.Equals( frame.N, 0.001f ) );
	//-------------

	VectorBundle2<Real> vb22( 
		Position<Real>(12,4,-8),
		Vector<Real>(1,0,0),
		Vector<Real>(0,1,0) );

	Frame<Real> frame2(	vb22 );
	BOOST_CHECK( vb22.P.Equals( frame2.P, 0.001f ) );
	BOOST_CHECK( vb22.T.Equals( frame2.T, 0.001f ) );
	BOOST_CHECK( vb22.N.Equals( frame2.N, 0.001f ) );
	//---------------
}

BOOST_AUTO_TEST_CASE( frame_orthonormalize )
{
	Frame<Real> startFrame{ {1,2,3}, {1,1,1}, {-1,1,0}, {0,0,1} };
	startFrame.OrthoNormalize();
	BOOST_CHECK( startFrame.IsOrthoNormal() );
}

BOOST_AUTO_TEST_CASE( frame_lookat )
{
	Frame<Real> frame, reference;
	frame.Init();
	reference.Init();

	frame.LookTAt( Vector<Real>( -1, 0, 0 ) );
	BOOST_CHECK( frame.T.Equals( -reference.T, 0.001f ) );
	BOOST_CHECK( frame.N.Equals( -reference.N, 0.001f ) );
	BOOST_CHECK( frame.B.Equals( reference.B, 0.001f ) );

	frame.Init();
	Vector<Real> v1( -1, 1, 0 );
	v1.Normalize();
	Vector<Real> v2( -1, -1, 0 );
	v2.Normalize();

	frame.LookTAt( v1 );
	BOOST_CHECK( frame.T.Equals( v1, 0.001f ) );
	BOOST_CHECK( frame.N.Equals( v2, 0.001f ) );
	BOOST_CHECK( frame.B.Equals( reference.B, 0.001f ) );
}

BOOST_AUTO_TEST_CASE( frame_lookat2 )
{
	Frame<Real> frame = { 
		{ -91.5103836, -108.284927, 151.642899 }, 
		{ -0.915139854, -0.403132617, -0.00173116114 }, 
		{ 0.403133273, -0.915141225, 8.66334133e-08 }, 
		{ -0.00158429239, -0.000697809621, 0.999998331 } };
	Frame<Real> reference = frame;

	frame.LookAt( Ez<Real>, -frame.N );
	BOOST_CHECK( frame.B.Equals( -reference.N, 0.01f ) );
}

BOOST_AUTO_TEST_CASE( frame_lookat3 )
{
	Frame<Real> frame = { 
		{ 0, 0, 0 }, 
		Ez<Real>, 
		Ex<Real>, 
		Ey<Real> };
//	Frame<Real> reference = frame;

	frame.LookAt( Ez<Real>, Ey<Real> );
	BOOST_CHECK( frame.B.Equals( Ey<Real>, 0.01f ) );
}

BOOST_AUTO_TEST_CASE( frame_AircraftPrincipalAxes )
{
	Frame<Real> frame;
	frame.Init();
	Position<Real> angles{ pi/8, pi/4, pi/2 };
	frame.AircraftPrincipalAxes( angles );
	Position<Real> results = frame.AircraftPrincipalAxes();
	BOOST_CHECK_CLOSE_SPATIAL( angles, results, 0.001f );

	angles *= -1.f;
	frame.AircraftPrincipalAxes( angles );
	results = frame.AircraftPrincipalAxes();
	BOOST_CHECK_CLOSE_SPATIAL( angles, results, 0.001f );
}

BOOST_AUTO_TEST_CASE( frame_AircraftPrincipalAxesSeconquarterForYaw )
{
	Frame<Real> frame;
	frame.Init();
	Position<Real> angles{ 3*pi/4, pi/4, pi/2 };
	frame.AircraftPrincipalAxes( angles );
	Position<Real> results = frame.AircraftPrincipalAxes();
	BOOST_CHECK_CLOSE_SPATIAL( angles, results, 0.001f );

	angles *= -1.f;
	frame.AircraftPrincipalAxes( angles );
	results = frame.AircraftPrincipalAxes();
	BOOST_CHECK_CLOSE_SPATIAL( angles, results, 0.001f );
}

BOOST_AUTO_TEST_CASE( frame_AircraftPrincipalAxesGimbleLock )
{
	Frame<Real> frame;
	frame.Init();
	Position<Real> angles{ pi/8, pi/2, pi/4 };
	frame.AircraftPrincipalAxes( angles );
	Position<Real> results = frame.AircraftPrincipalAxes();
	BOOST_CHECK_CLOSE( 0.f, results.x, 0.001f );
	BOOST_CHECK_CLOSE( angles.y, results.y, 0.001f );

	angles *= -1.f;
	frame.AircraftPrincipalAxes( angles );
	results = frame.AircraftPrincipalAxes();
	BOOST_CHECK_CLOSE( 0.f, results.x, 0.001f );
	BOOST_CHECK_CLOSE( angles.y, results.y, 0.001f );
}

BOOST_AUTO_TEST_CASE( frame_Rotate )
{
	Frame<Real> frame;
	frame.Init();
	VectorBundle<Real,Real> axis{ { 10, 0, 0 }, {0, 0, pi/2 } };

	frame.Rotate( axis );

	Frame<Real> result{ { 10, -10, 0 }, Ey<Real>, -Ex<Real>, Ez<Real> };
	BOOST_CHECK_CLOSE_SPATIAL2( frame, result, 0.001f, 0.001f );
}

BOOST_AUTO_TEST_SUITE_END() //Frame_tests
BOOST_AUTO_TEST_SUITE(Matrix_tests)

///////////////////////////////////////
BOOST_AUTO_TEST_CASE( matrixIdentity )
{
	for( unsigned short i = 0; i < 4; ++i ){
		for( unsigned short j = 0; j < 4; ++j ){
			if( i == j )
				BOOST_CHECK_EQUAL( IdentityTransformation<Real>( i, j ), 1 );
			else
				BOOST_CHECK_EQUAL( IdentityTransformation<Real>( i, j ), 0 );
		}
	}
}

BOOST_AUTO_TEST_CASE( matrixConstruction )
{
	Rotation<Real> R1, R2, R3;
	R1.SetIdentity();
	R2.SetIdentity();
	R3.SetIdentity();
	Matrix<Real,3,3> M = R1;
	SquareMatrix<Real,3> S = R2;
	Rotation<Real> R = R3;

	BOOST_CHECK( M.IsEqual( R1 ) );
	BOOST_CHECK( S.IsEqual( R2 ) );
	BOOST_CHECK( R.IsEqual( R3 ) );
	BOOST_CHECK( S.IsIdentity() );
	BOOST_CHECK( R.IsIdentity() );
}

BOOST_AUTO_TEST_CASE( matrixConstructionShortInitializerList )
{
	SquareMatrix<Real,2> S1{ 1.f, 2.f, 3.f };
	BOOST_CHECK( S1.IsEqual( SquareMatrix<Real,2>{ 1.f, 2.f, 3.f, 0.f } ) );
}

BOOST_AUTO_TEST_CASE( matrixConstructionLongInitializerList )
{
	SquareMatrix<Real,2> S1{ 1.f, 2.f, 3.f, 4.f, 5.f };
	BOOST_CHECK( S1.IsEqual( SquareMatrix<Real,2>{ 1.f, 2.f, 3.f, 4.f } ) );
}

BOOST_AUTO_TEST_CASE( matrixMove )
{
	SquareMatrix<Real,2> S1{ 1.f, 2.f, 3.f, 4.f };
	SquareMatrix<Real,2> S2 = std::move( S1 );

	BOOST_CHECK_EQUAL( S2( 0, 0 ), 1.f );
	BOOST_CHECK_EQUAL( S2( 1, 0 ), 2.f );
	BOOST_CHECK_EQUAL( S2( 0, 1 ), 3.f );
	BOOST_CHECK_EQUAL( S2( 1, 1 ), 4.f );

//	BOOST_CHECK_EQUAL( S1( 0, 0 ), 1.f );
}

BOOST_AUTO_TEST_CASE( matrixAssignment )
{
	Rotation<Real> R1, R2, R3;
	R1.SetIdentity();
	R2.SetIdentity();
	R3.SetIdentity();
	Matrix<Real,3,3> M;
	SquareMatrix<Real,3> S;
	Rotation<Real> R;

	M = R1;
	S = R2;
	R = R3;

	BOOST_CHECK( M.IsEqual( R1 ) );
	BOOST_CHECK( S.IsEqual( R2 ) );
	BOOST_CHECK( R.IsEqual( R3 ) );
	BOOST_CHECK( S.IsIdentity() );
	BOOST_CHECK( R.IsIdentity() );
}

BOOST_AUTO_TEST_CASE( testMultiply )
{
	//constexpr bool is = std::is_pod<Real>::value;
	//using type = std::enable_if<std::is_pod<SquareMatrix<Real,4>>::value, int>::type;
	//type what = 1;



	Matrix<Real,4,4> m1,m2,m3;
	m3 = m1 * m2;

	SquareMatrix<Real,4> s1,s2,s3;
	s1.SetIdentity();
	s2.SetIdentity();

	s3 = s1 * s2;

	BOOST_CHECK( s3.IsIdentity() );

	Transformation<Real> t1,t2,t3;
	t1.SetIdentity();
	t2.SetIdentity();

	t3 = t1 * t2;

	BOOST_CHECK( t3.IsIdentity() );

	Rotation<Real> r1,r2,r3;
	r1.SetIdentity();
	r2.SetIdentity();

	r3 = r1 * r2;

	BOOST_CHECK( r3.IsIdentity() );

	auto x1 = m1 * s1;
	auto x2 = s2 * t1;
	auto x3 = m1 * t2;

	Real f = 1.0f;
	auto x4 = f * m1;
	auto x5 = f * s1;
	auto x6 = f * r1;

	// not implemented due to compiler fault...
	//auto x7 = m1 * f;
	//auto x8 = s1 * f;
	//auto x9 = r1 * f;
}
///////////////////////////////////////
#ifdef TRAX_OPEN_SOURCE
BOOST_AUTO_TEST_CASE( testRotationCalc )
{
	Rotation<Real> R{	0,0,0,
						0,0,0,
						0,0,0 };

	R.CreateFromAxis( pi/2, 0.0f, 0.0f );
	BOOST_CHECK_EQUAL( R.RotationAxis(), Vector<Real>( 1.0f, 0.0f, 0.0f ) );
	BOOST_CHECK_EQUAL( R.RotationAngle(), pi/2 );

	R.CreateFromAxis( 0.0f, pi/2, 0.0f );
	BOOST_CHECK_EQUAL( R.RotationAxis(), Vector<Real>( 0.0f, 1.0f, 0.0f ) );
	BOOST_CHECK_EQUAL( R.RotationAngle(), pi/2 );

	R.CreateFromAxis( 0.0f, 0.0f, pi/2 );
	BOOST_CHECK_EQUAL( R.RotationAxis(), Vector<Real>( 0.0f, 0.0f, 1.0f ) );
	BOOST_CHECK_EQUAL( R.RotationAngle(), pi/2 );

	Vector<Real> Axis( pi/8, pi/4, pi/2 );
	R.CreateFromAxis( Axis );
	Real Angle = Axis.Normalize();
	BOOST_CHECK( Axis.Equals( R.RotationAxis(), 0.001f ) );
	BOOST_CHECK_CLOSE( R.RotationAngle(), Angle, 0.001f );

	Vector<Real> rotAxis( 1, 2, 3 );
	R.CreateFromAxis( rotAxis );
	Angle = rotAxis.Normalize();
	Angle = common::Wrap( Angle, -3.14159265358979, +3.14159265358979 );
	BOOST_CHECK_CLOSE( abs(R.RotationAngle()), abs(Angle), 0.001f );
	BOOST_CHECK_CLOSE_SPATIAL( rotAxis, R.RotationAxis(), 0.001f );

	Frame<Real> frame( R );
	Rotation<Real> spatmatB( frame );

	BOOST_CHECK_EQUAL( R, spatmatB );
}
#endif // TRAX_OPEN_SOURCE
///////////////////////////////////////
BOOST_AUTO_TEST_CASE ( testDismantle )
{
	Transformation<Real> T, R, S;
	T.CreateTranslation	( 1.0f, 2.0f, 3.0f );
	R.CreateRotation	( pi/2, 0.0f, 0.0f );
	S.CreateScaling		( 1.0f, 2.0f, 1.0 );

	Transformation<Real> M = T * R * S;

	Transformation<Real> T2, R2, S2;
	BOOST_CHECK(M.Dismantle( T2, R2, S2 ));
	BOOST_CHECK_EQUAL( T, T2 );
	BOOST_CHECK_EQUAL( R, R2 );
	BOOST_CHECK_EQUAL( S, S2 );


	T.CreateTranslation	( 1.0f, 2.0f, 3.0f );
	R.CreateRotation	( pi/2, pi/4, pi/8 );
	S.CreateScaling		( 1.0f, 2.0f, 1.0 );

	M = T * R * S;

	BOOST_CHECK(M.Dismantle( T2, R2, S2 ));
	BOOST_CHECK_EQUAL( T, T2 );
	BOOST_CHECK_EQUAL( R, R2 );
	BOOST_CHECK_EQUAL( S, S2 );
}
///////////////////////////////////////
#ifdef TRAX_OPEN_SOURCE
BOOST_AUTO_TEST_CASE ( testSlerp )
{
	Transformation<Real> A, B, T;

	A.CreateRotation( pi/2, 0, 0 );
	B.CreateRotation( pi, 0, 0 );

	BOOST_CHECK(Slerp( T, A, B, narrow_cast<Real>(0.5) ));

	Rotation<Real> R = T;
	BOOST_CHECK_CLOSE( R.RotationAngle(), 3*pi/4, narrow_cast<Real>(0.001f) );

	BOOST_CHECK(Slerp( T, A, B, narrow_cast<Real>(0) ));
	BOOST_CHECK_EQUAL( T, A );

	BOOST_CHECK(Slerp( T, A, B, narrow_cast<Real>(1) ));
	BOOST_CHECK_EQUAL( T, B );

	BOOST_CHECK(Slerp( T, IdentityTransformation<Real>, A, narrow_cast<Real>(0.5) ));
	R = T;
	BOOST_CHECK_CLOSE( R.RotationAngle(), pi/4, 0.001f );
}
#endif // TRAX_OPEN_SOURCE
///////////////////////////////////////
BOOST_AUTO_TEST_CASE ( testMatrixMultiplication )
{
	Transformation<int> A{ 0, 1, 2, 3,
								4, 5, 6, 7,
								8, 9, 10, 11, 
								12, 13, 14, 15 };

	Transformation<int> B{ 0, 1, 2, 3,
								4, 5, 6, 7,
								8, 9, 10, 11, 
								12, 13, 14, 15 };

	Transformation<int> T{ 56, 62, 68, 74,
								152, 174, 196, 218,
								248, 286, 324, 362,
								344, 398, 452, 506 };

	Transformation<int> R = A * B;

	BOOST_CHECK_EQUAL( R(0,0) , T(0,0) );
	BOOST_CHECK_EQUAL( R(1,0) , T(1,0) );
	BOOST_CHECK_EQUAL( R(2,0) , T(2,0) );
	BOOST_CHECK_EQUAL( R(3,0) , T(3,0) );

	BOOST_CHECK_EQUAL( R(0,1) , T(0,1) );
	BOOST_CHECK_EQUAL( R(1,1) , T(1,1) );
	BOOST_CHECK_EQUAL( R(2,1) , T(2,1) );
	BOOST_CHECK_EQUAL( R(3,1) , T(3,1) );

	BOOST_CHECK_EQUAL( R(0,2) , T(0,2) );
	BOOST_CHECK_EQUAL( R(1,2) , T(1,2) );
	BOOST_CHECK_EQUAL( R(2,2) , T(2,2) );
	BOOST_CHECK_EQUAL( R(3,2) , T(3,2) );

	BOOST_CHECK_EQUAL( R(0,3) , T(0,3) );
	BOOST_CHECK_EQUAL( R(1,3) , T(1,3) );
	BOOST_CHECK_EQUAL( R(2,3) , T(2,3) );
	BOOST_CHECK_EQUAL( R(3,3) , T(3,3) );

	BOOST_CHECK_EQUAL( R , T );
}
///////////////////////////////////////
BOOST_AUTO_TEST_CASE ( testMatrixInversion )
{
	Transformation<Real> O1{	0.5f, 0.0f, 0.0f, 0.0f,
								0.0f, 0.5f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.5f, 0.0f,
								0.0f, 0.0f, 0.0f, 0.5f };

	Transformation<Real> I1{	0.5f, 0.0f, 0.0f, 0.0f,
								0.0f, 0.5f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.5f, 0.0f,
								0.0f, 0.0f, 0.0f, 0.5f };

	BOOST_CHECK_NO_THROW( I1.Invert() );
	BOOST_CHECK_NO_THROW( I1.Invert() );

	BOOST_CHECK_EQUAL( O1 , I1 );


	Transformation<Real> O2{	0.5f, 7.0f, 1.0f, 23.0f,
								2.0f, 2.5f, 8.0f, 5.0f,
								6.0f, 1.0f, 3.5f, 9.0f,
								4.0f, 1.0f, 3.0f, 8.5f };

	Transformation<Real> I2{	0.5f, 7.0f, 1.0f, 23.0f,
								2.0f, 2.5f, 8.0f, 5.0f,
								6.0f, 1.0f, 3.5f, 9.0f,
								4.0f, 1.0f, 3.0f, 8.5f };

	BOOST_CHECK_NO_THROW( I2.Invert() );
	BOOST_CHECK_NO_THROW( I2.Invert() );

	BOOST_CHECK( I2.IsEqual( O2, 0.0001f ) );

	SquareMatrix<Real,3> O3;
	O3(0,0) = 0.0f;
	O3(1,0) = 1.0f;
	O3(2,0) = 2.0f;
	O3(0,1) = 3.0f;
	O3(1,1) = 4.0f;
	O3(2,1) = 5.0f;
	O3(0,2) = 6.0f;
	O3(1,2) = 7.0f;
	O3(2,2) = 8.0f;

	BOOST_CHECK_EQUAL( Determinant(O3), 0.0f );


	Transformation<Real> V;
	V.CreateViewport(	1024,
						768,
						0.0f,
						1.0f );

	Transformation<Real> P;
	P.CreateCameraProjection( 
		1.5707963267948966192313216916398f, 
		1024.0f/768, 
		0.0001f, 
		10000.0f );

	Transformation<Real> T = V * P;
	Transformation<Real> IT( T );
	IT.Invert();

	Transformation<Real> E = IT * T;
	BOOST_CHECK( E.IsIdentity( 0.00001f ) );
}

BOOST_AUTO_TEST_CASE ( testTransformationFrame )
{
	Frame<Real> frameA;
	frameA.Init();
	frameA.P = Position<Real>( 1,2,3 );
	frameA.RotateTan( 1.2345f );
	frameA.RotateNor( 0.2345f );
	frameA.RotateBin( 2.2345f );

	Transformation<Real> trans(	frameA );
	Frame<Real> frameB(trans);

	BOOST_CHECK( frameA.Equals( frameB, 0.0001f, 0.0001f ) );

	frameA.Init();
	frameA.P = Position<Real>( 4,5,6 );
	frameA.RotateTan( 0.2345f );
	frameA.RotateNor( 2.2345f );
	frameA.RotateBin( 1.2345f );

	trans = frameA;
	Frame<Real> frameC(trans);

	BOOST_CHECK( frameA.Equals( frameC, 0.0001f, 0.0001f ) );
}

BOOST_AUTO_TEST_CASE ( testMirrorTransformationFrame )
{
	Transformation<Real> M;
	M.CreateMirror( VectorBundle<Real>{{1,0,0},{1,0,0}} );

	Transformation<Real> F{ Identity<Real> };

	Transformation<Real> R = M * F;
	Frame<Real> frame{ R };

	Frame<Real> frameSupposed{ {2,0,0}, {-1,0,0}, {0,1,0}, {0,0,1} };

	BOOST_CHECK( frame.Equals( frameSupposed, 0.0001f, 0.0001f ) );
}

BOOST_AUTO_TEST_CASE ( testMirrorTransformationFrame2 )
{
	Frame<Real> f{ Identity<Real> };
	f.RotateBin( pi/4 );
	f.TransportTan( 5 );

	Transformation<Real> M;
	M.CreateMirror( VectorBundle<Real>{f.P + 5 * f.T,f.T} );

	auto Pm = M * f.P;
	BOOST_CHECK( Pm.Equals( {15.f/std::sqrt(2.f),15.f/std::sqrt(2.f),0}, 0.0001f ) );
	M(3,3) = 0;
	auto Tm = M * f.T;
	BOOST_CHECK( Tm.Equals( {-1.f/std::sqrt(2.f),-1.f/std::sqrt(2.f),0}, 0.0001f ) );

	Transformation<Real> F{f};
	Transformation<Real> R = M * F;
	Frame<Real> frame{ R };

	Frame<Real> frameSupposed{ {15.f/std::sqrt(2.f),15.f/std::sqrt(2.f),0}, -f.T, f.N, f.B };

	BOOST_CHECK( frame.Equals( frameSupposed, 0.0001f, 0.0001f ) );
}

BOOST_AUTO_TEST_CASE ( testCopyMajor )
{
	Matrix<Real,5,6> matrix;
	for( unsigned short c = 0; c < 5; ++c )
		for( unsigned short r = 0; r < 6; ++r )
			matrix(c,r) = static_cast<Real>(c+r);

	Matrix<Real,5,6> original = matrix;
	BOOST_CHECK_EQUAL( matrix, original );

	Real array[5*6];
	copy_column_major( matrix, array );

	BOOST_CHECK_EQUAL( array[0], matrix(0,0) );
	BOOST_CHECK_EQUAL( array[1], matrix(0,1) );
	BOOST_CHECK_EQUAL( array[2], matrix(0,2) );
	BOOST_CHECK_EQUAL( array[3], matrix(0,3) );
	BOOST_CHECK_EQUAL( array[4], matrix(0,4) );
	BOOST_CHECK_EQUAL( array[5], matrix(0,5) );
	BOOST_CHECK_EQUAL( array[6], matrix(1,0) );

	copy_row_major( array, matrix );
	copy_row_major( matrix, array );

	BOOST_CHECK_EQUAL( array[0], matrix(0,0) );
	BOOST_CHECK_EQUAL( array[1], matrix(1,0) );
	BOOST_CHECK_EQUAL( array[2], matrix(2,0) );
	BOOST_CHECK_EQUAL( array[3], matrix(3,0) );
	BOOST_CHECK_EQUAL( array[4], matrix(4,0) );
	BOOST_CHECK_EQUAL( array[5], matrix(0,1) );

	copy_column_major( array, matrix );

	BOOST_CHECK_EQUAL( matrix, original );
}

BOOST_AUTO_TEST_CASE ( testMultiplicationOperator )
{
	Matrix<Real,4,6> matrix1;
	Matrix<Real,6,4> matrix2;
	Matrix<Real,4,4> matrix3;
	Matrix<Real,6,6> matrix4;

	matrix1.SetNull();
	matrix2.SetNull();

	matrix4 = matrix1 * matrix2;
	matrix3 = matrix2 * matrix1;

	SquareMatrix<Real,4> matrix5;
	SquareMatrix<Real,4> matrix6;
	SquareMatrix<Real,4> matrix7;

	matrix7 = matrix6 * matrix7;

	Transformation<Real> matrix8;
	Transformation<Real> matrix9;
	Transformation<Real> matrix10;

	matrix10 = matrix8 * matrix9;
	matrix10 = matrix8 * matrix5;

	Rotation<Real> matrix11;
	Rotation<Real> matrix12;
	Rotation<Real> matrix13;

	matrix13 = matrix11 * matrix12;

//	auto row = Row( matrix13, 2 );

	matrix13 = Rotation<Real>{ 0,0,0,
								0,0,0,
								 0,0,0 };

	matrix13 = { 1,1,1,
				 1,1,1,
				 1,1,1 };

	Matrix<Real,3,3> matrix14;
	matrix13 = matrix14;

}

BOOST_AUTO_TEST_SUITE_END() //Matrix_tests
BOOST_AUTO_TEST_SUITE_END() //Spatial_tests

#endif // WITH_BOOST_TESTS
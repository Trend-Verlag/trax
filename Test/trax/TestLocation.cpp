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
#include <boost/test/unit_test.hpp>

#include "../Test/dim/BoostTestDimensionedValuesHelpers.h"
#include "trax/support/Fixtures.h"
#include "trax/Location.h"
#include "trax/support/TraxSupportStream.h"

using namespace trax;
using namespace spat;
using namespace std;



BOOST_AUTO_TEST_SUITE(trax_tests)
BOOST_AUTO_TEST_SUITE(Location_tests)

BOOST_AUTO_TEST_CASE( assignment )
{
	TrackLocation tl1{ 10_m, false };
	TrackLocation tl2{ 20_m, true };
	tl2 = tl1;
	BOOST_CHECK_EQUAL( tl1, tl2 );

	TrackLocationRef tlr1{ 30_m, true, 1 };
	TrackLocationRef tlr2{ 40_m, false, 2 };
	tlr2 = tlr1;
	BOOST_CHECK_EQUAL( tlr1, tlr2 );
	tlr1.refid = 7;
	BOOST_CHECK_NE( tlr1, tlr2 );

	tlr1 = TrackLocationRef{ tl1 };
	BOOST_CHECK_EQUAL( tlr1.location, tl1 );

	tl2 = tlr2.location;
	BOOST_CHECK_EQUAL( tlr2.location, tl2 );

	TrackRange tr1{ 1, 10_m, 20_m };
	TrackRange tr2{ 2, 30_m, 40_m };
	tr2 = tr1;
	BOOST_CHECK_EQUAL( tr2, tr1 );
}

BOOST_AUTO_TEST_CASE( failes_to_putontrack )
{
	Location loc;
	BOOST_CHECK_THROW( loc.PutOn( nullptr, {0_m,Orientation::Value::para} ), std::invalid_argument );
	BOOST_CHECK_THROW( loc.Curvature(), std::logic_error );
	BOOST_CHECK_THROW( loc.Torsion(), std::logic_error );
	spat::Frame<Length,One> frame;
	BOOST_CHECK_THROW( loc.Transition( frame ), std::logic_error );
	BOOST_CHECK_THROW( loc.Move( 1_m ), std::logic_error );
	BOOST_CHECK_THROW( loc.MoveToEnd(), std::logic_error );
	BOOST_CHECK_THROW( loc.MoveToEnd( Orientation::Value::para ), std::logic_error );
	BOOST_CHECK_THROW( loc.MoveTransit(), std::logic_error );
	BOOST_CHECK_THROW( loc.MoveTransit( Orientation::Value::para ), std::logic_error );
	BOOST_CHECK_THROW( loc.Distance( loc, 1_m ), std::logic_error );
	BOOST_CHECK_THROW( loc.Reserve( { 0_m, 1_m }, 1 ), std::logic_error );
	BOOST_CHECK_THROW( loc.DeleteReservation( 1 ), std::logic_error );
	BOOST_CHECK_THROW( loc.Overlaps( 1 ), std::logic_error );
}

BOOST_AUTO_TEST_CASE( move_ontrack )
{
	std::shared_ptr<trax::TrackBuilder> pTrack = TrackBuilder::Make();

	shared_ptr<Cubic> pCubic = Cubic::Make();
	pTrack->Attach( pCubic, pCubic->Create( {{0_m,0_m,0_m}, {150_m, 0_m, 0_m}}, {{10_m, 10_m,0_m}, {0_m, 150_m, 0_m }} ) );

	trax::Location Loc{ pTrack, trax::TrackLocation( 0_m, true ) };

	VectorBundle<Length,One> b;
	Loc.Transition( b );

	BOOST_CHECK( b.P == Origin3D<Length> );
	BOOST_CHECK( b.T == Ex<One> );

	Loc.Move( pTrack->GetLength() );

	Loc.Transition( b );

	BOOST_CHECK( b.P.Equals( Position<Length>(10_m, 10_m,0_m), epsilon__length) );
	BOOST_CHECK( b.T.Equals( Vector<One>(0, 1,0), 0.01f) );
}

BOOST_FIXTURE_TEST_CASE( moveToEndThenMoveZeroMoveToStartThenMoveZero, TrackAndLocation )
	// Moving zero distance at the ends of a track
{
	BOOST_CHECK_EQUAL( m_LocationOnTrack.Move( m_pTrack->GetLength()/2 ).first, 0_m );
	BOOST_CHECK_EQUAL( m_LocationOnTrack.Move( 0_m ).first, 0_m );
	BOOST_CHECK_EQUAL( m_LocationOnTrack.Move( -m_pTrack->GetLength() ).first, 0_m );
	BOOST_CHECK_EQUAL( m_LocationOnTrack.Move( -0_m ).first, 0_m );
}

BOOST_FIXTURE_TEST_CASE( measureDistanceOverOpenTrackEnds, TrackAndLocation )
{
	Location otherLocation;

	BOOST_CHECK_THROW( m_LocationOnTrack.Distance( otherLocation, -m_pTrack->GetLength() ), std::logic_error );
	BOOST_CHECK_THROW( m_LocationOnTrack.Distance( otherLocation, m_pTrack->GetLength() ), std::logic_error );

	auto pOther = TrackBuilder::Make();
	shared_ptr<trax::ArcP> pArc1 = ArcP::Make();
	pArc1->Create( { Origin3D<Length>, { 1, 0, 0 }, { 0, 10, 0 } } );
	pOther->Attach( pArc1, {0_m,10_m*pi/2} );

	otherLocation.PutOn( pOther, {pOther->GetLength()/2,Orientation::Value::para} );

	BOOST_CHECK_THROW( m_LocationOnTrack.Distance( otherLocation, -m_pTrack->GetLength() ), std::out_of_range );
	BOOST_CHECK_THROW( m_LocationOnTrack.Distance( otherLocation, m_pTrack->GetLength() ), std::out_of_range );
	BOOST_CHECK_EQUAL( m_LocationOnTrack.Distance( otherLocation, -m_pTrack->GetLength()/2 ), -m_pTrack->GetLength()/2 );
	BOOST_CHECK_EQUAL( m_LocationOnTrack.Distance( otherLocation, m_pTrack->GetLength()/2 ), m_pTrack->GetLength()/2 );

	m_pTrack->Couple( std::make_pair(m_pTrack,Track::EndType::front), std::make_pair(pOther, Track::EndType::end) );
	BOOST_CHECK_EQUAL( m_LocationOnTrack.Distance( otherLocation, -m_pTrack->GetLength()/2 - pOther->GetLength()), -m_pTrack->GetLength()/2 - pOther->GetLength()/2 );
}

BOOST_FIXTURE_TEST_CASE( ParameterDistanceFrom3DDistance0, TrackAndLocation )
{
	m_LocationOnTrack.PutOn( m_pTrack, {0_m,Orientation::Value::para} );
	Length distanceIn3D = 0_m;
	Length parameter = ParameterDistanceFrom3DDistance( m_LocationOnTrack, distanceIn3D, 1_km );
	BOOST_CHECK_SMALL( distanceIn3D - parameter, epsilon__length );
}

BOOST_FIXTURE_TEST_CASE( ParameterDistanceFrom3DDistance1, TrackAndLocation )
{
	m_LocationOnTrack.PutOn( m_pTrack, {0_m,Orientation::Value::para} );
	Length distanceIn3D = 100_m * sqrt( 2.0f );
	Length parameter = ParameterDistanceFrom3DDistance( m_LocationOnTrack, distanceIn3D, 1_km );
	BOOST_CHECK_SMALL( 100_m*pi/2 - parameter, epsilon__length );
}

BOOST_FIXTURE_TEST_CASE( ParameterDistanceFrom3DDistance2, TrackCircle )
{
	m_Location.PutOn( m_pTrack1, {0_m,Orientation::Value::para} );
	Length distanceIn3D = 210_m; // can not be reached
	Length parameter = ParameterDistanceFrom3DDistance( m_Location, distanceIn3D, 1_km );
	BOOST_CHECK_EQUAL( parameter, 1_km );
}

BOOST_FIXTURE_TEST_CASE( ParameterDistanceFrom3DDistance3, TrackCircle )
{
	m_Location.PutOn( m_pTrack1, {100_m*pi/4,Orientation::Value::para} );
	Length distanceIn3D = -100_m * sqrt( 2.0f );
	Length parameter = ParameterDistanceFrom3DDistance( m_Location, distanceIn3D, 1_km );
	BOOST_CHECK_SMALL( -100_m*pi/2 - parameter, epsilon__length );
}

BOOST_FIXTURE_TEST_CASE( ParameterDistanceFrom3DDistance4, TrackAndLocation )
{
	auto pBuildTrack = TrackBuilder::Make();
	auto pLine = LineP::Make();
	pLine->Create( VectorBundle<Length,One>{ {10_m,10_m,10_m}, {1,0,0} } );
	pBuildTrack->Attach( std::move(pLine), {10_m,200_m} );

	m_LocationOnTrack.PutOn( pBuildTrack, {50_m,Orientation::Value::para} );

	Length parameter = ParameterDistanceFrom3DDistance( m_LocationOnTrack, 100_m, 1_km );
	BOOST_CHECK_EQUAL( parameter, 100_m );

	m_LocationOnTrack.PutOn( pBuildTrack, {150_m,Orientation::Value::anti} );
	parameter = ParameterDistanceFrom3DDistance( m_LocationOnTrack, 100_m, 1_km );
	BOOST_CHECK_EQUAL( parameter, 100_m );
}

BOOST_FIXTURE_TEST_CASE( MoveTransitPara, TrackCircle )
{
	m_Location.PutOn( m_pTrack1, {100_m*pi/4,Orientation::Value::para} );

	m_Location.MoveTransit();
	m_Location.MoveTransit();
	m_Location.MoveTransit();
	m_Location.MoveTransit();
	BOOST_CHECK( m_Location.GetTrack() == m_pTrack1 );

	m_Location.MoveTransit( Orientation::Value::anti );
	m_Location.MoveTransit( Orientation::Value::anti );
	m_Location.MoveTransit( Orientation::Value::anti );
	m_Location.MoveTransit( Orientation::Value::anti );
	BOOST_CHECK( m_Location.GetTrack() == m_pTrack1 );

	m_Location.MoveToEnd( Orientation::Value::anti );
	auto curve = m_pTrack1->GetCurve();
	curve.second.Far( 1_m );
	m_pTrack1->Attach( curve );

	curve = m_pTrack2->GetCurve();
	curve.second.Far( 1_m );
	m_pTrack2->Attach( curve );

	curve = m_pTrack3->GetCurve();
	curve.second.Far( 1_m );
	m_pTrack3->Attach( curve );

	curve = m_pTrack4->GetCurve();
	curve.second.Far( 1_m );
	m_pTrack4->Attach( curve );

	m_Location.MoveTransit();
	m_Location.MoveTransit();
	m_Location.MoveTransit();
	m_Location.MoveTransit();
	BOOST_CHECK( m_Location.GetTrack() == m_pTrack1 );

	m_Location.MoveTransit( Orientation::Value::anti );
	m_Location.MoveTransit( Orientation::Value::anti );
	m_Location.MoveTransit( Orientation::Value::anti );
	m_Location.MoveTransit( Orientation::Value::anti );
	BOOST_CHECK( m_Location.GetTrack() == m_pTrack1 );
}

BOOST_FIXTURE_TEST_CASE( MoveTransitAnti, TrackCircle )
{
	m_Location.PutOn( m_pTrack1, {100_m*pi/4,Orientation::Value::anti} );

	m_Location.MoveTransit();
	m_Location.MoveTransit();
	m_Location.MoveTransit();
	m_Location.MoveTransit();
	BOOST_CHECK( m_Location.GetTrack() == m_pTrack1 );

	m_Location.MoveTransit( Orientation::Value::anti );
	m_Location.MoveTransit( Orientation::Value::anti );
	m_Location.MoveTransit( Orientation::Value::anti );
	m_Location.MoveTransit( Orientation::Value::anti );
	BOOST_CHECK( m_Location.GetTrack() == m_pTrack1 );

	auto curve = m_pTrack1->GetCurve();
	curve.second.Near( 23.456_m );
	curve.second.Far( -12.000001_m );
	m_pTrack1->Attach( curve );

	curve = m_pTrack2->GetCurve();
	curve.second.Near( 0.000001_m );
	curve.second.Far( 17.999999_m );
	m_pTrack2->Attach( curve );

	curve = m_pTrack3->GetCurve();
	curve.second.Near( 9.9999999_m );
	curve.second.Far( 18.0000001_m );
	m_pTrack3->Attach( curve );

	curve = m_pTrack4->GetCurve();
	curve.second.Near( _m(epsilon) );
	curve.second.Far( 1_m + _m(epsilon) );
	m_pTrack4->Attach( curve );

	m_Location.MoveTransit();
	m_Location.MoveTransit();
	m_Location.MoveTransit();
	m_Location.MoveTransit();
	BOOST_CHECK( m_Location.GetTrack() == m_pTrack1 );

	m_Location.MoveTransit( Orientation::Value::anti );
	m_Location.MoveTransit( Orientation::Value::anti );
	m_Location.MoveTransit( Orientation::Value::anti );
	m_Location.MoveTransit( Orientation::Value::anti );
	BOOST_CHECK( m_Location.GetTrack() == m_pTrack1 );
}

BOOST_FIXTURE_TEST_CASE( MoveTransitExtremeCases, TrackCircle )
{
	m_Location.PutOn( m_pTrack1, {m_pTrack1->GetLength(),Orientation::Value::para} );
	BOOST_CHECK( m_Location.GetTrack() == m_pTrack1 );
	m_Location.MoveTransit();
	BOOST_CHECK( m_Location.GetTrack() == m_pTrack2 );

	m_Location.PutOn( m_pTrack1, {0_m,Orientation::Value::para} );
	m_Location.MoveTransit( Orientation::Value::anti );
	BOOST_CHECK( m_Location.GetTrack() == m_pTrack4 );

	m_Location.PutOn( m_pTrack1, {epsilon__length,Orientation::Value::para} );
	m_Location.MoveTransit( Orientation::Value::anti );
	BOOST_CHECK( m_Location.GetTrack() == m_pTrack4 );

	m_Location.PutOn( m_pTrack1, {Length{epsilon},Orientation::Value::para} );
	m_Location.MoveTransit( Orientation::Value::anti );
	BOOST_CHECK( m_Location.GetTrack() == m_pTrack4 );

	m_Location.PutOn( m_pTrack1, {m_pTrack1->GetLength()+epsilon__length,Orientation::Value::para} );
	m_Location.MoveTransit();
	BOOST_CHECK( m_Location.GetTrack() == m_pTrack3 );

	m_Location.PutOn( m_pTrack1, {-epsilon__length,Orientation::Value::para} );
	BOOST_CHECK( m_Location.GetTrack() == m_pTrack4 );
	m_Location.MoveTransit( Orientation::Value::anti );
	BOOST_CHECK( m_Location.GetTrack() == m_pTrack3 );

	m_Location.PutOn( m_pTrack1, {Length{-epsilon},Orientation::Value::para} );
	BOOST_CHECK( m_Location.GetTrack() == m_pTrack4 );
	m_Location.MoveTransit( Orientation::Value::anti );
	BOOST_CHECK( m_Location.GetTrack() == m_pTrack3 );
}

BOOST_FIXTURE_TEST_CASE( moveoverseveraltracks, ThreeTracksInALineFixture )
//Move a location over three tracks and beyond. Measure the distance returned.
{
	Location Loc( m_pTrack1, trax::TrackLocation( 0_m, true ) );
	auto rest = Loc.Move( m_pTrack1->GetLength() + m_pTrack2->GetLength() + m_pTrack3->GetLength() + m_pTrack3->GetLength() );

	BOOST_CHECK_EQUAL( rest.first, m_pTrack3->GetLength() );
}

BOOST_FIXTURE_TEST_CASE( movetransit, ThreeTracksInALineFixture )
//MoveTransit backward over track connection
{
	Location Loc( m_pTrack2, trax::TrackLocation( 0_m, true ) );
	const auto delta = Loc.MoveTransit( Orientation::Value::anti );
	BOOST_CHECK_EQUAL( Loc.GetTrack(), m_pTrack1 );
	BOOST_CHECK_EQUAL( delta.first, -0_m );
	BOOST_CHECK( delta.second );

	m_pTrack1->DeCouple();
}

BOOST_FIXTURE_TEST_CASE( distanceoverseveraltracks, ThreeTracksInALineFixture )
//Distance over coupled track ends.
{
	Location Loc1{ m_pTrack3, trax::TrackLocation{ 5_m, true } };
	Length dist = Loc1.Distance( Location( m_pTrack1, trax::TrackLocation{ m_pTrack1->Range().Center(), true } ), -1_km );
	BOOST_CHECK_CLOSE_DIMENSION( dist, -20_m, 0.01 );

	Location Loc2{ m_pTrack2, trax::TrackLocation{ 0_m, true } };
	dist = Loc2.Distance( Location( m_pTrack1, trax::TrackLocation{ m_pTrack1->GetLength(), true } ), -1_km );
	BOOST_CHECK_SMALL( dist, epsilon__length );

	Location Loc3{ m_pTrack3, trax::TrackLocation{ 0_m, true } };
	dist = Loc3.Distance( Location( m_pTrack1, trax::TrackLocation{ m_pTrack1->GetLength(), true } ), -1_km );
	BOOST_CHECK_CLOSE_DIMENSION( dist, -10_m, 0.01 );
}

BOOST_FIXTURE_TEST_CASE( resolveFromEnd, ThreeTracksInALineFixture )
// Don't resolve from track end.
{	
	Location Loc1{ m_pTrack2, trax::TrackLocation{ m_pTrack2->GetLength(), true } };
	BOOST_CHECK_EQUAL( Loc1.GetTrack(), m_pTrack2 );
	BOOST_CHECK_EQUAL( Loc1.Param(), m_pTrack2->GetLength() );

	Location Loc2{ m_pTrack2, trax::TrackLocation{ m_pTrack2->GetLength() + Length{ epsilon }, true } };
	BOOST_CHECK_EQUAL( Loc2.GetTrack(), m_pTrack2 );
	BOOST_CHECK_EQUAL( Loc2.Param(), m_pTrack2->GetLength() + Length{ epsilon } );

}

BOOST_AUTO_TEST_SUITE_END() //Location_tests
BOOST_AUTO_TEST_SUITE_END() //trax_tests
#endif
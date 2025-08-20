// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2019
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#if defined( WITH_BOOST_TESTS )
#include <boost/test/unit_test.hpp>

#include "trax/Fixtures.h"
#include "dim/tests/BoostTestDimensionedValuesHelpers.h"

namespace reservation_tests{

using namespace common;
using namespace trax;
using namespace spat;
using namespace std;


BOOST_AUTO_TEST_SUITE(Reservation_tests)

BOOST_FIXTURE_TEST_CASE( reserveTestDeleteTest, TrackFixture ){
	BOOST_CHECK( !m_pTrack->IsReserved( {0_m,m_pTrack->GetLength()}, 1 ) );
	m_pTrack->Reserve( {50_m,100_m}, 1 );
	BOOST_CHECK( m_pTrack->IsReserved( {0_m,m_pTrack->GetLength()}, 1 ) );
	m_pTrack->DeleteReservation( {0_m,m_pTrack->GetLength()}, 1 );
	BOOST_CHECK( !m_pTrack->IsReserved( {0_m,m_pTrack->GetLength()}, 1 ) );
}

BOOST_FIXTURE_TEST_CASE( reserveTwoForSameIDDeleteSeparately, TrackFixture ){
	m_pTrack->Reserve( {10_m,20_m}, 1 );
	BOOST_CHECK( m_pTrack->IsReserved( {15_m,15_m}, 1 ) );
	m_pTrack->Reserve( {80_m,90_m}, 1 );
	BOOST_CHECK( m_pTrack->IsReserved( {85_m,85_m}, 1 ) );
	BOOST_CHECK( !m_pTrack->IsReserved( {40_m,50_m}, 1 ) );
	m_pTrack->DeleteReservation( {85_m,85_m}, 1 );
	BOOST_CHECK( m_pTrack->IsReserved( {15_m,15_m}, 1 ) );
	BOOST_CHECK( !m_pTrack->IsReserved( {85_m,85_m}, 1 ) );
	m_pTrack->DeleteReservation( {15_m,15_m}, 1 );
	BOOST_CHECK( !m_pTrack->IsReserved( {0_m,m_pTrack->GetLength()}, 1 ) );
}

BOOST_FIXTURE_TEST_CASE( reserveOutsideTrackRangeUnconnected, TrackFixture ){
	BOOST_CHECK_THROW( m_pTrack->Reserve( {m_pTrack->GetLength() + 1_m,m_pTrack->GetLength()+20_m}, 1 ), std::range_error );
	BOOST_CHECK( !m_pTrack->IsReserved( {m_pTrack->GetLength() + 1_m,m_pTrack->GetLength()+20_m}, 1 ) );
	BOOST_CHECK_THROW( m_pTrack->Reserve( {-100_m,-50_m}, 1 ), std::range_error );
	BOOST_CHECK( !m_pTrack->IsReserved( {-100_m,-50_m}, 1 ) );
}

BOOST_FIXTURE_TEST_CASE( reserveOutsideTrackRangeConnected, TrackCircle ){
	BOOST_CHECK_THROW( m_pTrack1->Reserve( {m_pTrack1->GetLength() + 1_m,m_pTrack1->GetLength()+20_m}, 1 ), std::range_error );
	BOOST_CHECK( !m_pTrack1->IsReserved( {m_pTrack1->GetLength() + 1_m,m_pTrack1->GetLength()+20_m}, 1 ) );
	BOOST_CHECK_THROW( m_pTrack1->Reserve( {-100_m,-50_m}, 1 ), std::range_error );
	BOOST_CHECK( !m_pTrack1->IsReserved( {-100_m,-50_m}, 1 ) );
}
// TODO: make the following two tests work ...
/*
BOOST_FIXTURE_TEST_CASE( reservePartlyOutsideTrackReenterFromOtherEnd, TrackCircle ){
	m_pTrack1->Reserve( {m_pTrack1->GetLength() - 10_m,m_pTrack1->GetLength()+10_m}, 1 );
	BOOST_CHECK( m_pTrack2->IsReserved( {5_m,5_m}, 1 ) );
	m_pTrack2->DeleteReservation( {5_m,5_m}, 1 );
	BOOST_CHECK( !m_pTrack1->IsReserved( {m_pTrack1->GetLength() - 5_m,m_pTrack1->GetLength() - 5_m}, 1 ) );
	
	//Cyclic:
	BOOST_CHECK( !m_pTrack1->IsReserved( {5_m,5_m}, 1 ) );
	m_pTrack1->Reserve( { m_pTrack1->GetLength()/2,
		m_pTrack1->GetLength()/2 +
		m_pTrack1->GetLength()+
		m_pTrack2->GetLength()+
		m_pTrack3->GetLength()+
		m_pTrack4->GetLength() }, 1 );
	BOOST_CHECK( m_pTrack1->IsReserved( {5_m,5_m}, 1 ) );
	m_pTrack1->DeleteReservation( {5_m,5_m}, 1 );
	BOOST_CHECK( !m_pTrack1->IsReserved( {5_m,5_m}, 1 ) );
	BOOST_CHECK( !m_pTrack1->IsReserved( {m_pTrack1->GetLength()/2,m_pTrack1->GetLength()/2}, 1 ) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {m_pTrack2->GetLength()/2,m_pTrack2->GetLength()/2}, 1 ) );
	BOOST_CHECK( !m_pTrack3->IsReserved( {m_pTrack3->GetLength()/2,m_pTrack3->GetLength()/2}, 1 ) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {m_pTrack4->GetLength()/2,m_pTrack4->GetLength()/2}, 1 ) );
}

BOOST_FIXTURE_TEST_CASE( reservePartlyOutsideTrackReenterFromOtherEndWithGap, TrackCircle ){
	m_pTrack1->Reserve( {m_pTrack1->GetLength() - 10_m,m_pTrack1->GetLength()+10_m}, 1 );
	BOOST_CHECK( m_pTrack2->IsReserved( {5_m,5_m}, 1 ) );
	m_pTrack2->DeleteReservation( {5_m,5_m}, 1 );
	BOOST_CHECK( !m_pTrack1->IsReserved( {m_pTrack1->GetLength() - 5_m,m_pTrack1->GetLength() - 5_m}, 1 ) );
	
	//Cyclic:
	BOOST_CHECK( !m_pTrack1->IsReserved( {5_m,5_m}, 1 ) );
	m_pTrack1->Reserve( { m_pTrack1->GetLength()/2,
		m_pTrack1->GetLength()/2 - 1_m +
		m_pTrack1->GetLength()+
		m_pTrack2->GetLength()+
		m_pTrack3->GetLength()+
		m_pTrack4->GetLength() }, 1 );
	BOOST_CHECK( m_pTrack1->IsReserved( {5_m,5_m}, 1 ) );
	m_pTrack1->DeleteReservation( {5_m,5_m}, 1 );
	BOOST_CHECK( !m_pTrack1->IsReserved( {5_m,5_m}, 1 ) );
	BOOST_CHECK( !m_pTrack1->IsReserved( {m_pTrack1->GetLength()/2,m_pTrack1->GetLength()/2}, 1 ) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {m_pTrack2->GetLength()/2,m_pTrack2->GetLength()/2}, 1 ) );
	BOOST_CHECK( !m_pTrack3->IsReserved( {m_pTrack3->GetLength()/2,m_pTrack3->GetLength()/2}, 1 ) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {m_pTrack4->GetLength()/2,m_pTrack4->GetLength()/2}, 1 ) );
}
*/
BOOST_FIXTURE_TEST_CASE( reserveMarginal, TrackFixture ){
	m_pTrack->Reserve( {40_m,50_m}, 1 );
	m_pTrack->DeleteReservation( Interval<Length>{ 40_m }, 1 );
	BOOST_CHECK( !m_pTrack->IsReserved( {0_m,m_pTrack->GetLength()}, 1) );

	m_pTrack->Reserve( {40_m,50_m}, 1 );
	m_pTrack->DeleteReservation( Interval<Length>{ 50_m }, 1 );
	BOOST_CHECK( m_pTrack->IsReserved( {0_m,m_pTrack->GetLength()}, 1) );
}

BOOST_FIXTURE_TEST_CASE( reserveMarginalOutsideTrackRangeConnected, TrackCircle ){
	m_pTrack1->Reserve( {0_m,m_pTrack1->GetLength()}, 1 );
	BOOST_CHECK( m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	m_pTrack1->DeleteReservation( Interval<Length>{ 0_m }, 1 );
	BOOST_CHECK( !m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	m_pTrack1->Reserve( {-epsilon__length,m_pTrack1->GetLength()+epsilon__length}, 1 );
	BOOST_CHECK( m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	m_pTrack1->DeleteReservation( Interval<Length>{ 0_m }, 1 );
	BOOST_CHECK( !m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	m_pTrack1->Reserve( {-epsilon__length,m_pTrack1->GetLength()}, 1 );
	BOOST_CHECK( m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	m_pTrack4->DeleteReservation( Interval<Length>{ m_pTrack4->GetLength() }, 1 );
	BOOST_CHECK( !m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	m_pTrack1->Reserve( {0_m,m_pTrack1->GetLength()+epsilon__length}, 1 );
	BOOST_CHECK( m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	m_pTrack2->DeleteReservation( Interval<Length>{ 0_m }, 1 );
	BOOST_CHECK( !m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );
}

BOOST_FIXTURE_TEST_CASE( reserveInfinitesimalOutsideTrackRangeConnected, TrackCircle ){
	m_pTrack1->Reserve( {Length{-epsilon},m_pTrack1->GetLength()+Length{epsilon}}, 1 );
	BOOST_CHECK( m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	m_pTrack1->DeleteReservation( Interval<Length>{ 0_m }, 1 );
	BOOST_CHECK( !m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	m_pTrack1->Reserve( {-epsilon__length/2,m_pTrack1->GetLength()+epsilon__length/2}, 1 );
	BOOST_CHECK( m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	m_pTrack1->DeleteReservation( Interval<Length>{ 0_m }, 1 );
	BOOST_CHECK( !m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );
}

BOOST_FIXTURE_TEST_CASE( reserveInfiniteRange, TrackCircle ){
	m_pTrack1->Reserve( {m_pTrack1->GetLength()/2,+infinite__length}, 1 );
	BOOST_CHECK( m_pTrack1->IsReserved( {m_pTrack1->GetLength()/4,m_pTrack1->GetLength()/4}, 1 ) );
}

BOOST_FIXTURE_TEST_CASE( reserveOverlapping, TrackFixture ){
	m_pTrack->Reserve( {10_m,50_m}, 1 );
	m_pTrack->Reserve( {40_m,80_m}, 2 );

	auto overlaps = m_pTrack->Overlaps( 1 );
	BOOST_REQUIRE_EQUAL( overlaps.size(), 1u );
	BOOST_CHECK_EQUAL( overlaps.front().withID, 2 );
	BOOST_CHECK( !overlaps.front().forNear );
	BOOST_CHECK( overlaps.front().forFar );
	BOOST_CHECK( overlaps.front().withNear );
	BOOST_CHECK( !overlaps.front().withFar );

	m_pTrack->Reserve( {80_m,40_m}, 3 );
	overlaps = m_pTrack->Overlaps( 1 );
	BOOST_REQUIRE_EQUAL( overlaps.size(), 2u );
	BOOST_CHECK_EQUAL( overlaps[0].withID, 2 );
	BOOST_CHECK( !overlaps[0].forNear );
	BOOST_CHECK( overlaps[0].forFar );
	BOOST_CHECK( overlaps[0].withNear );
	BOOST_CHECK( !overlaps[0].withFar );

	BOOST_CHECK_EQUAL( overlaps[1].withID, 3 );
	BOOST_CHECK( !overlaps[1].forNear );
	BOOST_CHECK( overlaps[1].forFar );
	BOOST_CHECK( !overlaps[1].withNear );
	BOOST_CHECK( overlaps[1].withFar );
}

BOOST_FIXTURE_TEST_CASE( reserveOverlapOnConnectedTrack, TrackCircle )
// reservations overlapping on track 1, but we ask at track 2 for the overlapping.
{
	m_pTrack1->Reserve( {m_pTrack1->GetLength()/4,3*m_pTrack1->GetLength()/4}, 1 );
	m_pTrack2->Reserve( {-m_pTrack1->GetLength()/2,m_pTrack2->GetLength()/2}, 2 );

	auto overlaps = m_pTrack2->Overlaps( 2 );
	BOOST_CHECK_EQUAL( overlaps.size(), 1u );
	BOOST_CHECK_EQUAL( overlaps[0].forID, 2 );
	BOOST_CHECK_EQUAL( overlaps[0].withID, 1 );

	BOOST_CHECK( overlaps[0].forNear );
	BOOST_CHECK( !overlaps[0].forFar );
	BOOST_CHECK( !overlaps[0].withNear );
	BOOST_CHECK( overlaps[0].withFar );
}

BOOST_FIXTURE_TEST_CASE( reserveOverlapWithSeveralReservationsOnDifferentTrack, TrackCircle )
{
	m_pTrack1->Reserve( {m_pTrack1->GetLength()/2,m_pTrack1->GetLength() + m_pTrack2->GetLength() + m_pTrack3->GetLength() + m_pTrack4->GetLength()/2 }, 1 );
	m_pTrack2->Reserve( {m_pTrack2->GetLength()/4,3*m_pTrack2->GetLength()/4}, 2 );
	m_pTrack3->Reserve( {m_pTrack3->GetLength()/4,3*m_pTrack3->GetLength()/4}, 3 );
	m_pTrack4->Reserve( {m_pTrack4->GetLength()/4,3*m_pTrack4->GetLength()/4}, 4 );

	auto overlaps = m_pTrack1->Overlaps( 1 );
	BOOST_CHECK_EQUAL( overlaps.size(), 3u );
}

BOOST_FIXTURE_TEST_CASE( reserveLocation1, TrackAndLocation ){
	m_LocationOnTrack.Reserve( {-10_m,10_m}, 1 );
	m_LocationOnTrack.Flip();
	m_LocationOnTrack.Reserve( {-20_m,-9_m}, 2 );
	m_LocationOnTrack.Flip();
	auto overlaps = m_LocationOnTrack.Overlaps(1);
	BOOST_REQUIRE_EQUAL( overlaps.size(), 1u );
	BOOST_CHECK_EQUAL( overlaps[0].withID, 2 );
	BOOST_CHECK( !overlaps[0].forNear );
	BOOST_CHECK( overlaps[0].forFar );
	BOOST_CHECK( !overlaps[0].withNear );
	BOOST_CHECK( overlaps[0].withFar );

	overlaps = m_LocationOnTrack.Overlaps(2);
	BOOST_REQUIRE_EQUAL( overlaps.size(), 1u );
	BOOST_CHECK_EQUAL( overlaps[0].withID, 1 );
	BOOST_CHECK( !overlaps[0].forNear );
	BOOST_CHECK( overlaps[0].forFar );
	BOOST_CHECK( !overlaps[0].withNear );
	BOOST_CHECK( overlaps[0].withFar );
}

BOOST_FIXTURE_TEST_CASE( reserveLocation2, TrackAndLocation ){
	m_LocationOnTrack.Reserve( {-10_m,10_m}, 1 );
	m_LocationOnTrack.Move( 19_m );
	m_LocationOnTrack.Flip();
	m_LocationOnTrack.Reserve( {-10_m,10_m}, 2 );
	auto overlaps = m_LocationOnTrack.Overlaps(1);
	BOOST_REQUIRE_EQUAL( overlaps.size(), 1u );
	BOOST_CHECK_EQUAL( overlaps[0].withID, 2 );
	BOOST_CHECK( !overlaps[0].forNear );
	BOOST_CHECK( overlaps[0].forFar );
	BOOST_CHECK( !overlaps[0].withNear );
	BOOST_CHECK( overlaps[0].withFar );

	overlaps = m_LocationOnTrack.Overlaps(2);
	BOOST_REQUIRE_EQUAL( overlaps.size(), 1u );
	BOOST_CHECK_EQUAL( overlaps[0].withID, 1 );
	BOOST_CHECK( !overlaps[0].forNear );
	BOOST_CHECK( overlaps[0].forFar );
	BOOST_CHECK( !overlaps[0].withNear );
	BOOST_CHECK( overlaps[0].withFar );
}

BOOST_FIXTURE_TEST_CASE( deleteReservationViaLocation, TrackCircle ){
	Location locationOnTrack{ m_pTrack1, { m_pTrack1->GetLength()/2, Orientation::Value::para } };

	locationOnTrack.Reserve( {-m_pTrack1->GetLength()/2,m_pTrack1->GetLength()/2}, 1 );
	BOOST_CHECK( m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	locationOnTrack.DeleteReservation( 1 );
	BOOST_CHECK( !m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	locationOnTrack.Reserve( {-m_pTrack1->GetLength(),m_pTrack1->GetLength()}, 1 );
	BOOST_CHECK( m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	locationOnTrack.DeleteReservation( 1 );
	BOOST_CHECK( !m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	locationOnTrack.Reserve( {-m_pTrack1->GetLength(),2*m_pTrack1->GetLength()}, 1 );
	BOOST_CHECK( m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack3->IsReserved( {0_m,m_pTrack3->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	locationOnTrack.DeleteReservation( 1 );
	BOOST_CHECK( !m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack3->IsReserved( {0_m,m_pTrack3->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	locationOnTrack.Reserve( {-m_pTrack1->GetLength(),2*m_pTrack1->GetLength()}, 1 );
	BOOST_CHECK( m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack3->IsReserved( {0_m,m_pTrack3->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	m_pTrack1->DeleteReservation( {0_m,m_pTrack1->GetLength()}, 1 );
	BOOST_CHECK( !m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack3->IsReserved( {0_m,m_pTrack3->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	locationOnTrack.Reserve( {-1.5f*m_pTrack1->GetLength() - epsilon__length,1.5f*m_pTrack1->GetLength()}, 1 );
	BOOST_CHECK( m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack3->IsReserved( {0_m,m_pTrack3->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	locationOnTrack.DeleteReservation( 1 );
	BOOST_CHECK( !m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack3->IsReserved( {0_m,m_pTrack3->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	locationOnTrack.Reserve( {-2*m_pTrack1->GetLength(),2*m_pTrack1->GetLength()}, 1 );
	BOOST_CHECK( m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack3->IsReserved( {0_m,m_pTrack3->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	locationOnTrack.DeleteReservation( 1 );
	BOOST_CHECK( !m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack3->IsReserved( {0_m,m_pTrack3->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	locationOnTrack.Reserve( {-3*m_pTrack1->GetLength(),3*m_pTrack1->GetLength()}, 1 );
	BOOST_CHECK( m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack3->IsReserved( {0_m,m_pTrack3->GetLength()}, 1) );
	BOOST_CHECK( m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );

	locationOnTrack.DeleteReservation( 1 );
	BOOST_CHECK( !m_pTrack1->IsReserved( {0_m,m_pTrack1->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack2->IsReserved( {0_m,m_pTrack2->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack3->IsReserved( {0_m,m_pTrack3->GetLength()}, 1) );
	BOOST_CHECK( !m_pTrack4->IsReserved( {0_m,m_pTrack4->GetLength()}, 1) );
}

BOOST_AUTO_TEST_SUITE_END() //Reservation_tests

}

#endif // WITH_BOOST_TESTS
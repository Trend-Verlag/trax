// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2020 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#if defined( WITH_BOOST_TESTS )
#include <boost/test/unit_test.hpp> // NOLINT 

#include "trax/collections/support/FixturesCollections.h"
#include "trax/Track.h"
#include "trax/Section.h"
//#include "trax/StaticTrack.h"
#include "trax/collections/TrackSystem.h"
#include "trax/collections/TrackCollection.h"
#include "trax/collections/TrackCollectionContainer.h"
#include "trax/support/TraxSupportStream.h"

#include "../Test/dim/BoostTestDimensionedValuesHelpers.h"
#include "../Test/spat/BoostTestSpatialHelpers.h"

using namespace trax;
using namespace spat;
using namespace std;


BOOST_AUTO_TEST_SUITE(trax_tests)
BOOST_AUTO_TEST_SUITE(Collection_Tests)

BOOST_FIXTURE_TEST_CASE( Collection_Correctness, TrackSystemCircleFixture )
{	
	auto pCollectionA = m_pTrackSystem->GetCollectionContainer()->GetFirst();

	BOOST_CHECK_EQUAL( m_pTrack1->ID(), 1 );
	BOOST_CHECK_EQUAL( m_pTrack2->ID(), 2 );
	BOOST_CHECK_EQUAL( m_pTrack3->ID(), 3 );
	BOOST_CHECK_EQUAL( m_pTrack4->ID(), 4 );

	auto pCollectionB = TrackCollection::Make();
	pCollectionB->Take( *pCollectionA );
	BOOST_CHECK_EQUAL( pCollectionA->Count(), 0 );
	BOOST_CHECK_EQUAL( pCollectionB->Count(), 4 );
}

BOOST_FIXTURE_TEST_CASE( CollectionFrameShift_Test, TrackSystemFixture )
{
	std::shared_ptr<trax::TrackBuilder> pTrack1 = TrackBuilder::Make();
	std::shared_ptr<trax::TrackBuilder> pTrack2 = TrackBuilder::Make();
	std::shared_ptr<trax::ArcP> pArc = ArcP::Make();
	Length R = 100_m;
	pArc->Create( { Origin3D<Length>, {1,0,0}, {0,_m(R),0} } );
	pTrack1->Attach( pArc, {0_m,R*pi/2} );
	pTrack2->Attach( pArc, {0_m,R*pi/2} );
	pTrack1->Attach( LinearTwist::Make() );
	pTrack2->Attach( LinearTwist::Make() );

	m_pTrackSystem->Add( pTrack1 );
	IDType id = m_pTrackSystem->CreateCollection();
	Frame<Length,One> frame, frame1, frame2;
	frame.Init();
	frame.TransportTan( 100_m );
	m_pTrackSystem->SetCollectionFrame( id, frame );
	m_pTrackSystem->Add( pTrack2 );

	pTrack1->Transition( 10_m, frame1 );
	pTrack2->Transition( 10_m, frame2 );
	m_pTrackSystem->GetCollectionFrame( id ).FromParent( frame2 );

	BOOST_CHECK_CLOSE_SPATIAL( frame1.P, frame2.P, epsilon__length );
}

BOOST_FIXTURE_TEST_CASE( CollectionFrameShift_Test2, TrackSystemFixture )
{
	std::shared_ptr<trax::TrackBuilder> pTrack1 = TrackBuilder::Make();
	std::shared_ptr<trax::TrackBuilder> pTrack2 = TrackBuilder::Make();
	std::shared_ptr<trax::ArcP> pArc = ArcP::Make();
	Length R = 100_m;
	pArc->Create( { Origin3D<Length>, {1,0,0}, {0,_m(R),0} } );
	pTrack1->Attach( pArc, {0_m,R*pi/2} );
	pTrack2->Attach( pArc, {0_m,R*pi/2} );
	pTrack1->Attach( LinearTwist::Make() );
	pTrack2->Attach( LinearTwist::Make() );

	m_pTrackSystem->Add( pTrack1 );
	IDType id = m_pTrackSystem->CreateCollection();
	Frame<Length,One> frame, frame1, frame2;
	frame.Init();
	frame.TransportTan( 100_m );
	m_pTrackSystem->Add( pTrack2 );
	m_pTrackSystem->SetCollectionFrame( id, frame );

	pTrack1->Transition( 10_m, frame1 );
	pTrack2->Transition( 10_m, frame2 );
	m_pTrackSystem->GetCollectionFrame( id ).FromParent( frame2 );

	BOOST_CHECK_CLOSE_SPATIAL( frame1.P, frame2.P, epsilon__length );
}

BOOST_AUTO_TEST_SUITE_END() //Collection_Tests
BOOST_AUTO_TEST_SUITE(TrackSystem_Tests)

BOOST_FIXTURE_TEST_CASE( TrackSystem_Correctness, TrackSystemCircleFixture )
{	
	std::shared_ptr<TrackSystem> pTrackSystemB{ TrackSystem::Make() };
	pTrackSystemB->CreateCollection();

	pTrackSystemB->Take( *m_pTrackSystem );
	BOOST_CHECK_EQUAL( pTrackSystemB->Count(), 4 );
}

BOOST_FIXTURE_TEST_CASE( TrackSystem_CleanAddRemoveCycle, TrackSystemFixture )
{	
	BOOST_CHECK_EQUAL( m_pTrackSystem->Count(), 0 );

	auto pTrack = TrackBuilder::Make();
	pTrack->Attach( Line::Make(), { 0_m, 100_m } );

	IDType id = m_pTrackSystem->Add( pTrack );
	BOOST_CHECK_EQUAL( id, 1 );
	BOOST_CHECK_EQUAL( pTrack->ID(), 1 );

	BOOST_CHECK( m_pTrackSystem->Remove( pTrack.get(), true ) );
	BOOST_CHECK_EQUAL( pTrack->ID(), 0 );
	BOOST_CHECK_EQUAL( m_pTrackSystem->Count(), 0 );

	id = m_pTrackSystem->Add( pTrack );
	BOOST_CHECK_EQUAL( id, 1 );
	BOOST_CHECK_EQUAL( pTrack->ID(), 1 );
	BOOST_CHECK_EQUAL( m_pTrackSystem->Count(), 1 );
}

BOOST_FIXTURE_TEST_CASE( FoR_TrackTrackCollection1_Test, TrackAndTrackSystem )
	// Frame Of Reference consistency test with different
	// orders of attachment and setting of the frames.
{
	Frame<Length,One> LocBefore;
	m_LocationOnTrack.Transition( LocBefore );

	m_pTrack->SetFrame( m_FrameA );
	m_pTrackSystem->SetCollectionFrame( 1, m_FrameB );
	m_pTrackSystem->Add( m_pTrack );

	Frame<Length,One> LocAfter;
	m_LocationOnTrack.Transition( LocAfter );

	BOOST_CHECK( LocBefore.Equals( LocAfter, epsilon__length, 0.001f) );
}

BOOST_FIXTURE_TEST_CASE( FoR_TrackTrackCollection2_Test, TrackAndTrackSystem )
	// Frame Of Reference consistency test with different
	// orders of attachment and setting of the frames.
{
	Frame<Length,One> LocBefore;
	m_LocationOnTrack.Transition( LocBefore );

	m_pTrackSystem->SetCollectionFrame( 1, m_FrameB );
	m_pTrackSystem->Add( m_pTrack );
	m_pTrack->SetFrame( m_FrameA );

	Frame<Length,One> LocAfter;
	m_LocationOnTrack.Transition( LocAfter );

	BOOST_CHECK( LocBefore.Equals( LocAfter, epsilon__length, 0.001f) );
}

BOOST_FIXTURE_TEST_CASE( FoR_TrackTrackCollection3_Test, TrackAndTrackSystem )
	// Frame Of Reference consistency test with different
	// orders of attachment and setting of the frames.
{
	Frame<Length,One> LocBefore;
	m_LocationOnTrack.Transition( LocBefore );

	m_pTrackSystem->Add( m_pTrack );
	m_pTrack->SetFrame( m_FrameA );
	m_pTrackSystem->SetCollectionFrame( 1, m_FrameB );

	Frame<Length,One> LocAfter;
	m_LocationOnTrack.Transition( LocAfter );

	BOOST_CHECK( LocBefore.Equals( LocAfter, epsilon__length, 0.001f) );
}

BOOST_FIXTURE_TEST_CASE( FoR_TrackTrackCollection4_Test, TrackAndTrackSystem )
	// Frame Of Reference consistency test with different
	// orders of attachment and setting of the frames.
{
	Frame<Length,One> LocBefore;
	m_LocationOnTrack.Transition( LocBefore );

	m_pTrack->SetFrame( m_FrameA );
	m_pTrackSystem->Add( m_pTrack );
	m_pTrackSystem->SetCollectionFrame( 1, m_FrameB );

	Frame<Length,One> LocAfter;
	m_LocationOnTrack.Transition( LocAfter );

	BOOST_CHECK( LocBefore.Equals( LocAfter, epsilon__length, 0.001f) );
}

BOOST_FIXTURE_TEST_CASE( FoR_TrackTrackCollection5_Test, TrackAndTrackSystem )
	// Frame Of Reference consistency test with different
	// orders of attachment and setting of the frames.
{
	Frame<Length,One> LocBefore;
	m_LocationOnTrack.Transition( LocBefore );

	m_pTrackSystem->Add( m_pTrack );
	m_pTrackSystem->SetCollectionFrame( 1, m_FrameB );
	m_pTrack->SetFrame( m_FrameA );

	Frame<Length,One> LocAfter;
	m_LocationOnTrack.Transition( LocAfter );

	BOOST_CHECK( LocBefore.Equals( LocAfter, epsilon__length, 0.001f) );
}

BOOST_FIXTURE_TEST_CASE( FoR_TrackTrackCollection6_Test, TrackAndTrackSystem )
	// Frame Of Reference consistency test with different
	// orders of attachment and setting of the frames.
{
	Frame<Length,One> LocBefore;
	m_LocationOnTrack.Transition( LocBefore );

	m_pTrackSystem->SetCollectionFrame( 1, m_FrameB );
	m_pTrack->SetFrame( m_FrameA );
	m_pTrackSystem->Add( m_pTrack );

	Frame<Length,One> LocAfter;
	m_LocationOnTrack.Transition( LocAfter );

	BOOST_CHECK( LocBefore.Equals( LocAfter, epsilon__length, 0.001f) );
}

BOOST_AUTO_TEST_SUITE_END() //TrackSystem_Tests
BOOST_AUTO_TEST_SUITE_END() //trax_tests
#endif // WITH_BOOST_TESTS
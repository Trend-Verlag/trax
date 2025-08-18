// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2023 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#include <boost/test/unit_test.hpp>

#include "trax/collections/tests/FixturesCollections.h"
#include "trax/engines/PhysX/PhysX_Scene.h"
#include "trax/engines/PhysX/PhysX_Simulator.h"
#include "trax/StaticTrack.h"
#include "trax/Curve.h"
#include "trax/Section.h"

#include "dim/support/DimSupportStream.h"
#include "dim/tests/BoostTestDimensionedValuesHelpers.h"
#include "spat/tests/BoostTestSpatialHelpers.h"

using namespace dim;
using namespace trax;

BOOST_AUTO_TEST_SUITE(TestStaticTrack)

BOOST_AUTO_TEST_CASE( testInfinity )
{
	PhysX_Simulator simulator{};
	PhysX_Scene scene{ simulator };

	std::shared_ptr<StaticTrack> pStaticTrack = StaticTrack::Make( scene );
	BOOST_CHECK( pStaticTrack );

	std::shared_ptr<Arc> pArc = Arc::Make();
	BOOST_CHECK( pArc );
	pArc->Create( { 1_1/100_m } );
	pStaticTrack->Attach( pArc );
	BOOST_CHECK_EQUAL( pStaticTrack->GetLength(), +infinite__length );

	std::shared_ptr<Section> pSection = Section::Make( Section::SpecialSections::vignol_UIC60 );
	BOOST_CHECK( pSection );
	pStaticTrack->Attach( pSection );

	BOOST_CHECK( !pStaticTrack->IsValid() );
}

BOOST_FIXTURE_TEST_CASE( CollectionFrameShift_Test3, TrackSystemFixture )
{
	PhysX_Simulator simulator{};
	PhysX_Scene scene{ simulator };

	std::shared_ptr<StaticTrack> pTrack1 = StaticTrack::Make(scene);
	std::shared_ptr<StaticTrack> pTrack2 = StaticTrack::Make(scene);

	std::shared_ptr<Section> pSection = Section::Make(Section::SpecialSections::vignol_UIC60);
	pTrack1->Attach(pSection);
	pTrack2->Attach(pSection);

	std::shared_ptr<ArcP> pArc = ArcP::Make();
	Length R = 100_m;
	pArc->Create({ Origin3D<Length>, {1,0,0}, {0,_m(R),0} });
	pTrack1->Attach(pArc, { 0_m,R * pi / 2 });
	pTrack2->Attach(pArc, { 0_m,R * pi / 2 });
	pTrack1->Attach(LinearTwist::Make());
	pTrack2->Attach(LinearTwist::Make());

	m_pTrackSystem->Add(pTrack1);
	IDType id = m_pTrackSystem->CreateCollection();
	Frame<Length, One> frame, frame1, frame2;
	frame.Init();
	frame.TransportTan(100_m);
	m_pTrackSystem->SetCollectionFrame(id, frame);
	m_pTrackSystem->Add(pTrack2);

	pTrack1->Transition(10_m, frame1);
	pTrack2->Transition(10_m, frame2);
	m_pTrackSystem->GetCollectionFrame(id).FromParent(frame2);

	BOOST_CHECK_CLOSE_SPATIAL(frame1.P, frame2.P, epsilon__length);

	m_pTrackSystem.reset();
}

BOOST_AUTO_TEST_SUITE_END() //TestStaticTrack

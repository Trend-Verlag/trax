//	trax track library
//	AD 2023 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//
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

#include <boost/test/unit_test.hpp>

#include "trax/collections/FixturesCollections.h"
#include "trax/rigid/Scene.h"
#include "trax/rigid/Simulator.h"
#include "trax/rigid/StaticTrack.h"
#include "trax/Curve.h"
#include "trax/Section.h"

#include "dim/support/DimSupportStream.h"
#include "spat/Frame.h"
#include "spat/BoostTestSpatialHelpers.h"

using namespace dim;
using namespace spat;
using namespace trax;

BOOST_AUTO_TEST_SUITE(TestStaticTrack)

BOOST_AUTO_TEST_CASE( testInfinity )
{
	std::unique_ptr<Simulator> pSimulator = Simulator::Make( Simulator::Type::PhysX );
	BOOST_REQUIRE( pSimulator );
	std::unique_ptr<Scene> pScene = Scene::Make( *pSimulator );
	BOOST_REQUIRE( pScene );

	std::shared_ptr<StaticTrack> pStaticTrack = StaticTrack::Make( *pScene );
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
	std::unique_ptr<Simulator> pSimulator = Simulator::Make( Simulator::Type::PhysX );
	BOOST_REQUIRE( pSimulator );
	std::unique_ptr<Scene> pScene = Scene::Make( *pSimulator );
	BOOST_REQUIRE( pScene );

	std::shared_ptr<StaticTrack> pTrack1 = StaticTrack::Make(*pScene);
	std::shared_ptr<StaticTrack> pTrack2 = StaticTrack::Make(*pScene);

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

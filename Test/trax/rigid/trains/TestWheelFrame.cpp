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

#include "trax/support/Fixtures.h"

#include "trax/rigid/Simulator.h"
#include "trax/rigid/Scene.h"
#include "trax/rigid/trains/WheelFrame.h"
#include "trax/rigid/Body.h"
#include "trax/support/TraxSupportStream.h"

#include "dim/support/DimSupportStream.h"
#include "dim/BoostTestDimensionedValuesHelpers.h"

using namespace spat;
using namespace trax;

BOOST_AUTO_TEST_SUITE(TestWheelFrame)
BOOST_AUTO_TEST_CASE( testSimulator )
{
	std::unique_ptr<trax::Simulator> pSimulator = Simulator::Make( Simulator::Type::PhysX );
	BOOST_REQUIRE( pSimulator );
	std::unique_ptr<trax::Scene> pScene = Scene::Make( *pSimulator );
	BOOST_REQUIRE( pScene );
}

BOOST_FIXTURE_TEST_CASE( testTrackJoint, TrackFixture )
{
	std::unique_ptr<trax::Simulator> pSimulator = Simulator::Make( Simulator::Type::PhysX );
	BOOST_REQUIRE( pSimulator );
	std::unique_ptr<trax::Scene> pScene = Scene::Make( *pSimulator );
	BOOST_REQUIRE( pScene );

	auto pBody = pScene->CreateBody();

	std::shared_ptr<TrackJointFeeder> pFeeder = pScene->CreateTrackJointFeeder( *pBody, spat::Identity<Length,One> );

	pScene->Release( *pFeeder );
}

BOOST_FIXTURE_TEST_CASE( testWheelFrame, TrackFixture )
{
	std::unique_ptr<trax::Simulator> pSimulator = Simulator::Make( Simulator::Type::PhysX );
	BOOST_REQUIRE( pSimulator );
	std::unique_ptr<trax::Scene> pScene = Scene::Make( *pSimulator );
	BOOST_REQUIRE( pScene );

	{
		Location location{ m_pTrack, TrackLocation{ m_pTrack->GetLength()/2 } };
		std::shared_ptr<WheelFrame> pWheelFrame = WheelFrame::Make( *pScene );
		pWheelFrame->Rail( location );
		Velocity velocity = 1_mIs;
		pWheelFrame->SetVelocity( velocity );

		BOOST_CHECK( pWheelFrame->IsRailed() );
		BOOST_CHECK_CLOSE_DIMENSION( pWheelFrame->GetVelocity(), velocity, 1 );
		BOOST_CHECK_EQUAL( pWheelFrame->GetLocation(), location );

		std::unique_ptr<PulseCounter> pCounter = PulseCounter::Make();
		pScene->JackOnSimulationStep().Insert( &pCounter->PlugToCountUp() );
		pWheelFrame->JackOnDerail().Insert( &pScene->PlugToStop() );

		pScene->Simulate();

		BOOST_CHECK_CLOSE_DIMENSION( fixed_timestep * pCounter->Counter(), m_pTrack->GetLength()/2 / velocity, 0.05f );

		BOOST_CHECK( !pWheelFrame->IsRailed() );
	}
}

BOOST_FIXTURE_TEST_CASE( testWheelFrameResetAnchor, TrackFixture )
{
	std::unique_ptr<trax::Simulator> pSimulator = Simulator::Make( Simulator::Type::PhysX );
	BOOST_REQUIRE( pSimulator );
	std::unique_ptr<trax::Scene> pScene = Scene::Make( *pSimulator );
	BOOST_REQUIRE( pScene );

	{
		Location location{ m_pTrack, TrackLocation{ m_pTrack->GetLength()/2 } };
		std::shared_ptr<WheelFrame> pWheelFrame = WheelFrame::Make( *pScene );
		Frame<Length,One> anchor;
		anchor.Init();
		anchor.TransportBin( -1.0_m );
		pWheelFrame->Anchor( anchor );
		pWheelFrame->Rail( location );
		Velocity velocity = 1_mIs;
		pWheelFrame->SetVelocity( velocity );

		BOOST_CHECK( pWheelFrame->IsRailed() );
		BOOST_CHECK_EQUAL( pWheelFrame->GetLocation(), location );

		std::unique_ptr<PulseCounter> pCounter = PulseCounter::Make();
		pScene->JackOnSimulationStep().Insert( &pCounter->PlugToCountUp() );
		pWheelFrame->JackOnDerail().Insert( &pScene->PlugToStop() );

		pScene->Simulate();

		BOOST_CHECK( !pWheelFrame->IsRailed() );
	}
}

BOOST_AUTO_TEST_SUITE_END()

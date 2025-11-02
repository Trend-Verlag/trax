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

#include "trax/rigid/trains/support/FixturesTrain.h"

#include "trax/rigid/trains/RollingStock.h"
#include "trax/rigid/trains/WheelFrame.h"
#include "trax/rigid/trains/support/RollingStockFileReader.h"
#include "trax/rigid/trains/support/RollingStockCreator.h"

#include "trax/rigid/Gestalt.h"

#include "dim/support/DimSupportStream.h"
#include "dim/BoostTestDimensionedValuesHelpers.h"
#include "spat/BoostTestSpatialHelpers.h"

using namespace dim;
using namespace spat;
using namespace trax;

BOOST_AUTO_TEST_SUITE(TestRollingStock)
//BOOST_FIXTURE_TEST_CASE( testWheelArrangementCreation, TrackFixture )
//{
//	PhysX_SimulatorTrain simulator{};
//
//	{
//		Location location{ m_pTrack, TrackLocation{ m_pTrack->GetLength()/2 } };
//		std::shared_ptr<RollingStock> pRS = RollingStock::Make( simulator );
//
//		BOOST_REQUIRE( pRS );
//		BOOST_REQUIRE_EQUAL( pRS->GetNumberOfWheelFrames(), 2 );
//		BOOST_CHECK_EQUAL( pRS->GetWheelFrame( 0 ).CntWheelsets(), 2 );
//		BOOST_CHECK_EQUAL( pRS->GetWheelFrame( 1 ).CntWheelsets(), 2 );
//
//		pRS->Rail( location );
//		pRS->TargetVelocity( 10_mIs );
//		pRS->Thrust( 0.5 );
//
//		BOOST_REQUIRE( pRS->IsRailed() );
//		BOOST_CHECK_EQUAL( pRS->TargetVelocity(), 10_mIs );
//		BOOST_CHECK_EQUAL( pRS->Thrust(), 0.5 );
//		BOOST_CHECK( pRS->ThrustAbsolute() );
//		BOOST_CHECK_EQUAL( pRS->GetLength(), 14_m );
//
//		BOOST_CHECK_CLOSE_DIMENSION( pRS->GetCouplingHeight( RailRunner::EndType::north ), 1_m, 0.0001 );
//		BOOST_CHECK_CLOSE_DIMENSION( pRS->GetCouplingHeight( RailRunner::EndType::south ), 1_m, 0.0001 );
//
//		Frame<Length,One> globalAnchor = pRS->GetWheelFrame( 0 ).Anchor(), bodyFrame, trackLocationFrame;
//		pRS->GetWheelFrame( 0 ).GetGestalt().GetFrame( bodyFrame );
//		bodyFrame.ToParent( globalAnchor );
//		location.Transition( trackLocationFrame );
//		BOOST_CHECK_CLOSE_SPATIAL2( globalAnchor, trackLocationFrame, epsilon__length, epsilon__angle );
//
//		globalAnchor = pRS->GetWheelFrame( 1 ).Anchor();
//		pRS->GetWheelFrame( 1 ).GetGestalt().GetFrame( bodyFrame );
//		bodyFrame.ToParent( globalAnchor );
//		location.Move( -7_m );
//		location.Transition( trackLocationFrame );
//		BOOST_CHECK_CLOSE_SPATIAL2( globalAnchor, trackLocationFrame, epsilon__length, epsilon__angle );
//
//		pRS->JackOnDerail().Insert( &simulator.PlugToStop() );
//
//		simulator.Loop();
//
//		BOOST_CHECK( !pRS->IsRailed() );
//	}
//}

//BOOST_FIXTURE_TEST_CASE( testWheelArrangementJacobsBogies1, TrainFixtureVisualDebugger )
BOOST_FIXTURE_TEST_CASE( testWheelArrangementJacobsBogies1, TrainFixture )
{
	RollingStockCreator creator{ *m_pScene };
	RollingStockFileReader reader{ creator, FixturePath() };
	BOOST_REQUIRE( reader( "Commuter.rollingstock" ) );
	std::shared_ptr<RollingStock> pRollingStock = creator.GetRollingStock();

	BOOST_REQUIRE( pRollingStock );

	pRollingStock->Rail( m_Location );
	pRollingStock->TargetVelocity( 10_mIs );
	pRollingStock->Thrust( 0.5 );

	BOOST_REQUIRE( pRollingStock->IsValid() );
	BOOST_REQUIRE( pRollingStock->IsRailed() );
	BOOST_CHECK_EQUAL( pRollingStock->GetNumberOfTopmostParents(), 4 );
	Frame<Length,One> globalLocation;
	pRollingStock->GetLocation().Transition( globalLocation );
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );

	pRollingStock->GetTopmostParent( 1 ).GetLocation().Transition( globalLocation );
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetTopmostParent( 1 ).GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );
	pRollingStock->GetTopmostParent( 2 ).GetLocation().Transition( globalLocation );
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetTopmostParent( 2 ).GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );
	pRollingStock->GetTopmostParent( 3 ).GetLocation().Transition( globalLocation );
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetTopmostParent( 3 ).GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );

	BOOST_CHECK_EQUAL( pRollingStock->GetNumberOfWheelFrames(), 5 );
	pRollingStock->GetWheelFrame( 0 ).GetLocation().Transition( globalLocation );
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetWheelFrame( 0 ).GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );
	pRollingStock->GetWheelFrame( 1 ).GetLocation().Transition( globalLocation );
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetWheelFrame( 1 ).GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );
	pRollingStock->GetWheelFrame( 2 ).GetLocation().Transition( globalLocation );
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetWheelFrame( 2 ).GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );
	pRollingStock->GetWheelFrame( 3 ).GetLocation().Transition( globalLocation );
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetWheelFrame( 3 ).GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );
	pRollingStock->GetWheelFrame( 4 ).GetLocation().Transition( globalLocation );
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetWheelFrame( 4 ).GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );

	m_pScene->Loop( 30_s );

	BOOST_CHECK( pRollingStock->IsRailed() );
}

BOOST_FIXTURE_TEST_CASE( testWheelArrangementJacobsBogiesRailOnTRackConnection, TrainFixture )
//BOOST_FIXTURE_TEST_CASE( testWheelArrangementJacobsBogiesRailOnTRackConnection, TrainFixtureVisualDebugger )
{
	RollingStockCreator creator{ *m_pScene };
	RollingStockFileReader reader{ creator, FixturePath() };
	BOOST_REQUIRE( reader( "Commuter.rollingstock" ) );
	std::shared_ptr<RollingStock> pRollingStock = creator.GetRollingStock();

	BOOST_REQUIRE( pRollingStock );

	m_Location.PutOn( m_pTrack5, TrackLocation{ 0_m, true } );
	pRollingStock->Rail( m_Location );
	pRollingStock->TargetVelocity( 0_mIs );
	pRollingStock->Thrust( 0.5 );

	BOOST_REQUIRE( pRollingStock->IsValid() );
	BOOST_REQUIRE( pRollingStock->IsRailed() );
	BOOST_CHECK_EQUAL( pRollingStock->GetNumberOfTopmostParents(), 4 );
	Frame<Length,One> globalLocation;
	pRollingStock->GetLocation().Transition( globalLocation );
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );

	pRollingStock->GetTopmostParent( 1 ).GetLocation().Transition( globalLocation );
	globalLocation.FlipBin();
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetTopmostParent( 1 ).GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );
	pRollingStock->GetTopmostParent( 2 ).GetLocation().Transition( globalLocation );
	globalLocation.FlipBin();
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetTopmostParent( 2 ).GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );
	pRollingStock->GetTopmostParent( 3 ).GetLocation().Transition( globalLocation );
	globalLocation.FlipBin();
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetTopmostParent( 3 ).GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );

	BOOST_CHECK_EQUAL( pRollingStock->GetNumberOfWheelFrames(), 5 );
	pRollingStock->GetWheelFrame( 0 ).GetLocation().Transition( globalLocation );
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetWheelFrame( 0 ).GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );
	pRollingStock->GetWheelFrame( 1 ).GetLocation().Transition( globalLocation );
	globalLocation.FlipBin();
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetWheelFrame( 1 ).GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );
	pRollingStock->GetWheelFrame( 2 ).GetLocation().Transition( globalLocation );
	globalLocation.FlipBin();
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetWheelFrame( 2 ).GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );
	pRollingStock->GetWheelFrame( 3 ).GetLocation().Transition( globalLocation );
	globalLocation.FlipBin();
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetWheelFrame( 3 ).GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );
	pRollingStock->GetWheelFrame( 4 ).GetLocation().Transition( globalLocation );
	globalLocation.FlipBin();
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetWheelFrame( 4 ).GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );

	m_pScene->Loop( 10_s );

	BOOST_CHECK( pRollingStock->IsRailed() );
}

BOOST_FIXTURE_TEST_CASE( testAxisArrangementReader, TrainFixture )//TrainFixtureVisualDebugger )
{
	std::cout << "simulator version: " << m_pSimulator->GetVersionString() << std::endl;

	RollingStockCreator creator{ *m_pScene };
	RollingStockFileReader reader{ creator, FixturePath() };
	BOOST_REQUIRE( reader( "BR212_267-9_2cb_SM2.rollingstock" ) );
	std::shared_ptr<RollingStock> pRollingStock = creator.GetRollingStock();
	BOOST_REQUIRE( pRollingStock );

	pRollingStock->Rail( m_Location );
	pRollingStock->TargetVelocity( 10_mIs );
	pRollingStock->Thrust( 0.5 );

	BOOST_REQUIRE( pRollingStock->IsValid() );
	BOOST_REQUIRE( pRollingStock->IsRailed() );

	m_pScene->Loop( 30_s );

	BOOST_CHECK( pRollingStock->IsRailed() );
}

BOOST_FIXTURE_TEST_CASE( testAxisArrangementReader2, TrainFixture )//TrainFixtureVisualDebugger )
{
	std::cout << "simulator version: " << m_pSimulator->GetVersionString() << std::endl;

	RollingStockCreator creator{ *m_pScene };
	RollingStockFileReader reader{ creator, FixturePath() };
	BOOST_REQUIRE( reader( "Okmm38.rollingstock" ) );
	std::shared_ptr<RollingStock> pRollingStock = creator.GetRollingStock();
	BOOST_REQUIRE( pRollingStock );

	pRollingStock->Rail( m_Location );
	pRollingStock->SetVelocity( 5_mIs );

	BOOST_REQUIRE( pRollingStock->IsValid() );
	BOOST_REQUIRE( pRollingStock->IsRailed() );

	m_pScene->Loop( 30_s );

	BOOST_CHECK( pRollingStock->IsRailed() );
}

//BOOST_FIXTURE_TEST_CASE( testRollingStockReaderReader, TrainFixture )//TrainFixtureVisualDebugger )
BOOST_FIXTURE_TEST_CASE( testRollingStockReaderReader, TrainFixtureVisualDebugger )
{
	RollingStockCreator creator{ *m_pScene };
	RollingStockFileReader reader{ creator, FixturePath() };
	BOOST_REQUIRE( reader( "DB_212.rollingstock" ) );
	std::shared_ptr<RollingStock> pRollingStock = creator.GetRollingStock();
	BOOST_REQUIRE( pRollingStock );

	pRollingStock->Rail( m_Location );
	pRollingStock->TargetVelocity( 10_mIs );
	pRollingStock->Thrust( 0.5 );

	BOOST_REQUIRE( pRollingStock->IsValid() );
	BOOST_REQUIRE( pRollingStock->IsRailed() );
	Frame<Length,One> globalLocation;
	pRollingStock->GetLocation().Transition( globalLocation );
	BOOST_CHECK_CLOSE_SPATIAL2( pRollingStock->GetGlobalAnchor(), globalLocation, epsilon__length, epsilon__angle );

	m_pScene->Loop( 30_s );

	BOOST_CHECK( pRollingStock->IsRailed() );
}

BOOST_FIXTURE_TEST_CASE( testRollingStockReaderReader2, TrackCircle )
{
	std::unique_ptr<trax::Simulator> pSimulator = Simulator::Make( Simulator::Type::PhysX );
	BOOST_REQUIRE( pSimulator );
	std::unique_ptr<trax::Scene> pScene = Scene::Make( *pSimulator );
	BOOST_REQUIRE( pScene );

	RollingStockCreator creator{ *pScene };
	RollingStockFileReader reader{ creator, FixturePath() };
	BOOST_REQUIRE( reader( "DR_Eas5951-EpIV_SK2-v8.rollingstock" ) );
	std::shared_ptr<RollingStock> pRollingStock = creator.GetRollingStock();
	BOOST_REQUIRE( pRollingStock );

	pRollingStock->Rail( m_Location );
	pRollingStock->TargetVelocity( 10_mIs );
	pRollingStock->Thrust( 0.5 );

	BOOST_REQUIRE( pRollingStock->IsValid() );
	BOOST_REQUIRE( pRollingStock->IsRailed() );

	pScene->Loop( 30_s );

	BOOST_CHECK( pRollingStock->IsRailed() );
}

BOOST_FIXTURE_TEST_CASE( testRollingStockReaderReader3, TrainFixture )
//BOOST_FIXTURE_TEST_CASE( testRollingStockReaderReader3, TrainFixtureVisualDebugger )
{
	RollingStockCreator creator{ *m_pScene };
	RollingStockFileReader reader{ creator, FixturePath() };
	BOOST_REQUIRE( reader( "DR_Eas5951-EpIV_SK2-v8.rollingstock" ) );
	std::shared_ptr<RollingStock> pRollingStock = creator.GetRollingStock();
	BOOST_REQUIRE( pRollingStock );

	m_Location.PutOn( m_pTrack5, TrackLocation{ 5_m, true } );
	pRollingStock->Rail( m_Location );
	pRollingStock->Rail( m_Location ); // rerailing of railed rolling stock has to work.
	pRollingStock->TargetVelocity( 10_mIs );
	pRollingStock->Thrust( 0.5 );

	BOOST_REQUIRE( pRollingStock->IsValid() );
	BOOST_REQUIRE( pRollingStock->IsRailed() );

	m_pScene->Loop( 10_s );

	BOOST_CHECK( pRollingStock->IsRailed() );

	Frame<Length, One> globalLocationWF1;
	pRollingStock->GetWheelFrame( 0 ).GetGestalt().GetFrame( globalLocationWF1 );
	Frame<Length, One> globalLocationWF2;
	pRollingStock->GetWheelFrame( 1 ).GetGestalt().GetFrame( globalLocationWF2 );

	// on a straight track all wheelframes must roll in the same direction:
	BOOST_CHECK_CLOSE_SPATIAL( globalLocationWF1.T, globalLocationWF2.T, epsilon__angle );
}

BOOST_FIXTURE_TEST_CASE( testRollingStockReaderReader4, TrainFixture )
//BOOST_FIXTURE_TEST_CASE( testRollingStockReaderReader3, TrainFixtureVisualDebugger )
{
	RollingStockCreator creator{ *m_pScene };
	RollingStockFileReader reader{ creator, FixturePath() };
	BOOST_REQUIRE( reader( "DR_Eas5951-EpIV_SK2-v8.rollingstock" ) );
	std::shared_ptr<RollingStock> pRollingStock = creator.GetRollingStock();
	BOOST_REQUIRE( pRollingStock );

	m_Location.PutOn( m_pTrack5, TrackLocation{ -5_m, true } );
	pRollingStock->Rail( m_Location );
	pRollingStock->Rail( m_Location ); // rerailing of railed rolling stock has to work.
	pRollingStock->TargetVelocity( 10_mIs );
	pRollingStock->Thrust( 0.5 );

	BOOST_REQUIRE( pRollingStock->IsValid() );
	BOOST_REQUIRE( pRollingStock->IsRailed() );

	m_pScene->Loop( 10_s );

	BOOST_CHECK( pRollingStock->IsRailed() );

	Frame<Length, One> globalLocationWF1;
	pRollingStock->GetWheelFrame( 0 ).GetGestalt().GetFrame( globalLocationWF1 );
	Frame<Length, One> globalLocationWF2;
	pRollingStock->GetWheelFrame( 1 ).GetGestalt().GetFrame( globalLocationWF2 );

	// on a straight track all wheelframes must roll in the same direction:
	BOOST_CHECK_CLOSE_SPATIAL( globalLocationWF1.T, globalLocationWF2.T, epsilon__angle );
}

BOOST_AUTO_TEST_SUITE_END()

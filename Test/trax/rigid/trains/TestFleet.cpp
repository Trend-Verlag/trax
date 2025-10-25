//	trax track library
//	AD 2025 
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
#include "trax/collections/support/FixturesCollections.h"
#include "trax/rigid/trains/support/TrainFileReader.h"
#include "trax/rigid/trains/Bogie.h"
#include "trax/rigid/trains/RollingStock.h"
#include "trax/rigid/trains/Train.h"
#include "trax/rigid/trains/collections/Fleet.h"

using namespace trax;

//BOOST_FIXTURE_TEST_CASE( testTrainUncoupleCouple, TrainFixtureVisualDebugger )
BOOST_FIXTURE_TEST_CASE( testTrainUncoupleCouple, TrainFixture ) 
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "Cargo.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );
	BOOST_CHECK( pTrain->IsValid() );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 10 );

	std::shared_ptr<Fleet> pFleet = Fleet::Make();
	BOOST_REQUIRE( pFleet );
	m_pScene->Register( *pFleet );
	BOOST_REQUIRE( pFleet->Add( pTrain ) );
	BOOST_CHECK_EQUAL( pFleet->Count(), 1 );

	m_Location.PutOn( m_pTrack5, TrackLocation{ m_pTrack5->GetLength() - 50_m, true } );	
	pTrain->Rail( m_Location );
	BOOST_CHECK( pTrain->IsRailed() );

	pTrain->TargetVelocity( 10_mIs );
	pTrain->Thrust( 0.75 );
	pTrain->Brake( 0.75 );
	pTrain->GetComponent( 4 )->GetTipAt( RailRunner::EndType::north ).first.BreakCoupling( RailRunner::EndType::north );

	m_pScene->Loop( 2_s );

	BOOST_CHECK_EQUAL( pFleet->Count(), 2 );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 4 );
	BOOST_CHECK_EQUAL( pFleet->GetActive()->GetNumberOfComponents(), 6 );

	pTrain->TargetVelocity( -1_mIs );
	pFleet->GetActive()->TargetVelocity( 0_mIs );
	pTrain->ActivateCoupling( RailRunner::EndType::south );
	pFleet->GetActive()->ActivateCoupling( RailRunner::EndType::north );

	m_pScene->Loop( 10_s );

	BOOST_CHECK_EQUAL( pFleet->Count(), 3 );
	BOOST_CHECK_EQUAL( pFleet->GetActive()->GetNumberOfComponents(), 2 );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 4 );

	pFleet->GetActive()->TargetVelocity( 10_mIs );
	m_pScene->Loop( 10_s );

	m_pScene->Unregister( *pFleet );
}

//BOOST_FIXTURE_TEST_CASE( testTrainUncoupleCouple, TrainFixtureVisualDebugger )
BOOST_FIXTURE_TEST_CASE( testTwoRollingStockCouple, TrainFixture ) 
{
	RollingStockCreator creator{ *m_pScene };
	RollingStockFileReader reader{ creator, FixturePath() };
	BOOST_REQUIRE( reader( "BR212_267-9_2cb_SM2.rollingstock" ) );
	std::shared_ptr<RollingStock> pLoco = creator.GetRollingStock();
	BOOST_REQUIRE( pLoco );
	std::shared_ptr<Train> pLocoTrain = Train::Make();
	BOOST_REQUIRE( pLocoTrain );
	pLocoTrain->Create( *pLoco );

	BOOST_REQUIRE( reader( "Okmm38.rollingstock" ) );
	std::shared_ptr<RollingStock> pWaggon = creator.GetRollingStock();
	BOOST_REQUIRE( pWaggon );
	std::shared_ptr<Train> pWaggonTrain = Train::Make();
	BOOST_REQUIRE( pWaggonTrain );
	pWaggonTrain->Create( *pWaggon );
	pWaggonTrain->ActivateCoupling( RailRunner::EndType::north );

	m_Location.PutOn( m_pTrack5, TrackLocation{ m_pTrack5->GetLength() - 50_m, true } );	
	pLoco->Rail( m_Location );
	BOOST_CHECK( pLoco->IsRailed() );

	m_Location.PutOn( m_pTrack5, TrackLocation{ m_pTrack5->GetLength() - 70_m, true } );	
	pWaggon->Rail( m_Location );
	BOOST_CHECK( pWaggon->IsRailed() );

	std::shared_ptr<Fleet> pFleet = Fleet::Make();
	BOOST_REQUIRE( pFleet );
	m_pScene->Register( *pFleet );
	BOOST_REQUIRE( pFleet->Add( pLocoTrain ) );
	BOOST_REQUIRE( pFleet->Add( pWaggonTrain ) );

	pLocoTrain->TargetVelocity( -5_mIs );
	pLocoTrain->Thrust( 0.75 );
	pLocoTrain->Brake( 0.75 );
	pLocoTrain->ActivateCoupling( RailRunner::EndType::south );

	BOOST_CHECK_EQUAL( pFleet->Count(), 2 );
	m_pScene->Loop( 5_s );
	BOOST_CHECK_EQUAL( pFleet->Count(), 1 );
}
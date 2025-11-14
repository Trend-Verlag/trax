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

#if defined( WITH_BOOST_TESTS )
#include <boost/test/unit_test.hpp>

#include "trax/Jack.h"
#include "trax/LogicElements.h"
#include "trax/Plug.h"
#include "trax/rigid/trains/Bogie.h"
#include "trax/rigid/trains/Train.h"
#include "trax/rigid/trains/support/FixturesTrain.h"
#include "trax/rigid/trains/support/TrainFileReader.h"

#include "dim/BoostTestDimensionedValuesHelpers.h"

using namespace trax;

BOOST_AUTO_TEST_SUITE(trax_tests)
BOOST_AUTO_TEST_SUITE(TestJacksNPlugs)

BOOST_FIXTURE_TEST_CASE( testTrainSplit, TrainFixture ) //
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "Cargo.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );
	BOOST_CHECK( pTrain->IsValid() );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 10 );

	std::unique_ptr<PulseCounter> pCounter = PulseCounter::Make();
	pTrain->JackOnSeparation().InsertAtTail( &pCounter->PlugToCountUp().Make() );

	auto subs = pTrain->Split( 1 );									// pTrain split in two subtrains
	BOOST_CHECK_EQUAL( pCounter->Counter(), 0 );

	BOOST_REQUIRE( subs.first );
	BOOST_CHECK( subs.first->IsValid() );
	BOOST_CHECK_EQUAL( subs.first->GetNumberOfComponents(), 2 );	// First subtrain 2 ...
	BOOST_REQUIRE( subs.second );
	BOOST_CHECK( subs.second->IsValid() );
	BOOST_CHECK_EQUAL( subs.second->GetNumberOfComponents(), 8 );	// Second subtrain 8 components

	subs.first->Uncouple();											// Uncouple first sub from second
	BOOST_CHECK_EQUAL( pCounter->Counter(), 1 );
	pTrain->Clear();												// pTrain lets go its subtrains
	BOOST_CHECK( !pTrain->IsValid() );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 0 );

	pCounter->PlugToCountUp().Clear();
	pCounter->Counter( 0 );
	subs.second->JackOnSeparation().InsertAtTail( &pCounter->PlugToCountUp().Make() );
	auto subsubs = subs.second->Split( 3 );							// Second sub split in halfes (two subsub trains)
	BOOST_CHECK_EQUAL( pCounter->Counter(), 0 );

	BOOST_REQUIRE( subsubs.first );
	BOOST_CHECK( subsubs.first->IsValid() );
	BOOST_CHECK_EQUAL( subsubs.first->GetNumberOfComponents(), 4 );	// Each subsub has 4 components
	BOOST_REQUIRE( subsubs.second );
	BOOST_CHECK( subsubs.second->IsValid() );
	BOOST_CHECK_EQUAL( subsubs.second->GetNumberOfComponents(), 4 );

	subsubs.first->Uncouple();										// Uncouple the subsubs
	BOOST_CHECK_EQUAL( pCounter->Counter(), 1 );

	std::shared_ptr<Train> pLastTrain = subs.second->Separate();	// Separate second subtrain
	BOOST_REQUIRE( pLastTrain );									// pLastTrain becomes train of subsubs.second
	BOOST_CHECK( pLastTrain->IsValid() );
	BOOST_CHECK_EQUAL( pLastTrain->GetNumberOfComponents(), 1 );
	BOOST_CHECK_EQUAL( subsubs.second->GetTrain(), pLastTrain.get() );

	pLastTrain->Reduce();
	BOOST_CHECK( pLastTrain->IsValid() );
	BOOST_CHECK_EQUAL( pLastTrain->GetNumberOfComponents(), 4 );	// pLastTrain has 4 components
	BOOST_CHECK( !subsubs.second->IsValid() );
	BOOST_CHECK_EQUAL( subsubs.second->GetNumberOfComponents(), 0 );// subsubs.second is no longer valid
	BOOST_CHECK_EQUAL( subsubs.second->GetTrain(), nullptr );

	BOOST_CHECK( subs.second->IsValid() );							// sub.second is still valid,
	BOOST_CHECK_EQUAL( subs.second->GetNumberOfComponents(), 1 );	// left with 1 components.
	BOOST_CHECK( subsubs.first->IsValid() );						// subsubs.first is still valid,
	BOOST_CHECK_EQUAL( subsubs.first->GetNumberOfComponents(), 4 );	// left with 4 components.
}

BOOST_FIXTURE_TEST_CASE( testTrainUncouple, TrainFixture ) //
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "Cargo.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );
	BOOST_CHECK( pTrain->IsValid() );

	std::unique_ptr<PulseCounter> pCounter = PulseCounter::Make();

	pTrain->GetComponent( 1 )->GetTipAt( RailRunner::EndType::south ).first.JackOnUnCouple( RailRunner::EndType::south ).InsertAtTail( &pCounter->PlugToCountUp().Make() );
	pTrain->GetComponent( 1 )->JackOnUnCouple( RailRunner::EndType::south ).InsertAtTail( &pCounter->PlugToCountUp().Make() );
	pTrain->JackOnSeparation().InsertAtTail( &pCounter->PlugToCountUp().Make() );

	pTrain->GetComponent( 1 )->Uncouple( RailRunner::EndType::south );
	BOOST_CHECK_EQUAL( pCounter->Counter(), 3 );

	std::shared_ptr<Train> pNewTrain = pTrain->Separate();
	BOOST_REQUIRE( pNewTrain );
	BOOST_CHECK( pNewTrain->IsValid() );
	BOOST_CHECK_EQUAL( pNewTrain->GetNumberOfComponents(), 8 );	
	BOOST_CHECK( pTrain->IsValid() );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 2 );
}

BOOST_FIXTURE_TEST_CASE( testOnCouplingActivated, TrainFixture ) //
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "Cargo.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );
	BOOST_CHECK( pTrain->IsValid() );
	BOOST_CHECK( !pTrain->IsActivated( RailRunner::EndType::north ) );
	BOOST_CHECK( !pTrain->IsActivated( RailRunner::EndType::south ) );

	std::unique_ptr<PulseCounter> pCounter = PulseCounter::Make();

	pTrain->GetComponent( 0 )->GetTipAt( RailRunner::EndType::north ).first.JackOnCouplingActivated( RailRunner::EndType::north ).InsertAtTail( &pCounter->PlugToCountUp().Make() );
	pTrain->GetComponent( 0 )->JackOnCouplingActivated( RailRunner::EndType::north ).InsertAtTail( &pCounter->PlugToCountUp().Make() );
	pTrain->JackOnCouplingActivated( RailRunner::EndType::north ).InsertAtTail( &pCounter->PlugToCountUp().Make() );

	pTrain->ActivateCoupling( RailRunner::EndType::north );
	BOOST_CHECK_EQUAL( pCounter->Counter(), 3 );
}

BOOST_FIXTURE_TEST_CASE( testOnUnCouplingCoupling, TrainFixture )
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "Cargo2.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );
	BOOST_CHECK( pTrain->IsValid() );
	BOOST_CHECK( !pTrain->IsActivated( RailRunner::EndType::north ) );
	BOOST_CHECK( !pTrain->IsActivated( RailRunner::EndType::south ) );

	std::unique_ptr<PulseCounter> pCounter = PulseCounter::Make();

	Plug* pNamedPlug = &pCounter->PlugToCountUp().Make();
	pNamedPlug->Reference( "parent", "PlugOnUncouple" );
	pTrain->GetComponent( 0 )->GetTipAt( RailRunner::EndType::north ).first.JackOnUnCouple( RailRunner::EndType::north ).InsertAtTail( pNamedPlug );
	pTrain->GetComponent( 0 )->JackOnUnCouple( RailRunner::EndType::north ).InsertAtTail( &pCounter->PlugToCountUp().Make() );
	pTrain->GetComponent( 0 )->Uncouple( RailRunner::EndType::north );
	BOOST_CHECK_EQUAL( pCounter->Counter(), 2 );
	pCounter->PlugToCountUp().Clear();
	pCounter->Counter( 0 );

	std::shared_ptr<Train> pOtherTrain = pTrain->Separate();
	BOOST_REQUIRE( pOtherTrain );
	BOOST_CHECK( pOtherTrain->IsValid() );
	BOOST_CHECK_EQUAL( pOtherTrain->GetNumberOfComponents(), 9 );	
	BOOST_CHECK( pTrain->IsValid() );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 1 );

	pTrain->GetComponent( 0 )->GetTipAt( RailRunner::EndType::north ).first.JackOnCouple( RailRunner::EndType::north ).InsertAtTail( &pCounter->PlugToCountUp().Make() );
	pTrain->GetComponent( 0 )->JackOnCouple( RailRunner::EndType::north ).InsertAtTail( &pCounter->PlugToCountUp().Make() );
	pTrain->JackOnCouple( RailRunner::EndType::south ).InsertAtTail( &pCounter->PlugToCountUp().Make() );
	
	pOtherTrain->GetComponent( 0 )->GetTipAt( RailRunner::EndType::north ).first.JackOnCouple( RailRunner::EndType::north ).InsertAtTail( &pCounter->PlugToCountUp().Make() );
	pOtherTrain->GetComponent( 0 )->JackOnCouple( RailRunner::EndType::north ).InsertAtTail( &pCounter->PlugToCountUp().Make() );
	pOtherTrain->JackOnCouple( RailRunner::EndType::north ).InsertAtTail( &pCounter->PlugToCountUp().Make() );
	
	BOOST_CHECK( pTrain->Couple( RailRunner::EndType::south, *pOtherTrain, RailRunner::EndType::north ) );
	BOOST_CHECK_EQUAL( pCounter->Counter(), 6 );
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END() //trax_tests
#endif
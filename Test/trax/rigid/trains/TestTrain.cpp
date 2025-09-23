//	trax track library
//	AD 2024 
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

#include "trax/support/TraxSupportStream.h"
#include "trax/rigid/StaticTrack.h"
#include "trax/collections/TrackSystem.h"
#include "trax/rigid/trains/support/RollingStockCreator.h"
#include "trax/rigid/trains/Train.h"
#include "trax/rigid/trains/support/TrainFileReader.h"

#include "dim/DimLimits.h"
#include "dim/support/DimSupportStream.h"
#include "dim/BoostTestDimensionedValuesHelpers.h"
#include "spat/BoostTestSpatialHelpers.h"

using namespace spat;
using namespace trax;

BOOST_AUTO_TEST_SUITE(TestTrain)

//BOOST_FIXTURE_TEST_CASE( testTrainCreation, TrainFixture )
BOOST_FIXTURE_TEST_CASE( testTrainCreation, TrainFixtureVisualDebugger )
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "Cargo.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );
	BOOST_CHECK( pTrain->IsValid() );

	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 10 );
	// 11.6_m + 9 * 13.2_m + 9 * 2 * 0.4_m;
	Length theoreticalLength = 137.6_m;
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetLength( TrainComponent::DistanceType::max ), theoreticalLength, 0.1 );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetLength( TrainComponent::DistanceType::actual ), theoreticalLength, 0.1 );

//	m_Location.PutOn( m_pTrack5, TrackLocation{ m_pTrack5->GetLength() - 10_m, true } );	
	pTrain->Rail( m_Location );
	pTrain->TargetVelocity( 10_mIs );
	pTrain->Thrust( 1 );

	BOOST_REQUIRE( pTrain->IsValid() );
	BOOST_REQUIRE( pTrain->IsRailed() );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetOverhang( RailRunner::EndType::north, TrainComponent::DistanceType::max ), 2.94_m, 0.1 );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetOverhang( RailRunner::EndType::south, TrainComponent::DistanceType::max ), theoreticalLength - 2.94_m, 0.1 );

	BOOST_CHECK_EQUAL( pTrain->TargetVelocity(), 10_mIs );
	BOOST_CHECK_EQUAL( pTrain->Thrust(), 1 );
	BOOST_CHECK( pTrain->ThrustAbsolute() );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetLength( TrainComponent::DistanceType::max ), theoreticalLength, 0.1 );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetTotalMass(), 73_t + 9 * 26_t, 0.1 );

	Time simulationTime = 25_s;

	const auto start = std::chrono::steady_clock::now();
	m_pScene->Loop( simulationTime );
	const auto end = std::chrono::steady_clock::now();

	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count();
	std::cout << "Duration of simulation: " << duration << "ms; simulates ";
	std::cout << simulationTime / _ms(duration);
	std::cout << " times faster than real world time." << std::endl;

	BOOST_CHECK( pTrain->IsRailed() );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetVelocity(), 10_mIs, 1 );
}

//BOOST_FIXTURE_TEST_CASE( testTrainCreation2, TrainFixtureVisualDebugger ) 
BOOST_FIXTURE_TEST_CASE( testTrainCreation2, TrainFixture )
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "Commuter.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );
	BOOST_CHECK( pTrain->IsValid() );

	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 1 );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetLength( TrainComponent::DistanceType::max ), 79_m, 0.1 );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetLength( TrainComponent::DistanceType::actual ), 79_m, 0.1 );

	//m_Location.PutOn( m_pTrack5, TrackLocation{ m_pTrack5->GetLength()/2, true } );
	pTrain->Rail( m_Location );
	pTrain->TargetVelocity( 10_mIs );
	pTrain->Thrust( 0.75 );

	BOOST_REQUIRE( pTrain->IsValid() );
	BOOST_REQUIRE( pTrain->IsRailed() );
	BOOST_CHECK_EQUAL( pTrain->MaxThrust(), 220_kNm / 50_cm );
	BOOST_CHECK_EQUAL( pTrain->MaxBrake(), 333.3334_kNm / 50_cm );
	BOOST_CHECK_EQUAL( pTrain->TargetVelocity(), 10_mIs );
	BOOST_CHECK_EQUAL( pTrain->Thrust(), 0.75 );
	BOOST_CHECK( pTrain->ThrustAbsolute() < 220_kNm / 50_cm );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetLength( TrainComponent::DistanceType::max ), 79_m, 0.1 );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetTotalMass(), 143_t , 0.1 );

	Time simulationTime = 40_s;

	const auto start = std::chrono::steady_clock::now();
	m_pScene->Loop( simulationTime );
	const auto end = std::chrono::steady_clock::now();

	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count();
	std::cout << "Duration of simulation: " << duration << "ms; simulates ";
	std::cout << simulationTime / _ms(duration);
	std::cout << " times faster than real world time." << std::endl;

	BOOST_CHECK( pTrain->IsRailed() );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetVelocity(), 10_mIs, 1 );
}

//BOOST_FIXTURE_TEST_CASE( testTrainCreation3, TrainFixtureVisualDebugger ) 
BOOST_FIXTURE_TEST_CASE( testTrainCreation3, TrainFixture )
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "Commuter.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );
	BOOST_CHECK( pTrain->IsValid() );

	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 1 );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetLength( TrainComponent::DistanceType::max ), 79_m, 0.1 );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetLength( TrainComponent::DistanceType::actual ), 79_m, 0.1 );

	m_Location.PutOn( m_pTrack5, TrackLocation{ 30_m, true } );
	pTrain->Rail( m_Location );
	pTrain->TargetVelocity( 0_mIs );
	pTrain->Thrust( 0.75 );

	BOOST_REQUIRE( pTrain->IsValid() );
	BOOST_REQUIRE( pTrain->IsRailed() );
	BOOST_CHECK_EQUAL( pTrain->MaxThrust(), 220_kNm / 50_cm );
	BOOST_CHECK_EQUAL( pTrain->MaxBrake(), 333.3334_kNm / 50_cm );
	BOOST_CHECK_EQUAL( pTrain->TargetVelocity(), 0_mIs );
	BOOST_CHECK_EQUAL( pTrain->Thrust(), 0.75 );
	BOOST_CHECK( pTrain->ThrustAbsolute() < 220_kNm / 50_cm );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetLength( TrainComponent::DistanceType::max ), 79_m, 0.1 );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetTotalMass(), 143_t , 0.1 );

	Time simulationTime = 10_s;

	const auto start = std::chrono::steady_clock::now();
	m_pScene->Loop( simulationTime );
	const auto end = std::chrono::steady_clock::now();

	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count();
	std::cout << "Duration of simulation: " << duration << "ms; simulates ";
	std::cout << simulationTime / _ms(duration);
	std::cout << " times faster than real world time." << std::endl;

	BOOST_CHECK( pTrain->IsRailed() );
	BOOST_CHECK_SMALL( pTrain->GetVelocity(), epsilon__velocity );
}

BOOST_FIXTURE_TEST_CASE( testTrainCreation4, TrainFixture )
//BOOST_FIXTURE_TEST_CASE( testTrainCreation4, TrainFixtureVisualDebugger )
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "DoubleCommuter.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );
	BOOST_CHECK( pTrain->IsValid() );

	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 2 );
	// 2*79_m + 2*0.5_m
	Length theoreticalLength = 159_m;
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetLength( TrainComponent::DistanceType::max ), theoreticalLength, 0.1 );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetLength( TrainComponent::DistanceType::actual ), theoreticalLength, 0.1 );

	//m_Location.PutOn( m_pTrack5, TrackLocation{ m_pTrack5->GetLength() - 10_m, true } );
	pTrain->Rail( m_Location );
	pTrain->TargetVelocity( 10_mIs );
	pTrain->Thrust( 0.75 );

	BOOST_REQUIRE( pTrain->IsValid() );
	BOOST_REQUIRE( pTrain->IsRailed() );
	BOOST_CHECK_EQUAL( pTrain->TargetVelocity(), 10_mIs );
	BOOST_CHECK_EQUAL( pTrain->Thrust(), 0.75 );
	BOOST_CHECK( pTrain->ThrustAbsolute() );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetLength( TrainComponent::DistanceType::max ), theoreticalLength, 0.1 );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetTotalMass(), 2*143_t , 0.1 );

	Time simulationTime = 40_s;

	const auto start = std::chrono::steady_clock::now();
	m_pScene->Loop( simulationTime );
	const auto end = std::chrono::steady_clock::now();

	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count();
	std::cout << "Duration of simulation: " << duration << "ms; simulates ";
	std::cout << simulationTime / _ms(duration);
	std::cout << " times faster than real world time." << std::endl;

	BOOST_CHECK( pTrain->IsRailed() );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetVelocity(), 10_mIs, 1 );
}

BOOST_FIXTURE_TEST_CASE( testTrainCreation5, TrainFixture ) //VisualDebugger
//BOOST_FIXTURE_TEST_CASE( testTrainCreation5, TrainFixtureVisualDebugger ) //
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "CargoTwoLokos.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );
	BOOST_CHECK( pTrain->IsValid() );

	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 11 );
	BOOST_CHECK_EQUAL( pTrain->GetComponent(0)->GetOrientation(), Orientation::Value::para );
	BOOST_CHECK_EQUAL( pTrain->GetComponent(1)->GetOrientation(), Orientation::Value::anti );
	BOOST_CHECK_EQUAL( pTrain->GetComponent(10)->GetOrientation(), Orientation::Value::anti );
	// 2*11.6_m + 9 * 13.2_m + 10 * 2 * 0.4_m;
	Length theoreticalLength = 150_m;
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetLength( TrainComponent::DistanceType::max ), theoreticalLength, 0.1 );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetLength( TrainComponent::DistanceType::actual ), theoreticalLength, 0.1 );

	//m_Location.PutOn( m_pTrack5, TrackLocation{ m_pTrack5->GetLength() -10_m, true } );
	pTrain->Rail( m_Location );
	pTrain->TargetVelocity( 5_mIs );
	pTrain->Thrust( 0.75 );

	BOOST_REQUIRE( pTrain->IsValid() );
	BOOST_REQUIRE( pTrain->IsRailed() );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetOverhang( RailRunner::EndType::north, TrainComponent::DistanceType::max ), 2.94_m, 0.1 );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetOverhang( RailRunner::EndType::south, TrainComponent::DistanceType::max ), theoreticalLength - 2.94_m, 0.1 );
	BOOST_CHECK_EQUAL( pTrain->TargetVelocity(), 5_mIs );
	BOOST_CHECK_EQUAL( pTrain->Thrust(), 0.75 );
	BOOST_CHECK( pTrain->ThrustAbsolute() );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetLength( TrainComponent::DistanceType::max ), theoreticalLength, 0.1 );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetTotalMass(), 2*73_t + 9 * 26_t, 0.1 );

	Time simulationTime = 40_s;

	const auto start = std::chrono::steady_clock::now();
	m_pScene->Loop( simulationTime );
	const auto end = std::chrono::steady_clock::now();

	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count();
	std::cout << "Duration of simulation: " << duration << "ms; simulates ";
	std::cout << simulationTime / _ms(duration);
	std::cout << " times faster than real world time." << std::endl;

	BOOST_CHECK( pTrain->IsRailed() );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetVelocity(), 5_mIs, 1 );
}

BOOST_FIXTURE_TEST_CASE( testTrainCreation6, TrainFixture )//VisualDebugger
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "CompoundTrain.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );
	BOOST_CHECK( pTrain->IsValid() );

	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 4 );
	// (79_m + (2*79_m + 2*0.5_m) + (11.6_m + 9 * 13.2_m + 9 * 2 * 0.4_m) + (2*11.6_m + 9 * 13.2_m + 10 * 2 * 0.4_m) +3*(0.5_m + 0.4_m))
	Length theoreticalLength = 79_m + 159_m + 150_m + 137.6_m + 2.7_m;
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetLength( TrainComponent::DistanceType::max ), theoreticalLength, 0.1 );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetLength( TrainComponent::DistanceType::actual ), theoreticalLength, 0.1 );

	//m_Location.PutOn( m_pTrack5, TrackLocation{ m_pTrack5->GetLength() - 10_m, true } );
	pTrain->Rail( m_Location );
	pTrain->TargetVelocity( 10_mIs );
	pTrain->Thrust( 0.75 );
	pTrain->Brake( 0.5 );

	BOOST_REQUIRE( pTrain->IsValid() );
	BOOST_REQUIRE( pTrain->IsRailed() );
	BOOST_CHECK_EQUAL( pTrain->TargetVelocity(), 10_mIs );
	BOOST_CHECK_EQUAL( pTrain->Thrust(), 0.75 );
	BOOST_CHECK_EQUAL( pTrain->Brake(), 0.5 );
	BOOST_CHECK( pTrain->ThrustAbsolute() );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetLength( TrainComponent::DistanceType::max ), theoreticalLength, 0.1 );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetTotalMass(), 3*143_t + (73_t + 9 * 26_t) + (2*73_t + 9 * 26_t), 0.1 );

	Time simulationTime = 40_s;

	const auto start = std::chrono::steady_clock::now();
	m_pScene->Loop( simulationTime );
	const auto end = std::chrono::steady_clock::now();

	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count();
	std::cout << "Duration of simulation: " << duration << "ms; simulates ";
	std::cout << simulationTime / _ms(duration);
	std::cout << " times faster than real world time." << std::endl;

	BOOST_CHECK( pTrain->IsRailed() );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetVelocity(), 10_mIs, 1 );

	pTrain->Reduce();
	pTrain->TargetVelocity( 0_mIs );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 11 + 10 + 2 + 1 );
	BOOST_CHECK_EQUAL( pTrain->TargetVelocity(), 0_mIs );
	m_pScene->Loop( simulationTime );
	BOOST_CHECK( pTrain->IsValid() );
	BOOST_CHECK( pTrain->IsRailed() );
	BOOST_CHECK_SMALL( pTrain->GetVelocity(), epsilon__velocity );
}

BOOST_FIXTURE_TEST_CASE( testTrainCreation7, TrainFixture ) //VisualDebugger
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "Mixed.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );
	BOOST_CHECK( pTrain->IsValid() );

	pTrain->Rail( m_Location );
	pTrain->TargetVelocity( 5_mIs );
	pTrain->Thrust( 0.75 );

	BOOST_REQUIRE( pTrain->IsValid() );
	BOOST_REQUIRE( pTrain->IsRailed() );

	BOOST_CHECK_EQUAL( pTrain->TargetVelocity(), 5_mIs );
	BOOST_CHECK_EQUAL( pTrain->Thrust(), 0.75 );
	BOOST_CHECK( pTrain->ThrustAbsolute() );

	Time simulationTime = 40_s;

	const auto start = std::chrono::steady_clock::now();
	m_pScene->Loop( simulationTime );
	const auto end = std::chrono::steady_clock::now();

	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count();
	std::cout << "Duration of simulation: " << duration << "ms; simulates ";
	std::cout << simulationTime / _ms(duration);
	std::cout << " times faster than real world time." << std::endl;

	BOOST_CHECK( pTrain->IsRailed() );
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->GetVelocity(), 5_mIs, 1 );
}

BOOST_FIXTURE_TEST_CASE( testMultipleTrainsRunning, MultiTrackSystemFixture ) //VisualDebugger ) 
{
	{
		int cntSystems = 8u; //1u; //64u; //125u; //216u;//27u;
		BuildFixture( cntSystems );

		BOOST_CHECK_EQUAL( m_pTrackSystem->Count(), 4*cntSystems );

		std::vector<std::shared_ptr<Train>> trains;

		for( int i = 0; i < cntSystems; ++i )
		{
			TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
			BOOST_REQUIRE( reader( "Cargo.train" ) );
			std::shared_ptr<Train> pTrain = reader.GetTrain();
			BOOST_REQUIRE( pTrain );

			Location location{ m_pTrackSystem->Get( 4*i+1 ), TrackLocation{ 0_m, true } };
			pTrain->Rail( location );
			pTrain->TargetVelocity( 10_mIs );
			pTrain->Thrust( 0.75 );

			BOOST_REQUIRE( pTrain->IsValid() );
			BOOST_REQUIRE( pTrain->IsRailed() );

			Position<Length> pos;
			pTrain->GetLocation().Transition( pos );
			std::cout << "Train " << i << " is at " << pos << std::endl;

			trains.push_back( pTrain );
		}

		Time simulationTime = 40_s;

		const auto start = std::chrono::steady_clock::now();
		m_pScene->Loop( simulationTime );
		const auto end = std::chrono::steady_clock::now();

		const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count();
		std::cout << "Duration of simulation: " << duration << "ms; simulates ";
		std::cout << simulationTime / _ms(duration);
		std::cout << " times faster than real world time." << std::endl;

		for( int i = 0; i < cntSystems; ++i )
		{
			BOOST_CHECK( trains[i]->IsRailed() );
			BOOST_CHECK_CLOSE_DIMENSION( trains[i]->GetVelocity(), 10_mIs, 1 );
		}
	}

	m_pTrackSystem.reset();
}

BOOST_AUTO_TEST_SUITE_END()
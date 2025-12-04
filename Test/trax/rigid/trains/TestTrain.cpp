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
// For further information, please contact: horstmann.marc@trendverlag.de

#if defined( WITH_BOOST_TESTS )

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

BOOST_AUTO_TEST_SUITE(trax_tests)
BOOST_AUTO_TEST_SUITE(TrainCreationTests)

BOOST_FIXTURE_TEST_CASE( testTrainCreation, TrainFixture )
//BOOST_FIXTURE_TEST_CASE( testTrainCreation, TrainFixtureVisualDebugger )
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
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->TotalMass(), 73_t + 9 * 26_t, 0.1 );

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
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->TotalMass(), 143_t , 0.1 );

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
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->TotalMass(), 143_t , 0.1 );

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
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->TotalMass(), 2*143_t , 0.1 );

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

BOOST_FIXTURE_TEST_CASE( testTrainCreation5, TrainFixture )
//BOOST_FIXTURE_TEST_CASE( testTrainCreation5, TrainFixtureVisualDebugger )
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
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->TotalMass(), 2*73_t + 9 * 26_t, 0.1 );

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
	BOOST_CHECK_CLOSE_DIMENSION( pTrain->TotalMass(), 3*143_t + (73_t + 9 * 26_t) + (2*73_t + 9 * 26_t), 0.1 );

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

//BOOST_FIXTURE_TEST_CASE( testTrainCreation7, TrainFixture ) //VisualDebugger
BOOST_FIXTURE_TEST_CASE( testTrainCreation7, TrainFixtureVisualDebugger )
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "Mixed.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );
	BOOST_CHECK( pTrain->IsValid() );

	pTrain->Rail( m_Location );
	pTrain->TargetVelocity( 5_mIs );
	pTrain->Thrust( 1 );
	pTrain->Brake( 0.75 );

	BOOST_REQUIRE( pTrain->IsValid() );
	BOOST_REQUIRE( pTrain->IsRailed() );

	BOOST_CHECK_EQUAL( pTrain->TargetVelocity(), 5_mIs );
	BOOST_CHECK_EQUAL( pTrain->Thrust(), 1 );
	BOOST_CHECK( pTrain->ThrustAbsolute() );

	Time simulationTime = 20_s;

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

BOOST_AUTO_TEST_SUITE_END() // TrainCreationTests
BOOST_AUTO_TEST_SUITE(TrainRunningTests)

//BOOST_FIXTURE_TEST_CASE( testMultipleTrainsRunning, MultiTrackSystemFixtureVisualDebugger )
BOOST_FIXTURE_TEST_CASE( testMultipleTrainsRunning, MultiTrackSystemFixture )
{
	{
		// Cargo.train has 10 cars with 20 wheelframes altogether. 
		// Simulate cntSystems*20 WheelFrames:
		int cntSystems = 4u;// //1u; //8u; //64u; //125u; //216u; //264u; //27u;
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

		std::cout << "Simulating " << cntSystems*20 << " WheelFrames, " << cntSystems*20 << " Hinges, " << cntSystems*9 << " couplings." << std::endl;

		Time simulationTime = 40_s;

		const auto start = std::chrono::steady_clock::now();
		m_pScene->Loop( simulationTime );
		const auto end = std::chrono::steady_clock::now();

		const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count();
		std::cout << "Duration of simulation: " << duration << "ms; simulates ";
		std::cout << simulationTime / _ms(duration);
		std::cout << " times faster than real world time." << std::endl;
		m_pScene->DumpTasksTo( std::cout );

		for( int i = 0; i < cntSystems; ++i )
		{
			BOOST_CHECK( trains[i]->IsRailed() );
			BOOST_CHECK_CLOSE_DIMENSION( trains[i]->GetVelocity(), 10_mIs, 1 );
		}
	}

	m_pTrackSystem.reset();
}

BOOST_AUTO_TEST_SUITE_END() // TrainRunningTests
BOOST_AUTO_TEST_SUITE(TrainCouplingTests)

BOOST_FIXTURE_TEST_CASE( testTrainDecouple, TrainFixture )
//BOOST_FIXTURE_TEST_CASE( testTrainDecouple, TrainFixtureVisualDebugger )
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "Cargo.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );
	BOOST_CHECK( pTrain->IsValid() );

	BOOST_CHECK_NO_THROW( pTrain->Rail( m_Location ) );

	BOOST_REQUIRE( pTrain->IsValid() );
	BOOST_REQUIRE( pTrain->IsRailed() );

	// no split happening:
	std::pair<std::shared_ptr<Train>,std::shared_ptr<Train>> Split = pTrain->SplitAfter( pTrain->GetNumberOfComponents()-1 );
	BOOST_CHECK_EQUAL( Split.first, nullptr );
	BOOST_CHECK_EQUAL( Split.second, nullptr );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 10 );
		
	BOOST_CHECK_NO_THROW( Split = pTrain->SplitAfter( pTrain->GetNumberOfComponents() ) );
	BOOST_CHECK_EQUAL( Split.first, nullptr );
	BOOST_CHECK_EQUAL( Split.second, nullptr );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 10 );

	BOOST_CHECK_NO_THROW( Split = pTrain->SplitAfter( -1 ) );
	BOOST_CHECK_EQUAL( Split.first, nullptr );
	BOOST_CHECK_EQUAL( Split.second, nullptr );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 10 );


	// split happens:
	BOOST_CHECK_NO_THROW( Split = pTrain->SplitAfter( 5 ) );
	BOOST_REQUIRE( Split.first );
	BOOST_REQUIRE( Split.second );
	BOOST_CHECK( Split.first->IsValid() );
	BOOST_CHECK( Split.second->IsValid() );
	BOOST_CHECK_EQUAL( Split.first->GetNumberOfComponents(), 6 );
	BOOST_CHECK_EQUAL( Split.second->GetNumberOfComponents(), 4 );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 2 );
	BOOST_CHECK_EQUAL( Split.first->GetCoupledTrainComponent( Train::EndType::south ).first, Split.second );
	BOOST_CHECK_EQUAL( Split.first->GetCoupledTrainComponent( Train::EndType::north ).first, nullptr );
	BOOST_CHECK_EQUAL( Split.second->GetCoupledTrainComponent( Train::EndType::north ).first, Split.first );
	BOOST_CHECK_EQUAL( Split.second->GetCoupledTrainComponent( Train::EndType::south ).first, nullptr );
	BOOST_CHECK( pTrain->IsValid() );
	BOOST_CHECK( !pTrain->IsUnCoupledInternally() );
	BOOST_CHECK( !Split.first->IsUnCoupledInternally() );
	BOOST_CHECK( !Split.second->IsUnCoupledInternally() );
	BOOST_CHECK_EQUAL( Split.first->GetTrain(), pTrain.get() );
	BOOST_CHECK_EQUAL( Split.second->GetTrain(), pTrain.get() );

	pTrain->Clear();
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 0 );
	BOOST_CHECK( !pTrain->IsValid() );
	BOOST_CHECK_EQUAL( Split.first->GetTrain(), nullptr );
	BOOST_CHECK_EQUAL( Split.second->GetTrain(), nullptr );


	// recouple:
	BOOST_CHECK( !Split.first->Uncouple( Train::EndType::south ) );
	BOOST_CHECK( Split.first->Couple( Train::EndType::south, *Split.second, Train::EndType::north ) );
	BOOST_CHECK( Split.first->IsCoupled( Train::EndType::south ) );
	BOOST_CHECK( Split.second->IsCoupled( Train::EndType::north ) );
	BOOST_CHECK( Split.first->Uncouple( Train::EndType::south ) );
	BOOST_CHECK( !Split.first->IsCoupled( Train::EndType::south ) );
	BOOST_CHECK( !Split.second->IsCoupled( Train::EndType::north ) );


	// Re-insert to train:
	BOOST_CHECK_NO_THROW( pTrain->Append( Train::EndType::south, Split.first, Train::EndType::north ) );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 1 );
	BOOST_CHECK_NO_THROW( pTrain->Append( Train::EndType::south, Split.second, Train::EndType::north ) );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 2 );
	BOOST_CHECK( pTrain->IsValid() );
	BOOST_CHECK( !pTrain->IsUnCoupledInternally() );
	pTrain->Reduce();
	BOOST_CHECK( pTrain->IsValid() );
	BOOST_CHECK( pTrain->IsRailed() );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 10 );
	BOOST_CHECK( !pTrain->IsUnCoupledInternally() );
}

BOOST_FIXTURE_TEST_CASE( testTrainSplit, TrainFixture )
//BOOST_FIXTURE_TEST_CASE( testTrainSplit, TrainFixtureVisualDebugger )
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "Cargo.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );
	BOOST_CHECK( pTrain->IsValid() );

	BOOST_CHECK_NO_THROW( pTrain->Rail( m_Location ) );

	BOOST_REQUIRE( pTrain->IsValid() );
	BOOST_REQUIRE( pTrain->IsRailed() );

	std::shared_ptr<TrainComponent> pTrainComponent = pTrain->GetComponent( 5 );
	BOOST_REQUIRE( pTrainComponent );

	BOOST_CHECK( pTrainComponent->Uncouple( RailRunner::EndType::north ) );
	BOOST_CHECK( pTrain->IsUnCoupledInternally() );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 10 );
	std::shared_ptr<Train> pSeparate = pTrain->Separate();
	BOOST_REQUIRE( pSeparate );
	BOOST_CHECK( pSeparate->IsValid() );
	BOOST_CHECK( pSeparate->IsRailed() );
	BOOST_CHECK_EQUAL( pSeparate->GetNumberOfComponents(), 5 );
	BOOST_CHECK( !pSeparate->IsUnCoupledInternally() );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 5 );
	BOOST_CHECK( !pTrain->IsUnCoupledInternally() );

	BOOST_CHECK_NO_THROW( pTrain->Append( Train::EndType::south, pSeparate, Train::EndType::north ) );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 6 );
	BOOST_CHECK( pTrain->IsValid() );

	pTrain->Reduce();
	BOOST_CHECK( pTrain->IsValid() );
	BOOST_CHECK( pTrain->IsRailed() );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 10 );
}

BOOST_FIXTURE_TEST_CASE( testTrainDeepStacked, TrainFixture )
//BOOST_FIXTURE_TEST_CASE( testTrainDeepStacked, TrainFixtureVisualDebugger )
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "Cargo.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );
	BOOST_CHECK_NO_THROW( pTrain->Rail( m_Location ) );


	// split Train to three levels:
	std::pair<std::shared_ptr<Train>,std::shared_ptr<Train>> Split = pTrain->SplitAfter( 4 );
	BOOST_REQUIRE( Split.first );
	BOOST_REQUIRE( Split.second );
	BOOST_CHECK_EQUAL( Split.first->GetNumberOfComponents(), 5 );
	BOOST_CHECK_EQUAL( Split.second->GetNumberOfComponents(), 5 );

	std::pair<std::shared_ptr<Train>,std::shared_ptr<Train>> SplitA = Split.first->SplitAfter( 2 );
	BOOST_REQUIRE( SplitA.first );
	BOOST_REQUIRE( SplitA.second );
	BOOST_CHECK_EQUAL( SplitA.first->GetNumberOfComponents(), 3 );
	BOOST_CHECK_EQUAL( SplitA.second->GetNumberOfComponents(), 2 );

	std::pair<std::shared_ptr<Train>,std::shared_ptr<Train>> SplitB = Split.second->SplitAfter( 2 );
	BOOST_REQUIRE( SplitB.first );
	BOOST_REQUIRE( SplitB.second );
	BOOST_CHECK_EQUAL( SplitB.first->GetNumberOfComponents(), 3 );
	BOOST_CHECK_EQUAL( SplitB.second->GetNumberOfComponents(), 2 );

	BOOST_CHECK( pTrain->IsValid() );
	BOOST_CHECK( !pTrain->IsUnCoupledInternally() );
	BOOST_CHECK( pTrain->IsRailed() );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 2 );

	pTrain->Reduce();

	BOOST_CHECK( pTrain->IsValid() );
	BOOST_CHECK( pTrain->IsRailed() );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 10 );
	BOOST_CHECK( !pTrain->IsUnCoupledInternally() );
}

BOOST_FIXTURE_TEST_CASE( testTrainSeparate, TrainFixture )
//BOOST_FIXTURE_TEST_CASE( testTrainSeparate, TrainFixtureVisualDebugger )
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "Cargo.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );
	BOOST_CHECK_NO_THROW( pTrain->Rail( m_Location ) );

	std::unique_ptr<PulseCounter> pPulseCounter = PulseCounter::Make();
	pTrain->JackOnUnCoupleInternal().Insert( &pPulseCounter->PlugToCountUp() );
	BOOST_CHECK_EQUAL( pPulseCounter->Counter(), 0 );
	std::pair<std::shared_ptr<Train>, std::shared_ptr<Train>> Split = pTrain->SplitAfter( 5 );
	BOOST_CHECK_EQUAL( pPulseCounter->Counter(), 0 );
	BOOST_CHECK( !pTrain->IsUnCoupledInternally() );
	BOOST_CHECK( !pTrain->Separate() );
	pTrain->Reduce();
	
	BOOST_CHECK_NO_THROW( BOOST_CHECK( pTrain->GetComponent( 5 )->Uncouple( RailRunner::EndType::south ) ) );
	BOOST_CHECK_EQUAL( pPulseCounter->Counter(), 1 );
	BOOST_CHECK( pTrain->IsUnCoupledInternally() );

	std::shared_ptr<Train> pTrain2;
	BOOST_CHECK_NO_THROW( pTrain2 = pTrain->Separate() );
	BOOST_REQUIRE( pTrain2 );
	BOOST_CHECK( !pTrain->IsUnCoupledInternally() );
	BOOST_CHECK( !pTrain2->IsUnCoupledInternally() );

	BOOST_CHECK_NO_THROW( pTrain->Append( Train::EndType::south, pTrain2, Train::EndType::north ) );
	BOOST_CHECK( !pTrain->IsUnCoupledInternally() );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 7 );
	pTrain->Reduce();
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 10 );
	BOOST_CHECK( pTrain->GetComponent( 5 )->Uncouple( RailRunner::EndType::south ) );
	BOOST_CHECK( pTrain->IsUnCoupledInternally() );
	BOOST_CHECK_EQUAL( pPulseCounter->Counter(), 2 );
	BOOST_CHECK( pTrain->GetComponent( 1 )->Uncouple( RailRunner::EndType::south ) );
	BOOST_CHECK( pTrain->IsUnCoupledInternally() );
	BOOST_CHECK_EQUAL( pPulseCounter->Counter(), 3 );

	std::shared_ptr<Train> pTrain3;
	BOOST_CHECK_NO_THROW( pTrain3 = pTrain->Separate() );
	BOOST_REQUIRE( pTrain3 );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 2 );
	BOOST_CHECK_EQUAL( pTrain3->GetNumberOfComponents(), 8 );
	BOOST_CHECK( !pTrain->IsUnCoupledInternally() );
	BOOST_CHECK( pTrain3->IsUnCoupledInternally() );
	std::shared_ptr<Train> pTrain4;
	BOOST_CHECK_NO_THROW( pTrain4 = pTrain3->Separate() );
	BOOST_REQUIRE( pTrain4 );
	BOOST_CHECK_EQUAL( pTrain3->GetNumberOfComponents(), 4 );
	BOOST_CHECK_EQUAL( pTrain4->GetNumberOfComponents(), 4 );
	BOOST_CHECK( !pTrain3->IsUnCoupledInternally() );
	BOOST_CHECK( !pTrain4->IsUnCoupledInternally() );
}

BOOST_FIXTURE_TEST_CASE( testTrainTake, TrainFixture )
//BOOST_FIXTURE_TEST_CASE( testTrainTake, TrainFixtureVisualDebugger )
// split Train to three levels and retake them:
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "Cargo.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );
	BOOST_CHECK_NO_THROW( pTrain->Rail( m_Location ) );


	// first run:
	std::pair<std::shared_ptr<Train>, std::shared_ptr<Train>> Split;
	BOOST_CHECK_NO_THROW( Split = pTrain->SplitAfter( 4 ) );
	BOOST_REQUIRE( Split.first );
	BOOST_REQUIRE( Split.second );
	std::pair<std::shared_ptr<Train>, std::shared_ptr<Train>> SplitA;
	BOOST_CHECK_NO_THROW( SplitA = Split.first->SplitAfter( 2 ) );
	BOOST_REQUIRE( SplitA.first );
	BOOST_REQUIRE( SplitA.second );
	std::pair<std::shared_ptr<Train>, std::shared_ptr<Train>> SplitB;
	BOOST_CHECK_NO_THROW( SplitB = Split.second->SplitAfter( 2 ) );
	BOOST_REQUIRE( SplitB.first );
	BOOST_REQUIRE( SplitB.second );

	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 2 );
	BOOST_CHECK_NO_THROW( Split.first->Reduce( false ) );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 3 );
	BOOST_CHECK_NO_THROW( Split.second->Reduce( false ) );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 4 );
	BOOST_CHECK_NO_THROW( SplitA.first->Reduce( false ) );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 6 );
	BOOST_CHECK_NO_THROW( SplitA.second->Reduce( false ) );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 7 );
	BOOST_CHECK_NO_THROW( SplitB.first->Reduce( false ) );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 9 );
	BOOST_CHECK_NO_THROW( SplitB.second->Reduce( false ) );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 10 );


	// second run:
	BOOST_CHECK_NO_THROW( Split = pTrain->SplitAfter( 4 ) );
	BOOST_REQUIRE( Split.first );
	BOOST_REQUIRE( Split.second );
	BOOST_CHECK_NO_THROW( SplitA = Split.first->SplitAfter( 2 ) );
	BOOST_REQUIRE( SplitA.first );
	BOOST_REQUIRE( SplitA.second );
	BOOST_CHECK_NO_THROW( SplitB = Split.second->SplitAfter( 2 ) );
	BOOST_REQUIRE( SplitB.first );
	BOOST_REQUIRE( SplitB.second );

	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 2 );
	BOOST_CHECK_NO_THROW( SplitA.first->Reduce( false ) );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 2 );
	BOOST_CHECK_NO_THROW( SplitA.second->Reduce( false ) );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 2 );
	BOOST_CHECK_NO_THROW( SplitB.first->Reduce( false ) );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 2 );
	BOOST_CHECK_NO_THROW( SplitB.second->Reduce( false ) );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 2 );
	BOOST_CHECK_NO_THROW( Split.first->Reduce( false ) );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 6 );
	BOOST_CHECK_NO_THROW( Split.second->Reduce( false ) );
	BOOST_CHECK_EQUAL( pTrain->GetNumberOfComponents(), 10 );
}

BOOST_FIXTURE_TEST_CASE( testTrainGetTip, TrainFixture )
//BOOST_FIXTURE_TEST_CASE( testTrainGetTip, TrainFixtureVisualDebugger )
// Get the tip at the respective TrainComponent:
{
	TrainFileReferenceReader reader{ *m_pScene, FixturePath() };
	BOOST_REQUIRE( reader( "Cargo.train" ) );
	std::shared_ptr<Train> pTrain = reader.GetTrain();
	BOOST_REQUIRE( pTrain );

	// don't propagate to Train end:
	BOOST_CHECK_EQUAL( &pTrain->GetComponent( 0 )->GetTipAt( RailRunner::EndType::north ).first, &pTrain->GetTipAt( RailRunner::EndType::north ).first );
	BOOST_CHECK_EQUAL( &pTrain->GetComponent( 9 )->GetTipAt( RailRunner::EndType::south ).first, &pTrain->GetTipAt( RailRunner::EndType::south ).first );
	BOOST_CHECK_NE( &pTrain->GetComponent( 4 )->GetTipAt( RailRunner::EndType::north ).first, &pTrain->GetTipAt( RailRunner::EndType::north ).first );
	BOOST_CHECK_NE( &pTrain->GetComponent( 4 )->GetTipAt( RailRunner::EndType::south ).first, &pTrain->GetTipAt( RailRunner::EndType::south ).first );
}

BOOST_AUTO_TEST_SUITE_END() // TrainCouplingTests
BOOST_AUTO_TEST_SUITE_END() // trax_tests
#endif
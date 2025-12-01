//	trax track library
//	AD 2018 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
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

#include "dim/support/DimSupportStream.h"
#include "../Test/dim/BoostTestDimensionedValuesHelpers.h"
#include "trax/rigid/TractionForceCharacteristic.h"

using namespace trax;

BOOST_AUTO_TEST_SUITE(TractionForceCharacteristicSection)

BOOST_AUTO_TEST_CASE( testSpeedStepInfinity )
{
	auto pTractionForceCharacteristic = TractionForceCharacteristic::Make();
	BOOST_CHECK( pTractionForceCharacteristic );

	pTractionForceCharacteristic->AddSpeedStep( 0_kmIh, 1.f );
	pTractionForceCharacteristic->AddSpeedStep( infinite__velocity, 1.f );

	BOOST_CHECK_EQUAL( pTractionForceCharacteristic->MaximumVelocity(), infinite__velocity );
	BOOST_CHECK_EQUAL( pTractionForceCharacteristic->MaximumPowerFraction(), decltype(Power{}/Force{}){ +infinite } );
}

BOOST_AUTO_TEST_CASE( testSpeedStepOpen )
{
	auto pTractionForceCharacteristic = TractionForceCharacteristic::Make();
	BOOST_CHECK( pTractionForceCharacteristic );

	pTractionForceCharacteristic->AddSpeedStep( 0_kmIh, 1.f );
	pTractionForceCharacteristic->AddSpeedStep( 100_kmIh, 1.f );

	BOOST_CHECK_EQUAL( pTractionForceCharacteristic->MaximumVelocity(), 100_kmIh );
	BOOST_CHECK_EQUAL( pTractionForceCharacteristic->MaximumPowerFraction(), 100_kmIh );
}

BOOST_AUTO_TEST_SUITE_END() //TractionForceCharacteristicSection
#endif // WITH_BOOST_TESTS
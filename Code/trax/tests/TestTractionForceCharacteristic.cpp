// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2018 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#include <boost/test/unit_test.hpp>

#include "dim/support/DimSupportStream.h"
#include "dim/tests/BoostTestDimensionedValuesHelpers.h"
#include "trax/TractionForceCharacteristic.h"

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
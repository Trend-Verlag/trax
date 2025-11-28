//	trax track library
//	AD 2020 
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

#include <boost/test/unit_test.hpp> // NOLINT 

#include "dim/DimensionedValues.h"
#include "dim/support/DimSupportStream.h"
#include "BoostTestDimensionedValuesHelpers.h"

using namespace dim;

BOOST_AUTO_TEST_SUITE(DimDocumentation_tests)

BOOST_AUTO_TEST_CASE( dim_basics )
{
	Length length = 2_m;
	std::cout << "Length is " << length << std::endl; // prints "Length is 2000mm"

	Length width = 300_cm;
	Length height = 6_ft + 3_in;
	BOOST_CHECK( height < length );

	Area area = width * length; // area has dimension Length^2
	Volume volume = area * height; // volume has dimension Length^3
	std::cout << "Volume is " << volume << std::endl; // prints "Volume is 11.43m3"

	Mass mass = 7.874_t;
	BOOST_CHECK( mass == 7874_kg );
	Density DensityOfIron = mass / 1_m3; // density has dimension Mass / Volume
	BOOST_CHECK( DensityOfIron == 7.874_gIcm3 );

	Length distance = 100_m;
	Time dt = 9.58_s; // Usain Bolt's 100m world record
	Velocity vUsain = distance / dt; // velocity has dimension Length / Time
	std::cout << "Usain's velocity was " << vUsain << std::endl; // prints "Usain's velocity was 10.4384mIs"
	std::cout << "Usain's velocity was " << _mIs << vUsain << std::endl; // prints "Usain's average velocity was 10.44m/s"

	Acceleration g = 9.81_mIs2; // acceleration due to gravity.
	Time tJump = sqrt(2 * distance / g);
	std::cout << "Time to fall " << _m << distance << ": " << _ms << tJump << std::endl; // prints "Time to fall 100m: 4515ms" 
	Velocity v = g * tJump;
	std::cout << "Velocity when hitting the ground: " << v << std::endl; // prints "Velocity when hitting the ground: 44.29m/s"
	std::cout << _kmIh << "Velocity when hitting the ground: " << v << std::endl; // prints "Velocity when hitting the ground: 159.46km/h"
}

BOOST_AUTO_TEST_CASE( dim_conversions )
{
	float lengthInCentimeters = 250.0f;
	Length length = _cm(lengthInCentimeters);
	std::cout << "Length is " << _m << length << std::endl; // prints "Length is 2.5m"

	Real lengthInMeters = _m(length);
	std::cout << "Length in meters: " << lengthInMeters << std::endl; // prints "Length in meters: 2.5"

}

BOOST_AUTO_TEST_SUITE_END() //DimDocumentation_tests

#endif //WITH_BOOST_TESTS
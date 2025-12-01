//	trax track library
//	AD 2013 
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
// For further information, please contact: horstmann.marc@trendverlag.de

#if defined( WITH_BOOST_TESTS )
#include <boost/test/unit_test.hpp>

#include "common/Interval.h"


BOOST_AUTO_TEST_SUITE(common_tests)

BOOST_AUTO_TEST_SUITE(Interval_tests)

BOOST_AUTO_TEST_CASE( inclusion )
{
	common::Interval<float> I1{40,80};
	common::Interval<float> I2{80,40};

	BOOST_CHECK( I1.Includes( 40 ) );
	BOOST_CHECK( !I1.Includes( 80 ) );
	BOOST_CHECK( I2.Includes( 80 ) );
	BOOST_CHECK( !I2.Includes( 40 ) );

	BOOST_CHECK( I1.Includes( I1 ) );
	BOOST_CHECK( I2.Includes( I2 ) );
	BOOST_CHECK( !I1.Includes( I2 ) );
	BOOST_CHECK( !I2.Includes( I1 ) );

	BOOST_CHECK( Intersecting( I1, I2 ) );
}

BOOST_AUTO_TEST_CASE( intersection )
{
	common::Interval<float> I1{0,80};
	common::Interval<float> I2{100,80};

	BOOST_CHECK( !Intersecting( I1, I2 ) );
	I2 = {80,100};
	BOOST_CHECK( !Intersecting( I1, I2 ) );
	I1 = {80,0};
	BOOST_CHECK( Intersecting( I1, I2 ) );
	I2 = {100,80};
	BOOST_CHECK( !Intersecting( I1, I2 ) );
}

BOOST_AUTO_TEST_SUITE_END() //Interval_tests
BOOST_AUTO_TEST_SUITE_END() //common_tests

#endif //WITH_BOOST_TESTS
// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2013 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


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
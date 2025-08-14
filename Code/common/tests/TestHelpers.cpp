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

#include "common/Helpers.h"

BOOST_AUTO_TEST_SUITE(common_tests)

BOOST_AUTO_TEST_CASE( Helpers )
{
	BOOST_CHECK_EQUAL( common::Indent(5).size(), 5u );
}

BOOST_AUTO_TEST_SUITE_END() //common_tests
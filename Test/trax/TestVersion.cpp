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
#include "trax/Version.h"

BOOST_AUTO_TEST_SUITE(Version)

BOOST_AUTO_TEST_CASE(self_consistency)
{
	BOOST_CHECK( TRAX_VERSION_MAJOR == trax::Version::Major() );
	BOOST_CHECK( TRAX_VERSION_MINOR == trax::Version::Minor() );
	BOOST_CHECK( TRAX_VERSION_PATCH == trax::Version::Patch() );

	BOOST_CHECK( trax::Version::IsAtLeast(0,0,0) );

	std::cout << "trax version: " << trax::Version::Readable() << std::endl;

	//std::cout << "sizeof float: " << sizeof(float) << std::endl;
	//std::cout << "sizeof double: " << sizeof(double) << std::endl;
	//std::cout << "sizeof long double: " << sizeof(long double) << std::endl;
	//std::cout << "sizeof Real: " << sizeof(trax::Real) << std::endl;
	//std::cout << "sizeof long: " << sizeof(long) << std::endl;
} 

BOOST_AUTO_TEST_CASE( traxVersion_Test )
{
	BOOST_CHECK( trax::CheckVersion() );
}

BOOST_AUTO_TEST_CASE(traxVersion_CommonRuntime)
{
	BOOST_CHECK(trax::CheckRuntime());
}

BOOST_AUTO_TEST_SUITE_END()
#endif // WITH_BOOST_TESTS
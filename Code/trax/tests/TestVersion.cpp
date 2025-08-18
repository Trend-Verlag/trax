// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//

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
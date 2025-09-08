//	trax track library
//	AD 2021 
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

#include "spat/Spatial.h"
#include "spat/support/SpatSupportStream.h"
#include "BoostTestSpatialHelpers.h"

using namespace spat;

BOOST_AUTO_TEST_CASE( streaming )
{
	Vector<float> v1(5434,975,476);
	Vector<float> copy = v1;

	std::stringstream strgstr;

	strgstr << v1;

	std::string result( strgstr.str() );

	std::string str( "My String" );
	std::stringstream strgstr2( str );
	std::string str2;

	strgstr2 >> str2;
}

BOOST_AUTO_TEST_CASE( streamingVectorIn )
{
	std::istringstream strgstr{ "Vector( 0, 0, 1 )" };
	Vector<float> vResult{.0,.0,1.};

	Vector<float> v1;
	strgstr >> v1;
	BOOST_CHECK( !strgstr.fail() );

	BOOST_CHECK_CLOSE_SPATIAL( v1, vResult, std::numeric_limits<float>::epsilon() );
}

BOOST_AUTO_TEST_CASE( streamingVectorIn2 )
{
	std::istringstream strgstr{ "Vector(0,0,1)" };
	Vector<float> vResult{.0,.0,1.};

	Vector<float> v1;
	strgstr >> v1;
	BOOST_CHECK( !strgstr.fail() );

	BOOST_CHECK_CLOSE_SPATIAL( v1, vResult, std::numeric_limits<float>::epsilon() );
}

BOOST_AUTO_TEST_CASE( streamingVectorIn3 )
{
	std::istringstream strgstr{ "Vector (0 ,0 ,1)" };
	Vector<float> vResult{.0,.0,1.};

	Vector<float> v1;
	strgstr >> v1;
	BOOST_CHECK( !strgstr.fail() );

	BOOST_CHECK_CLOSE_SPATIAL( v1, vResult, std::numeric_limits<float>::epsilon() );
}

BOOST_AUTO_TEST_CASE( streamingVectorRoundtrip )
{
	std::stringstream strgstr;
	Vector<float> vOriginal{ 1.0f, 0.0f, 95.0075912f };
	Vector<float> vResult;
	
	strgstr << std::setprecision(std::numeric_limits<float>::max_digits10) << vOriginal;
	strgstr >> vResult;
	BOOST_CHECK( !strgstr.fail() );

	BOOST_CHECK_EQUAL( vOriginal, vResult );
}

#endif // WITH_BOOST_TESTS

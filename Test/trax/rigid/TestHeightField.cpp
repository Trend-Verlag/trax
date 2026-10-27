//	trax track library
//	AD 2026 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//
// Copyright (c) 2026 Trend Redaktions- und Verlagsgesellschaft mbH
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
// For further information, please contact: horstmann@traxlibrary.dev

#if defined( WITH_BOOST_TESTS )
#include <boost/test/unit_test.hpp>

#include "trax/rigid/HeightField.h"

#include "dim/support/DimSupportStream.h"
#include "dim/BoostTestDimensionedValuesHelpers.h"

#include <stdexcept>
#include <vector>

using namespace dim;
using namespace spat;
using namespace trax;

namespace{
	// 3 x 2 cells -> 4 x 3 raster points
	const Vector2D<int> testExtent{ 3, 2 };
	const Vector2D<int> testRasterRect{ 4, 3 };
	const size_t testPoints = 4 * 3;

	bool IsClose( Length a, Length b ) noexcept{
		return abs( a - b ) <= epsilon__length;
	}

	std::unique_ptr<HeightField> MakeFlat( Length rasterScale = 1_m ){
		std::unique_ptr<HeightField> pHeightField = HeightField::Make();
		BOOST_REQUIRE( pHeightField );
		BOOST_REQUIRE( pHeightField->Create( testRasterRect, rasterScale ) );

		Frame<Length,One> frame;
		frame.Init();
		pHeightField->SetFrame( frame );
		return pHeightField;
	}
}

BOOST_AUTO_TEST_SUITE(HeightFieldSection)

BOOST_AUTO_TEST_CASE( testMake )
{
	std::unique_ptr<HeightField> pHeightField = HeightField::Make();
	BOOST_CHECK( pHeightField );
}

BOOST_AUTO_TEST_CASE( testCreateFail )
{
	std::unique_ptr<HeightField> pHeightField = HeightField::Make();
	BOOST_CHECK( pHeightField );

	BOOST_REQUIRE( !pHeightField->Create( { 0, 0 }, 1_m ) );
	BOOST_REQUIRE( !pHeightField->Create( { -2, 5 }, 1_m ) );
	BOOST_REQUIRE( !pHeightField->Create( { 2, 2 }, -1_m ) );
	BOOST_REQUIRE( !pHeightField->Create( { 2, 2 }, -0_m ) );

	BOOST_CHECK_THROW( pHeightField->GetHeight( { 0, 0 } ), std::out_of_range );
}

BOOST_AUTO_TEST_CASE( testCreateFlat )
{
	auto pHeightField = MakeFlat( 2_m );

	BOOST_CHECK_EQUAL( pHeightField->GetRasterExtent().dx, testExtent.dx );
	BOOST_CHECK_EQUAL( pHeightField->GetRasterExtent().dy, testExtent.dy );
	BOOST_CHECK_EQUAL( pHeightField->GetRasterScale(), 2_m );
}

BOOST_AUTO_TEST_CASE( testCreateFromLengthSamples )
{
	std::vector<Length> samples( testPoints );
	for( size_t i = 0; i < testPoints; ++i )
		samples[i] = Length{ static_cast<Real>(i) };
	std::unique_ptr<bool[]> holes = std::make_unique<bool[]>( testPoints );
	for( size_t i = 0; i < testPoints; ++i )
		holes[i] = (i == 5);

	std::unique_ptr<HeightField> pHeightField = HeightField::Make();
	BOOST_REQUIRE( pHeightField );
	BOOST_REQUIRE( pHeightField->Create( common::MakeConstSpan( samples ), common::Span<const bool>{ holes.get(), testPoints }, testRasterRect, 1_m ) );

	BOOST_CHECK_EQUAL( pHeightField->GetRasterExtent().dx, testExtent.dx );
	BOOST_CHECK_EQUAL( pHeightField->GetRasterExtent().dy, testExtent.dy );
	BOOST_CHECK_EQUAL( pHeightField->GetRasterScale(), 1_m );

	const int nCols = testExtent.dx + 1;
	for( int y = 0; y <= testExtent.dy; ++y ){
		for( int x = 0; x <= testExtent.dx; ++x ){
			const size_t i = static_cast<size_t>(y * nCols + x);
			BOOST_CHECK_EQUAL( pHeightField->GetHeight( Position2D<int>{ x, y } ), samples[i] );
			BOOST_CHECK_EQUAL( pHeightField->IsHole( Position2D<int>{ x, y } ), holes[i] );
		}
	}

	BOOST_CHECK_EQUAL( pHeightField->HeightRange().Near(), 0_m );
	BOOST_CHECK_EQUAL( pHeightField->HeightRange().Far(), Length{ static_cast<Real>(testPoints - 1) } );
}

BOOST_AUTO_TEST_CASE( testCreateFromShortSamples )
{
	std::vector<short> samples( testPoints );
	for( size_t i = 0; i < testPoints; ++i )
		samples[i] = static_cast<short>(i) - 3;
	std::unique_ptr<bool[]> holes = std::make_unique<bool[]>( testPoints );
	for( size_t i = 0; i < testPoints; ++i )
		holes[i] = false;

	std::unique_ptr<HeightField> pHeightField = HeightField::Make();
	BOOST_REQUIRE( pHeightField );
	BOOST_REQUIRE( pHeightField->Create( common::MakeConstSpan( samples ), common::Span<const bool>{ holes.get(), testPoints }, testRasterRect, 1_m, 0.5_m ) );

	const int nCols = testExtent.dx + 1;
	for( int y = 0; y <= testExtent.dy; ++y ){
		for( int x = 0; x <= testExtent.dx; ++x ){
			const size_t i = static_cast<size_t>(y * nCols + x);
			BOOST_CHECK( IsClose( pHeightField->GetHeight( Position2D<int>{ x, y } ), samples[i] * 0.5_m ) );
		}
	}

	BOOST_CHECK( IsClose( pHeightField->HeightRange().Near(), -3 * 0.5_m ) );
	BOOST_CHECK( IsClose( pHeightField->HeightRange().Far(), 8 * 0.5_m ) );
}

BOOST_AUTO_TEST_CASE( testSetGetHeightPoint ) 
{
	auto pHeightField = MakeFlat();

	pHeightField->SetHeight( Position2D<int>{ 1, 1 }, 5_m );
	BOOST_CHECK_EQUAL( pHeightField->GetHeight( Position2D<int>{ 1, 1 } ), 5_m );
	BOOST_CHECK_EQUAL( (*pHeightField)( Position2D<int>{ 1, 1 } ), 5_m );

	// corners are valid raster points
	pHeightField->SetHeight( Position2D<int>{ 0, 0 }, -1_m );
	pHeightField->SetHeight( Position2D<int>{ testExtent.dx, testExtent.dy }, 2_m );
	BOOST_CHECK_EQUAL( pHeightField->GetHeight( Position2D<int>{ 0, 0 } ), -1_m );
	BOOST_CHECK_EQUAL( pHeightField->GetHeight( Position2D<int>{ testExtent.dx, testExtent.dy } ), 2_m );

	BOOST_CHECK_EQUAL( pHeightField->HeightRange().Near(), -1_m );
	BOOST_CHECK_EQUAL( pHeightField->HeightRange().Far(), 5_m );
}

BOOST_AUTO_TEST_CASE( testSetHeightRectEdgesInclusive )
{
	auto pHeightField = MakeFlat();

	pHeightField->SetHeight( Rect<int>{ Position2D<int>{ 1, 0 }, Position2D<int>{ 2, 1 } }, 3_m );

	for( int y = 0; y <= testExtent.dy; ++y ){
		for( int x = 0; x <= testExtent.dx; ++x ){
			const bool inside = x >= 1 && x <= 2 && y >= 0 && y <= 1;
			BOOST_CHECK_EQUAL( pHeightField->GetHeight( Position2D<int>{ x, y } ), inside ? 3_m : 0_m );
		}
	}
}

BOOST_AUTO_TEST_CASE( testHeightRangeRect )
{
	auto pHeightField = MakeFlat();

	pHeightField->SetHeight( Position2D<int>{ 0, 0 }, -4_m );
	pHeightField->SetHeight( Position2D<int>{ 2, 1 }, 7_m );
	pHeightField->SetHeight( Position2D<int>{ 3, 2 }, 9_m );

	// edges inclusive: (2,1) is on the corner of the area
	common::Interval<Length> range = pHeightField->HeightRange( Rect<int>{ Position2D<int>{ 1, 1 }, Position2D<int>{ 2, 2 } } );
	BOOST_CHECK_EQUAL( range.Near(), 0_m );
	BOOST_CHECK_EQUAL( range.Far(), 7_m );

	range = pHeightField->HeightRange( Rect<int>{ Position2D<int>{ 0, 0 }, Position2D<int>{ testExtent.dx, testExtent.dy } } );
	BOOST_CHECK_EQUAL( range.Near(), -4_m );
	BOOST_CHECK_EQUAL( range.Far(), 9_m );
}

BOOST_AUTO_TEST_CASE( testHoles )
{
	auto pHeightField = MakeFlat();

	BOOST_CHECK( !pHeightField->IsHole( Position2D<int>{ 2, 1 } ) );
	pHeightField->SetHole( Position2D<int>{ 2, 1 }, true );
	BOOST_CHECK( pHeightField->IsHole( Position2D<int>{ 2, 1 } ) );
	BOOST_CHECK( !pHeightField->IsHole( Position2D<int>{ 1, 1 } ) );
	pHeightField->SetHole( Position2D<int>{ 2, 1 }, false );
	BOOST_CHECK( !pHeightField->IsHole( Position2D<int>{ 2, 1 } ) );
}

BOOST_AUTO_TEST_CASE( testGetHeightParentPoint )
{
	auto pHeightField = MakeFlat( 2_m );
	pHeightField->SetHeight( Position2D<int>{ 1, 1 }, 4_m );

	BOOST_CHECK( IsClose( pHeightField->GetHeight( Position<Length>{ 2_m, 2_m, 0_m } ), 4_m ) );
	BOOST_CHECK( IsClose( (*pHeightField)( Position<Length>{ 2_m, 2_m, 0_m } ), 4_m ) );
	BOOST_CHECK( IsClose( pHeightField->GetHeight( Position<Length>{ 0_m, 0_m, 0_m } ), 0_m ) );
}

BOOST_AUTO_TEST_CASE( testGetHeightParentPointOutside )
{
	auto pHeightField = MakeFlat( 2_m );
	pHeightField->SetHeight( Rect<int>{ Origin2D<int>,  Origin2D<int> + testExtent }, 4_m );

	BOOST_CHECK_EQUAL( pHeightField->GetHeight( Position<Length>{ -1_m, 1_m, 0_m } ), 0_m );
	BOOST_CHECK_EQUAL( pHeightField->GetHeight( Position<Length>{ 1_m, -1_m, 0_m } ), 0_m );
	BOOST_CHECK_EQUAL( pHeightField->GetHeight( Position<Length>{ 7_m, 1_m, 0_m } ), 0_m );
	BOOST_CHECK_EQUAL( pHeightField->GetHeight( Position<Length>{ 1_m, 5_m, 0_m } ), 0_m );
	BOOST_CHECK_EQUAL( (*pHeightField)( Position<Length>{ 100_m, 100_m, 0_m } ), 0_m );
}

BOOST_AUTO_TEST_CASE( testOutOfRangeThrows )
{
	auto pHeightField = MakeFlat();
	const Position2D<int> outside[] = {
		{ -1, -1 }, { -1, 0 }, { 0, -1 }, { testExtent.dx + 1, 0 }, { 0, testExtent.dy + 1 }, { testExtent.dx + 1, testExtent.dy + 1 }
	};

	for( const Position2D<int>& p : outside ){
		BOOST_CHECK_THROW( pHeightField->GetHeight( p ), std::out_of_range );
		BOOST_CHECK_THROW( (*pHeightField)( p ), std::out_of_range );
		BOOST_CHECK_THROW( pHeightField->SetHeight( p, 1_m ), std::out_of_range );
		BOOST_CHECK_THROW( pHeightField->IsHole( p ), std::out_of_range );
		BOOST_CHECK_THROW( pHeightField->SetHole( p, true ), std::out_of_range );
	}

	const Rect<int> outsideRect{ Position2D<int>{ 0, 0 }, Position2D<int>{ testExtent.dx + 1, testExtent.dy } };
	BOOST_CHECK_THROW( pHeightField->HeightRange( outsideRect ), std::out_of_range );
	BOOST_CHECK_THROW( pHeightField->SetHeight( outsideRect, 1_m ), std::out_of_range );
}

BOOST_AUTO_TEST_SUITE_END() //HeightFieldSection
#endif // WITH_BOOST_TESTS

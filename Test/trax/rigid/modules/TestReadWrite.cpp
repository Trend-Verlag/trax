// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//


#include <boost/test/unit_test.hpp>

#include "trax/support/Fixtures.h"

#include "trax/rigid/modules/Module.h"
#include "trax/rigid/modules/ModuleCollection.h"
#include "trax/rigid/modules/support/AnlReader.h"
#include "trax/rigid/modules/support/AnlWriter.h"

#include "trax/collections/TrackSystem.h"


using namespace trax;
using namespace spat;
using namespace std;


BOOST_AUTO_TEST_SUITE(trax_tests)
BOOST_AUTO_TEST_SUITE(Module_tests)

BOOST_AUTO_TEST_CASE( RoundtripTest )
{
	AnlReader reader;
	std::unique_ptr<ModuleCollection> pModuleCollection;
	BOOST_CHECK_NO_THROW( pModuleCollection = reader.ReadModuleCollection( FixtureBase::FixturePath() / "Demo.anl3" ) );
	BOOST_REQUIRE( pModuleCollection );

	int numTracks = pModuleCollection->GetFirst()->GetTrackSystem()->Count();
	BOOST_WARN_NE( numTracks, 0 );
	BOOST_CHECK_EQUAL( numTracks, 175 );
	BOOST_CHECK_NO_THROW( Write( *pModuleCollection, FixtureBase::FixturePath() / "Demo.anl4" ) );
	BOOST_CHECK_NO_THROW( { pModuleCollection = reader.ReadModuleCollection( FixtureBase::FixturePath() / "Demo.anl4" ); } );

	BOOST_REQUIRE( pModuleCollection );
	BOOST_REQUIRE_EQUAL( pModuleCollection->Count(), 1 );
	BOOST_REQUIRE( pModuleCollection->GetFirst()->GetTrackSystem() );
	BOOST_CHECK_EQUAL( numTracks, pModuleCollection->GetFirst()->GetTrackSystem()->Count() );
}

BOOST_AUTO_TEST_CASE( RoundtripTest2 )
{
	AnlReader reader;
	std::unique_ptr<ModuleCollection> pModuleCollection;
	BOOST_CHECK_NO_THROW( pModuleCollection = reader.ReadModuleCollection( FixtureBase::FixturePath() / "NewCurves.anl3" ) );
	BOOST_REQUIRE( pModuleCollection );

	int numTracks = pModuleCollection->GetFirst()->GetTrackSystem()->Count();
	BOOST_WARN_NE( numTracks, 0 );
	BOOST_CHECK_EQUAL( numTracks, 5 );
	BOOST_CHECK_NO_THROW( Write( *pModuleCollection, FixtureBase::FixturePath() / "NewCurves.anl4" ) );
	BOOST_CHECK_NO_THROW( pModuleCollection = reader.ReadModuleCollection( FixtureBase::FixturePath() / "NewCurves.anl4" ) );

	BOOST_REQUIRE( pModuleCollection );
	BOOST_REQUIRE_EQUAL( pModuleCollection->Count(), 1 );
	BOOST_REQUIRE( pModuleCollection->GetFirst()->GetTrackSystem() );
	BOOST_CHECK_EQUAL( numTracks, pModuleCollection->GetFirst()->GetTrackSystem()->Count() );
}

BOOST_AUTO_TEST_CASE( RoundtripTest3 )
{
	AnlReader reader;
	std::unique_ptr<Module> pModule;
	BOOST_CHECK_NO_THROW( pModule = reader.ReadModule( FixtureBase::FixturePath() / "Demo.anl3" ) );
	BOOST_REQUIRE( pModule );

	int numTracks = pModule->GetTrackSystem()->Count();
	BOOST_WARN_NE( numTracks, 0 );
	BOOST_CHECK_EQUAL( numTracks, 175 );
	BOOST_CHECK_NO_THROW( Write( *pModule, FixtureBase::FixturePath() / "Demo.anl4" ) );
	BOOST_CHECK_NO_THROW( pModule = reader.ReadModule( FixtureBase::FixturePath() / "Demo.anl4" ) );

	BOOST_REQUIRE( pModule );
	BOOST_REQUIRE( pModule->GetTrackSystem() );
	BOOST_CHECK_EQUAL( numTracks, pModule->GetTrackSystem()->Count() );
}

BOOST_AUTO_TEST_SUITE_END() //Module_tests
BOOST_AUTO_TEST_SUITE_END() //trax_tests
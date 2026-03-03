// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//


#include <boost/test/unit_test.hpp>

#include "trax/rigid/trains/support/FixturesTrain.h"

#include "trax/rigid/modules/Module.h"
#include "trax/rigid/modules/ModuleCollection.h"
#include "trax/rigid/modules/support/AnlReader.h"
#include "trax/rigid/modules/support/AnlWriter.h"
#include "trax/rigid/modules/support/Anl4ModuleReader.h"
#include "trax/rigid/modules/support/Anl4ModuleWriter.h"

#include "trax/collections/TrackSystem.h"


using namespace trax;
using namespace spat;
using namespace std;


BOOST_AUTO_TEST_SUITE(trax_tests)
BOOST_AUTO_TEST_SUITE(Module_tests)

BOOST_FIXTURE_TEST_CASE( RoundtripTest, TrainFixture )
{
	AnlReader reader{ *m_pScene };
	std::unique_ptr<ModuleCollection> pModuleCollection;
	BOOST_CHECK_NO_THROW( pModuleCollection = reader.ReadModuleCollection( FixturePath() / "Demo.anl3" ) );
	BOOST_REQUIRE( pModuleCollection );

	int numTracks = pModuleCollection->GetFirst()->GetTrackSystem()->Count();
	BOOST_WARN_NE( numTracks, 0 );
	BOOST_CHECK_EQUAL( numTracks, 175 );
	BOOST_CHECK_NO_THROW( Write( *pModuleCollection, FixturePath() / "Demo.anl4" ) );
	BOOST_CHECK_NO_THROW( { pModuleCollection = reader.ReadModuleCollection( FixturePath() / "Demo.anl4" ); } );

	BOOST_REQUIRE( pModuleCollection );
	BOOST_REQUIRE_EQUAL( pModuleCollection->Count(), 1 );
	BOOST_REQUIRE( pModuleCollection->GetFirst()->GetTrackSystem() );
	BOOST_CHECK_EQUAL( numTracks, pModuleCollection->GetFirst()->GetTrackSystem()->Count() );
}

BOOST_FIXTURE_TEST_CASE( RoundtripTest2, TrainFixture )
{
	AnlReader reader{ *m_pScene };
	std::unique_ptr<ModuleCollection> pModuleCollection;
	BOOST_CHECK_NO_THROW( pModuleCollection = reader.ReadModuleCollection( FixturePath() / "NewCurves.anl3" ) );
	BOOST_REQUIRE( pModuleCollection );

	int numTracks = pModuleCollection->GetFirst()->GetTrackSystem()->Count();
	BOOST_WARN_NE( numTracks, 0 );
	BOOST_CHECK_EQUAL( numTracks, 5 );
	BOOST_CHECK_NO_THROW( Write( *pModuleCollection, FixturePath() / "NewCurves.anl4" ) );
	BOOST_CHECK_NO_THROW( pModuleCollection = reader.ReadModuleCollection( FixturePath() / "NewCurves.anl4" ) );

	BOOST_REQUIRE( pModuleCollection );
	BOOST_REQUIRE_EQUAL( pModuleCollection->Count(), 1 );
	BOOST_REQUIRE( pModuleCollection->GetFirst()->GetTrackSystem() );
	BOOST_CHECK_EQUAL( numTracks, pModuleCollection->GetFirst()->GetTrackSystem()->Count() );
}

BOOST_FIXTURE_TEST_CASE( RoundtripTest3, TrainFixture )
{
	AnlReader reader{ *m_pScene };
	std::unique_ptr<Module> pModule;
	BOOST_CHECK_NO_THROW( pModule = reader.ReadModule( FixturePath() / "Demo.anl3" ) );
	BOOST_REQUIRE( pModule );

	int numTracks = pModule->GetTrackSystem()->Count();
	BOOST_WARN_NE( numTracks, 0 );
	BOOST_CHECK_EQUAL( numTracks, 175 );
	BOOST_CHECK_NO_THROW( Write( *pModule, FixturePath() / "Demo.anl4" ) );
	BOOST_CHECK_NO_THROW( pModule = reader.ReadModule( FixturePath() / "Demo.anl4" ) );

	BOOST_REQUIRE( pModule );
	BOOST_REQUIRE( pModule->GetTrackSystem() );
	BOOST_CHECK_EQUAL( numTracks, pModule->GetTrackSystem()->Count() );
}

BOOST_FIXTURE_TEST_CASE( RoundtripTest_Serializing, TrainFixture )
{
	std::unique_ptr<ModuleCollection> pModuleCollection;
	BOOST_CHECK_NO_THROW( pModuleCollection = ReadModuleCollection( FixturePath() / "DefaultLayout.anl4", *m_pScene ) );
	BOOST_REQUIRE( pModuleCollection );
	BOOST_CHECK_EQUAL( pModuleCollection->Count(), 1 );
	auto pModule = pModuleCollection->GetFirst();	
	BOOST_REQUIRE( pModule );

	int numTracks = pModule->GetTrackSystem()->Count();
	BOOST_WARN_NE( numTracks, 0 );
	BOOST_CHECK_EQUAL( numTracks, 11 );
	BOOST_CHECK_NO_THROW( WriteModuleCollection( *pModuleCollection, FixturePath() / "DefaultLayoutWrite.anl4" ) );
	BOOST_CHECK_NO_THROW( pModuleCollection = ReadModuleCollection( FixturePath() / "DefaultLayoutWrite.anl4", *m_pScene ) );

	BOOST_REQUIRE( pModuleCollection );
	BOOST_CHECK_EQUAL( pModuleCollection->Count(), 1 );
	pModule = pModuleCollection->GetFirst();	
	BOOST_REQUIRE( pModule );
	BOOST_REQUIRE( pModule->GetTrackSystem() );
	BOOST_CHECK_EQUAL( numTracks, pModule->GetTrackSystem()->Count() );
}

BOOST_AUTO_TEST_SUITE_END() //Module_tests
BOOST_AUTO_TEST_SUITE_END() //trax_tests
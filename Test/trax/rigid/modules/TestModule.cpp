// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2026 Trend Verlag;
//
//	trax track library
//	AD 2020 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#if defined( WITH_BOOST_TESTS )
#include <boost/test/unit_test.hpp>

#include "trax/collections/support/FixturesCollections.h"

#include "trax/Track.h"
#include "trax/collections/TrackSystem.h"
#include "trax/rigid/modules/Module.h"


using namespace trax;
using namespace spat;
using namespace std;

BOOST_AUTO_TEST_SUITE(trax_tests)
BOOST_AUTO_TEST_SUITE(Module_tests)

BOOST_FIXTURE_TEST_CASE( FoR_Mirrored, TrackSystemCircleFixture )
{
	std::unique_ptr<Module> pModule = Module::Make( false );
	BOOST_REQUIRE( pModule );

	pModule->Attach( m_pTrackSystem );
	pModule->SetFrame( Frame<Length,One>{ Origin3D<Length>, Ex<One>, -Ey<One>, Ez<One> } );


}

BOOST_AUTO_TEST_SUITE_END() //Module_tests
BOOST_AUTO_TEST_SUITE_END() //trax_tests
#endif // WITH_BOOST_TESTS
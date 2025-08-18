// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2023 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#include <boost/test/unit_test.hpp>

#include "trax/engines/PhysX/PhysX_Simulator.h"
#include "trax/engines/PhysX/PhysX_Scene.h"
#include "trax/Gestalt.h"
#include "trax/LogicElements.h"

#include "dim/support/DimSupportStream.h"
#include "dim/tests/BoostTestDimensionedValuesHelpers.h"
#include "spat/tests/BoostTestSpatialHelpers.h"

using namespace dim;
using namespace trax;

BOOST_AUTO_TEST_SUITE(TestBody)

BOOST_AUTO_TEST_CASE( testSimulator )
{
	PhysX_Simulator simulator{};
	PhysX_Scene scene{ simulator };

	std::cout << "simulator version: " << simulator.GetVersionString() << std::endl;

	BOOST_CHECK_EQUAL( -scene.Gravity().Length(), g );

	{
		auto pGestalt = scene.CreateGestalt( spat::Box<Length>{ 1_m, 1_m, 1_m }, 1_t );
		BOOST_CHECK( pGestalt );

		Frame<Length,One> frame, frame2;
		frame.Init();
		frame.TransportBin( 20_m );
		pGestalt->SetFrame( frame );
		pGestalt->GetFrame( frame2 );
		BOOST_CHECK_CLOSE_SPATIAL2( frame, frame2, epsilon__length, epsilon__angle );

		std::unique_ptr<PulseCounter> pCounter = PulseCounter::Make();
		pCounter->Threshold( static_cast<int>(10_s / fixed_timestep + 1_1) );
		
		pCounter->JackOnReachThreshold().Insert( &scene.PlugToStop() );
		scene.JackOnSimulationStep().Insert( &pCounter->PlugToCountUp() );

		scene.Loop();

		pGestalt->GetFrame( frame2 );
		frame.TransportBin( 0.5_1 * g * 10_s*10_s );
		BOOST_CHECK_CLOSE_SPATIAL2( frame, frame2, 1_m, epsilon__angle );

		Vector<Velocity> v;
		pGestalt->GetLinearVelocity( v );

		BOOST_CHECK_CLOSE_DIMENSION( v.dz, g * 10_s, 0.1 );
	}
}

BOOST_AUTO_TEST_CASE( testBodyExcentricPlacement )
{
	PhysX_Simulator simulator{};
	PhysX_Scene scene{ simulator };

	auto pGestalt = scene.CreateGestalt( spat::Box<Length>{ 1_m, 1_m, 1_m, 2_m, 2_m, 2_m }, 1_t );
	BOOST_CHECK( pGestalt );

	Frame<Length,One> bodyFrame;
	pGestalt->GetFrame( bodyFrame );
	spat::Position<Length> P{ 1.5_m, 1.5_m, 1.5_m };
	BOOST_CHECK_CLOSE_SPATIAL( bodyFrame.P, P, epsilon__length );
}

BOOST_AUTO_TEST_SUITE_END()

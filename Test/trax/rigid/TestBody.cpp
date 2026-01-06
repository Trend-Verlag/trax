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

#include "trax/rigid/Scene.h"
#include "trax/rigid/Simulator.h"
#include "trax/rigid/Gestalt.h"
#include "trax/LogicElements.h"
#include "trax/Plug.h"

#include "dim/support/DimSupportStream.h"
#include "dim/BoostTestDimensionedValuesHelpers.h"
#include "spat/BoostTestSpatialHelpers.h"

using namespace dim;
using namespace spat;
using namespace trax;

BOOST_AUTO_TEST_SUITE(TestBody)

BOOST_AUTO_TEST_CASE( testSimulator )
{
	std::unique_ptr<Simulator> pSimulator = Simulator::Make( Simulator::Type::PhysX );
	BOOST_REQUIRE( pSimulator );
	std::unique_ptr<Scene> pScene = Scene::Make( *pSimulator );
	BOOST_REQUIRE( pScene );

	std::cout << "simulator version: " << pSimulator->GetVersionString() << std::endl;

	BOOST_CHECK_EQUAL( -pScene->Gravity().Length(), g );

	{
		auto pGestalt = pScene->CreateGestalt( spat::Box<Length>{ 1_m, 1_m, 1_m }, 1_t );
		BOOST_CHECK( pGestalt );

		Frame<Length,One> frame, frame2;
		frame.Init();
		frame.TransportBin( 20_m );
		pGestalt->SetFrame( frame );
		pGestalt->GetFrame( frame2 );
		BOOST_CHECK_CLOSE_SPATIAL2( frame, frame2, epsilon__length, epsilon__angle );

		std::unique_ptr<PulseCounter> pCounter = PulseCounter::Make();
		pCounter->Threshold( static_cast<int>(10_s / fixed_timestep + 1_1) );
		
		pCounter->JackOnReachThreshold().Insert( &pScene->PlugToStop() );
		pScene->JackOnSimulationStep().Insert( &pCounter->PlugToCountUp() );

		pScene->Simulate();

		pGestalt->GetFrame( frame2 );
		frame.TransportBin( 0.5_1 * g * 10_s*10_s );
		BOOST_CHECK_CLOSE_SPATIAL2( frame, frame2, 1_m, epsilon__angle );

		Vector<Velocity> v;
		pGestalt->GetLinearVelocity( v );

		BOOST_CHECK_CLOSE_DIMENSION( v.dz, g * 10_s, 0.1 );
	}
}

BOOST_AUTO_TEST_CASE( testBodyMassAndInertia )
// test wether mass and inertia are set correctly when setting one of them
{
	std::unique_ptr<Simulator> pSimulator = Simulator::Make( Simulator::Type::PhysX );
	BOOST_REQUIRE( pSimulator );
	std::unique_ptr<Scene> pScene = Scene::Make( *pSimulator );
	BOOST_REQUIRE( pScene );

	Box<Length> box{ 3_m, 3_m, 3_m };
	Mass mass1 = 2_t;
	auto pGestalt = pScene->CreateGestalt( box, mass1 );
	BOOST_CHECK( pGestalt );

	BOOST_CHECK_CLOSE_FRACTION_DIMENSION( pGestalt->GetMass(), mass1, epsilon__mass );
	BOOST_CHECK_CLOSE_SPATIAL( pGestalt->PrincipalMomentsOfInertia(), mass1 * SpecificInertiaTensorFor(box), epsilon__momentOfInertia );

	Mass mass2 = 3_t;
	pGestalt->SetMass( mass2 );
	BOOST_CHECK_CLOSE_FRACTION_DIMENSION( pGestalt->GetMass(), mass2, epsilon__mass );
	BOOST_CHECK_CLOSE_SPATIAL( pGestalt->PrincipalMomentsOfInertia(), mass1 * SpecificInertiaTensorFor(box), epsilon__momentOfInertia );
}

BOOST_AUTO_TEST_CASE( testBodyMassAndInertiaPose )
// test wether 
{
	std::unique_ptr<Simulator> pSimulator = Simulator::Make( Simulator::Type::PhysX );
	BOOST_REQUIRE( pSimulator );
	std::unique_ptr<Scene> pScene = Scene::Make( *pSimulator );
	BOOST_REQUIRE( pScene );

	Box<Length> box{ 3_m, 3_m, 3_m };
	Mass mass = 2_t;
	auto pGestalt = pScene->CreateGestalt( box, mass );
	BOOST_CHECK( pGestalt );

	Frame<Length,One> CoMPose{ {5_m, 5_m, 5_m} };
	CoMPose.Rotate( Normalize(Ex<One> + Ey<One> + Ez<One>).second, pi / 4 );
	pGestalt->CenterOfMassLocalPose( CoMPose );
	BOOST_CHECK_CLOSE_SPATIAL( pGestalt->CenterOfMass(), CoMPose.P, epsilon__length );

	pGestalt->CenterOfMass( Origin3D<Length> );
	BOOST_CHECK_CLOSE_SPATIAL( pGestalt->CenterOfMassLocalPose().P, Origin3D<Length>, epsilon__length );
}

BOOST_AUTO_TEST_CASE( testBodyExcentricPlacement )
{
	std::unique_ptr<Simulator> pSimulator = Simulator::Make( Simulator::Type::PhysX );
	BOOST_REQUIRE( pSimulator );
	std::unique_ptr<Scene> pScene = Scene::Make( *pSimulator );
	BOOST_REQUIRE( pScene );

	Box<Length> box{ 1_m, 1_m, 1_m, 2_m, 2_m, 2_m };
	Mass mass = 2_t;
	auto pGestalt = pScene->CreateGestalt( box, mass );
	BOOST_CHECK( pGestalt );

	Frame<Length,One> bodyFrame;
	pGestalt->GetFrame( bodyFrame );
	Position<Length> P{ 1.5_m, 1.5_m, 1.5_m };
	BOOST_CHECK_CLOSE_SPATIAL( bodyFrame.P, P, epsilon__length );
	BOOST_CHECK_EQUAL( pGestalt->CenterOfMassLocalPose().ToParent(pGestalt->CenterOfMass()), Origin3D<Length> );
	BOOST_CHECK_EQUAL( bodyFrame.ToParent(pGestalt->CenterOfMassLocalPose().ToParent(pGestalt->CenterOfMass())), P );
}

BOOST_AUTO_TEST_CASE( testBodyParameters )
{
	std::unique_ptr<Simulator> pSimulator = Simulator::Make( Simulator::Type::PhysX );
	BOOST_REQUIRE( pSimulator );
	std::unique_ptr<Scene> pScene = Scene::Make( *pSimulator );
	BOOST_REQUIRE( pScene );

	Box<Length> box{ 1_m, 2_m, 3_m };
	Mass mass = 3_t;
	auto pGestalt = pScene->CreateGestalt( box, mass );
	BOOST_CHECK( pGestalt );

	BOOST_CHECK_EQUAL( pGestalt->GetMass(), mass );
	BOOST_CHECK_CLOSE_SPATIAL( pGestalt->PrincipalMomentsOfInertia(), mass * SpecificInertiaTensorFor(box), epsilon__momentOfInertia );

	BOOST_CHECK_EQUAL( pGestalt->CenterOfMass(), box.Center() );
	BOOST_CHECK_EQUAL( pGestalt->CenterOfMassLocalPose(), (Identity<Length,One>) );
}

BOOST_AUTO_TEST_CASE( testBodyParameters2 )
{
	std::unique_ptr<Simulator> pSimulator = Simulator::Make( Simulator::Type::PhysX );
	BOOST_REQUIRE( pSimulator );
	std::unique_ptr<Scene> pScene = Scene::Make( *pSimulator );
	BOOST_REQUIRE( pScene );

	Position<Length> P1{ 10_m, 0_m, 0_m };
	Position<Length> P2{ 0_m, 0_m, 0_m };
	Mass mass1 = 2_t;
	Mass mass2 = 4_t;

	std::unique_ptr<Gestalt> pGestalt = pScene->CreateGestalt();
	std::unique_ptr<GeomCuboid> pCuboid1 = pScene->CreateGeomCuboid();
	std::unique_ptr<GeomCuboid> pCuboid2 = pScene->CreateGeomCuboid();

	pCuboid1->Diagonal( { 1_m, 1_m, 1_m } );
	pCuboid2->Diagonal( { 2_m, 2_m, 2_m } );
	pCuboid1->SetFrame(Frame<Length, One>{ P1 });
	pCuboid2->SetFrame(Frame<Length, One>{ P2 });

	int idx1 = pGestalt->Attach( std::move(pCuboid1), mass1 );
	int idx2 = pGestalt->Attach( std::move(pCuboid2), mass2 );

	Frame<Length, One> bodyFrame;
	pGestalt->GetFrame( bodyFrame );
	BOOST_CHECK_CLOSE_SPATIAL2( bodyFrame, (Identity<Length, One>), epsilon__length, epsilon__angle );

	BOOST_CHECK_EQUAL( pGestalt->GetMass(), mass1 + mass2 );
	Position<Length> expectedCOM = Origin3D<Length> + (mass1 * (P1 - Origin3D<Length>) + mass2 * (P2 - Origin3D<Length>)) / (mass1 + mass2);
	BOOST_CHECK_CLOSE_SPATIAL( pGestalt->CenterOfMass(), expectedCOM, epsilon__length );

	spat::Vector<MomentOfInertia> I = pGestalt->PrincipalMomentsOfInertia();
	BOOST_CHECK( I.dx < I.dy);
	BOOST_CHECK( I.dx < I.dz);
	BOOST_CHECK_EQUAL( I.dy, I.dz );

	Frame<Length,One> CoMPose = pGestalt->CenterOfMassLocalPose();
	BOOST_CHECK_CLOSE_SPATIAL( CoMPose.P, expectedCOM, epsilon__length);

	// CoM is local to body:
	bodyFrame = Frame<Length, One>{ { 10_m, 10_m, 10_m } };
	bodyFrame.Rotate( Normalize(Ex<One> + Ez<One>).second, pi / 4 );
	pGestalt->SetFrame( bodyFrame );
	BOOST_CHECK_CLOSE_SPATIAL( pGestalt->CenterOfMass(), expectedCOM, epsilon__length );
	Frame<Length,One> CoMPose2 = pGestalt->CenterOfMassLocalPose();
	BOOST_CHECK_CLOSE_SPATIAL( CoMPose, CoMPose2, epsilon__length );

	Position<Length> COMWord{ bodyFrame.P + (expectedCOM - Origin3D<Length>).Length() * bodyFrame.T };
	BOOST_CHECK_EQUAL( bodyFrame.ToParent(pGestalt->CenterOfMass()), COMWord );
}


BOOST_AUTO_TEST_SUITE_END()

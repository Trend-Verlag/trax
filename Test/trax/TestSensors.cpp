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

#include "trax/Plug.h"
#include "trax/Jack.h"
#include "trax/support/TraxSupportStream.h"
#include "trax/support/Fixtures.h"

using namespace trax;
using namespace spat;
using namespace std;


BOOST_AUTO_TEST_SUITE(trax_tests)
BOOST_AUTO_TEST_SUITE(Sensor_tests)

BOOST_FIXTURE_TEST_CASE( doubleplug_test, SensorFixture )
	// assigning a plug to a jack 2x should not work
{	
	std::cout << "sensortest!" << std::endl;
	m_pSensor->JackOnTrigger().Insert( &m_pPulseCounter->PlugToCountUp() );
	Plug* pPlug = m_pSensor->JackOnTrigger().GetPlug();
	BOOST_CHECK( pPlug );
	BOOST_CHECK( !pPlug->JackOnPulse().Plugged() );
}

BOOST_FIXTURE_TEST_CASE( reattach_test, SensorFixture )
	// attaching a sensor that is already attached.
{		 
	m_pTrack1->Attach( m_pSensor, TrackLocation( m_pTrack1->GetLength() / 2, true ) );
	m_pTrack1->Attach( m_pSensor, TrackLocation( m_pTrack1->GetLength() / 4, false ) );

	TrackLocation tl;
	m_pTrack1->Attached( *m_pSensor.get(), &tl );
	BOOST_CHECK_EQUAL( tl.parameter, m_pTrack1->GetLength() / 4 );
	BOOST_CHECK_EQUAL( tl.orientation, Orientation::Value::anti );

	m_pTrack2->Attach( m_pSensor, TrackLocation( m_pTrack1->GetLength() / 4, false ) );
	BOOST_CHECK( !m_pTrack1->Attached( *m_pSensor.get() ) );
	BOOST_CHECK_EQUAL( m_pSensor->TrackAttached(), m_pTrack2.get() );
}

BOOST_FIXTURE_TEST_CASE( trigger_test1, SensorFixture )
	// going to and fro over a sensor, location and sensor positive orientated
{		 
	m_pTrack1->Attach( m_pSensor, TrackLocation( m_pTrack1->GetLength() / 2, true ) );
	m_Location.PutOn( m_pTrack1 , TrackLocation( m_pTrack1->GetLength() / 4, true ) );

	m_Location.MoveToEnd( Orientation::Value::para, m_pEvent.get() );
	BOOST_CHECK_EQUAL( m_pPulseCounter->Counter(), 1 );

	m_Location.MoveToEnd( Orientation::Value::anti, m_pEvent.get() );
	BOOST_CHECK_EQUAL( m_pPulseCounter->Counter(), 1 );

	m_Location.MoveToEnd( Orientation::Value::para, m_pEvent.get() );
	BOOST_CHECK_EQUAL( m_pPulseCounter->Counter(), 2 );
}

BOOST_FIXTURE_TEST_CASE( trigger_test2, SensorFixture )
	// going to and fro over a sensor, sensor negative orientated
{		 
	m_pTrack1->Attach( m_pSensor, TrackLocation( m_pTrack1->GetLength() / 2, false ) );
	m_Location.PutOn( m_pTrack1 , TrackLocation( m_pTrack1->GetLength() / 4, true ) );

	m_Location.MoveToEnd( Orientation::Value::para, m_pEvent.get() );
	BOOST_CHECK_EQUAL( m_pPulseCounter->Counter(), 0 );

	m_Location.MoveToEnd( Orientation::Value::anti, m_pEvent.get() );
	BOOST_CHECK_EQUAL( m_pPulseCounter->Counter(), 1 );

	m_Location.MoveToEnd( Orientation::Value::para, m_pEvent.get() );
	BOOST_CHECK_EQUAL( m_pPulseCounter->Counter(), 1 );
}

BOOST_FIXTURE_TEST_CASE( trigger_test3, SensorFixture )
	// going to and fro over a sensor, location negative orientated
{		 
	m_pTrack1->Attach( m_pSensor, TrackLocation( m_pTrack1->GetLength() / 2, true ) );
	m_Location.PutOn( m_pTrack1 , TrackLocation( m_pTrack1->GetLength() / 4, false ) );

	m_Location.MoveToEnd( Orientation::Value::anti, m_pEvent.get() );
	BOOST_CHECK_EQUAL( m_pPulseCounter->Counter(), 1 );

	m_Location.MoveToEnd( Orientation::Value::para, m_pEvent.get() );
	BOOST_CHECK_EQUAL( m_pPulseCounter->Counter(), 1 );

	m_Location.MoveToEnd( Orientation::Value::anti, m_pEvent.get() );
	BOOST_CHECK_EQUAL( m_pPulseCounter->Counter(), 2 );
}

BOOST_FIXTURE_TEST_CASE( trigger_test_movedirection, SensorFixture )
	// going backwards over a sensor but in right direction
{		 
	m_pTrack1->Attach( m_pSensor, TrackLocation( m_pTrack1->GetLength() / 2, true ) );
	m_Location.PutOn( m_pTrack1 , TrackLocation( m_pTrack1->GetLength() / 4, false ) );

	m_Location.MoveToEnd( Orientation::Value::anti, m_pEvent.get() );
	BOOST_CHECK_EQUAL( m_pPulseCounter->Counter(), 1 );
}

BOOST_FIXTURE_TEST_CASE( trigger_test4, SensorFixture )
	// resetting a sensors direction.
{		 
	m_pTrack1->Attach( m_pSensor, TrackLocation( m_pTrack1->GetLength() / 2, true ) );
	m_Location.PutOn( m_pTrack1 , TrackLocation( m_pTrack1->GetLength() / 4, true ) );

	m_Location.MoveToEnd( Orientation::Value::para, m_pEvent.get() );
	BOOST_CHECK_EQUAL( m_pPulseCounter->Counter(), 1 );

	m_pTrack1->Attach( m_pSensor, TrackLocation( m_pTrack1->GetLength() / 2, false ) );
	m_Location.PutOn( m_pTrack1 , TrackLocation( m_pTrack1->GetLength() / 4, true ) );
	m_Location.MoveToEnd( Orientation::Value::para, m_pEvent.get() );
	BOOST_CHECK_EQUAL( m_pPulseCounter->Counter(), 1 );

	m_Location.MoveToEnd( Orientation::Value::anti, m_pEvent.get() );
	BOOST_CHECK_EQUAL( m_pPulseCounter->Counter(), 2 );
}

BOOST_FIXTURE_TEST_CASE( twosensors_trigger_right_order, SensorFixture )
	// going over two sensors assuming they are triggered in the correct order. 
{		 
	std::shared_ptr<Sensor> pSensor2 = Sensor::Make();
	pSensor2->JackOnTrigger().Insert( &m_pPulseCounter->PlugToCountDown() );

	m_pTrack1->Attach( m_pSensor, { m_pTrack1->GetLength() / 2, Orientation::Value::para } );
	m_pTrack1->Attach( pSensor2, { m_pTrack1->GetLength() / 2 + 1_m, Orientation::Value::para } );
	m_Location.PutOn( m_pTrack1 , { m_pTrack1->GetLength() / 4, Orientation::Value::para } );

	std::unique_ptr<trax::PulseCounter>	pPulseCounter2 = PulseCounter::Make();
	m_pPulseCounter->Threshold( -1 );
	m_pPulseCounter->JackOnReachThreshold().Insert( &pPulseCounter2->PlugToCountUp() );

	m_Location.MoveToEnd( Orientation::Value::para, m_pEvent.get() );
	BOOST_CHECK_EQUAL( m_pPulseCounter->Counter(), 0 );
	BOOST_CHECK_EQUAL( pPulseCounter2->Counter(), 0 );
}

BOOST_FIXTURE_TEST_CASE( twosensors_trigger_right_order2, SensorFixture )
	// going over two sensors against the track direction, assuming they are triggered in the correct order. 
{		 
	std::shared_ptr<Sensor> pSensor2 = Sensor::Make();
	pSensor2->JackOnTrigger().Insert( &m_pPulseCounter->PlugToCountDown() );

	m_pTrack1->Attach( m_pSensor, { m_pTrack1->GetLength() / 2, Orientation::Value::anti } );
	m_pTrack1->Attach( pSensor2, { m_pTrack1->GetLength() / 2 + 1_m, Orientation::Value::anti } );
	m_Location.PutOn( m_pTrack1 , { 3*m_pTrack1->GetLength() / 4, Orientation::Value::anti } );

	std::unique_ptr<trax::PulseCounter>	pPulseCounter2 = PulseCounter::Make();
	m_pPulseCounter->Threshold( 1 );
	m_pPulseCounter->JackOnReachThreshold().Insert( &pPulseCounter2->PlugToCountUp() );

	m_Location.MoveToEnd( Orientation::Value::para, m_pEvent.get() );
	BOOST_CHECK_EQUAL( m_pPulseCounter->Counter(), 0 );
	BOOST_CHECK_EQUAL( pPulseCounter2->Counter(), 0 );
}

BOOST_AUTO_TEST_SUITE_END() //Sensor_tests
BOOST_AUTO_TEST_SUITE_END() //trax_tests

#endif // WITH_BOOST_TESTS
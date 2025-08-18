// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2013 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/Location.h"
#include "trax/Curve.h"
#include "trax/Sensor.h"
#include "trax/LogicElements.h"

#include <filesystem>

struct FixtureBase{

	static std::filesystem::path FixturePath();

};

/// \brief One quater circle arc curved track of length 100_m*pi/2 (and radius 100_m)
struct TrackFixture{
	TrackFixture();
	~TrackFixture();

	std::shared_ptr<trax::TrackBuilder> m_pTrack;	///< track unattached to system
};

struct SwichFixture{
	SwichFixture();
	~SwichFixture();

	std::shared_ptr<trax::TrackBuilder> m_pTrack1;	
	std::shared_ptr<trax::TrackBuilder> m_pTrack2;	
	std::shared_ptr<trax::TrackBuilder> m_pTrack3;	
};


/// \brief arc Track with Location in the middle
struct TrackAndLocation : TrackFixture
{
	TrackAndLocation();

	trax::Location m_LocationOnTrack;		///< location in middle of the track
};


// Circle with radius 10 around the origin made from 4 tracks
// connected to each other.
struct TrackCircle : FixtureBase{
	TrackCircle();
	~TrackCircle();

	std::shared_ptr<trax::TrackBuilder>	m_pTrack1;
	std::shared_ptr<trax::TrackBuilder>	m_pTrack2;
	std::shared_ptr<trax::TrackBuilder>	m_pTrack3;
	std::shared_ptr<trax::TrackBuilder>	m_pTrack4;

	std::shared_ptr<trax::ArcP> m_pArc1;
	std::shared_ptr<trax::ArcP> m_pArc2;
	std::shared_ptr<trax::ArcP> m_pArc3;
	std::shared_ptr<trax::ArcP> m_pArc4;

	trax::Location m_Location;

	trax::Length R;
};	
//
//struct TrackCircleIntegrated : TrackCircle{
//	TrackCircleIntegrated() : TrackCircle(trax::Curve::ArcIntegrated){}
//};

struct SensorFixture : TrackCircle{
	SensorFixture();
	~SensorFixture();

	std::shared_ptr<trax::Sensor>			m_pSensor;
	std::unique_ptr<trax::PulseCounter>		m_pPulseCounter;
	std::unique_ptr<trax::Event>			m_pEvent;
};


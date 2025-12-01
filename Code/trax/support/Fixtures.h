//	trax track library
//	AD 2013 
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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/Location.h"
#include "trax/Curve.h"
#include "trax/Sensor.h"
#include "trax/Signal.h"
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

struct ThreeTracksInALineFixture{
	ThreeTracksInALineFixture();
	~ThreeTracksInALineFixture();

	std::shared_ptr<trax::TrackBuilder> m_pTrack1;	
	std::shared_ptr<trax::TrackBuilder> m_pTrack2;	
	std::shared_ptr<trax::TrackBuilder> m_pTrack3;	
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

class TestSignalTarget : public trax::SignalTarget{
public:

	// SignalTarget:
	bool Notify( const trax::Signal& /*signal*/, dim::Length /*distance*/ ) override{
		m_bNotifyFlag = true;
		return true;
	}


	void Active( bool bActive ) noexcept override{
		m_bActive = bActive;
	}

	bool Active() const noexcept override{
		return m_bActive;
	}


	bool m_bNotifyFlag = false;
private:
	bool m_bActive = true;
};



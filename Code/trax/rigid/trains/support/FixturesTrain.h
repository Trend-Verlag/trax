//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
// 
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


#include "trax/support/Fixtures.h"

#include "trax/rigid/StaticTrack.h"
#include "trax/rigid/Simulator.h"
#include "trax/rigid/Scene.h"

namespace trax{
	struct TrackSystem;
}

struct TrainFixture : FixtureBase{
	TrainFixture( bool bVisualDebugger = false );
	~TrainFixture();

	std::unique_ptr<trax::Simulator> m_pSimulator;
	std::unique_ptr<trax::Scene> m_pScene;

	std::shared_ptr<trax::StaticTrack>	m_pTrack1;
	std::shared_ptr<trax::StaticTrack>	m_pTrack2;
	std::shared_ptr<trax::StaticTrack>	m_pTrack3;
	std::shared_ptr<trax::StaticTrack>	m_pTrack4;
	std::shared_ptr<trax::StaticTrack>	m_pTrack5;
	std::shared_ptr<trax::StaticTrack>	m_pTrack6;

	std::shared_ptr<trax::ArcP> m_pArc1;
	std::shared_ptr<trax::ArcP> m_pArc2;
	std::shared_ptr<trax::ArcP> m_pArc3;
	std::shared_ptr<trax::ArcP> m_pArc4;
	std::shared_ptr<trax::Line> m_pLine1;

	trax::Location m_Location;

	trax::Length R;
};

struct TrainFixtureVisualDebugger : TrainFixture{
	TrainFixtureVisualDebugger() : TrainFixture( true ){}
};


struct MultiTrackSystemFixture : FixtureBase
{
	MultiTrackSystemFixture( bool bVisualDebugger = false );
	~MultiTrackSystemFixture();

	void BuildFixture( int nTrackCollections );

	std::unique_ptr<trax::Simulator> m_pSimulator;
	std::unique_ptr<trax::Scene> m_pScene;
	std::shared_ptr<trax::TrackSystem> m_pTrackSystem;
};

struct MultiTrackSystemFixtureVisualDebugger : MultiTrackSystemFixture
{
	MultiTrackSystemFixtureVisualDebugger() : MultiTrackSystemFixture( true ){}
};
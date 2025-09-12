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

#include "trax/collections/support/FixturesCollections.h"

#include "dim/DimensionedValues.h"

using namespace trax;
using namespace spat;

///////////////////////////////////////
TrackSystemFixture::TrackSystemFixture()
		: m_pTrackSystem	{ trax::TrackSystem::Make() }
{
	m_pTrackSystem->CreateCollection();
}

TrackSystemFixture::~TrackSystemFixture()
{
	if( m_pTrackSystem )
		m_pTrackSystem->DeCoupleAll();
}
///////////////////////////////////////
TrackSystemCircleFixture::TrackSystemCircleFixture()
{
	m_pTrackSystem->Add( m_pTrack1 );
	m_pTrackSystem->Add( m_pTrack2 );
	m_pTrackSystem->Add( m_pTrack3 );
	m_pTrackSystem->Add( m_pTrack4 );
}

TrackSystemCircleFixture::~TrackSystemCircleFixture()
{
}
///////////////////////////////////////
TrackAndTrackSystem::TrackAndTrackSystem()
{
	spat::Vector<Angle> rotator( 1.0f, 1.0f, 1.0f );
	rotator.Normalize();
	rotator *= pi/2;

	m_FrameA.Init();
	m_FrameA.Rotate( rotator );
	m_FrameB.Init();
	m_FrameB.Rotate( -rotator );
}
///////////////////////////////////////


// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2023 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/Fixtures.h"

#include "trax/collections/TrackSystem.h"
#include "spat/Frame.h"

struct TrackSystemFixture
{
	TrackSystemFixture();
	~TrackSystemFixture();

	std::shared_ptr<trax::TrackSystem> m_pTrackSystem;
};

struct TrackSystemCircleFixture: TrackSystemFixture,
								 TrackCircle
{
	TrackSystemCircleFixture();
	~TrackSystemCircleFixture();


};

/// \brief Unattached (to TrackSystem) arc Track with Location in the middle
/// and a TrackSystem.
struct TrackAndTrackSystem : TrackAndLocation,
							 TrackSystemFixture
{
	TrackAndTrackSystem();

	spat::Frame<trax::Length,trax::One> m_FrameA, m_FrameB;	///< two transformation inverse to each other
};


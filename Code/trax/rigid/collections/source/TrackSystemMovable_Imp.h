//	trax track library
//	AD 2025 
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

#include "trax/collections/source/TrackSystem_Imp.h"

namespace trax{

	struct MovableTrackAutoConnecting;

	class TrackSystemMovable_Imp : public TrackSystem_Imp
	{
	public:
		static std::unique_ptr<TrackSystem> Make() noexcept;

		TrackSystemMovable_Imp( 
			std::unique_ptr<TrackCollectionContainer> pTrackCollectionContainer,
			std::unique_ptr<ConnectorCollection> pConnectorCollection ) noexcept;


		IDType Add( std::shared_ptr<TrackBuilder> pTrack ) override;

		bool Remove( TrackBuilder* pTrack, bool zeroIDs = false ) override;

		void Clear() noexcept override;


		// Simulated:
		void Update( Time dt ) noexcept override;

	private:
		std::vector<std::shared_ptr<MovableTrack>>					m_MovableTracks;
		std::vector<std::shared_ptr<MovableTrackAutoConnecting>>	m_MovableTracksAutoConnecting;
	};

}
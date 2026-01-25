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

#include "../MovableTrack.h"
#include "trax/source/Track_Imp.h"

namespace trax{

	class MovableTrack_Imp :	public virtual MovableTrack,
								public virtual Track_Imp
							 
	{
	public:
		MovableTrack_Imp() noexcept;

		// Pose_Imp:
		void PropagateAbsoluteFrameToClients() noexcept override;


		// Track:
		TrackType GetTrackType() const noexcept override;

		std::shared_ptr<const MovableTrack> GetMovableTrack() const noexcept override;

		std::shared_ptr<MovableTrack> GetMovableTrack() noexcept override;


		// Inherited via MovableTrack:
		void SetBody( std::shared_ptr<Body> pBody ) noexcept override;

		std::shared_ptr<Body> GetBody() const noexcept override;

		void UpdateTrackPose() noexcept override;

		bool IsMoving() const noexcept override;
	private:
		spat::Frame<Length,One>	m_RelativePose;
		std::shared_ptr<Body>	m_pBody;
		bool 					m_bFramePropagationToBodyOnSetFrame;
	};

}
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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/Track.h"

namespace trax{

	struct Body;


	/// \brief Track that can get attached to a moving body.
	struct MovableTrack : virtual TrackBuilder{

		/// \brief Makes a MovableTrack object.
		static dclspc std::shared_ptr<MovableTrack> Make() noexcept;


		/// \brief Sets a body the track is attached to (if any).
		virtual void SetBody( std::shared_ptr<const Body> pBody ) noexcept = 0;


		/// \returns Returns the body a track is assigned to or nullptr.
		virtual std::shared_ptr<const Body> GetBody() const noexcept = 0;


		/// \brief Updates the track's position from the moving body.
		virtual void UpdateTrackPose() = 0;


		/// \returns true if the track is actually in motion.
		virtual bool IsMoving() const noexcept = 0;
	};

}
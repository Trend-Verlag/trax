//	trax track library
//	AD 2026 
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

#include "trax/SectionTrack.h"


namespace trax{

	struct Shape;

	struct CollidableTrack : virtual SectionTrack
	{

		/// \brief Sets a shape the track is attached to (if any).
		///
		/// From the attachment of a shape on, setting the frame for a track will
		/// automatically adjust the shape's frame to keep the relative pose.
		/// To update the track's position from the shape, call UpdateTrackPose(),
		/// after the shape was located.
		/// \param pShape A shared pointer to the shape the track should be attached to.
		virtual void SetShape( std::shared_ptr<Shape> pShape ) noexcept = 0;


		/// \returns Returns the shape a track is assigned to or nullptr.
		virtual std::shared_ptr<Shape> GetShape() const noexcept = 0;


		/// \brief Updates the track's position from the shape.
		virtual void UpdateTrackPose() noexcept = 0;
	};


}


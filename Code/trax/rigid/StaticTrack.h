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

#include "trax/SectionTrack.h"

namespace trax{

	struct Material;
	struct Shape;
	struct Scene;

	struct StaticTrack : virtual SectionTrack{

		/// \brief Makes a StaticTrack object.
		static dclspc std::shared_ptr<StaticTrack> Make( const Scene& scene ) noexcept;
		

		/// \brief Sets the physical material for the static track.
		virtual void SetMaterial( const Material& material ) noexcept = 0;


		/// \brief Gets the physical material for the static track.
		virtual const Material& GetMaterial() const noexcept = 0;


		/// \returns Returns the shape of the track.
		virtual const Shape& GetShape() const noexcept = 0;
	};


	/// \brief Get an axis aligned box that completely contains the track.
	/// 
	/// It is guaranteed that the section points are contained in the box as well
	/// as every position returned by the Track::Transition() methods.
	/// 
	/// \param track Track to calculate a box for.
	/// \param pSection Optional section to calculate the box for. If nullptr, the box will be calculated for the center line..
	/// \return An axis aligned box containing the track.
	dclspc spat::Box<Length> GetBoxFor( const TrackBuilder& track, const Section* pSection = nullptr );

} // namespace trax

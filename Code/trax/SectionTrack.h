//	trax track library
//	AD 2024 
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

#include "Track.h"

namespace trax{

	struct Section;

	struct SectionTrack : virtual TrackBuilder{

		/// \brief Makes a SectionTrack object.
		static dclspc std::shared_ptr<SectionTrack> Make() noexcept;


		using TrackBuilder::Attach;


		/// \brief Sets a Section describing the lateral geometry as a profile.
		///	\param pSection Pointer to the Section object to use.
		virtual void Attach( std::shared_ptr<const Section> pSection ) noexcept = 0;


		/// \brief Detaches the Section from the track.
		/// \returns A pointer to the previous attached Section or nullptr if none.
		virtual std::shared_ptr<const Section> DetachSection() noexcept = 0;


		/// \returns A pointer to the attached Section or nullptr if none.
		virtual std::shared_ptr<const Section> GetSection() const noexcept = 0;
	};


}

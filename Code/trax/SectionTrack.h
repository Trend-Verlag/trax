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

#include <memory>

namespace trax{

	struct Section;


	/// \brief TrackBuilder interface to build tracks with sections.
	///
	/// The SectionTrack holds one or more cross-sections (Section) that
	/// define the profile of the track laterally to its center curve.
	/// From it can be derived the full 3D geometry of the track.
	struct SectionTrack : virtual TrackBuilder{

		/// \brief Makes a SectionTrack object.
		static dclspc std::shared_ptr<SectionTrack> Make() noexcept;


		/// \brief Maximum number of sections allowed.
		static const int maxCntSections = std::numeric_limits<int>::max();


		using TrackBuilder::Attach;


		/// \brief Sets a Section describing the lateral geometry as a profile.
		///	\param pSection Pointer to the Section object to use.
		/// \returns The index of the attached section or < 0 on failure.
		/// \throws std::invalid_argument if pSection is nullptr or does not contain 
		/// at least 2 section points.
		/// \throws std::length_error if the maximum number of sections is exceeded.
		virtual int Attach( std::shared_ptr<const Section> pSection ) = 0;


		/// \brief Detaches the Section from the track.
		/// \param index Index of the section to set [0,CntSections()-1].
		/// \returns A pointer to the previous attached Section or nullptr if none.
		virtual std::shared_ptr<const Section> DetachSection( int index = 0 ) noexcept = 0;


		/// \param index Index of the section to set [0,CntSections()-1].
		/// \returns A pointer to the attached Section or nullptr if none.
		virtual std::shared_ptr<const Section> GetSection( int index = 0 ) const noexcept = 0;


		/// \returns The number of sections, that are attached to the track.
		virtual int CntSections() const noexcept = 0;
	};


}

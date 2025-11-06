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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/SectionTrack.h"
#include "Track_Imp.h"

namespace trax{

	class SectionTrack_Imp : public virtual SectionTrack,
							 public Track_Imp{
	public:
		// Track:
		TrackType GetTrackType() const noexcept override;

		bool IsValid() const noexcept override;


		// SectionTrack:
		void Attach( std::shared_ptr<const Section> pSection ) noexcept override;

		std::shared_ptr<const Section> DetachSection() noexcept override;

		std::shared_ptr<const Section> GetSection() const noexcept override;
	private:
		std::shared_ptr<const Section> m_pSection;
	};

}

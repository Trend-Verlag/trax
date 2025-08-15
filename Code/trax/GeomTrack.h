// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#pragma once

#include "Track.h"

namespace trax{

	struct Section;

	struct GeomTrack : virtual TrackBuilder{

		/// \brief Makes a GeomTrack object.
		static dclspc std::shared_ptr<GeomTrack> Make() noexcept;


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

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

#include "trax/GeomTrack.h"
#include "Track_Imp.h"

namespace trax{

	class GeomTrack_Imp :	public virtual GeomTrack,
							public Track_Imp{
	public:
		// Track:
		TrackType GetTrackType() const noexcept override;

		bool IsValid() const noexcept override;


		// GeomTrack:
		void Attach( std::shared_ptr<const Section> pSection ) noexcept override;

		std::shared_ptr<const Section> DetachSection() noexcept override;

		std::shared_ptr<const Section> GetSection() const noexcept override;
	private:
		std::shared_ptr<const Section> m_pSection;
	};

}

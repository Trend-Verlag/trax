// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2013 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "Track_Imp.h"

namespace trax{

	class MovableTrack_Imp :	public virtual MovableTrack,
								public Track_Imp
							 
	{
	public:
		MovableTrack_Imp() noexcept;

		TrackType GetTrackType() const noexcept override;

		// Inherited via MovableTrack:
		void SetBody( std::shared_ptr<const Body> pBody ) noexcept override;

		std::shared_ptr<const Body> GetBody() const noexcept override;

		void UpdateTrackPose() override;

		bool IsMoving() const noexcept override;
	private:
		spat::Frame<Length,One>		m_RelativePose;
		std::shared_ptr<const Body>	m_pBody;
	};

}
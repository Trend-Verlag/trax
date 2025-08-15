// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2023 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/Track.h"

namespace trax {

	struct TrackSystem;

	struct MovableTrackAutoConnecting : public virtual MovableTrack {
	public:

		/// \brief Makes a MovableTrackAutoConnecting object.
		static dclspc std::shared_ptr<MovableTrackAutoConnecting> Make() noexcept;


		/// \name AutoConnecting
		/// \brief Specifies, wether the track should disconnect and try to reconnect.
		/// \param distance Distance to disconnect if the actual distance between this 
		/// track's ends and the coupled ones get greater than. Or try to reconnect if
		/// getting smaller. A value <= 0 disables autoconnecting.
		///@{
		virtual void AutoConnecting( Length distance ) noexcept = 0;

		virtual Length AutoConnecting() const noexcept = 0;

		virtual bool IsAutoConnecting() const noexcept = 0;
		///@}


		/// \brief Updates the track, e.g. reconnecting the end couplings.
		virtual void Update( const TrackSystem& trackSystem ) = 0;
	};
}
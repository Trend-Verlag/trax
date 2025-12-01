//	trax track library
//	AD 2023 
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

#include "trax/rigid/MovableTrack.h"

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
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

#include "trax/collections/MovableTrackAutoConnecting.h"
#include "trax/source/MovableTrack_Imp.h"

namespace trax
{
	class MovableTrackAutoConnecting_Imp: public MovableTrackAutoConnecting,
										  public MovableTrack_Imp
	{
	public:
		MovableTrackAutoConnecting_Imp();

		// Inherited via MovableTrackAutoConnecting:
		void AutoConnecting( Length distance ) noexcept override;

		Length AutoConnecting() const noexcept override;

		bool IsAutoConnecting() const noexcept override;

		void Update( const TrackSystem& trackSystem ) override;
	private:
		Length m_AutoConnectingDistance;
	};

}

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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

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

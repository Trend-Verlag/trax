//	trax track library
//	AD 2013 
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

#include "trax/rigid/Body.h"
#include "trax/rigid/MovableTrack.h"
#include "trax/source/Track_Imp.h"
#include "FrameSync_Impl.h"

namespace trax{

	class MovableTrack_ImpBase : public virtual MovableTrack,
								 public virtual Track_Imp
	{		
	public:
		MovableTrack_ImpBase() noexcept;
		~MovableTrack_ImpBase() noexcept;
		
		
		void AutoDeconnect( Length atDistance = 1_m, Angle atAngle = pi/4, bool bRemoveFromConnector = true ) override;		
	private:
		// AutoDeconnect:
		Plug_Imp_ParentPointer<MovableTrack_ImpBase> m_PlugToDeconnect;
		void OnTryDeconnect() noexcept;
		void OnTryDeconnect( trax::EndType endType ) noexcept;
		Length m_LengthThreshold;
		Angle m_AngleThreshold;
		bool m_bAutoDeconnectRemoveFromConnector;
	};


	class MovableTrack_Imp : public FrameSync_Imp<MovableTrack_ImpBase,Body>
							 
	{
	public:
		// Track:
		TrackType GetTrackType() const noexcept override;

		std::shared_ptr<const MovableTrack> GetMovableTrack() const noexcept override;

		std::shared_ptr<MovableTrack> GetMovableTrack() noexcept override;


		// Inherited via MovableTrack:
		void SetBody( std::shared_ptr<Body> pBody ) noexcept override;

		virtual std::shared_ptr<Body> GetBody() const noexcept override;

		bool IsMoving() const noexcept override;
	};

}
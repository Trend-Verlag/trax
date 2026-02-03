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

#include "MovableTrack_Imp.h"
#include "trax/rigid/Body.h"

#include <iostream>

namespace trax{
	using namespace spat;

MovableTrack_Imp::MovableTrack_Imp() noexcept
	: m_pBody								{ nullptr }
	, m_bFramePropagationToBodyOnSetFrame	{ true }
{
}

void MovableTrack_Imp::PropagateAbsoluteFrameToClients() noexcept
{
	Track_Imp::PropagateAbsoluteFrameToClients();

	if( m_bFramePropagationToBodyOnSetFrame && m_pBody )
	{
		Frame<Length,One> bodyFrame = GetFrame();
		m_RelativePose.ToParent( bodyFrame );
		m_pBody->SetFrame( bodyFrame );
	}
}

Track::TrackType MovableTrack_Imp::GetTrackType() const noexcept{
	return TrackType::movable;
}

std::shared_ptr<const MovableTrack> MovableTrack_Imp::GetMovableTrack() const noexcept{
	return std::dynamic_pointer_cast<const MovableTrack>(This());
}

std::shared_ptr<MovableTrack> MovableTrack_Imp::GetMovableTrack() noexcept{
	return std::dynamic_pointer_cast<MovableTrack>(This());
}

bool MovableTrack_Imp::IsValid() const noexcept
{
	if( !Track_Imp::IsValid() )
		return false;

	if( m_pBody )
	{
		Frame<Length,One> bodyFrame, relative = m_RelativePose;
		m_pBody->GetFrame( bodyFrame );
		bodyFrame.ToParent( relative );
		return relative.Equals( GetFrame(), epsilon__length, epsilon__angle );
	}

	return true;
}

bool MovableTrack_Imp::Diagnose( std::ostream& os ) const noexcept
{
	if( !Track_Imp::Diagnose( os ) )
		return false;

	if( !m_pBody )
	{
		os << "MovableTrack_Imp::Diagnose: No body assigned." << std::endl;
	}
	else{
		Frame<Length, One> bodyFrame, relative = m_RelativePose;
		m_pBody->GetFrame( bodyFrame );
		bodyFrame.ToParent( relative );
		if( !relative.Equals( GetFrame(), epsilon__length, epsilon__angle ) )
		{
			os << "MovableTrack_Imp::Diagnose: Track frame does not match body frame + relative pose." << std::endl;
			return false;
		}
	}

	return true;
}

void MovableTrack_Imp::SetBody( std::shared_ptr<Body> pBody ) noexcept{
	m_pBody = pBody;

	if( m_pBody )
	{
		Frame<Length,One> bodyFrame;
		m_pBody->GetFrame( bodyFrame );
		m_RelativePose = GetFrame();
		bodyFrame.FromParent( m_RelativePose );	
	}
}

std::shared_ptr<Body> MovableTrack_Imp::GetBody() const noexcept{
	return m_pBody;
}

void MovableTrack_Imp::UpdateTrackPose() noexcept
{
	if( m_pBody )
	{
		Frame<Length,One> bodyFrame, trackFrame = m_RelativePose;
		m_pBody->GetFrame( bodyFrame );
		bodyFrame.ToParent( trackFrame );
		common::FlagBlocker fb{ m_bFramePropagationToBodyOnSetFrame };
		SetFrame( trackFrame.OrthoNormalize() );
	}
}

bool MovableTrack_Imp::IsMoving() const noexcept{
	return m_pBody ? !m_pBody->IsSleeping() : false;
}

}
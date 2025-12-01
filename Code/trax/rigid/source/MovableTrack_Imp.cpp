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
///////////////////////////////////////
std::shared_ptr<MovableTrack> trax::MovableTrack::Make() noexcept
{
	try{
		return std::make_shared<MovableTrack_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
MovableTrack_Imp::MovableTrack_Imp() noexcept
	:	m_pBody{nullptr}
{
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

void MovableTrack_Imp::SetBody( std::shared_ptr<const Body> pBody ) noexcept{
	m_pBody = pBody;

	if( m_pBody )
	{
		Frame<Length,One> bodyFrame;
		m_pBody->GetFrame( bodyFrame );
		m_RelativePose = GetFrame();
		bodyFrame.FromParent( m_RelativePose );	
	}
}

std::shared_ptr<const Body> MovableTrack_Imp::GetBody() const noexcept{
	return m_pBody;
}

void MovableTrack_Imp::UpdateTrackPose() noexcept
{
	if( m_pBody )
	{
		Frame<Length,One> bodyFrame, trackFrame = m_RelativePose;
		m_pBody->GetFrame( bodyFrame );
		bodyFrame.ToParent( trackFrame );
		SetFrame( trackFrame );
	}
}

bool MovableTrack_Imp::IsMoving() const noexcept{
	return m_pBody ? !m_pBody->IsSleeping() : false;
}

}
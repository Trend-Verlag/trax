//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2013 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#include "MovableTrack_Imp.h"
#include "trax/Body.h"

#include <iostream>

namespace trax{
	using namespace spat;
///////////////////////////////////////
std::shared_ptr<MovableTrack> trax::MovableTrack::Make() noexcept
{
	try{
		if( std::shared_ptr<MovableTrack_Imp> pRetval = std::make_shared<MovableTrack_Imp>() ){
			pRetval->SetWeakPointerToSelf( pRetval );
			return pRetval;
		}

		return nullptr;
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

void MovableTrack_Imp::UpdateTrackPose()
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
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
#include "trax/Connector.h"
#include "trax/rigid/Body.h"

#include <iostream>

namespace trax{
	using namespace spat;

///////////////////////////////////////
MovableTrack_ImpBase::MovableTrack_ImpBase() noexcept
	: m_PlugToDeconnect						{ *this, &MovableTrack_ImpBase::OnTryDeconnect }
	, m_LengthThreshold						{ 0_m }
	, m_AngleThreshold						{ 0_deg }
	, m_bAutoDeconnectRemoveFromConnector	{ false }
{
}

MovableTrack_ImpBase::~MovableTrack_ImpBase() noexcept
{
}

void MovableTrack_ImpBase::AutoDeconnect( Length atDistance, Angle atAngle, bool bRemoveFromConnector )
{
	if( !GetBody() )
		throw std::logic_error{ "MovableTrack_Imp::AutoDeconnect: No body assigned to track." };

	m_LengthThreshold = atDistance;
	m_AngleThreshold = atAngle;
	m_bAutoDeconnectRemoveFromConnector = bRemoveFromConnector;

	if( m_LengthThreshold > 0_m || m_AngleThreshold > 0_deg )
		GetBody()->JackOnSleep().InsertAtTail( &m_PlugToDeconnect );
	else
		m_PlugToDeconnect.Remove();
}

void MovableTrack_ImpBase::OnTryDeconnect() noexcept
{
	OnTryDeconnect( trax::EndType::north );
	OnTryDeconnect( trax::EndType::south );
}

void MovableTrack_ImpBase::OnTryDeconnect( trax::EndType endType ) noexcept
{
	if( TrackEnd othersEnd = TransitionEnd( endType ); othersEnd.pTrack )
	{
		Frame<Length,One> thisFrame, otherFrame;
		Transition( ParameterFrom( endType ), thisFrame );
		othersEnd.pTrack->Transition( othersEnd.pTrack->ParameterFrom( othersEnd.end ), otherFrame );

		if( (m_LengthThreshold > 0_m && (otherFrame.P - thisFrame.P).Length() > m_LengthThreshold) ||
			(m_AngleThreshold > 0_rad && asin( (otherFrame.T % thisFrame.T).Length() ) > m_AngleThreshold) )
		{
			Disconnect( endType );

			if( m_bAutoDeconnectRemoveFromConnector )
				if( Connector* pConnector = GetConnector( endType ); pConnector )
					pConnector->Clear( pConnector->Slot( *this, endType ) );
		}
	}
}
///////////////////////////////////////
Track::TrackType MovableTrack_Imp::GetTrackType() const noexcept{
	return TrackType::movable;
}

std::shared_ptr<const MovableTrack> MovableTrack_Imp::GetMovableTrack() const noexcept{
	return std::dynamic_pointer_cast<const MovableTrack>(This());
}

std::shared_ptr<MovableTrack> MovableTrack_Imp::GetMovableTrack() noexcept{
	return std::dynamic_pointer_cast<MovableTrack>(This());
}

void MovableTrack_Imp::SetBody( std::shared_ptr<Body> pBody ) noexcept
{
	m_pSyncee = pBody;

	ReadRelativePose();
}

std::shared_ptr<Body> MovableTrack_Imp::GetBody() const noexcept
{
	return m_pSyncee;
}

bool MovableTrack_Imp::IsMoving() const noexcept{
	return m_pSyncee ? !m_pSyncee->IsSleeping() : false;
}
///////////////////////////////////////
}
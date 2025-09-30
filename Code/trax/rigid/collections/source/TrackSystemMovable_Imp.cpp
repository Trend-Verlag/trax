//	trax track library
//	AD 2025 
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

#include "TrackSystemMovable_Imp.h"
#include "../MovableTrackAutoConnecting.h"

namespace trax{

IDType TrackSystemMovable_Imp::Add( std::shared_ptr<TrackBuilder> pTrack )
{
	if( IDType retval = TrackSystem_Imp::Add( pTrack ); retval )
	{
		if( auto pMovableTrack = std::dynamic_pointer_cast<MovableTrack>(pTrack) )
			if( std::find( m_MovableTracks.begin(), m_MovableTracks.end(), pMovableTrack ) == m_MovableTracks.end() )
				m_MovableTracks.push_back( pMovableTrack );

		if( auto pMovableTrackAutoConnecting = std::dynamic_pointer_cast<MovableTrackAutoConnecting>(pTrack) )
			if( std::find( m_MovableTracksAutoConnecting.begin(), m_MovableTracksAutoConnecting.end(), pMovableTrackAutoConnecting ) == m_MovableTracksAutoConnecting.end() )
				m_MovableTracksAutoConnecting.push_back( pMovableTrackAutoConnecting );

		return retval;
	}

	return 0;
}

bool TrackSystemMovable_Imp::Remove( TrackBuilder* pTrack, bool /*zeroIDs*/ )
{
	if( TrackSystem_Imp::Remove( pTrack ) )
	{
		if( auto pMovableTrackAutoConnecting = dynamic_cast<MovableTrackAutoConnecting*>(pTrack) ){
			auto iter = std::find_if( m_MovableTracksAutoConnecting.begin(), m_MovableTracksAutoConnecting.end(), 
				[pMovableTrackAutoConnecting]( const auto& pMT ) noexcept { return pMovableTrackAutoConnecting == pMT.get(); } );
			if( iter != m_MovableTracksAutoConnecting.end() )
				m_MovableTracksAutoConnecting.erase( iter );
		}

		if( auto pMovableTrack = dynamic_cast<MovableTrack*>(pTrack) ){
			auto iter = std::find_if( m_MovableTracks.begin(), m_MovableTracks.end(), 
				[pMovableTrack]( const auto& pMT ) noexcept { return pMovableTrack == pMT.get(); } );
			if( iter != m_MovableTracks.end() )
				m_MovableTracks.erase( iter );
		}

		return true;
	}

	return false;
}

void TrackSystemMovable_Imp::Clear() noexcept
{
	m_MovableTracks.clear();
	m_MovableTracksAutoConnecting.clear();

	TrackSystem_Imp::Clear();
}

void TrackSystemMovable_Imp::Update( Time dt )
{
	TrackSystem_Imp::Update( dt );

	for( auto& pMovableTrack : m_MovableTracks )
		pMovableTrack->UpdateTrackPose();

	for( auto& pMovableTrackAutoConnecting : m_MovableTracksAutoConnecting )
		pMovableTrackAutoConnecting->Update( *this );
}

}

//	trax track library
//	AD 2014 
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

#include "TrackCollectionContainer_Imp.h"
#include "TrackCollection_Imp.h"
#include "trax/source/Track_Imp.h"
#include "trax/collections/TrackSystem.h"

#include <algorithm>

namespace trax{
///////////////////////////////////////
TrackCollectionContainer_Imp::TrackCollectionContainer_Imp() noexcept
	: m_pParent{ nullptr }
{
	m_AbsoluteFrame.Init();
}

TrackCollectionContainer_Imp::TrackCollectionContainer_Imp( std::shared_ptr<TrackCollection> pTrackCollection )
	: m_pParent( nullptr )
{
	m_AbsoluteFrame.Init();
	DoAdd( pTrackCollection );
}

std::unique_ptr<TrackCollectionContainer> TrackCollectionContainer::Make( bool doCreateCollections ) noexcept
{
	if( doCreateCollections )
		return std::make_unique<TrackCollectionContainer_Imp>( TrackCollection::Make() );

	return std::make_unique<TrackCollectionContainer_Imp>();
}

void TrackCollectionContainer_Imp::SetParent( TrackSystem* pParent ){
	if( m_pParent == pParent )
		return;

	if( m_pParent ){
		for( auto& trackCollection : *this )
			for( auto& track : trackCollection )
				m_pParent->Remove( &track );
	}
	else if( pParent ){
		for( auto& trackCollection : *this )
			for( auto pTrack = trackCollection.GetFirst(); pTrack; pTrack = trackCollection.GetNext( pTrack ) )
				pParent->Add( pTrack );
	}

	m_pParent = pParent;
}

TrackSystem* TrackCollectionContainer_Imp::GetParent() const noexcept{
	return m_pParent;
}

void TrackCollectionContainer_Imp::SetAbsoluteFrame( const Frame<Length,One>& frame ) noexcept{
	m_AbsoluteFrame = frame;

	SetCollectionsAbsoluteFrame(m_AbsoluteFrame);
}

const char* TrackCollectionContainer_Imp::TypeName() const noexcept{
	return "TrackCollectionContainer";
}

bool TrackCollectionContainer_Imp::IsValid() const noexcept{
	return IsValid_Imp( m_Container );
}

IDType TrackCollectionContainer_Imp::Add( std::shared_ptr<TrackCollection> pTrackCollection ){
	return DoAdd( pTrackCollection );
}

bool TrackCollectionContainer_Imp::Remove( TrackCollection* pTrackCollection, bool zeroIDs ){
	if( TrackCollectionContainer_Base::Remove( pTrackCollection, zeroIDs ) )
	{
		if( auto pTrackCollection_Imp = dynamic_cast<TrackCollection_Imp*>( pTrackCollection ) ){
			pTrackCollection_Imp->SetParent( nullptr );
			pTrackCollection_Imp->SetAbsoluteFrame( Identity<Length,One> );
		}

		return true;
	}

	return false;
}
		
void TrackCollectionContainer_Imp::Clear() noexcept {
	SetCollectionsAbsoluteFrame( Identity<Length,One> );

	TrackCollectionContainer_Base::Clear();
}

void TrackCollectionContainer_Imp::SetCollectionsAbsoluteFrame( const Frame<Length,One>& frame ) noexcept{
	std::for_each( m_Container.begin(), m_Container.end(),
		[&frame]( const std::pair<IDType,std::shared_ptr<TrackCollection>>& pair ) noexcept
	{
		if( auto pTrackCollection_Imp = decorator_cast<TrackCollection_Imp*>( pair.second.get() ) )
			pTrackCollection_Imp->SetAbsoluteFrame(frame);
	});
}

IDType TrackCollectionContainer_Imp::DoAdd( std::shared_ptr<TrackCollection> pTrackCollection ){
	if( IDType retval = TrackCollectionContainer_Base::Add( pTrackCollection ) )
	{
		if( auto pTrackCollection_Imp = decorator_cast<TrackCollection_Imp*>( pTrackCollection.get() ) ){
			pTrackCollection_Imp->SetAbsoluteFrame(m_AbsoluteFrame);
			pTrackCollection_Imp->SetParent( GetParent() );
		}

		return retval;
	}

	return 0;
}
///////////////////////////////////////
}

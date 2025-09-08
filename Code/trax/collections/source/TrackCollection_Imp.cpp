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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#include "TrackCollection_Imp.h"
#include "TrackCollectionContainer_Imp.h"
#include "trax/source/Track_Imp.h"

#include "trax/collections/TrackSystem.h"
#include "trax/Location.h"

#include "spat/Sphere.h"

#include <algorithm>

namespace trax{

TrackCollection_Imp::TrackCollection_Imp() noexcept
	: m_pParent{ nullptr }
{}

TrackCollection_Imp::~TrackCollection_Imp(){
	DoClear();
}

std::unique_ptr<TrackCollection> TrackCollection::Make() noexcept
{
	try{
		return std::make_unique<TrackCollection_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

void TrackCollection_Imp::SetParent( TrackSystem* pParent ){
	if( m_pParent == pParent )
		return;

	if( m_pParent ){
		for( auto& track : *this )
			m_pParent->Remove( &track );
	}
	if( pParent ){
		for( auto pTrack = GetFirst(); pTrack; pTrack = GetNext( pTrack ) )
			pParent->Add( pTrack );
	}
	
	m_pParent = pParent;
}

//TrackSystem* TrackCollection_Imp::GetParent() const noexcept{
//	return m_pParent;
//}

const char* TrackCollection_Imp::TypeName() const{
	return "TrackCollection";
}

IDType TrackCollection_Imp::Add( std::shared_ptr<TrackBuilder> pTrack ){
	if( IDType retval = TrackCollection_Base::Add( pTrack ) )
	{
		if( auto pTrack_Imp = dynamic_cast<Track_Imp*>(pTrack.get()) )
			pTrack_Imp->SetAbsoluteFrame( GetAbsoluteFrame() * GetFrame() );

		return retval;
	}

	return 0;
}

bool TrackCollection_Imp::Remove( TrackBuilder* pTrack, bool zeroIDs ){
	if( TrackCollection_Base::Remove( pTrack, zeroIDs ) )
	{
		if( auto pTrack_Imp = dynamic_cast<Track_Imp*>(pTrack) )
			pTrack_Imp->SetAbsoluteFrame( Identity<Length,One> );

		return true;
	}

	return false;
}

void TrackCollection_Imp::Clear() noexcept {
	DoClear();

	TrackCollection_Base::Clear();
}

void TrackCollection_Imp::PropagateAbsoluteFrameToClients() noexcept{
	TrackCollection_Base::PropagateAbsoluteFrameToClients();
	SetTracksAbsoluteFrames( GetAbsoluteFrame() * GetFrame() );
}

void TrackCollection_Imp::SetTrackAbsoluteFrame( TrackBuilder* pTrack, const Frame<Length,One>& frame ) const noexcept{
	if( Track_Imp* pImp = dynamic_cast<Track_Imp*>( pTrack ) )
		pImp->SetAbsoluteFrame(frame);
}

void TrackCollection_Imp::SetTracksAbsoluteFrames( const Frame<Length,One>& frame ) const noexcept{
	std::for_each( m_Container.begin(), m_Container.end(),
		[&frame,this]( const std::pair<IDType,std::shared_ptr<TrackBuilder>>& pair  ) noexcept
	{
		SetTrackAbsoluteFrame( pair.second.get(), frame );
	});
}

void TrackCollection_Imp::DoClear() noexcept {
	SetFrame( Identity<Length,One> );

	for( TrackBuilder& track : *this ){
		if( auto pTrack_Imp = dynamic_cast<Track_Imp*>(&track) )
			pTrack_Imp->SetAbsoluteFrame( Identity<Length,One> );
	}
}
///////////////////////////////////////
std::vector<std::tuple<std::shared_ptr<TrackBuilder>,Track::EndType,Length>> FindTrackEnds( 
	const TrackCollection& collection, 
	const spat::Sphere<Length>& area, 
	bool sort )
{
	std::vector<std::tuple<std::shared_ptr<TrackBuilder>,Track::EndType,Length>> retval;

	for( const TrackBuilder& track : collection ){
		if( track.IsValid() ){
			Position<Length> trackEndPos;
		
			track.Transition( 0_m, trackEndPos );
			if( area.Includes(trackEndPos) )
				retval.push_back( { track.This(), Track::EndType::front, (trackEndPos-area.c).Length() } );

			track.Transition( track.GetLength(), trackEndPos );
			if( area.Includes(trackEndPos) )
				retval.push_back( { track.This(), Track::EndType::end, (trackEndPos-area.c).Length() } );
		}
	}

	if( sort )
		std::sort( retval.begin(), retval.end(), 
			[]( const std::tuple<std::shared_ptr<TrackBuilder>,Track::EndType,Length>& a, const std::tuple<std::shared_ptr<TrackBuilder>,Track::EndType,Length>& b ) -> bool { return std::get<2>(a) < std::get<2>(b); } );

	return retval;
}

std::vector<std::pair<Location, Length>> FindTrackLocations(
	const TrackCollection& collection, 
	const Sphere<Length>& area, 
	bool sort )
{
	std::vector<std::pair<Location, Length>> locations;

	for( const auto& track : collection ){
		Sphere<Length> boundingSphere;
		boundingSphere.r = track.GetLength()/2;
		track.Transition( boundingSphere.r, boundingSphere.c );

		if( Intersecting( area, boundingSphere ) ){
			const Length s = Closest( area.Center(), track, false );
			if( s >= 0_m ){
				Position<Length> p;
				track.Transition( s, p );
				Length distance = (p - area.Center()).Length();

				if( distance < area.Radius() )
					locations.push_back( std::make_pair( Location{ track.This(), TrackLocation{ s } }, distance ) );
			}
		}
	}

	if( sort )
		std::sort( locations.begin(), locations.end(), 
			[]( const std::pair<Location, Length>& a, const std::pair<Location, Length>& b ) -> bool { return a.second < b.second; } );

	return locations;
}

std::vector<std::pair<Location,Length>> FindTrackLocations(
	const TrackCollection& collection,
	const spat::VectorBundle<Length,One>& ray,
	Length gauge,
	bool sort )
{
	std::vector<std::pair<Location, Length>> locations;

	if( gauge > 0_m )
	{
		for( const auto& track : collection )
		{
			Sphere<Length> boundingSphere;
			boundingSphere.r = track.GetLength()/2;
			track.Transition( boundingSphere.r, boundingSphere.c );
			boundingSphere.r += gauge/2;

			if( Intersecting( boundingSphere, ray ) )
			{
				const Length s = Closest( ray, track, false );
				if( s >= 0_m ){
					Frame<Length,One> F;
					track.Transition( s, F );
					const Vector<Length> D = (F.P - ray.P) - ((F.P - ray.P)*ray.T)*ray.T;

					if( D.Length() < gauge/2 )
						locations.push_back( std::make_pair( Location{ track.This(), TrackLocation{ s } }, D.Length() ) );
				}
			}
		}

		if( sort )
			std::sort( locations.begin(), locations.end(), 
				[]( const std::pair<Location, Length>& a, const std::pair<Location, Length>& b ) -> bool { return a.second < b.second; } );
	}

	return locations;
}

std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> Snap(
	const TrackCollection& collection,
	TrackBuilder& track,
	Track::EndType endType,
	Length maxDistance,
	bool bUncoupled )
{
	if( maxDistance <= 0_m )
		return { nullptr, Track::EndType::none };

	Length s;
	switch( endType )
	{
	case Track::EndType::front:
	case Track::EndType::any:
		s = 0_m;
		break;
	case Track::EndType::end:
		s = track.GetLength();
		break;
	default:
		return { nullptr, Track::EndType::none };
	};

	spat::Position<trax::Length> trackEndPosition;
	track.Transition( s, trackEndPosition );
	const spat::Sphere<trax::Length> searchArea{ trackEndPosition, maxDistance };

	auto TrackEnds = trax::FindTrackEnds( collection, searchArea, true );
	TrackEnds.erase( 
		std::remove_if( 
			TrackEnds.begin(), 
			TrackEnds.end(), 
			[&track]( std::tuple<std::shared_ptr<TrackBuilder>,Track::EndType,Length>& tuple ) noexcept { return std::get<0>(tuple).get() == &track; }
		), 
		TrackEnds.end() 
	);

	if( bUncoupled )
		TrackEnds.erase( 
			std::remove_if( 
				TrackEnds.begin(), 
				TrackEnds.end(), 
				[]( std::tuple<std::shared_ptr<TrackBuilder>,Track::EndType,Length>& tuple ) noexcept { return std::get<0>(tuple)->IsCoupled( std::get<1>(tuple) ); }
			), 
			TrackEnds.end() 
		);
		
	if( !TrackEnds.empty() )
	{
		std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> retval{ std::get<0>(TrackEnds.front()), std::get<1>(TrackEnds.front()) };

		spat::Frame<Length,One> alignTo;
		const Length s2 = (retval.second == Track::EndType::front) ? 0_m : retval.first->GetLength();
		retval.first->Transition( s2, alignTo );
		if( (s == 0_m && s2 == 0_m) || (s != 0_m && s2 != 0_m) ){
			alignTo.T *= -1;
			alignTo.N *= -1;
		}

		track.SetFrame( alignTo, s );

		return retval;
	}

	return { nullptr, Track::EndType::none };
}

std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> Couple(
	const TrackCollection& collection,
	TrackBuilder& track,
	Track::EndType endType,
	Length maxDistance, 
	bool bUncoupled )
{
	if( maxDistance <= 0_m )
		return { nullptr, Track::EndType::none };

	Length s;
	switch( endType )
	{
	case Track::EndType::front:
	case Track::EndType::any:
		s = 0_m;
		break;
	case Track::EndType::end:
		s = track.GetLength();
		break;
	default:
		return { nullptr, Track::EndType::none };
	};

	spat::Position<trax::Length> trackEndPosition;
	track.Transition( s, trackEndPosition );
	const spat::Sphere<trax::Length> searchArea{ trackEndPosition, maxDistance };

	std::vector<std::tuple<std::shared_ptr<TrackBuilder>,Track::EndType,Length>> TrackEnds = trax::FindTrackEnds( collection, searchArea, true );
	TrackEnds.erase( 
		std::remove_if( 
			TrackEnds.begin(), 
			TrackEnds.end(), 
			[&track]( std::tuple<std::shared_ptr<TrackBuilder>,Track::EndType,Length>& tuple ) noexcept { return std::get<0>(tuple).get() == &track; }
		), 
		TrackEnds.end() 
	);

	if( bUncoupled )
		TrackEnds.erase( 
			std::remove_if( 
				TrackEnds.begin(), 
				TrackEnds.end(), 
				[]( std::tuple<std::shared_ptr<TrackBuilder>,Track::EndType,Length>& tuple ) noexcept { return std::get<0>(tuple)->IsCoupled( std::get<1>(tuple) ); }
			), 
			TrackEnds.end() 
		);
		
	if( !TrackEnds.empty() )
	{
		std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> retval{ std::get<0>(TrackEnds.front()), std::get<1>(TrackEnds.front()) };

		trax::Couple( std::make_pair( track.This(), endType ), retval );

		return retval;
	}

	return { nullptr, Track::EndType::none };
}
///////////////////////////////////////
}
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

#include "TrackSystem_Imp.h"
#include "trax/collections/TrackCollection.h"
#include "trax/collections/ConnectorCollection.h"
#include "trax/Sensor.h"
#include "TrackCollectionContainer_Imp.h"
#include "trax/source/Track_Imp.h"
#include "spat/Sphere.h"

#include <iostream>

namespace trax{

TrackSystem_Imp::TrackSystem_Imp( 
	std::unique_ptr<TrackCollectionContainer> pTrackCollectionContainer,
	std::unique_ptr<ConnectorCollection> pConnectorCollection ) noexcept
	:	m_pTrackCollectionContainer	{std::move(pTrackCollectionContainer)},
		m_IDActiveTrackCollection	{0u},
		m_pConnectorCollection		{std::move(pConnectorCollection)}
{
	if( auto pTrackCollectionContainer_Imp = decorator_cast<TrackCollectionContainer_Imp*>(m_pTrackCollectionContainer.get()) )
		pTrackCollectionContainer_Imp->SetParent( this );
}

TrackSystem_Imp::~TrackSystem_Imp(){
	DoClear();
}

std::shared_ptr<TrackSystem> TrackSystem::Make() noexcept
{
	return Make(
		trax::TrackCollectionContainer::Make(), 
		trax::ConnectorCollection::Make() );
}

std::shared_ptr<TrackSystem> TrackSystem::Make(
	std::unique_ptr<TrackCollectionContainer> pTrackCollectionContainer) noexcept
{
	return Make(
		std::move(pTrackCollectionContainer),
		trax::ConnectorCollection::Make() );
}

std::shared_ptr<TrackSystem> TrackSystem::Make(
	std::unique_ptr<TrackCollectionContainer> pTrackCollectionContainer,
	std::unique_ptr<ConnectorCollection> pConnectorCollection ) noexcept
{
	try{
		std::shared_ptr<TrackSystem_Imp> pRetval = std::make_shared<TrackSystem_Imp>(
			std::move(pTrackCollectionContainer),
			std::move(pConnectorCollection) );

		if(pRetval)
			pRetval->SetWeakPointerToSelf(pRetval);

		return pRetval;
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

void TrackSystem_Imp::SetWeakPointerToSelf( std::weak_ptr<TrackSystem_Imp> pThis ) noexcept{
	m_pThis = pThis;
}

void TrackSystem_Imp::SetAbsoluteFrame( const Frame<Length,One>& frame ) noexcept{
	if( auto pCollectionContainer = decorator_cast<TrackCollectionContainer_Imp*>(m_pTrackCollectionContainer.get()) )
		pCollectionContainer->SetAbsoluteFrame( frame );
}

const char*	TrackSystem_Imp::TypeName() const noexcept{
	return "TrackSystem";
}

bool TrackSystem_Imp::IsValid() const noexcept
{
	bool bOK = true;
	if( m_pTrackCollectionContainer == nullptr )
	{
		std::cout << Verbosity::verbose << "No TrackCollectionContainer assigned to TrackSystem!" << std::endl;	
		bOK = false;
	}
	else{
		for( const auto& collection : *m_pTrackCollectionContainer )
		{
			if( !collection.GetFrame().IsOrthoNormal() )
			{
				std::cout << Verbosity::verbose << "TrackCollection with ID " << collection.ID() << " has a non orthonormal frame!" << std::endl;	
				bOK = false;
			}
		}
	}

	for( auto iter = begin(); iter != end(); ++iter ){
		if( !iter->IsValid() )
		{
			std::cout << Verbosity::verbose << "Track with ID " << iter->ID() << " is not valid!" << std::endl;
			bOK = false;
		}
	}

	if( m_pConnectorCollection == nullptr )
	{
		std::cout << Verbosity::verbose << "No ConnectorCollection assigned to TrackSystem!" << std::endl;
		bOK = false;
	}
	else{
		if( !m_pConnectorCollection->IsValid() )
			bOK = false;
		else 
			for( const auto& Connector : *m_pConnectorCollection )
			{
				for( int slot = 0; slot < Connector.CntSlots(); ++slot )
				{
					auto trackEnd = Connector.Slot( slot );
					if( trackEnd.first )
					{
						if( !IsMember( *trackEnd.first ) )
						{
							std::cout << Verbosity::verbose << "Connector with ID " << Connector.ID() << " has a track end assigned to a track not in the track system! Track ID: " << trackEnd.first->ID() << std::endl;
							bOK = false;
						}
					}
				}
			}
	}

	return bOK;
}

std::shared_ptr<TrackSystem> TrackSystem_Imp::This() const noexcept{
	return m_pThis.lock();
}

IDType TrackSystem_Imp::Add( std::shared_ptr<TrackBuilder> pTrack ){
	if( !pTrack || !pTrack->IsValid() )
		return 0;

	if( auto pCollection = m_pTrackCollectionContainer->GetActive() ){
		if( IDType retval = TrackSystem_Base::Add( pTrack ) ){
			pCollection->Add( pTrack );
			return retval;
		}
	}
	else{
		if( pTrack )
			std::cerr << Verbosity::error << "No active track collection to add track to!" << std::endl;
	}

	return 0;
}

bool TrackSystem_Imp::Remove( TrackBuilder* pTrack, bool zeroIDs ){
	if( TrackSystem_Base::Remove( pTrack ) )
	{		 
		for( auto iter = m_pTrackCollectionContainer->begin(); iter != m_pTrackCollectionContainer->end(); ++iter )
			if( iter->Remove( pTrack ) )
			{
				if( zeroIDs )
					pTrack->ID( 0 );

				return true;
			}
	}

	return false;
}

int TrackSystem_Imp::Take( TrackSystem& trackSystem ){
	if( m_pTrackCollectionContainer->GetActive() == nullptr )
		throw std::runtime_error( "No active TrackCollection in Tracksystem to add track to!" );

	const int offset = TrackSystem_Base::Take( trackSystem );

	if( m_pConnectorCollection )
		if( auto pConnectorCollection = trackSystem.GetConnectorCollection() )
			m_pConnectorCollection->Take( *pConnectorCollection );

	return offset;
}

void TrackSystem_Imp::Clear() noexcept {
	DoClear();

	TrackSystem_Base::Clear();
}

IDType TrackSystem_Imp::CreateCollection( IDType id ){
	if( id &&
		m_pTrackCollectionContainer->IsMember( id ) )
		return id;

	if( std::shared_ptr<TrackCollection> pTrackCollection = TrackCollection::Make() ){
		pTrackCollection->ID( id );
		id = m_pTrackCollectionContainer->Add( pTrackCollection );
		SetActiveCollection( id );
		return id;
	}

	return 0;
}

void TrackSystem_Imp::RemoveCollection( IDType id ){
	assert( m_pTrackCollectionContainer->IsMember( id ) );

	m_pTrackCollectionContainer->Remove( 
		m_pTrackCollectionContainer->Get(id).get() );

	if( m_IDActiveTrackCollection == id )
		m_IDActiveTrackCollection = 0;
}

void TrackSystem_Imp::SetCollectionFrame( IDType id, const Frame<Length,One>& frame ){
	assert( m_pTrackCollectionContainer->IsMember( id ) );

	if( auto pCollection = m_pTrackCollectionContainer->Get(id) )
		pCollection->SetFrame( frame );
}

const Frame<Length,One>& TrackSystem_Imp::GetCollectionFrame( IDType id ) const{
	if( auto pCollection = m_pTrackCollectionContainer->Get(id) )
		return pCollection->GetFrame();

	throw std::invalid_argument( "No collection with id in TrackSystem!" );
}

IDType TrackSystem_Imp::SetActiveCollection( IDType id ){
	assert( m_pTrackCollectionContainer->IsMember( id ) );

	if( m_pTrackCollectionContainer->IsMember( id ) )
		m_IDActiveTrackCollection = id;

	return m_IDActiveTrackCollection;
}
		
IDType TrackSystem_Imp::GetActiveCollection() const noexcept{
	return m_IDActiveTrackCollection;
}

std::shared_ptr<TrackCollectionContainer> TrackSystem_Imp::SetTrackCollectionContainer( std::shared_ptr<TrackCollectionContainer> pTrackCollectionContainer ){
	std::shared_ptr<TrackCollectionContainer> pRetval = m_pTrackCollectionContainer;
	if( auto pTrackCollectionContainer_Imp = decorator_cast<TrackCollectionContainer_Imp*>(m_pTrackCollectionContainer.get()) )
		pTrackCollectionContainer_Imp->SetParent( nullptr );
	m_pTrackCollectionContainer = pTrackCollectionContainer;
	if( auto pTrackCollectionContainer_Imp = decorator_cast<TrackCollectionContainer_Imp*>(m_pTrackCollectionContainer.get()) )
		pTrackCollectionContainer_Imp->SetParent( Decorator() );
	return pRetval;
}

std::shared_ptr<TrackCollectionContainer> TrackSystem_Imp::GetCollectionContainer() const noexcept{
	return m_pTrackCollectionContainer;
}

std::shared_ptr<ConnectorCollection> TrackSystem_Imp::SetConnectorCollection( std::shared_ptr<ConnectorCollection> pConnectorCollection ) noexcept{
	std::shared_ptr<ConnectorCollection> retval = std::move(m_pConnectorCollection);
	m_pConnectorCollection = pConnectorCollection;
	return retval;
}

ConnectorCollection* TrackSystem_Imp::GetConnectorCollection() const noexcept{
	return m_pConnectorCollection.get();
}

void TrackSystem_Imp::Couple(	
	std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> trackEnd1, 
	std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> trackEnd2 ) const
{
	if( trackEnd1.first && trackEnd2.first ){
		trackEnd1.first->Couple( trackEnd1, trackEnd2 );
		return;
	}

	std::ostringstream stream;
	stream << "Track A of coupling doesn't exist in this track system!"  << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::invalid_argument( stream.str() );
}

void TrackSystem_Imp::Couple( const Track::Coupling& coupling, bool bUncoupledOnly ) const{
	if( auto pTrackA = Get( coupling.theOne.id ) ){
		if( auto pTrackB = Get( coupling.theOther.id ) ){
			if( bUncoupledOnly && (pTrackA->IsCoupled( coupling.theOne.type ) || pTrackB->IsCoupled( coupling.theOther.type )) )
				return;

			pTrackA->Couple( std::make_pair(pTrackA, coupling.theOne.type), std::make_pair(pTrackB, coupling.theOther.type) );
			return;
		}
	}

	std::ostringstream stream;
	stream << "Track A (id:" << coupling.theOne.id << ") of coupling doesn't exist in this track system!" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::invalid_argument( stream.str() );
}

std::shared_ptr<Sensor> TrackSystem_Imp::GetSensor( 
	IDType id, 
	TrackLocation* pTrackLocation ) const
{
	for( const auto& track : *this ){
		for( int idx = 0; idx < track.CountSensors(); ++idx )
			if( track.GetSensor( idx )->ID() == id )
				if( auto pSensor = track.GetSensor( idx ) ){
					if( pTrackLocation )
						track.Attached( *pSensor, pTrackLocation );

					return pSensor;
				}
	}

	return nullptr;
}

Length TrackSystem_Imp::CalculateGapSize( const Track::End& theOne, const Track::End& theOther ) const
{
	return DistanceOf( 
		Track::cTrackEnd{ Get( theOne.id ), theOne.type }, 
		Track::cTrackEnd{ Get( theOther.id ), theOther.type } );
}

std::vector<Track::End> TrackSystem_Imp::GetUncoupledIn( const Sphere<Length>& area ) const{
	std::vector<Track::End> ends;
	Position<Length> trackEndPos;
	for( const auto& track : *this ){
		if( !track.IsCoupled( Track::EndType::front ) ){
			track.Transition( 0_m, trackEndPos );
			if( area.Includes(trackEndPos) )
				ends.push_back( {track.ID(),Track::EndType::front} );
		}

		if( !track.IsCoupled( Track::EndType::end ) ){
			track.Transition( track.GetLength(), trackEndPos );
			if( area.Includes(trackEndPos) )
				ends.push_back( {track.ID(),Track::EndType::end} );
		}
	}

	return ends;
}

void TrackSystem_Imp::Connection( Track::Coupling& coupling ) const{
	if( !coupling.theOther.id ){
		if( auto pTrackA = Get( coupling.theOne.id ) )
			if( Track::TrackEnd trackEndB = pTrackA->TransitionEnd( coupling.theOne.type ); trackEndB.first ){
				coupling.theOther.id = trackEndB.first->ID();
				coupling.theOther.type = trackEndB.second;
			}
	}
	else if( !coupling.theOne.id ){
		if( auto pTrackB = Get( coupling.theOther.id ) )
			if( Track::TrackEnd trackEndA = pTrackB->TransitionEnd( coupling.theOther.type ); trackEndA.first ){
				coupling.theOne.id = trackEndA.first->ID();
				coupling.theOne.type = trackEndA.second;
			}
	}
}

void TrackSystem_Imp::Connection( const Track::End& end, Track::End& coupled ) const{
	if( auto pTrack = Get( end.id ) )
		if( Track::TrackEnd otherTrackEnd = pTrack->TransitionEnd( end.type ); otherTrackEnd.first ){
			coupled.id = otherTrackEnd.first->ID();
			coupled.type = otherTrackEnd.second;
		}
}

void TrackSystem_Imp::Connection( const Track::Coupling& couplings, Track::Coupling& active ) const{
	if( auto pTrack = Get( couplings.theOne.id ) )
		if( Track::TrackEnd otherTrackEnd = pTrack->TransitionEnd( couplings.theOne.type ); otherTrackEnd.first ){
			active.theOne.id = otherTrackEnd.first->ID();
			active.theOne.type = otherTrackEnd.second;
		}
	if( auto pTrack = Get( couplings.theOther.id ) )
		if( Track::TrackEnd otherTrackEnd = pTrack->TransitionEnd( couplings.theOther.type ); otherTrackEnd.first ){
			active.theOther.id = otherTrackEnd.first->ID();
			active.theOther.type = otherTrackEnd.second;
		}
}

void TrackSystem_Imp::CoupleAll( Length maxDistance, Angle maxKink, bool bSilent )
{
	if( m_pTrackCollectionContainer )
	{
		for( TrackCollection& trackCollection : *m_pTrackCollectionContainer )
		{
			for( TrackBuilder& track : trackCollection )
			{
				if( !track.IsCoupled( Track::EndType::front ) )
					trax::Couple( trackCollection, {track.This(), Track::EndType::front}, maxDistance, maxKink, bSilent );

				if( !track.IsCoupled( Track::EndType::end ) )
					trax::Couple( trackCollection, {track.This(), Track::EndType::end}, maxDistance, maxKink, bSilent );
			}
		}
	}
}

void TrackSystem_Imp::DeCoupleAll()
{
	DoDeCoupleAll();
}

bool TrackSystem_Imp::Start( Scene& /*scene*/ ) noexcept
{
	return true;
}

void TrackSystem_Imp::Idle() noexcept
{
}

void TrackSystem_Imp::Update( Time /*dt*/ ) noexcept
{
}

void TrackSystem_Imp::Pause() noexcept
{
}

void TrackSystem_Imp::Resume() noexcept
{
}

void TrackSystem_Imp::Stop() noexcept
{
}

int TrackSystem_Imp::CountJacks() const noexcept
{
	int count = 0;
	for( const auto& track : *this )
		count += dynamic_cast<const JackEnumerator&>(track).CountJacks();

	return count;
}

void TrackSystem_Imp::SetDecorator( TrackSystem* pTrackSystemDecorator ) noexcept{
	TrackSystem_Base::SetDecorator( pTrackSystemDecorator );

	if( auto pTrackCollectionContainer_Imp = decorator_cast<TrackCollectionContainer_Imp*>(m_pTrackCollectionContainer.get()) )
		pTrackCollectionContainer_Imp->SetParent( Decorator() );

	//if( auto pTrackCollectionContainer_Imp = decorator_cast<TrackCollectionContainer_Imp*>(m_pTrackCollectionContainer.get()) ){
	//	if( pTrackSystemDecorator ){
	//		assert( decorator_cast<TrackSystem_Imp*>(pTrackSystemDecorator) == this );
	//		pTrackCollectionContainer_Imp->SetParent( pTrackSystemDecorator );
	//	}
	//	else{
	//		pTrackCollectionContainer_Imp->SetParent( this );
	//	}
	//}
}

const Jack& TrackSystem_Imp::_GetJack( int idx ) const{
	int count = 0;
	for( const auto& track : *this ){
		const JackEnumerator& jackEnumerator = dynamic_cast<const JackEnumerator&>(track);
		count += jackEnumerator.CountJacks();
		if( count > idx )
			return jackEnumerator.GetJack( idx - count + jackEnumerator.CountJacks() );
	}

	std::ostringstream stream;
	stream << "Out of range!" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::range_error( stream.str() );
}

void TrackSystem_Imp::DoDeCoupleAll(){
	std::for_each( m_Container.begin(), m_Container.end(),
		[]( const std::pair<IDType,std::shared_ptr<TrackBuilder>>& pair  )
	{ 
		pair.second->DeCouple();
	});
}

void TrackSystem_Imp::DoClear(){
	DoDeCoupleAll();

	if( m_pTrackCollectionContainer )
		m_pTrackCollectionContainer->Clear();

	if( m_pConnectorCollection )
		m_pConnectorCollection->Clear();
}
///////////////////////////////////////
std::vector<std::tuple<std::shared_ptr<TrackBuilder>,Track::EndType,Length>> FindTrackEnds( 
	const TrackSystem& system, 
	const spat::Sphere<Length>& area, 
	bool sort )
{
	std::vector<std::tuple<std::shared_ptr<TrackBuilder>,Track::EndType,Length>> retval;

	if( auto pCollectionContainer = system.GetCollectionContainer() ){
		for( const auto& trackCollection : *pCollectionContainer ){
			auto moreTracks = FindTrackEnds( trackCollection, area );
			retval.insert( retval.end(), moreTracks.begin(), moreTracks.end() );
		}
	}

	if( sort )
		std::sort( retval.begin(), retval.end(), 
			[]( const std::tuple<std::shared_ptr<TrackBuilder>,Track::EndType,Length>& a, const std::tuple<std::shared_ptr<TrackBuilder>,Track::EndType,Length>& b ){ return std::get<2>(a) < std::get<2>(b); } );

	return retval; 
}

std::vector<std::pair<Location,Length>> FindTrackLocations( 
	const TrackSystem& system, 
	const spat::Sphere<Length>& area, 
	bool sort )
{
	std::vector<std::pair<Location,Length>> locations;

	if( auto pCollectionContainer = system.GetCollectionContainer() ){
		for( const auto& trackCollection : *pCollectionContainer ){
			auto moreTracks = FindTrackLocations( trackCollection, area );
			locations.insert( locations.end(), moreTracks.begin(), moreTracks.end() );
		}
	}

	if( sort )
		std::sort( locations.begin(), locations.end(), 
			[]( const std::pair<Location, Length>& a, const std::pair<Location, Length>& b ){ return a.second < b.second; } );

	return locations;
}

Location FindTrackLocation( const TrackSystem& system, const spat::Sphere<Length>& area )
{
	std::vector<std::pair<trax::Location,dim::Length>> Locations = trax::FindTrackLocations( 
		system, 
		area, 
		true );

	if( Locations.size() )
		return Locations.front().first;

	return {};
}

std::vector<std::pair<Location,Length>>dclspc FindTrackLocations(
	const TrackSystem & system,
	const spat::VectorBundle<Length,One>& ray,
	Length gauge,
	bool sort )
{
	std::vector<std::pair<Location,Length>> locations;

	if( auto pCollectionContainer = system.GetCollectionContainer() ){
		for( const auto& trackCollection : *pCollectionContainer ){
			auto moreTracks = FindTrackLocations( trackCollection, ray, gauge );
			locations.insert( locations.end(), moreTracks.begin(), moreTracks.end() );
		}
	}

	if( sort )
		std::sort( locations.begin(), locations.end(), 
			[]( const std::pair<Location, Length>& a, const std::pair<Location, Length>& b ){ return a.second < b.second; } );

	return locations;
}

std::pair<Track::TrackEnd,Track::TrackEnd> Couple( 
	const TrackSystem& system, 
	Track::TrackEnd trackEnd, 
	Length maxDistance, 
	Angle maxKink, 
	bool bSilent )
{	
	if( system.IsMember( *trackEnd.first->This() ) )
	{
		for( const TrackCollection& collection : *system.GetCollectionContainer() )
		{
			if( collection.IsMember( *trackEnd.first->This() ) )
			{
				return Couple( 
					collection, 
					trackEnd,
					maxDistance,
					maxKink,
					bSilent );
			}
		}
	}

	for( const TrackCollection& collection : *system.GetCollectionContainer() )
	{
		std::pair<Track::TrackEnd,Track::TrackEnd> coupledTo = Couple( 
			collection, 
			trackEnd,
			maxDistance,
			maxKink,
			bSilent );

		if( coupledTo.first.first || coupledTo.second.first )
			return coupledTo;
	}

	return {};
}

std::pair<Track::TrackEnd,Track::TrackEnd> CoupleAndSnap( 
	const TrackSystem& system, 
	Track::TrackEnd trackEnd, 
	Length maxDistance, 
	Angle maxKink, 
	bool bSilent )
{
	std::pair<trax::Track::TrackEnd,trax::Track::TrackEnd> CoupledTo = Couple( 
		system, 
		trackEnd,
		maxDistance,
		maxKink,
		bSilent );

	if( CoupledTo.first.first )
	{
		trax::Snap( 
			std::make_pair( trackEnd.first, trax::Track::EndType::front ), 
			CoupledTo.first );
	}
	else if( CoupledTo.second.first )
	{
		trax::Snap( 
			std::make_pair( trackEnd.first, trax::Track::EndType::end ), 
			CoupledTo.second );
	}

	return CoupledTo;
}
///////////////////////////////////////
}

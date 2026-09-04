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
// For further information, please contact: horstmann@traxlibrary.dev

#include "TrackSystem_Imp.h"

#include "trax/Exceptions.h"
#include "trax/Sensor.h"
#include "trax/Switch.h"
#include "trax/collections/TrackCollection.h"
#include "trax/collections/ConnectorCollection.h"
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
		TrackCollectionContainer::Make(), 
		ConnectorCollection::Make() );
}

std::shared_ptr<TrackSystem> TrackSystem::Make(
	std::unique_ptr<TrackCollectionContainer> pTrackCollectionContainer) noexcept
{
	return Make(
		std::move(pTrackCollectionContainer),
		ConnectorCollection::Make() );
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

void TrackSystem_Imp::Connect(	
	std::pair<std::shared_ptr<TrackBuilder>,EndType> trackEnd1, 
	std::pair<std::shared_ptr<TrackBuilder>,EndType> trackEnd2 ) const
{
	if( trackEnd1.first && trackEnd2.first ){
		trackEnd1.first->Connect( trackEnd1, trackEnd2 );
		return;
	}

	std::ostringstream stream;
	stream << "Track A of coupling doesn't exist in this track system!"  << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::invalid_argument( stream.str() );
}

void TrackSystem_Imp::Connect( const Track::Connection& coupling, bool bUnconnectedOnly ) const{
	if( auto pTrackA = Get( coupling.theOne.id ) ){
		if( auto pTrackB = Get( coupling.theOther.id ) ){
			if( bUnconnectedOnly && (pTrackA->IsConnected( coupling.theOne.type ) || pTrackB->IsConnected( coupling.theOther.type )) )
				return;

			pTrackA->Connect( std::make_pair(pTrackA, coupling.theOne.type), std::make_pair(pTrackB, coupling.theOther.type) );
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

common::Span<const Track::End> TrackSystem_Imp::GetUnconnectedIn( const Sphere<Length>& area ) const{
	static thread_local std::vector<Track::End> ends;
	ends.clear();

	Position<Length> trackEndPos;
	for( const auto& track : *this ){
		if( !track.IsConnected( EndType::north ) ){
			track.Transition( 0_m, trackEndPos );
			if( area.Includes(trackEndPos) )
				ends.push_back( {track.ID(),EndType::north} );
		}

		if( !track.IsConnected( EndType::south ) ){
			track.Transition( track.GetLength(), trackEndPos );
			if( area.Includes(trackEndPos) )
				ends.push_back( {track.ID(),EndType::south} );
		}
	}

	return { ends.data(), ends.size() };
}

void TrackSystem_Imp::Connection( Track::Connection& coupling ) const{
	if( !coupling.theOther.id ){
		if( auto pTrackA = Get( coupling.theOne.id ) )
			if( Track::TrackEnd trackEndB = pTrackA->TransitionEnd( coupling.theOne.type ); trackEndB.pTrack ){
				coupling.theOther.id = trackEndB.pTrack->ID();
				coupling.theOther.type = trackEndB.end;
			}
	}
	else if( !coupling.theOne.id ){
		if( auto pTrackB = Get( coupling.theOther.id ) )
			if( Track::TrackEnd trackEndA = pTrackB->TransitionEnd( coupling.theOther.type ); trackEndA.pTrack ){
				coupling.theOne.id = trackEndA.pTrack->ID();
				coupling.theOne.type = trackEndA.end;
			}
	}
}

void TrackSystem_Imp::Connection( const Track::End& end, Track::End& coupled ) const{
	if( auto pTrack = Get( end.id ) )
		if( Track::TrackEnd otherTrackEnd = pTrack->TransitionEnd( end.type ); otherTrackEnd.pTrack ){
			coupled.id = otherTrackEnd.pTrack->ID();
			coupled.type = otherTrackEnd.end;
		}
}

void TrackSystem_Imp::Connection( const Track::Connection& couplings, Track::Connection& active ) const{
	if( auto pTrack = Get( couplings.theOne.id ) )
		if( Track::TrackEnd otherTrackEnd = pTrack->TransitionEnd( couplings.theOne.type ); otherTrackEnd.pTrack ){
			active.theOne.id = otherTrackEnd.pTrack->ID();
			active.theOne.type = otherTrackEnd.end;
		}
	if( auto pTrack = Get( couplings.theOther.id ) )
		if( Track::TrackEnd otherTrackEnd = pTrack->TransitionEnd( couplings.theOther.type ); otherTrackEnd.pTrack ){
			active.theOther.id = otherTrackEnd.pTrack->ID();
			active.theOther.type = otherTrackEnd.end;
		}
}

void TrackSystem_Imp::ConnectAll( Length maxDistance, Angle maxKink )
{
	if( m_pTrackCollectionContainer )
	{
		for( TrackCollection& trackCollection : *m_pTrackCollectionContainer )
		{
			for( TrackBuilder& track : trackCollection )
			{
				if( !track.IsConnected( EndType::north ) )
					trax::Connect( trackCollection, {track.This(), EndType::north}, maxDistance, maxKink );

				if( !track.IsConnected( EndType::south ) )
					trax::Connect( trackCollection, {track.This(), EndType::south}, maxDistance, maxKink );
			}
		}
	}
}

void TrackSystem_Imp::DisconnectAll()
{
	DoDisconnectAll();
}

void TrackSystem_Imp::Registered( Scene & scene ) noexcept
{}

void TrackSystem_Imp::Unregistered( Scene & scene ) noexcept
{}

bool TrackSystem_Imp::Start() noexcept
{
	return true;
}

void TrackSystem_Imp::Idle() noexcept
{
}

void TrackSystem_Imp::PreUpdate()
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
	throw std::out_of_range( stream.str() );
}

void TrackSystem_Imp::DoDisconnectAll(){
	std::for_each( m_Container.begin(), m_Container.end(),
		[]( const std::pair<IDType,std::shared_ptr<TrackBuilder>>& pair  )
	{ 
		pair.second->Disconnect();
	});
}

void TrackSystem_Imp::DoClear(){
	DoDisconnectAll();

	if( m_pTrackCollectionContainer )
		m_pTrackCollectionContainer->Clear();

	if( m_pConnectorCollection )
		m_pConnectorCollection->Clear();
}
///////////////////////////////////////
common::Span<const std::pair<Track::End,Length>> FindTrackEnds( 
	const TrackSystem& system, 
	const spat::Sphere<Length>& area, 
	bool sort )
{
	std::vector<std::pair<Track::End,Length>> collector;

	if( auto pCollectionContainer = system.GetCollectionContainer() ){
		for( const auto& trackCollection : *pCollectionContainer ){
			auto moreTracks = FindTrackEnds( trackCollection, area );
			collector.insert( collector.end(), moreTracks.begin(), moreTracks.end() );
		}
	}

	if( sort )
		std::sort( collector.begin(), collector.end(), 
			[]( const std::pair<Track::End,Length>& a, const std::pair<Track::End,Length>& b ){ return a.second < b.second; } );

	static thread_local std::vector<std::pair<Track::End, Length>> retval;
	retval = std::move(collector);
	return { retval.data(), retval.size() };
}

common::Span<const std::pair<TrackSystemLocation,Length>> FindTrackLocations( 
	const TrackSystem& system, 
	const spat::Sphere<Length>& area, 
	bool sort )
{
	std::vector<std::pair<TrackSystemLocation,Length>> collector;

	if( auto pCollectionContainer = system.GetCollectionContainer() ){
		for( const auto& trackCollection : *pCollectionContainer ){
			auto moreTracks = FindTrackLocations( trackCollection, area );
			collector.insert( collector.end(), moreTracks.begin(), moreTracks.end() );
		}
	}

	if( sort )
		std::sort( collector.begin(), collector.end(), 
			[]( const std::pair<TrackSystemLocation, Length>& a, const std::pair<TrackSystemLocation, Length>& b ){ return a.second < b.second; } );

	static thread_local std::vector<std::pair<TrackSystemLocation,Length>> retval;
	retval = std::move(collector);
	return { retval.data(), retval.size() };
}

Location FindTrackLocation( const TrackSystem& system, const spat::Sphere<Length>& area )
{
	common::Span<const std::pair<TrackSystemLocation,dim::Length>> Locations = FindTrackLocations( 
		system, 
		area, 
		true );

	if( Locations.size )
		return { system.Get( Locations.front().first.refid ), Locations.front().first.location };

	return {};
}

common::Span<const std::pair<TrackSystemLocation,Length>> FindTrackLocations(
	const TrackSystem & system,
	const spat::VectorBundle<Length,One>& ray,
	Length gauge,
	bool sort )
{
	std::vector<std::pair<TrackSystemLocation,Length>> collector;

	if( auto pCollectionContainer = system.GetCollectionContainer() ){
		for( const auto& trackCollection : *pCollectionContainer ){
			auto moreTracks = FindTrackLocations( trackCollection, ray, gauge );
			collector.insert( collector.end(), moreTracks.begin(), moreTracks.end() );
		}
	}

	if( sort )
		std::sort( collector.begin(), collector.end(), 
			[]( const std::pair<TrackSystemLocation, Length>& a, const std::pair<TrackSystemLocation, Length>& b ){ return a.second < b.second; } );

	static thread_local std::vector<std::pair<TrackSystemLocation,Length>> retval;
	retval = std::move(collector);
	return { retval.data(), retval.size() };
}

std::pair<Track::TrackEnd,Track::TrackEnd> Connect( 
	const TrackSystem& system, 
	Track::TrackEnd trackEnd, 
	Length maxDistance, 
	Angle maxKink )
{	
	if( system.IsMember( *trackEnd.pTrack->This() ) )
	{
		for( const TrackCollection& collection : *system.GetCollectionContainer() )
		{
			if( collection.IsMember( *trackEnd.pTrack->This() ) )
			{
				return Connect( 
					collection, 
					trackEnd,
					maxDistance,
					maxKink );
			}
		}
	}

	for( const TrackCollection& collection : *system.GetCollectionContainer() )
	{
		std::pair<Track::TrackEnd,Track::TrackEnd> coupledTo = Connect( 
			collection, 
			trackEnd,
			maxDistance,
			maxKink );

		if( coupledTo.first.pTrack || coupledTo.second.pTrack )
			return coupledTo;
	}

	return {};
}

std::pair<Track::TrackEnd,Track::TrackEnd> ConnectAndSnap( 
	const TrackSystem& system, 
	Track::TrackEnd trackEnd, 
	Length maxDistance, 
	Angle maxKink )
{
	std::pair<Track::TrackEnd,Track::TrackEnd> CoupledTo = Connect( 
		system, 
		trackEnd,
		maxDistance,
		maxKink );

	if( CoupledTo.first.pTrack )
	{
		Snap( 
			{ trackEnd.pTrack, EndType::north }, 
			CoupledTo.first );
	}
	else if( CoupledTo.second.pTrack )
	{
		Snap( 
			{ trackEnd.pTrack, EndType::south }, 
			CoupledTo.second );
	}

	return CoupledTo;
}

std::shared_ptr<Connector> ConnectConnectorAware( 
	const TrackSystem& system, 
	Track::TrackEnd trackEnd, 
	Track::TrackEnd toTrackEnd )
{
	if( trackEnd.pTrack == toTrackEnd.pTrack )
		throw std::invalid_argument( "ConnectConnectorAware: Cannot connect a track to itself!" );

	if( !IsConcreteEnd( trackEnd ) )
		throw std::invalid_argument( "ConnectConnectorAware: trackEnd is not a concrete end!" );

	if( !IsConcreteEnd( toTrackEnd ) )
		throw std::invalid_argument( "ConnectConnectorAware: toTrackEnd is not a concrete end!" );

	if(	IsConnected( trackEnd ) )
		throw std::invalid_argument( "ConnectConnectorAware: trackEnd is already connected!" );

	if(	trackEnd.pTrack->GetConnector( trackEnd.end ) )
		throw std::invalid_argument( "ConnectConnectorAware: trackEnd is already a member of a connector!" );

	Track::TrackEnd wasConnected = toTrackEnd.pTrack->TransitionEnd( toTrackEnd.end );
	Connector* pConnector = toTrackEnd.pTrack->GetConnector( toTrackEnd.end );
	Switch* pSwitch = dynamic_cast<Switch*>(pConnector);

	if( pConnector && (!pSwitch || pSwitch->NarrowTrack().first != toTrackEnd.pTrack ) )
		// We have no option for these for now.
		throw NotImplemented{ "ConnectConnectorAware: Building four-way switch or slip switches!" };

	Connect( trackEnd, toTrackEnd );

	if( Connected( trackEnd, toTrackEnd ) )
	{
		if( pConnector )
		{
			if( pSwitch )
			{
				if( std::shared_ptr<ThreeWaySwitch> pThreeWaySwitch = ThreeWaySwitch::Make(); pThreeWaySwitch )
				{
					pThreeWaySwitch->NarrowTrack( pSwitch->ClearNarrowTrack() );
					pThreeWaySwitch->StraightTrack( pSwitch->ClearStraightTrack() );
					pThreeWaySwitch->DivergedTrack1( pSwitch->ClearDivergedTrack() );
					pThreeWaySwitch->DivergedTrack2( trackEnd );
					pThreeWaySwitch->Normalize();

					system.GetConnectorCollection()->Add( pThreeWaySwitch );
					system.GetConnectorCollection()->Remove( pSwitch );

					return pThreeWaySwitch;
				}
			}
		}
		else if( wasConnected.pTrack && wasConnected.pTrack != trackEnd.pTrack )
		// Creating a switch if applicable:
		{
			if( std::shared_ptr<Switch> pNewSwitch = Switch::Make(); pNewSwitch )
			{
				pNewSwitch->NarrowTrack( toTrackEnd );
				pNewSwitch->StraightTrack( wasConnected );
				pNewSwitch->DivergedTrack( trackEnd );
				pNewSwitch->Normalize();

				system.GetConnectorCollection()->Add( pNewSwitch );

				return pNewSwitch;
			}
		}
	}

	return nullptr;
}

std::shared_ptr<Connector> ConnectConnectorAware( 
	const TrackSystem& system, 
	Track::TrackEnd trackEnd, 
	const Length maxDistance, 
	Angle maxKink )
{
	if( !IsConcreteEnd( trackEnd ) )
		throw std::invalid_argument( "ConnectConnectorAware: trackEnd is not a concrete end!" );

	if( IsConnected( trackEnd ) )
		throw std::invalid_argument( "ConnectConnectorAware: trackEnd is already connected!" );

	if(	trackEnd.pTrack->GetConnector( trackEnd.end ) )
		throw std::invalid_argument( "ConnectConnectorAware: trackEnd is already a member of a connector!" );

	spat::Sphere<Length> area{ spat::Origin3D<Length>, maxDistance };
	trackEnd.pTrack->Transition( trackEnd.pTrack->ParameterFrom( trackEnd.end ), area.c );
	common::Span<const std::pair<Track::End,Length>> trackEnds = FindTrackEnds( 
		system, area, true );

	for( const auto& trackEndPair : trackEnds )
	{
		if( trackEndPair.first.id == trackEnd.pTrack->ID() )
			continue;

		Track::TrackEnd toTrackEnd{ system.Get( trackEndPair.first.id ), trackEndPair.first.type };
		if( trackEndPair.second <= maxDistance &&
			KinkOf( trackEnd, toTrackEnd ) <= maxKink )
		{
			return ConnectConnectorAware( system, trackEnd, toTrackEnd );
		}
	}

	return nullptr;
}

std::pair<std::shared_ptr<Connector>,std::shared_ptr<Connector>> ConnectAndSnap( 
	const TrackSystem& system, 
	const Track::TrackEnd trackEnd, 
	const Track::TrackEnd toTrackEnd, 
	const Length maxDistance, 
	Angle maxKink )
{
	std::pair<std::shared_ptr<Connector>,std::shared_ptr<Connector>> retval;
	retval.first = ConnectConnectorAware( system, trackEnd, toTrackEnd );

	if( Connected( trackEnd, toTrackEnd ) &&
		Snap( trackEnd, toTrackEnd ) )
	{
		// Track needs to get connected on opposite end if applicable:		
		const Track::TrackEnd otherTrackEnd = !trackEnd;

		if( IsConnected( otherTrackEnd ) &&
			DistanceToConnected( otherTrackEnd ) > maxDistance )
		{
			trackEnd.pTrack->This()->Disconnect( otherTrackEnd.end );
		}

		retval.second = ConnectConnectorAware( system, otherTrackEnd, maxDistance, maxKink );		
	}

	return retval;
}
///////////////////////////////////////
}

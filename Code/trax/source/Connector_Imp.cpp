//	trax track library
//	AD 2018
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

#include "Connector_Imp.h"

#include "dim/support/DimSupportStream.h"
#include "trax/Location.h"
#include "Track_Imp.h"

#include <iostream>

namespace trax{

std::string ToString( ConnectorType ct )
{
	switch( ct ){
	case ConnectorType::unknown:
		return "unknown";
	case ConnectorType::none:
		return "none";
	case ConnectorType::TwoWaySwitch:
		return "Switch";
	case ConnectorType::ThreeWaySwitch:
		return "ThreeWaySwitch";
	case ConnectorType::SingleSlipSwitch:
		return "SingleSlipSwitch";
	case ConnectorType::DoubleSlipSwitch:
		return "DoubleSlipSwitch";
	default:
		assert( !"unknown connector type!" );
		return "unknown";
	};
}

ConnectorType ToConnectorType( const std::string& name ) noexcept
{
	if( name == "Switch" )
		return ConnectorType::TwoWaySwitch;
	else if( name == "ThreeWaySwitch" )
		return ConnectorType::ThreeWaySwitch;
	else if( name == "SingleSlipSwitch" )
		return ConnectorType::SingleSlipSwitch;
	else if( name == "DoubleSlipSwitch" )
		return ConnectorType::DoubleSlipSwitch;
	else if( name == "none" )
		return ConnectorType::none;
	else
		return ConnectorType::unknown;
}
///////////////////////////////////////
Connector_Imp::Connector_Imp( int cntSlots )
	:	m_Slots			{ common::narrow_cast<std::size_t>(cntSlots) },
		m_PlugToToggle	{ *this }
{
	m_PlugToToggle.Reference( "name", "PlugToToggle" );
}

bool Connector_Imp::IsValid() const noexcept
{
	return Check( std::cout );
}

void Connector_Imp::Disconnect(){
	for( const auto& pair : m_Slots )
		if( pair.first )
			pair.first->DeCouple( pair.second, false );
}

int Connector_Imp::Slot( 
	const int slot, 
	std::shared_ptr<TrackBuilder> pTrack, 
	EndType trackend, 
	bool connectAnonymous )
{
	if( !pTrack ){
		std::cerr << Verbosity::error << "Connector_Imp::Slot: Invalid parameter! Tried to polpulate connector slot with nullptr. Use Clear(slot) to clear a connectors's slot."
				  << "Connector ID: " << ID() << " trackend: " << ToString(trackend) << std::endl;
		return -1;
	}

	if( slot < 0 || slot >= static_cast<int>(m_Slots.size()) )
		throw std::out_of_range{ "Slot ID is out of range of valid slots!" };

	if( auto pConnector = pTrack->GetConnector( trackend ); pConnector ){
		assert( pConnector->Slot( *pTrack, trackend ) != -1 );

		if( pConnector == this && Slot( *pTrack, trackend ) == slot )
			return slot; // already connected to this slot, call redundant but ok.

		std::cerr << Verbosity::normal << "Connector (" << ID() << "," << TypeName() 
			<< ") Slot: " << slot << ", connect a track end (" << pTrack->ID() << "," << ToString(trackend) 
			<< ") that was already connected to a different connector-slot: Connector ("
			<< pConnector->ID() << "," << pConnector->TypeName() << ") Slot: " 
			<< pConnector->Slot( *pTrack, trackend ) << std::endl;
		std::cerr << Verbosity::detailed << "Possible reason: EEP switch to switch head on connection." << std::endl;

		if( !connectAnonymous ){
			pConnector->Clear( pConnector->Slot( *pTrack, trackend ) );
			assert( pConnector->Slot( *pTrack, trackend ) == -1 );
		}
	}

	if( Slot( slot ).first )
		Clear( slot );

	m_Slots[slot].first = pTrack;
	m_Slots[slot].second = trackend;

	if( !connectAnonymous ){
		if( auto pTrackImp = dynamic_cast<Track_Imp*>(pTrack.get()) )
			pTrackImp->AddConnector( this, trackend );
	}

	return slot;
}

int Connector_Imp::Slot( std::shared_ptr<TrackBuilder> pTrack, EndType trackend, bool connectAnonymous ){
	for( int slotID = 0; slotID < CntSlots(); ++slotID )
		if( !Slot( slotID ).first )
			return Slot( slotID, pTrack, trackend, connectAnonymous );

	return -1;
}

std::pair<std::shared_ptr<TrackBuilder>,EndType> Connector_Imp::Slot( int slot ) const noexcept{
	assert( m_Slots.size() < static_cast<std::size_t>(std::numeric_limits<int>::max()) );
	if( 0 <= slot && slot < static_cast<int>(m_Slots.size()) )
		return m_Slots[slot];

	return {};
}

int Connector_Imp::Slot( const Track& track, EndType trackend ) const noexcept{
	for( int slot = 0; slot < static_cast<int>(m_Slots.size()); ++slot )
		if( m_Slots[slot].first.get() == &track && m_Slots[slot].second == trackend )
			return slot;

	return -1;
}

int Connector_Imp::CntSlots() const noexcept{
	return static_cast<int>(m_Slots.size());
}

void Connector_Imp::SwapSlots( int slotA, int slotB ){
	std::swap( m_Slots.at(slotA), m_Slots.at(slotB) );
}

bool Connector_Imp::IsComplete() const noexcept{
	for( const auto& pair : m_Slots )
		if( !pair.first )
			return false;

	return true;
}

void Connector_Imp::Clear( int slot ){
	if( 0 <= slot && slot < static_cast<int>(m_Slots.size()) ){
		if( auto pTrackImp = dynamic_cast<Track_Imp*>(m_Slots.at(slot).first.get()) ){
			if( pTrackImp->GetConnector( m_Slots[slot].second ) == this )
				pTrackImp->AddConnector( nullptr, m_Slots[slot].second );
		}

		m_Slots[slot].first.reset();
	}
}

void Connector_Imp::Clear(){
	for( int slot = 0; slot < CntSlots(); ++slot )
		Clear( slot );
}

MultiPlug& Connector_Imp::PlugToToggle() noexcept{
	return m_PlugToToggle;
}

const MultiPlug& Connector_Imp::PlugToToggle() const noexcept{
	return m_PlugToToggle;
}

Jack& Connector_Imp::JackOnChange() noexcept{
	return m_JackOnChange;
}

const Jack& Connector_Imp::JackOnChange() const noexcept{
	return m_JackOnChange;
}

void Connector_Imp::RegisterSockets( SocketRegistry& module ){
	module.RegisterPlug( m_PlugToToggle );
	module.ConnectJack( m_JackOnChange );
}

void Connector_Imp::UnregisterSockets( SocketRegistry& module ){
	module.RemoveJack( m_JackOnChange );
	module.UnRegisterPlug( m_PlugToToggle );
}

int Connector_Imp::CountPlugs() const{
	return m_PlugToToggle.CountPlugs();
}

int Connector_Imp::CountJacks() const noexcept{
	return 1;
}

void Connector_Imp::UserData( ConnectorUserData* pData ) noexcept{
	m_pUserData = pData;
}

ConnectorUserData* Connector_Imp::UserData() const noexcept{
	return m_pUserData;
}

const Plug& Connector_Imp::_GetPlug( int idx ) const{
	return m_PlugToToggle.GetPlug( idx );
}

const Jack& Connector_Imp::_GetJack( int idx ) const{
	if( idx == 0 )
		return m_JackOnChange;

	std::ostringstream stream;
	stream << "Out of range!" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::range_error( stream.str() );
}

bool Connector_Imp::CheckSlot( int slot, std::ostream& os, Length e_distance, Angle e_kink, Angle e_twist ) const noexcept
{
	if( slot < 0 || slot >= CntSlots() ){
		os << Verbosity::error << "Slot ID is out of range!" << std::endl;
		return false;
	}

	if( !m_Slots[slot].first ){
		os << Verbosity::verbose << "Slot is empty!" << std::endl;
		return false;
	}

	bool bOk = true;
	if( auto pTrack = m_Slots[slot].first; pTrack ){
		try{
			if( e_distance > 0_m ){
				if( DistanceToCoupled( *pTrack, m_Slots[slot].second ) > e_distance ){
					os << Verbosity::verbose << "Distance to other end is too large! " << "Track end: (" << pTrack->ID() << "," << ToString(m_Slots[slot].second) << ")" 
						<< ", coupled with: (" << m_Slots[slot].first->TransitionEnd(m_Slots[slot].second).pTrack->ID() << "," 
						<< ToString(m_Slots[slot].first->TransitionEnd(m_Slots[slot].second).end) << ")."
						<< " Distance: " << DistanceToCoupled( *pTrack, m_Slots[slot].second ) << std::endl;
					bOk = false;
				}
			}

			if( e_kink > 0_deg ){
				if( KinkToCoupled( *pTrack, m_Slots[slot].second ) > e_kink ){
					os << Verbosity::verbose << "Kink to other end is too large! " << "Track end: (" << pTrack->ID() << "," << ToString(m_Slots[slot].second) << ")" 
						<< ", coupled with: (" << m_Slots[slot].first->TransitionEnd(m_Slots[slot].second).pTrack->ID() << "," 
						<< ToString(m_Slots[slot].first->TransitionEnd(m_Slots[slot].second).end) << ")."
						<< " Kink: " << _deg(KinkToCoupled( *pTrack, m_Slots[slot].second )) << " degree." << std::endl;
					bOk = false;
				}
			}

			if( e_twist > 0_deg ){
				if( TwistToCoupled( *pTrack, m_Slots[slot].second ) > e_twist ){
					os << Verbosity::verbose << "Twist to other end is too large! " << "Track end: (" << pTrack->ID() << "," << ToString(m_Slots[slot].second) << ")" 
						<< ", coupled with: (" << m_Slots[slot].first->TransitionEnd(m_Slots[slot].second).pTrack->ID() << "," 
						<< ToString(m_Slots[slot].first->TransitionEnd(m_Slots[slot].second).end) << ")."
						<< " Twist: " << _deg(TwistToCoupled( *pTrack, m_Slots[slot].second )) << " degree." << std::endl;
					bOk = false;
				}
			}
		}
		catch( const std::exception& e ){
			os << Verbosity::error << "Connector_Imp::CheckSlot: Exception caught during check: " << e.what() << std::endl;
			return false;
		}
	}

	return bOk;
}
///////////////////////////////////////
void Couple( 
	std::pair<std::shared_ptr<TrackBuilder>,EndType> trackEndA, 
	std::pair<std::shared_ptr<TrackBuilder>,EndType> trackEndB, 
	int slotA,
	int slotB,
	bool connectAnyway )
{
	if( !trackEndA.first || !trackEndA.first->IsValid() || !trackEndB.first || !trackEndB.first->IsValid() )
		throw std::invalid_argument( "Both tracks have to be valid." );
	if( trackEndA == trackEndB )
		throw std::invalid_argument( "Cannot couple to self." );

	if( auto pConnector = trackEndA.first->GetConnector(trackEndA.second) ){
		if( slotA >= pConnector->CntSlots() )
			throw std::out_of_range{ "Can not do coupling, since slot does not exist!" };

		if( auto pConnectorOther = trackEndB.first->GetConnector(trackEndB.second) ){
			if( slotB >= pConnectorOther->CntSlots() )
				throw std::out_of_range{ "Can not do coupling, since slot does not exist!" };

			if( pConnectorOther != pConnector ){
				if( connectAnyway ){
					slotA > -1 ?
					pConnector->Slot( slotA, trackEndB.first, trackEndB.second, true ) :
					pConnector->Slot( trackEndB.first, trackEndB.second, true );
					slotB > -1 ?
					pConnectorOther->Slot( slotB, trackEndA.first, trackEndA.second, true ) :
					pConnectorOther->Slot( trackEndA.first, trackEndA.second, true );
				}
				else
					throw std::runtime_error( "The track ends are handled by different connectors!" );
			}
			else
				pConnector->Set( *trackEndA.first, trackEndA.second, *trackEndB.first, trackEndB.second, false );
		}
		else{
			slotA > -1 ?
			pConnector->Slot( slotA, trackEndB.first, trackEndB.second ) :
			pConnector->Slot( trackEndB.first, trackEndB.second );
		}
	}
	else if( trackEndB.first->GetConnector(trackEndB.second) )
		Couple( trackEndB, trackEndA, slotB, slotA, connectAnyway );
	else 
		trackEndA.first->Couple( trackEndA, trackEndB );
}

void DeCouple( TrackBuilder& track, EndType trackend, int ownerSlot )
{
	if( auto pConnector = track.GetConnector(trackend) ){
		int slot = pConnector->Slot(track,trackend);
		if( slot != ownerSlot )
			pConnector->Clear( slot );

		if( Track::TrackEnd otherTrack = track.TransitionEnd( trackend ); otherTrack.pTrack ){
			assert( pConnector == otherTrack.pTrack->GetConnector(otherTrack.end) );
			slot = pConnector->Slot(*otherTrack.pTrack,otherTrack.end);
			if( slot != ownerSlot )
				pConnector->Clear( slot );
		}
	}

	track.DeCouple( trackend );
}

Location GetFarEndLocation( const Connector& connector, int slotID )
{
	auto trackEnd = connector.Slot(slotID); 
	if( trackEnd.first )
		return Location{ trackEnd.first, (trackEnd.second == EndType::north) ? 
											TrackLocation{ trackEnd.first->GetLength(), Orientation::Value::para } : 
											TrackLocation{ 0_m, Orientation::Value::anti } };

	return {};
}

Location GetEndLocation( const Connector& connector, int slotID )
{
	auto trackEnd = connector.Slot(slotID); 
	if( trackEnd.first )
		return Location{ trackEnd.first, (trackEnd.second == EndType::north) ? 
											TrackLocation{ 0_m, Orientation::Value::para } : 
											TrackLocation{ trackEnd.first->GetLength(), Orientation::Value::anti } };

	return {};
}

spat::Vector<Length> DistanceToOtherEnd( const Connector& connector, int fromSlot )
{
	auto trackEnd = connector.Slot(fromSlot); 
	if( trackEnd.first ){
		spat::Position<Length> p1, p2;
		trackEnd.first->Transition( 0_m, p1 );
		trackEnd.first->Transition( trackEnd.first->GetLength(), p2 );
		return (trackEnd.second == EndType::north ? +1 : -1) * (p2 - p1);
	}

	throw std::runtime_error( "Slot not connected!" );
}

}
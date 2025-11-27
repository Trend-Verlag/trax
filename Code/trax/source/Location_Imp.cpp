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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#include "trax/Location.h"
#include "trax/RoadwayTwist.h"
#include "trax/TrackData.h"
#include "trax/Sensor.h"
#include "trax/Event.h"
#include "trax/support/TraxSupportStream.h"

#include "common/Helpers.h"

#include <iostream>
#include <sstream>
#include <cmath>

namespace trax{

using namespace spat;

Location::Location( std::shared_ptr<const Track> pTrack, const TrackLocation& tl ){
	PutOn( pTrack, tl );
}

Location::Location( const Track& track, const TrackLocation& tl ){
	PutOn( track.This(), tl );
}

void Location::PutOn( std::shared_ptr<const Track> pTrack, const TrackLocation& tl ){
	if( !pTrack || !pTrack->IsValid() )
		throw std::invalid_argument( "Tried to put a location on an invalid track!" );

	m_pTrack = pTrack;
	m_TLocation = tl;

	if( !Resolve() )
	{
		if( common::Interval<Length>{ 
				m_pTrack->Range().Far(), 
				m_pTrack->Range().Far() + epsilon__length }.Includes( m_TLocation.parameter ) )
			// Have the Location bee sticky to the end of the track within epsilon__length
			// to prevent resolution issues due to infinite back and forth transitions.
		{
			m_TLocation.parameter = m_pTrack->Range().Far();
		}
		else{
			std::ostringstream stream;
			stream << "Couldn't resolve track location: " << tl << " on track: " << pTrack->ID();
			stream << ". Resolved up to: " << m_TLocation << " on track: " << m_pTrack->ID() << std::endl;
			throw std::out_of_range( stream.str() );
		}
	}
}

void Location::PutOff() noexcept{
	m_pTrack = nullptr;
}

bool Location::IsOnTrack() const noexcept{
	return m_pTrack ? true : false;
}

std::shared_ptr<const Track> Location::GetTrack() const noexcept{
	return m_pTrack;
}

Length Location::Param() const noexcept{
	return m_TLocation.parameter;
}

Orientation Location::Orient() const noexcept{
	return m_TLocation.orientation;
}

AnglePerLength Location::Curvature() const{
	if( !m_pTrack )
		throw std::logic_error( "Tried to access uninitialized Location" );

	return m_pTrack->Curvature( m_TLocation.parameter );
}

AnglePerLength Location::Torsion() const{
	if( !m_pTrack )
		throw std::logic_error( "Tried to access uninitialized Location" );

	return m_pTrack->Torsion( m_TLocation.parameter );
}

void Location::TNBFrame( Frame<Length,One>& frame ) const{
	if( !m_pTrack )
		throw std::logic_error( "Tried to access uninitialized Location" );

	m_pTrack->TNBFrame( m_TLocation.parameter, frame );
}

void Location::Get( TrackLocation& tl ) const noexcept{
	tl = m_TLocation;
}

void Location::Get( TrackLocationRef& tlr ) const noexcept{
	tlr = TrackLocationRef{ m_TLocation };
	tlr.refid = m_pTrack ? m_pTrack->ID() : IDType{0u};
}

void Location::Transition( Position<Length>& pos ) const{
	if( !m_pTrack )
		throw std::logic_error( "Tried to transition uninitialized Location" );

	m_pTrack->Transition( m_TLocation.parameter, pos );
}

void Location::Transition( Vector<One>& tan ) const{
	if( !m_pTrack )
		throw std::logic_error( "Tried to transition uninitialized Location" );

	m_pTrack->Transition( m_TLocation.parameter, tan );

	if( m_TLocation.orientation == Orientation::Value::anti )
		tan *= -1;
}

void Location::Transition( VectorBundle<Length,One>& bundle ) const{
	if( !m_pTrack )
		throw std::logic_error( "Tried to transition uninitialized Location" );

	m_pTrack->Transition( m_TLocation.parameter, bundle );

	if( m_TLocation.orientation == Orientation::Value::anti )
		bundle.T *= -1;
}

void Location::Transition( VectorBundle2<Length,One>& bundle ) const{
	if( !m_pTrack )
		throw std::logic_error( "Tried to transition uninitialized Location" );

	m_pTrack->Transition( m_TLocation.parameter, bundle );

	if( m_TLocation.orientation == Orientation::Value::anti ){
		bundle.T *= -1;
		bundle.N *= -1;
	}
}

void Location::Transition( Frame<Length,One>& frame ) const{
	if( !m_pTrack )
		throw std::logic_error( "Tried to transition uninitialized Location" );

	m_pTrack->Transition( m_TLocation.parameter, frame );

	if( m_TLocation.orientation == Orientation::Value::anti ){
		frame.T *= -1;
		frame.N *= -1;
	}
}

void Location::Transition( TrackData<Real>& td, Real engine_meters_per_unit ) const{
	if( !m_pTrack )
		throw std::logic_error( "Tried to transition uninitialized Location" );

	m_pTrack->Transition( m_TLocation.parameter, td, engine_meters_per_unit );
}

std::pair<Length,bool> Location::Move( Length dParam ){
	return Move( dParam, Orientation::Value::para, nullptr, nullptr );
}

std::pair<Length,bool> Location::Move( Length dParam, Orientation principalDirection, const Event* pEvent, SignalTarget* pSignalTarget ){
	if( !m_pTrack )
		throw std::logic_error( "Tried to move uninitialized Location" );

	std::pair<Length,bool> retval = { 0_m, false };

	const common::Interval<Length> range{
		m_TLocation.parameter,
		m_TLocation.parameter + (m_TLocation.orientation ? +dParam : -dParam) };

	if( pSignalTarget /*&& pSignalTarget->Active()*/ ){
		if( !m_pTrack->DoSignal( range, m_TLocation.orientation ? principalDirection : !principalDirection, *pSignalTarget ) )
			return retval;
	}

	if( abs(dParam) <= Length{epsilon} ) // zero distance is no movement
		return retval;

	if( pEvent )
	{
		if( !pEvent->Moving( *this, range ) ){
			retval.first = dParam;
			return retval;
		}

		m_pTrack->DoTrigger( range, *pEvent );
	}

	if( range.Far() < 0_m )
	{
		dParam -= common::Sign(dParam) * m_TLocation.parameter;
		m_TLocation.parameter = 0_m;
		if( !TrackTransition( Track::EndType::front, pEvent ) ){
			retval.first = dParam;
			return retval;
		}

		retval.second = true;
		retval.first = Move( dParam, principalDirection, pEvent, pSignalTarget ).first;
	}
	else if ( range.Far() > m_pTrack->GetLength() )
	{
		dParam -= common::Sign(dParam) * (m_pTrack->GetLength() - m_TLocation.parameter);
		m_TLocation.parameter = m_pTrack->GetLength();
		if( !TrackTransition( Track::EndType::end, pEvent ) ){
			retval.first = dParam;
			return retval;
		}

		retval.second = true;
		retval.first = Move( dParam, principalDirection, pEvent, pSignalTarget ).first;
	}
	else{
		m_TLocation.parameter = range.Far();
	}

	return retval;
}

std::pair<Length,bool> Location::Move( Length dParam, const Event* pEvent ){
	return Move( dParam, Orientation::Value::para, pEvent, nullptr );
}

std::pair<Length,bool> Location::Move( Length dParam, Orientation principalDirection, SignalTarget* pSignalTarget ){
	return Move( dParam, principalDirection, nullptr, pSignalTarget );
}

Length Location::MoveToEnd( Orientation direction, const Event* pEvent, SignalTarget* pSignalTarget ){
	if( !m_pTrack )
		throw std::logic_error( "Tried to move uninitialized Location" );

	Length paramlength;
	if( Orient() ){
		if( direction )
			paramlength = m_pTrack->GetLength() - m_TLocation.parameter;
		else
			paramlength = -m_TLocation.parameter;
	}
	else{
		if( direction )
			paramlength = m_TLocation.parameter;
		else
			paramlength = -(m_pTrack->GetLength() - m_TLocation.parameter);
	}

	Move( nexttoward( paramlength, 0.0L ), paramlength < 0_m ? Orientation::Value::anti : Orientation::Value::para, pEvent, pSignalTarget );

	m_TLocation.parameter = (m_TLocation.parameter < m_pTrack->GetLength() / 2 ? 0_m : m_pTrack->GetLength());

	return paramlength;
}

std::pair<Length,bool> Location::MoveTransit( Orientation direction, const Event* pEvent, SignalTarget* pSignalTarget ){
	if( !m_pTrack )
		throw std::logic_error( "Tried to move uninitialized Location" );

	Length paramlength;
	Track::EndType toend = Track::EndType::front;
	if( Orient() ){
		if( direction ){
			paramlength = m_pTrack->GetLength() - m_TLocation.parameter;
			toend = Track::EndType::end;
		}
		else{
			paramlength = -m_TLocation.parameter;
			toend = Track::EndType::front;
		}
	}
	else{
		if( direction ){
			paramlength = m_TLocation.parameter;
			toend = Track::EndType::front;
		}
		else{
			paramlength = -(m_pTrack->GetLength() - m_TLocation.parameter);
			toend = Track::EndType::end;
		}
	}

	Move( nexttoward( paramlength, 0.0L ), paramlength < 0_m ? Orientation::Value::anti : Orientation::Value::para, pEvent, pSignalTarget );

	m_TLocation.parameter = (m_TLocation.parameter < m_pTrack->GetLength() / 2 ? 0_m : m_pTrack->GetLength());

	return std::make_pair( paramlength, TrackTransition(toend,pEvent) );
}

Location& Location::Flip() noexcept{
	m_TLocation.Flip();
	return *this;
}

bool Location::Equals( const Location& loc, Length _epsilon ) const noexcept{
	if(	m_pTrack == loc.m_pTrack )
		return m_TLocation.Equals( loc.m_TLocation, _epsilon );

	if( IsOnTrack() )
	// there are situations at the end of coupled tracks that have to get handled seperately...
	{
		if( m_TLocation.parameter <= _epsilon ){
			Location thisLoc( *this );
			thisLoc.TrackTransition( Track::EndType::front, nullptr );
			return thisLoc.m_TLocation.Equals( loc.m_TLocation, _epsilon );
		}

		if( m_pTrack->GetLength() - m_TLocation.parameter <= _epsilon ){
			Location thisLoc( *this );
			thisLoc.TrackTransition( Track::EndType::end, nullptr );
			return thisLoc.m_TLocation.Equals( loc.m_TLocation, _epsilon );
		}
	}

	return false;
}

Length Location::Distance( const Location& loc, const Length maxdistance ) const{
	if( !IsOnTrack() || !loc.IsOnTrack() )
		throw std::logic_error{ "Location: A location to be evaluated is not sitting on a track!" };

	Location thisLoc{ *this };
	Length distance{0};

	while( thisLoc.GetTrack() != loc.GetTrack() ){
		const auto delta = thisLoc.MoveTransit( Orientation{ maxdistance > 0_m } );
		distance += delta.first;

		if( abs(distance) >= abs(maxdistance) )
			return maxdistance;

		if( !delta.second )
			throw std::out_of_range{ "Reached open track end while searching for location!" };
	}

	distance += common::Sign(maxdistance) * abs( thisLoc.Param() - loc.Param() );

	// Even if we found the loc, we not return values greater than
	// maxdistance to keep certain algorithms work.
	return abs(distance) > abs(maxdistance) ? maxdistance : distance;
}

void Location::Reserve( common::Interval<Length> range, IDType forID ){
	if( !m_pTrack )
		throw std::logic_error( "Tried to reserve with uninitialized Location" );
	   
    if( auto pMutableTrack = std::const_pointer_cast<Track>(m_pTrack); pMutableTrack )
		pMutableTrack->Reserve( m_TLocation.parameter + (m_TLocation.orientation ? +range : -range), forID  );
	else
        throw std::logic_error( "Track doesn't support reservation." );
}

void Location::DeleteReservation( IDType forID ){
	if( !m_pTrack )
		throw std::logic_error( "Tried to reserve with uninitialized Location" );
	  
    if( auto pMutableTrack = std::const_pointer_cast<Track>(m_pTrack); pMutableTrack )
 		pMutableTrack->DeleteReservation( m_TLocation.orientation ? 
							common::Interval<Length>{0_m,m_pTrack->GetLength()} : 
							common::Interval<Length>{m_pTrack->GetLength(),0_m}, forID );
	else
		throw std::logic_error( "Track doesn't support reservation." );
}

std::vector<Track::Overlap> Location::Overlaps(IDType forID) const{
	if( !m_pTrack )
		throw std::logic_error( "Tried to reserve with uninitialized Location" );

	return m_pTrack->Overlaps( forID );
}

std::shared_ptr<Track> Location::GetMutableTrack() const noexcept{
	return std::const_pointer_cast<Track>(m_pTrack);
}

bool Location::TrackTransition( Track::EndType frontend, const Event* pEvent ) noexcept{
	assert( m_pTrack );
	Track::TrackEnd nextTrackEnd{ m_pTrack->TransitionEnd( frontend ) };
	if( !nextTrackEnd.first )
		return false;

	if( pEvent ) 
		pEvent->Transitioning( *m_pTrack, frontend, *nextTrackEnd.first, nextTrackEnd.second );

	if( frontend == Track::EndType::front )
	{
		if( nextTrackEnd.second == Track::EndType::front )
			// front coupled with front: change orientation
		{
			m_TLocation.orientation.Flip();
			m_TLocation.parameter = -m_TLocation.parameter;
		}
		else
			// front coupled with end
		{
			m_TLocation.parameter += nextTrackEnd.first->GetLength();
		}
	}
	else
	{
		if( nextTrackEnd.second == Track::EndType::end )
			// end coupled with end: change orientation
		{
			m_TLocation.orientation.Flip();
			m_TLocation.parameter = nextTrackEnd.first->GetLength() + m_pTrack->GetLength() - m_TLocation.parameter;
		}
		else
			// end coupled with front
		{
			m_TLocation.parameter -= m_pTrack->GetLength();
		}
	}

	m_pTrack = nextTrackEnd.first;
	return true;
}

bool Location::Resolve() noexcept{
	if( IsResolved() )
		return true;

	if( m_TLocation.parameter < 0_m ){
		if( TrackTransition( Track::EndType::front, nullptr ) )
			return Resolve();
		else
			return false;
	}

	if( m_TLocation.parameter > m_pTrack->GetLength() ){
		if( TrackTransition( Track::EndType::end, nullptr ) )
			return Resolve();
		else
			return false;
	}

	return true;
}

bool Location::IsResolved() const noexcept{
	if( m_pTrack ){
		if( m_pTrack->Range().Includes( m_TLocation.parameter ) )
			return true;

		if( m_TLocation.parameter >= 0_m &&
			m_TLocation.parameter <= m_pTrack->Range().Far() + Length{ epsilon } )
			// Far belongs to the track range within epsilon
			return true;
	}

	return false;
}
///////////////////////////////////////
Length ParameterDistanceFrom3DDistance( 
	const Location& location, 
	const Length distanceIn3D, 
	const Length maxParameter,
	const Length _epsilon )
{
	if( _epsilon <= 0_m )
		throw std::invalid_argument( "The epsilon must be > 0!" );

	Position<Length> start, end;
	location.Transition( start );
	Length retval, move = distanceIn3D;
	Location target{location};

	while( !target.Move( move ).first ){
		retval += move;
		if( abs(retval) >= maxParameter )
			return maxParameter;

		target.Transition( end );
		const Length delta = (end-start).Length() - abs(distanceIn3D);
		if( delta >= -_epsilon )
			return retval;

		move = -common::Sign(distanceIn3D) * delta;
	}
	
	throw std::out_of_range( "Encountered an open track end!" );
}

std::tuple<std::shared_ptr<const Track>,Track::EndType,Length> EndOfLine( 
	const Location& _location, 
	const Length maxDistance, 
	const bool bDeadConnectorOnly )
{
	if( !_location.IsOnTrack() )
		return std::make_tuple( nullptr, Track::EndType::end, 0_m );

	Location location{_location};
	Length distance = 0_m;

	for(;;){
		distance += location.MoveToEnd();
		if( distance >= maxDistance )
			return std::make_tuple( nullptr, Track::EndType::end, maxDistance );

		auto pTrack = location.GetTrack();
		Track::EndType end = location.Orient() ? Track::EndType::end : Track::EndType::front;
		if( pTrack->GetConnector( end ) )
		{
			if( bDeadConnectorOnly )
			{
				if( !location.MoveTransit( Orientation::Value::para ).second )
					return std::make_tuple( pTrack, end, distance );
				location.MoveTransit( Orientation::Value::anti );
				if( pTrack != location.GetTrack() )
					return std::make_tuple( pTrack, end, distance );
			}
			else
				return std::make_tuple( pTrack, end, distance );
		}

		if( !location.MoveTransit().second )
			return std::make_tuple( pTrack, end, distance );
	}
}
///////////////////////////////////////
}
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

#include "trax/Sensor_Imp.h"
#include "Plug_Imp.h"
#include "trax/Event.h"

#include <iostream>
#include <sstream>

namespace trax{
///////////////////////////////////
std::unique_ptr<Sensor> Sensor::Make() noexcept{
	return std::make_unique<Sensor_Imp>();
}
///////////////////////////////////
Sensor_Imp::Sensor_Imp()
	:	m_pTrackBuilder	{ nullptr }
{
}

Sensor_Imp::Sensor_Imp( const Sensor_Imp& sensor )
	:	m_pTrackBuilder	{ sensor.m_pTrackBuilder }
{
}
//
//Sensor_Imp& Sensor_Imp::operator=( const Sensor_Imp& sensor ){
//	m_pTrackBuilder = sensor.m_pTrackBuilder;
//	return *this;
//}

const char*	Sensor_Imp::TypeName() const noexcept{
	return "Sensor";
}

void Sensor_Imp::Trigger( const Event& /*_event*/ ){
	m_JackOnTrigger.Pulse();
}

Jack& Sensor_Imp::JackOnTrigger() noexcept{
	return m_JackOnTrigger;
}

void Sensor_Imp::RegisterSockets( SocketRegistry& module ){
	module.ConnectJack( m_JackOnTrigger );
}

void Sensor_Imp::UnregisterSockets( SocketRegistry& module ){
	module.RemoveJack( m_JackOnTrigger );
}

TrackBuilder* Sensor_Imp::TrackAttached() const noexcept{
	return m_pTrackBuilder;
}

int Sensor_Imp::CountJacks() const noexcept{
	return 1;
}

void Sensor_Imp::Attach( TrackBuilder* pTrackBuilder ) noexcept{
	m_pTrackBuilder = pTrackBuilder;
}

const Jack& Sensor_Imp::_GetJack( int idx ) const{
	if( idx == 0 )
		return m_JackOnTrigger;

	std::ostringstream stream;
	stream << "Out of range!" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::range_error( stream.str() );
}
/*
///////////////////////////////////////
std::unique_ptr<SensorFilterJack> SensorFilterJack::Make( int flags ) noexcept{
	return std::make_unique<SensorFilterJack_Imp>( flags );
}///////////////////////////////////////
SensorFilterJack_Imp::SensorFilterJack_Imp( int flags )
	:	m_pTrackBuilder	{ nullptr },
		m_FilterFlags	{ flags }
{
}

SensorFilterJack_Imp::SensorFilterJack_Imp( const SensorFilterJack_Imp& sensor )
	:	m_pTrackBuilder	{ sensor.m_pTrackBuilder }
{
}
//
//SensorFilterJack_Imp& SensorFilterJack_Imp::operator=( const SensorFilterJack_Imp& sensor ){
//	m_pTrackBuilder = sensor.m_pTrackBuilder;
//	return *this;
//}

const char*	SensorFilterJack_Imp::TypeName() const noexcept{
	return "SensorFilterJack";
}

void SensorFilterJack_Imp::Trigger( const Event& _event ){
	if( m_FilterFlags == any ){
		m_JackOnTrigger.Pulse();
		return;
	}

	if( const EventFilter* pEvent = dynamic_cast<const EventFilter*>(&_event) )
	{
		if( m_FilterFlags & name ){
			if( pEvent->Name().find( Reference( "name" ) ) == std::string::npos )
				return;

			std::clog << "Event: " << pEvent->Name() << " passed name filter: " << Reference( "name" ) << std::endl;
		}

		if( m_FilterFlags & not_name ){
			if( pEvent->Name().find( Reference( "name" ) ) != std::string::npos )
				return;

			std::clog << "Event: " << pEvent->Name() << " passed not_name filter: " << Reference( "name" ) << std::endl;
		}

		switch( pEvent->GetType() ){
		case EventFilter::Type::wheelframe:
			if( m_FilterFlags & wheelframe )
				m_JackOnTrigger.Pulse();
			break;
		case EventFilter::Type::traintip_north:
			if( m_FilterFlags & train_north							||
				m_FilterFlags & wheelframe							||
				(pEvent->TrainTip() && m_FilterFlags & train_tip)	||
				(pEvent->TrainEnd() && m_FilterFlags & train_end) )
				m_JackOnTrigger.Pulse();
			break;
		case EventFilter::Type::traintip_south:
			if( m_FilterFlags & train_south							||
				m_FilterFlags & wheelframe							||
				(pEvent->TrainTip() && m_FilterFlags & train_tip)	||
				(pEvent->TrainEnd() && m_FilterFlags & train_end) )
				m_JackOnTrigger.Pulse();
			break;
		case EventFilter::Type::traintip_both:
			if( m_FilterFlags & train_north					||
				m_FilterFlags & train_south					||
				m_FilterFlags & wheelframe					||
				m_FilterFlags & train_tip					||
				m_FilterFlags & train_end )
				m_JackOnTrigger.Pulse();
			break;
		case EventFilter::Type::any:
			m_JackOnTrigger.Pulse();
			break;
		default:
		{
			std::ostringstream stream;
			stream << " Unknown enumeration value! ("<< static_cast<int>(pEvent->GetType()) << ')' << std::endl;
			stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
			throw std::invalid_argument( stream.str() );
		}
		}
	}
}

int SensorFilterJack_Imp::GetEffect() const noexcept{
	return m_FilterFlags;
}

void SensorFilterJack_Imp::SetEffect( int effect ) noexcept{
	m_FilterFlags = effect;
}

Jack& SensorFilterJack_Imp::JackOnTrigger() noexcept{
	return m_JackOnTrigger;
}

void SensorFilterJack_Imp::RegisterSockets( SocketRegistry& module ){
	module.ConnectJack( m_JackOnTrigger );
}

void SensorFilterJack_Imp::UnregisterSockets( SocketRegistry& module ){
	module.RemoveJack( m_JackOnTrigger );
}

TrackBuilder* SensorFilterJack_Imp::TrackAttached() const noexcept{
	return m_pTrackBuilder;
}

int SensorFilterJack_Imp::CountJacks() const noexcept{
	return 1;
}

void SensorFilterJack_Imp::Attach( TrackBuilder* pTrackBuilder ) noexcept{
	m_pTrackBuilder = pTrackBuilder;
}

const Jack& SensorFilterJack_Imp::_GetJack( int idx ) const{
	if( idx == 0 )
		return m_JackOnTrigger;

	std::ostringstream stream;
	stream << "Out of range!" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::range_error( stream.str() );
}
///////////////////////////////////////
std::string ToString( SensorFilterJack::Effect effect ){
	switch( effect ){
	case SensorFilterJack::none:
		return "none";
	case SensorFilterJack::train_tip:
		return "train_tip";
	case SensorFilterJack::train_end:
		return "train_end";
	case SensorFilterJack::train_north:
		return "train_north";
	case SensorFilterJack::train_south:
		return "train_south";
	case SensorFilterJack::wheelframe:
		return "wheelframe";
	case SensorFilterJack::name:
		return "name";
	case SensorFilterJack::not_name:
		return "not_name";
	case SensorFilterJack::any:
		return "any";
	default:
		assert( !"Unknown SensorFilterJack::Effect enumerator!" );
		return "unknown";
	}
}

SensorFilterJack::Effect SensorEffectFromString( const std::string& effect ){
	if( effect == "none" )
		return SensorFilterJack::none;
	else if( effect == "train_tip" )
		return SensorFilterJack::train_tip;
	else if( effect == "train_end" )
		return SensorFilterJack::train_end;
	else if( effect == "train_north" )
		return SensorFilterJack::train_north;
	else if( effect == "train_south" )
		return SensorFilterJack::train_south;
	else if( effect == "wheelframe" )
		return SensorFilterJack::wheelframe;
	else if( effect == "name" )
		return SensorFilterJack::name;
	else if( effect == "not_name" )
		return SensorFilterJack::not_name;
	else if( effect == "any" )
		return SensorFilterJack::any;

	std::ostringstream stream;
	stream << "Invalid SensorFilterJack::Effect string: " << effect << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::invalid_argument( stream.str() );
}
///////////////////////////////////////
*/
}
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

#include "LogicElements_Imp.h"

#include <sstream>

namespace trax{
///////////////////////////////////////
std::unique_ptr<PulseCounter> PulseCounter::Make() noexcept{
	return std::make_unique<PulseCounter_Imp>();
}

PulseCounter_Imp::PulseCounter_Imp()
	:	m_PlugToCountUp			( *this, &PulseCounter_Imp::CountUp ),
		m_PlugToCountDown		( *this, &PulseCounter_Imp::CountDown ),
		m_PlugToReset			( *this, &PulseCounter_Imp::Reset ),
		m_Counter				(0),
		m_Threshold				(0)
{
	m_PlugToCountUp.Reference( "name", "PlugToCountUp" );
	m_PlugToCountDown.Reference( "name", "PlugToCountDown" );
}

void PulseCounter_Imp::CountUp() noexcept{
	m_JackOnCountUp.Pulse();

	if( m_Counter == m_Threshold )
		m_JackOnLeaveThreshold.Pulse();
	if( ++m_Counter == m_Threshold )
		m_JackOnReachThreshold.Pulse();
}

void PulseCounter_Imp::CountDown() noexcept{
	m_JackOnCountDown.Pulse();

	if( m_Counter == m_Threshold )
		m_JackOnLeaveThreshold.Pulse();
	if( --m_Counter == m_Threshold )
		m_JackOnReachThreshold.Pulse();
}

void PulseCounter_Imp::Threshold( int threshold ){
	m_Threshold = threshold;
}

int PulseCounter_Imp::Threshold() const noexcept{
	return m_Threshold;
}

void PulseCounter_Imp::Counter( int counter ){
	m_Counter = counter;
}

int PulseCounter_Imp::Counter() const noexcept{
	return m_Counter;
}

void PulseCounter_Imp::Reset() noexcept{
	m_Counter = 0;
}

MultiPlug& PulseCounter_Imp::PlugToCountUp(){
	return m_PlugToCountUp;
}

MultiPlug& PulseCounter_Imp::PlugToCountDown(){
	return m_PlugToCountDown;
}

MultiPlug& PulseCounter_Imp::PlugToReset(){
	return m_PlugToReset;
}

Jack& PulseCounter_Imp::JackOnReachThreshold(){
	return m_JackOnReachThreshold;
}

Jack& PulseCounter_Imp::JackOnLeaveThreshold(){
	return m_JackOnLeaveThreshold;
}

Jack& PulseCounter_Imp::JackOnCountUp(){
	return m_JackOnCountUp;
}

Jack& PulseCounter_Imp::JackOnCountDown(){
	return m_JackOnCountDown;
}

void PulseCounter_Imp::RegisterSockets( SocketRegistry& registry ){
	registry.RegisterPlug( m_PlugToCountUp );
	registry.RegisterPlug( m_PlugToCountDown );
	registry.RegisterPlug( m_PlugToReset );
	registry.ConnectJack( m_JackOnReachThreshold );
	registry.ConnectJack( m_JackOnLeaveThreshold );
	registry.ConnectJack( m_JackOnCountUp );
	registry.ConnectJack( m_JackOnCountDown );
}

void PulseCounter_Imp::UnregisterSockets( SocketRegistry& registry ){
	registry.RemoveJack( m_JackOnCountDown );
	registry.RemoveJack( m_JackOnCountUp );
	registry.RemoveJack( m_JackOnLeaveThreshold );
	registry.RemoveJack( m_JackOnReachThreshold );
	registry.UnRegisterPlug( m_PlugToReset );
	registry.UnRegisterPlug( m_PlugToCountDown );
	registry.UnRegisterPlug( m_PlugToCountUp );
}

int PulseCounter_Imp::CountPlugs() const{
	return	m_PlugToCountUp.CountPlugs()	+ 
			m_PlugToCountDown.CountPlugs()	+ 
			m_PlugToReset.CountPlugs();
}

int PulseCounter_Imp::CountJacks() const noexcept{
	return 4;
}

const Plug& PulseCounter_Imp::_GetPlug( int idx ) const{
	if( idx < m_PlugToCountUp.CountPlugs() )
		return m_PlugToCountUp.GetPlug( idx );
	idx -= m_PlugToCountUp.CountPlugs();

	if( idx < m_PlugToCountDown.CountPlugs() )
		return m_PlugToCountDown.GetPlug( idx );
	idx -= m_PlugToCountDown.CountPlugs();

	if( idx < m_PlugToReset.CountPlugs() )
		return m_PlugToReset.GetPlug( idx );

	assert( 0 );
	std::ostringstream stream;
	stream << __FILE__ << '(' << __LINE__ << ')' << "Out of range!";
	throw std::range_error( stream.str() );
}

const Jack & PulseCounter_Imp::_GetJack( int idx ) const{
	switch( idx ){
	case 0:
		return m_JackOnReachThreshold;
	case 1:
		return m_JackOnLeaveThreshold;
	case 2:
		return m_JackOnCountUp;
	case 3:
		return m_JackOnCountDown;
	default:
		assert( 0 );
		std::ostringstream stream;
		stream << __FILE__ << '(' << __LINE__ << ')' << "Out of range!";
		throw std::range_error( stream.str() );
	}
}
///////////////////////////////////////
//std::string PulseCounterCollection_Imp::TypeName() const{
//	return "PulseCounterCollection";
//}
///////////////////////////////////////
}
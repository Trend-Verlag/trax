//	trax track library
//	AD 2016 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//
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

#include "Timer_Imp.h"
#include "trax/Units.h"

#include <sstream>

namespace trax{
////////////////////////////////////////
std::unique_ptr<Timer> Timer::Make() noexcept{
	try{
		return std::make_unique<Timer_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
////////////////////////////////////////
Timer_Imp::Timer_Imp()
	:	m_TimeLapse		(fixed_timestep),
		m_bPeriodic		(false),
		m_Timer			(0),
		m_bStarted		(false),
		m_PlugToStart	( *this, &Timer_Imp::Start ),
		m_PlugToReset	( *this, &Timer_Imp::Reset ),
		m_PlugToPause	( *this, &Timer_Imp::Pause )
{
	m_PlugToStart.Reference( "name", "PlugToStart" );
	m_PlugToReset.Reference( "name", "PlugToReset" );
	m_PlugToPause.Reference( "name", "PlugToPause" );
}

const char* Timer_Imp::TypeName() const noexcept{
	return "Timer";
}

void Timer_Imp::Start() noexcept{
	m_bStarted = true;
	m_JackOnStart.Pulse();
}

bool Timer_Imp::IsStarted() const noexcept{
	return m_bStarted;
}

void Timer_Imp::Reset() noexcept{
	m_bStarted = false;
	m_Timer = 0_s;
	m_JackOnReset.Pulse();
}

void Timer_Imp::Pause() noexcept{
	m_bStarted = false;
	m_JackOnPause.Pulse();
}

void Timer_Imp::Lapse( Time timeLapse ){
	if( timeLapse < fixed_timestep )
		throw std::invalid_argument( "A timer's lapse can not be smaller than the simulation time step." );

	m_TimeLapse = timeLapse;
}

Time Timer_Imp::Lapse() const{
	return m_TimeLapse;
}

void Timer_Imp::Periodic( bool bPeriodic ){
	m_bPeriodic = bPeriodic;
}

bool Timer_Imp::Periodic() const{
	return m_bPeriodic;
}

void Timer_Imp::SetTime( Time time ){
	m_Timer = time;
}

Time Timer_Imp::GetTime() const{
	return m_Timer;
}

MultiPlug& Timer_Imp::PlugToStart() noexcept{
	return m_PlugToStart;
}

MultiPlug & Timer_Imp::PlugToReset() noexcept{
	return m_PlugToReset;
}

MultiPlug& Timer_Imp::PlugToPause() noexcept{
	return m_PlugToPause;
}

Jack& Timer_Imp::JackOnStart() noexcept{
	return m_JackOnStart;
}

Jack& Timer_Imp::JackOnReset() noexcept{
	return m_JackOnReset;
}

Jack& Timer_Imp::JackOnPause() noexcept{
	return m_JackOnPause;
}

Jack& Timer_Imp::JackOnTick() noexcept{
	return m_JackOnTick;
}

void Timer_Imp::RegisterSockets( SocketRegistry& registry ){
	registry.RegisterPlug( m_PlugToStart );
	registry.RegisterPlug( m_PlugToReset );
	registry.RegisterPlug( m_PlugToPause );
	registry.ConnectJack( m_JackOnStart );
	registry.ConnectJack( m_JackOnReset );
	registry.ConnectJack( m_JackOnPause );
	registry.ConnectJack( m_JackOnTick );
}

void Timer_Imp::UnregisterSockets( SocketRegistry& registry ){
	registry.RemoveJack( m_JackOnTick );
	registry.RemoveJack( m_JackOnPause );
	registry.RemoveJack( m_JackOnReset );
	registry.RemoveJack( m_JackOnStart );
	registry.UnRegisterPlug( m_PlugToPause );
	registry.UnRegisterPlug( m_PlugToReset );
	registry.UnRegisterPlug( m_PlugToStart );
}

const std::string& Timer_Imp::Reference( const std::string & name ) const{
	return TimerBase::Reference( name );
}

IDType Timer_Imp::ID() const noexcept{
	return TimerBase::ID();
}

int Timer_Imp::CountPlugs() const{
	return	m_PlugToStart.CountPlugs() + 
			m_PlugToReset.CountPlugs() +
			m_PlugToPause.CountPlugs();
}

int Timer_Imp::CountJacks() const noexcept{
	return 4;
}

void Timer_Imp::Update(){
	if( m_bStarted ){
		m_Timer += fixed_timestep;
		if( m_Timer >= m_TimeLapse ){
			m_Timer -= m_TimeLapse;

			if( !m_bPeriodic )
				m_bStarted = false;

			m_JackOnTick.Pulse();
		}
	}
}

const Jack& Timer_Imp::_GetJack( int idx ) const{
	switch( idx ){
	case 0:
		return m_JackOnTick;
	case 1:
		return m_JackOnPause;
	case 2:
		return m_JackOnReset;
	case 3:
		return m_JackOnStart;
	}

	assert( 0 );
	std::ostringstream stream;
	stream << __FILE__ << '(' << __LINE__ << ')' << "Out of range!";
	throw std::range_error( stream.str() );
}

const Plug& Timer_Imp::_GetPlug( int idx ) const{
	if( idx < m_PlugToStart.CountPlugs() )
		return m_PlugToStart.GetPlug( idx );
	idx -= m_PlugToStart.CountPlugs();

	if( idx < m_PlugToReset.CountPlugs() )
		return m_PlugToReset.GetPlug( idx );
	idx -= m_PlugToReset.CountPlugs();

	if( idx < m_PlugToPause.CountPlugs() )
		return m_PlugToPause.GetPlug( idx );

	assert( 0 );
	std::ostringstream stream;
	stream << __FILE__ << '(' << __LINE__ << ')' << "Out of range!";
	throw std::range_error( stream.str() );
}
///////////////////////////////////////
//std::string TimerCollection_Imp::TypeName() const{
//	return "TimerCollection";
//}
//
//void TimerCollection_Imp::Update(){
//	for( auto& timer : *this )
//		timer.Update();
//}
///////////////////////////////////////
}
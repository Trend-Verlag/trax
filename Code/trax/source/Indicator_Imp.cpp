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

#include "Indicator_Imp.h"
#include "trax/Plug.h"
#include "trax/Jack.h"
#include "trax/Switch.h"
#include "trax/Signal.h"
#include "SignalSemaphore_Imp.h"
#include "SwitchSemaphore_Imp.h"

#include <iostream>
#include <sstream>
#include <cctype>

namespace trax{
	using namespace common;
///////////////////////////////////////
const char* ToString( Indicator::Type type ){
	switch( type ){
	case Indicator::Type::unknown:
		return "unknown";
	case Indicator::Type::none:
		return "none";
	case Indicator::Type::switch_mono:
		return "switch_mono";
	case Indicator::Type::switch_multi:
		return "switch_multi";
	case Indicator::Type::velocity_control:
		return "velocity_control";
	default:
		assert( !"Unknown Indicator::Type enumerator!" );
		return "unknown";
	}
}

const char* ToString( Indicator::Status status ){
	switch( status ){
	case Indicator::Status::none:
		return "none";
	case Indicator::Status::unknown:
		return "unknown";
	case Indicator::Status::one:
		return "one";
	case Indicator::Status::two:
		return "two";
	case Indicator::Status::three:
		return "three";
	case Indicator::Status::four:
		return "four";
	case Indicator::Status::five:
		return "five";
	case Indicator::Status::six:
		return "six";
	case Indicator::Status::seven:
		return "seven";
	case Indicator::Status::eight:
		return "eight";
	case Indicator::Status::nine:
		return "nine";
	case Indicator::Status::ten:
		return "ten";
	case Indicator::Status::eleven:
		return "eleven";
	case Indicator::Status::twelve:
		return "twelve";
	case Indicator::Status::thirteen:
		return "thirteen";
	case Indicator::Status::toggle:
		return "toggle";
	case Indicator::Status::change:
		return "change";
	default:
		assert( !"Unknown Indicator::Status enumerator!" );
		return "unknown";
	}
}

Indicator::Status ToIndicatorStatus( const char* status ){
	if( strcmp( status, "none" ) == 0 )
		return Indicator::Status::none;
	else if( strcmp( status, "unknown" ) == 0 )
		return Indicator::Status::unknown;
	else if( strcmp( status, "one" ) == 0 )
		return Indicator::Status::one;
	else if( strcmp( status, "two" ) == 0 )
		return Indicator::Status::two;
	else if( strcmp( status, "three" ) == 0 )
		return Indicator::Status::three;
	else if( strcmp( status, "four" ) == 0 )
		return Indicator::Status::four;
	else if( strcmp( status, "five" ) == 0 )
		return Indicator::Status::five;
	else if( strcmp( status, "six" ) == 0 )
		return Indicator::Status::six;
	else if( strcmp( status, "seven" ) == 0 )
		return Indicator::Status::seven;
	else if( strcmp( status, "eight" ) == 0 )
		return Indicator::Status::eight;
	else if( strcmp( status, "nine" ) == 0 )
		return Indicator::Status::nine;
	else if( strcmp( status, "ten" ) == 0 )
		return Indicator::Status::ten;
	else if( strcmp( status, "eleven" ) == 0 )
		return Indicator::Status::eleven;
	else if( strcmp( status, "twelve" ) == 0 )
		return Indicator::Status::twelve;
	else if( strcmp( status, "thirteen" ) == 0 )
		return Indicator::Status::thirteen;
	else if( strcmp( status, "toggle" ) == 0 )
		return Indicator::Status::toggle;
	else if( strcmp( status, "change" ) == 0 )
		return Indicator::Status::change;

	return Indicator::Status::unknown;
}

Indicator::Status IndicatorStatusFrom( const char* socketName ){
	if( strcmp( socketName, "JackOnOne" ) == 0 ||
		strcmp( socketName, "PlugToOne" ) == 0 )
		return Indicator::Status::one;
	else if( strcmp( socketName, "JackOnTwo" ) == 0 ||
				strcmp( socketName, "PlugToTwo" ) == 0 )
		return Indicator::Status::two;
	else if( strcmp( socketName, "JackOnThree" ) == 0 ||
		strcmp( socketName, "PlugToThree" ) == 0 )
		return Indicator::Status::three;
	else if( strcmp( socketName, "JackOnFour" ) == 0 ||
		strcmp( socketName, "PlugToFour" ) == 0 )
		return Indicator::Status::four;
	else if( strcmp( socketName, "JackOnFive" ) == 0 ||
		strcmp( socketName, "PlugToFive" ) == 0 )
		return Indicator::Status::five;
	else if( strcmp( socketName, "JackOnSix" ) == 0 ||
		strcmp( socketName, "PlugToSix" ) == 0 )
		return Indicator::Status::six;
	else if( strcmp( socketName, "JackOnSeven" ) == 0 ||
		strcmp( socketName, "PlugToSeven" ) == 0 )
		return Indicator::Status::seven;
	else if( strcmp( socketName, "JackOnEight" ) == 0 ||
		strcmp( socketName, "PlugToEight" ) == 0 )
		return Indicator::Status::eight;
	else if( strcmp( socketName, "JackOnNine" ) == 0 ||
		strcmp( socketName, "PlugToNine" ) == 0 )
		return Indicator::Status::nine;
	else if( strcmp( socketName, "JackOnTen" ) == 0 ||
		strcmp( socketName, "PlugToTen" ) == 0 )
		return Indicator::Status::ten;
	else if( strcmp( socketName, "JackOnEleven" ) == 0 ||
		strcmp( socketName, "PlugToEleven" ) == 0 )
		return Indicator::Status::eleven;
	else if( strcmp( socketName, "JackOnTwelve" ) == 0 ||
		strcmp( socketName, "PlugToTwelve" ) == 0 )
		return Indicator::Status::twelve;
	else if( strcmp( socketName, "JackOnThirteen" ) == 0 ||
		strcmp( socketName, "PlugToThirteen" ) == 0 )
		return Indicator::Status::thirteen;
	else if( strcmp( socketName, "JackOnChange" ) == 0 )
		return Indicator::Status::change;
	else if( strcmp( socketName, "PlugToToggle" ) == 0 )
		return Indicator::Status::toggle;

	return Indicator::Status::unknown;
}
///////////////////////////////////////
Multicator::Multicator()
	:	m_Status				{ Status::none },
		m_StatusChangeBlocker	{ false },
		m_PlugToToggle			{ *this }
{
	m_PlugToToggle.Reference( "name", "PlugToToggle" );
}

void Multicator::Toggle( bool pulse ){
	auto iter = m_IndicatorStates.begin();
	for( ; iter != m_IndicatorStates.end(); ++iter ){
		if( iter->first == Get() ){
			if( ++iter == m_IndicatorStates.end() )
				iter = m_IndicatorStates.begin();

			break;
		}
	}

	assert( iter != m_IndicatorStates.end() );
	Set( iter->first, pulse );
}

Indicator::Status Multicator::Set( Status status, bool pulse ){
	if( status >= Status::count )
		throw std::invalid_argument( "Invalid Indicator status value!" );

	if( m_StatusChangeBlocker )
	// several different indicator status might relate to
	// the same signal status. Make sure the first selected 
	// status stays valid, even if the signal might come back
	// to us here with another status.
		return m_Status;

	const Status retval = m_Status;

	//if( m_Status != status ) //we need pulses to transire, so do the set in any case
	{
		if( status == Status::none ){
			m_Status = status;
			return retval;
		}

		Create( status );

		m_Status = status;
		if( pulse ){
			m_StatusChangeBlocker = true;
			JackOn( m_Status ).Pulse();
			JackOnChange().Pulse();
			m_StatusChangeBlocker = false;
		}
	}

	return retval;
}

Indicator::Status Multicator::Get() const{
	return m_Status;
}

void Multicator::Get( spat::Frame<Length, One>& frame ) const
{
	auto iter = m_IndicatorStates.find( m_Status );
	if( iter == m_IndicatorStates.end() )
		throw std::runtime_error( "Status not yet created for Multicator!" );

	frame = GetFrame() * iter->second.m_LocalFrame;
}

bool Multicator::IsValidState( Status status ) const{
	return m_IndicatorStates.find( status ) != m_IndicatorStates.end();
}

void Multicator::LocalFrameForStatus( Status status, const spat::Frame<Length, One>& frame )
{
	Create( status );

	auto iter = m_IndicatorStates.find( status );
	if( iter == m_IndicatorStates.end() )
		throw std::runtime_error( "Status not yet created for Multicator!" );

	iter->second.m_LocalFrame = frame;
}

const spat::Frame<Length, One>& Multicator::LocalFrameForStatus( Status status ) const
{
	auto iter = m_IndicatorStates.find( status );
	if( iter == m_IndicatorStates.end() )
		throw std::runtime_error( "Status not yet created for Multicator!" );

	return iter->second.m_LocalFrame;
}

void Multicator::RotateWithStatus( Status status, Real angle ){
	auto iter = m_IndicatorStates.find( status );
	if( iter == m_IndicatorStates.end() )
		throw std::runtime_error( "Status not yet created for Multicator!" );

	iter->second.m_RotAngle = angle;
}

Real Multicator::RotateWithStatus(Status status) const{
	auto iter = m_IndicatorStates.find( status );
	if( iter == m_IndicatorStates.end() )
		return 0;

	return iter->second.m_RotAngle;
}

Jack& Multicator::JackOn( Status status ){
	switch( status ){
	case Status::one:
	case Status::two:
	case Status::three:
	case Status::four:
	case Status::five:
	case Status::six:
	case Status::seven:
	case Status::eight:
	case Status::nine:
	case Status::ten:
	case Status::eleven:
	case Status::twelve:
	case Status::thirteen:
		Create( status );
		return m_IndicatorStates.at( status ).m_JackOnStatus;
	case Status::change:
		return m_JackOnChange;
	default:
		assert( 0 );
		std::ostringstream stream;
		stream << "No status for jack!" << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::range_error( stream.str() );
	}
}

Plug& Multicator::PlugTo( Status status ){
	switch( status ){
	case Status::one:
	case Status::two:
	case Status::three:
	case Status::four:
	case Status::five:
	case Status::six:
	case Status::seven:
	case Status::eight:
	case Status::nine:
	case Status::ten:
	case Status::eleven:
	case Status::twelve:
	case Status::thirteen:
		Create( status );
		return m_IndicatorStates.at(status).m_PlugToStatus;
	case Status::toggle:
		return m_PlugToToggle;
	default:
		assert( 0 );
		std::ostringstream stream;
		stream << "No status for plug!" << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::range_error( stream.str() );
	}
}

Plug& Multicator::PlugToToggle() noexcept{
	return m_PlugToToggle;
}

Jack& Multicator::JackOnChange() noexcept{
	return m_JackOnChange;
}

void Multicator::RegisterSockets( SocketRegistry& registry ){
	registry.RegisterPlug( m_PlugToToggle );
	registry.ConnectJack( m_JackOnChange );

	for( auto& pair : m_IndicatorStates ){
		registry.RegisterPlug( pair.second.m_PlugToStatus );
		registry.ConnectJack( pair.second.m_JackOnStatus );
	}
}

void Multicator::UnregisterSockets( SocketRegistry& registry ){
	registry.UnRegisterPlug( m_PlugToToggle );
	registry.RemoveJack( m_JackOnChange );

	for( auto& pair : m_IndicatorStates ){
		registry.UnRegisterPlug( pair.second.m_PlugToStatus );
		registry.RemoveJack( pair.second.m_JackOnStatus );
	}
}

void Multicator::RefTargetID( IDType id ) noexcept{
	m_RefTargetID = id;
}

IDType Multicator::RefTargetID() const noexcept{
	return m_RefTargetID;
}

void Multicator::UserData( IndicatorUserData* pData ) noexcept{
	m_pUserData = pData;
}

IndicatorUserData * Multicator::UserData() const noexcept{
	return m_pUserData;
}

const char* Multicator::TypeName() const noexcept{
	return "Multicator";
}

const char* Multicator::Reference( const char* name ) const{
	return Multicator_Base::Reference( name );
}

IDType Multicator::ID() const noexcept{
	return Multicator_Base::ID();
}

int Multicator::CountPlugs() const{
	int count = 1;

	for( const auto& pair : m_IndicatorStates )
		count += pair.second.m_PlugToStatus.CountPlugs();

	return count;
}

int Multicator::CountJacks() const{
	return narrow_cast<int>(1 + m_IndicatorStates.size());
}

const Plug& Multicator::_GetPlug( int idx ) const{
	if( idx == 0 )
		return m_PlugToToggle;
	idx -= 1;

	for( const auto& pair : m_IndicatorStates ){
		if( idx < pair.second.m_PlugToStatus.CountPlugs() )
			return pair.second.m_PlugToStatus.GetPlug( idx );
		idx -= pair.second.m_PlugToStatus.CountPlugs();
	}

	assert( 0 );
	std::ostringstream stream;
	stream << "Unknown index for plug!" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::out_of_range( stream.str() );
}

const Jack& Multicator::_GetJack( int idx ) const{
	if( idx == 0 )
		return m_JackOnChange;
	idx -= 1;

	for( const auto& pair : m_IndicatorStates ){
		if( idx-- == 0 )
			return pair.second.m_JackOnStatus;
	}

	assert( 0 );
	std::ostringstream stream;
	stream << "Unknown index for jack!" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::out_of_range( stream.str() );
}

void Multicator::Create( Status status ){
	if( m_IndicatorStates.find( status ) == m_IndicatorStates.end() ){
		// insert into map in a way that does not copy or move.
		// This is needed, to supply the parent by reference and to
		// keep our references to Plugs and Jacks valid.
		m_IndicatorStates.emplace( std::piecewise_construct,
			std::forward_as_tuple( status ),
			std::forward_as_tuple( *this, status ) );
	}
}

Multicator::IndicatorState::IndicatorState( 
	Multicator& parent, Indicator::Status status )
	:	m_RotAngle		{0},
		m_JackOnStatus	{ "" },
		m_PlugToStatus	{parent}
{
	std::string nStatus{ ToString(status) };
	nStatus[0] = narrow_cast<char>(std::toupper( nStatus[0] ));

	m_JackOnStatus.Reference( "name", "JackOn" + nStatus );
	m_PlugToStatus.Reference( "name", "PlugTo" + nStatus );
	m_PlugToStatus.SetStatus( status );
}
////////////////////////////////////////
std::unique_ptr<Indicator> Indicator::Make( Type type ) noexcept{
	try{
		switch( type ){
			case Type::switch_mono:
				return std::make_unique<SwitchSemaphore>();
			case Type::switch_multi:
				return std::make_unique<SwitchMultiSemaphore>();
			case Type::velocity_control:
				return std::make_unique<VelocityControlSemaphore>();
			case Type::multicator:
				return std::make_unique<Multicator>();
			case Type::unknown:
			case Type::none:
			default:
				return nullptr;
		}
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
////////////////////////////////////////
std::unique_ptr<BinaryIndicator> BinaryIndicator::Make( Type type ) noexcept{
	try{
		switch( type ){
			case Type::switch_semaphore:
				return std::make_unique<SwitchSemaphore_Imp>();
			case Type::switch_mono:
				return std::make_unique<SwitchSemaphore>();
			case Type::switch_multi:
			case Type::velocity_control:
				std::cerr << "BinaryIndicator::Make: Invalid IndicatorType! (" << ToString(type) << ")" << std::endl;
				[[fallthrough]];
			case Type::unknown:
			case Type::none:
			default:
				return nullptr;
		}
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
////////////////////////////////////////
}

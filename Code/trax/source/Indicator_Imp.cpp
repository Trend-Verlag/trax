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
std::string ToString( Indicator::Type type ){
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

std::string ToString( Indicator::Status status ){
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

Indicator::Status ToIndicatorStatus( const std::string& status ){
	if( status == "none" )
		return Indicator::Status::none;
	else if( status == "unknown" )
		return Indicator::Status::unknown;
	else if( status == "one" )
		return Indicator::Status::one;
	else if( status == "two" )
		return Indicator::Status::two;
	else if( status == "three" )
		return Indicator::Status::three;
	else if( status == "four" )
		return Indicator::Status::four;
	else if( status == "five" )
		return Indicator::Status::five;
	else if( status == "six" )
		return Indicator::Status::six;
	else if( status == "seven" )
		return Indicator::Status::seven;
	else if( status == "eight" )
		return Indicator::Status::eight;
	else if( status == "nine" )
		return Indicator::Status::nine;
	else if( status == "ten" )
		return Indicator::Status::ten;
	else if( status == "eleven" )
		return Indicator::Status::eleven;
	else if( status == "twelve" )
		return Indicator::Status::twelve;
	else if( status == "thirteen" )
		return Indicator::Status::thirteen;
	else if( status == "toggle" )
		return Indicator::Status::toggle;
	else if( status == "change" )
		return Indicator::Status::change;

	return Indicator::Status::unknown;
}

Indicator::Status IndicatorStatusFrom( const std::string& socketName ){
	if( socketName == "JackOnOne" ||
		socketName == "PlugToOne" )
		return Indicator::Status::one;
	else if(	socketName == "JackOnTwo" ||
				socketName == "PlugToTwo" )
		return Indicator::Status::two;
	else if( socketName == "JackOnThree" ||
		socketName == "PlugToThree" )
		return Indicator::Status::three;
	else if( socketName == "JackOnFour" ||
		socketName == "PlugToFour" )
		return Indicator::Status::four;
	else if( socketName == "JackOnFive" ||
		socketName == "PlugToFive" )
		return Indicator::Status::five;
	else if( socketName == "JackOnSix" ||
		socketName == "PlugToSix" )
		return Indicator::Status::six;
	else if( socketName == "JackOnSeven" ||
		socketName == "PlugToSeven" )
		return Indicator::Status::seven;
	else if( socketName == "JackOnEight" ||
		socketName == "PlugToEight" )
		return Indicator::Status::eight;
	else if( socketName == "JackOnNine" ||
		socketName == "PlugToNine" )
		return Indicator::Status::nine;
	else if( socketName == "JackOnTen" ||
		socketName == "PlugToTen" )
		return Indicator::Status::ten;
	else if( socketName == "JackOnEleven" ||
		socketName == "PlugToEleven" )
		return Indicator::Status::eleven;
	else if( socketName == "JackOnTwelve" ||
		socketName == "PlugToTwelve" )
		return Indicator::Status::twelve;
	else if( socketName == "JackOnThirteen" ||
		socketName == "PlugToThirteen" )
		return Indicator::Status::thirteen;
	else if( socketName == "JackOnChange" )
		return Indicator::Status::change;
	else if( socketName == "PlugToToggle" )
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

bool Multicator::IsValidState( Status status ) const{
	return m_IndicatorStates.find( status ) != m_IndicatorStates.end();
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

const char* Multicator::TypeName() const noexcept{
	return "Multicator";
}

const std::string& Multicator::Reference( const std::string & name ) const{
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
	throw std::range_error( stream.str() );
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
	throw std::range_error( stream.str() );
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

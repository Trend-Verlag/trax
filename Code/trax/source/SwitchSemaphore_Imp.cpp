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

#include "SwitchSemaphore_Imp.h"
#include "trax/Location.h"

namespace trax{
	using namespace spat;
///////////////////////////////////////
SwitchSemaphore::SwitchSemaphore()
	:	m_Status				{Status::unknown},
		m_bPreserveUpDirection	{false},
		m_RotAngleOne			{0},
		m_RotAngleTwo			{0},
		m_PlugToGo				{*this,Status::one},
		m_PlugToBranch			{*this,Status::two},
		m_PlugToToggle			{*this}
{
	m_PlugToGo.Reference( "name", "PlugToOne" );
	m_PlugToBranch.Reference( "name", "PlugToTwo" );
	m_PlugToToggle.Reference( "name", "PlugToToggle" );
}

void SwitchSemaphore::Attach( std::shared_ptr<Connector> pSwitch ) noexcept{
	if( (m_pSwitch = pSwitch) != nullptr ){
	//	Set( m_pSwitch->Get() == Switch::go ? one : two, false );

//		Connect( *this, *m_pSwitch );
	}
}

bool SwitchSemaphore::PreserveUpDirection() const noexcept{
	return m_bPreserveUpDirection;
}

void SwitchSemaphore::PreserveUpDirection( bool bPreserveUpDirection ) noexcept{
	m_bPreserveUpDirection = bPreserveUpDirection;
}

IDType SwitchSemaphore::GetSwitchRef() const noexcept{
	if( m_pSwitch )
		return m_pSwitch->ID();

	return 0u;
}

std::shared_ptr<Connector> SwitchSemaphore::GetSwitch() const noexcept{
	return m_pSwitch;
}

const char* SwitchSemaphore::TypeName() const noexcept{
	return "SwitchSemaphore";
}

void SwitchSemaphore::Get( Frame<Length,One>& frame ) const{
	if( m_pSwitch ){

		Frame<Length,One> center;
		m_pSwitch->GetCenter( center );

		frame = center * GetFrame();

		if( m_bPreserveUpDirection ){
			if( !frame.IsOrthoNormal() )
				frame.OrthoNormalize();

			frame.LookBAt( Up );
		}

		frame.RotateBin( RotateWithStatus( Get() ) );


		//if( m_bRotateWithStatus ){
		//	if( m_bReverseRotation )
		//		frame.RotateBin( (m_Status == one ? 0.0f : -1.0f) * pi/2 );
		//	else
		//		frame.RotateBin( (m_Status == one ? -1.0f : 0.0f) * pi/2 );
		//}

		//if( auto pLocation = m_pSwitch->Bifurcation() ){
		//	pLocation->Get( base );

		//	//const Switch::Status status 
		//	//	= m_pSwitch->Set( Switch::branch, false );

		//	//pLocation->Move( m_pSwitch->DivergedTrack()->Length() );
		//	//m_pSwitch->Set( status, false );

		//	//spat::Position<trax::Real> EndPosDiverged;
		//	//pLocation->Get( EndPosDiverged );
		//	//base.FromParent( EndPosDiverged );

		//	//if( Sign( EndPosDiverged.y ) < 0 )
		//	//	base.RotateTan( pi );

		//	frame = base * GetFrame();

		//	if( m_bPreserveUpDirection )
		//		frame.LookBAt( Up );

		//	if( m_bRotateWithStatus ){
		//		if( m_bReverseRotation )
		//			frame.RotateBin( (m_Status == one ? 0.0f : -1.0f) * pi/2 );
		//		else
		//			frame.RotateBin( (m_Status == one ? -1.0f : 0.0f) * pi/2 );
		//	}
		//}
	}
}

BinaryIndicator::Status SwitchSemaphore::Set( Status status, bool pulse ){
	const Status Retval = m_Status;

	//if( m_Status != status ) //we need pulses to transire, so do the set in any case
	{
		m_Status = status;

		if( pulse ){
			JackOn( m_Status ).Pulse();
			JackOnChange().Pulse();
		}
	}

	return Retval;
}

void SwitchSemaphore::Toggle( bool pulse ){
	Set( m_Status == Status::one ? Status::two : Status::one, pulse );
}

BinaryIndicator::Status SwitchSemaphore::Get() const noexcept{
	return m_Status;
}

bool SwitchSemaphore::IsValidState( Status status ) const noexcept{
	return status == Status::one || status == Status::two;
}

void SwitchSemaphore::RotateWithStatus( Status status, Real angle ) noexcept{
	switch( status ){
	case Status::one:
		m_RotAngleOne = angle;
		break;
	case Status::two:
		m_RotAngleTwo = angle;
		break;
	default:
		break;
	}
}

Real SwitchSemaphore::RotateWithStatus(Status status) const noexcept{
	switch( status ){
	case Status::one:
		return m_RotAngleOne;
	case Status::two:
		return m_RotAngleTwo;
	default:
		return 0;
	}
}

Plug& SwitchSemaphore::PlugToToggle() noexcept{
	return m_PlugToToggle;
}

Jack& SwitchSemaphore::JackOnChange() noexcept{
	return m_JackOnChange;
}

Jack& SwitchSemaphore::JackOn( Status status ){
	switch( status ){
	case Status::one:
		return JackOnOne();
	case Status::two:
		return JackOnTwo();
	case Status::change:
		return JackOnChange();
	default:
		throw std::invalid_argument( "Unknown enumeration value!" );
	}
}

Plug& SwitchSemaphore::PlugTo( Status status ){
	switch( status ){
	case Status::one:
		return PlugToOne();
	case Status::two:
		return PlugToTwo();
	case Status::toggle:
		return PlugToToggle();
	default:
		throw std::invalid_argument( "Unknown enumeration value!" );
	}
}

Jack& SwitchSemaphore::JackOnOne() noexcept{
	return m_JackOnGo;
}

Plug& SwitchSemaphore::PlugToOne() noexcept{
	return m_PlugToGo;
}

Jack& SwitchSemaphore::JackOnTwo() noexcept{
	return m_JackOnBranch;
}

Plug& SwitchSemaphore::PlugToTwo() noexcept{
	return m_PlugToBranch;
}

void SwitchSemaphore::RegisterSockets( SocketRegistry& module ){
	module.RegisterPlug( m_PlugToGo );
	module.RegisterPlug( m_PlugToBranch );
	module.RegisterPlug( m_PlugToToggle );

	module.ConnectJack( m_JackOnGo );
	module.ConnectJack( m_JackOnBranch );
	module.ConnectJack( m_JackOnChange );
}

void SwitchSemaphore::UnregisterSockets( SocketRegistry& module ){
	module.UnRegisterPlug( m_PlugToGo );
	module.UnRegisterPlug( m_PlugToBranch );
	module.UnRegisterPlug( m_PlugToToggle );

	module.RemoveJack( m_JackOnGo );
	module.RemoveJack( m_JackOnBranch );
	module.RemoveJack( m_JackOnChange );
}

const Plug& SwitchSemaphore::_GetPlug( int idx ) const{
	switch( idx ){
	case 0:
		return m_PlugToGo;
	case 1:
		return m_PlugToBranch;
	case 2:
		return m_PlugToToggle;
	default:
		std::ostringstream stream;
		stream << "Out of range!" << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::range_error( stream.str() );
	}
}

const Jack& SwitchSemaphore::_GetJack( int idx ) const{
	switch( idx ){
	case 0:
		return m_JackOnGo;
	case 1:
		return m_JackOnBranch;
	case 2:
		return m_JackOnChange;
	default:
		std::ostringstream stream;
		stream << "Out of range!" << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::range_error( stream.str() );
	}
}

int SwitchSemaphore::CountPlugs() const noexcept{
	return 3;
}

int SwitchSemaphore::CountJacks() const noexcept{
	return 3;
}
///////////////////////////////////////
SwitchMultiSemaphore::SwitchMultiSemaphore() noexcept
	:	m_bPreserveUpDirection{ false }
{
}

const char* SwitchMultiSemaphore::TypeName() const noexcept{
	return "SwitchMultiSemaphore";
}

void SwitchMultiSemaphore::Get( Frame<Length,One>& frame ) const{
	if( m_pSwitch ){
		Frame<Length,One> center;
		m_pSwitch->GetCenter( center );

		frame = center * GetFrame();

		if( m_bPreserveUpDirection ){
			if( !frame.IsOrthoNormal() )
				frame.OrthoNormalize();

			frame.LookBAt( Up );
		}

		frame.RotateBin( RotateWithStatus( Get() ) );


		//switch( Get() ){
		//case one:
		//	frame.RotateBin( (m_bReverseRotation ? -1.0f : +1.0f) * pi/2 );
		//	break;
		//case two:
		//	break;
		//case three:
		//	frame.RotateBin( (m_bReverseRotation ? -1.0f : +1.0f) * pi );
		//	break;
		//case four:
		//	break;
		//default:
		//	assert( !"State not supported yet!" );
		//};
	
	}
}

void SwitchMultiSemaphore::Attach( std::shared_ptr<Connector> pSwitch ) noexcept{
	m_pSwitch = pSwitch;
}

bool SwitchMultiSemaphore::PreserveUpDirection() const noexcept{
	return m_bPreserveUpDirection;
}

void SwitchMultiSemaphore::PreserveUpDirection( bool bPreserveUpDirection ) noexcept{
	m_bPreserveUpDirection = bPreserveUpDirection;
}

IDType SwitchMultiSemaphore::GetSwitchRef() const noexcept{
	if( m_pSwitch )
		return m_pSwitch->ID();

	return 0u;
}

std::shared_ptr<Connector> SwitchMultiSemaphore::GetSwitch() const noexcept{
	return m_pSwitch;
}
///////////////////////////////////////
}

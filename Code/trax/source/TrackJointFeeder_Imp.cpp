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

#include "trax/TrackJointFeeder.h"
#include "trax/Wheelset.h"
#include "trax/TractionForceCharacteristic.h"

#include "trax/Plug.h"

#include <algorithm>

namespace trax{
///////////////////////////////////////
TrackJointFeeder::TrackJointFeeder( Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept
	:	m_EngineMetersPerUnit	{ engine_meters_per_unit },
		m_EngineKilogramsPerUnit{ engine_kilograms_per_unit },
		m_WakeupFlag			{ false }
{
}

bool TrackJointFeeder::Update( Time dt )
{
	if( IsMovableBodySleeping() && 
		IsRailed() && 
		m_Location.GetTrack()->GetMovableTrack() && 
		m_Location.GetTrack()->GetMovableTrack()->IsMoving() )
		WakeUp();

	if( m_WakeupFlag ){
		m_WakeupFlag = false;
		DoWakeUp();
	}

	if( IsRailed() && !IsMovableBodySleeping() )
	{
		SetTimeStep( dt );

		UpdateBodies();

		const Length Ds = ds();
		if( dt > 0_s )
			m_CurrentVelocity = Ds/dt;
		else
			m_CurrentVelocity = std::numeric_limits<Velocity>::infinity();

		if( MoveBy( Ds ).first ){
			DeRail();
			std::clog << "trax: TrackJoint derailed over open end!" << std::endl;
			return false;
		}
		else{
			TrackData<Real> TrackData;
			m_Location.Transition( TrackData, EngineMetersPerUnit() );

			if( !SetValues( TrackData ) ){
				DeRail();
				return false;
			}
		}
	}
	else
		m_CurrentVelocity = 0_mIs;

	if( m_bRolling != !common::Equals( m_CurrentVelocity, 0_mIs, epsilon__velocity ) ){
		m_bRolling = !m_bRolling;
		m_bRolling ? m_JackOnRollingStart.Pulse() : m_JackOnRollingStop.Pulse();
	}

	return true;
}

bool TrackJointFeeder::Rail( std::shared_ptr<Track> pTrack, const TrackLocation& tl, bool doPulse )
{
	try{
		m_Location.PutOn( pTrack, tl );

		if( m_Location.Orient() == Orientation::Value::anti )
			TurnAnchor();

		TrackData<Real> TrackData;
		m_Location.Transition( TrackData, EngineMetersPerUnit() );
		SetValues( TrackData );

		if( TrackData.wF.T * GlobalAnchor().T < 0 )
			std::cerr << "Railing appears to be in the wrong direction. Is this intended?" << std::endl;

		DoRail();

		if( doPulse )
			m_JackOnRail.Pulse();

		return true;
	}
	catch( const std::out_of_range& )
	{
		return false;
	}
}

bool TrackJointFeeder::Rail( const Location& location, bool doPulse )
{
	TrackLocation tl;
	location.Get( tl );
	return Rail( location.GetTrack(), tl, doPulse );
}

void TrackJointFeeder::DeRail( bool doPulse )
{
	if( IsRailed() )
	{
		if( m_Location.Orient() == Orientation::Value::anti )
			TurnAnchor();

		m_Location.PutOff();
		DoDerail();

		if( doPulse )
			m_JackOnDerail.Pulse();
	}
}

bool TrackJointFeeder::IsRailed() const noexcept
{
	return m_Location.IsOnTrack();
}

bool TrackJointFeeder::IsMovableBodySleeping() const noexcept
{
	return DoIsMovableBodySleeping();
}

void TrackJointFeeder::WakeUp() noexcept{
	m_WakeupFlag = true;
}

void TrackJointFeeder::SetVelocity( Velocity velocity ){
	m_CurrentVelocity = velocity;
}

Velocity TrackJointFeeder::GetVelocity() const noexcept{
	return m_CurrentVelocity;
}

const Location& TrackJointFeeder::GetLocation() const noexcept{
	return m_Location;
}

Jack& trax::TrackJointFeeder::JackOnRail() noexcept{
	return m_JackOnRail;
}

Jack& trax::TrackJointFeeder::JackOnDerail() noexcept{
	return m_JackOnDerail;
}

Jack& trax::TrackJointFeeder::JackOnRollingStart() noexcept{
	return m_JackOnRollingStart;
}

Jack& trax::TrackJointFeeder::JackOnRollingStop() noexcept{
	return m_JackOnRollingStop;
}

Jack& trax::TrackJointFeeder::JackOnStartTilting() noexcept{
	return m_JackOnStartTilting;
}

Jack& trax::TrackJointFeeder::JackOnStopTilting() noexcept{
	return m_JackOnStopTilting;
}

Jack& trax::TrackJointFeeder::JackOnTrackTransition() noexcept{
	return m_JackOnTrackTransition;
}


std::pair<Length,bool> TrackJointFeeder::MoveBy( Length ds )
{
	if( IsRailed() )
	{
		const Orientation oldOrientation = m_Location.Orient();
		auto retval = m_Location.Move( ds );
		if( retval.second ){
			if( m_Location.Orient() != oldOrientation )
				TurnAnchor();
			TrackBodyChange( (retval.first == 0_m && m_Location.GetTrack()) ? m_Location.GetTrack()->GetBody() : nullptr );
			m_JackOnTrackTransition.Pulse();
		}

		return retval;
	}

	return { 0_m, false };
}
///////////////////////////////////////
TrackJointFeederMotorModel::TrackJointFeederMotorModel( Real engine_meters_per_unit, Real engine_kilograms_per_unit )
	:	TrackJointFeeder{engine_meters_per_unit,engine_kilograms_per_unit},
		m_pTFC			{TractionForceCharacteristic::Make()},
		m_ThrustFactor	{0},
		m_BrakingFactor	{0},
		m_TargetVelocity{0},
		m_bMotorAction	{false},
		m_bBrakeAction	{false}
{
}

bool TrackJointFeederMotorModel::Update( Time dt )
{
	bool retval = TrackJointFeeder::Update( dt );

	if( m_bMotorAction != CalculateMotorAction() ){
		m_bMotorAction = !m_bMotorAction;
		m_bMotorAction ? m_JackOnMotorActive.Pulse() : m_JackOnMotorInactive.Pulse();
	}

	if( m_bBrakeAction != CalculateBrakeAction() ){
		m_bBrakeAction = !m_bBrakeAction;
		m_bBrakeAction ? m_JackOnBrakeActive.Pulse() : m_JackOnBrakeInactive.Pulse();
	}

	UpdateMotorValues();

	return retval;
}

void TrackJointFeederMotorModel::TargetVelocity( Velocity vTarget ) noexcept{
	m_TargetVelocity = vTarget;	
	UpdateMotorValues();
}

Velocity TrackJointFeederMotorModel::TargetVelocity() const noexcept{
	return m_TargetVelocity;
}

Power TrackJointFeederMotorModel::MaximumPowerOutput() const noexcept{
	return MaxPossibleWheelThrust() * m_pTFC->MaximumPowerFraction();
}

Power TrackJointFeederMotorModel::CurrentPowerOutput() const noexcept{
	return m_bMotorAction ? Thrust() * MaxWheelThrust() * abs(GetVelocity()) : 0_W;
}

Velocity TrackJointFeederMotorModel::MaximumVelocity() const noexcept{
	return m_pTFC->MaximumVelocity();
}

void TrackJointFeederMotorModel::Thrust( One byfactor ) noexcept{
	m_ThrustFactor = std::clamp( byfactor, One{0}, One{1} );
	UpdateMotorValues();
}

One TrackJointFeederMotorModel::Thrust() const noexcept{
	return m_ThrustFactor;
}

void TrackJointFeederMotorModel::ThrustAbsolute( Force thrust ) noexcept{
	if( const Force maxthrust = MaxThrust() )
		Thrust( thrust/maxthrust );
}

Force TrackJointFeederMotorModel::ThrustAbsolute() const noexcept{
	return m_ThrustFactor * MaxThrust();
}

Force TrackJointFeederMotorModel::MaxThrust() const noexcept{
	return MaxWheelThrust();
}

Force TrackJointFeederMotorModel::MaxThrust( Velocity velocity ) const noexcept{
	return Characteristic( velocity ) * MaxPossibleWheelThrust();
}

void TrackJointFeederMotorModel::Brake( One byfactor ) noexcept{
	m_BrakingFactor = std::clamp( byfactor, One{0}, One{1} );	
	UpdateMotorValues();
}

One TrackJointFeederMotorModel::Brake() const noexcept{
	return m_BrakingFactor;
}

void TrackJointFeederMotorModel::BrakeAbsolute( Force brake ) noexcept{
	if( const Force maxbraking = MaxBrake() )
		Brake( brake/maxbraking );
}

Force TrackJointFeederMotorModel::BrakeAbsolute() const noexcept{
	return m_BrakingFactor * MaxBrake();
}

Force TrackJointFeederMotorModel::MaxBrake() const noexcept{
	return MaxWheelBraking();
}

void TrackJointFeederMotorModel::SetTractionForceCharacteristic(
	std::shared_ptr<const TractionForceCharacteristic> pCharacteristic )
{
	m_pTFC = pCharacteristic;
	if( !m_pTFC )
	{
		m_pTFC = TractionForceCharacteristic::Make();
	}
}

std::shared_ptr<const TractionForceCharacteristic> 
TrackJointFeederMotorModel::GetTractionForceCharacteristic() const noexcept
{
	return m_pTFC;
}

int TrackJointFeederMotorModel::Attach( const Wheelset& ws ){
	if( IsRailed() )
		throw std::logic_error( "Can not change railed TrackJointFeeder" );
	if( ws.Radius <= 0_m )
		throw std::invalid_argument( "Wheelset radius has to be > 0m!" );
	if( ws.Flange <= 0_m )
		throw std::invalid_argument( "Wheelset flange has to be > 0m!" );

	m_Wheelsets.push_back( ws );
	
	return common::narrow_cast<int>(m_Wheelsets.size()) - 1;
}

void TrackJointFeederMotorModel::DetachAllWheelsets() noexcept
{
	m_Wheelsets.clear();
}

int TrackJointFeederMotorModel::CntWheelsets() const{
	return common::narrow_cast<int>(m_Wheelsets.size());
}

const Wheelset& TrackJointFeederMotorModel::GetWheelset( int idx ) const{
	return m_Wheelsets.at(idx);
}

Jack& TrackJointFeederMotorModel::JackOnMotorActive() noexcept{
	return m_JackOnMotorActive;
}

Jack& TrackJointFeederMotorModel::JackOnMotorInactive() noexcept{
	return m_JackOnMotorInactive;
}

Jack& TrackJointFeederMotorModel::JackOnBrakeActive() noexcept{
	return m_JackOnBrakeActive;
}

Jack& TrackJointFeederMotorModel::JackOnBrakeInactive() noexcept{
	return m_JackOnBrakeInactive;
}

std::pair<Length, bool> TrackJointFeederMotorModel::MoveBy( Length ds )
{
	for( Wheelset& ws : m_Wheelsets )
	{
		ws.Rotation += ds / ws.Radius;
	}

	return TrackJointFeeder::MoveBy( ds );
}

Force TrackJointFeederMotorModel::MaxPossibleWheelThrust() const noexcept{
	Force maxThrust{0};

	for( const Wheelset& ws : m_Wheelsets ) 
		maxThrust += ws.MaxMotorTorque / ws.Radius;

	return maxThrust;
}

Force TrackJointFeederMotorModel::MaxWheelThrust() const noexcept{
	return Characteristic( GetVelocity() ) * MaxPossibleWheelThrust();
}

Force TrackJointFeederMotorModel::MaxWheelBraking() const noexcept{
	Force maxBraking{0};

	for( const Wheelset& ws : m_Wheelsets )
		maxBraking += ws.MaxBrakingTorque / ws.Radius;

	return maxBraking;
}

Force TrackJointFeederMotorModel::MaxWheelFriction() const noexcept{
	Force maxFriction{0};

	for( const Wheelset& ws : m_Wheelsets ) 
		maxFriction += ws.FrictionTorque / ws.Radius;

	return maxFriction;
}

Real TrackJointFeederMotorModel::Characteristic( Velocity velocity ) const noexcept{
	return (*m_pTFC)( velocity );
}

void TrackJointFeederMotorModel::UpdateMotorValues() noexcept
{
	SetMotorTarget( TargetVelocity() );

	const Force brake	= Brake() * MaxWheelBraking() + MaxWheelFriction(); // brakes are supported by friction
	Force thrust = Thrust() * MaxWheelThrust();
	if( thrust > 0_N )
		thrust = std::max( thrust - MaxWheelFriction(), 0_N ); // thrust gets reduced by friction
	
	// Apply the motor to speed up to target velocity,
	// even if we go in the wrong direction (no brakes used
	// in that situation). If we are too fast, use the brakes. 
	// If we stand still then maintain this velocity by using 
	// the brakes only.
	// See CalculateMotorState() and CalculateBrakeState()

	if( TargetVelocity() > epsilon__velocity )
		SetMotorForceLimits( -brake, thrust );
	else if( TargetVelocity() < -epsilon__velocity )
		SetMotorForceLimits( -thrust, brake );
	else
		SetMotorForceLimits( -brake, brake );
}

bool TrackJointFeederMotorModel::CalculateMotorAction() noexcept{
	if( Thrust() * MaxWheelThrust() == 0_N )
		return false;

	if( TargetVelocity() < 0_mIs ){
		if( GetVelocity() > TargetVelocity() + epsilon__velocity )
			return true;
		else
			return false;
	}
	else if( TargetVelocity() > 0_mIs ){
		if( GetVelocity() < TargetVelocity() - epsilon__velocity )
			return true;
		else
			return false;
	}
	
	return false;
}

bool TrackJointFeederMotorModel::CalculateBrakeAction() noexcept{
	if( Brake() * MaxWheelBraking() == 0_N )
		return false;

	if( TargetVelocity() < 0_mIs ){
		if( GetVelocity() > TargetVelocity() - epsilon__velocity )
			return false;
		else
			return true;
	}
	else if( TargetVelocity() > 0_mIs ){
		if( GetVelocity() < TargetVelocity() + epsilon__velocity )
			return false;
		else
			return true;
	}
	
	return !common::Equals( GetVelocity(), 0_mIs, epsilon__velocity );
}
///////////////////////////////////////
}
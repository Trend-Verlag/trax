//	trax track library
//	AD 2023 
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

#include "Bogie_Imp.h"

#include "trax/rigid/Scene.h"
#include "../WheelFrame.h"
#include "trax/rigid/Gestalt.h"
#include "trax/rigid/Joint.h"

#include "dim/support/DimSupportStream.h"

#include <iostream>

namespace trax{
	using namespace spat;
///////////////////////////////////////
std::shared_ptr<Bogie> Bogie::Make( Scene& scene, std::shared_ptr<Gestalt> pGestalt ) noexcept
{
	try{
		std::shared_ptr<Bogie_Imp> pRetval = std::make_shared<Bogie_Imp>( scene, pGestalt );
		
		if( pRetval )
			pRetval->SetWeakPointerToSelf( pRetval );

		return pRetval;
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
const Angle Bogie_Imp::scm_BendingAngle = pi/64;

Bogie_Imp::Bogie_Imp( Scene& scene, std::shared_ptr<Gestalt> pGestalt )
	: m_Scene				{ scene }
	, m_CouplingNorth		{ *this }
	, m_CouplingSouth		{ *this }
	, m_pGestalt			{ pGestalt }
	, m_pRollingStock		{ nullptr }
	, m_bNorthSwivelBending	{ false }
	, m_bSouthSwivelBending	{ false }
	, m_TargetDirection		{ EndType::none }
{
	m_CouplingNorth.m_JackOnCouple.Insert( &m_JackOnCouple.PlugToPulse().Make() );
	m_CouplingSouth.m_JackOnCouple.Insert( &m_JackOnCouple.PlugToPulse().Make() );
	m_CouplingNorth.m_JackOnUnCouple.Insert( &m_JackOnUnCouple.PlugToPulse().Make() );
	m_CouplingSouth.m_JackOnUnCouple.Insert( &m_JackOnUnCouple.PlugToPulse().Make() );
	m_CouplingNorth.m_JackOnCouplingActivated.Insert( &m_JackOnCouplingActivated.PlugToPulse().Make() );
	m_CouplingSouth.m_JackOnCouplingActivated.Insert( &m_JackOnCouplingActivated.PlugToPulse().Make() );
	m_CouplingNorth.m_JackOnCouplingDeactivated.Insert( &m_JackOnCouplingDeactivated.PlugToPulse().Make() );
	m_CouplingSouth.m_JackOnCouplingDeactivated.Insert( &m_JackOnCouplingDeactivated.PlugToPulse().Make() );
	m_CouplingNorth.m_JackOnForceLimitExceeded.Insert( &m_JackOnForceLimitExceeded.PlugToPulse().Make() );
	m_CouplingSouth.m_JackOnForceLimitExceeded.Insert( &m_JackOnForceLimitExceeded.PlugToPulse().Make() );

	if( !m_pGestalt )
	{
		if( m_pGestalt = m_Scene.CreateGestalt( Box<Length>{ 10_m, 1_m, 1_m }, 1_t ); !m_pGestalt )
			throw std::bad_alloc();
	}

	// On shallow slopes a wheelframe might run uphill, slow down,
	// revert direction and speed up downhill. To prevent it from
	// sleeping, we set the sleep delay the sleep threshold:
	m_pGestalt->SetSleepDelay( 1_s );
	m_pGestalt->SetSleepThreshold( epsilon__velocity * epsilon__velocity / 2 );

	for( Jack& jack : *this )
		jack.Reference( "parent", TypeName() );
}
/* Needed by RemoveUnusedWheelFrames but temporarily disfunct ...
Bogie_Imp::Bogie_Imp( Bogie_Imp&& bogie ) noexcept
	: Bogie_Base					{ std::move(bogie) }
	, m_Scene						{ bogie.m_Scene }
	, m_pGestalt					{ std::move( bogie.m_pGestalt ) }
	, m_pSwivelParentNorth			{ std::move( bogie.m_pSwivelParentNorth ) }
	, m_pSwivelParentSouth			{ std::move( bogie.m_pSwivelParentSouth ) }
	, m_pSwivelChildNorth			{ std::move( bogie.m_pSwivelChildNorth ) }
	, m_pSwivelChildSouth			{ std::move( bogie.m_pSwivelChildSouth ) }
	, m_pBogieChildNorth			{ std::move( bogie.m_pBogieChildNorth ) }
	, m_pBogieChildSouth			{ std::move( bogie.m_pBogieChildSouth ) }
	, m_CouplingNorth				{ std::move( bogie.m_CouplingNorth ) }
	, m_CouplingSouth				{ std::move( bogie.m_CouplingSouth ) }
	, m_pRollingStock				{ std::move( bogie.m_pRollingStock ) }
	, m_JackOnCouple				{ std::move( bogie.m_JackOnCouple ) }
	, m_JackOnUnCouple				{ std::move( bogie.m_JackOnUnCouple ) }
	, m_JackOnCouplingActivated		{ std::move( bogie.m_JackOnCouplingActivated ) }
	, m_JackOnCouplingDeactivated	{ std::move( bogie.m_JackOnCouplingDeactivated ) }
	, m_JackOnForceLimitExceeded	{ std::move( bogie.m_JackOnForceLimitExceeded ) }
	, m_JackOnMotorStart			{ std::move( bogie.m_JackOnMotorStart ) }
	, m_JackOnMotorStop				{ std::move( bogie.m_JackOnMotorStop ) }
	, m_JackOnBrakeApply			{ std::move( bogie.m_JackOnBrakeApply ) }
	, m_JackOnBrakeRelease			{ std::move( bogie.m_JackOnBrakeRelease ) }
	, m_JackOnNorthSwivelBendingIn	{ std::move( bogie.m_JackOnNorthSwivelBendingIn ) }
	, m_JackOnNorthSwivelBendingOut	{ std::move( bogie.m_JackOnNorthSwivelBendingOut ) }
	, m_JackOnSouthSwivelBendingIn	{ std::move( bogie.m_JackOnSouthSwivelBendingIn ) }
	, m_JackOnSouthSwivelBendingOut	{ std::move( bogie.m_JackOnSouthSwivelBendingOut ) }
	, m_bNorthSwivelBending			{ bogie.m_bNorthSwivelBending }
	, m_bSouthSwivelBending			{ bogie.m_bSouthSwivelBending }
	, m_TargetDirection				{ bogie.m_TargetDirection }
{
	m_CouplingNorth.m_JackOnCouple.Insert( &m_JackOnCouple.PlugToPulse().Make() );
	m_CouplingSouth.m_JackOnCouple.Insert( &m_JackOnCouple.PlugToPulse().Make() );
	m_CouplingNorth.m_JackOnUnCouple.Insert( &m_JackOnUnCouple.PlugToPulse().Make() );
	m_CouplingSouth.m_JackOnUnCouple.Insert( &m_JackOnUnCouple.PlugToPulse().Make() );

	m_CouplingNorth.m_JackOnForceLimitExceeded.Insert( &m_JackOnForceLimitExceeded.PlugToPulse().Make() );
	m_CouplingSouth.m_JackOnForceLimitExceeded.Insert( &m_JackOnForceLimitExceeded.PlugToPulse().Make() );

	// insert at tails what bogie had...
	m_CouplingNorth.m_JackOnCouple.InsertAtTail( bogie.m_CouplingNorth.m_JackOnCouple.GetPlug() );
	m_CouplingSouth.m_JackOnCouple.InsertAtTail( bogie.m_CouplingSouth.m_JackOnCouple.GetPlug() );
	m_CouplingNorth.m_JackOnUnCouple.InsertAtTail( bogie.m_CouplingNorth.m_JackOnUnCouple.GetPlug() );
	m_CouplingSouth.m_JackOnUnCouple.InsertAtTail( bogie.m_CouplingSouth.m_JackOnUnCouple.GetPlug() );
	m_CouplingNorth.m_JackOnCouplingActivated.InsertAtTail( bogie.m_CouplingNorth.m_JackOnCouplingActivated.GetPlug() );
	m_CouplingSouth.m_JackOnCouplingActivated.InsertAtTail( bogie.m_CouplingSouth.m_JackOnCouplingActivated.GetPlug() );
	m_CouplingNorth.m_JackOnCouplingDeactivated.InsertAtTail( bogie.m_CouplingNorth.m_JackOnCouplingDeactivated.GetPlug() );
	m_CouplingSouth.m_JackOnCouplingDeactivated.InsertAtTail( bogie.m_CouplingSouth.m_JackOnCouplingDeactivated.GetPlug() );
	m_CouplingNorth.m_JackOnForceLimitExceeded.InsertAtTail( bogie.m_CouplingNorth.m_JackOnForceLimitExceeded.GetPlug() );
	m_CouplingSouth.m_JackOnForceLimitExceeded.InsertAtTail( bogie.m_CouplingSouth.m_JackOnForceLimitExceeded.GetPlug() );

	if( m_pSwivelParentNorth )
		m_pSwivelParentNorth->SetBogieB( this );

	if( m_pSwivelParentSouth )
		m_pSwivelParentSouth->SetBogieB( this );

	if( m_pSwivelChildNorth ){
		m_pSwivelChildNorth->SetBogieA( this );
	}

	if( m_pSwivelChildSouth ){
		m_pSwivelChildSouth->SetBogieA( this );
	}

	if( m_CouplingNorth.pCoupling )
		m_CouplingNorth.pCoupling->BogieA().get() == &bogie ? 
		m_CouplingNorth.pCoupling->SetBogieA( this ) : 
		m_CouplingNorth.pCoupling->SetBogieB( this );

	if( m_CouplingSouth.pCoupling )
		m_CouplingSouth.pCoupling->BogieA().get() == &bogie ? 
		m_CouplingSouth.pCoupling->SetBogieA( this ) :
		m_CouplingSouth.pCoupling->SetBogieB( this );	
}
*/
Bogie_Imp::~Bogie_Imp() noexcept
{
	m_pGestalt->SetName( nullptr );
	m_Scene.Unregister( *this );
}

void Bogie_Imp::SetWeakPointerToSelf( std::weak_ptr<RailRunner_Imp> pThis ) noexcept
{
	Bogie_Base::SetWeakPointerToSelf( pThis );

	// This is a hack to get the bogie child pointers set correctly on move construction:
	if( m_pSwivelParentNorth )
		m_pSwivelParentNorth->BogieA()->m_pBogieChildNorth = std::dynamic_pointer_cast<Bogie_Imp>(This());

	if( m_pSwivelParentSouth )
		m_pSwivelParentSouth->BogieA()->m_pBogieChildSouth = std::dynamic_pointer_cast<Bogie_Imp>(This());
}

bool Bogie_Imp::Start( Scene& ) noexcept{
	return true;
}

void Bogie_Imp::Idle() noexcept
{
}

void Bogie_Imp::PreUpdate()
{
}

void Bogie_Imp::Update( Time dt ) noexcept
{
	if( m_CouplingNorth.CheckCoupling( dt ) )
		Uncouple( EndType::north );
	if( m_CouplingSouth.CheckCoupling( dt ) )
		Uncouple( EndType::south );

	if( m_pSwivelChildNorth )
	{
		if( m_bNorthSwivelBending == (m_pSwivelChildNorth->GetBendAngle() <= scm_BendingAngle) ){
			m_bNorthSwivelBending = !m_bNorthSwivelBending;
			m_bNorthSwivelBending ? m_JackOnNorthSwivelBendingIn.Pulse() : m_JackOnNorthSwivelBendingOut.Pulse();
		}
	}

	if( m_pSwivelChildSouth )
	{
		if( m_bSouthSwivelBending == (m_pSwivelChildSouth->GetBendAngle() <= scm_BendingAngle) ){
			m_bSouthSwivelBending = !m_bSouthSwivelBending;
			m_bSouthSwivelBending ? m_JackOnSouthSwivelBendingIn.Pulse() : m_JackOnSouthSwivelBendingOut.Pulse();
		}
	}
}

void Bogie_Imp::Pause() noexcept{
}

void Bogie_Imp::Resume() noexcept{
}

void Bogie_Imp::Stop() noexcept{
}

void Bogie_Imp::Reference( const std::string& name, const std::string& reference ){
	Bogie_Base::Reference( name, reference );
	if( name == "name" ){
		if( reference.empty() )
			m_pGestalt->SetName( nullptr );
		else
			m_pGestalt->SetName( Reference( "name" ).c_str() );
	}
}

const char* Bogie_Imp::TypeName() const noexcept{
	return "Bogie";
}

bool Bogie_Imp::IsValid() const noexcept
{
	if( GetParent( EndType::north ).first == GetParent( EndType::south ).first &&
		GetParent( EndType::north ).first != nullptr )
		return false;

	if( GetChild( EndType::north ).first == GetChild( EndType::south ).first &&
		GetChild( EndType::north ).first != nullptr )
		return false;

	if( GetParent( EndType::north ).first.get() == this ||
		GetParent( EndType::south ).first.get() == this ) 
		return false;

	if( GetChild( EndType::north ).first.get() == this ||
		GetChild( EndType::south ).first.get() == this )
		return false;

	return true;
}

void Bogie_Imp::Rail( const Location& location, bool bMoveTo )
{
	RailChildBogies( location, bMoveTo );

	if( bMoveTo )
		AlignToChildBogies();
}

Location Bogie_Imp::GetLocation() const noexcept{
	if( m_pBogieChildNorth )
		if( Location loc = m_pBogieChildNorth->GetLocation(); loc.IsOnTrack() )
			return loc;

	if( m_pBogieChildSouth )
		if( Location loc = m_pBogieChildSouth->GetLocation(); loc.IsOnTrack() )
			return loc;

	return {};
}

void Bogie_Imp::DeRail() noexcept
{
	if( m_pBogieChildNorth )
		m_pBogieChildNorth->DeRail();

	if( m_pBogieChildSouth )
		m_pBogieChildSouth->DeRail();
}

bool Bogie_Imp::IsRailed() const noexcept{
	return	m_pBogieChildNorth ? m_pBogieChildNorth->IsRailed() : true &&
			m_pBogieChildSouth ? m_pBogieChildSouth->IsRailed() : true;
}

Frame<Length,One> Bogie_Imp::GetGlobalAnchor() const{
	try{
		if( m_pBogieChildNorth )
			return m_pBogieChildNorth->GetGlobalAnchor();
	}
	catch( const std::logic_error& ){
	}

	if( m_pBogieChildSouth )
		return m_pBogieChildSouth->GetGlobalAnchor();

	throw std::logic_error{ "Bogie_Imp::GetGlobalAnchor(): no designated anchor found." };
}

void Bogie_Imp::ResetToGlobalAnchor( const Frame<Length,One>& anchor )
{
	SetGlobalAnchorToChildBogies( anchor );
	AlignToChildBogies();
}

void Bogie_Imp::SetVelocity( Velocity velocity )
{
	Vector<One> T = LocalHorizontalDirection();
	Frame<Length,One> bodyFrame;
	GetGestalt().GetFrame( bodyFrame );
	bodyFrame.ToParent( T );
	GetGestalt().SetLinearVelocity( { T.dx * velocity, T.dy * velocity, T.dz * velocity } );

	if( m_pBogieChildNorth )
		m_pBogieChildNorth->SetVelocity( velocity );

	if( m_pBogieChildSouth )
		m_pBogieChildSouth->SetVelocity( velocity );
}

Velocity Bogie_Imp::GetVelocity() const
{
	if( m_pBogieChildNorth )
		return m_pBogieChildNorth->GetVelocity();
	
	if( m_pBogieChildSouth )
		return m_pBogieChildSouth->GetVelocity();

	return 0_mIs;
}

void Bogie_Imp::TargetVelocity( Velocity tvelocity )
{
	m_pGestalt->WakeUp();

	if( m_pBogieChildNorth )
		m_pBogieChildNorth->TargetVelocity( tvelocity );
	
	if( m_pBogieChildSouth )
		m_pBogieChildSouth->TargetVelocity( tvelocity );
}

Velocity Bogie_Imp::TargetVelocity() const noexcept
{
	if( m_pBogieChildNorth )
		return m_pBogieChildNorth->TargetVelocity();
	
	if( m_pBogieChildSouth )
		return m_pBogieChildSouth->TargetVelocity();

	return 0_mIs;
}

EndType Bogie_Imp::TargetDirection() const noexcept{
	return m_TargetDirection;
}

Power Bogie_Imp::MaximumPowerOutput() const noexcept
{
	Power maxPower = 0_kW;

	if( m_pBogieChildNorth )
		maxPower += m_pBogieChildNorth->MaximumPowerOutput();

	if( m_pBogieChildSouth )
		maxPower += m_pBogieChildSouth->MaximumPowerOutput();

	return maxPower;
}

Power Bogie_Imp::CurrentPowerOutput() const noexcept
{
	Power currentPower = 0_kW;

	if( m_pBogieChildNorth )
		currentPower += m_pBogieChildNorth->CurrentPowerOutput();

	if( m_pBogieChildSouth )
		currentPower += m_pBogieChildSouth->CurrentPowerOutput();

	return currentPower;
}

Velocity Bogie_Imp::MaximumVelocity() const noexcept
{
	Velocity maxVelocity = Velocity{infinite};

	if( m_pBogieChildNorth )
		maxVelocity = std::min( maxVelocity, m_pBogieChildNorth->MaximumVelocity() );

	if( m_pBogieChildSouth )
		maxVelocity = std::min( maxVelocity, m_pBogieChildSouth->MaximumVelocity() );

	return maxVelocity;
}

void Bogie_Imp::Thrust( One byfactor )
{
	One wasThrustFactor = Thrust();

	m_pGestalt->WakeUp();

	if( m_pBogieChildNorth )
		m_pBogieChildNorth->Thrust( byfactor );

	if( m_pBogieChildSouth )
		m_pBogieChildSouth->Thrust( byfactor );

	One isThrustFactor = Thrust();
	if( wasThrustFactor < epsilon && isThrustFactor >= epsilon )
		m_JackOnMotorStart.Pulse();
	else if( wasThrustFactor >= epsilon && isThrustFactor < epsilon )
		m_JackOnMotorStop.Pulse();
}

Force Bogie_Imp::ThrustAbsolute() const noexcept
{
	Force force = 0_N;

	if( m_pBogieChildNorth )
		force += m_pBogieChildNorth->ThrustAbsolute();

	if( m_pBogieChildSouth )
		force += m_pBogieChildSouth->ThrustAbsolute();

	return force;
}

Force Bogie_Imp::MaxThrust() const noexcept
{
	Force maxThrust = 0_N;

	if( m_pBogieChildNorth )
		maxThrust += m_pBogieChildNorth->MaxThrust();

	if( m_pBogieChildSouth )
		maxThrust += m_pBogieChildSouth->MaxThrust();

	return maxThrust;
}

Force Bogie_Imp::MaxThrust( Velocity velocity ) const noexcept
{
	Force maxThrust = 0_N;

	if( m_pBogieChildNorth )
		maxThrust += m_pBogieChildNorth->MaxThrust( velocity );

	if( m_pBogieChildSouth )
		maxThrust += m_pBogieChildSouth->MaxThrust( velocity );

	return maxThrust;
}

void Bogie_Imp::Brake( One byfactor )
{
	One wasBrakingFactor = Brake();

	m_pGestalt->WakeUp();

	if( m_pBogieChildNorth )
		m_pBogieChildNorth->Brake( byfactor );

	if( m_pBogieChildSouth )
		m_pBogieChildSouth->Brake( byfactor );

	One isBrakingFactor = Brake();
	if( wasBrakingFactor <= epsilon && isBrakingFactor > epsilon )
		m_JackOnBrakeApply.Pulse();
	else if( wasBrakingFactor > epsilon && isBrakingFactor <= epsilon )
		m_JackOnBrakeRelease.Pulse();
}

Force Bogie_Imp::BrakeAbsolute() const noexcept
{
	Force force = 0_N;

	if( m_pBogieChildNorth )
		force += m_pBogieChildNorth->BrakeAbsolute();

	if( m_pBogieChildSouth )
		force += m_pBogieChildSouth->BrakeAbsolute();

	return force;
}

Force Bogie_Imp::MaxBrake() const noexcept
{
	Force maxBrake = 0_N;

	if( m_pBogieChildNorth )
		maxBrake += m_pBogieChildNorth->MaxBrake();

	if( m_pBogieChildSouth )
		maxBrake += m_pBogieChildSouth->MaxBrake();

	return maxBrake;
}

Mass Bogie_Imp::TotalMass() const noexcept{
	return m_pGestalt ? m_pGestalt->GetMass() : 0_t;
}

void Bogie_Imp::ActivateCoupling( EndType end )
{
	switch( end )
	{
		case EndType::none:
			break;
		case EndType::north:
			if( m_CouplingNorth.pCoupling )
				throw std::runtime_error( "Bogie_Imp::ActivateCoupling: Coupling already is coupled!" );

			if( m_CouplingNorth.CouplingTypeIdx == 0 )
				throw std::runtime_error( "Bogie_Imp::ActivateCoupling( EndType::north ): Coupling type index is not set!" );

			m_CouplingNorth.bActivated = true;
			m_CouplingNorth.m_JackOnCouplingActivated.Pulse();
			break;
		case EndType::south:
			if( m_CouplingSouth.pCoupling )
				throw std::runtime_error( "Bogie_Imp::ActivateCoupling: Coupling already is coupled!" );

			if( m_CouplingSouth.CouplingTypeIdx == 0 )
				throw std::runtime_error( "Bogie_Imp::ActivateCoupling( EndType::south ): Coupling type index is not set!" );

			m_CouplingSouth.bActivated = true;
			m_CouplingSouth.m_JackOnCouplingActivated.Pulse();
			break;
		case EndType::both:
		case EndType::any:
			ActivateCoupling( EndType::north );
			ActivateCoupling( EndType::south );
			break;
	}
}

void Bogie_Imp::DeactivateCoupling( EndType end )
{
	switch( end )
	{
		case EndType::none:
			break;
		case EndType::north:
			if( m_CouplingNorth.pCoupling )
				throw std::runtime_error( "Bogie_Imp::DeactivateCoupling: Coupling already is coupled!" );
			m_CouplingNorth.bActivated = false;
			m_CouplingNorth.m_JackOnCouplingDeactivated.Pulse();
			break;
		case EndType::south:
			if( m_CouplingSouth.pCoupling )
				throw std::runtime_error( "Bogie_Imp::DeactivateCoupling: Coupling already is coupled!" );
			m_CouplingSouth.bActivated = false;
			m_CouplingSouth.m_JackOnCouplingDeactivated.Pulse();
			break;
		case EndType::any:
		case EndType::both:
			DeactivateCoupling( EndType::north );
			DeactivateCoupling( EndType::south );
			break;
	}
}

bool Bogie_Imp::IsActivated( EndType end ) const noexcept
{
	switch( end )
	{
		case EndType::none:
			break;
		case EndType::north:
			return m_CouplingNorth.bActivated;
		case EndType::south:
			return m_CouplingSouth.bActivated;
		case EndType::any:
			return IsActivated( EndType::north ) || IsActivated( EndType::south );
		case EndType::both:
			return IsActivated( EndType::north ) && IsActivated( EndType::south );
	}

	return false;
}

bool Bogie_Imp::Uncouple( EndType end, bool btriggerPulses ) noexcept
{
	switch( end )
	{
		case EndType::none:
			break;
		case EndType::north:
			return m_CouplingNorth.Uncouple( btriggerPulses );
		case EndType::south:
			return m_CouplingSouth.Uncouple( btriggerPulses );
		case EndType::any:
			return Uncouple( EndType::north, btriggerPulses ) || Uncouple( EndType::south, btriggerPulses );
		case EndType::both:
		{
			bool retval = false;

			if( Uncouple( EndType::north, btriggerPulses ) )
				retval = true;
			if( Uncouple( EndType::south, btriggerPulses ) )
				retval = true;

			return retval;
		}
	}

	return false;
}

bool Bogie_Imp::IsCoupled( EndType end ) const noexcept
{
	switch( end )
	{
		case EndType::none:
			break;
		case EndType::north:
			return m_CouplingNorth.pCoupling != nullptr;
		case EndType::south:
			return m_CouplingSouth.pCoupling != nullptr;
		case EndType::any:
			return IsCoupled( EndType::north ) || IsCoupled( EndType::south );
		case EndType::both:
			return IsCoupled( EndType::north ) && IsCoupled( EndType::south );
	}

	return false;
}

Length Bogie_Imp::GetCouplingHeight( EndType end ) const noexcept
{
	switch( end )
	{
		case EndType::north:
			return (m_CouplingNorth.Position.Center() - GetLocalAnchor()) * LocalVerticalDirection();
		case EndType::south:
			return (m_CouplingSouth.Position.Center() - GetLocalAnchor()) * LocalVerticalDirection();
		case EndType::any:
			return std::min( GetCouplingHeight( EndType::north ), GetCouplingHeight( EndType::south ) );
		case EndType::both:
			return std::max( GetCouplingHeight( EndType::north ), GetCouplingHeight( EndType::south ) );
		default:
			return 0_m;
	}
}

void Bogie_Imp::EnableSimulation( bool enable ) noexcept
{
	if( m_pGestalt ){
		m_pGestalt->EnableSimulation( enable );

		if( enable )
			m_pGestalt->WakeUp();
	}
}

bool Bogie_Imp::IsSimulationEnabled() const noexcept
{
	if( m_pGestalt )
		return m_pGestalt->IsSimulationEnabled();

	return false;
}

std::shared_ptr<Bogie> Bogie_Imp::ThisBogie() const noexcept{
	return std::dynamic_pointer_cast<Bogie>(This());
}

RollingStock* Bogie_Imp::GetRollingStock() const noexcept{
	return m_pRollingStock;
}

Gestalt& Bogie_Imp::GetGestalt() const noexcept{
	return *m_pGestalt;
}

void Bogie_Imp::Attach( std::shared_ptr<Bogie> pChildBogie, EndType atEnd, const Frame<Length,One>& swivelPose )
{
	if( !OrthoParallel( LocalVerticalDirection(), swivelPose.B, epsilon__angle ) )
		throw std::invalid_argument( "Swivel pose B vector is not parallel to vertical direction! All child and parent swivels have to be ortho-parallel" );

	if( std::shared_ptr<Bogie_Imp> pChildBogie_Imp = std::dynamic_pointer_cast<Bogie_Imp>(pChildBogie); pChildBogie_Imp )
	{
		Frame<Length,One> childPose, parentPose, childLocalPose = swivelPose;
		pChildBogie->GetGestalt().GetFrame( childPose );
		GetGestalt().GetFrame( parentPose );
		childPose.FromParent( parentPose.ToParent( childLocalPose ) );

		if( atEnd == EndType::north ){
			if( m_pSwivelChildNorth )
				throw std::runtime_error( "Bogie already has a child bogie at north end!" );
			if( pChildBogie_Imp->m_pSwivelParentSouth )
				throw std::runtime_error( "Child bogie already has a parent bogie at south end!" );

			if( std::shared_ptr<BogieSwivel> pSwivel = std::make_shared<BogieSwivel>( m_Scene, *this, swivelPose, *pChildBogie_Imp, childLocalPose ) )
			{
				m_Scene.Register( *this );
				m_pBogieChildNorth = pChildBogie_Imp;
				m_pSwivelChildNorth = pSwivel;
				m_pSwivelChildNorth->SetName( "SwivelChildNorth" );
				pChildBogie_Imp->m_pSwivelParentSouth = pSwivel;
			}
			else
				throw std::bad_alloc();
		}

		else if( atEnd == EndType::south ){
			if( m_pSwivelChildSouth )
				throw std::runtime_error( "Bogie already has a child bogie at south end!" );
			if( pChildBogie_Imp->m_pSwivelParentNorth )
				throw std::runtime_error( "Child bogie already has a parent bogie at north end!" );

			if( std::shared_ptr<BogieSwivel> pSwivel = std::make_shared<BogieSwivel>( m_Scene, *this, swivelPose, *pChildBogie_Imp, childLocalPose ) )
			{
				m_Scene.Register( *this );
				m_pBogieChildSouth = pChildBogie_Imp;
				m_pSwivelChildSouth = pSwivel;
				m_pSwivelChildSouth->SetName( "SwivelChildSouth" );
				pChildBogie_Imp->m_pSwivelParentNorth = pSwivel;
			}
			else
				throw std::bad_alloc();
		}

		else
			throw std::invalid_argument( "Invalid end type!" );
	}
}

void Bogie_Imp::Attach( std::shared_ptr<Bogie> pChildBogie, EndType atEnd )
{
	if( pChildBogie )
	{
		Frame<Length,One> swivelPose, bodyFrame, childBodyFrame;
		GetGestalt().GetFrame( bodyFrame );
		pChildBogie->GetGestalt().GetFrame( childBodyFrame );

		if( std::shared_ptr<WheelFrame> pWheelFrame = std::dynamic_pointer_cast<WheelFrame>(pChildBogie); pWheelFrame )
			swivelPose = pWheelFrame->Anchor();
		else if( pChildBogie->GetChild( EndType::north ).first && pChildBogie->GetChild( EndType::south ).first )
		{
			swivelPose = pChildBogie->GetChild( EndType::south ).second;
			swivelPose.TransportBy( (pChildBogie->GetChild( EndType::north ).second.P - swivelPose.P) / 2 );
		}
		else
			throw std::invalid_argument( "Bogie_Imp::Attach: Child bogie has to have a child bogie at both ends!" );

		childBodyFrame.ToParent( swivelPose );
		bodyFrame.FromParent( swivelPose );
		Attach( pChildBogie, atEnd, swivelPose );
	}
}

std::pair<std::shared_ptr<Bogie>,Frame<Length,One>> Bogie_Imp::GetChild( EndType atEnd ) const noexcept
{
	if( atEnd == EndType::any )
	{
		if( m_pSwivelChildNorth )
			atEnd = EndType::north;
		else if( m_pSwivelChildSouth )
			atEnd = EndType::south;
	}

	if( atEnd == EndType::north )
	{
		if( m_pSwivelChildNorth )
			return std::make_pair( m_pSwivelChildNorth->BogieB(), m_pSwivelChildNorth->LocalPoseB() );
	}

	else if( atEnd == EndType::south )
	{
		if( m_pSwivelChildSouth )
			return std::make_pair( m_pSwivelChildSouth->BogieB(), m_pSwivelChildSouth->LocalPoseB() );
	}

	return std::make_pair(nullptr,Identity<Length,One>);
}

std::pair<std::shared_ptr<Bogie>,Frame<Length,One>> Bogie_Imp::GetParent( EndType atEnd ) const noexcept
{
	if( atEnd == EndType::any )
	{
		if( m_pSwivelParentNorth )
			atEnd = EndType::north;
		else if( m_pSwivelParentSouth )
			atEnd = EndType::south;
	}

	if( atEnd == EndType::north )
	{
		if( m_pSwivelParentNorth )
			return std::make_pair( m_pSwivelParentNorth->BogieA(), m_pSwivelParentNorth->LocalPoseA() );
	}

	else if( atEnd == EndType::south )
	{
		if( m_pSwivelParentSouth )
			return std::make_pair( m_pSwivelParentSouth->BogieA(), m_pSwivelParentSouth->LocalPoseA() );
	}

	return std::make_pair(nullptr,Identity<Length,One>);
}

std::shared_ptr<Bogie> Bogie_Imp::GetFirst() const noexcept
{
	const Bogie* pFirst = this;
	while( pFirst->GetPrevious() )
		pFirst = pFirst->GetPrevious().get();

	return pFirst->ThisBogie();
}

std::shared_ptr<Bogie> Bogie_Imp::GetNext() const noexcept
{
	if( std::shared_ptr<Bogie> pNext = GetChild( EndType::south ).first; pNext )
		return pNext;

	if( std::shared_ptr<Bogie> pNext = GetParent( EndType::south ).first; pNext )
		return pNext;

	if( std::shared_ptr<Bogie> pNext = GetParent( EndType::north ).first; pNext )
		return pNext->GetParent( EndType::south ).first;
	
	return nullptr;
}

std::shared_ptr<Bogie> Bogie_Imp::GetPrevious() const noexcept
{
	if( std::shared_ptr<Bogie> pPrevious = GetParent( EndType::north ).first; pPrevious )
		return pPrevious;

	if( std::shared_ptr<Bogie> pPrevious = GetChild( EndType::north ).first; pPrevious )
		return pPrevious;

	if( std::shared_ptr<Bogie> pPrevious = GetParent( EndType::south ).first; pPrevious )
		return pPrevious->GetParent( EndType::north ).first;
	
	return nullptr;
}

std::shared_ptr<Bogie> Bogie_Imp::GetLast() const noexcept
{
	const Bogie* pLast = this;
	while( pLast->GetNext() )
		pLast = pLast->GetNext().get();

	return pLast->ThisBogie();
}

void Bogie_Imp::Detach( EndType atEnd ) noexcept
{
	switch( atEnd )
	{
	case EndType::north:
		if( m_pSwivelChildNorth )
		{
			m_pBogieChildNorth->m_pSwivelParentSouth.reset();
			m_pSwivelChildNorth = nullptr;
			m_pBogieChildNorth = nullptr;
		}
		break;
	case EndType::south:
		if( m_pSwivelChildSouth )
		{
			m_pBogieChildSouth->m_pSwivelParentNorth.reset();
			m_pSwivelChildSouth = nullptr;
			m_pBogieChildSouth = nullptr;
		}
		break;
	case EndType::any:
		if( m_pSwivelChildNorth )
			Detach( EndType::north );
		else
			Detach( EndType::south );
		break;
	case EndType::both:
		Detach( EndType::north );
		Detach( EndType::south );
		break;
	}
}

bool Bogie_Imp::Couple( EndType end, Bogie& with, EndType withEnd, bool btriggerPulses ) noexcept
{
	// Check parameters:
	if( end == EndType::none || withEnd == EndType::none )
		return true;
	if( end == EndType::any || withEnd == EndType::any )
		return false;

	// No self coupling:
	if( &with == this ) 
		return false;

	// Handle already coupled:
	auto alreadCoupled = GetCoupledBogie( end );
	if( alreadCoupled.first.get() == &with )
		return alreadCoupled.second == withEnd;
	else if( alreadCoupled.first )
		return false;
	if( AreCoupled( *this, with ) )
		return false;

	CouplingProps_Ext& thisCoupling = end == EndType::north ? m_CouplingNorth : m_CouplingSouth;
	CouplingProps_Ext& thatCoupling = withEnd == EndType::north ? 
		dynamic_cast<Bogie_Imp&>(with).m_CouplingNorth : dynamic_cast<Bogie_Imp&>(with).m_CouplingSouth;

	assert( thisCoupling.pCoupling == nullptr );
	assert( thatCoupling.pCoupling == nullptr );

	// Check index type:
	if( thisCoupling.CouplingTypeIdx != thatCoupling.CouplingTypeIdx )
		return false;
	if( thisCoupling.CouplingTypeIdx <= 0 )
		return false;

	thisCoupling.pCoupling = thatCoupling.pCoupling = std::make_shared<BogieCoupling>( 
		m_Scene,
		*this, 
		Frame<Length,One>{ thisCoupling.Position.Center(), Ex<One>, Ey<One>, Ez<One> }, 
		dynamic_cast<Bogie_Imp&>(with), 
		Frame<Length,One>{ thatCoupling.Position.Center(), Ex<One>, Ey<One>, Ez<One> } );

	if( !thisCoupling.pCoupling )
		return false;

	m_Scene.Register( *this );
	thisCoupling.pCoupling->SetName( "Coupling" );
	thisCoupling.pCoupling->SetLength( GetCouplingDistance( {*this,end}, {with,withEnd} ) );
	Length couplinglength = thisCoupling.pCoupling->GetLength();
	if( couplinglength > 5_m )
		std::clog << Verbosity::normal << "Very long coupling length: " << couplinglength << " is this intended?" << std::endl;

	thisCoupling.bActivated = false;
	thatCoupling.bActivated = false;

	if( btriggerPulses )
	{
		thisCoupling.m_JackOnCouple.Pulse();
		thatCoupling.m_JackOnCouple.Pulse();
	}

	return true;
}

std::pair<std::shared_ptr<Bogie>,EndType> Bogie_Imp::GetCoupledBogie( EndType end ) const noexcept
{
	switch( end )
	{
		case EndType::north:
			if( m_CouplingNorth.pCoupling ){
				std::shared_ptr<Bogie_Imp> pOtherBogie = m_CouplingNorth.pCoupling->GetCoupledBogie( *this );
				if( pOtherBogie->m_CouplingNorth.pCoupling == m_CouplingNorth.pCoupling )
					return std::make_pair( pOtherBogie, EndType::north );
				else if( pOtherBogie->m_CouplingSouth.pCoupling == m_CouplingNorth.pCoupling )
					return std::make_pair( pOtherBogie, EndType::south );
			}
			break;
		case EndType::south:
			if( m_CouplingSouth.pCoupling ){
				std::shared_ptr<Bogie_Imp> pOtherBogie = m_CouplingSouth.pCoupling->GetCoupledBogie( *this );
				if( pOtherBogie->m_CouplingNorth.pCoupling == m_CouplingSouth.pCoupling )
					return std::make_pair( pOtherBogie, EndType::north );
				else if( pOtherBogie->m_CouplingSouth.pCoupling == m_CouplingSouth.pCoupling )
					return std::make_pair( pOtherBogie, EndType::south );
			}
			break;
		case EndType::any:
			if( auto retval = GetCoupledBogie( EndType::north ); retval.first )
				return retval;
			else if( retval = GetCoupledBogie( EndType::south ); retval.first )
				return retval;
			break;
	}

	return { nullptr, EndType::none };
}

void Bogie_Imp::SetCouplingProps( EndType atEnd, const CouplingProps& props )
{
	switch( atEnd )
	{
		case EndType::none:
			break;
		case EndType::north:
			if( m_CouplingNorth.pCoupling )
				throw std::runtime_error( "Bogie_Imp::SetCouplingProps: Coupling already is coupled!" );
			m_CouplingNorth = props;
			break;
		case EndType::south:
			if( m_CouplingSouth.pCoupling )
				throw std::runtime_error( "Bogie_Imp::SetCouplingProps: Coupling already is coupled!" );
			m_CouplingSouth = props;
			break;
		default:
			throw std::invalid_argument( "Bogie_Imp::SetCouplingProps: Invalid end type!" );
	}
}

Bogie::CouplingProps Bogie_Imp::GetCouplingProps( EndType atEnd ) const noexcept
{
	switch( atEnd )
	{
	case EndType::any:
	case EndType::north:
		return m_CouplingNorth;
	case EndType::south:
		return m_CouplingSouth;
	default:
		return GetCouplingProps( EndType::north );
	}
}

spat::Sphere<Length> Bogie_Imp::GetCoupling( EndType end ) const noexcept
{
	Frame<Length, One> bogieFrame;
	GetGestalt().GetFrame( bogieFrame );

	switch( end )
	{
		case EndType::north:
			return bogieFrame * m_CouplingNorth.Position;
		case EndType::south:
			return bogieFrame * m_CouplingSouth.Position;
		default:
			break;
	}

	return {};
}

void Bogie_Imp::SetCouplingHeight( EndType end, Length height )
// (CP - AP) * v == height
// CP  = CP + (hnew - hold) * v
//     = CP + (height - (CP - AP) * v) * v
{
	Vector<One> v = LocalVerticalDirection();
	Position<Length> AP = GetLocalAnchor();

	switch( end )
	{
	case EndType::north:
		m_CouplingNorth.Position.c = m_CouplingNorth.Position.Center()
			+ (height - (m_CouplingNorth.Position.Center() - AP) * v) * v;
		break;
	case EndType::south:
		m_CouplingSouth.Position.c = m_CouplingSouth.Position.Center()
			+ (height - (m_CouplingSouth.Position.Center() - AP) * v) * v;
		break;
	case EndType::any:
		SetCouplingHeight( EndType::north, height );
		SetCouplingHeight( EndType::south, height );
		break;
	default:
		break;
	}
}

bool Bogie_Imp::BreakCoupling( EndType atEnd, bool btriggerPulses ) noexcept
{
	switch( atEnd )
	{
	case EndType::any:
	case EndType::north:
		return m_CouplingNorth.Break( btriggerPulses );
	case EndType::south:
		return m_CouplingSouth.Break( btriggerPulses );
	case EndType::both:
		{
			bool retval = false;
			if( m_CouplingNorth.Break( btriggerPulses ) )
				retval = true;
			if( m_CouplingSouth.Break( btriggerPulses ) )
				retval = true;
			return retval;
		}
	default:
		return false;
	}
}

Jack& Bogie_Imp::JackOnCouple( EndType end ){
	switch( end ){
	case EndType::north:
		return m_CouplingNorth.m_JackOnCouple;
	case EndType::south:
		return m_CouplingSouth.m_JackOnCouple;
	case EndType::any:
		return m_JackOnCouple;
	default:
		throw std::invalid_argument( "Bogie_Imp::JackOnCouple: Invalid end type!" );
	}
}

Jack& Bogie_Imp::JackOnUnCouple( EndType end ){
	switch( end ){
	case EndType::north:
		return m_CouplingNorth.m_JackOnUnCouple;
	case EndType::south:
		return m_CouplingSouth.m_JackOnUnCouple;
	case EndType::any:
		return m_JackOnUnCouple;
	default:
		throw std::invalid_argument( "Bogie_Imp::JackOnUnCouple: Invalid end type!" );
	}
}

Jack& Bogie_Imp::JackOnCouplingActivated( EndType end ){
	switch( end ){
	case EndType::north:
		return m_CouplingNorth.m_JackOnCouplingActivated;
	case EndType::south:
		return m_CouplingSouth.m_JackOnCouplingActivated;
	case EndType::any:
		return m_JackOnCouplingActivated;
	default:
		throw std::invalid_argument( "Bogie_Imp::JackOnCouplingActivated: Invalid end type!" );
	}
}

Jack& Bogie_Imp::JackOnCouplingDeactivated( EndType end ){
	switch( end ){
	case EndType::north:
		return m_CouplingNorth.m_JackOnCouplingDeactivated;
	case EndType::south:
		return m_CouplingSouth.m_JackOnCouplingDeactivated;
	case EndType::any:
		return m_JackOnCouplingDeactivated;
	default:
		throw std::invalid_argument( "Bogie_Imp::JackOnUnCouplingDeactivated: Invalid end type!" );
	}
}

Jack& Bogie_Imp::JackOnCouplingForceLimitExceeded( EndType end ){
	switch( end ){
	case EndType::north:
		return m_CouplingNorth.m_JackOnForceLimitExceeded;
	case EndType::south:
		return m_CouplingSouth.m_JackOnForceLimitExceeded;
	case EndType::any:
		return m_JackOnForceLimitExceeded;
	default:
		throw std::invalid_argument( "Bogie_Imp::JackOnCouplingForceLimitExceeded: Invalid end type!" );
	}
}

Jack& Bogie_Imp::JackOnMotorStart() noexcept{
	return m_JackOnMotorStart;
}

Jack& Bogie_Imp::JackOnMotorStop() noexcept{
	return m_JackOnMotorStop;
}

Jack& Bogie_Imp::JackOnBrakeApply() noexcept{
	return m_JackOnBrakeApply;
}

Jack& Bogie_Imp::JackOnBrakeRelease() noexcept{
	return m_JackOnBrakeRelease;
}

Jack& Bogie_Imp::JackOnNorthSwivelBendingIn() noexcept{
	return m_JackOnNorthSwivelBendingIn;
}

Jack& Bogie_Imp::JackOnNorthSwivelBendingOut() noexcept{
	return m_JackOnNorthSwivelBendingOut;
}

Jack& Bogie_Imp::JackOnSouthSwivelBendingIn() noexcept{
	return m_JackOnSouthSwivelBendingIn;
}

Jack& Bogie_Imp::JackOnSouthSwivelBendingOut() noexcept{
	return m_JackOnSouthSwivelBendingOut;
}

int Bogie_Imp::CountJacks() const noexcept{
	return 23;
}
//
//int Bogie_Imp::CountPlugs() const
//{
//	return 0;
//}

void Bogie_Imp::SetRollingStock( RollingStock* pRollingStock ) noexcept{
	m_pRollingStock = pRollingStock;
}

const Jack& Bogie_Imp::_GetJack( int idx ) const
{
	switch( idx )
	{
	case 0:
		return m_CouplingNorth.m_JackOnCouple;
	case 1:
		return m_CouplingSouth.m_JackOnCouple;
	case 2:
		return m_JackOnCouple;
	case 3:
		return m_CouplingNorth.m_JackOnUnCouple;
	case 4:
		return m_CouplingSouth.m_JackOnUnCouple;
	case 5:
		return m_JackOnUnCouple;
	case 6:
		return m_CouplingNorth.m_JackOnCouplingActivated;
	case 7:
		return m_CouplingSouth.m_JackOnCouplingActivated;
	case 8:
		return m_JackOnCouplingActivated;
	case 9:
		return m_CouplingNorth.m_JackOnCouplingDeactivated;
	case 10:
		return m_CouplingSouth.m_JackOnCouplingDeactivated;
	case 11:
		return m_JackOnCouplingDeactivated;
	case 12:
		return m_JackOnMotorStart;
	case 13:
		return m_JackOnMotorStop;
	case 14:
		return m_JackOnBrakeApply;
	case 15:
		return m_JackOnBrakeRelease;
	case 16:
		return m_CouplingNorth.m_JackOnForceLimitExceeded;
	case 17:
		return m_CouplingSouth.m_JackOnForceLimitExceeded;
	case 18:
		return m_JackOnForceLimitExceeded;
	case 19:
		return m_JackOnNorthSwivelBendingIn;
	case 20:
		return m_JackOnNorthSwivelBendingOut;
	case 21:
		return m_JackOnSouthSwivelBendingIn;
	case 22:
		return m_JackOnSouthSwivelBendingOut;
	}

	std::ostringstream stream;
	stream << "Out of range!" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::range_error( stream.str() );
}
//
//const Plug& Bogie_Imp::_GetPlug( int idx ) const
//{
//	switch( idx )
//	{
//	}
//	
//	assert( 0 );
//	std::ostringstream stream;
//	stream << __FILE__ << '(' << __LINE__ << ')' << "Out of range!";
//	throw std::range_error( stream.str() );
//}

void Bogie_Imp::RailChildBogies( const trax::Location& location, bool bMoveTo )
{
	if( m_pBogieChildNorth )
		m_pBogieChildNorth->Rail( location, bMoveTo );

	if( m_pBogieChildSouth )
	{
		Location southBogieLocation = location;
		MoveLocation( southBogieLocation, AnchorToChildSouth() - m_pBogieChildSouth->AnchorToParentNorth() );

		m_pBogieChildSouth->Rail( southBogieLocation, bMoveTo );
	}
}

void Bogie_Imp::SetGlobalAnchorToChildBogies( const Frame<Length,One>& anchor )
{
	if( m_pBogieChildNorth )
		m_pBogieChildNorth->ResetToGlobalAnchor( anchor );

	if( m_pBogieChildSouth )
	{
		Frame<Length,One> southBogieAnchor = anchor;
		southBogieAnchor.TransportTan(
			AnchorToChildSouth() - m_pBogieChildSouth->AnchorToParentNorth() );

		m_pBogieChildSouth->ResetToGlobalAnchor( southBogieAnchor );
	}
}

void Bogie_Imp::AlignToChildBogies()
// PF * PP = R * CF * CP -> PF = R * CF * CP * Inv(PP)
{
	if( m_pBogieChildNorth && m_pBogieChildSouth )
	{
		Frame<Length,One> CF;
		m_pBogieChildSouth->GetGestalt().GetFrame( CF );
		Position<Length> ssP = m_pSwivelChildSouth->LocalPoseB().P;
		CF.ToParent( ssP );

		m_pBogieChildNorth->GetGestalt().GetFrame( CF );
		Frame<Length,One> nsP = m_pSwivelChildNorth->LocalPoseB();
		CF.ToParent( nsP );

		Vector<One> d = Normalize((nsP.P - ssP) - (nsP.P - ssP) * nsP.B * nsP.B).second;
		Angle angle = ((nsP.T % d) * nsP.B >= 0 ? +1 : -1) * acos( common::Clamp( nsP.T * d, -1_1, +1_1 ) );
		nsP.Rotate( nsP.B, angle );

		Transformation<One> RxCFxCP{ nsP };
		Transformation<One> PP{ m_pSwivelChildNorth->LocalPoseA() };
		Transformation<One> PPI{ PP };
		PPI.Invert();

		GetGestalt().SetFrame( Frame<Length,One>{ RxCFxCP * PPI } );
	}
}
///////////////////////////////////////
trax::Bogie_Imp::BogieSwivel::BogieSwivel(
	const Scene& scene,
	Bogie_Imp& bogieParent, 
	const spat::Frame<Length,One>& poseParent, 
	Bogie_Imp& bogieChild, 
	const spat::Frame<Length,One>& poseChild )
	: BogieJoint<HingeJoint>{ 
		scene.CreateHingeJoint( &bogieParent.GetGestalt(), poseParent, &bogieChild.GetGestalt(), poseChild ), 
		bogieParent, 
		bogieChild }
{
}

spat::VectorBundle<Length,One> trax::Bogie_Imp::BogieSwivel::LocalAxisA() const noexcept{
	return m_pJoint->LocalAxisA();
}

spat::VectorBundle<Length,One> trax::Bogie_Imp::BogieSwivel::LocalAxisB() const noexcept{
	return m_pJoint->LocalAxisB();
}

Angle trax::Bogie_Imp::BogieSwivel::GetBendAngle() const noexcept{
	return m_pJoint->GetBendAngle();
}
///////////////////////////////////////
trax::Bogie_Imp::BogieCoupling::BogieCoupling(
	const Scene& scene, 
	Bogie_Imp& bogieParent, 
	const spat::Frame<Length,One>& poseParent, 
	Bogie_Imp& bogieChild, 
	const spat::Frame<Length,One>& poseChild ) noexcept
	: BogieJoint<DistanceJoint>{ 
		scene.CreateDistanceJoint( &bogieParent.GetGestalt(), poseParent, &bogieChild.GetGestalt(), poseChild ),
		bogieParent, 
		bogieChild }
{
}

std::shared_ptr<Bogie_Imp> trax::Bogie_Imp::BogieCoupling::GetCoupledBogie( const Bogie_Imp& toBogie ) const noexcept
{
	if( &toBogie == &m_BogieA )
		return BogieB();
	else if( &toBogie == &m_BogieB )
		return BogieA();

	return nullptr;
}

Length trax::Bogie_Imp::BogieCoupling::GetLength() const noexcept{
	return m_pJoint->GetDistance();
}

void trax::Bogie_Imp::BogieCoupling::SetLength( Length length ) noexcept{
	m_pJoint->SetDistance( length );
}

spat::Vector<Force> trax::Bogie_Imp::BogieCoupling::GetForce() const noexcept{
	return m_pJoint->GetForce();
}
///////////////////////////////////////
Vector<One> Bogie_Imp::LocalVerticalDirection() const noexcept
{
	if( m_pSwivelParentNorth )
		return m_pSwivelParentNorth->LocalAxisB().T;

	if( m_pSwivelParentSouth )
		return m_pSwivelParentSouth->LocalAxisB().T;

	if( m_pSwivelChildNorth )
		return m_pSwivelChildNorth->LocalAxisA().T;

	if( m_pSwivelChildSouth )
		return m_pSwivelChildSouth->LocalAxisA().T;

	return Null<One>;
}

Vector<One> Bogie_Imp::LocalHorizontalDirection() const noexcept
{
	if( m_pSwivelChildNorth && m_pSwivelChildSouth )
		return Normalize( Distance( m_pSwivelChildSouth->LocalAxisA(), m_pSwivelChildNorth->LocalAxisA() ).T ).second;

	if( m_pSwivelParentNorth && m_pSwivelParentSouth )
		return Normalize( Distance( m_pSwivelParentSouth->LocalAxisB(), m_pSwivelParentNorth->LocalAxisB() ).T ).second;

	assert( !"Should'nt get here!" );
	return Vector<One>();
}

Length Bogie_Imp::AnchorToParentNorth() const noexcept
{
	if( m_pBogieChildNorth )
		return m_pBogieChildNorth->AnchorToParentSouth() 
			+ LocalHorizontalDirection() * Distance( m_pSwivelChildNorth->LocalAxisA(), m_pSwivelParentNorth->LocalAxisB() ).T;

	return 0_m;
}

Length Bogie_Imp::AnchorToParentSouth() const noexcept
{
	if( m_pBogieChildNorth )
		return m_pBogieChildNorth->AnchorToParentSouth() 
			+ LocalHorizontalDirection() * Distance( m_pSwivelChildNorth->LocalAxisA(), m_pSwivelParentSouth->LocalAxisB() ).T;

	return 0_m;
}

Length Bogie_Imp::AnchorToChildSouth() const noexcept
{
	if( m_pBogieChildNorth )
		return m_pBogieChildNorth->AnchorToParentSouth() 
			+ LocalHorizontalDirection() * Distance( m_pSwivelChildNorth->LocalAxisA(), m_pSwivelChildSouth->LocalAxisA() ).T;

	return 0_m;
}

Length Bogie_Imp::AnchorToChildNorth() const noexcept
{
	if( m_pBogieChildNorth )
		return m_pBogieChildNorth->AnchorToParentSouth();

	return 0_m;
}

Position<Length> Bogie_Imp::GetLocalAnchor() const{
	Frame<Length,One> bodyFrame;
	GetGestalt().GetFrame( bodyFrame );
	Position<Length> localAnchor = GetGlobalAnchor().P;
	bodyFrame.FromParent( localAnchor );
	return localAnchor;
}

Bogie_Imp::CouplingProps_Ext::CouplingProps_Ext( const Bogie_Imp& parentBogie ) noexcept
	: CouplingProps	{}
	, ParentBogie	{ parentBogie }
{
}

Bogie_Imp::CouplingProps_Ext& Bogie_Imp::CouplingProps_Ext::operator=( const Bogie::CouplingProps& props ) noexcept
{
	CouplingProps::operator=( props );
	return *this;
}

bool Bogie_Imp::CouplingProps_Ext::CheckCoupling( Time dt ) noexcept
{
	if( Type == onNever )
		return false;

	if( pCoupling && BreakingForce < infinite__force )
	{
		Force force = pCoupling->GetForce().Length();
		if( force > BreakingForce )
		{
			std::clog << Verbosity::detailed << "Coupling force: " << force << std::endl;
			m_JackOnForceLimitExceeded.Pulse();

			switch( Type ){
			case onForceLimitReached:
				return true;
			case onMomentumConsumedWithRecuperation:
				ActualBreakingSpareMomentum += RecuperationForce * dt;
				[[fallthrough]];
			case onMomentumConsumed:
				ActualBreakingSpareMomentum -= (force - BreakingForce) * dt;
				return ActualBreakingSpareMomentum < 0_Ns;
			default:
				break;
			}
		}
	}

	return false;
}

bool Bogie_Imp::CouplingProps_Ext::Uncouple( bool btriggerPulses ) noexcept
{
	if( pCoupling ){
		CouplingProps_Ext* pOtherCoupling = nullptr;
		std::shared_ptr<Bogie_Imp> pOtherBogie = pCoupling->GetCoupledBogie( ParentBogie );
		if( pOtherBogie->m_CouplingNorth.pCoupling == pCoupling )
		{
			pOtherCoupling = &pOtherBogie->m_CouplingNorth;
			pOtherCoupling->pCoupling = nullptr;
		}
		else if( pOtherBogie->m_CouplingSouth.pCoupling == pCoupling )
		{
			pOtherCoupling = &pOtherBogie->m_CouplingSouth;
			pOtherCoupling->pCoupling = nullptr;
		}

		pCoupling = nullptr;

		if( btriggerPulses ){
			m_JackOnUnCouple.Pulse();
			if( pOtherCoupling )
				pOtherCoupling->m_JackOnUnCouple.Pulse();
		}

		return true;
	}

	return false;
}

bool Bogie_Imp::CouplingProps_Ext::Break( bool btriggerPulses ) noexcept{
	if( btriggerPulses )
		m_JackOnForceLimitExceeded.Pulse();

	return Uncouple( btriggerPulses );
}
///////////////////////////////////////
bool AreCoupled( const Bogie& a, const Bogie& b ) noexcept{
	return	a.GetCoupledBogie( EndType::north ).first.get() == &b ||
			a.GetCoupledBogie( EndType::south ).first.get() == &b;
}

bool Couple( std::pair<Bogie&,EndType> a, std::pair<Bogie&,EndType> b ) noexcept{
	return a.first.Couple( a.second, b.first, b.second );
}

Length GetCouplingDistance( 
	std::pair<const Bogie&,EndType> a, 
	std::pair<const Bogie&,EndType> b ) noexcept
{
	spat::Frame<Length,One> bodyFrame;
	a.first.GetGestalt().GetFrame( bodyFrame );
	spat::Position<Length> pos1 = a.first.GetCouplingProps( a.second ).Position.Center();
	bodyFrame.ToParent( pos1 );
	b.first.GetGestalt().GetFrame( bodyFrame );
	spat::Position<Length> pos2 = b.first.GetCouplingProps( b.second ).Position.Center();
	bodyFrame.ToParent( pos2 );			
	return (pos1 - pos2).Length();
}
///////////////////////////////////////
}

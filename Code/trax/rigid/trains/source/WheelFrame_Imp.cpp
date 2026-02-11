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

#include "WheelFrame_Imp.h"

#include "trax/Event.h"
#include "trax/rigid/Gestalt.h"
#include "trax/rigid/Material.h"
#include "trax/rigid/Scene.h"
#include "trax/rigid/TrackJointFeeder.h"

namespace trax{
	using namespace spat;
///////////////////////////////////////
std::shared_ptr<WheelFrame> WheelFrame::Make( 
	Scene& scene, 
	std::shared_ptr<Gestalt> pGestalt ) noexcept
{
	try{
		std::shared_ptr<WheelFrame_Imp> pRetval = std::make_shared<WheelFrame_Imp>( scene, pGestalt );
		
		if( pRetval )
			pRetval->SetWeakPointerToSelf( pRetval );
	
		return pRetval;
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

WheelFrame_Imp::WheelFrame_Imp( 
	Scene& scene, 
	std::shared_ptr<Gestalt> pGestalt )
	: Bogie_Imp			{ scene, pGestalt }
	, m_Flange			{ 0_m }
	, m_bEnableDerailing{ true }
{
	spat::Frame<Length,One> anchor = spat::Identity<Length,One>;

	if( !pGestalt )
	// Body was created with shape by Bogie_Imp
	{
		spat::Box<Length> shape{ 2_m, 1_m, 1_m };
		GetGestalt().PrincipalMomentsOfInertia( GetGestalt().GetMass() * SpecificInertiaTensorFor( shape ) );
		dynamic_cast<GeomCuboid*>(&GetGestalt().Get())->SetFromBox( shape );
		anchor.TransportBin( -0.5_m );
	}

	if( m_pTrackJointFeederMotorModel = m_Scene.CreateTrackJointFeederMotorModel( 
		GetGestalt(), anchor ); !m_pTrackJointFeederMotorModel )
		throw std::runtime_error( "WheelFrame_Imp::WheelFrame_Imp: failed to create TrackJointFeederMotorModel!" );

	m_pTrackJointFeederMotorModel->AttachEvent( std::make_shared<Event_Imp>( *this ) );

	for( Jack& jack : *this )
		jack.Reference( "parent", WheelFrame_Imp::TypeName() );
}

WheelFrame_Imp::~WheelFrame_Imp()
{
	if( m_pTrackJointFeederMotorModel ){
		m_pTrackJointFeederMotorModel->AttachEvent( nullptr );
		m_Scene.Release( *m_pTrackJointFeederMotorModel );
	}
}

const char* WheelFrame_Imp::TypeName() const noexcept{
	return "WheelFrame";
}

bool WheelFrame_Imp::IsValid() const noexcept
{
	if( !m_pTrackJointFeederMotorModel )
		return false;

	return Bogie_Imp::IsValid();
}

void WheelFrame_Imp::Rail( const Location& location, bool bMoveTo )
{
	Location wheelFrameLocation = location;
	if( m_pSwivelChildNorth )
		MoveLocation( wheelFrameLocation, AnchorToChildNorth() + Anchor().T * Distance( m_pSwivelChildNorth->LocalAxisA(), { Anchor().P, Anchor().B } ).T );

	RailChildBogies( location, bMoveTo );

	if( m_pTrackJointFeederMotorModel->IsRailed() )
		m_pTrackJointFeederMotorModel->DeRail( false );

	if( bMoveTo ){
		spat::Frame<Length,One> locationFrame;
		wheelFrameLocation.Transition( locationFrame );
		AlignWithAnchorTo( locationFrame );
	}

	m_pTrackJointFeederMotorModel->Rail( wheelFrameLocation );
}

Location WheelFrame_Imp::GetLocation() const noexcept{
	return m_pTrackJointFeederMotorModel->GetLocation();
}

void WheelFrame_Imp::DeRail() noexcept
{
	m_pTrackJointFeederMotorModel->DeRail();

	Bogie_Imp::DeRail();
}

bool WheelFrame_Imp::IsRailed() const noexcept{
	if( !m_pTrackJointFeederMotorModel->IsRailed() )
		return false;
	return Bogie_Imp::IsRailed();
}

spat::Frame<Length,One> WheelFrame_Imp::GetGlobalAnchor() const{
	spat::Frame<Length,One> bodyFrame, globalAnchor = Anchor();
	GetGestalt().GetFrame( bodyFrame );
	return bodyFrame.ToParent( globalAnchor );
}

void WheelFrame_Imp::ResetToGlobalAnchor( const spat::Frame<Length,One>& anchor )
{
	assert( !IsRailed() );

	spat::Frame<Length,One> wheelFrameAnchor = anchor;
	if( m_pSwivelChildNorth )
		wheelFrameAnchor.TransportTan( AnchorToChildNorth() + Anchor().T * Distance( m_pSwivelChildNorth->LocalAxisA(), { Anchor().P, Anchor().B } ).T );

	AlignWithAnchorTo( wheelFrameAnchor );
	SetGlobalAnchorToChildBogies( anchor );
}

void WheelFrame_Imp::SetVelocity( Velocity velocity ){
	m_pTrackJointFeederMotorModel->SetVelocity( velocity );
	Bogie_Imp::SetVelocity( velocity );
}

Velocity WheelFrame_Imp::GetVelocity() const{
	return m_pTrackJointFeederMotorModel->GetVelocity();
}

void WheelFrame_Imp::TargetVelocity( Velocity tvelocity ){
	m_pTrackJointFeederMotorModel->TargetVelocity( tvelocity );
	Bogie_Imp::TargetVelocity( tvelocity );
}

Velocity WheelFrame_Imp::TargetVelocity() const noexcept{
	return m_pTrackJointFeederMotorModel->TargetVelocity();
}

Power WheelFrame_Imp::MaximumPowerOutput() const noexcept{
	return Bogie_Imp::MaximumPowerOutput() +
		m_pTrackJointFeederMotorModel->MaximumPowerOutput();
}

Power WheelFrame_Imp::CurrentPowerOutput() const noexcept{
	return Bogie_Imp::CurrentPowerOutput() +
		m_pTrackJointFeederMotorModel->CurrentPowerOutput();
}

Velocity WheelFrame_Imp::MaximumVelocity() const noexcept{
	return std::min( Bogie_Imp::MaximumVelocity(), m_pTrackJointFeederMotorModel->MaximumVelocity() );
}

void WheelFrame_Imp::Thrust( One byfactor ){
	m_pTrackJointFeederMotorModel->Thrust( byfactor );
	Bogie_Imp::Thrust( byfactor );
}

Force WheelFrame_Imp::ThrustAbsolute() const noexcept{
	return Bogie_Imp::ThrustAbsolute() + m_pTrackJointFeederMotorModel->ThrustAbsolute();
}

Force WheelFrame_Imp::MaxThrust() const noexcept{
	return Bogie_Imp::MaxThrust() + m_pTrackJointFeederMotorModel->MaxThrust();
}

Force WheelFrame_Imp::MaxThrust( Velocity velocity ) const noexcept{
	return Bogie_Imp::MaxThrust( velocity ) + m_pTrackJointFeederMotorModel->MaxThrust( velocity );
}

void WheelFrame_Imp::Brake( One byfactor ){
	m_pTrackJointFeederMotorModel->Brake( byfactor );
	Bogie_Imp::Brake( byfactor );
}

Force WheelFrame_Imp::BrakeAbsolute() const noexcept{
	return Bogie_Imp::BrakeAbsolute() + m_pTrackJointFeederMotorModel->BrakeAbsolute();
}

Force WheelFrame_Imp::MaxBrake() const noexcept{
	return Bogie_Imp::MaxBrake() + m_pTrackJointFeederMotorModel->MaxBrake();
}

void WheelFrame_Imp::Limits( const TrackJointLimits& wfl ){
	try{
		SetLimits( wfl );
	}
	catch( std::invalid_argument& e ){
		SetLimits( m_TrackJointLimits );

		std::ostringstream stream;
		stream << "WheelFrame_Imp::Limits: " << e.what() << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::invalid_argument( stream.str() );
	}

	m_TrackJointLimits = wfl;
}

const TrackJointLimits& WheelFrame_Imp::Limits() const noexcept{
	return m_TrackJointLimits;
}

std::shared_ptr<const TractionForceCharacteristic> 
WheelFrame_Imp::SetTractionForceCharacteristic( 
	std::shared_ptr<const TractionForceCharacteristic> pCharacteristic )
{
	std::shared_ptr<const TractionForceCharacteristic> pRetval = m_pTrackJointFeederMotorModel->GetTractionForceCharacteristic();
	m_pTrackJointFeederMotorModel->SetTractionForceCharacteristic( pCharacteristic );
	return pRetval;
}

std::shared_ptr<const TractionForceCharacteristic> 
WheelFrame_Imp::GetTractionForceCharacteristic() const
{
	return m_pTrackJointFeederMotorModel->GetTractionForceCharacteristic();
}

void WheelFrame_Imp::EnableDerailing( bool bEnable ) noexcept
{
	m_bEnableDerailing = bEnable;

	//Reset the force limits by their values, so that
	//the trackjoint values can get set to infinity if
	//derailing is switched off or else to their proper values:
	SetLimits( m_TrackJointLimits );
}

bool WheelFrame_Imp::IsDerailingEnabled() const noexcept
{
	return m_bEnableDerailing;
}

void WheelFrame_Imp::Anchor( const spat::Frame<Length,One>& frame ) noexcept{
	m_pTrackJointFeederMotorModel->Anchor( frame );
	CalculateWheelsetFramesFromAnchor();
}

spat::Frame<Length,One> WheelFrame_Imp::Anchor() const noexcept{
	return m_pTrackJointFeederMotorModel->Anchor();
}

spat::Frame<Length, One> WheelFrame_Imp::UnflippedAnchor() const noexcept{
	if( IsRailed() && GetLocation().Orient() == Orientation::Value::anti ){
		Frame<Length, One> unflippedAnchor = Anchor();
		unflippedAnchor.FlipBin();
		return unflippedAnchor;
	}

	return Anchor();
}

int WheelFrame_Imp::Attach( const Wheelset& ws )
{
	assert( !IsRailed() );

	int retval = m_pTrackJointFeederMotorModel->Attach( ws );
	m_Flange = (std::max)( m_Flange, ws.Flange );
	m_pTrackJointFeederMotorModel->SetFlange( m_Flange );

	if( std::unique_ptr<GeomCapsule> pWheelsetGeom = m_Scene.CreateGeomCapsule(); pWheelsetGeom )
	{
		Frame<Length,One> wheelsetGeomFrame;
		wheelsetGeomFrame.Init();
		wheelsetGeomFrame.RotateTan( pi/2 );
		wheelsetGeomFrame.TransportTan( ws.AxlePosition.x );
		wheelsetGeomFrame.TransportNor( ws.AxlePosition.y );
		wheelsetGeomFrame.TransportBin( -ws.Shift );
		Anchor().ToParent( wheelsetGeomFrame );

		pWheelsetGeom->SetFrame( wheelsetGeomFrame );
		pWheelsetGeom->SetName( "Wheelset" );
		pWheelsetGeom->Radius( ws.Radius );
		pWheelsetGeom->SetLength( std::max( ws.Gauge - ws.Radius, 0.1_m ) ); // caps stick out only half a radius; 0 is not a valid value
		pWheelsetGeom->SetMaterial( Material::Type::steel );
		pWheelsetGeom->TypeFilter( Geom::fWheelset );
		pWheelsetGeom->CollisionFilter( CollisionFilterFor(pWheelsetGeom->TypeFilter()) );

		GetGestalt().Attach( std::move(pWheelsetGeom), ws.TotalMass );
	}

	return retval;
}

void WheelFrame_Imp::DetachAllWheelsets(){
	for( int idx = 0; idx < GetGestalt().Count(); ++idx )
	{
		if( GetGestalt().Get( idx ).TypeFilter() == Geom::fWheelset )
			GetGestalt().Remove( idx-- );
	}

	m_Flange = 0_m;

	return m_pTrackJointFeederMotorModel->DetachAllWheelsets();
}

int WheelFrame_Imp::CntWheelsets() const noexcept{
	return m_pTrackJointFeederMotorModel->CntWheelsets();
}

const Wheelset& WheelFrame_Imp::GetWheelset( int idx ) const{
	return m_pTrackJointFeederMotorModel->GetWheelset(idx);
}

void WheelFrame_Imp::CalculateWheelsetFramesFromAnchor() noexcept
{
	assert( !IsRailed() );

	int idWheelset = 0;
	for( int idx = 0; idx < GetGestalt().Count(); ++idx )
	{
		if( GetGestalt().Get( idx ).TypeFilter() == Geom::fWheelset )
		{
			const Wheelset& ws = m_pTrackJointFeederMotorModel->GetWheelset(idWheelset);

			Frame<Length,One> wheelsetGeomFrame;
			wheelsetGeomFrame.Init();
			wheelsetGeomFrame.RotateTan( pi/2 );
			wheelsetGeomFrame.TransportTan( ws.AxlePosition.x );
			wheelsetGeomFrame.TransportNor( ws.AxlePosition.y );
			wheelsetGeomFrame.TransportBin( -ws.Shift );
			Anchor().ToParent( wheelsetGeomFrame );
		
			GetGestalt().Get( idx ).SetFrame( wheelsetGeomFrame );
			++idWheelset;
		}
	}
}

void WheelFrame_Imp::Update( Time dt ) noexcept{
	// track joints are updated by scene ...
	//m_pTrackJointFeederMotorModel->Update( dt );
	Bogie_Imp::Update( dt );
}

Jack& WheelFrame_Imp::JackOnRail() noexcept{
	return m_pTrackJointFeederMotorModel->JackOnRail();
}

Jack& WheelFrame_Imp::JackOnDerail() noexcept{
	return m_pTrackJointFeederMotorModel->JackOnDerail();
}

Jack& WheelFrame_Imp::JackOnRollingStart() noexcept{
	return m_pTrackJointFeederMotorModel->JackOnRollingStart();
}

Jack& WheelFrame_Imp::JackOnRollingStop() noexcept{
	return m_pTrackJointFeederMotorModel->JackOnRollingStop();
}

Jack& WheelFrame_Imp::JackOnMotorActive() noexcept{
	return m_pTrackJointFeederMotorModel->JackOnMotorActive();
}

Jack& WheelFrame_Imp::JackOnMotorInactive() noexcept{
	return m_pTrackJointFeederMotorModel->JackOnMotorInactive();
}

Jack& WheelFrame_Imp::JackOnBrakeActive() noexcept{
	return m_pTrackJointFeederMotorModel->JackOnBrakeActive();
}

Jack& WheelFrame_Imp::JackOnBrakeInactive() noexcept{
	return m_pTrackJointFeederMotorModel->JackOnBrakeInactive();
}

Jack& WheelFrame_Imp::JackOnStartTilting() noexcept{
	return m_pTrackJointFeederMotorModel->JackOnStartTilting();
}

Jack& WheelFrame_Imp::JackOnStopTilting() noexcept{
	return m_pTrackJointFeederMotorModel->JackOnStopTilting();
}

Jack& WheelFrame_Imp::JackOnTrackTransition() noexcept{
	return m_pTrackJointFeederMotorModel->JackOnTrackTransition();
}

int WheelFrame_Imp::CountJacks() const noexcept{
	return Bogie_Imp::CountJacks() + 11;
}
//
//int WheelFrame_Imp::CountPlugs() const{
//	return Bogie_Imp::CountPlugs() + 0;
//}

const Jack& WheelFrame_Imp::_GetJack( int idx ) const{
	if( idx < Bogie_Imp::CountJacks() )
		return Bogie_Imp::_GetJack( idx );

	idx -= Bogie_Imp::CountJacks();

	switch( idx )
	{
	case 0:
		return m_pTrackJointFeederMotorModel->JackOnRail();
	case 1:
		return m_pTrackJointFeederMotorModel->JackOnDerail();
	case 2: 
		return m_pTrackJointFeederMotorModel->JackOnRollingStart();
	case 3:
		return m_pTrackJointFeederMotorModel->JackOnRollingStop();
	case 4:
		return m_pTrackJointFeederMotorModel->JackOnMotorActive();
	case 5:
		return m_pTrackJointFeederMotorModel->JackOnMotorInactive();
	case 6:
		return m_pTrackJointFeederMotorModel->JackOnBrakeActive();
	case 7:
		return m_pTrackJointFeederMotorModel->JackOnBrakeInactive();
	case 8:
		return m_pTrackJointFeederMotorModel->JackOnStartTilting();
	case 9:
		return m_pTrackJointFeederMotorModel->JackOnStopTilting();
	case 10:
		return m_pTrackJointFeederMotorModel->JackOnTrackTransition();
	}

	std::ostringstream stream;
	stream << "Out of range!" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::range_error( stream.str() );
}
//
//const Plug& WheelFrame_Imp::_GetPlug( int idx ) const
//{
//	if( idx < Bogie_Imp::CountPlugs() )
//		return Bogie_Imp::_GetPlug( idx );
//
//	std::ostringstream stream;
//	stream << "Out of range!" << std::endl;
//	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
//	throw std::range_error( stream.str() );
//}

void WheelFrame_Imp::SetLimits( const TrackJointLimits& wfl )
{
	TrackJointLimits limits = wfl;
	if( !m_bEnableDerailing ){
		limits.NormalForceLimitMin = -infinite__force;
		limits.NormalForceLimitMax = +infinite__force;
		limits.BinormalForceLimitMin = -infinite__force;
		limits.BinormalForceLimitMax = +infinite__force;
		limits.NormalTorqueLimit = infinite__torque;
		limits.TorqueLimit = infinite__torque;
		limits.ThresholdPosition = infinite__length;
		limits.ThresholdTangent = infinite__angle;
		limits.ThresholdNormal = infinite__angle;
		limits.ThresholdBinormal = infinite__angle;
	}

	m_pTrackJointFeederMotorModel->SetLimits( limits );
}

void WheelFrame_Imp::AlignWithAnchorTo( const spat::Frame<Length,One>& frame ) noexcept
// B * A = L  ->  B = L * Inv(A)
{
	Transformation<One> L{ frame }, AI{ Anchor() };
	AI.Invert();
	GetGestalt().SetFrame( Frame<Length,One>{ L * AI } );
}

Length WheelFrame_Imp::AnchorToParentNorth() const noexcept{
	assert( m_pSwivelParentNorth );
	if( m_pBogieChildNorth )
		return Bogie_Imp::AnchorToParentNorth();

	return UnflippedAnchor().T * Distance( { Anchor().P, Anchor().B }, m_pSwivelParentNorth->LocalAxisB() ).T;
}

Length WheelFrame_Imp::AnchorToParentSouth() const noexcept{
	assert( m_pSwivelParentSouth );
	if( m_pBogieChildNorth )
		return Bogie_Imp::AnchorToParentSouth();

	return UnflippedAnchor().T * Distance( { Anchor().P, Anchor().B }, m_pSwivelParentSouth->LocalAxisB() ).T;
}

Length WheelFrame_Imp::AnchorToChildSouth() const noexcept{
	assert( m_pSwivelChildNorth );
	if( m_pBogieChildNorth )
		return Bogie_Imp::AnchorToChildSouth();

	return UnflippedAnchor().T * Distance( { Anchor().P, Anchor().B }, m_pSwivelChildSouth->LocalAxisA() ).T;
}

Vector<One> WheelFrame_Imp::LocalHorizontalDirection() const noexcept{
	return UnflippedAnchor().T;
}

WheelFrame_Imp::Event_Imp::Event_Imp( WheelFrame_Imp& wheelFrame )
	: m_Parent{ wheelFrame }
{
}

WheelFrame& WheelFrame_Imp::Event_Imp::GetWheelFrame() const noexcept{
	return m_Parent;
}

///////////////////////////////////////
//std::shared_ptr<Bogie> RemoveUnusedWheelFrames( std::shared_ptr<Bogie> pBogie )
//{
//	std::shared_ptr<Bogie> pRetval = pBogie;
//	for( std::shared_ptr<Bogie> pNextBogie = pBogie->GetFirst(); 
//		 pNextBogie; 
//		 pNextBogie = pNextBogie->GetNext() )
//	{
//		if( std::shared_ptr<WheelFrame> pWheelFrame = std::dynamic_pointer_cast<WheelFrame>(pNextBogie); 
//			pWheelFrame && pWheelFrame->CntWheelsets() == 0 )
//		{
//			std::shared_ptr<Bogie_Imp> pNewBogie = std::make_shared<Bogie_Imp>( std::move(*std::dynamic_pointer_cast<Bogie_Imp>(pNextBogie)) );
//			pNewBogie->SetWeakPointerToSelf( pNewBogie );
//
//			if( pNextBogie == pBogie )
//				pRetval = pNewBogie;
//
//			pNextBogie = pNewBogie;
//		}
//	}
//
//	return pRetval;
//}
///////////////////////////////////////
}

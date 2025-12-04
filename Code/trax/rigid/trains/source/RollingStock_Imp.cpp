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

#include "RollingStock_Imp.h"
#include "trax/rigid/Gestalt.h"
//#include "trax/rigid/Simulator.h"
//#include "trax/rigid/Wheelset.h"
//#include "trax/rigid/Scene.h"
#include "trax/rigid/trains/WheelFrame.h"

//#include "spat/Position2D.h"

#include <iostream>

namespace trax{
	using namespace spat;
///////////////////////////////////////
std::shared_ptr<RollingStock> RollingStock::Make() noexcept{
	try{
		std::shared_ptr<RollingStock_Imp> pRetval = std::make_shared<RollingStock_Imp>();
		
		if( pRetval )
			pRetval->SetWeakPointerToSelf( pRetval );
	
		return pRetval;
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

RollingStock_Imp::RollingStock_Imp() noexcept
	: RollingStock_Base	{}
	, m_pNorthTip		{ nullptr }
	, m_pSouthTip		{ nullptr }
	, m_Overhangs		{ 0_m, 0_m }
{
	for( Jack& jack : *static_cast<JackEnumerator*>(this) )
		jack.Reference( "parent", RollingStock_Imp::TypeName() );
}

RollingStock_Imp::~RollingStock_Imp()
{
	for( auto& pBogie : m_Bogies )
		pBogie->SetRollingStock( nullptr );
}

const char* RollingStock_Imp::TypeName() const noexcept{
	return "RollingStock";
}

bool RollingStock_Imp::IsValid() const noexcept
{
	if( m_Bogies.empty() ){
		std::cerr << __FILE__ << '(' << __LINE__ << ')' << "No bogies found!" << std::endl;
		return false;
	}

	if( m_TopmostBogies.empty() ){
		std::cerr << __FILE__ << '(' << __LINE__ << ')' << "No topmost bogies found!" << std::endl;
		return false;
	}

	if( m_WheelFrames.empty() ){
		std::cerr << __FILE__ << '(' << __LINE__ << ')' << "No wheel frames found!" << std::endl;
		return false;
	}

	if( m_pNorthTip == nullptr )
		return false;

	if( m_pSouthTip == nullptr )
		return false;

	for( const auto& pBogie : m_Bogies )
	{
		if( pBogie == nullptr )
			return false;

		if( !pBogie->IsValid() ){
			std::cerr << __FILE__ << '(' << __LINE__ << ')' << "Bogie is not valid!" << std::endl;
			return false;
		}

		if( pBogie->GetRollingStock() != this )
			return false;

		if( auto parentNorth = pBogie->GetParent( EndType::north ); parentNorth.first )
		{
			if( parentNorth.first->GetChild( EndType::south ).first != pBogie ){
				std::cerr << __FILE__ << '(' << __LINE__ << ')' << "Parent-child relationship broken! A RollingStock assumes strict north-to-south relationships." << std::endl;
				return false;
			}
		}

		if( auto parentSouth = pBogie->GetParent( EndType::south ); parentSouth.first )
		{
			if( parentSouth.first->GetChild( EndType::north ).first != pBogie ){
				std::cerr << __FILE__ << '(' << __LINE__ << ')' << "Parent-child relationship broken! A RollingStock assumes strict north-to-south relationships." << std::endl;
				return false;
			}
		}

		if( auto childNorth = pBogie->GetChild( EndType::north ); childNorth.first )
		{
			if( childNorth.first->GetParent( EndType::south ).first != pBogie ){
				std::cerr << __FILE__ << '(' << __LINE__ << ')' << "Parent-child relationship broken! A RollingStock assumes strict north-to-south relationships." << std::endl;
				return false;
			}
		}

		if( auto childSouth = pBogie->GetChild( EndType::south ); childSouth.first )
		{
			if( childSouth.first->GetParent( EndType::north ).first != pBogie ){
				std::cerr << __FILE__ << '(' << __LINE__ << ')' << "Parent-child relationship broken! A RollingStock assumes strict north-to-south relationships." << std::endl;
				return false;
			}
		}
	}

	for( const auto& pTopmostBogie : m_TopmostBogies )
		if( pTopmostBogie == nullptr )
			return false;

	for( const auto& pWheelFrame : m_WheelFrames )
		if( pWheelFrame == nullptr )
			return false;

	return true;
}

void RollingStock_Imp::Rail( const Location& location, bool bMoveTo )
{
	assert( !m_TopmostBogies.empty() );

	m_TopmostBogies.front()->Rail( location, bMoveTo );

	for( auto iter = m_TopmostBogies.begin() + 1; iter != m_TopmostBogies.end(); ++iter )
		//The northmost child wheel frame was railed by previous Rail() call.
		(*iter)->Rail( GetNorthmostChildWheelFrame( **iter ).GetLocation(), bMoveTo ); 																				
}

void RollingStock_Imp::Rail( const Location& location, bool bMoveTo, DistanceType )
{
	Rail( location, bMoveTo );
}

Location RollingStock_Imp::GetLocation() const noexcept
{
	assert( !m_Bogies.empty() );
	return m_Bogies.front()->GetLocation();
}

void RollingStock_Imp::DeRail() noexcept
{
	for( auto& pBogie : m_TopmostBogies )
		pBogie->DeRail();
}

bool RollingStock_Imp::IsRailed() const noexcept
{
	for( const auto& pWheelFrames : m_WheelFrames )
		if( !pWheelFrames->IsRailed() )
			return false;

	return true;
}

spat::Frame<Length, One> RollingStock_Imp::GetGlobalAnchor() const{
	return m_Bogies.front()->GetGlobalAnchor();
}

void RollingStock_Imp::ResetToGlobalAnchor( const spat::Frame<Length,One>& anchor )
{
	assert( !m_TopmostBogies.empty() );

	m_TopmostBogies.front()->ResetToGlobalAnchor( anchor );

	for( auto iter = m_TopmostBogies.begin() + 1; iter != m_TopmostBogies.end(); ++iter )
		//The northmost child wheel frame was resetted by previous ResetToGlobalAnchor() call.
		(*iter)->ResetToGlobalAnchor( GetNorthmostChildWheelFrame( **iter ).GetGlobalAnchor() );
}

void RollingStock_Imp::ResetToGlobalAnchor( 
	const spat::Frame<Length,One>& anchorPose, 
	DistanceType )
{
	ResetToGlobalAnchor( anchorPose );
}

void RollingStock_Imp::SetVelocity( Velocity velocity ){
	for( auto& pBogie : m_TopmostBogies )
		pBogie->SetVelocity( velocity );
}

Velocity RollingStock_Imp::GetVelocity() const{
	return m_WheelFrames.front()->GetVelocity();
}

void RollingStock_Imp::TargetVelocity( Velocity tvelocity ){
	for( auto& pBogie : m_TopmostBogies )
		pBogie->TargetVelocity( tvelocity );
}

Velocity RollingStock_Imp::TargetVelocity() const noexcept{
	return m_TopmostBogies.front()->TargetVelocity();
}

RailRunner::EndType RollingStock_Imp::TargetDirection() const noexcept{
	return m_TopmostBogies.front()->TargetDirection();
}

Power RollingStock_Imp::MaximumPowerOutput() const noexcept
{
	Power maxPower = 0_W;

	for( const auto& pWheelFrame : m_WheelFrames )
		maxPower += pWheelFrame->MaximumPowerOutput();

	return maxPower;
}

Power RollingStock_Imp::CurrentPowerOutput() const noexcept
{
	Power currentPower = 0_W;

	for( const auto& pWheelFrame : m_WheelFrames )
		currentPower += pWheelFrame->CurrentPowerOutput();

	return currentPower;
}

Velocity RollingStock_Imp::MaximumVelocity() const noexcept
{
	Velocity maxVelocity = Velocity{infinite};

	for( auto& pBogie : m_TopmostBogies )
		maxVelocity = std::min(maxVelocity,pBogie->MaximumVelocity());

	return maxVelocity;
}

void RollingStock_Imp::Thrust( One byfactor )
{
	for( auto& pBogie : m_TopmostBogies )
		pBogie->Thrust( byfactor );
}

Force RollingStock_Imp::ThrustAbsolute() const noexcept
{
	Force thrustAbsolute = 0_N;

	for( const auto& pWheelFrame : m_WheelFrames )
		thrustAbsolute += pWheelFrame->ThrustAbsolute();

	return thrustAbsolute;
}

Force RollingStock_Imp::MaxThrust() const noexcept
{
	Force maxThrust = 0_N;

	for( const auto& pWheelFrame : m_WheelFrames )
		maxThrust += pWheelFrame->MaxThrust();

	return maxThrust;
}

Force RollingStock_Imp::MaxThrust( Velocity velocity ) const noexcept
{
	Force maxThrust = 0_N;

	for( const auto& pWheelFrame : m_WheelFrames )
		maxThrust += pWheelFrame->MaxThrust( velocity );

	return maxThrust;
}

void RollingStock_Imp::Brake( One byfactor )
{
	for( auto& pBogie : m_TopmostBogies )
		pBogie->Brake( byfactor );
}


Force RollingStock_Imp::BrakeAbsolute() const noexcept
{
	Force brakeAbsolute = 0_N;

	for( const auto& pWheelFrame : m_WheelFrames )
		brakeAbsolute += pWheelFrame->BrakeAbsolute();

	return brakeAbsolute;
}

Force RollingStock_Imp::MaxBrake() const noexcept
{
	Force maxBrake = 0_N;

	for( const auto& pWheelFrame : m_WheelFrames )
		maxBrake += pWheelFrame->MaxBrake();

	return maxBrake;
}

Mass RollingStock_Imp::TotalMass() const noexcept
{
	Mass totalMass = 0_kg;

	for( const auto& pBogie : m_Bogies )
		totalMass += pBogie->TotalMass();

	return totalMass;
}

bool RollingStock_Imp::IsFinal() const noexcept{
	return true;
}

Length RollingStock_Imp::GetOverhang( EndType end, DistanceType ) const noexcept
// DistanceType makes no difference here. It determains the internal distances between
// TrainComponents.
{
	switch( end ){
	case EndType::any:
	case EndType::north:
		return m_Overhangs.first;
	case EndType::south:
		return m_Overhangs.second;
	default:
		return 0_m;
	}
}

Length RollingStock_Imp::GetLength( DistanceType ) const noexcept
// DistanceType makes no difference here. It determains the internal distances between
// TrainComponents.
{
	return m_Overhangs.first + m_Overhangs.second;
}

std::pair<Bogie&,RailRunner::EndType> RollingStock_Imp::GetTipAt( EndType end ){
	switch( end )
	{
	case EndType::any:
	case EndType::north:
		if( m_pNorthTip )
			return { *m_pNorthTip, EndType::north };
		break;
	case EndType::south:
		if( m_pSouthTip )
			return { *m_pSouthTip, EndType::south };
		break;
	default:
		throw std::invalid_argument( "RollingStock_Imp::GetTipAt: Invalid end type!" );
	}

	std::ostringstream stream;
	stream << "RollingStock_Imp::GetTipAt: No tip found at end " << static_cast<int>(end) << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::invalid_argument( stream.str() );
}

std::pair<const Bogie&,RailRunner::EndType> RollingStock_Imp::GetTipAt( EndType end ) const{
	switch( end )
	{
	case EndType::any:
	case EndType::north:
		if( m_pNorthTip )
			return { *m_pNorthTip, EndType::north };
		break;
	case EndType::south:
		if( m_pSouthTip )
			return { *m_pSouthTip, EndType::south };
		break;
	default:
		throw std::invalid_argument( "RollingStock_Imp::GetTipAt: Invalid end type!" );
	}

	std::ostringstream stream;
	stream << "RollingStock_Imp::GetTipAt: No tip found at end " << static_cast<int>(end) << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::invalid_argument( stream.str() );
}

std::shared_ptr<RollingStock> RollingStock_Imp::ThisRollingStock() const noexcept
{
	return std::dynamic_pointer_cast<RollingStock>(This());
}

bool RollingStock_Imp::Attach( Bogie& bogie ) noexcept
{
	Clear();

	std::shared_ptr<Bogie_Imp> pBogie = std::dynamic_pointer_cast<Bogie_Imp>(bogie.GetFirst());
	while( pBogie ){
		pBogie->SetRollingStock( this );

		m_Bogies.push_back( pBogie );

		if( pBogie->GetParent( EndType::any ).first == nullptr )
			m_TopmostBogies.push_back( pBogie );

		if( std::shared_ptr<WheelFrame> pWheelFrame = std::dynamic_pointer_cast<WheelFrame>(pBogie); pWheelFrame )
			m_WheelFrames.push_back( pWheelFrame );

		pBogie = std::dynamic_pointer_cast<Bogie_Imp>(pBogie->GetNext());
	}

	CalculateOverhangs();

	ReconnectJacks();

	if( !IsValid() )
	{
		Clear();
		return false;
	}

	return true;
}

int RollingStock_Imp::GetNumberOfBogies() const noexcept{
	return common::narrow_cast<int>(m_Bogies.size());
}

Bogie& RollingStock_Imp::GetBogie( int idx ) const{
	return *m_Bogies.at( idx );
}

int RollingStock_Imp::GetNumberOfTopmostParents() const noexcept{
	return common::narrow_cast<int>(m_TopmostBogies.size());
}

Bogie& RollingStock_Imp::GetTopmostParent( int idx ) const{
	return *m_TopmostBogies.at( idx );
}

int RollingStock_Imp::GetNumberOfWheelFrames() const noexcept{
	return common::narrow_cast<int>(m_WheelFrames.size());
}

WheelFrame& RollingStock_Imp::GetWheelFrame( int idx ) const{
	return *m_WheelFrames.at( idx );
}

std::pair<std::shared_ptr<RollingStock>,RailRunner::EndType> 
RollingStock_Imp::GetCoupledRollingStock( EndType end ) const noexcept
{
	if( end == EndType::north ||
		end == EndType::south )
	{
		std::pair<const Bogie&,RailRunner::EndType> coupling = GetTipAt( end );
		if( std::pair<std::shared_ptr<Bogie>,RailRunner::EndType> coupled = coupling.first.GetCoupledBogie( coupling.second ); coupled.first )
		{
			if( RollingStock* pRollingStock = coupled.first->GetRollingStock(); pRollingStock )
			{
				return std::make_pair( pRollingStock->ThisRollingStock(), coupled.second );
			}
		}
	}
	else if( end == EndType::any )
	{
		if( IsCoupled( EndType::north ) )
			return GetCoupledRollingStock( EndType::north );
		if( IsCoupled( EndType::south ) )
			return GetCoupledRollingStock( EndType::south );
	}

	return { nullptr, RailRunner::EndType::none };
}

Jack& RollingStock_Imp::JackOnHoot() noexcept{
	return m_JackOnHoot;
}

Jack& RollingStock_Imp::JackOnSteam() noexcept{
	return m_JackOnSteam;
}

int trax::RollingStock_Imp::CountJacks() const noexcept{
	return RollingStock_Base::CountJacks() + 2;
}

const Jack& trax::RollingStock_Imp::_GetJack( int idx ) const
{
	if( idx < RollingStock_Base::CountJacks() )
		return RollingStock_Base::_GetJack( idx );

	idx -= RollingStock_Base::CountJacks();

	switch( idx )
	{
	case 0:
		return m_JackOnHoot;
	case 1:
		return m_JackOnSteam;
	}

	std::ostringstream stream;
	stream << "Out of range!" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::range_error( stream.str() );
}

void RollingStock_Imp::ConnectJacks()
{
	RollingStock_Base::ConnectJacks();

	for( auto& pWheelFrames : m_WheelFrames )
	{
		pWheelFrames->JackOnRail().InsertAndAppend( &m_PlugRail.Make() );
		pWheelFrames->JackOnDerail().InsertAndAppend( &m_PlugDerail.Make() );
	}
}

void RollingStock_Imp::CalculateOverhangs() noexcept
{
	if( !m_WheelFrames.empty() )
	{
		Frame<Length,One> bodyFrame;
		Frame<Length,One> globalAnchor = m_WheelFrames.front()->Anchor();
		m_WheelFrames.front()->GetGestalt().GetFrame( bodyFrame );
		bodyFrame.ToParent( globalAnchor );

		m_Overhangs = std::make_pair( -infinite__length, -infinite__length );

		for( const auto& pBogie : m_Bogies ){
			Bogie::CouplingProps cprops = pBogie->GetCouplingProps( EndType::north );
			if( cprops.CouplingTypeIdx >= 0 ){
				Position<Length> pos = cprops.Position.Center();
				pBogie->GetGestalt().GetFrame( bodyFrame );
				bodyFrame.ToParent( pos );
				Length overhang = (pos - globalAnchor.P) * globalAnchor.T;
				if( overhang > m_Overhangs.first ){
					m_Overhangs.first = overhang;
					m_pNorthTip = pBogie.get();
				}
			}

			cprops = pBogie->GetCouplingProps( EndType::south );
			if( cprops.CouplingTypeIdx >= 0 ){
				Position<Length> pos = cprops.Position.Center();
				pBogie->GetGestalt().GetFrame( bodyFrame );
				bodyFrame.ToParent( pos );
				Length overhang = (pos - globalAnchor.P) * -globalAnchor.T;
				if( overhang > m_Overhangs.second ){
					m_Overhangs.second = overhang;
					m_pSouthTip = pBogie.get();
				}
			}
		}
	}
}

const WheelFrame& RollingStock_Imp::GetNorthmostChildWheelFrame( const Bogie& bogie ) const noexcept
{
	if( bogie.GetChild( EndType::north ).first )
		return GetNorthmostChildWheelFrame( *bogie.GetChild( EndType::north ).first );
	else
		return dynamic_cast<const WheelFrame&>(bogie);
}

void RollingStock_Imp::Clear() noexcept
{
	RollingStock_Base::Clear();

	for( auto& pBogie : m_Bogies )
		pBogie->SetRollingStock( nullptr );

	m_Bogies.clear();
	m_TopmostBogies.clear();
	m_WheelFrames.clear();

	m_pNorthTip = nullptr;
	m_pSouthTip = nullptr;
	m_Overhangs = { 0_m,0_m };
}
///////////////////////////////////////
}
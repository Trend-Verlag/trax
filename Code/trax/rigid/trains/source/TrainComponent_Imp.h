//	trax track library
//	AD 2024 
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

#pragma once

#include "../Bogie.h"
#include "../RollingStock.h"
#include "../Train.h"
#include "trax/rigid/Gestalt.h"
#include "trax/source/Plug_Imp.h"

#include "RailRunner_Imp.h"

#include <iostream>

namespace trax{


	template<class Base>
	class TrainComponent_Imp :	public RailRunner_Imp<Base>,
								public PlugEnumerator{
	public:
		TrainComponent_Imp() noexcept;

		// Identified:
		using RailRunner_Imp<Base>::ID;

		void ID( IDType id ) noexcept override;


		// RailRunner:
		bool IsValid() const noexcept override;

		void Rail( const class Location& location, bool bMoveTo = true ) override;

		void ActivateCoupling( EndType end ) override;

		void DeactivateCoupling( EndType end ) override;

		bool IsActivated( EndType end ) const noexcept override;

		bool Uncouple( EndType end, bool btriggerPulses = true ) noexcept override;

		bool IsCoupled( EndType end = EndType::any ) const noexcept override;
		
		Length GetCouplingHeight( EndType end ) const noexcept override;


		// TrainComponent:
		std::shared_ptr<TrainComponent> ThisTrainComponent() const noexcept override;

		Train* GetTrain() const noexcept override;

		Orientation GetOrientation() const noexcept override;

		std::pair<Train*,Orientation> GetTopmostTrain() const noexcept override;

		bool IsFinal() const noexcept override;

		bool IsParent( const Train& train ) const noexcept override;

		void Rail( const Location& location, bool bMoveTo, TrainComponent::DistanceType distance, bool bFailOnReservationConflicts = false ) override;

		bool Couple( EndType end, TrainComponent& with, EndType withEnd, bool btriggerPulses = true ) override;

		std::pair<std::shared_ptr<TrainComponent>,EndType> GetCoupledTrainComponent( EndType end ) const override;
		
		Length GetCouplingLength( EndType end, TrainComponent::DistanceType type = TrainComponent::DistanceType::max ) const override;

		bool MakeReservation() const noexcept override;

		bool DeleteReservation() const noexcept override;

	
		// Jacks 'N Plugs:

		Jack& JackOnRail() noexcept override;

		Jack& JackOnDerail() noexcept override;

		Jack& JackOnCouple( EndType end ) override;

		Jack& JackOnUnCouple( EndType end ) override;

		Jack& JackOnCouplingActivated( EndType end ) override;

		Jack& JackOnCouplingDeactivated( EndType end ) override;

		// Inherited via JackEnumerator
		int CountJacks() const noexcept override;

		// Inherited via PlugEnumerator
		int CountPlugs() const override;
	protected:
		const Jack& _GetJack( int idx ) const override;
		const Plug& _GetPlug( int idx ) const override;


		// TrainComponent_Imp:
		void SetTrain( Train* pTrain ) noexcept override{
			m_pTrain = pTrain;
		}

		void SetOrientation( Orientation orientation ) noexcept override{
			m_TrainOrientation = orientation;
		}

		virtual void DisconnectJacks() noexcept;
		virtual void ConnectJacks();
		void ReconnectJacks();
		virtual void Clear() noexcept;

		MultiPlug_Imp<Plug_Imp_ParentPointer<TrainComponent_Imp<Base>>> m_PlugRail;
		MultiPlug_Imp<Plug_Imp_ParentPointer<TrainComponent_Imp<Base>>> m_PlugDerail;
	private:
		Train*			m_pTrain;
		Orientation		m_TrainOrientation;
		mutable bool	m_bMadeReservation;

		bool m_bRailed;
		void OnRail() noexcept;
		void OnDerail() noexcept;

		Jack_Imp m_JackOnRail								{ "JackOnRail" };
		Jack_Imp m_JackOnDerail								{ "JackOnDerail" };
		MultiPlugJack_Imp m_JackOnCoupleNorth				{ "JackOnCoupleNorth" };
		MultiPlugJack_Imp m_JackOnCoupleSouth				{ "JackOnCoupleSouth" };
		MultiPlugJack_Imp m_JackOnCouple					{ "JackOnCouple" };
		MultiPlugJack_Imp m_JackOnUnCoupleNorth				{ "JackOnUnCoupleNorth" };
		MultiPlugJack_Imp m_JackOnUnCoupleSouth				{ "JackOnUnCoupleSouth" };
		MultiPlugJack_Imp m_JackOnUnCouple					{ "JackOnUnCouple" };
		MultiPlugJack_Imp m_JackOnCouplingActivatedNorth	{ "JackOnCouplingActivatedNorth" };
		MultiPlugJack_Imp m_JackOnCouplingActivatedSouth	{ "JackOnCouplingActivatedSouth" };
		MultiPlugJack_Imp m_JackOnCouplingActivated			{ "JackOnCouplingActivated" };
		MultiPlugJack_Imp m_JackOnCouplingDeactivatedNorth	{ "JackOnCouplingDeactivatedNorth" };
		MultiPlugJack_Imp m_JackOnCouplingDeactivatedSouth	{ "JackOnCouplingDeactivatedSouth" };
		MultiPlugJack_Imp m_JackOnCouplingDeactivated		{ "JackOnCouplingDeactivated" };
	};
	

template<class Base>
TrainComponent_Imp<Base>::TrainComponent_Imp() noexcept
	: m_PlugRail		{ *this, &TrainComponent_Imp<Base>::OnRail }
	, m_PlugDerail		{ *this, &TrainComponent_Imp<Base>::OnDerail }
	, m_pTrain			{ nullptr }
	, m_TrainOrientation{ Orientation::Value::para }
	, m_bMadeReservation{ false }
	, m_bRailed			{ false }
{
	m_PlugRail.Reference( "name", "PlugRail" );
	m_PlugDerail.Reference( "name", "PlugDerail" );

	m_JackOnCoupleNorth.Insert( &m_JackOnCouple.PlugToPulse().Make() );
	m_JackOnCoupleSouth.Insert( &m_JackOnCouple.PlugToPulse().Make() );
	m_JackOnUnCoupleNorth.Insert( &m_JackOnUnCouple.PlugToPulse().Make() );
	m_JackOnUnCoupleSouth.Insert( &m_JackOnUnCouple.PlugToPulse().Make() );
	m_JackOnCouplingActivatedNorth.Insert( &m_JackOnCouplingActivated.PlugToPulse().Make() );
	m_JackOnCouplingActivatedSouth.Insert( &m_JackOnCouplingActivated.PlugToPulse().Make() );
	m_JackOnCouplingDeactivatedNorth.Insert( &m_JackOnCouplingDeactivated.PlugToPulse().Make() );
	m_JackOnCouplingDeactivatedSouth.Insert( &m_JackOnCouplingDeactivated.PlugToPulse().Make() );
}

template<class Base>
inline void TrainComponent_Imp<Base>::ID( IDType id ) noexcept
{
	if( ID() != id )
		DeleteReservation();

	RailRunner_Imp<Base>::ID( id );
}

template<class Base>
bool TrainComponent_Imp<Base>::IsValid() const noexcept
{
	if( !RailRunner_Imp<Base>::IsValid() )
		return false;

	if( !ThisTrainComponent() )
		return false;

	return true;
}

template<class Base>
void TrainComponent_Imp<Base>::Rail( const Location& location, bool bMoveTo )
{
	Rail( location, bMoveTo, TrainComponent::DistanceType::actual );
}

template<class Base>
void TrainComponent_Imp<Base>::ActivateCoupling( EndType end )
{
	switch( end )
	{
		case EndType::none:
			return;
		case EndType::both:
		case EndType::any:
			ActivateCoupling( EndType::north );
			ActivateCoupling( EndType::south );
			return;
	}

	std::pair<Bogie&,EndType> coupling = this->GetTipAt( end );
	coupling.first.ActivateCoupling( coupling.second );
}

template<class Base>
void TrainComponent_Imp<Base>::DeactivateCoupling( EndType end )
{
	switch( end )
	{
		case EndType::none:
			return;
		case EndType::both:
		case EndType::any:
			DeactivateCoupling( EndType::north );
			DeactivateCoupling( EndType::south );
			return;
	}

	std::pair<Bogie&,EndType> coupling = this->GetTipAt( end );
	coupling.first.DeactivateCoupling( coupling.second );
}

template<class Base>
bool TrainComponent_Imp<Base>::IsActivated( EndType end ) const noexcept
{
	switch( end )
	{
		case EndType::none:
			return false;
		case EndType::any:
			return IsActivated( EndType::north ) ||
				IsActivated( EndType::south );
		case EndType::both:
			return IsActivated( EndType::north ) &&
				IsActivated( EndType::south );
	}	

	try{
		std::pair<const Bogie&, EndType> coupling = this->GetTipAt( end );
		return coupling.first.IsActivated( coupling.second );
	}
	catch( const std::exception& e ){
		std::cerr << "TrainComponent_Imp::IsActivated: " << e.what() << std::endl;
		return false;
	}
	catch( ... ){
		std::cerr << "TrainComponent_Imp::IsActivated: unknown exception." << std::endl;
		return false;
	}
}

template<class Base>
bool TrainComponent_Imp<Base>::Uncouple( EndType end, bool btriggerPulses ) noexcept
{
	switch( end )
	{
		case EndType::none:
			return false;
		case EndType::any:
			return Uncouple( EndType::north, btriggerPulses ) ||
				Uncouple( EndType::south, btriggerPulses );
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

	try{
		std::pair<Bogie&, EndType> coupling = this->GetTipAt( end );
		return coupling.first.Uncouple( coupling.second, btriggerPulses );
	}
	catch( const std::exception& e ){
		std::cerr << "TrainComponent_Imp::Uncouple: " << e.what() << std::endl;
		return false;
	}
	catch( ... ){
		std::cerr << "TrainComponent_Imp::Uncouple: unknown exception." << std::endl;
		return false;
	}
}

template<class Base>
bool TrainComponent_Imp<Base>::IsCoupled( EndType end ) const noexcept
{
	switch( end )
	{
		case EndType::none:
			return false;
		case EndType::any:
			return IsCoupled( EndType::north ) ||
				IsCoupled( EndType::south );
		case EndType::both:
			return IsCoupled( EndType::north ) &&
				IsCoupled( EndType::south );
	}

	try{
		std::pair<const Bogie&, EndType> coupling = this->GetTipAt( end );
		return coupling.first.IsCoupled( coupling.second );
	}
	catch( const std::exception& e ){
		std::cerr << "TrainComponent_Imp::IsCoupled: " << e.what() << std::endl;
		return false;
	}
	catch( ... ){
		std::cerr << "TrainComponent_Imp::IsCoupled: unknown exception." << std::endl;
		return false;
	}
}

template<class Base>
Length TrainComponent_Imp<Base>::GetCouplingHeight( EndType end ) const noexcept
{
	switch( end )
	{
		case EndType::none:
			return 0_m;
		case EndType::any:
			return std::min( GetCouplingHeight( EndType::north ), GetCouplingHeight( EndType::south ) );
		case EndType::both:
			return std::max( GetCouplingHeight( EndType::north ), GetCouplingHeight( EndType::south ) );
		default:
			break;
	}

	try{
		std::pair<const Bogie&, EndType> coupling = this->GetTipAt( end );
		return coupling.first.GetCouplingHeight( coupling.second );
	}
	catch( const std::exception& e ){
		std::cerr << "TrainComponent_Imp::GetCouplingHeight: " << e.what() << std::endl;
		return 0_m;
	}
	catch( ... ){
		std::cerr << "TrainComponent_Imp::GetCouplingHeight: unknown exception." << std::endl;
		return 0_m;
	}
}

template<class Base>
std::shared_ptr<TrainComponent> TrainComponent_Imp<Base>::ThisTrainComponent() const noexcept{
	return std::dynamic_pointer_cast<TrainComponent>( this->This() );
}

template<class Base>
Train* TrainComponent_Imp<Base>::GetTrain() const noexcept{
	return m_pTrain;
}

template<class Base>
Orientation TrainComponent_Imp<Base>::GetOrientation() const noexcept{
	return m_TrainOrientation;
}

template<class Base>
std::pair<Train*,Orientation> TrainComponent_Imp<Base>::GetTopmostTrain() const noexcept
{
	if( m_pTrain )
	{
		Train* pTrain = m_pTrain;
		Orientation orientation = m_TrainOrientation;

		while( pTrain->GetTrain() )
		{
			orientation += pTrain->GetOrientation();
			pTrain = pTrain->GetTrain();
		}

		return std::make_pair( pTrain, orientation );
	}

	return {};
}

template<class Base>
bool TrainComponent_Imp<Base>::IsFinal() const noexcept{
	return false;
}

template<class Base>
bool TrainComponent_Imp<Base>::IsParent( const Train& train ) const noexcept
{
	Train* pTrain = GetTrain(); 
	while( pTrain )
	{
		if( pTrain == &train )
			return true;

		pTrain = pTrain->GetTrain();
	}
	
	return false;
}

template<class Base>
void TrainComponent_Imp<Base>::Rail( 
	const Location& location, 
	bool bMoveTo, 
	TrainComponent::DistanceType distance, 
	bool bFailOnReservationConflicts )
{
	if( bFailOnReservationConflicts && ID() )
	{
		location.Reserve( 
			{ -this->GetOverhang( EndType::south ), 
			   this->GetOverhang( EndType::north ) }, 
			ID() 
		);

		bool bOverlaps = !location.Overlaps( ID() ).empty();
		location.DeleteReservation( ID() );
		if( bOverlaps )
			throw std::logic_error( "TrainComponent_Imp::Rail: reservation conflict detected." );
	}
}

template<class Base>
bool TrainComponent_Imp<Base>::Couple( 
	EndType end, 
	TrainComponent& with, 
	EndType withEnd, 
	bool btriggerPulses )
{
	if( end == EndType::none || withEnd == EndType::none )
		return false;
	if( end == EndType::any || withEnd == EndType::any )
		return false;
	if( end == EndType::both || withEnd == EndType::both )
		return false;
	// No self coupling:
	if( &with == this )
		return false;

	std::pair<Bogie&,EndType> couplingA = this->GetTipAt( end );
	std::pair<Bogie&,EndType> couplingB = with.GetTipAt( withEnd );

	if( couplingA.first.IsCoupled( couplingA.second ) ||
		couplingB.first.IsCoupled( couplingB.second ) )
	{
		std::cerr << "TrainComponent_Imp::Couple: coupling already established." << std::endl;
		return false;
	}

	return couplingA.first.Couple( couplingA.second, couplingB.first, couplingB.second, btriggerPulses );
}

template<class Base>
std::pair<std::shared_ptr<TrainComponent>,EndType> 
TrainComponent_Imp<Base>::GetCoupledTrainComponent( EndType end ) const
{
	if( end == EndType::north ||
		end == EndType::south )
	{
		std::pair<const Bogie&,EndType> coupling = this->GetTipAt( end );
		if( std::pair<std::shared_ptr<Bogie>,EndType> coupled = coupling.first.GetCoupledBogie( coupling.second ); coupled.first )
		{
			if( TrainComponent* pTopMostTrainComponent = coupled.first->GetRollingStock(); pTopMostTrainComponent )
			{
				EndType TopMostTrainEnd = coupled.second;
				while( pTopMostTrainComponent->GetTrain() && !IsParent( *pTopMostTrainComponent->GetTrain() ) )
				{
					TopMostTrainEnd = pTopMostTrainComponent->GetOrientation() == Orientation::Value::para ? TopMostTrainEnd : !TopMostTrainEnd;
					pTopMostTrainComponent = pTopMostTrainComponent->GetTrain();
				}

				return std::make_pair( pTopMostTrainComponent->ThisTrainComponent(), TopMostTrainEnd );
			}
		}
	}
	else if( end == EndType::any )
	{
		if( IsCoupled( EndType::north ) )
			return GetCoupledTrainComponent( EndType::north );
		if( IsCoupled( EndType::south ) )
			return GetCoupledTrainComponent( EndType::south );
	}

	return { nullptr, EndType::none };
}

template<class Base>
Length TrainComponent_Imp<Base>::GetCouplingLength( EndType end, TrainComponent::DistanceType distance ) const
{
	switch( end )
	{
		case EndType::none:
			return 0_m;
		case EndType::any:
			return std::min( GetCouplingLength( EndType::north ), GetCouplingLength( EndType::south ) );
		case EndType::both:
			return std::max( GetCouplingLength( EndType::north ), GetCouplingLength( EndType::south ) );
	}

	std::pair<const Bogie&,EndType> coupling = this->GetTipAt( end );
	if( std::pair<std::shared_ptr<Bogie>,EndType> coupled = coupling.first.GetCoupledBogie( coupling.second ); coupled.first )
	{
		switch( distance )
		{
			case TrainComponent::DistanceType::actual:
				return GetCouplingDistance( coupling, { *coupled.first, coupled.second } );
			case TrainComponent::DistanceType::min:
				return 0_m;
			case TrainComponent::DistanceType::max:
				return coupling.first.GetCouplingProps( coupling.second ).Position.Radius() + coupled.first->GetCouplingProps( coupled.second ).Position.Radius();
			case TrainComponent::DistanceType::half:
				return (coupling.first.GetCouplingProps( coupling.second ).Position.Radius() + coupled.first->GetCouplingProps( coupled.second ).Position.Radius()) / 2;
		}
	}

	return 0_m;
}

template<class Base>
bool TrainComponent_Imp<Base>::MakeReservation() const noexcept
{
	if( m_bRailed && ID() )
	{
		this->GetLocation().Reserve( 
			{ -this->GetOverhang( EndType::south ), 
			   this->GetOverhang( EndType::north ) }, 
			ID() 
		);

		m_bMadeReservation = true;

		return true;
	}

	return false;
}

template<class Base>
bool TrainComponent_Imp<Base>::DeleteReservation() const noexcept
{
	if( m_bRailed && ID() )
	{
		if( m_bMadeReservation )
		{		
			this->GetLocation().DeleteReservation( ID() );
			m_bMadeReservation = false;
			return true;
		}
	}

	return false;
}

template<class Base>
Jack& TrainComponent_Imp<Base>::JackOnRail() noexcept{
	return m_JackOnRail;
}

template<class Base>
Jack& TrainComponent_Imp<Base>::JackOnDerail() noexcept{
	return m_JackOnDerail;
}

template<class Base>
Jack& TrainComponent_Imp<Base>::JackOnCouple( EndType end )
{
	switch( end )
	{
		case EndType::north:
			return m_JackOnCoupleNorth;
		case EndType::south:
			return m_JackOnCoupleSouth;
		case EndType::any:
			return m_JackOnCouple;
		default:
			throw std::invalid_argument( "TrainComponent_Imp::JackOnCouple: end is none." );
	}
}

template<class Base>
Jack& TrainComponent_Imp<Base>::JackOnUnCouple( EndType end )
{
	switch( end )
	{
		case EndType::north:
			return m_JackOnUnCoupleNorth;
		case EndType::south:
			return m_JackOnUnCoupleSouth;
		case EndType::any:
			return m_JackOnUnCouple;
		default:
			throw std::invalid_argument( "TrainComponent_Imp::JackOnUnCouple: end is none." );
	}
}

template<class Base>
Jack& TrainComponent_Imp<Base>::JackOnCouplingActivated( EndType end )
{
	switch( end )
	{
		case EndType::north:
			return m_JackOnCouplingActivatedNorth;
		case EndType::south:
			return m_JackOnCouplingActivatedSouth;
		case EndType::any:
			return m_JackOnCouplingActivated;
		default:
			throw std::invalid_argument( "TrainComponent_Imp::JackOnCouplingActivated: end is none." );
	}
}

template<class Base>
inline Jack& TrainComponent_Imp<Base>::JackOnCouplingDeactivated( EndType end )
{
	switch( end )
	{
		case EndType::north:
			return m_JackOnCouplingDeactivatedNorth;
		case EndType::south:
			return m_JackOnCouplingDeactivatedSouth;
		case EndType::any:
			return m_JackOnCouplingDeactivated;
		default:
			throw std::invalid_argument( "TrainComponent_Imp::JackOnUnCouplingDeactivated: end is none." );
	}
}

template<class Base>
int TrainComponent_Imp<Base>::CountJacks() const noexcept{
	return 14;
}

template<class Base>
int TrainComponent_Imp<Base>::CountPlugs() const
{
	return m_PlugRail.CountPlugs() + m_PlugDerail.CountPlugs();
}

template<class Base>
const Jack& TrainComponent_Imp<Base>::_GetJack( int idx ) const
{
	switch( idx )
	{
	case 0:
		return m_JackOnRail;
	case 1:
		return m_JackOnDerail;
	case 2:
		return m_JackOnCoupleNorth;
	case 3:
		return m_JackOnCoupleSouth;
	case 4:
		return m_JackOnCouple;
	case 5:
		return m_JackOnUnCoupleNorth;
	case 6:
		return m_JackOnUnCoupleSouth;
	case 7:
		return m_JackOnUnCouple;
	case 8:
		return m_JackOnCouplingActivatedNorth;
	case 9:
		return m_JackOnCouplingActivatedSouth;
	case 10:
		return m_JackOnCouplingActivated;
	case 11:
		return m_JackOnCouplingDeactivatedNorth;
	case 12:
		return m_JackOnCouplingDeactivatedSouth;
	case 13:
		return m_JackOnCouplingDeactivated;
	}

	std::ostringstream stream;
	stream << "Out of range!" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::range_error( stream.str() );
}

template<class Base>
const Plug& TrainComponent_Imp<Base>::_GetPlug( int idx ) const{
	if( idx < m_PlugRail.CountPlugs() )
		return m_PlugRail.GetPlug( idx );
	idx -= m_PlugRail.CountPlugs();

	if( idx < m_PlugDerail.CountPlugs() )
		return m_PlugDerail.GetPlug( idx );
	idx -= m_PlugDerail.CountPlugs();

	assert( 0 );
	std::ostringstream stream;
	stream << __FILE__ << '(' << __LINE__ << ')' << "Out of range!";
	throw std::range_error( stream.str() );
}

template<class Base>
void TrainComponent_Imp<Base>::DisconnectJacks() noexcept
{
	m_PlugRail.Clear();
	m_PlugDerail.Clear();
	m_JackOnCoupleNorth.ClearPlugs();
	m_JackOnCoupleSouth.ClearPlugs();
	m_JackOnUnCoupleNorth.ClearPlugs();
	m_JackOnUnCoupleSouth.ClearPlugs();
	m_JackOnCouplingActivatedNorth.ClearPlugs();
	m_JackOnCouplingActivatedSouth.ClearPlugs();
	m_JackOnCouplingDeactivatedNorth.ClearPlugs();
	m_JackOnCouplingDeactivatedSouth.ClearPlugs();	
}

template<class Base>
void TrainComponent_Imp<Base>::ConnectJacks()
{
	std::pair<Bogie&,EndType> couplingTipNorth = this->GetTipAt( EndType::north );
	std::pair<Bogie&,EndType> couplingTipSouth = this->GetTipAt( EndType::south );

	couplingTipNorth.first.JackOnCouple( couplingTipNorth.second ).InsertAndAppend( &m_JackOnCoupleNorth.PlugToPulse().Make() );
	couplingTipSouth.first.JackOnCouple( couplingTipSouth.second ).InsertAndAppend( &m_JackOnCoupleSouth.PlugToPulse().Make() );
	couplingTipNorth.first.JackOnUnCouple( couplingTipNorth.second ).InsertAndAppend( &m_JackOnUnCoupleNorth.PlugToPulse().Make() );
	couplingTipSouth.first.JackOnUnCouple( couplingTipSouth.second ).InsertAndAppend( &m_JackOnUnCoupleSouth.PlugToPulse().Make() );
	couplingTipNorth.first.JackOnCouplingActivated( couplingTipNorth.second ).InsertAndAppend( &m_JackOnCouplingActivatedNorth.PlugToPulse().Make() );
	couplingTipSouth.first.JackOnCouplingActivated( couplingTipSouth.second ).InsertAndAppend( &m_JackOnCouplingActivatedSouth.PlugToPulse().Make() );
	couplingTipNorth.first.JackOnCouplingDeactivated( couplingTipNorth.second ).InsertAndAppend( &m_JackOnCouplingDeactivatedNorth.PlugToPulse().Make() );
	couplingTipSouth.first.JackOnCouplingDeactivated( couplingTipSouth.second ).InsertAndAppend( &m_JackOnCouplingDeactivatedSouth.PlugToPulse().Make() );
}

template<class Base>
void TrainComponent_Imp<Base>::ReconnectJacks()
{
	DisconnectJacks();
	ConnectJacks();
}

template<class Base>
void TrainComponent_Imp<Base>::OnRail() noexcept
{	
	if( this->IsRailed() )
	{
		m_bRailed = true;
		m_JackOnRail.Pulse();
	}
}

template<class Base>
void TrainComponent_Imp<Base>::OnDerail() noexcept
{
	if( m_bRailed )
	{
		DeleteReservation();

		m_bRailed = false;

		m_JackOnDerail.Pulse();
	}
}

template<class Base>
void TrainComponent_Imp<Base>::Clear() noexcept
{	
	DeleteReservation();

	m_bRailed = false;

	DisconnectJacks();
}

}

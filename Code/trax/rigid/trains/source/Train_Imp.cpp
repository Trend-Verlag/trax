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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#include "Train_Imp.h"
#include "../Bogie.h"

#include <iostream>

namespace trax{

	using namespace dim;
	using namespace spat;
	
std::shared_ptr<Train> Train::Make() noexcept
{
	try{
		std::shared_ptr<Train_Imp> pRetval = std::make_shared<Train_Imp>();
		
		if( pRetval )
			pRetval->SetWeakPointerToSelf( pRetval );
	
		return pRetval;
	}
	catch( const std::exception& e ){
		std::cerr << "Failed to create Train: " << e.what() << std::endl;
		return nullptr;
	}
}

Train_Imp::Train_Imp()
	: Train_Base{}
{
	for( Jack& jack : *static_cast<JackEnumerator*>(this) )
		jack.Reference( "parent", TypeName() );
}

Train_Imp::~Train_Imp()
{
	Clear();
}

const char* Train_Imp::TypeName() const noexcept{
	return "Train";
}

bool Train_Imp::IsValid() const noexcept
{
	if( m_Train.empty() )
		return false;

	for( auto iter = m_Train.begin(); iter != m_Train.end(); ++iter )
	{
		if( *iter == nullptr )
			return false;

		if( !(*iter)->IsValid() )
			return false;

		if( (*iter)->GetTrain() != this )
			return false;

		if( iter == m_Train.end() - 1 )
			break; // Last element has no next element.

		if( (*iter)->GetCoupledTrainComponent( Back( **iter ) ) != std::make_pair( *(iter + 1), Front( **(iter + 1) ) ) )
			return false;
	}

	return true;
}

void Train_Imp::Rail( const Location& location, bool bMoveTo )
{
	Rail( location, bMoveTo, TrainComponent::DistanceType::actual );
}

void Train_Imp::Rail( const Location& location, bool bMoveTo, DistanceType distance )
{
	std::vector<Location> railLocations;
	Location componentLocation = location;
	m_Train.front()->GetOrientation() ?
		MoveLocation( componentLocation, m_Train.front()->GetOverhang( EndType::north, distance ) ) :
		MoveLocation( componentLocation, m_Train.front()->GetOverhang( EndType::south, distance ) );

	for( auto& pTrainComponent : m_Train )
	{
		if( pTrainComponent->GetOrientation() ){
			MoveLocation( componentLocation, -pTrainComponent->GetOverhang( EndType::north, distance ) );

			railLocations.push_back( componentLocation );

			MoveLocation( componentLocation, 
				- pTrainComponent->GetOverhang( EndType::south, distance )
				- pTrainComponent->GetCouplingLength( EndType::south, distance ) );
		}
		else{
			MoveLocation( componentLocation, -pTrainComponent->GetOverhang( EndType::south, distance ) );

			railLocations.push_back( componentLocation );
			railLocations.back().Flip();

			MoveLocation( componentLocation, 
				- pTrainComponent->GetOverhang( EndType::north, distance )
				- pTrainComponent->GetCouplingLength( EndType::north, distance ) );
		}
	}

	for( std::size_t index = 0; index < m_Train.size(); ++index )
	{
		m_Train[index]->Rail( railLocations[index], bMoveTo, distance );
	}
}

Location Train_Imp::GetLocation() const noexcept{
	return m_Train.front()->GetOrientation() ? 
		m_Train.front()->GetLocation() : m_Train.front()->GetLocation().Flip();
}

void Train_Imp::DeRail() noexcept
{
	for( auto& pTrainComponent : m_Train )
		pTrainComponent->DeRail();
}

bool Train_Imp::IsRailed() const noexcept
{
	for( auto& pTrainComponent : m_Train )
		if( !pTrainComponent->IsRailed() )
			return false;

	return true;
}

spat::Frame<Length,One> Train_Imp::GetGlobalAnchor() const{
	return m_Train.front()->GetOrientation() ? 
		m_Train.front()->GetGlobalAnchor() : m_Train.front()->GetGlobalAnchor().FlipBin();
}

void Train_Imp::ResetToGlobalAnchor( const spat::Frame<Length,One>& anchor )
{
	ResetToGlobalAnchor( anchor, DistanceType::actual );
}

void Train_Imp::ResetToGlobalAnchor( 
	const spat::Frame<Length,One>& anchorPose, 
	DistanceType distance )
{
	assert( !IsRailed() );

	std::vector<Frame<Length,One>> locFrames;
	spat::Frame<Length,One> frame = anchorPose;
	m_Train.front()->GetOrientation() ?
		frame.TransportTan( m_Train.front()->GetOverhang( EndType::north, distance ) ) :
		frame.TransportTan( m_Train.front()->GetOverhang( EndType::south, distance ) );

	for( auto& pTrainComponent : m_Train )
	{
		if( pTrainComponent->GetOrientation() ){
			frame.TransportTan( -pTrainComponent->GetOverhang( EndType::north, distance ) );
			
			locFrames.push_back( frame );

			frame.TransportTan( 
				- pTrainComponent->GetOverhang( EndType::south, distance ) 
				- pTrainComponent->GetCouplingLength( EndType::south, distance ) );
		}
		else{
			frame.TransportTan( -pTrainComponent->GetOverhang( EndType::south, distance ) );

			locFrames.push_back( frame );			
			locFrames.back().FlipBin();

			frame.TransportTan( 
				- pTrainComponent->GetOverhang( EndType::north, distance ) 
				- pTrainComponent->GetCouplingLength( EndType::north, distance ) );
		}
	}

	for( std::size_t index = 0; index < m_Train.size(); ++index )
	{
		m_Train[index]->ResetToGlobalAnchor( locFrames[index] );
	}
}

void Train_Imp::SetVelocity( Velocity velocity )
{
	for( auto& pTrainComponent : m_Train )
		pTrainComponent->SetVelocity( pTrainComponent->GetOrientation() * velocity );
}

Velocity Train_Imp::GetVelocity() const
{
	Velocity velocity = 0_kmIh;

	for( auto& pTrainComponent : m_Train )
		velocity += pTrainComponent->GetOrientation() * pTrainComponent->GetVelocity();

	return velocity / m_Train.size();
}

void Train_Imp::TargetVelocity( Velocity tvelocity )
{
	for( auto& pTrainComponent : m_Train )
		pTrainComponent->TargetVelocity( pTrainComponent->GetOrientation() * tvelocity );
}

Velocity Train_Imp::TargetVelocity() const noexcept
{
	Velocity targetVelocity = 0_kmIh;

	for( auto& pTrainComponent : m_Train )
		targetVelocity += pTrainComponent->GetOrientation() * pTrainComponent->TargetVelocity();

	return targetVelocity / m_Train.size();
}

RailRunner::EndType Train_Imp::TargetDirection() const noexcept{
	return m_Train.front()->TargetDirection();
}

Power Train_Imp::MaximumPowerOutput() const noexcept
{
	Power maxPower = 0_kW;

	for( auto& pTrainComponent : m_Train )
		maxPower += pTrainComponent->MaximumPowerOutput();

	return maxPower;
}

Power Train_Imp::CurrentPowerOutput() const noexcept
{
	Power currentPower = 0_kW;

	for( auto& pTrainComponent : m_Train )
		currentPower += pTrainComponent->CurrentPowerOutput();

	return currentPower;
}

Velocity Train_Imp::MaximumVelocity() const noexcept
{
	Velocity maxVel = +infinite__velocity;

	for( auto& pTrainComponent : m_Train )
		maxVel = std::min( maxVel, pTrainComponent->MaximumVelocity() );

	return maxVel;
}

void Train_Imp::Thrust( One byfactor )
{
	for( auto& pTrainComponent : m_Train )
		pTrainComponent->Thrust( byfactor );
}

Force Train_Imp::ThrustAbsolute() const noexcept
{
	Force thrustAbsolute = 0_N;

	for( auto& pTrainComponent : m_Train )
		thrustAbsolute += pTrainComponent->ThrustAbsolute();

	return thrustAbsolute;
}

Force Train_Imp::MaxThrust() const noexcept
{
	Force maxThrust = 0_N;

	for( auto& pTrainComponent : m_Train )
		maxThrust += pTrainComponent->MaxThrust();

	return maxThrust;
}

Force Train_Imp::MaxThrust( Velocity velocity ) const noexcept
{
	Force maxThrust = 0_N;

	for( auto& pTrainComponent : m_Train )
		maxThrust += pTrainComponent->MaxThrust( velocity );

	return maxThrust;
}

void Train_Imp::Brake( One byfactor )
{
	for( auto& pTrainComponent : m_Train )
		pTrainComponent->Brake( byfactor );
}

Force Train_Imp::BrakeAbsolute() const noexcept
{
	Force brakeAbsolute = 0_N;

	for( auto& pTrainComponent : m_Train )
		brakeAbsolute += pTrainComponent->BrakeAbsolute();

	return brakeAbsolute;
}

Force Train_Imp::MaxBrake() const noexcept
{
	Force maxBrake = 0_N;

	for( auto& pTrainComponent : m_Train )
		maxBrake += pTrainComponent->MaxBrake();

	return maxBrake;
}

Length Train_Imp::GetOverhang( EndType end, DistanceType distance ) const noexcept
{
	switch( end ){
	case EndType::north:
		return m_Train.front()->GetOrientation() ?
			m_Train.front()->GetOverhang( EndType::north, distance ) :
			m_Train.front()->GetOverhang( EndType::south, distance );
	case EndType::south:
		return m_Train.front()->GetOrientation() ?
			m_Train.front()->GetOverhang( EndType::south, distance ) + GetLength( distance ) - m_Train.front()->GetLength( distance ) :
			m_Train.front()->GetOverhang( EndType::north, distance ) + GetLength( distance ) - m_Train.front()->GetLength( distance );
	case EndType::any:
		return GetOverhang( EndType::north, distance );
	default:
		return 0_m;
	}
}

Length Train_Imp::GetLength( DistanceType distance ) const noexcept{
	Length length = 0_m;

	for( auto& pTrainComponent : m_Train )
		length += pTrainComponent->GetLength( distance ) + 
				 (pTrainComponent != m_Train.back() ? pTrainComponent->GetCouplingLength( pTrainComponent->GetOrientation() ? EndType::south : EndType::north, distance ) : 0_m);

	return length;
}

std::pair<Bogie&,RailRunner::EndType> Train_Imp::GetTipAt( EndType end )
{
	if( m_Train.empty() )
		throw std::runtime_error( "Train_Imp::GetTipAt: empty train" );

	switch( end ){
		case EndType::north:
			return m_Train.front()->GetTipAt( m_Train.front()->GetOrientation() ? EndType::north : EndType::south );
		case EndType::south:
			return m_Train.back()->GetTipAt( m_Train.back()->GetOrientation() ? EndType::south : EndType::north );
		case EndType::any:
			return GetTipAt(EndType::north );
	}

	throw std::invalid_argument( "Train_Imp::GetTipAt: invalid end type" );
}

std::pair<const Bogie&,RailRunner::EndType> Train_Imp::GetTipAt( EndType end ) const
{
	if( m_Train.empty() )
		throw std::runtime_error( "Train_Imp::GetTipAt: empty train" );

	switch( end ){
		case EndType::north:
			return m_Train.front()->GetTipAt( m_Train.front()->GetOrientation() ? EndType::north : EndType::south );
		case EndType::south:
			return m_Train.back()->GetTipAt( m_Train.back()->GetOrientation() ? EndType::south : EndType::north );
		case EndType::any:
			return GetTipAt(EndType::north );
	}

	throw std::invalid_argument( "Train_Imp::GetTipAt: invalid end type" );
}

Mass Train_Imp::GetTotalMass() const noexcept
{
	Mass totalMass = 0_kg;

	for( auto& pTrainComponent : m_Train )
		totalMass += pTrainComponent->GetTotalMass();

	return totalMass;
}

std::shared_ptr<Train> Train_Imp::ThisTrain() const noexcept
{
	return std::dynamic_pointer_cast<Train>(This());
}

int Train_Imp::GetNumberOfComponents() const noexcept{
	return static_cast<int>(m_Train.size());
}

std::shared_ptr<TrainComponent> Train_Imp::GetComponent( int index ) const{
	return m_Train.at(index);
}

int Train_Imp::GetIndexOf( const TrainComponent& component ) const noexcept
{
	for( std::size_t idx = 0; idx < m_Train.size(); ++idx ){
		if( m_Train[idx].get() == &component )
			return static_cast<int>(idx);
	}

	return -1;
}

void Train_Imp::Reduce() noexcept
{
	for( auto iter = m_Train.begin(); iter != m_Train.end();  ){
		if( std::shared_ptr<Train_Imp> pTrain = std::dynamic_pointer_cast<Train_Imp>(*iter) )
		{
			pTrain->Reduce();

			for( std::size_t idx = 0; idx < pTrain->m_Train.size(); ++idx ){
				std::shared_ptr<TrainComponent> pTrainComponent = pTrain->m_Train.at(idx);
				pTrainComponent->SetTrain( this );
				pTrainComponent->SetOrientation( pTrain->GetOrientation() == pTrainComponent->GetOrientation() ? Orientation::Value::para : Orientation::Value::anti );
				pTrainComponent->JackOnUnCouple( Front( *pTrainComponent ) ).InsertAtTail( &m_JackOnSeparation.PlugToPulse().Make() );

				iter = m_Train.insert( iter, pTrainComponent );
				++iter;
			}
			
			pTrain->m_JackOnSeparation.ClearPlugs();
			pTrain->m_Train.clear();
			pTrain->SetTrain( nullptr );

			iter = m_Train.erase( iter );
		}
		else
			++iter;
	}
}

bool Train_Imp::Create( TrainComponent& trainComponent, Orientation orientation )
{
	Clear();

	std::pair<std::shared_ptr<TrainComponent>,EndType> trainTip = 
		GetTipAt( trainComponent, orientation ? EndType::north : EndType::south );

	// Build train from tip to end:
	do{
		Append( EndType::south, trainTip.first, trainTip.second );
		trainTip = trainTip.first->GetCoupledTrainComponent( !trainTip.second );
	}
	while( trainTip.first );

	return true;
}

bool Train_Imp::Create( 
	const std::vector<std::pair<std::shared_ptr<TrainComponent>,Orientation>>& trainComponents, 
	Orientation orientation, 
	bool bMoveTo,
	DistanceType distance )
{
	Clear();

	if( trainComponents.empty() )
		return false; // Nothing to create.

	for( auto& pair : trainComponents )
	{
		Append( orientation ? EndType::south : EndType::north, 
				pair.first, 
				orientation ? pair.second : !pair.second );
	}

	if( m_Train.empty() )
		return false; // Nothing to create.

	if( bMoveTo )
	{
		if( m_Train.front()->IsRailed() )
		{
			Location location = m_Train.front()->GetLocation();
			if( m_Train.front()->GetOrientation() == Orientation::Value::anti )
				location.Flip();

			Rail( location, true, distance );
		}
		else
		{
			Frame<Length,One> frame = m_Train.front()->GetGlobalAnchor();
			if( m_Train.front()->GetOrientation() == Orientation::Value::anti )
				frame.FlipBin();

			ResetToGlobalAnchor( frame, distance );
		}

		Recouple();
	}

	return true;
}

void Train_Imp::Align( EndType atEnd, TrainComponent& component, EndType withEnd ) const noexcept
{
	if( m_Train.empty() )
		return; // Nothing to align with.
	if( m_Train.size() == 1 && m_Train.front().get() == &component )
		return; // Component is first element: simplify iterations over components.

	Length couplingLength = GetTipAt( atEnd ).first.GetCouplingProps( atEnd ).Position.Radius() + 
		component.GetTipAt( withEnd ).first.GetCouplingProps( withEnd ).Position.Radius();

	if( IsRailed() ) {
		Location location = GetLocation();
		if( atEnd == withEnd ){
			atEnd == EndType::north ? location.Move( GetOverhang( EndType::north ) + component.GetOverhang( EndType::north ) + couplingLength ) : 
				location.Move( -GetOverhang( EndType::south ) - component.GetOverhang( EndType::south ) - couplingLength );
			location.Flip();
		}
		else{
			atEnd == EndType::north ? location.Move( GetOverhang( EndType::north ) + component.GetOverhang( EndType::south ) + couplingLength ) : 
				location.Move( -GetOverhang( EndType::south ) - component.GetOverhang( EndType::north ) - couplingLength );
		}

		component.Rail( location );
	}
	else{
		spat::Frame<Length,One> globalAnchor = GetGlobalAnchor();
		if( atEnd == withEnd ){
			atEnd == EndType::north ? globalAnchor.TransportTan( GetOverhang( EndType::north ) + component.GetOverhang( EndType::north ) + couplingLength ) : 
				globalAnchor.TransportTan( -GetOverhang( EndType::south ) - component.GetOverhang( EndType::south ) - couplingLength );
			globalAnchor.FlipBin();
		}
		else{
			atEnd == EndType::north ? globalAnchor.TransportTan( GetOverhang( EndType::north ) + component.GetOverhang( EndType::south ) + couplingLength ) : 
				globalAnchor.TransportTan( -GetOverhang( EndType::south ) - component.GetOverhang( EndType::north ) - couplingLength );
		}

		component.ResetToGlobalAnchor( globalAnchor );
	}
}

void Train_Imp::Align( EndType atEnd, TrainComponent& component, Orientation orientation ) const noexcept
{
	Align( atEnd, component, orientation ? trax::RailRunner::EndType::north : trax::RailRunner::EndType::south );
}

void Train_Imp::Append( EndType atEnd, std::shared_ptr<TrainComponent> pComponent, EndType withEnd )
{
	if( !pComponent )
		return; // Nothing to append.
	if( !pComponent->IsValid() )
		throw std::invalid_argument( "Train_Imp: component invalid" );
	if( pComponent->GetTrain() )
		throw std::invalid_argument( "Train_Imp::Append: component already in train" );

	pComponent->SetTrain( this );
	pComponent->SetOrientation( withEnd == EndType::north ? Orientation::Value::para : Orientation::Value::anti );

	if( !m_Train.empty() ){
		std::pair<Bogie&,RailRunner::EndType> tip = GetTipAt( atEnd );
		tip.first.JackOnUnCouple( tip.second ).InsertAtTail( &m_JackOnSeparation.PlugToPulse().Make() );
		if( !trax::Couple( tip, pComponent->GetTipAt( withEnd ) ) )
			throw std::runtime_error( "Train_Imp::Append: could not couple" );
	}

	if( atEnd == EndType::north )
		m_Train.push_front( pComponent );
	else
		m_Train.push_back( pComponent );

	ConnectJacks();

	pComponent->JackOnRail().InsertAtTail( &m_PlugRail.Make() );
	pComponent->JackOnDerail().InsertAtTail( &m_PlugDerail.Make() );

	if( std::shared_ptr<Train> pTrain = std::dynamic_pointer_cast<Train>(pComponent) )
	{
		pTrain->JackOnSeparation().InsertAtTail( &m_JackOnSeparation.PlugToPulse().Make() );
	}
}

void Train_Imp::Append( EndType atEnd, std::shared_ptr<TrainComponent> pComponent, Orientation orientation )
{
	Append( atEnd, pComponent, orientation ? trax::RailRunner::EndType::north : trax::RailRunner::EndType::south );
}

void Train_Imp::Take( EndType atEnd, Train& from, EndType withEnd )
{
	Train_Imp& from_imp = dynamic_cast<Train_Imp&>(from);

	if( withEnd == EndType::north )
	{
		for( auto iter = from_imp.m_Train.begin(); iter != from_imp.m_Train.end(); ++iter )
		{
			(*iter)->SetTrain( nullptr );
			Append( atEnd, *iter, (*iter)->GetOrientation() );
		}
	}
	else if( withEnd == EndType::south )
	{
		for( auto iter = from_imp.m_Train.rbegin(); iter != from_imp.m_Train.rend(); ++iter )
		{
			(*iter)->SetTrain( nullptr );
			Append( atEnd, *iter, !(*iter)->GetOrientation() );
		}
	}
	else
		throw std::invalid_argument( "Train_Imp::Take: invalid end type" );

	from_imp.m_JackOnSeparation.ClearPlugs();
	from_imp.m_Train.clear();
}

bool Train_Imp::IsUnCoupledInternally() const noexcept
{
	if( m_Train.size() < 2 )
		return false;

	for( auto iter = m_Train.begin(); iter != m_Train.end()-1; ++iter )
	{
		if( std::shared_ptr<Train> pTrain = std::dynamic_pointer_cast<Train>(*iter) )
		{
			if( pTrain->IsUnCoupledInternally() )
				return true;
		}

		if( !(*iter)->IsCoupled( Back( GetOrientation() ) ) )
			return true;
	}

	return false;
}

bool Train_Imp::Couple( EndType end, Train& with, EndType withEnd ) noexcept{
	std::pair<Bogie&,RailRunner::EndType> coupling = GetTipAt( end );
	std::pair<Bogie&,RailRunner::EndType> withcoupling = with.GetTipAt( withEnd );
	return coupling.first.Couple( coupling.second, withcoupling.first, withcoupling.second );
}

std::pair<std::shared_ptr<struct Train>,std::shared_ptr<struct Train>> 
Train_Imp::Split( int at ) noexcept
{
	std::size_t _at = static_cast<std::size_t>(at);
	if( _at >= 0 && 
		_at < m_Train.size()-1 &&
		m_Train.size() >= 2 )
	{
		m_JackOnSeparation.ClearPlugs();

		std::shared_ptr<Train> pTrainA = std::make_shared<Train_Imp>();
		std::shared_ptr<Train> pTrainB = std::make_shared<Train_Imp>();

		for( std::size_t idx = 0; idx < m_Train.size(); ++idx )
		{
			m_Train[idx]->SetTrain( nullptr );

			if( idx <= _at )
			{
				pTrainA->Append( EndType::south, m_Train[idx], m_Train[idx]->GetOrientation() );
			}
			else
			{
				pTrainB->Append( EndType::south, m_Train[idx], m_Train[idx]->GetOrientation() );
			}
		}

		m_Train.clear();

		Append( EndType::south, pTrainA, Orientation::Value::para );
		Append( EndType::south, pTrainB, Orientation::Value::para );

		return std::make_pair( pTrainA, pTrainB );
	}

	return std::make_pair( nullptr, nullptr );
}

std::shared_ptr<Train> Train_Imp::Separate()
{
	for( auto iter = m_Train.begin(); iter != m_Train.end(); ++iter )
	{
		std::shared_ptr<Train> pNewTrain;

		if( std::shared_ptr<Train> pChildTrain = std::dynamic_pointer_cast<Train>(*iter); 
			pChildTrain && pChildTrain->IsUnCoupledInternally() )
		{
			pNewTrain = pChildTrain->Separate();
		}
		else if( *iter != m_Train.back() && !(*iter)->IsCoupled( (*iter)->GetOrientation() ? EndType::south : EndType::north ) )
		{
			pNewTrain = Train::Make();
		}

		if( pNewTrain )
		{
			m_JackOnSeparation.ClearPlugs();

			++iter;
			while( iter != m_Train.end() )
			{
				(*iter)->SetTrain( nullptr );
				pNewTrain->Append( EndType::south, *iter, (*iter)->GetOrientation() );
				iter = m_Train.erase( iter );
			}

			// Add the plugs for the remainig components again:
			for( auto& pTrainComponent : m_Train )
			{
				pTrainComponent->JackOnUnCouple( Back( *pTrainComponent ) ).InsertAtTail( 
						&m_JackOnSeparation.PlugToPulse().Make() );
			}

			ConnectJacks();
			return pNewTrain;
		}
	}

	return nullptr;
}

void Train_Imp::Clear() noexcept
{
	m_JackOnSeparation.ClearPlugs();

	for( auto& pTrainComponent : m_Train ){
		pTrainComponent->SetTrain( nullptr );
		pTrainComponent->Uncouple();
	}

	m_Train.clear();
}

Jack& Train_Imp::JackOnSeparation() noexcept{
	return m_JackOnSeparation;
}

int Train_Imp::CountJacks() const noexcept{
	return Train_Base::CountJacks() + 1;
}

const Jack& Train_Imp::_GetJack( int idx ) const
{
	if( idx < Train_Base::CountJacks() )
		return Train_Base::_GetJack( idx );

	idx -= Train_Base::CountJacks();

	switch( idx )
	{
	case 0:
		return m_JackOnSeparation;
	}

	std::ostringstream stream;
	stream << "Out of range!" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::range_error( stream.str() );
}

std::pair<std::shared_ptr<TrainComponent>,RailRunner::EndType> Train_Imp::GetTipAt( 
	const TrainComponent& trainComponent, 
	EndType end ) const noexcept
{
	// Search for train tip:
	std::pair<std::shared_ptr<TrainComponent>,EndType> trainTip = 
		std::make_pair( trainComponent.ThisTrainComponent(), end );

	while( trainTip.first->GetCoupledTrainComponent( trainTip.second ).first ){
		trainTip = trainTip.first->GetCoupledTrainComponent( trainTip.second );
		trainTip.second = !trainTip.second;
	}

	return trainTip;
}

void Train_Imp::Recouple() noexcept
{
	for( std::size_t index = 1; index < m_Train.size(); ++index )
	{
		TrainComponent::Coupling coupling{
			m_Train[index-1],
			m_Train[index-1]->GetOrientation() ? EndType::south : EndType::north,
			m_Train[index],
			m_Train[index]->GetOrientation() ? EndType::north : EndType::south
		};

		coupling.pTrainComponentA->Uncouple( coupling.endA );
		trax::Couple( coupling );
	}
}

} // namespace trax
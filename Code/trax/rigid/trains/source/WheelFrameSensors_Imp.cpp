//	trax track library
//	AD 2025 
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

#include "WheelFrameSensors_Imp.h"

#include "trax/rigid/trains/RollingStock.h"
#include "trax/rigid/trains/Train.h"
#include "trax/rigid/trains/WheelFrame.h"


namespace trax
{

std::unique_ptr<VelocitySensor> VelocitySensor::Make() noexcept{
	try{
		return std::make_unique<VelocitySensor_Imp>();
	}
	catch( ... ){
		return nullptr;
	}
}

VelocitySensor_Imp::VelocitySensor_Imp()
	: Sensor_Imp		{}
	, m_bTriggerInside{ true }
{
	m_Velocity.Init( 0_mIs, Velocity{+infinite} );
}

const char* VelocitySensor_Imp::TypeName() const noexcept{
	return "VelocitySensor";
}

void VelocitySensor_Imp::Trigger( const Event& _event ){
	if( const WheelFrame::Event* pEvent = dynamic_cast<const WheelFrame::Event*>(&_event) )
	{
		if( m_Velocity.Includes( abs(pEvent->GetWheelFrame().GetVelocity()) ) ){
			if( m_bTriggerInside )
				Sensor_Imp::Trigger( _event );
		}
		else{
			if( !m_bTriggerInside )
				Sensor_Imp::Trigger( _event );
		}
	}
}

void VelocitySensor_Imp::SetVelocity( Velocity velocityMin, Velocity velocityMax ) noexcept{
	m_Velocity.Near( velocityMin );
	m_Velocity.Far( velocityMax );
}

void VelocitySensor_Imp::SetVelocity( const common::Interval<Velocity>& velocity ) noexcept{
	m_Velocity = velocity;
}

const common::Interval<Velocity>& VelocitySensor_Imp::GetVelocity() const noexcept{
	return m_Velocity;
}

void VelocitySensor_Imp::VelocityMin( Velocity velocityMin ) noexcept{
	m_Velocity.Near( velocityMin );
}

Velocity VelocitySensor_Imp::VelocityMin() const noexcept{
	return m_Velocity.Near();
}

void VelocitySensor_Imp::VelocityMax( Velocity velocityMax ) noexcept{
	m_Velocity.Far( velocityMax );
}

Velocity VelocitySensor_Imp::VelocityMax() const noexcept{
	return m_Velocity.Far();
}

void VelocitySensor_Imp::TriggerInside( bool bTriggerInside ) noexcept{
	m_bTriggerInside = bTriggerInside;
}

bool VelocitySensor_Imp::TriggerInside() const noexcept{
	return m_bTriggerInside;
}
///////////////////////////////////////
std::unique_ptr<WeighSensor> WeighSensor::Make() noexcept{
	try{
		return std::make_unique<WeighSensor_Imp>();
	}
	catch( ... ){
		return nullptr;
	}
}

WeighSensor_Imp::WeighSensor_Imp()
	: Sensor_Imp		{}
	, m_bTriggerInside	{ true }
	, m_bTrain			{ true }
{
	m_Weight.Init( 0_kg, Mass{+infinite} );
}

const char* WeighSensor_Imp::TypeName() const noexcept{
	return "WeighSensor";
}

void WeighSensor_Imp::Trigger( const Event& _event ){
	if( const WheelFrame::Event* pEvent = dynamic_cast<const WheelFrame::Event*>(&_event) )
	{
		if( m_Weight.Includes( GetMassFrom( pEvent->GetWheelFrame() ) ) ){
			if( m_bTriggerInside )
				Sensor_Imp::Trigger( _event );
		}
		else{
			if( !m_bTriggerInside )
				Sensor_Imp::Trigger( _event );
		}
	}
}

void WeighSensor_Imp::Weight( Mass weightMin, Mass weightMax ) noexcept{
	m_Weight.Near( weightMin );
	m_Weight.Far( weightMax );
}

void WeighSensor_Imp::Weight( const common::Interval<Mass>& weight ) noexcept{
	m_Weight = weight;
}

const common::Interval<Mass>& WeighSensor_Imp::Weight() const noexcept{
	return m_Weight;
}

void WeighSensor_Imp::WeightMin( Mass weightMin ) noexcept{
	m_Weight.Near( weightMin );
}

Mass WeighSensor_Imp::WeightMin() const noexcept{
	return m_Weight.Near();
}

void WeighSensor_Imp::WeightMax( Mass weightMax ) noexcept{
	m_Weight.Far( weightMax );
}

Mass WeighSensor_Imp::WeightMax() const noexcept{
	return m_Weight.Far();
}

void WeighSensor_Imp::TriggerInside( bool bTriggerInside ) noexcept{
	m_bTriggerInside = bTriggerInside;
}

bool WeighSensor_Imp::TriggerInside() const noexcept{
	return m_bTriggerInside;
}

void WeighSensor_Imp::WeighTrain( bool bTrain ) noexcept{
	m_bTrain = bTrain;
}

bool WeighSensor_Imp::WeighTrain() const noexcept{
	return m_bTrain;
}

Mass WeighSensor_Imp::GetMassFrom( const WheelFrame& wheelFrame ) const noexcept
{
	if( wheelFrame.GetRollingStock() )
	{
		if( !m_bTrain )
		{
			return wheelFrame.GetRollingStock()->TotalMass();
		}
		else if( wheelFrame.GetRollingStock()->GetTrain() )
		{
			return wheelFrame.GetRollingStock()->GetTrain()->TotalMass();
		}
	}

	return 0_kg;
}
///////////////////////////////////////
std::unique_ptr<IntervalSensor> IntervalSensor::Make() noexcept{
	try{
		return std::make_unique<IntervalSensor_Imp>();
	}
	catch( ... ){
		return nullptr;
	}
}

IntervalSensor_Imp::IntervalSensor_Imp()
	: Sensor_Imp		{}
	, m_bTriggerInside	{ true }
{
	m_Value.Init( 0, +infinite );
}

void IntervalSensor_Imp::Value( Real valueMin, Real valueMax ) noexcept{
	m_Value.Near( valueMin );
	m_Value.Far( valueMax );
}

void IntervalSensor_Imp::Value( const common::Interval<Real>& interval ) noexcept{
	m_Value = interval;
}

const common::Interval<Real>& IntervalSensor_Imp::Value() const noexcept{
	return m_Value;
}

void IntervalSensor_Imp::ValueMin( Real valueMin ) noexcept{
	m_Value.Near( valueMin );
}

Real IntervalSensor_Imp::ValueMin() const noexcept{
	return m_Value.Near();
}

void IntervalSensor_Imp::ValueMax(Real valueMax) noexcept{
	m_Value.Far( valueMax );
}

Real IntervalSensor_Imp::ValueMax() const noexcept{
	return m_Value.Far();
}

void IntervalSensor_Imp::TriggerInside( bool bTriggerInside ) noexcept{
	m_bTriggerInside = bTriggerInside;
}

bool IntervalSensor_Imp::TriggerInside() const noexcept{
	return m_bTriggerInside;
}

bool IntervalSensor_Imp::ShallTrigger( Real value ) const noexcept{
	if( m_Value.Includes( value ) ){
		if( m_bTriggerInside )
			return true;
	}
	else{
		if( !m_bTriggerInside )
			return true;
	}
	
	return false;
}
///////////////////////////////////////
TractionSensor_Imp::TractionSensor_Imp()
	: IntervalSensor_Imp{}
{
}

const char* TractionSensor_Imp::TypeName() const noexcept{
	return "TractionSensor";
}

void TractionSensor_Imp::Trigger( const Event& _event ){
	if( const EventFilter* pEvent = dynamic_cast<const EventFilter*>(&_event) )
	{
		//if( auto pWheelFrame = pEvent->GetWheelFrame() )
		//	if( ShallTrigger( pWheelFrame->GetTrain()->MaxThrust( 0_mIs ).Units() ) )
		//		IntervalSensor_Imp::Trigger( _event );
	}
}
///////////////////////////////////////
}
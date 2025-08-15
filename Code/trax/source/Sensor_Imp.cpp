// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel



#include "Sensor_Imp.h"
#include "Plug_Imp.h"
#include "trax/Event.h"

#include <iostream>
#include <sstream>

namespace trax{

using namespace common;

//
//Sensor_Imp::Sensor_Imp()
//	:	m_pTrackBuilder	{ nullptr }
//{
//}
//
//Sensor_Imp::Sensor_Imp( const Sensor_Imp& sensor )
//	:	m_pTrackBuilder	{ sensor.m_pTrackBuilder }
//{
//}
////
////Sensor_Imp& Sensor_Imp::operator=( const Sensor_Imp& sensor ){
////	m_pTrackBuilder = sensor.m_pTrackBuilder;
////	return *this;
////}
//
//std::string	Sensor_Imp::TypeName() const noexcept{
//	return "Sensor";
//}
//
//void Sensor_Imp::Trigger( const Event& /*_event*/ ){
//	m_JackOnTrigger.Pulse();
//}
//
//Jack& Sensor_Imp::JackOnTrigger() noexcept{
//	return m_JackOnTrigger;
//}
//
//void Sensor_Imp::RegisterSockets( SocketRegistry& module ){
//	module.ConnectJack( m_JackOnTrigger );
//}
//
//void Sensor_Imp::UnregisterSockets( SocketRegistry& module ){
//	module.RemoveJack( m_JackOnTrigger );
//}
//
//TrackBuilder* Sensor_Imp::TrackAttached() const noexcept{
//	return m_pTrackBuilder;
//}
//
//int Sensor_Imp::CountJacks() const noexcept{
//	return 1;
//}
//
//void Sensor_Imp::Attach( TrackBuilder* pTrackBuilder ) noexcept{
//	m_pTrackBuilder = pTrackBuilder;
//}
//
//const Jack & Sensor_Imp::_GetJack( int idx ) const{
//	if( idx == 0 )
//		return m_JackOnTrigger;
//
//	std::ostringstream stream;
//	stream << "Out of range!" << std::endl;
//	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
//	throw std::range_error( stream.str() );
//}
//*/
///////////////////////////////////////
//std::unique_ptr<Sensor> Sensor::Make() noexcept{
//	return std::make_unique<Sensor_Imp>();
//}
///////////////////////////////////////
Sensor_Imp::Sensor_Imp( int flags )
	:	m_pTrackBuilder	{ nullptr },
		m_FilterFlags	{ flags }
{
}

Sensor_Imp::Sensor_Imp( const Sensor_Imp& sensor )
	:	m_pTrackBuilder	{ sensor.m_pTrackBuilder }
{
}
//
//Sensor_Imp& Sensor_Imp::operator=( const Sensor_Imp& sensor ){
//	m_pTrackBuilder = sensor.m_pTrackBuilder;
//	return *this;
//}

const char*	Sensor_Imp::TypeName() const noexcept{
	return "Sensor";
}

void Sensor_Imp::Trigger( const Event& _event ){
	if( m_FilterFlags == any ){
		m_JackOnTrigger.Pulse();
		return;
	}

	if( const EventFilter* pEvent = dynamic_cast<const EventFilter*>(&_event) )
	{
		if( m_FilterFlags & name ){
			if( pEvent->Name().find( Reference( "name" ) ) == std::string::npos )
				return;

			std::clog << "Event: " << pEvent->Name() << " passed name filter: " << Reference( "name" ) << std::endl;
		}

		if( m_FilterFlags & not_name ){
			if( pEvent->Name().find( Reference( "name" ) ) != std::string::npos )
				return;

			std::clog << "Event: " << pEvent->Name() << " passed not_name filter: " << Reference( "name" ) << std::endl;
		}

		switch( pEvent->GetType() ){
		case EventFilter::Type::wheelframe:
			if( m_FilterFlags & wheelframe )
				m_JackOnTrigger.Pulse();
			break;
		case EventFilter::Type::traintip_north:
			if( m_FilterFlags & train_north							||
				m_FilterFlags & wheelframe							||
				(pEvent->TrainTip() && m_FilterFlags & train_tip)	||
				(pEvent->TrainEnd() && m_FilterFlags & train_end) )
				m_JackOnTrigger.Pulse();
			break;
		case EventFilter::Type::traintip_south:
			if( m_FilterFlags & train_south							||
				m_FilterFlags & wheelframe							||
				(pEvent->TrainTip() && m_FilterFlags & train_tip)	||
				(pEvent->TrainEnd() && m_FilterFlags & train_end) )
				m_JackOnTrigger.Pulse();
			break;
		case EventFilter::Type::traintip_both:
			if( m_FilterFlags & train_north					||
				m_FilterFlags & train_south					||
				m_FilterFlags & wheelframe					||
				m_FilterFlags & train_tip					||
				m_FilterFlags & train_end )
				m_JackOnTrigger.Pulse();
			break;
		case EventFilter::Type::any:
			m_JackOnTrigger.Pulse();
			break;
		default:
		{
			std::ostringstream stream;
			stream << " Unknown enumeration value! ("<< static_cast<int>(pEvent->GetType()) << ')' << std::endl;
			stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
			throw std::invalid_argument( stream.str() );
		}
		}
	}
}

int Sensor_Imp::GetEffect() const noexcept{
	return m_FilterFlags;
}

void Sensor_Imp::SetEffect( int effect ) noexcept{
	m_FilterFlags = effect;
}

Jack& Sensor_Imp::JackOnTrigger() noexcept{
	return m_JackOnTrigger;
}

void Sensor_Imp::RegisterSockets( SocketRegistry& module ){
	module.ConnectJack( m_JackOnTrigger );
}

void Sensor_Imp::UnregisterSockets( SocketRegistry& module ){
	module.RemoveJack( m_JackOnTrigger );
}

TrackBuilder* Sensor_Imp::TrackAttached() const noexcept{
	return m_pTrackBuilder;
}

int Sensor_Imp::CountJacks() const noexcept{
	return 1;
}

void Sensor_Imp::Attach( TrackBuilder* pTrackBuilder ) noexcept{
	m_pTrackBuilder = pTrackBuilder;
}

const Jack& Sensor_Imp::_GetJack( int idx ) const{
	if( idx == 0 )
		return m_JackOnTrigger;

	std::ostringstream stream;
	stream << "Out of range!" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::range_error( stream.str() );
}
///////////////////////////////////////
std::string ToString( SensorFilterJack::Effect effect ){
	switch( effect ){
	case SensorFilterJack::none:
		return "none";
	case SensorFilterJack::train_tip:
		return "train_tip";
	case SensorFilterJack::train_end:
		return "train_end";
	case SensorFilterJack::train_north:
		return "train_north";
	case SensorFilterJack::train_south:
		return "train_south";
	case SensorFilterJack::wheelframe:
		return "wheelframe";
	case SensorFilterJack::name:
		return "name";
	case SensorFilterJack::not_name:
		return "not_name";
	case SensorFilterJack::any:
		return "any";
	default:
		assert( !"Unknown SensorFilterJack::Effect enumerator!" );
		return "unknown";
	}
}

SensorFilterJack::Effect SensorEffectFromString( const std::string& effect ){
	if( effect == "none" )
		return SensorFilterJack::none;
	else if( effect == "train_tip" )
		return SensorFilterJack::train_tip;
	else if( effect == "train_end" )
		return SensorFilterJack::train_end;
	else if( effect == "train_north" )
		return SensorFilterJack::train_north;
	else if( effect == "train_south" )
		return SensorFilterJack::train_south;
	else if( effect == "wheelframe" )
		return SensorFilterJack::wheelframe;
	else if( effect == "name" )
		return SensorFilterJack::name;
	else if( effect == "not_name" )
		return SensorFilterJack::not_name;
	else if( effect == "any" )
		return SensorFilterJack::any;

	std::ostringstream stream;
	stream << "Invalid SensorFilterJack::Effect string: " << effect << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::invalid_argument( stream.str() );
}
///////////////////////////////////////
std::unique_ptr<SensorFilterJack> SensorFilterJack::Make( int flags ) noexcept{
	return std::make_unique<Sensor_Imp>( flags );
}
///////////////////////////////////////
std::unique_ptr<VelocitySensor> VelocitySensor::Make( int flags ) noexcept{
	try{
		return std::make_unique<VelocitySensor_Imp>( flags );
	}
	catch( ... ){
		return nullptr;
	}
}

VelocitySensor_Imp::VelocitySensor_Imp( int flags )
	:	Sensor_Imp		{ flags },
		m_bTriggerInside{ true }
{
	m_Velocity.Init( 0_mIs, Velocity{+infinite} );
}

const char* VelocitySensor_Imp::TypeName() const noexcept{
	return "VelocitySensor";
}

void VelocitySensor_Imp::Trigger( const Event& _event ){
	if( const EventFilter* pEvent = dynamic_cast<const EventFilter*>(&_event) )
	{
		//if( auto pWheelFrame = pEvent->GetWheelFrame() ){
		//	if( m_Velocity.Includes( abs(pWheelFrame->GetVelocity()) ) ){
		//		if( m_bTriggerInside )
		//			Sensor_Imp::Trigger( _event );
		//	}
		//	else{
		//		if( !m_bTriggerInside )
		//			Sensor_Imp::Trigger( _event );
		//	}
		//}
	}
}

void VelocitySensor_Imp::SetVelocity( Velocity velocityMin, Velocity velocityMax ) noexcept{
	m_Velocity.Near( velocityMin );
	m_Velocity.Far( velocityMax );
}

void VelocitySensor_Imp::SetVelocity( const Interval<Velocity>& velocity ) noexcept{
	m_Velocity = velocity;
}

const Interval<Velocity>& VelocitySensor_Imp::GetVelocity() const noexcept{
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
std::unique_ptr<WeighSensor> WeighSensor::Make( int flags ) noexcept{
	try{
		return std::make_unique<WeighSensor_Imp>( flags );
	}
	catch( ... ){
		return nullptr;
	}
}

WeighSensor_Imp::WeighSensor_Imp(int flags)
	:	Sensor_Imp		{ flags },
		m_bTriggerInside{ true },
		m_bTrain		{ true }
{
	m_Weight.Init( 0_kg, Mass{+infinite} );
}

const char* WeighSensor_Imp::TypeName() const noexcept{
	return "WeighSensor";
}

void WeighSensor_Imp::Trigger( const Event& _event ){
	if( const EventFilter* pEvent = dynamic_cast<const EventFilter*>(&_event) )
	{
		//if( auto pWheelFrame = pEvent->GetWheelFrame() ){
		//	if( m_Weight.Includes( m_bTrain ? pWheelFrame->GetTrain()->TotalMass() : GetTopmostParent( *pWheelFrame )->TotalMass() ) ){
		//		if( m_bTriggerInside )
		//			Sensor_Imp::Trigger( _event );
		//	}
		//	else{
		//		if( !m_bTriggerInside )
		//			Sensor_Imp::Trigger( _event );
		//	}
		//}
	}
}

void WeighSensor_Imp::Weight( Mass weightMin, Mass weightMax ) noexcept{
	m_Weight.Near( weightMin );
	m_Weight.Far( weightMax );
}

void WeighSensor_Imp::Weight( const Interval<Mass>& weight ) noexcept{
	m_Weight = weight;
}

const Interval<Mass>& WeighSensor_Imp::Weight() const noexcept{
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
///////////////////////////////////////
std::unique_ptr<IntervalSensor> IntervalSensor::Make( int flags ) noexcept{
	try{
		return std::make_unique<IntervalSensor_Imp>( flags );
	}
	catch( ... ){
		return nullptr;
	}
}

IntervalSensor_Imp::IntervalSensor_Imp( int flags )
	:	Sensor_Imp		{ flags },
		m_bTriggerInside{ true }
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
TractionSensor_Imp::TractionSensor_Imp( int flags )
	: IntervalSensor_Imp{ flags }
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
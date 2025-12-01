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

#include "TractionForceCharacteristic_Imp.h"

#include <algorithm>

namespace trax{
///////////////////////////////////////
std::unique_ptr<TractionForceCharacteristic> TractionForceCharacteristic::Make( 
	TractionType type, 
	Velocity vmax, 
	Velocity vtransit ) noexcept
{
	try{
		switch( type )
		{
		case TractionType::hydrodynamic:
			return std::make_unique<TFC_Hydrodynamic>(
				vmax, vtransit );
		case TractionType::electric:
		case TractionType::steam:
			return std::make_unique<TFC_CriticalVelocities>(
				type, vmax, 0_mIs );
		case TractionType::speedsteps:
			return std::make_unique<TFC_SpeedSteps>( TractionForceCharacteristic::TractionType::speedsteps );
		case TractionType::unknown:
		case TractionType::mechanic:
		case TractionType::other:
		default:
			return nullptr;
		};
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
TFC_SpeedSteps::TFC_SpeedSteps( TractionType /*type*/ )
//	:	m_Type{type}
{}

TFC_SpeedSteps::TFC_SpeedSteps( TractionType /*type*/, const std::vector<std::pair<Velocity,One>>& speedsteps )
	:	//m_Type		{type},
		m_SpeedSteps{speedsteps}
{
	assert( !speedsteps.empty() );

	std::sort( m_SpeedSteps.begin(), m_SpeedSteps.end(), 
		[]( const std::pair<Velocity,Real>& pair1, const std::pair<Velocity,Real>& pair2 ) noexcept
	{ return pair1.first < pair2.first; } );
}

TractionForceCharacteristic::TractionType TFC_SpeedSteps::GetTractionType() const noexcept{
	return TractionType::speedsteps;
}

One TFC_SpeedSteps::operator()( Velocity v ) const noexcept{
	if( m_SpeedSteps.empty() )
		return 1.0f;

	v = abs(v); // TODO: system can get extended to handle asymetric characteristics...

	auto iter = std::find_if( m_SpeedSteps.rbegin(), m_SpeedSteps.rend(), 
		[v]( const std::pair<Velocity,Real>& pair ) noexcept
	{	return v > pair.first; } );

	if( iter == m_SpeedSteps.rend() )
		return m_SpeedSteps.front().second;

	if( iter == m_SpeedSteps.rbegin() )
		return iter->second;

	auto iternext = iter-1;

	const Real fstep = iter->second;
	const Velocity vstep = iter->first;
	const Real fnextstep = iternext->second;
	const Velocity vnextstep = iternext->first;

	return fstep + (fnextstep - fstep) * (v - vstep) / (vnextstep - vstep);
}

decltype(Power{}/Force{}) TFC_SpeedSteps::MaximumPowerFraction() const noexcept{
	if( !m_bMaximumPowerCalculated )
	{
		if( MaximumVelocity() == Velocity{ +infinite } ){
			m_MaxPower = decltype(Power{}/Force{}){ +infinite };
			m_VMaxPower = Velocity{ +infinite };
		}
		else{
			const Velocity velocityStep = 1_mIs;
			Velocity v = 0_mIs;
			for( ; v < MaximumVelocity(); v += velocityStep ){
				const decltype(Power{}/Force{}) vmv = operator()( v ) * v;
				if( vmv > m_MaxPower ){
					m_MaxPower = vmv;
					m_VMaxPower = v;
				}
			}

			v = MaximumVelocity();
			const decltype(Power{}/Force{}) vmv = operator()( v ) * v;
			if( vmv > m_MaxPower ){
				m_MaxPower = vmv;
				m_VMaxPower = v;
			}
		}

		m_bMaximumPowerCalculated = true;
	}

	return m_MaxPower;
}

Velocity TFC_SpeedSteps::MaximumVelocity() const noexcept{
	if( !m_bMaximumVelocityCalculated ){
		auto riter = std::find_if( m_SpeedSteps.rbegin(), m_SpeedSteps.rend(),
			[]( const std::pair<Velocity,Real>& pair ) noexcept { return pair.second >= 0; } ); 

		if( riter != m_SpeedSteps.rend() )
			m_MaxVelocity = riter->first;

		m_bMaximumVelocityCalculated = true;
	}

	return m_MaxVelocity;
}

void TFC_SpeedSteps::AddSpeedStep( Velocity velocity, One value ){
	if( value < -epsilon || value > 1+10*epsilon )
		throw std::invalid_argument( "Invalid value; must be 0 <= value <= 1!" );
	if( velocity < -epsilon__velocity )
		throw std::invalid_argument( "Symmetric characteristic!" );

	if( value < 0 )
		value = 0;
	if( value > 1 )
		value = 1;

	m_SpeedSteps.push_back( std::make_pair( velocity, value ) );

	std::sort( m_SpeedSteps.begin(), m_SpeedSteps.end(),
		[]( const std::pair<Velocity,Real>& pair1, const std::pair<Velocity,Real>& pair2 ) noexcept
	{ return pair1.first < pair2.first; } );

	m_bMaximumPowerCalculated = false;
	m_bMaximumVelocityCalculated = false;
}

void TFC_SpeedSteps::GetSpeedSteps( std::vector<std::pair<Velocity,One>>& speedsteps ) const{
	speedsteps = m_SpeedSteps;
}
///////////////////////////////////////
TFC_CriticalVelocities::TFC_CriticalVelocities( TractionType type, Velocity vm, Velocity vt )
	:	TFC_SpeedSteps{ type }
{
	AddSpeedStep( 0_mIs, 1 );
	AddSpeedStep( vt, 1 );

	const Velocity a = 20_mIs;
	const Velocity x = sqrt( (vm-vt) * (a + (vm-vt)/4) ) - (vm+vt)/2;

	for( Velocity v = vt; v < vm; v += 3_mIs )
		AddSpeedStep( v, a*(vm-v)/((v+x)*(vm+x)) );

	AddSpeedStep( vm, 0 );
}
///////////////////////////////////////
TFC_Hydrodynamic::TFC_Hydrodynamic( Velocity vm, Velocity vt )
	:	TFC_SpeedSteps{ TractionType::hydrodynamic }
{
	AddSpeedStep( 0_mIs, 1 );
	AddSpeedStep( vt, 0.33f );
	AddSpeedStep( vm/2, 0.25f );
	AddSpeedStep( vm, 0 );
}
///////////////////////////////////////
}
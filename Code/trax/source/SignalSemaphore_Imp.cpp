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

#include "SignalSemaphore_Imp.h"

#include "trax/Units.h"
#include "trax/Location.h"
#include "trax/Signal.h"

namespace trax{
	using namespace spat;

VelocityControlSemaphore::VelocityControlSemaphore() noexcept
	:	m_ParameterOffset		{ 0 },
		m_bRotateWithStatus		{ false },
		m_bPreserveUpDirection	{ false }
{
	m_BaseFrame.Init();
}

const char* VelocityControlSemaphore::TypeName() const noexcept{
	return "VelocityControlSemaphore";
}

void VelocityControlSemaphore::Get( Frame<Length,One>& frame ) const{
	if( m_pSignal ){
		frame = m_BaseFrame * GetFrame();

		if( m_bRotateWithStatus )
			frame.RotateNor( static_cast<int>(Get()) * -pi/4 );
	}
}

void VelocityControlSemaphore::Attach(
	std::shared_ptr<Signal> pSignal, Length parameterOffset )
{
	if( (m_pSignal = pSignal) != nullptr ){
		m_ParameterOffset = parameterOffset;

		Location location{ m_pSignal->GetLocation() };
		if( location.IsOnTrack() )
		// does only work if switches are set correctly.
		{ 
			location.Move( m_ParameterOffset );

			location.Transition( m_BaseFrame );

			if( m_bPreserveUpDirection ){
				if( !m_BaseFrame.IsOrthoNormal() )
					m_BaseFrame.OrthoNormalize();

				m_BaseFrame.LookBAt( Up ); // Todo: common up direction is defined in Simulator...
			}
		}


		//Set( pSignal->Get() == Signal::stop ? zero : one, false );

		//Connect( *this, *m_pSignal );
	}
}

IDType VelocityControlSemaphore::GetSignalRef() const noexcept{
	if( m_pSignal )
		return m_pSignal->ID();

	return 0u;
}

Length VelocityControlSemaphore::ParameterOffset() const noexcept{
	return m_ParameterOffset;
}

void VelocityControlSemaphore::ParameterOffset( Length parameterOffset ) noexcept{
	m_ParameterOffset = parameterOffset;
}

bool VelocityControlSemaphore::PreserveUpDirection() const noexcept{
	return m_bPreserveUpDirection;
}

void VelocityControlSemaphore::PreserveUpDirection( bool bPreserveUpDirection ) noexcept{
	m_bPreserveUpDirection = bPreserveUpDirection;
}

}
//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel



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
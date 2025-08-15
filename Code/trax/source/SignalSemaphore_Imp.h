//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/Signal.h"
#include "Indicator_Imp.h"


namespace trax{

	class VelocityControlSemaphore :	public Multicator,
										public SignalAligned{
	public:
		VelocityControlSemaphore() noexcept;

		// Inherited via Multicator
		const char* TypeName() const noexcept override;

		void Get( spat::Frame<Length,One>& frame ) const override;

		using Multicator::Get;

		// Inherited via SignalAligned
		void Attach( std::shared_ptr<Signal> pSignal, Length parameterOffset = 0_m ) override;

		IDType GetSignalRef() const noexcept override;

		Length ParameterOffset() const noexcept override;

		void ParameterOffset( Length parameterOffset ) noexcept override;

		bool PreserveUpDirection() const noexcept override;

		void PreserveUpDirection( bool bPreserveUpDirection ) noexcept override;
	private:
		std::shared_ptr<Signal>	m_pSignal;
		spat::Frame<Length,One>	m_BaseFrame;
		Length					m_ParameterOffset;
		bool					m_bRotateWithStatus;
		bool					m_bPreserveUpDirection;
	};

}
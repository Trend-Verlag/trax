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
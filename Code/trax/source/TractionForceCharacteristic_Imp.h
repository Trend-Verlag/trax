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

#pragma once

#include "trax/TractionForceCharacteristic.h"

namespace trax{

	// Symmetric bidirectional characteristic based on discrete speed steps.
	class TFC_SpeedSteps : public TractionForceCharacteristic{
	public:
		TFC_SpeedSteps( TractionType type );

		TFC_SpeedSteps( TractionType type, const std::vector<std::pair<Velocity,One>>& speedsteps );

		TractionType GetTractionType() const noexcept override;

		One operator()( Velocity velocity ) const noexcept override;

		decltype(Power{}/Force{}) MaximumPowerFraction() const noexcept override;

		Velocity MaximumVelocity() const noexcept override;

		void AddSpeedStep( Velocity velocity, One value ) override;

		void GetSpeedSteps( std::vector<std::pair<Velocity,One>>& speedsteps ) const override;
	private:
	//	TractionType							m_Type = TractionType::unknown;
		std::vector<std::pair<Velocity,One>>	m_SpeedSteps;
		mutable decltype(Power{}/Force{})		m_MaxPower = 0_mIs;
		mutable Velocity						m_VMaxPower = 0_mIs;
		mutable bool							m_bMaximumPowerCalculated = false;
		mutable Velocity						m_MaxVelocity = 0_mIs;
		mutable bool							m_bMaximumVelocityCalculated = false;
	};

	class TFC_CriticalVelocities : public TFC_SpeedSteps{
	public:
		TFC_CriticalVelocities( TractionType type, Velocity vm, Velocity vt );
	};

	class TFC_Hydrodynamic : public TFC_SpeedSteps{
	public:
		TFC_Hydrodynamic( Velocity vm, Velocity vt );
	};

}
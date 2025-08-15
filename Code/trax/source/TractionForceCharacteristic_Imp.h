// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

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
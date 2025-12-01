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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/rigid/trains/WheelFrameSensors.h"
#include "trax/Sensor_Imp.h"

namespace trax
{

	class VelocitySensor_Imp :	public VelocitySensor,
								public Sensor_Imp{
	public:
		VelocitySensor_Imp();

		const char*	TypeName() const noexcept override;

		void Trigger( const Event& _event ) override;

		void SetVelocity( Velocity velocityMin, Velocity velocityMax ) noexcept override;

		void SetVelocity( const common::Interval<Velocity>& velocity ) noexcept override;

		const common::Interval<Velocity>& GetVelocity() const noexcept override;

		void VelocityMin( Velocity velocityMin ) noexcept override;

		Velocity VelocityMin() const noexcept override;

		void VelocityMax( Velocity velocityMax ) noexcept override;

		Velocity VelocityMax() const noexcept override;

		void TriggerInside( bool bTriggerInside ) noexcept override;

		bool TriggerInside() const noexcept override;


	private:
		common::Interval<Velocity> m_Velocity;
		bool m_bTriggerInside;
	};

	class WeighSensor_Imp : public WeighSensor,
							public Sensor_Imp{
	public:
		WeighSensor_Imp();

		const char*	TypeName() const noexcept override;

		void Trigger( const Event& _event ) override;

		void Weight( Mass weightMin, Mass weightMax ) noexcept override;

		void Weight( const common::Interval<Mass>& weight ) noexcept override;

		const common::Interval<Mass>& Weight() const noexcept override;

		void WeightMin( Mass weightMin ) noexcept override;

		Mass WeightMin() const noexcept override;

		void WeightMax( Mass weightMax ) noexcept override;

		Mass WeightMax( ) const noexcept override;

		void TriggerInside( bool bTriggerInside ) noexcept override;

		bool TriggerInside() const noexcept override;

		void WeighTrain( bool bTrain ) noexcept override;

		bool WeighTrain() const noexcept override;
	private:
		common::Interval<Mass> m_Weight;
		bool m_bTriggerInside;
		bool m_bTrain;

		Mass GetMassFrom( const struct WheelFrame& wheelFrame ) const noexcept;
	};


	class IntervalSensor_Imp :	public IntervalSensor,
								public Sensor_Imp{
	public:
		IntervalSensor_Imp();

		void Value( Real valueMin, Real valueMax ) noexcept override;

		void Value( const common::Interval<Real>& interval ) noexcept override;

		const common::Interval<Real>& Value() const noexcept override;

		void ValueMin( Real valueMin ) noexcept override;

		Real ValueMin() const noexcept override;

		void ValueMax( Real valueMax ) noexcept override;

		Real ValueMax() const noexcept override;

		void TriggerInside( bool bTriggerInside ) noexcept override;

		bool TriggerInside() const noexcept override;
	protected:
		bool ShallTrigger( Real value ) const noexcept;
	private:
		common::Interval<Real> m_Value;
		bool m_bTriggerInside;
	};


	class TractionSensor_Imp :	public IntervalSensor_Imp{
	public:
		TractionSensor_Imp();

		const char*	TypeName() const noexcept override;

		void Trigger( const Event& _event ) override;
	};


}

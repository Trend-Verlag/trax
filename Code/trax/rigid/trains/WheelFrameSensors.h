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

#include "trax/Sensor.h"

namespace trax{

	//Todo: general interval - sensor for the next two

	/// \brief A sensor that detects, wether a wheelframe passing by 
	/// does this within certain velocity limits.
	struct VelocitySensor : virtual Sensor{

		/// \brief Makes a VelocitySensor object.
		static dclspc std::unique_ptr<VelocitySensor> Make() noexcept;

		/// \name Velocity
		/// \brief Sets and Gets the velocity limits for the sensor.
		/// \param velocityMin lower limit for the velocity interval.
		/// \param velocityMax upper limit for the velocity interval.
		///@{
		virtual void SetVelocity( Velocity velocityMin, Velocity velocityMax ) = 0;

		virtual void SetVelocity( const common::Interval<Velocity>& velocity ) = 0;

		virtual const common::Interval<Velocity>& GetVelocity() const = 0;

		virtual void VelocityMin( Velocity velocityMin ) = 0;

		virtual Velocity VelocityMin() const = 0;

		virtual void VelocityMax( Velocity velocityMax ) = 0;

		virtual Velocity VelocityMax( ) const = 0;
		///@}

		/// \name Trigger Condition
		/// \brief Sets the sensor to wether it riggers if the wheelframe's velocity
		/// is inside the interval (true, the default) or outside (false).
		///@{
		virtual void TriggerInside( bool bTriggerInside ) = 0;

		virtual bool TriggerInside() const = 0;
		///@}
	};

	/// \brief A sensor that detects wether the weight of a passing railrunner
	/// falls into certain limits.
	struct WeighSensor : virtual Sensor{

		/// \brief Makes a WeighSensor object.
		static dclspc std::unique_ptr<WeighSensor> Make() noexcept;

		/// \name Weight
		/// \brief Sets and Gets the weight limits for the sensor.
		/// \param weightMin lower limit for the weight interval.
		/// \param weightMax upper limit for the weight interval.
		///@{
		virtual void Weight( Mass weightMin, Mass weightMax ) = 0;

		virtual void Weight( const common::Interval<Mass>& weight ) = 0;

		virtual const common::Interval<Mass>& Weight() const = 0;

		virtual void WeightMin( Mass weightMin ) = 0;

		virtual Mass WeightMin() const = 0;

		virtual void WeightMax( Mass weightMax ) = 0;

		virtual Mass WeightMax( ) const = 0;
		///@}

		/// \name Trigger Condition
		/// \brief Sets the sensor to wether it riggers if the wheelframe's weight
		/// is inside the interval (true, the default) or outside (false).
		///@{
		virtual void TriggerInside( bool bTriggerInside ) = 0;

		virtual bool TriggerInside() const = 0;
		///@}


		/// \name WeighTrain
		/// \brief Determines wether to weigh the whole train or only the wagon, the wheelframe belongs to.
		///
		/// \param bTrain true for wighing the train, the default.
		///@{
		virtual void WeighTrain( bool bTrain ) = 0;

		virtual bool WeighTrain() const = 0;
		///@}
	};


	/// \brief A sensor that detects, wether a wheelframe passing by 
	/// does this with some value like weight, velocity or traction in certain limits.
	struct IntervalSensor : virtual Sensor{

		/// \brief Makes a IntervalSensor object.
		static dclspc std::unique_ptr<IntervalSensor> Make() noexcept;

		/// \name Value
		/// \brief Sets and Gets the value limits for the sensor.
		/// \param valueMin lower limit for the interval.
		/// \param valueMax upper limit for the interval.
		///@{
		virtual void Value( Real valueMin, Real valueMax ) = 0;

		virtual void Value( const common::Interval<Real>& interval ) = 0;

		virtual const common::Interval<Real>& Value() const = 0;

		virtual void ValueMin( Real valueMin ) = 0;

		virtual Real ValueMin() const = 0;

		virtual void ValueMax( Real valueMax ) = 0;

		virtual Real ValueMax( ) const = 0;
		///@}

		/// \name Trigger Condition
		/// \brief Sets the sensor to wether it riggers if the wheelframe's weight
		/// is inside the interval (true, the default) or outside (false).
		///@{
		virtual void TriggerInside( bool bTriggerInside ) = 0;

		virtual bool TriggerInside() const = 0;
		///@}
	};

}
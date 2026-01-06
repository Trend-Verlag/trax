//	trax track library
//	AD 2025 
//
//  "It is computed that eleven thousand persons 
//   have at several times suffered death, 
//   rather than submit to break their eggs 
//   at the smaller end."
//
//								Jonathan Swift
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

/// \page docu_dim Dimensionated Values
/// 
/// \section dim_limits Limits
/// 
/// The limits of numerical calculations. E.g. for EEP, the smallest value in 
/// length that would make a difference to the user, trax::epsilon__length, 
/// would be something about 1_cm. From this the trax::plausible_maximum_length 
/// can be estimated to be about 10_km for 32bit trax::Real values. At that 
/// distance from the origin, the calculations would start to produce inaccurate 
/// results with respect to the 1_cm difference. It is not accidential, that 
/// the biggest layouts ever build in EEP are about 20_km in diameter. For 
/// optimal results, the trax::meters_per_unit should not be 0.01 nor 1 but 
/// rather something like 15.
/// 
/// 

#include "DimensionedValues.h"

/// \file DimLimits.h User configurationable values for the dim library.
/// The default values relate to a real world railroad simulation.
namespace dim
{
	/// \name Marginal values
	///@{
	constexpr One		epsilon__one	= 10 * epsilon;
	constexpr Angle		epsilon__angle	= 1_deg;
	constexpr Length	epsilon__length = 1_cm;
	constexpr Mass		epsilon__mass	= 10_kg;
	constexpr Time		epsilon__time	= 1_s;
	///@}

	/// \name Maximum calculatablel values
	/// \brief the distance from origin, our epsilon accuracy calculations will possibly break down.
	///
	///@{
	constexpr Angle		maximum__angle	= epsilon__angle / epsilon__one;
	constexpr Length	maximum__length = epsilon__length / epsilon__one;
	constexpr Mass		maximum__mass	= epsilon__mass / epsilon__one;
	constexpr Time		maximum__time	= epsilon__time / epsilon__one;
	///@}

	
	/// \name Plausible maximum values
	///@{
	constexpr Angle		plausible_maximum__angle= 360_deg;
	constexpr Length	plausible_maximum_length= maximum__length;
	constexpr Mass		plausible_maximum__mass	= maximum__mass;
	constexpr Time		plausible_maximum__time	= 1_h;
	///@}

	static_assert( plausible_maximum__angle <= maximum__angle, "plausible_maximum__angle must be smaller or equal then maximum__angle" );
	static_assert( plausible_maximum_length <= maximum__length, "plausible_maximum_length must be smaller or equal then maximum__length" );
	static_assert( plausible_maximum__mass <= maximum__mass, "plausible_maximum__mass must be smaller or equal then maximum__mass" );
	static_assert( plausible_maximum__time <= maximum__time, "plausible_maximum__time must be smaller or equal then maximum__time" );


	/// \name Derived marginal values
	///@{
	constexpr AnglePerLength	epsilon__anglePerLength		= epsilon__angle / plausible_maximum_length;
	constexpr AngularVelocity	epsilon__angularVelocity	= epsilon__angle / plausible_maximum__time;
	constexpr Area				epsilon__area				= epsilon__length * epsilon__length;
	constexpr Volume			epsilon__volume				= epsilon__area * epsilon__length;
	constexpr Density			epsilon__density			= 0.001_gIcm3;
	constexpr Frequency			epsilon__frequency			= 1 / plausible_maximum__time;
	constexpr Velocity			epsilon__velocity			= 1_cmIs;
	constexpr Acceleration		epsilon__acceleration		= 0.001_mIs2;
	constexpr Force				epsilon__force				= 0.1_N;
	constexpr Pressure			epsilon__pressure			= 1_Pa;
	constexpr Momentum			epsilon__momentum			= epsilon__mass * epsilon__velocity;
	constexpr AngularMomentum	epsilon__angularMomentum	= epsilon__momentum * epsilon__length;
	constexpr MomentOfInertia	epsilon__momentOfInertia	= epsilon__mass * epsilon__length * epsilon__length;
	constexpr Torque			epsilon__torque				= 1_Nm;
	constexpr Energy			epsilon__energy				= 1_J;
	constexpr Power				epsilon__power				= 1_W;
	///@}

} // namespace dim

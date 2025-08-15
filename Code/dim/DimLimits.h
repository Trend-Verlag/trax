// Dimensioned Numbers With Units Are Values!
// ******************************************
//
// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2025 
//
//  "It is computed that eleven thousand persons 
//   have at several times suffered death, 
//   rather than submit to break their eggs 
//   at the smaller end."
//
//								Jonathan Swift

#pragma once

/// \file DimLimits.h User configurationable values for the dim library.
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
	constexpr Torque			epsilon__torque				= 1_Nm;
	constexpr Energy			epsilon__energy				= 1_J;
	constexpr Power				epsilon__power				= 1_W;
	///@}

} // namespace dim

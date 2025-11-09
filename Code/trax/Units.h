//	trax track library
//	AD 2014
//
//  "She asked you if you love her
//	and you nod and say: Uh-Huh"
//
//						Jade Bird
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

/// \page docu_units Units
/// \section units_intro Introduction
///
/// \section units_motivation Motivation
///
/// \section units_detail Details
/// 
/// The spatial structures work together with dimensionated values seamlessly, just be aware of the type of the result:
/// \code
/// Vector<AngularVelocity> w{ Ez<One> * 20_deg / 1_s };
/// Vector<Length> d{ Ex<One> * 2.8_m };
///	Vector<Velocity> v = w % d;
///	std::cout << "the momentary velocity of a point rotating around " << w << " at distance " << d << ": " << v << std::endl; // prints: "[...]  Vector( 0km/h, 3.52km/h, 0km/h )" 
/// \endcode
///
/// Don't mind too much about performance: all these functions and operators are declared 'constexpr'. This means 'evaluated at compile time 
/// if so possible' (intellisense over the 'v'). This is even better than inlining (which of course they are declared, too). Even if compile time evaluation is not possible, with constexpr 
/// the compiler has a hint for optimizing in release version.
/// 
/// \section units_examples Examples
///
/// To use spatial structures with dimensionated values:
/// \code
///	Position<Length> P1{1_m,2_m,3_m};
///	Position<Length> P2{1_m,1_m,1_m};
///	
///	Vector<Length> D = P2 - P1;
///	BOOST_CHECK( D.Length() == sqrt(5_m2) );
/// 
/// Vector<Velocity> V = D / 2_s;
/// BOOST_CHECK( V.Length() != 2.5_mIs );
/// \endcode
///
/// Do also look at the tests in the test suit for further examples.

#include "dim/DimensionedValues.h"
#include "dim/DimLimits.h"

#include "spat/Vector.h"

namespace trax {

	using namespace dim;


	/// \brief The maximum time, a user is expected to concentrate on one single apect of the simulation.
	constexpr Time attention_span = 3_s;	

	constexpr Velocity maximum__velocity = 500_kmIh;

	/// \brief Acceleration due to gravity
	constexpr Acceleration g{-9.81f * units_per_meter};

	constexpr Velocity bolt = 100_m / 9.58_s;	///< fastest man in the world

	/// \brief A default value for the acceptable distance between a
	/// track position and something beeing still considered as railed.
	constexpr Length default_derailing_distance = 1.435_m / 4;

	constexpr Length earth_radius = 6.371e6_m;

	// \brief Fixed simulation steps per second.
	constexpr Real steps_per_second = 60;

	/// \brief The fixed timestep to use with the simulation.
	constexpr Time fixed_timestep = 1_s / steps_per_second;

	/// <summary>
	/// Vector pointing in the up direction with respect to gravity.
	/// </summary>
	/// <remarks>
	/// Should not be Ex<One>! <see cref="Curve">Curve</see>
	/// </remarks>
	constexpr spat::Vector<One> Up = spat::Ez<One>;

	constexpr spat::Vector<Acceleration> G = g * Up;

	/// \brief Marginal curvature is earth's curvature.
	constexpr AnglePerLength epsilon__curvature = 1 / earth_radius;
} // namespace trax
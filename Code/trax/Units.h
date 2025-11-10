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

/// \page docu_units Space
/// \section units_intro Introduction
/// The spat and the dim libraries work seemlessly together to provide for 
/// our railway simulation what it calls 'the space'. A Position<Length>
/// represents a point in this space; a Vector<One> represents a direction.
/// Together they form a Frame<Length,One> that represents a complete frame 
/// of reference and allows for easy transformations. A velocity can be 
/// expressed as a Vector<Velocity>, an acceleration as a Vector<Acceleration>.
/// A force might be a VectorBundle<Length,Force>, giving us both the point
/// of application and the force vector itself.
///
/// You get the idea.
///
/// \section units_examples Examples
/// 
/// \code
/// Mass m = 130_kg;
/// Vector<Acceleration> a = G;	// acceleration due to gravity
/// Vector<Force> F = m * a;
/// std::cout << "the weight of a mass of " << m << " is " << F << std::endl; // prints: "the weight of a mass of 130kg is Vector( -0kN, -0kN, -1.2753kN )"
/// \endcode
/// 
/// \code
/// Vector<AngularVelocity> w{ Ez<One> * 20_deg / 1_s };
/// Vector<Length> d{ Ex<One> * 2.8_m };
///	Vector<Velocity> v = w % d;
///	std::cout << "the momentary velocity of a point rotating around " << w << " at distance " << d << ": " << v << std::endl; // prints: "[...]  Vector( 0km/h, 3.52km/h, 0km/h )" 
/// \endcode
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
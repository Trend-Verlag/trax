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

/// \page units_docu Units
/// \section units_intro Introduction
/// Calculations in physics not only deal with stark naked numbers (for tranquility of mind we call them 'values'), 
/// but they are generally also connected with two other aspects: dimension and unit. The dimension is the 
/// quality that is dealt with, be it a length in space, a time span or an inertial mass or any combination of these.
/// The units on the other hand specify the quantity we are measuring, telling us that 1000 are actually 1000_kg, not 1000_t.
/// Basically the trax library automated the lecture of our old physics teacher, who said: check the dimensions of your result and 
/// then reason about its magnitude.
///
/// \section units_motivation Motivation
/// One wouldn't believe how much can go wrong on adding up two numbers. There were several very costly accidents due to software failure in the past (luckily nobody got hurt). As it turned out, calculations are a common
/// source of bugs due to mistaken units or dimensions of values. This brought the luminaries of our field to suggest a way for typesafe
/// handling of physical values. We make use of that partly for bug reduction, partly because it looks neat, but mostly because it 
/// gives us the possibility to adjust the range of the actual floating point calculations lateron. E.g. for EEP, the smallest value in length that
/// would make a difference to the user, trax::epsilon__length, would be something about 1_cm. From this the trax::plausible_maximum_length can be estimated
/// to be about 10_km for 32bit trax::Real values. At that distance from the origin, the calculations would start to produce inaccurate results with respect to the 1_cm difference.
/// It is not accidential, that the biggest layouts ever build in EEP are about 20_km in diameter. For optimal results, the trax::meters_per_unit 
/// should not be 0.01 nor 1 but rather something like 15.
///
/// \section units_detail Details
/// 
/// The idiom: trax::Length l{1.2f} has to be used with care, because it assigns a value directly. This is typically done only internally in the library. 
/// Instead Length l = 1.2_m should be used, if the 1.2 happen to be meters. If you have a bare value 'lengthInCentimers' and happen to know it to be centimeters, assign it like this:
/// \code
/// Length length = _cm(lengthInCentimers);
/// \endcode
/// If you want to get a value in centimeters from a trax::Length length, do this:
/// \code
/// Real lengthInCentimers = _cm(length);
/// Real lengthInCentimers = length / Length{1}; //Don't do this.
/// Real lengthInCentimers = length.Units()      //And don't do this either.
/// \endcode
///
/// The spatial structures work together with dimensionated values seamlessly, just be aware of the type of the result:
/// \code
/// constexpr spat::Vector<AngularVelocity> w{ spat::Ez<One> * 20_deg / 1_s };
/// constexpr spat::Vector<Length> d{ spat::Ex<One> * 2.8_m };
/// constexpr spat::Vector<Velocity> v = w % d; // the momentary velocity of a point rotating around w at distance d.
/// \endcode
///
/// Don't mind too much about performance: all these functions and operators are declared 'constexpr'. This means 'evaluated at compile time 
/// if so possible' (intellisense over the 'v'). This is even better than inlining (which of course they are declared, too). Even if compile time evaluation is not possible, with constexpr 
/// the compiler has a hint for optimizing in release version.
/// 
/// \section units_examples Examples
///
/// To add two masses:
/// \code
/// Mass result1 = 1_kg + 2_kg;
/// assert( result1 == 3_kg );
/// Mass result2 = 1_t + 200_kg;
/// assert( result2 == 1200_kg );
/// \endcode
///
/// To multiply two lengthes:
/// \code
/// Length l1 = 1_m, l2 = 2_m;
/// Length result = l1 * l2; // would not compile
/// Area result = l1 * l2; // fine
/// assert( result == 2_m2);
/// \endcode
///
/// To deal with mixed dimensions:
/// \code
/// Density result = 7.874_t / 1_m3;
/// assert( result == 7.874_gIcm3 );
/// \endcode
/// 
/// To use mathematical functions:
/// \code
/// Length l = trax::abs(-2_m);
/// assert( l == 2_m );
/// Area a = trax::pow<2>(l);
/// assert( a == 4_m2 );
/// \endcode
/// 
/// To use spatial structures with dimensionated values:
/// \code
///	spat::Position<Length> P1{1_m,2_m,3_m};
///	spat::Position<Length> P2{1_m,1_m,1_m};
///	
///	spat::Vector<Length> D = P2 - P1;
///	assert( D.Length() == trax::sqrt(5_m2) );
/// 
/// spat::Vector<Velocity> V = D / 2_s;
/// assert( V.Length() != 2.5_mIs );
/// \endcode
///
/// To calculate the resting energy of a bumblebee:
/// \code
/// Mass m = 1.2_g;
/// Velocity c = 299792458_mIs;
/// Energy E = m * c * c;
/// std::cout << E << std::endl; // should print: 1.07851e+11kNm  (would be about 100 Terajoule or 25kT TNT - just the energy that was released by the Nagasaki bomb ...)
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
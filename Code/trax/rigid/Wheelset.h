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

#include "trax/Units.h"

#include "spat/Position2D.h"

namespace trax{

	/// \brief Describing data of a wheelset.
	///
	/// A wheelset is an axle with two wheels attached to it.
	/// It is used for calculating driving forces and to build
	/// a collision geom for the wheel within a WheelFrame.
	struct Wheelset{
		Length Radius;							///< Radius of the wheelpart it is rolling on the track with.
		Length Gauge;							///< Track gauge.
		Length Flange;							///< Height of flange around the radius.
		Length WheelWidth;						///< Width of the wheel.
		Length AxleRadius;						///< Radius of the axle that holds the two wheels
		Mass TotalMass;							///< Total mass of this wheelset.
		spat::Position2D<Length> AxlePosition;	///< Position of wheel axis relative to the WheelFrame's anchor A on the track (in A.T x A.B plane).
		Angle Rotation;							///< Rotation angle around axle (A.N).
		Length Shift;							///< Shifting state along axle (A.N).

		Torque MaxMotorTorque;					///< Maximum torque that this wheelset can apply for acceleration.
		Torque MaxBrakingTorque;				///< Maximum braking torque that this wheelset can apply.
		Torque FrictionTorque;					///< Friction that is introduced by this wheel.

		Wheelset() noexcept
			:	Radius				{0.5_m},
				Gauge				{1.435_m},
				Flange				{3_cm},
				WheelWidth			{0.2_m},
				AxleRadius			{0.1_m},
				TotalMass			{1500_kg},
				AxlePosition		{0_m,0.5_m},
				Rotation			{0},
				Shift				{0_m},
				MaxMotorTorque		{0}, //(30000.0f * units_per_kilogram * Square(units_per_meter)),	//N*m
				MaxBrakingTorque	{0}, //(30000.0f * units_per_kilogram * Square(units_per_meter)),	//N*m
				FrictionTorque		{0}
		{}

		Wheelset(
			Length Radius,
			Length Gauge,
			Length Flange,
			Length WheelWidth,
			Length AxleRadius,
			Mass TotalMass,
			spat::Position2D<Length> AxlePosition,
			Angle Rotation,
			Length Shift,

			Torque MaxMotorTorque,
			Torque MaxBrakingTorque,
			Torque FrictionTorque ) noexcept
			:	Radius			{Radius},
				Gauge			{Gauge},
				Flange			{Flange},
				WheelWidth		{WheelWidth},
				AxleRadius		{AxleRadius},
				TotalMass		{TotalMass},
				AxlePosition	{AxlePosition},
				Rotation		{Rotation},
				Shift			{Shift},
				MaxMotorTorque	{MaxMotorTorque},
				MaxBrakingTorque{MaxBrakingTorque},
				FrictionTorque	{FrictionTorque}
		{}

		inline bool IsValid() const{
			return	Radius > 0_m &&
					Gauge > 0_m &&
					TotalMass > 0_kg;
		}
	};
}
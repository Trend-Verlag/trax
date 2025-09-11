//	trax track library
//	AD 2015
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//
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

#include "trax/Identified.h"
#include "trax/Units.h"

#include "spat/Frame.h"

#include <memory>

namespace trax{

	struct Body;

	struct Joint : Identified<Joint>
	{
		/// \brief The dominant body of the connection.
		virtual std::shared_ptr<Body> BodyA() const noexcept = 0;


		/// \brief Set the dominant body of the connection.
		virtual void BodyA( std::shared_ptr<Body> pBody ) const noexcept = 0;


		/// \brief The pose of the joint in the local coordinate system of body A.
		virtual spat::Frame<Length,One> LocalPoseA() const noexcept = 0;


		/// \brief Set the pose of the joint in the local coordinate system of body A.
		virtual void LocalPoseA( const spat::Frame<Length,One>& pose ) const noexcept = 0;


		/// \brief The subordinate body of the connection.
		virtual std::shared_ptr<Body> BodyB() const noexcept = 0;


		/// \brief Set the subordinate body of the connection.
		virtual void BodyB( std::shared_ptr<Body> pBody ) const noexcept = 0;


		/// \brief The pose of the joint in the local coordinate system of body B.
		virtual spat::Frame<Length,One> LocalPoseB() const noexcept = 0;


		/// \brief Set the pose of the joint in the local coordinate system of body B.
		virtual void LocalPoseB( const spat::Frame<Length,One>& pose ) const noexcept = 0;
	};


	struct HingeJoint : Joint
	{
		virtual Angle GetAngle() const noexcept = 0;


		virtual void SetAngle( Angle angle ) const noexcept = 0;



	};


	struct SliderJoint : Joint
	{
		virtual Length GetPosition() const noexcept = 0;


		virtual void SetPosition( Length position ) const noexcept = 0;
	};


	struct BallAndSocketJoint : Joint
	{


	};


	struct DistanceJoint : Joint
	{
		virtual Length GetDistance() const noexcept = 0;

		virtual void SetDistance( Length distance ) const noexcept = 0;
	};
}

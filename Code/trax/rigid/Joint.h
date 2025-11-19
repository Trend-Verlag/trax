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

/// \page docu_joints Joints
/// 
/// \section joints_intro Introduction
/// A Joint connects two Bodies together, restricting relative movement 
/// between them in certain ways. Different types of Joints provide different 
/// constraints on the movement of the connected Bodies. 

#include "trax/Identified.h"
#include "trax/Units.h"

#include "spat/Frame.h"

#include <memory>

namespace trax{

	struct Body;

	struct Joint
	{
		/// \brief Sets the name of the Joint.
		/// 
		/// This is for debug purposes; the string is not copied,
		/// only the pointer is stored.
		virtual void SetName( const char* name ) noexcept = 0;


		/// \returns The name of the Joint.
		virtual const char* GetName() const noexcept = 0;


		/// \brief The dominant body of the connection.
		virtual Body* BodyA() const noexcept = 0;


		/// \brief Set the dominant body of the connection.
		virtual void BodyA( Body* pBody ) = 0;


		/// \brief The pose of the joint in the local coordinate system of body A.
		virtual spat::Frame<Length,One> LocalPoseA() const noexcept = 0;


		/// \brief Set the pose of the joint in the local coordinate system of body A.
	//	virtual void LocalPoseA( const spat::Frame<Length,One>& pose ) const noexcept = 0;


		/// \brief The subordinate body of the connection.
		virtual Body* BodyB() const noexcept = 0;


		/// \brief Set the subordinate body of the connection.
		virtual void BodyB( Body* pBody ) = 0;


		/// \brief The pose of the joint in the local coordinate system of body B.
		virtual spat::Frame<Length,One> LocalPoseB() const noexcept = 0;


		/// \brief Set the pose of the joint in the local coordinate system of body B.
	//	virtual void LocalPoseB( const spat::Frame<Length,One>& pose ) const noexcept = 0;

		virtual ~Joint() = default;
	};


	struct HingeJoint : Joint
	{
		/// \returns The axis of rotation in local coordinates of the parent (BodyA).
		virtual spat::VectorBundle<Length,One> LocalAxisA() const noexcept = 0;

		/// \returns The axis of rotation in local coordinates of the child (BodyB).
		virtual spat::VectorBundle<Length,One> LocalAxisB() const noexcept = 0;

		/// \returns The rotational angle the child turned around the axis of rotation.
		virtual Angle GetTurnAngle() const noexcept = 0;

		/// \returns The side bending or tilt between parent and child.
		virtual Angle GetBendAngle() const noexcept = 0;

		/// \returns The pitch angle of the child relative to the parent.
		virtual Angle GetPitchAngle() const noexcept = 0;
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

		virtual spat::Vector<Force> GetForce() const noexcept = 0;
	};
}

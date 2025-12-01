//	trax track library
//	AD 2025
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
// For further information, please contact: horstmann.marc@trendverlag.de

#include "PhysX_Joints_Imp.h"

#include "spat/VectorBundle.h"

namespace trax{
///////////////////////////////////////
PhysX_HingeJoint_Imp::PhysX_HingeJoint_Imp( 
	physx::PxPhysics& physics, 
	Body* pBodyA, 
	spat::Frame<Length,One> poseParent, 
	Body* pBodyB, 
	spat::Frame<Length,One> poseChild, 
	Real engine_meters_per_unit )
: PhysX_Joint_Imp<HingeJoint>{ pBodyA, pBodyB, engine_meters_per_unit }
, m_D6Joint{ *physx::PxD6JointCreate(
		physics,
		pBodyA ? &Actor( *pBodyA ) : nullptr,
		PoseFrom( poseParent.RotateNor( pi/2 ) ),
		pBodyB ? &Actor( *pBodyB ) : nullptr,
		PoseFrom( poseChild.RotateNor( pi/2 ) ) ) }
{
	m_D6Joint.setMotion( physx::PxD6Axis::eTWIST, physx::PxD6Motion::eFREE );
	m_D6Joint.setMotion( physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLIMITED );
	m_D6Joint.setMotion( physx::PxD6Axis::eSWING2, physx::PxD6Motion::eLOCKED );
//	m_D6Joint.setMotion( physx::PxD6Axis::eSWING2, physx::PxD6Motion::eLIMITED );

	physx::PxReal driveForceLimit = 10000;
	physx::PxReal driveStiffness = 10000;
	physx::PxReal driveDamping = 10000;

	physx::PxJointLimitCone swingLimit{ 
		_rad( 30_deg ), 
		_rad( 1_deg ), 
		physx::PxSpring{ driveStiffness, driveDamping } 
	};//PX_MAX_F32, PX_MAX_F32 } };
	swingLimit.restitution = 1.0f;
	m_D6Joint.setSwingLimit( swingLimit );

	m_D6Joint.setAngularDriveConfig( physx::PxD6AngularDriveConfig::eSWING_TWIST );
	m_D6Joint.setDrive( physx::PxD6Drive::eSWING1, physx::PxD6JointDrive{ driveStiffness, driveDamping, driveForceLimit, false } );
	m_D6Joint.setDrivePosition( PoseFrom( poseParent.RotateNor( pi/2 ) ) );

	m_D6Joint.setConstraintFlag( physx::PxConstraintFlag::eCOLLISION_ENABLED, true );
	m_D6Joint.setConstraintFlag( physx::PxConstraintFlag::eVISUALIZATION, true ); // Visual Debugger causes benign PhysX/PVD validation warning caused by a mismatch between the D6 joint's angular-drive 'mode' and what the Visual Debugger tries to query. 
}

PhysX_HingeJoint_Imp::~PhysX_HingeJoint_Imp()
{
	m_D6Joint.release();
}

spat::Frame<Length,One> PhysX_HingeJoint_Imp::LocalPoseA() const noexcept{
	return PhysX_Joint_Imp<HingeJoint>::LocalPoseA().RotateNor( -pi/2 );
}

spat::Frame<Length,One> PhysX_HingeJoint_Imp::LocalPoseB() const noexcept{
	return PhysX_Joint_Imp<HingeJoint>::LocalPoseB().RotateNor( -pi/2 );
}

spat::VectorBundle<Length,One> PhysX_HingeJoint_Imp::LocalAxisA() const noexcept
{
	spat::Frame<Length,One> localPoseA = LocalPoseA();
	return { localPoseA.P, localPoseA.B };
}

spat::VectorBundle<Length,One> PhysX_HingeJoint_Imp::LocalAxisB() const noexcept
{
	spat::Frame<Length,One> localPoseB = LocalPoseB();
	return { localPoseB.P, localPoseB.B };
}

Angle PhysX_HingeJoint_Imp::GetTurnAngle() const noexcept{
	return _rad(m_D6Joint.getTwistAngle());
}

Angle PhysX_HingeJoint_Imp::GetBendAngle() const noexcept{
	return _rad( m_D6Joint.getSwingZAngle() );
}

Angle PhysX_HingeJoint_Imp::GetPitchAngle() const noexcept{
	return _rad( m_D6Joint.getSwingYAngle() );
}

physx::PxJoint& PhysX_HingeJoint_Imp::BodyJoint() const noexcept{
	return m_D6Joint;
}
///////////////////////////////////////
PhysX_SliderJoint_Imp::PhysX_SliderJoint_Imp( 
	physx::PxPhysics& physics, 
	Body* pBodyA, 
	spat::Frame<Length,One> poseA, 
	Body* pBodyB, 
	spat::Frame<Length, One> poseB, 
	Real engine_meters_per_unit )
: PhysX_Joint_Imp<SliderJoint>{ pBodyA, pBodyB, engine_meters_per_unit }
, m_D6Joint{ *physx::PxD6JointCreate(
		physics,
		pBodyA ? &Actor( *pBodyA ) : nullptr,
		PoseFrom( poseA ),
		pBodyB ? &Actor( *pBodyB ) : nullptr,
		PoseFrom( poseB ) ) }
{
}

PhysX_SliderJoint_Imp::~PhysX_SliderJoint_Imp()
{
	m_D6Joint.release();
}

Length PhysX_SliderJoint_Imp::GetPosition() const noexcept
{
	return Length();
}

void PhysX_SliderJoint_Imp::SetPosition( Length position ) const noexcept
{
}

physx::PxJoint& PhysX_SliderJoint_Imp::BodyJoint() const noexcept
{
	return m_D6Joint;
}
///////////////////////////////////////
PhysX_BallAndSocketJoint_Imp::PhysX_BallAndSocketJoint_Imp(
	physx::PxPhysics& physics, 
	Body* pBodyA, 
	spat::Frame<Length,One> poseA, 
	Body* pBodyB, 
	spat::Frame<Length, One> poseB, 
	Real engine_meters_per_unit )
: PhysX_Joint_Imp<BallAndSocketJoint>{ pBodyA, pBodyB, engine_meters_per_unit }
, m_D6Joint{ *physx::PxD6JointCreate( 
		physics, 
		pBodyA ? &Actor( *pBodyA ) : nullptr, 
		PoseFrom( poseA), 
		pBodyB ? &Actor( *pBodyB ) : nullptr, 
		PoseFrom( poseB ) ) }
{
}

PhysX_BallAndSocketJoint_Imp::~PhysX_BallAndSocketJoint_Imp()
{
	m_D6Joint.release();
}

physx::PxJoint& PhysX_BallAndSocketJoint_Imp::BodyJoint() const noexcept
{
	return m_D6Joint;
}
///////////////////////////////////////
PhysX_DistanceJoint_Imp::PhysX_DistanceJoint_Imp(
	physx::PxPhysics& physics, 
	Body* pBodyA, 
	spat::Frame<Length,One> poseA, 
	Body* pBodyB, 
	spat::Frame<Length, One> poseB, 
	Real engine_meters_per_unit )
: PhysX_Joint_Imp<DistanceJoint>{ pBodyA, pBodyB, engine_meters_per_unit }
	, m_DistanceJoint{ *physx::PxDistanceJointCreate( 
		physics, 
		pBodyA ? &Actor( *pBodyA ) : nullptr, 
		PoseFrom( poseA), 
		pBodyB ? &Actor( *pBodyB ) : nullptr, 
		PoseFrom( poseB ) ) }
{
	m_DistanceJoint.setMaxDistance( static_cast<physx::PxReal>(_m(1_m)/engine_meters_per_unit) );
	m_DistanceJoint.setDistanceJointFlag( physx::PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true );
	m_DistanceJoint.setConstraintFlag( physx::PxConstraintFlag::eCOLLISION_ENABLED, true );
	m_DistanceJoint.setConstraintFlag( physx::PxConstraintFlag::eVISUALIZATION, true );
}

PhysX_DistanceJoint_Imp::~PhysX_DistanceJoint_Imp()
{
	m_DistanceJoint.release();
}

Length PhysX_DistanceJoint_Imp::GetDistance() const noexcept{
	return _m(m_DistanceJoint.getMaxDistance() * m_Engine_meters_per_unit);
}

void PhysX_DistanceJoint_Imp::SetDistance( Length distance ) const noexcept{
	m_DistanceJoint.setMaxDistance( static_cast<physx::PxReal>(_m(distance) / m_Engine_meters_per_unit) );
}

spat::Vector<Force> PhysX_DistanceJoint_Imp::GetForce() const noexcept{
	physx::PxVec3 force, torque;
	m_DistanceJoint.getConstraint()->getForce( force, torque );
	return VecFrom<Force>(force * m_Engine_meters_per_unit);
}

physx::PxJoint& PhysX_DistanceJoint_Imp::BodyJoint() const noexcept{
	return m_DistanceJoint;
}
///////////////////////////////////////
}

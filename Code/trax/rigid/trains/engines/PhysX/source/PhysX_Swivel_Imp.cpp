//	trax track library
//	AD 2023 
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

#include "PhysX_Swivel_Imp.h"

#include "trax/rigid/trains/source/Bogie_Imp.h"
#include "trax/rigid/engines/PhysX/source/PhysX_Body_Imp.h"

namespace trax{


PhysX_Swivel_Imp::PhysX_Swivel_Imp( 
	physx::PxPhysics& physics, 
	Bogie* pBogieParent, 
	spat::Frame<Length,One> poseParent, 
	Bogie* pBogieChild, 
	spat::Frame<Length,One> poseChild, 
	Real engine_meters_per_unit )
	: PhysX_Joint_Imp<Swivel>{ pBogieParent, pBogieChild, engine_meters_per_unit }
	, m_D6Joint{ *physx::PxD6JointCreate(
		physics,
		pBogieParent ? &Actor( pBogieParent->GetGestalt() ) : nullptr,
		PoseFrom( poseParent.RotateNor( pi/2 ) ),
		pBogieChild ? &Actor( pBogieChild->GetGestalt() ) : nullptr,
		PoseFrom( poseChild.RotateNor( pi/2 ) ) ) }
{
	m_D6Joint.setMotion( physx::PxD6Axis::eTWIST, physx::PxD6Motion::eFREE );
	m_D6Joint.setMotion( physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLIMITED );
	m_D6Joint.setMotion( physx::PxD6Axis::eSWING2, physx::PxD6Motion::eLOCKED );
//	m_D6Joint.setMotion( physx::PxD6Axis::eSWING2, physx::PxD6Motion::eLIMITED );

	physx::PxReal driveForceLimit = 10000;
	physx::PxReal driveStiffness = 10000;
	physx::PxReal driveDamping = 10000;

	physx::PxJointLimitCone swingLimit{ _rad( 10_deg ), _rad( 1_deg ), physx::PxSpring{ driveStiffness, driveDamping } };//PX_MAX_F32, PX_MAX_F32 } };
	swingLimit.restitution = 1.0f;
	m_D6Joint.setSwingLimit( swingLimit );

	m_D6Joint.setDrive( physx::PxD6Drive::eSWING, physx::PxD6JointDrive{ driveStiffness, driveDamping, driveForceLimit, false } );
	m_D6Joint.setDrivePosition( PoseFrom( poseParent.RotateNor( pi/2 ) ) );


	m_D6Joint.setConstraintFlag( physx::PxConstraintFlag::eCOLLISION_ENABLED, true );
	m_D6Joint.setConstraintFlag( physx::PxConstraintFlag::eVISUALIZATION, true );
}

PhysX_Swivel_Imp::~PhysX_Swivel_Imp() noexcept
{
	m_D6Joint.release();
}

spat::Frame<Length,One> PhysX_Swivel_Imp::LocalPoseA() const noexcept{
	return PhysX_Joint_Imp<Swivel>::LocalPoseA().RotateNor( -pi/2 );
}

spat::Frame<Length,One> PhysX_Swivel_Imp::LocalPoseB() const noexcept{
	return PhysX_Joint_Imp<Swivel>::LocalPoseB().RotateNor( -pi/2 );
}

spat::VectorBundle<Length,One> PhysX_Swivel_Imp::LocalAxisA() const noexcept
{
	spat::Frame<Length,One> localPoseA = LocalPoseA();
	return { localPoseA.P, localPoseA.B };
}

spat::VectorBundle<Length,One> PhysX_Swivel_Imp::LocalAxisB() const noexcept
{
	spat::Frame<Length,One> localPoseB = LocalPoseB();
	return { localPoseB.P, localPoseB.B };
}

Angle PhysX_Swivel_Imp::GetTurnAngle() const noexcept{
	return _rad(m_D6Joint.getTwistAngle());
}

Angle PhysX_Swivel_Imp::GetBendAngle() const noexcept{
	return _rad( m_D6Joint.getSwingZAngle() );
}

Angle PhysX_Swivel_Imp::GetPitchAngle() const noexcept{
	return _rad( m_D6Joint.getSwingYAngle() );
}

physx::PxJoint& PhysX_Swivel_Imp::BogieJoint() const noexcept{
	return m_D6Joint;
}

	/*
PhysX_Swivel_Imp::PhysX_Swivel_Imp( 
	physx::PxPhysics& physics, 
	Bogie* pBogieParent,
	spat::Frame<Length,One> poseParent,
	Bogie* pBogieChild,
	spat::Frame<Length,One> poseChild,
	Real engine_meters_per_unit )
	: PhysX_Joint_Imp<Swivel>{ pBogieParent, pBogieChild, engine_meters_per_unit }
	, m_RevoluteJoint{ *physx::PxRevoluteJointCreate( 
		physics,
		pBogieParent ? &Actor(pBogieParent->GetGestalt()) : nullptr, 
		PoseFrom( poseParent.RotateNor( pi/2 ) ), 
		pBogieChild ? &Actor(pBogieChild->GetGestalt()) : nullptr, 
		PoseFrom( poseChild.RotateNor( pi/2 ) ) ) }
{
	m_RevoluteJoint.setConstraintFlag( physx::PxConstraintFlag::eVISUALIZATION, true );

//	m_RevoluteJoint.setLimit( PxJointAngularLimitPair{,} );
}

PhysX_Swivel_Imp::~PhysX_Swivel_Imp() noexcept
{
	m_RevoluteJoint.release();
}

spat::Frame<Length,One> PhysX_Swivel_Imp::LocalPoseA() const noexcept{
	return PhysX_Joint_Imp<Swivel>::LocalPoseA().RotateNor( -pi/2 );
}

spat::Frame<Length,One> PhysX_Swivel_Imp::LocalPoseB() const noexcept{
	return PhysX_Joint_Imp<Swivel>::LocalPoseB().RotateNor( -pi/2 );
}

spat::VectorBundle<Length,One> PhysX_Swivel_Imp::LocalAxisA() const noexcept
{
	spat::Frame<Length,One> localPoseA = LocalPoseA();
	return { localPoseA.P, localPoseA.B };
}

spat::VectorBundle<Length,One> PhysX_Swivel_Imp::LocalAxisB() const noexcept
{
	spat::Frame<Length,One> localPoseB = LocalPoseB();
	return { localPoseB.P, localPoseB.B };
}

physx::PxJoint& PhysX_Swivel_Imp::Joint() const noexcept{
	return m_RevoluteJoint;
}
*/
}

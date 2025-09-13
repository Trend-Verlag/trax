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

#include "PhysX_Coupling_Imp.h"

#include "trax/rigid/trains/source/Bogie_Imp.h"
#include "trax/rigid/engines/PhysX/source/PhysX_Body_Imp.h"

namespace trax{

	using namespace spat;

PhysX_Coupling_Imp::PhysX_Coupling_Imp( 
	physx::PxPhysics& physics, 
	Bogie* pBogieParent, 
	const spat::Frame<Length,One>& poseParent,
	Bogie* pBogieChild,
	const spat::Frame<Length,One>& poseChild,
	Real engine_meters_per_unit  )
	: PhysX_Joint_Imp<Coupling>{ pBogieParent, pBogieChild, engine_meters_per_unit }
	, m_DistanceJoint{ *physx::PxDistanceJointCreate( 
		physics, 
		pBogieParent ? &Actor(pBogieParent->GetGestalt()) : nullptr, 
		PoseFrom( poseParent), 
		pBogieChild ? &Actor(pBogieChild->GetGestalt()) : nullptr, 
		PoseFrom( poseChild ) ) }
{
	m_DistanceJoint.setMaxDistance( static_cast<physx::PxReal>(_m(1_m)/engine_meters_per_unit) );
	m_DistanceJoint.setDistanceJointFlag( physx::PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true );
	m_DistanceJoint.setConstraintFlag( physx::PxConstraintFlag::eCOLLISION_ENABLED, true );
	m_DistanceJoint.setConstraintFlag( physx::PxConstraintFlag::eVISUALIZATION, true );
}

PhysX_Coupling_Imp::~PhysX_Coupling_Imp() noexcept
{
	m_DistanceJoint.release();
}

std::shared_ptr<Bogie_Imp> PhysX_Coupling_Imp::GetCoupledBogie( const Bogie_Imp& toBogie ) const noexcept
{
	if( &toBogie == m_pBogieA )
		return BogieB();
	else if( &toBogie == m_pBogieB )
		return BogieA();

	return nullptr;
}

Length PhysX_Coupling_Imp::GetLength() const noexcept{
	return _m(m_DistanceJoint.getMaxDistance() * m_Engine_meters_per_unit);
}

void PhysX_Coupling_Imp::SetLength( Length length ) noexcept{
	m_DistanceJoint.setMaxDistance( static_cast<physx::PxReal>(_m(length) / m_Engine_meters_per_unit) );
}

Vector<Force> PhysX_Coupling_Imp::GetForce() const noexcept{
	physx::PxVec3 force, torque;
	m_DistanceJoint.getConstraint()->getForce( force, torque );
	return VecFrom<Force>(force * m_Engine_meters_per_unit);
}

physx::PxJoint& PhysX_Coupling_Imp::BogieJoint() const noexcept{
	return m_DistanceJoint;
}

}

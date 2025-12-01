//	trax track library
//	AD 2017 
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
// For further information, please contact: horstmann.marc@trendverlag.de

#include "PhysX_TrackJoint.h"

#include "PhysX_Adapter.h"
#include "PhysX_Body_Imp.h"


namespace trax{
	using namespace spat;
///////////////////////////////////////
PhysXTrackJointBase::PhysXTrackJointBase(
	physx::PxRigidBody* pMovingBody, 
	const physx::PxTransform& localFrameMovingBody, 
	physx::PxRigidBody* pTrackBody )
{
	if( pTrackBody == pMovingBody )
		throw std::runtime_error( "PhysXTrackJointBase: actors must be different" );

	m_Body[0] = pMovingBody;
	m_Body[1] = pTrackBody;

	Anchor( From<physx::PxReal>( localFrameMovingBody.getNormalized() ) );

	if( pMovingBody ){
		SceneLockRead lock{ m_Body[0]->getScene() };
		SetMovingBodyCOM( From<physx::PxReal>(pMovingBody->getCMassLocalPose()) );
		Precalculate( From<physx::PxReal>( m_Body[0]->getGlobalPose() ) );
	}
	if( pTrackBody ){
		SceneLockRead lock{ m_Body[1]->getScene() };
		SetTrackBodyCOM( From<physx::PxReal>(pTrackBody->getCMassLocalPose()) );
		SetTrackBody( From<physx::PxReal>( m_Body[1]->getGlobalPose() ) );
	}
}

bool PhysXTrackJointBase::SetValues( const TrackData& td ) noexcept{
	markDirty();

	if( !TrackJoint<physx::PxReal>::SetValues( td ) )
		return false;

	if( m_Body[0] ){
		SceneLockRead lock{ m_Body[0]->getScene() };
		if( !SetMovingBody( From<physx::PxReal>( m_Body[0]->getGlobalPose() ) ) )
			return false;
	}

	if( m_Body[1] ){
		SceneLockRead lock{ m_Body[1]->getScene() };
		if( !SetTrackBody( From<physx::PxReal>( m_Body[1]->getGlobalPose() ) ) )
			return false;
	}

	return true;
}

bool PhysXTrackJointBase::IsMovableBodySleeping() const noexcept
{
	if( m_Body[0] ){
		SceneLockRead lock{ m_Body[0]->getScene() };
		if( physx::PxRigidDynamic* pRD = m_Body[0]->is<physx::PxRigidDynamic>(); pRD )
			return pRD->isSleeping();
	}

	return true;
}

void PhysXTrackJointBase::WakeUp() const
{
    if( m_Body[0] ){
        trax::SceneLockWrite lock{ m_Body[0]->getScene() };
        if( physx::PxRigidDynamic* pRD = m_Body[0]->is<physx::PxRigidDynamic>(); pRD )
            pRD->wakeUp();
    }
    if( m_Body[1] ){
        trax::SceneLockWrite lock{ m_Body[1]->getScene() };
        if( physx::PxRigidDynamic* pRD = m_Body[1]->is<physx::PxRigidDynamic>(); pRD )
            pRD->wakeUp();
    }
}

void PhysXTrackJointBase::UpdateBodies()
{
	if( m_Body[0] ){
		SceneLockRead lock{ m_Body[0]->getScene() };
		Precalculate( From<physx::PxReal>( m_Body[0]->getGlobalPose() ) );
	}

	if( m_Body[1] ){
		SceneLockRead lock{ m_Body[1]->getScene() };
		SetTrackBody( From<physx::PxReal>( m_Body[1]->getGlobalPose() ) );
	}
}

void PhysXTrackJointBase::UpdateTrackBody( std::shared_ptr<const Body> pToBody )
{
	m_Body[1] = nullptr;

	if( std::shared_ptr<const PhysX_Body_Imp> pPhysXBody = std::dynamic_pointer_cast<const PhysX_Body_Imp>(pToBody) )
	{
		m_Body[1] = &pPhysXBody->Actor();
		UpdateBodies();
	}
}

void* PhysXTrackJointBase::prepareData() noexcept{
	return static_cast<TrackJoint<physx::PxReal>*>(this);
}

bool PhysXTrackJointBase::updatePvdProperties(
	physx::pvdsdk::PvdDataStream& /*pvdConnection*/,
	const physx::PxConstraint* /*c*/,
	physx::PxPvdUpdateType::Enum /*updateType*/ ) const noexcept
{
	return true;
}

void PhysXTrackJointBase::onConstraintRelease() noexcept{
	delete this;
}

void PhysXTrackJointBase::onComShift( physx::PxU32 actor ){
	if( actor == 0 )
		SetMovingBodyCOM( From<physx::PxReal>(m_Body[actor]->getCMassLocalPose()) );
	if( actor == 1 )
		SetTrackBodyCOM( From<physx::PxReal>(m_Body[actor]->getCMassLocalPose()) );

	markDirty();
}

void PhysXTrackJointBase::onOriginShift( const physx::PxVec3& shift ) noexcept
{
	// move all global positions by shift ...
	F.P += VecFrom<physx::PxReal>( shift );
	wF.P += VecFrom<physx::PxReal>( shift );
}

void* PhysXTrackJointBase::getExternalReference( physx::PxU32& typeID ) noexcept{
	typeID = 0;
	return this;
}

physx::PxBase* PhysXTrackJointBase::getSerializable() noexcept{
	return nullptr;
}

physx::PxConstraintSolverPrep PhysXTrackJointBase::getPrep() const noexcept{
	return solverPrep;
}

const void* PhysXTrackJointBase::getConstantBlock() const noexcept{
	return static_cast<const TrackJoint<physx::PxReal>*>(this);
}

#if (PX_PHYSICS_VERSION_MAJOR >= 5)
physx::PxU32 PhysXTrackJointBase::solverPrep(physx::Px1DConstraint* constraints,
	physx::PxVec3p& body0WorldOffset,
	physx::PxU32 maxConstraints,
	physx::PxConstraintInvMassScale&,
	const void* constantBlock,
	const physx::PxTransform& bA2w,
	const physx::PxTransform& bB2w,
	bool useExtendedLimits,
	physx::PxVec3p& cAtW,
	physx::PxVec3p& cBtW )
#else
physx::PxU32 PhysXTrackJointBase::solverPrep(
	physx::Px1DConstraint* constraints,
	physx::PxVec3& body0WorldOffset,
	physx::PxU32 maxConstraints,
	physx::PxConstraintInvMassScale&,
	const void* constantBlock,
	const physx::PxTransform& bA2w,
	const physx::PxTransform& bB2w
#ifndef TRAX_WITH_PHYSX_3_4	
	,
	bool /*useExtendedLimits*/,
	physx::PxVec3& cAtW,
	physx::PxVec3& cBtW )
#else
	)
#endif
#endif
{
	PX_UNUSED(maxConstraints);
	PX_UNUSED(bA2w);
	PX_UNUSED(bB2w);
#if (PX_PHYSICS_VERSION_MAJOR >= 5)
	PX_UNUSED(useExtendedLimits);
#endif

	const TrackJoint<physx::PxReal>& data = *reinterpret_cast<const TrackJoint<physx::PxReal>*>(constantBlock);

	body0WorldOffset = From( data.MovingBodyGlobalCOM2AnchorOffset() );

#ifndef TRAX_WITH_PHYSX_3_4	
	cAtW = From( data.GlobalAnchor().P );
	cBtW = From( data.wF.P );
#endif

	unsigned short dim = 0;
	for( ; dim < data.DimensionsTotal(); ++dim ){
		data.Jacobian( dim,
					*reinterpret_cast<Vector<physx::PxReal>*>(&constraints[dim].linear0.x), 
					*reinterpret_cast<Vector<physx::PxReal>*>(&constraints[dim].angular0.x), 
					*reinterpret_cast<Vector<physx::PxReal>*>(&constraints[dim].linear1.x), 
					*reinterpret_cast<Vector<physx::PxReal>*>(&constraints[dim].angular1.x), 
					constraints[dim].velocityTarget,
					constraints[dim].geometricError,
					constraints[dim].minImpulse,
					constraints[dim].maxImpulse );

		constraints[dim].linear1 *= -1;
		constraints[dim].angular1 *= -1;
		constraints[dim].geometricError *= -1;
		constraints[dim].minImpulse *= static_cast<physx::PxReal>(_s(data.GetTimeStep())); //jacobian gives forces, 
		constraints[dim].maxImpulse *= static_cast<physx::PxReal>(_s(data.GetTimeStep())); //and dp/dt == m*dv/dt == m*a = F
		constraints[dim].solveHint = 0;
		constraints[dim].flags = physx::Px1DConstraintFlag::eOUTPUT_FORCE;

		if( data.IsAngularConstraint( dim ) )
			constraints[dim].flags |= physx::Px1DConstraintFlag::eANGULAR_CONSTRAINT;
		if( data.IsDriveConstraint( dim ) ){
#if PX_PHYSICS_VERSION_MAJOR < 5
			constraints[dim].flags |= physx::Px1DConstraintFlag::eDRIVE_ROW;
#endif
			if( data.IsConstraintLimited( dim ) )
				constraints[dim].flags |= physx::Px1DConstraintFlag::eHAS_DRIVE_LIMIT;
		}
	}

	return data.DimensionsTotal();
}
///////////////////////////////////////
physx::PxConstraintShaderTable PhysXTrackJoint::s_ShaderTable = { 
	&PhysXTrackJointBase::solverPrep, 
#if PX_PHYSICS_VERSION_MAJOR < 5
	&PhysXTrackJoint::project, 
#endif
	&PhysXTrackJoint::visualize,
	physx::PxConstraintFlag::Enum(0) };

PhysXTrackJoint::PhysXTrackJoint(
	physx::PxPhysics& physics,
	physx::PxRigidBody* pMovingBody, const physx::PxTransform& localFrameMovingBody,
	physx::PxRigidBody* pTrackBody )
	: PhysXTrackJointBase{ pMovingBody, localFrameMovingBody, pTrackBody }
{
	if( (pMovingBody && !pMovingBody->is<physx::PxRigidStatic>()) ||
		(pTrackBody && !pTrackBody->is<physx::PxRigidStatic>()) )
	{
		SceneLockRead lock{ pMovingBody ? pMovingBody->getScene() : pTrackBody->getScene() };
		m_pConstraint = physics.createConstraint(pMovingBody, pTrackBody, *this, s_ShaderTable, sizeof(TrackJoint<physx::PxReal>));
	}
	else
		throw std::runtime_error( "PhysXTrackJoint: at least one actor must be non-static" );
}

void PhysXTrackJoint::release(){
	if( m_pConstraint ){
		SceneLockWrite lock{ m_pConstraint->getScene() };
		physx::PxConstraint* pConstraint = m_pConstraint;
		m_pConstraint = nullptr;
		pConstraint->release(); //might delete this, so no further access to this object after this call
	}
}

void* PhysXTrackJoint::getExternalReference( physx::PxU32& typeID ) noexcept{
	typeID = TYPE_ID;
	return this;
}

void PhysXTrackJoint::visualize(
	physx::PxConstraintVisualizer& viz,
	const void* constantBlock,
	const physx::PxTransform& body0Transform,
	const physx::PxTransform& body1Transform,
	physx::PxU32 flags )
{
	PX_UNUSED(flags);
	PX_UNUSED(body0Transform);
	PX_UNUSED(body1Transform);

	const TrackJoint<physx::PxReal>& data = *reinterpret_cast<const TrackJoint<physx::PxReal>*>(constantBlock);
	viz.visualizeJointFrames( PoseFrom( data.wF ), PoseFrom( data.GlobalAnchor() ) );
//	viz.visualizeLine( From( data.GlobalAnchor().P ), From( Origin3D<Length> ), 0x0000FF );
}

void PhysXTrackJoint::project(
	const void* constantBlock,
	physx::PxTransform& bodyAToWorld,
	physx::PxTransform& bodyBToWorld,
	bool projectToA )
{
	PX_UNUSED(constantBlock);
	PX_UNUSED(bodyAToWorld);
	PX_UNUSED(bodyBToWorld);
	PX_UNUSED(projectToA);
}

}
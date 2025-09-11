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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/rigid/TrackJoint.h"

#include "PhysX_Imp.h"

namespace trax{

	/// \brief Basic implementation of a PhysX track joint.
	///
	/// Implements physx::PxConstraintConnector by means of TrackJoint<physx::PxReal>,
	/// but does not create the joint. Gets deleted by onConstraintRelease() callback
	class PhysXTrackJointBase : public TrackJoint<physx::PxReal>,
								public physx::PxConstraintConnector
	{
	public:
		PhysXTrackJointBase(	physx::PxRigidBody* pMovingBody, 
								const physx::PxTransform& localFrameMovingBody,
								physx::PxRigidBody* pTrackBody );

		bool SetValues( const TrackData& td ) noexcept override;

		bool IsMovableBodySleeping() const noexcept override;

		void WakeUp() const override;

		void UpdateBodies() override;

		void UpdateTrackBody( std::shared_ptr<const Body> pToBody ) override;

		// Inherited via PxConstraintConnector:
		void* prepareData() noexcept override;

		bool updatePvdProperties( 
			physx::pvdsdk::PvdDataStream& pvdConnection, 
			const physx::PxConstraint* c, 
			physx::PxPvdUpdateType::Enum updateType ) const noexcept override;

#if (PX_PHYSICS_VERSION_MAJOR >= 5)
		void updateOmniPvdProperties() const noexcept override
		{}
#endif
		void onConstraintRelease() noexcept override;

		void onComShift( physx::PxU32 actor ) override;

		void onOriginShift( const physx::PxVec3& shift ) noexcept override;

		void* getExternalReference( physx::PxU32& typeID ) noexcept override;

		physx::PxBase* getSerializable() noexcept override;

		physx::PxConstraintSolverPrep getPrep() const noexcept override;

		const void* getConstantBlock() const noexcept override;
		// Inherited via PxConstraintConnector.
	public:
#if (PX_PHYSICS_VERSION_MAJOR >= 5)
		static physx::PxU32 solverPrep(physx::Px1DConstraint* constraints,
			physx::PxVec3p& body0WorldOffset,
			physx::PxU32 maxConstraints,
			physx::PxConstraintInvMassScale&,
			const void* constantBlock,
			const physx::PxTransform& bA2w,
			const physx::PxTransform& bB2w,
			bool useExtendedLimits,
			physx::PxVec3p& cAtW,
			physx::PxVec3p& cBtW );
#else
		static physx::PxU32 solverPrep(physx::Px1DConstraint* constraints,
			physx::PxVec3& body0WorldOffset,
			physx::PxU32 maxConstraints,
			physx::PxConstraintInvMassScale&,
			const void* constantBlock,
			const physx::PxTransform& bA2w,
			const physx::PxTransform& bB2w
#ifndef TRAX_WITH_PHYSX_3_4	
			,
			bool useExtendedLimits,
			physx::PxVec3& cAtW,
			physx::PxVec3& cBtW);
#else
			);
#endif
#endif
		physx::PxRigidBody* m_Body[2];
	};


	/// \brief A PhysX track joint.
	///
	/// Joint gets created by constructor.
	class PhysXTrackJoint : public PhysXTrackJointBase{
	public:
		static const physx::PxU32 TYPE_ID = physx::PxConcreteType::eFIRST_USER_EXTENSION;

		PhysXTrackJoint( physx::PxPhysics& physics, 
						 physx::PxRigidBody* pMovingBody, 
						 const physx::PxTransform& localFrameMovingBody,
						 physx::PxRigidBody* pTrackBody );

		// Inherited via TrackJoint<physx::PxReal>
		void release() override; //deletes this...

		// Inherited via PxConstraintConnector
		void* getExternalReference(physx::PxU32 & typeID) noexcept override;

	protected:

		void markDirty() noexcept override
		{ 
			m_pConstraint->markDirty();
		}

	private:
		static void	visualize(physx::PxConstraintVisualizer& viz,
							  const void* constantBlock,
							  const physx::PxTransform&	body0Transform,
							  const physx::PxTransform&	body1Transform,
							  physx::PxU32 flags);

		static void project(const void* constantBlock,
							physx::PxTransform& bodyAToWorld,
							physx::PxTransform& bodyBToWorld,
							bool projectToA);


		physx::PxConstraint* m_pConstraint;
		static physx::PxConstraintShaderTable s_ShaderTable;
	};

}
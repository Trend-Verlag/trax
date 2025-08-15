// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2017 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#pragma once

#include "trax/TrackJoint.h"

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
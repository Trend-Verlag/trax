// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#include "PhysX_Gestalt_Imp.h"
#include "PhysX_GeomBase_Imp.h"
#include "PhysX_Adapter.h"

#include "common/NarrowCast.h"

namespace trax{
///////////////////////////////////////
PhysX_Gestalt_ImpBase::PhysX_Gestalt_ImpBase( physx::PxRigidDynamic& actor, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept
	: PhysX_Body_ImpBase	{ actor, engine_meters_per_unit, engine_kilograms_per_unit }
	, PhysX_Shape_ImpBase	{ actor, engine_meters_per_unit }
{
}

void PhysX_Gestalt_ImpBase::SetName( const char* name ) noexcept{
	PhysX_Shape_ImpBase::SetName( name );
}

const char* PhysX_Gestalt_ImpBase::GetName() const noexcept{
	return PhysX_Shape_ImpBase::GetName();
}

void PhysX_Gestalt_ImpBase::EnableSimulation( bool enable ) noexcept{
	PhysX_Body_ImpBase::EnableSimulation( enable );
}

bool PhysX_Gestalt_ImpBase::IsSimulationEnabled() const noexcept{
	return PhysX_Body_ImpBase::IsSimulationEnabled();
}

void PhysX_Gestalt_ImpBase::SetFrame( const spat::Frame<Length,One>& frame ) noexcept{
	PhysX_Body_ImpBase::SetFrame( frame );
}

void PhysX_Gestalt_ImpBase::GetFrame( spat::Frame<Length,One>& frame ) const noexcept{
	PhysX_Body_ImpBase::GetFrame( frame );
}

std::unique_ptr<Geom> PhysX_Gestalt_ImpBase::Remove( int idx ) noexcept
{
	if( PhysX_GeomBase_Imp* pPhysXGeom = dynamic_cast<PhysX_GeomBase_Imp*>(&Get(idx)); pPhysXGeom )
		pPhysXGeom->OnDetach();
	
	return Gestalt_Imp::Remove(idx);
}

void PhysX_Gestalt_ImpBase::AddMassProperties(
	Mass mass,
	const spat::Frame<Length,One>& massLocalPose,
	const spat::SquareMatrix<MomentOfInertia,3>& inertiaTensor ) noexcept
{
	assert( mass >= 0_kg );
	assert( massLocalPose.IsOrthoNormal() );
	assert( inertiaTensor(0,0) >= 0_kgm2 );
	assert( inertiaTensor(1,1) >= 0_kgm2 );
	assert( inertiaTensor(2,2) >= 0_kgm2 );
	assert( inertiaTensor.IsSymmetric() );

	// --- Build current(body) mass properties in ACTOR LOCAL space ---
	const physx::PxReal bodyMass = Actor().getMass();

	const physx::PxTransform bodyCM = Actor().getCMassLocalPose(); // actor -> COM frame
	const physx::PxMat33 Rbody(bodyCM.q);

	// full inertia about COM, expressed in actor-local axes
	const physx::PxMat33 Dbody = physx::PxMat33::createDiagonal( Actor().getMassSpaceInertiaTensor() );
	const physx::PxMat33 Ibody_actor = Rbody * Dbody * Rbody.getTranspose();

	physx::PxMassProperties bodyProps{ bodyMass, Ibody_actor, bodyCM.p };

	// --- Build additional mass properties in ACTOR LOCAL space ---
	const physx::PxReal addMass = static_cast<physx::PxReal>(_kg(mass) / EngineKilogramsPerUnit());

	// principal moments are given in the additional mass's local principal axes
	const physx::PxMat33 Dadd = From( inertiaTensor, EngineMetersPerUnit(), EngineKilogramsPerUnit() );

	// massLocalPose is (actor -> additional-mass frame)
	const physx::PxTransform addPose = PoseFrom( massLocalPose, EngineMetersPerUnit() );
	const physx::PxMat33 Radd(addPose.q);

	// rotate inertia into actor-local axes (still about the additional mass COM)
	const physx::PxMat33 Iadd_actor = Radd * Dadd * Radd.getTranspose();

	physx::PxMassProperties addProps{ addMass, Iadd_actor, addPose.p };

	// --- Sum them (both defined in actor-local reference frame) ---
	physx::PxMassProperties props[2]{ bodyProps, addProps };
	physx::PxTransform poses[2]{ physx::PxTransform(physx::PxIdentity), physx::PxTransform(physx::PxIdentity) };

	physx::PxMassProperties total = physx::PxMassProperties::sum( props, poses, 2 );

	// Convert full inertia tensor to (diag + massFrame) expected by rigid body API
	physx::PxQuat massFrame;
	physx::PxVec3 massSpaceInertia = physx::PxMassProperties::getMassSpaceInertia(total.inertiaTensor, massFrame);

	SceneLockWrite lock{ Actor().getScene() };
	Actor().setMass( total.mass );
	Actor().setMassSpaceInertiaTensor( massSpaceInertia );
	Actor().setCMassLocalPose( physx::PxTransform{ total.centerOfMass, massFrame } );
}
///////////////////////////////////////
PhysX_Gestalt_Imp::PhysX_Gestalt_Imp( physx::PxScene& scene, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept
	: PhysX_Gestalt_ImpBase	{ *scene.getPhysics().createRigidDynamic( physx::PxTransform{physx::PxIdentity} ), engine_meters_per_unit, engine_kilograms_per_unit }
{
	{
		SceneLockWrite lock{ &scene };
		scene.addActor( Actor() );
	}
}

PhysX_Gestalt_Imp::~PhysX_Gestalt_Imp() noexcept
{
	SceneLockWrite lock{ Actor().getScene() };
	Actor().release();
}
//
//const char* PhysX_Gestalt_Imp::TypeName() const noexcept{
//	return "PhysXGestalt";
//}
///////////////////////////////////////
}
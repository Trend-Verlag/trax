// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#include "PhysX_Shape_Imp.h"
#include "PhysX_GeomBase_Imp.h"
#include "PhysX_Adapter.h"

namespace trax{
///////////////////////////////////////
PhysX_Shape_ImpBase::PhysX_Shape_ImpBase( physx::PxRigidActor& actor, Real engine_meters_per_unit ) noexcept
	: m_Actor				{ actor }
	, m_EngineMetersPerUnit	{ engine_meters_per_unit }
{
}

void PhysX_Shape_ImpBase::SetName( const char* name ) noexcept{
	Shape_ImpBase::SetName( name );
	SceneLockWrite lock{ Actor().getScene() };
	Actor().setName( Shape_ImpBase::GetName() );
}

void PhysX_Shape_ImpBase::SetFrame( const spat::Frame<Length, One>& frame ) noexcept{
	SceneLockWrite lock{ Actor().getScene() };
	assert( frame.IsOrthoNormal() ); // No scaling works by PhysX
	Actor().setGlobalPose( PoseFrom( frame, m_EngineMetersPerUnit ) );
}

void PhysX_Shape_ImpBase::GetFrame( spat::Frame<Length, One>& frame ) const noexcept{
	SceneLockRead lock{ Actor().getScene() };
	frame = From( Actor().getGlobalPose(), m_EngineMetersPerUnit );
}

void PhysX_Shape_ImpBase::EnableSimulation( bool enable ) noexcept{
	SceneLockWrite lock{ Actor().getScene() };
	Actor().setActorFlag( physx::PxActorFlag::eDISABLE_SIMULATION, !enable );
}

bool PhysX_Shape_ImpBase::IsSimulationEnabled() const noexcept{
	SceneLockRead lock{ Actor().getScene() };
	return !Actor().getActorFlags().isSet( physx::PxActorFlag::eDISABLE_SIMULATION );
}
///////////////////////////////////////
PhysX_Shape_Imp::PhysX_Shape_Imp(physx::PxScene& scene, Real engine_meters_per_unit ) noexcept
	: PhysX_Shape_ImpBase{ *scene.getPhysics().createRigidStatic( physx::PxTransform{physx::PxIdentity} ), engine_meters_per_unit }
{	
	{
		SceneLockWrite lock{ &scene };
		scene.addActor( Actor() );
	}
}

PhysX_Shape_Imp::~PhysX_Shape_Imp() noexcept
{
	SceneLockWrite lock{ Actor().getScene() };
	Actor().release();
}
///////////////////////////////////////
} // namespace trax
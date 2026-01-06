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

void PhysX_Gestalt_ImpBase::DoCalculateMassProperties()
// Recalculates mass properties from attached shapes.
// 
{
	std::vector<physx::PxMassProperties> shapeMasses;
	std::vector<physx::PxTransform> shapeTransforms;

	for( int idx = 0; idx < Count(); ++idx ){
		if( GeomMass( idx ) == 0_kg )
			continue;	// Zero mass shapes are not considered for mass calculation.

		if( const PhysX_GeomBase_Imp* pPhysXGeom = dynamic_cast<const PhysX_GeomBase_Imp*>(&Get(idx)); pPhysXGeom ){
			shapeMasses.push_back( physx::PxMassProperties{ pPhysXGeom->Geometry() } );
			physx::PxReal mass = static_cast<physx::PxReal>(_kg(GeomMass( idx ))/EngineKilogramsPerUnit());
			shapeMasses.back().inertiaTensor *= mass / shapeMasses.back().mass;
			shapeMasses.back().mass = mass;

			Frame<Length,One> frame;
			pPhysXGeom->GetFrame( frame );
			shapeTransforms.push_back( PoseFrom( frame ) );
		}
	}

	if( !shapeMasses.empty() ){
		physx::PxMassProperties massProp = physx::PxMassProperties::sum( 
			shapeMasses.data(), 
			shapeTransforms.data(), 
			common::narrow_cast<physx::PxU32>(shapeMasses.size()) );

		physx::PxQuat massFrame;
		physx::PxVec3 massSpaceInertia = physx::PxMassProperties::getMassSpaceInertia( massProp.inertiaTensor, massFrame );

		SceneLockWrite lock{ Actor().getScene() };
		Actor().setMass( massProp.mass );
		Actor().setMassSpaceInertiaTensor( massSpaceInertia );
		Actor().setCMassLocalPose( physx::PxTransform{ massProp.centerOfMass, massFrame } );
	}
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
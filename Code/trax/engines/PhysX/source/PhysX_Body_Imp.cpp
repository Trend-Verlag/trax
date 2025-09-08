//	trax track library
//	AD 2023 
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

#include "trax/engines/PhysX/PhysX_Body_Imp.h"
#include "trax/engines/PhysX/PhysX_Adapter.h"

namespace trax{
	using namespace spat;

///////////////////////////////////////
PhysX_Body_ImpBase::PhysX_Body_ImpBase( 
	physx::PxRigidDynamic& actor, 
	Real engine_meters_per_unit, 
	Real engine_kilograms_per_unit ) noexcept
	:	m_Actor					{ actor },
		m_EngineMetersPerUnit	{ engine_meters_per_unit },
		m_EngineKilogramsPerUnit{ engine_kilograms_per_unit }
{
#ifdef _DEBUG
	Actor().setActorFlag( physx::PxActorFlag::eVISUALIZATION, true );
#endif
}

PhysX_Body_ImpBase::~PhysX_Body_ImpBase() noexcept{
}

void PhysX_Body_ImpBase::SetName( const char* name ) noexcept{
	SceneLockWrite lock{ Actor().getScene() };
	Actor().setName( name );
}

const char* PhysX_Body_ImpBase::GetName() const noexcept{
	SceneLockRead lock{ Actor().getScene() };
	return Actor().getName();
}

void PhysX_Body_ImpBase::SetFrame( const spat::Frame<Length,One>& frame ) noexcept{
	SceneLockWrite lock{ Actor().getScene() };
	assert( frame.IsOrthoNormal( epsilon__angle ) ); // No scaling works by PhysX
	Actor().setGlobalPose( PoseFrom( frame, m_EngineMetersPerUnit ) );
}

void PhysX_Body_ImpBase::GetFrame( spat::Frame<Length,One>& frame ) const noexcept{
	SceneLockRead lock{ Actor().getScene() };
	frame = From( Actor().getGlobalPose(), m_EngineMetersPerUnit );
}

void PhysX_Body_ImpBase::WakeUp() noexcept{
	SceneLockWrite lock{ Actor().getScene() };
	Actor().wakeUp();
}

bool PhysX_Body_ImpBase::IsSleeping() const noexcept{
	SceneLockRead lock{ Actor().getScene() };
	return Actor().isSleeping();
}

void PhysX_Body_ImpBase::SetSleepDelay( Time threshold )
{
	SceneLockWrite lock{ Actor().getScene() };
	Actor().setSleepThreshold( _s(threshold) );
}

Time PhysX_Body_ImpBase::GetSleepDelay() const noexcept
{
	SceneLockRead lock{ Actor().getScene() };
	return _s(Actor().getSleepThreshold());
}

void PhysX_Body_ImpBase::SetSleepThreshold( decltype(Velocity{}*Velocity{}) threshold )
{
	SceneLockWrite lock{ Actor().getScene() };
	Actor().setSleepThreshold( _m2Is2(threshold) / m_EngineMetersPerUnit / m_EngineMetersPerUnit );
}

decltype(Velocity{}*Velocity{}) PhysX_Body_ImpBase::GetSleepThreshold() const noexcept
{
	SceneLockRead lock{ Actor().getScene() };
	return _m2Is2( Actor().getSleepThreshold() * m_EngineMetersPerUnit * m_EngineMetersPerUnit );
}

void PhysX_Body_ImpBase::SetMass( Mass mass ){
	Body_Imp::SetMass(mass);

	SquareMatrix<Area,3> iTensor{ InertiaTensor() };

	SceneLockWrite lock{ Actor().getScene() };
	Actor().setMass( static_cast<physx::PxReal>(_kg(mass)/m_EngineKilogramsPerUnit) );

	InertiaTensor( iTensor );
}

Mass PhysX_Body_ImpBase::GetMass() const noexcept{
	SceneLockRead lock{ Actor().getScene() };
	return _kg(m_EngineKilogramsPerUnit*Actor().getMass());
}

void PhysX_Body_ImpBase::CenterOfMass( const Position<Length>& pos ) noexcept{
	//keep rotational transformations, the inertia tensor will only change
	//if the mass space is rotated. 
	SceneLockWrite lock{ Actor().getScene() };
	Frame<Length,One> pose{ From(Actor().getCMassLocalPose(),m_EngineMetersPerUnit) };
	pose.P = pos;
	Actor().setCMassLocalPose( PoseFrom(pose,m_EngineMetersPerUnit) );
}

Position<Length> PhysX_Body_ImpBase::CenterOfMass() const noexcept{
	SceneLockRead lock{ Actor().getScene() };
	return PosFrom( Actor().getCMassLocalPose(), m_EngineMetersPerUnit );
}

void PhysX_Body_ImpBase::InertiaTensor( const SquareMatrix<Area,3>& specificInertiaTensor ){
	if( !specificInertiaTensor.IsDiagonal() )
	// pick mass space transformation, so that tensor get diagonal...
	{
		assert( 0 );
		//SquareMatrix<Real,3> iTensor( specificInertiaTensor );

		//Frame<Real> pose( iTensor );
		//pose.Normalize();
		//pose.P = PosFrom<Real>(m_pActor->getCMassLocalPose());

		//pose.FromParent( iTensor );
		//assert( iTensor.IsDiagonal() );

		//m_pActor->setCMassLocalPose( PoseFrom(pose) );
		physx::PxVec3 diagonal{ 
			static_cast<float>(_m2(specificInertiaTensor(0,0))/m_EngineMetersPerUnit/m_EngineMetersPerUnit), 
			static_cast<float>(_m2(specificInertiaTensor(1,1))/m_EngineMetersPerUnit/m_EngineMetersPerUnit), 
			static_cast<float>(_m2(specificInertiaTensor(2,2))/m_EngineMetersPerUnit/m_EngineMetersPerUnit) };

		SceneLockWrite lock{ Actor().getScene() };
		diagonal *= Actor().getMass();
		Actor().setMassSpaceInertiaTensor( diagonal );
	}
	else{
		//Frame<Real> pose;
		//pose.Init();
		//pose.P = PosFrom<Real>(m_pActor->getCMassLocalPose());

		//m_pActor->setCMassLocalPose( PoseFrom(pose) );

		physx::PxVec3 diagonal{ 
			static_cast<float>(_m2(specificInertiaTensor(0,0))/m_EngineMetersPerUnit/m_EngineMetersPerUnit), 
			static_cast<float>(_m2(specificInertiaTensor(1,1))/m_EngineMetersPerUnit/m_EngineMetersPerUnit), 
			static_cast<float>(_m2(specificInertiaTensor(2,2))/m_EngineMetersPerUnit/m_EngineMetersPerUnit) };

		SceneLockWrite lock{ Actor().getScene() };
		diagonal *= Actor().getMass();
		Actor().setMassSpaceInertiaTensor( diagonal );
	}
}

SquareMatrix<Area,3> PhysX_Body_ImpBase::InertiaTensor() const{
	SceneLockRead lock{ Actor().getScene() };
	physx::PxVec3 diag{ Actor().getMassSpaceInertiaTensor() };
	SquareMatrix<Area,3> tensor;
	tensor.SetIdentity();
	tensor(0,0) = _m2(diag.x/Actor().getMass()*m_EngineMetersPerUnit*m_EngineMetersPerUnit);
	tensor(1,1) = _m2(diag.y/Actor().getMass()*m_EngineMetersPerUnit*m_EngineMetersPerUnit);
	tensor(2,2) = _m2(diag.z/Actor().getMass()*m_EngineMetersPerUnit*m_EngineMetersPerUnit);

	//Frame<Real> pose(From<Real>(m_pActor->getCMassLocalPose()));
	//pose.ToParent(tensor);

	return tensor;
}

void PhysX_Body_ImpBase::SetLinearVelocity( const spat::Vector<Velocity>& v ){
	SceneLockWrite lock{ Actor().getScene() };
	Actor().setLinearVelocity( From( v, m_EngineMetersPerUnit ) );
}

void PhysX_Body_ImpBase::GetLinearVelocity( spat::Vector<Velocity>& v ) const{
	SceneLockRead lock{ Actor().getScene() };
	v = VelocityFrom( Actor().getLinearVelocity(), m_EngineMetersPerUnit );
}

void PhysX_Body_ImpBase::SetAngularVelocity( const spat::Vector<AngularVelocity>& w ){
	SceneLockWrite lock{ Actor().getScene() };
	Actor().setAngularVelocity( From(w) );
}

void PhysX_Body_ImpBase::GetAngularVelocity( spat::Vector<AngularVelocity>& w ) const{
	SceneLockRead lock{ Actor().getScene() };
	w = From<AngularVelocity>( Actor().getAngularVelocity() );
}
///////////////////////////////////////
PhysX_Body_Imp::PhysX_Body_Imp( physx::PxPhysics& physics, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept
	: PhysX_Body_ImpBase{ *physics.createRigidDynamic( physx::PxTransform{physx::PxIdentity} ), engine_meters_per_unit, engine_kilograms_per_unit }
{
}

PhysX_Body_Imp::PhysX_Body_Imp( physx::PxScene& scene, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept
	: PhysX_Body_ImpBase{ *scene.getPhysics().createRigidDynamic( physx::PxTransform{physx::PxIdentity} ), engine_meters_per_unit, engine_kilograms_per_unit }
{
	SceneLockWrite lock{ &scene };
	scene.addActor( Actor() );
}

PhysX_Body_Imp::PhysX_Body_Imp( physx::PxAggregate& aggregate, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept
	: PhysX_Body_ImpBase{ *aggregate.getScene()->getPhysics().createRigidDynamic( physx::PxTransform{physx::PxIdentity} ), engine_meters_per_unit, engine_kilograms_per_unit }
{
	SceneLockWrite lock{ aggregate.getScene() };
	aggregate.addActor( Actor() );
}

PhysX_Body_Imp::~PhysX_Body_Imp() noexcept
{
	SceneLockWrite lock{ Actor().getScene() };
	Actor().release();
}
///////////////////////////////////////
physx::PxRigidDynamic& Actor( const Body& body )
{
	return dynamic_cast<const PhysX_Body_ImpBase&>(body).Actor();
}
///////////////////////////////////////
}


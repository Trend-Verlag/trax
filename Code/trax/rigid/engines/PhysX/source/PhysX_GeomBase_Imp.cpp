//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2015 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#include "PhysX_GeomBase_Imp.h"

#include "PhysX_Imp.h"
#include "PhysX_Adapter.h"
#include "PhysX_Body_Imp.h"
#include "PhysX_Shape_Imp.h"


namespace trax{
///////////////////////////////////////
PhysX_GeomBase_Imp::PhysX_GeomBase_Imp( Real engineMetersPerUnit ) noexcept
	:	m_EngineMetersPerUnit	{ engineMetersPerUnit }
	,	m_pMaterial				{ nullptr }
	,	m_pShape				{ nullptr }
{
	m_Frame.Init();
}

PhysX_GeomBase_Imp::~PhysX_GeomBase_Imp(){
	//if( m_pShape )
	//	m_pShape->setName( nullptr );
}

void PhysX_GeomBase_Imp::SetName( const char* name ) noexcept{
	Geom_Imp::SetName( name );

	if( m_pShape ){
		physx::PxRigidActor* pActor = m_pShape->getActor();
		SceneLockWrite lock{ pActor ? pActor->getScene() : nullptr };
		m_pShape->setName( name ? nullptr : GetName() );
	}
}

void PhysX_GeomBase_Imp::SetFrame( const Frame<Length,One>& frame ) noexcept{
	m_Frame = frame;
	if( m_pShape ){
		physx::PxRigidActor* pActor = m_pShape->getActor();
		SceneLockWrite lock{ pActor ? pActor->getScene() : nullptr };
		m_pShape->setLocalPose( PoseFrom( Normalize(m_Frame).second ) );
	}
}

void PhysX_GeomBase_Imp::GetFrame( Frame<Length,One>& frame ) const noexcept{
	frame = m_Frame;
}

void PhysX_GeomBase_Imp::SetMaterial( const Material& material ) noexcept{
	Geom_Imp::SetMaterial( material );

	if( m_pMaterial ){
		m_pMaterial->setStaticFriction( static_cast<physx::PxReal>(GetMaterial().staticFriction) );
		m_pMaterial->setDynamicFriction( static_cast<physx::PxReal>(GetMaterial().dynamicFriction) );
		m_pMaterial->setRestitution( static_cast<physx::PxReal>(GetMaterial().restitution) );
	}

	SetFilter();
}

void PhysX_GeomBase_Imp::TypeFilter( Filters typeFlags ) noexcept{
	Geom_Imp::TypeFilter( typeFlags );

	SetFilter();
}

void PhysX_GeomBase_Imp::CollisionFilter( unsigned int collideWith ) noexcept{
	Geom_Imp::CollisionFilter(collideWith);

	SetFilter();
}

void PhysX_GeomBase_Imp::OnAttach( const Shape_ImpBase& shape )
{
	if( const PhysX_Shape_ImpBase* pPhysXShape = dynamic_cast<const PhysX_Shape_ImpBase*>(&shape); pPhysXShape )	
		OnAttach( pPhysXShape->Actor() );
}

void PhysX_GeomBase_Imp::OnAttach( physx::PxRigidActor& actor )
{
	SceneLockWrite lock{ actor.getScene() };

	m_pMaterial = actor.getScene()->getPhysics().createMaterial(	
		static_cast<physx::PxReal>(GetMaterial().staticFriction),
		static_cast<physx::PxReal>(GetMaterial().dynamicFriction),
		static_cast<physx::PxReal>(GetMaterial().restitution) );
	if( !m_pMaterial )
		throw std::runtime_error("trax::PhysX_GeomBase_Imp: physx::PxPhysics::createMaterial failed!");

	m_pShape = physx::PxRigidActorExt::createExclusiveShape( actor, Geometry(), *m_pMaterial );
	//m_pShape = actor.createShape( Geometry(), *m_pMaterial );
	if( !m_pShape )
		throw std::runtime_error("trax::PhysX_GeomBase_Imp: physx::PxRigidActorExt::createExclusiveShape failed!");

	m_pShape->setName( GetName() ? nullptr : GetName() );
	m_pShape->setLocalPose( PoseFrom( Normalize(m_Frame).second ) );

#ifdef _DEBUG
	m_pShape->setFlag( physx::PxShapeFlag::eVISUALIZATION, true );
#endif

	SetFilter();
}

void PhysX_GeomBase_Imp::OnDetach(){
	if( m_pMaterial ){
		m_pMaterial->release();
		m_pMaterial = nullptr;
	}

	if( m_pShape ){
		physx::PxRigidActor* pActor = m_pShape->getActor();
		SceneLockWrite lock{ pActor ? pActor->getScene() : nullptr };

		m_pShape->setName( nullptr );
		assert( m_pShape->getActor() );
		m_pShape->getActor()->detachShape( *m_pShape );
		m_pShape = nullptr;
	}
}

void PhysX_GeomBase_Imp::AdjustShapeGeometry(){
	if( Shape() ){
		Shape()->setGeometry( Geometry() );
		//isRigidBody()
		if( auto pRigidBody = Shape()->getActor()->is<physx::PxRigidBody>() )
			physx::PxRigidBodyExt::setMassAndUpdateInertia( *pRigidBody, pRigidBody->getMass() );
	}
}

void PhysX_GeomBase_Imp::SetFilter(){
	if( m_pShape ){
		physx::PxFilterData filterData;
		filterData.word0 = TypeFilter();
		filterData.word1 = CollisionFilter();
		filterData.word2 = static_cast<physx::PxU32>(GetMaterial().type);

		m_pShape->setSimulationFilterData(filterData);
		m_pShape->setQueryFilterData(filterData);

		if( IsTrigger(filterData.word0) ){
			m_pShape->setFlag( physx::PxShapeFlag::eSIMULATION_SHAPE, false );
			m_pShape->setFlag( physx::PxShapeFlag::eTRIGGER_SHAPE, true );
		}
		else{
			m_pShape->setFlag( physx::PxShapeFlag::eTRIGGER_SHAPE, false );
			m_pShape->setFlag( physx::PxShapeFlag::eSIMULATION_SHAPE, true );
		}
	}
}
///////////////////////////////////////
physx::PxQueryFlags::InternalType From( unsigned int filterFlags )
{
	physx::PxQueryFlags::InternalType retval = 0;
	if( filterFlags & Geom::Filters::fBogie )
		retval |= physx::PxQueryFlag::eDYNAMIC;
	if( filterFlags & Geom::Filters::fChildBogie )
		retval |= physx::PxQueryFlag::eDYNAMIC;
	if( filterFlags & Geom::Filters::fWheelset )
		retval |= physx::PxQueryFlag::eDYNAMIC;
	if( filterFlags & Geom::Filters::fCargo )
		retval |= physx::PxQueryFlag::eDYNAMIC;
	if( filterFlags & Geom::Filters::fBogieSpace )
		retval |= physx::PxQueryFlag::eDYNAMIC;
	if( filterFlags & Geom::Filters::fCrane )
		retval |= physx::PxQueryFlag::eDYNAMIC;
	if( filterFlags & Geom::Filters::fHook )
		retval |= physx::PxQueryFlag::eDYNAMIC;
	if( filterFlags & Geom::Filters::fCouplingNorth )
		retval |= physx::PxQueryFlag::eDYNAMIC;
	if( filterFlags & Geom::Filters::fCouplingSouth )
		retval |= physx::PxQueryFlag::eDYNAMIC;
	if( filterFlags & Geom::Filters::fCabin )
		retval |= physx::PxQueryFlag::eDYNAMIC;
	if( filterFlags & Geom::Filters::fBogieNoCabin )
		retval |= physx::PxQueryFlag::eDYNAMIC;
	if( filterFlags & Geom::Filters::fBuffer )
		retval |= physx::PxQueryFlag::eDYNAMIC;
	if( filterFlags & Geom::Filters::fMovableTrack )
		retval |= physx::PxQueryFlag::eDYNAMIC;

	if( filterFlags & Geom::Filters::fTerrain )
		retval |= physx::PxQueryFlag::eSTATIC;
	if( filterFlags & Geom::Filters::fConstruction )
		retval |= physx::PxQueryFlag::eSTATIC;
	if( filterFlags & Geom::Filters::fTrack )
		retval |= physx::PxQueryFlag::eSTATIC;

	return retval;
}
///////////////////////////////////////
}
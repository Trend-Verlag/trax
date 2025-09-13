// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2023 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/rigid/trains/source/BogieJoint.h"
#include "trax/rigid/trains/Bogie.h"
#include "trax/rigid/Gestalt.h"

#include "trax/rigid/engines/PhysX/source/PhysX_Adapter.h"


namespace trax{

	template<class Base>
	class PhysX_Joint_Imp : public Base{
	public:
		PhysX_Joint_Imp( 
			Bogie* pBogieA, 
			Bogie* pBogieB, 
			Real engine_meters_per_unit );

		// Joint interface:
		void SetName( const char* name ) noexcept override;

		const char* GetName() const noexcept override;

		std::shared_ptr<Bogie_Imp> BogieA() const noexcept override;

		spat::Frame<Length,One> LocalPoseA() const noexcept override;

		std::shared_ptr<Bogie_Imp> BogieB() const noexcept override;

		spat::Frame<Length,One> LocalPoseB() const noexcept override;

		void SetBogieA( Bogie_Imp* pBogieA ) override;

		void SetBogieB( Bogie_Imp* pBogieB ) override;

		// PhysX_Joint_Imp interface:
		virtual physx::PxJoint& BogieJoint() const noexcept = 0;
	protected:
		Bogie_Imp* m_pBogieA = nullptr;
		Bogie_Imp* m_pBogieB = nullptr;
		Real m_Engine_meters_per_unit = 1.0f;
	};


template<class Base>
PhysX_Joint_Imp<Base>::PhysX_Joint_Imp( 
	Bogie* pBogieA,
	Bogie* pBogieB, 
	Real engine_meters_per_unit )
	: m_pBogieA					{ dynamic_cast<Bogie_Imp*>(pBogieA) }
	, m_pBogieB					{ dynamic_cast<Bogie_Imp*>(pBogieB) }
	, m_Engine_meters_per_unit	{ engine_meters_per_unit }
{
}

template<class Base>
void PhysX_Joint_Imp<Base>::SetName( const char* name ) noexcept{
	BogieJoint().setName( name );
}

template<class Base>
const char* PhysX_Joint_Imp<Base>::GetName() const noexcept{
	return BogieJoint().getName();
}

template<class Base>
std::shared_ptr<Bogie_Imp> PhysX_Joint_Imp<Base>::BogieA() const noexcept{
	if( m_pBogieA )
		return std::dynamic_pointer_cast<Bogie_Imp>(m_pBogieA->This());
	else
		return nullptr;
}

template<class Base>
spat::Frame<Length,One> PhysX_Joint_Imp<Base>::LocalPoseA() const noexcept{
	return From(BogieJoint().getLocalPose( physx::PxJointActorIndex::eACTOR0 ), m_Engine_meters_per_unit );
}

template<class Base>
std::shared_ptr<Bogie_Imp> PhysX_Joint_Imp<Base>::BogieB() const noexcept{
	if( m_pBogieB )
		return std::dynamic_pointer_cast<Bogie_Imp>(m_pBogieB->This());
	else
		return nullptr;
}

template<class Base>
spat::Frame<Length,One> PhysX_Joint_Imp<Base>::LocalPoseB() const noexcept{
	return From(BogieJoint().getLocalPose( physx::PxJointActorIndex::eACTOR1 ), m_Engine_meters_per_unit );
}

template<class Base>
void PhysX_Joint_Imp<Base>::SetBogieA( Bogie_Imp* pBogieA )
{
	m_pBogieA = pBogieA;

	physx::PxRigidDynamic* pActorA = m_pBogieA ? &Actor( m_pBogieA->GetGestalt() ) : nullptr;
	physx::PxRigidDynamic* pActorB = m_pBogieB ? &Actor( m_pBogieB->GetGestalt() ) : nullptr;

	if( pActorA == nullptr && pActorB == nullptr )
		throw std::runtime_error( "Both actors can't be nullptr." );

	BogieJoint().setActors( pActorA, pActorB );
}

template<class Base>
void PhysX_Joint_Imp<Base>::SetBogieB( Bogie_Imp* pBogieB )
{
	m_pBogieB = pBogieB;

	physx::PxRigidDynamic* pActorA = m_pBogieA ? &Actor( m_pBogieA->GetGestalt() ) : nullptr;
	physx::PxRigidDynamic* pActorB = m_pBogieB ? &Actor( m_pBogieB->GetGestalt() ) : nullptr;

	if( pActorA == nullptr && pActorB == nullptr )
		throw std::runtime_error( "Both actors can't be nullptr." );

	BogieJoint().setActors( pActorA, pActorB );
}

}

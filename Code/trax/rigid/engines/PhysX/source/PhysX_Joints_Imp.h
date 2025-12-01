//	trax track library
//	AD 2025
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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/rigid/Body.h"
#include "trax/rigid/Joint.h"

#include "trax/rigid/engines/PhysX/source/PhysX_Adapter.h"

namespace trax{

	template<class Base>
	class PhysX_Joint_Imp : public Base{
	public:
		PhysX_Joint_Imp( 
			Body* pBodyA, 
			Body* pBodyB, 
			Real engine_meters_per_unit );

		// Joint interface:
		void SetName( const char* name ) noexcept override;

		const char* GetName() const noexcept override;

		Body* BodyA() const noexcept override;

		spat::Frame<Length,One> LocalPoseA() const noexcept override;

		Body* BodyB() const noexcept override;

		spat::Frame<Length,One> LocalPoseB() const noexcept override;

		void BodyA( Body* pBodyA ) override;

		void BodyB( Body* pBodyB ) override;

		// PhysX_Joint_Imp interface:
		virtual physx::PxJoint& BodyJoint() const noexcept = 0;
	protected:
		Body* m_pBodyA = nullptr;
		Body* m_pBodyB = nullptr;
		Real m_Engine_meters_per_unit = 1.0f;
	};


	class PhysX_HingeJoint_Imp : public PhysX_Joint_Imp<HingeJoint>{
	public:
		PhysX_HingeJoint_Imp(  
			physx::PxPhysics& physics, 
			Body* pBodyA,
			spat::Frame<Length,One> poseA,
			Body* pBodyB,
			spat::Frame<Length,One> poseB, 
			Real engine_meters_per_unit  );
		~PhysX_HingeJoint_Imp();


		// Joint:
		spat::Frame<Length,One> LocalPoseA() const noexcept override;

		spat::Frame<Length,One> LocalPoseB() const noexcept override;

		// HingeJoint:
		spat::VectorBundle<Length,One> LocalAxisA() const noexcept override;

		spat::VectorBundle<Length,One> LocalAxisB() const noexcept override;
		
		Angle GetTurnAngle() const noexcept override;

		Angle GetBendAngle() const noexcept override;

		Angle GetPitchAngle() const noexcept override;


		physx::PxJoint& BodyJoint() const noexcept override;
	private:
		physx::PxD6Joint& m_D6Joint;
	};


	class PhysX_SliderJoint_Imp : public PhysX_Joint_Imp<SliderJoint>{
	public:
		PhysX_SliderJoint_Imp(  
			physx::PxPhysics& physics, 
			Body* pBodyA,
			spat::Frame<Length,One> poseA,
			Body* pBodyB,
			spat::Frame<Length,One> poseB, 
			Real engine_meters_per_unit  );
		~PhysX_SliderJoint_Imp();

		Length GetPosition() const noexcept override;

		void SetPosition( Length position ) const noexcept override;

		physx::PxJoint& BodyJoint() const noexcept override;
	private:
		physx::PxD6Joint& m_D6Joint;
	};

	class PhysX_BallAndSocketJoint_Imp : public PhysX_Joint_Imp<BallAndSocketJoint>{
	public:
		PhysX_BallAndSocketJoint_Imp(  
			physx::PxPhysics& physics, 
			Body* pBodyA,
			spat::Frame<Length,One> poseA,
			Body* pBodyB,
			spat::Frame<Length,One> poseB, 
			Real engine_meters_per_unit  );
		~PhysX_BallAndSocketJoint_Imp();

		physx::PxJoint& BodyJoint() const noexcept override;
	private:
		physx::PxD6Joint& m_D6Joint;
	};

	class PhysX_DistanceJoint_Imp : public PhysX_Joint_Imp<DistanceJoint>{
	public:
		PhysX_DistanceJoint_Imp(  
			physx::PxPhysics& physics, 
			Body* pBodyA,
			spat::Frame<Length,One> poseA,
			Body* pBodyB,
			spat::Frame<Length,One> poseB, 
			Real engine_meters_per_unit  );
		~PhysX_DistanceJoint_Imp();

		Length GetDistance() const noexcept override;

		void SetDistance( Length distance ) const noexcept override;

		spat::Vector<Force> GetForce() const noexcept override;

		physx::PxJoint& BodyJoint() const noexcept override;
	private:
		physx::PxDistanceJoint& m_DistanceJoint;
	};


template<class Base>
PhysX_Joint_Imp<Base>::PhysX_Joint_Imp( 
	Body* pBodyA,
	Body* pBodyB, 
	Real engine_meters_per_unit )
	: m_pBodyA					{ pBodyA }
	, m_pBodyB					{ pBodyB }
	, m_Engine_meters_per_unit	{ engine_meters_per_unit }
{
}

template<class Base>
void PhysX_Joint_Imp<Base>::SetName( const char* name ) noexcept{
	BodyJoint().setName( name );
}

template<class Base>
const char* PhysX_Joint_Imp<Base>::GetName() const noexcept{
	return BodyJoint().getName();
}

template<class Base>
Body* PhysX_Joint_Imp<Base>::BodyA() const noexcept{
	return m_pBodyA;
}

template<class Base>
spat::Frame<Length,One> PhysX_Joint_Imp<Base>::LocalPoseA() const noexcept{
	return From(BodyJoint().getLocalPose( physx::PxJointActorIndex::eACTOR0 ), m_Engine_meters_per_unit );
}

template<class Base>
Body* PhysX_Joint_Imp<Base>::BodyB() const noexcept{
	return m_pBodyB;
}

template<class Base>
spat::Frame<Length,One> PhysX_Joint_Imp<Base>::LocalPoseB() const noexcept{
	return From(BodyJoint().getLocalPose( physx::PxJointActorIndex::eACTOR1 ), m_Engine_meters_per_unit );
}

template<class Base>
void PhysX_Joint_Imp<Base>::BodyA( Body* pBodyA )
{
	m_pBodyA = pBodyA;

	physx::PxRigidDynamic* pActorA = m_pBodyA ? &Actor( *m_pBodyA ) : nullptr;
	physx::PxRigidDynamic* pActorB = m_pBodyB ? &Actor( *m_pBodyB ) : nullptr;

	if( pActorA == nullptr && pActorB == nullptr )
		throw std::runtime_error( "Both actors can't be nullptr." );

	BodyJoint().setActors( pActorA, pActorB );
}

template<class Base>
void PhysX_Joint_Imp<Base>::BodyB( Body* pBodyB )
{
	m_pBodyB = pBodyB;

	physx::PxRigidDynamic* pActorA = m_pBodyA ? &Actor( *m_pBodyA ) : nullptr;
	physx::PxRigidDynamic* pActorB = m_pBodyB ? &Actor( *m_pBodyB ) : nullptr;

	if( pActorA == nullptr && pActorB == nullptr )
		throw std::runtime_error( "Both actors can't be nullptr." );

	BodyJoint().setActors( pActorA, pActorB );
}



}

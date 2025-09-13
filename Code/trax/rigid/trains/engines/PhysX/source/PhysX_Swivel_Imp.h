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

#include "trax/rigid/trains/source/Swivel.h"

#include "PhysX_Joint_Imp.h"

namespace trax{

	class PhysX_Swivel_Imp : public PhysX_Joint_Imp<Swivel>{
	public:
		PhysX_Swivel_Imp( 
			physx::PxPhysics& physics, 
			Bogie* pBogieParent,
			spat::Frame<Length,One> poseParent,
			Bogie* pBogieChild,
			spat::Frame<Length,One> poseChild, 
			Real engine_meters_per_unit );
		~PhysX_Swivel_Imp() noexcept;

		// Joint:
		spat::Frame<Length,One> LocalPoseA() const noexcept override;

		spat::Frame<Length,One> LocalPoseB() const noexcept override;

		// Swivel:
		spat::VectorBundle<Length,One> LocalAxisA() const noexcept override;

		spat::VectorBundle<Length,One> LocalAxisB() const noexcept override;
		
		Angle GetTurnAngle() const noexcept override;

		Angle GetBendAngle() const noexcept override;

		Angle GetPitchAngle() const noexcept override;


		physx::PxJoint& BogieJoint() const noexcept override;
	private:
		physx::PxD6Joint& m_D6Joint;
	};

	//class PhysX_Swivel_Imp : public PhysX_Joint_Imp<Swivel>{
	//public:
	//	PhysX_Swivel_Imp( 
	//		physx::PxPhysics& physics, 
	//		Bogie* pBogieParent,
	//		spat::Frame<Length,One> poseParent,
	//		Bogie* pBogieChild,
	//		spat::Frame<Length,One> poseChild, 
	//		Real engine_meters_per_unit );
	//	~PhysX_Swivel_Imp() noexcept;

	//	// Joint:
	//	spat::Frame<Length,One> LocalPoseA() const noexcept override;

	//	spat::Frame<Length,One> LocalPoseB() const noexcept override;

	//	// Swivel:
	//	spat::VectorBundle<Length,One> LocalAxisA() const noexcept override;

	//	spat::VectorBundle<Length,One> LocalAxisB() const noexcept override;


	//	physx::PxJoint& Joint() const noexcept override;
	//private:
	//	physx::PxRevoluteJoint& m_RevoluteJoint;
	//};
}


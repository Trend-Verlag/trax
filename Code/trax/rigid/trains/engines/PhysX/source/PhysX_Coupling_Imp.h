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
//							Peter Gabriel

#pragma once


#include "trax/rigid/trains/source/Coupling.h"

#include "PhysX_Joint_Imp.h"

namespace trax{

	class PhysX_Coupling_Imp : public PhysX_Joint_Imp<Coupling>{
	public:
		PhysX_Coupling_Imp( 
			physx::PxPhysics& physics, 
			Bogie* pBogieParent, 
			const spat::Frame<Length,One>& poseParent,
			Bogie* pBogieChild, 
			const spat::Frame<Length,One>& poseChild, 
			Real engine_meters_per_unit );
		~PhysX_Coupling_Imp() noexcept;

		std::shared_ptr<Bogie_Imp> GetCoupledBogie( const Bogie_Imp& toBogie ) const noexcept override;

		Length GetLength() const noexcept override;

		void SetLength( Length length ) noexcept override;

		spat::Vector<Force> GetForce() const noexcept override;

		physx::PxJoint& BogieJoint() const noexcept override;
	private:
		physx::PxDistanceJoint& m_DistanceJoint;
	};
}
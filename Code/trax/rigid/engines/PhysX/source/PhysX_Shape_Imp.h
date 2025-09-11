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

#pragma once

#include "trax/rigid/source/Shape_Imp.h"

#include "PhysX_Imp.h"

namespace trax{

	class PhysX_Shape_ImpBase : public virtual Shape_ImpBase{
	public:
		PhysX_Shape_ImpBase( physx::PxRigidActor& actor, Real engine_meters_per_unit ) noexcept;

		//const char*	TypeName() const noexcept override;


		// Shape:
		void SetName( const char* name ) noexcept override;

		const char* GetName() const noexcept override;


		// Shape:
		void SetFrame( const spat::Frame<Length,One>& frame ) noexcept override;

		void GetFrame( spat::Frame<Length,One>& frame ) const noexcept override;	

		inline physx::PxRigidActor& Actor() const noexcept{
			return m_Actor;
		}

	private:
		physx::PxRigidActor& m_Actor;
		const Real m_EngineMetersPerUnit;
	};


	class PhysX_Shape_Imp :	public PhysX_Shape_ImpBase{
	public:
		PhysX_Shape_Imp( physx::PxScene& scene, Real engine_meters_per_unit ) noexcept;
		~PhysX_Shape_Imp() noexcept;

	//	const char*	TypeName() const noexcept override;
	};
};

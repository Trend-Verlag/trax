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

#include "trax/rigid/source/Gestalt_Imp.h"

#include "PhysX_Body_Imp.h"
#include "PhysX_Shape_Imp.h"

namespace trax
{

	class PhysX_Gestalt_ImpBase :	public Gestalt_Imp,
									public PhysX_Body_ImpBase,
									public PhysX_Shape_ImpBase
	{
	public:
		PhysX_Gestalt_ImpBase( physx::PxRigidDynamic& actor, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept;

		//Body/Shape:
		void SetName( const char* name ) noexcept override;

		const char* GetName() const noexcept override;


		//Gestalt:
		void SetFrame( const spat::Frame<Length,One>& frame ) noexcept override;

		void GetFrame( spat::Frame<Length,One>& frame ) const noexcept override;
		

		//Shape:
		std::unique_ptr<Geom> Remove( int idx ) noexcept override;

		inline physx::PxRigidDynamic& Actor() const noexcept{
			return PhysX_Body_ImpBase::Actor();
		}

	protected:
		void AddMassProperties( Mass mass, const spat::Frame<Length,One>& massLocalPose, const spat::SquareMatrix<MomentOfInertia,3>& inertiaTensor ) noexcept override;
	};


	class PhysX_Gestalt_Imp : public PhysX_Gestalt_ImpBase
	{
	public:
		PhysX_Gestalt_Imp( physx::PxScene& scene, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept;
		~PhysX_Gestalt_Imp() noexcept;

		//const char*	TypeName() const noexcept override;
	};

}

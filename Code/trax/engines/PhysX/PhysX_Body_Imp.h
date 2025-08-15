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

#include "trax/source/Body_Imp.h"

#include "PhysX_Imp.h"

namespace trax
{
	class PhysX_Body_ImpBase : public Body_Imp{
	public:
		PhysX_Body_ImpBase( physx::PxRigidDynamic& actor, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept;
		~PhysX_Body_ImpBase() noexcept;


		// Body:
		void SetName( const char* name ) noexcept override;

		const char* GetName() const noexcept override;

		void SetFrame( const spat::Frame<Length,One>& frame ) noexcept override;

		void GetFrame( spat::Frame<Length,One>& frame ) const noexcept override;


		void WakeUp() noexcept override;

		bool IsSleeping() const noexcept override;

		void SetSleepDelay( Time threshold ) override;

		Time GetSleepDelay() const noexcept override;

		void SetSleepThreshold( decltype(Velocity{}*Velocity{}) threshold ) override;

		decltype(Velocity{}*Velocity{}) GetSleepThreshold() const noexcept override;


		void SetMass( Mass mass ) override;

		Mass GetMass() const noexcept override;

		void CenterOfMass( const spat::Position<Length>& pos ) noexcept override;

		spat::Position<Length> CenterOfMass() const noexcept override;

		void InertiaTensor( const spat::SquareMatrix<Area,3>& tensor ) override;

		spat::SquareMatrix<Area,3> InertiaTensor() const override;


		void SetLinearVelocity( const spat::Vector<Velocity>& v ) override;

		void GetLinearVelocity( spat::Vector<Velocity>& v ) const override;

		void SetAngularVelocity( const spat::Vector<AngularVelocity>& w ) override;

		void GetAngularVelocity( spat::Vector<AngularVelocity>& w ) const override;


		inline physx::PxRigidDynamic& Actor() const noexcept{
			return m_Actor;
		}

		//inline Real EngineMetersPerUnit() const noexcept{
		//	return m_EngineMetersPerUnit;
		//}

		inline Real EngineKilogramsPerUnit() const noexcept{
			return m_EngineKilogramsPerUnit;
		}
	protected:

	private:
		physx::PxRigidDynamic& m_Actor;
		const Real m_EngineMetersPerUnit;
		const Real m_EngineKilogramsPerUnit;
	};


	class PhysX_Body_Imp : public PhysX_Body_ImpBase{
	public:
		PhysX_Body_Imp( physx::PxPhysics& physics, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept;
		PhysX_Body_Imp( physx::PxScene& scene, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept;
		PhysX_Body_Imp( physx::PxAggregate& aggregate, Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept;

		~PhysX_Body_Imp() noexcept;
	};


}

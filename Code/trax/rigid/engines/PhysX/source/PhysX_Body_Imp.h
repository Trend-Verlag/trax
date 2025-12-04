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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/rigid/source/Body_Imp.h"

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

		void SpecificInertiaTensor( const spat::SquareMatrix<Area,3>& tensor ) override;

		spat::SquareMatrix<Area,3> SpecificInertiaTensor() const override;


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

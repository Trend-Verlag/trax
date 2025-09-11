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

#include "trax/rigid/source/Simulator_Imp.h"
#include "PhysX_Imp.h"

namespace physx{
	class PxOmniPvd;
}

namespace trax
{
	using namespace spat;

	class PhysX_Simulator :	public Simulator_Imp
	{
	public:
		PhysX_Simulator();
		PhysX_Simulator( const PhysX_Simulator& ) = delete;
		PhysX_Simulator( PhysX_Simulator&& ) = delete;
		PhysX_Simulator( bool bVisualDebugger );
		PhysX_Simulator( physx::PxPhysics& physics );
		~PhysX_Simulator();

		PhysX_Simulator& operator=( const PhysX_Simulator& ) = delete;
		PhysX_Simulator& operator=( PhysX_Simulator&& ) = delete;

		// Simulator:
		const char*	TypeName() const noexcept override{
			return "PhysXSimulator";
		}

		const char* GetVersionString() const override;


		inline physx::PxPhysics&	Physics() const noexcept { return m_Physics; }

#if (PX_PHYSICS_VERSION_MAJOR < 5)
		inline physx::PxCooking&	Cooking() const noexcept { return *m_pCooking; }
#endif
	private:
		std::string m_VersionString;
		bool m_bPhysicsCreated;

		physx::PxDefaultAllocator*		m_pAllocatorCallback;
		physx::PxDefaultErrorCallback*	m_pErrorCallback;
		physx::PxFoundation*			m_pFoundation;
		physx::PxPvd*					m_pPhysxVisualDebugger;
		physx::PxPvdTransport*			m_pTransport;
		physx::PxOmniPvd*				m_pOmniPhysxVisualDebugger;

		physx::PxPhysics&				m_Physics;
#if (PX_PHYSICS_VERSION_MAJOR < 5)
		physx::PxCooking*				m_pCooking;
#endif

		void MakeVersionString();
		physx::PxPhysics& InitPhysX( bool bVisualDebugger );
		void ExitPhysX() noexcept;

		void InitPvd() noexcept;
		void ExitPvd() noexcept;
		void InitOmniPvd() noexcept;
		void ExitOmniPvd() noexcept;
	};

}

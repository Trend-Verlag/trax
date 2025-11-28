//	trax track library
//	AD 2015
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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

// NOLINTBEGIN
#ifdef TRAX_UNREAL
//#include "E:\Program Files\Epic Games\UE_5.5\Engine\Source\Runtime\Core\Public\GenericPlatform\GenericPlatformCompilerPreSetup.h"
//#include "E:\Program Files\Epic Games\UE_5.5\Engine\Source\Runtime\Core\Public\MSVC\MSVCPlatformCompilerPreSetup.h"
//THIRD_PARTY_INCLUDES_START
//PRAGMA_PUSH_PLATFORM_DEFAULT_PACKING
//__pragma(pack(push, 8))
#pragma warning( push )
#pragma warning(disable: 4324) // 'structname': structure was padded due to alignment specifier
#endif
//#include "PxPhysicsAPI.h" // NOLINT 
#include "extensions/PxExtensionsAPI.h"

#include "foundation/PxMat44.h"
#include "foundation/PxTransform.h"
#include "foundation/PxPhysicsVersion.h"
#include "foundation/PxVec3.h"

#include "geometry/PxHeightField.h"
#include "geometry/PxHeightFieldDesc.h"

#include "pvd/PxPvdTransport.h"

#include "PxConstraint.h"
#include "PxConstraintDesc.h"
#include "PxMaterial.h"
#include "PxPhysics.h"
#include "PxRigidActor.h"
#include "PxRigidBody.h"
#include "PxRigidDynamic.h"
#include "PxRigidStatic.h"
#include "PxScene.h"
#include "PxSimulationEventCallback.h"
#ifdef TRAX_UNREAL
#pragma warning( pop )
//__pragma(pack(pop))
//PRAGMA_POP_PLATFORM_DEFAULT_PACKING
//THIRD_PARTY_INCLUDES_END
#endif
// NOLINTEND

namespace trax{

	struct Body;

	physx::PxRigidDynamic& Actor( const Body& body );

	class SceneLockRead {
	public:
		SceneLockRead() = delete;
		SceneLockRead( const SceneLockRead& ) = delete;
		SceneLockRead( SceneLockRead&& lr ) noexcept
		:	m_pScene{ lr.m_pScene },
			m_bLock	{ lr.m_bLock }
		{
			lr.m_pScene = nullptr;
			lr.m_bLock = false;
		}
		SceneLockRead( physx::PxScene* pScene ) noexcept
		:	m_pScene{ pScene },
			m_bLock	{ false }
		{
			if( m_pScene && m_pScene->getFlags().isSet(physx::PxSceneFlag::eREQUIRE_RW_LOCK) ){
				m_pScene->lockRead();
				m_bLock = true;
			}
			else
				m_pScene = nullptr;
		}

		~SceneLockRead() noexcept{
			if( m_bLock )
				m_pScene->unlockRead();
		}

		SceneLockRead& operator=( const SceneLockRead& ) = delete;
		SceneLockRead& operator=( SceneLockRead&& lr ) noexcept{
			if( m_bLock )
				m_pScene->unlockRead();

			m_pScene = lr.m_pScene;
			lr.m_pScene = nullptr;
			m_bLock = lr.m_bLock;
			lr.m_bLock = false;
			return *this;
		}

	private:
		physx::PxScene* m_pScene;
		bool m_bLock;
	};

	class SceneLockWrite {
	public:
		SceneLockWrite() = delete;
		SceneLockWrite( const SceneLockWrite& ) = delete;
		SceneLockWrite( SceneLockWrite&& lr ) noexcept
		:	m_pScene{ lr.m_pScene },
			m_bLock	{ lr.m_bLock }
		{
			lr.m_pScene = nullptr;
			lr.m_bLock = false;
		}
		SceneLockWrite( physx::PxScene* pScene ) noexcept
		:	m_pScene{ pScene },
			m_bLock	{ false }
		{
			if( m_pScene && m_pScene->getFlags().isSet(physx::PxSceneFlag::eREQUIRE_RW_LOCK) ){
				m_pScene->lockWrite();
				m_bLock = true;
			}
			else
				m_pScene = nullptr;
		}

		~SceneLockWrite() noexcept {
			if( m_bLock )
				m_pScene->unlockWrite();
		}

		SceneLockWrite& operator=( const SceneLockWrite& ) = delete;
		SceneLockWrite& operator=( SceneLockWrite&& lr ) noexcept{
			if( m_bLock )
				m_pScene->unlockWrite();

			m_pScene = lr.m_pScene;
			lr.m_pScene = nullptr;
			m_bLock = lr.m_bLock;
			lr.m_bLock = false;
			return *this;
		}
	private:
		physx::PxScene* m_pScene;
		bool m_bLock;
	};
}
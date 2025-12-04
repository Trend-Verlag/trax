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

#include "PhysX_Simulator.h"

#ifdef TRAX_OMNIVERSE
#if (PX_PHYSICS_VERSION_MAJOR >= 5)
#include "omnipvd/PxOmniPvd.h"
#include "OmniPvdWriter.h"
#include "OmniPvdFileWriteStream.h"
#endif
#endif

#include <iostream>

using namespace spat;

namespace trax
{
///////////////////////////////////////
std::unique_ptr<Simulator> Simulator::Make( Type type ) noexcept
{
	try{
		switch( type )
		{
			case Type::PhysX:
				return std::make_unique<PhysX_Simulator>();
			case Type::PhysX_VisualDebugger:
				return std::make_unique<PhysX_Simulator>( true );
		}
	}
	catch( const std::runtime_error& e ){
		std::cerr << e.what() << std::endl;
	}
	catch( const std::bad_alloc& e ){
		std::cerr << e.what() << std::endl;
	}
	catch( ... ){
		std::cerr << "An unknown error occurred." << std::endl;
	}

	return nullptr;
}
///////////////////////////////////////
PhysX_Simulator::PhysX_Simulator()
	: Simulator_Imp				{}
	, m_bPhysicsCreated			{ false }
	, m_pAllocatorCallback		{ nullptr }
	, m_pErrorCallback			{ nullptr }
	, m_pFoundation				{ nullptr }
	, m_pPhysxVisualDebugger	{ nullptr }
	, m_pTransport				{ nullptr }
	, m_pOmniPhysxVisualDebugger{ nullptr }
	, m_Physics					{ InitPhysX( false ) } 
{
	MakeVersionString();
}

PhysX_Simulator::PhysX_Simulator(
	bool bVisualDebugger )
	: Simulator_Imp				{}
	, m_bPhysicsCreated			{ false }
	, m_pAllocatorCallback		{ nullptr }
	, m_pErrorCallback			{ nullptr }
	, m_pFoundation				{ nullptr }
	, m_pPhysxVisualDebugger	{ nullptr }
	, m_pTransport				{ nullptr }
	, m_pOmniPhysxVisualDebugger{ nullptr }
	, m_Physics					{ InitPhysX( bVisualDebugger ) } 
{
	MakeVersionString();
}

PhysX_Simulator::PhysX_Simulator( 
	physx::PxPhysics& physics )
	: Simulator_Imp				{}
	, m_bPhysicsCreated			{ false }
	, m_pAllocatorCallback		{ nullptr }
	, m_pErrorCallback			{ nullptr }
	, m_pFoundation				{ nullptr }
	, m_pPhysxVisualDebugger	{ nullptr }
	, m_pTransport				{ nullptr }
	, m_pOmniPhysxVisualDebugger{ nullptr }
	, m_Physics					{ physics } 
{
	MakeVersionString();
}

PhysX_Simulator::~PhysX_Simulator()
{
	if( m_bPhysicsCreated ){
		ExitPhysX();
	}
}

const char* PhysX_Simulator::GetVersionString() const{
	return m_VersionString.c_str();
}

void LogFunction( char* logLine ){
	std::clog << logLine << std::endl;
}

void PhysX_Simulator::MakeVersionString()
{
	m_VersionString = Simulator_Imp::GetVersionString();
	m_VersionString += " with PhysX ";
	m_VersionString += std::to_string(PX_PHYSICS_VERSION_MAJOR);
	m_VersionString += ".";
	m_VersionString += std::to_string(PX_PHYSICS_VERSION_MINOR);
	m_VersionString += ".";
	m_VersionString += std::to_string(PX_PHYSICS_VERSION_BUGFIX);
	m_VersionString += ".";
}

physx::PxPhysics& PhysX_Simulator::InitPhysX( bool bVisualDebugger )
{
	m_pAllocatorCallback = new physx::PxDefaultAllocator;
	m_pErrorCallback = new physx::PxDefaultErrorCallback;

#ifdef TRAX_WITH_PHYSX_3_4
	m_pFoundation = PxCreateFoundation( PX_FOUNDATION_VERSION, DefaultAllocatorCallback, DefaultErrorCallback );
	m_pFoundation = &PxGetFoundation();
#else
	if( m_pFoundation = PxIsFoundationValid(); !m_pFoundation )
		m_pFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, *m_pAllocatorCallback, *m_pErrorCallback );
#endif
	if(!m_pFoundation)
		throw std::runtime_error("PxCreateFoundation failed!");

	if( bVisualDebugger )
	{
		InitPvd();
		InitOmniPvd();
	}

#if (PX_PHYSICS_VERSION_MAJOR < 5)
	physx::PxPhysics* pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, physx::PxTolerancesScale(), false, m_pPhysxVisualDebugger );
	if( !pPhysics )
		throw std::runtime_error("PxCreatePhysics failed!");

	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, physx::PxCookingParams(physx::PxTolerancesScale()));
	if (!m_pCooking)
		throw std::runtime_error("PxCreateCooking failed!");
#else
	physx::PxPhysics* pPhysics = PxCreatePhysics(
		PX_PHYSICS_VERSION, 
		*m_pFoundation, 
		physx::PxTolerancesScale{ units_per_meter, (abs(g) * 1_s).Units() }, 
		false, 
		m_pPhysxVisualDebugger, 
		m_pOmniPhysxVisualDebugger );

	if( !pPhysics )
		throw std::runtime_error("PxCreatePhysics failed!");

#ifdef TRAX_OMNIVERSE
	if( m_pOmniPhysxVisualDebugger ){
		m_pOmniPhysxVisualDebugger->getFileWriteStream()->setFileName("myoutpufile.ovd");
		m_pOmniPhysxVisualDebugger->startSampling();
	}
#endif
#endif

	if(!PxInitExtensions(*pPhysics,m_pPhysxVisualDebugger))
		throw std::runtime_error("PxInitExtensions failed!");

	m_bPhysicsCreated = true;
	return *pPhysics;
}

void PhysX_Simulator::ExitPhysX() noexcept
{
#if (PX_PHYSICS_VERSION_MAJOR < 5)
	if( m_pCooking ){
		m_pCooking->release();
		m_pCooking = nullptr;
	}
#endif

	PxCloseExtensions();
	Physics().release();

	ExitPvd();
	ExitOmniPvd();

	if( m_pFoundation ){
		m_pFoundation->release();
		m_pFoundation = nullptr;
	}

	if( m_pErrorCallback ){
		delete m_pErrorCallback;
		m_pErrorCallback = nullptr;
	}

	if( m_pAllocatorCallback ){
		delete m_pAllocatorCallback;
		m_pAllocatorCallback = nullptr;
	}
}

void PhysX_Simulator::InitPvd() noexcept
		//TODO: this causes memory leaks in Boost test cases. (PhysX init alo does. Is it real or
		// wrong error: try in simple main() app...
{
	if( (m_pPhysxVisualDebugger = physx::PxCreatePvd(*m_pFoundation)) != nullptr ){
		if( (m_pTransport = physx::PxDefaultPvdSocketTransportCreate(
			"127.0.0.1",			// IP of the PC which is running PVD
			5425,					// TCP port to connect to, where PVD is listening
			2000 )) != nullptr )	// timeout in milliseconds to wait for PVD to respond, 
		{
			if( !m_pPhysxVisualDebugger->connect(*m_pTransport,physx::PxPvdInstrumentationFlag::eALL) )
				std::clog << Verbosity::normal << "PhysX_Simulator: PhysX Visual Debugger connection failed!" << std::endl;
		}
	}

	// PhysX 3.2? 
	//if( pPhysics->getPvdConnectionManager() ){
	//	// setup connection parameters
	//	const char*     pvd_host_ip = "127.0.0.1";  // IP of the PC which is running PVD
	//	int             port        = 5425;         // TCP port to connect to, where PVD is listening
	//	unsigned int    timeout     = 2000;          // timeout in milliseconds to wait for PVD to respond,
	//												// consoles and remote PCs need a higher timeout.
	//	physx::PxVisualDebuggerConnectionFlags connectionFlags = physx::PxVisualDebuggerExt::getAllConnectionFlags();

	//	// and now try to connect
	//	m_pVisualDebuggerConnection =
	//		physx::PxVisualDebuggerExt::createConnection( 
	//			m_pPhysics->getPvdConnectionManager(),
	//			pvd_host_ip, 
	//			port, 
	//			timeout, 
	//			connectionFlags );

	//	m_pPhysics->getVisualDebugger()->setVisualDebuggerFlags( physx::PxVisualDebuggerFlag::eTRANSMIT_CONTACTS | physx::PxVisualDebuggerFlag::eTRANSMIT_CONSTRAINTS );
	//}
}

void PhysX_Simulator::ExitPvd() noexcept
{
	if( m_pPhysxVisualDebugger ){			
		m_pPhysxVisualDebugger->disconnect();

		m_pTransport->release();
		m_pTransport = nullptr;
		m_pPhysxVisualDebugger->release();
		m_pPhysxVisualDebugger = nullptr;
	}

	//if( m_pVisualDebuggerConnection ){
	//	m_pVisualDebuggerConnection->release();
	//	m_pVisualDebuggerConnection = nullptr;
	//}
}

void PhysX_Simulator::InitOmniPvd() noexcept
{
#ifdef TRAX_OMNIVERSE
#if (PX_PHYSICS_VERSION_MAJOR >= 5)
	// Create the OmniPVD instance
	if( m_pOmniPhysxVisualDebugger = PxCreateOmniPvd(*m_pFoundation); m_pOmniPhysxVisualDebugger )
	{
		OmniPvdWriter* omniWriter = m_pOmniPhysxVisualDebugger->getWriter();
		// Uncomment for debugging the OmniPvd write stream
		omniWriter->setLogFunction( LogFunction );
		OmniPvdFileWriteStream* omniFileWriteStream = m_pOmniPhysxVisualDebugger->getFileWriteStream();
		if( omniWriter && omniFileWriteStream )
		{
			omniWriter->setWriteStream( *omniFileWriteStream );
		}
	}
#endif
#endif
}

void PhysX_Simulator::ExitOmniPvd() noexcept
{
#ifdef TRAX_OMNIVERSE
#if (PX_PHYSICS_VERSION_MAJOR >= 5)
	if( m_pOmniPhysxVisualDebugger ){
		m_pOmniPhysxVisualDebugger->release();
		m_pOmniPhysxVisualDebugger = nullptr;
	}
#endif
#endif
}

}
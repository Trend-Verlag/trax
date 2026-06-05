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

#include "Scene_Imp.h"

#include "../Gestalt.h"
#include "../GeomType.h"
#include "trax/Simulated.h"

#include <iostream>

namespace trax
{
	using namespace spat;

Scene_Imp::Scene_Imp()
	: m_PlugToStop{ *this, &Scene_Imp::Stop }
{
	m_PlugToStop.Reference( "name", "PlugToStop" );
}

Scene_Imp::~Scene_Imp()
{
	for( auto& [pSimulated,bRegistered] : m_Simulated )
	{
		std::cerr << Verbosity::error << "There is a Simulator interface still registered: " << pSimulated->TypeName() << "." << std::endl;
	}

	if( !m_Simulated.empty() )
		std::cerr << Verbosity::detailed << "Not having Simulator interfaces already unregistered when destroying Scene_Imp is almost certainly a bug." << std::endl;
}

void Scene_Imp::Register( Simulated& simulated ) noexcept
{
	if( std::find_if( m_Simulated.begin(), m_Simulated.end(),
		[&simulated](const std::pair<Simulated*, bool>& pair){ return pair.first == &simulated; } ) == m_Simulated.end() &&
		std::find( m_ToBeRegistered.begin(), m_ToBeRegistered.end(), &simulated ) == m_ToBeRegistered.end() )
	{
		m_ToBeRegistered.push_back( &simulated );
		simulated.Registered( *this );
	}
}

void Scene_Imp::Unregister( Simulated& simulated ) noexcept
{
	if( auto iter = std::find_if( m_Simulated.begin(), m_Simulated.end(),
		[&simulated](const std::pair<Simulated*, bool>& pair){ return pair.first == &simulated; } ); iter != m_Simulated.end() &&
		std::find( m_ToBeUnregistered.begin(), m_ToBeUnregistered.end(), &simulated ) == m_ToBeUnregistered.end() )
	{
		m_ToBeUnregistered.push_back( &simulated );
		iter->second = false;
		simulated.Unregistered( *this );
	}
}

void Scene_Imp::UnregisterAllSimulated() noexcept
{
	if( m_bSimulationRunning )
	{
		for( auto& [pSimulated,bRegistered] : m_Simulated )
		{
			if( bRegistered )
				pSimulated->Stop();
			std::cout << Verbosity::detailed << "Unregistered a " << pSimulated->TypeName() << " from simulation." << std::endl;
		}
	}

	m_Simulated.clear();
}

void Scene_Imp::Simulate()
{
	BeginSimulation();

	while( m_bSimulationRunning )
	{
		Step();
	}

	EndSimulation();
}

void Scene_Imp::Simulate( Time forTimePeriod )
{
	BeginSimulation();

	Loop( forTimePeriod );

	EndSimulation();
}

bool Scene_Imp::IsSimulationRunning() const noexcept{
	return m_bSimulationRunning;
}

Time Scene_Imp::SimulationTime() const noexcept{
	return m_SimulationTime;
}

void Scene_Imp::BeginSimulation() noexcept
{
	m_bSimulationRunning = true;
	m_SimulationTime = 0_s;
	m_LoopTime = 0_s;

	for( auto& [pSimulated,bRegistered] : m_Simulated )
	{
		if( bRegistered )
			pSimulated->Start();
	}
}

void Scene_Imp::Loop( Time forTimePeriod )
{
	m_LoopTime += forTimePeriod;
	m_bLoopRunning = true;

	while( m_bLoopRunning )
	{
		if( m_bPaused )
			continue;

		Step();

		if( m_LoopTime -= fixed_timestep; m_LoopTime <= 0_s )
			m_bLoopRunning = false;
	}
}

void Scene_Imp::Step( Time dt )
{
	DoRegistrations();

	StartStep( dt );

	Idle();

	while( !EndStep() )
		;

	m_SimulationTime += dt;

	PreUpdate();

	Update( dt );

	m_JackOnSimulationStep.Pulse();
}

void Scene_Imp::EndSimulation() noexcept
{
	for( auto& [pSimulated,bRegistered] : m_Simulated )
	{
		if( bRegistered )
			pSimulated->Stop();
	}

	m_bSimulationRunning = false;
}

void Scene_Imp::Pause() noexcept{
	m_bPaused = true;
}

void Scene_Imp::Resume() noexcept{
	m_bPaused = false;
}

void Scene_Imp::Stop() noexcept{
	m_bLoopRunning = false;
	m_bSimulationRunning = false;
}

std::unique_ptr<Gestalt> Scene_Imp::CreateGestalt( Box<Length> box, Mass mass ) const noexcept
{
	Frame<Length,One> bodyFrame;
	bodyFrame.Init();
	bodyFrame.P = box.Center();
	box.Move( Origin3D<Length> - bodyFrame.P );

	if( std::unique_ptr<Gestalt> pGestalt = CreateGestalt(); pGestalt )
	{
		if( std::unique_ptr<Geom> pGeom = CreateGeom( GeomType::box ); pGeom )
		{
			dynamic_cast<GeomCuboid*>(pGeom.get())->SetFromBox( box );
			pGestalt->SetMass( mass );
			pGestalt->PrincipalMomentsOfInertia( mass * SpecificInertiaTensorFor( box ) );
			pGestalt->Attach( std::move(pGeom) );
			pGestalt->SetFrame( bodyFrame );
			return pGestalt;
		}
	}

	return nullptr;
}

std::unique_ptr<Geom> Scene_Imp::CreateGeom( GeomType type ) const
{
	switch( type ){
	case GeomType::unknown:
	case GeomType::none:
		return nullptr;
	case GeomType::box:
		return CreateGeomCuboid();
	case GeomType::cylinder:
		return CreateGeomCylinder();
	case GeomType::capsule:
		return CreateGeomCapsule();
	case GeomType::sphere:
		return CreateGeomSphere();
	case GeomType::heightField:
		return CreateGeomHeightField();
	case GeomType::convexMesh:
		return CreateGeomConvexMesh();
	case GeomType::triangleMesh:
		return CreateGeomTriangleMesh();
	default:
		assert( false );
		return nullptr;
	}
}

Jack& Scene_Imp::JackOnSimulationStep() noexcept{
	return m_JackOnSimulationStep;
}

MultiPlug& Scene_Imp::PlugToStop() noexcept{
	return m_PlugToStop;
}

int Scene_Imp::CountPlugs() const{
	return m_PlugToStop.CountPlugs();
}

int Scene_Imp::CountJacks() const noexcept{
	return 1;
}

const Jack& Scene_Imp::_GetJack( int idx ) const{
	switch( idx ){
	case 0:
		return m_JackOnSimulationStep;
	}

	assert( 0 );
	std::ostringstream stream;
	stream << __FILE__ << '(' << __LINE__ << ')' << "Out of range!";
	throw std::range_error( stream.str() );
}

const Plug& Scene_Imp::_GetPlug( int idx ) const{
	if( idx < m_PlugToStop.CountPlugs() )
		return m_PlugToStop.GetPlug( idx );

	assert( 0 );
	std::ostringstream stream;
	stream << __FILE__ << '(' << __LINE__ << ')' << "Out of range!";
	throw std::range_error( stream.str() );
}

void Scene_Imp::DoRegistrations() noexcept
{
	while( !m_ToBeRegistered.empty() )
	{
		auto& pSimulated = m_ToBeRegistered.back();

		m_Simulated.push_back( std::make_pair(pSimulated,true) );
		std::cout << Verbosity::detailed << "Registered a " << pSimulated->TypeName() << " for simulation." << std::endl;
		m_ToBeRegistered.pop_back();
	}

	while( !m_ToBeUnregistered.empty() )
	{
		auto& pSimulated = m_ToBeUnregistered.back();

		if( m_bSimulationRunning )
			pSimulated->Stop();

		m_Simulated.erase( std::remove_if( m_Simulated.begin(), m_Simulated.end(),
			[pSimulated](const std::pair<Simulated*, bool>& pair){ return pair.first == pSimulated; } ),
			m_Simulated.end() );

		std::cout << Verbosity::detailed << "Unregistered a " << pSimulated->TypeName() << " from simulation." << std::endl;
		m_ToBeUnregistered.pop_back();
	}
}

void Scene_Imp::Idle()
{
	for( auto& [pSimulated,bRegistered] : m_Simulated )
	{
		if( bRegistered )
			pSimulated->Idle();
	}
}

void Scene_Imp::PreUpdate()
{
	for( auto& [pSimulated,bRegistered] : m_Simulated )
	{
		if( bRegistered )
			pSimulated->PreUpdate();
	}
}

void Scene_Imp::Update( Time dt )
{
	for( auto& [pSimulated,bRegistered] : m_Simulated )
	{
		if( bRegistered )
			pSimulated->Update( dt );
	}
}
}
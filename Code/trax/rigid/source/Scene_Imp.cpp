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

namespace trax
{
	using namespace spat;

Scene_Imp::Scene_Imp()
	: m_PlugToStop{ *this, &Scene_Imp::Stop }
{
	m_PlugToStop.Reference( "name", "PlugToStop" );
}

void Scene_Imp::Register( Simulated& simulated ) noexcept
{
	if( std::find( m_Simulated.begin(), m_Simulated.end(), &simulated ) == m_Simulated.end() )
		m_Simulated.push_back( &simulated );
}

void Scene_Imp::Unregister( const Simulated& simulated ) noexcept
{
	auto it = std::find( m_Simulated.begin(), m_Simulated.end(), &simulated );

	if( it != m_Simulated.end() )
	{
		if( m_bSimulationRunning )
			(*it)->Stop();

		m_Simulated.erase( it );
	}
}

void Scene_Imp::UnregisterAllSimulated() noexcept
{
	if( m_bSimulationRunning )
	{
		for( auto& pSimulated : m_Simulated )
		{
			pSimulated->Stop();
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

	for( auto& pSimulated : m_Simulated )
	{
		pSimulated->Start( *this );
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
	StartStep( dt );

	Idle();

	while( !EndStep() )
		;

	m_SimulationTime += dt;

	Update( dt );

	m_JackOnSimulationStep.Pulse();
}

void Scene_Imp::EndSimulation() noexcept
{
	for( auto& pSimulated : m_Simulated )
	{
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

void Scene_Imp::Idle() const
{
	for( auto& pSimulated : m_Simulated )
	{
		pSimulated->Idle();
	}
}

void Scene_Imp::Update( Time dt )
{
	for( auto& pSimulated : m_Simulated )
	{
		pSimulated->Update( dt );
	}
}
}
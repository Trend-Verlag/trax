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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#include "Scene_Imp.h"

#include "../Gestalt.h"
#include "../GeomType.h"
#include "trax/Simulated.h"

namespace trax
{
	using namespace spat;

Scene_Imp::Scene_Imp()
	: m_bLoopRunning{ false }
	, m_PlugToStop	{ *this, &Scene_Imp::Stop }
{
	m_PlugToStop.Reference( "name", "PlugToStop" );
}

void Scene_Imp::Register( std::shared_ptr<Simulated> pSimulated )
{
	if( !pSimulated )
		return;

	if( std::find( m_Simulated.begin(), m_Simulated.end(), pSimulated ) == m_Simulated.end() )
		m_Simulated.push_back( pSimulated );
}

void Scene_Imp::Unregister( std::shared_ptr<Simulated> pSimulated )
{
	if( !pSimulated )
		return;

	auto it = std::find( m_Simulated.begin(), m_Simulated.end(), pSimulated );
	if( it != m_Simulated.end() )
	{
		if( m_bLoopRunning )
			(*it)->Stop();

		m_Simulated.erase( it );
	}
	else
	{
		assert( false && "Unregistering a Simulated object that was not registered!" );
	}
}

void Scene_Imp::Update( Time dt ) noexcept
{
	for( auto& pSimulated : m_Simulated )
	{
		pSimulated->Update( dt );
	}
}

void Scene_Imp::Step( Time dt ) noexcept
{
	StartStep( dt );

	while( !EndStep() )
		;

	Update( dt );

	m_JackOnSimulationStep.Pulse();
}

void Scene_Imp::Loop() noexcept
{
	m_bLoopRunning = true;

	for( auto& pSimulated : m_Simulated )
	{
		pSimulated->Start( *this );
	}

	while( m_bLoopRunning )
	{
		Step();
	}

	for( auto& pSimulated : m_Simulated )
	{
		pSimulated->Stop();
	}
}

void Scene_Imp::Loop( Time forTimePeriod ) noexcept
{
	m_LoopTime = forTimePeriod;
	m_bLoopRunning = true;

	for( auto& pSimulated : m_Simulated )
	{
		pSimulated->Start( *this );
	}

	while( m_bLoopRunning )
	{
		Step();

		if( m_LoopTime -= fixed_timestep; m_LoopTime <= 0_s ){
		//	m_LoopTime = 0_s;
			m_bLoopRunning = false;
		}
	}

	for( auto& pSimulated : m_Simulated )
	{
		pSimulated->Stop();
	}
}

void Scene_Imp::Stop() noexcept{
	m_bLoopRunning = false;
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
			pGestalt->InertiaTensor( SpecificInertiaTensorFor( box ) );
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

}
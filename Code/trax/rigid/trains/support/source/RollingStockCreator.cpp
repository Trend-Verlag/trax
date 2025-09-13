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

#include "../RollingStockCreator.h"

#include "trax/rigid/GeomType.h"
#include "trax/rigid/Gestalt.h"
#include "trax/rigid/TractionForceCharacteristic.h"
#include "trax/rigid/trains/RollingStock.h"
#include "trax/rigid/trains/TrainScene.h"
#include "trax/rigid/trains/WheelFrame.h"

namespace trax{

RollingStockCreator::RollingStockCreator( TrainScene& scene )
	: m_Scene{ scene }
{
}

std::shared_ptr<RollingStock> RollingStockCreator::GetRollingStock() const noexcept
{
	return m_pRollingStock;
}

bool RollingStockCreator::ParsingStart() noexcept(false)
{
	m_ActiveFrames.push( spat::Identity<dim::Length, dim::One> );
	return true;
}

bool RollingStockCreator::RollingStockStart() noexcept(false)
{
	if( m_pRollingStock = RollingStock::Make(); m_pRollingStock )
		return true;
	
	return false;
}

bool RollingStockCreator::BogieStart( 
	IDType id,
	const std::string& name,
	const std::string& model ) noexcept(false)
{
	if( std::shared_ptr<Bogie> pBogie = Bogie::Make( m_Scene, m_Scene.CreateGestalt() ); pBogie )
	{
		pBogie->ID( id );
		pBogie->Reference( "name", name );
		pBogie->Reference( "model", model );
		m_AllBogies.push_back( pBogie );
		m_ActiveBogie.push( pBogie );
		m_ActiveFrames.push( m_ActiveFrames.top() );
		return true;
	}

	return false;
}

void RollingStockCreator::BogieFrame( const spat::Frame<dim::Length, dim::One>& frame ) noexcept
{
	m_ActiveFrames.top() *= frame;
	m_ActiveBogie.top()->GetGestalt().SetFrame( m_ActiveFrames.top() );
}

bool RollingStockCreator::GeomStart( 
	[[maybe_unused]] const std::string& name,
	[[maybe_unused]] Geom::Filters filter,
	[[maybe_unused]] GeomType type,
	[[maybe_unused]] Mass mass ) noexcept(false)
{
	return true;
}

void RollingStockCreator::Material( const trax::Material& material ) noexcept
{
	m_Material = material;
}

void RollingStockCreator::GeomEnd( 
	const std::string& name, 
	Geom::Filters filter, 
	GeomType type, 
	Mass mass ) noexcept(false)
{
	if( auto pGeom = CreateGeom( type, m_Area ) ){
		pGeom->SetName( name.c_str() );

		spat::Frame<Length,One> geomFrame;
		pGeom->GetFrame( geomFrame );
		m_Frame.ToParent( geomFrame );
		pGeom->SetFrame( geomFrame );

		pGeom->SetMaterial( m_Material );
		pGeom->TypeFilter( filter );
		pGeom->CollisionFilter( CollisionFilterFor( filter ) );

		m_ActiveBogie.top()->GetGestalt().Attach( std::move( pGeom ), mass );
	}
}

void RollingStockCreator::Coupling( 
	RailRunner::EndType end, 
	Length bufferLength, 
	Force maxForce, 
	IDType typeID, 
	const spat::Frame<dim::Length, dim::One>& frame ) noexcept(false)
{
	Bogie::CouplingProps props;
	props.Position.r		= bufferLength;
	props.BreakingForce		= maxForce;
	props.CouplingTypeIdx	= typeID;
	props.Position.c		= frame.P;

	m_ActiveBogie.top()->SetCouplingProps( end, props );
}

void RollingStockCreator::Swivel( 
	RailRunner::EndType slot, 
	IDType childID, 
	const spat::Frame<dim::Length, dim::One>& frame ) noexcept(false)
{
	if( slot != RailRunner::EndType::none )
		m_Swivels.push_back( std::make_tuple( m_ActiveBogie.top(), slot, childID, frame ) );
}

void RollingStockCreator::BogieEnd( [[maybe_unused]] const std::string& sndGroup ) noexcept
{
	m_ActiveFrames.pop();
	m_ActiveBogie.pop();
}

bool RollingStockCreator::WheelFrameStart( 
	IDType id,
	const std::string& name,
	const std::string& model ) noexcept(false)
{
	if( std::shared_ptr<WheelFrame> pWheelFrame = WheelFrame::Make( m_Scene, m_Scene.CreateGestalt() ); pWheelFrame )
	{
		pWheelFrame->ID( id );
		pWheelFrame->Reference( "name", name );
		pWheelFrame->Reference( "model", model );
		m_AllBogies.push_back( pWheelFrame );
		m_ActiveBogie.push( pWheelFrame );
		m_ActiveWheelFrame.push( pWheelFrame );
		m_ActiveFrames.push( m_ActiveFrames.top() );
		return true;
	}

	return false;
}

bool RollingStockCreator::TractionForceCharacteristicStart() noexcept(false)
{
	if( m_pCharacteristic = TractionForceCharacteristic::Make(); m_pCharacteristic )
		return true;

	return false;
}

void RollingStockCreator::SpeedStep( Velocity velocity, One value ) noexcept
{
	if( m_pCharacteristic )
		m_pCharacteristic->AddSpeedStep( velocity, value );
}

void RollingStockCreator::TractionForceCharacteristicEnd() noexcept(false)
{
	m_ActiveWheelFrame.top()->SetTractionForceCharacteristic( m_pCharacteristic );
}

void RollingStockCreator::TrackJointLimits( const trax::TrackJointLimits& trackJointLimits ) noexcept
{
	m_ActiveWheelFrame.top()->Limits( trackJointLimits );
}

void RollingStockCreator::Wheelset( const trax::Wheelset& wheelset ) noexcept
{
	m_ActiveWheelFrame.top()->Attach( wheelset );
}

bool RollingStockCreator::TrackLocation( const TrackLocationRef& trackLocation ) noexcept(false)
{
	return false;
}

void RollingStockCreator::Anchor( const spat::Frame<dim::Length, dim::One>& anchor ) noexcept
{
	m_ActiveWheelFrame.top()->Anchor( anchor );
}

void RollingStockCreator::WheelFrameEnd( [[maybe_unused]] const std::string & sndGroup ) noexcept
{
	m_pCharacteristic.reset();
	m_ActiveBogie.pop();
	m_ActiveWheelFrame.pop();
	m_ActiveFrames.pop();
}

void RollingStockCreator::RollingStockEnd() noexcept(false)
{
	for( const auto& [pBogie, slot, childID, frame] : m_Swivels )
	{
		if( std::shared_ptr<Bogie> pChild = GetBogie( childID ); pChild )
			pBogie->Attach( pChild, slot, frame );
	}

	if( !m_AllBogies.empty() && m_pRollingStock ){
		if( !m_pRollingStock->Attach( *m_AllBogies.front() ) )
			m_pRollingStock.reset();
	}
}

void RollingStockCreator::ParsingEnd() noexcept
{
	m_AllBogies.clear();
	m_Swivels.clear();
	m_ActiveFrames.pop();
}

std::shared_ptr<Bogie> RollingStockCreator::GetBogie( int idx ) const
{
	auto iter = std::find_if( m_AllBogies.begin(), m_AllBogies.end(), [idx](const auto& p){ return p->ID() == idx; } );
	if( iter != m_AllBogies.end() )
		return *iter;

	return nullptr;
}

std::unique_ptr<Geom> RollingStockCreator::CreateGeom( GeomType type, const spat::Box<Length>& box ) const
{
	switch( type )
	{
	case trax::GeomType::unknown:
	case GeomType::none:
		return nullptr;
	case GeomType::box:
	{
		auto pGeom = m_Scene.CreateGeomCuboid();
		pGeom->SetFromBox( box );
		return pGeom;
	}
	case GeomType::cylinder:
	{
		auto pGeom = m_Scene.CreateGeomCylinder();
		pGeom->Radius( std::min( box.EdgeX()/2, box.EdgeY()/2 ) );
		pGeom->SetLength( box.EdgeZ() );
		return pGeom;
	}
	case GeomType::capsule:
	{
		auto pGeom = m_Scene.CreateGeomCapsule();
		pGeom->Radius( std::min( box.EdgeX()/2, box.EdgeY()/2 ) );
		pGeom->SetLength( box.EdgeZ() );
		return pGeom;
	}
	case GeomType::sphere:
	{
		auto pGeom = m_Scene.CreateGeomSphere();
		pGeom->Radius( std::min( std::min( box.EdgeX()/2, box.EdgeY()/2 ), box.EdgeZ()/2 ) );
		return pGeom;
	}
	default:
		assert( false );
		return nullptr;
	}
}

} // namespace trax
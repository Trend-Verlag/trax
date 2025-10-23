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

#include "Module_Imp.h"

#include "trax/collections/IndicatorCollection.h"
#include "trax/collections/SignalCollection.h"
#include "trax/collections/TrackSystem.h"
#include "trax/rigid/trains/collections/CargoCollection.h"
#include "trax/rigid/trains/collections/Fleet.h"

#include <iostream>

namespace trax{

std::unique_ptr<Module> Module::Make( bool bCreateCollections ) noexcept
{
	try{
		std::unique_ptr<Module_Imp> pRetval = std::make_unique<Module_Imp>(
			bCreateCollections );

		return pRetval;
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

Module_Imp::Module_Imp( bool bCreateCollections )
	: m_Frame{ spat::Identity<Length,One> }
{
	if( bCreateCollections )
	{
		m_pTrackSystem = trax::TrackSystem::Make();
		m_pFleet = trax::Fleet::Make();
		m_pSignalCollection = trax::SignalCollection::Make();
		m_pIndicatorCollection = trax::IndicatorCollection::Make();
		m_pCargoCollection = trax::CargoCollection::Make();
	}
}

bool Module_Imp::IsValid( bool bSilent ) const noexcept
{
	if( !m_Frame.IsOrthoNormal() )
	{
		if( !bSilent )
			std::cout << "Module '" << Reference( "name" ) << " id:" << ID() << "' has a non-orthonormal frame." << std::endl;
		return false;
	}

	if( m_Volume.Volume() <= 0_m3 )
	{
		if( !bSilent )
			std::cout << "Module '" << Reference( "name" ) << " id:" << ID() << "' has an invalid volume." << std::endl;
		return false;
	}

	if( !m_pTrackSystem )
	{
		if( !bSilent )
			std::cout << "Module '" << Reference( "name" ) << " id:" << ID() << "' has no TrackSystem attached." << std::endl;
		return false;
	} 
	else if( !m_pTrackSystem->IsValid( bSilent ) )
		return false;

	//if( !m_pFleet )
	//{
	//	if( !bSilent )
	//		std::cout << "Module '" << Reference( "name" ) << " id:" << ID() << "' has no Fleet attached." << std::endl;
	//	return false;
	//}
	//else if( !m_pFleet->IsValid( bSilent ) )
	//	return false;

	//if( !m_pSignalCollection )
	//{
	//	if( !bSilent )
	//		std::cout << "Module '" << Reference( "name" ) << " id:" << ID() << "' has no SignalCollection attached." << std::endl;
	//	return false;
	//}
	//else if( !m_pSignalCollection->IsValid( bSilent ) )
	//	return false;

	//if( !m_pIndicatorCollection )
	//{
	//	if( !bSilent )
	//		std::cout << "Module '" << Reference( "name" ) << " id:" << ID() << "' has no IndicatorCollection attached." << std::endl;
	//	return false;
	//}
	//else if( !m_pIndicatorCollection->IsValid( bSilent ) )
	//	return false;

	//if( !m_pCargoCollection )
	//{
	//	if( !bSilent )
	//		std::cout << "Module '" << Reference( "name" ) << " id:" << ID() << "' has no CargoCollection attached." << std::endl;
	//	return false;
	//}
	//else if( !m_pCargoCollection->IsValid( bSilent ) )
	//	return false;

	return true;
}

void Module_Imp::SetFrame( const spat::Frame<Length,One>& frame ) noexcept{
	assert( m_Frame.IsOrthoNormal() );
	m_Frame = frame;
}

const spat::Frame<Length,One>& Module_Imp::GetFrame() const noexcept{
	return m_Frame;
}

void Module_Imp::SetVolume( const spat::Box<Length>& volume ) noexcept{
	assert( volume.Volume() > 0_m3 );
	m_Volume = volume;
}

const spat::Box<Length>& Module_Imp::GetVolume() const noexcept{
	return m_Volume;
}

void Module_Imp::Attach( std::shared_ptr<TrackSystem> pTrackSystem ) noexcept{
	m_pTrackSystem = pTrackSystem;
}

std::shared_ptr<TrackSystem> Module_Imp::GetTrackSystem() const noexcept{
	return m_pTrackSystem;
}

void Module_Imp::Attach( std::shared_ptr<Fleet> pFleet ) noexcept{
	m_pFleet = pFleet;
}

std::shared_ptr<Fleet> Module_Imp::GetFleet() const noexcept{
	return m_pFleet;
}

void Module_Imp::Attach( std::shared_ptr<SignalCollection> pSignalCollection ) noexcept{
	m_pSignalCollection = pSignalCollection;
}

std::shared_ptr<SignalCollection> Module_Imp::GetSignalCollection() const noexcept{
	return m_pSignalCollection;
}

void Module_Imp::Attach( std::shared_ptr<IndicatorCollection> pIndicatorCollection ) noexcept{
	m_pIndicatorCollection = pIndicatorCollection;
}

std::shared_ptr<IndicatorCollection> Module_Imp::GetIndicatorCollection() const noexcept{
	return m_pIndicatorCollection;
}

void Module_Imp::Attach( std::shared_ptr<CargoCollection> pCargoCollection ) noexcept{
	m_pCargoCollection = pCargoCollection;
}

std::shared_ptr<CargoCollection> Module_Imp::GetCargoCollection() const noexcept{
	return m_pCargoCollection;
}

void Module_Imp::Take( Module& fromModule )
{
	if( m_pTrackSystem && fromModule.GetTrackSystem() )
		m_pTrackSystem->Take( *fromModule.GetTrackSystem() );
	if( m_pFleet && fromModule.GetFleet() )
		m_pFleet->Take( *fromModule.GetFleet() );
	if( m_pSignalCollection && fromModule.GetSignalCollection() )
		m_pSignalCollection->Take( *fromModule.GetSignalCollection() );
	if( m_pIndicatorCollection && fromModule.GetIndicatorCollection() )
		m_pIndicatorCollection->Take( *fromModule.GetIndicatorCollection() );
	if( m_pCargoCollection && fromModule.GetCargoCollection() )
		m_pCargoCollection->Take( *fromModule.GetCargoCollection() );

	//if( m_pCameraCollection && fromModule.GetCameraCollection() )
	//	m_pCameraCollection->Take( *fromModule.GetCameraCollection() );
	//if( m_pPulseCounterCollection && fromModule.GetPulseCounterCollection() )
	//	m_pPulseCounterCollection->Take( *fromModule.GetPulseCounterCollection() );
	//if( m_pTimerCollection && fromModule.GetTimerCollection() )
	//	m_pTimerCollection->Take( *fromModule.GetTimerCollection() );
}

void Module_Imp::Clear()
{
	m_pTrackSystem.reset();
	m_pFleet.reset();
	m_pSignalCollection.reset();
	m_pIndicatorCollection.reset();
	m_pCargoCollection.reset();
}

void Module_Imp::ClearCollections()
{
	if( m_pTrackSystem )
		m_pTrackSystem->Clear();
	if( m_pFleet )
		m_pFleet->Clear();
	if( m_pSignalCollection )
		m_pSignalCollection->Clear();
	if( m_pIndicatorCollection )
		m_pIndicatorCollection->Clear();
	if( m_pCargoCollection )
		m_pCargoCollection->Clear();
}



}

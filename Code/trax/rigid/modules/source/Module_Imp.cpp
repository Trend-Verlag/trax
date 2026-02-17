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

#include "Module_Imp.h"

#include "trax/collections/IndicatorCollection.h"
#include "trax/collections/PulseCounterCollection.h"
#include "trax/collections/SignalCollection.h"
#include "trax/collections/TimerCollection.h"
#include "trax/rigid/Scene.h"
#include "trax/rigid/collections/source/TrackSystemMovable_Imp.h"
#include "trax/rigid/modules/Camera.h"
#include "trax/rigid/trains/collections/CargoCollection.h"
#include "trax/rigid/trains/collections/Fleet.h"

#include <iostream>

namespace trax{

std::unique_ptr<Module> Module::Make( bool bCreateCollections ) noexcept
{
	return std::make_unique<Module_Imp>( bCreateCollections );
}

Module_Imp::Module_Imp( bool bCreateCollections ) noexcept
	: m_Frame{ Identity<Length,One> }
{
	if( bCreateCollections )
	{
		if( m_pTrackSystem = TrackSystemMovable_Imp::Make(); m_pTrackSystem )
			m_pTrackSystem->CreateCollection();

		m_pFleet = Fleet::Make();
		m_pSignalCollection = SignalCollection::Make();
		m_pIndicatorCollection = IndicatorCollection::Make();
		m_pCargoCollection = CargoCollection::Make();
	}
}

bool Module_Imp::IsValid() const noexcept
{
	if( !m_Frame.IsOrthoNormal() )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has a non-orthonormal frame." << std::endl;
		return false;
	}

	if( m_Volume.Volume() <= 0_m3 )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has an invalid volume." << std::endl;
		return false;
	}

	if( !m_pTrackSystem )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has no TrackSystem attached." << std::endl;
		return false;
	} 
	else if( !m_pTrackSystem->IsValid() )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has invalid TrackSystem attached." << std::endl;
		return false;
	} 

	if( !m_pFleet )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has no Fleet attached." << std::endl;
		return false;
	}
	else if( !m_pFleet->IsValid() )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has invalid Fleet attached." << std::endl;
		return false;
	}

	if( !m_pSignalCollection )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has no SignalCollection attached." << std::endl;
		return false;
	}
	else if( !m_pSignalCollection->IsValid() ){
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has invalid SignalCollection attached." << std::endl;
		return false;
	}

	if( !m_pIndicatorCollection )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has no IndicatorCollection attached." << std::endl;
		return false;
	}
	else if( !m_pIndicatorCollection->IsValid() )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has invalid IndicatorCollection attached." << std::endl;
		return false;
	}

	if( !m_pCargoCollection )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has no CargoCollection attached." << std::endl;
		return false;
	}
	else if( !m_pCargoCollection->IsValid() )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has invalid CargoCollection attached." << std::endl;
		return false;
	}

	return true;
}

void Module_Imp::RegisterCollections( Scene& withScene ) const noexcept
{
	if( m_pTrackSystem )
		withScene.Register( *m_pTrackSystem );
	if( m_pFleet )
		withScene.Register( *m_pFleet );
}

void Module_Imp::UnregisterCollections( Scene& withScene ) const noexcept
{
	if( m_pFleet )
		withScene.Unregister( *m_pFleet );
	if( m_pTrackSystem )
		withScene.Unregister( *m_pTrackSystem );
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

void Module_Imp::Attach( std::shared_ptr<TimerCollection> pTimerCollection ) noexcept{
	m_pTimerCollection = pTimerCollection;
}

std::shared_ptr<TimerCollection> Module_Imp::GetTimerCollection() const noexcept{
	return m_pTimerCollection;
}

void Module_Imp::Attach( std::shared_ptr<PulseCounterCollection> pPulseCounterCollection ) noexcept{
	m_pPulseCounterCollection = pPulseCounterCollection;
}

std::shared_ptr<PulseCounterCollection> Module_Imp::GetPulseCounterCollection() const noexcept{
	return m_pPulseCounterCollection;
}

void Module_Imp::Attach( std::shared_ptr<CameraCollection> pCameraCollection ) noexcept{
	m_pCameraCollection = pCameraCollection;
}

std::shared_ptr<CameraCollection> Module_Imp::GetCameraCollection() const noexcept{
	return m_pCameraCollection;
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
	if( m_pTimerCollection && fromModule.GetTimerCollection() )
		m_pTimerCollection->Take( *fromModule.GetTimerCollection() );
	if( m_pPulseCounterCollection && fromModule.GetPulseCounterCollection() )
		m_pPulseCounterCollection->Take( *fromModule.GetPulseCounterCollection() );
	if( m_pCameraCollection && fromModule.GetCameraCollection() )
		m_pCameraCollection->Take( *fromModule.GetCameraCollection() );
}

void Module_Imp::Clear() noexcept
{
	m_pTrackSystem.reset();
	m_pFleet.reset();
	m_pSignalCollection.reset();
	m_pIndicatorCollection.reset();
	m_pCargoCollection.reset();
	m_pTimerCollection.reset();
	m_pPulseCounterCollection.reset();
	m_pCameraCollection.reset();
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
	if( m_pTimerCollection )
		m_pTimerCollection->Clear();
	if( m_pPulseCounterCollection )
		m_pPulseCounterCollection->Clear();
	if( m_pCameraCollection )
		m_pCameraCollection->Clear();
}

int Module_Imp::CountJacks() const
{
	int count = 0;
	if( auto pJackEnumerator = decorator_cast<JackEnumerator*>(m_pTrackSystem.get()) )
		count += pJackEnumerator->CountJacks();

	if( auto pJackEnumerator = decorator_cast<JackEnumerator*>(m_pPulseCounterCollection.get()) )
		count += pJackEnumerator->CountJacks();

	if( auto pJackEnumerator = decorator_cast<JackEnumerator*>(m_pTimerCollection.get()) )
		count += pJackEnumerator->CountJacks();

	// todo: count further jacks here ...

	return count;
}

const Jack& Module_Imp::_GetJack( int idx ) const
{
	if( auto pJackEnumerator = decorator_cast<const JackEnumerator*>(m_pTrackSystem.get()); pJackEnumerator ){
		const int countJacks = pJackEnumerator->CountJacks();
		if( idx < countJacks )
			return pJackEnumerator->GetJack( idx );
		idx -= countJacks;
	}

	if( auto pJackEnumerator = decorator_cast<const JackEnumerator*>(m_pFleet.get()); pJackEnumerator ){
		const int countJacks = pJackEnumerator->CountJacks();
		if( idx < countJacks )
			return pJackEnumerator->GetJack( idx );
		idx -= countJacks;
	}

	if( auto pJackEnumerator = decorator_cast<const JackEnumerator*>(m_pSignalCollection.get()); pJackEnumerator ){
		const int countJacks = pJackEnumerator->CountJacks();
		if( idx < countJacks )
			return pJackEnumerator->GetJack( idx );
		idx -= countJacks;
	}

	if( auto pJackEnumerator = decorator_cast<const JackEnumerator*>(m_pIndicatorCollection.get()); pJackEnumerator ){
		const int countJacks = pJackEnumerator->CountJacks();
		if( idx < countJacks )
			return pJackEnumerator->GetJack( idx );
		idx -= countJacks;
	}

	if( auto pJackEnumerator = decorator_cast<const JackEnumerator*>(m_pCargoCollection.get()); pJackEnumerator ){
		const int countJacks = pJackEnumerator->CountJacks();
		if( idx < countJacks )
			return pJackEnumerator->GetJack( idx );
		idx -= countJacks;
	}

	if( auto pJackEnumerator = decorator_cast<const JackEnumerator*>(m_pTimerCollection.get()); pJackEnumerator ){
		const int countJacks = pJackEnumerator->CountJacks();
		if( idx < countJacks )
			return pJackEnumerator->GetJack( idx );
		idx -= countJacks;
	}

	if( auto pJackEnumerator = decorator_cast<const JackEnumerator*>(m_pPulseCounterCollection.get()); pJackEnumerator ){
		const int countJacks = pJackEnumerator->CountJacks();
		if( idx < countJacks )
			return pJackEnumerator->GetJack( idx );
		idx -= countJacks;
	}

	if( auto pJackEnumerator = decorator_cast<const JackEnumerator*>(m_pCameraCollection.get()); pJackEnumerator ){
		const int countJacks = pJackEnumerator->CountJacks();
		if( idx < countJacks )
			return pJackEnumerator->GetJack( idx );
	}

	std::ostringstream stream;
	stream << "Out of range!" << std::endl;
	stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
	throw std::range_error( stream.str() );
}

}

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
	: m_Frame			{ Identity<Length,One> }
	, m_pLogbook		{ nullptr }
	, m_SimulationSteps	{ 0 }
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
	} 
	else if( !m_pTrackSystem->IsValid() )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has invalid TrackSystem attached." << std::endl;
		return false;
	} 

	if( !m_pFleet )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has no Fleet attached." << std::endl;
	}
	else if( !m_pFleet->IsValid() )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has invalid Fleet attached." << std::endl;
		return false;
	}

	if( !m_pSignalCollection )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has no SignalCollection attached." << std::endl;
	}
	else if( !m_pSignalCollection->IsValid() ){
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has invalid SignalCollection attached." << std::endl;
		return false;
	}

	if( !m_pIndicatorCollection )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has no IndicatorCollection attached." << std::endl;
	}
	else if( !m_pIndicatorCollection->IsValid() )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has invalid IndicatorCollection attached." << std::endl;
		return false;
	}

	if( !m_pCargoCollection )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has no CargoCollection attached." << std::endl;
	}
	else if( !m_pCargoCollection->IsValid() )
	{
		std::cout << Verbosity::detailed << "Module '" << Reference( "name" ) << " id:" << ID() << "' has invalid CargoCollection attached." << std::endl;
		return false;
	}

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

void Module_Imp::SetLogbook( cmnd::Logbook* pLogbook )
{
	m_pLogbook = pLogbook;
}

bool Module_Imp::Process( std::unique_ptr<cmnd::Command> Command )
{
	if( Command  )
	{
		m_Play.Push( std::move( Command ) );
		return true;
	}

	return false;
}

bool Module_Imp::Execute( std::unique_ptr<cmnd::Command> Command )
{
	if( Command  )
	{
		Command->TimeStamp( m_SimulationSteps );
		if( m_pLogbook )
			m_pLogbook->Log( *Command );
		m_History.Push( Command->Clone() );
		Command->Execute();
		return true;
	}

	return false;
}

bool Module_Imp::Replay( std::unique_ptr<cmnd::Macro> pMacro )
{
	if( pMacro && !pMacro->Empty() && !IsReplaying() )
	{
		m_Replay.Swap( *pMacro );
		DoReplay();
		return true;
	}

	return false;
}

bool Module_Imp::Replay()
{
	if( !IsReplaying() && !m_History.Empty() )
	{
		m_Replay.Swap( m_History );
		DoReplay();
		return true;
	}

	return false;
}

bool Module_Imp::IsReplaying() const noexcept
{
	return !m_Replay.Empty();
}

void Module_Imp::StopReplay() noexcept
{
	if( IsReplaying() )
	{
		std::clog << Verbosity::detailed << "Replay stopped with " << m_Replay.Size() << " commands left." << std::endl;
		m_Replay.Clear();
		m_JackOnReplayStop.Pulse();
	}
}

bool Module_Imp::Undo()
{
	return false;
}

bool Module_Imp::Redo()
{
	return false;
}

void Module_Imp::Registered( Scene& scene ) noexcept
{
	if( m_pTrackSystem )
		scene.Register( *m_pTrackSystem );
	if( m_pFleet )
		scene.Register( *m_pFleet );
}

void Module_Imp::Unregistered( Scene& scene ) noexcept
{
	if( m_pFleet )
		scene.Unregister( *m_pFleet );
	if( m_pTrackSystem )
		scene.Unregister( *m_pTrackSystem );
}

bool Module_Imp::Start()
{
	m_SimulationSteps = 0;
	return true;
}

void Module_Imp::Idle()
{}

void Module_Imp::PreUpdate()
{}

void Module_Imp::Update( Time dt )
{
	++m_SimulationSteps; // number of the latest finished simulation step, so the first step is 1, not 0.

	if( IsReplaying() )
	{
		while( auto pCommand = m_Replay.PopFront( m_SimulationSteps ) ){
			assert( m_SimulationSteps == pCommand->TimeStamp() );
			Execute( std::move( pCommand ) );

			if( m_Replay.Empty() ){
				std::clog << Verbosity::detailed << "Replay finished." << std::endl;
				m_JackOnReplayStop.Pulse();
			}
		}
	}

	while( auto pCommand = m_Play.PopFront() ){
		if( pCommand->InfluencesSimulationResult() )
			StopReplay();	// if some command is to be executed, end replay since typically 
							// comands will not work on different targets etc...

		pCommand->SetAllTimeStamps( m_SimulationSteps );
		pCommand->Freeze();

		Execute( std::move( pCommand ) );
	}
}

void Module_Imp::Pause() noexcept
{}

void Module_Imp::Resume() noexcept
{}

void Module_Imp::Stop() noexcept
{}

Jack& Module_Imp::JackOnReplayStart() noexcept{
	return m_JackOnReplayStart;
}

Jack& Module_Imp::JackOnReplayStop() noexcept{
	return m_JackOnReplayStop;
}

int Module_Imp::CountJacks() const
{
	int count = 2;

	if( auto pJackEnumerator = decorator_cast<JackEnumerator*>(m_pTrackSystem.get()) )
		count += pJackEnumerator->CountJacks();

	if( auto pJackEnumerator = decorator_cast<JackEnumerator*>(m_pFleet.get()) )
		count += pJackEnumerator->CountJacks();

	if( auto pJackEnumerator = decorator_cast<JackEnumerator*>(m_pSignalCollection.get()) )
		count += pJackEnumerator->CountJacks();

	if( auto pJackEnumerator = decorator_cast<JackEnumerator*>(m_pIndicatorCollection.get()) )
		count += pJackEnumerator->CountJacks();

	if( auto pJackEnumerator = decorator_cast<JackEnumerator*>(m_pCargoCollection.get()) )
		count += pJackEnumerator->CountJacks();

	if( auto pJackEnumerator = decorator_cast<JackEnumerator*>(m_pTimerCollection.get()) )
		count += pJackEnumerator->CountJacks();

	if( auto pJackEnumerator = decorator_cast<JackEnumerator*>(m_pPulseCounterCollection.get()) )
		count += pJackEnumerator->CountJacks();

	if( auto pJackEnumerator = decorator_cast<JackEnumerator*>(m_pTimerCollection.get()) )
		count += pJackEnumerator->CountJacks();

	return count;
}

const Jack& Module_Imp::_GetJack( int idx ) const
{
	switch( idx ){
	case 0:
		return m_JackOnReplayStart;
	case 1:
		return m_JackOnReplayStop;
	}

	idx -= 2;

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

void Module_Imp::DoReplay()
{
	m_Play.Clear();
	m_SimulationSteps = 0;
	std::clog << Verbosity::detailed << "Replay started with " << m_Replay.Size() << " commands." << std::endl;
	m_JackOnReplayStart.Pulse();
}

}

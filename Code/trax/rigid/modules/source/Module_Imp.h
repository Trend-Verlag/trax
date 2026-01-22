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

#pragma once

#include "../Module.h"
#include "trax/ImplementationHelper.h"
#include "trax/source/SocketRegistry_Imp.h"

namespace trax{

	typedef ObjectID_Imp<Module> Module_Base;

	class Module_Imp :  public Module_Base,
						public SocketRegistry_Imp,
						public JackEnumerator
	{
	public:
		Module_Imp( bool bCreateCollections = true ) noexcept;


		//Module:
		const char*	TypeName() const noexcept override{
			return "Module";
		}

		bool IsValid() const noexcept override;

		void RegisterCollections( Scene& withScene ) const noexcept override;

		void UnregisterCollections( Scene& withScene ) const noexcept override;

		void SetFrame( const spat::Frame<Length,One>& frame ) noexcept override;

		const spat::Frame<Length,One>& GetFrame() const noexcept override;

		void SetVolume( const spat::Box<Length>& volume ) noexcept override;

		const spat::Box<Length>& GetVolume() const noexcept override;


		void Attach( std::shared_ptr<TrackSystem> pTrackSystem ) noexcept override;

		std::shared_ptr<TrackSystem> GetTrackSystem() const noexcept override;

		void Attach( std::shared_ptr<Fleet> pFleet ) noexcept override;

		std::shared_ptr<Fleet> GetFleet() const noexcept override;

		void Attach( std::shared_ptr<SignalCollection> pSignalCollection ) noexcept override;

		std::shared_ptr<SignalCollection> GetSignalCollection() const noexcept override;

		void Attach( std::shared_ptr<IndicatorCollection> pIndicatorCollection ) noexcept override;

		std::shared_ptr<IndicatorCollection> GetIndicatorCollection() const noexcept override;

		void Attach( std::shared_ptr<CargoCollection> pCargoCollection ) noexcept override;

		std::shared_ptr<CargoCollection> GetCargoCollection() const noexcept override;

		void Attach( std::shared_ptr<TimerCollection> pTimerCollection ) noexcept;

		std::shared_ptr<TimerCollection> GetTimerCollection() const noexcept;

		void Attach( std::shared_ptr<PulseCounterCollection> pPulseCounterCollection ) noexcept;
		
		std::shared_ptr<PulseCounterCollection> GetPulseCounterCollection() const noexcept;

		void Attach( std::shared_ptr<CameraCollection> pCameraCollection ) noexcept;

		std::shared_ptr<CameraCollection> GetCameraCollection() const noexcept;

		void Take( Module& fromModule ) override;

		void Clear() noexcept override;

		void ClearCollections() override;


		int CountJacks() const override;
	protected:
		const Jack& _GetJack( int idx ) const override;

	private:
		spat::Frame<Length, One>	m_Frame;
		spat::Box<Length>			m_Volume;

		std::shared_ptr<TrackSystem>			m_pTrackSystem;
		std::shared_ptr<Fleet>					m_pFleet;
		std::shared_ptr<SignalCollection>		m_pSignalCollection;
		std::shared_ptr<IndicatorCollection>	m_pIndicatorCollection;
		std::shared_ptr<CargoCollection>		m_pCargoCollection;
		std::shared_ptr<TimerCollection>		m_pTimerCollection;
		std::shared_ptr<PulseCounterCollection> m_pPulseCounterCollection;
		std::shared_ptr<CameraCollection>		m_pCameraCollection;
	};
}

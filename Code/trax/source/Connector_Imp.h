//	trax track library
//	AD 2018 
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

#include "trax/Connector.h"
#include "Plug_Imp.h"

namespace trax{

	class Connector_Imp :	public virtual Connector,
							public PlugEnumerator,
							public JackEnumerator{
	public:
		Connector_Imp( int cntSlots );

		bool IsValid() const noexcept override;

		void Disconnect() override;

		int Slot( int slot, std::shared_ptr<TrackBuilder> pTrack, Track::EndType trackend, bool connectAnonymous = false ) override;

		int Slot( std::shared_ptr<TrackBuilder> pTrack, Track::EndType trackend, bool connectAnonymous = false ) override;

		std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> Slot( int slot ) const noexcept override;

		int Slot( const Track& track, Track::EndType trackend ) const noexcept override;

		int CntSlots() const noexcept override;

		void SwapSlots( int slotA, int slotB ) override;

		bool IsComplete() const noexcept override;

		void Clear( int slot ) override;

		void Clear() override;

		MultiPlug& PlugToToggle() noexcept override;

		const MultiPlug& PlugToToggle() const noexcept override;

		Jack& JackOnChange() noexcept override;

		const Jack& JackOnChange() const noexcept override;

		void RegisterSockets( SocketRegistry& module ) override;

		void UnregisterSockets( SocketRegistry& module ) override;

		int CountPlugs() const override;

		int CountJacks() const noexcept override;

		void UserData( ConnectorUserData* pData ) noexcept override;

		ConnectorUserData* UserData() const noexcept override;
	protected:
		const Plug & _GetPlug(int idx) const override;
		const Jack & _GetJack(int idx) const override;

		bool CheckSlot( int slot, std::ostream& os, Length e_distance, Angle e_kink, Angle e_twist ) const noexcept;
	private:
		std::vector<std::pair<std::shared_ptr<TrackBuilder>,Track::EndType>> m_Slots;

		Jack_Imp m_JackOnChange{ "JackOnChange" };
		MultiPlug_Imp<Toggle_Pug<Connector_Imp>> m_PlugToToggle;
		ConnectorUserData* m_pUserData = nullptr;
	};
}
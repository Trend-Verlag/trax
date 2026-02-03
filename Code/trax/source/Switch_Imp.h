//	trax track library
//	AD 2014 
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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/Switch.h"
#include "Connector_Imp.h"
#include "trax/ImplementationHelper.h"

namespace trax{
	using namespace spat;

	typedef ObjectID_Imp<NarrowSwitch> NarrowSwitch_Base;

	class NarrowSwitch_Imp :	public NarrowSwitch_Base,
								public Connector_Imp{
	public:
		NarrowSwitch_Imp( unsigned char numBranches );


		// Connector:
		ConnectorType GetConnectorType() const noexcept override;

		const char* TypeName() const noexcept override;

		void Toggle( bool pulse = true ) override;

		void Set( const Track& trackA, EndType trackendA, const Track& trackB, EndType trackendB, bool pulse = true ) override;

		using Connector_Imp::Slot;

		int Slot( int slot, std::shared_ptr<TrackBuilder> pTrack, EndType trackend, bool connectAnonymous = false ) override;

		bool Check( std::ostream& os, Length e_distance = epsilon__length, Angle e_kink = epsilon__angle, Angle e_twist = epsilon__angle ) const noexcept override;

		void GetCenter( Frame<Length,One>& center ) const override;

		void RegisterSockets( SocketRegistry& module ) override;

		void UnregisterSockets( SocketRegistry& module ) override;


		// NarrowSwitch:
		Status Set( Status to, bool pulse = true ) override;

		Status Get() const noexcept override;

		void NarrowTrack(std::shared_ptr<TrackBuilder> pNarrowTrack,EndType trackend) override;

		std::pair<std::shared_ptr<TrackBuilder>,EndType> NarrowTrack() const noexcept override;

		void ClearNarrowTrack() override;

		void StraightTrack(std::shared_ptr<TrackBuilder> pStaightTrack,EndType trackend) override;

		std::pair<std::shared_ptr<TrackBuilder>,EndType> StraightTrack() const noexcept override;

		void ClearStraightTrack() override;

		void DivergedTrack( int divTrackID, std::shared_ptr<TrackBuilder> pDivergedTrack,EndType trackend) override;

		std::pair<std::shared_ptr<TrackBuilder>,EndType> DivergedTrack( int divTrackID ) const noexcept override;

		void ClearDivergedTrack( int divTrackID ) override;

		int CntDivergedTracks() const noexcept override;

		Location Bifurcation() const override;


		Jack& JackOnGo() noexcept override;

		MultiPlug& PlugToGo() noexcept override;

		Jack& JackOn( Status status ) override;

		MultiPlug& PlugTo( Status status ) override;

		Jack& JackOnDivergedTrack( int divTrackID ) noexcept override;

		MultiPlug& PlugToDivergedTrack( int divTrackID ) noexcept override;


		int CountPlugs() const override;

		int CountJacks() const noexcept override;
	protected:
		const Plug & _GetPlug( int idx ) const override;
		const Jack & _GetJack( int idx ) const override;

	private:
		Status m_Status;

		Jack_Imp m_JackOnGo{ "JackOnGo" };
		std::vector<std::unique_ptr<Jack_Imp>> m_JacksOnBranches;

		MultiPlug_Imp<StatusHolder_Plug<NarrowSwitch_Imp>> m_PlugToGo;
		std::vector<std::unique_ptr<MultiPlug_Imp<StatusHolder_Plug<NarrowSwitch_Imp>>>> m_PlugsToBranches;
	};


	class Switch_Imp :	public Switch,
						public NarrowSwitch_Imp
	{
	public:
		Switch_Imp();

		// Connector:
		ConnectorType GetConnectorType() const noexcept override;

		const char* TypeName() const noexcept override;


		// Switch:
		using NarrowSwitch_Imp::DivergedTrack;

		void DivergedTrack( std::shared_ptr<TrackBuilder> pDivergedTrack, EndType trackend ) override;

		std::pair<std::shared_ptr<TrackBuilder>,EndType> DivergedTrack() const noexcept override;

		using NarrowSwitch_Imp::ClearDivergedTrack;

		void ClearDivergedTrack() override;

		void SwapTracks() override;

		bool IsNormal( bool bStopAtFirstTrack = true, Length maxDistance = infinite__length ) const override;

		bool Normalize() override;

		bool IsY( bool bStopAtFirstTrack = true, Length maxDistance = infinite__length, AnglePerLength margin = epsilon__angle / 80_m ) const override;

		bool IsHorizontal( bool bStopAtFirstTrack = true, Length maxDistance = infinite__length ) const override;

		bool IsVertical( bool bStopAtFirstTrack = true, Length maxDistance = infinite__length ) const override;

		bool BranchLeftOrRight( bool bStopAtFirstTrack = true, Length maxDistance = infinite__length ) const override;

		bool BranchUpOrDown( bool bStopAtFirstTrack = true, Length maxDistance = infinite__length ) const override;


		Jack& JackOnBranch() noexcept override;

		MultiPlug& PlugToBranch() noexcept override;

	private:
		Length MinimumOutgoingTrackEndpoints( Position<Length>& posStraight, Position<Length>& posDiverged, bool bStopAtFirstTrack, Length maxDistance ) const;
		void StreamSwitch( std::ostream& ostream ) const;
	};


	class ThreeWaySwitch_Imp :  public ThreeWaySwitch,
								public NarrowSwitch_Imp
	{
	public:
		ThreeWaySwitch_Imp();


		// Connector:
		ConnectorType GetConnectorType() const noexcept override;

		const char* TypeName() const noexcept override;


		// ThreeWaySwitch:

		void DivergedTrack1(std::shared_ptr<TrackBuilder> pDivergedTrack,EndType trackend) override;

		std::pair<std::shared_ptr<TrackBuilder>,EndType> DivergedTrack1() const noexcept override;

		void ClearDivergedTrack1() override;

		void DivergedTrack2(std::shared_ptr<TrackBuilder> pDivergedTrack,EndType trackend) override;

		std::pair<std::shared_ptr<TrackBuilder>,EndType> DivergedTrack2() const noexcept override;

		void ClearDivergedTrack2() override;

		void SwapStraightWithDiverged() override;

		void SwapStraightWithDiverged2() override;

		void SwapDivergedWithDiverged2() override;

		bool IsNormal() const override;

		bool Normalize() override;

		Status StatusToLeft() const override;

		Status StatusToMiddle() const override;

		Status StatusToRight() const override;

		Jack& JackOnBranch1() noexcept override;

		MultiPlug& PlugToBranch1() noexcept override;

		Jack& JackOnBranch2() noexcept override;

		MultiPlug& PlugToBranch2() noexcept override;

	private:
		bool MinimumOutgoingTrackEndpoints( Position<Length>& posStraight, Position<Length>& posDiverged1, Position<Length>& posDiverged2 ) const;
	};


	typedef ObjectID_Imp<SingleSlipSwitch> SingleSlipSwitch_Base;

	class SingleSlipSwitch_Imp :	public SingleSlipSwitch_Base,
									public Connector_Imp
	{
	public:
		SingleSlipSwitch_Imp();

		// Connector:
		ConnectorType GetConnectorType() const noexcept override;

		const char* TypeName() const noexcept override;

		void Toggle( bool pulse = true ) override;

		void Set( const Track& trackA, EndType trackendA, const Track& trackB, EndType trackendB, bool pulse = true ) override;

		bool Check( std::ostream& os, Length e_distance = epsilon__length, Angle e_kink = epsilon__angle, Angle e_twist = epsilon__angle ) const noexcept override;

		void GetCenter( Frame<Length,One>& center ) const noexcept override;


		// SingleSlipSwitch
		Status Set( Status to, bool pulse = true ) override;

		Status Get() const noexcept override;

		void SetCenter( const Frame<Length,One>& center ) noexcept override;

		Jack& JackOn( Status status ) override;

		MultiPlug& PlugTo( Status status ) override;


		// Connector
		using Connector_Imp::Slot;

		int Slot( int slot,std::shared_ptr<TrackBuilder> pTrack, EndType trackend, bool connectAnonymous = false ) override;

		void RegisterSockets( SocketRegistry& module ) override;

		void UnregisterSockets( SocketRegistry& module ) override;

		int CountPlugs() const override;

		int CountJacks() const noexcept override;
	protected:
		const Plug & _GetPlug(int idx) const override;
		const Jack & _GetJack(int idx) const override;

	private:
		Status m_Status;
		Frame<Length,One> m_Center;

		Jack_Imp m_JackOnGo1{ "JackOnGo1" };
		Jack_Imp m_JackOnGo2{ "JackOnGo2" };
		Jack_Imp m_JackOnBranch{ "JackOnBranch" };

		MultiPlug_Imp<StatusHolder_Plug<SingleSlipSwitch_Imp>>	m_PlugToGo1;
		MultiPlug_Imp<StatusHolder_Plug<SingleSlipSwitch_Imp>>	m_PlugToGo2;
		MultiPlug_Imp<StatusHolder_Plug<SingleSlipSwitch_Imp>>	m_PlugToBranch;
	};


	typedef ObjectID_Imp<DoubleSlipSwitch> DoubleSlipSwitch_Base;

	class DoubleSlipSwitch_Imp :	public DoubleSlipSwitch_Base,
									public Connector_Imp
	{
	public:
		DoubleSlipSwitch_Imp();

		// Connector:
		ConnectorType GetConnectorType() const noexcept override;

		const char* TypeName() const noexcept override;

		void Toggle( bool pulse = true ) override;

		void Set( const Track& trackA, EndType trackendA, const Track& trackB, EndType trackendB, bool pulse = true ) override;

		bool Check( std::ostream& os, Length e_distance = epsilon__length, Angle e_kink = epsilon__angle, Angle e_twist = epsilon__angle ) const noexcept override;

		void GetCenter( Frame<Length,One>& center ) const noexcept override;



		// DoubleSlipSwitch
		Status Set( Status to, bool pulse = true ) override;

		Status Get() const noexcept override;

		void SetCenter( const Frame<Length,One>& center ) noexcept override;

		Jack& JackOn( Status status ) override;

		MultiPlug& PlugTo( Status status ) override;


		// Connector
		using Connector_Imp::Slot;

		int Slot( int slot,std::shared_ptr<TrackBuilder> pTrack, EndType trackend, bool connectAnonymous = false ) override;

		void RegisterSockets( SocketRegistry& module ) override;

		void UnregisterSockets( SocketRegistry& module ) override;

		int CountPlugs() const override;

		int CountJacks() const noexcept override;
	protected:
		const Plug & _GetPlug(int idx) const override;
		const Jack & _GetJack(int idx) const override;

	private:
		Status m_Status;
		Frame<Length,One> m_Center;

		Jack_Imp m_JackOnGo1{ "JackOnGo1" };
		Jack_Imp m_JackOnGo2{ "JackOnGo2" };
		Jack_Imp m_JackOnBranch1{ "JackOnBranch1" };
		Jack_Imp m_JackOnBranch2{ "JackOnBranch2" };

		MultiPlug_Imp<StatusHolder_Plug<DoubleSlipSwitch_Imp>>	m_PlugToGo1;
		MultiPlug_Imp<StatusHolder_Plug<DoubleSlipSwitch_Imp>>	m_PlugToGo2;
		MultiPlug_Imp<StatusHolder_Plug<DoubleSlipSwitch_Imp>>	m_PlugToBranch1;
		MultiPlug_Imp<StatusHolder_Plug<DoubleSlipSwitch_Imp>>	m_PlugToBranch2;
	};
}
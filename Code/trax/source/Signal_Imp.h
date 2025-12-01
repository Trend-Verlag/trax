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

#include "trax/Signal.h"
#include "trax/Location.h"
#include "Plug_Imp.h"

#include <map>

#if defined(_MSC_VER)
#	pragma warning(disable: 4250) // virtual inheritance of methods via dominance
#endif

namespace trax{

	typedef ObjectID_Imp<Signal> Signal_Base;

	class Signal_Imp :	public Signal_Base,
						public PlugEnumerator,
						public JackEnumerator
	{
	public:
		Signal_Imp();

		const char*	TypeName() const noexcept override;

		Status Set( Status status, bool pulse = true ) override;

		using Signal_Base::Set;

		void Toggle( bool pulse = true ) override;

		Status Get() const noexcept override;

		int CountStatus() const override;
		
		void Attach( TrackBuilder* pTrackBuilder ) noexcept override;

		TrackBuilder* TrackAttached() const noexcept override;

		void GetTrackRange( TrackRange& trackRange ) const override;

		Location GetLocation() const override;

		MultiPlug& PlugToToggle() noexcept override;

		Jack& JackOnChange() noexcept override;
	
		Jack& JackOn( Status status ) override;

		Jack& JackOnChangeTo( Status status ) override;

		MultiPlug& PlugTo( Status status ) override;

		void RegisterSockets( SocketRegistry& module ) override;

		void UnregisterSockets( SocketRegistry& module ) override;

		const std::string& Reference( const std::string& name ) const override{
			return Signal_Base::Reference( name );
		}

		void Reference( const std::string& name, const std::string& reference ) override{
			return Signal_Base::Reference( name, reference );
		}

		IDType ID() const noexcept override{
			return Signal_Base::ID();
		}

		void ID( IDType id ) noexcept override{
			return Signal_Base::ID( id );
		}

		int CountPlugs() const override;

		int CountJacks() const noexcept override;

	protected:
		const Plug& _GetPlug( int idx ) const override;
		const Jack& _GetJack( int idx ) const override;

	private:
		Status			m_Status;
		TrackBuilder*	m_pTrackBuilder;

		Jack_Imp m_JackOnChange{ "JackOnChange" };
		MultiPlug_Imp<Toggle_Pug<Signal_Imp>> m_PlugToToggle;
	};


	class SignalClear_Imp : public Signal_Imp{
	public:
		SignalClear_Imp();

		int CountStatus() const noexcept override;

		Jack& JackOn( Status status ) override;

		Jack& JackOnChangeTo( Status status ) override;

		MultiPlug& PlugTo( Status status ) override;

		Jack& JackOnClear() noexcept;

		Jack& JackOnChangeToClear() noexcept;

		MultiPlug& PlugToClear() noexcept;

		void RegisterSockets( SocketRegistry& registry ) override;

		void UnregisterSockets( SocketRegistry& registry ) override;

		int CountPlugs() const override;

		int CountJacks() const noexcept override;

	protected:
		const Plug& _GetPlug( int idx ) const override;
		const Jack& _GetJack( int idx ) const override;

	private:
		Jack_Imp m_JackOnClear{ "JackOnClear" };
		Jack_Imp m_JackOnChangeToClear{ "JackOnChangeToClear" };
		MultiPlug_Imp<StatusHolder_Plug<Signal_Imp>> m_PlugToClear;
	};


	class VelocityControl_Imp : public VelocityControl,
								public Signal_Imp{
	public:
		VelocityControl_Imp() noexcept;

		// Inherited via Signal_Imp
		const char*	TypeName() const noexcept override;

		Status Set( Status status, bool pulse = true ) override;

		void Toggle( bool pulse = true ) override;

		int CountStatus() const override;

		Jack& JackOn( Status status ) override;

		Jack& JackOnChangeTo( Status status ) override;

		MultiPlug& PlugTo( Status status ) override;

		void RegisterSockets( SocketRegistry& registry ) override;

		void UnregisterSockets( SocketRegistry& registry ) override;

		int CountPlugs() const override;

		int CountJacks() const noexcept override;


		// Inherited via VelocityControl
		bool AffectTravelVelocity( Status status, bool bTravel ) override;
		
		bool AffectTravelVelocity( Status status ) const override;

		void SetVelocity( Status status, Velocity velocityMin, Velocity velocityMax ) override;

		void SetVelocity( Status status, Velocity velocity ) override;

		void VelocityMin( Status status, Velocity velocityMin ) override;

		void VelocityMax( Status status, Velocity velocityMax ) override;

		Velocity VelocityMin( Status status ) const override;

		Velocity VelocityMax( Status status ) const override;

		bool IsValidState( Status status ) const override;

		void Remove( Status status ) override;

		void Clear() noexcept override;

		void StopDistance( Length distance ) noexcept override;

		Length StopDistance() const noexcept override;

		Jack& JackOnVelocity( Status status ) override;

		MultiPlug& PlugToVelocity( Status status ) override;
	protected:
		const Plug& _GetPlug( int idx ) const override;
		const Jack& _GetJack( int idx ) const override;
	private:
		struct ControlState{
			ControlState( VelocityControl_Imp& parent, Status status, Velocity minVelocity, Velocity maxVelocity );
			
			// Todo: use Interval<Real> m_Velocity;
			Velocity m_MinVelocity;
			Velocity m_MaxVelocity;
			bool m_bAffectTravelVelocity;

			Jack_Imp m_JackOnVelocity;
			Jack_Imp m_JackOnChangeToVelocity;
			MultiPlug_Imp<StatusHolder_Plug<VelocityControl_Imp>> m_PlugToVelocity;
		};

		std::map<Status,ControlState> m_ControlStates;
		Length m_StopDistance;
	};

	class JumpSite_Imp :	public JumpSite,
							public SignalClear_Imp{
	public:
		JumpSite_Imp();

		const char*	TypeName() const noexcept override;

		Status Set( Status status, bool pulse = true ) override;

		void Toggle( bool pulse = true ) override;

		int CountStatus() const noexcept override;

		void Target( const Location& target ) noexcept override;

		void Target( std::shared_ptr<Depot> pDepot ) noexcept override;

		bool IsTargetLocation() const noexcept override;
	
		const Location& TargetLocation() const noexcept override;
		
		std::shared_ptr<Depot> TargetDepot() const noexcept override;

		Jack& JackOnJump() noexcept override;

		Jack& JackOnChangeToJump() noexcept override;

		MultiPlug& PlugToJump() noexcept override;

		Jack& JackOnClear() noexcept override;

		Jack& JackOnChangeToClear() noexcept override;

		MultiPlug& PlugToClear() noexcept override;

		Jack& JackOn( Status status ) override;

		Jack& JackOnChangeTo( Status status ) override;

		MultiPlug& PlugTo( Status status ) override;

		void RegisterSockets( SocketRegistry& module ) override;

		void UnregisterSockets( SocketRegistry& module ) override;

		int CountPlugs() const override;

		int CountJacks() const noexcept override;
	protected:
		const Plug& _GetPlug( int idx ) const override;
		const Jack& _GetJack( int idx ) const override;
	private:
		bool m_bTrackLocation;
		Location m_TargetLocation;
		std::shared_ptr<Depot> m_pTargetDepot;

		Jack_Imp m_JackOnJump{ "JackOnJump" };
		Jack_Imp m_JackOnChangeToJump{ "JackOnChangeToJump" };
		MultiPlug_Imp<StatusHolder_Plug<Signal_Imp>> m_PlugToJump;
	};

	class TrackEndTransition_Imp :	public TrackEndTransition,
									public SignalClear_Imp{
	public:
		TrackEndTransition_Imp( Track* pToTrack, Track::EndType toEnd );

		int CountStatus() const noexcept override;

		void Set( Track* pToTrack, Track::EndType toEnd ) noexcept;

		using TrackEndTransition::Set;

		Track* From( Track::EndType& end ) const noexcept override;

		Track* From() const noexcept override;

		Track* To( Track::EndType& end ) const noexcept override;

		Track* To() const noexcept override;
	private:
		Track*			m_pFromTrack;
		Track::EndType	m_FromEnd;
		Track*			m_pToTrack;
		Track::EndType	m_ToEnd;
	};
}


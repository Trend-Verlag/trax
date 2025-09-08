//	trax track library
//	AD 2016 
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
#include "trax/Indicator.h"

#include "Plug_Imp.h"
#include "Jack_Imp.h"
#include "trax/ImplementationHelper.h"

namespace trax{

	typedef Pose_Imp<ObjectID_Imp<Indicator>> Multicator_Base;

	class Multicator :	public Multicator_Base,
						public PlugEnumerator,
						public JackEnumerator{
	public:
		Multicator();

		// Inherited via Multicator_Base
		void Toggle( bool pulse = true ) override;

		Status Set( Status status, bool pulse = true ) override;

		Status Get() const override;

		bool IsValidState( Status status ) const override;

		void RotateWithStatus( Status status, Real angle ) override;

		Real RotateWithStatus( Status status ) const override;

		Jack& JackOn( Status status ) override;

		Plug& PlugTo( Status status ) override;

		Plug& PlugToToggle() noexcept override;

		Jack& JackOnChange() noexcept override;

		void RegisterSockets( SocketRegistry& registry ) override;

		void UnregisterSockets( SocketRegistry& registry ) override;

		// Inherited via PlugEnumerator
		const char* TypeName() const noexcept override;
		
		const std::string& Reference( const std::string & name ) const override;
		
		IDType ID() const noexcept override;
		
		int CountPlugs() const override;

		// Inherited via JackEnumerator
		int CountJacks() const override;

	protected:
		const Plug& _GetPlug( int idx ) const override;
		const Jack& _GetJack( int idx ) const override;

	private:
		Indicator::Status m_Status;
		bool m_StatusChangeBlocker;

		struct IndicatorState{
			IndicatorState( Multicator& parent, Indicator::Status status );

			Real											m_RotAngle;
			Jack_Imp										m_JackOnStatus;
			MultiPlug_Imp<StatusHolder_Plug<Multicator>>	m_PlugToStatus;
		};

		std::map<Status, IndicatorState> m_IndicatorStates;

		// Create if not already created.
		void Create( Status status );

		Jack_Imp m_JackOnChange{ "JackOnChange" };
		Toggle_Pug<Multicator>	m_PlugToToggle;
	};

}

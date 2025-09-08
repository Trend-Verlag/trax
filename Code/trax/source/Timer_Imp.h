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

#include "trax/Timer.h"
#include "trax/ObjectID.h"
#include "Plug_Imp.h"

namespace trax{

	using TimerBase = ObjectID_Imp<Timer>;

	class Timer_Imp :	public TimerBase,
						public PlugEnumerator,
						public JackEnumerator
	{
	public:
		Timer_Imp();
		Timer_Imp( const Timer_Imp& ) = delete;
		Timer_Imp( Timer_Imp&& ) = delete;
		~Timer_Imp() = default;

		Timer_Imp& operator=( const Timer_Imp& ) = delete;
		Timer_Imp& operator=( Timer_Imp&& ) = delete;

		// Inherited via ObjectID_Imp
		const char* TypeName() const noexcept override;

		void Start() override;

		bool IsStarted() const override;

		void Reset() override;

		void Pause() override;

		void Lapse( Time timeLapse ) override;

		Time Lapse() const override;

		void Periodic( bool bPeriodic ) override;

		bool Periodic() const override;

		void SetTime( Time time ) override;
		
		Time GetTime() const override;

		MultiPlug& PlugToStart() noexcept override;

		MultiPlug& PlugToReset() noexcept override;

		MultiPlug& PlugToPause() noexcept override;

		Jack& JackOnStart() noexcept override;

		Jack& JackOnReset() noexcept override;

		Jack& JackOnPause() noexcept override;

		Jack& JackOnTick() noexcept override;

		void RegisterSockets( SocketRegistry& registry ) override;

		void UnregisterSockets( SocketRegistry& registry ) override;

		// Inherited via PlugEnumerator

		const std::string& Reference( const std::string& name ) const override;

		IDType ID() const noexcept override;

		int CountPlugs() const override;


		// Inherited via JackEnumerator
		int CountJacks() const noexcept override;


		void Update() override;
	protected:
		const Jack& _GetJack( int idx ) const override;
		const Plug& _GetPlug( int idx ) const override;

	private:
		Time	m_TimeLapse;
		bool	m_bPeriodic;
		Time	m_Timer;
		bool	m_bStarted;

		MultiPlug_Imp<Plug_Imp_ParentPointer<Timer_Imp>> m_PlugToStart;
		MultiPlug_Imp<Plug_Imp_ParentPointer<Timer_Imp>> m_PlugToReset;
		MultiPlug_Imp<Plug_Imp_ParentPointer<Timer_Imp>> m_PlugToPause;

		Jack_Imp m_JackOnStart{ "JackOnStart" };
		Jack_Imp m_JackOnReset{ "JackOnReset" };
		Jack_Imp m_JackOnPause{ "JackOnPause" };
		Jack_Imp m_JackOnTick{ "JackOnTick" };
	};

	//class TimerCollection_Imp : public JackEnumerator_Imp<Container_Imp<Timer, TimerCollection>>{
	//public:
	//	std::string	TypeName() const override;

	//	void Update() override;


	//	// Inherited via JackEnumerator
	//	//const std::string& Reference( const std::string & name ) const override{
	//	//	static std::string empty;
	//	//	return empty;
	//	//}

	//	//IDType ID() const override{
	//	//	return IDType();
	//	//}
	//};

}

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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/LogicElements.h"
#include "trax/ObjectID.h"
#include "Plug_Imp.h"

namespace trax{

	class PulseCounter_Imp :	public ObjectID_Imp<PulseCounter>,
								public PlugEnumerator,
								public JackEnumerator
	{
	public:
		PulseCounter_Imp();

		const char*	TypeName() const noexcept override{
			return "PulseCounter";
		}

		const std::string& Reference( const std::string& name ) const override{
			return ObjectID_Imp<PulseCounter>::Reference( name );
		}

		void Reference( const std::string& name, const std::string& reference ) override{
			return ObjectID_Imp<PulseCounter>::Reference( name, reference );
		}

		IDType ID() const noexcept override{
			return ObjectID_Imp<PulseCounter>::ID();
		}

		void ID( IDType id ) noexcept override{
			return ObjectID_Imp<PulseCounter>::ID( id );
		}

		// Inherited via PulseCounter
		void CountUp() noexcept override;

		void CountDown() noexcept override;

		void Threshold( int threshold ) override;

		int Threshold() const noexcept override;

		void Counter( int counter ) override;

		int Counter() const noexcept override;

		void Reset() noexcept override;

		MultiPlug& PlugToCountUp() override;

		MultiPlug& PlugToCountDown() override;

		MultiPlug& PlugToReset() override;

		Jack& JackOnReachThreshold() override;

		Jack& JackOnLeaveThreshold() override;

		Jack& JackOnCountUp() override;

		Jack& JackOnCountDown() override;

		void RegisterSockets( SocketRegistry& registry ) override;

		void UnregisterSockets( SocketRegistry& registry ) override;

		int CountPlugs() const override;

		// Inherited via JackEnumerator
		int CountJacks() const noexcept override;
	protected:
		const Plug& _GetPlug( int idx ) const override;
		const Jack& _GetJack( int idx ) const override;

	private:
		MultiPlug_Imp<Plug_Imp_ParentPointer<PulseCounter_Imp>> m_PlugToCountUp;
		MultiPlug_Imp<Plug_Imp_ParentPointer<PulseCounter_Imp>> m_PlugToCountDown;
		MultiPlug_Imp<Plug_Imp_ParentPointer<PulseCounter_Imp>> m_PlugToReset;

		Jack_Imp m_JackOnReachThreshold{ "JackOnReachThreshold" };
		Jack_Imp m_JackOnLeaveThreshold{ "JackOnLeaveThreshold" };
		Jack_Imp m_JackOnCountUp{ "JackOnCountUp" };
		Jack_Imp m_JackOnCountDown{ "JackOnCountDown" };

		int m_Counter;
		int m_Threshold;
	};


	//using PulseCounterCollection_Base = JackEnumerator_Imp<ObjectID_Imp<Container_Imp<PulseCounter,PulseCounterCollection>>>;

	//class PulseCounterCollection_Imp : public PulseCounterCollection_Base{
	//public:
	//	std::string TypeName() const override;

	//	// Inherited via JackEnumerator
	//	const std::string& Reference( const std::string & /*name*/ ) const override{
	//		static std::string empty;
	//		return empty;
	//	}

	//	IDType ID() const noexcept override{
	//		return IDType();
	//	}
	//};

}
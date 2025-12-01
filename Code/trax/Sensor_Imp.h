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
#include "trax/Sensor.h"
#include "trax/ObjectID.h" 
#include "source/Plug_Imp.h"

#include "common/Interval.h"

#if defined(_MSC_VER)
#	pragma warning(disable: 4250) // virtual inheritance of methods via dominance
#endif

namespace trax{

	using namespace spat;

	class Sensor_Imp :	public ObjectID_Imp<Sensor>,
						public JackEnumerator
	{
	public:
		Sensor_Imp();
		Sensor_Imp( const Sensor_Imp& sensor );
		Sensor_Imp( Sensor_Imp&& ) = default;
		~Sensor_Imp() = default;

		Sensor_Imp& operator=( const Sensor_Imp& sensor ) = delete;
		Sensor_Imp& operator=( Sensor_Imp&& ) = delete;

		const char*	TypeName() const noexcept override;

		const std::string& Reference( const std::string& name ) const override{
			return ObjectID_Imp<Sensor>::Reference( name );
		}

		void Reference( const std::string& name, const std::string& reference ) override{
			return ObjectID_Imp<Sensor>::Reference( name, reference );
		}

		IDType ID() const noexcept override{
			return ObjectID_Imp<Sensor>::ID();
		}

		void ID( IDType id ) noexcept override{
			return ObjectID_Imp<Sensor>::ID( id );
		}

		void Trigger( const Event& _event ) override;

		Jack& JackOnTrigger() noexcept override;

		void RegisterSockets( SocketRegistry& module ) override;

		void UnregisterSockets( SocketRegistry& module ) override;

		void Attach( TrackBuilder* pTrackBuilder ) noexcept override;

		TrackBuilder* TrackAttached() const noexcept override;

		int CountJacks() const noexcept override;
	protected:
		const Jack& _GetJack( int idx ) const override;

	private:
		TrackBuilder* m_pTrackBuilder;
		Jack_Imp m_JackOnTrigger{ "JackOnTrigger" };
	};



	//class SensorFilterJack_Imp :	public ObjectID_Imp<SensorFilterJack>,
	//					public JackEnumerator
	//{
	//public:
	//	SensorFilterJack_Imp( int flags );
	//	SensorFilterJack_Imp( const SensorFilterJack_Imp& sensor );
	//	SensorFilterJack_Imp( SensorFilterJack_Imp&& ) = default;
	//	~SensorFilterJack_Imp() = default;

	//	SensorFilterJack_Imp& operator=( const SensorFilterJack_Imp& sensor ) = delete;
	//	SensorFilterJack_Imp& operator=( SensorFilterJack_Imp&& ) = delete;

	//	const char*	TypeName() const noexcept override;

	//	const std::string& Reference( const std::string& name ) const override{
	//		return ObjectID_Imp<SensorFilterJack>::Reference( name );
	//	}

	//	void Reference( const std::string& name, const std::string& reference ) override{
	//		return ObjectID_Imp<SensorFilterJack>::Reference( name, reference );
	//	}

	//	IDType ID() const noexcept override{
	//		return ObjectID_Imp<SensorFilterJack>::ID();
	//	}

	//	void ID( IDType id ) noexcept override{
	//		return ObjectID_Imp<SensorFilterJack>::ID( id );
	//	}

	//	void Trigger( const Event& _event ) override;

	//	int GetEffect() const noexcept override;

	//	void SetEffect( int effect ) noexcept override;

	//	Jack& JackOnTrigger() noexcept override;

	//	void RegisterSockets( SocketRegistry& module ) override;

	//	void UnregisterSockets( SocketRegistry& module ) override;

	//	void Attach( TrackBuilder* pTrackBuilder ) noexcept override;

	//	TrackBuilder* TrackAttached() const noexcept override;

	//	int CountJacks() const noexcept override;
	//protected:
	//	const Jack& _GetJack( int idx ) const override;

	//private:
	//	TrackBuilder* m_pTrackBuilder;
	//	int m_FilterFlags = 0;

	//	Jack_Imp m_JackOnTrigger{ "JackOnTrigger" };
	//};
}
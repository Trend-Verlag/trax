//	trax track library
//	AD 2026 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//
// Copyright (c) 2026 Trend Redaktions- und Verlagsgesellschaft mbH
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
// For further information, please contact: horstmann@traxlibrary.dev

#pragma once

#include "dim/DimensionedValues.h"

#include <chrono>

namespace trax{

	using namespace dim;


	/// \brief A simple clock to measure elapsed time.
	class Clock
	{
	public:
		inline Clock() noexcept{
			Start();
		}


		/// \brief Start the clock.
		inline void Start() noexcept;


		/// \brief Get the elapsed time since the clock 
		/// was started.
		inline Time Elapsed() const noexcept;


		/// \brief Stop the clock and return the elapsed time.
		inline Time Stop() noexcept;
	private:
		std::chrono::steady_clock::time_point m_start;
	};


	/// \brief Compares an accumulated 'simulation time' with accumulated real
	/// world time, needed to calculate the simulation.
	/// 
	/// A simulation factor is calculated as the ratio of simulation time to 
	/// real world time, used to calculate the simulation. So a factor of 1 
	/// means that the simulation is running in real time, a factor of 2 means 
	/// that the simulation is running twice as fast as real time, and a factor 
	/// of 0.5 means that the simulation is running half as fast as real time.
	/// 
	/// Call StartRealTimeMeasurement() before simulation calculation starts, 
	/// then call Accumulate() after it ends, passing the simulation time for the
	/// simulation step.
	class Comparator{
	public:
		/// \brief Construct a Comparator with a given time span.
		/// \param timeSpan The simulation time span to average the time factor over. 
		/// Must be greater than zero.
		/// \throws std::invalid_argument if timeSpan is less than or equal to zero.
		inline Comparator( Time timeSpan = 1_s );


		/// \brief Starts the real time clock.
		///
		/// The Accumulate() method will take the time between the 
		/// call of StartRealTimeMeasurement() and Accumulate()
		/// to accumulate the real time, needed to calculate the simulation..
		inline void StartRealTimeMeasurement();


		/// \brief Accumulate the times and return 
		/// the factor of simulation time to real world time.
		/// \param dt The simulation time passed since last call.
		/// \returns The factor of simulation time to real world time.
		inline One Accumulate( Time dt ) noexcept;

		
		/// \brief Get the current time factor.
		/// \returns The factor of simulation time to real world time.
		inline One GetTimeFactor() const noexcept{
			return m_TimeFactor;
		}
	private:
		Clock m_Clock;
		const Time m_TimeSpan;
		Time m_AccumSimTime;
		Time m_AccumRealTime;
		One m_TimeFactor;
	};


	/// \brief Measures the frame rate of a simulation.
	class FrameRate{
	public:
		/// \brief Construct a FrameRate with a given time 
		/// span to average the frame rate over.
		FrameRate( Time timeSpan = 1_s );


		/// \brief Call this method once per frame.
		inline void Tick();


		/// \returns the current frame rate.
		inline Frequency Get() const noexcept;
	private:
		Clock m_Clock;
		const Time m_TimeSpan;
		int m_Ticks;
		Frequency m_FrameRate;
	};


///////////////////////////////////////
inline void Clock::Start() noexcept
{
	m_start = std::chrono::steady_clock::now();
}

inline Time Clock::Elapsed() const noexcept
{
	const auto now = std::chrono::steady_clock::now();
	const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( now - m_start ).count();
	return _ms( duration );
}

inline Time Clock::Stop() noexcept
{
	return Elapsed();
}
///////////////////////////////////////
inline Comparator::Comparator( Time timeSpan )
	: m_TimeSpan		{ timeSpan }
	, m_AccumSimTime	{ 0_s }
	, m_AccumRealTime	{ 0_s }
	, m_TimeFactor		{ 1 }
{
	if( m_TimeSpan <= 0_s )
	{
		throw std::invalid_argument( "timeSpan must be greater than zero" );
	}
}

inline void Comparator::StartRealTimeMeasurement()
{
	m_Clock.Start();
}

inline One Comparator::Accumulate( Time dt ) noexcept
{
	m_AccumSimTime += dt;
	m_AccumRealTime += m_Clock.Stop();

	if( m_AccumSimTime >= m_TimeSpan )
	{
		m_TimeFactor = m_AccumSimTime / m_AccumRealTime;
		m_AccumSimTime = 0_s;
		m_AccumRealTime = 0_s;
	}

	m_Clock.Start();
	return m_TimeFactor;
}
///////////////////////////////////////
inline FrameRate::FrameRate( Time timeSpan )
	: m_TimeSpan	{ timeSpan }
	, m_Ticks		{ 0 }
	, m_FrameRate	{ 0_Hz }
{
	if( m_TimeSpan <= 0_s )
	{
		throw std::invalid_argument( "timeSpan must be greater than zero" );
	}
}

inline void FrameRate::Tick()
{
	++m_Ticks;

	if( m_Clock.Elapsed() >= m_TimeSpan )
	{
		m_FrameRate = m_Ticks / m_TimeSpan;
		m_Ticks = 0;
		m_Clock.Start();
	}
}

inline Frequency FrameRate::Get() const noexcept{
	return m_FrameRate;
}
///////////////////////////////////////
} // namespace trax
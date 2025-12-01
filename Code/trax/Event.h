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

#include "Track.h"

namespace common{
	/// \private
	template<typename> struct Interval;
}

namespace trax{

	class Location;

	struct Event{

		/// \brief Makes a simple unqualified Event object.
		static dclspc std::unique_ptr<Event> Make() noexcept;


		virtual ~Event() = default;

		Event( Event&& ) = delete;
		Event& operator=( const Event& ) = delete;
		Event& operator=( Event&& ) = delete;
	protected:
		Event() = default;
		Event( const Event& ) = default;
	};

	/// \brief An event is sent to a sensor if it gets triggered.
	struct EventFilter : Event{

		enum class Type{
			none = -1,
			wheelframe = 0,
			wheelset,
			traintip_north,
			traintip_south,
			traintip_both,
			any
		};

		/// \brief Makes a EventFilter object.
		static dclspc std::unique_ptr<EventFilter> Make( EventFilter::Type type = EventFilter::Type::any ) noexcept;


		static std::string From( Type type );

		static Type From( const std::string& type );


		virtual std::string Name() const = 0;

		virtual void Name( const std::string& name ) = 0;

		virtual Type GetType() const noexcept = 0;

		virtual void SetType( Type type ) noexcept = 0;

		virtual void TrainTip( bool bTrainTip ) noexcept = 0;

		virtual bool TrainTip() const noexcept = 0;

		virtual void TrainEnd( bool bTrainEnd ) noexcept = 0;

		virtual bool TrainEnd() const noexcept = 0;
	};
}


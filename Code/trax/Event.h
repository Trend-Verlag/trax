// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2018 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "Track.h"

namespace common{
	/// \private
	template<typename> struct Interval;
}

namespace trax{

	class Location;

	struct Event{

		/// \brief Method that gets called before a location is moved along a specific track.
		/// \returns true to let the move actually happen, false if the moving should get suppressed.
		virtual bool Moving( const Location& _location, const common::Interval<Length>& range ) const noexcept = 0;


		/// \brief Method that gets called before a location is changing from one track to another.
		virtual void Transitioning( const Track& fromTrack, Track::EndType fromEnd, const Track& toTrack, Track::EndType toEnd ) const noexcept = 0;


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


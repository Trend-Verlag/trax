//	trax track library
//	AD 2013 
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

#include "IDType.h"
#include "Units.h"
#include "Orientation.h"

#include "common/Interval.h"

namespace trax{

	///	\brief A TrackLocation ist a location on a single track.
	///
	///	The location on a track is specified by a single parameter
	/// giving the arc length along the track from its start and a second value 
	/// determining the orientation.
	struct TrackLocation
	{
		Length		parameter;		///< Parameter along a track starting from Track::front
		Orientation orientation;	///< Orientation on the track. True means orientation in track direction.


		/// \name Construction
		/// \param param Parameter value
		/// \param orient Orientation
		///@{
		TrackLocation() noexcept
			: parameter{0}, orientation{ Orientation::Value::para }{}
		explicit TrackLocation( Length param ) noexcept
			: parameter{ param }, orientation{ Orientation::Value::para }{}
		explicit TrackLocation( Length param, bool orient ) noexcept
			: parameter{param}, orientation{orient}{}
		TrackLocation( Length param, Orientation::Value orient ) noexcept
			: parameter{param}, orientation{orient}{}
		TrackLocation( Length param, Orientation orient ) noexcept
			: parameter{ param }, orientation{ orient }{}
		///@}


		bool operator==( const TrackLocation& tl ) const noexcept{
			return parameter == tl.parameter && orientation == tl.orientation;
		}

		bool operator!=( const TrackLocation& tl ) const noexcept{
			return !operator==( tl );
		}

		/// \returns true if the parameter value is in [range.Near(),range.Far()].
		bool InRange( const common::Interval<Length>& range ) const noexcept{
			return !(parameter < range.Near() ||
					 parameter > range.Far());
		}


		/// \param loc TrackLocation to compare with.
		/// \param _epsilon Threshold for parameter equality.
		/// \returns true if the two TrackLocation differ only
		/// by less than epsilon.
		bool Equals( const TrackLocation& loc, Length _epsilon ) const noexcept{
			return	orientation == loc.orientation &&
					common::Equals( parameter, loc.parameter, _epsilon );
		}


		/// \brief Turns the orientation of this TrackLocation
		/// to the other side.
		void Flip() noexcept{
			orientation.Flip();
		}
	};


	/// \brief Moves the TrackLocation by dp, obeying its orientation.
	inline void operator+=( TrackLocation& tl, Length dp ) noexcept;


	/// \brief Moves the TrackLocation by -dp, obeying its orientation.
	inline void operator-=( TrackLocation& tl, Length dp ) noexcept;


	/// \brief A track location, referencing the track by id.
	struct TrackLocationRef
	{
		TrackLocation	location;	///< location on track.
		IDType			refid;		///< ID of track.


		/// \name Construction
		/// \param tl TrackLocation to construct from
		///@{
		TrackLocationRef() noexcept = default;
		explicit TrackLocationRef( const TrackLocation& tl ) noexcept
			: location{tl} 
		{}
		TrackLocationRef( Length param, bool orient, IDType id ) noexcept
			: location{ param, orient },
			refid{id}
		{}
		///@}


		bool operator==( const TrackLocationRef& tlr ) const noexcept{
			return location == tlr.location && refid == tlr.refid;
		}

		bool operator!=( const TrackLocationRef& tlr ) const noexcept{
			return !operator==( tlr );
		}
	};


	/// \brief An interval on a track, referenced by an id.
	struct TrackRange{
		common::Interval<Length> range; ///< range on track.
		IDType refid;					///< ID of track.

		/// \name Construction
		/// \param id track id
		/// \param near_ near value of interval
		/// \param far_ far value of interval
		/// \param interval Interval
		///@{
		TrackRange() noexcept = default;
		explicit TrackRange( IDType id ) noexcept
			:	refid{id} 
		{}
		TrackRange( IDType id, trax::Length near_, trax::Length far_ ) noexcept
			:	range{near_,far_},
				refid{id}
		{}
		explicit TrackRange( const common::Interval<trax::Length>& interval ) noexcept
			:	range{interval}
		{}
		///@}


		bool operator==( const TrackRange& tr ) const noexcept{
			return range == tr.range && refid == tr.refid;
		}

		bool operator!=( const TrackRange& tr ) const noexcept{
			return !operator==( tr );
		}
	};


//inlines:
inline void operator+=( TrackLocation& tl, Length dp ) noexcept{
	if( tl.orientation )
		tl.parameter += dp;
	else
		tl.parameter -= dp;
}

inline void operator-=( TrackLocation& tl, Length dp ) noexcept{
	if( tl.orientation )
		tl.parameter -= dp;
	else
		tl.parameter += dp;
}

}
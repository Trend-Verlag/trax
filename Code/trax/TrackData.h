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

#include "spat/Frame.h"

namespace trax{


	/// \brief Full geometrical data at a point on a track.
	template<typename Valtype> 
	struct TrackData{
		spat::Frame<Valtype> F;		///< TNB Frame of the curve 
		spat::Frame<Valtype> wF;	///< twisted TNB Frame
		Valtype c;					///< curvature
		Valtype t;					///< torsion

		TrackData()
			: F{}, wF{}, c{0}, t{0}
		{}
		TrackData( const TrackData& ) = default;
		TrackData( TrackData&& ) = default;
		TrackData( spat::Frame<Valtype> _F, spat::Frame<Valtype> _wF, Valtype _c, Valtype _t )
			: F{_F}, wF{_wF}, c{_c}, t{_t}
		{}

		TrackData& operator=( const TrackData& ) = default;
		TrackData& operator=( TrackData&& ) = default;

		template<typename Valtype2>
		inline TrackData& operator=( const TrackData<Valtype2>& );

		/// \brief Initializes all values.
		void Init() noexcept{
			F.Init();
			wF.Init();
			c = 0;
			t = 0;
		}

		/// \brief Sets the curve values for calculating the contraint.
		///
		/// \param td values of the curve 
		/// \returns true if the values do not represent a violation of 
		/// the thresholds.
		virtual bool SetValues(	const TrackData& td ) noexcept;

		virtual ~TrackData() = default;
	};


template<typename Valtype>
template<typename Valtype2>
TrackData<Valtype>& TrackData<Valtype>::operator=( const TrackData<Valtype2>& td ){
	F   = spat::Frame<Valtype>{ td.F.ptr() };
	wF  = spat::Frame<Valtype>{ td.wF.ptr() };
	c	= static_cast<Valtype>(td.c);
	t	= static_cast<Valtype>(td.t);
	return *this;
}

template<typename Valtype>
bool TrackData<Valtype>::SetValues(	const TrackData& td ) noexcept{
	F	= td.F;
	wF	= td.wF;
	c	= td.c;
	t	= td.t;
	return true;
}

}

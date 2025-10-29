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

#include "trax/collections/TrackCollection.h"
#include "trax/ImplementationHelper.h"

namespace trax{

	using namespace spat;

	struct TrackSystem;

	typedef Pose_Imp<ObjectID_Imp<Container_Imp<TrackBuilder,TrackCollection>>> TrackCollection_Base;

	class TrackCollection_Imp : public TrackCollection_Base{
	public:
		TrackCollection_Imp() noexcept;
		TrackCollection_Imp( const TrackCollection_Imp& ) = delete;
		TrackCollection_Imp( TrackCollection_Imp&& ) = delete;
		~TrackCollection_Imp();

		TrackCollection_Imp& operator=( const TrackCollection_Imp& ) = delete;
		TrackCollection_Imp& operator=( TrackCollection_Imp&& ) = delete;

		void SetParent( TrackSystem* pParent );

//		TrackSystem* GetParent() const noexcept;


		const char* TypeName() const override;

		bool	IsValid	( bool bSilent = true ) const noexcept override;

		IDType	Add		( std::shared_ptr<TrackBuilder> pTrack ) override;

		bool	Remove	( TrackBuilder* pTrack, bool zeroIDs = false ) override;
		
		void	Clear	() noexcept override;
	private:
		TrackSystem* m_pParent;

		void PropagateAbsoluteFrameToClients() noexcept override;

		void SetTrackAbsoluteFrame( TrackBuilder* pTrack, const Frame<Length,One>& frame ) const noexcept;

		void SetTracksAbsoluteFrames( const Frame<Length,One>& frame ) const noexcept;

		void DoClear() noexcept;
	};

}
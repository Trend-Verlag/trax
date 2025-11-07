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

#include "trax/collections/TrackCollectionContainer.h"
#include "trax/ImplementationHelper.h"

namespace trax{

	struct TrackSystem;

	typedef ObjectID_Imp<Container_Imp<TrackCollection,TrackCollectionContainer>> TrackCollectionContainer_Base;

	class TrackCollectionContainer_Imp : public TrackCollectionContainer_Base{
	public:
		TrackCollectionContainer_Imp() noexcept;
		TrackCollectionContainer_Imp( std::shared_ptr<TrackCollection> pTrackCollection );

		void SetParent( TrackSystem* pParent );

		TrackSystem* GetParent() const noexcept;

		const char* TypeName() const noexcept override;

		bool IsValid() const noexcept override;

		void SetAbsoluteFrame( const spat::Frame<Length,One>& frame ) noexcept;

		IDType Add( std::shared_ptr<TrackCollection> pTrackCollection ) override;

		bool Remove( TrackCollection* pTrackCollection, bool zeroIDs = false ) override;
		
		void Clear() noexcept override;
	private:
		spat::Frame<Length,One>	m_AbsoluteFrame;
		TrackSystem*			m_pParent;

		void SetCollectionsAbsoluteFrame( const spat::Frame<Length,One>& frame ) noexcept;

		IDType DoAdd( std::shared_ptr<TrackCollection> pTrackCollection );
	};

}
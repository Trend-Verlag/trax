// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

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

		const char* TypeName() const override;

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
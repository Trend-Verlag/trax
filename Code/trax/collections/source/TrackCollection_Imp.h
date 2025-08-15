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
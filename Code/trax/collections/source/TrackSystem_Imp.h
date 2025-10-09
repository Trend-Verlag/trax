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

#include "../TrackSystem.h"
#include "trax/ImplementationHelper.h"
#include "trax/Jack.h"

namespace trax{
	using namespace spat;


	typedef ObjectID_Imp<Container_Imp<TrackBuilder,TrackSystem>> TrackSystem_Base;

	class TrackSystem_Imp : public TrackSystem_Base,
							public JackEnumerator{
	public:
		TrackSystem_Imp() = delete;
		TrackSystem_Imp( const TrackSystem_Imp& ) = delete;
		TrackSystem_Imp( TrackSystem_Imp&& ) = delete;
		TrackSystem_Imp(	std::unique_ptr<TrackCollectionContainer>	pTrackCollectionContainer,
							std::unique_ptr<ConnectorCollection>		pConnectorCollection ) noexcept;
		~TrackSystem_Imp();

		TrackSystem_Imp& operator=( const TrackSystem_Imp& ) = delete;
		TrackSystem_Imp& operator=( TrackSystem_Imp&& ) = delete;


		/// \brief to return correct shared_ptr to this, this object have 
		/// to know about the use count structure.
		void SetWeakPointerToSelf( std::weak_ptr<TrackSystem_Imp> pThis ) noexcept;

		void SetAbsoluteFrame( const Frame<Length,One>& frame ) noexcept;


		// Collection:
		const char* TypeName() const noexcept override;

		IDType Add( std::shared_ptr<TrackBuilder> pTrack ) override;

		bool Remove( TrackBuilder* pTrack, bool zeroIDs = false ) override;

		int Take( TrackSystem& trackSystem ) override;

		void Clear() noexcept override;


		// TrackSystem:
		std::shared_ptr<TrackSystem> This() const noexcept override;
	 
		IDType CreateCollection( IDType id = 0 ) override;

		void RemoveCollection( IDType id ) override;

		void SetCollectionFrame( IDType id, const Frame<Length,One>& frame ) override;

		const Frame<Length,One>& GetCollectionFrame( IDType id ) const override;

		IDType SetActiveCollection( IDType id ) override;

		IDType GetActiveCollection() const noexcept override;

		std::shared_ptr<TrackCollectionContainer> SetTrackCollectionContainer( std::shared_ptr<TrackCollectionContainer> pTrackCollectionContainer ) override;

		std::shared_ptr<TrackCollectionContainer> GetCollectionContainer() const noexcept override;

		std::shared_ptr<ConnectorCollection> SetConnectorCollection	( std::shared_ptr<ConnectorCollection> pConnectorCollection ) noexcept override;

		ConnectorCollection* GetConnectorCollection	() const noexcept override;

		void Couple( std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> trackEnd1, std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> trackEnd2 ) const override;

		void Couple( const Track::Coupling& coupling, bool bUncoupledOnly = false ) const override;

		std::shared_ptr<Sensor> GetSensor( IDType id, TrackLocation* pTrackLocation ) const override;

		Length CalculateGapSize( const Track::End& theOne, const Track::End& theOther ) const override;

		std::vector<Track::End> GetUncoupledIn( const Sphere<Length>& area ) const override;

		void Connection( Track::Coupling& coupling ) const override;

		void Connection( const Track::End& end, Track::End& coupled ) const override;

		void Connection( const Track::Coupling& couplings, Track::Coupling& active ) const override;

		void CoupleAll( Length maxDistance = 1_m ) override;

		void DeCoupleAll() override;


		// Simulated:
		bool Start( Scene& scene ) noexcept override;

		void Update( Time dt ) noexcept override;

		void Pause() noexcept override;

		void Resume() noexcept override;

		void Stop() noexcept override;


		// Identified:
		const std::string& Reference( const std::string& name ) const override{
			return TrackSystem_Base::Reference( name );
		}

		void Reference( const std::string& name, const std::string& reference ) override{
			return TrackSystem_Base::Reference( name, reference );
		}

		IDType ID() const noexcept override{
			return TrackSystem_Base::ID();
		}

		void ID( IDType id ) noexcept override{
			return TrackSystem_Base::ID( id );
		}

		// JackEnumerator:
		int CountJacks() const noexcept override;
	protected:
		void SetDecorator( TrackSystem* pTrackSystemDecorator ) noexcept override;
		const Jack& _GetJack( int idx ) const override;

	private:
		std::weak_ptr<TrackSystem_Imp>								m_pThis;
		std::shared_ptr<TrackCollectionContainer>					m_pTrackCollectionContainer;
		IDType														m_IDActiveTrackCollection;

		std::shared_ptr<ConnectorCollection>						m_pConnectorCollection;

		void DoClear();
	};

}
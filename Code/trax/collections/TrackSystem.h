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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "Collection.h"
#include "CollectionDecorator.h"
#include "trax/Identified.h"

#include "trax/Simulated.h"
#include "trax/Track.h"
#include "trax/Location.h"

namespace spat{
	template<typename> struct Sphere;
}

namespace trax{

	struct ConnectorCollection;
	//struct Module;
	struct TrackCollectionContainer;

	/// \brief A track system is the container for tracks. 
	/// 
	/// The TrackSystem maintains TrackCollection objects that are used to group 
	/// and locate tracks inside of a TrackSystem. A track will deliver module 
	/// relative coordinates with its Transition() methods, taking into account
	/// the TrackCollection's frame of reference.
	struct TrackSystem :	Collection<TrackSystem,TrackBuilder>,
							Simulated,
							Identified<TrackSystem>
	{
		/// \brief Makes a standard TrackSystem object.
		static dclspc std::shared_ptr<TrackSystem> Make() noexcept;

		static dclspc std::shared_ptr<TrackSystem> Make( 
			std::unique_ptr<TrackCollectionContainer> pTrackCollectionContainer ) noexcept;

		static dclspc std::shared_ptr<TrackSystem> Make( 
			std::unique_ptr<TrackCollectionContainer> pTrackCollectionContainer,
			std::unique_ptr<ConnectorCollection> pConnectorCollection ) noexcept;


		/// \brief Gets a shared pointer to this.
		virtual std::shared_ptr<TrackSystem> This() const noexcept = 0;


		// TrackCollections:

		/// \brief Creates a TrackCollection with the supplied id.
		/// \param id id for the new TrackCollection or 0 if one should be made automatically.
		/// \returns id of the new TrackCollection.
		virtual	IDType CreateCollection( IDType id = 0 ) = 0;


		/// \brief Removes the TrackCollection with the specified id.
		/// \param id of the TrackCollection to remove.
		virtual	void RemoveCollection( IDType id ) = 0;


		/// \brief Sets the frame of reference for a TrackCollection.
		virtual void SetCollectionFrame( IDType id, const spat::Frame<Length,One>& frame ) = 0;


		/// \brief Gets the frame of reference for a TrackCollection.
		virtual const spat::Frame<Length,One>& GetCollectionFrame( IDType id ) const = 0;


		/// \brief Sets a TrackCollection to be the active one.
		virtual	IDType SetActiveCollection( IDType id ) = 0;


		/// \returns The id of the active TrackCollection.
		virtual	IDType GetActiveCollection() const = 0;


		virtual std::shared_ptr<TrackCollectionContainer> SetTrackCollectionContainer( std::shared_ptr<TrackCollectionContainer> pTrackCollectionContainer ) = 0;


		/// \returns A pointer to the TrackCollectionContainer object that holds the
		/// TrackCollection objects.
		virtual std::shared_ptr<TrackCollectionContainer> GetCollectionContainer() const = 0;


		/// \brief Attaches a ConnectorCollection to the track system
		/// \returns The old attached connector collection.
		virtual std::shared_ptr<ConnectorCollection> SetConnectorCollection( std::shared_ptr<ConnectorCollection> pConnectorCollection ) = 0;


		/// \returns A pointer to the ConnectorCollection attached to this TrackSystem.
		virtual ConnectorCollection* GetConnectorCollection() const = 0;


		/// \name Couple
		/// \brief Couples two tracks that are members of this TrackSystem.
		/// \param trackEnd1 Shared pointer to and end of track A.
		/// \param trackEnd2 Shared pointer to and end of track B.
		/// \param bUncoupledOnly true if only loose ends should get connected, but existing connections stay untouched.
		///@{
		virtual void Couple( std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> trackEnd1, std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> trackEnd2 ) const = 0;

		virtual void Couple( const Track::Coupling& coupling, bool bUncoupledOnly = false ) const = 0;
		///@}


		/// \returns a pointer to the sensor with the respective id.
		virtual std::shared_ptr<Sensor> GetSensor( IDType id, TrackLocation* pTrackLocation ) const = 0;


		/// \returns the distance between the two track ends.
		virtual Length CalculateGapSize( const Track::End& theOne, const Track::End& theOther ) const = 0;


		/// \brief Searches for a free, uncoupled track end in the area.
		virtual std::vector<Track::End> GetUncoupledIn( const spat::Sphere<Length>& area ) const = 0;


		/// \brief Completes the coupling.
		///
		/// \param coupling A track coupling with one of its track ids set to 0. This will be completed
		/// by the correct connection.
		virtual void Connection( Track::Coupling& coupling ) const = 0;

		virtual void Connection( const Track::End& end, Track::End& coupled ) const = 0;

		/// \brief Writes the coupled tracks data of the trackends in couplings to active.
		///
		/// \param couplings Two track ends.
		/// \param active Coupled track end data for the track ends in couplings.
		virtual void Connection( const Track::Coupling& couplings, Track::Coupling& active ) const = 0;
		

		///\brief Couples all open ends in the track system, if they are closer
		/// than maxDistance.
		/// \param maxDistance A threshold for the distance to search track ends around the to be coupled end.
		/// \param maxKink A threshold for the maximum allowed kink angle in T and B respectively.
		/// \param bSilent If true no log messages are emitted.
		virtual void CoupleAll( Length maxDistance = 1_m, Angle maxKink = pi, bool bSilent = true ) = 0;


		/// \brief Uncouples all the Tracks in the TrackSystem from each other.
		///
		/// This is needed since the coupled tracks hold shared pointer 
		/// to each other.
		virtual void DeCoupleAll() = 0;
	};
	

	/// \brief Finds all track ends in the area.
	/// 
	/// \param system Track system with tracks to evaluate.
	/// \param area Spherical area to detect track ends in.
	/// \param sort If true the list gets sorted by distance to the center of the area (closest first). 
	/// \return A list with track ends as well as their distance from the area center. If both track ends are inside the area, the track will be listed twice.
	std::vector<std::tuple<std::shared_ptr<TrackBuilder>,Track::EndType,Length>> dclspc FindTrackEnds( const TrackSystem& system, const spat::Sphere<Length>& area, bool sort = false );


	/// \brief Finds all tracks that run trough an area.
	/// 
	/// \param system Track system with tracks to evaluate.
	/// \param area Spherical area to detect tracks in.
	/// \param sort If true the list gets sorted by distance to the center of the area (closest first). 
	/// \return A list with track Locations as well as their distance from the area center.
	std::vector<std::pair<Location,Length>> dclspc FindTrackLocations( const TrackSystem& system, const spat::Sphere<Length>& area, bool sort = false );

	Location dclspc FindTrackLocation( const TrackSystem& system, const spat::Sphere<Length>& area );
	

	/// \brief Finds all tracks that got hit by a ray.
	/// 
	/// \param system Track system with tracks to evaluate.
	/// \param ray A line starting at ray.P and running in the ray.T direction ad infinitum.
	/// \param gauge If this is > 0_m it is used as the width of a ribbon to hit, also defined 
	/// by the track's up direction. 
	/// If <= 0_m the track's Section gauge is evaluated for a width of the track.
	/// \param sort If true the list gets sorted by distance to ray.P (closest first). 
	/// \return A list with track Locations as well as their distance from the ray's starting 
	/// point.
	std::vector<std::pair<Location,Length>> dclspc FindTrackLocations( const TrackSystem& system, const spat::VectorBundle<Length,One>& ray, Length gauge = 0_m, bool sort = false );
	

	/// \brief Searches open track ends inside an area around a given track end and couples to 
	/// the closest.
	/// 
	/// The function will couple tracks from one collection only, so it will not happen that
	/// tracks from different collections get coupled to both ends. If the track itself is part
	/// of a collection, only tracks from that collection will be considered.
	/// \param system The track system to search track ends in.
	/// \param trackEnd The track end to couple.
	/// \param maxDistance A threshold for the distance to search track ends around the to be 
	/// coupled end.
	/// \param maxKink A threshold for the maximum allowed kink angle in T and B respectively.
	/// \param bSilent If true no log messages are emitted.
	/// \return The other track ends, the track was coupled to, pair.first for front and 
	/// pair.second for back end; or { nullptr, Track::EndType::none } if no suitable track 
	/// end was found.
	/// \throws std::invalid_argument If the end type is not recocnised.
	std::pair<Track::TrackEnd,Track::TrackEnd> dclspc Couple( const TrackSystem& system, Track::TrackEnd trackEnd, Length maxDistance = 1_m, Angle maxKink = pi, bool bSilent = true );


	/// \brief Searches open track ends inside an area around a given track end and couples 
	/// and snaps to the closest.
	/// 
	/// The method will snap only once.
	/// \param system The track system to search track ends in.
	/// \param trackEnd The track end to couple.
	/// \param maxDistance A threshold for the distance to search track ends around the to be 
	/// coupled end.
	/// \param maxKink A threshold for the maximum allowed kink angle in T and B respectively.
	/// \param bSilent If true no log messages are emitted.
	/// \return The other track ends, the track was coupled to, pair.first for front and 
	/// pair.second for back end; or { nullptr, Track::EndType::none } if no suitable track 
	/// end was found.
	/// \throws std::invalid_argument If the end type is not recocnised.
	std::pair<Track::TrackEnd,Track::TrackEnd> dclspc CoupleAndSnap( const TrackSystem& system, Track::TrackEnd trackEnd, Length maxDistance = 1_m, Angle maxKink = pi, bool bSilent = true );


	/// \brief A decorator for TrackSystems.
	///
	/// With trax decorators can get used to augment trax objects with additional 
	/// behaviour and services. Derive your own class from the decorator and overwrite
	/// its methods. Create your decorator with the trax object and supply it to 
	/// the library instead of the original object.
	/// In case of a track system one might build e.g. graphics data for a track
	/// that gets added to a TrackSystem and destroy it if the track gets removed.
	class TrackSystemDecorator : public CollectionDecorator<SimulatedDecorator<ObjectIDDecorator<Decorator<TrackSystem>>>>{
	public:

		/// \brief Type of the decoree.
		typedef TrackSystem ComponentType;


		TrackSystemDecorator() = delete;
		
		/// \brief Constructs the decorator.
		/// \param pTrackSystem Pointer to object to decorate.
		TrackSystemDecorator( std::shared_ptr<TrackSystem> pTrackSystem )
			: CollectionDecorator<SimulatedDecorator<ObjectIDDecorator<Decorator<TrackSystem>>>>{ pTrackSystem }
		{}


		/// \name Forwarding
		/// \brief TrackSystem member function forwarding.
		///@{
		std::shared_ptr<TrackSystem> This() const noexcept override{
			return m_pComponent->This();
		}

		IDType CreateCollection( IDType id = 0 ) override{
			return m_pComponent->CreateCollection(id);
		}

		void RemoveCollection( IDType id ) override{
			return m_pComponent->RemoveCollection(id);
		}

		void SetCollectionFrame( IDType id, const spat::Frame<Length,One>& frame ) override{
			return m_pComponent->SetCollectionFrame(id,frame);
		}

		const spat::Frame<Length,One>& GetCollectionFrame( IDType id ) const override{
			return m_pComponent->GetCollectionFrame(id);
		}

		IDType SetActiveCollection( IDType id ) override{
			return m_pComponent->SetActiveCollection(id);
		}

		IDType GetActiveCollection() const override{
			return m_pComponent->GetActiveCollection();
		}

		std::shared_ptr<TrackCollectionContainer> GetCollectionContainer() const override{
			return m_pComponent->GetCollectionContainer();
		}

		std::shared_ptr<TrackCollectionContainer> SetTrackCollectionContainer( std::shared_ptr<TrackCollectionContainer> pTrackCollectionContainer ) override{
			return m_pComponent->SetTrackCollectionContainer( pTrackCollectionContainer );
		}
		
		std::shared_ptr<ConnectorCollection> SetConnectorCollection( std::shared_ptr<ConnectorCollection> pConnectorCollection ) override{
			return m_pComponent->SetConnectorCollection( pConnectorCollection );
		}

		ConnectorCollection* GetConnectorCollection() const override{
			return m_pComponent->GetConnectorCollection();
		}

		void Couple( std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> trackEnd1, std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> trackEnd2 ) const override{
			return m_pComponent->Couple( trackEnd1, trackEnd2 );
		}

		void Couple( const Track::Coupling& coupling, bool bUncoupledOnly = false ) const override{
			return m_pComponent->Couple( coupling, bUncoupledOnly );
		}

		std::shared_ptr<Sensor> GetSensor( IDType id, TrackLocation* pTrackLocation ) const override{
			return m_pComponent->GetSensor( id, pTrackLocation );
		}

		Length CalculateGapSize( const Track::End& theOne, const Track::End& theOther ) const override{
			return m_pComponent->CalculateGapSize( theOne, theOther );
		}

		std::vector<Track::End> GetUncoupledIn( const spat::Sphere<Length>& area ) const override{
			return m_pComponent->GetUncoupledIn( area );
		}

		void Connection( Track::Coupling& coupling ) const override{
			return m_pComponent->Connection(coupling);
		}

		void Connection( const Track::End& end, Track::End& coupled ) const override{
			return m_pComponent->Connection(end,coupled);
		}

		void Connection( const Track::Coupling& couplings, Track::Coupling& active ) const override{
			return m_pComponent->Connection(couplings,active);
		}

		void DeCoupleAll() override{
			return m_pComponent->DeCoupleAll();
		}
		///@}
	};
}
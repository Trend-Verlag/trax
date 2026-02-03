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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/Identified.h"
#include "Collection.h"
#include "CollectionDecorator.h"
#include "trax/Track.h"

#include <vector>

namespace spat{
	/// \private
	template<typename> struct Sphere;
}

namespace trax{

	class Location;

	/// \brief Collection to hold a set of Track objects.
	struct TrackCollection :	Collection<TrackCollection,TrackBuilder>,
								Identified<TrackCollection>
	{
		/// \brief Makes a standard TrackCollection object.
		static dclspc std::unique_ptr<TrackCollection> Make() noexcept;


		///	\brief Sets the frame of references for this TrackCollection 
		/// relative to the Module.
		/// 
		/// The tracks added to this collection will give Module relative
		/// coordinates on transition, so this frame is included in the
		/// calculations.
		virtual void SetFrame( const spat::Frame<Length,One>& frame ) noexcept = 0;


		///	\brief Gets the frame of references for this TrackCollection 
		/// relative to the Module.
		virtual const spat::Frame<Length,One>& GetFrame() const noexcept = 0;
	};


	/// \brief Finds all track ends in the area.
	/// 
	/// \param collection Track collection with tracks to evaluate.
	/// \param area Spherical area to detect track ends in.
	/// \param sort If true the list gets sorted by distance to the center of the area (closest first). 
	/// \return A list with track ends as well as their distance from the area center. If both track ends are inside the area, the track will be listed twice.
	std::vector<std::tuple<std::shared_ptr<TrackBuilder>,EndType,Length>> dclspc FindTrackEnds( const TrackCollection& collection, const spat::Sphere<Length>& area, bool sort = false );


	/// \brief Finds all tracks that run trough an area.
	/// 
	/// \param collection Track collection with tracks to evaluate.
	/// \param area Spherical area to detect tracks in.
	/// \param sort If true the list gets sorted by distance to the center of the area (closest first). 
	/// \return A list with track Locations as well as their distance from the area center.
	std::vector<std::pair<Location,Length>> dclspc FindTrackLocations( const TrackCollection& collection, const spat::Sphere<Length>& area, bool sort = false );
	

	/// \brief Finds all tracks that got hit by a ray.
	/// 
	/// \param collection Track collection with tracks to evaluate.
	/// \param ray A line starting at ray.P and running in the ray.T direction ad infinitum.
	/// \param gauge If this is > 0_m it is used as the width of a ribbon to hit, also defined by the track's up direction. 
	/// \param sort If true the list gets sorted by distance to ray.P (closest first). 
	/// \return A list with track Locations as well as their distance from the ray's starting point.
	std::vector<std::pair<Location,Length>> dclspc FindTrackLocations( const TrackCollection& collection, const spat::VectorBundle<Length,One>& ray, Length gauge, bool sort = false );


	/// \brief Moves a track so that one of it's ends aligns to another track end in a collection.
	/// \param collection The track collection to search track ends in.
	/// \param track The track to snap.
	/// \param endType The track's end to snap.
	/// \param maxDistance A threshold for the distance to search track ends around the to be snapped end.
	/// \param bUncoupled If true only uncoupled track ends are taken into consideration.
	/// \return The other track end, the track was snapped to, or { nullptr, EndType::none } if no suitable track end was found.
	std::pair<std::shared_ptr<TrackBuilder>,EndType> dclspc Snap( const TrackCollection& collection, Track::TrackEnd trackEnd, Length maxDistance, bool bUncoupled = true );


	/// \brief Searches open track ends inside an area around a given track end and couples to the closest.
	/// \param collection The track collection to search track ends in.
	/// \param trackEnd The track end to couple. trackEnd.second == EndType::any and EndType::both 
	/// are supported.
	/// \param maxDistance A threshold for the distance to search track ends around the to be coupled end.
	/// \param maxKink A threshold for the maximum allowed kink angle in T and B respectively.
	/// \param bSilent If true no log messages are emitted.
	/// \return The other track ends, the track was coupled to, or { nullptr, EndType::none } if no suitable 
	/// track end was found. For a coupling with front the result is in pair.first, for a coupling with end in pair.second.
	/// \throws std::invalid_argument If the end type is not recocnized.
	std::pair<Track::TrackEnd,Track::TrackEnd> dclspc Couple( const TrackCollection& collection, Track::TrackEnd trackEnd, Length maxDistance = 1_m, Angle maxKink = pi, bool bSilent = true );


	/// \brief A decorator for TrackCollection.
	///
	/// With trax decorators can get used to augment trax objects with additional 
	/// behaviour and services. Derive your own class from the decorator and overwrite
	/// its methods. Create your decorator with the trax object and supply it to 
	/// the library instead of the original object.
	class TrackCollectionDecorator : public CollectionDecorator<ObjectIDDecorator<Decorator<TrackCollection>>>{
	public:
		/// \brief Type of the decoree.		
		typedef TrackCollection ComponentType;


		TrackCollectionDecorator() = delete;

		/// \brief Constructs the decorator.
		/// \param pTrackCollection Pointer to object to decorate.
		TrackCollectionDecorator( std::shared_ptr<TrackCollection> pTrackCollection )
			: CollectionDecorator<ObjectIDDecorator<Decorator<TrackCollection>>>{ pTrackCollection }
		{}

		void SetFrame( const spat::Frame<Length, One>& frame ) noexcept override{
			return m_pComponent->SetFrame(frame);
		}

		const spat::Frame<Length, One>& GetFrame() const noexcept override{
			return m_pComponent->GetFrame();
		}
	};
}
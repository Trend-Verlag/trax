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

#include "TrackCollection.h"

namespace trax{

	/// \brief Collection to hold a set of TrackCollection objects.
	struct TrackCollectionContainer :	Collection<TrackCollectionContainer,TrackCollection>,
										Identified<TrackCollectionContainer>
	{
		/// \brief Makes a standard TrackCollectionContainer object.
		static dclspc std::unique_ptr<TrackCollectionContainer> Make( bool doCreateCollections = false ) noexcept;

	};


	/// \brief A decorator for TrackCollectionContainer.
	///
	/// With trax decorators can get used to augment trax objects with additional 
	/// behaviour and services. Derive your own class from the decorator and overwrite
	/// its methods. Create your decorator with the trax object and supply it to 
	/// the library instead of the original object.
	class TrackCollectionContainerDecorator : public CollectionDecorator<ObjectIDDecorator<Decorator<TrackCollectionContainer>>>{
	public:

		TrackCollectionContainerDecorator() = delete;

		/// \brief Constructs the decorator.
		/// \param pTrackCollectionContainer Pointer to object to decorate.
		TrackCollectionContainerDecorator( std::unique_ptr<TrackCollectionContainer> pTrackCollectionContainer )
			: CollectionDecorator<ObjectIDDecorator<Decorator<TrackCollectionContainer>>>(std::move(pTrackCollectionContainer))
		{}

	};
}
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

#include "trax/Connector.h"
#include "Collection.h"
#include "CollectionDecorator.h"

namespace trax{

	/// \brief Collection to hold a set of Connector objects.
	struct ConnectorCollection : Collection<ConnectorCollection,Connector>{

		/// \brief Makes a standard ConnectorCollection object.
		static std::unique_ptr<ConnectorCollection> Make() noexcept;

	};


	/// \brief A decorator for ConnectorCollections.
	///
	/// With trax decorators can get used to augment trax objects with additional 
	/// behaviour and services. Derive your own class from the decorator and overwrite
	/// its methods. Create your decorator with the trax object and supply it to 
	/// the library instead of the original object.
	class ConnectorCollectionDecorator : public CollectionDecorator<Decorator<ConnectorCollection>>{
	public:
		/// \brief Type of the decoree.		
		typedef ConnectorCollection ComponentType;


		/// \brief Constructs the decorator.
		/// \param pConnectorCollection Pointer to object to decorate.
		ConnectorCollectionDecorator( std::unique_ptr<ConnectorCollection> pConnectorCollection )
			: CollectionDecorator<Decorator<ConnectorCollection>>(std::move(pConnectorCollection))
		{}
	};
}
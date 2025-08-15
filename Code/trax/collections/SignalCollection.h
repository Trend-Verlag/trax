//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/Signal.h"
#include "Collection.h"
#include "CollectionDecorator.h"

namespace trax{

	/// \brief Collection to hold a set of Signal objects.
	struct SignalCollection : Collection<SignalCollection,Signal>{

		/// \brief Makes a SignalCollection object.
		static dclspc std::unique_ptr<SignalCollection> Make() noexcept;
	};


	/// \brief A decorator for SignalCollection.
	///
	/// With trax decorators can get used to augment trax objects with additional 
	/// behaviour and services. Derive your own class from the decorator and overwrite
	/// its methods. Create your decorator with the trax object and supply it to 
	/// the library instead of the original object.
	class SignalCollectionDecorator : public CollectionDecorator<Decorator<SignalCollection>>{
	public:
		/// \brief Type of the decoree.		
		typedef SignalCollection ComponentType;


		/// \brief Constructs the decorator.
		/// \param pSignalCollection Pointer to object to decorate.
		SignalCollectionDecorator( std::unique_ptr<SignalCollection> pSignalCollection )
			: CollectionDecorator<Decorator<SignalCollection>>(std::move(pSignalCollection))
		{}
	};
}
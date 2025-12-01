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
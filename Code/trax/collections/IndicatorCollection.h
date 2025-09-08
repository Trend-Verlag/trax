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

#include "trax/Indicator.h"
#include "Collection.h"
#include "CollectionDecorator.h"

namespace trax{

	struct Module;

	/// \brief Collection to hold a set of Indicator objects.
	struct IndicatorCollection : Collection<IndicatorCollection,Indicator>
	{
		/// \brief Makes a IndicatorCollection object.
		static dclspc std::unique_ptr<IndicatorCollection> Make() noexcept;


		/// \brief Called during simulation to update position values.
		///
		/// This is usefull for decorators to override to keep data in sync.
		virtual void Update() = 0;
	};


	/// \brief A decorator for IndicatorCollection.
	///
	/// With trax decorators can get used to augment trax objects with additional 
	/// behaviour and services. Derive your own class from the decorator and overwrite
	/// its methods. Create your decorator with the trax object and supply it to 
	/// the library instead of the original object.
	class IndicatorCollectionDecorator : public CollectionDecorator<Decorator<IndicatorCollection>>{
	public:
		/// \brief Type of the decoree.		
		typedef IndicatorCollection ComponentType;


		/// \brief Constructs the decorator.
		/// \param pSwitchCollection Pointer to object to decorate.
		IndicatorCollectionDecorator( std::unique_ptr<IndicatorCollection> pSwitchCollection )
			: CollectionDecorator<Decorator<IndicatorCollection>>(std::move(pSwitchCollection))
		{}


		/// \name Forwarding
		/// \brief IndicatorCollection member function forwarding.
		//@{
		void Update() override{
			return m_pComponent->Update();
		}
		//@}
	};
}
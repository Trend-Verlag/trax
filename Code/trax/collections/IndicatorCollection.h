//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

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
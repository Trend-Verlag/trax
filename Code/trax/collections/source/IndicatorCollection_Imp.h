//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/collections/IndicatorCollection.h"
#include "trax/ImplementationHelper.h"

namespace trax{

	typedef Container_Imp<Indicator,IndicatorCollection> IndicatorCollection_Base;

	class IndicatorCollection_Imp : public IndicatorCollection_Base{
	public:
		IndicatorCollection_Imp() noexcept;

		const char* TypeName() const override;

		void Update() override;

		IDType Add( std::shared_ptr<Indicator> pIndicator ) override;

		bool Remove( Indicator* pIndicator, bool zeroIDs = false ) override;
	private:
	};

}
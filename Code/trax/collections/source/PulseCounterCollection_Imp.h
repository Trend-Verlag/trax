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

#include "trax/ImplementationHelper.h"
#include "trax/collections/PulseCounterCollection.h"
#include "trax/source/Jack_Imp.h"

namespace trax{

	using PulseCounterCollection_Base = JackEnumerator_Imp<ObjectID_Imp<Container_Imp<PulseCounter,PulseCounterCollection>>>;

	class PulseCounterCollection_Imp : public PulseCounterCollection_Base{
	public:
		const char* TypeName() const noexcept override;

		// Inherited via JackEnumerator
		const std::string& Reference( const std::string & /*name*/ ) const override{
			static std::string empty;
			return empty;
		}

		IDType ID() const noexcept override{
			return IDType();
		}
	};

}

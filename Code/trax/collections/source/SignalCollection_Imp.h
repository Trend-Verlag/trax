//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/collections/SignalCollection.h"
#include "trax/ImplementationHelper.h"

namespace trax{

	typedef Container_Imp<Signal,SignalCollection> SignalCollection_Base;

	class SignalCollection_Imp : public SignalCollection_Base{
	public:
		const char* TypeName() const override;
	};

}

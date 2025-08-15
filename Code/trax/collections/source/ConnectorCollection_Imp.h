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

#include "trax/collections/ConnectorCollection.h"
#include "trax/ImplementationHelper.h"

namespace trax{

	typedef Container_Imp<Connector,ConnectorCollection> ConnectorCollection_Base;

	class ConnectorCollection_Imp : public ConnectorCollection_Base{
	public:
		const char* TypeName() const override;
	};

}
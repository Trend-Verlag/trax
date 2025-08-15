// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel



#include "ConnectorCollection_Imp.h"

namespace trax{

std::unique_ptr<ConnectorCollection> ConnectorCollection::Make() noexcept
{
	try{
		return std::make_unique<ConnectorCollection_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

const char* ConnectorCollection_Imp::TypeName() const{
	return "ConnectorCollection";
}

}


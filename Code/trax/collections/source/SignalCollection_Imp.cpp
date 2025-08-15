//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel



#include "SignalCollection_Imp.h"

namespace trax{
////////////////////////////////////////
std::unique_ptr<SignalCollection> SignalCollection::Make() noexcept{
	try{
		return std::make_unique<SignalCollection_Imp>();
	}
	catch( ... ){
		return nullptr;
	}
}
////////////////////////////////////////
const char* SignalCollection_Imp::TypeName() const{
	return "SignalCollection";
}
////////////////////////////////////////
}

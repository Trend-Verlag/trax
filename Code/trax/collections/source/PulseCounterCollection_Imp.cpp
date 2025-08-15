// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
// 
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#include "PulseCounterCollection_Imp.h"

namespace trax{
////////////////////////////////////////
std::unique_ptr<PulseCounterCollection> PulseCounterCollection::Make() noexcept{
	return std::make_unique<PulseCounterCollection_Imp>();
}
////////////////////////////////////////
const char* PulseCounterCollection_Imp::TypeName() const noexcept{
	return "PulseCounterCollection";
}
////////////////////////////////////////
}
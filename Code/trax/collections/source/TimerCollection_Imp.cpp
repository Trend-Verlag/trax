// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
// 
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#include "TimerCollection_Imp.h"

namespace trax{
////////////////////////////////////////
std::unique_ptr<TimerCollection> TimerCollection::Make() noexcept{
	return std::make_unique<TimerCollection_Imp>();
}
////////////////////////////////////////
const char* TimerCollection_Imp::TypeName() const noexcept{
	return "TimerCollection";
}

void TimerCollection_Imp::Update(){
	for( auto& timer : *this )
		timer.Update();
}
////////////////////////////////////////
}
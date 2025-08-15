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
#include "trax/collections/TimerCollection.h"
#include "trax/source/Jack_Imp.h"

namespace trax{

	class TimerCollection_Imp : public JackEnumerator_Imp<Container_Imp<Timer,TimerCollection>>{
	public:
		const char*	TypeName() const noexcept override;

		void Update() override;


		// Inherited via JackEnumerator
		//const std::string& Reference( const std::string & name ) const override{
		//	static std::string empty;
		//	return empty;
		//}

		//IDType ID() const override{
		//	return IDType();
		//}
	};

}
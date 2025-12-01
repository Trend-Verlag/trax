//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//
// Copyright (c) 2025 Trend Redaktions- und Verlagsgesellschaft mbH
// Copyright (c) 2019 Marc-Michael Horstmann
//
// Permission is hereby granted to any person obtaining a copy of this software 
// and associated source code (the "Software"), to use, view, and study the 
// Software for personal or internal business purposes, subject to the following 
// conditions:
//
// 1. Redistribution, modification, sublicensing, or commercial use of the 
// Software is NOT permitted without prior written consent from the copyright 
// holder.
//
// 2. The Software is provided "AS IS", without warranty of any kind, express 
// or implied.
//
// 3. All copies of the Software must retain this license notice.
//
// For further information, please contact: horstmann.marc@trendverlag.de

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
//	trax track library
//	AD 2026 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
// 
// 
// Copyright (c) 2026 Trend Redaktions- und Verlagsgesellschaft mbH
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

#include "Consist_Imp.h"

namespace trax{

std::unique_ptr<Consist> trax::Consist::Make() noexcept
{
	try{
		return std::make_unique<Consist_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

Consist_Imp::Consist_Imp() noexcept
{}

const char*	Consist_Imp::TypeName() const noexcept{
	return "Consist";
}

}

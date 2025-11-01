//	trax track library
//	AD 2025 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//
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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "../ModuleCollection.h"

#include "trax/ImplementationHelper.h"

namespace trax{

	typedef Container_Imp<Module,ModuleCollection> ModuleCollection_Base;

	class ModuleCollection_Imp : public ModuleCollection_Base
	{
	public:
		ModuleCollection_Imp();

		// ModuleCollection interface
		const char* TypeName() const override;

		bool IsValid( bool bSilent = true ) const noexcept override;
	};

}

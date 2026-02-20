//	trax track library
//	AD 2026 
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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/collections/TrackSystemParser.h"
#include "trax/rigid/trains/collections/FleetParser.h"

namespace trax{

	struct Module;
	struct ModuleCollection;

	struct ModuleParser : Parser{

		virtual TrackSystemParser& GetTrackSystemParser() = 0;

		virtual FleetParser& GetFleetParser() = 0;

		virtual bool ModuleStart( IDType /*id*/ ) noexcept(false) { return true; }



		virtual void ModuleEnd() noexcept( false ) {}


	};


	struct ModuleCollectionParser : Parser{

		virtual ModuleParser& GetModuleParser() noexcept(false) = 0;

		virtual bool ModuleCollectionStart() noexcept(false) { return true; }

		virtual void ModuleCollectionEnd() noexcept(false) {}

	};


	bool dclspc ParseModule( const Module& module, ModuleParser& callback ) noexcept(false);

	bool dclspc ParseModuleCollection( const ModuleCollection& moduleCollection, ModuleCollectionParser& callback ) noexcept(false);

}

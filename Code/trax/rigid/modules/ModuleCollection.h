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
// For further information, please contact: horstmann@traxlibrary.dev

#pragma once

#include "trax/Configuration.h"
#include "trax/collections/Collection.h"

namespace trax{

	struct Module;
	struct Terrain;

	/// \brief A collection of modules.
	///
	/// Holds several modules that m
	struct ModuleCollection : Collection<ModuleCollection,Module>
	{
		/// \brief Makes a standard ModuleCollection object.
		static dclspc std::unique_ptr<ModuleCollection> Make() noexcept;


		/// \brief Attaches a Terrain to this Module.
		/// \param pTerrain Pointer to object to attach or nullptr to remove.
		virtual void Attach( std::shared_ptr<Terrain> pTerrain ) noexcept = 0;


		/// \returns A pointer to the attached Terrain.
		virtual std::shared_ptr<Terrain> GetTerrain() const noexcept = 0;


	};
}

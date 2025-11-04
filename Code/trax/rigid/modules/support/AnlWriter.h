// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2024
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include <filesystem>

namespace trax{

	struct Module;
	struct ModuleCollection;

	void Write( const ModuleCollection& moduleSet, const std::filesystem::path& path );
	
	void Write( const Module& modul, const std::filesystem::path& path );

} // namespace trax

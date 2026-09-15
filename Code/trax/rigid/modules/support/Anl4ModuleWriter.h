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

#include "trax/Configuration.h"

#include <filesystem>

namespace trax{

	struct Camera;
	struct CameraCollection;
	struct Module;
	struct ModuleCollection;

	void dclspc WriteModuleCollection( const ModuleCollection& moduleCollection, const std::filesystem::path& anl4FilePath );

} // namespace trax

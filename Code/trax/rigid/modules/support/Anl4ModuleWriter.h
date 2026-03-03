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
#include <boost/property_tree/ptree.hpp>
#include <filesystem>

namespace trax{

	struct Camera;
	struct CameraCollection;
	struct Module;
	struct ModuleCollection;

	void dclspc WriteModuleCollection( const ModuleCollection& moduleCollection, const std::filesystem::path& anl4FilePath );

	namespace ptreesupport{
		/// \name Property Tree Streaming Support for Trax Classes
		///@{
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const ModuleCollection& moduleCollection );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Module& module );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const CameraCollection& cameraCollection );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Camera& camera );
		///@}

	} // namespace ptreesupport
} // namespace trax

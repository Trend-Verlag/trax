// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2026 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2026
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/support/TraxSupportXML.h"

namespace trax{
	
	struct Fleet;
	struct Train;
	struct TrainComponent;
	struct RailRunner;
	struct RollingStock;
	struct Bogie;
	struct WheelFrame;

	namespace ptreesupport{
		/// \name Property Tree Streaming Support for Trax Classes
		///@{
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const RailRunner& railRunner );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Train& train );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const RollingStock& rollingStock );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Bogie& bogie );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const WheelFrame& wheelFrame );
		///@}
	} 
}

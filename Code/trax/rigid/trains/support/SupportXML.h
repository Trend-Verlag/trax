// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "If you belive it,
//  don't keep it all inside"
//
//					Stiltskin


#pragma once

#include "trax/rigid/support/RigidSupportXML.h"

namespace trax{

	struct Bogie;
	struct RollingStock;
	struct Scene;
	struct Train;
	struct WheelFrame;

	namespace ptreesupport{

		/// \name Explicit XML Reading
		///@{	
		void dclspc Read( const boost::property_tree::ptree& pt, Scene& scene, Train& train );
		void dclspc Read( const boost::property_tree::ptree& pt, Scene& scene, RollingStock& rollingStock );
		void dclspc Read( const boost::property_tree::ptree& pt, Scene& scene, Bogie& bogie );
		void dclspc Read( const boost::property_tree::ptree& pt, Scene& scene, WheelFrame& wheelFrame );
		///@}

	}
}

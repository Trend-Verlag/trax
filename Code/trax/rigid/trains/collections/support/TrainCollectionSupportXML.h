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

#pragma once

#include "common/support/CommonSupportXML.h"
#include "trax/Configuration.h"

namespace trax{

	struct Fleet;
	struct Scene;

	namespace ptreesupport{

		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Fleet& fleet );

		/// \name Property Tree Streaming Support for Trax Classes
		///@{
		void dclspc Read( const boost::property_tree::ptree& pt, Scene& scene, Fleet& fleet );
		///@}

	}
}


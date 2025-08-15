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

#include "trax/Parser.h"

#include <boost/property_tree/ptree.hpp>

namespace trax{
	namespace ptreesupport{

		void ParseDisplayName( const boost::property_tree::ptree& pt, Parser& callback ) noexcept;

		void ParseDescription( const boost::property_tree::ptree& pt, Parser& callback ) noexcept;

		void ParseInterval( const boost::property_tree::ptree& pt, Parser& callback ) noexcept;

		void ParseFrame( const boost::property_tree::ptree& pt, Parser& callback ) noexcept;

		void ParseArea( const boost::property_tree::ptree& pt, Parser& callback ) noexcept;

	}
}


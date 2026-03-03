//	trax track library
//	AD 2014 
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

#include "trax/collections/Collection.h"
#include "trax/support/TraxSupportXML.h"

namespace trax{
	namespace ptreesupport{

		/// \name Property Tree Streaming Support for Trax Classes
		///@{
		template< class Collection_Type, class Value_Type >
		inline boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const Collection<Collection_Type,Value_Type>& collection ){	
			boost::property_tree::ptree ptCollection;

			for( const auto& element : collection )
				ptCollection << element;

			move_child( pt, collection.TypeName(), ptCollection );
			return pt;
		} // do not move away from here, doesn't belong to CollectionSupportXML.h since it is writing code ...

	} // namespace ptreesupport
} // namespace trax
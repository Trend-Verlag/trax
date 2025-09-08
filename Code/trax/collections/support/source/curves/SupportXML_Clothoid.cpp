//	trax track library
//	AD 2022 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#include "trax/collections/support/CollectionSupportXML.h"

namespace trax
{
namespace ptreesupport
{
	using namespace dim::ptreesupport;

	std::unique_ptr<Curve> CreateClothoid( const boost::property_tree::ptree& pt )
	{
		if( std::unique_ptr<Clothoid> pClothoid = Clothoid::Make() ){
			Clothoid::Data data;
			data.a = get( pt, "<xmlattr>.a", data.a, _m );

			pClothoid->Create( data );
			return pClothoid;
		}

		return nullptr;
	}
}
}
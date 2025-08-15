// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2022 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#include "trax/collections/support/CollectionSupportXML.h"

namespace trax
{
	namespace ptreesupport
	{
		std::unique_ptr<Curve> CreateLine( const boost::property_tree::ptree& /*pt*/ )
		{
			if( auto pLine = Line::Make() )
			{
				return pLine;
			}

			return nullptr;
		}
	}
}
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
		std::unique_ptr<Curve> CreateCubic( const boost::property_tree::ptree& pt )
		{
			if( std::unique_ptr<Cubic> pCubic = Cubic::Make() ){
				Cubic::Data data;
				Read( pt, data );
				pCubic->Create( data );
				return pCubic;
			}

			return nullptr;
		}	
	}
}
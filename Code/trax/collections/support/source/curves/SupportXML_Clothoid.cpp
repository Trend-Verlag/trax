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
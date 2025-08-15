// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
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

	std::unique_ptr<Curve> CreateHelix( const boost::property_tree::ptree& pt )
	{
		if( std::unique_ptr<Helix> pHelix = Helix::Make() ){
			Helix::Data data;

			data.k = get( pt, "<xmlattr>.k", data.k, _1Im );
			data.t = get( pt, "<xmlattr>.t", data.t, _1Im );

			pHelix->Create( data );
			return pHelix;
		}

		return nullptr;
	}
}
}
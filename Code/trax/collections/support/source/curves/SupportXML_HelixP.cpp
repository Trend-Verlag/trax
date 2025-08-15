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
	using namespace spat::ptreesupport;

	std::unique_ptr<Curve> CreateHelixP( const boost::property_tree::ptree& pt )
	{
		if( std::unique_ptr<HelixP> pHelix = HelixP::Make() ){
			HelixP::Data data;

			data.a = get( pt, "<xmlattr>.a", data.a, _m );
			data.b = get( pt, "<xmlattr>.b", data.b, _m );

			for( const auto& pair : pt )
			{
				if( pair.first == "VectorBundle2" )
					ReadVectorBundle2( pair.second, data.center );
			}

			pHelix->Create( data );
			return pHelix;
		}

		return nullptr;
	}
}
}
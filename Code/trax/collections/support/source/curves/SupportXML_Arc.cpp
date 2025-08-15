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

	std::unique_ptr<trax::Curve> CreateArc( const boost::property_tree::ptree& pt )
	{
		if( auto pArc = Arc::Make() ){
			Arc::Data data;

			data.k = get( pt, "<xmlattr>.k", data.k, _1Im );

			pArc->Create( data );
			return pArc;
		}

		return nullptr;
	}
}
}
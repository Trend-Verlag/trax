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
	using namespace spat::ptreesupport;

	std::unique_ptr<trax::Curve> CreateEEPCurve( const boost::property_tree::ptree& pt )
	{
		if( auto pEEPCurve = EEPCurve::Make() ){
			EEPCurve::Data data;
         	Read( pt, data );
			pEEPCurve->Create( data );
			return pEEPCurve;
		}

		return nullptr;
	}
}
}
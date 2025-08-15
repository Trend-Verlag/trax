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
		std::unique_ptr<Curve> CreateSampledCurve( const boost::property_tree::ptree& pt )
		{
			if( std::unique_ptr<SampledCurve> pSampledCurve = SampledCurve::Make() )
			{
				SampledCurve::Data data;

				for( const auto& pair : pt ){
					if( pair.first == "Sample" ){
						data.push_back( {} );
						Read( pair.second, data.back() );
					}
				}

				pSampledCurve->Create( data );
				return pSampledCurve;
			}

			return nullptr;
		}
	}
}
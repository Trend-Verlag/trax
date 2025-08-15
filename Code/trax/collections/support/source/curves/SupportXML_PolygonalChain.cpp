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

		std::unique_ptr<Curve> CreatePolygonalChain( const boost::property_tree::ptree& pt )
		{
			if( std::unique_ptr<PolygonalChain> pPolygonalChain = PolygonalChain::Make() )
			{
				PolygonalChain::Data data;

				for( const auto& pair : pt ){
					if( pair.first == "VectorBundle" ){
						data.push_back( {} );
						ReadVectorBundle( pair.second, data.back() );
					}
				}

				pPolygonalChain->Create( data );
				return pPolygonalChain;
			}

			return nullptr;
		}
	}
}
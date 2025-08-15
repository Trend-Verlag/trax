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

		std::unique_ptr<Curve> CreateLineP( const boost::property_tree::ptree& pt )
		{
			if( auto pLine = LineP::Make() )
			{
				LineP::Data data;
				for( const auto& pair : pt )
				{
					if( pair.first == "VectorBundle" ){
						ReadVectorBundle( pair.second, data.vb );
						data.vb.T.Normalize();
					}

					else if( pair.first == "Vector" )
						ReadVector( pair.second, data.up );
				}

				pLine->Create( data );
				return pLine;
			}

			return nullptr;
		}
	}
}
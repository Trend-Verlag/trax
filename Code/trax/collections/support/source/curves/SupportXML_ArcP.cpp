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

		std::unique_ptr<trax::Curve> CreateArcP( const boost::property_tree::ptree& pt )
		{
			if( auto pArc = ArcP::Make() ){
				ArcP::Data data;

				for( const auto& pair : pt )
				{
					if( pair.first == "VectorBundle2" ){
						ReadVectorBundle2( pair.second, data.vb2 );
					}
				}

				pArc->Create( data );
				return pArc;
			}

			return nullptr;
		}
	}
}
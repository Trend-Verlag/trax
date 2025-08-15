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
		std::unique_ptr<Curve> CreateSpline( const boost::property_tree::ptree& pt )
		{
			if( std::unique_ptr<Spline> pSpline = Spline::Make() ){
				Spline::Data data;

				for( const auto& pair : pt )
				{
					if( pair.first == "Cubic" ){
						data.push_back( {} );
						Read( pair.second, data.back() );
					}
				}

				pSpline->Create( data );
				return pSpline;
			}

			return nullptr;
		}
	}
}
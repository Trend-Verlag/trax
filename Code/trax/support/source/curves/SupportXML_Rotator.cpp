//	trax track library
//	AD 2022 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//
// Copyright (c) 2025 Trend Redaktions- und Verlagsgesellschaft mbH
// Copyright (c) 2019 Marc-Michael Horstmann
//
// Permission is hereby granted to any person obtaining a copy of this software 
// and associated source code (the "Software"), to use, view, and study the 
// Software for personal or internal business purposes, subject to the following 
// conditions:
//
// 1. Redistribution, modification, sublicensing, or commercial use of the 
// Software is NOT permitted without prior written consent from the copyright 
// holder.
//
// 2. The Software is provided "AS IS", without warranty of any kind, express 
// or implied.
//
// 3. All copies of the Software must retain this license notice.
//
// For further information, please contact: horstmann.marc@trendverlag.de

#include "trax/collections/support/CollectionSupportXML.h"

namespace trax
{
	namespace ptreesupport
	{
		std::unique_ptr<Curve> CreateRotator( const boost::property_tree::ptree& pt )
		{
			Rotator::Data data;
			data.a = pt.get( "<xmlattr>.a", data.a );
			data.b = pt.get( "<xmlattr>.b", data.b );
			data.a0 = pt.get( "<xmlattr>.a0", data.a0 );
			data.b0 = pt.get( "<xmlattr>.b0", data.b0 );

			if( data.a0 == 0_rad && data.b0 == 0_rad ){
				if( std::unique_ptr<Rotator> pRotator = Rotator::Make() ){
					pRotator->Create( data );
					return pRotator;
				}
			}
			else{
				if( std::unique_ptr<Rotator> pRotator = Rotator::Make( Curve::CurveType::RotatorWithOffset ) ){
					pRotator->Create( data );
					return pRotator;
				}
			}

			return nullptr;
		}

		std::unique_ptr<Curve> CreateRotatorChain( const boost::property_tree::ptree& pt )
		{
			if( std::unique_ptr<RotatorChain> pRotatorChain = RotatorChain::Make() )
			{
				RotatorChain::Data data;

				for( const auto& pair : pt ){
					if( pair.first == "Link" )
						data.push_back( {	
							pair.second.get( "<xmlattr>.a", 0_rad ),
							pair.second.get( "<xmlattr>.b", 0_rad ),
							pair.second.get( "<xmlattr>.length", 1_m ) } );
				}

				pRotatorChain->Create( data );
				return pRotatorChain;
			}

			return nullptr;
		}
	}
}
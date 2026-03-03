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
		using namespace dim::ptreesupport;
		using namespace spat::ptreesupport;

        const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Line& line )
        {
            return pt;
        }

        const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, LineP& lineP )
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

		    lineP.Create( data );
		    return pt;
        }

        const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Arc& arc )
        {
    		Arc::Data data;
			data.k = get( pt, "<xmlattr>.k", data.k, _1Im );
			arc.Create( data );
		    return pt;
        }

        const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, ArcP& arcP )
        {
  			ArcP::Data data;

			for( const auto& pair : pt )
			{
				if( pair.first == "VectorBundle2" ){
					ReadVectorBundle2( pair.second, data.vb2 );
				}
			}

			arcP.Create( data );
		    return pt;
        }

        const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Helix& helix )
        {
 			Helix::Data data;

			data.k = get( pt, "<xmlattr>.k", data.k, _1Im );
			data.t = get( pt, "<xmlattr>.t", data.t, _1Im );

			helix.Create( data );
		    return pt;
        }

        const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, HelixP& helixP )
        {
			HelixP::Data data;

			data.a = get( pt, "<xmlattr>.a", data.a, _m );
			data.b = get( pt, "<xmlattr>.b", data.b, _m );

			for( const auto& pair : pt )
			{
				if( pair.first == "VectorBundle2" )
					ReadVectorBundle2( pair.second, data.center );
			}

			helixP.Create( data );
		    return pt;
        }

        const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Cubic& cubic )
        {
			Cubic::Data data;
			Read( pt, data );
			cubic.Create( data );
		    return pt;
        }

        const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Spline& spline )
        {
 			Spline::Data data;

			for( const auto& pair : pt )
			{
				if( pair.first == "Cubic" ){
					data.push_back( {} );
					Read( pair.second, data.back() );
				}
			}

			spline.Create( data );
		    return pt;
        }

        const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Clothoid& clothoid )
        {
 			Clothoid::Data data;
			data.a = get( pt, "<xmlattr>.a", data.a, _m );
			clothoid.Create( data );
			return pt;
        }

        const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Rotator& rotator )
        {
			Rotator::Data data;
			data.a = pt.get( "<xmlattr>.a", data.a );
			data.b = pt.get( "<xmlattr>.b", data.b );
			data.a0 = pt.get( "<xmlattr>.a0", data.a0 );
			data.b0 = pt.get( "<xmlattr>.b0", data.b0 );
			rotator.Create( data );
		    return pt;
        }

        const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, RotatorChain& rotatorChain )
        {
 			RotatorChain::Data data;

			for( const auto& pair : pt ){
				if( pair.first == "Link" )
					data.push_back( {	
						pair.second.get( "<xmlattr>.a", 0_rad ),
						pair.second.get( "<xmlattr>.b", 0_rad ),
						pair.second.get( "<xmlattr>.length", 1_m ) } );
			}

			rotatorChain.Create( data );
		    return pt;
        }

        const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, PolygonalChain& polygonalChain )
        {
  			PolygonalChain::Data data;

			for( const auto& pair : pt ){
				if( pair.first == "VectorBundle" ){
					data.push_back( {} );
					ReadVectorBundle( pair.second, data.back() );
				}
			}

			polygonalChain.Create( data );
		    return pt;
        }

        const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, SampledCurve& sampledCurve )
        {
			SampledCurve::Data data;

			for( const auto& pair : pt ){
				if( pair.first == "Sample" ){
					data.push_back( {} );
					Read( pair.second, data.back() );
				}
			}

			sampledCurve.Create( data );
		    return pt;
        }
	}
}
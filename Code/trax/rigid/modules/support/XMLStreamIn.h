//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//
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
// For further information, please contact: horstmann@traxlibrary.dev

#pragma once

#include "trax/Identified.h"
#include "trax/collections/Interfaces.h"

#include <boost/property_tree/ptree.hpp>

namespace trax{

	namespace ptreesupport{

		/// \name Property tree streaming reading support for trax classes
		///@{

		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, TrackSystem& trackSystem );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, TrackCollection& trackCollection );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, ConnectorCollection& connectorCollection );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, IndicatorCollection& indicatorCollection );

		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, TrackBuilder& track );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Section& section );

		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Switch& switchObject );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, ThreeWaySwitch& threeWaySwitch );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, SingleSlipSwitch& singleSlipSwitch );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, DoubleSlipSwitch& doubleSlipSwitch );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, BinaryIndicator& binaryIndicator );
		
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Line& line );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, LineP& lineP );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Arc& arc );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, ArcP& arcP );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Helix& helix );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, HelixP& helixP );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Cubic& cubic );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Spline& spline );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Clothoid& clothoid );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Rotator& rotator );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, RotatorChain& rotatorChain );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, PolygonalChain& polygonalChain );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, SampledCurve& sampledCurve );

		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, ConstantTwist& constantTwist );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, LinearTwist& linearTwist );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, PiecewiseTwist& piecewiseTwist );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, DirectionalTwist& directionalTwist );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, PiecewiseDirectionalTwist& piecewiseDirectionalTwist );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, CombinedTwist& combinedTwist );

		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Plug& plug );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, MultiPlug& plug );
		dclspc const boost::property_tree::ptree& operator >> ( const boost::property_tree::ptree& pt, Jack& jack );
		///@}


		template<class Interface>
		inline void ReadLocalizedTag( 
			const boost::property_tree::ptree& pt, 
			const char* tagName, 
			Interface& refInterface, 
			const char* localeString )
		{
			auto optional = pt.get_optional<std::string>( "<xmlattr>.language" );
			if( optional ){
				refInterface.Reference( std::string{ tagName } + "_" + *optional, pt.data() );

				if( refInterface.IsEmptyReference( tagName ) || localeString == *optional )
					refInterface.Reference( tagName, pt.data() );
			}
			else{
				refInterface.Reference( std::string{ tagName } + "_unknown", pt.data() );

				if( refInterface.IsEmptyReference( tagName ) )
					refInterface.Reference( tagName, pt.data() );
			}
		}

		template<class T>
		inline void AttributesToReferences( const boost::property_tree::ptree& pt, Identified<T>& refInterface ){
			const auto iter = pt.find( "<xmlattr>" );
			if( iter != pt.not_found() )
			{
				for( const auto& attribute : iter->second ){
					refInterface.Reference( attribute.first, attribute.second.data() );
				}
			}
		}
	}
}

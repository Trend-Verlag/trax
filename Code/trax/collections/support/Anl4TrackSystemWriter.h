//	trax track library
//	AD 2024
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

#pragma once

#include "trax/collections/Interfaces.h"
#include "trax/collections/Collection.h"

#include "trax/support/TraxSupportXML.h"

namespace trax{

	namespace ptreesupport{

		/// \name Property Tree Streaming Support for Trax Classes
		///@{
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const TrackSystem& trackSystem );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const TrackCollection& trackCollection );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const TrackBuilder& track );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Location& location );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const TrackLocation& tlr );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const TrackLocationRef& tlr );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const TrackRange& tr );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Sensor& sensor );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Section& section );

	//	dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const ConnectorCollection& connectorCollection );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Connector& connector );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const SignalCollection& signalCollection );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const VelocityControl& signal );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const JumpSite& signal );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Indicator& indicator );

		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Curve& curve );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const CurveSample& sample );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Line& curve );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Arc& curve );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Helix& curve );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const LineP& curve );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const ArcP& curve );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const HelixP& curve );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Cubic& curve );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Spline& curve );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Clothoid& curve );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Rotator& curve );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const RotatorChain& curve );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const PolygonalChain& curve );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const SampledCurve& curve );

		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const RoadwayTwist& twist );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const ConstantTwist& twist );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const LinearTwist& twist );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const PiecewiseTwist& twist );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const DirectionalTwist& twist );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const CombinedTwist& twist );

		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const PlugEnumerator& plugEnumerator );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Plug& plug );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const JackEnumerator& jackEnumerator );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Jack& jack );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const PulseCounter& pulseCounter );
		dclspc boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Timer& timer );
		///@}


		/// \name Property Tree Streaming Support for Trax Classes
		///@{
		template< class Collection_Type, class Value_Type >
		boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const Collection<Collection_Type,Value_Type>& collection ){	
			boost::property_tree::ptree ptCollection;

			for( const auto& element : collection )
				ptCollection << element;

			move_child( pt, collection.TypeName(), ptCollection );
			return pt;
		} // do not move away from here, doesn't belong to CollectionSupportXML.h since it is writing code ...


	} // namespace ptreesupport
} // namespace trax

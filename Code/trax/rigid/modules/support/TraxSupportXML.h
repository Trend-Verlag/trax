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

#include "common/support/CommonSupportXML.h"
#include "dim/support/DimSupportXML.h"
#include "spat/support/SpatSupportXML.h"

#include "trax/Configuration.h"
#include "trax/Interfaces.h"
#include "trax/Curve.h"
#include "trax/Track.h"

namespace trax{

	namespace ptreesupport{

		using spat::ptreesupport::operator<<;



		/// \name Explicit XML Reading
		///@{
		dclspc std::unique_ptr<Curve> CreateCurve( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateLine( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateLineP( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateArc( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateArcP( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateHelix( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateHelixP( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateCubic( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateSpline( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateClothoid( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateRotator( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateRotatorChain( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreatePolygonalChain( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateSampledCurve( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<Curve> CreateEEPCurve( const boost::property_tree::ptree& pt );

		dclspc std::unique_ptr<RoadwayTwist> CreateTwist( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<RoadwayTwist> CreateConstantTwist( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<RoadwayTwist> CreateLinearTwist( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<RoadwayTwist> CreatePiecewiseTwist( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<RoadwayTwist> CreatePiecewiseLinearTwist( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<RoadwayTwist> CreatePiecewiseCircularTwist( const boost::property_tree::ptree& pt );
		dclspc std::unique_ptr<RoadwayTwist> CreateDirectionalTwist( const boost::property_tree::ptree& pt );
		dclspc void ReadConnection( const boost::property_tree::ptree& pt, Track::End& trackend );
		dclspc void ReadTrackEnd( const boost::property_tree::ptree& pt, Track::End& trackend );

		dclspc void ReadTrackLocation( const boost::property_tree::ptree& pt, TrackLocation& trackLocation ) noexcept;
		dclspc void ReadTrackLocationRef( const boost::property_tree::ptree& pt, TrackSystemLocation& trackLocationRef ) noexcept;
		dclspc void ReadTrackRange( const boost::property_tree::ptree& pt, TrackRange& trackRange ) noexcept;

		dclspc void Read( const boost::property_tree::ptree& pt, Cubic::Data& curve );
		dclspc void Read( const boost::property_tree::ptree& pt, CurveSample& sample );
		dclspc void Read( const boost::property_tree::ptree& pt, EEPCurve::Data& data );
		///@}

	} // namespace ptreesupport
} // namespace trax
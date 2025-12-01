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
        void RegisterAllCurveReaders( PTreeReader& toReader )
        {
            toReader.RegisterCurveReader( TypeToName( Curve::CurveType::Line ), CreateLine );
            toReader.RegisterCurveReader( TypeToName( Curve::CurveType::LineP ), CreateLineP );
            toReader.RegisterCurveReader( TypeToName( Curve::CurveType::Arc ), CreateArc );
            toReader.RegisterCurveReader( TypeToName( Curve::CurveType::ArcP ), CreateArcP );
            toReader.RegisterCurveReader( TypeToName( Curve::CurveType::Helix ), CreateHelix );
            toReader.RegisterCurveReader( TypeToName( Curve::CurveType::HelixP ), CreateHelixP );
            toReader.RegisterCurveReader( TypeToName( Curve::CurveType::Cubic ), CreateCubic );
            toReader.RegisterCurveReader( TypeToName( Curve::CurveType::Spline ), CreateSpline );
            toReader.RegisterCurveReader( TypeToName( Curve::CurveType::Clothoid ), CreateClothoid );
            toReader.RegisterCurveReader( TypeToName( Curve::CurveType::Rotator ), CreateRotator );
            toReader.RegisterCurveReader( TypeToName( Curve::CurveType::RotatorChain ), CreateRotatorChain );
            toReader.RegisterCurveReader( TypeToName( Curve::CurveType::PolygonalChain ), CreatePolygonalChain );
            toReader.RegisterCurveReader( TypeToName( Curve::CurveType::SampledCurve ), CreateSampledCurve );
			toReader.RegisterCurveReader( TypeToName( Curve::CurveType::EEPCurve ), CreateEEPCurve );
        }
	}
}
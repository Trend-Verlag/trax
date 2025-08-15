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
#ifdef TRAX_SUPPORT_EEPCURVE
			toReader.RegisterCurveReader( TypeToName( Curve::CurveType::EEPCurve ), CreateEEPCurve );
#endif
        }
	}
}
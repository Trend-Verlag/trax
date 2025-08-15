// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2024 
//
//  "Rome wasn't built in a day,
//   But in twenty years 
//   You should have something."
//
//						Johnossi


#pragma once

#include "dim/DimensionedValues.h"

#if defined(_MSC_VER)
#	pragma warning( push )
#	pragma warning( disable: 4003 ) // not enough arguments for function-like macro invocation
#	pragma warning( disable: 4244 ) // 'initializing': conversion from '' to '', possible loss of data
#	pragma warning( disable: 4305 ) // 'argument': truncation from 'double' to 'float'
#endif
#include <pxr/usd/usdGeom/basisCurves.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/xformable.h>
#if defined(_MSC_VER)
#	pragma warning( pop )
#endif


namespace trax{

	struct Curve;
	struct CurveSample;
    struct CubicData;
	struct Line;
	struct LineP;
	struct Arc;
	struct ArcP;
	struct Helix;
	struct HelixP;
	struct Cubic;
	struct Spline;
	struct Clothoid;
	struct Rotator;
	struct RotatorChain;
	struct PolygonalChain;
	struct SampledCurve;

	struct RoadwayTwist;
	struct ConstantTwist;
	struct LinearTwist;
	struct PiecewiseTwist;
	struct DirectionalTwist;
	struct PiecewiseDirectionalTwist;
	struct CombinedTwist;

    struct Section;

	struct TrackBuilder;
	struct Connector;
	struct Switch;
	struct ThreeWaySwitch;
	struct SingleSlipSwitch;
	struct DoubleSlipSwitch;
	struct TrackSystem;
	struct TrackCollection;
	struct ConnectorCollection;

}

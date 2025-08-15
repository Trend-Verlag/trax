// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#pragma once

#include "trax/Configuration.h"
#include <pxr/usd/usdGeom/tokens.h>

namespace trax{


/// \class UsdTraxTokenTypes
/// \brief Token identifiers for trax USD Schema definitions.
struct UsdTraxTokenTypes {
	dclspc UsdTraxTokenTypes();
	/// \brief "TSInterval"
	///
	/// Schema identifer for UsdTSInterval
	/// UsdTSTrack
	const pxr::TfToken Interval;
	/// \brief "near"
	///
	/// Property identifier for UsdTSInterval
	const pxr::TfToken Near;
	/// \brief "far"
	///
	/// Property identifier for UsdTSInterval
	const pxr::TfToken Far;
	/// \brief "intervalNear"
	///
	/// Possible property identifier for UsdTSTrack
    const pxr::TfToken IntervalNear;
	/// \brief "intervalFar"
	///
	/// Possible property identifier for UsdTSTrack
    const pxr::TfToken IntervalFar;
	/// \brief "TSPosition"
	///
	/// Schema identifer for UsdTSPosition
	const pxr::TfToken Position;
	/// \brief "x"
	///
	/// Property identifier for UsdTSPosition
	const pxr::TfToken X;
	/// \brief "y"
	///
	/// Property identifier for UsdTSPosition
	const pxr::TfToken Y;
	/// \brief "z"
	///
	/// Property identifier for UsdTSPosition
	const pxr::TfToken Z;
	/// \brief "TSVector"
	///
	/// Schema identifer for UsdTSVector
	const pxr::TfToken Vector;
	/// \brief "TSDistance"
	///
	/// Schema identifer for UsdTSDistance
	const pxr::TfToken Distance;
	/// \brief "TSAttractor"
	///
	/// Schema identifer for UsdTSAttractor
	const pxr::TfToken Attractor;
	/// \brief "dx"
	///
	/// Property identifier for UsdTSVector, UsdTSTangent, UsdTSNormal, UsdTSBinormal, UsdTSDistance, UsdTSAttractor
	const pxr::TfToken Dx;
	/// \brief "dy"
	///
	/// Property identifier for UsdTSVector, UsdTSTangent, UsdTSNormal, UsdTSBinormal, UsdTSDistance, UsdTSAttractor
	const pxr::TfToken Dy;
	/// \brief "dz"
	///
	/// Property identifier for UsdTSVector, UsdTSTangent, UsdTSNormal, UsdTSBinormal, UsdTSDistance, UsdTSAttractor
	const pxr::TfToken Dz;
	/// \brief "TSTangent"
	///
	/// Schema identifer for UsdTSTangent
	/// UsdTSFrame
	const pxr::TfToken Tangent;
	/// \brief "TSNormal"
	///
	/// Schema identifer for UsdTSNormal
	/// UsdTSFrame
	const pxr::TfToken Normal;
	/// \brief "TSBinormal"
	///
	/// Schema identifer for UsdTSBinormal
	/// UsdTSFrame
	const pxr::TfToken Binormal;
	/// \brief "TSVectorBundle"
	///
	/// Schema identifer for UsdTSVectorBundle
	const pxr::TfToken VectorBundle;
	/// \brief "TSVectorBundle2"
	///
	/// Schema identifer for UsdTSVectorBundle2
	const pxr::TfToken VectorBundle2;
	/// \brief "TSFrame"
	///
	/// Schema identifer for UsdTSFrame
	/// UsdTSTrack
	const pxr::TfToken Frame;
	/// \brief "TSTrackSystem"
	///
	/// Schema identifer for UsdTSTrackSystem
	const pxr::TfToken TrackSystem;
	/// \brief "TSTrackCollection"
	///
	/// Schema identifer for UsdTSTrackCollection
	/// UsdTSTrackSystem
	const pxr::TfToken TrackCollection;
	/// \brief "TSTrack"
	///
	/// Schema identifer for UsdTSTrack
	/// UsdTSTrackCollection, UsdTSTrackSystem
	const pxr::TfToken Track;
	/// \brief "trackBody"
	///
	/// Property identifier for UsdTSTrack
	const pxr::TfToken TrackBody;
	/// \brief "frontConnection"
	///
	/// Property identifier for UsdTSTrack
	const pxr::TfToken FrontConnection;
	/// \brief "frontConnectionEndType"
	///
	/// Property identifier for UsdTSTrack
	const pxr::TfToken FrontConnectionEnd;
	/// \brief "endConnection"
	///
	/// Property identifier for UsdTSTrack
	const pxr::TfToken EndConnection;
	/// \brief "endConnectionEndType"
	///
	/// Property identifier for UsdTSTrack
	const pxr::TfToken EndConnectionEnd;
	/// \brief "front"
	///
	/// Value identifier for frontConnectionEndType, endConnectionEndType
	const pxr::TfToken FrontEnd;
	/// \brief "end"
	///
	/// Value identifier for frontConnectionEndType, endConnectionEndType
	const pxr::TfToken EndEnd;
	/// \brief "TSCurveSample"
	///
	/// Schema identifer for UsdTSCurveSample
	const pxr::TfToken CurveSample;
	/// \brief "TSCubicData"
	///
	/// Schema identifer for UsdTSCubicData
	const pxr::TfToken CubicData;
	/// \brief "TSLine"
	///
	/// Schema identifer for UsdTSLine
	/// UsdTSTrack
	const pxr::TfToken Line;
	/// \brief "TSLineP"
	///
	/// Schema identifer for UsdTSLineP
	/// UsdTSTrack
	const pxr::TfToken LineP;
	/// \brief "TSArc"
	///
	///	Schema identifer for UsdTSArc
	const pxr::TfToken Arc;
	/// \brief "TSArcP"
	///
	/// Schema identifer for UsdTSArcP
	/// UsdTSTrack
	const pxr::TfToken ArcP;
	/// \brief "TSHelix"
	///
	/// Schema identifer for UsdTSHelix
	/// UsdTSTrack
	const pxr::TfToken Helix;
	/// \brief "TSHelixP"
	///
	///	Schema identifer for UsdTSHelixP
	/// UsdTSTrack
	const pxr::TfToken HelixP;
	/// \brief "TSCubic"
	///
	/// Schema identifer for UsdTSCubic
	/// UsdTSTrack
	const pxr::TfToken Cubic;
	/// \brief "TSSpline"
	///
	/// Schema identifer for UsdTSSpline
	/// UsdTSTrack
	const pxr::TfToken Spline;
	/// \brief "TSClothoid"
	///
	/// Schema identifer for UsdTSClothoid
	/// UsdTSTrack
	const pxr::TfToken Clothoid;
	/// \brief "TSRotator"
	///
	/// Schema identifer for UsdTSRotator
	/// UsdTSTrack
	const pxr::TfToken Rotator;
	/// \brief "TSRotatorChain"
	///
	/// Schema identifer for UsdTSRotatorChain
	/// UsdTSTrack
	const pxr::TfToken RotatorChain;
	/// \brief "TSPolygonalChain"
	///
	/// Schema identifer for UsdTSPolygonalChain
	/// UsdTSTrack
	const pxr::TfToken PolygonalChain;
	/// \brief "TSSampledCurve"
	///
	/// Schema identifer for UsdTSSampledCurve
	/// UsdTSTrack
	const pxr::TfToken SampledCurve;
	/// \brief "curve"
	///
	/// Property identifier for UsdTSTrack
	const pxr::TfToken Curve;
	/// \brief "curvature"
	///
	/// Property identifier for UsdTSArc, UsdTSHelix, UsdTSCurveSample
	const pxr::TfToken Curvature;
	/// \brief "torsion"
	///
	/// Property identifier for UsdTSHelix, UsdTSCurveSample
	const pxr::TfToken Torsion;
	/// \brief "length"
	///
	/// Property identifier for UsdTSCurveSample, UsdTSRotatorChain
	const pxr::TfToken Length;
	/// \brief "a"
	///
	/// Property identifier for UsdTSCubicData
	const pxr::TfToken A;
	/// \brief "b"
	///
	/// Property identifier for UsdTSCubicData
	const pxr::TfToken B;
	/// \brief "c"
	///
	/// Property identifier for UsdTSCubicData
	const pxr::TfToken C;
	/// \brief "d"
	///
	/// Property identifier for UsdTSCubicData
	const pxr::TfToken D;
	/// \brief "a0"
	///
	/// Property identifier for UsdTSRotator
	const pxr::TfToken A0;
	/// \brief "b0"
	///
	/// Property identifier for UsdTSRotator
	const pxr::TfToken B0;
	/// \brief "TSConstantTwist"
	///
	/// Schema identifer for UsdTSConstantTwist
	/// UsdTSTrack
	const pxr::TfToken ConstantTwist;
	/// \brief "TSLinearTwist"
	///
	/// Schema identifer for UsdTSLinearTwist
	/// UsdTSTrack
	const pxr::TfToken LinearTwist;
	/// \brief "TSPiecewiseTwist"
	///
	/// Schema identifer for UsdTSPiecewiseTwist
	/// UsdTSTrack
	const pxr::TfToken PiecewiseTwist;
	/// \brief "TSDirectionalTwist"
	///
	/// Schema identifer for UsdTSDirectionalTwist
	/// UsdTSTrack
	const pxr::TfToken DirectionalTwist;
	/// \brief "TSPiecewiseDirectionalTwist"
	///
	/// Schema identifer for UsdTSPiecewiseDirectionalTwist
	/// UsdTSTrack
	const pxr::TfToken PiecewiseDirectionalTwist;
	/// \brief "TSCombinedTwist"
	///
	/// Schema identifer for UsdTSCombinedTwist
	/// UsdTSTrack
	const pxr::TfToken CombinedTwist;
	/// \brief "TSSection"
	///
	/// Schema identifer for UsdTSSection
	/// UsdTSTrack
	const pxr::TfToken Section;
	/// \brief "type"
	///
	/// Property identifier for UsdTSTrack, UsdTSSection
	const pxr::TfToken Type;
	/// \brief "id"
	///
	/// Property identifier for UsdTSTrack, TSTwoWaySwitch, TSThreeWaySwitch, TSSingleSlipSwitch, TSDoubleSlipSwitch
	const pxr::TfToken Id;
	/// \brief "from"
	///
	/// Property identifier for UsdTSLinearTwist
	const pxr::TfToken From;
	/// \brief "to"
	///
	/// Property identifier for UsdTSLinearTwist
	const pxr::TfToken To;
	/// \brief "TSPair"
	///
	/// Schema identifer for UsdTSPair
	/// UsdTSPiecewiseTwist, UsdTSPiecewiseDirectionalTwist
	const pxr::TfToken Pair;
	/// \brief "TSTuple"
	///
	/// Schema identifer for UsdTSTuple
	/// UsdTSRotatorChain
	const pxr::TfToken Tuple;
	/// \brief "TSConnectorCollection"
	///
	/// Schema identifer for UsdTSConnectorCollection
	/// UsdTSTrackystem
	const pxr::TfToken ConnectorCollection;
	/// \brief "TS2WaySwitch"
	///
	/// Schema identifer for UsdTS2WaySwitch
	/// UsdTSConnectorCollection
	const pxr::TfToken TwoWaySwitch;
	/// \brief "TS3WaySwitch"
	///
	/// Schema identifer for UsdTS3WaySwitch
	/// UsdTSConnectorCollection
	const pxr::TfToken ThreeWaySwitch;
	/// \brief "TSSingleSlipSwitch"
	///
	/// Schema identifer for UsdTSSingleSlipSwitch
	/// UsdTSConnectorCollection
	const pxr::TfToken SingleSlipSwitch;
	/// \brief "TSDoubleSlipSwitch"
	///
	/// Schema identifer for UsdTSDoubleSlipSwitch
	/// UsdTSConnectorCollection
	const pxr::TfToken DoubleSlipSwitch;
	/// \brief "state"
	///
	/// Property identifier for UsdTS2WaySwitch, UsdTS3WaySwitch, UsdTSSingleSlipSwitch, UsdTSDoubleSlipSwitch
	const pxr::TfToken SwitchState;
	/// \brief "narrowTrack"
	///
	/// Property identifier for UsdTS2WaySwitch, UsdTS3WaySwitch
	const pxr::TfToken NarrowTrack;
	/// \brief "narrowTrackEndType"
	///
	/// Property identifier for UsdTS2WaySwitch, UsdTS3WaySwitch
	const pxr::TfToken NarrowTrackEnd;
	/// \brief "straightTrack"
	///
	/// Property identifier for UsdTS2WaySwitch, UsdTS3WaySwitch
	const pxr::TfToken StraightTrack;
	/// \brief "straightTrackEndType"
	///
	/// Property identifier for UsdTS2WaySwitch, UsdTS3WaySwitch
	const pxr::TfToken StraightTrackEnd;
	/// \brief "divergingTrack"
	///
	/// Property identifier for UsdTS2WaySwitch
	const pxr::TfToken DivergingTrack;
	/// \brief "divergingTrackEndType"
	///
	/// Property identifier for UsdTS2WaySwitch
	const pxr::TfToken DivergingTrackEnd;
	/// \brief "divergingTrack1"
	///
	/// Property identifier for UsdTS3WaySwitch
	const pxr::TfToken DivergingTrack1;
	/// \brief "divergingTrack1EndType"
	///
	/// Property identifier for UsdTS3WaySwitch
	const pxr::TfToken DivergingTrack1End;
	/// \brief "divergingTrack2"
	///
	/// Property identifier for UsdTS3WaySwitch
	const pxr::TfToken DivergingTrack2;
	/// \brief "divergingTrack2EndType"
	///
	/// Property identifier for UsdTS3WaySwitch
	const pxr::TfToken DivergingTrack2End;
	/// \brief "incomingTrack1"
	///
	/// Property identifier for UsdTSDoubleSlipSwitch, UsdTSSingleSlipSwitch
	const pxr::TfToken IncomingTrack1;
	/// \brief "incomingTrack1EndType"
	///
	/// Property identifier for UsdTSDoubleSlipSwitch, UsdTSSingleSlipSwitch
	const pxr::TfToken IncomingTrack1End;
	/// \brief "incomingTrack2"
	///
	/// Property identifier for UsdTSDoubleSlipSwitch, UsdTSSingleSlipSwitch
	const pxr::TfToken IncomingTrack2;
	/// \brief "incomingTrack2EndType"
	///
	/// Property identifier for UsdTSDoubleSlipSwitch, UsdTSSingleSlipSwitch
	const pxr::TfToken IncomingTrack2End;
	/// \brief "outgoingTrack1"
	///
	/// Property identifier for UsdTSDoubleSlipSwitch, UsdTSSingleSlipSwitch
	const pxr::TfToken OutgoingTrack1;
	/// \brief "outgoingTrack1EndType"
	///
	/// Property identifier for UsdTSDoubleSlipSwitch, UsdTSSingleSlipSwitch
	const pxr::TfToken OutgoingTrack1End;
	/// \brief "outgoingTrack2"
	///
	/// Property identifier for UsdTSDoubleSlipSwitch, UsdTSSingleSlipSwitch
	const pxr::TfToken OutgoingTrack2;
	/// \brief "outgoingTrack2EndType"
	///
	/// Property identifier for UsdTSDoubleSlipSwitch, UsdTSSingleSlipSwitch
	const pxr::TfToken OutgoingTrack2End;
	/// \brief "track1Diverge"
	///
	/// Property identifier for UsdTSDoubleSlipSwitch, UsdTSSingleSlipSwitch
	const pxr::TfToken Track1Diverge;
	/// \brief "track1DivergeIncomingEndType"
	///
	/// Property identifier for UsdTSDoubleSlipSwitch, UsdTSSingleSlipSwitch
	const pxr::TfToken Track1DivergeEnd;
	/// \brief "track1Straight"
	///
	/// Property identifier for UsdTSDoubleSlipSwitch
	const pxr::TfToken Track1Straight;
	/// \brief "track1StraightIncomingEndType"
	///
	/// Property identifier for UsdTSDoubleSlipSwitch
	const pxr::TfToken Track1StraightEnd;
	/// \brief "track2Diverge"
	///
	/// Property identifier for UsdTSDoubleSlipSwitch
	const pxr::TfToken Track2Diverge;
	/// \brief "track2DivergeIncomingEndType"
	///
	/// Property identifier for UsdTSDoubleSlipSwitch
	const pxr::TfToken Track2DivergeEnd;
	/// \brief "track2Straight"	
	///
	/// Property identifier for UsdTSDoubleSlipSwitch
	const pxr::TfToken Track2Straight;
	/// \brief "track2StraightIncomingEndType"
	///
	/// Property identifier for UsdTSDoubleSlipSwitch
	const pxr::TfToken Track2StraightEnd;
	/// \brief "go"
	///
	/// Value identifier for UsdTS2WaySwitch, UsdTS3WaySwitch, UsdTSSingleSlipSwitch, UsdTSDoubleSlipSwitch
	const pxr::TfToken Go;
	/// \brief "branch"
	///
	/// Value identifier for UsdTS2WaySwitch, UsdTS3WaySwitch, UsdTSSingleSlipSwitch, UsdTSDoubleSlipSwitch
	const pxr::TfToken Branch;

	//const pxr::TfToken Go1;
	//const pxr::TfToken Go2;
	//const pxr::TfToken Branch1;
	//const pxr::TfToken Branch2;


    //These are defined with pxr::UsdGeomTokens, but only in
	//the original Pixar SDK, not with Omniverse...
	/// \private
    const pxr::TfToken Scope;
	/// \private
    const pxr::TfToken BasisCurves;
};

/// \var UsdTraxTokens
///
/// A global variable with static, efficient TfTokens
/// for use in all public USD API.  \sa UsdGeomTokensType
extern dclspc pxr::TfStaticData<UsdTraxTokenTypes> UsdTraxTokens;

}

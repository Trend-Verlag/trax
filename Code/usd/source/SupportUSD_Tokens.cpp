// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#include "usd/SupportUSD_Tokens.h"

namespace trax{

UsdTraxTokenTypes::UsdTraxTokenTypes()
	: Interval					{ "TSInterval" }
	, Near						{ "near" }
	, Far						{ "far" }
	, IntervalNear				{ "intervalNear" }
	, IntervalFar				{ "intervalFar" }
	, Position					{ "TSPosition" }
	, X							{ "x" }
	, Y							{ "y" }
	, Z							{ "z" }
	, Vector					{ "TSVector" }
	, Distance					{ "TSDistance" }
	, Attractor					{ "TSAttractor" }
	, Dx						{ "dx" }
	, Dy						{ "dy" }
	, Dz						{ "dz" }
	, Tangent					{ "TSTangent" }
	, Normal					{ "TSNormal" }
	, Binormal					{ "TSBinormal" }
	, VectorBundle				{ "TSVectorBundle" }
	, VectorBundle2				{ "TSVectorBundle2" }
	, Frame						{ "TSFrame" }
	, TrackSystem				{ "TSTrackSystem" }
	, TrackCollection			{ "TSTrackCollection" }
	, Track						{ "TSTrack" }
	, TrackBody					{ "trackBody" }
	, FrontConnection			{ "frontConnection" }
	, FrontConnectionEnd		{ "frontConnectionEndType" }
	, EndConnection				{ "endConnection" }
	, EndConnectionEnd			{ "endConnectionEndType" }
	, FrontEnd					{ "front" }
	, EndEnd					{ "end" }
	, CurveSample				{ "TSCurveSample" }
	, CubicData					{ "TSCubicData" }
	, Line						{ "TSLine" }
	, LineP						{ "TSLineP" }
	, Arc						{ "TSArc" }
	, ArcP						{ "TSArcP" }
	, Helix						{ "TSHelix" }
	, HelixP					{ "TSHelixP" }
	, Cubic						{ "TSCubic" }
	, Spline					{ "TSSpline" }
	, Clothoid					{ "TSClothoid" }
	, Rotator					{ "TSRotator" }
	, RotatorChain				{ "TSRotatorChain" }
	, PolygonalChain			{ "TSPolygonalChain" }
	, SampledCurve				{ "TSSampledCurve" }
	, Curve						{ "curve" }
	, Curvature					{ "curvature" }
	, Torsion					{ "torsion" }
	, Length					{ "length" }
	, A							{ "a" }
	, B							{ "b" }
	, C							{ "c" }
	, D							{ "d" }
	, A0						{ "a0" }
	, B0						{ "b0" }
	, ConstantTwist				{ "TSConstantTwist" }
	, LinearTwist				{ "TSLinearTwist" }
	, PiecewiseTwist			{ "TSPiecewiseTwist" }
	, DirectionalTwist			{ "TSDirectionalTwist" }
	, PiecewiseDirectionalTwist	{ "TSPiecewiseDirectionalTwist" }
	, CombinedTwist				{ "TSCombinedTwist" }
	, Section					{ "TSSection" }
	, Type						{ "type" }
	, Id						{ "id" }
	, From						{ "from" }
	, To						{ "to" }
	, Pair						{ "TSPair" }
	, Tuple						{ "TSTuple" }
	, ConnectorCollection		{ "TSConnectorCollection" }
	, TwoWaySwitch				{ "TS2WaySwitch" }
	, ThreeWaySwitch			{ "TS3WaySwitch" }
	, SingleSlipSwitch			{ "TS1SlipSwitch" }
	, DoubleSlipSwitch			{ "TS2SlipSwitch" }
	, SwitchState				{ "state" }
	, NarrowTrack				{ "narrowTrack" }
	, NarrowTrackEnd			{ "narrowTrackEndType" }
	, StraightTrack				{ "straightTrack" }
	, StraightTrackEnd			{ "straightTrackEndType" }
	, DivergingTrack			{ "divergingTrack" }
	, DivergingTrackEnd			{ "divergingTrackEndType" }
	, DivergingTrack1			{ "divergingTrack1" }
	, DivergingTrack1End		{ "divergingTrack1EndType" }
	, DivergingTrack2			{ "divergingTrack2" }
	, DivergingTrack2End		{ "divergingTrack2EndType" }
	, IncomingTrack1			{ "incomingTrack1" }
	, IncomingTrack1End			{ "incomingTrack1EndType" }
	, IncomingTrack2			{ "incomingTrack2" }
	, IncomingTrack2End			{ "incomingTrack2EndType" }
	, OutgoingTrack1			{ "outgoingTrack1" }
	, OutgoingTrack1End			{ "outgoingTrack1EndType" }
	, OutgoingTrack2			{ "outgoingTrack2" }
	, OutgoingTrack2End			{ "outgoingTrack2EndType" }
	, Track1Diverge				{ "track1Diverge" }
	, Track1DivergeEnd			{ "track1DivergeIncomingEndType" }
	, Track1Straight			{ "track1Straight" }
	, Track1StraightEnd			{ "track1StraightIncomingEndType" }
	, Track2Diverge				{ "track2Diverge" }
	, Track2DivergeEnd			{ "track2DivergeIncomingEndType" }
	, Track2Straight			{ "track2Straight" }
	, Track2StraightEnd			{ "track2StraightIncomingEndType" }
	, Go						{ "go" }	
	, Branch					{ "branch" }
	//, Go1						{ "go1" }
	//, Go2						{ "go2" }
	//, Branch1					{ "branch1" }
	//, Branch2					{ "branch2" }


	, Scope						{ "Scope" }
    , BasisCurves				{ "BasisCurves" }
{
}

pxr::TfStaticData<UsdTraxTokenTypes> UsdTraxTokens;

}

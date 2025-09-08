//	trax track library
//	AD 2018 
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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

namespace trax{

	struct Arc;
	struct ArcP;
	struct Axis;
	struct BinaryIndicator;
	struct Body;
	struct Clothoid;
	struct CombinedTwist;
	struct Connector;
	struct ConstantTwist;
	struct Cubic;
	struct Curve;
	struct CurveSample;
	struct DirectionalTwist;
	struct DoubleSlipSwitch;
	struct EEPCurve;
	struct Event;
	struct GeomTrack;
	struct Helix;
	struct HelixP;
	struct Indicator;
	struct IntervalSensor;
	struct Jack;
	struct JackEnumerator;
	struct JumpSite;
	struct Line;
	struct LineP;
	struct LinearTwist;
	class  Location;
	struct MovableTrack;
	struct MultiPlug;
	struct NarrowSwitch;
	struct ParallelizableTrack;
	struct PiecewiseTwist;
	struct Plug;
	struct PlugEnumerator;
	struct PolygonalChain;
	struct PulseCounter;
	struct RoadwayTwist;
	struct Rotator;
	struct RotatorChain;
	struct SampledCurve;
	struct Section;
	struct Sensor;
	struct SensorFilterJack;
	struct Simulator;
	struct SingleSlipSwitch;
	struct SocketRegistry;
	struct Spline;
	struct Switch;
	struct Timer;
	struct ThreeWaySwitch;
	struct Track;
	struct TrackBuilder;
	template<typename> 
	struct TrackData;
	template<typename> 
	class  TrackJoint;
	class  TrackJointFeeder;
	class  TrackJointFeederMotorModel;
	struct TrackJointLimits;
	struct TrackLocation;
	struct TrackLocationRef;
	struct TrackRange;
	struct TractionForceCharacteristic;
	struct VelocityControl;
	struct VelocitySensor;
	struct WeighSensor;
	struct Wheelset;
}
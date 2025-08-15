//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2018 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

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
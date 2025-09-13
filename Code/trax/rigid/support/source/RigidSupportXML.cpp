//	trax track library
//	AD 2014 
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

#include "../RigidSupportXML.h"

#include "dim/support/DimSupportXML.h"
#include "spat/support/SpatSupportXML.h"

#include "trax/rigid/TractionForceCharacteristic.h"
#include "trax/rigid/TrackJointLimits.h"
#include "trax/rigid/Material.h"
#include "trax/rigid/Wheelset.h"

namespace trax{
	using namespace dim;
	using namespace dim::ptreesupport;
	using namespace spat;
	using namespace spat::ptreesupport;

namespace ptreesupport{

void ReadMaterial( const boost::property_tree::ptree& pt, Material& material ) noexcept{
	material.type = MaterialType( pt.get( "<xmlattr>.type", "none" ) );
	material.staticFriction = pt.get( "<xmlattr>.staticFriction", 0.6f );
	material.dynamicFriction= pt.get( "<xmlattr>.dynamicFriction", 0.4f );
	material.restitution	= pt.get( "<xmlattr>.restitution", 0.1f );
}

void ReadTrackJointLimits( const boost::property_tree::ptree& pt, TrackJointLimits& wfl ) noexcept{
	const TrackJointLimits def;

	wfl.NormalForceLimitMin		= get( pt, "<xmlattr>.NormalForceLimitMin", def.NormalForceLimitMin, _kN );
	wfl.NormalForceLimitMax		= get( pt, "<xmlattr>.NormalForceLimitMax", def.NormalForceLimitMax, _kN );
	wfl.BinormalForceLimitMin	= get( pt, "<xmlattr>.BinormalForceLimitMin", def.BinormalForceLimitMin, _kN );
	wfl.BinormalForceLimitMax	= get( pt, "<xmlattr>.BinormalForceLimitMax", def.BinormalForceLimitMax, _kN );
	wfl.NormalTorqueLimit		= get( pt, "<xmlattr>.NormalTorqueLimit", def.NormalTorqueLimit, _kNm );
	wfl.TorqueLimit				= get( pt, "<xmlattr>.TorqueLimit", def.TorqueLimit, _kNm );
	wfl.ThresholdPosition		= get( pt, "<xmlattr>.ThresholdPosition", def.ThresholdPosition, _m );
	wfl.ThresholdTangent		= pt.get( "<xmlattr>.ThresholdTangent", def.ThresholdTangent );
	wfl.ThresholdNormal			= pt.get( "<xmlattr>.ThresholdNormal", def.ThresholdNormal );
	wfl.ThresholdBinormal		= pt.get( "<xmlattr>.ThresholdBinormal", def.ThresholdBinormal );
	wfl.bTiltingEnabled			= pt.get( "<xmlattr>.bTiltingEnabled", def.bTiltingEnabled );
}

void ReadWheelset( const boost::property_tree::ptree& pt, Wheelset& wheelset ) noexcept{
	const Wheelset def;
	wheelset.Radius				= get( pt, "<xmlattr>.radius", def.Radius, _m );
	wheelset.Gauge				= get( pt, "<xmlattr>.gauge", def.Gauge, _m );
	wheelset.Flange				= get( pt, "<xmlattr>.flange", def.Flange, _m );
	wheelset.WheelWidth			= get( pt, "<xmlattr>.wheel_width", def.WheelWidth, _m );
	wheelset.AxleRadius			= get( pt, "<xmlattr>.axle_radius", def.AxleRadius, _m );
	wheelset.TotalMass			= get( pt, "<xmlattr>.mass", def.TotalMass, _t );
	wheelset.Rotation			= pt.get( "<xmlattr>.rotation", def.Rotation );
	wheelset.Shift				= get( pt, "<xmlattr>.shift", def.Shift, _m );
	wheelset.MaxMotorTorque		= get( pt, "<xmlattr>.motor_torque", def.MaxMotorTorque, _kNm );
	wheelset.MaxBrakingTorque	= get( pt, "<xmlattr>.brake_torque", def.MaxBrakingTorque, _kNm );
	wheelset.FrictionTorque		= get( pt, "<xmlattr>.friction_torque", def.FrictionTorque, _kNm );

	std::for_each( pt.begin(), pt.end(),
		[&wheelset]( const boost::property_tree::ptree::value_type& pair )
	{
		if( pair.first == "Position2D" )
			ReadPosition2D( pair.second, wheelset.AxlePosition );
	} );
}

void ReadTractionForceCharacteristic( const boost::property_tree::ptree& pt, TractionForceCharacteristic& TFC ) noexcept
{
	for( const auto& pair : pt )
	{
		if( pair.first == "SpeedStep" )
			TFC.AddSpeedStep( get(  pair.second, "<xmlattr>.velocity", 0_mIs, _mIs ),
									pair.second.get( "<xmlattr>.value", 1.0f ) );
	};
}

} // namespace ptreesupport
} // namespace trax
//  _____                                    _                               _                _       _     _                                _  _  
// |_   _|    _ _   __ _    __ __           | |_      _ _   __ _     __     | |__            | |     (_)   | |__      _ _   __ _      _ _   | || | 
//   | |     | '_| / _` |   \ \ /    ___    |  _|    | '_| / _` |   / _|    | / /    ___     | |     | |   | '_ \    | '_| / _` |    | '_|   \_, | 
//  _|_|_   _|_|_  \__,_|   /_\_\   |___|   _\__|   _|_|_  \__,_|   \__|_   |_\_\   |___|   _|_|_   _|_|_  |_.__/   _|_|_  \__,_|   _|_|_   _|__/  
//_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_|"""""|_| """"| 
//"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-'"`-0-0-' 
//################################################################################################################################################
// 
//	trax track library
//	AD 2013 
//
//  "we are all the voices under the tracks, 
//	 drawing you forward, pushing you back."
//
//								Funeral Party
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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

/// \page main_page Overview
/// 
/// - \subpage docu_dim
/// - \subpage docu_spat
/// - \subpage docu_units
/// - \subpage docu_curve
/// - \subpage docu_twist
/// - \subpage docu_track
/// - \subpage docu_connector
/// - \subpage docu_location
/// - \subpage docu_sensor
/// - \subpage docu_signal
/// - \subpage docu_jacksnplugs
/// - \subpage docu_gestalt
/// - \subpage docu_joints
/// - \subpage docu_simulation
/// - \subpage docu_railrunners

/// \brief Namespace of all the trax track libraries classes and methods.
namespace trax{
}

#include "Configuration.h"
#include "Connector.h"
#include "ConnectorType.h"
#include "Curve.h"
#include "Curve_Imp.h"
#include "Event.h"
#include "Identified.h"
#include "IDType.h"
#include "ImplementationHelper.h"
#include "Indicator.h"
#include "Interfaces.h"
#include "Jack.h"
#include "Location.h"
#include "LogicElements.h"
#include "Numerics.h"
#include "ObjectID.h"
#include "Orientation.h"
#include "ParallelTrack.h"
#include "Parser.h"
#include "Plug.h"
#include "RoadwayTwist.h"
#include "Section.h"
#include "SectionTrack.h"
#include "Sensor.h"
#include "Signal.h"
#include "Simulated.h"
#include "SocketRegistry.h"
#include "Switch.h"
#include "Timer.h"
#include "Track.h"
#include "TrackData.h"
#include "TrackLocation.h"
#include "TrackPainter.h"
#include "Units.h"
#include "UnitsHelper.h"
#include "Version.h"

#include "spat/Spatial.h"

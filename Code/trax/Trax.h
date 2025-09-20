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


/// \mainpage Overview
/// <p>The trax library implements the logic of track guided vehicles together
/// with a signaling and sensor system as well as cargo transportation. It makes use
/// of the Nvidia PhysX engine to simulate the dynamics of trains moving along tracks 
/// and beyond.</p>
/// 
/// 
/// <p>The library was used to build the railroad simulators 
/// <a href="https://eepshopping.de/eep_train_simulator_mission_v1_122|7683.html">EEP Train Simulator Mission</a> 
/// and <a href="https://eepshopping.de/eep_tsm_gotthardbahn_nordrampe_modul_erstfeld|8235.html"> EEP TSM Gotthardbahn Nordrampe Modul Erstfeld</a>. 
/// It also is part of the <a href="https://eepshopping.de/eep_eisenbahn_exe_professional_18_download_version|11927.html">Eisenbahn.exe Professional (EEP)</a> product line.
/// There is a <a href="../EisenbahnNow.pdf">whitepaper</a> that describes the basic ideas behind the library. 
/// The mathematical foundations for the railroad dynamics are described <a href="../TrackConstraint.pdf">here</a>.</p>
///
///
/// \section gettingStarted_tools 1. Tools
///	
/// Install these tools prior to following the installation instructions for 
/// the project.
/// 
/// <ul>
///		<li><b><a href="https://git-scm.com/">Git</a></b>: A version control system. Needed to get the source code from the repository.</li>
///		<li>Register a <b><a href="https://github.com/">GitHub Account</a></b>: Here the 'Trend Trax Library' resides.</li>
///		<li><b><a href="http://visualstudio.com/">Visual Studio 2022 Community Edition</a></b>: Download and install the community edition. We will
///			only need the 'workload' called Desktop Development with C++.
///		</li>
///     <li><b><a href="https://cmake.org/">CMake</a></b>, a project configuration tool, will also be used by the PhysX SDK.</li>
///     <li><b><a href="http://www.stack.nl/~dimitri/doxygen/">Doxygen</a></b>: A documentation generator.</li>
///     <li><b><a href="https://www.python.org/">Python</a></b> will be used by the PhysX SDK.</li>
/// </ul>
///
/// \section gettingStarted_dependencies 2. Dependencies
///
/// Currently there are two dependencies of the library:
/// </p>
/// <ul>
///		<li>
///			<a href="https://www.boost.org">Boost</a>, referenced in the project by the environment variable BOOST_ROOT.<br />
///			From that we currently need the following libraries:
///			<ul>
///				<li>boost/property_tree</li>
///				<li>boost/algorithm</li>
///				<li>boost/test</li>
///			</ul>
///				<br />From these only the last one needs to get built. After downloading and unpacking the library, copy the batch file:
///				.\\Tools\\BuildBoost.bat
///            <br />to the boost root and execute it. (Alternatively execute the commands within accordingly.)
///			</li>
///			<li>
///				<a href="https://developer.nvidia.com/gameworks-physx-overview">PhysX</a>, referenced by the environment variable PHYSX_ROOT.<br />
///				Get the newest version. It provides a batch file that lets you create the project files for VS; we need a '64 bit CPU only' solution.
///				Also the PhysX library has to link with the 'Multi-threaded DLL' runtime libary. That can be done, by editing the respective xml 
///				data files in the folder: .\physx\buildtools\presets\public. So PX_GENERATE_STATIC_LIBRARIES should be set to 'False':
///				\code
///					<cmakeSwitch name="PX_GENERATE_STATIC_LIBRARIES" value="False" comment="Generate static libraries" />
///				\endcode
///            <br />Open the generated soulution and build all the targets ('Release', 'Debug', 'Checked', 'Profile') for the 'INSTALL' project.
///        </li>
///    </ul>
///
/// \section gettingStarted_installation 3. Installation
///
/// <p>The first step would be to install the tools from the list (see above); 
/// download, build and install the dependencies and then get the latest version of 
/// trax from GitHub: with a valid account use git:</p>
/// 
/// \code
/// git clone https://github.com/Trend-Verlag/trax.git Trax2
/// \endcode
/// 
/// \section gettingStarted_build 4. Build
///
/// <p>There is a batch file (Install.bat) in the root folder of trax, that does 
/// necessary installations like setting environment variables, generating project 
/// files with CMake, build Debug version as a compile test and generate this 
/// documentation:</p>
/// 
/// \code
/// cd ./Trax2
/// ./Install.bat
/// \endcode
/// 
/// <p>Open the solution in the ./_Build folder, select 'Release' as a configuration and 
/// and build and run the 'ALL_TESTS' project.</p>
/// 

#pragma once

#include <memory>


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

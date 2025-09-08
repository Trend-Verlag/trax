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
/// <p><b>The trax library implements</b> the logic of track guided vehicles together
/// with a signaling and sensor system as well as cargo transportation. It makes use
/// of the Nvidia PhysX engine to simulate the dynamics of trains moving along tracks 
/// and beyond.</p>
/// 
/// <p>The library was used to build the railroad simulators <a href="http://train-simulator.trendverlag.com/">EEP Train Simulator Mission</a> and 
/// <a href="https://store.steampowered.com/app/722200">EEP TSM Gotthardbahn Nordrampe Modul Erstfeld</a>. There is a 
/// <a href="../../EisenbahnNow.pdf">whitepaper</a> that describes the basic ideas behind the library. The mathematical foundations for the railroad 
/// dynamics are described <a href="../../TrackConstraint.pdf">here</a>.</p>
///
///
/// \section gettingStarted_tools 1. Tools
///	
/// <b>Install these tools</b> prior to following the installation instructions for the project.
/// <ul>
///		<li><b><a href="http://visualstudio.com/">Visual Studio 2017 Community Edition</a></b>: Download and install the community edition. We will
///			only need the 'workload' called Desktop Development with C++.
///		</li>
///     <li><b><a href="https://www.visualsvn.com/">VisualSVN</a> and TortoiseSVN</b>: Plugin for VS 2017 that provides easy to use access to Subversion. 
///			Install the client.
///		</li>
///     <li><b>Subversion (SVN)</b>: A version control system. This software is installed by TortoiseSVN.</li>
///     <li><b><a href="http://www.stack.nl/~dimitri/doxygen/">Doxygen</a></b>: A documentation generator.</li>
///     <li><b><a href="https://cmake.org/">CMake</a></b>, a project configuration tool, will be used by the PhysX SDK.</li>
///     <li><b><a href="https://www.python.org/">Python</a></b> will be used by the PhysX SDK.</li>
/// </ul>
///
/// \section gettingStarted_installation 2. Installation
///
/// <p><b>The first step</b> would be to install the tools from the list (see above). Then get the latest version of trax from subversion: with a valid 
/// account use Subversion (svn co --username YourName https://82.165.38.60/repos/eeptsm/trunk/Trax) or use VisualSVN.</p>
///
///	From here there are two ways to get the library SDKs. A compareably easy way, using my 'BinaryPackage', downloadable from my folder ('marc')  
/// on the SoftPro Server or a hard one by getting these libraries from their respective projects.</p>
///
///	<p>
///		<b>As a minimum</b> you need to get a copy of the <a href="https://www.boost.org">Boost</a> library. The other libraries are only
///		needed if you actually want to make use of them.
///	</p>
///
///	<h3>2.1. The easy way: Binary Package</h3>
///
///	<p>
///		<b>The BinaryPackage contains</b> the external libraries currently used. It can be found on the SoftPro-Server under marc/BinaryPackage/LibrariesExternal.rar.
///		All this stuff would have to get extracted somewhere (e.g some folder named 'C:\\Trend\\LibrariesExternal'). To set the environment variables run the
///		SetEnvironmentVariables.bat batch file from the package root.
///	</p>
///
///	<h3>2.2. The hard way: Getting the full libraries</h3>
///
/// <p>
/// <b>There is no guarantee for different C++ compilers</b> to produce the same binary output or even for
/// dynamic or static libraries to be usable together when compiled with different compilers. This
/// in general not even holds for different versions of the same compiler (VS is binary compatible since VS 2015). 
/// For this reason the library vendors encourage their users to compile the binaries of their libraries on their 
/// own compilers.
/// </p>
///	<p>
/// Currently there are three dependencies of the library:
/// </p>
/// <ul>
///		<li>
///			<a href="https://www.boost.org">Boost</a>, referenced in the project by the environment variable BOOST_ROOT.<br />
///			From that we currently need the following libraries:
///		<ul>
///			<li>boost/property_tree</li>
///			<li>boost/algorithm</li>
///			<li>boost/test</li>
///			</ul>
///				<br />From these only the last one needs to get built. After downloading and unpacking the library, copy the batch file:
///				.\\Tools\\BuildBoost.bat
///            <br />to the boost root and execute it. (Alternatively execute the commands within per hand.)
///			</li>
///			<li>
///				<a href="https://developer.nvidia.com/gameworks-physx-overview">PhysX</a>, referenced by the environment variable PHYSX_ROOT.<br />
///				Get version 4.0. It provides a batch file that lets you create the project files for VS; let it create the 32 bit and 64 bit
///				solutions. We need the PhysX library being compiled with the 'Multi-threaded DLL' runtime libary, so open the solutions and
///				in the solution explorer mark all the projects under the folder 'PhysX SDK', rightclick and open the properties dialog for them.
///				Under 'Configuration Properties->C/C++->Code Generation->Runtime Library' switch to 'Multi-threaded DLL (/MD)', respectively
///            'Multi-threaded Debug DLL (/MD)' for all the project configurations (debug, profile, checked and release). From the menue
///				select 'Build->Batch Build' and select the following projects in all their configurations:
///				<ul>
///                <li>PhysX</li>
///                <li>PhysXCommon</li>
///                <li>PhysXCooking</li>
///                <li>PhysXExtensions</li>
///                <li>PhysXFoundation</li>
///                <li>PhysXPvdSDK</li>
///            </ul>
///            <br />Hit 'Build'. Do this for the 32 and 64 bit solutions.
///        </li>
///        <li>
///            <a href="http://ode.org/">Open Dynamics Engine (ODE)</a>, referenced by the environment variable ODE_ROOT.<br />
///            There is a link to the sourcecode on <a href="https://bitbucket.org/odedevs/ode/downloads/">bitbucket</a> with a download section,
///            containing stable releases. Get ode-0.13 (actually you need a modified version ode-0.13Modified.rar, see SoftPro Server). Follow the
///            installation instructions by creating a project for the newest VS version. This only is needed to get the Config.h file, the project itself comes with the trax solution.
///            It might be necessary to trigger a reload of the file locations if the environment variable ODE_ROOT changes, by forcing a change to the ODE.vcxproj.filters
///            and ODE.vcxproj.
///        </li>
///    </ul>
///
/// \section gettingStarted_build 3. Build
///
/// <p>There is a batch file (Installation.cmd) in the root folder of trax that would do necessary installations like generating this documentation 
/// and setting environment variables: update the pathes to the libraries and run it.</p>
///
/// <p>Open the solution in the Trax/Projects folder, go to Menue->Build->Batch Build ..., select all projects and hit 'Build'.</p>
///	
/// <p>Run the tests.</p>
///
#pragma once

#include <memory>


/// \brief Namespace of all the trax track libraries classes and methods.
namespace trax{
}

#include "Body.h"
#include "Configuration.h"
#include "Connector.h"
#include "ConnectorType.h"
#include "Curve.h"
#include "Curve_Imp.h"
#include "Event.h"
#include "GeomTrack.h"
#include "Identified.h"
#include "IDType.h"
#include "ImplementationHelper.h"
#include "Indicator.h"
#include "Interfaces.h"
#include "Jack.h"
#include "Location.h"
#include "LogicElements.h"
#include "Material.h"
#include "Numerics.h"
#include "ObjectID.h"
#include "Orientation.h"
#include "ParallelTrack.h"
#include "Parser.h"
#include "Plug.h"
#include "RoadwayTwist.h"
#include "Section.h"
#include "Sensor.h"
#include "Signal.h"
#include "Simulated.h"
#include "SocketRegistry.h"
#include "Switch.h"
#include "Timer.h"
#include "Track.h"
#include "TrackData.h"
#include "TrackJoint.h"
#include "TrackJointFeeder.h"
#include "TrackJointLimits.h"
#include "TrackLocation.h"
#include "TractionForceCharacteristic.h"
#include "Units.h"
#include "UnitsHelper.h"
#include "Version.h"
#include "Wheelset.h"

#include "spat/Spatial.h"





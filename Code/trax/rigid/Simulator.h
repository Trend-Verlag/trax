//	trax track library
//	AD 2023 
//
//  "A monkey needs dance, so do you"
//
//							Johnossi
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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

/// \page docu_simulation Simulation
/// 
/// \section simulation_intro Introduction
/// A physics \link trax::Simulator Simulator \endlink is the main component
/// of a physics simulation. It manages the setup and the shutdown of the 
/// physical simulator (in our case for now: NVIDIA's PhysX). A \link trax::Scene
/// Scene \endlink creates all the physical objects in one simulation. Several
/// Scenes can be simulated by one Simulator, independent of each other.
/// 
/// \section simulation_loop The Simulation Loop
/// The simulation loop is the main loop of a physics simulation. It updates 
/// the physics world by advancing the simulation time, handling collisions, 
/// and applying forces. The loop typically runs at a fixed time step to 
/// ensure consistent simulation results. The Scene object provides the 
/// methods to either step the simulation by a fixed time step or running the
/// simulation on its own.
/// 

#include "trax/Configuration.h"
#include "trax/Units.h"

#include "spat/Frame.h"
#include "spat/Box.h"

#include <memory>

namespace trax{

	/// \brief Interface for the physics simulator.
	struct Simulator{

		enum class Type{
			none = 0,
			Standard,
			PhysX,
			PhysX_VisualDebugger,
			Unknown,
		};

		/// \brief Makes a Simulator object.
		static dclspc std::unique_ptr<Simulator> Make( Type type ) noexcept;


		/// \returns the name for the object type that implements this interface. 
		virtual const char*	TypeName() const noexcept = 0;


		/// \returns a human readable string for the Simulator
		/// version and the version of the underlying physics 
		/// engine.
		/// \see Version
		virtual const char* GetVersionString() const = 0;


		virtual ~Simulator() = default;
	};
}
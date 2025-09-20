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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/Configuration.h"
#include "trax/Units.h"

#include "spat/Frame.h"
#include "spat/Box.h"

#include <memory>

namespace trax{

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
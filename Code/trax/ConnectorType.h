//	trax track library
//	AD 2024 
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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "Configuration.h"

#include <string>

namespace trax{

	enum class ConnectorType{
		unknown = -1,
		none = 0,

		NarrowSwitch,
		TwoWaySwitch,		///<
		ThreeWaySwitch,
		SingleSlipSwitch,
		DoubleSlipSwitch,

		count
	};

	/// \brief Gets the type name of a curve from its type enumerator.
	dclspc std::string ToString( ConnectorType ct );


	/// \brief Gets the type enumerator of a curve from its type name.
	dclspc ConnectorType ToConnectorType( const std::string& name ) noexcept;

} // namespace trax

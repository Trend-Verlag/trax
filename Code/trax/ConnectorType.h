// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


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

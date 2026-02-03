//	trax track library
//	AD 2026 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
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

#include "Configuration.h"

namespace trax{

	/// \brief Type of ends e.g. of a Track or a RailRunner.
	enum class EndType : char{
		none = 0,
		north,		///< Denotes the front end.
		south,		///< Denotes the back end.
		any,		///< Denotes the north or south end.
		both		///< Denotes both ends.
	};


	/// \brief Gets the opposite end.
	EndType operator!( EndType end ) noexcept;

	/// \brief Tests whether the end is a concrete end (north or south).
	bool IsConcreteEnd( EndType end ) noexcept;

	/// \brief Makes a string from the end.
	dclspc std::string ToString( EndType end );


	/// \brief Makes an end value from a string.
	/// \throws std::invalid_argument if the string was not recognized.
	dclspc EndType ToEndType( const std::string& endtype );


// inlines:
	inline EndType operator!( EndType end ) noexcept{
		switch( end ){
			case EndType::north: return EndType::south;
			case EndType::south: return EndType::north;
			default:							
				return EndType::none;
		}
	}

	inline bool IsConcreteEnd( EndType end ) noexcept
	{
		return end == EndType::north || end == EndType::south;
	}

	/// \defgroup Group_RailRunnerEndHelpers RailRunner's End Helpers
	/// \brief Helper functions to get the front and back ends of an oriented element.

	/// \name End Helpers
	/// @{
	


	///@}



}

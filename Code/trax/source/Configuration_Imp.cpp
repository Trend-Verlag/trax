//	trax track library
//	AD 2025
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

#include "trax/Configuration.h"

namespace trax{

std::ostream cnull{ nullptr };

static Verbosity g_ReportVerbosity = Verbosity::normal;

std::string ToString( Verbosity type )
{
    switch( type )
	{
		case Verbosity::unknown:
			return "unknown";
		case Verbosity::silent:
			return "silent";
		case Verbosity::error:
			return "error";
		case Verbosity::normal:
			return "normal";
		case Verbosity::detailed:
			return "detailed";
		case Verbosity::verbose:
			return "verbose";
	}

	throw std::logic_error{ "Invalid Verbosity type." };
}

Verbosity ToVerbosity( const std::string& type ) noexcept
{
	if( type == "unknown" )
		return Verbosity::unknown;
	if( type == "silent" )
		return Verbosity::silent;
	if( type == "error" )
		return Verbosity::error;
	if( type == "normal" )
		return Verbosity::normal;
	if( type == "detailed" )
		return Verbosity::detailed;
	if( type == "verbose" )
		return Verbosity::verbose;

    return Verbosity::unknown;
}

void SetReportVerbosity( Verbosity verbosity ) noexcept {
    g_ReportVerbosity = verbosity;
}
Verbosity GetReportVerbosity() noexcept {
    return g_ReportVerbosity;
}

std::ostream& operator<<( std::ostream& stream, Verbosity verbosity ){
    return (verbosity <= g_ReportVerbosity) ? stream : cnull;
}

}
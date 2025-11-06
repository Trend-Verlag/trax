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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#include "trax/Configuration.h"

namespace trax{

std::ostream cnull{ false };

static Verbosity g_ReportVerbosity = Verbosity::normal;


std::string ToString( Verbosity type )
{
    return std::string dclspc();
}

Verbosity ToVerbosity( const std::string & type )
{
    return Verbosity dclspc();
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
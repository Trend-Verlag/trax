//	trax track library
//	AD 2016
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

#include <ostream>

#if defined( _WIN32 )
#	if defined( TRAX_STATIC_LIBRARY )
#		define dclspc
#	else
#		if defined( TRAX_DLL_LIBRARY )
#			define dclspc __declspec( dllexport )
#		else
#			define dclspc __declspec( dllimport )
#		endif
#	endif
#else
#	define dclspc
#endif

namespace trax{

    /// \brief Verbosity scale of trax library messages.
    enum class Verbosity : char{
		unknown = 0,//< unknown verbosity
	    silent,		//< no messages at all
	    error,		//< errors only
	    normal,		//< important messages only
	    detailed,	//< detailed report
	    verbose		//< everything
    };

	dclspc std::string ToString( Verbosity type );

	dclspc Verbosity ToVerbosity( const std::string& type ) noexcept;

	dclspc void SetReportVerbosity( Verbosity verbosity ) noexcept;

    dclspc Verbosity GetReportVerbosity() noexcept;

	dclspc std::ostream& operator<<( std::ostream& stream, Verbosity verbosity );


	class ReportVerbosityGuard{
		Verbosity m_PreviousVerbosity;
	public:
		ReportVerbosityGuard( Verbosity verbosity ) noexcept {
			m_PreviousVerbosity = GetReportVerbosity();
			SetReportVerbosity( verbosity );
		}
		~ReportVerbosityGuard() noexcept {
			SetReportVerbosity( m_PreviousVerbosity );
		}
	};

} // namespace trax
//	trax track library
//	AD 2026 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//
// Copyright (c) 2026 Trend Redaktions- und Verlagsgesellschaft mbH
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

#if defined( _WIN32 )
#	if defined( DIM_STATIC_LIBRARY )
#		define dclspc_dim
#	else
#		if defined( DIM_DLL_LIBRARY )
#			define dclspc_dim __declspec( dllexport )
#		else
#			define dclspc_dim __declspec( dllimport )
#		endif
#	endif
#else
#	define dclspc_dim
#endif

namespace dim
{
	constexpr int DIM_VERSION_MAJOR = 3;
	constexpr int DIM_VERSION_MINOR = 0;
	constexpr int DIM_VERSION_PATCH = 0;

#if defined( DIM_SINGLE )
	typedef float Real; ///< Underlying floating point type to be used with the dim library.
#elif defined( DIM_DOUBLE )
	typedef double Real; ///< Underlying floating point type to be used with the dim library.
#elif defined( DIM_LONG_DOUBLE )
	typedef long double Real; ///< Underlying floating point type to be used with the dim library.
#else
	typedef float Real; ///< Underlying floating point type to be used with the dim library.
#endif

};

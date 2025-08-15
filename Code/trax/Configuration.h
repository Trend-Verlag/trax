// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
//
//	trax track library
//	AD 2016
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

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

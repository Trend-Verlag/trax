//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2014 
//
// "It ain't a song until you sing it."
//
//				Elvis Presley

#pragma once


#if defined( _WIN32 )
#	if defined( APPFRAME_STATIC_LIBRARY )
#		define dclspc_appframe
#	else
#		if defined( APPFRAME_DLL_LIBRARY )
#			define dclspc_appframe __declspec( dllexport )
#		else
#			define dclspc_appframe __declspec( dllimport )
#		endif
#	endif
#else
#	define dclspc_appframe
#endif



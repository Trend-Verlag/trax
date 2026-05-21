//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2020
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include <string>

namespace trax {

	/// \brief Type of the camera.
	///
	/// The type of the camera specifies its behaviour.
	enum class CameraType {
		cam_basic,
		cam_firstperson,
		cam_orbiter,
		cam_terrainconfined,
		cam_dolly,
		cam_cabin
	};

	dclspc const char* From( CameraType type ) noexcept;

	dclspc CameraType CameraTypeFrom( const char* name );

	CameraType CameraTypeFrom( const std::string& name );
	
///////////////////////////////////////	
inline CameraType CameraTypeFrom( const std::string& name ){
	return CameraTypeFrom( name.c_str() );
}

}

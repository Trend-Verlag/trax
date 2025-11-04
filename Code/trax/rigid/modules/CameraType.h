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

	const char* From( CameraType type ) noexcept;

	CameraType CameraTypeFrom( const std::string& name );

}

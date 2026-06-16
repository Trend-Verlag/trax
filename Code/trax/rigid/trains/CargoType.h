//  Copyright (c) 2021 Marc-Michael Horstmann. All Rights Reserved.
//
//	trax track library
//	AD 2020
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

namespace trax {

	/// \brief Types of transportable cargo.
	enum class CargoType {
		unknown = -1,
		none = 0,
		axle,
		container,
		barrel,
		ball
	};

}

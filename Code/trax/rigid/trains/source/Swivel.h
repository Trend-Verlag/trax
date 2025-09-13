// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2023 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "BogieJoint.h"

namespace trax{

	struct Swivel : BogieJoint{

		/// \returns The axis of rotation in local coordinates of the parent (BogieA).
		virtual spat::VectorBundle<Length,One> LocalAxisA() const noexcept = 0;

		/// \returns The axis of rotation in local coordinates of the child (BogieB).
		virtual spat::VectorBundle<Length,One> LocalAxisB() const noexcept = 0;

		/// \returns The rotational angle the child turned around the axis of rotation.
		virtual Angle GetTurnAngle() const noexcept = 0;

		/// \returns The side bending or tilt between parent and child.
		virtual Angle GetBendAngle() const noexcept = 0;

		/// \returns The pitch angle of the child relative to the parent.
		virtual Angle GetPitchAngle() const noexcept = 0;
	};

}


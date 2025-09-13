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

	struct Coupling : BogieJoint{

		virtual std::shared_ptr<Bogie_Imp> GetCoupledBogie( const Bogie_Imp& toBogie ) const noexcept = 0;

		virtual Length GetLength() const noexcept = 0;

		virtual void SetLength( Length length ) noexcept = 0;

		virtual spat::Vector<Force> GetForce() const noexcept = 0;
	};

}


// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
// 
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/collections/Collection.h"
#include "trax/LogicElements.h"

namespace trax{

	struct PulseCounterCollection : Collection<PulseCounterCollection, PulseCounter>,
									Identified<PulseCounterCollection>
	{
		/// \brief Makes a PulseCounterCollection object.
		static dclspc std::unique_ptr<PulseCounterCollection> Make() noexcept;

	};

}
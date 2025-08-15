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
#include "trax/Timer.h"

namespace trax{

	struct TimerCollection : Collection<TimerCollection,Timer>
	{
		/// \brief Makes a TimerCollection object.
		static dclspc std::unique_ptr<TimerCollection> Make() noexcept;


		virtual void Update() = 0;
	};

}

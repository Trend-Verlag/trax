//	trax track library
//	AD 2023 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
// 
// 
// Copyright (c) 2025 Trend Redaktions- und Verlagsgesellschaft mbH
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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/rigid/Scene.h"
#include "trax/collections/Collection.h"

namespace trax{

	struct Bogie;
	struct Coupling;
	struct Swivel;

	struct TrainScene : public virtual Scene{

		/// \brief Makes a TrainScene object.
		static dclspc std::unique_ptr<TrainScene> Make( const Simulator& simulator ) noexcept;


		virtual std::unique_ptr<Coupling> CreateCoupling(
			std::shared_ptr<Bogie> bogieParent, 
			const spat::Frame<Length,One>& poseParent,
			std::shared_ptr<Bogie> bogieChild,
			const spat::Frame<Length,One>& poseChild ) const noexcept = 0;

		virtual std::unique_ptr<Swivel> CreateSwivel(
			std::shared_ptr<Bogie> bogieParent, 
			const spat::Frame<Length,One>& poseParent,
			std::shared_ptr<Bogie> bogieChild,
			const spat::Frame<Length,One>& poseChild ) const noexcept = 0;
	};


	struct TrainSceneCollection : public Collection<TrainSceneCollection,TrainScene>{

		/// \brief Makes a standard TrainSceneCollection object.
		static dclspc std::unique_ptr<TrainSceneCollection> Make() noexcept;


	};
}

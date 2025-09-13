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

#include "trax/rigid/trains/source/TrainScene_Imp.h"
#include "trax/rigid/engines/PhysX/source/PhysX_Scene.h"

namespace trax
{
	class PhysX_TrainScene :	public TrainScene_Imp,
								public PhysX_Scene{
	public:
		PhysX_TrainScene() = default;
		PhysX_TrainScene( const PhysX_TrainScene& ) = delete;
		PhysX_TrainScene( PhysX_TrainScene&& ) = delete;
		PhysX_TrainScene( const PhysX_Simulator& simulator, Vector<Acceleration> G = trax::g * trax::Up );
		PhysX_TrainScene( const PhysX_Simulator& simulator, Real engine_meters_per_unit, Real engine_kilograms_per_unit, Vector<Acceleration> G = trax::g * trax::Up );
		~PhysX_TrainScene() = default;

		PhysX_TrainScene& operator=( const PhysX_TrainScene& ) = delete;
		PhysX_TrainScene& operator=( PhysX_TrainScene&& ) = delete;

		// Simulator:
		const char*	TypeName() const noexcept override{
			return "PhysXSimulatorTrain";
		}

		// SimulatorTrain:
		std::unique_ptr<Coupling> CreateCoupling(
			std::shared_ptr<Bogie> bogieParent, 
			const spat::Frame<Length,One>& poseParent,
			std::shared_ptr<Bogie> bogieChild,
			const spat::Frame<Length,One>& poseChild ) const noexcept override;

		std::unique_ptr<Swivel> CreateSwivel(
			std::shared_ptr<Bogie> bogieParent, 
			const spat::Frame<Length,One>& poseParent,
			std::shared_ptr<Bogie> bogieChild,
			const spat::Frame<Length,One>& poseChild ) const noexcept override;
	};
}


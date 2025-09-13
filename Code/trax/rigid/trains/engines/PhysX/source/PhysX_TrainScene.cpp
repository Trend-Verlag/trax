//	trax track library
//	AD 2023 
//
//  "the resolution of all the fruitless searches"
//
//		
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

#include "PhysX_TrainScene.h"

#include "trax/rigid/trains/source/Bogie_Imp.h"
#include "trax/rigid/engines/PhysX/source/PhysX_Simulator.h"
#include "PhysX_Swivel_Imp.h"
#include "PhysX_Coupling_Imp.h"

#include <iostream>

namespace trax{

std::unique_ptr<TrainScene> TrainScene::Make( const Simulator& simulator ) noexcept
{
	try{
		if( const PhysX_Simulator* pPhysX_Simulator = dynamic_cast<const PhysX_Simulator*>(&simulator); pPhysX_Simulator )
			return std::make_unique<PhysX_TrainScene>( *pPhysX_Simulator );
		else{
			std::cerr << "Scene::Make: Simulator is of wrong type!" << std::endl;
		}
	}
	catch( const std::runtime_error& e ){
		std::cerr << e.what() << std::endl;
	}
	catch( const std::bad_alloc& e ){
		std::cerr << e.what() << std::endl;
	}
	catch( ... ){
		std::cerr << "An unknown error occurred." << std::endl;
	}

	return nullptr;
}

PhysX_TrainScene::PhysX_TrainScene( 
	const PhysX_Simulator& simulator, 
	Vector<Acceleration> _G )
	: PhysX_Scene{ simulator, _G }
{
}

PhysX_TrainScene::PhysX_TrainScene( 
	const PhysX_Simulator& simulator, 
	Real engine_meters_per_unit, 
	Real engine_kilograms_per_unit, 
	Vector<Acceleration> _G )
	: PhysX_Scene{ simulator, engine_meters_per_unit, engine_kilograms_per_unit, _G }
{
}

std::unique_ptr<Coupling> PhysX_TrainScene::CreateCoupling( 
	std::shared_ptr<Bogie> bogieParent, 
	const spat::Frame<Length, One>& poseParent, 
	std::shared_ptr<Bogie> bogieChild, 
	const spat::Frame<Length, 
	One>& poseChild ) const noexcept
{
	return std::make_unique<PhysX_Coupling_Imp>( Simulator().Physics(), bogieParent.get(), poseParent, bogieChild.get(), poseChild, EngineMetersPerUnit() );
}

std::unique_ptr<Swivel> PhysX_TrainScene::CreateSwivel( 
	std::shared_ptr<Bogie> bogieParent, 
	const spat::Frame<Length, One>& poseParent, 
	std::shared_ptr<Bogie> bogieChild, 
	const spat::Frame<Length, One>& poseChild ) const noexcept
{
	return std::make_unique<PhysX_Swivel_Imp>( Simulator().Physics(), bogieParent.get(), poseParent, bogieChild.get(), poseChild, EngineMetersPerUnit() );
}

}
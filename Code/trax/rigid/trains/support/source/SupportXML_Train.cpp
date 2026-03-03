//	trax track library
//	AD 2014 
//
//  "It's all over and I'm standing pretty
//   In the dust that was a city"
//
//						Goldfinger
// 
// 
// Copyright (c) 2026 Trend Redaktions- und Verlagsgesellschaft mbH
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
// For further information, please contact: horstmann.marc@trendverlag.de


#include "trax/rigid/trains/support/SupportXML.h"

#include "trax/rigid/Gestalt.h"
#include "trax/rigid/trains/WheelFrame.h"
#include "trax/rigid/trains/RollingStock.h"
#include "trax/rigid/trains/Train.h"


namespace trax{
namespace ptreesupport{
	using namespace spat::ptreesupport;

void Read( const boost::property_tree::ptree& pt, Scene& scene, Train& train )
{
	AttributesToReferences( pt, train );

	for( const auto& pair : pt )
	{
		if( pair.first == "Train" )
		{
			if( std::shared_ptr<Train> pTrain = Train::Make(); pTrain )
			{
				Read( pair.second, scene, *pTrain );
				train.Append( EndType::south, pTrain, ToOrientation( pTrain->Reference( "orientation" ) ) );
			}
		}

		else if( pair.first == "RollingStock" )
		{
			if( std::shared_ptr<RollingStock> pRollingStock = RollingStock::Make(); pRollingStock )
			{
				Read( pair.second, scene, *pRollingStock );
				train.Append( EndType::south, pRollingStock, ToOrientation( pRollingStock->Reference( "orientation" ) ) );
			}
		}
	}
}

void Read( const boost::property_tree::ptree& pt, Scene& scene, RollingStock& rollingStock )
{
	AttributesToReferences( pt, rollingStock );
	std::vector<std::shared_ptr<Bogie>> topmostBogies;

	for( const auto& pair : pt )
	{
		if( pair.first == "Bogie" )
		{
			if( std::shared_ptr<Bogie> pChildBogie = Bogie::Make( scene ); pChildBogie )
			{
				Read( pair.second, scene, *pChildBogie );
				topmostBogies.push_back( pChildBogie );
			}
		}

		else if( pair.first == "WheelFrame" )
		{
			if( std::shared_ptr<WheelFrame> pChildWheelFrame = WheelFrame::Make( scene ); pChildWheelFrame )
			{
				Read( pair.second, scene, *pChildWheelFrame );
				topmostBogies.push_back( pChildWheelFrame );
			}
		}
	}

	if( !topmostBogies.empty() )
	{
		// connect the adjacent south child bogies to their south parents ...
		for( size_t i = 0; i < topmostBogies.size() - 1; ++i )
			topmostBogies[i]->Attach( topmostBogies[i + 1]->GetChild( EndType::north ).first, EndType::north );

		rollingStock.Attach( *topmostBogies.front() );
	}
}

void Read( const boost::property_tree::ptree& pt, Scene& scene, Bogie& bogie )
{
	AttributesToReferences( pt, bogie );

	bogie.TargetVelocity( pt.get( "<xmlattr>.target_velocity", 0_kmIh ) );
	bogie.Thrust( pt.get( "<xmlattr>.thrust", 0_1 ) );
	bogie.Brake( pt.get( "<xmlattr>.brake", 0_1 ) );
	if( pt.get( "<xmlattr>.couplingNorthActivated", false ) )
		bogie.ActivateCoupling( EndType::north );
	if( pt.get( "<xmlattr>.couplingSouthActivated", false ) )
		bogie.ActivateCoupling( EndType::south );

	std::shared_ptr<Bogie> pChildNorthBogie, pChildSouthBogie;

	for( const auto& pair : pt )
	{
		if( pair.first == "Frame" ){
			spat::Frame<Length,One> frame;
			ReadFrame( pair.second, frame );
			bogie.GetGestalt().SetFrame( frame );
		}

		else if( pair.first == "Velocity" ){
			spat::Vector<Velocity> velocity;
			ReadVector( pair.second, velocity );
			bogie.GetGestalt().SetLinearVelocity( velocity );
		}

		else if( pair.first == "AngularVelocity" ){
			spat::Vector<AngularVelocity> angularVelocity;
			ReadVector( pair.second, angularVelocity );
			bogie.GetGestalt().SetAngularVelocity( angularVelocity );
		}

		else if( pair.first == "Bogie" ){
			if( std::shared_ptr<Bogie> pChildBogie = Bogie::Make( scene ); pChildBogie )
			{
				Read( pair.second, scene, *pChildBogie );
				pChildNorthBogie ? pChildSouthBogie : pChildNorthBogie = pChildBogie;
			}
		}

		else if( pair.first == "WheelFrame" ){
			if( std::shared_ptr<WheelFrame> pChildWheelFrame = WheelFrame::Make( scene ); pChildWheelFrame )
			{
				Read( pair.second, scene, *pChildWheelFrame );
				pChildNorthBogie ? pChildSouthBogie : pChildNorthBogie = pChildWheelFrame;
			}
		}
	}

	if( pChildNorthBogie )
		bogie.Attach( pChildNorthBogie, EndType::north );

	if( pChildSouthBogie )
		bogie.Attach( pChildSouthBogie, EndType::south );
}

void Read( const boost::property_tree::ptree& pt, Scene& scene, WheelFrame& wheelFrame )
{
	Read( pt, scene, static_cast<Bogie&>(wheelFrame) );

	// ...
}

}
}

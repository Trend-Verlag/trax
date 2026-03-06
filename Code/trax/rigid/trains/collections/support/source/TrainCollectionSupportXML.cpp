//	trax track library
//	AD 2026 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
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

#include "../TrainCollectionSupportXML.h"
#include "trax/rigid/trains/Train.h"
#include "trax/rigid/trains/support/SupportXML.h"
#include "trax/rigid/trains/collections/Fleet.h"

namespace trax{

namespace ptreesupport{

static void EmplaceSubTrains( Train& train, Fleet& fromFleet )
{
	for( int index = 0; index < train.GetNumberOfComponents(); ++index )
	{
		if( std::shared_ptr<TrainComponent> pComponent = train.GetComponent( index ) )
		{
			if( std::shared_ptr<Train> pSubTrain = std::dynamic_pointer_cast<Train>( pComponent ) )
			{
				if( pSubTrain->GetNumberOfComponents() == 0 )
				{
					if( std::shared_ptr<Train> pFleetTrain = fromFleet.Get( pSubTrain->ID() ) )
					{
						train.Replace( index, *pFleetTrain );
					}
				}
			}
		}
	}
}

void Read( const boost::property_tree::ptree& pt, Scene& scene, Fleet& fleet )
{
	for( const auto& pair : pt )
	{
		if( pair.first == "Train" )
		{
			if( std::shared_ptr<Train> pTrain = Train::Make(); pTrain )
			{
				Read( pair.second, scene, *pTrain );
				fleet.Add( pTrain );
			}
		}
	}

	// Empty sub-trains with ids mean to be references to other trains 
	// in the fleet, so we need to replace them with the actual trains.
	for( Train& train : fleet )
	{
		EmplaceSubTrains( train, fleet );
	}
}

}
}


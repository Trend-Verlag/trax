// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2026 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2026
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/rigid/trains/support/Anl4RailRunnerWriter.h"

#include "trax/rigid/Gestalt.h"

#include "trax/rigid/trains/Train.h"
#include "trax/rigid/trains/RollingStock.h"
#include "trax/rigid/trains/WheelFrame.h"

#include <iostream>


namespace trax{
namespace ptreesupport{

static void WriteTrainContent( boost::property_tree::ptree& ptTrain, const Train& train )
{
	ptTrain.add( "<xmlattr>.orientation", ToString( train.GetOrientation() ) );
	
	for( int index = 0; index < train.GetNumberOfComponents(); ++index )
	{
		ptTrain << *train.GetComponent( index );
	}
}

static void WriteRollingStockContent( boost::property_tree::ptree& ptRollingStock, const RollingStock& rollingStock )
{
	ptRollingStock.add( "<xmlattr>.orientation", ToString( rollingStock.GetOrientation() ) );

	for( int index = 0; index < rollingStock.GetNumberOfTopmostParents(); ++index )
	{
		ptRollingStock << rollingStock.GetTopmostParent( index );
	}
}

static void WriteBogieContent( boost::property_tree::ptree& ptBogie, const Bogie& bogie )
{		
	ptBogie.add( "<xmlattr>.couplingNorthActivated", bogie.IsActivated( EndType::north ) );
	ptBogie.add( "<xmlattr>.couplingSouthActivated", bogie.IsActivated( EndType::south ) );
	ptBogie.add( "<xmlattr>.target_velocity", bogie.TargetVelocity() );
	ptBogie.add( "<xmlattr>.thrust", bogie.Thrust() );
	ptBogie.add( "<xmlattr>.brake", bogie.Brake() );

	spat::Frame<Length,One> BodyFrame;
	bogie.GetGestalt().GetFrame( BodyFrame );
	ptBogie << BodyFrame;
	spat::Vector<Velocity> velocity;
	bogie.GetGestalt().GetLinearVelocity( velocity );
	ptBogie << velocity;
	spat::Vector<AngularVelocity> angularVelocity;
	bogie.GetGestalt().GetAngularVelocity( angularVelocity );
	ptBogie << angularVelocity;

	if( std::shared_ptr<Bogie> pBogieNorth = bogie.GetChild( EndType::north ).first; pBogieNorth )
		ptBogie << *pBogieNorth;
	if( std::shared_ptr<Bogie> pBogieSouth = bogie.GetChild( EndType::south ).first
		; pBogieSouth && !pBogieSouth->GetParent( EndType::south ).first ) // children are written by there south parent if any.
		ptBogie << *pBogieSouth;
}

static void WriteWheelFrameContent( boost::property_tree::ptree& ptWheelFrame, const WheelFrame& wheelFrame )
{
	WriteBogieContent( ptWheelFrame, wheelFrame );
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const RailRunner& railRunner ){
	boost::property_tree::ptree ptRailRunner;
	ReferencesToAttributes( ptRailRunner, railRunner );

	if( const Train* pTrain = dynamic_cast<const Train*>(&railRunner); pTrain )
		WriteTrainContent( ptRailRunner, *pTrain );
	else if( const RollingStock* pRollingStock = dynamic_cast<const RollingStock*>(&railRunner); pRollingStock )
		WriteRollingStockContent( ptRailRunner, *pRollingStock );
	else if( const WheelFrame* pWheelFrame = dynamic_cast<const WheelFrame*>(&railRunner); pWheelFrame )
		WriteWheelFrameContent( ptRailRunner, *pWheelFrame );
	else if( const Bogie* pBogie = dynamic_cast<const Bogie*>(&railRunner); pBogie )
		WriteBogieContent( ptRailRunner, *pBogie );
	else{
		std::ostringstream stream;
		stream << "Unknown RailRunner type!" << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::logic_error{ stream.str() };
	}

	move_child( pt, railRunner.TypeName(), ptRailRunner );
	return pt;
}

} // namespace ptreesupport
} // namespace trax
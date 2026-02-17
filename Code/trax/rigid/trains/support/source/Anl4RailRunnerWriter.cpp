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

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const RailRunner& railRunner ){
	if( const Train* pTrain = dynamic_cast<const Train*>(&railRunner); pTrain )
		pt << *pTrain;
	else if( const RollingStock* pRollingStock = dynamic_cast<const RollingStock*>(&railRunner); pRollingStock )
		pt << *pRollingStock;
	else if( const WheelFrame* pWheelFrame = dynamic_cast<const WheelFrame*>(&railRunner); pWheelFrame )
		pt << *pWheelFrame;
	else if( const Bogie* pBogie = dynamic_cast<const Bogie*>(&railRunner); pBogie )
		pt << *pBogie;
	else{
		std::ostringstream stream;
		stream << "Unknown RailRunner type!" << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::logic_error{ stream.str() };
	}

	return pt;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const Train& train ){
	boost::property_tree::ptree ptTrain;
	ReferencesToAttributes( ptTrain, train );

	for( int index = 0; index < train.GetNumberOfComponents(); ++index )
	{
		ptTrain << *train.GetComponent( index );
	}

	move_child( pt, train.TypeName(), ptTrain );
	return pt;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const RollingStock& rollingStock ){
	boost::property_tree::ptree ptRollingStock;
	ReferencesToAttributes( ptRollingStock, rollingStock );

	for( int index = 0; index < rollingStock.GetNumberOfTopmostParents(); ++index )
	{
		ptRollingStock << rollingStock.GetTopmostParent( index );
	}

	move_child( pt, rollingStock.TypeName(), ptRollingStock );
	return pt;
}

static void WriteBogieContent( boost::property_tree::ptree& ptBogie, const Bogie& bogie ){
	spat::Frame<Length,One> frame;
	bogie.GetGestalt().GetFrame( frame );
	ptBogie << frame;

	if( std::shared_ptr<Bogie> pBogieNorth = bogie.GetChild( EndType::north ).first; pBogieNorth )
		ptBogie << *pBogieNorth;

	if( std::shared_ptr<Bogie> pBogieSouth = bogie.GetChild( EndType::south ).first; pBogieSouth )
		ptBogie << *pBogieSouth;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const Bogie& bogie ){
	if( const WheelFrame* pWheelFrame = dynamic_cast<const WheelFrame*>(&bogie); pWheelFrame ){
		pt << *pWheelFrame;
	}
	else{
		boost::property_tree::ptree ptBogie;
		ReferencesToAttributes( ptBogie, bogie );

		WriteBogieContent( ptBogie, bogie );

		move_child( pt, bogie.TypeName(), ptBogie );
	}

	return pt;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const WheelFrame& wheelFrame ){
	boost::property_tree::ptree ptWheelFrame;

	WriteBogieContent( ptWheelFrame, wheelFrame );

	move_child( pt, wheelFrame.TypeName(), ptWheelFrame );
	return pt;
}

} // namespace ptreesupport
} // namespace trax
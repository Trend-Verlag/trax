//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
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
// For further information, please contact: horstmann.marc@trendverlag.de

#include "trax/collections/support/Anl4TrackSystemWriter.h"
#include "trax/collections/support/CollectionSupportWriteXML.h"

#include "trax/collections/ConnectorCollection.h"
#include "trax/collections/SignalCollection.h"
#include "trax/collections/TrackSystem.h"
#include "trax/collections/TrackCollectionContainer.h"


namespace trax{
namespace ptreesupport{

using namespace common;
using namespace common::ptreesupport;
using namespace dim::ptreesupport;
using namespace spat;
using namespace spat::ptreesupport;

using common::ptreesupport::operator<<;
using spat::ptreesupport::operator<<;

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const TrackSystem& tracksystem ){
	boost::property_tree::ptree ptTrackSystem;

	if( auto pTrack = tracksystem.GetActive() )
		ptTrackSystem.put( "<xmlattr>.activeTrack", pTrack->ID() );
	
	if( std::shared_ptr<const TrackCollectionContainer> pContainer = tracksystem.GetCollectionContainer() )
	{
		for( const TrackCollection& trackcollection : *pContainer )
			ptTrackSystem << trackcollection;
	}

	if( const ConnectorCollection* pConnectorCollection = tracksystem.GetConnectorCollection() )
		ptTrackSystem << *pConnectorCollection;

	move_child( pt, tracksystem.TypeName(), ptTrackSystem );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const TrackCollection& trackcollection ){
	boost::property_tree::ptree ptTrackCollection;

	ptTrackCollection.put( "<xmlattr>.id", trackcollection.ID() );
	ReferencesToAttributes( ptTrackCollection, trackcollection );
	ptTrackCollection << trackcollection.GetFrame();

	for( const TrackBuilder& track : trackcollection )
		ptTrackCollection << track;

	move_child( pt, trackcollection.TypeName(), ptTrackCollection );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const SignalCollection& signalCollection ){
	boost::property_tree::ptree ptSignalCollection;

	for( const Signal& signal : signalCollection )
	{
		if( auto pVelocityControl = dynamic_cast<const VelocityControl*>(&signal) )
			ptSignalCollection << *pVelocityControl;

		else if( auto pJumpSignal = dynamic_cast<const JumpSite*>(&signal) )
			ptSignalCollection << *pJumpSignal;
	}

	move_child( pt, signalCollection.TypeName(), ptSignalCollection );
	return pt;
}


} // namespace ptreesupport
} // namespace trax
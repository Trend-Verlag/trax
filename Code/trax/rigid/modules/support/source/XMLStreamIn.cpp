//	trax track library
//	AD 2014 
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
// For further information, please contact: horstmann@traxlibrary.dev

#include "../XMLStreamIn.h"
#include "../TraxSupportXML.h"
#include "dim/support/DimSupportXML.h"
#include "spat/support/SpatSupportXML.h"
#include "trax/support/TraxSupportStream.h"

#include "trax/collections/ConnectorCollection.h"
#include "trax/collections/IndicatorCollection.h"
#include "trax/collections/PulseCounterCollection.h"
#include "trax/collections/SignalCollection.h"
#include "trax/collections/TimerCollection.h"
#include "trax/collections/TrackCollectionContainer.h"
#include "trax/collections/TrackSystem.h"

#include "trax/rigid/Gestalt.h"
#include "trax/rigid/MovableTrack.h"
#include "trax/rigid/modules/Camera.h"
#include "trax/rigid/modules/Module.h"
#include "trax/rigid/modules/ModuleCollection.h"
#include "trax/rigid/trains/Cargo.h"
#include "trax/rigid/trains/collections/CargoCollection.h"
#include "trax/rigid/trains/collections/Fleet.h"
#include "trax/rigid/trains/RollingStock.h"
#include "trax/rigid/trains/Train.h"
#include "trax/rigid/trains/WheelFrame.h"

#include "trax/Curve.h"
#include "trax/Plug.h"
#include "trax/Section.h"
#include "trax/SectionTrack.h"
#include "trax/Sensor.h"
#include "trax/Switch.h"

namespace trax{

namespace ptreesupport{

	using namespace dim::ptreesupport;
	using namespace spat::ptreesupport;

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, TrackSystem& trackSystem )
{
	AttributesToReferences( pt, trackSystem );

	for( const auto& pair : pt )
	{
		if( pair.first == "TrackCollection" )
		{
			if( std::unique_ptr<TrackCollection> pTrackCollection = TrackCollection::Make(); pTrackCollection )
			{
				pair.second >> *pTrackCollection;
				if( auto pTrackCollectionContainer = trackSystem.GetCollectionContainer() )
					pTrackCollectionContainer->Add( std::move( pTrackCollection ) );
			}
		}

		else if( pair.first == "ConnectorCollection" )
		{
			if( std::unique_ptr<ConnectorCollection> pConnectorCollection = ConnectorCollection::Make(); pConnectorCollection )
			{
				pair.second >> *pConnectorCollection;
				trackSystem.SetConnectorCollection( std::move( pConnectorCollection ) );
			}
		}
	}

	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, TrackCollection& trackCollection )
{
	AttributesToReferences( pt, trackCollection );

	for( const auto& pair : pt )
	{
		if( pair.first == "Frame" ){
			spat::Frame<Length,One> frame;
			ReadFrame( pair.second, frame );
			trackCollection.SetFrame( frame );
		}

		else if( pair.first == "Track" )
		{
			if( std::shared_ptr<TrackBuilder> pTrack = MovableTrack::Make( TrackType( pair.second.get( "<xmlattr>.type", "standard" ) ) ); pTrack )
			{
				pair.second >> *pTrack;
				trackCollection.Add( std::move( pTrack ) );
			}
		}
	}

	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, ConnectorCollection& connectorCollection )
{
	for( const auto& pair : pt )
	{
		if( pair.first == "Switch" )
		{
			if( std::unique_ptr<Switch> pSwitch = Switch::Make(); pSwitch )
			{
				pair.second >> *pSwitch;
				connectorCollection.Add( std::move( pSwitch ) );
			}
		}

		else if( pair.first == "ThreeWaySwitch" )
		{
			if( std::unique_ptr<ThreeWaySwitch> pThreeWaySwitch = ThreeWaySwitch::Make(); pThreeWaySwitch )
			{
				pair.second >> *pThreeWaySwitch;
				connectorCollection.Add( std::move( pThreeWaySwitch ) );
			}
		}

		else if( pair.first == "SingleSlipSwitch" )
		{
			if( std::unique_ptr<SingleSlipSwitch> pSingleSlipSwitch = SingleSlipSwitch::Make(); pSingleSlipSwitch )
			{
				pair.second >> *pSingleSlipSwitch;
				connectorCollection.Add( std::move( pSingleSlipSwitch ) );
			}
		}

		else if( pair.first == "DoubleSlipSwitch" )
		{
			if( std::unique_ptr<DoubleSlipSwitch> pDoubleSlipSwitch = DoubleSlipSwitch::Make(); pDoubleSlipSwitch )
			{
				pair.second >> *pDoubleSlipSwitch;
				connectorCollection.Add( std::move( pDoubleSlipSwitch ) );
			}
		}
	}

	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Switch& switchObject )
{
	switchObject.Reference( "name", pt.get( "<xmlattr>.name", "" ) );
	switchObject.ID( pt.get( "<xmlattr>.id", 0 ) );
	switchObject.Set( ToConnectorStatus( pt.get( "<xmlattr>.status", "none" ) ), false );

	std::string refName = "NarrowTrack";
	for( const auto& pair : pt )
	{
		if( pair.first == "TrackEnd" ){
			Track::End end;
			ReadTrackEnd( pair.second, end );
			std::ostringstream stream;
			stream << end;
						
			switchObject.Reference( refName, stream.str() );

			if( refName == "NarrowTrack" )
				refName = "StraightTrack";
			else if( refName == "StraightTrack" )
				refName = "DivergedTrack";
		}

		else if( pair.first == "Plug" )
			pair.second >> switchObject.PlugTo( ConnectorStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) );

		else if( pair.first == "Jack" )
			pair.second >> switchObject.JackOn( ConnectorStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) );
	}

	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, ThreeWaySwitch& threeWaySwitch )
{
	threeWaySwitch.Reference( "name", pt.get( "<xmlattr>.name", "" ) );
	threeWaySwitch.ID( pt.get( "<xmlattr>.id", 0 ) );
	threeWaySwitch.Set( ToConnectorStatus( pt.get( "<xmlattr>.status", "none" ) ), false );

	std::string refName = "NarrowTrack";
	for( const auto& pair : pt )
	{
		if( pair.first == "TrackEnd" ){
			Track::End end;
			ReadTrackEnd( pair.second, end );
			std::ostringstream stream;
			stream << end;
						
			threeWaySwitch.Reference( refName, stream.str() );

			if( refName == "NarrowTrack" )
				refName = "StraightTrack";
			else if( refName == "StraightTrack" )
				refName = "DivergedTrack1";
			else if( refName == "DivergedTrack1" )
				refName = "DivergedTrack2";
		}

		else if( pair.first == "Plug" )
			pair.second >> threeWaySwitch.PlugTo( ConnectorStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) );

		else if( pair.first == "Jack" )
			pair.second >> threeWaySwitch.JackOn( ConnectorStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) );
	}

	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, SingleSlipSwitch& singleSlipSwitch )
{
	singleSlipSwitch.Reference( "name", pt.get( "<xmlattr>.name", "" ) );
	singleSlipSwitch.ID( pt.get( "<xmlattr>.id", 0 ) );
	singleSlipSwitch.Set( ToConnectorStatus( pt.get( "<xmlattr>.status", "none" ) ), false );

	SingleSlipSwitch::SlotNames slotName = SingleSlipSwitch::slot_0;
	for( const auto& pair : pt )
	{
		if( pair.first == "TrackEnd" ){
			Track::End end;
			ReadTrackEnd( pair.second, end );
			std::ostringstream stream;
			stream << end;
						
			singleSlipSwitch.Reference( ToString( slotName ), stream.str() );
			slotName = static_cast<SingleSlipSwitch::SlotNames>(slotName + 1);
		}

		else if( pair.first == "Plug" )
			pair.second >> singleSlipSwitch.PlugTo( ConnectorStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) );

		else if( pair.first == "Jack" )
			pair.second >> singleSlipSwitch.JackOn( ConnectorStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) );
	}

	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, DoubleSlipSwitch& doubleSlipSwitch )
{
	doubleSlipSwitch.Reference( "name", pt.get( "<xmlattr>.name", "" ) );
	doubleSlipSwitch.ID( pt.get( "<xmlattr>.id", 0 ) );
	doubleSlipSwitch.Set( ToConnectorStatus( pt.get( "<xmlattr>.status", "none" ) ), false );

	DoubleSlipSwitch::SlotNames slotName = DoubleSlipSwitch::slot_0;
	for( const auto& pair : pt )
	{
		if( pair.first == "TrackEnd" ){
			Track::End end;
			ReadTrackEnd( pair.second, end );
			std::ostringstream stream;
			stream << end;
						
			doubleSlipSwitch.Reference( ToString( slotName ), stream.str() );
			slotName = static_cast<DoubleSlipSwitch::SlotNames>(slotName + 1);
		}

		else if( pair.first == "Plug" )
			pair.second >> doubleSlipSwitch.PlugTo( ConnectorStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) );

		else if( pair.first == "Jack" )
			pair.second >> doubleSlipSwitch.JackOn( ConnectorStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) );
	}

	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, IndicatorCollection& indicatorCollection )
{
	for( const auto& pair : pt )
	{
		if( pair.first == "SwitchSemaphore" )
		{
			if( std::unique_ptr<BinaryIndicator> pBinaryIndicator = BinaryIndicator::Make( Indicator::Type::switch_semaphore ); pBinaryIndicator )
			{
				pair.second >> *pBinaryIndicator;
				indicatorCollection.Add( std::move( pBinaryIndicator ) );
			}
		}
	}

	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Line& line )
{
    return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, LineP& lineP )
{
	LineP::Data data;
	for( const auto& pair : pt )
	{
		if( pair.first == "VectorBundle" ){
			ReadVectorBundle( pair.second, data.vb );
			data.vb.T.Normalize();
		}

		else if( pair.first == "Vector" )
			ReadVector( pair.second, data.up );
	}

	lineP.Create( data );
	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Arc& arc )
{
    Arc::Data data;
	data.k = get( pt, "<xmlattr>.k", data.k, _1Im );
	arc.Create( data );
	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, ArcP& arcP )
{
  	ArcP::Data data;

	for( const auto& pair : pt )
	{
		if( pair.first == "VectorBundle2" ){
			ReadVectorBundle2( pair.second, data.vb2 );
		}
	}

	arcP.Create( data );
	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Helix& helix )
{
 	Helix::Data data;

	data.k = get( pt, "<xmlattr>.k", data.k, _1Im );
	data.t = get( pt, "<xmlattr>.t", data.t, _1Im );

	helix.Create( data );
	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, HelixP& helixP )
{
	HelixP::Data data;

	data.a = get( pt, "<xmlattr>.a", data.a, _m );
	data.b = get( pt, "<xmlattr>.b", data.b, _m );

	for( const auto& pair : pt )
	{
		if( pair.first == "VectorBundle2" )
			ReadVectorBundle2( pair.second, data.center );
	}

	helixP.Create( data );
	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Cubic& cubic )
{
	Cubic::Data data;
	Read( pt, data );
	cubic.Create( data );
	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Spline& spline )
{
 	Spline::Data data;

	for( const auto& pair : pt )
	{
		if( pair.first == "Cubic" ){
			data.push_back( {} );
			Read( pair.second, data.back() );
		}
	}

	spline.Create( data );
	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Clothoid& clothoid )
{
 	Clothoid::Data data;
	data.a = get( pt, "<xmlattr>.a", data.a, _m );
	clothoid.Create( data );
	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Rotator& rotator )
{
	Rotator::Data data;
	data.a = pt.get( "<xmlattr>.a", data.a );
	data.b = pt.get( "<xmlattr>.b", data.b );
	data.a0 = pt.get( "<xmlattr>.a0", data.a0 );
	data.b0 = pt.get( "<xmlattr>.b0", data.b0 );
	rotator.Create( data );
	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, RotatorChain& rotatorChain )
{
 	RotatorChain::Data data;

	for( const auto& pair : pt ){
		if( pair.first == "Link" )
			data.push_back( {	
				pair.second.get( "<xmlattr>.a", 0_rad ),
				pair.second.get( "<xmlattr>.b", 0_rad ),
				pair.second.get( "<xmlattr>.length", 1_m ) } );
	}

	rotatorChain.Create( data );
	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, PolygonalChain& polygonalChain )
{
  	PolygonalChain::Data data;

	for( const auto& pair : pt ){
		if( pair.first == "VectorBundle" ){
			data.push_back( {} );
			ReadVectorBundle( pair.second, data.back() );
		}
	}

	polygonalChain.Create( data );
	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, SampledCurve& sampledCurve )
{
	SampledCurve::Data data;

	for( const auto& pair : pt ){
		if( pair.first == "Sample" ){
			data.push_back( {} );
			Read( pair.second, data.back() );
		}
	}

	sampledCurve.Create( data );
	return pt;
}

const boost::property_tree::ptree& trax::ptreesupport::operator>>( const boost::property_tree::ptree& pt, ConstantTwist& constantTwist )
{
	constantTwist.TwistValue( pt.get( "<xmlattr>.angle", 0_rad ) );
	return pt;
}

const boost::property_tree::ptree& trax::ptreesupport::operator>>( const boost::property_tree::ptree& pt, LinearTwist& linearTwist )
{
	linearTwist.From( pt.get( "<xmlattr>.startangle", 0_rad ) );
	linearTwist.To( pt.get( "<xmlattr>.endangle", 0_rad ) );
	return pt;
}

const boost::property_tree::ptree& trax::ptreesupport::operator>>( const boost::property_tree::ptree& pt, PiecewiseTwist& piecewiseTwist )
{
	PiecewiseTwist::Data data;

	for( const auto& pair : pt )
	{
		if( pair.first == "TwistAngle" )
			data.push_back( std::make_pair( 
				get( pair.second, "<xmlattr>.s", 0_m, _m ),
				pair.second.get( "<xmlattr>.value", 0_rad ) ) );
	}
	piecewiseTwist.Create( data );

	return pt;
}

const boost::property_tree::ptree& trax::ptreesupport::operator>>( const boost::property_tree::ptree& pt, DirectionalTwist& directionalTwist )
{
	directionalTwist.Freeze( pt.get( "<xmlattr>.bFrozen", false ) );

	for( const auto& pair : pt )
	{
		if( pair.first == "Vector" ){
			spat::Vector<One> attractor;
			ReadVector( pair.second, attractor );
			directionalTwist.Attractor( attractor );
		}
	}

	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, PiecewiseDirectionalTwist& piecewiseDirectionalTwist )
{

	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, CombinedTwist& combinedTwist )
{
	std::vector<std::unique_ptr<RoadwayTwist>> twists;
	twists.reserve(2);

	for( const auto& pair : pt )
	{
		if( pair.first == "Twist" )
		{
			twists.push_back( CreateTwist( pair.second ) );
		}
	}

	if( twists.size() > 0 && twists.at(0) )
		combinedTwist.AttachTwist1( std::move(twists.at(0)) );

	if( twists.size() > 1 && twists.at(1) )
		combinedTwist.AttachTwist2( std::move(twists.at(1)) );

	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Section& section )
{
	if( section.GetSectionType() == Section::SpecialSections::custom )
	{
		section.SetCntPoints( pt.get( "<xmlattr>.count", 0 ) );
		int idx = 0;
		for( const auto& pair : pt )
		{
			if( pair.first == "Position2D" ){
				spat::Position2D<Length> position;
				ReadPosition2D( pair.second, position );
				section.Set( idx++, position );
			}
		}
	}

	section.Shift( spat::Vector2D<Length>{ get( pt, "<xmlattr>.shiftX", 0_m, _m ), get( pt, "<xmlattr>.shiftY", 0_m, _m ) } );
	section.Scale( get( pt, "<xmlattr>.scaleX", 1_1, _1 ), get( pt, "<xmlattr>.scaleY", 1_1, _1 ) );
	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, TrackBuilder& track )
{
	track.ID( pt.get( "<xmlattr>.id", IDType{0} ) );
	AttributesToReferences( pt, track );
	common::Interval<Length> range{ 0_m, get( pt, "<xmlattr>.length", 10_m, _m ) };

	if( MovableTrack* pMovableTrack = dynamic_cast<MovableTrack*>( &track ); pMovableTrack )
	{
		if( std::shared_ptr<Body> pBody = pMovableTrack->GetBody(); pBody )
		{
			pBody->Freeze( pt.get( "<xmlattr>.bFrozen", false ) );
		}
	}

	for( const auto& pair : pt )
	{
		if( pair.first == "Frame" ){
			spat::Frame<Length,One> frame;
			ReadFrame( pair.second, frame );
			track.SetFrame( frame );
		}

		else if( pair.first == "Interval" )
		{
			common::ptreesupport::ReadInterval( pair.second, range );
		}

		else if( pair.first == "Curve" )
		{
			track.Attach( CreateCurve( pair.second ), range );
		}

		else if( pair.first == "Twist" )
		{
			track.Attach( CreateTwist( pair.second ) );
		}

		else if( pair.first == "Section" )
		{
			if( SectionTrack* pSectionTrack = dynamic_cast<SectionTrack*>(&track); pSectionTrack )
			{
				if( std::unique_ptr<Section> pSection = Section::Make( SpecialSection( pair.second.get( "<xmlattr>.type", "custom" ) ) ); pSection )
				{
					pair.second >> *pSection;
					pSectionTrack->Attach( std::move(pSection) );
				}
			}
		}
	}

	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, BinaryIndicator& binaryIndicator )
{
	binaryIndicator.ID( pt.get( "<xmlattr>.id", IDType{0} ) );
	binaryIndicator.RefTargetID( pt.get( "<xmlattr>.refid", IDType{0} ) );
	binaryIndicator.Set( ToIndicatorStatus( pt.get( "<xmlattr>.status", ToString(Indicator::Status::unknown) )) );
	AttributesToReferences( pt, binaryIndicator );
	
	Indicator::Status status = Indicator::Status::unknown;
	for( const auto& pair : pt )
	{
		if( pair.first == "Frame" )
		{
			spat::Frame<Length,One> frame;
			ReadFrame( pair.second, frame );

			if( status == Indicator::Status::unknown )	
				binaryIndicator.SetFrame( frame );
			else
				binaryIndicator.LocalFrameForStatus( status, frame );

			status = static_cast<Indicator::Status>(static_cast<int>(status) + 1);
		}

		else if( pair.first == "Plug" )
			pair.second >> binaryIndicator.PlugTo( IndicatorStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) );
		
		else if( pair.first == "Jack" )
			pair.second >> binaryIndicator.JackOn( IndicatorStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) );
	}

	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Plug& plug ){
	plug.Reference( "name", pt.get( "<xmlattr>.name", "" ) );

	if( pt.get( "<xmlattr>.plugid", IDType{} ) == IDType{} )
		std::cerr << "Reading 'Plug': attribute 'plugid' not present or zero. Plug:id=\"" 
				  << pt.get<IDType>( "<xmlattr>.id", IDType{} ) << "\"" << std::endl;
	plug.ID( pt.get( "<xmlattr>.plugid", pt.get( "<xmlattr>.id", IDType{} ) ) );

	for( const auto& pair : pt ){
		if( pair.first == "Jack" )
			pair.second >> plug.JackOnPulse();
	}

	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, MultiPlug& plug ){
	pt >> (plug.ID() ? plug.Make( nullptr ) : static_cast<Plug&>(plug));	
	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, Jack& jack ){
	jack.Reference( "name", pt.get( "<xmlattr>.name", "" ) );
	jack.ID( pt.get( "<xmlattr>.id", IDType{0} ) );
	jack.RefPlugID( pt.get( "<xmlattr>.plugid", IDType{0} ) );
	return pt;
}



}


}
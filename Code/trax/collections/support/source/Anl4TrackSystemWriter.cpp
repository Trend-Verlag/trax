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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#include "trax/collections/support/Anl4TrackSystemWriter.h"

#include "trax/collections/SignalCollection.h"
#include "trax/collections/TrackSystem.h"
#include "trax/collections/TrackCollectionContainer.h"
#include "trax/collections/ConnectorCollection.h"

#include "trax/GeomTrack.h"
#include "trax/Indicator.h"
#include "trax/Jack.h"
#include "trax/LogicElements.h"
#include "trax/Plug.h"
#include "trax/Section.h"
#include "trax/Sensor.h"
#include "trax/Switch.h"
#include "trax/Timer.h"

#include "common/support/CommonSupportXML.h"
#include "dim/support/DimSupportXML.h"
#include "spat/support/SpatSupportXML.h"
#include "trax/collections/support/CollectionSupportXML.h"


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
		ptTrackSystem.add( "<xmlattr>.activeTrack", pTrack->ID() );
	
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

	ptTrackCollection.add( "<xmlattr>.id", trackcollection.ID() );
	if( !trackcollection.Reference( "name" ).empty() )
		ptTrackCollection.add( "<xmlattr>.name", trackcollection.Reference( "name" ) );

	ptTrackCollection << trackcollection.GetFrame();

	for( const TrackBuilder& track : trackcollection )
		ptTrackCollection << track;

	move_child( pt, trackcollection.TypeName(), ptTrackCollection );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Track::End& end ){
	boost::property_tree::ptree ptTrackEnd;

	ptTrackEnd.add( "<xmlattr>.refid", end.id );
	ptTrackEnd.add( "<xmlattr>.type", end.type == Track::EndType::front ? "front" : "end" );

	move_child( pt, "TrackEnd", ptTrackEnd );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const TrackBuilder& track ){
	boost::property_tree::ptree ptTrack;

	ptTrack.add( "<xmlattr>.id", track.ID() );
	ptTrack.add( "<xmlattr>.type", track.Reference( "type" ).empty() ? ToString( track.GetTrackType() ) : track.Reference( "type" ) );
	if( !track.Reference( "name" ).empty() )
		ptTrack.add( "<xmlattr>.name", track.Reference( "name" ) );
	ptTrack.add( "<xmlattr>.reference", track.Reference( "reference" ) );
	if( !track.Reference( "scale" ).empty() )
		ptTrack.add( "<xmlattr>.scale", track.Reference( "scale" ) );
	if( !track.Reference( "waterDepth" ).empty() )
		ptTrack.add( "<xmlattr>.waterDepth", track.Reference( "waterDepth" ) );

	if( track.Reference( "electrified" ) == "true" )
		ptTrack.add( "<xmlattr>.electrified", true );

	{
		boost::property_tree::ptree ptBegin;
		if( !track.Reference( "electrificationShiftBegin" ).empty() )
			ptBegin.add( "<xmlattr>.electrificationShift", track.Reference( "electrificationShiftBegin" ) );

		const Track::End frontend = track.TransitionEnd( Track::EndType::front );
		if( frontend.id ){
			boost::property_tree::ptree ptConnection;
			ptConnection << frontend;

			ptBegin.add_child( "Connection", ptConnection );
		}
		else if( track.Reference( "bufferStopBegin" ) == "true" ){
			boost::property_tree::ptree ptBufferStop;
			ptBegin.add_child( "BufferStop", ptBufferStop );
		}
		else{
			boost::property_tree::ptree ptOpenEnd;
			ptBegin.add_child( "OpenEnd", ptOpenEnd );
		}
		ptTrack.add_child( "Begin", ptBegin );
	}

	{
		boost::property_tree::ptree ptEnd;
		if( !track.Reference( "electrificationShiftEnd" ).empty() )
			ptEnd.add( "<xmlattr>.electrificationShift", track.Reference( "electrificationShiftEnd" ) );

		const Track::End backend = track.TransitionEnd( Track::EndType::end );
		if( backend.id ){
			boost::property_tree::ptree ptConnection;
			ptConnection << backend;

			ptEnd.add_child( "Connection", ptConnection );
		}
		else if( track.Reference( "bufferStopEnd" ) == "true" ){
			boost::property_tree::ptree ptBufferStop;
			ptEnd.add_child( "BufferStop", ptBufferStop );
		}
		else{
			boost::property_tree::ptree ptOpenEnd;
			ptEnd.add_child( "OpenEnd", ptOpenEnd );
		}
		ptTrack.add_child( "End", ptEnd );
	}

	ptTrack << track.GetFrame();

	auto pair = track.GetCurve(); 
	if( pair.first ){
		ptTrack << pair.second;
		ptTrack	<< *pair.first;
	}

	ptTrack	<< track.GetTwist();

	if( const GeomTrack* pGeomTrack = dynamic_cast<const GeomTrack*>(&track); pGeomTrack )
		if( auto pSection = pGeomTrack->GetSection() )
			ptTrack << *pSection;

	for( int idx = 0; idx < track.CountSensors(); ++idx )
		ptTrack << *track.GetSensor( idx );
	
	move_child( pt, "Track", ptTrack );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Location& location ){
	TrackLocationRef tlr;
	location.Get( tlr );
	pt << tlr;
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const TrackLocation& tlr ){
	boost::property_tree::ptree ptTrackLocation;

	ptTrackLocation.add( "<xmlattr>.parameter", tlr.parameter );
	ptTrackLocation.add( "<xmlattr>.orientation", tlr.orientation == Orientation::Value::para ? "para" : "anti" );

	move_child( pt, "TrackLocation", ptTrackLocation );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const TrackLocationRef& tlr ){
	boost::property_tree::ptree ptTrackLocationRef;

	ptTrackLocationRef.add( "<xmlattr>.refid", tlr.refid );
	ptTrackLocationRef.add( "<xmlattr>.parameter", tlr.location.parameter );
	ptTrackLocationRef.add( "<xmlattr>.orientation", tlr.location.orientation == Orientation::Value::para ? "para" : "anti" );

	move_child( pt, "TrackLocation", ptTrackLocationRef );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const TrackRange& tr ){
	boost::property_tree::ptree ptTrackRange;

	ptTrackRange.add( "<xmlattr>.refid", tr.refid );
	ptTrackRange.add( "<xmlattr>.near", tr.range.Near() );
	ptTrackRange.add( "<xmlattr>.far", tr.range.Far() );

	move_child( pt, "TrackRange", ptTrackRange );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const SensorFilterJack::Effect& effect ){
	boost::property_tree::ptree ptEffect;

	ptEffect.add<std::string>( "<xmlattr>.type", ToString( effect ) );

	move_child( pt, "Effect", ptEffect );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Sensor& sensor ){
	boost::property_tree::ptree ptSensor;

	ptSensor.add( "<xmlattr>.name", sensor.Reference( "name" ) );
	ptSensor.add( "<xmlattr>.id", sensor.ID() );

	//if( auto pVelocitySensor = dynamic_cast<const VelocitySensor*>(&sensor) ){
	//	ptSensor.add( "<xmlattr>.minVelocity", pVelocitySensor->VelocityMin() );
	//	ptSensor.add( "<xmlattr>.maxVelocity", pVelocitySensor->VelocityMax() );
	//	ptSensor.add( "<xmlattr>.triggerInside", pVelocitySensor->TriggerInside() );
	//}

	//if( auto pWeighSensor = dynamic_cast<const WeighSensor*>(&sensor) ){
	//	ptSensor.add( "<xmlattr>.minWeight", pWeighSensor->WeightMin() );
	//	ptSensor.add( "<xmlattr>.maxWeight", pWeighSensor->WeightMax() );
	//	ptSensor.add( "<xmlattr>.triggerInside", pWeighSensor->TriggerInside() );
	//	ptSensor.add( "<xmlattr>.weighTrain", pWeighSensor->WeighTrain() );
	//}

	if( auto pSensorFilterJack = dynamic_cast<const SensorFilterJack*>(&sensor) ){
		if( pSensorFilterJack->GetEffect() & SensorFilterJack::Effect::train_tip )
			ptSensor << SensorFilterJack::Effect::train_tip;
		if( pSensorFilterJack->GetEffect() & SensorFilterJack::Effect::train_end )
			ptSensor << SensorFilterJack::Effect::train_end;
		if( pSensorFilterJack->GetEffect() & SensorFilterJack::Effect::train_north )
			ptSensor << SensorFilterJack::Effect::train_north;
		if( pSensorFilterJack->GetEffect() & SensorFilterJack::Effect::train_south )
			ptSensor << SensorFilterJack::Effect::train_south;
		if( pSensorFilterJack->GetEffect() & SensorFilterJack::Effect::wheelframe )
			ptSensor << SensorFilterJack::Effect::wheelframe;
		if( pSensorFilterJack->GetEffect() & SensorFilterJack::Effect::name )
			ptSensor << SensorFilterJack::Effect::name;
		if( pSensorFilterJack->GetEffect() & SensorFilterJack::Effect::not_name )
			ptSensor << SensorFilterJack::Effect::not_name;
	}

	TrackLocation trackLocation;
	assert( sensor.TrackAttached() );
	sensor.TrackAttached()->Attached( sensor, &trackLocation );
	ptSensor << trackLocation;

	if( auto pPlugEnumerator = dynamic_cast<const PlugEnumerator*>(&sensor) )
		ptSensor << *pPlugEnumerator;

	if( auto pJackEnumerator = dynamic_cast<const JackEnumerator*>(&sensor) )
		ptSensor << *pJackEnumerator;

	move_child( pt, sensor.TypeName(), ptSensor );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Section& section ){
	boost::property_tree::ptree ptSection;
	ptSection.add( "<xmlattr>.type", ToString( section.GetSectionType() ) );

	if( section.GetSectionType() == Section::SpecialSections::custom ){
		ptSection.add( "<xmlattr>.count", section.CountPoints() );
		for( int idx = 0; idx < section.CountPoints(); ++idx )
			ptSection << section.Get( idx ).p;
	}

	move_child( pt, section.TypeName(), ptSection );
	return pt;
}
//
//boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const ConnectorCollection& connectorCollection ){
//	boost::property_tree::ptree ptConnectorCollection;
//
//	for( const Connector& connector : connectorCollection )
//		ptConnectorCollection << connector;
//
//	move_child( pt, connectorCollection.TypeName(), ptConnectorCollection );
//	return pt;
//}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Connector& connector ){
	boost::property_tree::ptree ptConnector;

	ptConnector.add( "<xmlattr>.id", connector.ID() );
	if( !connector.Reference( "name" ).empty() )
		ptConnector.add( "<xmlattr>.name", connector.Reference( "name" ) );
	if( !connector.Reference( "Key_Id" ).empty() )
		ptConnector.add( "<xmlattr>.Key_Id", connector.Reference( "Key_Id" ) );
	if( !connector.Reference( "ImmoIdx" ).empty() )
		ptConnector.add( "<xmlattr>.ImmoIdx", connector.Reference( "ImmoIdx" ) );
	if( !connector.Reference( "kollektorID" ).empty() )
		ptConnector.add( "<xmlattr>.kollektorID", connector.Reference( "DKWType" ) );
	if( !connector.Reference( "DKWType" ).empty() )
		ptConnector.add( "<xmlattr>.DKWType", connector.Reference( "DKWType" ) );

	if( auto pSwitch = dynamic_cast<const Switch*>(&connector) )
		ptConnector.add( "<xmlattr>.status", ToString(pSwitch->Get()) );
	else if( auto pThreeWaySwitch = dynamic_cast<const ThreeWaySwitch*>(&connector) )
		ptConnector.add( "<xmlattr>.status", ToString(pThreeWaySwitch->Get()) );
	else if( auto pSingleSlipSwitch = dynamic_cast<const SingleSlipSwitch*>(&connector) ){
		ptConnector.add( "<xmlattr>.status", ToString(pSingleSlipSwitch->Get()) );
		Frame<Length,One> center;
		pSingleSlipSwitch->GetCenter( center );
		ptConnector << center;
	}
	else if( auto pDoubleSlipSwitch = dynamic_cast<const DoubleSlipSwitch*>(&connector) ){
		ptConnector.add( "<xmlattr>.status", ToString(pDoubleSlipSwitch->Get()) );
		Frame<Length,One> center;
		pDoubleSlipSwitch->GetCenter( center );
		ptConnector << center;
	}
	else{
		std::ostringstream stream;
		stream << "Unknown Connector type!" << std::endl;
		stream << __FILE__ << '(' << __LINE__ << ')' << std::endl;
		throw std::invalid_argument( stream.str() );
	}

	for( int slot = 0; slot < connector.CntSlots(); ++slot ){
		boost::property_tree::ptree ptTrackEnd;

		auto trackEnd = connector.Slot( slot ); 
		if( trackEnd.first ){
			ptTrackEnd.add( "<xmlattr>.refid", trackEnd.first->ID() );
			ptTrackEnd.add( "<xmlattr>.type", ToString(trackEnd.second) );
		}
		else{
			ptTrackEnd.add( "<xmlattr>.refid", 0 );
		}

		ptConnector.add_child( "TrackEnd", ptTrackEnd );
	}

	if( auto pPlugEnumerator = dynamic_cast<const PlugEnumerator*>(&connector) )
		ptConnector << *pPlugEnumerator;

	if( auto pJackEnumerator = dynamic_cast<const JackEnumerator*>(&connector) )
		ptConnector << *pJackEnumerator;

	move_child( pt, connector.TypeName(), ptConnector );
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

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const VelocityControl& signal ){
	boost::property_tree::ptree ptSignal;

	ptSignal.add( "<xmlattr>.id", signal.ID() );
	if( !signal.Reference( "name" ).empty() )
		ptSignal.add( "<xmlattr>.name", signal.Reference( "name" ) );
	ptSignal.add( "<xmlattr>.status", ToString( signal.Get() ) );
	ptSignal.add( "<xmlattr>.stopDistance", signal.StopDistance() );
	if( !signal.Reference( "Key_Id" ).empty() )
		ptSignal.add( "<xmlattr>.refid", signal.Reference( "Key_Id" ) );

	try{ 
		Location location{signal.GetLocation()};
		if( location.IsOnTrack() )
		{
			Frame<Length,One> frame;
			location.Transition( frame );
			ptSignal << frame;
		}
	}
	catch( const std::out_of_range& e ){
		std::cerr << "VelocityControl signal's track location couldn't get resolved. Could not write a frame. " 
			<< "Signal id: " << signal.ID() << ": " << e.what() << std::endl;
	}

	TrackRange trackRange;
	signal.GetTrackRange( trackRange );
	ptSignal << trackRange;

	for( int i = static_cast<int>(Signal::Status::velocity0); i <= static_cast<int>(Signal::Status::velocityMax); ++i ){
		const Signal::Status status{ static_cast<Signal::Status>(i) };
		if( signal.IsValidState( status ) ){
			boost::property_tree::ptree ptControlState;

			ptControlState.add( "<xmlattr>.status", ToString( status ) );
			ptControlState.add( "<xmlattr>.velocityMin", signal.VelocityMin( status ) );
			ptControlState.add( "<xmlattr>.velocityMax", signal.VelocityMax( status ) );
			ptControlState.add( "<xmlattr>.affectTravelVelocity", signal.AffectTravelVelocity( status ) );

			ptSignal.add_child( "ControlState", ptControlState );
		}
	}

	if( auto pPlugEnumerator = dynamic_cast<const PlugEnumerator*>(&signal) )
		ptSignal << *pPlugEnumerator;

	if( auto pJackEnumerator = dynamic_cast<const JackEnumerator*>(&signal) )
		ptSignal << *pJackEnumerator;

	move_child( pt, signal.TypeName(), ptSignal );
	return pt;
}


boost::property_tree::ptree & operator<<( boost::property_tree::ptree& pt, const JumpSite& signal ){
	boost::property_tree::ptree ptSignal;

	ptSignal.add( "<xmlattr>.id", signal.ID() );
	if( !signal.Reference( "name" ).empty() )
		ptSignal.add( "<xmlattr>.name", signal.Reference( "name" ) );
	ptSignal.add( "<xmlattr>.status", ToString( signal.Get() ) );

	TrackRange trackRange;
	signal.GetTrackRange( trackRange );
	ptSignal << trackRange;

	boost::property_tree::ptree ptTarget;
	if( signal.IsTargetLocation() )
		ptTarget << signal.TargetLocation();
	//else if( auto pDepot = signal.TargetDepot() ){
	//	boost::property_tree::ptree ptDepot;
	//	ptDepot.add( "<xmlattr>.depotID", pDepot->ID() );
	//	ptTarget.add_child( "Depot", ptDepot );
	//}
	ptSignal.add_child( "Target", ptTarget );

	if( auto pPlugEnumerator = dynamic_cast<const PlugEnumerator*>(&signal) )
		ptSignal << *pPlugEnumerator;

	if( auto pJackEnumerator = dynamic_cast<const JackEnumerator*>(&signal) )
		ptSignal << *pJackEnumerator;

	move_child( pt, signal.TypeName(), ptSignal );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Indicator& indicator ){
	boost::property_tree::ptree ptIndicator;

	ptIndicator.add( "<xmlattr>.id", indicator.ID() );
	if( !indicator.Reference( "name" ).empty() )
		ptIndicator.add( "<xmlattr>.name", indicator.Reference( "name" ) );
	ptIndicator.add( "<xmlattr>.reference", indicator.Reference( "reference" ) );
	ptIndicator.add( "<xmlattr>.status", ToString( indicator.Get() ) );
	ptIndicator << indicator.GetFrame();

	if( auto pPlugEnumerator = dynamic_cast<const PlugEnumerator*>(&indicator) )
		ptIndicator << *pPlugEnumerator;

	if( auto pJackEnumerator = dynamic_cast<const JackEnumerator*>(&indicator) )
		ptIndicator << *pJackEnumerator;

	if( auto pSignalAligned = dynamic_cast<const SignalAligned*>(&indicator) ){
		ptIndicator.add( "<xmlattr>.refid", pSignalAligned->GetSignalRef() );
		ptIndicator.add( "<xmlattr>.parameterOffset", pSignalAligned->ParameterOffset() );
		ptIndicator.add( "<xmlattr>.bPreserveUpDirection", pSignalAligned->PreserveUpDirection() );
	}

	if( auto pSwitchAligned = dynamic_cast<const SwitchAligned*>(&indicator) ){
		ptIndicator.add( "<xmlattr>.refid", pSwitchAligned->GetSwitchRef() );
		ptIndicator.add( "<xmlattr>.bPreserveUpDirection", pSwitchAligned->PreserveUpDirection() );
	}

	move_child( pt, indicator.TypeName(), ptIndicator );
	return pt;
}
boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Curve& curve ){
	boost::property_tree::ptree ptCurve;

	if( auto pLine = dynamic_cast<const LineP*>(&curve) )
		ptCurve << *pLine;
	else if( auto pArc = dynamic_cast<const ArcP*>(&curve) )
		ptCurve << *pArc;
	else if( auto pHelix = dynamic_cast<const HelixP*>(&curve) )
		ptCurve << *pHelix;
	if( auto pLine = dynamic_cast<const Line*>(&curve) )
		ptCurve << *pLine;
	else if( auto pArc = dynamic_cast<const Arc*>(&curve) )
		ptCurve << *pArc;
	else if( auto pHelix = dynamic_cast<const Helix*>(&curve) )
		ptCurve << *pHelix;
	else if( auto pCubic = dynamic_cast<const Cubic*>(&curve) )
		ptCurve << *pCubic;
	else if( auto pSpline = dynamic_cast<const Spline*>(&curve) )
		ptCurve << *pSpline;
	else if( auto pClothoid = dynamic_cast<const Clothoid*>(&curve) )
		ptCurve << *pClothoid;
	else if( auto pRotator = dynamic_cast<const Rotator*>(&curve) )
		ptCurve << *pRotator;
	else if( auto pRotatorChain = dynamic_cast<const RotatorChain*>(&curve) )
		ptCurve << *pRotatorChain;
	else if( auto pPolygonalChain = dynamic_cast<const PolygonalChain*>(&curve) )
		ptCurve << *pPolygonalChain;
	else if( auto pSampledCurve = dynamic_cast<const SampledCurve*>(&curve) )
		ptCurve << *pSampledCurve;

	move_child( pt, "Curve", ptCurve );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const CurveSample& sample ){
	boost::property_tree::ptree ptSample;
	using namespace spat::ptreesupport;

	ptSample.add( "<xmlattr>.s", sample.s );
	ptSample.add( "<xmlattr>.k", sample.k );
	ptSample.add( "<xmlattr>.t", sample.t );
	ptSample << sample.F;

	move_child( pt, "Sample", ptSample );
	return pt;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const Line & curve )
{
	boost::property_tree::ptree ptCurve;

	move_child( pt, curve.TypeName(), ptCurve );
	return pt;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const Arc & curve )
{
	boost::property_tree::ptree ptCurve;

	ptCurve.add( "<xmlattr>.k", curve.GetData().k );

	move_child( pt, curve.TypeName(), ptCurve );
	return pt;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const Helix & curve )
{
	boost::property_tree::ptree ptCurve;

	ptCurve.add( "<xmlattr>.k", curve.GetData().k );
	ptCurve.add( "<xmlattr>.t", curve.GetData().t );

	move_child( pt, curve.TypeName(), ptCurve );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const LineP& curve ){
	boost::property_tree::ptree ptCurve;

	ptCurve << curve.GetData().vb;
	ptCurve << curve.GetData().up;

	move_child( pt, curve.TypeName(), ptCurve );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const ArcP& curve ){
	boost::property_tree::ptree ptCurve;

	ptCurve << curve.GetData().vb2;

	move_child( pt, curve.TypeName(), ptCurve );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const HelixP& curve ){
	boost::property_tree::ptree ptCurve;

	ptCurve.add( "<xmlattr>.a", curve.GetData().a );
	ptCurve.add( "<xmlattr>.b", curve.GetData().b );
	ptCurve << curve.GetData().center;

	move_child( pt, curve.TypeName(), ptCurve );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Cubic& curve ){
	boost::property_tree::ptree ptCurve;

	const Cubic::Data& data = curve.GetData();
	ptCurve << data.a << data.b << data.c << data.d;

	move_child( pt, curve.TypeName(), ptCurve );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Spline& curve ){
	boost::property_tree::ptree ptCurve;

	const Spline::Data& data = curve.GetData();
	for( const auto& cubic : data ){
		boost::property_tree::ptree ptCubic;
		ptCubic << cubic.a << cubic.b << cubic.c << cubic.d;
		move_child( ptCurve, "Cubic", ptCubic );
	}

	move_child( pt, curve.TypeName(), ptCurve );
	return pt;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const Clothoid& curve ){
	boost::property_tree::ptree ptCurve;

	ptCurve.add( "<xmlattr>.a", curve.GetData().a );

	move_child( pt, curve.TypeName(), ptCurve );
	return pt;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const Rotator& curve ){
	boost::property_tree::ptree ptCurve;

	ptCurve.add( "<xmlattr>.a", curve.GetData().a );
	ptCurve.add( "<xmlattr>.b", curve.GetData().b );
	if( curve.GetData().a0 != 0_rad || curve.GetData().b0 != 0_rad ){
		ptCurve.add( "<xmlattr>.a0", curve.GetData().a0 );
		ptCurve.add( "<xmlattr>.b0", curve.GetData().b0 );
	}

	move_child( pt, curve.TypeName(), ptCurve );
	return pt;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const RotatorChain& curve ){
	boost::property_tree::ptree ptCurve;

	for( const auto& tuple : curve.GetData() ){
		boost::property_tree::ptree ptLink;

		ptLink.add( "<xmlattr>.a", std::get<0>(tuple) );
		ptLink.add( "<xmlattr>.b", std::get<1>(tuple) );
		ptLink.add( "<xmlattr>.length", std::get<2>(tuple) );

		move_child( ptCurve, "Link", ptLink );
	}

	move_child( pt, curve.TypeName(), ptCurve );
	return pt;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const PolygonalChain& curve ){
	boost::property_tree::ptree ptCurve;

	for( const auto& pos : curve.GetData() )
		ptCurve << pos;

	move_child( pt, curve.TypeName(), ptCurve );
	return pt;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const SampledCurve& curve ){
	boost::property_tree::ptree ptCurve;

	for( const auto& sample : curve.GetData() )
		ptCurve << sample;

	move_child( pt, curve.TypeName(), ptCurve );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const RoadwayTwist& twist ){
	boost::property_tree::ptree ptTwist;

	if( auto pConstantTwist = dynamic_cast<const ConstantTwist*>(&twist) )
		ptTwist << *pConstantTwist;
	else if( auto pLinearTwist = dynamic_cast<const LinearTwist*>(&twist) )		
		ptTwist << *pLinearTwist;
	else if( auto pPiecewiseTwist = dynamic_cast<const PiecewiseTwist*>(&twist) )
		ptTwist << *pPiecewiseTwist;
	else if( auto pDirectionalTwist = dynamic_cast<const DirectionalTwist*>(&twist) )		
		ptTwist << *pDirectionalTwist;
	else if( auto pCombinedTwist = dynamic_cast<const CombinedTwist*>(&twist) )
		ptTwist << *pCombinedTwist;
	else{
		boost::property_tree::ptree ptZeroTwist;
		ptTwist.add_child( "ZeroTwist", ptZeroTwist );
	}

	move_child( pt, "Twist", ptTwist );
	return pt;
}
	
boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const ConstantTwist& twist ){
	boost::property_tree::ptree ptTwist;

	ptTwist.add( "<xmlattr>.angle", twist.TwistValue() );

	move_child( pt, twist.TypeName(), ptTwist );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const LinearTwist& twist ){
	boost::property_tree::ptree ptTwist;

	ptTwist.add( "<xmlattr>.startangle", twist.From() );
	ptTwist.add( "<xmlattr>.endangle", twist.To() );

	move_child( pt, twist.TypeName(), ptTwist );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const PiecewiseTwist& twist ){
	boost::property_tree::ptree ptTwist;

	for( int idx = 0; idx < twist.CntTwistValues(); ++idx ){
		boost::property_tree::ptree ptTwistAngle;

		ptTwistAngle.add( "<xmlattr>.s", twist.Twist(idx).first );
		ptTwistAngle.add( "<xmlattr>.value", twist.Twist(idx).second );

		ptTwist.add_child( "TwistAngle", ptTwistAngle );
	}

	move_child( pt, twist.TypeName(), ptTwist );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const DirectionalTwist& twist ){
	boost::property_tree::ptree ptTwist;
	
	if( twist.IsFrozen() )
		ptTwist.add( "<xmlattr>.bFrozen", true );

	ptTwist << twist.Attractor();

	move_child( pt, twist.TypeName(), ptTwist );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const CombinedTwist& twist ){
	boost::property_tree::ptree ptTwist;
	
	ptTwist << twist.Twist1() << twist.Twist2();

	move_child( pt, twist.TypeName(), ptTwist );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const PlugEnumerator& plugEnumerator ){
	for( const Plug& plug : plugEnumerator )
		pt << plug;

	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Plug& plug ){
	if( plug.Plugged() || plug.JackOnPulse().Plugged() ){
		boost::property_tree::ptree ptPlug;

		ptPlug.add( "<xmlattr>.plugid", plug.ID() );
		ptPlug.add( "<xmlattr>.name", plug.Reference( "name" ) );

		if( plug.JackOnPulse().Plugged() )
			ptPlug << plug.JackOnPulse();

		move_child( pt, "Plug", ptPlug );
	}

	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const JackEnumerator& jackEnumerator ){
	for( const Jack& jack : jackEnumerator )
		pt << jack;

	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Jack& jack ){
	if( jack.Plugged() ){
		assert( jack.GetPlug() );

		if( const Plug* pPlug = GetFirstNonZeroIDPlugInChain( *jack.GetPlug() ) ){
			boost::property_tree::ptree ptJack;

			if( !jack.Reference( "name" ).empty() )
				ptJack.add( "<xmlattr>.name", jack.Reference( "name" ) );

			ptJack.add( "<xmlattr>.plugid", pPlug->ID() );

			move_child( pt, "Jack", ptJack );
		}
	}

	return pt;
}

boost::property_tree::ptree & operator<<( boost::property_tree::ptree& pt, const PulseCounter& pulseCounter ){
	boost::property_tree::ptree ptPulseCounter;

	ptPulseCounter.add( "<xmlattr>.id", pulseCounter.ID() );
	if( !pulseCounter.Reference( "name" ).empty() )
		ptPulseCounter.add( "<xmlattr>.name", pulseCounter.Reference( "name" ) );
	ptPulseCounter.add( "<xmlattr>.threshold", pulseCounter.Threshold() );
	ptPulseCounter.add( "<xmlattr>.counter", pulseCounter.Counter() );

	if( auto pPlugEnumerator = dynamic_cast<const PlugEnumerator*>(&pulseCounter) )
		ptPulseCounter << *pPlugEnumerator;

	if( auto pJackEnumerator = dynamic_cast<const JackEnumerator*>(&pulseCounter) )
		ptPulseCounter << *pJackEnumerator;

	move_child( pt, pulseCounter.TypeName(), ptPulseCounter );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Timer& timer ){
	boost::property_tree::ptree ptTimer;

	ptTimer.add( "<xmlattr>.id", timer.ID() );
	if( !timer.Reference( "name" ).empty() )
		ptTimer.add( "<xmlattr>.name", timer.Reference( "name" ) );
	ptTimer.add( "<xmlattr>.lapse", timer.Lapse() );
	ptTimer.add( "<xmlattr>.periodic", timer.Periodic() );
	ptTimer.add( "<xmlattr>.started", timer.IsStarted() );
	ptTimer.add( "<xmlattr>.time", timer.GetTime() );

	if( auto pPlugEnumerator = dynamic_cast<const PlugEnumerator*>(&timer) )
		ptTimer << *pPlugEnumerator;

	if( auto pJackEnumerator = dynamic_cast<const JackEnumerator*>(&timer) )
		ptTimer << *pJackEnumerator;

	move_child( pt, timer.TypeName(), ptTimer );
	return pt;
}

} // namespace ptreesupport
} // namespace trax
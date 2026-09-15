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
// For further information, please contact: horstmann@traxlibrary.dev

#include "../XMLStreamOut.h"
#include "common/support/CommonSupportXML.h"
#include "dim/support/DimSupportStream.h"
#include "spat/support/SpatSupportXML.h"

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

#include <iostream>

namespace trax{

namespace ptreesupport{

	using common::ptreesupport::operator<<;
	using namespace spat;
	using spat::ptreesupport::operator<<;

///////////////////////////////////////
boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const ModuleCollection& moduleCollection ){
	boost::property_tree::ptree ptModuleCollection;

	for( const auto& module : moduleCollection )
		ptModuleCollection << module;

	move_child( pt, moduleCollection.TypeName(), ptModuleCollection );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Module& _module )
{
	boost::property_tree::ptree ptModule;
	ReferencesToAttributes( ptModule, _module );
	ptModule.put( "<xmlattr>.id", _module.ID() );
	ptModule.put( "<xmlattr>.maxValidPlugID", _module.MaxValidPlugID() );

	WriteLocalizedTag( ptModule, "DisplayName", _module );
	WriteLocalizedTag( ptModule, "Description", _module );

	spat::Box<Length> area{_module.GetVolume()};
	area.Move( spat::Origin3D<Length> - _module.GetFrame().P );

	ptModule << area;
	ptModule << _module.GetFrame();

	if( const auto pTrackSystem = _module.GetTrackSystem(); pTrackSystem )
		ptModule << *pTrackSystem;

	if( const auto pCargoCollection = _module.GetCargoCollection(); pCargoCollection )
		ptModule << *pCargoCollection;

	if( const auto pFleet = _module.GetFleet(); pFleet )
		ptModule << *pFleet;

	if( const auto pSignalCollection = _module.GetSignalCollection(); pSignalCollection )
		ptModule << *pSignalCollection;
	
	if( const auto pIndicatorCollection = _module.GetIndicatorCollection(); pIndicatorCollection )
		ptModule << *pIndicatorCollection;

	if( const auto pPulseCounterCollection = _module.GetPulseCounterCollection(); pPulseCounterCollection )
		ptModule << *pPulseCounterCollection;

	if( const auto pTimerCollection = _module.GetTimerCollection(); pTimerCollection )
		ptModule << *pTimerCollection;

	if( const auto pCameraCollection = _module.GetCameraCollection() )
		ptModule << *pCameraCollection;

	move_child( pt, _module.TypeName(), ptModule );
	return pt;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const CameraCollection& cameraCollection ){
	boost::property_tree::ptree ptCameraCollection;

	ptCameraCollection.put( "<xmlattr>.userCameraType", From( cameraCollection.UserCameraType() ) );
	ptCameraCollection.put( "<xmlattr>.userStartCamera", cameraCollection.UserStartCameraID() );

	for( const auto& camera : cameraCollection )
		ptCameraCollection << camera;

	//ptCameraCollection << static_cast<const trax::TriggerShapeCollection&>(cameraCollection);

	move_child( pt, cameraCollection.TypeName(), ptCameraCollection );
	return pt;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const Camera& camera ){
	boost::property_tree::ptree ptCamera;
	ReferencesToAttributes( ptCamera, camera );
	ptCamera.put( "<xmlattr>.id", camera.ID() );
	ptCamera.put( "<xmlattr>.type", From( camera.GetType() ) );
	ptCamera << camera.GetFrame();

	//if( auto pBodyCam = dynamic_cast<const trax::BodyCam*>(&camera) )
	//	if( auto pRailRunner = std::dynamic_pointer_cast<const RailRunner>(pBodyCam->GetBody()); pRailRunner )
	//		ptCamera.put( "<xmlattr>.railRunnerName", pRailRunner->Reference( "name" ) );

	move_child( pt, camera.TypeName(), ptCamera );
	return pt;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const Fleet& fleet )
{
	boost::property_tree::ptree ptFleet;

	for( const auto& element : fleet )
	{
		if( element.GetTrain() == nullptr ) // trains get written by their parent.
			ptFleet << element;
	}

	move_child( pt, fleet.TypeName(), ptFleet );
	return pt;
}

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

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Cargo& cargo )
{
	boost::property_tree::ptree ptCargo;

	ptCargo.put( "<xmlattr>.id", cargo.ID() );
	ReferencesToAttributes( ptCargo, cargo );

	if( std::shared_ptr<Shape> pShape = cargo.GetShape(); pShape )
	{
		spat::Frame<Length,One> frame;
		pShape->GetFrame( frame );
		ptCargo << frame;
	}

	if( const MoveableCargo* pMovableCargo = dynamic_cast<const MoveableCargo*>(&cargo); pMovableCargo )
	{
		if( std::shared_ptr<Gestalt> pGestalt = pMovableCargo->GetGestalt(); pGestalt )
		{
			spat::Vector<Velocity> velocity;
			pGestalt->GetLinearVelocity( velocity );
			ptCargo << velocity;
			spat::Vector<AngularVelocity> angularVelocity;
			pGestalt->GetAngularVelocity( angularVelocity );
			ptCargo << angularVelocity;
		}
	}

	move_child( pt, cargo.TypeName(), ptCargo );
	return pt;
}

static void WriteTrainContent( boost::property_tree::ptree& ptTrain, const Train& train )
{
	ptTrain.put( "<xmlattr>.id", train.ID() );
	ptTrain.put( "<xmlattr>.orientation", ToString( train.GetOrientation() ) );
	
	for( int index = 0; index < train.GetNumberOfComponents(); ++index )
	{
		ptTrain << *train.GetComponent( index );
	}
}

static void WriteRollingStockContent( boost::property_tree::ptree& ptRollingStock, const RollingStock& rollingStock )
{
	ptRollingStock.put( "<xmlattr>.id", rollingStock.ID() );
	ptRollingStock.put( "<xmlattr>.orientation", ToString( rollingStock.GetOrientation() ) );

	for( int index = 0; index < rollingStock.GetNumberOfTopmostParents(); ++index )
	{
		ptRollingStock << rollingStock.GetTopmostParent( index );
	}
}

static void WriteBogieContent( boost::property_tree::ptree& ptBogie, const Bogie& bogie )
{
	ptBogie.add( "<xmlattr>.couplingNorthActivated", bogie.IsActivated( EndType::north ) );
	ptBogie.add( "<xmlattr>.couplingSouthActivated", bogie.IsActivated( EndType::south ) );

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

	ptWheelFrame.add( "<xmlattr>.target_velocity", wheelFrame.TargetVelocity() );
	ptWheelFrame.add( "<xmlattr>.thrust", wheelFrame.Thrust() );
	ptWheelFrame.add( "<xmlattr>.brake", wheelFrame.Brake() );
	ptWheelFrame.add( "<xmlattr>.bRailed", wheelFrame.IsRailed() );
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

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Track::End& end ){
	boost::property_tree::ptree ptTrackEnd;

	ptTrackEnd.put( "<xmlattr>.refid", end.id );
	ptTrackEnd.put( "<xmlattr>.type", end.type == EndType::north ? "front" : "end" );

	move_child( pt, "TrackEnd", ptTrackEnd );
	return pt;
}


static void WriteTrack( boost::property_tree::ptree& ptTrack, const TrackBuilder& track )
{
	ptTrack.put( "<xmlattr>.id", track.ID() );
	ReferencesToAttributes( ptTrack, track );
	ptTrack.put( "<xmlattr>.type", track.IsEmptyReference( "type" ) ? ToString( track.GetTrackType() ) : track.Reference( "type" ) );

	{
		boost::property_tree::ptree ptBegin;
		if( !track.IsEmptyReference( "electrificationShiftBegin" ) )
			ptBegin.put( "<xmlattr>.electrificationShift", track.Reference( "electrificationShiftBegin" ) );

		const Track::End frontend = track.TransitionEnd( EndType::north );
		if( frontend.id ){
			boost::property_tree::ptree ptConnection;
			ptConnection << frontend;

			ptBegin.add_child( "Connection", ptConnection );
		}
		else if( track.IsReference( "bufferStopBegin", "true" ) ){
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
		if( !track.IsEmptyReference( "electrificationShiftEnd" ) )
			ptEnd.put( "<xmlattr>.electrificationShift", track.Reference( "electrificationShiftEnd" ) );

		const Track::End backend = track.TransitionEnd( EndType::south );
		if( backend.id ){
			boost::property_tree::ptree ptConnection;
			ptConnection << backend;

			ptEnd.add_child( "Connection", ptConnection );
		}
		else if( track.IsReference( "bufferStopEnd", "true" ) ){
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

	for( int idx = 0; idx < track.CountSensors(); ++idx )
		ptTrack << *track.GetSensor( idx );
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const TrackBuilder& track )
{
	boost::property_tree::ptree ptTrack;

	WriteTrack( ptTrack, track );

	if( const SectionTrack* pSectionTrack = dynamic_cast<const SectionTrack*>(&track); pSectionTrack )
	{
		for( int idx = 0; idx < pSectionTrack->CntSections(); ++idx )
			if( std::shared_ptr<const Section> pSection = pSectionTrack->GetSection( idx ); pSection )
				ptTrack << *pSection;
	}

	if( const MovableTrack* pMovableTrack = dynamic_cast<const MovableTrack*>(&track); pMovableTrack )
	{
		if( std::shared_ptr<const Body> pBody = pMovableTrack->GetBody(); pBody )
		{
			pBody->IsFrozen() ? ptTrack.put( "<xmlattr>.bFrozen", true ) : ptTrack.put( "<xmlattr>.bFrozen", false );
		}
	}
	
	move_child( pt, "Track", ptTrack );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Location& location ){
	TrackSystemLocation tlr;
	location.Get( tlr );
	pt << tlr;
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const TrackLocation& tlr ){
	boost::property_tree::ptree ptTrackLocation;

	ptTrackLocation.put( "<xmlattr>.parameter", tlr.parameter );
	ptTrackLocation.put( "<xmlattr>.orientation", tlr.orientation == Orientation::Value::para ? "para" : "anti" );

	move_child( pt, "TrackLocation", ptTrackLocation );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const TrackSystemLocation& tlr ){
	boost::property_tree::ptree ptTrackLocationRef;

	ptTrackLocationRef.put( "<xmlattr>.refid", tlr.refid );
	ptTrackLocationRef.put( "<xmlattr>.parameter", tlr.location.parameter );
	ptTrackLocationRef.put( "<xmlattr>.orientation", tlr.location.orientation == Orientation::Value::para ? "para" : "anti" );

	move_child( pt, "TrackLocation", ptTrackLocationRef );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const TrackRange& tr ){
	boost::property_tree::ptree ptTrackRange;

	ptTrackRange.put( "<xmlattr>.refid", tr.refid );
	ptTrackRange.put( "<xmlattr>.near", tr.range.Near() );
	ptTrackRange.put( "<xmlattr>.far", tr.range.Far() );

	move_child( pt, "TrackRange", ptTrackRange );
	return pt;
}
//
//boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const SensorFilterJack::Effect& effect ){
//	boost::property_tree::ptree ptEffect;
//
//	ptEffect.add<std::string>( "<xmlattr>.type", ToString( effect ) );
//
//	move_child( pt, "Effect", ptEffect );
//	return pt;
//}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Sensor& sensor ){
	boost::property_tree::ptree ptSensor;
	ReferencesToAttributes( ptSensor, sensor );

	//if( auto pVelocitySensor = dynamic_cast<const VelocitySensor*>(&sensor) ){
	//	ptSensor.put( "<xmlattr>.minVelocity", pVelocitySensor->VelocityMin() );
	//	ptSensor.put( "<xmlattr>.maxVelocity", pVelocitySensor->VelocityMax() );
	//	ptSensor.put( "<xmlattr>.triggerInside", pVelocitySensor->TriggerInside() );
	//}

	//if( auto pWeighSensor = dynamic_cast<const WeighSensor*>(&sensor) ){
	//	ptSensor.put( "<xmlattr>.minWeight", pWeighSensor->WeightMin() );
	//	ptSensor.put( "<xmlattr>.maxWeight", pWeighSensor->WeightMax() );
	//	ptSensor.put( "<xmlattr>.triggerInside", pWeighSensor->TriggerInside() );
	//	ptSensor.put( "<xmlattr>.weighTrain", pWeighSensor->WeighTrain() );
	//}

	//if( auto pSensorFilterJack = dynamic_cast<const SensorFilterJack*>(&sensor) ){
	//	if( pSensorFilterJack->GetEffect() & SensorFilterJack::Effect::train_tip )
	//		ptSensor << SensorFilterJack::Effect::train_tip;
	//	if( pSensorFilterJack->GetEffect() & SensorFilterJack::Effect::train_end )
	//		ptSensor << SensorFilterJack::Effect::train_end;
	//	if( pSensorFilterJack->GetEffect() & SensorFilterJack::Effect::train_north )
	//		ptSensor << SensorFilterJack::Effect::train_north;
	//	if( pSensorFilterJack->GetEffect() & SensorFilterJack::Effect::train_south )
	//		ptSensor << SensorFilterJack::Effect::train_south;
	//	if( pSensorFilterJack->GetEffect() & SensorFilterJack::Effect::wheelframe )
	//		ptSensor << SensorFilterJack::Effect::wheelframe;
	//	if( pSensorFilterJack->GetEffect() & SensorFilterJack::Effect::name )
	//		ptSensor << SensorFilterJack::Effect::name;
	//	if( pSensorFilterJack->GetEffect() & SensorFilterJack::Effect::not_name )
	//		ptSensor << SensorFilterJack::Effect::not_name;
	//}

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
	ptSection.put( "<xmlattr>.type", ToString( section.GetSectionType() ) );

	if( section.GetSectionType() == Section::SpecialSections::custom ){
		ptSection.put( "<xmlattr>.count", section.CountPoints() );
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

	ptConnector.put( "<xmlattr>.id", connector.ID() );
	ReferencesToAttributes( ptConnector, connector );
	if( !connector.IsEmptyReference( "kollektorID" ) )
		ptConnector.put( "<xmlattr>.kollektorID", connector.Reference( "DKWType" ) );

	if( auto pSwitch = dynamic_cast<const Switch*>(&connector) )
		ptConnector.put( "<xmlattr>.status", ToString(pSwitch->Get()) );
	else if( auto pThreeWaySwitch = dynamic_cast<const ThreeWaySwitch*>(&connector) )
		ptConnector.put( "<xmlattr>.status", ToString(pThreeWaySwitch->Get()) );
	else if( auto pSingleSlipSwitch = dynamic_cast<const SingleSlipSwitch*>(&connector) ){
		ptConnector.put( "<xmlattr>.status", ToString(pSingleSlipSwitch->Get()) );
		Frame<Length,One> center;
		pSingleSlipSwitch->GetCenter( center );
		ptConnector << center;
	}
	else if( auto pDoubleSlipSwitch = dynamic_cast<const DoubleSlipSwitch*>(&connector) ){
		ptConnector.put( "<xmlattr>.status", ToString(pDoubleSlipSwitch->Get()) );
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
			ptTrackEnd.put( "<xmlattr>.refid", trackEnd.first->ID() );
			ptTrackEnd.put( "<xmlattr>.type", ToString(trackEnd.second) );
		}
		else{
			ptTrackEnd.put( "<xmlattr>.refid", 0 );
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

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const VelocityControl& signal ){
	boost::property_tree::ptree ptSignal;
	ReferencesToAttributes( ptSignal, signal );
	ptSignal.put( "<xmlattr>.status", ToString( signal.Get() ) );
	ptSignal.put( "<xmlattr>.stopDistance", signal.StopDistance() );
	if( !signal.IsEmptyReference( "Key_Id" ) )
		ptSignal.put( "<xmlattr>.refid", signal.Reference( "Key_Id" ) );

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

			ptControlState.put( "<xmlattr>.status", ToString( status ) );
			ptControlState.put( "<xmlattr>.velocityMin", signal.VelocityMin( status ) );
			ptControlState.put( "<xmlattr>.velocityMax", signal.VelocityMax( status ) );
			ptControlState.put( "<xmlattr>.affectTravelVelocity", signal.AffectTravelVelocity( status ) );

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
	ReferencesToAttributes( ptSignal, signal );
	ptSignal.put( "<xmlattr>.status", ToString( signal.Get() ) );

	TrackRange trackRange;
	signal.GetTrackRange( trackRange );
	ptSignal << trackRange;

	boost::property_tree::ptree ptTarget;
	if( signal.IsTargetLocation() )
		ptTarget << signal.TargetLocation();
	//else if( auto pDepot = signal.TargetDepot() ){
	//	boost::property_tree::ptree ptDepot;
	//	ptDepot.put( "<xmlattr>.depotID", pDepot->ID() );
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

	ptIndicator.put( "<xmlattr>.id", indicator.ID() );
	ptIndicator.put( "<xmlattr>.refid", indicator.RefTargetID() );
	ReferencesToAttributes( ptIndicator, indicator );
	ptIndicator.put( "<xmlattr>.status", ToString( indicator.Get() ) );
	ptIndicator << indicator.GetFrame();

	for( Indicator::Status status = Indicator::Status::one; status < Indicator::Status::count
		 ; status = static_cast<Indicator::Status>(static_cast<int>(status) + 1))
	{
		try{
			if( indicator.IsValidState( status ) )			
				ptIndicator << indicator.LocalFrameForStatus( status );
		}
		catch( const std::logic_error& e ){
			std::cerr << "Indicator " << indicator.ID() << ": Local frame for status " << ToString( status ) << " is not valid. " 
				<< "Error: " << e.what() << std::endl;
		}
	}

	if( auto pPlugEnumerator = dynamic_cast<const PlugEnumerator*>(&indicator) )
		ptIndicator << *pPlugEnumerator;

	if( auto pJackEnumerator = dynamic_cast<const JackEnumerator*>(&indicator) )
		ptIndicator << *pJackEnumerator;

	if( auto pSignalAligned = dynamic_cast<const SignalAligned*>(&indicator) ){
		ptIndicator.put( "<xmlattr>.refid", pSignalAligned->GetSignalRef() );
		ptIndicator.put( "<xmlattr>.parameterOffset", pSignalAligned->ParameterOffset() );
		ptIndicator.put( "<xmlattr>.bPreserveUpDirection", pSignalAligned->PreserveUpDirection() );
	}

	if( auto pSwitchAligned = dynamic_cast<const SwitchAligned*>(&indicator) ){
		ptIndicator.put( "<xmlattr>.refid", pSwitchAligned->GetSwitchRef() );
		ptIndicator.put( "<xmlattr>.bPreserveUpDirection", pSwitchAligned->PreserveUpDirection() );
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

	ptSample.put( "<xmlattr>.s", sample.s );
	ptSample.put( "<xmlattr>.k", sample.k );
	ptSample.put( "<xmlattr>.t", sample.t );
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

	ptCurve.put( "<xmlattr>.k", curve.GetData().k );

	move_child( pt, curve.TypeName(), ptCurve );
	return pt;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const Helix & curve )
{
	boost::property_tree::ptree ptCurve;

	ptCurve.put( "<xmlattr>.k", curve.GetData().k );
	ptCurve.put( "<xmlattr>.t", curve.GetData().t );

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

	ptCurve.put( "<xmlattr>.a", curve.GetData().a );
	ptCurve.put( "<xmlattr>.b", curve.GetData().b );
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

	ptCurve.put( "<xmlattr>.a", curve.GetData().a );

	move_child( pt, curve.TypeName(), ptCurve );
	return pt;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const Rotator& curve ){
	boost::property_tree::ptree ptCurve;

	ptCurve.put( "<xmlattr>.a", curve.GetData().a );
	ptCurve.put( "<xmlattr>.b", curve.GetData().b );
	if( curve.GetData().a0 != 0_rad || curve.GetData().b0 != 0_rad ){
		ptCurve.put( "<xmlattr>.a0", curve.GetData().a0 );
		ptCurve.put( "<xmlattr>.b0", curve.GetData().b0 );
	}

	move_child( pt, curve.TypeName(), ptCurve );
	return pt;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const RotatorChain& curve ){
	boost::property_tree::ptree ptCurve;

	for( const auto& tuple : curve.GetData() ){
		boost::property_tree::ptree ptLink;

		ptLink.put( "<xmlattr>.a", std::get<0>(tuple) );
		ptLink.put( "<xmlattr>.b", std::get<1>(tuple) );
		ptLink.put( "<xmlattr>.length", std::get<2>(tuple) );

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

	ptTwist.put( "<xmlattr>.angle", twist.TwistValue() );

	move_child( pt, twist.TypeName(), ptTwist );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const LinearTwist& twist ){
	boost::property_tree::ptree ptTwist;

	ptTwist.put( "<xmlattr>.startangle", twist.From() );
	ptTwist.put( "<xmlattr>.endangle", twist.To() );

	move_child( pt, twist.TypeName(), ptTwist );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const PiecewiseTwist& twist ){
	boost::property_tree::ptree ptTwist;

	for( int idx = 0; idx < twist.CntTwistValues(); ++idx ){
		boost::property_tree::ptree ptTwistAngle;

		ptTwistAngle.put( "<xmlattr>.s", twist.Twist(idx).first );
		ptTwistAngle.put( "<xmlattr>.value", twist.Twist(idx).second );

		ptTwist.add_child( "TwistAngle", ptTwistAngle );
	}

	move_child( pt, twist.TypeName(), ptTwist );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const DirectionalTwist& twist ){
	boost::property_tree::ptree ptTwist;
	
	if( twist.IsFrozen() )
		ptTwist.put( "<xmlattr>.bFrozen", true );

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
		ReferencesToAttributes( ptPlug, plug );
		ptPlug.put( "<xmlattr>.plugid", plug.ID() );

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
			ReferencesToAttributes( ptJack, jack );
			ptJack.put( "<xmlattr>.plugid", pPlug->ID() );

			move_child( pt, "Jack", ptJack );
		}
	}

	return pt;
}

boost::property_tree::ptree & operator<<( boost::property_tree::ptree& pt, const PulseCounter& pulseCounter ){
	boost::property_tree::ptree ptPulseCounter;
	ReferencesToAttributes( ptPulseCounter, pulseCounter );
	ptPulseCounter.put( "<xmlattr>.threshold", pulseCounter.Threshold() );
	ptPulseCounter.put( "<xmlattr>.counter", pulseCounter.Counter() );

	if( auto pPlugEnumerator = dynamic_cast<const PlugEnumerator*>(&pulseCounter) )
		ptPulseCounter << *pPlugEnumerator;

	if( auto pJackEnumerator = dynamic_cast<const JackEnumerator*>(&pulseCounter) )
		ptPulseCounter << *pJackEnumerator;

	move_child( pt, pulseCounter.TypeName(), ptPulseCounter );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Timer& timer ){
	boost::property_tree::ptree ptTimer;
	ReferencesToAttributes( ptTimer, timer );
	ptTimer.put( "<xmlattr>.lapse", timer.Lapse() );
	ptTimer.put( "<xmlattr>.periodic", timer.Periodic() );
	ptTimer.put( "<xmlattr>.started", timer.IsStarted() );
	ptTimer.put( "<xmlattr>.time", timer.GetTime() );

	if( auto pPlugEnumerator = dynamic_cast<const PlugEnumerator*>(&timer) )
		ptTimer << *pPlugEnumerator;

	if( auto pJackEnumerator = dynamic_cast<const JackEnumerator*>(&timer) )
		ptTimer << *pJackEnumerator;

	move_child( pt, timer.TypeName(), ptTimer );
	return pt;
}
///////////////////////////////////////
boost::property_tree::ptree& operator << ( boost::property_tree::ptree& opt, const spat::Vector<Velocity>& v ){
	boost::property_tree::ptree ptVector;
	ptVector.add( "<xmlattr>.dx", v.dx );
	ptVector.add( "<xmlattr>.dy", v.dy );
	ptVector.add( "<xmlattr>.dz", v.dz );
	opt.add_child( "Velocity", ptVector );
	return opt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& opt, const spat::Vector<AngularVelocity>& v ){
	boost::property_tree::ptree ptVector;
	ptVector.add( "<xmlattr>.dx", v.dx );
	ptVector.add( "<xmlattr>.dy", v.dy );
	ptVector.add( "<xmlattr>.dz", v.dz );
	opt.add_child( "AngularVelocity", ptVector );
	return opt;
}
///////////////////////////////////////
}
}
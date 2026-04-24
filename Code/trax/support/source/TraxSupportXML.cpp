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

#include "trax/support/TraxSupportXML.h"
#include "trax/support/TraxSupportStream.h"

#include "common/Interval.h"

#include "trax/Indicator.h"
#include "trax/Location.h"
#include "trax/LogicElements.h"
#include "trax/Section.h"
#include "trax/SectionTrack.h"
#include "trax/Sensor.h"
#include "trax/Signal.h"
#include "trax/Switch.h"
#include "trax/Timer.h"
#include "trax/TrackLocation.h"
#include "trax/Plug.h"

namespace trax{

namespace ptreesupport{
	using common::ptreesupport::operator<<;
	using namespace dim;
	using namespace dim::ptreesupport;
	using namespace spat;
	using namespace spat::ptreesupport;

///////////////////////////////////////
boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Track::End& end ){
	boost::property_tree::ptree ptTrackEnd;

	ptTrackEnd.put( "<xmlattr>.refid", end.id );
	ptTrackEnd.put( "<xmlattr>.type", end.type == EndType::north ? "front" : "end" );

	move_child( pt, "TrackEnd", ptTrackEnd );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const TrackBuilder& track ){
	boost::property_tree::ptree ptTrack;

	ptTrack.put( "<xmlattr>.id", track.ID() );
	ReferencesToAttributes( ptTrack, track );
	ptTrack.put( "<xmlattr>.type", track.Reference( "type" ).empty() ? ToString( track.GetTrackType() ) : track.Reference( "type" ) );

	{
		boost::property_tree::ptree ptBegin;
		if( !track.Reference( "electrificationShiftBegin" ).empty() )
			ptBegin.put( "<xmlattr>.electrificationShift", track.Reference( "electrificationShiftBegin" ) );

		const Track::End frontend = track.TransitionEnd( EndType::north );
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
			ptEnd.put( "<xmlattr>.electrificationShift", track.Reference( "electrificationShiftEnd" ) );

		const Track::End backend = track.TransitionEnd( EndType::south );
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

	if( const SectionTrack* pSectionTrack = dynamic_cast<const SectionTrack*>(&track); pSectionTrack )
		if( auto pSection = pSectionTrack->GetSection() )
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

	ptTrackLocation.put( "<xmlattr>.parameter", tlr.parameter );
	ptTrackLocation.put( "<xmlattr>.orientation", tlr.orientation == Orientation::Value::para ? "para" : "anti" );

	move_child( pt, "TrackLocation", ptTrackLocation );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const TrackLocationRef& tlr ){
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
	if( !connector.Reference( "kollektorID" ).empty() )
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
	if( !signal.Reference( "Key_Id" ).empty() )
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
static std::unique_ptr<Curve> CreateCurve( const boost::property_tree::ptree& pt )
{
	for( const auto& pair : pt )
	{
		if( pair.first == "BasicLine" )
		{
			if( std::unique_ptr<Line> pLine = Line::Make(); pLine )
			{
				pair.second >> *pLine;
				return pLine;
			}
		}

		else if( pair.first == "Line" )
		{
			if( std::unique_ptr<LineP> pLineP = LineP::Make(); pLineP )
			{
				pair.second >> *pLineP;
				return pLineP;
			}
		}

		else if( pair.first == "BasicArc" )
		{
			if( std::unique_ptr<Arc> pArc = Arc::Make(); pArc )
			{
				pair.second >> *pArc;
				return pArc;
			}
		}

		else if( pair.first == "Arc" )
		{
			if( std::unique_ptr<ArcP> pArcP = ArcP::Make(); pArcP )
			{
				pair.second >> *pArcP;
				return pArcP;
			}
		}

		else if( pair.first == "BasicHelix" )
		{
			if( std::unique_ptr<Helix> pHelix = Helix::Make(); pHelix )
			{
				pair.second >> *pHelix;
				return pHelix;
			}
		}

		else if( pair.first == "Helix" )
		{
			if( std::unique_ptr<HelixP> pHelixP = HelixP::Make(); pHelixP )
			{
				pair.second >> *pHelixP;
				return pHelixP;
			}
		}

		else if( pair.first == "Cubic" )
		{
			if( std::unique_ptr<Cubic> pCubic = Cubic::Make(); pCubic )
			{
				pair.second >> *pCubic;
				return pCubic;
			}
		}

		else if( pair.first == "Spline" )
		{
			if( std::unique_ptr<Spline> pSpline = Spline::Make(); pSpline )
			{
				pair.second >> *pSpline;
				return pSpline;
			}
		}

		else if( pair.first == "Clothoid" )
		{
			if( std::unique_ptr<Clothoid> pClothoid = Clothoid::Make(); pClothoid )
			{
				pair.second >> *pClothoid;
				return pClothoid;
			}
		}

		else if( pair.first == "Rotator" )
		{
			Curve::CurveType type = (pt.get( "<xmlattr>.a0", 0_1Im ) == 0_1Im && pt.get( "<xmlattr>.b0", 0_1Im ) == 0_1Im) ? 
				Curve::CurveType::Rotator : Curve::CurveType::RotatorWithOffset;

			if( std::unique_ptr<Rotator> pRotator = Rotator::Make( type ); pRotator )
			{
				pair.second >> *pRotator;
				return pRotator;
			}
		}

		else if( pair.first == "RotatorChain" )
		{
			if( std::unique_ptr<RotatorChain> pRotatorChain = RotatorChain::Make(); pRotatorChain )
			{
				pair.second >> *pRotatorChain;
				return pRotatorChain;
			}
		}

		else if( pair.first == "PolygonalChain" )
		{
			if( std::unique_ptr<PolygonalChain> pPolygonalChain = PolygonalChain::Make(); pPolygonalChain )
			{
				pair.second >> *pPolygonalChain;
				return pPolygonalChain;
			}
		}

		else if( pair.first == "SampledCurve" )
		{
			if( std::unique_ptr<SampledCurve> pSampledCurve = SampledCurve::Make(); pSampledCurve )
			{
				pair.second >> *pSampledCurve;
				return pSampledCurve;
			}
		}
	}

	return nullptr;
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

	section.Shift( Vector2D<Length>{ get( pt, "<xmlattr>.shiftX", 0_m, _m ), get( pt, "<xmlattr>.shiftY", 0_m, _m ) } );
	section.Scale( get( pt, "<xmlattr>.scaleX", 1_1, _1 ), get( pt, "<xmlattr>.scaleY", 1_1, _1 ) );
	return pt;
}

const boost::property_tree::ptree& operator>>( const boost::property_tree::ptree& pt, TrackBuilder& track )
{
	track.ID( pt.get( "<xmlattr>.id", IDType{0} ) );
	AttributesToReferences( pt, track );
	common::Interval<Length> range{ 0_m, get( pt, "<xmlattr>.length", 10_m, _m ) };

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

void ReadConnection( const boost::property_tree::ptree& pt, Track::End& trackend ){
	for( const auto& pair : pt )
	{
		if( pair.first == "TrackEnd" )
			ReadTrackEnd( pair.second, trackend );
	}
}

void ReadTrackEnd( const boost::property_tree::ptree& pt, Track::End& trackend ){
	trackend.id		= pt.get( "<xmlattr>.refid", IDType{0} );
	trackend.type	= ToEndType( pt.get( "<xmlattr>.type", "front" ) );
}

void ReadTrackLocation( const boost::property_tree::ptree& pt, TrackLocation& trackLocation ) noexcept{
	trackLocation.parameter		= get( pt, "<xmlattr>.parameter", 0_m, _m );
	trackLocation.orientation	= pt.get( "<xmlattr>.orientation", "para" ) == "anti" ? Orientation::Value::anti : Orientation::Value::para;
}

void ReadTrackLocationRef( const boost::property_tree::ptree& pt, TrackLocationRef& trackLocationRef ) noexcept{
	ReadTrackLocation( pt, trackLocationRef.location );
	trackLocationRef.refid = pt.get( "<xmlattr>.refid", IDType{0} );
}

void ReadTrackRange( const boost::property_tree::ptree& pt, TrackRange& trackRange ) noexcept{
	trackRange.range.Near( get( pt, "<xmlattr>.near", 0_m, _m ) );
	trackRange.range.Far( get( pt, "<xmlattr>.far", 0_m, _m ) );
	trackRange.refid = pt.get( "<xmlattr>.refid", IDType{0} );
}

void Read( const boost::property_tree::ptree& pt, Cubic::Data& data ){
	std::vector<spat::Vector<Length>> vectors;
	vectors.reserve( 3 );

	for( const auto& pair : pt )
	{
		if( pair.first == "Position" )
			ReadPosition( pair.second, data.a );

		else if( pair.first == "Vector" ){
			vectors.push_back( {} );
			ReadVector( pair.second, vectors.back() );
		}
	}

	if( vectors.size() > 0 )
		data.b = vectors.at(0);

	if( vectors.size() > 1 )
		data.c = vectors.at(1);

	if( vectors.size() > 2 )
		data.d = vectors.at(2);
}

void Read( const boost::property_tree::ptree& pt, CurveSample& sample ){
	sample.s = pt.get( "<xmlattr>.s", sample.s );
	sample.k = pt.get( "<xmlattr>.k", sample.k );
	sample.t = pt.get( "<xmlattr>.t", sample.t );

	for( const auto& pair : pt ){
		if( pair.first == "Frame" )
			ReadFrame( pair.second, sample.F );
	}
}

void Read( const boost::property_tree::ptree& pt, EEPCurve::Data& data ){
	data.gc_Kruemmung = pt.get( "<xmlattr>.Kruemmung", data.gc_Kruemmung );
	data.gc_Windung = pt.get( "<xmlattr>.Torsion", data.gc_Windung );
	data.gc_Verdrillung = pt.get( "<xmlattr>.Fuehrungsverdrehung", data.gc_Verdrillung );
	data.gc_Laenge = pt.get( "<xmlattr>.Laenge", data.gc_Laenge );
	data.gc_Kurve = pt.get( "<xmlattr>.Kurve", data.gc_Kurve );
	data.m_FuehrungsVerdrehung = pt.get( "<xmlattr>.Anfangsfuehrungsverdrehung", data.m_FuehrungsVerdrehung );

	for( const auto& pair : pt )
	{
		if( pair.first == "Frame" )
			ReadFrame( pair.second, data.m_AnfangsBein );
	}
}

} // namespace ptreesupport
} // namespace trax
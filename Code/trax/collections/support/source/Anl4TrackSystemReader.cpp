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

#include "trax/collections/support/Anl4TrackSystemReader.h"

#include "trax/collections/ConnectorCollection.h"
#include "trax/collections/IndicatorCollection.h"
#include "trax/collections/PulseCounterCollection.h"
#include "trax/collections/SignalCollection.h"
#include "trax/collections/TimerCollection.h"
#include "trax/collections/TrackSystem.h"
#include "trax/collections/TrackCollectionContainer.h"

#include "trax/SectionTrack.h"
#include "trax/Jack.h"
#include "trax/Plug.h"
#include "trax/Section.h"
#include "trax/Sensor.h"
#include "trax/SocketRegistry.h"
#include "trax/Switch.h"

#include "trax/support/TraxSupportXML.h"

#include <iostream>

namespace trax{
namespace ptreesupport{

using namespace common;
using namespace common::ptreesupport;
using namespace dim::ptreesupport;
using namespace spat;
using namespace spat::ptreesupport;


Anl4TrackSystemReader::Anl4TrackSystemReader( const char* pLocale )
	: PTreeReader{ pLocale }
{
}

Anl4TrackSystemReader::Anl4TrackSystemReader(
	SocketRegistry& socketRegistry, 
	const char* pLocale )
	: PTreeReader{ socketRegistry, pLocale }
{
}

std::shared_ptr<TrackSystem> Anl4TrackSystemReader::ReadTrackSystem(
	const boost::property_tree::ptree& pt ) const
{
	return CreateTrackSystem( pt );
}

std::shared_ptr<TrackSystem> Anl4TrackSystemReader::CreateTrackSystem( 
	const boost::property_tree::ptree& pt ) const
{
	if( std::shared_ptr<TrackSystem> pTrackSystem{ TrackSystem::Make( TrackCollectionContainer::Make() ) }; pTrackSystem )
	{
		pTrackSystem->ID( pt.get( "<xmlattr>.id", 0 ) );
		pTrackSystem->Reference( "Name", pt.get( "<xmlattr>.name", "TrackSystem" + to_string(pTrackSystem->ID()) ) );

		std::vector<Track::Coupling> couplings;
		for( const auto& pair : pt )
		{
			if( pair.first == "TrackCollection" ){
				if( auto pTrackCollectionContainer = pTrackSystem->GetCollectionContainer() )
					pTrackCollectionContainer->Add( CreateTrackCollection( pair.second, couplings ) );
			}

			else if( pair.first == "ConnectorCollection" )
				pTrackSystem->SetConnectorCollection( CreateConnectorCollection( pair.second, *pTrackSystem ) );
		}

		// contract doublettes:
		for( auto i = couplings.begin(); i != couplings.end(); ++i ){
			for( auto j = i+1; j != couplings.end(); ++j ){
				if( (*i).theOther == (*j).theOne &&
					(*i).theOne == (*j).theOther )
				{
					(*i).theOther.type = (*j).theOne.type;
					couplings.erase( j );
					break;
				}
			}
		}

		for( const Track::Coupling& coupling : couplings )
			pTrackSystem->Couple( coupling, true );

		if( const IDType activeTrackID = pt.get( "<xmlattr>.activeTrack", 0 ) )
			pTrackSystem->PushActive( activeTrackID );
		
		return pTrackSystem;
	}

	return nullptr;
}

std::shared_ptr<TrackCollection> Anl4TrackSystemReader::CreateTrackCollection( 
	const boost::property_tree::ptree& pt, 
	std::vector<Track::Coupling>& couplings ) const
{
	if( std::unique_ptr<TrackCollection> pTrackCollection = TrackCollection::Make(); pTrackCollection )
	{
		pTrackCollection->ID( pt.get( "<xmlattr>.id", 0 ) );
		pTrackCollection->Reference( "Name", pt.get( "<xmlattr>.name", "TrackCollection" + to_string(pTrackCollection->ID()) ) );

		for( const auto& pair : pt )
		{
			if( pair.first == "Frame" ){
				spat::Frame<Length,One> frame;
				ReadFrame( pair.second, frame );
				pTrackCollection->SetFrame( frame );
			}

			else if( pair.first == "Track" )
				pTrackCollection->Add( CreateTrack( pair.second, couplings ) );
		}

		return pTrackCollection;
	}

	return nullptr;
}

std::shared_ptr<TrackBuilder> Anl4TrackSystemReader::CreateTrack(
	const boost::property_tree::ptree& pt, 
	std::vector<Track::Coupling>& couplings ) const
{
	if( auto pTrack = TrackBuilder::Make( TrackType( pt.get( "<xmlattr>.type", "standard" ) ) ); pTrack )
	{
		pTrack->ID( pt.get( "<xmlattr>.id", 0 ) );
		pTrack->Reference( "name", pt.get( "<xmlattr>.name", "Track" + to_string(pTrack->ID()) ) );
		pTrack->Reference( "reference", pt.get( "<xmlattr>.reference", "" ) );
		common::Interval<Length> range{0_m,get( pt, "<xmlattr>.length", 10_m, _m )};

		for( const auto& pair : pt )
		{
			if( pair.first == "Frame" ){
				spat::Frame<Length,One> frame;
				ReadFrame( pair.second, frame );
				pTrack->SetFrame( frame );
			}

			else if( pair.first == "Interval" )
				ReadInterval( pair.second, range );

			else if( pair.first == "Curve" ){
				pTrack->Attach( CreateCurve( pair.second ), range );
			}

			else if( pair.first == "Twist" ){
				pTrack->Attach( CreateTwist( pair.second ) );
			}

			else if( pair.first == "Section" ){
				ReadSection( pair.second, *pTrack );
			}

			else if( pair.first == "Begin" ){
				ReadEnd( pair.second, *pTrack, Track::EndType::front, couplings );
			}

			else if( pair.first == "End" ){
				ReadEnd( pair.second, *pTrack, Track::EndType::end, couplings );
			}

			else if( pair.first == "Sensor" ){
				TrackLocation location;
				std::shared_ptr<SensorFilterJack> pSensor = CreateSensor( pair.second, location );
				pTrack->Attach( pSensor, location );
			}

			else if( pair.first == "VelocitySensor" ){
				TrackLocation location;
				std::shared_ptr<SensorFilterJack> pSensor = CreateVelocitySensor( pair.second, location );
				pTrack->Attach( pSensor, location );
			}

			else if( pair.first == "WeighSensor" ){
				TrackLocation location;
				std::shared_ptr<SensorFilterJack> pSensor = CreateWeighSensor( pair.second, location );
				pTrack->Attach( pSensor, location );
			}

			else if( pair.first == "TractionSensor" ){
				TrackLocation location;
				std::shared_ptr<SensorFilterJack> pSensor = CreateTractionSensor( pair.second, location );
				pTrack->Attach( pSensor, location );
			}		
		}

		return pTrack;
	}

	return nullptr;
}

std::unique_ptr<ConnectorCollection> Anl4TrackSystemReader::CreateConnectorCollection( 
	const boost::property_tree::ptree& pt, 
	const TrackSystem& trackSystem ) const
{
	if( std::unique_ptr<ConnectorCollection> pConnectorCollection = ConnectorCollection::Make(); pConnectorCollection )
	{
		for( const auto& pair : pt )
		{
			if( pair.first == "Switch" )
				pConnectorCollection->Add( CreateSwitch( pair.second, trackSystem ) );

			else if( pair.first == "ThreeWaySwitch" )
				pConnectorCollection->Add( CreateThreeWaySwitch( pair.second, trackSystem ) );

			else if( pair.first == "SingleSlipSwitch" )
				pConnectorCollection->Add( CreateSingleSlipSwitch( pair.second, trackSystem ) );

			else if( pair.first == "DoubleSlipSwitch" )
				pConnectorCollection->Add( CreateDoubleSlipSwitch( pair.second, trackSystem ) );
		}
		
		return pConnectorCollection;
	}

	return nullptr;
}

std::unique_ptr<Switch> Anl4TrackSystemReader::CreateSwitch( 
	const boost::property_tree::ptree& pt, 
	const TrackSystem& trackSystem ) const
{
	if( std::unique_ptr<Switch> pSwitch = Switch::Make(); pSwitch )
	{
		pSwitch->Reference( "name", pt.get( "<xmlattr>.name", "" ) );
		pSwitch->ID( pt.get( "<xmlattr>.id", 0 ) );
		pSwitch->Set( ToNarrowSwitchStatus( pt.get( "<xmlattr>.status", "none" ) ), false );

		int slot = 0;
		for( const auto& pair : pt )
		{
			if( pair.first == "TrackEnd" ){
				Track::End end;
				ReadTrackEnd( pair.second, end );
				if( auto pTrack = trackSystem.Get( end.id ) )
					pSwitch->Slot( slot, pTrack, end.type );

				++slot;
			}

			else if( pair.first == "Plug" )
				ReadPlug( pair.second, pSwitch->PlugTo( NarrowSwitchStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) ) );

			else if( pair.first == "Jack" )
				ReadJack( pair.second, pSwitch->JackOn( NarrowSwitchStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) ) );
		}

        if( !pSwitch->Check(std::cerr,10_cm) )
            std::cerr << "trax: a switch's slots are not fully populated or show gaps or kinks or twists: " << pSwitch->ID() << std::endl;

		return pSwitch;
	}

	return nullptr;
}

std::unique_ptr<ThreeWaySwitch> Anl4TrackSystemReader::CreateThreeWaySwitch(
	const boost::property_tree::ptree& pt,
	const TrackSystem& trackSystem ) const
{
	if( std::unique_ptr<ThreeWaySwitch> pSwitch = ThreeWaySwitch::Make(); pSwitch )
	{
		pSwitch->Reference( "name", pt.get( "<xmlattr>.name", "" ) );
		pSwitch->ID( pt.get( "<xmlattr>.id", 0 ) );
		pSwitch->Set( ToNarrowSwitchStatus( pt.get( "<xmlattr>.status", "none" ) ), false );

		int slot = 0;
		for( const auto& pair : pt )
		{
			if( pair.first == "TrackEnd" ){
				Track::End end;
				ReadTrackEnd( pair.second, end );
				if( auto pTrack = trackSystem.Get( end.id ) )
					pSwitch->Slot( slot, pTrack, end.type );

				++slot;
			}

			else if( pair.first == "Plug" )
				ReadPlug( pair.second, pSwitch->PlugTo( NarrowSwitchStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) ) );

			else if( pair.first == "Jack" )
				ReadJack( pair.second, pSwitch->JackOn( NarrowSwitchStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) ) );
		}

        if( !pSwitch->Check(std::cerr,10_cm) )
            std::cerr << "trax: a three way switch's slots are not fully populated or show gaps or kinks or twists: " << pSwitch->ID() << std::endl;

		return pSwitch;
	}

	return nullptr;
}

std::unique_ptr<SingleSlipSwitch> Anl4TrackSystemReader::CreateSingleSlipSwitch(
	const boost::property_tree::ptree& pt,
	const TrackSystem& trackSystem ) const
{
	if( std::unique_ptr<SingleSlipSwitch> pSwitch = SingleSlipSwitch::Make(); pSwitch )
	{
		pSwitch->Reference( "name", pt.get( "<xmlattr>.name", "" ) );
		pSwitch->ID( pt.get( "<xmlattr>.id", 0 ) );
		pSwitch->Set( ToSingleSlipSwitchStatus( pt.get( "<xmlattr>.status", "none" ) ), false );

		int slot = 0;
		for( const auto& pair : pt )
		{
			if( pair.first == "Frame" ){
				Frame<Length,One> frame;
				ReadFrame( pair.second, frame );
				pSwitch->SetCenter( frame );
			}

			else if( pair.first == "TrackEnd" ){
				Track::End end;
				ReadTrackEnd( pair.second, end );
				if( auto pTrack = trackSystem.Get( end.id ) )
					pSwitch->Slot( slot, pTrack, end.type );

				++slot;
			}

			else if( pair.first == "Plug" )
				ReadPlug( pair.second, pSwitch->PlugTo( SingleSlipSwitchStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) ) );

			else if( pair.first == "Jack" )
				ReadJack( pair.second, pSwitch->JackOn( SingleSlipSwitchStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) ) );
		}

        if( !pSwitch->Check(std::cerr,10_cm) )
            std::cerr << "trax: a single slip switch's slots are not fully populated or show gaps or kinks or twists: " << pSwitch->ID() << std::endl;

		return pSwitch;
	}

	return nullptr;
}

std::unique_ptr<DoubleSlipSwitch> Anl4TrackSystemReader::CreateDoubleSlipSwitch(
	const boost::property_tree::ptree& pt,
	const TrackSystem& trackSystem ) const
{
	if( std::unique_ptr<DoubleSlipSwitch> pSwitch = DoubleSlipSwitch::Make(); pSwitch )
	{
		pSwitch->Reference( "name", pt.get( "<xmlattr>.name", "" ) );
		pSwitch->ID( pt.get( "<xmlattr>.id", 0 ) );
		pSwitch->Set( ToDoubleSlipSwitchStatus( pt.get( "<xmlattr>.status", "none" ) ), false );

		int slot = 0;
		for( const auto& pair : pt )
		{
			if( pair.first == "Frame" ){
				Frame<Length,One> frame;
				ReadFrame( pair.second, frame );
				pSwitch->SetCenter( frame );
			}

			else if( pair.first == "TrackEnd" ){
				Track::End end;
				ReadTrackEnd( pair.second, end );
				if( auto pTrack = trackSystem.Get( end.id ) )
					pSwitch->Slot( slot, pTrack, end.type );

				++slot;
			}

			else if( pair.first == "Plug" )
				ReadPlug( pair.second, pSwitch->PlugTo( DoubleSlipSwitchStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) ) );

			else if( pair.first == "Jack" )
				ReadJack( pair.second, pSwitch->JackOn( DoubleSlipSwitchStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) ) );
		}

        if( !pSwitch->Check(std::cerr,10_cm) )
            std::cerr << "trax: a double slip switch's slots are not fully populated or show gaps or kinks or twists: " << pSwitch->ID() << std::endl;

		return pSwitch;
	}

	return nullptr;
}

std::unique_ptr<Location> Anl4TrackSystemReader::CreateLocation( 
	const boost::property_tree::ptree& pt, 
	const TrackSystem& trackSystem ) const
{
	if( std::unique_ptr<Location> pLocation = std::make_unique<Location>(); pLocation ){
		TrackLocationRef trackLocation;
		ReadTrackLocationRef( pt, trackLocation );
		pLocation->PutOn( trackSystem.Get( trackLocation.refid ), trackLocation.location );
		return pLocation;
	}

	return nullptr;
}

std::unique_ptr<IndicatorCollection> Anl4TrackSystemReader::CreateIndicatorCollection( 
	const boost::property_tree::ptree& pt, 
	const ConnectorCollection& connectorCollection,
	const SignalCollection& signalCollection ) const
{
	if( std::unique_ptr<IndicatorCollection> pIndicatorCollection = IndicatorCollection::Make(); 
		pIndicatorCollection )
	{
		for( const auto& pair : pt )
		{
			if( pair.first == "SwitchSemaphore" )
				pIndicatorCollection->Add( CreateSwitchSemaphore( pair.second, connectorCollection ) );

			if( pair.first == "SwitchMultiSemaphore" )
				pIndicatorCollection->Add( CreateSwitchMultiSemaphore( pair.second, connectorCollection ) );
		
			else if( pair.first == "VelocityControlSemaphore" )
				pIndicatorCollection->Add( CreateVelocityControlSemaphore( pair.second, signalCollection ) );
		}

		return pIndicatorCollection;
	}

	return nullptr;
}

std::unique_ptr<BinaryIndicator> Anl4TrackSystemReader::CreateSwitchSemaphore( 
	const boost::property_tree::ptree& pt,
	const ConnectorCollection& connectorCollection ) const
{
	if( auto pSwitchSemaphore = BinaryIndicator::Make( Indicator::Type::switch_mono ); 
		pSwitchSemaphore )
	{
		ReadIndicator( pt, *pSwitchSemaphore, connectorCollection );

		return pSwitchSemaphore;
	}

	return nullptr;
}

std::unique_ptr<Indicator> Anl4TrackSystemReader::CreateSwitchMultiSemaphore(
	const boost::property_tree::ptree& pt,
	const ConnectorCollection& connectorCollection ) const
{
	if( auto pSwitchMultiSemaphore = Indicator::Make( Indicator::Type::switch_multi ); pSwitchMultiSemaphore )
	{
		ReadIndicator( pt, *pSwitchMultiSemaphore, connectorCollection );

		return pSwitchMultiSemaphore;
	}

	return nullptr;
}

std::unique_ptr<Indicator> Anl4TrackSystemReader::CreateVelocityControlSemaphore( 
	const boost::property_tree::ptree& pt, 
	const SignalCollection& signalCollection ) const
{
	if( std::unique_ptr<Indicator> pVelocityControlSemaphore = Indicator::Make( Indicator::Type::velocity_control ); 
		pVelocityControlSemaphore )
	{
		pVelocityControlSemaphore->Reference( "name", pt.get( "<xmlattr>.name", "" ) );
		pVelocityControlSemaphore->Reference( "reference", pt.get( "<xmlattr>.reference", "" ) );
		pVelocityControlSemaphore->ID( pt.get( "<xmlattr>.id", 0 ) );
		const IDType target = pt.get( "<xmlattr>.refid", 0 );

		for( const auto& pair : pt )
		{
			if( pair.first == "Frame" ){
				Frame<Length,One> frame;
				ReadFrame( pair.second, frame );
				pVelocityControlSemaphore->SetFrame( frame );
			}

			else if( pair.first == "Plug" )
				ReadPlug( pair.second, pVelocityControlSemaphore->PlugTo( IndicatorStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) ) );

			else if( pair.first == "Jack" )
				ReadJack( pair.second, pVelocityControlSemaphore->JackOn( IndicatorStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) ) );
		}

		pVelocityControlSemaphore->Set( ToIndicatorStatus( pt.get( "<xmlattr>.status", "none" ) ), false );

		if( auto pSignal = signalCollection.Get( target ) ){
			if( auto pAligned = dynamic_cast<SignalAligned*>(pVelocityControlSemaphore.get()) ){
				pAligned->PreserveUpDirection( pt.get( "<xmlattr>.bPreserveUpDirection", false ) );
				pAligned->Attach( pSignal, get( pt, "<xmlattr>.parameterOffset", 0_m, _m ) );
			}
		}

		return pVelocityControlSemaphore;
	}

	return nullptr;
}

std::unique_ptr<SensorFilterJack> Anl4TrackSystemReader::CreateSensor( 
	const boost::property_tree::ptree& pt, 
	TrackLocation& trackLocation ) const
{
	if( std::unique_ptr<SensorFilterJack> pSensor = SensorFilterJack::Make(); pSensor )
	{
		ReadSensor( pt, *pSensor, trackLocation );
		return pSensor;
	}

	return nullptr;
}

std::unique_ptr<VelocitySensor> Anl4TrackSystemReader::CreateVelocitySensor( 
	const boost::property_tree::ptree& pt, 
	TrackLocation& trackLocation ) const
{
	if( std::unique_ptr<VelocitySensor> pSensor = VelocitySensor::Make(); pSensor )
	{
		ReadSensor( pt, *pSensor, trackLocation );

		pSensor->SetVelocity(
			get( pt, "<xmlattr>.minVelocity", 0_mIs, _mIs ),
			get( pt, "<xmlattr>.maxVelocity", 0_mIs, _mIs ) );
		pSensor->TriggerInside( pt.get( "<xmlattr>.triggerInside", true ) );

		return pSensor;
	}

	return nullptr;
}

std::unique_ptr<WeighSensor> Anl4TrackSystemReader::CreateWeighSensor(
	const boost::property_tree::ptree& pt,
	TrackLocation & trackLocation) const
{
	if( std::unique_ptr<WeighSensor> pSensor = WeighSensor::Make(); pSensor )
	{
		ReadSensor( pt, *pSensor, trackLocation );

		pSensor->Weight(
			get( pt, "<xmlattr>.minWeight", 0_t, _t ),
			get( pt, "<xmlattr>.maxWeight", 0_t, _t ) );
		pSensor->TriggerInside( pt.get( "<xmlattr>.triggerInside", true ) );
		pSensor->WeighTrain( pt.get( "<xmlattr>.weighTrain", true ) );

		return pSensor;
	}

	return nullptr;
}

std::unique_ptr<IntervalSensor> Anl4TrackSystemReader::CreateTractionSensor(
	const boost::property_tree::ptree& /*pt*/,
	TrackLocation& /*trackLocation*/ ) const
{
	//if( auto pSensor = TractionSensor::Make() )
	//{
	//	ReadSensor( pt, *pSensor, trackLocation );

	//	pSensor->Value(
	//		pt.get( "<xmlattr>.minTraction", 0.0f ),
	//		pt.get( "<xmlattr>.maxTraction", 0.0f ) );
	//	pSensor->TriggerInside( pt.get( "<xmlattr>.triggerInside", true ) );

	//	return pSensor;
	//}

	return nullptr;
}

std::unique_ptr<SignalCollection> Anl4TrackSystemReader::CreateSignalCollection( 
	const boost::property_tree::ptree& pt, 
	const TrackSystem& trackSystem/*, 
	const Fleet& fleet*/ ) const
{
	if( std::unique_ptr<SignalCollection> pSignalCollection = SignalCollection::Make(); pSignalCollection )
	{
		for( const auto& pair : pt )
		{
			if( pair.first == "VelocityControl" )
				pSignalCollection->Add( CreateVelocityControl( pair.second, trackSystem ) );

			else if( pair.first == "JumpSignal" )
				pSignalCollection->Add( CreateJumpSignal( pair.second, trackSystem/*, fleet*/ ) );
		}

		return pSignalCollection;
	}

	return nullptr;
}

std::shared_ptr<VelocityControl> Anl4TrackSystemReader::CreateVelocityControl( 
	const boost::property_tree::ptree& pt, 
	const TrackSystem& trackSystem ) const
{
	if( std::shared_ptr<VelocityControl> pSignal = VelocityControl::Make(); pSignal )
	{
		for( const auto& pair : pt )
		{
			if( pair.first == "ControlState" ){
				pSignal->SetVelocity( 
					ToSignalStatus( pair.second.get( "<xmlattr>.status", "none" ) ),
					get( pair.second, "<xmlattr>.velocityMin", 0_mIs, _mIs ),
					get( pair.second, "<xmlattr>.velocityMax", 0_mIs, _mIs ) );

				pSignal->AffectTravelVelocity( 
					ToSignalStatus( pair.second.get( "<xmlattr>.status", "none" ) ),
					pair.second.get( "<xmlattr>.affectTravelVelocity", false ) );
			}
		}

		pSignal->StopDistance( get( pt, "<xmlattr>.stopDistance", 0_m, _m ) );
		ReadSignal( pt, pSignal, trackSystem );
		return pSignal;
	}

	return nullptr;
}

std::shared_ptr<JumpSite> Anl4TrackSystemReader::CreateJumpSignal(
	const boost::property_tree::ptree& pt, 
	const TrackSystem& trackSystem/*, 
	const Fleet& fleet*/ ) const
{
	if( std::shared_ptr<JumpSite> pSignal = JumpSite::Make(); pSignal )
	{
		for( const auto& pair : pt )
		{
			if( pair.first == "Target" )
				ReadJumpSignalTarget( pair.second, pSignal, trackSystem/*, fleet*/ );
		}

		ReadSignal( pt, pSignal, trackSystem );
		return pSignal;
	}

	return nullptr;
}

std::unique_ptr<PulseCounterCollection> Anl4TrackSystemReader::CreatePulseCounterCollection( 
	const boost::property_tree::ptree& pt ) const
{
	if( std::unique_ptr<PulseCounterCollection> pPulseCounterCollection = PulseCounterCollection::Make(); pPulseCounterCollection )
	{
		for( const auto& pair : pt )
		{
			if( pair.first == "PulseCounter" )
				pPulseCounterCollection->Add( CreatePulseCounter( pair.second ) );
		}

		return pPulseCounterCollection;
	}

	return nullptr;
}

std::unique_ptr<PulseCounter> Anl4TrackSystemReader::CreatePulseCounter( 
	const boost::property_tree::ptree& pt ) const
{
	if( std::unique_ptr<PulseCounter> pPulseCounter = PulseCounter::Make(); pPulseCounter ){
		pPulseCounter->ID( pt.get( "<xmlattr>.id", 0 ) );
		pPulseCounter->Reference( "name", pt.get( "<xmlattr>.name", "" ) );
		pPulseCounter->Threshold( pt.get( "<xmlattr>.threshold", 0 ) );
		pPulseCounter->Counter( pt.get( "<xmlattr>.counter", 0 ) );

		for( const auto& pair : pt )
		{
			if( pair.first == "Plug" ){
				std::string name = pair.second.get( "<xmlattr>.name", "" );
				if( name == "PlugToCountUp" )
					ReadPlug( pair.second, pPulseCounter->PlugToCountUp() );
				else if( name == "PlugToCountDown" )
					ReadPlug( pair.second, pPulseCounter->PlugToCountDown() );
				else if( name == "PlugToReset" )
					ReadPlug( pair.second, pPulseCounter->PlugToReset() );
			}

			else if( pair.first == "Jack" ){
				std::string name = pair.second.get( "<xmlattr>.name", "" );
				if( name == "JackOnReachThreshold" )
					ReadJack( pair.second, pPulseCounter->JackOnReachThreshold() );
				else if( name == "JackOnLeaveThreshold" )
					ReadJack( pair.second, pPulseCounter->JackOnLeaveThreshold() );
				else if( name == "JackOnCountUp" )
					ReadJack( pair.second, pPulseCounter->JackOnCountUp() );
				else if( name == "JackOnCountDown" )
					ReadJack( pair.second, pPulseCounter->JackOnCountDown() );
			}
		}

		return pPulseCounter;
	}

	return nullptr;
}

std::unique_ptr<TimerCollection> Anl4TrackSystemReader::CreateTimerCollection(
	const boost::property_tree::ptree& pt ) const
{
	if( std::unique_ptr<TimerCollection> pTimerCollection = TimerCollection::Make(); pTimerCollection )
	{
		for( const auto& pair : pt )
		{
			if( pair.first == "Timer" )
				pTimerCollection->Add( CreateTimer( pair.second ) );
		}

		return pTimerCollection;
	}

	return nullptr;
}

std::unique_ptr<Timer> Anl4TrackSystemReader::CreateTimer( 
	const boost::property_tree::ptree& pt ) const
{
	if( std::unique_ptr<Timer> pTimer = Timer::Make(); pTimer ){
		pTimer->ID( pt.get( "<xmlattr>.id", 0 ) );
		pTimer->Reference( "name", pt.get( "<xmlattr>.name", "" ) );
		pTimer->Lapse( get( pt, "<xmlattr>.lapse", fixed_timestep, _ms ) );
		pTimer->Periodic( pt.get( "<xmlattr>.periodic", false ) );
		pTimer->SetTime( get( pt, "<xmlattr>.time", 0_ms, _ms ) );
		if( pt.get( "<xmlattr>.started", false ) )
			pTimer->Start();

		for( const auto& pair : pt )
		{
			if( pair.first == "Plug" ){
				std::string name = pair.second.get( "<xmlattr>.name", "" );
				if( name == "PlugToStart" )
					ReadPlug( pair.second, pTimer->PlugToStart() );
				else if( name == "PlugToReset" )
					ReadPlug( pair.second, pTimer->PlugToReset() );
				else if( name == "PlugToPause" )
					ReadPlug( pair.second, pTimer->PlugToPause() );
			}

			else if( pair.first == "Jack" ){
				std::string name = pair.second.get( "<xmlattr>.name", "" );
				if( name == "JackOnTick" )
					ReadJack( pair.second, pTimer->JackOnTick() );
				else if( name == "JackOnStart" )
					ReadJack( pair.second, pTimer->JackOnStart() );
				else if( name == "JackOnReset" )
					ReadJack( pair.second, pTimer->JackOnReset() );
				else if( name == "JackOnPause" )
					ReadJack( pair.second, pTimer->JackOnPause() );
			}
		}

		return pTimer;
	}

	return nullptr;
}

std::unique_ptr<Section> Anl4TrackSystemReader::CreateSection( const boost::property_tree::ptree& pt ) const
{
	if( auto pSection = Section::Make( SpecialSection( pt.get( "<xmlattr>.type", "custom" ) ) ); pSection ){
		if( pSection->GetSectionType() == Section::SpecialSections::custom )
		{
			pSection->SetCntPoints( pt.get( "<xmlattr>.count", 0 ) );
			int idx = 0;
			for( const auto& pair : pt )
			{
				if( pair.first == "Position2D" ){
					spat::Position2D<Length> position;
					ReadPosition2D( pair.second, position );
					pSection->Set( idx++, position );
				}
			}
		}

		return pSection;
	}

	return nullptr;
}

void Anl4TrackSystemReader::ReadJumpSignalTarget( 
	const boost::property_tree::ptree& pt, 
	std::shared_ptr<JumpSite> pSignal, 
	const TrackSystem& trackSystem/*,
	const Fleet& fleet*/ ) const
{
	for( const auto& pair : pt )
	{
		if( pair.first == "TrackLocation" ){
			if( auto pLocation = CreateLocation( pair.second, trackSystem ) )
				pSignal->Target( *pLocation );
		}

		//else if( pair.first == "Depot" )
		//	pSignal->Target( fleet.GetDepotCollection()->Get( pair.second.get( "<xmlattr>.depotID", 0 ) ) );
	}
}


void Anl4TrackSystemReader::ReadSensor(
	const boost::property_tree::ptree& pt,
	SensorFilterJack& sensor,
	TrackLocation& trackLocation ) const
{
	sensor.Reference( "name", pt.get( "<xmlattr>.name", "" ) );
	sensor.ID( pt.get( "<xmlattr>.id", 0 ) );

	for( const auto& pair : pt )
	{
		if( pair.first == "Effect" )
			sensor.SetEffect( sensor.GetEffect() | SensorEffectFromString( pair.second.get( "<xmlattr>.type", "" ) ) );

		else if( pair.first == "TrackLocation" )
			ReadTrackLocation( pair.second, trackLocation );

		else if( pair.first == "Jack" )
			ReadJack( pair.second, sensor.JackOnTrigger() );
	}
}

void Anl4TrackSystemReader::ReadSignal(
	const boost::property_tree::ptree& pt, 
	std::shared_ptr<Signal> pSignal,
	const TrackSystem& trackSystem ) const
{
	pSignal->Reference( "name", pt.get( "<xmlattr>.name", "" ) );
	pSignal->ID( pt.get( "<xmlattr>.id", 0 ) );
	pSignal->Set( ToSignalStatus( pt.get( "<xmlattr>.status", "none" ) ), false );

	TrackRange trackRange;
	for( const auto& pair : pt )
	{
		if( pair.first == "TrackRange" )
			ReadTrackRange( pair.second, trackRange );

		else if( pair.first == "Plug" )
			ReadPlug( pair.second, pSignal->PlugTo( SignalStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) ) );

		else if( pair.first == "Jack" ){
			std::string name = pair.second.get( "<xmlattr>.name", "" );
			if( name.find( "ChangeTo" ) != std::string::npos )
				ReadJack( pair.second, pSignal->JackOnChangeTo( SignalStatusFrom( name ) ) );
			else
				ReadJack( pair.second, pSignal->JackOn( SignalStatusFrom( name ) ) );
		}
	}

	if( auto pTrack = trackSystem.Get( trackRange.refid ) )
		pTrack->Attach( pSignal, trackRange.range );
}

void Anl4TrackSystemReader::ReadIndicator( 
	const boost::property_tree::ptree& pt, 
	Indicator& indicator, 
	const ConnectorCollection& connectorCollection ) const
{
	indicator.Reference( "name", pt.get( "<xmlattr>.name", "" ) );
	indicator.Reference( "reference", pt.get( "<xmlattr>.reference", "" ) );
	indicator.ID( pt.get( "<xmlattr>.id", 0 ) );
	indicator.Set( ToIndicatorStatus( pt.get( "<xmlattr>.status", "none" ) ), false );
	const IDType target = pt.get( "<xmlattr>.refid", 0 );

	for( const auto& pair : pt )
	{
		if( pair.first == "Frame" ){
			Frame<Length,One> frame;
			ReadFrame( pair.second, frame );
			indicator.SetFrame( frame );
		}

		else if( pair.first == "Plug" )
			ReadPlug( pair.second, indicator.PlugTo( IndicatorStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) ) );

		else if( pair.first == "Jack" )
			ReadJack( pair.second, indicator.JackOn( IndicatorStatusFrom( pair.second.get( "<xmlattr>.name", "" ) ) ) );
	}

	if( auto pConnector = connectorCollection.Get( target ) ){
		if( auto pAligned = dynamic_cast<SwitchAligned*>(&indicator) ){
			pAligned->PreserveUpDirection( pt.get( "<xmlattr>.bPreserveUpDirection", false ) );
			pAligned->Attach( pConnector );
		}
	}
}

void Anl4TrackSystemReader::ReadJack( const boost::property_tree::ptree& pt, Jack& jack ) const{
	jack.Reference( "name", pt.get( "<xmlattr>.name", "" ) );
	jack.ID( pt.get( "<xmlattr>.id", IDType{0} ) );
	jack.RefPlugID( pt.get( "<xmlattr>.plugid", IDType{0} ) );
	m_SocketRegistry.ConnectJack( jack );
}

void Anl4TrackSystemReader::ReadPlug( const boost::property_tree::ptree& pt, Plug& plug ) const{
	plug.Reference( "name", pt.get( "<xmlattr>.name", "" ) );

	if( pt.get( "<xmlattr>.plugid", IDType{} ) == IDType{} )
		std::cerr << "Reading 'Plug': attribute 'plugid' not present or zero. Plug:id=\"" 
				  << pt.get<IDType>( "<xmlattr>.id", IDType{} ) << "\"" << std::endl;
	plug.ID( pt.get( "<xmlattr>.plugid", pt.get( "<xmlattr>.id", IDType{} ) ) );

	m_SocketRegistry.RegisterPlug( plug );

	for( const auto& pair : pt ){
		if( pair.first == "Jack" )
			ReadJack( pair.second, plug.JackOnPulse() );
	}
}

void Anl4TrackSystemReader::ReadPlug( const boost::property_tree::ptree& pt, MultiPlug& plug ) const{
	ReadPlug( pt, plug.ID() ? plug.Make( nullptr ) : static_cast<Plug&>(plug) );
}

void Anl4TrackSystemReader::ReadSection( const boost::property_tree::ptree& pt, trax::TrackBuilder& track ) const noexcept
{
	if( SectionTrack* pSectionTrack = dynamic_cast<SectionTrack*>(&track); pSectionTrack )
		pSectionTrack->Attach( CreateSection( pt ) );
}
///////////////////////////////////////
void ReadEnd( const boost::property_tree::ptree& pt,
	TrackBuilder& track,
	Track::EndType endtype,
	std::vector<Track::Coupling>& couplings )
{
	if( !pt.get( "<xmlattr>.electrificationShift", "" ).empty() ){
		if( endtype == Track::EndType::front )
			track.Reference( "electrificationShiftBegin", pt.get( "<xmlattr>.electrificationShift", "" ) );
		else if( endtype == Track::EndType::end )
			track.Reference( "electrificationShiftEnd", pt.get( "<xmlattr>.electrificationShift", "" ) );
	}

	for( const auto& pair : pt )
	{
		if( pair.first == "BufferStop" ){
			if( endtype == Track::EndType::front )
				track.Reference( "bufferStopBegin", "true" );
			else if( endtype == Track::EndType::end )
				track.Reference( "bufferStopEnd", "true" );
		}

		else if( pair.first == "OpenEnd" ){

		}

		else if( pair.first == "Connection" ){
			Track::Coupling coupling;
			coupling.theOne.id	= track.ID();
			coupling.theOne.type= endtype;

			ReadConnection( pair.second, coupling.theOther );
			couplings.push_back( coupling );
		}
	}
}
///////////////////////////////////////
std::shared_ptr<trax::TrackSystem> ReadTrackSystemFromANL4(
	const ptreesupport::PTreeReader& reader, 
	const boost::property_tree::ptree& pt, 
	int atIdx )
{
	int Idx = 0;
	for( const auto& pair : pt )
	{
		if( pair.first == "traxML" )
		{
			for( const auto& pairModule : pair.second ){
				if( pairModule.first == "Module" )
				{
					for( const auto& pairTrackSystem : pairModule.second ){
						if( pairTrackSystem.first == "TrackSystem" )
						{
							if( ++Idx == atIdx )
								return reader.ReadTrackSystem( pairTrackSystem.second );
							else
								break;
						}
					}
				}
			}	
		}
	}

	return nullptr;
}
///////////////////////////////////////
} // namespace ptreesupport
} // namespace trax
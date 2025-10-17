// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#include "trax/collections/support/Anl3TrackSystemReader.h"

#include "trax/collections/ConnectorCollection.h"
#include "trax/collections/IndicatorCollection.h"
#include "trax/collections/PulseCounterCollection.h"
#include "trax/collections/SignalCollection.h"
#include "trax/collections/TimerCollection.h"
#include "trax/collections/TrackSystem.h"
#include "trax/collections/TrackCollection.h"
#include "trax/collections/TrackCollectionContainer.h"

#include "trax/SectionTrack.h"
#include "trax/Indicator.h"
#include "trax/Jack.h"
#include "trax/LogicElements.h"
#include "trax/Plug.h"
#include "trax/Section.h"
#include "trax/Sensor.h"
#include "trax/Timer.h"

#include "common/support/CommonSupportXML.h"
#include "spat/support/SpatSupportXML.h"

#include <iostream>

namespace trax{
namespace ptreesupport{

using namespace common;
using namespace common::ptreesupport;
using namespace spat;
using namespace spat::ptreesupport;

Anl3TrackSystemReader::Anl3TrackSystemReader( const char* pLocale )
	: PTreeReader{ pLocale }
{
}

Anl3TrackSystemReader::Anl3TrackSystemReader( 
	SocketRegistry& socketRegistry, 
	const char* pLocale )
	: PTreeReader{ socketRegistry, pLocale }
{
}

std::shared_ptr<TrackSystem> Anl3TrackSystemReader::ReadTrackSystem( const boost::property_tree::ptree& pt ) const
{
	std::vector<std::pair<Track::Coupling, std::string>> couplings;
	std::unique_ptr<SignalCollection> pSignalCollection= SignalCollection::Make();
	std::unique_ptr<IndicatorCollection> pIndicatorCollection= IndicatorCollection::Make();
	std::unique_ptr<TimerCollection> pTimerCollection= TimerCollection::Make();
	std::unique_ptr<PulseCounterCollection> pPulseCounterCollection= PulseCounterCollection::Make();
	std::map<IDType, Velocity> travelVelocities;
	IDType maxSensorID = 0;

	return CreateTrackSystem( pt, 
		couplings, 
		*pSignalCollection, 
		*pIndicatorCollection, 
		*pTimerCollection, 
		*pPulseCounterCollection, 
		travelVelocities, 
		maxSensorID );
}

std::shared_ptr<TrackSystem> Anl3TrackSystemReader::CreateTrackSystem( 
	const boost::property_tree::ptree& pt, 
	std::vector<std::pair<Track::Coupling,std::string>>& couplings,
	SignalCollection& signalCollection,
	IndicatorCollection& indicatorCollection, 
	TimerCollection& timerCollection, 
	PulseCounterCollection& pulseCounterCollection,
	std::map<IDType,Velocity>& travelVelocities, 
	IDType& maxSensorID ) const
{
	if( std::shared_ptr<TrackSystem> pTrackSystem = TrackSystem::Make(); pTrackSystem ){
		for( const auto& pairTrackSystem : pt )
		{
			if( pairTrackSystem.first == "Gleissystem" )
			{
				if( std::shared_ptr<TrackCollection> pTrackCollection = TrackCollection::Make(); pTrackCollection )
				{
					// There is a separate frame for 'Gleissystem' in anl3, and it sometimes
					// apears shifted. But EEP doesn't actually use that value ...
					//Frame<trax::Real> frame;
					//ReadDreibein( pt, frame );
					//pTrackCollection->SetFrame( frame );

					pTrackCollection->ID( pairTrackSystem.second.get( "<xmlattr>.GleissystemID", 0 ) );
					pTrackCollection->Reference( "TrackSystemNumber", pairTrackSystem.second.get( "<xmlattr>.TrackSystemNumber", "" ) );
					pTrackCollection->Reference( "Name", "TrackCollection_" + pairTrackSystem.second.get( "<xmlattr>.TrackSystemNumber", "" ) );

					pTrackSystem->GetCollectionContainer()->Add( pTrackCollection );

					std::vector<std::tuple<std::shared_ptr<Signal>, TrackBuilder*, Interval<Length>>> signals;

					for( const auto& pair : pairTrackSystem.second ){
						if( pair.first == "Gleis" )
							pTrackSystem->Add( CreateTrack( pair.second, *pTrackSystem->GetConnectorCollection(), signals, indicatorCollection, timerCollection, pulseCounterCollection, travelVelocities, maxSensorID ) );

						if( pair.first == "Gleisverbindung" ){
							try{
								const std::pair<Track::Coupling,std::string> coupling{ CreateTrackCoupling( pair.second, *pTrackSystem ) };
								const Length gap = pTrackSystem->CalculateGapSize( coupling.first.theOne, coupling.first.theOther );
								if( gap > 30_cm ){
									std::clog << "Warning: ";
									std::clog << "There is a gap between two tracks in EEP. GleissystemID (" << pairTrackSystem.second.get( "<xmlattr>.GleissystemID", 0 );								
									std::clog << ") : (" << coupling.first.theOne.id << ',';
									std::clog << ToString( coupling.first.theOne.type ) << ") and (" << coupling.first.theOther.id << ',' << ToString( coupling.first.theOther.type );
									std::clog << ") that are supposed to be coupled. Gap size: ";
									std::clog << gap << ". This might lead to rolling stocks derailing." << std::endl;
								}

								pTrackSystem->Couple( coupling.first );
								couplings.push_back( coupling );
							}
							catch( const std::runtime_error& e ){
								std::clog << "Warning: ";
								std::clog << "The coupling between track (" << pair.second.get( "<xmlattr>.GleisID1", 0 ) << "," << pair.second.get( "<xmlattr>.Anschluss1", "" ) << ") ";
								std::clog << "and track (" << pair.second.get( "<xmlattr>.GleisID2", 0 ) << "," << pair.second.get( "<xmlattr>.Anschluss2", "" ) << ") ";
								std::clog << "could not be established. Reason: " << e.what() << std::endl;
							}
						}
					}

					for( auto iter = pTrackSystem->GetConnectorCollection()->begin(); 
						iter != pTrackSystem->GetConnectorCollection()->end(); ++iter )
					{					
						if( !iter->IsComplete() ){
							std::clog << "There is a track missing for the arm of a switch. GleisID(" << iter->Reference( "GleisID" ) 
							<< "), GleissystemID(" << pairTrackSystem.second.get( "<xmlattr>.GleissystemID", 0 ) << ")."<< std::endl;
							continue;
						}

						// bit 0 1=show electric // bit 1 1=hide weichen// bit 2 1=weichen at right// bit 3 1=weichen at left //bit 4 DKW
						const int data = std::stoi( iter->Reference( "data" ) );
						if( !(data & 0x02) ){
							if( auto pSwitch = std::dynamic_pointer_cast<Switch>(iter.operator->()); pSwitch ){
								if( std::shared_ptr<BinaryIndicator> pSwitchIndicator = BinaryIndicator::Make( Indicator::Type::switch_mono ); pSwitchIndicator ){
									pSwitchIndicator->Reference( "reference", pSwitch->Reference( "gsbname" ) );
									std::string TipTxt = pSwitch->Reference( "TipTxt" );
									if( !TipTxt.empty() ){
										pSwitchIndicator->Reference( "TipTxt", TipTxt );
										pSwitchIndicator->Reference( "TipBGCR", pSwitch->Reference( "TipBGCR" ) );
										pSwitchIndicator->Reference( "TipBGCG", pSwitch->Reference( "TipBGCG" ) );
										pSwitchIndicator->Reference( "TipBGCB", pSwitch->Reference( "TipBGCB" ) );
									}

									Frame<Length,One> offset;
									offset.Init();

									Real sign = pSwitch->BranchLeftOrRight() ? -1.0f : +1.0f;
									if( data & 0x04 )
										sign = -1.0f;
									if( data & 0x08 )
										sign = +1.0f;
									offset.P.y += sign * 2_m;

									pSwitchIndicator->SetFrame( offset );

									pSwitchIndicator->Set( static_cast<trax::Indicator::Status>(std::stoi( pSwitch->Reference( "weichenstellung" ) )), false );

									if( auto pAligned = dynamic_cast<SwitchAligned*>(pSwitchIndicator.get()) ){
										pAligned->PreserveUpDirection( true );
										pAligned->Attach( pSwitch );
									}

									for( int i = 1; i <= Switch::status_count; ++i ){
										pSwitchIndicator->JackOn( static_cast<trax::Indicator::Status>(i) ).InsertAtTail( &pSwitch->PlugTo( SwitchStatusFromEEP( i ) ).Unplugged( &m_SocketRegistry ) );
										pSwitch->JackOn( SwitchStatusFromEEP( i ) ).InsertAtTail( &pSwitchIndicator->PlugTo( static_cast<trax::Indicator::Status>(i) ) );
									}

									indicatorCollection.Add( pSwitchIndicator );
									pSwitchIndicator->RegisterSockets( m_SocketRegistry );
								}
							}

							else if( std::shared_ptr<ThreeWaySwitch> pThreeWaySwitch = std::dynamic_pointer_cast<ThreeWaySwitch>(iter.operator->()); pThreeWaySwitch ){
								if( std::shared_ptr<Indicator> pSwitchIndicator = Indicator::Make( Indicator::Type::switch_multi ); pSwitchIndicator ){
									pSwitchIndicator->Reference( "reference", pThreeWaySwitch->Reference( "gsbname" ) );
									std::string TipTxt = pThreeWaySwitch->Reference( "TipTxt" );
									if( !TipTxt.empty() ){
										pSwitchIndicator->Reference( "TipTxt", TipTxt );
										pSwitchIndicator->Reference( "TipBGCR", pThreeWaySwitch->Reference( "TipBGCR" ) );
										pSwitchIndicator->Reference( "TipBGCG", pThreeWaySwitch->Reference( "TipBGCG" ) );
										pSwitchIndicator->Reference( "TipBGCB", pThreeWaySwitch->Reference( "TipBGCB" ) );
									}

									Frame<Length,One> offset;
									offset.Init();

									Real sign = +1.0f;
									if( data & 0x04 )
										sign = -1.0f;
									if( data & 0x08 )
										sign = +1.0f;
									offset.P.y += sign * 2_m;

									pSwitchIndicator->SetFrame( offset );

									pSwitchIndicator->Set( static_cast<trax::Indicator::Status>(std::stoi( pThreeWaySwitch->Reference( "weichenstellung" ) )), false );

									if( auto pAligned = dynamic_cast<SwitchAligned*>(pSwitchIndicator.get()) ){
										pAligned->PreserveUpDirection( true );
										pAligned->Attach( pThreeWaySwitch );
									}

									for( int i = 1; i <= ThreeWaySwitch::status_count; ++i ){
										pSwitchIndicator->JackOn( static_cast<trax::Indicator::Status>(i) ).InsertAtTail( &pThreeWaySwitch->PlugTo( ThreeWaySwitchStatusFromEEP( i ) ).Unplugged( &m_SocketRegistry ) );
										pThreeWaySwitch->JackOn( ThreeWaySwitchStatusFromEEP( i ) ).InsertAtTail( &pSwitchIndicator->PlugTo( static_cast<trax::Indicator::Status>(i) ) );
									}

									indicatorCollection.Add( pSwitchIndicator );
									pSwitchIndicator->RegisterSockets( m_SocketRegistry );
								}
							}
						}
					}

					for( const auto& tuple : signals ){
						signalCollection.Add( std::get<0>(tuple) );
						std::get<1>( tuple )->Attach( std::get<0>( tuple ), std::get<2>( tuple ) );
					}
				}
			}
		}

		return pTrackSystem;
	}

	return nullptr;
}

std::shared_ptr<TrackBuilder> Anl3TrackSystemReader::CreateTrack( 
	const boost::property_tree::ptree& pt, 
	ConnectorCollection& connectors,
	std::vector<std::tuple<std::shared_ptr<Signal>, TrackBuilder*, Interval<Length>>>& signals,
	IndicatorCollection& indicatorCollection,
	TimerCollection& timerCollection, 
	PulseCounterCollection& pulseCounterCollection,
	std::map<IDType,Velocity>& travelVelocities, 
	IDType& maxSensorID ) const
{
	if( std::shared_ptr<SectionTrack> pTrack = SectionTrack::Make(); pTrack )
	{
		pTrack->ID( pt.get( "<xmlattr>.GleisID", 0 ) );
		pTrack->Reference( "name", pt.get( "<xmlattr>.name", "Track_" + std::to_string(pTrack->ID()) ) );
		pTrack->Reference( "reference", pt.get( "<xmlattr>.gsbname", "" ) );
		pTrack->Reference( "scale", pt.get( "<xmlattr>.scale", "1.0" ) );

		if( auto waterDepth = pt.get_optional<Real>( "<xmlattr>.u3" ); waterDepth )
			pTrack->Reference( "waterDepth", std::to_string( 10.0f * waterDepth.value() ) );

		const int data = pt.get( "<xmlattr>.data", 0 );
		// bit 0 1=show electric // bit 1 1=hide weichen// bit 2 1=weichen at right// bit 3 1=weichen at left //bit 4 DKW
		if( data & 0x01 ){
			pTrack->Reference( "electrified", "true" );
			pTrack->Reference( "electrificationShiftBegin", pt.get( "<xmlattr>.ElectSideS", "0" ) );
			pTrack->Reference( "electrificationShiftEnd", pt.get( "<xmlattr>.ElectSideE", "0" ) );
		}
		if( data & 0x02 ){



		}

		std::string clsid = pt.get( "<xmlattr>.clsid", "" );
		if( !(data & 0x10) && clsid == "C889EADB-63B5-44A2-AAB9-457424CFF15F" )
		// switch
		{
			if( std::shared_ptr<Switch> pSwitch = Switch::Make(); pSwitch )
			{
				AttributesToReferences( pt, *pSwitch );

				pSwitch->NarrowTrack( pTrack, Track::EndType::end );
				pSwitch->Set( SwitchStatusFromEEP( pt.get( "<xmlattr>.weichenstellung", 0 ) ) );
				pSwitch->RegisterSockets( m_SocketRegistry );

				connectors.Add( pSwitch );
			}
		}
		else if( clsid == "B0818DD8-5DFD-409F-8022-993FD3C90759" )
		//three way switch
		{
			if( std::shared_ptr<ThreeWaySwitch> pSwitch = ThreeWaySwitch::Make(); pSwitch )
			{
				AttributesToReferences( pt, *pSwitch );

				pSwitch->NarrowTrack( pTrack, Track::EndType::end );
				pSwitch->Set( ThreeWaySwitchStatusFromEEP( pt.get( "<xmlattr>.weichenstellung", 0 ) ) );
				pSwitch->RegisterSockets( m_SocketRegistry );

				connectors.Add( pSwitch );
			}
		}
		else if( clsid == "06D80C90-4E4B-469B-BFE0-509A573EBC99" )
		// end track with buffer
		{
			pTrack->Reference( "bufferStopEnd", "true" );

			// ...
		}

		const auto citer = pt.find( "Curve" ); 
		if( citer == pt.not_found() ){
			Real From = 0;
			auto iter = pt.find( "Anfangsfuehrungsverdrehung" );
			if( iter != pt.not_found() )
				From = (*iter).second.get( "<xmlattr>.Wert", 0.0f );

			Real To = 0;
			Real Kruemmung = 0;
			Real Torsion = 0;
			Real Kurve = 0;
			Length Laenge{0};

			iter = pt.find( "Charakteristik" );
			if( iter != pt.not_found() ){
				Kruemmung	= 100.0f / units_per_meter * (*iter).second.get( "<xmlattr>.Kruemmung", 0.0f );
				Torsion		= 100.0f / units_per_meter * (*iter).second.get( "<xmlattr>.Torsion", 0.0f );
				Kurve		= (*iter).second.get( "<xmlattr>.Kurve", 0.0f );
				Laenge		= _cm( (*iter).second.get( "<xmlattr>.Laenge", 0.0f ) );
				To			= From + (*iter).second.get( "<xmlattr>.Fuehrungsverdrehung", 0.0f ) * (*iter).second.get( "<xmlattr>.Laenge", 0.0f );
			}

			//Todo: substitute with CreateNonEEPCurve function
			Frame<Length,One> frame;
			ReadDreibein( pt, frame );
			frame.RotateTan( -From );	// The starting Frame includes the twist at the starting point. 
										// For trax this is not the case, so we calculate this out here:

			if( std::fabs( Kruemmung ) == 0 && std::fabs( Torsion ) == 0 && std::fabs( Kurve ) == 0 ){
				if( std::shared_ptr<LineP> pLine = LineP::Make() ){
					pTrack->SetFrame( frame );
					//From += pi/2;
					//To += pi/2;

					pLine->Create( VectorBundle<Length,One>{ Origin3D<Length>, Vector<One>{Laenge/Length{1}, 0, 0} }, Ez<One> );

					pTrack->Attach( pLine, {0_m,Laenge} );
				}
			}
			else if( std::fabs( Torsion ) == 0 && std::fabs( Kurve ) == 0 ){
				if( std::shared_ptr<ArcP> pArc = ArcP::Make() ){
					//pTrack->SetFrame( frame );
					//pArc->Create(	VectorBundle2<Real>(	0,0,0,
					//										1,0,0,
					//										0,Sign(Kruemmung),0 ),
					//				Laenge,
					//				fabs(Kruemmung) );

					frame.N *= Sign( Kruemmung );
					pArc->Create( VectorBundle2<Length,One>{ frame }, _1Im(std::fabs( Kruemmung )) );

					if( Kruemmung < 0 ){
						From	+= pi;
						To		+= pi;
					}

					pTrack->Attach( pArc, {0_m,Laenge} );
				}
			}
			else if( std::fabs( Kurve ) == 0 ){
				if( std::shared_ptr<HelixP> pHelix = HelixP::Make() ){
					frame.N *= Sign( Kruemmung );
					frame.B *= Sign( Kruemmung );
					Torsion *= Sign( Kruemmung );

					pHelix->Create( frame,
						_1Im(std::fabs(Kruemmung)),
						_1Im(Torsion) );

					if( Kruemmung < 0 ){
						From	+= pi;
						To		+= pi;
					}

					pTrack->Attach( pHelix, {0_m,Laenge} );
				}
			}
			else if( std::fabs( Kruemmung ) == 0 && std::fabs( Torsion ) == 0 ){
				if( std::shared_ptr<ArcP> pArc = ArcP::Make() ){

					frame.RotateTan( Sign( Kurve ) * pi/2 );
					pArc->Create( VectorBundle2<Length,One>{ frame }, abs( Kurve / Laenge ) );

					From-= Sign( Kurve ) * pi/2;
					To	-= Sign( Kurve ) * pi/2;

					pTrack->Attach( pArc, {0_m,Laenge} );
				}
			}
			else if( std::fabs( Torsion ) == 0 )
			{
				assert( !"Curve geometry!" );
				throw std::logic_error( "Invalid curve geometry: EEP curve not supported." );

				//// Second best approach: idiosyncratic EEPCurve in trax (prefer translation into Arc or Helix...):
				//if( std::shared_ptr<EEPCurve> pEEPCurve = EEPCurve::Make() ){
				//	EEPCurve::Data curvedata; 
				//	curvedata.gc_Kruemmung		= (*iter).second.get( "<xmlattr>.Kruemmung", 0.0f );
				//	curvedata.gc_Windung		= (*iter).second.get( "<xmlattr>.Torsion", 0.0f );
				//	curvedata.gc_Verdrillung	= (*iter).second.get( "<xmlattr>.Fuehrungsverdrehung", 0.0f );
				//	curvedata.gc_Laenge			= (*iter).second.get( "<xmlattr>.Laenge", 0.0f );
				//	curvedata.gc_Kurve			= (*iter).second.get( "<xmlattr>.Kurve", 0.0f );
				//	curvedata.m_FuehrungsVerdrehung = From;
				//	curvedata.m_AnfangsBein = spatial_cast<Real>(frame);
				//	curvedata.m_AnfangsBein.P *= 100 * meters_per_unit;
				//	curvedata.m_AnfangsBein.RotateTan( From ); // undo rotation from above

				//	pEEPCurve->Create( curvedata );

				//	// The twist is completely contained in the eep - curve...
				//	From = 0;
				//	To = 0;

				//	pTrack->Attach( pEEPCurve, {0_m,Laenge} );
				//}
	/*
				// Better approach: simple helix (not entirely correct)	
				// only works for right handed helix
				if( std::shared_ptr<Helix> pHelix = CreateHelix() )
					// For a helix it holds:
					// curvature = a / (pow<2>(a)+pow<2>(b))
					// torsion = b / (pow<2>(a)+pow<2>(b))
					// with:
					// a = 1/Kruemmung
					// b = h/ (Kruemmung * sqrt( Laenge*Laenge - h*h ));
					//
					// (1) Kurve*R = Laenge (R: radius of ascending curve)
					// (2) sin(Kurve/2) = d/2R
					// (3) h = d * sin(Kurve/2)
					// (+2)	 = 2R * pow<2>(sin(Kurve/2))
					// (+1)	 = 2*Laenge/Kurve * pow<2>(sin(Kurve/2))
				{
					frame.N *= Sign( Kruemmung );
					frame.B *= Sign( Kruemmung );

					frame.RotateNor( -Sign( Kruemmung ) * Kurve/2 );

					Real r = 1/fabs(Kruemmung);
					Real h = 2*Laenge/Kurve*Square( sin( Kurve/2 ) );

					Real a = r;
					Real b = h*r/ sqrt( Laenge*Laenge - h*h );

					Real curvature = a / (a*a + b*b);
					Real torsion = b / (a*a + b*b);

					pHelix->Create( frame, curvature, torsion );

					if( Kruemmung < 0 ){
						From	+= pi;
						To		+= pi;
					}

					pTrack->Attach( pHelix, Laenge );
				}
				*/
			}
			else{
				assert( !"Curve geometry?" );
				throw std::logic_error( "Invalid curve geometry: an EEP curve has nonzero Kruemmung, Torsion, Kurve all at once." );
			}
	
			pTrack->Attach( LinearTwist::Make(
				From,
				To ) );
		}

		pTrack->Attach( Section::Make( Section::SpecialSections::vignol_UIC60 ) );

		Interval<Length> range;
		for( const auto& pair : pt )
		{
			if( pair.first == "Frame" ){
				Frame<Length,One> frame;
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

			if( pair.first == "Meldung" ){
				Interval<Length> trackRange;
				std::vector<int> functionMap;
				// Here something is missing. The functionMap would have to be set up correctly ...
				//functionMap.push_back( 0 );
				//functionMap.push_back( 1 );
				//functionMap.push_back( 2 );

				if( std::shared_ptr<Signal> pSignal = CreateSignal( pair.second, indicatorCollection, trackRange, travelVelocities, functionMap, true, true ) ){				
					signals.push_back( std::make_tuple( pSignal, pTrack.get(), trackRange ) );
					//pTrack->Attach( pSignal, trackRange ); // too early, not all the tracks are created yet.
				}
			}

			else if( pair.first == "Kontakt" ){
				TrackLocation trackLocation;
				if( pair.second.get( "<xmlattr>.fireOnPara", false ) ){
					if( std::shared_ptr<SensorFilterJack> pSensor = CreateKontakt( pair.second, timerCollection, pulseCounterCollection, trackLocation, maxSensorID ) )
						pTrack->Attach( pSensor, trackLocation );
				}

				if( pair.second.get( "<xmlattr>.fireOnAnti", false ) ){
					if( std::shared_ptr<SensorFilterJack> pSensor = CreateKontakt( pair.second, timerCollection, pulseCounterCollection, trackLocation, maxSensorID ) ){
						trackLocation.orientation.Flip();
						pTrack->Attach( pSensor, trackLocation );
					}
				}
			}
				
			else if( pair.first == "KontaktZiel" )
				pTrack->Reference( "KontaktZiel", pair.second.data() );
		}

		if( pTrack->IsValid() )
			return pTrack;
		else
			std::cerr << "Warning: Track " << pTrack->ID() << " is invalid. " << pTrack->Reference( "reference" ) << std::endl;
	}

	return nullptr;
}

std::pair<Track::Coupling,std::string> Anl3TrackSystemReader::CreateTrackCoupling( 
	const boost::property_tree::ptree& pt, 
	const TrackSystem& trackSystem ) const
{
	std::pair<Track::Coupling,std::string> coupling;
	coupling.first.theOne.id					= pt.get( "<xmlattr>.GleisID1", 0 );
	coupling.first.theOne.type					= From( pt.get( "<xmlattr>.Anschluss1", "" ) );
	coupling.second								= pt.get( "<xmlattr>.Anschluss1", "" );	
	coupling.first.theOther.id					= pt.get( "<xmlattr>.GleisID2", 0 );
	coupling.first.theOther.type				= From( pt.get( "<xmlattr>.Anschluss2", "" ) );

	if( coupling.first.theOne.type == Track::EndType::end )
		// A might be switch
	{
		for( auto iter = trackSystem.GetConnectorCollection()->begin(); iter != trackSystem.GetConnectorCollection()->end(); ++iter ){
			assert( iter->Slot(0).first );
			if( coupling.first.theOne.id == iter->Slot(0).first->ID() ){
				if( pt.get( "<xmlattr>.Anschluss1", "" ) == "Ende" )
					iter->Slot( 1, trackSystem.Get( coupling.first.theOther.id ), coupling.first.theOther.type, true );
				else if( pt.get( "<xmlattr>.Anschluss1", "" ) == "EndeAbzweig" )
					iter->Slot( 2, trackSystem.Get( coupling.first.theOther.id ), coupling.first.theOther.type, true );
				else if( pt.get( "<xmlattr>.Anschluss1", "" ) == "EndeKoAbzweig" )
					iter->Slot( 3, trackSystem.Get( coupling.first.theOther.id ), coupling.first.theOther.type, true );
			}
		}
	}

	if( coupling.first.theOther.type == Track::EndType::end )
		// B might be switch
	{
		for( auto iter = trackSystem.GetConnectorCollection()->begin(); iter != trackSystem.GetConnectorCollection()->end(); ++iter ){
			assert( iter->Slot(0).first );
			if( coupling.first.theOther.id == iter->Slot(0).first->ID() ){
				if( pt.get( "<xmlattr>.Anschluss2", "" ) == "Ende" )
					iter->Slot( 1, trackSystem.Get( coupling.first.theOne.id ), coupling.first.theOne.type, true );
				else if( pt.get( "<xmlattr>.Anschluss2", "" ) == "EndeAbzweig" )
					iter->Slot( 2, trackSystem.Get( coupling.first.theOne.id ), coupling.first.theOne.type, true );
				else if( pt.get( "<xmlattr>.Anschluss2", "" ) == "EndeKoAbzweig" )
					iter->Slot( 3, trackSystem.Get( coupling.first.theOne.id ), coupling.first.theOne.type, true );
			}
		}
	}

	return coupling;
}

std::shared_ptr<Signal> Anl3TrackSystemReader::CreateSignal(
	const boost::property_tree::ptree& pt, 
	IndicatorCollection& indicatorCollection,
	Interval<Length>& trackRange, 
	std::map<IDType,Velocity>& travelVelocities,
	std::vector<int>& functionMap, 
	bool bCreateSignalSemaphore, 
	bool bCreateVorsignalSemaphore ) const
{
	if( std::shared_ptr<VelocityControl> pSignal = VelocityControl::Make() )
	{
		pSignal->ID( pt.get( "<xmlattr>.Key_Id", 0 ) );
		AttributesToReferences( pt, *pSignal );

		std::string name = pSignal->Reference( "name" );
		if( name == "Sign_Counter_SM2" ||
		    name == "Sign_Countr1_SM2" )
			//exclude some signals with special functionality that have no implementation yet...
		{
			std::cerr << "Warning: Signal " << pSignal->ID() << " is not implemented yet. " << pSignal->Reference( "name" ) << std::endl;
			return nullptr;
		}

		for( std::size_t stellung = 0; stellung < functionMap.size(); ++stellung ){
			const int EEPStellung = functionMap[stellung];

			switch( EEPStellung ){
			case 0:
				break;
			case 1:
				pSignal->SetVelocity( Signal::Status::clear, std::numeric_limits<Velocity>::lowest(), std::numeric_limits<Velocity>::max() );
				break;
			case 2:
				pSignal->SetVelocity( Signal::Status::stop, 0_mIs );
				break;
			default:
				int velLimit = 0;
				for( std::size_t i = 0; i != stellung; ++i ){
					if( functionMap[i] == 0 ||
						functionMap[i] == 1 ||
						functionMap[i] == 2 )
						continue;

					++velLimit;
				}

				const Signal::Status status{ static_cast<Signal::Status>(static_cast<int>(Signal::Status::velocity2) + velLimit) };
				constexpr Real factor = units_per_meter * 1000/ (60 * 60);

				if( EEPStellung > 6000 )
					// between sig/vorsig (setspeed)
					pSignal->SetVelocity( status, _mIs(factor * narrow_cast<Real>(EEPStellung % 1000)) );

				else if( EEPStellung > 5000 )
					// between sig/vorsig (slowdown)
					pSignal->VelocityMax( status, _mIs(factor * narrow_cast<Real>(EEPStellung % 1000)) );

				else if( EEPStellung > 4000 )
					// between sig/vorsig (speedup)
					pSignal->VelocityMin( status, _mIs(factor * narrow_cast<Real>(EEPStellung % 1000)) );

				else if( EEPStellung > 3000 ){
					pSignal->SetVelocity( status, _mIs(factor * narrow_cast<Real>(EEPStellung % 1000)) );
					pSignal->AffectTravelVelocity( status, true );
				}

				else if( EEPStellung > 2000 )
					// nie mniej niz  (speedup)
				{
					pSignal->VelocityMin( status, _mIs(factor * narrow_cast<Real>(EEPStellung % 1000)) );
					pSignal->AffectTravelVelocity( status, true );
				}

				else if( EEPStellung > 1000 )
					// nie wiecej niz (slowdown) -------------------------------------------------
				{
					pSignal->VelocityMax( status, _mIs(factor * narrow_cast<Real>(EEPStellung % 1000)) );
					pSignal->AffectTravelVelocity( status, true );
				}
			}
		}


		const auto kiter = pt.find( "KontaktZiel" );
		if( kiter != pt.not_found() )
			pSignal->Reference( "KontaktZiel", kiter->second.data() );

		const auto iter = pt.find( "Signal" );
		if( iter != pt.not_found() ){
			trackRange.Near( _cm(pt.get( "<xmlattr>.Position", 0.0f )) );
			trackRange.Far( trackRange.Near() + _cm((pt.get( "<xmlattr>.ParaOderAnti", true ) ? 1.0f : -1.0f) * iter->second.get( "<xmlattr>.wirkungsdistanz", 0.0f )) );

			pSignal->StopDistance( _cm(iter->second.get( "<xmlattr>.StopAt", 0.0f )) );

			AttributesToReferences( iter->second, *pSignal );

			for( unsigned int i = 0; i < functionMap.size(); ++i )
				pSignal->Reference( std::to_string( i ), std::to_string( functionMap[i] ) );

			pSignal->Set( SignalStatusFromEEP( iter->second.get( "<xmlattr>.stellung", 0 ), *pSignal ), false );

			for( const auto& pair : iter->second )
			{
				if( pair.first == "Wartender" ){
					if( IDType TrainID = pair.second.get( "<xmlattr>.zugverbandID", 0 ) )
						travelVelocities.insert( std::make_pair( TrainID, _mIs(pair.second.get( "<xmlattr>.sollgeschwindigkeit", 0.f )) ) );

					//pair.second.get<IDType>( "<xmlattr>.fuhrparkID", 0 )
				}
			}
		}

		pSignal->RegisterSockets( m_SocketRegistry ); //?

		if( bCreateSignalSemaphore )
		{
			if( std::shared_ptr<Indicator> pSignalSemaphore = Indicator::Make( Indicator::Type::velocity_control ) ){
				pSignalSemaphore->Reference( "reference", "\\Signale\\Signale\\" + pt.get( "<xmlattr>.name" , "" ) + ".3dm" );

				spat::Frame<Length,One> offset;
				offset.Init();
				offset.P.y = Length{-pt.get( "<xmlattr>.Hofs", 0.0f ) * units_per_meter / 100};
				offset.P.z = Length{-0.6f * units_per_meter}; // EEP indiscriminately sets all semaphores 60 cm down
				pSignalSemaphore->SetFrame( offset );

				if( auto pAligned = dynamic_cast<SignalAligned*>(pSignalSemaphore.get()) ){
					pAligned->Attach( pSignal );
					pAligned->PreserveUpDirection( true );
				}

				for( int i = 1; i < common::narrow_cast<int>(functionMap.size()); ++i ){
					pSignalSemaphore->Set( static_cast<Indicator::Status>(i), false );
					pSignalSemaphore->JackOn( static_cast<Indicator::Status>(i) ).InsertAtTail( &pSignal->PlugTo( SignalStatusFromEEP( i, *pSignal ) ).Unplugged( &m_SocketRegistry ) );
					pSignal->JackOn( SignalStatusFromEEP( i, *pSignal ) ).InsertAtTail( &pSignalSemaphore->PlugTo( static_cast<Indicator::Status>(i) ) );
				}

				try{ 
					pSignalSemaphore->Set( static_cast<Indicator::Status>(std::stoi(pSignal->Reference( "stellung" ))), false );
				}
				catch( const std::invalid_argument& e ){
					std::stringstream stream;
					stream << e.what() << ": Signal id:(" << pSignal->ID() << ") 'stellung' (" << pSignal->Reference( "stellung" ) << ") found that is not valid and could not get set for a signal semaphore." << std::endl;
					std::cerr << stream.str();
				}

				indicatorCollection.Add( pSignalSemaphore );
				pSignalSemaphore->RegisterSockets( m_SocketRegistry ); //?
			}
		}

		if( bCreateVorsignalSemaphore )
		{
			if( std::shared_ptr<Indicator> pSignalSemaphore = Indicator::Make( Indicator::Type::velocity_control ) ){
				pSignalSemaphore->Reference( "reference", "\\Signale\\Vorsignale\\" + pt.get( "<xmlattr>.name", "" ) + ".3dm" );

				spat::Frame<Length,One> offset;
				offset.Init();
				offset.P.y = Length{-pt.get( "<xmlattr>.Vofs", 0.0f ) * units_per_meter / 100};
				offset.P.z = Length{-0.6f * units_per_meter}; // EEP indiscriminately sets all semaphores 60 cm down
				pSignalSemaphore->SetFrame( offset );

				if( auto pAligned = dynamic_cast<SignalAligned*>(pSignalSemaphore.get()) ){
					pAligned->Attach( pSignal, Length{std::stof( pSignal->Reference( "wirkungsdistanz" ) ) * -units_per_meter / 100} );
					pAligned->PreserveUpDirection( true );
				}

				for( int i = 1; i < common::narrow_cast<int>(functionMap.size()); ++i ){
					pSignalSemaphore->Set( static_cast<Indicator::Status>(i), false );
					pSignalSemaphore->JackOn( static_cast<Indicator::Status>(i) ).InsertAtTail( &pSignal->PlugTo( SignalStatusFromEEP( i, *pSignal.get() ) ).Unplugged( &m_SocketRegistry ) );
					pSignal->JackOn( SignalStatusFromEEP( i, *pSignal ) ).InsertAtTail( &pSignalSemaphore->PlugTo( static_cast<Indicator::Status>(i) ) );
				}

				try{ 
					pSignalSemaphore->Set( static_cast<Indicator::Status>(std::stoi( pSignal->Reference( "stellung" ) )), false );
				}
				catch( const std::invalid_argument& e ){
					std::stringstream stream;
					stream << e.what() << ": Signal id:(" << pSignal->ID() << ")'stellung' (" << pSignal->Reference( "stellung" ) << ") found that is not valid and could not get set for a vorsignal semaphore" << std::endl;
					std::cerr << stream.str();
				}

				indicatorCollection.Add( pSignalSemaphore );
				pSignalSemaphore->RegisterSockets( m_SocketRegistry ); //?
			}
		}

		return pSignal;
	}
	
	return nullptr;
}

std::unique_ptr<SensorFilterJack> Anl3TrackSystemReader::CreateKontakt( 
	const boost::property_tree::ptree& pt, 
	TimerCollection& timerCollection, 
	PulseCounterCollection& pulseCounterCollection,
	TrackLocation& trackLocation,
	IDType& maxSensorID ) const
{
	if( pt.get( "<xmlattr>.KontaktZiel", "0" ) == "0" )
		return nullptr; // Lost contact.

	if( std::unique_ptr<SensorFilterJack> pSensor = SensorFilterJack::Make( pt.get<bool>( "<xmlattr>.TrainSide", false ) ? SensorFilterJack::train_end : SensorFilterJack::train_tip ); pSensor )
	{
		pSensor->ID( ++maxSensorID );

		std::string nameFilter{ pt.get( "<xmlattr>.NameFilter", "" ) };
		if( !nameFilter.empty() ){
			pSensor->Reference( "name", nameFilter );
			pSensor->SetEffect( pSensor->GetEffect() | SensorFilterJack::name );
		}

		std::string connectSignal;
		std::string connectSwitch;
		const int zuStellung = pt.get( "<xmlattr>.zuStellung", 0 );
		if( pt.get( "<xmlattr>.Wechseln", false ) ){
			connectSignal = "toggle";
			connectSwitch = "toggle";
		}
		else if( zuStellung == 1 ){
			connectSignal = "1";
			connectSwitch = "go";
		}
		else if( zuStellung == 2 ){
			connectSignal = "2";
			connectSwitch = "branch";
		}
		else if( zuStellung == 3 ){
			connectSignal = "3";
			connectSwitch = "cobranch";
		}
		else{
			connectSignal = pt.get( "<xmlattr>.zuStellung", "0" );
			connectSwitch = "";
		}


		// create timer or counter here for 
		// <xmlattr>.Delay / <xmlattr>.ActDelay
		// and
		// <xmlattr>.Teiler / <xmlattr>.ActTeiler
		// and connect the sensor. 
		// <xmlattr>.KontaktZiel / Signal::Status / Switch::Status then goes to
		// the logic element.
		// it is possible, to have both elements present...

		int teiler = pt.get( "<xmlattr>.Teiler", 1 );
		if( teiler != 1 ){
			if( std::shared_ptr<PulseCounter> pPulseCounter = PulseCounter::Make() ){
				pPulseCounter->Threshold( teiler );
				pPulseCounter->Counter( pt.get( "<xmlattr>.ActTeiler", 0 ) );
				pulseCounterCollection.Add( pPulseCounter );
				pSensor->JackOnTrigger().Insert( &pPulseCounter->PlugToCountUp() );
				pPulseCounter->RegisterSockets( m_SocketRegistry );

				if( Real delay = pt.get( "<xmlattr>.Delay", 0.f ) ){
					if( std::shared_ptr<Timer> pTimer = Timer::Make() ){
						pTimer->Lapse( _s(delay) );
						if( Real actDelay = 1000 * pt.get( "<xmlattr>.ActDelay", 0.f ) )
							// Timer is initially started and was at certain time
						{
							pTimer->SetTime( _ms(actDelay) );
							pTimer->Start();
						}

						timerCollection.Add( pTimer );
						pPulseCounter->JackOnReachThreshold().Insert( &pTimer->PlugToStart() );
						pTimer->RegisterSockets( m_SocketRegistry );
						pTimer->JackOnTick().Reference( "KontaktZiel", pt.get( "<xmlattr>.KontaktZiel", "0" ) );
						pTimer->JackOnTick().Reference( "Signal::Status", connectSignal );
						pTimer->JackOnTick().Reference( "Switch::Status", connectSwitch );
					}
				}
				else{
					pPulseCounter->JackOnReachThreshold().Reference( "KontaktZiel", pt.get( "<xmlattr>.KontaktZiel", "0" ) );
					pPulseCounter->JackOnReachThreshold().Reference( "Signal::Status", connectSignal );
					pPulseCounter->JackOnReachThreshold().Reference( "Switch::Status", connectSwitch );
				}
			}
		}
		else if( const Real delay = pt.get( "<xmlattr>.Delay", 0.f ) ){
			if( std::shared_ptr<Timer> pTimer = Timer::Make() ){
				pTimer->Lapse( _s(delay) );
				if( const Real actDelay = 1000 * pt.get( "<xmlattr>.ActDelay", 0.f ) )
					// Timer is initially started and was at certain time
				{
					pTimer->SetTime( _ms(actDelay) );
					pTimer->Start();
				}

				timerCollection.Add( pTimer );
				pSensor->JackOnTrigger().Insert( &pTimer->PlugToStart() );
				pTimer->RegisterSockets( m_SocketRegistry );
				pTimer->JackOnTick().Reference( "KontaktZiel", pt.get( "<xmlattr>.KontaktZiel", "0" ) );
				pTimer->JackOnTick().Reference( "Signal::Status", connectSignal );
				pTimer->JackOnTick().Reference( "Switch::Status", connectSwitch );
			}
		}
		else{
			pSensor->JackOnTrigger().Reference( "KontaktZiel", pt.get( "<xmlattr>.KontaktZiel", "0" ) );
			pSensor->JackOnTrigger().Reference( "Signal::Status", connectSignal );
			pSensor->JackOnTrigger().Reference( "Switch::Status", connectSwitch );
		}

		pSensor->RegisterSockets( m_SocketRegistry );

		trackLocation.parameter = _cm(pt.get( "<xmlattr>.Position", 0.0f ));
		trackLocation.orientation = Orientation{ pt.get( "<xmlattr>.ParaOderAnti", true ) };

		return pSensor;
	 }

	return nullptr;
}

Track::EndType Anl3TrackSystemReader::From( const std::string& name ) const{
	if( name == "Anfang" )
		return Track::EndType::front;
	else if(	name == "Ende" ||
				name == "EndeAbzweig" ||
				name == "EndeKoAbzweig" )
		return Track::EndType::end;

	throw std::runtime_error( "Unknown track end type in *.anl file: " + name );
}

Signal::Status Anl3TrackSystemReader::SignalStatusFromEEP( int stellung, const Signal& signal ) const{
	if( stellung < 0 )
		return Signal::Status::none;

	// It is dependent from the signal model, what the
	// number in stellung actually means. We copied the 
	// mapping to the signal's references and restore the 
	// list here.
	std::vector<int> functionMap;
	for( int i = 0; ; ++i ){
		std::string stellungstr( signal.Reference( std::to_string( i ) ) );
		if( !stellungstr.empty() )
			functionMap.push_back( std::stoi( stellungstr ) );
		else break;
	}

	if( stellung >= static_cast<int>(functionMap.size()) )
		return Signal::Status::none;

	const int EEPStellung = functionMap[stellung];

	//int EEPStellung = stellung;
	//std::string stellungstr( signal.Reference( std::to_string( stellung ) ) );
	//if( !stellungstr.empty() )
	//	EEPStellung = std::stoi( stellungstr );

	switch( EEPStellung )
		// now map EEP's 'real' status to trax's
	{
	case 0:
		return Signal::Status::none;
	case 1:
		return Signal::Status::clear;
	case 2:
		return Signal::Status::stop;
	default:
		int velLimit = 0;
		for( int i = 0; i != stellung; ++i ){
			if( functionMap[i] == 0 ||
				functionMap[i] == 1 ||
				functionMap[i] == 2 )
				continue;

			++velLimit;
		}

		return static_cast<Signal::Status>(static_cast<int>(Signal::Status::velocity2) + velLimit);
	}
}

Switch::Status Anl3TrackSystemReader::SwitchStatusFromEEP( int weichenstellung ) const noexcept{
	switch( weichenstellung ){
	case 1:
		return Switch::Status::go;
	case 2:
		return Switch::Status::branch;
	default:
		return Switch::Status::none;
	}
}

ThreeWaySwitch::Status Anl3TrackSystemReader::ThreeWaySwitchStatusFromEEP( int weichenstellung ) const noexcept{
	switch( weichenstellung ){
	case 1:
		return ThreeWaySwitch::Status::go;
	case 2:
		return ThreeWaySwitch::Status::branch1;
	case 3:
		return ThreeWaySwitch::Status::branch2;
	default:
		return ThreeWaySwitch::Status::none;
	}
}

SingleSlipSwitch::Status Anl3TrackSystemReader::SingleSlipSwitchStatusFromEEP( int weichenstellung ) const noexcept{
	switch( weichenstellung ){
	case 2:
		return SingleSlipSwitch::Status::go1;
	case 3:
		return SingleSlipSwitch::Status::branch;
	case 4:
		return SingleSlipSwitch::Status::go2;
	default:
		return SingleSlipSwitch::Status::none;
	};
}

DoubleSlipSwitch::Status Anl3TrackSystemReader::DoubleSlipSwitchStatusFromEEP( int weichenstellung ) const noexcept{
	switch( weichenstellung ){
	case 1:
		return DoubleSlipSwitch::Status::branch1;
	case 2:
		return DoubleSlipSwitch::Status::go1;
	case 3:
		return DoubleSlipSwitch::Status::branch2;
	case 4:
		return DoubleSlipSwitch::Status::go2;
	default:
		return DoubleSlipSwitch::Status::none;
	};
}

void Anl3TrackSystemReader::ReadDreibein( const boost::property_tree::ptree& pt, Frame<Length,One>& frame ) const{
	const auto iter = pt.find( "Dreibein" );
	if( iter != pt.not_found() ){
		const boost::property_tree::ptree& ptc = (*iter).second;

		Vector<One> Vektors[4];
		int i = 0;
		// iterate over Vektors:
		for( const auto& pair : ptc )
		{
			if( pair.first == "Vektor" ){
				Vektors[i].dx = pair.second.get( "<xmlattr>.x", 0.0f );
				Vektors[i].dy = pair.second.get( "<xmlattr>.y", 0.0f );
				Vektors[i].dz = pair.second.get( "<xmlattr>.z", 0.0f );
				++i;
			}
		}

		frame.P = Origin3D<Length> + Vektors[0] * Length{units_per_meter / 100};
		frame.T = Vektors[1];
		frame.N = Vektors[2];
		frame.B = Vektors[3];
	}
	else
		frame.Init();
}
///////////////////////////////////////
std::shared_ptr<trax::TrackSystem> ReadTrackSystemFromANL3( 
	const ptreesupport::PTreeReader& reader, 
	const boost::property_tree::ptree& pt )
{
	for( const auto& pair : pt )
	{
		if( pair.first == "sutrackp" )
		{
			return reader.ReadTrackSystem( pair.second );
		}
	}

	return nullptr;
}
///////////////////////////////////////
}
}
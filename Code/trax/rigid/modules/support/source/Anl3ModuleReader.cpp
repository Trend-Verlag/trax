// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#include "../Anl3ModuleReader.h"

#include "trax/rigid/modules/Camera.h"
#include "trax/rigid/modules/ModuleCollection.h"
#include "trax/rigid/modules/Module.h"


#include "trax/collections/ConnectorCollection.h"
#include "trax/collections/IndicatorCollection.h"
#include "trax/collections/PulseCounterCollection.h"
#include "trax/collections/SignalCollection.h"
#include "trax/collections/TimerCollection.h"
#include "trax/collections/TrackCollection.h"
#include "trax/collections/TrackCollectionContainer.h"
#include "trax/collections/TrackSystem.h"

#include "trax/Plug.h"

namespace trax{
namespace ptreesupport{

Anl3ModuleReader::Anl3ModuleReader( 
	const char * pLocale )
	: Anl3TrackSystemReader( pLocale )
{
}

Anl3ModuleReader::Anl3ModuleReader( 
	SocketRegistry& socketRegistry, 
	const char* pLocale )
	: Anl3TrackSystemReader( socketRegistry, pLocale )
{
}

std::unique_ptr<ModuleCollection> Anl3ModuleReader::ReadModuleCollection( 
	const boost::property_tree::ptree& pt ) const
{
	if( std::unique_ptr<ModuleCollection> pModuleCollection = ModuleCollection::Make(); pModuleCollection )
	{
		pModuleCollection->Add( ReadModule( pt ) );
		return pModuleCollection;
	}

	return nullptr;
}

std::unique_ptr<Module> Anl3ModuleReader::ReadModule( 
	const boost::property_tree::ptree& pt ) const
{
	// check version tag:
	const auto viter = pt.find( "Version" );
	if( viter != pt.not_found() )
	{
		auto version = (*viter).second.get( "<xmlattr>.EEP", 0.0f );
		if( version < 2.0f )
			throw std::runtime_error( "*.anl file is of wrong version" );
	}
	else
		throw std::runtime_error( "*.anl file has no version tag." );

	if( std::unique_ptr<Module> pModule = Module::Make(); pModule ){
		pModule->Attach( IndicatorCollection::Make() );
		pModule->Attach( TimerCollection::Make() );
		pModule->Attach( PulseCounterCollection::Make() );

		if( std::unique_ptr<SignalCollection> pSignalCollection = SignalCollection::Make(); pSignalCollection ){
			pModule->Attach( std::move( pSignalCollection ) );
			if( std::shared_ptr<TrackSystem> pTrackSystem = TrackSystem::Make(); pTrackSystem ){
				pModule->Attach( pTrackSystem );

				std::map<IDType,Velocity> travelVelocities;
				std::vector<int> trackIDOffsets;
				IDType maxSensorID{ 0 };
				std::vector<Kollektor> kollektors;
				std::vector<std::pair<Track::Coupling,std::string>> couplings;
				for( const auto& pair : pt ){
					if( pair.first == "Gleissystem" ){
						std::vector<std::pair<Track::Coupling,std::string>> local_couplings;
						if( auto pTempTrackSystem = CreateTrackSystem( pair.second, local_couplings, *pModule->GetSignalCollection(), *pModule->GetIndicatorCollection(), *pModule->GetTimerCollection(), *pModule->GetPulseCounterCollection(), travelVelocities, maxSensorID ) ){
							if( std::shared_ptr<TrackCollection> pTrackCollection = TrackCollection::Make(); pTrackCollection ){
								pModule->GetTrackSystem()->GetCollectionContainer()->Add( pTrackCollection );

								trackIDOffsets.push_back( pModule->GetTrackSystem()->Take( *pTempTrackSystem ) );
								for( auto& coupling : local_couplings ){
									coupling.first.theOne.id = coupling.first.theOne.id + trackIDOffsets.back();
									coupling.first.theOther.id = coupling.first.theOther.id + trackIDOffsets.back();
								}
								couplings.insert( couplings.end(), local_couplings.begin(), local_couplings.end() );
							}
						}
					}

					else if( pair.first == "Kollektor" ){
						Kollektor kollektor;
						kollektor.m_ID = pair.second.get( "<xmlattr>.id", IDType{} );
						for( const auto& pair2 : pair.second ){
							if( pair2.first == "Dreibein" )
								ReadDreibein( pair.second, kollektor.m_Frame );

							else if( pair2.first == "Gleis" )
								kollektor.m_Tracks.push_back( pair2.second.get( "<xmlattr>.gleisID", IDType{} ) + trackIDOffsets[static_cast<std::size_t>(pair2.second.get( "<xmlattr>.gleissystemID", 1 ))-1] );
						}
						
						kollektors.push_back( kollektor );
					}

					//else if( pair.first == "Fuhrpark" )
					//	pModule->Attach( EEPCreateFleet( pair.second, *pSimulator, *pModule->GetTrackSystem(), travelVelocities, trackIDOffsets ) );

					//else if( pair.first == "Goods" )
					//	pModule->Attach( EEPCreateBatch( pair.second, *pSimulator ) );

					else if( pair.first == "Gebaeudesammlung" )
						ReadGebaeudesammlung( pair.second, couplings, kollektors, *pModule->GetTrackSystem(), *pModule->GetIndicatorCollection() );

					else if( pair.first == "Kammerasammlung" )
						pModule->Attach( ReadCameraCollection( pair.second ) );

					else if( pair.first == "Settings" ){
						if( auto pCameraCollection = pModule->GetCameraCollection() )
							ReadUserCamera( pair.second, *pCameraCollection );
						else
							std::cerr << "Import error: No cameras available in anl3 - file!" << std::endl;
					}

					else if( pair.first == "Beschreibung" ){
						pModule->Reference( "Description_unknown", pair.second.data() );
						pModule->Reference( "Description", pair.second.data() );
					}

					// further collections:

					// ...	

					else if( pair.first == "Schandlaft" ){
						pModule->SetVolume( { 
							_cm(pair.second.get( "<xmlattr>.posX", 0.0f )),
							_cm(pair.second.get( "<xmlattr>.posY", 0.0f )),
							_cm(pair.second.get( "<xmlattr>.h_min", 0.0f )),
							_cm((pair.second.get( "<xmlattr>.posX", 0.0f ) + pair.second.get( "<xmlattr>.extX", 0.0f ) / pair.second.get( "<xmlattr>.dichte", 1.0f ))),
							_cm((pair.second.get( "<xmlattr>.posY", 0.0f ) + pair.second.get( "<xmlattr>.extY", 0.0f ) / pair.second.get( "<xmlattr>.dichte", 1.0f ))), 
							_cm(pair.second.get( "<xmlattr>.h_max", 0.0f )) 
						} );

						//if( auto pTerrain = EEPCreateTerrain( pair.second, *pSimulator ) ){
						//	const auto iter = pt.find( "Weather" );
						//	if( iter != pt.not_found() )
						//		ReadEEPWeather( iter->second, *pTerrain );

						//	pModule->Attach( std::move(pTerrain) );
						//}
					}
				}

				//if( pModule->GetFleet() == nullptr )
				//	pModule->Attach( m_Factory.CreateFleet() ); // need some empty fleet

				try{
					ConnectSockets( *pModule );
				}
				catch( const std::invalid_argument& e ){
					std::cerr << "Could not properly connect the Sockets: " << e.what() << std::endl;
				}

			//	pModule->Loaded( true );
				return pModule;
			}
		}
	}

	return nullptr;
}

std::unique_ptr<CameraCollection> Anl3ModuleReader::ReadCameraCollection(
	const boost::property_tree::ptree& pt ) const
{
	if( auto pCameraCollection = CameraCollection::Make() )
	{
		for( const auto& pair : pt ){
			if( pair.first == "Kammera" )
				pCameraCollection->Add( ReadCamera( pair.second ) );
		}

		return pCameraCollection;
	}

	return nullptr;
}

std::unique_ptr<Camera> Anl3ModuleReader::ReadCamera( const boost::property_tree::ptree& pt ) const
{
	if( pt.get( "<xmlattr>.name", "Leer" ) == "Leer" )
		return nullptr;
	if( pt.get( "<xmlattr>.name", "blank" ) == "blank" )
		return nullptr;

	if( auto pCamera = Camera::Make() )
	{
		pCamera->Reference( "name", pt.get( "<xmlattr>.name", "" ) );

		spat::Frame<Length,One> frame;
		ReadDreibein( pt, frame );
		frame.OrthoNormalize();
		pCamera->SetFrame( frame );

		return pCamera;
	}

	return nullptr;
}

void Anl3ModuleReader::ReadUserCamera( 
	const boost::property_tree::ptree& pt, 
	CameraCollection& cameraCollection ) const
{
	if( std::shared_ptr<Camera> pCamera = Camera::Make() )
	{
		pCamera->Reference( "name", "PlayerCamera" );

		spat::Frame<Real> frame( 
			{	{	pt.get( "<xmlattr>.ActCamera_POSX", 6000.0f ),
					pt.get( "<xmlattr>.ActCamera_POSY", -6000.0f ),
					pt.get( "<xmlattr>.ActCamera_POSZ", 400.0f ) },
				{	pt.get( "<xmlattr>.ActCamera_DIRX", -6000.0f ),
					pt.get( "<xmlattr>.ActCamera_DIRY", 6000.0f ),
					pt.get( "<xmlattr>.ActCamera_DIRZ", -400.0f ) },
				{	pt.get( "<xmlattr>.ActCamera_NORX", -0.7071068f ),
					pt.get( "<xmlattr>.ActCamera_NORY", -0.7071068f ),
					pt.get( "<xmlattr>.ActCamera_NORZ", .0f ) },
				{	pt.get( "<xmlattr>.ActCamera_BINX", -3.329636E-02f ),
					pt.get( "<xmlattr>.ActCamera_BINY", 3.329636E-02f ),
					pt.get( "<xmlattr>.ActCamera_BINZ", 0.9988908f ) } } 
		);

		frame.P *= 0.01f;
		frame.OrthoNormalize();
		pCamera->SetFrame( spat::spatial_cast<Length,One>(frame) );

		cameraCollection.UserStartCameraID( cameraCollection.Add( pCamera ) );
	}
}

void Anl3ModuleReader::ReadGebaeudesammlung(
	const boost::property_tree::ptree& pt,
	const std::vector<std::pair<Track::Coupling,std::string>>& couplings,
	const std::vector<Kollektor>& kollektors,
	const TrackSystem& trackSystem,
	IndicatorCollection& indicatorCollection) const
{
	for( const auto& pair : pt ){
		if( pair.first == "Immobile" )
		{
			if( pair.second.get( "<xmlattr>.DKWType", 0 ) == 1 ||
			    pair.second.get( "<xmlattr>.DKWType", 0 ) == 11 )
			// it is a double slip switch...
			{
				if( std::shared_ptr<DoubleSlipSwitch> pDoubleSlipSwitch = DoubleSlipSwitch::Make(); pDoubleSlipSwitch ){
					AttributesToReferences( pair.second, *pDoubleSlipSwitch );
					pDoubleSlipSwitch->Set( DoubleSlipSwitchStatusFromEEP( pair.second.get( "<xmlattr>.weichenstellung", 0 )), false );

					const int kollektorID = pair.second.get( "<xmlattr>.kollektorID", 0 );
					const auto iter = std::find_if( kollektors.begin(), kollektors.end(), 
						[&kollektorID]( const auto& kollektor ) noexcept { return kollektor.m_ID == kollektorID; } );
					if( iter != kollektors.end() ){
						pDoubleSlipSwitch->SetCenter( iter->m_Frame );

						if( iter->m_Tracks.size() != 8 )
							std::cerr << "Number of tracks for double-slip-switch " << pair.second.get( "<xmlattr>.gsbname", "" ) << " is: " << iter->m_Tracks.size() 
							<< ", but should be 8. Switch might not import correctly!" << std::endl;

						if( iter->m_Tracks.size() >= 4 )
						{
							pDoubleSlipSwitch->Slot( DoubleSlipSwitch::SlotNames::slot_0, trackSystem.Get( iter->m_Tracks[0] ), Track::EndType::end, true );
							pDoubleSlipSwitch->Slot( DoubleSlipSwitch::SlotNames::slot_1, trackSystem.Get( iter->m_Tracks[2] ), Track::EndType::end, true );
							pDoubleSlipSwitch->Slot( DoubleSlipSwitch::SlotNames::slot_2, trackSystem.Get( iter->m_Tracks[3] ), Track::EndType::end, true );
							pDoubleSlipSwitch->Slot( DoubleSlipSwitch::SlotNames::slot_3, trackSystem.Get( iter->m_Tracks[1] ), Track::EndType::end, true );

							std::pair<Track::End,Track::End> ends = GetEnds( *pDoubleSlipSwitch, DoubleSlipSwitch::SlotNames::slot_0, couplings );
							pDoubleSlipSwitch->Slot( DoubleSlipSwitch::SlotNames::slot_4, trackSystem.Get( ends.first.id ), ends.first.type, true );
							pDoubleSlipSwitch->Slot( DoubleSlipSwitch::SlotNames::slot_8, trackSystem.Get( ends.second.id ), ends.second.type, true );

							ends = GetEnds( *pDoubleSlipSwitch, DoubleSlipSwitch::SlotNames::slot_1, couplings );
							pDoubleSlipSwitch->Slot( DoubleSlipSwitch::SlotNames::slot_5, trackSystem.Get( ends.first.id ), ends.first.type, true );
							pDoubleSlipSwitch->Slot( DoubleSlipSwitch::SlotNames::slot_11, trackSystem.Get( ends.second.id ), ends.second.type, true );
				
							ends = GetEnds( *pDoubleSlipSwitch, DoubleSlipSwitch::SlotNames::slot_2, couplings );
							pDoubleSlipSwitch->Slot( DoubleSlipSwitch::SlotNames::slot_6, trackSystem.Get( ends.first.id ), ends.first.type, true );
							pDoubleSlipSwitch->Slot( DoubleSlipSwitch::SlotNames::slot_10, trackSystem.Get( ends.second.id ), ends.second.type, true );

							ends = GetEnds( *pDoubleSlipSwitch, DoubleSlipSwitch::SlotNames::slot_3, couplings );
							pDoubleSlipSwitch->Slot( DoubleSlipSwitch::SlotNames::slot_7, trackSystem.Get( ends.first.id ), ends.first.type, true );
							pDoubleSlipSwitch->Slot( DoubleSlipSwitch::SlotNames::slot_9, trackSystem.Get( ends.second.id ), ends.second.type, true );
						}

						if( !pDoubleSlipSwitch->Check(std::cerr, 30_cm) )
							std::cerr << "trax: a double slip switch's slots are not fully populated or show gaps or kinks or twists: " << pDoubleSlipSwitch->Reference( "ImmoIdx" ) 
								<< " " << pair.second.get( "<xmlattr>.gsbname", "" ) << ". DKWType: " << pair.second.get( "<xmlattr>.DKWType", 0 ) << std::endl;
					}

					if( std::shared_ptr<Indicator> pSwitchIndicator = Indicator::Make( Indicator::Type::switch_multi ); pSwitchIndicator ){
						pSwitchIndicator->Reference( "reference", pair.second.get( "<xmlattr>.gsbname", "" ) );

						pSwitchIndicator->Set( static_cast<trax::Indicator::Status>( pair.second.get( "<xmlattr>.weichenstellung", 0 ) ), false );

						if( auto pAligned = dynamic_cast<SwitchAligned*>(pSwitchIndicator.get()) ){
							pAligned->PreserveUpDirection( true );
							pAligned->Attach( pDoubleSlipSwitch );
						}

						for( int i = 1; i <= static_cast<int>(DoubleSlipSwitch::Status::count); ++i ){
							pSwitchIndicator->JackOn( static_cast<trax::Indicator::Status>(i) ).InsertAtTail( &pDoubleSlipSwitch->PlugTo( DoubleSlipSwitchStatusFromEEP( i ) ).Unplugged( &m_SocketRegistry ) );
							pDoubleSlipSwitch->JackOn( DoubleSlipSwitchStatusFromEEP( i ) ).InsertAtTail( &pSwitchIndicator->PlugTo( static_cast<trax::Indicator::Status>(i) ) );
						}

						indicatorCollection.Add( pSwitchIndicator );
						pSwitchIndicator->RegisterSockets( m_SocketRegistry );
					}

					pDoubleSlipSwitch->RegisterSockets( m_SocketRegistry );
					trackSystem.GetConnectorCollection()->Add( std::move(pDoubleSlipSwitch) );
				}
			}
			else if( pair.second.get( "<xmlattr>.DKWType", 0 ) == 2 ||
						pair.second.get( "<xmlattr>.DKWType", 0 ) == 12 )
			// it is a single slip switch...
			{
				if( std::shared_ptr<SingleSlipSwitch> pSingleSlipSwitch = SingleSlipSwitch::Make(); pSingleSlipSwitch )
				{
					AttributesToReferences( pair.second, *pSingleSlipSwitch );
					pSingleSlipSwitch->Set( SingleSlipSwitchStatusFromEEP( pair.second.get( "<xmlattr>.weichenstellung", 0 ) ), false );
		
					int kollektorID = pair.second.get( "<xmlattr>.kollektorID", 0 );
					auto iter = std::find_if( kollektors.begin(), kollektors.end(), 
						[&kollektorID]( const auto& kollektor ) noexcept { return kollektor.m_ID == kollektorID; } );
					if( iter != kollektors.end() ){
						pSingleSlipSwitch->SetCenter( iter->m_Frame );

						if( iter->m_Tracks.size() != (pair.second.get( "<xmlattr>.DKWType", 0 ) == 2 ? 7u : 5u) )
							std::cerr << "Number of tracks for single-slip-switch " << pair.second.get( "<xmlattr>.gsbname", "" ) << " is: " << iter->m_Tracks.size() 
							<< ", but should be " << (pair.second.get( "<xmlattr>.DKWType", 0 ) == 2 ? 7 : 5) << ". Switch might not import correctly!" << std::endl;

						if( iter->m_Tracks.size() >= 2 )
						{
							pSingleSlipSwitch->Slot( SingleSlipSwitch::SlotNames::slot_0, trackSystem.Get( iter->m_Tracks[1] ), Track::EndType::end, true );
							pSingleSlipSwitch->Slot( SingleSlipSwitch::SlotNames::slot_1, trackSystem.Get( iter->m_Tracks[0] ), Track::EndType::end, true );
		
							std::pair<Track::End,Track::End> ends = GetEnds( *pSingleSlipSwitch, SingleSlipSwitch::SlotNames::slot_0, couplings );
							pSingleSlipSwitch->Slot( SingleSlipSwitch::SlotNames::slot_2, trackSystem.Get( ends.first.id ), ends.first.type, true );
							pSingleSlipSwitch->Slot( SingleSlipSwitch::SlotNames::slot_4, trackSystem.Get( ends.second.id ), ends.second.type, true );

							ends = GetEnds( *pSingleSlipSwitch, SingleSlipSwitch::SlotNames::slot_1, couplings );
							pSingleSlipSwitch->Slot( SingleSlipSwitch::SlotNames::slot_3, trackSystem.Get( ends.first.id ), ends.first.type, true );
							pSingleSlipSwitch->Slot( SingleSlipSwitch::SlotNames::slot_5, trackSystem.Get( ends.second.id ), ends.second.type, true );
						}

						if( !pSingleSlipSwitch->Check(std::cerr, 30_cm) )
							std::cerr << "trax: a single slip switch's slots are not fully populated or show gaps or kinks or twists: " << pSingleSlipSwitch->Reference( "ImmoIdx" ) 
								<< " " << pair.second.get( "<xmlattr>.gsbname", "" ) << ". DKWType: " << pair.second.get( "<xmlattr>.DKWType", 0 ) << std::endl;
					}

					if( std::shared_ptr<Indicator> pSwitchIndicator = Indicator::Make( Indicator::Type::switch_multi ) ){
						pSwitchIndicator->Reference( "reference", pair.second.get( "<xmlattr>.gsbname", "" ) );

						pSwitchIndicator->Set( static_cast<trax::Indicator::Status>( pair.second.get( "<xmlattr>.weichenstellung", 0 ) ), false );

						if( auto pAligned = dynamic_cast<SwitchAligned*>(pSwitchIndicator.get()) ){
							pAligned->PreserveUpDirection( true );
							pAligned->Attach( pSingleSlipSwitch );
						}

						for( int i = 2; i <= 4; ++i ){
							pSwitchIndicator->JackOn( static_cast<trax::Indicator::Status>(i) ).InsertAtTail( &pSingleSlipSwitch->PlugTo( SingleSlipSwitchStatusFromEEP( i ) ).Unplugged( &m_SocketRegistry ) );
							pSingleSlipSwitch->JackOn( SingleSlipSwitchStatusFromEEP( i ) ).InsertAtTail( &pSwitchIndicator->PlugTo( static_cast<trax::Indicator::Status>(i) ) );
						}

						indicatorCollection.Add( pSwitchIndicator );
						pSwitchIndicator->RegisterSockets( m_SocketRegistry );
					}

					pSingleSlipSwitch->RegisterSockets( m_SocketRegistry );
					trackSystem.GetConnectorCollection()->Add( std::move(pSingleSlipSwitch) );
				}
			}
			else
				/*structureCollection.AddRelaxed( EEPCreateStructure( pair.second, simulator, kollektors, trackSystem ) )*/;
		}
	}
}

std::pair<Track::End,Track::End> Anl3ModuleReader::GetEnds( 
	const Connector& connector, 
	int slot,
	const std::vector<std::pair<Track::Coupling,std::string>>& couplings ) const
{
	std::pair<Track::End,Track::End> ends;
	for( const auto& coupling : couplings )
	{
		if( coupling.first.theOne.id == connector.Slot( slot ).first->ID() )
		{
			if( coupling.first.theOne.type == Track::EndType::end )
			{
				if( coupling.second == "Ende" )
					ends.first = coupling.first.theOther;
				else if( coupling.second == "EndeAbzweig" )
					ends.second = coupling.first.theOther;
				else
					std::cerr << "Invalid EEP track end type for double or single slip switch: " << coupling.second << std::endl;
			}
		}
	}

	return ends;
}

void Anl3ModuleReader::ConnectSockets( Module& modul ) const{
//	if( auto pJackEnumerator = decorator_cast<JackEnumerator*>(&modul) ){
	if( auto pJackEnumerator = dynamic_cast<JackEnumerator*>(&modul) ){
		for( auto& jack : *pJackEnumerator ){
			if( !jack.Reference( "KontaktZiel" ).empty() ){
				if( auto pSignalCollection = modul.GetSignalCollection() ){
					for( auto& signal : *pSignalCollection ){
						if( jack.Reference( "KontaktZiel" ) == signal.Reference( "KontaktZiel" ) ){
							if( Signal::Status status = SignalStatusFromEEP( std::stoi( jack.Reference( "Signal::Status" ) ), signal ); status != Signal::Status::none ){
								jack.InsertAtTail( &signal.PlugTo( status ).Unplugged( &modul ) );
								continue;
							}
						}
					}
				}

				if( const auto pTrackSystem = modul.GetTrackSystem() ){
					if( auto pConnectorCollection = pTrackSystem->GetConnectorCollection() ){
						for( auto& connector : *pConnectorCollection ){
							if( auto pSwitch = dynamic_cast<Switch*>(&connector) ){
								if( jack.Reference( "KontaktZiel" ) == pSwitch->NarrowTrack().first->Reference( "KontaktZiel" ) ){
									jack.InsertAtTail( &pSwitch->PlugTo( ToNarrowSwitchStatus( jack.Reference( "Switch::Status" ) ) ).Unplugged( &modul ) );
									continue;
								}
							}
						}
					}
				}
			}
		}
	}
	
	// signal and switch connections:
	if( auto pSignalCollection = modul.GetSignalCollection() ){
		if( const auto pTrackSystem = modul.GetTrackSystem() ){
			if( auto pConnectorCollection = pTrackSystem->GetConnectorCollection() )
			{
				for( auto& firstSignal : *pSignalCollection ){
					if( !firstSignal.Reference( "SignalData" ).empty() ){
						for( auto& secondSignal : *pSignalCollection ){
							if( firstSignal.Reference( "SignalData" ) == secondSignal.Reference( "Key_Id" ) )
							{
								if( !firstSignal.Reference( "Count" ).empty() ){
									const int count = std::stoi( firstSignal.Reference( "Count" ) ); 
									for( int fromFunction = 1; fromFunction <= count; ++fromFunction ){
										std::ostringstream stream;
										stream << "Fn";
										if( fromFunction < 9 )
											stream << '0';
										stream << fromFunction;

										if( const int toFunction = std::stoi( firstSignal.Reference( stream.str() ) ) )
											if( Signal::Status statusFrom = SignalStatusFromEEP( fromFunction, firstSignal ); statusFrom != Signal::Status::none )
												if( Signal::Status statusTo = SignalStatusFromEEP( toFunction, secondSignal ); statusTo != Signal::Status::none )
													firstSignal.JackOn( statusFrom ).InsertAtTail( &secondSignal.PlugTo( statusTo ).Unplugged( &modul ) );
									}
								}

								break;
							}
						}

						for( auto& secondConnector : *pConnectorCollection ){
							if( Switch* pSwitch = dynamic_cast<Switch*>(&secondConnector) ){
								if( firstSignal.Reference( "SignalData" ) == pSwitch->Reference( "Key_Id" ) )
								{
									if( !firstSignal.Reference( "Count" ).empty() ){
										const int count = std::stoi( firstSignal.Reference( "Count" ) );
										for( int fromFunction = 1; fromFunction <= count; ++fromFunction ){
											std::ostringstream stream;
											stream << "Fn";
											if( fromFunction < 9 )
												stream << '0';
											stream << fromFunction;

											if( const int toFunction = std::stoi( firstSignal.Reference( stream.str() ) ) )
												if( Signal::Status statusFrom = SignalStatusFromEEP( fromFunction, firstSignal ); statusFrom != Signal::Status::none )
													firstSignal.JackOn( statusFrom ).InsertAtTail( &pSwitch->PlugTo( SwitchStatusFromEEP( toFunction ) ).Unplugged( &modul ) );
										}
									}

									break;
								}
							}
						}
					}
				}

				for( auto& firstConnector : *pConnectorCollection ){
					if( Switch* pFirstSwitch = dynamic_cast<Switch*>(&firstConnector) ){
						if( !pFirstSwitch->Reference( "SignalData" ).empty() ){
							for( auto& secondSwitch : *pConnectorCollection ){
								if( Switch* pSecondSwitch = dynamic_cast<Switch*>(&secondSwitch) ){
									if( pFirstSwitch->Reference( "SignalData" ) == pSecondSwitch->Reference( "Key_Id" ) )
									{
										for( int fromFunction = 1; fromFunction <= 5; ++fromFunction ){
											std::ostringstream stream;
											stream << "NextSigFn";
											stream << fromFunction;

											if( const int toFunction = std::stoi( pFirstSwitch->Reference( stream.str() ) ) )
												pFirstSwitch->JackOn( SwitchStatusFromEEP( fromFunction ) ).
													InsertAtTail( &pSecondSwitch->PlugTo( SwitchStatusFromEEP( toFunction ) ).Unplugged( &modul ) );
										}

										break;
									}
								}
							}

							for( auto& secondSignal : *pSignalCollection ){
								if( pFirstSwitch->Reference( "SignalData" ) == secondSignal.Reference( "Key_Id" ) ){
									for( int fromFunction = 1; fromFunction <= 5; ++fromFunction ){
										std::ostringstream stream;
										stream << "NextSigFn";
										stream << fromFunction;

										if( const int toFunction = std::stoi( pFirstSwitch->Reference( stream.str() ) ) )
											if( Signal::Status statusTo = SignalStatusFromEEP( toFunction, secondSignal ); statusTo != Signal::Status::none )
												pFirstSwitch->JackOn( SwitchStatusFromEEP( fromFunction ) ).InsertAtTail( &secondSignal.PlugTo( statusTo ).Unplugged( &modul ) );
									}

									break;
								}
							}
						}
					}
				}
			}
		}
	}
}

} // namespace ptreesupport
} // namespace trax

// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel



#include "trax/rigid/modules/support/Anl4ModuleReader.h"
#include "spat/support/SpatSupportXML.h"

#include "trax/rigid/Gestalt.h"
#include "trax/rigid/modules/Camera.h"
#include "trax/rigid/modules/Module.h"
#include "trax/rigid/modules/ModuleCollection.h"
#include "trax/rigid/trains/RollingStock.h"
#include "trax/rigid/trains/Train.h"
#include "trax/rigid/trains/WheelFrame.h"
#include "trax/rigid/trains/collections/Fleet.h"

#include "trax/collections/SignalCollection.h"
#include "trax/collections/IndicatorCollection.h"
#include "trax/collections/PulseCounterCollection.h"
#include "trax/collections/TimerCollection.h"
#include "trax/collections/TrackSystem.h"

namespace trax{
namespace ptreesupport{

	using namespace dim::ptreesupport;
	using namespace spat;
	using namespace spat::ptreesupport;


Anl4ModuleReader::Anl4ModuleReader( 
	Scene& scene,
	const char* pLocale )
	: Anl4TrackSystemReader{ pLocale }
	, m_Scene{ scene }
{
}

Anl4ModuleReader::Anl4ModuleReader( 
	Scene& scene,
	SocketRegistry& socketRegistry, 
	const char* pLocale )
	: Anl4TrackSystemReader{ socketRegistry, pLocale }
	, m_Scene{ scene }
{
}

std::unique_ptr<ModuleCollection> Anl4ModuleReader::ReadModuleCollection( const boost::property_tree::ptree& pt ) const
{
	if( std::unique_ptr<ModuleCollection> pModuleCollection = trax::ModuleCollection::Make(); pModuleCollection )
	{
		for( const auto& pair : pt )
		{
			if( pair.first == "Module" )
				pModuleCollection->Add( ReadModule( pair.second ) );
		}

		return pModuleCollection;
	}

	return nullptr;
}

std::unique_ptr<Module> Anl4ModuleReader::ReadModule( const boost::property_tree::ptree& pt ) const
{
	if( std::unique_ptr<Module> pModule = trax::Module::Make(); pModule )
	{
		pModule->ID( pt.get( "<xmlattr>.id", 0 ) );
		pModule->Reference( "name", pt.get( "<xmlattr>.name", "" ) );
		pModule->Reference( "clearWallet", pt.get( "<xmlattr>.clearWallet", "false" ) );
		pModule->Reference( "noAlternativeSteuerdialog", pt.get( "<xmlattr>.noAlternativeSteuerdialog", "false" ) );
	//	pModule->NoReload( pt.get( "<xmlattr>.noReload", false ) );

		//// Load the terrain prior to the other stuff, because it might
		//// allocate huge chuncs of memory and these might not fit into
		//// memory after it is fragmented with all the tiny allocations.
		//auto iterTerrain = pt.find( "Terrain" );
		//if (iterTerrain != pt.not_found()) {
		//	//if (message) message("Terrain");
		//	pModule->Attach(CreateTerrain(iterTerrain->second, *pSimulator));
		//}

		//if (message) message( "Terrain loaded" );

		for( const auto& pair : pt )
		{
			//if (message) message(pair.first);

			if( pair.first == "DisplayName" )
				ReadLocalizedTag( pair.second, "DisplayName", *pModule, GetLocale() );

			else if( pair.first == "Description" )
				ReadLocalizedTag( pair.second, "Description", *pModule, GetLocale() );

			else if( pair.first == "Area" ){
				Box<Length> area;
				ReadArea( pair.second, area );
				pModule->SetVolume( area );
			}

			else if( pair.first == "Frame" ){
				Frame<Length,One> frame;
				ReadFrame( pair.second, frame );
				pModule->SetFrame( frame );
			}

			else if( pair.first == "TrackSystem" )
				pModule->Attach( CreateTrackSystem( pair.second ) );

			//else if( pair.first == "Batch" && pSimulator )
			//	pModule->Attach( CreateBatch( pair.second, *pModule ) );

			else if( pair.first == "Fleet" )
				pModule->Attach( CreateFleet( pair.second ) );

			else if( pair.first == "SignalCollection" && pModule->GetTrackSystem() /*&& pModule->GetFleet()*/ )
				pModule->Attach( CreateSignalCollection( pair.second, *pModule->GetTrackSystem()/*, *pModule->GetFleet()*/ ) );

			else if( pair.first == "IndicatorCollection" && pModule->GetTrackSystem() && pModule->GetTrackSystem()->GetConnectorCollection() && pModule->GetSignalCollection() )
				pModule->Attach( CreateIndicatorCollection(
					pair.second, 
					*pModule->GetTrackSystem()->GetConnectorCollection(),
					*pModule->GetSignalCollection()
					) );

			else if( pair.first == "PulseCounterCollection" )
				pModule->Attach( CreatePulseCounterCollection( pair.second ) );

			else if( pair.first == "TimerCollection" )
				pModule->Attach( CreateTimerCollection( pair.second ) );
		}

		return pModule;
	}

	return nullptr;
}

std::unique_ptr<Module>dclspc Anl4ModuleReader::ReadModule( const boost::property_tree::ptree & pt, int idx ) const
{
	int moduleNum = 0;
	for( const auto& pair : pt )
	{
		if( pair.first == "Module" && ++moduleNum == idx )
			return ReadModule( pair.second );
	}

	return nullptr;
}

std::unique_ptr<CameraCollection> Anl4ModuleReader::CreateCameraCollection( 
	const boost::property_tree::ptree& pt ) const
{
	if( auto pCameraCollection = CameraCollection::Make() )
	{
		for( const auto& pair : pt )
		{
			if( pair.first == "Camera" )
				pCameraCollection->Add( CreateCamera( pair.second ) );
		
			//else if( pair.first == "TriggerShape" )
			//	pCameraCollection->AttachTriggerShape( CreateTriggerShape( pair.second, registry ) );
		}

		pCameraCollection->UserCameraType( CameraTypeFrom( pt.get( "<xmlattr>.userCameraType", "firstperson" ) ) );
		pCameraCollection->UserStartCameraID( pt.get( "<xmlattr>.userStartCamera", 1 ) );

		return pCameraCollection;
	}

	return nullptr;
}

std::unique_ptr<Camera> Anl4ModuleReader::CreateCamera( 
	const boost::property_tree::ptree& pt ) const
{
	if( auto pCamera = Camera::Make( CameraTypeFrom( pt.get( "<xmlattr>.type", "basic" ) ) ); pCamera ){
		pCamera->Reference( "name", pt.get( "<xmlattr>.name", "" ) );
		pCamera->ID( pt.get( "<xmlattr>.id", 0 ) );
		pCamera->SetFromFofy(
			pt.get( "<xmlattr>.fovy", pCamera->GetType() == CameraType::cam_cabin ? 3*pi/8 : Camera::default_fovy ),
			pt.get( "<xmlattr>.aspectRatio", Camera::default_aspect ),
			get( pt, "<xmlattr>.nearPlane", pCamera->GetType() == CameraType::cam_cabin ? 0.3_m : Camera::default_nearPlane, _m ),
			get( pt, "<xmlattr>.farPlane", Camera::default_farPlane, _m ) );

		for( const auto& pair : pt )
		{
			if( pair.first == "Frame" ){
				Frame<Length,One> frame;
				ReadFrame( pair.second, frame );
				pCamera->SetFrame( frame );
			}

			//else if( pair.first == "StructureList" ){
			//	if( auto pStructureList = dynamic_cast<StructureList*>(&*pCamera) )
			//		ReadStructureList( pair.second, *pStructureList, structureCollection );
			//}
		}

		//if( auto pBodycam = dynamic_cast<BodyCam*>(pCamera.get()) ){
		//	if( auto pBody = std::dynamic_pointer_cast<Body>(fleet.Get( pt.get( "<xmlattr>.railRunnerName", "unknown" ) )); pBody )
		//		pBodycam->AttachTo( pBody );
		//}

		return pCamera;
	}

	return nullptr;
}

std::shared_ptr<Fleet> Anl4ModuleReader::CreateFleet( const boost::property_tree::ptree& pt ) const
{
	if( auto pFleet = Fleet::Make() )
	{
		for( const auto& pair : pt )
		{
			if( pair.first == "Train" )
				pFleet->Add( CreateTrain( pair.second ).first );
		}

		return pFleet;
	}

	return nullptr;
}

std::pair<std::shared_ptr<Train>,Orientation> Anl4ModuleReader::CreateTrain( const boost::property_tree::ptree& pt ) const
{
	if( std::shared_ptr<Train> pTrain = Train::Make(); pTrain )
	{
		AttributesToReferences( pt, *pTrain );

		for( const auto& pair : pt )
		{
			if( pair.first == "Train" ){
				if( auto [pChildTrain, orientation] = CreateTrain( pair.second ); pChildTrain )
					pTrain->Append( EndType::south, pChildTrain, orientation );
			}

			else if( pair.first == "RollingStock" ){
				if( auto [pRollingStock, orientation] = CreateRollingStock( pair.second ); pRollingStock )
					pTrain->Append( EndType::south, pRollingStock, orientation );
			}
		}

		return { pTrain, ToOrientation( pTrain->Reference( "orientation" ) ) };
	}

	return { nullptr, Orientation::Value::none };
}

std::pair<std::shared_ptr<RollingStock>,Orientation> Anl4ModuleReader::CreateRollingStock( const boost::property_tree::ptree& pt ) const
{
	if( std::shared_ptr<RollingStock> pRollingStock = RollingStock::Make(); pRollingStock )
	{
		AttributesToReferences( pt, *pRollingStock );
		std::vector<std::shared_ptr<Bogie>> topmostBogies;

		for( const auto& pair : pt )
		{
			if( pair.first == "Bogie" ){
				if( std::shared_ptr<Bogie> pChildBogie = CreateBogie( pair.second ); pChildBogie )
					topmostBogies.push_back( pChildBogie );
			}

			else if( pair.first == "WheelFrame" ){
				if( std::shared_ptr<WheelFrame> pChildWheelFrame = CreateWheelFrame( pair.second ); pChildWheelFrame )
					topmostBogies.push_back( pChildWheelFrame );
			}
		}

		if( !topmostBogies.empty() )
		{
			// connect the adjacent south child bogies to their south parents ...
			for( size_t i = 0; i < topmostBogies.size() - 1; ++i )
				topmostBogies[i]->Attach( topmostBogies[i + 1]->GetChild( EndType::north ).first, EndType::north );

			pRollingStock->Attach( *topmostBogies.front() );
			return { pRollingStock, ToOrientation( pRollingStock->Reference( "orientation" ) ) };
		}
	}

	return { nullptr, Orientation::Value::none };
}

void Anl4ModuleReader::ReadBogie( const boost::property_tree::ptree& pt, Bogie& bogie ) const
{
	AttributesToReferences( pt, bogie );
	bogie.TargetVelocity( pt.get( "<xmlattr>.target_velocity", 0_kmIh ) );
	bogie.Thrust( pt.get( "<xmlattr>.thrust", 0_1 ) );
	bogie.Brake( pt.get( "<xmlattr>.brake", 0_1 ) );
	if( pt.get( "<xmlattr>.couplingNorthActivated", false ) )
		bogie.ActivateCoupling( EndType::north );
	if( pt.get( "<xmlattr>.couplingSouthActivated", false ) )
		bogie.ActivateCoupling( EndType::south );

	std::shared_ptr<Bogie> pChildNorthBogie, pChildSouthBogie;

	for( const auto& pair : pt )
	{
		if( pair.first == "Frame" ){
			Frame<Length,One> frame;
			ReadFrame( pair.second, frame );
			bogie.GetGestalt().SetFrame( frame );
		}

		else if( pair.first == "Velocity" ){
			Vector<Velocity> velocity;
			ReadVector( pair.second, velocity );
			bogie.GetGestalt().SetLinearVelocity( velocity );
		}

		else if( pair.first == "AngularVelocity" ){
			Vector<AngularVelocity> angularVelocity;
			ReadVector( pair.second, angularVelocity );
			bogie.GetGestalt().SetAngularVelocity( angularVelocity );
		}

		else if( pair.first == "Bogie" ){
			pChildNorthBogie ? pChildSouthBogie : pChildNorthBogie = CreateBogie( pair.second );
		}

		else if( pair.first == "WheelFrame" ){
			pChildNorthBogie ? pChildSouthBogie : pChildNorthBogie = CreateWheelFrame( pair.second );
		}
	}

	if( pChildNorthBogie )
		bogie.Attach( pChildNorthBogie, EndType::north );

	if( pChildSouthBogie )
		bogie.Attach( pChildSouthBogie, EndType::south );
}

std::shared_ptr<Bogie> Anl4ModuleReader::CreateBogie( const boost::property_tree::ptree& pt ) const
{
	if( std::shared_ptr<Bogie> pBogie = Bogie::Make( m_Scene ); pBogie )
	{
		ReadBogie( pt, *pBogie );
		return pBogie;
	}

	return nullptr;
}

std::shared_ptr<WheelFrame> Anl4ModuleReader::CreateWheelFrame( const boost::property_tree::ptree& pt ) const
{
	if( std::shared_ptr<WheelFrame> pWheelFrame = WheelFrame::Make( m_Scene ); pWheelFrame )
	{
		ReadBogie( pt, *pWheelFrame );
		return pWheelFrame;
	}

	return nullptr;
}

} // namespace ptreesupport
} // namespace trax
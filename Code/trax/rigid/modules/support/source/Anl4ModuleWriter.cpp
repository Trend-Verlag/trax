// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#include "../Anl4ModuleWriter.h"

#include "trax/rigid/modules/Module.h"
#include "trax/rigid/modules/ModuleCollection.h"
#include "trax/rigid/modules/Camera.h"

#include "trax/collections/IndicatorCollection.h"
#include "trax/collections/PulseCounterCollection.h"
#include "trax/collections/TimerCollection.h"
#include "trax/rigid/trains/Train.h"
#include "trax/rigid/trains/collections/Fleet.h"

#include "trax/rigid/trains/support/Anl4RailRunnerWriter.h"
#include "trax/collections/support/Anl4TrackSystemWriter.h"

namespace trax{
namespace ptreesupport{

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const ModuleCollection& moduleCollection ){
	boost::property_tree::ptree pttraxML;
	pttraxML.add<std::string>( "<xmlattr>.xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" );
	pttraxML.add<std::string>( "<xmlattr>.xsi:noNamespaceSchemaLocation", "https://www.trendverlag.com/Schema/traxML.xsd" );

	for( const auto& module : moduleCollection )
		pttraxML << module;

	move_child( pt, "traxML", pttraxML );
	return pt;
}

boost::property_tree::ptree& operator << ( boost::property_tree::ptree& pt, const Module& _module )
{
	boost::property_tree::ptree ptModule;
	ReferencesToAttributes( ptModule, _module );
	ptModule.add( "<xmlattr>.id", _module.ID() );
	ptModule.add( "<xmlattr>.maxValidPlugID", _module.MaxValidPlugID() );

	WriteLocalizedTag( ptModule, "DisplayName", _module );
	WriteLocalizedTag( ptModule, "Description", _module );

	spat::Box<Length> area{_module.GetVolume()};
	area.Move( spat::Origin3D<Length> - _module.GetFrame().P );

	ptModule << area;

	ptModule << _module.GetFrame();

	if( const auto pTrackSystem = _module.GetTrackSystem(); pTrackSystem )
		ptModule << *pTrackSystem;

	//if( const auto* pBatch = _module.GetBatch(); pBatch )
	//	ptModule << *pBatch;

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

	ptCameraCollection.add( "<xmlattr>.userCameraType", From( cameraCollection.UserCameraType() ) );
	ptCameraCollection.add( "<xmlattr>.userStartCamera", cameraCollection.UserStartCameraID() );

	for( const auto& camera : cameraCollection )
		ptCameraCollection << camera;

	//ptCameraCollection << static_cast<const trax::TriggerShapeCollection&>(cameraCollection);

	move_child( pt, cameraCollection.TypeName(), ptCameraCollection );
	return pt;
}

boost::property_tree::ptree& operator<<( boost::property_tree::ptree& pt, const Camera& camera ){
	boost::property_tree::ptree ptCamera;
	ReferencesToAttributes( ptCamera, camera );
	ptCamera.add( "<xmlattr>.id", camera.ID() );
	ptCamera.add( "<xmlattr>.type", From( camera.GetType() ) );
	ptCamera << camera.GetFrame();

	//if( auto pBodyCam = dynamic_cast<const trax::BodyCam*>(&camera) )
	//	if( auto pRailRunner = std::dynamic_pointer_cast<const RailRunner>(pBodyCam->GetBody()); pRailRunner )
	//		ptCamera.add( "<xmlattr>.railRunnerName", pRailRunner->Reference( "name" ) );

	move_child( pt, camera.TypeName(), ptCamera );
	return pt;
}

} // namespace ptreesupport
} // namespace trax
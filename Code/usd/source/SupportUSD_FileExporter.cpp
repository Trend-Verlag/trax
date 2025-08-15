// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#include "usd/SupportUSD_FileExporter.h"

namespace trax{

USDFileExporter::USDFileExporter( 
	const std::filesystem::path& filePath, 
	const pxr::SdfPath& atPath, 
	double metersPerUnit,
	bool bWriteTrackMesh )
	: USDExporter{ pxr::UsdStage::CreateNew( filePath.generic_string() ), atPath, bWriteTrackMesh }
{
	if( !GetStage() )
		throw std::runtime_error{ "railOmniverse: Invalid stage" };

	// Set the current time to the start of the track system
	GetStage()->SetStartTimeCode( 0.0 );
	GetStage()->SetEndTimeCode( 0.0 );
	GetStage()->SetTimeCodesPerSecond( 60.0 );

	// Set the metadata
	//GetStage()->SetMetadata( pxr::TfToken("defaultPrim"), pxr::TfToken("World") );
	GetStage()->SetMetadata( pxr::TfToken("defaultPrim"), atPath.GetNameToken() );
	GetStage()->SetMetadata( pxr::UsdGeomTokens->metersPerUnit, pxr::VtValue(metersPerUnit) );
	GetStage()->SetMetadata( pxr::UsdGeomTokens->upAxis, pxr::VtValue("Z") );
}

}
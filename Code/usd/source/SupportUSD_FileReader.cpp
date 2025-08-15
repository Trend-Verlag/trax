// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#include "usd/SupportUSD_FileReader.h"


namespace trax{

USDFileReader::USDFileReader( const std::filesystem::path& filePath )
	: USDReader{ pxr::UsdStage::Open( filePath.generic_string() ) }
{
    if( GetStage() == nullptr )
		throw std::runtime_error( "USDFileReader: Could not open stage." );
}

}
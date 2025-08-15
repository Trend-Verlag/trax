// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#pragma once

#include "SupportUSD_Exporter.h"

#include <filesystem>

namespace trax{

	class USDFileExporter : public USDExporter{
	public:
		USDFileExporter( const std::filesystem::path& filePath, const pxr::SdfPath& atPath, double metersPerUnit = 1.0, bool bWriteTrackMesh = false );



	};

}

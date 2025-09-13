//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
// 
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

#include "../RollingStockFileReader.h"

#include "../RailRunnerParser.h"

#include <iostream>

namespace trax{
///////////////////////////////////////
FileReader::FileReader( std::filesystem::path basePath ) noexcept
	: m_BasePath{ basePath }
{
}

bool FileReader::MakeValidPath( std::filesystem::path& path ) const noexcept
{
	if( !std::filesystem::exists( path ) ){
		path = GetBasePath() / path.relative_path();
		if( !std::filesystem::exists( path ) )
			return false;
	}

	return true;
}
///////////////////////////////////////
RollingStockFileReader_Base::RollingStockFileReader_Base( RollingStockParser& parser, std::filesystem::path basePath ) noexcept
	: FileReader{ basePath }
	, m_Parser	{ parser }
{
}
///////////////////////////////////////
RollingStockFileReader::RollingStockFileReader( RollingStockParser& parser, std::filesystem::path basePath ) noexcept
	: RollingStockFileReader_Base{ parser, basePath }
{
}

bool RollingStockFileReader::Read( std::filesystem::path filePath )
{
	if( MakeValidPath( filePath ) ){
		ParseRollingStock( filePath, m_Parser );
		return true;
	}
	
	std::cerr << "File not found: " << filePath << std::endl;
	return false;
}
///////////////////////////////////////
} // namespace trax
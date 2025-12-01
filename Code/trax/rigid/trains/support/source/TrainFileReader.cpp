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
// For further information, please contact: horstmann.marc@trendverlag.de

#include "../TrainFileReader.h"
#include "../RailRunnerParser.h"
#include "../RollingStockCreator.h"

#include <iostream>

namespace trax{
///////////////////////////////////////
TrainFileReader::TrainFileReader( TrainParser& trainParser, std::filesystem::path basePath ) noexcept
	: FileReader				{ basePath }
	, m_Parser					{ trainParser }
{
}

bool TrainFileReader::Read( std::filesystem::path filePath )
{
	if( MakeValidPath( filePath ) ){
		ParseTrain( filePath, m_Parser );
		return true;
	}
	
	std::cerr << "File not found: " << filePath << std::endl;
	return false;
}
///////////////////////////////////////
TrainFileReferenceReader::TrainFileReferenceReader( 
		Scene& scene,
		std::filesystem::path basePath ) noexcept
	: TrainFileReader			{ *this, basePath }
	, TrainCreator				{ scene }
	, m_RollingStockReader		{ *this, basePath }
{
}

std::shared_ptr<Train> TrainFileReferenceReader::GetTrain() const noexcept
{
	return TrainCreator::GetTrain();
}

bool TrainFileReferenceReader::TrainStart( 
	IDType id, 
	const std::string& name, 
	const std::string& reference, 
	Velocity travel_velocity, 
	Velocity velocity, 
	One trainThrust, 
	One trainBrake, 
	int32_t credit ) noexcept(false)
{
	if( TrainCreator::TrainStart( id, name, reference, travel_velocity, velocity, trainThrust, trainBrake, credit ) )
	{
		if( !reference.empty() ){
			Read( reference );
			return false;
		}

		return true;
	}

	return false;
}

RollingStockParser* TrainFileReferenceReader::AxisArrangementStart( 
	const std::string& name, 
	const std::string& reference ) noexcept(false)
{
	if( reference.empty() )
		return TrainCreator::AxisArrangementStart( name, reference );

	m_RollingStockReader.Read( reference );
	return nullptr;
}

RollingStockParser* TrainFileReferenceReader::RollingStockStart( 
	const std::string& name, 
	const std::string& reference ) noexcept(false)
{
	if( reference.empty() )
		return TrainCreator::RollingStockStart( name, reference );

	m_RollingStockReader.Read( reference );
	return nullptr;
}
///////////////////////////////////////
} // namespace trax

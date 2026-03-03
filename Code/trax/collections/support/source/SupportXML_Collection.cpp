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
// For further information, please contact: horstmann.marc@trendverlag.de

#include "trax/collections/support/CollectionSupportXML.h"
#include "common/support/CommonSupportXML.h"

#include "trax/RoadwayTwist.h"
#include "trax/SocketRegistry.h"

#include <map>

namespace trax{
namespace ptreesupport{

using namespace common::ptreesupport;
using namespace spat;
using namespace spat::ptreesupport;

using common::ptreesupport::operator<<;
using spat::ptreesupport::operator<<;

///////////////////////////////////////
void RegisterAllCurveReaders( PTreeReader& toReader )
{
    toReader.RegisterCurveReader( TypeToName( Curve::CurveType::Line ), CreateLine );
    toReader.RegisterCurveReader( TypeToName( Curve::CurveType::LineP ), CreateLineP );
    toReader.RegisterCurveReader( TypeToName( Curve::CurveType::Arc ), CreateArc );
    toReader.RegisterCurveReader( TypeToName( Curve::CurveType::ArcP ), CreateArcP );
    toReader.RegisterCurveReader( TypeToName( Curve::CurveType::Helix ), CreateHelix );
    toReader.RegisterCurveReader( TypeToName( Curve::CurveType::HelixP ), CreateHelixP );
    toReader.RegisterCurveReader( TypeToName( Curve::CurveType::Cubic ), CreateCubic );
    toReader.RegisterCurveReader( TypeToName( Curve::CurveType::Spline ), CreateSpline );
    toReader.RegisterCurveReader( TypeToName( Curve::CurveType::Clothoid ), CreateClothoid );
    toReader.RegisterCurveReader( TypeToName( Curve::CurveType::Rotator ), CreateRotator );
    toReader.RegisterCurveReader( TypeToName( Curve::CurveType::RotatorChain ), CreateRotatorChain );
    toReader.RegisterCurveReader( TypeToName( Curve::CurveType::PolygonalChain ), CreatePolygonalChain );
    toReader.RegisterCurveReader( TypeToName( Curve::CurveType::SampledCurve ), CreateSampledCurve );
	toReader.RegisterCurveReader( TypeToName( Curve::CurveType::EEPCurve ), CreateEEPCurve );
}

void RegisterAllTwistReaders( class PTreeReader& toReader )
{
    toReader.RegisterTwistReader( TypeToName( RoadwayTwist::TwistType::Constant ), CreateConstantTwist );
    toReader.RegisterTwistReader( TypeToName( RoadwayTwist::TwistType::Linear ), CreateLinearTwist );
    toReader.RegisterTwistReader( TypeToName( RoadwayTwist::TwistType::Piecewise ), CreatePiecewiseTwist );
    toReader.RegisterTwistReader( TypeToName( RoadwayTwist::TwistType::PiecewiseLinear ), CreatePiecewiseLinearTwist );
    toReader.RegisterTwistReader( TypeToName( RoadwayTwist::TwistType::PiecewiseCircular ), CreatePiecewiseCircularTwist );
    toReader.RegisterTwistReader( TypeToName( RoadwayTwist::TwistType::Directional ), CreateDirectionalTwist );
}
///////////////////////////////////////
// std::map has no export interface, so have it static to the library
// by using the pimpl idiom.
class PTreeReaderBaseImpl{
public:
	std::map<std::string,PTreeReader::CurveReaderFunction> m_CurveReaders;
	std::map<std::string,PTreeReader::TwistReaderFunction> m_TwistReaders;
};
///////////////////////////////////////
PTreeReader::PTreeReader( const char* pLocale )
	: m_pSocketRegistry	{ SocketRegistry::Make() }
	, m_pImpl			{ new PTreeReaderBaseImpl{} }
	, m_pLocale			{ pLocale }
	, m_SocketRegistry	{ *m_pSocketRegistry }
{
}

PTreeReader::PTreeReader( SocketRegistry& socketRegistry, const char* pLocale )
	: m_pImpl			{ new PTreeReaderBaseImpl{} }
	, m_pLocale			{ pLocale }
	, m_SocketRegistry	{ socketRegistry }
{
}

//PTreeReader::PTreeReader( const PTreeReader& reader )
//	: m_pImpl			{ new PTreeReaderBaseImpl{} }
//	, m_pLocale			{ reader.m_pLocale }
//	, m_pSocketRegistry	{ reader.m_pSocketRegistry ? SocketRegistry::Make() : nullptr }
//	, m_SocketRegistry	{ reader.m_pSocketRegistry ? *m_pSocketRegistry : reader.m_pSocketRegistry }
//{
//	*m_pImpl = *reader.m_pImpl;
//	m_SocketRegistry = reader.m_SocketRegistry;
//}

PTreeReader::PTreeReader( PTreeReader&& reader ) noexcept
	: m_pSocketRegistry	{ std::move(reader.m_pSocketRegistry) }
	, m_pImpl			{ reader.m_pImpl }
	, m_pLocale			{ reader.m_pLocale }
	, m_SocketRegistry	{ reader.m_SocketRegistry }
{
	reader.m_pImpl = nullptr;
	reader.m_pLocale = nullptr;
}

PTreeReader::~PTreeReader(){
	if( m_pImpl )
		delete m_pImpl;
}

//PTreeReader& PTreeReader::operator=( const PTreeReader& reader )
//{
//	*m_pImpl = *reader.m_pImpl;
//	return *this;
//}
//
//PTreeReader& PTreeReader::operator=( PTreeReader&& reader ) noexcept
//{
//	if( m_pImpl )
//		delete m_pImpl;
//
//	m_pImpl = reader.m_pImpl;
//	reader.m_pImpl = nullptr;
//	m_pLocale = reader.m_pLocale;
//	reader.m_pLocale = nullptr;
//	return *this;
//}

void PTreeReader::RegisterCurveReader( const std::string& typeName, CurveReaderFunction readerFunc )
{
	m_pImpl->m_CurveReaders[typeName] = readerFunc;
}

void PTreeReader::UnregisterCurveReader( const std::string& typeName )
{
	m_pImpl->m_CurveReaders[typeName] = nullptr;
}

void PTreeReader::RegisterTwistReader( const std::string& typeName, TwistReaderFunction readerFunc )
{
	m_pImpl->m_TwistReaders[typeName] = readerFunc;
}

void PTreeReader::UnregisterTwistReader( const std::string& typeName )
{
	m_pImpl->m_TwistReaders[typeName] = nullptr;
}

std::unique_ptr<Curve> PTreeReader::CreateCurve(
	const boost::property_tree::ptree& pt ) const
{
	try{
		for( const auto& pair : pt )
		{
			if( auto iter = m_pImpl->m_CurveReaders.find( pair.first ); iter != m_pImpl->m_CurveReaders.end() && iter->second )
			{
				return (*iter->second)( pair.second );
			}
		}
	}
	catch( const std::exception& e ){
		std::cerr << "PTreeReader::CreateCurve: " << e.what() << std::endl;
	}

	return nullptr;
}

std::unique_ptr<RoadwayTwist> PTreeReader::CreateTwist( 
	const boost::property_tree::ptree & pt ) const
{
	for( const auto& pair : pt )
	{
		if( auto iter = m_pImpl->m_TwistReaders.find( pair.first ); iter != m_pImpl->m_TwistReaders.end() && iter->second )
		{
			return (*iter->second)( pair.second );
		}

		if( pair.first == "CombinedTwist" )
			return CreateCombinedTwist( pair.second );
	}

	return nullptr;
}

std::unique_ptr<RoadwayTwist> PTreeReader::CreateCombinedTwist( const boost::property_tree::ptree& pt ) const
{
	if( auto pTwist = CombinedTwist::Make() )
	{
		std::vector<std::unique_ptr<RoadwayTwist>> twists;
		twists.reserve(2);

		for( const auto& pair : pt )
		{
			if( pair.first == "Twist" )
				twists.push_back( CreateTwist( pair.second ) );
		}

		if( twists.size() > 0 && twists.at(0) )
			pTwist->AttachTwist1( std::move(twists.at(0)) );

		if( twists.size() > 1 && twists.at(1) )
			pTwist->AttachTwist2( std::move(twists.at(1)) );

		return pTwist;
	}

	return nullptr;
}
///////////////////////////////////////
} // namespace ptreesupport
} // namespace trax
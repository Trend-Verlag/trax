// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


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
	: m_pImpl			{ reader.m_pImpl }
	, m_pLocale			{ reader.m_pLocale }
	, m_SocketRegistry	{ reader.m_SocketRegistry }
	, m_pSocketRegistry	{ std::move(reader.m_pSocketRegistry) }
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
void Read( const boost::property_tree::ptree& pt, Cubic::Data& data ){
	std::vector<spat::Vector<Length>> vectors;
	vectors.reserve( 3 );

	for( const auto& pair : pt )
	{
		if( pair.first == "Position" )
			ReadPosition( pair.second, data.a );

		else if( pair.first == "Vector" ){
			vectors.push_back( {} );
			ReadVector( pair.second, vectors.back() );
		}
	}

	if( vectors.size() > 0 )
		data.b = vectors.at(0);

	if( vectors.size() > 1 )
		data.c = vectors.at(1);

	if( vectors.size() > 2 )
		data.d = vectors.at(2);
}

void Read( const boost::property_tree::ptree& pt, CurveSample& sample ){
	sample.s = pt.get( "<xmlattr>.s", sample.s );
	sample.k = pt.get( "<xmlattr>.k", sample.k );
	sample.t = pt.get( "<xmlattr>.t", sample.t );

	for( const auto& pair : pt ){
		if( pair.first == "Frame" )
			ReadFrame( pair.second, sample.F );
	}
}

void Read( const boost::property_tree::ptree& pt, EEPCurve::Data& data ){
	data.gc_Kruemmung = pt.get( "<xmlattr>.Kruemmung", data.gc_Kruemmung );
	data.gc_Windung = pt.get( "<xmlattr>.Torsion", data.gc_Windung );
	data.gc_Verdrillung = pt.get( "<xmlattr>.Fuehrungsverdrehung", data.gc_Verdrillung );
	data.gc_Laenge = pt.get( "<xmlattr>.Laenge", data.gc_Laenge );
	data.gc_Kurve = pt.get( "<xmlattr>.Kurve", data.gc_Kurve );
	data.m_FuehrungsVerdrehung = pt.get( "<xmlattr>.Anfangsfuehrungsverdrehung", data.m_FuehrungsVerdrehung );

	for( const auto& pair : pt )
	{
		if( pair.first == "Frame" )
			ReadFrame( pair.second, data.m_AnfangsBein );
	}
}
///////////////////////////////////////
} // namespace ptreesupport
} // namespace trax
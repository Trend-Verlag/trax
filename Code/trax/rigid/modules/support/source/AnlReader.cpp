// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2024
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#include "../AnlReader.h"
#include "trax/rigid/modules/Module.h"
#include "trax/rigid/modules/ModuleCollection.h"

#include "trax/rigid/modules/support/Anl3ModuleReader.h"
#include "trax/rigid/modules/support/Anl4ModuleReader.h"

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable: 6313) //  Incorrect operator:  zero-valued flag cannot be tested with bitwise-and.  Use an equality test to check for zero-valued flags.
#endif
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

namespace trax{

AnlReader::AnlReader( Scene& scene, const char* pLocale )
	: m_pSocketRegistry	{ SocketRegistry::Make() }
	, m_pLocale			{ pLocale }
	, m_SocketRegistry	{ *m_pSocketRegistry }
	, m_Scene			{ scene }
{
}

AnlReader::AnlReader( Scene& scene, SocketRegistry& socketRegistry, const char* pLocale )
	: m_pLocale			{ pLocale }
	, m_SocketRegistry	{ socketRegistry }
	, m_Scene			{ scene }
{
}

std::unique_ptr<ModuleCollection> ReadModuleCollection( Scene& scene, const boost::property_tree::ptree& ptr )
{
	for( const auto& pair : ptr )
	{
		if( pair.first == "traxML" ){
			ptreesupport::Anl4ModuleReader reader{ scene };
			RegisterAllCurveReaders( reader );
			RegisterAllTwistReaders( reader );
			return reader.ReadModuleCollection( pair.second );
		}
		else if( pair.first == "sutrackp" ){
			ptreesupport::Anl3ModuleReader reader;
			RegisterAllCurveReaders( reader );
			RegisterAllTwistReaders( reader );
			return reader.ReadModuleCollection( pair.second );
		}
	}

	return nullptr;
}

std::unique_ptr<ModuleCollection> AnlReader::ReadModuleCollection( const std::filesystem::path& path ) const
{
	boost::property_tree::ptree ptr;

	read_xml( path.string(), ptr );

	return trax::ReadModuleCollection( m_Scene, ptr );
}

std::unique_ptr<ModuleCollection> AnlReader::ReadModuleCollection( const unsigned char* bufferStart, const unsigned char* bufferEnd ) const
{
	std::string string( (const char*)bufferStart, bufferEnd - bufferStart );
	std::istringstream stream( string );

	boost::property_tree::ptree ptr;

	read_xml( stream, ptr );

	return trax::ReadModuleCollection( m_Scene, ptr );
}

std::unique_ptr<Module> ReadModule( Scene& scene, const boost::property_tree::ptree& ptr, int atIdx ){

	for( const auto& pair : ptr )
	{
		if( pair.first == "traxML" ){
			ptreesupport::Anl4ModuleReader reader{ scene };
			RegisterAllCurveReaders( reader );
			RegisterAllTwistReaders( reader );
			return reader.ReadModule( pair.second, atIdx );
		}
		else if( pair.first == "sutrackp" ){
			ptreesupport::Anl3ModuleReader reader;
			RegisterAllCurveReaders( reader );
			RegisterAllTwistReaders( reader );
			return reader.ReadModule( pair.second );
		}
	}

	return nullptr;
}

std::unique_ptr<Module> AnlReader::ReadModule( const std::filesystem::path& path, int atIdx ) const
{
	boost::property_tree::ptree ptr;

	read_xml( path.string(), ptr );

	return trax::ReadModule( m_Scene, ptr, atIdx );
}

std::unique_ptr<Module> AnlReader::ReadModule( const unsigned char* bufferStart, const unsigned char* bufferEnd, int atIdx ) const
{
	std::string string( (const char*)bufferStart, bufferEnd - bufferStart );
	std::istringstream stream( string );

	boost::property_tree::ptree ptr;

	read_xml( stream, ptr );

	return trax::ReadModule( m_Scene, ptr, atIdx );
}

static std::shared_ptr<TrackSystem> ReadTrackSystem( const boost::property_tree::ptree& ptr, int atIdx )
{
	for( const auto& pair : ptr )
	{
		if( pair.first == "traxML" ){
			ptreesupport::Anl4TrackSystemReader readerBase;
			RegisterAllCurveReaders( readerBase );
			RegisterAllTwistReaders( readerBase );
			return ReadTrackSystemFromANL4( readerBase, ptr, atIdx );
		}
		else if( pair.first == "sutrackp" ){
			ptreesupport::Anl3TrackSystemReader readerBase;
			RegisterAllCurveReaders( readerBase );
			RegisterAllTwistReaders( readerBase );
			return ReadTrackSystemFromANL3( readerBase, ptr );
		}
	}

	return nullptr;
}

std::shared_ptr<TrackSystem> AnlReader::ReadTrackSystem( const std::filesystem::path& path, int atIdx )
{	
	boost::property_tree::ptree ptr;

	read_xml( path.string(), ptr );

	return trax::ReadTrackSystem( ptr, atIdx );
}

std::shared_ptr<TrackSystem> AnlReader::ReadTrackSystem( const unsigned char* bufferStart, const unsigned char* bufferEnd, int atIdx )
{
	std::string string( (const char*)bufferStart, bufferEnd - bufferStart );
	std::istringstream stream( string );

	boost::property_tree::ptree ptr;

	read_xml( stream, ptr );

	return trax::ReadTrackSystem( ptr, atIdx );
}

} // namespace trax
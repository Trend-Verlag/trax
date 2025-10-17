//	trax track library
//	AD 2025 
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
// For additional permissions, please contact: horstmann.marc@trendverlag.de


#include "../TrackSystemReader.h"
#include "../Anl3TrackSystemReader.h"
#include "../Anl4TrackSystemReader.h"


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

TrackSystemReader::TrackSystemReader( const char * pLocale )
	: m_pSocketRegistry	{ SocketRegistry::Make() }
	, m_pLocale			{ pLocale }
	, m_SocketRegistry	{ *m_pSocketRegistry }
{
}

TrackSystemReader::TrackSystemReader( SocketRegistry& socketRegistry, const char * pLocale )
	: m_pLocale			{ pLocale }
	, m_SocketRegistry	{ socketRegistry }
{
}

static std::shared_ptr<TrackSystem> ReadTrackSystem( const boost::property_tree::ptree& ptr, int atIdx )
{
	for( const auto& pair : ptr )
	{
		if( pair.first == "traxML" ){
			ptreesupport::Anl4TrackSystemReader readerBase;
			ptreesupport::RegisterAllCurveReaders( readerBase );
			ptreesupport::RegisterAllTwistReaders( readerBase );
			return ptreesupport::ReadTrackSystemFromANL4( readerBase, ptr, atIdx );
		}
		else if( pair.first == "sutrackp" ){
			ptreesupport::Anl3TrackSystemReader readerBase;
			ptreesupport::RegisterAllCurveReaders( readerBase );
			ptreesupport::RegisterAllTwistReaders( readerBase );
			return ptreesupport::ReadTrackSystemFromANL3( readerBase, ptr );
		}
	}

	return nullptr;
}

std::shared_ptr<TrackSystem>dclspc TrackSystemReader::Read( const std::filesystem::path& fromPath, int atIdx ) const
{
	boost::property_tree::ptree ptr;

	boost::property_tree::read_xml( fromPath.string(), ptr );

	return trax::ReadTrackSystem( ptr, atIdx );
}

std::shared_ptr<TrackSystem>dclspc TrackSystemReader::Read( const unsigned char* bufferStart, const unsigned char * bufferEnd, int atIdx ) const
{
	std::string string( (const char*)bufferStart, bufferEnd - bufferStart );
	std::istringstream stream( string );

	boost::property_tree::ptree ptr;

	read_xml( stream, ptr );

	return trax::ReadTrackSystem( ptr, atIdx );
}

}
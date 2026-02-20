//	trax track library
//	AD 2026 
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

#include "trax/rigid/modules/support/ModuleCollectionXMLWriter.h"

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


bool XMLWriteModuleCollection( const boost::property_tree::ptree& pt, ModuleCollectionParser& callback ) noexcept(false)
{
	for( const auto& pair : pt )
	{


	}

	return false;
}

bool XMLWriteModuleCollection( const unsigned char* bufferStart, const unsigned char* bufferEnd, ModuleCollectionParser& callback ) noexcept(false)
{
	std::string string{ (const char*)bufferStart, (const char*)(bufferEnd - bufferStart) };
	std::istringstream stream{ string };

	boost::property_tree::ptree ptr;

	read_xml( stream, ptr );

	return XMLWriteModuleCollection( ptr, callback );
}

bool XMLWriteModuleCollection( std::basic_ostream<char>& stream, ModuleCollectionParser& callback ) noexcept(false)
{
	throw std::logic_error( "Not implemented yet" );
}

bool XMLWriteModuleCollection( std::filesystem::path filePath, ModuleCollectionParser& callback ) noexcept(false)
{
	boost::property_tree::ptree ptr;

	read_xml( filePath.string(), ptr );

	return XMLWriteModuleCollection( ptr, callback );
}

}
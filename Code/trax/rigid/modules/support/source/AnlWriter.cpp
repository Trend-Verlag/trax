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

#include "../AnlWriter.h"
#include "../Anl4ModuleWriter.h"

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

	using ptreesupport::operator<<;

void Write( const ModuleCollection& moduleCollection, const std::filesystem::path& path )
{
	boost::property_tree::ptree ptOut;
	ptOut << moduleCollection;
	write_xml( path.string(), ptOut, std::locale(),
				boost::property_tree::xml_parser::xml_writer_settings<std::string>( '\t', 1 ) );
}

void Write( const Module& modul, const std::filesystem::path& path )
{
	boost::property_tree::ptree ptOut;
	boost::property_tree::ptree pttraxML;
	pttraxML.add<std::string>( "<xmlattr>.xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" );
	pttraxML.add<std::string>( "<xmlattr>.xsi:noNamespaceSchemaLocation", "https://www.trendverlag.com/Schema/traxML.xsd" );
	
	pttraxML << modul;

	ptreesupport::move_child( ptOut, "traxML", pttraxML );

	write_xml( path.string(), ptOut, std::locale(),
				boost::property_tree::xml_parser::xml_writer_settings<std::string>( '\t', 1 ) );
}

} // namespace trax
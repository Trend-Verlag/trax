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

#include "RailRunnerParser_Imp.h"

#include "trax/support/TraxSupportXML.h"
#include "trax/rigid/support/RigidSupportXML.h"

#include "trax/rigid/GeomType.h"
#include "trax/rigid/Material.h"
#include "trax/rigid/TrackJointLimits.h"
#include "trax/rigid/Wheelset.h"
#include "trax/rigid/trains/Bogie.h"
#include "trax/rigid/trains/Train.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable: 6313) //  Incorrect operator:  zero-valued flag cannot be tested with bitwise-and.  Use an equality test to check for zero-valued flags.
#endif
#include <boost/property_tree/xml_parser.hpp>
#include "../RailRunnerParser.h"
#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

namespace trax{
namespace ptreesupport{

	using namespace dim;
	using namespace dim::ptreesupport;
	using namespace spat;

///////////////////////////////////////
void ParseBogieFrame( const boost::property_tree::ptree& pt, RollingStockParser& callback ) noexcept
{
	Frame<Length,One> frame;
	spat::ptreesupport::ReadFrame( pt, frame );
	callback.BogieFrame( frame );
}

void ParseWheelFrameLimits( const boost::property_tree::ptree& pt, RollingStockParser& callback ) noexcept
{
	TrackJointLimits trackJointLimits;
	ReadTrackJointLimits( pt, trackJointLimits );
	callback.TrackJointLimits( trackJointLimits );
}

void ParseWheelset( const boost::property_tree::ptree& pt, RollingStockParser& callback ) noexcept
{
	Wheelset wheelset;
	ReadWheelset( pt, wheelset );
	callback.Wheelset( wheelset );
}

void ParseSpeedStep( const boost::property_tree::ptree& pt, RollingStockParser& callback ) noexcept
{
	callback.SpeedStep( 
		get( pt, "<xmlattr>.velocity", 0_mIs, _mIs ), 
		pt.get( "<xmlattr>.value", 1_1 ) );
}

void ParseTractionForceCharacteristic( const boost::property_tree::ptree& pt, RollingStockParser& callback ) noexcept
{
	if( callback.TractionForceCharacteristicStart() )
	{
		for( const auto& pair : pt )
		{
			if( pair.first == "SpeedStep" )
				ParseSpeedStep( pair.second, callback );
		}
	}

	callback.TractionForceCharacteristicEnd();
}

template<class Parser>
void ParseMaterial( const boost::property_tree::ptree& pt, Parser& callback ) noexcept
{
	Material material;
	ReadMaterial( pt, material );
	callback.Material( material );
}
///////////////////////////////////////
void ParseAxisAxisArrangement( const boost::property_tree::ptree& /*pt*/, RollingStockParser& /*callback*/ )
{
	//if( callback.AxisStart(
	//		pt.get<IDType>( "<xmlattr>.id", 0 ),
	//		pt.get( "<xmlattr>.name", "" ),
	//		pt.get( "<xmlattr>.model", "" ), 
	//		pt.get( "<xmlattr>.type", "" ), 
	//		pt.get( "<xmlattr>.param", 0_1 ),
	//		pt.get( "<xmlattr>.min", 0_1 ),
	//		pt.get( "<xmlattr>.null", 0_1 ),
	//		pt.get( "<xmlattr>.max", 0_1 ),
	//		pt.get( "<xmlattr>.vmin", 0_1Is ),
	//		pt.get( "<xmlattr>.vmax", 0_1Is ) ) )
	//{
	//	for( const auto& pair : pt )
	//	{
	//		if( pair.first == "Frame" )
	//			ParseFrame( pair.second, callback );

	//		//else if( pair.first == "AxesCoupling" ){
	//		//	if( !ParseAxesCoupling( pair.second, callback ) )
	//		//		return false;
	//		//}
	//	}
	//}

	//callback.AxisEnd();
	//return true;
}

bool ParseCargoAxisArrangement( const boost::property_tree::ptree& pt, RollingStockParser& callback )
{
	for( const auto& pair : pt )
	{
		if( pair.first == "Frame" )
			ParseFrame( pair.second, callback );

		//else if( pair.first == "GeomCuboid" ){
		//	if( !Parse ( pair.second, callback ) )
		//		return false;
		//}
	}

	return true;
}
//
//bool ParseRollingStockAxisArrangement( const boost::property_tree::ptree& pt, RollingStockParser& callback )
//{
//	if( callback.RollingStockStart() )
//	{
//		for( const auto& pair : pt )
//		{
//			if( pair.first == "Cargo" ){
//				if( !ParseCargoAxisArrangement( pair.second, callback ) )
//					return false;
//			}
//
//			else if( pair.first == "Axis" ){
//				if( !ParseAxisAxisArrangement( pair.second, callback ) )
//					return false;
//			}
//
//			else if( pair.first == "TrackLocation" ){
//				TrackLocationRef trackLocation;
//				ReadTrackLocationRef( pair.second, trackLocation );
//				if( !callback.TrackLocation( trackLocation ) )
//					return false;
//			}
//		}
//	}
//
//	callback.RollingStockEnd();
//	return true;
//}

void ParseCouplingAxisArrangement( const boost::property_tree::ptree& pt, RollingStockParser& callback, Length length )
{
	EndType end = pt.get( "<xmlattr>.end", "" ) == "south" ? EndType::south : EndType::north;
	
	Frame<Length,One> frame;
	frame.Init();
	frame.P.x = (end == EndType::north ? +1_1 : -1_1) * length/2;
	frame.P.z = get( pt, "<xmlattr>.height", 1_m, _m );

	callback.Coupling( 
			end, 
			get( pt, "<xmlattr>.bufferLength", 0.4_m, _m ), 
			get( pt, "<xmlattr>.maxForce", Force{+infinite}, _kN ), 
			IDType{1}, 
			frame );
}

void ParseGeomAxisArrangement( const boost::property_tree::ptree& pt, RollingStockParser& callback )
{
	if( callback.GeomStart(
			pt.get( "<xmlattr>.name", "" ),
			GeomFilter( pt.get( "<xmlattr>.type", "" ) ),
			ToGeomType( pt.get( "<xmlattr>.shape", "box" ) ),
			get( pt, "<xmlattr>.mass", 100_kg, _t ) ) )
	{
		for( const auto& pair : pt )
		{
			if( pair.first == "Area" )
				ParseArea( pair.second, callback );

			else if( pair.first == "Material" )
				ParseMaterial( pair.second, callback );
		}
	}

	callback.GeomEnd(
		pt.get( "<xmlattr>.name", "" ),
		GeomFilter( pt.get( "<xmlattr>.type", "" ) ),
		ToGeomType( pt.get( "<xmlattr>.shape", "box" ) ),
		get( pt, "<xmlattr>.mass", 100_kg, _t ) );
}

void ParseBogieContentAxisArrangement( const boost::property_tree::ptree& pt, RollingStockParser& callback );

void ParseBogieAxisArrangement( const boost::property_tree::ptree& pt, RollingStockParser& callback )
{
	if( callback.BogieStart( 
			pt.get( "<xmlattr>.id", 0 ),
			pt.get( "<xmlattr>.name", "" ),
			pt.get( "<xmlattr>.model", "" ) ) )
	{
		ParseBogieContentAxisArrangement( pt, callback );
	}

	callback.BogieEnd( 
		pt.get( "<xmlattr>.soundGroup", "" ) );
}

void ParseWheelFrameAxisArrangement( const boost::property_tree::ptree& pt, RollingStockParser& callback )
{
	if( callback.WheelFrameStart( 
			pt.get( "<xmlattr>.id", 0 ),
			pt.get( "<xmlattr>.name", "" ),
			pt.get( "<xmlattr>.model", "" ) ) )
	{
		ParseBogieContentAxisArrangement( pt, callback );

		for( const auto& pair : pt )
		{
			if( pair.first == "WheelFrameLimits" )
				ParseWheelFrameLimits( pair.second, callback );

			else if( pair.first == "Wheelset" )
				ParseWheelset( pair.second, callback );

			else if( pair.first == "TractionForceCharacteristic" )
				ParseTractionForceCharacteristic( pair.second, callback );
		}
	}

	callback.WheelFrameEnd( 
		pt.get( "<xmlattr>.soundGroup", "" ) );
}

void ParseBogieContentAxisArrangement( const boost::property_tree::ptree& pt, RollingStockParser& callback )
{
	//Length height = get( pt, "<xmlattr>.height", 1.5_m, _m );
	Length length = get( pt, "<xmlattr>.length", 10_m, _m );

	if( auto centerShift = get_optional( pt, "<xmlattr>.centerShift", _m ); centerShift )
	{
		Frame<Length,One> bodyFrame;
		bodyFrame.Init();
		bodyFrame.P.x = *centerShift;
	//	bodyFrame.P.z = height;

		callback.BogieFrame( bodyFrame );
	}

	for( auto iter = pt.find( "WheelFrame" ); iter != pt.not_found(); ++iter )
	{	
		std::string role = iter->second.get( "<xmlattr>.name", "" );
		EndType parentEnd = EndType::none;
		if( role == "_vRadsatz" )
			parentEnd = EndType::north;
		else if( role == "_hRadsatz" )
			parentEnd = EndType::south;
		else
			continue;

		Frame<Length,One> frame;
		frame.Init();
		if( auto fiter = iter->second.find( "Frame" ); fiter != iter->second.not_found() )
			spat::ptreesupport::ReadFrame( fiter->second, frame );
		callback.Swivel( parentEnd, iter->second.get( "<xmlattr>.id", 0 ), frame );	
	}

	for( const auto& pair : pt )
	{
		if( pair.first == "Frame" )
			ParseBogieFrame( pair.second, callback );

		else if( pair.first == "Geom" ){
			ParseGeomAxisArrangement( pair.second, callback );
		}

		else if( pair.first == "Coupling" )
			ParseCouplingAxisArrangement( pair.second, callback, length );

		else if( pair.first == "Bogie" ){
			ParseBogieAxisArrangement( pair.second, callback );
		}

		else if( pair.first == "WheelFrame" ){
			ParseWheelFrameAxisArrangement( pair.second, callback );
		}

		else if( pair.first == "Axis" ){
			ParseAxisAxisArrangement( pair.second, callback );
		}
	}
}

void ParseAxisArrangement( const boost::property_tree::ptree& root, RollingStockParser& callback )
{
	if( std::filesystem::path reference = root.get<std::filesystem::path>( "<xmlattr>.reference", {} ); !reference.empty() )
	{
		ParseRollingStock( reference, callback );
		return;
	}

	if( callback.RollingStockStart() )
	{
		for( const auto& pair : root )
		{
			if( pair.first == "Bogie" ){
				ParseBogieAxisArrangement( pair.second, callback );
			}

			else if( pair.first == "WheelFrame" ){
				ParseWheelFrameAxisArrangement( pair.second, callback );
			}
		}
	}

	callback.RollingStockEnd();
}
///////////////////////////////////////
void ParseGeom( const boost::property_tree::ptree& pt, RollingStockParser& callback )
{
	if( callback.GeomStart(
		pt.get( "<xmlattr>.name", "" ),
		GeomFilter( pt.get( "<xmlattr>.type", "" ) ),
		ToGeomType( pt.get( "<xmlattr>.shape", "box" ) ),
		get( pt, "<xmlattr>.mass", 100_kg, _t ) ) )
	{
		for( const auto& pair : pt )
		{
			if( pair.first == "Frame" )
				ParseFrame( pair.second, callback );

			else if( pair.first == "Box" )
				ParseArea( pair.second, callback );

			else if( pair.first == "Material" )
				ParseMaterial( pair.second, callback );
		}
	}

	callback.GeomEnd(
		pt.get( "<xmlattr>.name", "" ),
		GeomFilter( pt.get( "<xmlattr>.type", "" ) ),
		ToGeomType( pt.get( "<xmlattr>.shape", "box" ) ),
		get( pt, "<xmlattr>.mass", 100_kg, _t ) );
}

void ParseCoupling( const boost::property_tree::ptree& pt, RollingStockParser& callback )
{
	EndType end = ToEndType( pt.get( "<xmlattr>.end", "none" ) );
	Length bufferLength = get( pt, "<xmlattr>.bufferLength", 0.4_m, _m );
	Force maxForce = get( pt, "<xmlattr>.maxForce", Force{ +infinite }, _kN );
	IDType typeID = pt.get( "<xmlattr>.typeID", 0 );

	for( const auto& pair : pt )
	{
		if( pair.first == "Frame" ){
			Frame<Length, One> frame;
			spat::ptreesupport::ReadFrame( pair.second, frame );
			callback.Coupling( end, bufferLength, maxForce, typeID, frame );
		}
	}
}

void ParseSwivel( const boost::property_tree::ptree& pt, RollingStockParser& callback )
{
	EndType slot = ToEndType( pt.get( "<xmlattr>.slot", "north" ) );
	IDType childID = pt.get( "<xmlattr>.childID", 0 );

	for( const auto& pair : pt )
	{
		if( pair.first == "Frame" ){
			spat::Frame<Length,One> frame;
			spat::ptreesupport::ReadFrame( pair.second, frame );
			callback.Swivel( slot, childID, frame );
		}
	}
}

void ParseAnchor( const boost::property_tree::ptree& pt, RollingStockParser& callback )
{
	for( const auto& pair : pt )
	{
		if( pair.first == "Frame" ){
			spat::Frame<dim::Length, dim::One> anchor;
			spat::ptreesupport::ReadFrame( pair.second, anchor );
			callback.Anchor( anchor );
		}
	}
}

void ParseBogieContent( const boost::property_tree::ptree& pt, RollingStockParser& callback );

void ParseBogie( const boost::property_tree::ptree& pt, RollingStockParser& callback )
{
	if( callback.BogieStart(
			pt.get( "<xmlattr>.id", 0 ),
			pt.get( "<xmlattr>.name", "" ),
			pt.get( "<xmlattr>.model", "" ) ) )
	{
		ParseBogieContent( pt, callback );
	}

	callback.BogieEnd( 
		pt.get( "<xmlattr>.soundGroup", "" ) );
}

void ParseWheelFrame( const boost::property_tree::ptree& pt, RollingStockParser& callback )
{
	if( callback.WheelFrameStart( 
			pt.get( "<xmlattr>.id", 0 ),
			pt.get( "<xmlattr>.name", "" ),
			pt.get( "<xmlattr>.model", "" ) ) )
	{
		ParseBogieContent( pt, callback );

		for( const auto& pair : pt )
		{
			if( pair.first == "WheelFrameLimits" )
				ParseWheelFrameLimits( pair.second, callback );

			else if( pair.first == "Wheelset" )
				ParseWheelset( pair.second, callback );

			else if( pair.first == "TractionForceCharacteristic" )
				ParseTractionForceCharacteristic( pair.second, callback );

			else if( pair.first == "Anchor" )
				ParseAnchor( pair.second, callback );
		}
	}

	callback.WheelFrameEnd( 
		pt.get( "<xmlattr>.soundGroup", "" ) );
}

void ParseBogieContent( const boost::property_tree::ptree& pt, RollingStockParser& callback )
{
	for( const auto& pair : pt )
	{
		if( pair.first == "Frame" )
			ParseBogieFrame( pair.second, callback );

		else if( pair.first == "Geom" ){
			ParseGeom( pair.second, callback );
		}

		else if( pair.first == "Coupling" ){
			ParseCoupling( pair.second, callback );
		}

		else if( pair.first == "Bogie" ){
			ParseBogie( pair.second, callback );
		}

		else if( pair.first == "WheelFrame" ){
			ParseWheelFrame( pair.second, callback );
		}

		else if( pair.first == "Swivel" ){
			ParseSwivel( pair.second, callback );
		}
	}
}

void ParseRollingStock( const boost::property_tree::ptree& root, RollingStockParser& callback )
{
	if( std::filesystem::path reference = root.get<std::filesystem::path>( "<xmlattr>.reference", {} ); !reference.empty() )
	{
		ParseRollingStock( reference, callback );
		return;
	}

	if( callback.RollingStockStart() )
	{
		for( const auto& pair : root )
		{
			if( pair.first == "Bogie" ){
				ParseBogie( pair.second, callback );
			}

			else if( pair.first == "WheelFrame" ){
				ParseWheelFrame( pair.second, callback );
			}
		}
	}

	callback.RollingStockEnd();
}
///////////////////////////////////////
void ParseTrain( const boost::property_tree::ptree& pt, TrainParser& callback )
{
	if( callback.TrainStart( 
			pt.get<IDType>( "<xmlattr>.id", 0 ), 
			pt.get( "<xmlattr>.name", "" ), 
			pt.get( "<xmlattr>.reference", "" ),
			get( pt, "<xmlattr>.travel_velocity", 0_mIs, _mIs ),
			get( pt, "<xmlattr>.velocity", 0_mIs, _mIs ),	
			pt.get( "<xmlattr>.trainThrust", 0_1 ),
			pt.get( "<xmlattr>.trainBrake", 0_1 ),
			pt.get( "<xmlattr>.credit", int32_t{0} ) ) )
	{
		for( const auto& pair : pt )
		{
			if( pair.first == "AxisArrangement" ){
				if( RollingStockParser* pAxisArrangementParser = callback.AxisArrangementStart( 
					pair.second.get( "<xmlattr>.name", "" ),
					pair.second.get( "<xmlattr>.reference", "" )); 
					pAxisArrangementParser )
				{	
					ParseAxisArrangement( pair.second, *pAxisArrangementParser );
				}

				callback.AxisArrangementEnd( ToOrientation( pair.second.get( "<xmlattr>.orientation", "para" ) ) );
			}

			else if( pair.first == "RollingStock" ){
				if( RollingStockParser* pRollingStockParser = callback.RollingStockStart( 
					pair.second.get( "<xmlattr>.name", "" ),
					pair.second.get( "<xmlattr>.reference", "" )); 
					pRollingStockParser )
				{	
					ParseRollingStock( pair.second, *pRollingStockParser );
				}

				callback.RollingStockEnd( ToOrientation( pair.second.get( "<xmlattr>.orientation", "para" ) ) );
			}

			else if( pair.first == "Train" ){
				ParseTrain( pair.second, callback );
			}

			else if( pair.first == "DisplayName" )
				ParseDisplayName( pair.second, callback );

			else if( pair.first == "TrackLocation" ){
				TrackLocationRef trackLocation;
				ReadTrackLocationRef( pair.second, trackLocation );
				callback.TrackLocation( trackLocation );
			}
		}
	}

	callback.TrainEnd(
		ToOrientation( pt.get( "<xmlattr>.orientation", "para" ) ),
		pt.get( "<xmlattr>.automatic", false ),	
		pt.get( "<xmlattr>.couplingNorthActivated", false ),	
		pt.get( "<xmlattr>.couplingSouthActivated", false )
	);
}

bool ParseFleet( const boost::property_tree::ptree& pt, FleetParser& callback )
{
	if( callback.FleetStart(
			pt.get<IDType>( "<xmlattr>.id", 0 ) ) )
	{
		for( const auto& pair : pt )
		{
			if( pair.first == "Train" ){
				ParseTrain( pair.second, callback );
			}
		}
	}
	else
		return false;

	callback.FleetEnd();
	return true;
}

void ParseRollingStockRoot( const boost::property_tree::ptree& root, RollingStockParser& callback )
{
	if( callback.ParsingStart() )
	{
		for( const auto& pair : root )
		{
			if( pair.first == "RollingStock" ){
				ParseRollingStock( pair.second, callback );
			}

			else if( pair.first == "AxisArrangement" ){
				ParseAxisArrangement( pair.second, callback );
			}
		}
	}

	callback.ParsingEnd();
}

void ParseTrainRoot( const boost::property_tree::ptree& root, TrainParser& callback )
{
	if( callback.ParsingStart() )
	{
		for( const auto& pair : root )
		{
			if( pair.first == "Train" ){
				ParseTrain( pair.second, callback );
			}
		}
	}

	callback.ParsingEnd();
}

} // namespace ptreesupport

void ParseRollingStock( std::basic_istream<char>& stream, RollingStockParser& callback ) noexcept(false)
{
	using boost::property_tree::ptree;
	ptree ptr;

	read_xml( stream, ptr );

	ptreesupport::ParseRollingStockRoot( ptr, callback );
}

void ParseRollingStock( const std::filesystem::path& filePath, RollingStockParser& callback ) noexcept(false)
{
	if( !std::filesystem::exists( filePath ) )
	{
		std::cerr << "File not found: " << filePath << std::endl;
		return;
	}

	using boost::property_tree::ptree;
	ptree ptr;

	read_xml( filePath.string(), ptr );

	ptreesupport::ParseRollingStockRoot( ptr, callback );
}

void ParseRollingStock( const Bogie& /*rollingStock*/, RollingStockParser& /*callback*/ ) noexcept(false)
{
	//callback.BogieStart( 
	//	rollingStock.Reference( "name" ), 
	//	rollingStock.Reference( "model" ),
	//	rollingStock.Center(),
	//	rollingStock.Height(),
	//	rollingStock.GetLength() );


	assert( !"Not implemented yet!" );
}

void ParseTrain( const unsigned char* bufferStart, const unsigned char* bufferEnd, TrainParser& callback ) noexcept(false)
{
	std::string string( (const char*)bufferStart, bufferEnd - bufferStart );
	std::istringstream stream( string );

	boost::property_tree::ptree ptr;

	read_xml( stream, ptr );

	ptreesupport::ParseTrainRoot( ptr, callback );
}

void ParseTrain( std::basic_istream<char>& stream, TrainParser& callback ) noexcept(false){
	using boost::property_tree::ptree;
	ptree ptr;

	read_xml( stream, ptr );

	ptreesupport::ParseTrainRoot( ptr, callback );
}

void ParseTrain( const std::filesystem::path& filePath, TrainParser& callback ) noexcept(false)
{
	if( !std::filesystem::exists( filePath ) )
	{
		std::cerr << "File not found: " << filePath << std::endl;
		return;
	}

	using boost::property_tree::ptree;
	ptree ptr;

	read_xml( filePath.string(), ptr );

	ptreesupport::ParseTrainRoot( ptr, callback );
}

void ParseTrain( const Train& /*train*/, TrainParser& /*callback*/ ) noexcept(false)
{
	assert( !"Not implemented yet!" );
}

bool ParseFleet( std::basic_istream<char>& /*stream*/, FleetParser& /*callback*/ ) noexcept(false)
{
	assert( !"Not implemented yet!" );
	return false;
}

bool ParseFleet( const std::filesystem::path& /*filePath*/, FleetParser& /*callback*/ ) noexcept(false)
{
	assert( !"Not implemented yet!" );
	return false;
}

bool ParseFleet( const Fleet& /*fleet*/, FleetParser& /*callback*/ ) noexcept(false)
{
	assert( !"Not implemented yet!" );
	return false;
}

} // namespace trax
//	trax track library
//	AD 2024 
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

#include "TrackSystemParser_Imp.h"
#include "trax/collections/support/CollectionSupportXML.h"

namespace trax{
namespace ptreesupport{

	using namespace dim;
	using namespace dim::ptreesupport;

bool ParseTwist( const boost::property_tree::ptree& pt, TrackSystemParser& callback );

bool ParseCombinedTwist( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	if( !callback.CombinedTwistStart( ) )
		return false;

	for( const auto& pair : pt )
	{
		if( pair.first == "Twist" )
			ParseTwist( pair.second, callback );
	}

	callback.CombinedTwistEnd();
	return true;
}

bool ParseDirectionalTwist( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	for( const auto& pair : pt )
	{
		if( pair.first == "Vector" ){
			spat::Vector<One> attractor;
			spat::ptreesupport::ReadVector( pair.second, attractor );
			return callback.DirectionalTwist( attractor );
		}
	}

	return false;
}

bool ParsePiecewiseTwist( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	PiecewiseTwist::Data data;
	for( const auto& pair : pt )
	{
		if( pair.first == "TwistAngle" )
			data.push_back( std::make_pair( get( pair.second, "<xmlattr>.s", 0_m, _m ), pair.second.get( "<xmlattr>.value", 0_rad ) ) );
	}

	return callback.PiecewiseTwist( data );
}

bool ParsePiecewiseLinearTwist( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	PiecewiseTwist::Data data;
	for( const auto& pair : pt )
	{
		if( pair.first == "TwistAngle" )
			data.push_back( std::make_pair( get( pair.second, "<xmlattr>.s", 0_m, _m ), pair.second.get( "<xmlattr>.value", 0_rad ) ) );
	}

	return callback.PiecewiseLinearTwist( data );
}

bool ParseTwist( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	if( !callback.TwistStart() )
		return false;

	for( const auto& pair : pt )
	{
		if( pair.first == "ZeroTwist" ){
		}

		else if( pair.first == "ConstantTwist" ){
			if( !callback.ConstantTwist( pair.second.get( "<xmlattr>.angle", 0_rad ) ) )
				return false;
		}

		else if( pair.first == "LinearTwist" ){
			if( !callback.LinearTwist( pair.second.get( "<xmlattr>.startangle", 0_rad ), pair.second.get( "<xmlattr>.endangle", 0_rad ) ) )
				return false;
		}

		else if( pair.first == "PiecewiseTwist" ){
			if( !ParsePiecewiseTwist( pair.second, callback ) )
				return false;
		}

		else if( pair.first == "PiecewiseLinearTwist" ){
			if( !ParsePiecewiseLinearTwist( pair.second, callback ) )
				return false;
		}

		else if( pair.first == "DirectionalTwist" ){
			if( !ParseDirectionalTwist( pair.second, callback ) )
				return false;
		}

		else if( pair.first == "CombinedTwist" ){
			if( !ParseCombinedTwist( pair.second, callback ) )
				return false;
		}
	}

	callback.TwistEnd();
	return true;
}

bool ParseEEPCurve( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	EEPCurve::Data data;
	Read( pt, data );
	return callback.EEPCurve( data );
}

bool ParseSampledCurve( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	SampledCurve::Data data;
	for( const auto& pair : pt ){
		if( pair.first == "CurveSample" ){
			data.push_back( {} );
			Read( pair.second, data.back() );
		}
	}

	return callback.SampledCurve( data );
}

bool ParsePolygonalChain( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	PolygonalChain::Data data;
	for( const auto& pair : pt ){
		if( pair.first == "VectorBundle" ){
			data.push_back( {} );
			spat::ptreesupport::ReadVectorBundle( pair.second, data.back() );
		}
	}

	return callback.PolygonalChain( data );
}

bool ParseRotatorChain( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	RotatorChain::Data data;
	for( const auto& pair : pt ){
		if( pair.first == "Link" )
			data.push_back( {	
				pair.second.get( "<xmlattr>.a", 0_rad ),
				pair.second.get( "<xmlattr>.b", 0_rad ),
				pair.second.get( "<xmlattr>.length", 1_m ) } );
	}

	return callback.RotatorChain( data );
}

bool ParseRotator( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	Rotator::Data data;
	data.a = pt.get( "<xmlattr>.a", data.a );
	data.b = pt.get( "<xmlattr>.b", data.b );
	data.a0 = pt.get( "<xmlattr>.a0", data.a0 );
	data.b0 = pt.get( "<xmlattr>.b0", data.b0 );
	return callback.Rotator( data );
}

bool ParseSpline( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	Spline::Data data;
	for( const auto& pair : pt )
	{
		if( pair.first == "Cubic" ){
			data.push_back( {} );
			Read( pair.second, data.back() );
		}
	}

	return callback.Spline( data );
}

bool ParseCubic( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	Cubic::Data data;
	Read( pt, data );
	return callback.Cubic( data );
}

bool ParseClothoid( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	Clothoid::Data data;
	data.a = get( pt, "<xmlattr>.a", data.a, _m );
	return callback.Clothoid( data );
}

bool ParseHelix( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	HelixP::Data data;
	for( const auto& pair : pt )
	{
		if( pair.first == "VectorBundle2" )
			spat::ptreesupport::ReadVectorBundle2( pair.second, data.center );
	}

	return callback.Helix( data );
}

bool ParseArc( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	ArcP::Data data;
	for( const auto& pair : pt )
	{
		if( pair.first == "VectorBundle2" ){
			spat::ptreesupport::ReadVectorBundle2( pair.second, data.vb2 );
		}
	}

	return callback.Arc( data );
}

bool ParseLine( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	LineP::Data data;
	for( const auto& pair : pt )
	{
		if( pair.first == "VectorBundle" ){
			spat::ptreesupport::ReadVectorBundle( pair.second, data.vb );
			data.vb.T.Normalize();
		}

		else if( pair.first == "Vector" )
			spat::ptreesupport::ReadVector( pair.second, data.up );
	}

	return callback.Line( data );
}

bool ParseCurve( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	if( !callback.CurveStart() )
		return false;

	for( const auto& pair : pt )
	{
		if( pair.first == "Line" ){
			if( !ParseLine( pair.second, callback ) )
				return false;
		}

		else if( pair.first == "Arc" ){
			if( !ParseArc( pair.second, callback ) )
				return false;
		}

		else if( pair.first == "Helix" ){
			if( !ParseHelix( pair.second, callback ) )
				return false;
		}

		else if( pair.first == "Clothoid" ){
			if( !ParseClothoid( pair.second, callback ) )
				return false;
		}

		else if( pair.first == "Cubic" ){
			if( !ParseCubic( pair.second, callback ) )
				return false;
		}

		else if( pair.first == "Spline" ){
			if( !ParseSpline( pair.second, callback ) )
				return false;
		}

		else if( pair.first == "Rotator" ){
			if( !ParseRotator( pair.second, callback ) )
				return false;
		}

		else if( pair.first == "RotatorChain" ){
			if( !ParseRotatorChain( pair.second, callback ) )
				return false;
		}

		else if( pair.first == "PolygonalChain" ){
			if( !ParsePolygonalChain( pair.second, callback ) )
				return false;
		}

		else if( pair.first == "SampledCurve" ){
			if( !ParseSampledCurve( pair.second, callback ) )
				return false;
		}

		else if( pair.first == "EEPCurve" ){
			if( !ParseEEPCurve( pair.second, callback ) )
				return false;
		}
	}

	callback.CurveEnd();
	return true;
}

bool ParseTrackEnd( Track::End theOne, const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	for( const auto& pair : pt )
	{
		if( pair.first == "Connection" ){
			Track::End theOther;
			ReadConnection( pair.second, theOther );
			if( !callback.TrackConnection( { theOne, theOther } ) )
				return false;
		}

		else if( pair.first == "BufferStop" ){
			if( !callback.BufferStop( theOne ) )
				return false;
		}

		else if( pair.first == "OpenEnd" ){
			if( !callback.OpenEnd( theOne ) )
				return false;
		}
	}

	return true;
}

bool ParseTrack( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	if( !callback.TrackStart(
			pt.get<IDType>( "<xmlattr>.id", 0 ),
			pt.get( "<xmlattr>.reference", "" ) ) )
		return false;

	if( auto length = pt.get_optional<Length>( "<xmlattr>.length" ); length )
		callback.Interval( { 0_m, *length } );

	for( const auto& pair : pt )
	{
		if( pair.first == "Begin" ){
			if( !ParseTrackEnd( { pt.get<IDType>( "<xmlattr>.id", 0 ), Track::EndType::front }, pair.second, callback ) )
				return false;
		}

		else if( pair.first == "End" ){
			if( !ParseTrackEnd( { pt.get<IDType>( "<xmlattr>.id", 0 ), Track::EndType::end },pair.second, callback ) )
				return false;
		}

		else if( pair.first == "Frame" )
			ParseFrame( pair.second, callback );

		else if( pair.first == "Interval" )
			ParseInterval( pair.second, callback );

		else if( pair.first == "Curve" ){
			if( !ParseCurve( pair.second, callback ) )
				return false;
		}

		else if( pair.first == "Twist" ){
			if( !ParseTwist( pair.second, callback ) )
				return false;
		}

		//else if( pair.first == "Section" ){
		//	if( !ParseSection( pair.second, callback ) )
		//		return false;
		//}
	}

	callback.TrackEnd();
	return true;
}

bool ParseTrackCollection( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	if( !callback.TrackCollectionStart(
			pt.get<IDType>( "<xmlattr>.id", 0 ) ) )
		return false;

	for( const auto& pair : pt )
	{
		if( pair.first == "Track" ){
			if( !ParseTrack( pair.second, callback ) )
				return false;
		}

		else if( pair.first == "Frame" )
			ParseFrame( pair.second, callback );
	}

	callback.TrackCollectionEnd();
	return true;
}

bool ParseTrackSystem( const boost::property_tree::ptree& pt, TrackSystemParser& callback )
{
	if( !callback.TrackSystemStart() )
		return false;

	for( const auto& pair : pt )
	{
		if( pair.first == "TrackCollection" ){
			if( !ParseTrackCollection( pair.second, callback ) )
				return false;
		}
	}

	callback.TrackSystemEnd(
			pt.get<IDType>( "<xmlattr>.activeTrack", 0 ) );
	return true;
}

} // namespace ptreesupport
	
bool ParseTrackSystem( std::basic_istream<char>& /*stream*/, TrackSystemParser& /*callback*/ ) noexcept(false){
	assert( !"Not implemented yet!" );
	return false;
}

bool ParseTrackSystem( std::string /*filePath*/, TrackSystemParser& /*callback*/ ) noexcept(false){
	assert( !"Not implemented yet!" );
	return false;
}
	 
bool ParseTrackSystem( const TrackSystem& /*trackSystem*/, TrackSystemParser& /*callback*/ ) noexcept(false){
	assert( !"Not implemented yet!" );
	return false;
}

} // namespace trax

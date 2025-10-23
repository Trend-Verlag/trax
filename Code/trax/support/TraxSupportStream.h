//	trax track library
//	AD 2020 
//
//  "I only want to be sixteen
//  "and free..."
//
//					Blind Melon
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

#pragma once

#include "trax/Event.h"
#include "trax/Track.h"
#include "trax/TrackLocation.h"
#include "trax/Location.h"
#include "trax/Curve.h"
#include "trax/Plug.h"
#include "trax/Switch.h"

#include "dim/support/DimSupportStream.h"
#include "spat/support/SpatSupportStream.h"

#include <iomanip>

namespace trax{


	/// \name Streaming functions for trax objects
	/// \brief Streams data out and in.
	///@{
	inline std::ostream& operator << ( std::ostream& os, const Track::EndType& endType );

	inline std::istream& operator >> ( std::istream& is, Track::EndType& endType );

	inline std::ostream& operator << ( std::ostream& os, const Track::End& end );

	inline std::istream& operator >> ( std::istream& is, Track::End& end );

	inline std::ostream& operator << ( std::ostream& ost, const Location& location );

	inline std::ostream& operator << ( std::ostream& ost, const TrackLocation& location );

	inline std::ostream& operator << ( std::ostream& ost, const TrackLocationRef& location );

	inline std::ostream& operator << ( std::ostream& ost, const TrackRange& range );

	inline std::ostream& operator<<( std::ostream& ost, const Orientation& orient );

	inline std::ostream& operator<<( std::ostream& ost, const Orientation::Value& orient );


	inline std::ostream& operator << ( std::ostream& ost, NarrowSwitch::Status status );


	inline std::ostream& operator << ( std::ostream& os, const LineP::Data& data );
	inline std::ostream& operator << ( std::ostream& os, const Arc::Data& data );
	inline std::ostream& operator << ( std::ostream& os, const ArcP::Data& data );
	inline std::ostream& operator << ( std::ostream& os, const Helix::Data& data );
	inline std::ostream& operator << ( std::ostream& os, const HelixP::Data& data );
	inline std::ostream& operator << ( std::ostream& os, const Cubic::Data& data );
	inline std::ostream& operator << ( std::ostream& os, const Spline::Data& data );
	inline std::ostream& operator << ( std::ostream& os, const Clothoid::Data& data );
	inline std::ostream& operator << ( std::ostream& os, const Rotator::Data& data );
	inline std::ostream& operator << ( std::ostream& os, const RotatorChain::Data& data );
	inline std::ostream& operator << ( std::ostream& os, const PolygonalChain::Data& data );

	inline std::istream& operator >> ( std::istream& is, LineP::Data& data );
	inline std::istream& operator >> ( std::istream& is, Arc::Data& data );
	inline std::istream& operator >> ( std::istream& is, ArcP::Data& data );
	inline std::istream& operator >> ( std::istream& is, Helix::Data& data );
	inline std::istream& operator >> ( std::istream& is, HelixP::Data& data );
	inline std::istream& operator >> ( std::istream& is, CubicData& data );
	inline std::istream& operator >> ( std::istream& is, Spline::Data& data );
	inline std::istream& operator >> ( std::istream& is, Clothoid::Data& data );
	inline std::istream& operator >> ( std::istream& is, Rotator::Data& data );
	inline std::istream& operator >> ( std::istream& is, RotatorChain::Data& data );
	inline std::istream& operator >> ( std::istream& is, PolygonalChain::Data& data );

	inline std::ostream& operator << ( std::ostream& os, const PiecewiseTwist::Data& data );
	inline std::istream& operator >> ( std::istream& is, PiecewiseTwist::Data& data );

	inline std::ostream& operator << ( std::ostream& os, const EventFilter::Type& type );

	inline std::istream& operator >> ( std::istream& is, EventFilter::Type& type );


	inline std::ostream& operator << ( std::ostream& os, const JackEnumerator& jackenumerator );
	inline std::ostream& operator << ( std::ostream& os, const Jack& jack );
	inline std::ostream& operator << ( std::ostream& os, const Plug& plug );

	///@}



	inline std::ostream& operator << ( std::ostream& os, const Track::EndType& endType ){
		os << "Track::EndType( " << ToString( endType ) << " )";
		return os;
	}

	inline std::istream& operator >> ( std::istream& is, Track::EndType& endType ){
		std::string token;
		is >> token;
		if( token != "Track::EndType(" )
			throw std::runtime_error( "No Track::EndType" );

		char c;
		is >> token;
		is >> c;

		endType = ToEndType( token );
		return is;
	}

	std::ostream& operator<<( std::ostream& os, const Track::End& end )
	{
		os << "Track::End( " << end.id << ", " << ToString(end.type) << " )";
		return os;
	}

	std::istream& operator>>( std::istream& is, Track::End& end )
	{
		std::string token;
		is >> token;
		if( token != "Track::End(" )
			throw std::runtime_error( "No Track::End" );

		char c;
		std::string typeStr;
		is >> end.id;
		is >> c; assert( c == ',' );
		is >> typeStr;
		end.type = ToEndType( typeStr );
		is >> c; assert( c == ')' );
		return is;
	}

	inline std::ostream& operator<<( std::ostream& ost, const Location& location )
	{
		TrackLocationRef tlr;
		location.Get( tlr );
		ost << tlr;
		return ost;
	}

	inline std::ostream& operator << ( std::ostream& ost, const TrackLocation& location ){
		ost << "TrackLocation( " << location.parameter << ", " << location.orientation << " )";
		return ost;
	}

	inline std::ostream& operator << ( std::ostream& ost, const TrackLocationRef& location ){
		ost << "TrackLocationRef( " << location.refid << ", " << location.location << " )";
		return ost;
	}

	inline std::ostream& operator << ( std::ostream& ost, const TrackRange& range ){
		ost << "TrackRange( " << range.refid << ", " << range.range << " )";
		return ost;
	}

	inline std::ostream& operator<<( std::ostream& ost, const Orientation& orient ){
		ost << (orient == Orientation::Value::anti ? "anti" : "para");
		return ost;
	}

	inline std::ostream& operator<<( std::ostream& ost, const Orientation::Value& orient ){
		ost << (orient == Orientation::Value::anti ? "anti" : "para");
		return ost;
	}

	inline std::ostream& operator<<(std::ostream& ost, NarrowSwitch::Status status){
		ost << "NarrowSwitch::Status(" << ToString(status) << " )";
		return ost;
	}

	inline std::ostream& operator << ( std::ostream& os, const LineP::Data& data ){
		os << "LineP::Data( " << data.vb << ", " << data.up << " )";
		return os;
	}

	inline std::ostream& operator << ( std::ostream& os, const Arc::Data& data ){
		os << "Arc::Data( " << data.k << " )";
		return os;
	}

	inline std::ostream& operator << ( std::ostream& os, const ArcP::Data& data ){
		os << "ArcP::Data( " << data.vb2 << " )";
		return os;
	}

	inline std::ostream& operator << ( std::ostream& os, const Helix::Data& data ){
		os << "Helix::Data( " << data.k << ", " << data.t << " )";
		return os;
	}

	inline std::ostream& operator << ( std::ostream& os, const HelixP::Data& data ){
		os << "HelixP::Data( " << data.center << ", " << data.a << ", " << data.b << " )";
		return os;
	}

	inline std::ostream& operator << ( std::ostream& os, const Cubic::Data& data ){
		os << "Cubic::Data( " << data.a << ", " << data.b << ", " << data.c << ", " << data.d << " )";
		return os;
	}

	inline std::ostream& operator << ( std::ostream& os, const Spline::Data& data ){
		if( !data.empty() ){
			os << "Spline::Data( " << data.front();
			std::for_each(data.begin() + 1, data.end(), [&]( const CubicData& cubicData )
			{
				os << ", " << cubicData;
			});
			os << " )";
		}
	
		return os;
	}

	inline std::ostream& operator << ( std::ostream& os, const Clothoid::Data& data ){
		os << "Clothoid::Data( " << data.a << " )";
		return os;
	}

	inline std::ostream& operator << ( std::ostream& os, const Rotator::Data& data ){
		os << "Rotator::Data( " << data.a << ", " << data.b << ", " << data.a0 << ", " << data.b0 << " )";
		return os;
	}

	inline std::ostream& operator << ( std::ostream& os, const RotatorChain::Data& data ){
		if( !data.empty() ){
			os << "RotatorChain::Data( " << data.front();
			std::for_each(data.begin() + 1, data.end(), [&]( const std::tuple<Angle,Angle,Length>& tuple )
			{
				os << ", " << tuple;
			});
			os << " )";
		}
	
		return os;
	}

	inline std::ostream& operator << ( std::ostream& os, const PolygonalChain::Data& data ){
		if( !data.empty() ){
			os << "PolygonalChain::Data( " << data.front();
			std::for_each(data.begin() + 1, data.end(), [&]( const spat::VectorBundle<Length,One>& bundle )
			{
				os << ", " << bundle;
			});
			os << " )";
		}

		return os;
	}

	inline std::istream& operator >> ( std::istream& is, LineP::Data& data )
	{
		common::StreamInHead( is, "LineP::Data" );

		char c;
		is >> data.vb;
		is >> c; assert( c == ',' );
		is >> data.up;
		is >> c; assert( c == ')' );
		return is;
	}

	inline std::istream& operator >> ( std::istream& is, Arc::Data& data )
	{
		common::StreamInHead( is, "Arc::Data" );

		char c;
		is >> data.k;
		is >> c; assert( c == ')' );
		return is;
	}

	inline std::istream& operator >> ( std::istream& is, ArcP::Data& data )
	{
		common::StreamInHead( is, "ArcP::Data" );

		char c;
		is >> data.vb2;
		is >> c; assert( c == ')' );
		return is;
	}
	
	inline std::istream& operator >> ( std::istream& is, Helix::Data& data )
	{
		common::StreamInHead( is, "Helix::Data" );

		char c;
		is >> data.k;
		is >> c; assert( c == ',' );
		is >> data.t;
		is >> c; assert( c == ')' );
		return is;
	}

	inline std::istream& operator >> ( std::istream& is, HelixP::Data& data )
	{
		common::StreamInHead( is, "HelixP::Data" );

		char c;
		is >> data.center;
		is >> c; assert( c == ',' );
		is >> data.a;
		is >> c; assert( c == ',' );
		is >> data.b;
		is >> c; assert( c == ')' );
		return is;
	}

	inline std::istream& operator>>( std::istream& is, CubicData& data )
	{
		common::StreamInHead( is, "Cubic::Data" );

		char c;
		is >> data.a;
		is >> c; assert( c == ',' );
		is >> data.b;
		is >> c; assert( c == ',' );
		is >> data.c;
		is >> c; assert( c == ',' );
		is >> data.d;
		is >> c; assert( c == ')' );
		return is;
	}

	template<typename ValueType>
	inline std::istream& StreamInVector( std::istream& is, std::vector<ValueType>& vector )
	{
		for(;;){
			ValueType value;
			is >> value;
			vector.push_back( value );
			char c;
			is >> c;
			if( c == ')' )
				break;
			assert( c == ',' );
		}

		return is;
	}

	inline std::istream& operator >> ( std::istream& is, Spline::Data& data )
	{
		common::StreamInHead( is, "Spline::Data" );
		return StreamInVector( is, data );
	}

	inline std::istream& operator >> ( std::istream& is, Clothoid::Data& data )
	{
		common::StreamInHead( is, "Clothoid::Data" );

		char c;
		is >> data.a;
		is >> c; assert( c == ')' );
		return is;
	}

	inline std::istream& operator >> ( std::istream& is, Rotator::Data& data )
	{
		common::StreamInHead( is, "Rotator::Data" );

		char c;
		is >> data.a;
		is >> c; assert( c == ',' );
		is >> data.b;
		is >> c; assert( c == ',' );
		is >> data.a0;
		is >> c; assert( c == ',' );
		is >> data.b0;
		is >> c; assert( c == ')' );
		return is;
	}

	inline std::istream& operator >> ( std::istream& is, RotatorChain::Data& data )
	{
		common::StreamInHead( is, "RotatorChain::Data" );
		return StreamInVector( is, data );
	}

	inline std::istream& operator >> ( std::istream& is, PolygonalChain::Data& data )
	{
		common::StreamInHead( is, "PolygonalChain::Data" );
		return StreamInVector( is, data );
	}

	inline std::ostream& operator << ( std::ostream& os, const PiecewiseTwist::Data& data ){
		if( !data.empty() ){
			os << "PiecewiseTwist::Data( " << data.front();
			std::for_each(data.begin() + 1, data.end(), [&]( const PiecewiseTwist::Data::value_type value )
			{
				os << ", " << value;
			});
			os << " )";
		}

		return os;
	}

	inline std::istream& operator >> ( std::istream& is, PiecewiseTwist::Data& data )
	{
		common::StreamInHead( is, "PiecewiseTwist::Data" );
		return StreamInVector( is, data );
	}

	inline std::ostream& operator << ( std::ostream& os, const EventFilter::Type& type ){
		os << "EventFilter::Type( " << EventFilter::From( type ) << " )";
		return os;
	}

	inline std::istream& operator >> ( std::istream& is, EventFilter::Type& type ){
		std::string token;
		is >> token;
		if( token != "EventFilter::Type(" )
			throw std::runtime_error( "No EventFilter::Type" );

		char c;
		is >> token;
		is >> c;

		type = EventFilter::From( token );
		return is;
	}

	inline std::ostream& operator<<( std::ostream& os, const JackEnumerator& jackenumerator )
	{
		os << "JackEnumerator( " << std::endl;
		for( auto& Jack : jackenumerator )
		{
			os << Jack << ", " << std::endl;
		}
		os << " )";
		return os;
	}

	inline std::ostream& operator<<( std::ostream& os, const Jack& jack )
	{
		os << jack.Reference( "name" ) << "[" << jack.Reference( "parent" ) << "]" << "( " << jack.ID() << ", ";
		if( jack.GetPlug() )
			os << *jack.GetPlug();
		else
			os << "0";	
		os << " )";
		return os;
	}

	inline std::ostream& operator<<( std::ostream& os, const Plug& plug )
	{
		os << plug.Reference( "name" ) << "( " << plug.ID() << ", " << plug.JackOnPulse() << " )";
		return os;
	}
}



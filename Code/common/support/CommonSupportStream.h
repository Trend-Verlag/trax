//	trax track library
//	AD 2021 
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

#pragma once

#include "../Interval.h"

#include <iostream>

namespace common{

	/// \defgroup Group_StreamingFunctions Streaming functions
	/// \brief Functions to stream data in and out.
	
	/// \name Streaming functions for Interval
	/// \brief Streams data out and in.
	///@{
	
	/// \brief Streams Interval data out.
	/// \ingroup Group_StreamingFunctions
	template<typename Valtype>
	std::ostream& operator << ( std::ostream& os, const common::Interval<Valtype>& i );

	/// \brief Streams Interval data in.
	/// \ingroup Group_StreamingFunctions
	template<typename Valtype>
	std::istream& operator >> ( std::istream& is, common::Interval<Valtype>& i );
	///@}



template<typename Valtype> inline
std::ostream& operator << ( std::ostream& os, const common::Interval<Valtype>& i ){
	os << "Interval( " << i.Near() << ", " << i.Far() << " )";
	return os;
}

inline void StreamInHead( std::istream& is, const std::string& head ){
	std::string token;
	std::getline( is, token, '(' );
	token.erase( std::remove_if( token.begin(), token.end(), [](unsigned char x) { return std::isspace(x); }), token.end() );
	if( token != head ){
		is.setstate( std::ios_base::failbit );
		throw std::runtime_error( "Stream in head: " + token + " is not " + head );
	}
}

template<typename Valtype> inline
std::istream& operator >> ( std::istream& is, common::Interval<Valtype>& i )
{
	StreamInHead( is, "Interval" );

	char c;
	is >> i.m_Near;
	is >> c; assert( c == ',' );
	is >> i.m_Far;
	is >> c; assert( c == ')' );
	return is;
}

}
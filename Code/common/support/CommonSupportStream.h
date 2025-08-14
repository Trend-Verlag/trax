// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2021 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//

#pragma once

#include "../Interval.h"

#include <iostream>

namespace common{

	/// \name Streaming functions for common objects
	/// \brief Streams data out and in.
	///@{
	template<typename Valtype>
	std::ostream& operator << ( std::ostream& os, const common::Interval<Valtype>& i );

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
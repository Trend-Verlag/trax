//	trax track library
//	AD 2018
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
// For further information, please contact: horstmann.marc@trendverlag.de

#include "trax/Version.h"

#include <sstream>

namespace trax{

bool Version::bNeedsInit = true;

std::string Version::readable;

int Version::Major() noexcept{
	return TRAX_VERSION_MAJOR;
}

int Version::Minor() noexcept{
	return TRAX_VERSION_MINOR;
}

int Version::Patch() noexcept{
	return TRAX_VERSION_PATCH;
}

const char* Version::Readable(){
	if( bNeedsInit ){
		std::ostringstream stream;
		stream << Major() << '.' << Minor() << '.' << Patch() << '.' << std::flush;
		readable = stream.str();
		bNeedsInit = false;
	}
	
	return readable.c_str();
}

bool Version::IsAtLeast( int major, int minor, int patch ) noexcept{
	if( Major() > major )
		return true;
	else if( Major() < major )
		return false;

	if( Minor() > minor )
		return true;
	else if( Minor() < minor )
		return false;

	if( Patch() > patch )
		return true;
	else if( Patch() < patch )
		return false;

	return true;
}

bool Version::HasFeature( const std::string& /*name*/ ) noexcept{
	return false;
}

int Version::SizeOfReal() noexcept{
	return sizeof(Real);
}

const char* Version::LongDescription() noexcept{
	return
		"Calculations in physics not only deal with stark - naked numbers (for tranquility of mind we \
call them 'values'), but they are generally also connected with two other aspects : dimension and unit. \
The dimension is the quality that is dealt with, be it a length in space, a time span or an inertial mass \
or any combination of these.The units on the other hand specify the quantity we are measuring, telling us \
that 1000 are actually 1000_kg, not 1000_t.Basically the trax library automated the lecture of our old \
physics teacher, who said : check the dimensions of your resultand then reason about its magnitude. \
\n\
		One wouldn't believe how much can go wrong on adding up two numbers.[1],[2] There were several \
very costly accidents due to software failure in the past (luckily nobody got hurt). As it turned out, \
calculations are a common source of bugs due to mistaken units or dimensions of values. This brought the \
luminaries of our field to suggest a way for typesafe handling of physical values. We make use of that \
partly for bug reduction, partly because it looks neat, but mostly because it gives us the possibility \
to adjust the range of the actual floating point calculations lateron. E.g. for EEP, the smallest value \
in length that would make a difference to the user, trax::epsilon__length, would be something about 1_cm. \
From this the trax::plausible_maximum_length can be estimated to be about 10_km for 32bit trax::Real values. \
At that distance from the origin, the calculations would start to produce inaccurate results with respect \
to the 1_cm difference. It is not accidential, that the biggest layouts ever build in EEP are about 20_km in diameter. \
\n\
		The units that are actually used by the trax library are specified by e.g.trax::meters_per_unit; \
as it turned out, for optimal results, that value should not be 0.01 nor 1 but rather something like 15. \
This is not implemented yet, for now it is 1, but this will change in the future.The units of trax for \
32bit floating point value then will be 15m, or whatever follows from the epsilon__length. \
\n\
		We tend to call the units that trax actually uses \'units\'.";
}

}
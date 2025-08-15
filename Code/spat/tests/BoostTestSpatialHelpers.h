// Spatial Coordinates And Vectors Library (spat)
// 
// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2018 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "spat/support/SpatSupportStream.h"

#define BOOST_CHECK_CLOSE_SPATIAL( a, b, epsilon ) if( !a.Equals( b, epsilon ) ){ BOOST_CHECK_EQUAL( a, b ); }

#define BOOST_CHECK_CLOSE_SPATIAL2( a, b, epsilon1, epsilon2 ) if( !a.Equals( b, epsilon1, epsilon2 ) ){ BOOST_CHECK_EQUAL( a, b ); }


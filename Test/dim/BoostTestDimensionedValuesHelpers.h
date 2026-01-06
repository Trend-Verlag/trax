// Dimensioned Numbers With Units Are Values!
// ******************************************
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

#define BOOST_CHECK_CLOSE_DIMENSION( a, b, percentage ) BOOST_CHECK_CLOSE( (a).Units(), (b).Units(), percentage );
#define BOOST_CHECK_CLOSE_FRACTION_DIMENSION( a, b, frac ) BOOST_CHECK_CLOSE_FRACTION( (a).Units(), (b).Units(), (frac).Units() );


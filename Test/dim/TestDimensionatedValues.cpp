//	trax track library
//	AD 2020 
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

#if defined( WITH_BOOST_TESTS )

#include <boost/test/unit_test.hpp>

#include "BoostTestDimensionedValuesHelpers.h"
#include "dim/support/DimSupportStream.h"
#include "dim/DimLimits.h"

#include <cmath>
#include <iomanip>

using namespace dim;

BOOST_AUTO_TEST_SUITE(DimensionedNumbersWithUnitsAreValues_tests)

BOOST_AUTO_TEST_CASE( sameUnitsdefinition )
{
	BOOST_CHECK(CheckUnits());
}

BOOST_AUTO_TEST_CASE( test_pod )
{
	BOOST_CHECK( std::is_trivially_copyable<One>::value );
	BOOST_CHECK( std::is_trivially_copyable<Angle>::value );
	BOOST_CHECK( std::is_trivially_copyable<AnglePerLength>::value );
	BOOST_CHECK( std::is_trivially_copyable<PixelPerLength>::value );
	BOOST_CHECK( std::is_trivially_copyable<AngularVelocity>::value );
	BOOST_CHECK( std::is_trivially_copyable<Length>::value );
	BOOST_CHECK( std::is_trivially_copyable<Area>::value );
	BOOST_CHECK( std::is_trivially_copyable<Volume>::value );
	BOOST_CHECK( std::is_trivially_copyable<Mass>::value );
	BOOST_CHECK( std::is_trivially_copyable<Density>::value );
	BOOST_CHECK( std::is_trivially_copyable<Time>::value );
	BOOST_CHECK( std::is_trivially_copyable<Velocity>::value );
	BOOST_CHECK( std::is_trivially_copyable<Acceleration>::value );
	BOOST_CHECK( std::is_trivially_copyable<Force>::value );
	BOOST_CHECK( std::is_trivially_copyable<Momentum>::value );
	BOOST_CHECK( std::is_trivially_copyable<AngularMomentum>::value );
	BOOST_CHECK( std::is_trivially_copyable<Torque>::value );
	BOOST_CHECK( std::is_trivially_copyable<Energy>::value );
	BOOST_CHECK( std::is_trivially_copyable<Power>::value );

	BOOST_CHECK(std::is_standard_layout<One>::value);
	BOOST_CHECK(std::is_standard_layout<Angle>::value);
	BOOST_CHECK(std::is_standard_layout<AnglePerLength>::value);
	BOOST_CHECK(std::is_standard_layout<PixelPerLength>::value);
	BOOST_CHECK(std::is_standard_layout<AngularVelocity>::value);
	BOOST_CHECK(std::is_standard_layout<Length>::value);
	BOOST_CHECK(std::is_standard_layout<Area>::value);
	BOOST_CHECK(std::is_standard_layout<Volume>::value);
	BOOST_CHECK(std::is_standard_layout<Mass>::value);
	BOOST_CHECK(std::is_standard_layout<Density>::value);
	BOOST_CHECK(std::is_standard_layout<Time>::value);
	BOOST_CHECK(std::is_standard_layout<Velocity>::value);
	BOOST_CHECK(std::is_standard_layout<Acceleration>::value);
	BOOST_CHECK(std::is_standard_layout<Force>::value);
	BOOST_CHECK(std::is_standard_layout<Momentum>::value);
	BOOST_CHECK(std::is_standard_layout<AngularMomentum>::value);
	BOOST_CHECK(std::is_standard_layout<Torque>::value);
	BOOST_CHECK(std::is_standard_layout<Energy>::value);
	BOOST_CHECK(std::is_standard_layout<Power>::value);
}

BOOST_AUTO_TEST_CASE( streamValues )
{
	std::cout << 98.2_N << std::endl;
	std::cout << _N << 98.2_N << std::endl;

	std::cout << "epsilon__one: " << epsilon__one << std::endl;
	std::cout << "epsilon__angle: " << epsilon__angle << std::endl;
	std::cout << "epsilon__length: " << epsilon__length << std::endl;
	std::cout << "epsilon__mass: " << epsilon__mass << std::endl;
	std::cout << "epsilon__time: " << epsilon__time << std::endl;
	std::cout << "maximum__angle: " << maximum__angle << std::endl;
	std::cout << "maximum__length: " << maximum__length << std::endl;
	std::cout << "maximum__mass: " << maximum__mass << std::endl;
	std::cout << "maximum__time: " << maximum__time << std::endl;
	std::cout << "plausible_maximum__angle: " << plausible_maximum__angle << std::endl;
	std::cout << "plausible_maximum_length: " << plausible_maximum_length << std::endl;
	std::cout << "plausible_maximum__mass: " << plausible_maximum__mass << std::endl;
	std::cout << "plausible_maximum__time: " << plausible_maximum__time << std::endl;
	std::cout << "epsilon__anglePerLength: " << epsilon__anglePerLength << std::endl;
	std::cout << "epsilon__angularVelocity: " << epsilon__angularVelocity << std::endl;
	std::cout << "epsilon__area: " << epsilon__area << std::endl;
	std::cout << "epsilon__volume: " << epsilon__volume << std::endl;
	std::cout << "epsilon__density: " << epsilon__density << std::endl;
	std::cout << "epsilon__frequency: " << epsilon__frequency << std::endl;
	std::cout << "epsilon__velocity: " << epsilon__velocity << std::endl;
	std::cout << "epsilon__acceleration: " << epsilon__acceleration << std::endl;
	std::cout << "epsilon__force: " << epsilon__force << std::endl;
	std::cout << "epsilon__pressure: " << epsilon__pressure << std::endl;
	std::cout << "epsilon__momentum: " << epsilon__momentum << std::endl;
	std::cout << "epsilon__angularMomentum: " << epsilon__angularMomentum << std::endl;
	std::cout << "epsilon__torque: " << epsilon__torque << std::endl;
	std::cout << "epsilon__energy: " << epsilon__energy << std::endl;
	std::cout << "epsilon__power: " << epsilon__power << std::endl;

	std::cout << "infinite__one: " << infinite__one << std::endl;
	std::cout << "infinite__angle: " << infinite__angle << std::endl;
	std::cout << "infinite__anglePerLength: " << infinite__anglePerLength << std::endl;
	std::cout << "infinite__angularVelocity: " << infinite__angularVelocity << std::endl;
	std::cout << "infinite__length: " << infinite__length << std::endl;
	std::cout << "infinite__area: " << infinite__area << std::endl;
	std::cout << "infinite__volume: " << infinite__volume << std::endl;
	std::cout << "infinite__mass: " << infinite__mass << std::endl;
	std::cout << "infinite__density: " << infinite__density << std::endl;
	std::cout << "infinite__time: " << infinite__time << std::endl;
	std::cout << "infinite__frequency: " << infinite__frequency << std::endl;
	std::cout << "infinite__velocity: " << infinite__velocity << std::endl;
	std::cout << "infinite__accelearation: " << infinite__accelearation << std::endl;
	std::cout << "infinite__force: " << infinite__force << std::endl;
	std::cout << "infinite__pressure: " << infinite__pressure << std::endl;
	std::cout << "infinite__momentum: " << infinite__momentum << std::endl;
	std::cout << "infinite__angularMomentum: " << infinite__angularMomentum << std::endl;
	std::cout << "infinite__torque: " << infinite__torque << std::endl;
	std::cout << "infinite__energy: " << infinite__energy << std::endl;
	std::cout << "infinite__power: " << infinite__power << std::endl;
}

BOOST_AUTO_TEST_CASE( readBackEpsilons )
{
	std::ostringstream ostr;
	ostr << 100*epsilon__one << ' ' << 100*epsilon__angle << ' ' << 100*epsilon__length << ' ' << 100*epsilon__mass << ' ' << 100*epsilon__time;
	ostr << ' ' << 100*epsilon__anglePerLength << ' ' << 100*epsilon__angularVelocity << ' ' << 100*epsilon__area << ' ' << 100*epsilon__volume;
	ostr << ' ' << 100*epsilon__density << ' ' << 100*epsilon__frequency << ' ' << 100*epsilon__velocity << ' ' << 100*epsilon__acceleration;
	ostr << ' ' << 100*epsilon__force << ' ' << 100*epsilon__pressure << ' ' << 100*epsilon__momentum << ' ' << 100*epsilon__angularMomentum;
	ostr << ' ' << 100*epsilon__torque << ' ' << 100*epsilon__energy << ' ' << 100*epsilon__power;

	std::istringstream istr{ ostr.str() };
	One one;
	Angle angle;
	Length length;
	Mass mass;
	Time time;
	AnglePerLength anglePerLength;
	AngularVelocity angularVelocity;
	Area area;
	Volume volume;
	Density density;
	Frequency frequency;
	Velocity velocity;
	Acceleration acceleration;
	Force force;
	Pressure pressure;
	Momentum momentum;
	AngularMomentum angularMomentum;
	Torque torque;
	Energy energy;
	Power power;
	
	istr >> one >> angle >> length >> mass >> time >> anglePerLength >> angularVelocity >> area >> volume >> density >> frequency >> velocity >> acceleration >> force >> pressure >> momentum >> angularMomentum >> torque >> energy >> power;

	BOOST_CHECK_CLOSE_DIMENSION( one, 100*epsilon__one, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( angle, 100*epsilon__angle, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( length, 100*epsilon__length, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( mass, 100*epsilon__mass, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( time, 100*epsilon__time, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( anglePerLength, 100*epsilon__anglePerLength, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( angularVelocity, 100*epsilon__angularVelocity, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( area, 100*epsilon__area, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( volume, 100*epsilon__volume, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( density, 100*epsilon__density, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( frequency, 100*epsilon__frequency, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( velocity, 100*epsilon__velocity, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( acceleration, 100*epsilon__acceleration, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( force, 100*epsilon__force, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( pressure, 100*epsilon__pressure, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( momentum, 100*epsilon__momentum, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( angularMomentum, 100*epsilon__angularMomentum, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( torque, 100*epsilon__torque, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( energy, 100*epsilon__energy, 1 );
	BOOST_CHECK_CLOSE_DIMENSION( power, 100*epsilon__power, 1 );
}

BOOST_AUTO_TEST_CASE( angleAndOne )
{
	Angle a = sto_noexcept<Angle>( "10deg" );
	One o = sto_noexcept<One>( "1.f" );

	BOOST_CHECK_EQUAL( a, 10_deg );
	BOOST_CHECK_EQUAL( o, 1.f );
}

BOOST_AUTO_TEST_CASE( anglePerLengthAndOne )
{
	AnglePerLength a = sto_noexcept<AnglePerLength>( "10deg/m" );
	One o = sto_noexcept<One>( "1.f" );

	BOOST_CHECK_CLOSE_DIMENSION( a, 10_degIm, 0.0001_degIm );
	BOOST_CHECK_EQUAL( o, 1.f );
}

BOOST_AUTO_TEST_CASE( makeComplexValue )
{
	Length l = 1_m;
	Force f = 33_N;
	Torque torque = 1000 * f * l;

	BOOST_CHECK_EQUAL( torque, 33_kNm );
	BOOST_CHECK_EQUAL( torque, 33000_Nm );
	BOOST_CHECK_EQUAL( torque.Units() * kilograms_per_unit * meters_per_unit * meters_per_unit, 33000 );
}

BOOST_AUTO_TEST_CASE( streamInComplexValue )
{
	std::string torqueString{ "33kNm" };
	std::istringstream stream{ torqueString };

	Torque torque;
	stream >> torque;

	BOOST_CHECK_EQUAL( torque, 33_kNm );
}

BOOST_AUTO_TEST_CASE( StreamAngleDegree )
{
	Angle a = 90_deg;

	std::ostringstream ostream;
	ostream << as(_deg,a) << ' ' << as(_deg0,a) << std::flush;
	BOOST_CHECK_EQUAL( ostream.str(), "90deg 90" );

	Angle b = 0_deg;
	std::istringstream istream{ ostream.str() };
	istream >> b;
	BOOST_CHECK_EQUAL( b, a );

	common::Resetter<StreamInAngle> resetter{ DefaultStreamInAngle, _deg };
	istream >> b;
	BOOST_CHECK_EQUAL( b, a );
}

BOOST_AUTO_TEST_CASE( StreamAngleRadiants )
{
	Angle a = pi;

	std::ostringstream ostream;
	ostream << as(_rad,a) << ' ' << a << std::flush;
	BOOST_CHECK_EQUAL( ostream.str(), "pi pi" );

	Angle b = 0_rad;
	std::istringstream istream{ ostream.str() };
	istream >> b;
	BOOST_CHECK_EQUAL( b, a );

	common::Resetter<StreamInAngle> resetter{ DefaultStreamInAngle, _rad };
	istream >> b;
	BOOST_CHECK_EQUAL( b, a );
}

BOOST_AUTO_TEST_CASE( StreamAngleOneCrossInfluence )
{
	{
		common::Resetter<StreamInAngle> resetter{ DefaultStreamInAngle, _deg };
		std::string string{ "90 1" };
		std::istringstream istream{ string };

		Angle a = 0_deg;
		istream >> a;
		BOOST_CHECK_EQUAL( a, 90_deg );

		One one = 0;
		istream >> one;
		BOOST_CHECK_EQUAL( one, pi / 180   ); //unfortunately...
	}

	std::string string{ "90 1" };
	std::istringstream istream{ string };

	Angle a = 0_deg;
	istream >> a;
	BOOST_CHECK_EQUAL( a, 90_rad );

	One one = 0;
	istream >> one;
	BOOST_CHECK_EQUAL( one, 1_1 );
}

BOOST_AUTO_TEST_CASE( StreamAnglePerLength )
{
	AnglePerLength apl = 90_deg / 1_m;

	std::ostringstream ostream;
	ostream << as(_degIm,apl) << std::flush;
	BOOST_CHECK_EQUAL( ostream.str(), "90deg/m" );
	ostream << ' ' << apl << std::flush;

	AnglePerLength bpl = 0_1Im;
	std::istringstream istream{ ostream.str() };
	istream >> bpl;
	BOOST_CHECK_EQUAL( bpl, apl );

	{
		common::Resetter<StreamInAnglePerLength> resetter{ DefaultStreamInAnglePerLength, _degIm };
		istream >> bpl;
		BOOST_CHECK_CLOSE_DIMENSION( bpl, apl, 0.1 );
	}
}

BOOST_AUTO_TEST_CASE( ConversionToReal )
{
	Time time = 2_s;
	//Time time1 = 2.f; 

	Real value = time.Units();
	//Real value2 = time; // implicit consversion
	Real value3{ time }; // explicit conversion

	BOOST_CHECK_EQUAL( value, time.Units() );
//	BOOST_CHECK_EQUAL( value2, time.Units() );
	BOOST_CHECK_EQUAL( value3, time.Units() );
}

BOOST_AUTO_TEST_CASE( Operators )
{
	Length a = 2_m, b= 3_m, c = 4_m;
	c += a - b;

	BOOST_CHECK_EQUAL( c, 3_m );
}

BOOST_AUTO_TEST_CASE ( testDimensionatedValueStreaming )
{
	std::stringstream stream;
	constexpr Length l1 = 120_cm, l2 = 5_m;
	stream << l1 << ' ' << l2 << std::endl;
	Length l1_, l2_;
	stream >> l1_ >> l2_;
	BOOST_CHECK_CLOSE_DIMENSION( l1, l1_, epsilon__length * 100  / l1 );
	BOOST_CHECK_CLOSE_DIMENSION( l2, l2_, epsilon__length * 100  / l2 );

	std::stringstream stream2;
	stream2 << _cm << l1 << ' ' << l2;
	BOOST_CHECK_EQUAL( stream2.str(), "120cm 500cm" );
	stream2 >> l1_ >> l2_;
	BOOST_CHECK_CLOSE_DIMENSION( l1, l1_, epsilon__length * 100  / l1 );
	BOOST_CHECK_CLOSE_DIMENSION( l2, l2_, epsilon__length * 100  / l2 );

	std::stringstream stream3;
	stream3 << _cm << l1 << _m << ' ' << l2;
	BOOST_CHECK_EQUAL( stream3.str(), "120cm 5m" );
	stream3 >> l1_ >> l2_;
	BOOST_CHECK_CLOSE_DIMENSION( l1, l1_, epsilon__length * 100  / l1 );
	BOOST_CHECK_CLOSE_DIMENSION( l2, l2_, epsilon__length * 100  / l2 );

	std::stringstream stream4;
	stream4 << _cm << l1;
	stream4 << ' ' << l2;
	BOOST_CHECK_EQUAL( stream4.str(), "120cm 500cm" );
	stream4 >> l1_ >> l2_;
	BOOST_CHECK_CLOSE_DIMENSION( l1, l1_, epsilon__length * 100  / l1 );
	BOOST_CHECK_CLOSE_DIMENSION( l2, l2_, epsilon__length * 100  / l2 );

	std::stringstream stream5;
	stream5 << l1 << ' ' << l2;
	BOOST_CHECK_EQUAL( stream5.str(), "1.20m 5m" );
	stream5 >> l1_ >> l2_;
	BOOST_CHECK_CLOSE_DIMENSION( l1, l1_, epsilon__length * 100  / l1 );
	BOOST_CHECK_CLOSE_DIMENSION( l2, l2_, epsilon__length * 100  / l2 );

	SetDefaultStreamOfLength( _cm );
	std::stringstream stream6;
	stream6 << l1 << ' ' << l2;
	BOOST_CHECK_EQUAL( stream6.str(), "120cm 500cm" );
	stream6 >> l1_ >> l2_;
	BOOST_CHECK_CLOSE_DIMENSION( l1, l1_, epsilon__length * 100  / l1 );
	BOOST_CHECK_CLOSE_DIMENSION( l2, l2_, epsilon__length * 100  / l2 );

	const Angle a1 = 60_deg, a2 = -90_deg;
	Angle a1_, a2_;
	std::stringstream stream7;
	stream7 << _deg << a1 << ' ' << a2;
	BOOST_CHECK_EQUAL( stream7.str(), "60deg -90deg" );
	stream7 >> a1_ >> a2_;
	BOOST_CHECK_CLOSE_DIMENSION( a1, a1_, epsilon__angle * 100  / a1 );
	BOOST_CHECK_CLOSE_DIMENSION( a2, a2_, epsilon__angle * 100  / abs(a2) );

	SetDefaultStreamOfLength( _m );

	std::stringstream stream8;
	stream8 << _ft << l1;
	BOOST_CHECK_EQUAL( stream8.str(), "3ft 11in" );
	stream8 >> l1_ >> l2_;
	BOOST_CHECK_CLOSE_DIMENSION( l1, l1_ + l2_, epsilon__length * 100  / l1  );

	std::stringstream stream9;
	stream9 << _ft << -10_m;
	BOOST_CHECK_EQUAL( stream9.str(), "-32ft -10in" );
	stream9 >> l1_ >> l2_;
	BOOST_CHECK_CLOSE_DIMENSION( -10_m, l1_ + l2_, epsilon__length * 100  / 10_m  );
}

BOOST_AUTO_TEST_CASE ( testDimensionatedRoundtrip1 )
{
	std::stringstream strgstr;
	Length original{ 95.0075912_km };
	Length result;
	
	strgstr << original;
	strgstr >> result;

	BOOST_CHECK_EQUAL( original, result );
}

BOOST_AUTO_TEST_CASE ( testDimensionatedRoundtrip2 )
{
	const Length original{ 95.0175912_m };
	Length result = original;
	
	for( int i = 0; i < 100; ++i )
	{
		std::stringstream strgstr;
		strgstr << result;
		strgstr >> result;
		BOOST_CHECK_CLOSE_DIMENSION( original, result, epsilon__length );
	}
}

BOOST_AUTO_TEST_CASE ( testDimensionatedRoundtrip3 )
{
	std::stringstream strgstr;
	Angle original{ 90.0075912_deg };
	Angle result;
	
	strgstr << original;
	strgstr >> result; 

	BOOST_CHECK_CLOSE_DIMENSION( original, result, epsilon__length );
}

BOOST_AUTO_TEST_CASE ( testDimensionatedRoundtrip4 )
{
	std::stringstream strgstr;
	One original{ 90.0075912f };
	One result;
	
	strgstr << std::setprecision(std::numeric_limits<One>::max_digits10) << original;
	strgstr >> result;

	BOOST_CHECK_CLOSE_DIMENSION( original, result, epsilon );
}

BOOST_AUTO_TEST_CASE ( testEpsilonAccuracy )
{
	Length e = _m(meters_per_unit * std::nextafter( maximum__length.Units(), infinite__length.Units() )) - maximum__length;
	BOOST_CHECK_LT( e, epsilon__length );
	std::cout	<< "In " << _km << maximum__length << " distance from the origin, the next representable value is in " 
				<< _mm << e << " apart." << std::endl;

	e = _m(meters_per_unit * std::nextafter( 10 * maximum__length.Units(), infinite__length.Units() )) - 10 * maximum__length;
	std::cout	<< "In " << _km << 10 * maximum__length << " distance from the origin, the next representable value is in " 
				<< _mm << e << " apart." << std::endl;
}

BOOST_AUTO_TEST_CASE ( testStreamingAppendixSeparation )
{
	Velocity v = 10_mph;

	std::stringstream strgstr;
	strgstr << v << ' ' << v;
	Velocity result;
	strgstr >> result;
	BOOST_CHECK_CLOSE_DIMENSION( result, v, 0.1 );
	strgstr >> result;
	BOOST_CHECK_CLOSE_DIMENSION( result, v, 0.1 );
}

BOOST_AUTO_TEST_SUITE_END() //DimensionatedValues_tests

#endif //WITH_BOOST_TESTS
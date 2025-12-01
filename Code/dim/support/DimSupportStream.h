//	trax track library
//	AD 2021 
//
//  "My cloth are dirty, but my hands are clean"
//
//							Bob Dylan
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

#pragma once


/// \page docu_dim Dimensionated Values
/// 
/// \section dim_outputstreams Output Streams
/// 
/// This section documents the output stream operators for dimensionated values.
/// 
/// \section dim_inputstreams Input Streams
/// 
/// This section documents the input stream operators for dimensionated values.

#include "dim/DimensionedValues.h"
#include "common/support/CommonSupportStream.h"

#include <cassert>
#include <istream>
#include <ostream>
#include <sstream>

namespace dim
{

#if defined( _WIN32 )
#	if defined( DIM_STATIC_LIBRARY )
#		define dclspc_dim
#	else
#		if defined( DIM_DLL_LIBRARY )
#			define dclspc_dim __declspec( dllexport )
#		else
#			define dclspc_dim __declspec( dllimport )
#		endif
#	endif
#else
#	define dclspc_dim
#endif

	dclspc_dim Real KilogramsPerUnit() noexcept;
	dclspc_dim Real MetersPerUnit() noexcept;
	dclspc_dim Real SecondsPerUnit() noexcept;

	inline bool CheckUnits() noexcept{
		if( kilograms_per_unit != KilogramsPerUnit() )
			return false;
		if( meters_per_unit != MetersPerUnit() )
			return false;
		if( seconds_per_unit != SecondsPerUnit() )
			return false;

		return true;
	}

	/// \name Dimensionated Values Streaming Functions:
	/// \brief These functions are used to stream dimensionated values to 
	/// standard streams.
	/// 
	/// These functions are meant for user interface or debug output/input, 
	/// so they are not strictly roundtrip-safe, as they loose precision of 
	/// floating point numbers according to the epsilon__[DimensionName] 
	/// limits. But it is ok to use them with inert round trip safe 
	/// data if a truncation in these limits is acceptable. Don't use 
	/// them if between load and safe, there is a calculation.
	/// For dimension 'One' the precision can be raised to floating point
	/// precision, streaming std::setprecision(std::numeric_limits<One>::max_digits10).
	///@{
	
	/// \brief Streams Dimensionated value.
	/// \ingroup Group_StreamingFunctions
	dclspc_dim std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<0,0,0>>& one );

	dclspc_dim std::istream& operator >> ( std::istream& istr, Value<Dimension<0,0,0>>& one );

	dclspc_dim std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<-1,0,0>>& anglePerLength );

	dclspc_dim std::istream& operator >> ( std::istream& istr, Value<Dimension<-1,0,0>>& anglePerLength );

	dclspc_dim std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<1,0,0>>& length );

	dclspc_dim std::istream& operator >> ( std::istream& istr, Value<Dimension<1,0,0>>& length );

	dclspc_dim std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<2,0,0>>& area );

	dclspc_dim std::istream& operator >> ( std::istream& istr, Value<Dimension<2,0,0>>& area );

	dclspc_dim std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<3,0,0>>& volume );

	dclspc_dim std::istream& operator >> ( std::istream& istr, Value<Dimension<3,0,0>>& volume );

	dclspc_dim std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<0,0,1>>& time );

	dclspc_dim std::istream& operator >> ( std::istream& istr, Value<Dimension<0,0,1>>& time );

	dclspc_dim std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<0,0,-1>>& angularvelocity );

	dclspc_dim std::istream& operator >> ( std::istream& istr, Value<Dimension<0,0,-1>>& angularvelocity );

	dclspc_dim std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<1,0,-1>>& velocity );

	dclspc_dim std::istream& operator >> ( std::istream& istr, Value<Dimension<1,0,-1>>& velocity );

	dclspc_dim std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<1,0,-2>>& acceleration );

	dclspc_dim std::istream& operator >> ( std::istream& istr, Value<Dimension<1,0,-2>>& acceleration );

	dclspc_dim std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<0,1,0>>& mass );

	dclspc_dim std::istream& operator >> ( std::istream& istr, Value<Dimension<0,1,0>>& mass );

	dclspc_dim std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<-3,1,0>>& density );

	dclspc_dim std::istream& operator >> ( std::istream& istr, Value<Dimension<-3,1,0>>& density );

	dclspc_dim std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<1,1,-2>>& force );

	dclspc_dim std::istream& operator >> ( std::istream& istr, Value<Dimension<1,1,-2>>& force );

	dclspc_dim std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<-1, 1, -2>>& pressure );

	dclspc_dim std::istream& operator >> ( std::istream& istr, Value<Dimension<-1, 1, -2>>& pressure );

	dclspc_dim std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<1, 1, -1>>& momentum );

	dclspc_dim std::istream& operator >> ( std::istream& istr, Value<Dimension<1, 1, -1>>& momentum );

	dclspc_dim std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<2, 1, -1>>& angularmomentum );

	dclspc_dim std::istream& operator >> ( std::istream& istr, Value<Dimension<2, 1, -1>>& angularmomentum );

	dclspc_dim std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<2,1,-2>>& torque );

	dclspc_dim std::istream& operator >> ( std::istream& istr, Value<Dimension<2,1,-2>>& torque );

	dclspc_dim std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<2,1,-3>>& power );

	dclspc_dim std::istream& operator >> ( std::istream& istr, Value<Dimension<2,1,-3>>& power );

	template<typename A, typename B>
	std::ostream& operator << ( std::ostream& ostr, const std::pair<A,B>& pair ){
		ostr << "pair( " << pair.first << ", " << pair.second << " )";
		return ostr;
	}

	template<typename A, typename B>
	std::istream& operator >> ( std::istream& is, std::pair<A,B>& pair )
	{
		common::StreamInHead( is, "pair" );

		char c;
		is >> pair.first;
		is >> c; assert( c == ',' );
		is >> pair.second;
		is >> c; assert( c == ')' );
		return is;
	}

	template<typename A, typename B, typename C>
	std::ostream& operator << ( std::ostream& ostr, const std::tuple<A,B,C>& tuple ){
		ostr << "tuple( " << std::get<0>( tuple ) << ", " << std::get<1>( tuple ) << ", " << std::get<2>( tuple ) << " )";
		return ostr;
	}

	template<typename A, typename B, typename C>
	std::istream& operator >> ( std::istream& is, std::tuple<A,B,C>& tuple )
	{
		common::StreamInHead( is, "tuple" );

		char c;
		is >> std::get<0>( tuple );
		is >> c; assert( c == ',' );
		is >> std::get<1>( tuple );
		is >> c; assert( c == ',' );
		is >> std::get<2>( tuple );
		is >> c; assert( c == ')' );
		return is;
	}
	///@}


	/// \name Setters for Output Units
	/// \brief These functions set the output units for dimensionated values
	///@{
	dclspc_dim std::ostream& _deg( std::ostream& ostr, Angle angle );

	dclspc_dim std::ostream& _deg( std::ostream& ostr );

	dclspc_dim std::ostream& _deg0( std::ostream& ostr, Angle angle );

	dclspc_dim std::ostream& _deg0( std::ostream& ostr );

	dclspc_dim std::ostream& _rad( std::ostream& ostr, Angle angle );

	dclspc_dim std::ostream& _rad( std::ostream& ostr );

	dclspc_dim std::ostream& _Angle( std::ostream& ostr );


	dclspc_dim std::ostream& _degIm( std::ostream& ostr, AnglePerLength angleperlength );

	dclspc_dim std::ostream& _degIm( std::ostream& ostr );

	dclspc_dim std::ostream& _degIcm( std::ostream& ostr, AnglePerLength angleperlength );

	dclspc_dim std::ostream& _degIcm( std::ostream& ostr );

	dclspc_dim std::ostream& _1Im( std::ostream& ostr, AnglePerLength angleperlength );

	dclspc_dim std::ostream& _1Im( std::ostream& ostr );

	dclspc_dim std::ostream& _1ILength( std::ostream& ostr );


	dclspc_dim std::ostream& _m( std::ostream& ostr, Length length );

	dclspc_dim std::ostream& _m( std::ostream& ostr );

	dclspc_dim std::ostream& _mm( std::ostream& ostr, Length length );

	dclspc_dim std::ostream& _mm( std::ostream& ostr );

	dclspc_dim std::ostream& _cm( std::ostream& ostr, Length length );

	dclspc_dim std::ostream& _cm( std::ostream& ostr );

	dclspc_dim std::ostream& _dm( std::ostream& ostr, Length length );

	dclspc_dim std::ostream& _dm( std::ostream& ostr );

	dclspc_dim std::ostream& _km( std::ostream& ostr, Length length );

	dclspc_dim std::ostream& _km( std::ostream& ostr );

	dclspc_dim std::ostream& _mi( std::ostream& ostr, Length length );

	dclspc_dim std::ostream& _mi( std::ostream& ostr );

	dclspc_dim std::ostream& _nmi( std::ostream& ostr, Length length );

	dclspc_dim std::ostream& _nmi( std::ostream& ostr );

	dclspc_dim std::ostream& _yd( std::ostream& ostr, Length length );

	dclspc_dim std::ostream& _yd( std::ostream& ostr );

	dclspc_dim std::ostream& _ft( std::ostream& ostr, Length length );

	dclspc_dim std::ostream& _ft( std::ostream& ostr );

	dclspc_dim std::ostream& _in( std::ostream& ostr, Length length );

	dclspc_dim std::ostream& _in( std::ostream& ostr );

	dclspc_dim std::ostream& _Length( std::ostream& ostr );


	dclspc_dim std::ostream& _cmIs( std::ostream& ostr, Velocity velocity );

	dclspc_dim std::ostream& _cmIs( std::ostream& ostr );

	dclspc_dim std::ostream& _kmIh( std::ostream& ostr, Velocity velocity );

	dclspc_dim std::ostream& _kmIh( std::ostream& ostr );

	dclspc_dim std::ostream& _mIs( std::ostream& ostr, Velocity velocity );

	dclspc_dim std::ostream& _mIs( std::ostream& ostr );

	dclspc_dim std::ostream& _mph( std::ostream& ostr, Velocity velocity );

	dclspc_dim std::ostream& _mph( std::ostream& ostr );

	dclspc_dim std::ostream& _kn( std::ostream& ostr, Velocity velocity );

	dclspc_dim std::ostream& _kn( std::ostream& ostr );

	dclspc_dim std::ostream& _Velocity( std::ostream& ostr );


	dclspc_dim std::ostream& _mIs2( std::ostream& ostr, Acceleration acceleration );

	dclspc_dim std::ostream& _mIs2( std::ostream& ostr );


	dclspc_dim std::ostream& _g( std::ostream& ostr, Mass mass );

	dclspc_dim std::ostream& _g( std::ostream& ostr );

	dclspc_dim std::ostream& _kg( std::ostream& ostr, Mass mass );

	dclspc_dim std::ostream& _kg( std::ostream& ostr );

	dclspc_dim std::ostream& _t( std::ostream& ostr, Mass mass );

	dclspc_dim std::ostream& _t( std::ostream& ostr );

	dclspc_dim std::ostream& _lb( std::ostream& ostr, Mass mass );

	dclspc_dim std::ostream& _lb( std::ostream& ostr );

	dclspc_dim std::ostream& _oz( std::ostream& ostr, Mass mass );

	dclspc_dim std::ostream& _oz( std::ostream& ostr );

	dclspc_dim std::ostream& _long_tons( std::ostream& ostr, Mass mass );

	dclspc_dim std::ostream& _long_tons( std::ostream& ostr );

	dclspc_dim std::ostream& _short_tons( std::ostream& ostr, Mass mass );

	dclspc_dim std::ostream& _short_tons( std::ostream& ostr );

	dclspc_dim std::ostream& _Mass( std::ostream& ostr );


	dclspc_dim std::ostream& _gIcm3( std::ostream& ostr, Density density );

	dclspc_dim std::ostream& _gIcm3( std::ostream& ostr );

	dclspc_dim std::ostream& _kgIm3( std::ostream& ostr, Density density );

	dclspc_dim std::ostream& _kgIm3( std::ostream& ostr );

	dclspc_dim std::ostream& _tIm3( std::ostream& ostr, Density density );

	dclspc_dim std::ostream& _tIm3( std::ostream& ostr );


	dclspc_dim std::ostream& _ms( std::ostream& ostr, Time time );

	dclspc_dim std::ostream& _ms( std::ostream& ostr );

	dclspc_dim std::ostream& _s( std::ostream& ostr, Time time );

	dclspc_dim std::ostream& _s( std::ostream& ostr );


	dclspc_dim std::ostream& _N( std::ostream& ostr, Force force );

	dclspc_dim std::ostream& _N( std::ostream& ostr );

	dclspc_dim std::ostream& _kN( std::ostream& ostr, Force force );

	dclspc_dim std::ostream& _kN( std::ostream& ostr );

	dclspc_dim std::ostream& _kp( std::ostream& ostr, Force force );

	dclspc_dim std::ostream& _kp( std::ostream& ostr );

	dclspc_dim std::ostream& _lbf( std::ostream& ostr, Force force );

	dclspc_dim std::ostream& _lbf( std::ostream& ostr );

	dclspc_dim std::ostream& _long_tons_force( std::ostream& ostr, Force force );

	dclspc_dim std::ostream& _long_tons_force( std::ostream& ostr );

	dclspc_dim std::ostream& _short_tons_force( std::ostream& ostr, Force force );

	dclspc_dim std::ostream& _short_tons_force( std::ostream& ostr );

	dclspc_dim std::ostream& _Force( std::ostream& ostr );


	dclspc_dim std::ostream& _Pa( std::ostream& ostr, Pressure pressure );

	dclspc_dim std::ostream& _Pa( std::ostream& ostr );

	dclspc_dim std::ostream& _kPa( std::ostream& ostr, Pressure pressure );

	dclspc_dim std::ostream& _kPa( std::ostream& ostr );

	dclspc_dim std::ostream& _bar( std::ostream& ostr, Pressure pressure );

	dclspc_dim std::ostream& _bar( std::ostream& ostr );

	dclspc_dim std::ostream& _Pressure( std::ostream& ostr );


	dclspc_dim std::ostream& _kgmIs( std::ostream& ostr, Momentum momentum );

	dclspc_dim std::ostream& _kgmIs( std::ostream& ostr );

	dclspc_dim std::ostream& _tmIs( std::ostream& ostr, Momentum momentum );

	dclspc_dim std::ostream& _tmIs( std::ostream& ostr );


	dclspc_dim std::ostream& _kgm2Is( std::ostream& ostr, AngularMomentum angularmomentum );

	dclspc_dim std::ostream& _kgm2Is( std::ostream& ostr );

	dclspc_dim std::ostream& _tm2Is( std::ostream& ostr, AngularMomentum angularmomentum );

	dclspc_dim std::ostream& _tm2Is( std::ostream& ostr );


	dclspc_dim std::ostream& _Nm( std::ostream& ostr, Torque torque );

	dclspc_dim std::ostream& _Nm( std::ostream& ostr );

	dclspc_dim std::ostream& _kNm( std::ostream& ostr, Torque torque );

	dclspc_dim std::ostream& _kNm( std::ostream& ostr );

	dclspc_dim std::ostream& _Torque( std::ostream& ostr );

	dclspc_dim std::ostream& _W( std::ostream& ostr, Power power );

	dclspc_dim std::ostream& _W( std::ostream& ostr );

	dclspc_dim std::ostream& _kW( std::ostream& ostr, Power power );

	dclspc_dim std::ostream& _kW( std::ostream& ostr );

	dclspc_dim std::ostream& _PS( std::ostream& ostr, Power power );

	dclspc_dim std::ostream& _PS( std::ostream& ostr );

	dclspc_dim std::ostream& _hp( std::ostream& ostr, Power power );

	dclspc_dim std::ostream& _hp( std::ostream& ostr );

	dclspc_dim std::ostream& _Power( std::ostream& ostr );
	///@}


	/// \name 
	///@{
	typedef std::ostream& (*StreamOutAnglePerLength)( std::ostream&, AnglePerLength );
	typedef std::ostream& (*StreamOutAngle)( std::ostream&, Angle );
	typedef std::ostream& (*StreamOutLength)( std::ostream&, Length );
	typedef std::ostream& (*StreamOutVelocity)( std::ostream&, Velocity );
	typedef std::ostream& (*StreamOutAcceleration)( std::ostream&, Acceleration );
	typedef std::ostream& (*StreamOutMass)( std::ostream&, Mass );
	typedef std::ostream& (*StreamOutDensity)( std::ostream&, Density );
	typedef std::ostream& (*StreamOutTime)( std::ostream&, Time );
	typedef std::ostream& (*StreamOutForce)( std::ostream&, Force );
	typedef std::ostream& (*StreamOutPressure)( std::ostream&, Pressure );
	typedef std::ostream& (*StreamOutMomentum)( std::ostream&, Momentum );
	typedef std::ostream& (*StreamOutAngularMomentum)( std::ostream&, AngularMomentum );
	typedef std::ostream& (*StreamOutTorque)( std::ostream&, Torque );
	typedef std::ostream& (*StreamOutPower)( std::ostream&, Power );
	///@}

	/// \name 
	///@{
	typedef AnglePerLength (*StreamInAnglePerLength)( Real );
	typedef Angle (*StreamInAngle)( Real );
	typedef Length (*StreamInLength)( Real );
	typedef Velocity (*StreamInVelocity)( Real );
	typedef Acceleration (*StreamInAcceleration)( Real );
	typedef Mass (*StreamInMass)( Real );
	typedef Density (*StreamInDensity)( Real );
	typedef Time (*StreamInTime)( Real );
	typedef Force (*StreamInForce)( Real );
	typedef Pressure (*StreamInPressure)( Real );
	typedef Momentum (*StreamInMomentum)( Real );
	typedef AngularMomentum (*StreamInAngularMomentum)( Real );
	typedef Torque (*StreamInTorque)( Real );
	typedef Power (*StreamInPower)( Real );
	///@}

	/// \name Default Streaming Unit Converters
	/// \brief If no unit is given for a number, these default
	/// values will be used. 
	/// 
	/// If changing the default temporarily, use of common::Resetter
	/// is recommended.
	///@{
	extern dclspc_dim StreamOutAnglePerLength DefaultStreamOutAnglePerLength;
	extern dclspc_dim StreamInAnglePerLength DefaultStreamInAnglePerLength;
	extern dclspc_dim StreamOutAngle DefaultStreamOutAngle;
	extern dclspc_dim StreamInAngle DefaultStreamInAngle;
	extern dclspc_dim StreamOutLength DefaultStreamOutLength;
	extern dclspc_dim StreamInLength DefaultStreamInLength;
	extern dclspc_dim StreamOutVelocity DefaultStreamOutVelocity;
	extern dclspc_dim StreamInVelocity DefaultStreamInVelocity;
	extern dclspc_dim StreamOutAcceleration DefaultStreamOutAcceleration;
	extern dclspc_dim StreamInAcceleration DefaultStreamInAcceleration;
	extern dclspc_dim StreamOutMass DefaultStreamOutMass;
	extern dclspc_dim StreamInMass DefaultStreamInMass;
	extern dclspc_dim StreamOutDensity DefaultStreamOutDensity;
	extern dclspc_dim StreamInDensity DefaultStreamInDensity;
	extern dclspc_dim StreamOutTime DefaultStreamOutTime;
	extern dclspc_dim StreamInTime DefaultStreamInTime;
	extern dclspc_dim StreamOutForce DefaultStreamOutForce;
	extern dclspc_dim StreamInForce DefaultStreamInForce;
	extern dclspc_dim StreamOutPressure DefaultStreamOutPressure;
	extern dclspc_dim StreamInPressure DefaultStreamInPressure;
	extern dclspc_dim StreamOutMomentum DefaultStreamOutMomentum;
	extern dclspc_dim StreamInMomentum DefaultStreamInMomentum;
	extern dclspc_dim StreamOutAngularMomentum DefaultStreamOutAngularMomentum;
	extern dclspc_dim StreamInAngularMomentum DefaultStreamInAngularMomentum;
	extern dclspc_dim StreamOutTorque DefaultStreamOutTorque;
	extern dclspc_dim StreamInTorque DefaultStreamInTorque;
	extern dclspc_dim StreamOutPower DefaultStreamOutPower;
	extern dclspc_dim StreamInPower DefaultStreamInPower;
	///@}

	///@{
	void dclspc_dim SetDefaultStreamOfAnglePerLength( StreamOutAnglePerLength _x = _1Im, StreamInAnglePerLength _y = _1Im ) noexcept;
	void dclspc_dim SetDefaultStreamOfAngle( StreamOutAngle _x = nullptr, StreamInAngle _y = nullptr ) noexcept;
	void dclspc_dim SetDefaultStreamOfLength( StreamOutLength _x = _m, StreamInLength _y = _m ) noexcept;
	void dclspc_dim SetDefaultStreamOfVelocity( StreamOutVelocity _x = _kmIh, StreamInVelocity _y = _kmIh ) noexcept;
	void dclspc_dim SetDefaultStreamOfMass( StreamOutMass _x = _kg, StreamInMass _y = _kg ) noexcept;
//	void dclspc_dim SetDefaultStreamOfDensity( StreamOutDensity _x = _kgIm3, StreamInDensity _y = _kgIm3 ) noexcept;
	void dclspc_dim SetDefaultStreamOfTime( StreamOutTime _x = _s, StreamInTime _y = _s ) noexcept;
	void dclspc_dim SetDefaultStreamOfForce( StreamOutForce _x = _kN, StreamInForce _y = _kN ) noexcept;
	void dclspc_dim SetDefaultStreamOfPressure( StreamOutPressure _x = _Pa, StreamInPressure _y = _Pa ) noexcept;
	void dclspc_dim SetDefaultStreamOfTorque( StreamOutTorque _x = _kNm, StreamInTorque _y = _kNm ) noexcept;
	void dclspc_dim SetDefaultStreamOfPower( StreamOutPower _x = _kW, StreamInPower _y = _kW ) noexcept;
	///@}

	///@{
	StreamOutLength dclspc_dim StreamOutLengthFrom( std::string unit );
	StreamOutAnglePerLength dclspc_dim StreamOutAnglePerLengthFrom( std::string unit );
	StreamOutVelocity dclspc_dim StreamOutVelocityFrom( std::string unit );
	StreamOutAcceleration dclspc_dim StreamOutAccelerationFrom( std::string unit );
	StreamOutMass dclspc_dim StreamOutMassFrom( std::string unit );
	StreamOutDensity dclspc_dim StreamOutDensityFrom( std::string unit );
	StreamOutForce dclspc_dim StreamOutForceFrom( std::string unit );
	StreamOutPressure dclspc_dim StreamOutPressureFrom( std::string unit );
	StreamOutMomentum dclspc_dim StreamOutMomentumFrom( std::string unit );
	StreamOutAngularMomentum dclspc_dim StreamOutAngularMomentumFrom( std::string unit );
	StreamOutTorque dclspc_dim StreamOutTorqueFrom( std::string unit );
	StreamOutPower dclspc_dim StreamOutPowerFrom( std::string unit );
	///@}

	///@{
	StreamInAnglePerLength dclspc_dim StreamInAnglePerLengthFrom( std::string unit );
	StreamInAngle dclspc_dim StreamInAngleFrom( std::string unit );
	StreamInLength dclspc_dim StreamInLengthFrom( std::string unit );
	StreamInVelocity dclspc_dim StreamInVelocityFrom( std::string unit );
	StreamInAcceleration dclspc_dim StreamInAccelerationFrom( std::string unit );
	StreamInMass dclspc_dim StreamInMassFrom( std::string unit );
	StreamInDensity dclspc_dim StreamInDensityFrom( std::string unit );
	StreamInTime dclspc_dim StreamInTimeFrom( std::string unit );
	StreamInForce dclspc_dim StreamInForceFrom( std::string unit );
	StreamInPressure dclspc_dim StreamInPressureFrom( std::string unit );
	StreamInMomentum dclspc_dim StreamInMomentumFrom( std::string unit );
	StreamInAngularMomentum dclspc_dim StreamInAngularMomentumFrom( std::string unit );
	StreamInTorque dclspc_dim StreamInTorqueFrom( std::string unit );
	StreamInPower dclspc_dim StreamInPowerFrom( std::string unit );
	///@}

	///@{
	template<int L,int M,int T>
	struct smanip{
		std::ostream& (*f)( std::ostream&, Value<Dimension<L,M,T>> );
		Value<Dimension<L,M,T>> dvalue;
		smanip( std::ostream& (*_f)( std::ostream&, Value<Dimension<L,M,T>> ), Value<Dimension<L,M,T>> _dvalue ) 
			: f{_f}, dvalue{_dvalue}
		{}
	};

	template<int L,int M,int T> inline 
	std::ostream& operator << ( std::ostream& ostr, const smanip<L,M,T>& m ){
		m.f(ostr,m.dvalue);
		return ostr;
	}

	template<int L,int M,int T> inline 
	smanip<L,M,T> as( std::ostream& (*f)( std::ostream&, Value<Dimension<L,M,T>> ), Value<Dimension<L,M,T>> dvalue ){
		return smanip<L,M,T>{f,dvalue};
	}
	///@}


	///@{
	template<int L,int M,int T>
	class DefaultUnit{
	public:
		typedef Value<Dimension<L,M,T>> (*StreamInDimension)( Real );
		typedef StreamInDimension (*StreamInDimensionFrom)( std::string );

		static StreamInDimension& GetDefaultStreamInDimension();

		static StreamInDimensionFrom GetStreamInDimensionFrom();
	};

	template<>
	class DefaultUnit<-1,0,0>{
	public:
		typedef Value<Dimension<-1,0,0>> (*StreamInDimension)( Real );
		typedef StreamInDimension (*StreamInDimensionFrom)( std::string );

		static StreamInDimension& GetDefaultStreamInDimension() noexcept{
			return DefaultStreamInAnglePerLength;
		}

		static StreamInDimensionFrom GetStreamInDimensionFrom() noexcept{
			return StreamInAnglePerLengthFrom;
		}

	};

	template<>
	class DefaultUnit<1,0,0>{
	public:
		typedef Value<Dimension<1,0,0>> (*StreamInDimension)( Real );
		typedef StreamInDimension (*StreamInDimensionFrom)( std::string );

		static StreamInDimension& GetDefaultStreamInDimension() noexcept{
			return DefaultStreamInLength;
		}

		static StreamInDimensionFrom GetStreamInDimensionFrom() noexcept{
			return StreamInLengthFrom;
		}

	};

	template<>
	class DefaultUnit<1,0,-1>{
	public:
		typedef Value<Dimension<1,0,-1>> (*StreamInDimension)( Real );
		typedef StreamInDimension (*StreamInDimensionFrom)( std::string );

		static StreamInDimension& GetDefaultStreamInDimension() noexcept{
			return DefaultStreamInVelocity;
		}

		static StreamInDimensionFrom GetStreamInDimensionFrom() noexcept{
			return StreamInVelocityFrom;
		}

	};

	template<>
	class DefaultUnit<0,1,0>{
	public:
		typedef Value<Dimension<0,1,0>> (*StreamInDimension)( Real );
		typedef StreamInDimension (*StreamInDimensionFrom)( std::string );

		static StreamInDimension& GetDefaultStreamInDimension() noexcept{
			return DefaultStreamInMass;
		}

		static StreamInDimensionFrom GetStreamInDimensionFrom() noexcept{
			return StreamInMassFrom;
		}
	};

	template<>
	class DefaultUnit<0,0,1>{
	public:
		typedef Value<Dimension<0,0,1>> (*StreamInDimension)( Real );
		typedef StreamInDimension (*StreamInDimensionFrom)( std::string );

		static StreamInDimension& GetDefaultStreamInDimension() noexcept{
			return DefaultStreamInTime;
		}

		static StreamInDimensionFrom GetStreamInDimensionFrom() noexcept{
			return StreamInTimeFrom;
		}
	};

	template<>
	class DefaultUnit<1,1,-2>{
	public:
		typedef Value<Dimension<1,1,-2>> (*StreamInDimension)( Real );
		typedef StreamInDimension (*StreamInDimensionFrom)( std::string );

		static StreamInDimension& GetDefaultStreamInDimension() noexcept{
			return DefaultStreamInForce;
		}

		static StreamInDimensionFrom GetStreamInDimensionFrom() noexcept{
			return StreamInForceFrom;
		}
	};

	template<>
	class DefaultUnit<-1,1,-2>{
	public:
		typedef Value<Dimension<-1,1,-2>> (*StreamInDimension)( Real );
		typedef StreamInDimension (*StreamInDimensionFrom)( std::string );

		static StreamInDimension& GetDefaultStreamInDimension() noexcept{
			return DefaultStreamInPressure;
		}

		static StreamInDimensionFrom GetStreamInDimensionFrom() noexcept{
			return StreamInPressureFrom;
		}
	};

	template<>
	class DefaultUnit<2,1,-2>{
	public:
		typedef Value<Dimension<2,1,-2>> (*StreamInDimension)( Real );
		typedef StreamInDimension (*StreamInDimensionFrom)( std::string );

		static StreamInDimension& GetDefaultStreamInDimension() noexcept{
			return DefaultStreamInTorque;
		}

		static StreamInDimensionFrom GetStreamInDimensionFrom() noexcept{
			return StreamInTorqueFrom;
		}
	};

	template<>
	class DefaultUnit<2,1,-3>{
	public:
		typedef Value<Dimension<2,1,-3>> (*StreamInDimension)( Real );
		typedef StreamInDimension (*StreamInDimensionFrom)( std::string );

		static StreamInDimension& GetDefaultStreamInDimension() noexcept{
			return DefaultStreamInPower;
		}

		static StreamInDimensionFrom GetStreamInDimensionFrom() noexcept{
			return StreamInPowerFrom;
		}
	};
	///@}


	template<int L,int M,int T> 
	std::string to_string( Value<Dimension<L,M,T>> val ){
		std::ostringstream stream;
		stream << val;
		return stream.str();
	}

	dclspc_dim std::string to_string_deg( Angle val );				/// < \brief Writes the suffix 'deg' after the value.
	dclspc_dim std::string to_string_deg0( Angle val );				/// < \brief Does'nt write a suffix after the value.
	dclspc_dim std::string to_string_degIm( AnglePerLength val );
	dclspc_dim std::string to_string_degIcm( AnglePerLength val );

	template<class DimValtype>
	DimValtype sto( std::string str ){
		std::istringstream stream{ str };
		DimValtype retval{0};
		for( DimValtype value; stream >> value; retval += value )
			;
		return retval;
	}

	template<class DimValtype>
	DimValtype sto_noexcept( std::string str ) noexcept{
		try{
			return sto<DimValtype>( str );
		}
		catch( ... ){
			return DimValtype{0};
		}
	}

	template<class DimValtypeFirst,class DimValtypeSecond>
	std::pair<DimValtypeFirst,DimValtypeSecond> sto( std::string str, char delimiter = ',' ){
		std::istringstream stream{ str };
		std::string token;
		std::getline( stream, token, delimiter );
		DimValtypeFirst first = sto<DimValtypeFirst>(token);
		std::getline( stream, token, delimiter );
		DimValtypeSecond second = sto<DimValtypeSecond>( token );
		return std::make_pair( first, second );
	}

	//extern template dclspc_dim One sto_noexcept( std::string str ) noexcept;
	////extern template dclspc_dim Angle sto_noexcept( std::string str ) noexcept;
	//extern template dclspc_dim AnglePerLength sto_noexcept( std::string str ) noexcept;
	//extern template dclspc_dim AngularVelocity sto_noexcept( std::string str ) noexcept;
	//extern template dclspc_dim Length sto_noexcept( std::string str ) noexcept;
	//extern template dclspc_dim Area sto_noexcept( std::string str ) noexcept;
	//extern template dclspc_dim Volume sto_noexcept( std::string str ) noexcept;
	//extern template dclspc_dim Mass sto_noexcept( std::string str ) noexcept;
	//extern template dclspc_dim Density sto_noexcept( std::string str ) noexcept;
	//extern template dclspc_dim Time sto_noexcept( std::string str ) noexcept;
	//extern template dclspc_dim Velocity sto_noexcept( std::string str ) noexcept;
	//extern template dclspc_dim Acceleration sto_noexcept( std::string str ) noexcept;
	//extern template dclspc_dim Force sto_noexcept( std::string str ) noexcept;
	//extern template dclspc_dim Pressure sto_noexcept( std::string str ) noexcept;
	//extern template dclspc_dim Momentum sto_noexcept( std::string str ) noexcept;
	//extern template dclspc_dim AngularMomentum sto_noexcept( std::string str ) noexcept;
	////extern template dclspc_dim Torque sto_noexcept( std::string str ) noexcept;
	//extern template dclspc_dim Energy sto_noexcept( std::string str ) noexcept;
	//extern template dclspc_dim Power sto_noexcept( std::string str ) noexcept;

} //namespace dim

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

#include "dim/support/DimSupportStream.h"
#include "dim/DimLimits.h"

#include <algorithm>
#include <cctype>

namespace dim{

	Real KilogramsPerUnit() noexcept{
		return kilograms_per_unit;
	}

	Real MetersPerUnit() noexcept{
		 return meters_per_unit;
	}

	Real SecondsPerUnit() noexcept{
		return seconds_per_unit;
	}

	//inline bool TryRead( std::istream& istr, std::string word ){
	//	if( word.empty() )
	//		return true;

	//	int c = istr.get();
	//	if( c == EOF ){
	//		istr.clear();
	//		return false;
	//	}

	//	if( c == word.front() && TryRead( istr, word.substr( 1 ) ) )
	//		return true;

	//	istr.unget();
	//	return false;
	//}

	//template<int L,int M,int T>
	//inline std::istream& ReadDimensionatedValue( std::istream& istr, Value<Dimension<L,M,T>>& dimValue, std::string units ){
	//	Real value;
	//	istr >> value;
	//	TryRead(istr,units);
	//	dimValue = Value<Dimension<L,M,T>>{value};
	//	return istr;
	//}

template<typename Value>
inline int geti() { 
	static int i = std::ios_base::xalloc();
	return i;
}

//typedef std::ostream& (*StreamerAngle)( std::ostream&, Angle );
//typedef Angle (*RealToAngle)( Real );

//StreamerAngle DefaultStreamerAngle = _rad;
//RealToAngle DefaultUnitAngle = _rad;

typedef std::ostream& (*StreamOutAngle)( std::ostream&, Angle );

StreamOutAnglePerLength DefaultStreamOutAnglePerLength = _1Im;
StreamInAnglePerLength DefaultStreamInAnglePerLength = _1Im;
StreamOutAngle DefaultStreamOutAngle = _rad;
StreamInAngle DefaultStreamInAngle = _rad;
StreamOutLength DefaultStreamOutLength = _m;
StreamInLength DefaultStreamInLength = _m;
StreamOutVelocity DefaultStreamOutVelocity = _kmIh;
StreamInVelocity DefaultStreamInVelocity = _kmIh;
StreamOutAcceleration DefaultStreamOutAcceleration = _mIs2;
StreamInAcceleration DefaultStreamInAcceleration = _mIs2;
StreamOutMass DefaultStreamOutMass = _kg;
StreamInMass DefaultStreamInMass = _kg;
StreamOutDensity DefaultStreamOutDensity = _gIcm3;
StreamInDensity DefaultStreamInDensity = _gIcm3;
StreamOutTime DefaultStreamOutTime = _s;
StreamInTime DefaultStreamInTime = _s;
StreamOutForce DefaultStreamOutForce = _kN;
StreamInForce DefaultStreamInForce = _kN;
StreamOutPressure DefaultStreamOutPressure = _kPa;
StreamInPressure DefaultStreamInPressure = _kPa;
StreamOutMomentum DefaultStreamOutMomentum = _tmIs;
StreamInMomentum DefaultStreamInMomentum = _tmIs;
StreamOutAngularMomentum DefaultStreamOutAngularMomentum = _tm2Is;
StreamInAngularMomentum DefaultStreamInAngularMomentum = _tm2Is;
StreamOutTorque DefaultStreamOutTorque = _kNm;
StreamInTorque DefaultStreamInTorque = _kNm;
StreamOutPower DefaultStreamOutPower = _kW;
StreamInPower DefaultStreamInPower = _kW;


	std::pair<Real,std::string> ReadValueAndAppendix( std::istream& istr ){
		std::string code;
		istr >> code;

		if( code.empty() )
			return std::make_pair(0.f,"");

		if( !std::isalnum( code.back() ) )
		// might have taken a separator with it...
		{
			istr.putback( code.back() );
			code.pop_back();
		}

		std::string specials{ code };
		std::transform( code.begin(), code.end(), specials.begin(), common::to_lowercase );

		if( specials == "inf" ||
			specials == "+inf" ||
			specials == "infinite" ||
			specials == "+infinite" )
			return std::make_pair(+infinite,"");
		else if( specials == "-inf" ||
				 specials == "-infinite" )
			return std::make_pair(-infinite,"");
		else{
			std::string::size_type sz;
			Real value = std::stof( code, &sz );
			return std::make_pair(value,code.substr( sz ));
		}
	}

	std::pair<Real,std::string> ReadAngleAndAppendix( std::istream& istr ){
		std::string code;
		istr >> code;

		if( code.empty() )
			return std::make_pair(0.f,"");

		if( !std::isalnum( code.back() ) )
		// might have taken a separator with it...
		{
			istr.putback( code.back() );
			code.pop_back();
		}

		std::string specials{ code };
		std::transform( code.begin(), code.end(), specials.begin(), common::to_lowercase );

		if( specials == "2pi" )
			return std::make_pair(+2*pi,"rad");
		else if( specials == "pi" )
			return std::make_pair(+pi,"rad");
		else if( specials == "pi/2" )
			return std::make_pair(+pi/2,"rad");
		else if( specials == "pi/4" )
			return std::make_pair(+pi/4,"rad");
		else if( specials == "pi/8" )
			return std::make_pair(+pi/8,"rad");
		else if( specials == "-pi/8" )
			return std::make_pair(-pi/8,"rad");
		else if( specials == "-pi/4" )
			return std::make_pair(-pi/4,"rad");
		else if( specials == "-pi/2" )
			return std::make_pair(-pi/2,"rad");
		else if( specials == "-pi" )
			return std::make_pair(-pi,"rad");
		else if( specials == "-2pi" )
			return std::make_pair(-2*pi,"rad");
		else if( specials == "tau" )
			return std::make_pair(+tau,"rad");
		else if( specials == "tau/2" )
			return std::make_pair(+tau/2,"rad");
		else if( specials == "tau/4" )
			return std::make_pair(+tau/4,"rad");
		else if( specials == "-tau" )
			return std::make_pair(-tau,"rad");
		else if( specials == "-tau/2" )
			return std::make_pair(-tau/2,"rad");
		else if( specials == "-tau/4" )
			return std::make_pair(-tau/4,"rad");
		else
		{
			std::string::size_type sz;
			Real value = std::stof( code, &sz );
			return std::make_pair(value,code.substr( sz ));
		}
	}

	std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<0,0,0>>& angle ){
		if( ostr.pword(geti<Angle>()) )
			return (reinterpret_cast<StreamOutAngle>(ostr.pword(geti<Angle>())))(ostr,angle);

		return DefaultStreamOutAngle( ostr, angle );
	}

	std::istream& operator >> ( std::istream& istr, Value<Dimension<0,0,0>>& one ){
		auto pair = ReadAngleAndAppendix( istr );

		if( auto _x = StreamInAngleFrom(pair.second) )
			one = _x(pair.first);
		else
			one = DefaultStreamInAngle(pair.first);

		return istr;
	}

	std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<-1,0,0>>& anglePerLength ){
		if( ostr.pword(geti<Value<Dimension<-1,0,0>>>()) )
			return (reinterpret_cast<StreamOutAnglePerLength>(ostr.pword(geti<Value<Dimension<-1,0,0>>>())))(ostr,anglePerLength);

		return DefaultStreamOutAnglePerLength ? DefaultStreamOutAnglePerLength( ostr, anglePerLength ) : ostr << anglePerLength.Units();


		//ostr << _1Im(anglePerLength);
		//if( abs(anglePerLength).Units() != infinite )
		//	ostr << "_1/m";

		//return ostr;
	}

	std::istream& operator >> ( std::istream& istr, Value<Dimension<-1,0,0>>& anglePerLength ){
		auto pair = ReadValueAndAppendix( istr );

		if( auto _x = StreamInAnglePerLengthFrom(pair.second) )
			anglePerLength = _x(pair.first);
		else
			anglePerLength = DefaultStreamInAnglePerLength(pair.first);

		return istr;


		//anglePerLength = _1Im(ReadValueAndAppendix( istr ).first);
		//return istr;
	}

	std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<1,0,0>>& length ){
		if( ostr.pword(geti<Value<Dimension<1,0,0>>>()) )
			return (reinterpret_cast<StreamOutLength>(ostr.pword(geti<Value<Dimension<1,0,0>>>())))(ostr,length);

		return DefaultStreamOutLength ? DefaultStreamOutLength( ostr, length ) : ostr << length.Units();
	}

	std::istream& operator >> ( std::istream& istr, Value<Dimension<1,0,0>>& length ){
		auto pair = ReadValueAndAppendix( istr );

		if( auto _x = StreamInLengthFrom(pair.second) )
			length = _x(pair.first);
		else
			length = DefaultStreamInLength(pair.first);

		return istr;
	}

	std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<2,0,0>>& area ){
		ostr << _m2(area);
		if( abs(area).Units() != infinite )	
			ostr << "m2";

		return ostr;
	}

	std::istream& operator >> ( std::istream& istr, Value<Dimension<2,0,0>>& area ){
		area = _m2(ReadValueAndAppendix( istr ).first);
		return istr;
	}

	std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<3,0,0>>& volume ){
		ostr << _m3(volume);
		if( abs(volume).Units() != infinite )		
			ostr << "m3";

		return ostr;
	}

	std::istream& operator >> ( std::istream& istr, Value<Dimension<3,0,0>>& volume ){
		volume = _m3(ReadValueAndAppendix( istr ).first);
		return istr;
	}

	std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<0,0,1>>& time ){
		if( ostr.pword(geti<Value<Dimension<0,0,1>>>()) )
			return (reinterpret_cast<StreamOutTime>(ostr.pword(geti<Value<Dimension<0,0,1>>>())))(ostr,time);

		return DefaultStreamOutTime ? DefaultStreamOutTime( ostr, time ) : ostr << time.Units();
	}

	std::istream& operator >> ( std::istream& istr, Value<Dimension<0,0,1>>& time ){
		auto pair = ReadValueAndAppendix( istr );

		if( auto _x = StreamInTimeFrom(pair.second) )
			time = _x(pair.first);
		else
			time = DefaultStreamInTime(pair.first);

		return istr;
	}

	std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<0,0,-1>>& angularvelocity ){
		ostr << _1Is(angularvelocity);
		if( abs(angularvelocity).Units() != infinite )		
			ostr << "_1/s";

		return ostr;
	}

	std::istream& operator >> ( std::istream& istr, Value<Dimension<0,0,-1>>& angularvelocity ){
		angularvelocity = _1Is(ReadValueAndAppendix( istr ).first);
		return istr;
	}

	std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<1,0,-1>>& velocity ){
		if( ostr.pword(geti<Value<Dimension<1,0,-1>>>()) )
			return (reinterpret_cast<StreamOutVelocity>(ostr.pword(geti<Value<Dimension<1,0,-1>>>())))(ostr,velocity);

		return DefaultStreamOutVelocity ? DefaultStreamOutVelocity( ostr, velocity ) : ostr << velocity.Units();
	}

	std::istream& operator >> ( std::istream& istr, Value<Dimension<1,0,-1>>& velocity ){
		auto pair = ReadValueAndAppendix( istr );

		if( auto _x = StreamInVelocityFrom(pair.second) )
			velocity = _x(pair.first);
		else
			velocity = DefaultStreamInVelocity(pair.first);

		return istr;
	}

	std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<1,0,-2>>& acceleration ){
		if( ostr.pword(geti<Value<Dimension<1,0,-2>>>()) )
			return (reinterpret_cast<StreamOutAcceleration>(ostr.pword(geti<Value<Dimension<1,0,-2>>>())))(ostr,acceleration);

		return DefaultStreamOutAcceleration ? DefaultStreamOutAcceleration( ostr, acceleration ) : ostr << acceleration.Units();
	}

	std::istream& operator >> ( std::istream& istr, Value<Dimension<1,0,-2>>& acceleration ){
		auto pair = ReadValueAndAppendix( istr );

		if( auto _x = StreamInAccelerationFrom(pair.second) )
			acceleration = _x(pair.first);
		else
			acceleration = DefaultStreamInAcceleration(pair.first);

		return istr;
	}

	std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<0,1,0>>& mass ){
		if( ostr.pword(geti<Value<Dimension<0,1,0>>>()) )
			return (reinterpret_cast<StreamOutMass>(ostr.pword(geti<Value<Dimension<0,1,0>>>())))(ostr,mass);

		return DefaultStreamOutMass ? DefaultStreamOutMass( ostr, mass ) : ostr << mass.Units();
	}

	std::istream& operator >> ( std::istream& istr, Value<Dimension<0,1,0>>& mass ){
		auto pair = ReadValueAndAppendix( istr );

		if( auto _x = StreamInMassFrom(pair.second) )
			mass = _x(pair.first);
		else
			mass = DefaultStreamInMass(pair.first);

		return istr;
	}

	std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<-3,1,0>>& density ){
		if( ostr.pword(geti<Value<Dimension<-3,1,0>>>()) )
			return (reinterpret_cast<StreamOutDensity>(ostr.pword(geti<Value<Dimension<-3,1,0>>>())))(ostr,density);

		return DefaultStreamOutDensity ? DefaultStreamOutDensity( ostr, density ) : ostr << density.Units();
	}

	std::istream& operator >> ( std::istream& istr, Value<Dimension<-3,1,0>>& density ){
		auto pair = ReadValueAndAppendix( istr );

		if( auto _x = StreamInDensityFrom(pair.second) )
			density = _x(pair.first);
		else
			density = DefaultStreamInDensity(pair.first);

		return istr;
	}

	std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<1,1,-2>>& force ){
		if( ostr.pword(geti<Value<Dimension<1,1,-2>>>()) )
			return (reinterpret_cast<StreamOutForce>(ostr.pword(geti<Value<Dimension<1,1,-2>>>())))(ostr,force);

		return DefaultStreamOutForce ? DefaultStreamOutForce( ostr, force ) : ostr << force.Units();
	}

	std::istream& operator >> ( std::istream& istr, Value<Dimension<1,1,-2>>& force ){
		auto pair = ReadValueAndAppendix( istr );

		if( auto _x = StreamInForceFrom(pair.second) )
			force = _x(pair.first);
		else
			force = DefaultStreamInForce(pair.first);

		return istr;
	}

	std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<-1, 1, -2>>& pressure ){
		if( ostr.pword(geti<Value<Dimension<-1,1,-2>>>()) )
			return (reinterpret_cast<StreamOutPressure>(ostr.pword(geti<Value<Dimension<1,1,-2>>>())))(ostr,pressure);

		return DefaultStreamOutPressure ? DefaultStreamOutPressure( ostr, pressure ) : ostr << pressure.Units();
	}

	std::istream& operator >> ( std::istream& istr, Value<Dimension<-1, 1, -2>>& pressure ){
		auto pair = ReadValueAndAppendix( istr );

		if( auto _x = StreamInPressureFrom(pair.second) )
			pressure = _x(pair.first);
		else
			pressure = DefaultStreamInPressure(pair.first);

		return istr;
	}
	
	std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<1, 1, -1>>& momentum ){
		if( ostr.pword(geti<Value<Dimension<1, 1, -1>>>()) )
			return (reinterpret_cast<StreamOutMomentum>(ostr.pword(geti<Value<Dimension<1, 1, -1>>>())))(ostr,momentum);

		return DefaultStreamOutMomentum ? DefaultStreamOutMomentum( ostr, momentum ) : ostr << momentum.Units();
	}

	std::istream& operator >> ( std::istream& istr, Value<Dimension<1, 1, -1>>& momentum ){
		auto pair = ReadValueAndAppendix( istr );

		if( auto _x = StreamInMomentumFrom(pair.second) )
			momentum = _x(pair.first);
		else
			momentum = DefaultStreamInMomentum(pair.first);

		return istr;
	}

	std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<2, 1, -1>>& angularmomentum ){
		if( ostr.pword(geti<Value<Dimension<2, 1, -1>>>()) )
			return (reinterpret_cast<StreamOutAngularMomentum>(ostr.pword(geti<Value<Dimension<2, 1, -1>>>())))(ostr,angularmomentum);

		return DefaultStreamOutAngularMomentum ? DefaultStreamOutAngularMomentum( ostr, angularmomentum ) : ostr << angularmomentum.Units();
	}

	std::istream& operator >> ( std::istream& istr, Value<Dimension<2, 1, -1>>& angularmomentum ){
		auto pair = ReadValueAndAppendix( istr );

		if( auto _x = StreamInAngularMomentumFrom(pair.second) )
			angularmomentum = _x(pair.first);
		else
			angularmomentum = DefaultStreamInAngularMomentum(pair.first);

		return istr;
	}

	std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<2,1,-2>>& torque ){
		ostr << _kNm(torque);
		if( abs(torque).Units() != infinite )		
			ostr << "kNm";

		return ostr;
	}

	std::istream& operator >> ( std::istream& istr, Value<Dimension<2,1,-2>>& torque ){
		auto pair = ReadValueAndAppendix( istr );

		if( pair.second == "kNm" )
			torque = _kNm(pair.first);
		else if( pair.second == "Nm" )
			torque = _Nm(pair.first);
		else
			torque = _kNm(pair.first);

		return istr;
	}

	std::ostream& operator << ( std::ostream& ostr, const Value<Dimension<2,1,-3>>& power ){
		if( ostr.pword(geti<Value<Dimension<2,1,-3>>>()) )
			return (reinterpret_cast<StreamOutPower>(ostr.pword(geti<Value<Dimension<2,1,-3>>>())))(ostr,power);

		return DefaultStreamOutPower ? DefaultStreamOutPower( ostr, power ) : ostr << power.Units();
	}

	std::istream& operator >> ( std::istream& istr, Value<Dimension<2,1,-3>>& power ){
		auto pair = ReadValueAndAppendix( istr );

		if( auto _x = StreamInPowerFrom(pair.second) )
			power = _x(pair.first);
		else
			power = DefaultStreamInPower(pair.first);

		return istr;
	}

///////////////////////////////////////
template<typename DValueType>
std::ostream& _streamValue( 
	std::ostream& ostr, 
	DValueType dValue, 
	Real (*_x)( DValueType ), 
	DValueType infinity, 
	DValueType epsilon_, 
	std::string unit )
{
	if( dValue == infinity )
		ostr << "+inf";
	else if( dValue == -infinity )
		ostr << "-inf";
	else{
		if( _x == nullptr )
			return ostr;

		const auto flags = ostr.setf(std::ios::fixed);
		Real integer;
		const Real frac = std::abs(std::modf( _x(dValue), &integer ));
		const Real _xEpsilon = _x(epsilon_);

		const auto precision = ostr.precision((frac < _xEpsilon || (1-frac) < _xEpsilon) ? 0 : static_cast<std::streamsize>(std::ceil(std::log10(1.f/_xEpsilon))));

		ostr << _x(dValue) << unit;

		ostr.precision(precision);
		ostr.setf(flags);
	}

	return ostr;
}

template<typename DValueType>
std::ostream& _streamValue( 
	std::ostream& ostr, 
	DValueType dValue, 
	Real (*_x)( DValueType ), 
	DValueType (*_y)( Real ),
	std::ostream& _frac( std::ostream&, DValueType ),
	DValueType infinity, 
	DValueType epsilon_, 
	std::string unit )
{
	if( dValue == infinity )
		ostr << "+inf";
	else if( dValue == -infinity )
		ostr << "-inf";
	else{
		if( _x == nullptr || _y == nullptr || _frac == nullptr )
			return ostr;

		const auto flags = ostr.setf(std::ios::fixed);
		Real integer;
		const Real frac = std::modf( _x(dValue), &integer );
		const Real _xEpsilon = _x(epsilon_);

		const auto precision = ostr.precision(0);

		if( std::abs(frac) >= 1.0f -_xEpsilon )
			ostr << (frac < 0 ? --integer : ++integer) << unit;
		else if( std::abs(frac) >= _xEpsilon ){
			if( integer )
				ostr << integer << unit << ' ';

			_frac(ostr, _y(frac));
		}
		else
			ostr << integer << unit;

		ostr.precision(precision);
		ostr.setf(flags);
	}

	return ostr;
}

std::ostream& _deg( std::ostream& ostr, Angle angle ){
	return _streamValue( ostr, angle, _deg, Angle{infinite}, epsilon__angle, "deg" );
}

std::ostream& _deg( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Angle ) = _deg;
	ostr.pword(geti<Angle>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _deg0(std::ostream& ostr, Angle angle){
	return _streamValue( ostr, angle, _deg, Angle{infinite}, epsilon__angle, "" );
}

std::ostream& _deg0( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Angle ) = _deg0;
	ostr.pword(geti<Angle>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _rad( std::ostream& ostr, Angle angle ){
	if( angle == +infinite )
		ostr << "+inf";
	else if( angle == +tau )
		ostr << "2pi";
	else if( angle == +pi )
		ostr << "pi";
	else if( angle == +pi/2 )
		ostr << "pi/2";
	else if( angle == +pi/4 )
		ostr << "pi/4";
	else if( angle == -pi/4 )
		ostr << "-pi/4";
	else if( angle == -pi/2 )
		ostr << "-pi/2";
	else if( angle == -pi )
		ostr << "-pi";
	else if( angle == -2*pi )
		ostr << "-2pi";
	else if( angle == -infinite )
		ostr << "-inf";
	else{
		const int neededPrecision = static_cast<int>(std::ceil( -std::log10( epsilon ) ));
		const auto precision = ostr.precision(neededPrecision); //Make Normalize() with epsilon work.
		ostr << angle.Units();
		ostr.precision( precision );
	}

	return ostr;
}

std::ostream& _rad( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Angle ) = _rad;
	ostr.pword(geti<Angle>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _Angle( std::ostream& ostr ){
	ostr.pword(geti<Angle>()) = nullptr;
	return ostr;
}

std::ostream& _degIm( std::ostream& ostr, AnglePerLength angleperlength ){
	return _streamValue( ostr, angleperlength, _degIm, infinite__anglePerLength, epsilon__anglePerLength, "deg/m" );
}

std::ostream& _degIm( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, AnglePerLength ) = _degIm;
	ostr.pword(geti<AnglePerLength>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _degIcm(std::ostream& ostr, AnglePerLength angleperlength){
	return _streamValue( ostr, angleperlength, _degIcm, infinite__anglePerLength, epsilon__anglePerLength, "deg/cm" );
}

std::ostream& _degIcm(std::ostream& ostr){
	std::ostream& (*pf)( std::ostream&, AnglePerLength ) = _degIcm;
	ostr.pword(geti<AnglePerLength>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _1Im( std::ostream& ostr, AnglePerLength angleperlength ){
	return _streamValue( ostr, angleperlength, _1Im, infinite__anglePerLength, epsilon__anglePerLength, "_1/m" );
}

std::ostream& _1Im( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, AnglePerLength ) = _1Im;
	ostr.pword(geti<AnglePerLength>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _1ILength( std::ostream& ostr ){
	ostr.pword(geti<AnglePerLength>()) = nullptr;
	return ostr;
}

std::ostream& _m( std::ostream& ostr, Length length ){
	return _streamValue( ostr, length, _m, infinite__length, epsilon__length, "m" );
}

std::ostream& _m( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Length ) = _m;
	ostr.pword(geti<Length>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _mm( std::ostream& ostr, Length length ){
	return _streamValue( ostr, length, _mm, infinite__length, epsilon__length, "mm" );
}

std::ostream& _mm( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Length ) = _mm;
	ostr.pword(geti<Length>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _cm( std::ostream& ostr, Length length ){
	return _streamValue( ostr, length, _cm, infinite__length, epsilon__length, "cm" );
}

std::ostream& _cm( std::ostream& ostr ){
	StreamOutLength pf = _cm;
	ostr.pword(geti<Length>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _dm( std::ostream& ostr, Length length ){
	return _streamValue( ostr, length, _dm, infinite__length, epsilon__length, "dm" );
}

std::ostream& _dm( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Length ) = _dm;
	ostr.pword(geti<Length>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _km( std::ostream& ostr, Length length ){
	return _streamValue( ostr, length, _km, infinite__length, epsilon__length, "km" );
}

std::ostream& _km( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Length ) = _km;
	ostr.pword(geti<Length>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _mi( std::ostream& ostr, Length length ){
	return _streamValue( ostr, length, _mi, _mi, _yd, infinite__length, epsilon__length, "mi" );
}

std::ostream & _mi( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Length ) = _mi;
	ostr.pword(geti<Length>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _nmi( std::ostream& ostr, Length length ){
	return _streamValue( ostr, length, _nmi, _nmi, _ft, infinite__length, epsilon__length, "nmi" );
}

std::ostream& _nmi( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Length ) = _nmi;
	ostr.pword(geti<Length>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _yd( std::ostream& ostr, Length length ){
	return _streamValue( ostr, length, _yd, _yd, _in, infinite__length, epsilon__length, "yd" );
}

std::ostream& _yd( std::ostream& ostr){
	std::ostream& (*pf)( std::ostream&, Length ) = _yd;
	ostr.pword(geti<Length>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _ft( std::ostream& ostr,Length length ){
	return _streamValue( ostr, length, _ft, _ft, _in, infinite__length, epsilon__length, "ft" );
}

std::ostream& _ft( std::ostream& ostr){
	std::ostream& (*pf)( std::ostream&, Length ) = _ft;
	ostr.pword(geti<Length>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _in( std::ostream& ostr, Length length ){
	return _streamValue( ostr, length, _in, infinite__length, epsilon__length, "in" );
}

std::ostream& _in( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Length ) = _in;
	ostr.pword(geti<Length>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _Length( std::ostream& ostr ){
	ostr.pword(geti<Length>()) = nullptr;
	return ostr;
}

std::ostream& _cmIs( std::ostream& ostr, Velocity velocity ){
	return _streamValue( ostr, velocity, _cmIs, infinite__velocity, epsilon__velocity, "cm/s" );
}

std::ostream& _cmIs( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Velocity ) = _cmIs;
	ostr.pword(geti<Velocity>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _kmIh( std::ostream& ostr, Velocity velocity ){
	return _streamValue( ostr, velocity, _kmIh, infinite__velocity, epsilon__velocity, "km/h" );
}

std::ostream& _kmIh( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Velocity ) = _kmIh;
	ostr.pword(geti<Velocity>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _mIs( std::ostream& ostr, Velocity velocity ){
	return _streamValue( ostr, velocity, _mIs, infinite__velocity, epsilon__velocity, "m/s" );
}

std::ostream& _mIs( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Velocity ) = _mIs;
	ostr.pword(geti<Velocity>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _mph( std::ostream& ostr, Velocity velocity ){
	return _streamValue( ostr, velocity, _mph, infinite__velocity, epsilon__velocity, "mph" );
}

std::ostream& _mph( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Velocity ) = _mph;
	ostr.pword(geti<Velocity>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _kn( std::ostream& ostr, Velocity velocity ){
	return _streamValue( ostr, velocity, _kn, infinite__velocity, epsilon__velocity, "kn" );
}

std::ostream& _kn( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Velocity ) = _kn;
	ostr.pword(geti<Velocity>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _Velocity( std::ostream& ostr ){
	ostr.pword(geti<Velocity>()) = nullptr;
	return ostr;
}

std::ostream& _mIs2( std::ostream& ostr, Acceleration acceleration ){
	return _streamValue( ostr, acceleration, _mIs2, infinite__accelearation, epsilon__acceleration, "m/s2" );
}

std::ostream& _mIs2( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Acceleration ) = _mIs2;
	ostr.pword(geti<Acceleration>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _g( std::ostream& ostr, Mass mass ){
	return _streamValue( ostr, mass, _g, infinite__mass, epsilon__mass, "g" );
}

std::ostream& _g( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Mass ) = _g;
	ostr.pword(geti<Mass>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _kg( std::ostream& ostr, Mass mass ){
	return _streamValue( ostr, mass, _kg, infinite__mass, epsilon__mass, "kg" );
}

std::ostream& _kg( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Mass ) = _kg;
	ostr.pword(geti<Mass>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _t( std::ostream& ostr, Mass mass ){
	return _streamValue( ostr, mass, _t, infinite__mass, epsilon__mass, "t" );
}

std::ostream& _t( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Mass ) = _t;
	ostr.pword(geti<Mass>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _lb( std::ostream& ostr, Mass mass ){
	return _streamValue( ostr, mass, _lb, _lb, _oz, infinite__mass, epsilon__mass, "lb" );
}

std::ostream& _lb( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Mass ) = _lb;
	ostr.pword(geti<Mass>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _oz( std::ostream& ostr, Mass mass ){
	return _streamValue( ostr, mass, _oz, infinite__mass, epsilon__mass, "oz" );
}

std::ostream& _oz( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Mass ) = _oz;
	ostr.pword(geti<Mass>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _long_tons( std::ostream& ostr, Mass mass ){
	return _streamValue( ostr, mass, _long_tons, _long_tons, _lb, infinite__mass, epsilon__mass, "long_tons" );
}

std::ostream& _long_tons( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Mass ) = _long_tons;
	ostr.pword(geti<Mass>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _short_tons( std::ostream& ostr, Mass mass ){
	return _streamValue( ostr, mass, _short_tons, _short_tons, _lb, infinite__mass, epsilon__mass, "short_tons" );
}

std::ostream& _short_tons( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Mass ) = _short_tons;
	ostr.pword(geti<Mass>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _Mass( std::ostream & ostr ){
	ostr.pword(geti<Mass>()) = nullptr;
	return ostr;
}


std::ostream& _gIcm3( std::ostream& ostr, Density density ){
	return _streamValue( ostr, density, _gIcm3, infinite__density, epsilon__density, "g/cm3" );
}

std::ostream& _gIcm3( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Density ) = _gIcm3;
	ostr.pword(geti<Density>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _kgIm3( std::ostream& ostr, Density density ){
	return _streamValue( ostr, density, _kgIm3, infinite__density, epsilon__density, "kg/m3" );
}

std::ostream& _kgIm3( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Density ) = _kgIm3;
	ostr.pword(geti<Density>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _tIm3( std::ostream& ostr, Density density ){
	return _streamValue( ostr, density, _tIm3, infinite__density, epsilon__density, "t/m3" );
}

std::ostream& _tIm3( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Density ) = _tIm3;
	ostr.pword(geti<Density>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _ms( std::ostream& ostr, Time time ){
	return _streamValue( ostr, time, _ms, infinite__time, epsilon__time, "ms" );
}

std::ostream& _ms( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Time ) = _ms;
	ostr.pword(geti<Time>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _s( std::ostream& ostr, Time time ){
	return _streamValue( ostr, time, _s, infinite__time, epsilon__time, "s" );
}

std::ostream& _s( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Time ) = _s;
	ostr.pword(geti<Time>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _N( std::ostream& ostr, Force force ){
	return _streamValue( ostr, force, _N, infinite__force, epsilon__force, "N" );
}

std::ostream& _N( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Force ) = _N;
	ostr.pword(geti<Force>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _kN( std::ostream& ostr, Force force ){
	return _streamValue( ostr, force, _kN, infinite__force, epsilon__force, "kN" );
}

std::ostream& _kN( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Force ) = _kN;
	ostr.pword(geti<Force>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _kp( std::ostream& ostr, Force force ){
	return _streamValue( ostr, force, _kp, infinite__force, epsilon__force, "kp" );
}

std::ostream& _kp( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Force ) = _kp;
	ostr.pword(geti<Force>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _lbf( std::ostream& ostr, Force force ){
	return _streamValue( ostr, force, _lbf, infinite__force, epsilon__force, "lbf" );
}

std::ostream& _lbf( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Force ) = _lbf;
	ostr.pword(geti<Force>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream & _long_tons_force( std::ostream& ostr, Force force ){
	return _streamValue( ostr, force, _long_tons_force, infinite__force, epsilon__force, "long-tons-force" );
}

std::ostream& _long_tons_force( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Force ) = _long_tons_force;
	ostr.pword(geti<Force>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _short_tons_force( std::ostream & ostr, Force force ){
	return _streamValue( ostr, force, _short_tons_force, infinite__force, epsilon__force, "short-tons-force" );
}

std::ostream& _short_tons_force( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Force ) = _short_tons_force;
	ostr.pword(geti<Force>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _Force( std::ostream& ostr ){
	ostr.pword(geti<Force>()) = nullptr;
	return ostr;
}

std::ostream& _Pa( std::ostream& ostr, Pressure pressure ){
	return _streamValue( ostr, pressure, _Pa, infinite__pressure, epsilon__pressure, "Pa" );	
}

std::ostream& _Pa( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Pressure ) = _Pa;
	ostr.pword(geti<Pressure>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _kPa( std::ostream & ostr, Pressure pressure ){
	return _streamValue( ostr, pressure, _kPa, infinite__pressure, epsilon__pressure, "kPa" );
}

std::ostream& _kPa( std::ostream & ostr ){
	std::ostream& (*pf)(std::ostream&, Pressure) = _kPa;
	ostr.pword( geti<Pressure>() ) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _bar( std::ostream & ostr, Pressure pressure ){
	return _streamValue( ostr, pressure, _bar, infinite__pressure, epsilon__pressure, "bar" );
}

std::ostream& _bar( std::ostream & ostr ){
	std::ostream& (*pf)(std::ostream&, Pressure) = _bar;
	ostr.pword( geti<Pressure>() ) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _Pressure( std::ostream& ostr ){
	ostr.pword(geti<Pressure>()) = nullptr;
	return ostr;
}

std::ostream& _kgmIs( std::ostream& ostr, Momentum momentum ){
	return _streamValue( ostr, momentum, _kgmIs, infinite__momentum, epsilon__momentum, "kgm/s" );
}

std::ostream& _kgmIs( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Momentum ) = _kgmIs;
	ostr.pword(geti<Momentum>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _tmIs( std::ostream& ostr, Momentum momentum ){
	return _streamValue( ostr, momentum, _tmIs, infinite__momentum, epsilon__momentum, "tm/s" );
}

std::ostream& _tmIs( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Momentum ) = _tmIs;
	ostr.pword(geti<Momentum>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _kgm2Is( std::ostream& ostr, AngularMomentum angularmomentum ){
	return _streamValue( ostr, angularmomentum, _kgm2Is, infinite__angularMomentum, epsilon__angularMomentum, "kgm2/s" );
}

std::ostream& _kgm2Is( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, AngularMomentum ) = _kgm2Is;
	ostr.pword(geti<AngularMomentum>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _tm2Is( std::ostream& ostr, AngularMomentum angularmomentum ){
	return _streamValue( ostr, angularmomentum, _tm2Is, infinite__angularMomentum, epsilon__angularMomentum, "tm2/s" );
}

std::ostream& _tm2Is( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, AngularMomentum ) = _tm2Is;
	ostr.pword(geti<AngularMomentum>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _Nm( std::ostream& ostr, Torque torque ){
	return _streamValue( ostr, torque, _Nm, infinite__torque, epsilon__torque, "Nm" );
}

std::ostream& _Nm( std::ostream& ostr ){
	std::ostream& (*pt)( std::ostream&, Torque ) = _Nm;
	ostr.pword(geti<Torque>()) = reinterpret_cast<void*>(pt);
	return ostr;
}

std::ostream& _kNm( std::ostream& ostr, Torque torque ){
	return _streamValue( ostr, torque, _kNm, infinite__torque, epsilon__torque, "kNm" );
}

std::ostream& _kNm( std::ostream& ostr ){
	std::ostream& (*pt)( std::ostream&, Torque ) = _kNm;
	ostr.pword(geti<Torque>()) = reinterpret_cast<void*>(pt);
	return ostr;
}

std::ostream & _Torque(std::ostream & ostr){
	ostr.pword(geti<Torque>()) = nullptr;
	return ostr;
}

std::ostream& _W( std::ostream& ostr, Power power ){
	return _streamValue( ostr, power, _W, infinite__power, epsilon__power, "W" );
}

std::ostream& _W( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Power ) = _W;
	ostr.pword(geti<Power>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _kW( std::ostream& ostr, Power power ){
	return _streamValue( ostr, power, _kW, infinite__power, epsilon__power, "kW" );
}

std::ostream& _kW( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Power ) = _kW;
	ostr.pword(geti<Power>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _PS( std::ostream& ostr, Power power ){
	return _streamValue( ostr, power, _PS, infinite__power, epsilon__power, "PS" );
}

std::ostream& _PS( std::ostream& ostr ){
	std::ostream& (*pf)( std::ostream&, Power ) = _PS;
	ostr.pword(geti<Power>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _hp( std::ostream& ostr, Power power ){
	return _streamValue( ostr, power, _hp, infinite__power, epsilon__power, "hp" );
}

std::ostream& _hp( std::ostream& ostr){
	std::ostream& (*pf)( std::ostream&, Power ) = _hp;
	ostr.pword(geti<Power>()) = reinterpret_cast<void*>(pf);
	return ostr;
}

std::ostream& _Power( std::ostream& ostr ){
	ostr.pword(geti<Power>()) = nullptr;
	return ostr;
}

void SetDefaultStreamOfAngle( StreamOutAngle _x, StreamInAngle _y ) noexcept{
	if( _x ) DefaultStreamOutAngle = _x;
	else DefaultStreamOutAngle = _rad;
	if( _y ) DefaultStreamInAngle = _y;
	else DefaultStreamInAngle = _rad;
}

void SetDefaultStreamOfAnglePerLength( StreamOutAnglePerLength _x, StreamInAnglePerLength _y ) noexcept{
	DefaultStreamOutAnglePerLength = _x;
	if( _y ) DefaultStreamInAnglePerLength = _y;
}

void SetDefaultStreamOfLength( StreamOutLength _x, StreamInLength _y ) noexcept{
	DefaultStreamOutLength = _x;
	if( _y ) DefaultStreamInLength = _y;
}

void SetDefaultStreamOfVelocity( StreamOutVelocity _x, StreamInVelocity _y ) noexcept{
	DefaultStreamOutVelocity = _x;
	if( _y ) DefaultStreamInVelocity = _y;
}

void SetDefaultStreamOfMass( StreamOutMass _x, StreamInMass _y ) noexcept{
	DefaultStreamOutMass = _x;
	if( _y ) DefaultStreamInMass = _y;
}

void SetDefaultStreamOfTime( StreamOutTime _x, StreamInTime _y ) noexcept{
	DefaultStreamOutTime = _x;
	if( _y ) DefaultStreamInTime = _y;
}

void SetDefaultStreamOfForce( StreamOutForce _x, StreamInForce _y ) noexcept{
	DefaultStreamOutForce = _x;
	if( _y ) DefaultStreamInForce = _y;
}

void SetDefaultStreamOfPressure( StreamOutPressure _x, StreamInPressure _y ) noexcept{
	DefaultStreamOutPressure = _x;
	if( _y ) DefaultStreamInPressure = _y;
}

void SetDefaultStreamOfTorque(StreamOutTorque _x,StreamInTorque _y) noexcept{
	DefaultStreamOutTorque = _x;
	if( _y ) DefaultStreamInTorque = _y;
}

void SetDefaultStreamOfPower( StreamOutPower _x, StreamInPower _y ) noexcept{
	DefaultStreamOutPower = _x;
	if( _y ) DefaultStreamInPower = _y;
}

StreamOutLength StreamOutLengthFrom( std::string unit ){
	if( unit == "km" )
		return _km;
	if( unit == "m" )
		return _m;
	if( unit == "dm" )
		return _dm;
	if( unit == "cm" )
		return _cm;
	if( unit == "mm" )
		return _mm;
	if( unit == "mi" )
		return _mi;
	if( unit == "nmi" )
		return _nmi;
	if( unit == "yd" )
		return _yd;
	if( unit == "ft" )
		return _ft;
	if( unit == "in" )
		return _in;
	
	return nullptr;
}

StreamOutAnglePerLength StreamOutAnglePerLengthFrom( std::string unit ){
	if( unit == "_1/m" )
		return _1Im;
	if( unit == "deg/m" )
		return _degIm;
	if( unit == "deg/cm" )
		return _degIcm;
	
	return nullptr;
}

StreamOutVelocity StreamOutVelocityFrom( std::string unit ){
	if( unit == "m/s" )
		return _mIs;
	if( unit == "km/h" )
		return _kmIh;
	if( unit == "cm/s" )
		return _cmIs;
	if( unit == "mph" )
		return _mph;
	if( unit == "kn" )
		return _kn;
	
	return nullptr;
}

StreamOutAcceleration StreamOutAccelerationFrom(std::string unit){
	if( unit == "m/s2" )
		return _mIs2;
	
	return nullptr;
}

StreamOutMass StreamOutMassFrom( std::string unit ){
	if( unit == "t" )
		return _t;
	if( unit == "kg" )
		return _kg;
	if( unit == "g" )
		return _g;
	if( unit == "lb" )
		return _lb;
	if( unit == "oz" )
		return _oz;
	if( unit == "long-tons" )
		return _long_tons;
	if( unit == "short-tons" )
		return _short_tons;
	
	return nullptr;
}

StreamOutDensity StreamOutDensityFrom( std::string unit ){
	if( unit == "t/m3" )
		return _tIm3;
	if( unit == "kg/m3" )
		return _kgIm3;
	if( unit == "g/cm3" )
		return _gIcm3;
	
	return nullptr;
}

StreamOutForce StreamOutForceFrom( std::string unit ){
	if( unit == "kN" )
		return _kN;
	if( unit == "N" )
		return _N;
	if( unit == "kp" )
		return _kp;
	if( unit == "lbf" )
		return _lbf;
	if( unit == "long-tons-force" )
		return _long_tons_force;
	if( unit == "short-tons-force" )
		return _short_tons_force;
	
	return nullptr;
}

StreamOutPressure StreamOutPressureFrom( std::string unit ){
	if( unit == "Pa" )
		return _Pa;
	if( unit == "kPa" )
		return _kPa;
	if( unit == "bar" )
		return _bar;

	return nullptr;
}

StreamOutMomentum StreamOutMomentumFrom( std::string unit ){
	if( unit == "kgm/s" )
		return _kgmIs;
	if( unit == "tm/s" )
		return _tmIs;

	return nullptr;
}

StreamOutAngularMomentum StreamOutAngularMomentumFrom( std::string unit ){
	if( unit == "kgm2/s" )
		return _kgm2Is;
	if( unit == "tm2/s" )
		return _tm2Is;
	
	return nullptr;
}

StreamOutTorque StreamOutTorqueFrom( std::string unit ){
	if( unit == "kNm" )
		return _kNm;
	if( unit == "Nm" )
		return _Nm;

	return nullptr;
}

StreamOutPower StreamOutPowerFrom( std::string unit ){
	if( unit == "kW" )
		return _kW;
	if( unit == "W" )
		return _W;
	if( unit == "PS" )
		return _PS;
	if( unit == "hp" )
		return _hp;
	
	return nullptr;
}

StreamInAnglePerLength StreamInAnglePerLengthFrom( std::string unit ){
	if( unit == "_1/m" )
		return _1Im;
	if( unit == "deg/m" )
		return _degIm;
	if( unit == "deg/cm" )
		return _degIcm;

	return nullptr;
}

StreamInAngle StreamInAngleFrom( std::string unit ){
	if( unit == "deg" )
		return _deg;
	if( unit == "rad" )
		return _rad;
	if( unit == "\xB0" )
		return _deg;

	return nullptr;
}

StreamInLength StreamInLengthFrom( std::string unit ){
	if( unit == "km" )
		return _km;
	if( unit == "m" )
		return _m;
	if( unit == "dm" )
		return _dm;
	if( unit == "cm" )
		return _cm;
	if( unit == "mm" )
		return _mm;
	if( unit == "mi" )
		return _mi;
	if( unit == "nmi" )
		return _nmi;
	if( unit == "yd" )
		return _yd;
	if( unit == "ft" )
		return _ft;
	if( unit == "in" )
		return _in;
	
	return nullptr;
}

StreamInVelocity StreamInVelocityFrom( std::string unit ){
	if( unit == "m/s" )
		return _mIs;
	if( unit == "km/h" )
		return _kmIh;
	if( unit == "cm/s" )
		return _cmIs;
	if( unit == "mph" )
		return _mph;
	if( unit == "kn" )
		return _kn;
	
	return nullptr;
}

StreamInAcceleration StreamInAccelerationFrom( std::string unit ){
	if( unit == "m/s2" )
		return _mIs2;
	
	return nullptr;
}

StreamInMass StreamInMassFrom( std::string unit ){
	if( unit == "t" )
		return _t;
	if( unit == "kg" )
		return _kg;
	if( unit == "g" )
		return _g;
	if( unit == "lb" )
		return _lb;
	if( unit == "oz" )
		return _oz;
	if( unit == "long-tons" )
		return _long_tons;
	if( unit == "short-tons" )
		return _short_tons;
	
	return nullptr;
}

StreamInDensity StreamInDensityFrom( std::string unit ){
	if( unit == "t/m3" )
		return _tIm3;
	if( unit == "kg/m3" )
		return _kgIm3;
	if( unit == "g/cm3" )
		return _gIcm3;
	
	return nullptr;
}

StreamInTime StreamInTimeFrom(std::string unit){
	if( unit == "s" )
		return _s;
	if( unit == "ms" )
		return _ms;

	return nullptr;
}

StreamInForce StreamInForceFrom( std::string unit ){
	if( unit == "kN" )
		return _kN;
	if( unit == "N" )
		return _N;
	if( unit == "kp" )
		return _kp;
	if( unit == "lbf" )
		return _lbf;
	if( unit == "long-tons-force" )
		return _long_tons_force;
	if( unit == "short-tons-force" )
		return _short_tons_force;
	
	return nullptr;
}

StreamInPressure StreamInPressureFrom( std::string unit ){
	if( unit == "Pa" )
		return _Pa;
	if( unit == "kPa" )
		return _kPa;
	if( unit == "bar" )
		return _bar;
	return nullptr;
}

StreamInMomentum StreamInMomentumFrom( std::string unit ){
	if( unit == "kgm/s" )
		return _kgmIs;
	if( unit == "tm/s" )
		return _tmIs;
	
	return nullptr;
}

StreamInAngularMomentum StreamInAngularMomentumFrom( std::string unit ){
	if( unit == "kgm2/s" )
		return _kgm2Is;
	if( unit == "tm2/s" )
		return _tm2Is;
	
	return nullptr;
}

StreamInTorque StreamInTorqueFrom( std::string unit ){
	if( unit == "kNm" )
		return _kNm;
	if( unit == "Nm" )
		return _Nm;
	
	return nullptr;
}

StreamInPower StreamInPowerFrom( std::string unit ){
	if( unit == "kW" )
		return _kW;
	if( unit == "W" )
		return _W;
	if( unit == "PS" )
		return _PS;
	if( unit == "hp" )
		return _hp;
	
	return nullptr;
}

std::string to_string_deg( Angle val ){
	std::ostringstream stream;
	stream << _deg << val;
	return stream.str();
}

std::string to_string_deg0( Angle val ){
	std::ostringstream stream;
	stream << _deg0 << val;
	return stream.str();
}

std::string to_string_degIm( AnglePerLength val ){
	std::ostringstream stream;
	stream << _degIm << val;
	return stream.str();
}

std::string to_string_degIcm( AnglePerLength val ){
	std::ostringstream stream;
	stream << _degIcm << val;
	return stream.str();
}
//
//template One sto_noexcept( std::string str ) noexcept;
////template Angle sto_noexcept( std::string str ) noexcept; // doesn't work for clang One and Angle identical...
//template AnglePerLength sto_noexcept( std::string str ) noexcept;
//template AngularVelocity sto_noexcept( std::string str ) noexcept;
//template Length sto_noexcept( std::string str ) noexcept;
//template Area sto_noexcept( std::string str ) noexcept;
//template Volume sto_noexcept( std::string str ) noexcept;
//template Mass sto_noexcept( std::string str ) noexcept;
//template Density sto_noexcept( std::string str ) noexcept;
//template Time sto_noexcept( std::string str ) noexcept;
//template Velocity sto_noexcept( std::string str ) noexcept;
//template Acceleration sto_noexcept( std::string str ) noexcept;
//template Force sto_noexcept( std::string str ) noexcept;
//template Pressure sto_noexcept( std::string str ) noexcept;
//template Momentum sto_noexcept( std::string str ) noexcept;
//template AngularMomentum sto_noexcept( std::string str ) noexcept;
////template Torque sto_noexcept( std::string str ) noexcept; // doesn't work for clang One and Angle identical...
//template Energy sto_noexcept( std::string str ) noexcept;
//template Power sto_noexcept( std::string str ) noexcept;
///////////////////////////////////////
} // namespace dim

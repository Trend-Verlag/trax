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

#include "Clothoid_Imp.h"

namespace trax{
///////////////////////////////////////
const Angle ClothoidFunction::maximumAngle = 1*pi;
///////////////////////////////////////
Angle Clothoid::MaxAngle() noexcept{
	return ClothoidFunction::maximumAngle;
}

std::tuple<Interval<AnglePerLength>,Interval<AnglePerLength>,Interval<Length>> 
Clothoid::Limits( AnglePerLength k0, AnglePerLength k1, Length l )
{
	if( l <= 0_m )
		throw std::invalid_argument( "Can not create Clothoid from l <= 0_m!" );
	if( k1 <= k0 )
		throw std::invalid_argument( "Can not create Clothoid from curvature1 <= curvature0!" );
	if( k0 < 0_1Im || k1 < 0_1Im )
		throw std::invalid_argument( "Curvature can not be smaller 0!" );

	const Angle T = MaxAngle();

	return std::make_tuple(
		Interval<AnglePerLength>{ 0_1Im						,  k1 * (1.f - k1*l/(2*T))		},
		Interval<AnglePerLength>{ T/l - sqrt(T/l*(T/l-2*k0)), T/l + sqrt(T/l*(T/l-2*k0))	},
		Interval<Length>		{ 0_m						, 2*T*(k1-k0)/(k1*k1)			} );
}

std::tuple<Interval<Length>,Interval<Length>,Interval<Length>> 
Clothoid::Limits( Length r0, Length r1, Length l )
{
	if( r0 <= 0_m || r1 <= 0_m )
		throw std::invalid_argument( "Curve radii can not be smaller or equal to zero!" );

	auto limits = Limits( 1.f / r0, 1.f / r1, l );

	return std::make_tuple( 
		Interval<Length>{ 1.f / std::get<0>(limits).Far(), +infinite__length },
		Interval<Length>{ 1.f / std::get<1>(limits).Far(), 1.f / std::get<1>(limits).Near() },
		std::get<2>(limits) );
}

///////////////////////////////////////
common::Interval<Length> ClothoidFunction::Create( const DataType& data ){
	if( data.a <= 0_m )
		throw std::invalid_argument( "Clothoides a parameter must be > 0 !" );

	m_Data = data;
	a2 = m_Data.a * m_Data.a;
	return Range();
}
///////////////////////////////////////
std::unique_ptr<Clothoid> Clothoid::Make() noexcept{
	try{
		return std::make_unique<Clothoid_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
const char* Clothoid_Imp::TypeName() const noexcept{
	return "Clothoid";
}

Curve::CurveType Clothoid_Imp::GetCurveType() const noexcept{
	return Curve::CurveType::Clothoid;
}

std::unique_ptr<Curve> Clothoid_Imp::Clone() const{
	return std::make_unique<Clothoid_Imp>( *this );
}

spat::Vector<One> Clothoid_Imp::LocalUp() const noexcept{
	return Ez<One>;
}

spat::SquareMatrix<One,2> Clothoid_Imp::Jacobian( Length s ) const{
	return f.Jacobian( s );
}

common::Interval<Length> Clothoid_Imp::Create( Area a2 ){
	if( a2 < 0_m2 )
		throw std::invalid_argument( "Clothoid_Imp::Create: a2 has to be > 0" );

	DataType data;
	data.a = sqrt(a2);
	return f.Create( data );
}

common::Interval<Length> Clothoid_Imp::Create( AnglePerLength k0, AnglePerLength k1, Length l ){
	if( l <= 0_m )
		throw std::invalid_argument( "Can not create Clothoid from l <= 0_m!" );
	if( k1 <= k0 )
		throw std::invalid_argument( "Can not create Clothoid from curvature1 <= curvature0!" );
	if( k0 < 0_1Im || k1 < 0_1Im )
		throw std::invalid_argument( "Curvature can not be smaller 0!" );

	DataType data;
	const Area a2 = l / (k1 - k0);
	data.a = sqrt(a2);
	common::Interval<Length> range{ k0*a2, k1*a2 };
	range.Intersection( f.Create( data ) );
	return range;
}

common::Interval<Length> Clothoid_Imp::Create( Length r0, Length r1, Length l ){
	if( r0 <= 0_m || r1 <= 0_m )
		throw std::invalid_argument( "Curve radii can not be smaller or equal to zero!" );
	if( r0 <= r1 )
		throw std::invalid_argument( "Can not create Clothoid from radius0 <= radius1!" );

	return Create( 1/r0, 1/r1, l );
}

spat::Vector<One> Clothoid_Imp::Direction( Length s ) const{
	if( s > 0_m )
	{
		auto J = Jacobian( s );
		const AnglePerLength k = Curvature( s );
		//const Length a = f.GetData().a;

		const Vector2D<Area> dxIdk{ -sqrt(s/k) / (2*k), 0_m2 };//1_m2 // };pow<2>(a) };
		//Vector2D<Area> dxIdk{ -1000_m2, 1_m2 };

		return Vector<One>{ Normalize( J * dxIdk ).second }; //dP/dk = -1/2 * sqrt(s/pow<3>(k)) * dP/da + pow<2>(a) * dP/ds
	}

	return Ex<One>;
}

AnglePerLength Clothoid_Imp::Strech( Length s, const spat::Position<Length>& Z )
{
	spat::Vector<One> dPIdkNorm = Direction( s );

	const auto function = [this,&Z,&dPIdkNorm,s]( AnglePerLength k ) -> Length{ 
		assert( k > 0_1Im );
		Create( s/k );
		Position<Length> E;
		Transition( s, E );

		return (Z - E) * dPIdkNorm - m_TargetOffset;
	};

	AnglePerLength k = solve_root( function, Curvature(s) );

	Create( s/k );
	
	return k;
}
///////////////////////////////////////
bool Equals( 
	const Clothoid::Data& A, 
	const Clothoid::Data& B, 
	common::Interval<Length> range, 
	Length epsilon_length, 
	Angle /*epsilon_angle*/ )
{
	ClothoidFunction fA, fB;
	fA.Create( A );
	fB.Create( B );

	return	(fA(range.m_Near) - fB(range.m_Near)).Length() <= epsilon_length &&
			(fA(range.m_Far) - fB(range.m_Far)).Length() <= epsilon_length;
}
///////////////////////////////////////
}
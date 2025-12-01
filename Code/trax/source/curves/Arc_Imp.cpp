//	trax track library
//	AD 2022 
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

#include "Arc_Imp.h"

#include "spat/Matrix.h"

namespace trax{
	using namespace spat;
///////////////////////////////////////
std::unique_ptr<Arc> Arc::Make() noexcept{
	try{
		return std::make_unique<Arc_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
const char*	Arc_Imp::TypeName() const noexcept{
	return "BasicArc";
}

Curve::CurveType Arc_Imp::GetCurveType() const noexcept{
	return Curve::CurveType::Arc;
}

std::unique_ptr<Curve> Arc_Imp::Clone() const{
	return std::make_unique<Arc_Imp>( *this );
}

bool Arc_Imp::Mirror( const spat::VectorBundle<Length,One>& /*mirrorPlane*/ ) noexcept
// Can not be mirrored.
{
	return false;
}

bool Arc_Imp::IsValid() const noexcept{
	return k > 0_1Im;
}

AnglePerLength Arc_Imp::Curvature( Length /*s*/ ) const noexcept{
	return k;
}

AnglePerLength Arc_Imp::Torsion( Length /*s*/ ) const noexcept{
	return 0_1Im;
}

bool Arc_Imp::IsFlat() const noexcept{
	return true;
}

void Arc_Imp::Transition( Length s, spat::Position<Length>& pos ) const noexcept{
	pos = Origin3D<Length> + cos(k*s)/k * Ex<One> + sin(k*s)/k * Ey<One>;
}

void Arc_Imp::Transition( Length s, spat::Vector<One>& tan ) const noexcept{
	tan = -sin(k*s) * Ex<One> + cos(k*s) * Ey<One>;
}

void Arc_Imp::Transition( Length s, spat::VectorBundle<Length,One>& bundle ) const noexcept{
	bundle.P = Origin3D<Length> + cos(k*s)/k * Ex<One> + sin(k*s)/k * Ey<One>;
	bundle.T = -sin(k*s) * Ex<One> + cos(k*s) * Ey<One>;
}

void Arc_Imp::Transition( Length s, spat::VectorBundle2<Length,One>& bundle ) const noexcept{
	bundle.P = Origin3D<Length> + cos(k*s)/k * Ex<One> + sin(k*s)/k * Ey<One>;
	bundle.T = -sin(k*s) * Ex<One> + cos(k*s) * Ey<One>;
	bundle.N = -cos(k*s) * Ex<One> - sin(k*s) * Ey<One>;
}

void Arc_Imp::Transition( Length s, spat::Frame<Length,One>& frame ) const noexcept{
	frame.P = Origin3D<Length> + cos(k*s)/k * Ex<One> + sin(k*s)/k * Ey<One>;
	frame.T = -sin(k*s) * Ex<One> + cos(k*s) * Ey<One>;
	frame.N = -cos(k*s) * Ex<One> - sin(k*s) * Ey<One>;
	frame.B = Ez<One>;
}

std::vector<Length> Arc_Imp::ZeroSet() const noexcept{
	return {};
}

common::Interval<Length> Arc_Imp::Range() const noexcept{
	return { -infinite__length, +infinite__length };
}

spat::Vector<One> Arc_Imp::LocalUp() const noexcept{
	return Ez<One>;
}

spat::Frame<Length, One> Arc_Imp::GetCurveLocalTransformation() const noexcept{
	spat::Frame<Length,One> retval;
	Transition( 0_m, retval );
	return retval;
}

bool Arc_Imp::Equals( 
	const Curve& toCurve, 
	common::Interval<Length> /*range*/, 
	Length epsilon_length, 
	Angle /*epsilon_angle*/ ) const noexcept
{
	if( this == &toCurve )
		return true;

	if( auto pToCurve = dynamic_cast<const Arc*>(&toCurve) )
		return trax::Equals( GetData(), pToCurve->GetData(), epsilon_length );

	return false;
}

spat::Matrix<One,2,3> Arc_Imp::Jacobian( Length s ) const
{
	spat::Matrix<One,2,3> jacobian;
	if( !IsValid() )
		return jacobian;

	jacobian(0,0) = cos(k*s) + (s*k) * sin(k*s);
	jacobian(0,1) = sin(k*s) - (s*k) * cos(k*s);
	jacobian(0,2) = 0;

	spat::Vector<One> tan;
	Transition( s, tan );
	jacobian(1,0) = tan.dx;
	jacobian(1,1) = tan.dy;
	jacobian(1,2) = tan.dz;

	return jacobian;
}

common::Interval<Length> Arc_Imp::Create( const Data& data ){
	if( data.k > 0_1Im ){
		m_Data = data;
		return Range();
	}

	throw std::invalid_argument( "Arc must have curvature > 0_1Im" );
}

const Arc::Data& Arc_Imp::GetData() const noexcept{
	return m_Data;
}

spat::Vector<One> Arc_Imp::Direction( Length s ) const{
	const auto J = Jacobian( s );
	Vector<One> Dir{ -J(0,0), -J(0,1), -J(0,2) }; //dP/dk = -1/pow<2>(k) * dP/dr + ds/dk * dP/ds = -1/pow<2>(k) * dP/dr
	Dir.Normalize();
	return Dir;
}

AnglePerLength Arc_Imp::Strech( Length s, const spat::Position<Length>& Z ){
	const spat::Vector<One> dPIdkNorm = Direction( s );

	const auto function = [this,&Z,&dPIdkNorm,s]( AnglePerLength _k ) -> Length{ 
		assert( _k > 0_1Im );
		Create( Data{ _k } );
		Position<Length> E;
		Transition( s, E );

		return (Z - E) * dPIdkNorm - m_TargetOffset;
	};

	AnglePerLength _k = solve_root( function, Curvature(s) );

	Create( Data{ _k } );
	
	return _k;
}
///////////////////////////////////////

bool Equals( 
	const Arc::Data& A, 
	const Arc::Data& B, 
	Length epsilon_length ) noexcept
{
	return abs(A.radius() - B.radius()) < epsilon_length;
}
///////////////////////////////////////
};
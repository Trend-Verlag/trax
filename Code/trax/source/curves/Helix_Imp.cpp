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

#include "Helix_Imp.h"

#include "spat/Matrix.h"

namespace trax{
	using namespace spat;
///////////////////////////////////////
std::unique_ptr<Helix> Helix::Make() noexcept{
	try{
		return std::make_unique<Helix_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
const char*	Helix_Imp::TypeName() const noexcept{
	return "BasicHelix";
}

Curve::CurveType Helix_Imp::GetCurveType() const noexcept{
	return Curve::CurveType::Helix;
}

std::unique_ptr<Curve> Helix_Imp::Clone() const{
	return std::make_unique<Helix_Imp>( *this );
}

bool Helix_Imp::Mirror( const spat::VectorBundle<Length,One>& /*mirrorPlane*/ ) noexcept
// Can not be mirrored.
{
	return false;
}

bool Helix_Imp::IsValid() const noexcept{
	return k > 0_1Im;
}

AnglePerLength Helix_Imp::Curvature( Length /*s*/ ) const noexcept{
	return k;
}

AnglePerLength Helix_Imp::Torsion( Length /*s*/ ) const noexcept{
	return t;
}

bool Helix_Imp::IsFlat() const noexcept{
	return t == 0_1Im;
}

void Helix_Imp::Transition( Length s, spat::Position<Length>& pos ) const noexcept{
	pos = Origin3D<Length> + a*cos( s/sqrta2b2 ) * Ex<One> + a*sin( s/sqrta2b2 ) * Ey<One> + b/sqrta2b2*s * Ez<One>;
}

void Helix_Imp::Transition( Length s, spat::Vector<One>& tan ) const noexcept{
	tan = -a/sqrta2b2*sin( s/sqrta2b2 ) * Ex<One> + a/sqrta2b2*cos( s/sqrta2b2 ) * Ey<One> + b/sqrta2b2 * Ez<One>;
}

void Helix_Imp::Transition( Length s, spat::VectorBundle<Length,One>& bundle ) const noexcept{
	bundle.P = Origin3D<Length> + a*cos( s/sqrta2b2 ) * Ex<One> + a*sin( s/sqrta2b2 ) * Ey<One> + b/sqrta2b2*s * Ez<One>;
	bundle.T = -a/sqrta2b2*sin( s/sqrta2b2 ) * Ex<One> + a/sqrta2b2*cos( s/sqrta2b2 ) * Ey<One> + b/sqrta2b2 * Ez<One>;
}

void Helix_Imp::Transition( Length s, spat::VectorBundle2<Length,One>& bundle ) const noexcept{
	bundle.P = Origin3D<Length> + a*cos( s/sqrta2b2 ) * Ex<One> + a*sin( s/sqrta2b2 ) * Ey<One> + b/sqrta2b2*s * Ez<One>;
	bundle.T = -a/sqrta2b2*sin( s/sqrta2b2 ) * Ex<One> + a/sqrta2b2*cos( s/sqrta2b2 ) * Ey<One> + b/sqrta2b2 * Ez<One>;
	bundle.N = -cos( s/sqrta2b2 ) * Ex<One> - sin( s/sqrta2b2 ) * Ey<One>;
}

void Helix_Imp::Transition( Length s, spat::Frame<Length,One>& frame ) const noexcept{
	frame.P = Origin3D<Length> + a*cos( s/sqrta2b2 ) * Ex<One> + a*sin( s/sqrta2b2 ) * Ey<One> + b/sqrta2b2*s * Ez<One>;
	frame.T = -a/sqrta2b2*sin( s/sqrta2b2 ) * Ex<One> + a/sqrta2b2*cos( s/sqrta2b2 ) * Ey<One> + b/sqrta2b2 * Ez<One>;
	frame.N = -cos( s/sqrta2b2 ) * Ex<One> - sin( s/sqrta2b2 ) * Ey<One>;
	frame.B = frame.T % frame.N;
}

std::vector<Length> Helix_Imp::ZeroSet() const noexcept{
	return {};
}

common::Interval<Length> Helix_Imp::Range() const noexcept{
	return { -infinite__length, +infinite__length };
}

spat::Vector<One> Helix_Imp::LocalUp() const noexcept{
	return Ez<One>;
}

spat::Frame<Length,One> Helix_Imp::GetCurveLocalTransformation() const noexcept{
	spat::Frame<Length,One> frame;
	Transition( 0_m, frame );
	frame.RotateNor( m_Data.slope() );
	return frame;
}

bool Helix_Imp::Equals( 
	const Curve& toCurve, 
	common::Interval<Length> /*range*/, 
	Length epsilon_length, 
	Angle /*epsilon_angle*/ ) const noexcept
{
	if( this == &toCurve )
		return true;

	if( auto pToCurve = dynamic_cast<const Helix*>(&toCurve) )
		return trax::Equals( GetData(), pToCurve->GetData(), epsilon_length );

	return false;
}

spat::SquareMatrix<One,3> Helix_Imp::Jacobian( Length s ) const{
	spat::SquareMatrix<One,3> jacobian;
	if( !IsValid() )
		return jacobian;

	const One sin_ = sin(s/sqrta2b2);
	const One cos_ = cos(s/sqrta2b2);
	const Area a2b2 = a*a + b*b;

	jacobian(0,0) = cos_ + sin_ * (a / a2b2) * (a / sqrta2b2) * s;
	jacobian(0,1) = sin_ - cos_ * (a / a2b2) * (a / sqrta2b2) * s;
	jacobian(0,2) =              -(a / a2b2) * (b / sqrta2b2) * s;

	jacobian(1,0) =  b * sin_ * (s/a2b2) * (a/sqrta2b2);
	jacobian(1,1) = -b * cos_ * (s/a2b2) * (a/sqrta2b2);
	jacobian(1,2) =  a        * (s/a2b2) * (a/sqrta2b2);

	spat::Vector<One> tan;
	Transition( s, tan );
	jacobian(2,0) = tan.dx;
	jacobian(2,1) = tan.dy;
	jacobian(2,2) = tan.dz;

	return jacobian;
}

common::Interval<Length> Helix_Imp::Create( const Data& data ){
	if( data.k > 0_1Im ){
		m_Data = data;
		a = m_Data.a();
		b = m_Data.b();
		sqrta2b2 = sqrt( a*a + b*b );
		return Range();
	}

	throw std::invalid_argument( "Helix must have curvature > 0_1Im" );
}

const Helix::Data& Helix_Imp::GetData() const noexcept{
	return m_Data;
}

spat::Vector<One> Helix_Imp::Direction( Length s ) const
// s2mh2 = pow<2>(s) - pow<2>(h)
// k2pt2 = pow<2>(k) + pow<2>(t)
// s2 = pow<2>(s)
//
// t(k) = h * k / sqrt(s2mh2), s,h: const
//
// a(k) = k / k2pt2 = (s2mh2) / (k*s2)       = sqrt(s2 - h2) * sqrt(s2mh2)/(k*s2);
// b(k) = t / k2pt2 = h*sqrt(s2mh2) / (k*s2) =      h        * sqrt(s2mh2)/(k*s2);
// s(k) = s = const
// 
// dP/dk = J * dx/dk  with x = { a, b, s };
{
	const auto J = Jacobian( s );
	
	const Vector<One> pdPIpda{ J(0,0), J(0,1), J(0,2) };
	const Vector<One> pdPIpdb{ J(1,0), J(1,1), J(1,2) };
	const Length h = b*s / sqrta2b2;
		
	const Area sh = s * s - h * h;	//sometimes 0 or minus value
	if( sh > 0_m2 )
		return Normalize(-sqrt(sh) * pdPIpda - h * pdPIpdb).second;
	return -Ex<One>;
	//return Normalize(-sqrt(s * s - h * h) * pdPIpda - h * pdPIpdb).second;
}

AnglePerLength Helix_Imp::Strech( Length s, const spat::Position<Length>& Z ){
	const spat::Vector<One> dPIdkNorm = Direction( s );
	const Length h = b*s / sqrta2b2;

	const auto function = [this,&Z,&dPIdkNorm,s,h]( AnglePerLength _k ) -> Length{ 
		assert( _k > 0_1Im );
		Create( Data{ _k, h*_k / sqrt(s*s-h*h) } );
		Position<Length> E;
		Transition( s, E );

		return (Z - E) * dPIdkNorm - m_TargetOffset;
	};

	const AnglePerLength _k = solve_root( function, Curvature(s) );

	Create( Data{ _k, h*_k / sqrt(s*s-h*h) } );

	return k;
}
///////////////////////////////////////
bool Equals( 
	const Helix::Data& A, 
	const Helix::Data& B, 
	Length epsilon_length ) noexcept
{
	return	abs(A.a() - B.a()) < epsilon_length &&
			abs(A.b() - B.b()) < epsilon_length;
}
///////////////////////////////////////
};
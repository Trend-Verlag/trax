//	trax track library
//	AD 2013 
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

#include "HelixP_Imp.h"

namespace trax{
///////////////////////////////////////
bool HelixFunction::Mirror( const spat::VectorBundle<Length,One>& mirrorPlane ){
	spat::Transformation<One> M;
	M.CreateMirror( mirrorPlane );

	m_Data.center.P = M * m_Data.center.P;
	m_Data.center.T = M * m_Data.center.T;
	m_Data.center.N = M * m_Data.center.N;
	m_Data.b = -m_Data.b;

	B = T % N;
	return true;
}

common::Interval<Length> HelixFunction::Create( const DataType& data ){
	if( data.a <= 0_m )
		throw std::invalid_argument( "Helix can not have a radius 0 or smaller." );
	if( !data.center.IsOrthoNormal() )
		throw std::invalid_argument( "Helix has to provide orthonormal center." );

	m_Data = data;
	B = T % N;

	const auto a2b2	= a*a + b*b;
	m_sqrta2b2		= sqrt(a2b2);
	c				= a / a2b2;
	t				= b / a2b2;

	return Range();
}
///////////////////////////////////////
std::unique_ptr<HelixP> HelixP::Make() noexcept{
	try{
		return std::make_unique<HelixP_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
const char*	HelixP_Imp::TypeName() const noexcept{
	return "Helix";
}

Curve::CurveType HelixP_Imp::GetCurveType() const noexcept{
	return Curve::CurveType::HelixP;
}

std::unique_ptr<Curve> HelixP_Imp::Clone() const{
	return std::make_unique<HelixP_Imp>( *this );
}

bool HelixP_Imp::IsFlat() const noexcept{
	return GetData().b == 0_m;
}

spat::Vector<One> HelixP_Imp::LocalUp() const noexcept{
	return f.GetData().center.T % f.GetData().center.N;
}

spat::Frame<Length, One> HelixP_Imp::GetCurveLocalTransformation() const noexcept
{
	spat::Frame<Length,One> frame;
	Transition( 0_m, frame );
	frame.RotateNor( Slope() );
	return frame;
}

VectorBundle2<Length,One> HelixP_Imp::Center() const noexcept{
	return f.GetData().center;
}

Length HelixP_Imp::Radius() const noexcept{
	return f.GetData().a;
}

One HelixP_Imp::Slope() const noexcept{
	return f.GetData().b/f.GetData().a;
}

spat::SquareMatrix<One,3> HelixP_Imp::Jacobian( Length s ) const{
	return f.Jacobian( s );
}

void HelixP_Imp::Create( 
	const Frame<Length,One>& start, 
	AnglePerLength curvature, 
	AnglePerLength torsion )
{
	if( curvature <= 0_1Im )
		throw std::invalid_argument( "Helix can not have a curvature of 0 or smaller!" );

	const auto c2pt2 = curvature*curvature + torsion*torsion;

	DataType data;
	data.a			= curvature / c2pt2;
	data.b			= torsion / c2pt2;

	Frame<Length,One> center = start;
	center.OrthoNormalize();
	center.TransportNor( data.a );
	center.RotateNor( std::atan(data.b/data.a) );
	center.RotateBin( -pi/2 );
	data.center	= center;

	f.Create( data );
}

common::Interval<Length> HelixP_Imp::Create(
	const spat::VectorBundle<Length,One>& start_,
	const spat::Position<Length>& end,
	const spat::Vector<One>& up, 
	Angle e_angle )
{
	if( e_angle < 0 )
		e_angle = 0;

	//Project the situation on a plane, defined by up:
	spat::VectorBundle<Length,One> start{ start_ };
	start.T -= (start.T * up) * up;
	if( start.T.Normalize() == 0 )
		throw std::invalid_argument( "start.T in up direction!" );

	Vector<Length> D = end - start.P;
	const Length h = (D * up);
	D -= h * up;
	const auto d = Normalize(D);
	if( d.first < epsilon__length )
		throw std::invalid_argument( "Start and end positions colocated!" );

	//Angle, 2*alpha is the turning angle of the spiral from s0 to smax
	const One cosAlpha = d.second*start.T;
	const Angle alpha = std::acos(cosAlpha);
	if( alpha <= e_angle/2 )
		throw std::invalid_argument( "start.T pointing to end; infinite a!" );
	if( abs(pi - alpha) <= e_angle/2 )
		throw std::invalid_argument( "start.T pointing from end; infinite a!" );

	const One upDown = common::Sign((start.T % d.second) * up);

	DataType data;
	data.a			= d.first / (2*std::sin(alpha));
	data.b			= upDown * h / (2*alpha);	//b * smax / m_sqrta2b2 = h -> b = h / (2*alpha)
	data.center.T	= upDown * (start.T % up);
	data.center.N	= start.T;
	data.center.P	= start.P - data.a * data.center.T;
	f.Create( data );

	return {0_m,sqrt( 4*alpha*alpha*data.a*data.a + h*h )}; // smax
}

common::Interval<Length> HelixP_Imp::Create( 
	const Position<Length>& start,
	const VectorBundle<Length,One>& end_,
	const spat::Vector<One>& up, 
	Angle e_angle )
{
	if( e_angle < 0 )
		e_angle = 0;

	//Project the situation on a plane, defined by up:
	spat::VectorBundle<Length,One> end{ end_ };
	end.T -= (end.T * up) * up;
	if( end.T.Normalize() == 0 )
		throw std::invalid_argument( "end.T in up direction!" );

	Vector<Length> D = end.P - start;
	const Length h = (D * up);
	D -= h * up;
	const auto d = Normalize(D);
	if( d.first < epsilon__length )
		throw std::invalid_argument( "Start and end positions colocated!" );

	//Angle, 2*alpha is the turning angle of the spiral from s0 to smax
	const One cosAlpha = d.second*end.T;
	const Angle alpha = std::acos(cosAlpha);
	if( alpha <= e_angle/2 )
		throw std::invalid_argument( "start.T pointing to end; infinite a!" );
	if( abs(pi - alpha) <= e_angle/2 )
		throw std::invalid_argument( "start.T pointing from end; infinite a!" );

	Vector<One> B = d.second % end.T;
	B.Normalize();
	Vector<One> T = end.T;
	T.Rotate( B, -2*alpha );

	const One upDown = common::Sign((T % d.second) * up);

	DataType data;
	data.a			= d.first / (2*std::sin(alpha));
	data.b			= upDown * h / (2*alpha);	//b * smax / m_sqrta2b2 = h -> b = h / (2*alpha)
	data.center.T = upDown * (T % up);
	data.center.N = T;
	data.center.P = start - data.a * data.center.T;
	f.Create( data );

	return {0_m,sqrt( 4*alpha*alpha*data.a*data.a + h*h )}; // smax
}

void HelixP_Imp::Create( 
	const VectorBundle2<Length,One>& center, 
	AnglePerLength curvature, 
	AnglePerLength torsion )
{
	if( curvature <= 0_1Im )
		throw std::invalid_argument( "Helix can not have a curvature of 0 or smaller!" );
	
	const auto c2pt2 = curvature*curvature + torsion*torsion;
	
	DataType data;
	data.a = curvature / c2pt2;
	data.b = torsion / c2pt2;
	data.center = center;
	data.center.OrthoNormalize();

	f.Create( data );
}

spat::Vector<One> HelixP_Imp::Direction( Length s ) const
// s2mh2 = pow<2>(s) - pow<2>((h)
// k2pt2 = pow<2>((k) + pow<2>(t)
// s2 = pow<2(s)
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
	const Length h = GetData().b*s/sqrt(GetData().a*GetData().a+GetData().b*GetData().b);
		
	const Area sh = (s * s - h * h);	//sometimes 0 or minus value
	if (sh.Units() > 0)
		return Normalize(-sqrt(sh) * pdPIpda - h * pdPIpdb).second;
	return { -1,0,0 };
	//return Normalize(-sqrt(s * s - h * h) * pdPIpda - h * pdPIpdb).second;
}

AnglePerLength HelixP_Imp::Strech( Length s, const spat::Position<Length>& Z )
{
	const spat::Vector<One> dPIdkNorm = Direction( s );
	spat::Frame<Length,One> curveStart;
	Transition( 0_m, curveStart );
	const Length h = GetData().b*s/sqrt(GetData().a*GetData().a+GetData().b*GetData().b);

	const auto function = [this,&Z,&dPIdkNorm,&curveStart,s,h]( AnglePerLength k ) -> Length{ 
		assert( k > 0_1Im );
		Create( curveStart, k, h*k / sqrt(s*s-h*h) );
		Position<Length> E;
		Transition( s, E );

		return (Z - E) * dPIdkNorm - m_TargetOffset;
	};

	const AnglePerLength k = solve_root( function, Curvature(s) );

	Create( curveStart, k, h*k / sqrt(s*s-h*h) );

	return k;
}
///////////////////////////////////////
bool Equals( 
	const HelixP::Data& A, 
	const HelixP::Data& B, 
	common::Interval<Length> range, 
	Length epsilon_length, 
	Angle epsilon_angle )
{
	HelixFunction fA, fB;
	fA.Create( A );
	fB.Create( B );
	Vector<One> upA{ A.center.T % A.center.N };
	Vector<One> upB{ B.center.T % B.center.N };
	upA.Normalize();
	upB.Normalize();

	return	(fA(range.Near()) - fB(range.Near())).Length() <= epsilon_length &&
			(fA(range.Far()) - fB(range.Far())).Length() <= epsilon_length &&
			upA * upB >= cos(epsilon_angle);
}
///////////////////////////////////////
}
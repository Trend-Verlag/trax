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

#include "ArcP_Imp.h"

namespace trax{
///////////////////////////////////////
bool ArcFunction::Mirror( const spat::VectorBundle<Length,One>& mirrorPlane ){
	spat::Transformation<One> M;
	M.CreateMirror( mirrorPlane );

	m_Data.vb2.P = M * m_Data.vb2.P;
	m_Data.vb2.T = M * m_Data.vb2.T;
	m_Data.vb2.N = M * m_Data.vb2.N;
	N = M * N;
	return true;
}

common::Interval<Length> ArcFunction::Create( const DataType& data ){
	if( !data.vb2.T.IsNormal() )
		throw std::invalid_argument( "Tangent T is not normal." );
	if( data.vb2.N.Length() == 0 )
		throw std::invalid_argument( "Arc can not have a curve radius 0." );

	m_Data = data;
	N = m_Data.vb2.N;
	r = _m(N.Normalize());
	c = 1/r;

	return Range();
}
///////////////////////////////////////
std::unique_ptr<ArcP> ArcP::Make() noexcept{
	try{
		return std::make_unique<ArcP_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
const char*	ArcP_Imp::TypeName() const noexcept{
	return "Arc";
}

Curve::CurveType ArcP_Imp::GetCurveType() const noexcept{
	return Curve::CurveType::ArcP;
}

std::unique_ptr<Curve> ArcP_Imp::Clone() const{
	return std::make_unique<ArcP_Imp>( *this );
}

bool ArcP_Imp::Equals( const Curve& toCurve, Length epsilon_length, Angle epsilon_angle ) const noexcept{
	if( this == &toCurve )
		return true;

	if( auto pArc = dynamic_cast<const ArcP*>(&toCurve) )
		return GetData().vb2.Equals( pArc->GetData().vb2, epsilon_length, epsilon_angle );

	return false;
}

spat::Vector<One> ArcP_Imp::LocalUp() const noexcept{
	return Normalize(f.GetData().vb2.T % f.GetData().vb2.N).second;
}

Length ArcP_Imp::Radius() const noexcept{
	return _m(GetData().vb2.N.Length());
}

spat::Matrix<One,2,3> ArcP_Imp::Jacobian( Length s ) const{
	return f.Jacobian( s );
}

void ArcP_Imp::Create( const VectorBundle2<Length,One>& center ){
	DataType data;
	data.vb2.P = center.P;
	data.vb2.T = center.T;
	data.vb2.T.Normalize();
	data.vb2.N = center.N;
	f.Create(data);
}

common::Interval<Length> ArcP_Imp::Create( 
	const VectorBundle<Length,One>& start_, 
	const Position<Length>& end, 
	Angle e_angle )
{
	if( e_angle < 0 )
		e_angle = 0;

	spat::VectorBundle<Length,One> start{ start_ };
	if( start.T.Normalize() == 0 )
		throw std::invalid_argument( "start.T zero!" );

	const auto D = Normalize(end - start.P);
	if( D.first < epsilon__length )
		throw std::invalid_argument( "Start and end positions colocated!" );

	//Angle, 2*alpha is the turning angle of the arc from s0 to smax
	const One cosAlpha = D.second*start.T;
	const Angle alpha = std::acos(cosAlpha);
	if( alpha <= e_angle/2 )
		throw std::invalid_argument( "start.T pointing to end; infinite radius!" );
	if( abs(pi - alpha) <= e_angle/2 )
		throw std::invalid_argument( "start.T pointing from end; infinite radius!" );

	const Length radius	= D.first / (2*std::sin(alpha));

	Vector<One> B = start.T % D.second;
	B.Normalize();

	DataType data;
	data.vb2.T = start.T;
	data.vb2.N = B % start.T;
	data.vb2.P = start.P + radius * data.vb2.N;
	data.vb2.N *= _m(radius);
	f.Create(data);

	return {0_m,2_1 * alpha * radius}; //smax
}

common::Interval<Length> ArcP_Imp::Create( 
	const Position<Length>& start, 
	const VectorBundle<Length,One>& end_, 
	Angle e_angle )
{
	if( e_angle < 0 )
		e_angle = 0;

	spat::VectorBundle<Length,One> end{ end_ };
	if( end.T.Normalize() == 0 )
		throw std::invalid_argument( "end.T zero!" );

	const auto D = Normalize(end.P - start);
	if( D.first < epsilon__length )
		throw std::invalid_argument( "Start and end positions collocated!" );

	//Angle, 2*alpha is the turning angle of the arc from s0 to smax
	const One cosAlpha = D.second*end.T;
	const Angle alpha = std::acos(cosAlpha);
	if( alpha <= e_angle/2 )
		throw std::invalid_argument( "start.T pointing to end; infinite radius!" );
	if( abs(pi - alpha) <= e_angle/2 )
		throw std::invalid_argument( "start.T pointing from end; infinite radius!" );

	const Length radius	= D.first / (2*std::sin(alpha));

	Vector<One> B = D.second % end.T;
	B.Normalize();

	DataType data;
	data.vb2.T = end.T;
	data.vb2.T.Rotate( B, -2*alpha );
	data.vb2.N = B % data.vb2.T;
	data.vb2.P = start + radius * data.vb2.N;
	data.vb2.N *= _m(radius);
	f.Create(data);

	return {0_m,2_1 * alpha * radius}; //smax
}

void ArcP_Imp::Create( const Position<Length>& start, const Vector<One>& tan, const Vector<One>& nor, AnglePerLength curvature ){
	if( curvature <= 0_1Im )
		throw std::invalid_argument( "Invalid curvature." );

	Vector<One> Tan = tan;
	if( Tan.Normalize() == 0 )
		throw std::invalid_argument( "direction can't be of zero length" );

	Vector<One> Nor = nor;
	Nor -= (Tan*Nor)*Tan;
	if( Nor.Normalize() == 0 )
		throw std::invalid_argument( "normal direction can't be parallel with tangent" );

	const Length radius = 1 / curvature;
	
	DataType data;
	data.vb2.T = Tan;
	data.vb2.N = Nor;
	data.vb2.P = start + radius * data.vb2.N;
	data.vb2.N *= _m(radius);
	f.Create(data);
}

void ArcP_Imp::Create( const VectorBundle2<Length,One>& start, AnglePerLength curvature ){
	Create( start.P, start.T, start.N, curvature );
}

spat::Vector<One> ArcP_Imp::Direction( Length s ) const{
	const auto J = Jacobian( s );
	Vector<One> Dir{ -J(0,0), -J(0,1), -J(0,2) }; //dP/dk = -1/pow<2>(k) * dP/dr
	Dir.Normalize();
	return Dir;
}

AnglePerLength ArcP_Imp::Strech( Length s, const spat::Position<Length>& Z )
{
	const spat::Vector<One> dPIdkNorm = Direction( s );
	spat::VectorBundle2<Length,One> curveStart;
	Transition( 0_m, curveStart );

	const auto function = [this,&Z,&dPIdkNorm,&curveStart,s]( AnglePerLength k ) -> Length{ 
		assert( k > 0_1Im );
		Create( curveStart, k );
		Position<Length> E;
		Transition( s, E );

		return (Z - E) * dPIdkNorm - m_TargetOffset;
	};

	const AnglePerLength k = solve_root( function, Curvature(s) );

	Create( curveStart, k );
	
	return k;
}
///////////////////////////////////////
bool Equals( 
	const ArcP::Data& A, 
	const ArcP::Data& B, 
	common::Interval<Length> range,
	Length epsilon_length, 
	Angle epsilon_angle )
{
	ArcFunction fA, fB;
	fA.Create( A );
	fB.Create( B );
	Vector<One> upA{ A.vb2.T % A.vb2.N };
	Vector<One> upB{ B.vb2.T % B.vb2.N };
	upA.Normalize();
	upB.Normalize();

	return	(fA(range.Near()) - fB(range.Near())).Length() <= epsilon_length &&
			(fA(range.Far()) - fB(range.Far())).Length() <= epsilon_length &&
			upA * upB >= cos(epsilon_angle) &&
			Length{std::abs(A.vb2.N.Length() - B.vb2.N.Length())} <= epsilon_length;
}
///////////////////////////////////////
}
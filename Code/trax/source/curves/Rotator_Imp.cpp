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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#include "Rotator_Imp.h"

namespace trax{
///////////////////////////////////////
const char*	Rotator_Imp::TypeName() const noexcept{
	return "Rotator";
}

Curve::CurveType Rotator_Imp::GetCurveType() const noexcept{
	return Curve::CurveType::Rotator;
}

std::unique_ptr<Curve> Rotator_Imp::Clone() const{
	return std::make_unique<Rotator_Imp>( *this );
}

AnglePerLength Rotator_Imp::Curvature( Length s ) const noexcept{
	return f.Curvature(s);
}

bool Rotator_Imp::IsFlat() const noexcept{
	return GetData().a == 0_1Im || GetData().b == 0_1Im;
}

spat::Vector<One> Rotator_Imp::LocalUp() const noexcept{
	return Ez<One>;
}

spat::Frame<Length, One> Rotator_Imp::GetCurveLocalTransformation() const noexcept{
	return Identity<Length,One>;
}

SquareMatrix<Real,3> Rotator_Imp::Jacobian( Length s ) const{
	return f.Jacobian( s );
}

common::Interval<Length> Rotator_Imp::Create( const Data& data ){
	if( data.a0 != 0_rad || data.b0 != 0_rad )
		throw std::invalid_argument{ "The simple rotator curve has no offset; create a rotator with offset!" };

	return Base::Create( data );
}
///////////////////////////////////////
bool Equals( 
	const Rotator::Data& A, 
	const Rotator::Data& B, 
	common::Interval<Length> range, 
	Length epsilon_length, 
	Angle epsilon_angle ) noexcept
{
	RotatorFunction fA, fB;
	fA.Create( A );
	fB.Create( B );

	return	abs(A.a - B.a) * range.Length() <= epsilon_angle &&
			abs(A.b - B.b) * range.Length() <= epsilon_angle &&
			abs(A.a0 - B.a0) <= epsilon_angle &&
			abs(A.b0 - B.b0) <= epsilon_angle && 
			(fA(range.m_Near) - fB(range.m_Near)).Length() <= epsilon_length &&
			(fA(range.m_Far) - fB(range.m_Far)).Length() <= epsilon_length;
}
///////////////////////////////////////
std::unique_ptr<Rotator> Rotator::Make( CurveType type ) noexcept{
	try{
		switch( type ){
		case CurveType::Rotator: 
			return std::make_unique<Rotator_Imp>();
		case CurveType::RotatorWithOffset: 
			return std::make_unique<RotatorWithOffset_Imp>();
		default:
			return nullptr;
		}
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
const char*	RotatorWithOffset_Imp::TypeName() const noexcept{
	return "Rotator";
}

Curve::CurveType RotatorWithOffset_Imp::GetCurveType() const noexcept{
	return Curve::CurveType::RotatorWithOffset;
}

bool RotatorWithOffset_Imp::IsFlat() const noexcept{
	return GetData().a == 0_1Im || GetData().b == 0_1Im;
}

spat::Vector<One> RotatorWithOffset_Imp::LocalUp() const noexcept{
	return Ez<One>;
}

spat::Frame<Length, One> RotatorWithOffset_Imp::GetCurveLocalTransformation() const noexcept{
	return Identity<Length,One>;
}

SquareMatrix<Real,3> RotatorWithOffset_Imp::Jacobian( Length /*s*/ ) const noexcept
{
	assert( !"Not implemented yet!" );
	return SquareMatrix<Real,3>();
}

std::unique_ptr<Curve> RotatorWithOffset_Imp::Clone() const{
	return std::make_unique<RotatorWithOffset_Imp>( *this );
}
///////////////////////////////////////
std::unique_ptr<RotatorChain> RotatorChain::Make() noexcept{
	try{
		return std::make_unique<RotatorChain_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
const char*	RotatorChain_Imp::TypeName() const noexcept{
	return "RotatorChain";
}

Curve::CurveType RotatorChain_Imp::GetCurveType() const noexcept{
	return Curve::CurveType::RotatorChain;
}

std::unique_ptr<Curve> RotatorChain_Imp::Clone() const{
	return std::make_unique<RotatorChain_Imp>( *this );
}

bool RotatorChain_Imp::IsFlat() const noexcept{
	Angle totalA = 0, totalB = 0;
	for( const auto& tuple : GetData() ){
		totalA += std::get<0>(tuple);
		totalB += std::get<1>(tuple);
	}

	return totalA == 0 || totalB == 0;
}

std::vector<Length> RotatorChain_Imp::ZeroSet() const{
	if( !IsValid() )
		throw std::runtime_error( "This curve has to be created prior to receiving the zero set." );

	if( GetData().size() <= 1 )
		return {};

	Length length;
	std::vector<Length> retval;

	for( std::size_t i = 0; i < GetData().size() - 1; ++i ){
		length += std::get<2>(GetData()[i]);
		VectorBundle2<Length,One> B1, B2;
		Transition( length - epsilon__length, B1 );
		Transition( length + epsilon__length, B2 );

		if( B1.N*B2.N <= -cos(epsilon__angle) )
			// we collect only the nearly perfect antiparallel cases. In general
			// the jump of the normal would not qualify for a zero flip.
			retval.push_back(length);
	}

	return retval;
}

spat::Vector<One> RotatorChain_Imp::LocalUp() const noexcept{
	return Ez<One>;
}

spat::Frame<Length, One> RotatorChain_Imp::GetCurveLocalTransformation() const noexcept{
	return Identity<Length,One>;
}

common::Interval<Length> RotatorChain_Imp::Create( const spat::Vector<Length>& advance )
// w/2     = r*sin(a)
// r - d/2 = r*cos(a)
// r - h/2 = r*cos(a)
// 1       = sin(a)*sin(a) + cos(a)*cos(a)
//
// --> rH = (w*w + d*d) / 4d
//     a  = asin( w / (2*rH) )
//     rV = (w*w + h*h) / 4h
//     b  = asin( w / (2*rV) )
//
//     l1 = a*rH               | h == 0
//          b*rV               | d == 0
//          b*h / (1 - cos(b)) | h != 0 && d != 0
//
//          l               l                                 l              
//    h/2 = S dCz/ds * ds = S sin(b/l*s) ds = -l*cos(b/l*s)/b | = -l*cos(b/l*l)/b + l*cos(b/l*0)/b
//          0               0                                 0
// 
//        =  l/b * (1 - cos(b))
//
// -->  l = b*h / 2(1 - cos(b))
//
// It is dependend from the dimension, the length l is calculated for, which
// advnace vaue will be strictly locked. Here we use advance.dz, due to best
// computability. The other two would need numerical integration. The best solution
// probably would be to have only advance.dx variable and lock the other two, by
// finding a length (with variable w) for which both integrals are eaqual, but that
// would be computational demanding.
{
	Data data;

	const Length w = advance.dx;
	const Length d = advance.dy;
	const Length h = advance.dz;

	const Length rH = (w*w + d*d) / (4*d);
	const Angle a   = asin( w / (2*rH) );
	const Length rV = (w*w + h*h) / (4*h);
	const Angle b   = asin( w / (2*rV) );

	Length l;
	if( h == 0_m )
		l = a*rH;
	else if( d == 0_m )
		l = b*rV;
	else
		l = b*h / (2*(1 - cos( b )));
	
	data.push_back( std::make_tuple( a, b, l ) );
	data.push_back( std::make_tuple( -a, -b, l ) );

	return Create( data );
}

common::Interval<Length> RotatorChain_Imp::Create( const Data& data ){
	if( data.empty() )
		throw std::invalid_argument( "No curves in the chain!" );

	for( const auto& tuple : data ){
		if( std::get<2>(tuple) <= 0_m )
			throw std::invalid_argument( "A curve in the chain has zero or less then zero length!" );
	}

	return Base::Create( data );
}
///////////////////////////////////////
bool Equals( 
	const RotatorChain::Data& A, 
	const RotatorChain::Data& B, 
	common::Interval<Length> /*range*/, 
	Length epsilon_length, 
	Angle epsilon_angle ) noexcept
{
	if( A.size() != B.size() )
		return false;

	std::tuple<Angle,Angle,Length> sumA, sumB;
	for( std::size_t i = 0; i < A.size(); ++i ){
		if( abs(std::get<0>(B[i]) - std::get<0>(A[i])) > epsilon_angle ||
			abs(std::get<1>(B[i]) - std::get<1>(A[i])) > epsilon_angle ||
			abs(std::get<2>(B[i]) - std::get<2>(A[i])) > epsilon_length )
			return false;

		std::get<0>(sumA) += std::get<0>(A[i]);
		std::get<1>(sumA) += std::get<1>(A[i]);
		std::get<2>(sumA) += std::get<2>(A[i]);

		std::get<0>(sumB) += std::get<0>(B[i]);
		std::get<1>(sumB) += std::get<1>(B[i]);
		std::get<2>(sumB) += std::get<2>(B[i]);
	}

	return	abs(std::get<0>(sumB) - std::get<0>(sumA)) <= epsilon_angle &&
			abs(std::get<1>(sumB) - std::get<1>(sumA)) <= epsilon_angle &&
			abs(std::get<2>(sumB) - std::get<2>(sumA)) <= epsilon_length;
}
///////////////////////////////////////
}
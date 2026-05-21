//	trax track library
//	AD 2014 
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

#include "trax/Curve_Imp.h"

#include "common/Helpers.h"
#include "spat/Position.h"

#include <boost/math/quadrature/gauss_kronrod.hpp>

namespace trax{
///////////////////////////////////////
bool Equals( 
	const std::pair<std::shared_ptr<const Curve>,common::Interval<Length>>& A, 
	const std::pair<std::shared_ptr<const Curve>,common::Interval<Length>>& B, 
	Length epsilon_length, 
	Angle epsilon_angle )
{
	return	A.second.Equals( B.second, epsilon_length ) &&
			A.first->Equals( *B.first, A.second, epsilon_length, epsilon_angle );
}

bool EqualsByCenters( 
	const Curve& A, 
	const Curve& B, 
	common::Interval<Length> range, 
	Length epsilon_length ) noexcept
{
	if( abs(range.Length()) <= 10*epsilon_length )
		return true;

	spat::Position<Length> posA, posB;
	A.Transition( range.Center(), posA );
	B.Transition( range.Center(), posB );
	if( !posA.Equals( posB, epsilon_length ) )
		return false;

	if( !EqualsByCenters( A, B, {range.Near(), range.Center()}, epsilon_length ) )
		return false;

	if( !EqualsByCenters( A, B, { range.Center(), range.Far() }, epsilon_length ) )
		return false;

	return true;
}

bool Equals( 
	const Curve& A, 
	const Curve& B, 
	common::Interval<Length> range, 
	Length epsilon_length ) noexcept
{
	if( !A.IsValid() || !B.IsValid() )
		return false;

	if( &A == &B )
		return true;

	return EqualsByCenters( A, B, range, epsilon_length );
}

const char* TypeToName( Curve::CurveType ct ){
	switch( ct ){		
	case Curve::CurveType::Line:
		return "BasicLine";
	case Curve::CurveType::Arc:
		return "BasicArc";
	case Curve::CurveType::Helix:
		return "BasicHelix";
	case Curve::CurveType::LineP:
		return "Line";
	case Curve::CurveType::ArcP:
		return "Arc";
	case Curve::CurveType::HelixP:
		return "Helix";
	case Curve::CurveType::Clothoid:
		return "Clothoid";
	case Curve::CurveType::Cubic:
		return "Cubic";
	case Curve::CurveType::Spline:
		return "Spline";
	case Curve::CurveType::Rotator:
		return "Rotator";
	case Curve::CurveType::RotatorWithOffset:
		return "RotatorWithOffset";
	case Curve::CurveType::RotatorChain:
		return "RotatorChain";
	case Curve::CurveType::PolygonalChain:
		return "PolygonalChain";
	case Curve::CurveType::SampledCurve:
		return "SampledCurve";
	case Curve::CurveType::Parallel:
		return "Parallel";
	case Curve::CurveType::EEPCurve:
		return "EEPCurve";
	case Curve::CurveType::EEPResidual:
		return "EEPResidual";
	case Curve::CurveType::EEPAlternative:
		return "EEPAlternative";
	case Curve::CurveType::Unknown:
		return "Unknown";
	case Curve::CurveType::UserDefined:
		return "UserDefined";
	default:
		return "none";
	}
}

Curve::CurveType CurveNameToType( const char* name ) noexcept{
	if( std::strcmp(name, "BasicLine") == 0 )
		return Curve::CurveType::Line;
	if( std::strcmp(name, "BasicArc") == 0 )
		return Curve::CurveType::Arc;
	if( std::strcmp(name, "BasicHelix") == 0 )
		return Curve::CurveType::Helix;
	if( std::strcmp(name, "Line") == 0 )
		return Curve::CurveType::LineP;
	if( std::strcmp(name, "Arc") == 0 )
		return Curve::CurveType::ArcP;
	if( std::strcmp(name, "Helix") == 0 )
		return Curve::CurveType::HelixP;
	if( std::strcmp(name, "Clothoid") == 0 )
		return Curve::CurveType::Clothoid;
	if( std::strcmp(name, "Cubic") == 0 )
		return Curve::CurveType::Cubic;
	if( std::strcmp(name, "Spline") == 0 )
		return Curve::CurveType::Spline;
	if( std::strcmp(name, "Rotator") == 0 )
		return Curve::CurveType::Rotator;
	if( std::strcmp(name, "RotatorWithOffset") == 0 )
		return Curve::CurveType::RotatorWithOffset;
	if( std::strcmp(name, "RotatorChain") == 0 )
		return Curve::CurveType::RotatorChain;
	if( std::strcmp(name, "PolygonalChain") == 0 )
		return Curve::CurveType::PolygonalChain;
	if( std::strcmp(name, "SampledCurve") == 0 )
		return Curve::CurveType::SampledCurve;
	if( std::strcmp(name, "Parallel") == 0 )
		return Curve::CurveType::Parallel;
	if( std::strcmp(name, "EEPCurve") == 0 )
		return Curve::CurveType::EEPCurve;
	if( std::strcmp(name, "EEPResidual") == 0 )
		return Curve::CurveType::EEPResidual;
	if( std::strcmp(name, "EEPAlternative") == 0 )
		return Curve::CurveType::EEPAlternative;
	if( std::strcmp(name, "Unknown") == 0 )
		return Curve::CurveType::Unknown;
	if( std::strcmp(name, "UserDefined") == 0 )
		return Curve::CurveType::UserDefined;

	return Curve::CurveType::none;
}

bool CurveSample::Equals( const CurveSample& sample, Length epsilon_length, One epsilon_angle ) const noexcept
{
	return	common::Equals( s, sample.s, epsilon_length ) &&
			common::Equals( k, sample.k, epsilon_angle / plausible_maximum_length ) &&
			common::Equals( t, sample.t, epsilon_angle / plausible_maximum_length ) &&
			F.Equals( sample.F, epsilon_length, epsilon_angle );
}

Angle TotalAngle( const Curve& curve, common::Interval<Length> range, Angle accuracy )
{
	if( range.Length() == 0_m )
		return 0;
	if( !curve.IsValid() )
		throw std::invalid_argument( "Curve is not valid !" );
	if( accuracy <= 0 )
		throw std::invalid_argument( "Accuracy must be > 0 !" );

	range.Normalize();

	return _rad(boost::math::quadrature::gauss_kronrod<Real, 15>::integrate(
		[&curve]( Real s ){ return _1Im(curve.Curvature(_m(s))); },
		_m(range.Near()), 
		_m(range.Far()), 
		5, 
		accuracy ) );
}

Angle TotalAngle( std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> curve, Angle accuracy ){
	if( !curve.first )
		throw std::invalid_argument( "Curve is not valid!" );

	return TotalAngle( *curve.first, curve.second, accuracy );
}
///////////////////////////////////////
const CurvatureStrecher* CurvatureStrecher::InterfaceFrom( const Curve& curve ) noexcept{
	return dynamic_cast<const CurvatureStrecher*>(&curve);
}

CurvatureStrecher* CurvatureStrecher::InterfaceFrom( Curve& curve ) noexcept{
	return dynamic_cast<CurvatureStrecher*>(&curve);
}
///////////////////////////////////////
}

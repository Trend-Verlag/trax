// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


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
	if( range.Length() <= 10*epsilon_length )
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

Curve::CurveType CurveNameToType( const std::string& name ) noexcept{
	if( name.compare( "BasicLine" ) == 0 )
		return Curve::CurveType::Line;
	if( name.compare( "BasicArc" ) == 0 )
		return Curve::CurveType::Arc;
	if( name.compare( "BasicHelix" ) == 0 )
		return Curve::CurveType::Helix;
	if( name.compare( "Line" ) == 0 )
		return Curve::CurveType::LineP;
	if( name.compare( "Arc" ) == 0 )
		return Curve::CurveType::ArcP;
	if( name.compare( "Helix" ) == 0 )
		return Curve::CurveType::HelixP;
	if( name.compare( "Clothoid" ) == 0 )
		return Curve::CurveType::Clothoid;
	if( name.compare( "Cubic" ) == 0 )
		return Curve::CurveType::Cubic;
	if( name.compare( "Spline" ) == 0 )
		return Curve::CurveType::Spline;
	if( name.compare( "Rotator" ) == 0 )
		return Curve::CurveType::Rotator;
	if( name.compare( "RotatorWithOffset" ) == 0 )
		return Curve::CurveType::RotatorWithOffset;
	if( name.compare( "RotatorChain" ) == 0 )
		return Curve::CurveType::RotatorChain;
	if( name.compare( "PolygonalChain" ) == 0 )
		return Curve::CurveType::PolygonalChain;
	if( name.compare( "SampledCurve" ) == 0 )
		return Curve::CurveType::SampledCurve;
	if( name.compare( "Parallel" ) == 0 )
		return Curve::CurveType::Parallel;
	if( name.compare( "EEPCurve" ) == 0 )
		return Curve::CurveType::EEPCurve;
	if( name.compare( "EEPResidual" ) == 0 )
		return Curve::CurveType::EEPResidual;
	if( name.compare( "EEPAlternative" ) == 0 )
		return Curve::CurveType::EEPAlternative;
	if( name.compare( "Unknown" ) == 0 )
		return Curve::CurveType::Unknown;
	if( name.compare( "UserDefined" ) == 0 )
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
		[&curve,&range]( Real s ){ return _1Im(curve.Curvature(_m(s))); },
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

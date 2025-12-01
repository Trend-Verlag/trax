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

#include "Cubic_Imp.h"

#include "common/NarrowCast.h"
#include "common/support/CommonSupportStream.h"

#include <algorithm>
#include <random>

#if BOOST_VERSION >= 108500
#	include <boost/math/optimization/jso.hpp>
#else
#	define STRING2(x) #x
#	define STRING(x) STRING2(x)
#	pragma message( "Too low a version number of Boost. 108500 needed for jso, but used " STRING(BOOST_VERSION) )
#endif // BOOST_VERSION >= 108500


namespace trax{
///////////////////////////////////////
bool CubicFunction::IsFlat() const noexcept{
	const Vector<One> B = Normalize(GetData().b).second;
	const Vector<One> C = Normalize(GetData().c).second;
	const Vector<One> D = Normalize(GetData().d).second;
	return Parallel( B % C, B % D, One{ epsilon } );
}

Vector<One> CubicFunction::LocalUp() const{
	const Vector<One> B = Normalize(GetData().b).second;
	const Vector<One> C = Normalize(GetData().c).second;
	const Vector<One> D = Normalize(GetData().d).second;
	const Vector<One> BxC = B % C;
	if( Parallel( BxC, B % D, One{ epsilon } ) && !BxC.Equals( Null<One>, epsilon ) )
		return Normalize(BxC).second;

	throw std::runtime_error( "This cubic does not maintain any notion of 'up'!" );
}

bool CubicFunction::Mirror(const VectorBundle<Length,One>& mirrorPlane){
	Transformation<One> M;
	M.CreateMirror( mirrorPlane );

	m_Data.a = M * m_Data.a;
	m_Data.b = M * m_Data.b;
	m_Data.c = M * m_Data.c;
	m_Data.d = M * m_Data.d;

	_2c = M * _2c;
	_3d = M * _3d;
	_6d = M * _6d;
	return true;
}

void CubicFunction::Shorten( Interval<One> toRange )
// u : [0,1] -> [u0,u1]
// v : [0,1] 
// u = v*(u1-u0) + u0
// Could also be used to 'lengthen' the curve.
{
	const Position<Length> a = m_Data.a;
	const Vector<Length> b = m_Data.b;
	const Vector<Length> c = m_Data.c;
	const Vector<Length> d = m_Data.d;

	const One u0 = toRange.Near();
	const One u1 = toRange.Far();

	DataType data;

	data.a = a + u0*b + (u0*u0)*c + (u0*u0*u0)*d;
	data.b = pow<1>(u1-u0) * (b + (2_1*u0)*c + (3_1*u0*u0)*d);
	data.c = pow<2>(u1-u0) * (c + (3_1*u0)*d);
	data.d = pow<3>(u1-u0) * d;

	Create( data );
}

void CubicFunction::Create( const Position<Length>& a_, const Vector<Length>& b_, const Vector<Length>& c_, const Vector<Length>& d_ ){
	if( b_ == Null<Length> && c_ == Null<Length> && d_ == Null<Length> )
		throw std::invalid_argument( "Cubic can not have b, c and d vectors all null." );
	if( a_ != a_ || b_ != b_ || c_ != c_ || d_ != d_ )
		throw std::invalid_argument( "Cubic can not get initialized with NaN values." );

	m_Data.a = a_;
	m_Data.b = b_;
	m_Data.c = c_;
	m_Data.d = d_;

	_2c = 2_1*m_Data.c;
	_3d = 3_1*m_Data.d;
	_6d = 6_1*m_Data.d;

	m_bCreated = true;
}

void CubicFunction::Create( const VectorBundle<Length>& start, const VectorBundle<Length>& end ){
	Create( 
		start.P, 
		start.T, 
		3_1*(end.P - start.P) - end.T - 2_1*start.T, 
		2_1*(start.P - end.P) + start.T + end.T );
}

void CubicFunction::CreateBezier(
	const spat::Position<Length>& P0,
	const spat::Position<Length>& P1,
	const spat::Position<Length>& P2,
	const spat::Position<Length>& P3 )
{
	Create( 
		{ P0, 3_1*(P1-P0) },
		{ P3, 3_1*(P3-P2) } );
}

void CubicFunction::Create( const DataType& data ){
	Create( data.a, data.b, data.c, data.d );
}
///////////////////////////////////////
std::unique_ptr<Cubic> Cubic::Make() noexcept{
	try{
		return std::make_unique<Cubic_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
const char* Cubic_Imp::TypeName() const noexcept{
	return "Cubic";
}

Curve::CurveType Cubic_Imp::GetCurveType() const noexcept{
	return Curve::CurveType::Cubic;
}

inline bool operator==( const Cubic::Data& dataA, const Cubic::Data& dataB ) noexcept{
	return	dataA.a == dataB.a &&
			dataA.b == dataB.b &&
			dataA.c == dataB.c &&
			dataA.d == dataB.d;
}

std::unique_ptr<Curve> Cubic_Imp::Clone() const{
	return std::make_unique<Cubic_Imp>( *this  );
}

bool Cubic_Imp::IsFlat() const noexcept{
	return f.IsFlat();
}

Vector<One> Cubic_Imp::LocalUp() const{
	if( ZeroSet().empty() )
		return f.LocalUp();

	throw std::runtime_error( "This cubic does not maintain any notion of 'up'!" );
}

Vector<Length> Cubic_Imp::CubicOvershootAt( Length s ) const noexcept{
	return f.D1( t(s) );
}

Interval<Length> Cubic_Imp::Create( 
	const VectorBundle<Length,Length>& start, 
	const VectorBundle<Length,Length>& end )
{
	f.Create( start, end );
	return Sample();
}

Interval<Length> Cubic_Imp::CreateBezier(
	const spat::Position<Length>& P0,
	const spat::Position<Length>& P1,
	const spat::Position<Length>& P2,
	const spat::Position<Length>& P3 )
{
	f.CreateBezier( P0, P1, P2, P3 );
	return Sample();
}

std::pair<common::Interval<Length>,Length> Cubic_Imp::Create( 
	const Curve& originalCurve, 
	common::Interval<Length> range, 
	[[maybe_unused]] Length maxDeviation )
{
	if( !originalCurve.IsValid() )
		throw std::invalid_argument( "Cubic_Imp::Create(): no valid curve to create from." );

	range.Intersection( originalCurve.Range() );
	range.Normalize();
	if( range.Length() < epsilon__length )
		throw std::invalid_argument( "Cubic_Imp::Create(): zero length curve." );

	VectorBundle<Length,One> startOriginal, endOriginal;
	Length overShootAtStart, overShootAtEnd;

	originalCurve.Transition( range.Near(), startOriginal );
	originalCurve.Transition( range.Far(), endOriginal );
	overShootAtStart = overShootAtEnd = (startOriginal.P - endOriginal.P).Length();

	if( overShootAtStart < epsilon__length )
		throw std::runtime_error( "Cubic_Imp::Create(): can not approximate cuve with a Cubic since start and end are collocated." );

	// Approximate known curves:
	if( originalCurve.GetCurveType() == Curve::CurveType::Line ||
		originalCurve.GetCurveType() == Curve::CurveType::LineP ){
		return std::make_pair( Create( { startOriginal.P, overShootAtStart * startOriginal.T }, { endOriginal.P, overShootAtEnd * endOriginal.T } ), 0_m );
	}
	else if( originalCurve.GetCurveType() == Curve::CurveType::Cubic ){
		return std::make_pair( Create( dynamic_cast<const Cubic&>(originalCurve).GetData() ), 0_m );
	}
	else if( auto pSpline = dynamic_cast<const Spline*>(&originalCurve); pSpline && pSpline->GetData().size() == 1 )
		return std::make_pair( Create( pSpline->GetData().front() ), 0_m );

#if BOOST_VERSION >= 108500
	using namespace boost::math::optimization;

	auto costFunction = [&]( const std::vector<double>& overshoots ) -> double
	{
		Cubic_Imp cubic;
		cubic.Create( { startOriginal.P, _m(static_cast<Real>(overshoots[0])) * startOriginal.T }, { endOriginal.P, _m(static_cast<Real>(overshoots[1])) * endOriginal.T } );
		return static_cast<double>(_m(cubic.MaxDistance( originalCurve, range )));	
	};

	// Algorithm jSO:
	auto de_params = jso_parameters <std::vector<double>>{};
	constexpr const size_t dimension = 2;
	de_params.lower_bounds.resize( dimension, _m(std::min(1_m,overShootAtStart-epsilon__length)) );
	de_params.upper_bounds.resize( dimension, _m(std::max(1000_m,overShootAtStart+epsilon__length)) );
	std::vector<double> initial_guess = { _m(overShootAtStart), _m(overShootAtEnd) };
	de_params.initial_guess = &initial_guess;
	de_params.max_function_evaluations = 1000; // its smaller than the default to prevent too long execution times
	std::random_device rd;
	std::mt19937_64 rng(rd());
	auto local_minima = jso( costFunction, de_params, rng, _m(maxDeviation/100) );

	return std::make_pair( 
		Create( { startOriginal.P, _m(static_cast<Real>(local_minima[0])) * startOriginal.T }, 
				{ endOriginal.P, _m(static_cast<Real>(local_minima[1])) * endOriginal.T } ),
		_m(static_cast<Real>(costFunction(local_minima))) );
#else
	throw std::runtime_error( "Cubic_Imp::Create(): Boost version too low for jso." );
#endif // BOOST_VERSION >= 108500
}

Interval<Length> Cubic_Imp::Shorten( Interval<Length> toRange ){
	toRange.Intersection( Range() );
	f.Shorten( {t(toRange.Near()), t(toRange.Far())} );
	return Sample();
}

Length Cubic_Imp::MaxDistance( const Curve& originalCurve, common::Interval<Length> range ) const
{
	Length sMax = 0_m;
	for( Length s = range.Near(); s < range.Far(); s += 100*epsilon__length ){
		Position<Length> Po, Pc;
		Transition( Range().Length() / range.Length() * (s - range.Near()), Pc );
		originalCurve.Transition( s, Po );
		sMax = std::max( sMax, (Pc - Po).Length() );
	}
	return sMax;
}
///////////////////////////////////////
bool Equals( 
	const Cubic::Data& A, 
	const Cubic::Data& B, 
	Interval<Length> /*range*/, 
	Length epsilon_length, 
	Angle /*epsilon_angle*/ ) noexcept
{
	CubicFunction fA, fB;
	fA.Create( A );
	fB.Create( B );
	
	return	(fA(0) - fB(0)).Length() <= epsilon_length &&
			(fA(1) - fB(1)).Length() <= epsilon_length &&
			(fA.D1(0) - fB.D1(0)).Length() <= epsilon_length &&
			(fA.D1(1) - fB.D1(1)).Length() <= epsilon_length;
}
///////////////////////////////////////
bool SplineFunction::IsFlat() const noexcept{
	if( !IsValid() )
		return false;

	try{
		const Vector<One> Base = m_CubicFunctions[0].LocalUp();
		for( std::size_t idx = 1; idx < m_CubicFunctions.size(); ++idx )
			if( !Parallel( Base, m_CubicFunctions[idx].LocalUp(), One{ epsilon } ) )
				return false;

		return true;
	}
	catch( std::runtime_error& ){
		return false;
	}
}

Vector<One> SplineFunction::LocalUp() const{
	if( !IsValid() )
		throw std::runtime_error( "Curve not properly created!" );

	const Vector<One> Base = m_CubicFunctions[0].LocalUp();
	for( std::size_t idx = 1; idx < m_CubicFunctions.size(); ++idx )
		if( !OrthoParallel( Base, m_CubicFunctions[idx].LocalUp(), One{ epsilon } ) )
			throw std::runtime_error( "This spline does not maintain any notion of 'up'!" );

	return Base;	
}

bool SplineFunction::Mirror( const VectorBundle<Length, One>& mirrorPlane ){
	for( auto& cubic : m_CubicFunctions )
		cubic.Mirror( mirrorPlane );

	for( std::size_t idx = 0; idx < m_CubicFunctions.size(); ++idx )
		m_Data[idx] = m_CubicFunctions[idx].GetData();

	return true;
}

Interval<One> SplineFunction::Shorten( Interval<One> toRange ){
	if( !IsValid() )
		throw std::runtime_error( "Spline function has to be created before it can get shortened!" );

	toRange.Normalize();

	Interval<One> t{ toRange };
	int iStart = narrow_cast<int>(CalcCubicIdx( t.m_Near ));
	if( common::Equals( t.m_Near, 1_1, 10_1*epsilon ) ){
		++iStart;
		t.m_Near = 0_1;
	}

	int iEnd = narrow_cast<int>(CalcCubicIdx( t.m_Far ));
	if( common::Equals( t.m_Far, 0_1, 10_1*epsilon ) && iEnd ){
		iEnd = iEnd-1;
		t.m_Far = 1_1;
	}

	if( iStart == iEnd )
		m_CubicFunctions[iStart].Shorten( {t.Near(),t.Far()} );
	else{
		m_CubicFunctions[iStart].Shorten( {t.Near(),1_1} );
		m_CubicFunctions[iEnd].Shorten( {0_1,t.Far()} );
	}

	DataType newSpline;
	newSpline.reserve( 1 + iEnd - iStart );
	for( int idx = iStart; idx <= iEnd; ++idx )
		newSpline.push_back( m_CubicFunctions[idx].GetData() );

	return Create( newSpline );
}

int SplineFunction::CountControlPoints() const noexcept{
	if( m_CubicFunctions.empty() )
		return 0;
	
	return static_cast<int>(m_CubicFunctions.size()) + 1;
}

Interval<One> SplineFunction::ResetControlPoint( int idx, const VectorBundle<Length>& bundle ){
	if( IsValid() ){
		if( idx < 0 )
			throw std::out_of_range( "Invalid index value!" );
		else if( idx == 0 ){
			m_CubicFunctions[0].Create( bundle, GetControlPoint( 1 ) );
			m_Data[0] = m_CubicFunctions[0].GetData();
		}
		else if( idx < static_cast<int>(m_CubicFunctions.size()) ){
			m_CubicFunctions[idx-1].Create( GetControlPoint( idx-1 ), bundle );
			m_Data[idx-1] = m_CubicFunctions[idx-1].GetData();
			m_CubicFunctions[idx].Create( bundle, GetControlPoint( idx+1 ) );
			m_Data[idx] = m_CubicFunctions[idx].GetData();
		}
		else if( idx == static_cast<int>(m_CubicFunctions.size()) ){
			m_CubicFunctions[idx-1].Create( GetControlPoint( idx-1 ), bundle );
			m_Data[idx-1] = m_CubicFunctions[idx-1].GetData();
		}
		else
			throw std::out_of_range( "Invalid index value!" );

		return { 0_1, narrow_cast<Real>(m_CubicFunctions.size()) };
	}

	throw std::logic_error( "The Spline is not created yet!" );
}

VectorBundle<Length> SplineFunction::GetControlPoint( int idx ) const{
	if( IsValid() ){
		if( idx >= 0 ){
			if( idx < static_cast<int>(m_CubicFunctions.size()) )
				return { m_CubicFunctions[idx](0.f), m_Data[idx].CubicOvershootAtStart() };
			if( idx == static_cast<int>(m_CubicFunctions.size()) )
				return { m_CubicFunctions[idx-1](1.f), m_Data[idx-1].CubicOvershootAtEnd() };
		}

		throw std::out_of_range( "Invalid index value!" );
	}

	throw std::logic_error( "The Spline is not created yet!" );
}

bool SplineFunction::HasGaps( Length e ) const noexcept{
	for( size_t idx = 0; idx < m_CubicFunctions.size()-1; ++idx )
		if( !m_CubicFunctions.at(idx).P(1.0f).Equals( m_CubicFunctions.at(idx+1).P(0.0f),e) )
			return true;

	return false;
}

bool SplineFunction::HasKinks( Angle e ) const noexcept{
	for( size_t idx = 0; idx < m_CubicFunctions.size()-1; ++idx )
		if( !Normalize(m_CubicFunctions.at(idx).D1(1.0f)).second.Equals( Normalize(m_CubicFunctions.at(idx+1).D1(0.0f)).second,e) )
			return true;

	return false;
}

Interval<One> SplineFunction::Create( const std::vector<VectorBundle<Length>>& controlPoints ){
	if( controlPoints.size() < 2 )
		throw std::invalid_argument( "Need at least 2 control points!" );
	if( controlPoints.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()) )
		throw std::invalid_argument( "Too many control points!" );

	m_CubicFunctions.resize( controlPoints.size() - 1 );
	m_Data.resize( controlPoints.size() - 1 );

	for( size_t idx = 0; idx < m_CubicFunctions.size(); ++idx ){
		m_CubicFunctions.at(idx).Create( controlPoints.at(idx), controlPoints.at(idx+1) );
		m_Data.at(idx) = m_CubicFunctions.at(idx).GetData();
	}

	return { 0.f, narrow_cast<Real>(m_CubicFunctions.size()) };
}

Interval<One> SplineFunction::Create( const std::vector<Position<Length>>& controlPoints ){
	return Create( controlPoints, false, Null<Length>, false, Null<Length> );
}

Interval<One> SplineFunction::Create( 
	const std::vector<Position<Length>>& controlPoints,
	bool bClampedAtStart, 
	const Vector<Length>& Yp1, 
	bool bClampedAtEnd, 
	const Vector<Length>& Ypn )
{
	if( controlPoints.size() < 2 )
		throw std::invalid_argument( "Need at least 2 control points!" );
	if( controlPoints.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()) )
		throw std::invalid_argument( "Too many control points!" );

	m_CubicFunctions.resize( controlPoints.size() - 1 );
	m_Data.resize( controlPoints.size() - 1 );

	std::vector<double> Yx( controlPoints.size() );
	std::vector<double> Yy( controlPoints.size() );
	std::vector<double> Yz( controlPoints.size() );

	for( size_t idx = 0; idx < controlPoints.size(); ++idx ){
		Yx[idx] = controlPoints[idx].x.Units();
		Yy[idx] = controlPoints[idx].y.Units();
		Yz[idx] = controlPoints[idx].z.Units();
	}

	std::vector<double> Y2x = SplineCalculateD2( Yx, bClampedAtStart, Yp1.dx.Units(), bClampedAtEnd, Ypn.dx.Units() );
	std::vector<double> Y2y = SplineCalculateD2( Yy, bClampedAtStart, Yp1.dy.Units(), bClampedAtEnd, Ypn.dy.Units() );
	std::vector<double> Y2z = SplineCalculateD2( Yz, bClampedAtStart, Yp1.dz.Units(), bClampedAtEnd, Ypn.dz.Units() );

	const auto fx = [&Yx,&Y2x]( double t ) noexcept -> double{
		return SplineEvaluate( Yx, Y2x, t );
	};
	const auto fy = [&Yy,&Y2y]( double t ) noexcept -> double{
		return SplineEvaluate( Yy, Y2y, t );
	};
	const auto fz = [&Yz,&Y2z]( double t ) noexcept -> double{
		return SplineEvaluate( Yz, Y2z, t );
	};
	
	for( size_t idx = 0; idx < m_CubicFunctions.size(); ++idx ){
		const double t = narrow_cast<double>(idx);
		m_CubicFunctions[idx].Create( 
#if BOOST_VERSION < 107000
 			{	{	Length{static_cast<Real>(fx(t))},
					Length{static_cast<Real>(fy(t))},
					Length{static_cast<Real>(fz(t))} }, 
				{	Length{static_cast<Real>(boost::math::tools::finite_difference_derivative( fx, t ))},
					Length{static_cast<Real>(boost::math::tools::finite_difference_derivative( fy, t ))},
					Length{static_cast<Real>(boost::math::tools::finite_difference_derivative( fz, t ))} } },			
			{	{	Length{static_cast<Real>(fx(t+1))},
					Length{static_cast<Real>(fy(t+1))},
					Length{static_cast<Real>(fz(t+1))} }, 
				{	Length{static_cast<Real>(boost::math::tools::finite_difference_derivative( fx, t+1 ))},
					Length{static_cast<Real>(boost::math::tools::finite_difference_derivative( fy, t+1 ))},
					Length{static_cast<Real>(boost::math::tools::finite_difference_derivative( fz, t+1 ))} } } 
		);
#else
			{	{	Length{static_cast<Real>(fx(t))},
					Length{static_cast<Real>(fy(t))},
					Length{static_cast<Real>(fz(t))} }, 
				{	Length{static_cast<Real>(boost::math::differentiation::finite_difference_derivative( fx, t ))},
					Length{static_cast<Real>(boost::math::differentiation::finite_difference_derivative( fy, t ))},
					Length{static_cast<Real>(boost::math::differentiation::finite_difference_derivative( fz, t ))} } },			
			{	{	Length{static_cast<Real>(fx(t+1))},
					Length{static_cast<Real>(fy(t+1))},
					Length{static_cast<Real>(fz(t+1))} }, 
				{	Length{static_cast<Real>(boost::math::differentiation::finite_difference_derivative( fx, t+1 ))},
					Length{static_cast<Real>(boost::math::differentiation::finite_difference_derivative( fy, t+1 ))},
					Length{static_cast<Real>(boost::math::differentiation::finite_difference_derivative( fz, t+1 ))} } } 
		);
#endif // BOOST_VERSION < 107000


		m_Data[idx] = m_CubicFunctions[idx].GetData();
	}

	return { 0.f, narrow_cast<Real>(m_CubicFunctions.size()) };
}

Interval<One> SplineFunction::CreateBezier( const std::vector<spat::Position<Length>>& controlPoints, Spline::WrapTypes wrap )
{
	if( controlPoints.size() < 4 )
		throw std::invalid_argument( "Need at least 4 control points!" );
	if( controlPoints.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()) )
		throw std::invalid_argument( "Too many control points!" );

	size_t cntSegments;
	switch( wrap ){
	case Spline::WrapTypes::nonperiodic:
		if( (controlPoints.size() - 4) % 3 != 0 )
			throw std::invalid_argument( "Invalid number of control points!" );

		cntSegments = 1 + (controlPoints.size() - 4) / 3;
		break;
	case Spline::WrapTypes::periodic:
		if( controlPoints.size() % 3 != 0 )
			throw std::invalid_argument( "Invalid number of control points!" );

		cntSegments = controlPoints.size() / 3;
		break;
	case Spline::WrapTypes::pinned:
		throw std::invalid_argument( "Pinned wrap type not supported for Bezier curves!" );
	default:
		throw std::invalid_argument( "Invalid wrap type!" );
	}

	m_CubicFunctions.resize( cntSegments );
	m_Data.resize( cntSegments );

	if( wrap == Spline::WrapTypes::periodic ){
		for( size_t idx = 0; idx < cntSegments-1; ++idx ){
			m_CubicFunctions[idx].CreateBezier(
				controlPoints[3*idx + 0],
				controlPoints[3*idx + 1],
				controlPoints[3*idx + 2],
				controlPoints[3*idx + 3] );
			m_Data[idx] = m_CubicFunctions[idx].GetData();
		}

		m_CubicFunctions.back().CreateBezier(
			controlPoints[3*(cntSegments-1) + 0],
			controlPoints[3*(cntSegments-1) + 1],
			controlPoints[3*(cntSegments-1) + 2],
			controlPoints[0] );
		m_Data.back() = m_CubicFunctions.back().GetData();
	}
	else{
		for( size_t idx = 0; idx < cntSegments; ++idx ){
			m_CubicFunctions[idx].CreateBezier( 
				controlPoints[3*idx + 0], 
				controlPoints[3*idx + 1],
				controlPoints[3*idx + 2],
				controlPoints[3*idx + 3] );
			m_Data[idx] = m_CubicFunctions[idx].GetData();
		}
	}

	return { 0.f, narrow_cast<Real>(cntSegments) };
}

Interval<One> SplineFunction::Create( const DataType& data ){
	if( data.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()) )
		throw std::invalid_argument( "Too many control points!" );

	m_Data = data;
	m_CubicFunctions.resize( m_Data.size() );

	for( size_t idx = 0; idx < m_Data.size(); ++idx )
		m_CubicFunctions[idx].Create( m_Data[idx] );

	return { 0.f, narrow_cast<Real>(m_CubicFunctions.size()) };
}

std::vector<double> SplineFunction::SplineCalculateD2( 
	const std::vector<double>& Y, 
	bool bClampedAtStart, 
	const double D11, 
	bool bClampedAtEnd, 
	const double D1n )
{
	const std::size_t n = Y.size();
	std::vector<double> D2(n);
	std::vector<double> V(n-1);
	if( bClampedAtStart ){
		D2[0] = -0.5;
		V[0] = 3.0 * (Y[1]- Y[0] - D11);
	}
	else
		D2[0] = V[0] = 0.0;

	for( std::size_t i = 1; i < n-1; ++i ){
		double p = D2[i-1u] + 4.0;
		D2[i] = -1.0 / p;
		V[i] = (Y[i+1]-Y[i]) - (Y[i]-Y[i-1u]);
		V[i] = (6.0 * V[i] - V[i-1u]) / p;
	}

	double Qn, Vn;
	if( bClampedAtEnd ){
		Qn = 0.5;
		Vn = 3.0 * (D1n - Y[n-1u] + Y[n-2u]);
	}
	else
		Qn = Vn = 0.0;

	D2[n-1u] = (Vn - Qn * V[n-2u]) / (Qn * D2[n-2] + 1.0 );
	for( int i = narrow_cast<int>(n-2); i >= 0; --i )
		D2[i] = D2[i] * D2[static_cast<std::size_t>(i)+1u] + V[i];

	return D2;
}

double SplineFunction::SplineEvaluate( 
	const std::vector<double>& Y, 
	const std::vector<double>& D2, 
	const double t ) noexcept
{
	const std::size_t n = Y.size();

	double idx;
	(void)std::modf( t, &idx );
	std::size_t k = static_cast<std::size_t>(idx);
	if( k > n-2 )
		k = n-2;

	const double a = (static_cast<double>(k+1)-t);
	const double b = (t - static_cast<double>(k));
	return a * Y[k] + b * Y[k+1] + ((a*a*a-a) * D2[k] + (b*b*b-b) * D2[k+1]) / 6.0f;
}
///////////////////////////////////////
std::unique_ptr<Spline> Spline::Make() noexcept{
	try{
		return std::make_unique<Spline_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
const char* Spline_Imp::TypeName() const noexcept{
	return "Spline";
}

Curve::CurveType Spline_Imp::GetCurveType() const noexcept{
	return CurveType::Spline;
}

std::unique_ptr<Curve> Spline_Imp::Clone() const{
	return std::make_unique<Spline_Imp>( *this );
}

bool Spline_Imp::IsFlat() const noexcept{
	return f.f.IsFlat();
}

Vector<One> Spline_Imp::LocalUp() const{
	if( ZeroSet().empty() )
		return f.f.LocalUp();

	throw std::runtime_error( "This spline does not maintain any notion of 'up'!" );
}

int Spline_Imp::CountControlPoints() const noexcept{
	return f.f.CountControlPoints();
}

common::Interval<Length> Spline_Imp::ResetControlPointAndTangent( int idx, const VectorBundle<Length>& bundle ){
	f.Length( f.f.ResetControlPoint( idx, bundle ).Length() );
	return Sample();
}

VectorBundle<Length> Spline_Imp::GetControlPointAndTangent( int idx ) const{
	return f.f.GetControlPoint( idx );
}

Length Spline_Imp::GetParameter( int idx ) const{
	if( IsValid() ){
		if( idx < 0 || idx >= CountControlPoints() )
			throw std::out_of_range( "Invalid index value!" );

		return s( static_cast<One>(idx) / (CountControlPoints()-1) );
	}

	return 0_m;
}

int Spline_Imp::UpperBound( Length s ) const{
	Real integral;
	std::modf( t( s ) * (CountControlPoints()-1), &integral );
	return narrow_cast<int>(integral + 1);
}

bool Spline_Imp::HasGaps( Length e ) const noexcept{
	if( !IsValid() )
		return true;

	return f.f.HasGaps( e );
}

bool Spline_Imp::HasKinks( Angle e ) const noexcept{
	if( !IsValid() )
		return true;

	return f.f.HasKinks( e );
}

Interval<Length> Spline_Imp::Create( 
	const Curve& originalCurve, 
	Interval<Length> range,
	Length maxDeviation,
	common::Interval<Length> sampleDistanceLimits )
{
	if( !originalCurve.IsValid() )
		throw std::invalid_argument( "Spline_Imp::Create(): no valid curve to create from." );
	if( !sampleDistanceLimits.Normal() ||
		sampleDistanceLimits.Near() <= 0_m )
		throw std::invalid_argument( "Spline_Imp::Create(): invalid parameter sampleDistanceLimits." );

	range.Intersection( originalCurve.Range() );
	range.Normalize();
	if( range.Length() < epsilon__length )
		throw std::invalid_argument( "Spline_Imp::Create(): zero length curve." );

	if( maxDeviation <= 0_m )
		throw std::invalid_argument( "Spline_Imp::Create(): invalid parameter epsilon." );

	Spline::Data data;
	while( range.Near() < range.Far() )
		range.Near( Subdivide( data, originalCurve, range, maxDeviation, sampleDistanceLimits ) );

	return Create( data );
}

common::Interval<Length> Spline_Imp::Create( 
	std::pair<std::shared_ptr<const Curve>, 
	common::Interval<Length>> originalCurve, 
	Length maxDeviation,
	common::Interval<Length> sampleDistanceLimits )
{
	if( !originalCurve.first )
		throw std::invalid_argument( "Spline_Imp::Create(): no valid curve to create from." );

	return Create( *originalCurve.first, originalCurve.second, maxDeviation, sampleDistanceLimits );
}

Interval<Length> Spline_Imp::Create( const std::vector<VectorBundle<Length>>& controlPointsAndTangents ){
	f.Length( f.f.Create( controlPointsAndTangents ).Length() );
	return Sample();
}

Interval<Length> Spline_Imp::Create( const std::vector<Position<Length>>& controlPoints ){
	f.Length( f.f.Create( controlPoints ).Length() );
	return Sample();
}

Interval<Length> Spline_Imp::Create( const std::vector<Position<Length>>& controlPoints, bool bClampedAtStart, const Vector<Length>& startTangent, bool bClampedAtEnd, const Vector<Length>& endTangent ){
	f.Length( f.f.Create( controlPoints, bClampedAtStart, startTangent, bClampedAtEnd, endTangent ).Length() );
	return Sample();
}

Interval<Length> Spline_Imp::CreateCatmullRom( 
	const std::vector<Position<Length>>& controlPoints, 
	One tension, 
	WrapTypes /*wrap*/ )
{
	if( controlPoints.size() < 2 )
		throw std::invalid_argument( "Need at least 2 control points!" );
	if( tension <= 0 )
		throw std::invalid_argument( "Tension has to be > 0!" );

	std::vector<VectorBundle<Length>> controlPointsAndTangents{ controlPoints.size() };

	controlPointsAndTangents[0].P = controlPoints[0];
	controlPointsAndTangents[0].T = tension * (controlPoints[1] - controlPoints[0]);

	size_t idx = 1;
	for( ; idx < controlPointsAndTangents.size() - 1; ++idx ){
		controlPointsAndTangents[idx].P = controlPoints[idx];
		controlPointsAndTangents[idx].T = tension * (controlPoints[idx+1] - controlPoints[idx-1]);
	}

	controlPointsAndTangents[idx].P = controlPoints[idx];
	controlPointsAndTangents[idx].T = tension * (controlPoints[idx] - controlPoints[idx-1]);

	return Create( controlPointsAndTangents );
}

Interval<Length> Spline_Imp::CreateCatmullRom( 
	const std::vector<spat::VectorBundle<Length,One>>& controlPoints, 
	One tension, 
	WrapTypes /*wrap*/ )
{
	if( controlPoints.size() < 2 )
		throw std::invalid_argument( "Need at least 2 control points!" );
	if( tension <= 0 )
		throw std::invalid_argument( "Tension has to be > 0!" );

	std::vector<VectorBundle<Length>> controlPointsAndTangents{ controlPoints.size() };

	controlPointsAndTangents[0].P = controlPoints[0].P;
	controlPointsAndTangents[0].T = tension * (controlPoints[1].P - controlPoints[0].P).Length() * controlPoints[0].T;

	size_t idx = 1;
	for( ; idx < controlPointsAndTangents.size() - 1; ++idx ){
		controlPointsAndTangents[idx].P = controlPoints[idx].P;
		controlPointsAndTangents[idx].T = tension * (controlPoints[idx+1].P - controlPoints[idx-1].P).Length() * controlPoints[idx].T;
	}

	controlPointsAndTangents[idx].P = controlPoints[idx].P;
	controlPointsAndTangents[idx].T = tension * (controlPoints[idx].P - controlPoints[idx-1].P).Length() * controlPoints[idx].T;

	return Create( controlPointsAndTangents );
}

Interval<Length> Spline_Imp::CreateBezier( 
	const std::vector<spat::Position<Length>>& controlPoints, 
	WrapTypes wrap )
{
	f.Length( f.f.CreateBezier( controlPoints, wrap ).Length() );
	return Sample();
}

common::Interval<Length> Spline_Imp::Shorten( common::Interval<Length> toRange ){
	toRange.Intersection( Range() );
	if( toRange.Length() < epsilon__length )
		return Range();

	f.Length( f.f.Shorten( f.Length() * Interval<One>{t(toRange.Near()), t(toRange.Far())} ).Length() );
	return Sample();
}

Length Spline_Imp::Subdivide( 
	Spline::Data& data,
	const Curve& originalCurve, 
	Interval<Length> range, 
	Length maxDeviation,
	common::Interval<Length> sampleDistanceLimits,
	bool bShorten )
{
	if( !sampleDistanceLimits.Includes( range.Length() - epsilon__length ) )
		throw std::runtime_error( "Spline_Imp::Subdivide(): sample distance out of limits." );

	try{
		std::unique_ptr<Cubic> pCubic = Cubic::Make();
		const Length deviation = pCubic->Create( originalCurve, range, maxDeviation ).second;
		if( deviation < maxDeviation ){
			if( bShorten && range.Length() > SampleStep())
			// the alignment of the curves with the sample step increases accuracy.
			{
				const Interval<Length> cubicsRange = pCubic->Range();
				if( cubicsRange.Length() > SampleStep() )
				{
					Real integer;
					const Real fraction = std::modf( cubicsRange.Length() / SampleStep(), &integer );
					pCubic->Shorten( { cubicsRange.Near(), cubicsRange.Far() - fraction * SampleStep() } );
					range.Far(range.Far() - fraction * SampleStep());
				}
			}

			data.push_back( pCubic->GetData() );
			return range.Far();
		}
	}
	catch( std::runtime_error& ){
	}

	return Subdivide( data, originalCurve, {range.Near(), range.Center()}, maxDeviation, sampleDistanceLimits, true );
}
///////////////////////////////////////
bool Equals( 
	const Spline::Data& A, 
	const Spline::Data& B, 
	Interval<Length> range, 
	Length epsilon_length, 
	Angle epsilon_angle ) noexcept
{
	if( A.size() != B.size() )
		return false;

	for( size_t idx = 0; idx < A.size(); ++idx )
		if( !Equals( A[idx], B[idx], range, epsilon_length, epsilon_angle ) )
			return false;

	return true;
}
///////////////////////////////////////
}
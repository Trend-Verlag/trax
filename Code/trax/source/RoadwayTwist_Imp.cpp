// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "In case of the sounds of serious battle [Major-General McPherson] will close in 
//	 on General Schofield, but otherwise will keep every man of his command at work 
//	 in destroying the railroad by tearing up track, burning the ties and iron, and 
//	 twisting the bars when hot. Officers should be instructed that bars simply bent 
//	 may be used again, but if when red hot they are twisted out of line they cannot 
//	 be used again. Pile the ties into shape for a bonfire, put the rails across and 
//	 when red hot in the middle, let a man at each end twist the bar so that its 
//	 surface becomes spiral."
//
//						- Wm. T. Sherman, Special Field Orders, July 18, 1864.



#include "RoadwayTwist_Imp.h"

#include <algorithm>

namespace trax{
	using namespace common;
	using namespace spat;

///////////////////////////////////////
std::unique_ptr<RoadwayTwist> RoadwayTwist::Make( TwistType type ) noexcept{
	switch( type ){
	case RoadwayTwist::TwistType::Constant:
		return ConstantTwist::Make();
	case RoadwayTwist::TwistType::Linear:
		return LinearTwist::Make();
	case RoadwayTwist::TwistType::Piecewise:
		return PiecewiseTwist::Make( TwistType::Piecewise );
	case RoadwayTwist::TwistType::PiecewiseLinear:
		return PiecewiseTwist::Make( TwistType::PiecewiseLinear );
	case RoadwayTwist::TwistType::PiecewiseCircular:
		return PiecewiseTwist::Make( TwistType::PiecewiseCircular );
	case RoadwayTwist::TwistType::Directional:
		return DirectionalTwist::Make();
	case RoadwayTwist::TwistType::Combined:
		return CombinedTwist::Make();
	case RoadwayTwist::TwistType::Zero:
	case RoadwayTwist::TwistType::Positional:
	case RoadwayTwist::TwistType::Parallel:
	default:
		return nullptr;
	}
}

std::unique_ptr<RoadwayTwist> RoadwayTwist::Make(
	TwistType type, 
	const RoadwayTwist& fromTwist ) noexcept
{
	switch( type ){
	case RoadwayTwist::TwistType::Constant:
		return ConstantTwist::Make( fromTwist );
	case RoadwayTwist::TwistType::Linear:
		return LinearTwist::Make( fromTwist );
	case RoadwayTwist::TwistType::Piecewise:
		return PiecewiseTwist::Make( TwistType::Piecewise, fromTwist );
	case RoadwayTwist::TwistType::PiecewiseLinear:
		return PiecewiseTwist::Make( TwistType::PiecewiseLinear, fromTwist );
	case RoadwayTwist::TwistType::PiecewiseCircular:
		return PiecewiseTwist::Make( TwistType::PiecewiseCircular, fromTwist );
	case RoadwayTwist::TwistType::Directional:
		return DirectionalTwist::Make( fromTwist );
	case RoadwayTwist::TwistType::Combined:
		return CombinedTwist::Make( fromTwist );
	case RoadwayTwist::TwistType::Zero:
	case RoadwayTwist::TwistType::Positional:
	case RoadwayTwist::TwistType::Parallel:
	default:
		return nullptr;
	}
}
///////////////////////////////////////
bool Equals( 
	const RoadwayTwist& twistA, 
	const RoadwayTwist& twistB, 
	Angle epsilon_angle ) noexcept
{
	return twistA.Equals( twistB, epsilon_angle );
}

const char* TypeToName( RoadwayTwist::TwistType type ){
	switch( type ){		
	case RoadwayTwist::TwistType::Zero:
		return "ZeroTwist";
	case RoadwayTwist::TwistType::Constant:
		return "ConstantTwist";
	case RoadwayTwist::TwistType::Linear:
		return "LinearTwist";
	case RoadwayTwist::TwistType::Piecewise:
		return "PiecewiseTwist";
	case RoadwayTwist::TwistType::PiecewiseLinear:
		return "PiecewiseLinearTwist";
	case RoadwayTwist::TwistType::PiecewiseCircular:
		return "PiecewiseCircularTwist";
	case RoadwayTwist::TwistType::Positional:
		return "PositionalTwist";
	case RoadwayTwist::TwistType::Directional:
		return "DirectionalTwist";
	case RoadwayTwist::TwistType::Combined:
		return "CombinedTwist";
	case RoadwayTwist::TwistType::Parallel:
		return "ParallelTwist";
	default:
		return "None";
	}
}

RoadwayTwist::TwistType TwistNameToType( const std::string& name ) noexcept{
	if( name.compare( "ZeroTwist" ) == 0 )
		return RoadwayTwist::TwistType::Zero;
	if( name.compare( "ConstantTwist" ) == 0 )
		return RoadwayTwist::TwistType::Constant;
	if( name.compare( "LinearTwist" ) == 0 )
		return RoadwayTwist::TwistType::Linear;
	if( name.compare( "PiecewiseTwist" ) == 0 )
		return RoadwayTwist::TwistType::Piecewise;
	if( name.compare( "PiecewiseLinearTwist" ) == 0 )
		return RoadwayTwist::TwistType::PiecewiseLinear;
	if( name.compare( "PiecewiseCircularTwist" ) == 0 )
		return RoadwayTwist::TwistType::PiecewiseCircular;
	if( name.compare( "PositionalTwist" ) == 0 )
		return RoadwayTwist::TwistType::Positional;
	if( name.compare( "DirectionalTwist" ) == 0 )
		return RoadwayTwist::TwistType::Directional;
	if( name.compare( "CombinedTwist" ) == 0 )
		return RoadwayTwist::TwistType::Combined;
	if( name.compare( "ParallelTwist" ) == 0 )
		return RoadwayTwist::TwistType::Parallel;

	return RoadwayTwist::TwistType::None;
}
///////////////////////////////////////
std::unique_ptr<ConstantTwist> ConstantTwist::Make() noexcept
{
	try{
		return std::make_unique<ConstantTwist_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

std::unique_ptr<ConstantTwist> ConstantTwist::Make( const RoadwayTwist& fromTwist ) noexcept
{
	try{
		return std::make_unique<ConstantTwist_Imp>( fromTwist );
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

ConstantTwist_Imp::ConstantTwist_Imp( Real twistvalue ) noexcept
	:	m_Twist{ twistvalue }
{}

ConstantTwist_Imp::ConstantTwist_Imp( const RoadwayTwist& fromTwist )
	:	m_Twist{ fromTwist.Twist( 0_m ) }
{
}

const char*	ConstantTwist_Imp::TypeName() const noexcept{
	return "ConstantTwist";
}

RoadwayTwist::TwistType ConstantTwist_Imp::GetTwistType() const noexcept{
	return TwistType::Constant;
}

std::unique_ptr<RoadwayTwist> ConstantTwist_Imp::Clone() const{
	return std::make_unique<ConstantTwist_Imp>( *this );
}

void ConstantTwist_Imp::Flip() noexcept{
	RoadwayTwist_Imp<ConstantTwist>::Flip();

	m_Twist = pi - m_Twist;
}

void ConstantTwist_Imp::Mirror() noexcept{
	m_Twist = -m_Twist;
}

bool ConstantTwist_Imp::Offset( Angle offset ) noexcept{
	m_Twist += offset;
	return true;
}

bool ConstantTwist_Imp::Equals( const RoadwayTwist& toTwist, Angle epsilon_angle ) const noexcept{
	if( auto pOther = dynamic_cast<const ConstantTwist*>(&toTwist) )
		return abs(TwistValue() - pOther->TwistValue()) <= epsilon_angle;

	return false;
}

void ConstantTwist_Imp::TwistValue( Angle twist ) noexcept{
	m_Twist = twist;
}

Angle ConstantTwist_Imp::TwistValue() const noexcept{
	return m_Twist;
}

const ConstantTwist::Data& ConstantTwist_Imp::GetData() const noexcept{
	return m_Twist;
}

void ConstantTwist_Imp::Create( const Data& data ){
	m_Twist = data;
}

Angle ConstantTwist_Imp::DoTwist( Length ) const noexcept{
	return m_Twist;
}

AnglePerLength ConstantTwist_Imp::DoD1( Length ) const{
	return 0_1Im;
}
///////////////////////////////////////
std::unique_ptr<LinearTwist> LinearTwist::Make( Angle from, Angle to ) noexcept
{
	try{
		return std::make_unique<LinearTwist_Imp>( from, to );
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

std::unique_ptr<LinearTwist> LinearTwist::Make( const RoadwayTwist& fromTwist ) noexcept
{
	try{
		return std::make_unique<LinearTwist_Imp>( fromTwist );
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

LinearTwist_Imp::LinearTwist_Imp( Angle from, Angle to ) noexcept
	: m_Data{ from, to }
{}

LinearTwist_Imp::LinearTwist_Imp( const RoadwayTwist& fromTwist )
{
	if( const LinearTwist* pLinearTwist = dynamic_cast<const LinearTwist*>(&fromTwist) )
	{
		m_Data.m_Near = pLinearTwist->From();
		m_Data.m_Far = pLinearTwist->To();
	}
	else if( const PiecewiseTwist* pPiecewiseTwist = dynamic_cast<const PiecewiseTwist*>(&fromTwist) )
	{
		if( pPiecewiseTwist->CntTwistValues() )
		{
			m_Data.m_Near = pPiecewiseTwist->Twist( 0 ).second;
			m_Data.m_Far = pPiecewiseTwist->Twist( pPiecewiseTwist->CntTwistValues() - 1 ).second;
		}
		else
		{
			m_Data.m_Far = m_Data.m_Near = 0_rad;
		}
	}
	else
		m_Data.m_Far = m_Data.m_Near = fromTwist.Twist( 0_m );
}

const char*	LinearTwist_Imp::TypeName() const noexcept{
	return "LinearTwist";
}

RoadwayTwist::TwistType LinearTwist_Imp::GetTwistType() const noexcept{
	return TwistType::Linear;
}

std::unique_ptr<RoadwayTwist> LinearTwist_Imp::Clone() const{
	return std::make_unique<LinearTwist_Imp>( *this );
}

void LinearTwist_Imp::Flip() noexcept{
	RoadwayTwist_Imp<LinearTwist>::Flip();

	std::swap(m_Data.m_Far,m_Data.m_Near);
	m_Data.m_Near	= pi - m_Data.m_Near;
	m_Data.m_Far	= pi - m_Data.m_Far;
}

void LinearTwist_Imp::Mirror() noexcept{
	m_Data.m_Near	= -m_Data.m_Near;
	m_Data.m_Far	= -m_Data.m_Far;
}

bool LinearTwist_Imp::Offset( Angle offset ) noexcept{
	m_Data.m_Near	+= offset;
	m_Data.m_Far	+= offset;
	return true;
}

bool LinearTwist_Imp::Equals( const RoadwayTwist & toTwist, Angle epsilon_angle ) const noexcept{
	if( auto pOther = dynamic_cast<const LinearTwist*>(&toTwist) )
		return abs(From() - pOther->From()) <= epsilon_angle &&
			   abs(To() - pOther->To()) <= epsilon_angle;

	return false;
}

Angle LinearTwist_Imp::DoTwist( Length s ) const{
	if( TrackLength() == 0_m )
		return m_Data.m_Near;

	return m_Data.m_Near + s / TrackLength() * (m_Data.m_Far - m_Data.m_Near);
}

AnglePerLength LinearTwist_Imp::DoD1( Length ) const{
	if( TrackLength() == 0_m )
		return std::numeric_limits<AnglePerLength>::max();

	return (m_Data.m_Far - m_Data.m_Near) / TrackLength();
}

void LinearTwist_Imp::From( Angle twist ) noexcept{
	m_Data.m_Near = twist;
}

Angle LinearTwist_Imp::From() const noexcept{
	return m_Data.m_Near;
}

void LinearTwist_Imp::To( Angle twist ) noexcept{
	m_Data.m_Far = twist;
}

Angle LinearTwist_Imp::To() const noexcept{
	return m_Data.m_Far;
}

const LinearTwist::Data& LinearTwist_Imp::GetData() const noexcept{
	return m_Data;
}

void LinearTwist_Imp::Create( const Data& data ){
	m_Data = data;
}
///////////////////////////////////////
std::unique_ptr<PiecewiseTwist> PiecewiseTwist::Make( TwistType type ) noexcept
{
	try{
		switch( type ){
		case TwistType::Piecewise:
			return std::make_unique<PiecewiseTwist_Imp>();
		case TwistType::PiecewiseLinear:
			return std::make_unique<PiecewiseLinearTwist_Imp>();
		case TwistType::PiecewiseCircular:
			return std::make_unique<PiecewiseCircularTwist_Imp>();
		default:
			return nullptr;
		}
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

std::unique_ptr<PiecewiseTwist> PiecewiseTwist::Make( TwistType type, const RoadwayTwist& fromTwist ) noexcept
{
	try{
		switch( type ){
		case TwistType::Piecewise:
			return std::make_unique<PiecewiseTwist_Imp>( fromTwist );
		case TwistType::PiecewiseLinear:
			return std::make_unique<PiecewiseLinearTwist_Imp>( fromTwist );
		case TwistType::PiecewiseCircular:
			return std::make_unique<PiecewiseCircularTwist_Imp>( fromTwist );
		default:
			return nullptr;
		}
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

PiecewiseTwist_Imp::PiecewiseTwist_Imp( const RoadwayTwist& fromTwist )
	:	PiecewiseTwist_ImpBase<PiecewiseTwist>{}
{
	if( const LinearTwist* pLinearTwist = dynamic_cast<const LinearTwist*>(&fromTwist) ){
		Add( 0_m, pLinearTwist->From() );
		Add( 100_m, pLinearTwist->To() );
	}
	else if( const PiecewiseTwist* pPiecewiseTwist = dynamic_cast<const PiecewiseTwist*>(&fromTwist) ){
		Create( pPiecewiseTwist->GetData() );
	}
	else if( const PiecewiseDirectionalTwist* pDirectionalTwist = dynamic_cast<const PiecewiseDirectionalTwist*>(&fromTwist) ){
		for( int idx = 0; idx < pDirectionalTwist->CntTwistValues(); ++idx )
			Add( pDirectionalTwist->Twist( idx ).first, pDirectionalTwist->Twist( pDirectionalTwist->Twist( idx ).first ) );
	}
	else{
		Add( 0_m, fromTwist.Twist( 0_m ) );
		Add( 100_m, fromTwist.Twist( 0_m ) );
	}
}

const char*	PiecewiseTwist_Imp::TypeName() const noexcept{
	return "PiecewiseTwist";
}

RoadwayTwist::TwistType PiecewiseTwist_Imp::GetTwistType() const noexcept{
	return TwistType::Piecewise;
}

std::unique_ptr<RoadwayTwist> PiecewiseTwist_Imp::Clone() const{
	return std::make_unique<PiecewiseTwist_Imp>( *this );
}

void PiecewiseTwist_Imp::Flip() noexcept{
	PiecewiseTwist_ImpBase<PiecewiseTwist>::Flip();

	for( auto& value : m_Data ){
		value.second = pi - value.second;
	}
}

void PiecewiseTwist_Imp::Mirror() noexcept{
	for( auto& value : m_Data )
		value.second = -value.second;
}

bool PiecewiseTwist_Imp::Offset( Angle offset ) noexcept{
	for( auto& value : m_Data )
		value.second += offset;

	return true;
}

bool PiecewiseTwist_Imp::IsConstant() const noexcept
{
	for( const auto& pair : m_Data )
		if( pair.second != m_Data.front().second )
			return false;

	return true;
}

bool PiecewiseTwist_Imp::IsNull() const noexcept
{
	for( const auto& pair : m_Data )
		if( pair.second != 0 )
			return false;

	return true;
}

bool PiecewiseTwist_Imp::Equals( const RoadwayTwist& toTwist, Angle epsilon_angle ) const noexcept
{
	if( auto pOther = dynamic_cast<const PiecewiseTwist_Imp*>(&toTwist) )
	{
		return std::equal(	m_Data.begin(), m_Data.end(),
							pOther->m_Data.begin(), pOther->m_Data.end(),
							[epsilon_angle]( const Data::value_type& pairA, const Data::value_type& pairB )
		{ 
			return common::Equals( pairA.first, pairB.first, epsilon__length ) &&
				   common::Equals( pairA.second, pairB.second, epsilon_angle );
		} );
	}

	return false;
}

Angle PiecewiseTwist_Imp::DoTwist( Length s ) const{
	Data::const_iterator iA, iB;
	Bracket( s, iA, iB );

	return iA->second;
}

AnglePerLength PiecewiseTwist_Imp::DoD1( Length s ) const{
	Data::const_iterator iA, iB;
	Bracket( s, iA, iB );
	return (iB->second - iA->second) / (iB->first - iA->first);
}
///////////////////////////////////////
PiecewiseLinearTwist_Imp::PiecewiseLinearTwist_Imp( const RoadwayTwist& fromTwist )
	: PiecewiseTwist_Imp{ fromTwist }
{
}

const char* PiecewiseLinearTwist_Imp::TypeName() const noexcept{
	return "PiecewiseLinearTwist";
}

RoadwayTwist::TwistType PiecewiseLinearTwist_Imp::GetTwistType() const noexcept{
	return TwistType::PiecewiseLinear;
}

std::unique_ptr<RoadwayTwist> PiecewiseLinearTwist_Imp::Clone() const{
	return std::make_unique<PiecewiseLinearTwist_Imp>( *this );
}

Angle PiecewiseLinearTwist_Imp::DoTwist( Length s ) const{
	Data::const_iterator iA, iB;
	Bracket( s, iA, iB );
	return iA->second + (s - iA->first) / (iB->first - iA->first)  * (iB->second - iA->second);
}
///////////////////////////////////////
PiecewiseCircularTwist_Imp::PiecewiseCircularTwist_Imp( const RoadwayTwist& fromTwist )
	: PiecewiseTwist_Imp{ fromTwist }
{
}

const char* PiecewiseCircularTwist_Imp::TypeName() const noexcept{
	return "PiecewiseCircularTwist";
}

RoadwayTwist::TwistType PiecewiseCircularTwist_Imp::GetTwistType() const noexcept{
	return TwistType::PiecewiseCircular;
}

std::unique_ptr<RoadwayTwist> PiecewiseCircularTwist_Imp::Clone() const{
	return std::make_unique<PiecewiseCircularTwist_Imp>( *this );
}

Angle PiecewiseCircularTwist_Imp::DoTwist( Length s ) const
{
	Data::const_iterator iA, iB;
	Bracket( s, iA, iB );

	const One u = (s - iA->first) / (iB->first - iA->first);
	const Angle a = Wrap( iA->second, 0_rad, 2*pi );
	const Angle b = Wrap( iB->second, 0_rad, 2*pi );
	const Angle bma = b - a;
	if( abs(bma) <= pi )
		return a + u * bma;
	else if( bma > pi )
		return a + u * (bma - 2*pi);
	else
		return a + u * (2*pi + bma);
}

AnglePerLength PiecewiseCircularTwist_Imp::DoD1( Length s ) const
{
	Data::const_iterator iA, iB;
	Bracket( s, iA, iB );

	const Length Delta_s = (iB->first - iA->first);
	const Angle a = Wrap( iA->second, 0_rad, 2*pi );
	const Angle b = Wrap( iB->second, 0_rad, 2*pi );
	const Angle bma = b - a;
	if( abs(bma) <= pi )
		return bma / Delta_s;
	else if( bma > pi )
		return (bma - 2*pi) / Delta_s;
	else
		return (2*pi + bma) / Delta_s;
}
///////////////////////////////////////
std::unique_ptr<DirectionalTwist> trax::DirectionalTwist::Make( const spat::Vector<One>& attractor ) noexcept{
	try{
		return std::make_unique<DirectionalTwist_Imp>( attractor );
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

std::unique_ptr<DirectionalTwist> DirectionalTwist::Make( const RoadwayTwist& fromTwist ) noexcept{
	try{
		return std::make_unique<DirectionalTwist_Imp>( fromTwist );
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

DirectionalTwist_Imp::DirectionalTwist_Imp() noexcept
	:	DirectionalTwist_ImpBase<DirectionalTwist>{},
		GlobalA	{Up},
		A		{Up}
{
	RoadwayTwist_Imp<DirectionalTwist>::ZeroFlip( false );
}

DirectionalTwist_Imp::DirectionalTwist_Imp( const RoadwayTwist& /*fromTwist*/ )
	: DirectionalTwist_Imp()
{
}

DirectionalTwist_Imp::DirectionalTwist_Imp( const DirectionalTwist_Imp& twist )
	:	DirectionalTwist_ImpBase<DirectionalTwist>	{ twist }
	,	GlobalA										{ twist.GlobalA }
	,	A											{ twist.A }
	,	m_bFrozen									{ twist.m_bFrozen }
	,	m_bAttached									{ twist.m_bAttached }
{
}

DirectionalTwist_Imp::DirectionalTwist_Imp( const Vector<One>& attractor )
	:	DirectionalTwist_ImpBase<DirectionalTwist>{}
{
	if( attractor == Null<One> )
		throw std::invalid_argument( "Attractor vector can not be null." );

	GlobalA = attractor;
	GlobalA.Normalize();
	A = GlobalA;
}

void DirectionalTwist_Imp::Attractor( const Vector<One>& direction ){
	if( direction == Null<One> )
		throw std::invalid_argument( "Attractor vector can not be null." );
	if( m_bAttached )
		throw std::logic_error( "Can not set the attractor, while the twist is attached to a track." );

	GlobalA = direction;
	GlobalA.Normalize();
	A = GlobalA;
}

const spat::Vector<One>& DirectionalTwist_Imp::Attractor() const noexcept{
	return m_bFrozen ? A : GlobalA;
}

const spat::Vector<One>& DirectionalTwist_Imp::Attractor( bool bGlobal ) const noexcept{
	return bGlobal ? GlobalA : A;
}

const DirectionalTwist::Data& DirectionalTwist_Imp::GetData() const noexcept{
	return Attractor();
}

void DirectionalTwist_Imp::Create( const Data& data ){
	Attractor( data );
}

const char*	DirectionalTwist_Imp::TypeName() const noexcept{
	return "DirectionalTwist";
}

RoadwayTwist::TwistType DirectionalTwist_Imp::GetTwistType() const noexcept{
	return TwistType::Directional;
}

std::unique_ptr<RoadwayTwist> DirectionalTwist_Imp::Clone() const{
	return std::make_unique<DirectionalTwist_Imp>( *this );
}

bool DirectionalTwist_Imp::IsNull() const noexcept
{
	if( m_pCurve )
	{
		if( m_pCurve->IsFlat() ||
			m_pCurve->GetCurveType() == Curve::CurveType::Helix ||
			m_pCurve->GetCurveType() == Curve::CurveType::HelixP )
		{
			return OrthoParallel( A, m_pCurve->LocalUp() );
		};
	}

	return false;
}

bool DirectionalTwist_Imp::Equals( const RoadwayTwist & toTwist, Angle epsilon_angle ) const noexcept
{
	if( auto pOther = dynamic_cast<const DirectionalTwist*>(&toTwist) ){
		if( Attractor() == pOther->Attractor() )
			return true;

		return Attractor() * pOther->Attractor() >= cos( epsilon_angle );
	}

	return false;
}

void DirectionalTwist_Imp::OnAttach( const TrackBuilder& track ) noexcept{
	DirectionalTwist_ImpBase<DirectionalTwist>::OnAttach( track );

	if( m_bAttached )
	{
		A = GlobalA;
		track.GetFrame().FromParent( track.GetAbsoluteFrame().FromParent( A ) );
	}
	else{
		m_bAttached = true;

		if( m_bFrozen ){
			GlobalA = A;
			track.GetAbsoluteFrame().ToParent( track.GetFrame().ToParent( GlobalA ) );
		}
		else{
			A = GlobalA;
			track.GetFrame().FromParent( track.GetAbsoluteFrame().FromParent( A ) );
		}
	}
}

void DirectionalTwist_Imp::OnDetach() noexcept{
	if( m_bFrozen )
		GlobalA = A;
	else
		A = GlobalA;

	m_bAttached = false;
	DirectionalTwist_ImpBase<DirectionalTwist>::OnDetach();
}

Angle DirectionalTwist_Imp::DoTwist( Length s ) const{
	return DirectionalTwist_ImpBase<DirectionalTwist>::DoTwist_( s, A );
}

AnglePerLength DirectionalTwist_Imp::DoD1( Length s ) const{
	return DirectionalTwist_ImpBase<DirectionalTwist>::DoD1_( s, A );
}
///////////////////////////////////////
std::unique_ptr<PiecewiseDirectionalTwist> PiecewiseDirectionalTwist::Make() noexcept
{
	try{
		return std::make_unique<PiecewiseDirectionalTwist_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

std::unique_ptr<PiecewiseDirectionalTwist> PiecewiseDirectionalTwist::Make( const RoadwayTwist& fromTwist ) noexcept
{
	try{
		return std::make_unique<PiecewiseDirectionalTwist_Imp>( fromTwist );
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

PiecewiseDirectionalTwist_Imp::PiecewiseDirectionalTwist_Imp( const RoadwayTwist& /*fromTwist*/ )
	: PiecewiseDirectionalTwist_Imp{}
{
}

const char*	PiecewiseDirectionalTwist_Imp::TypeName() const noexcept{
	return "PiecewiseDirectionalTwist";
}

RoadwayTwist::TwistType PiecewiseDirectionalTwist_Imp::GetTwistType() const noexcept{
	return TwistType::PiecewiseDirectional;
}

std::unique_ptr<RoadwayTwist> PiecewiseDirectionalTwist_Imp::Clone() const{
	return std::make_unique<PiecewiseDirectionalTwist_Imp>( *this );
}

bool PiecewiseDirectionalTwist_Imp::Equals( const RoadwayTwist& toTwist, Angle epsilon_angle ) const noexcept
{
	if( auto pOther = dynamic_cast<const PiecewiseDirectionalTwist_Imp*>(&toTwist) )
	{
		return std::equal(	m_Data.begin(), m_Data.end(),
							pOther->m_Data.begin(), pOther->m_Data.end(),
							[epsilon_angle]( const Data::value_type& pairA, const Data::value_type& pairB )
		{ 
			return common::Equals( pairA.first, pairB.first, epsilon__length ) &&
				   spat::Equals( pairA.second, pairB.second, epsilon_angle );
		} );
	}

	return false;
}
Angle PiecewiseDirectionalTwist_Imp::DoTwist( Length s ) const
{
	Data::const_iterator iA, iB;
	Bracket( s, iA, iB );
	const spat::Vector<One> A = spat::SlerpNormals( iA->second, iB->second, (s - iA->first) / (iB->first - iA->first) );
	
	return DoTwist_( s, A );
}

AnglePerLength PiecewiseDirectionalTwist_Imp::DoD1( Length s ) const
// a = -arctan(AN/AB)
// 
// D1 = da/ds	= -1 / (1 + pow<2>(AN/AB)) * d/ds(AN/AB)
//				= -1 / (1 + pow<2>(AN/AB)) * (A'N/AB + A*N'/AB - AN/ABAB * (A'B + AB') )
//				= -1 / (ANAN + ABAB) * (AN'*AB - AN*AB' + A'N*AB - AN*A'B)
//				= -1 / (ANAN + ABAB) * (A*(-kT + tB)*AB - AN*A*(-tN) + A'*(AB*N - AN*B) )
//				= -1 / (ANAN + ABAB) * (A*(-kT)*AB + A'*(AB*N - AN*B) ) - t
//				= (k*ATAB + A'*(AN*B - AB*N)) / (ANAN + ABAB) - t
{
	if( m_pCurve == nullptr )
		return 0_1Im;

	Data::const_iterator iA, iB;
	Bracket( s, iA, iB );
	const auto pair = spat::SlerpDNormals( iA->second, iB->second, (s - iA->first) / (iB->first - iA->first) );
	const spat::Vector<One>& A = pair.first;
	const spat::Vector<Value<Dimension<-1, 0, 0>>> dA = pair.second  / (iB->first - iA->first);

	spat::Frame<Length,One> F;
	m_pCurve->Transition( c(s), F );

	if( !m_CurveSegment.Normal() ){
		F.T *= -1;
		F.B *= -1;
	}

	const spat::Vector<One>& T = F.T;
	const spat::Vector<One>& N = F.N;
	const spat::Vector<One>& B = F.B;
	const One AT = A*T;
	const One AN = A*N;
	const One AB = A*B;
	const AnglePerLength k = m_pCurve->Curvature(c(s));
	const AnglePerLength t = m_pCurve->Torsion(c(s));

	if( AN || AB )
		return (k*AT*AB + dA*(AN*B - AB*N)) / (AN*AN + AB*AB) - Turn() * t;
	else
		return -Turn() * t;
}

void PiecewiseDirectionalTwist_Imp::Add( Length s, const spat::Vector<One>& attractor ){
	DirectionalTwist_ImpBase<PiecewiseTwist_ImpBase<PiecewiseDirectionalTwist>>::Add( s, Normalize(attractor).second );
}

void PiecewiseDirectionalTwist_Imp::Create( const Data& data ){
	for( const auto& pair: data )
	{
		if( !pair.second.IsNormal() )
			throw std::invalid_argument( TypeName() + std::string{ " invalid data: some vectors are not normalized!" } );
	}

	DirectionalTwist_ImpBase<PiecewiseTwist_ImpBase<PiecewiseDirectionalTwist>>::Create( data );
}
///////////////////////////////////////
std::unique_ptr<CombinedTwist> CombinedTwist::Make() noexcept{
	try{
		return std::make_unique<CombinedTwist_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

std::unique_ptr<CombinedTwist> CombinedTwist::Make( const RoadwayTwist& fromTwist ) noexcept
{
	try{
		return std::make_unique<CombinedTwist_Imp>( fromTwist );
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

CombinedTwist_Imp::CombinedTwist_Imp()
	:	m_pTwist1{ std::make_unique<ConstantTwist_Imp>() },
		m_pTwist2{ std::make_unique<ConstantTwist_Imp>() }
{
	m_pTwist1->ZeroFlip( false );
	m_pTwist2->ZeroFlip( false );
}

CombinedTwist_Imp::CombinedTwist_Imp( const RoadwayTwist& /*fromTwist*/ )
	: CombinedTwist_Imp()
{
}

CombinedTwist_Imp::CombinedTwist_Imp( const CombinedTwist_Imp& twist )
	:	RoadwayTwist_Imp<CombinedTwist>{twist},
		m_pTwist1{ twist.Twist1().Clone() },
		m_pTwist2{ twist.Twist2().Clone() }
{
}

const char*	CombinedTwist_Imp::TypeName() const noexcept{
	return "CombinedTwist";
}

RoadwayTwist::TwistType CombinedTwist_Imp::GetTwistType() const noexcept{
	return TwistType::Combined;
}

std::unique_ptr<RoadwayTwist> CombinedTwist_Imp::Clone() const{
	return std::make_unique<CombinedTwist_Imp>( *this );
}

bool CombinedTwist_Imp::IsValid() const noexcept{
	return	RoadwayTwist_Imp<CombinedTwist>::IsValid() &&
			m_pTwist1 && m_pTwist1->IsValid() && 
			m_pTwist2 && m_pTwist2->IsValid();
}

void CombinedTwist_Imp::Flip() noexcept{
	RoadwayTwist_Imp<CombinedTwist>::Flip();

	Offset( pi );	// on a flip static twists add pi. 
					// (pi + pi + pi == pi). Dynamic 
					// twist do nothing but do not need pi, 
					// pi + pi == 0,

	m_pTwist1->Flip();
	m_pTwist2->Flip();
}

void CombinedTwist_Imp::Mirror() noexcept{
	m_pTwist1->Mirror();
	m_pTwist2->Mirror();
}

bool CombinedTwist_Imp::Offset( Angle offset ) noexcept{
	if( m_pTwist1->Offset( offset ) )
		return true;

	return m_pTwist2->Offset( offset );
}

void CombinedTwist_Imp::Freeze( bool bFreeze ) noexcept{
	m_pTwist1->Freeze(bFreeze);
	m_pTwist2->Freeze(bFreeze);
}

bool CombinedTwist_Imp::IsDynamic() const noexcept{
	return m_pTwist1->IsDynamic() || m_pTwist2->IsDynamic();
}

bool CombinedTwist_Imp::IsDirectionalConstant() const noexcept{
	return (m_pTwist1->IsDirectionalConstant() && m_pTwist2->IsConstant()) ||
		   (m_pTwist2->IsDirectionalConstant() && m_pTwist1->IsConstant());
}

bool CombinedTwist_Imp::IsConstant() const noexcept{
	return m_pTwist1->IsConstant() && m_pTwist2->IsConstant();
}

bool CombinedTwist_Imp::IsNull() const noexcept{
	return IsConstant() && Twist( 0_m ) == 0;
}

bool CombinedTwist_Imp::IsFrozen() const noexcept{
	if( m_pTwist1->IsFrozen() && m_pTwist2->IsFrozen() )
		return true;

	return false;
}

bool CombinedTwist_Imp::Equals( const RoadwayTwist& toTwist, Angle epsilon_angle ) const noexcept{
	if( auto pOther = dynamic_cast<const CombinedTwist*>(&toTwist) )
		return (Twist1().Equals( pOther->Twist1(), epsilon_angle ) &&
				Twist2().Equals( pOther->Twist2(), epsilon_angle ) ) ||
				(Twist1().Equals( pOther->Twist2(), epsilon_angle ) &&
				Twist2().Equals( pOther->Twist1(), epsilon_angle ) );

	return false;
}

void CombinedTwist_Imp::OnAttach( const TrackBuilder& track ) noexcept{
	RoadwayTwist_Imp<CombinedTwist>::OnAttach( track );
	m_pTwist1->OnAttach( track );
	m_pTwist2->OnAttach( track );
}

void CombinedTwist_Imp::OnDetach() noexcept{
	m_pTwist1->OnDetach();
	m_pTwist2->OnDetach();
	RoadwayTwist_Imp<CombinedTwist>::OnDetach();
}

Angle CombinedTwist_Imp::DoTwist( Length s ) const{
	return m_pTwist1->DoTwist(s) + m_pTwist2->DoTwist(s); 
}

AnglePerLength CombinedTwist_Imp::DoD1( Length s ) const{
	return m_pTwist1->DoD1(s) + m_pTwist2->DoD1(s);
}

void CombinedTwist_Imp::AttachTwist1( std::unique_ptr<RoadwayTwist> pTwist ){
	if( !pTwist )
		throw std::invalid_argument( "Detach a twist propperly from a CombinedTwist!" );
	if( TrackLength() != 0_m )
		throw std::logic_error( "Can not attach a twist to a combined twist, while it is attached to a track. Detach combined twist first." );

	if( pTwist->IsDynamic() ){
		if( m_pTwist2 && m_pTwist2->IsDynamic() )
			throw std::invalid_argument( "Can only have one dynamic twist with a combined twist" );

		ZeroFlip( false ); // would add pi as a wrong offset after zero point
	}

	m_pTwist1 = std::move(pTwist);
	m_pTwist1->ZeroFlip( false );
}

RoadwayTwist& CombinedTwist_Imp::Twist1() const noexcept{
	return *m_pTwist1;
}

std::unique_ptr<RoadwayTwist> CombinedTwist_Imp::DetachTwist1()
{
	std::unique_ptr<RoadwayTwist> pRetval{ std::move(m_pTwist1) };
	m_pTwist1 = std::make_unique<ConstantTwist_Imp>();
	m_pTwist1->ZeroFlip( false );
	pRetval->OnDetach();
	return pRetval;
}

void CombinedTwist_Imp::AttachTwist2( std::unique_ptr<RoadwayTwist> pTwist ){
	if( !pTwist )
		throw std::invalid_argument( "Detach a twist propperly from a CombinedTwist!" );
	if( TrackLength() != 0_m )
		throw std::logic_error( "Can not attach a twist to a combined twist, while it is attached to a track. Detach combined twist first." );

	if( pTwist->IsDynamic() ){
		if( m_pTwist1 && m_pTwist1->IsDynamic() )
			throw std::invalid_argument( "Can only have one dynamic twist with a combined twist" );

		ZeroFlip( false ); // would add pi as a wrong offset after zero point
	}

	m_pTwist2 = std::move(pTwist);
	m_pTwist2->ZeroFlip( false );
}

RoadwayTwist& CombinedTwist_Imp::Twist2() const noexcept{
	return *m_pTwist2;
}

std::unique_ptr<RoadwayTwist> CombinedTwist_Imp::DetachTwist2()
{
	std::unique_ptr<RoadwayTwist> pRetval{ std::move(m_pTwist2) };
	m_pTwist2 = std::make_unique<ConstantTwist_Imp>();
	m_pTwist2->ZeroFlip( false );
	pRetval->OnDetach();
	return pRetval;
}
///////////////////////////////////////
template const ConstantTwist* FindTwist<ConstantTwist>( const RoadwayTwist& twist ) noexcept;
template const LinearTwist* FindTwist<LinearTwist>( const RoadwayTwist& twist ) noexcept;
template const PiecewiseTwist* FindTwist<PiecewiseTwist>( const RoadwayTwist& twist ) noexcept;
template const DirectionalTwist* FindTwist<DirectionalTwist>( const RoadwayTwist& twist ) noexcept;
template const CombinedTwist* FindTwist<CombinedTwist>( const RoadwayTwist& twist ) noexcept;

template ConstantTwist* FindTwist<ConstantTwist>( RoadwayTwist& twist ) noexcept;
template LinearTwist* FindTwist<LinearTwist>( RoadwayTwist& twist ) noexcept;
template PiecewiseTwist* FindTwist<PiecewiseTwist>( RoadwayTwist& twist ) noexcept;
template DirectionalTwist* FindTwist<DirectionalTwist>( RoadwayTwist& twist ) noexcept;
template CombinedTwist* FindTwist<CombinedTwist>( RoadwayTwist& twist ) noexcept;
///////////////////////////////////////
//std::unique_ptr<RoadwayTwist> CreateTwistFrom( RoadwayTwist::TwistType type, const RoadwayTwist& twist )
//{
//	switch( type )
//	{
//		case RoadwayTwist::TwistType::Zero:
//			return CreateZeroFrom( twist );
//			break;
//		case RoadwayTwist::TwistType::Constant:
//			return CreateConstantFrom( twist );
//			break;
//		case RoadwayTwist::TwistType::Linear:
//			return CreateLinearFrom( twist );
//			break;
//		case RoadwayTwist::TwistType::Piecewise:
//			return CreatePiecewiseFrom( twist );
//			break;
//		case RoadwayTwist::TwistType::PiecewiseLinear:
//			return CreatePiecewiseLinearFrom( twist );
//			break;
//		case RoadwayTwist::TwistType::PiecewiseCircular:
//			return CreatePiecewiseCircularFrom( twist );
//			break;
//		case RoadwayTwist::TwistType::PiecewiseDirectional:
//			return CreatePiecewiseDirectionalFrom( twist );
//			break;
//		case RoadwayTwist::TwistType::Directional:
//			return CreateDirectionalFrom( twist );
//			break;
//		default:
//			return nullptr;
//	}
//}
///////////////////////////////////////
}

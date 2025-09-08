//	trax track library
//	AD 2019 
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

#include "PolygonalChain_Imp.h"

#include "common/NarrowCast.h"
#include "spat/support/SpatSupportStream.h"

namespace trax{
///////////////////////////////////////
	/*
Length PolygonalChain_Imp::GetParameter( int idx ) const{		
	return m_Vertices.at(idx).s;
}

int PolygonalChain_Imp::UpperBound( Length s ) const{
	return common::narrow_cast<int>(std::upper_bound( m_Vertices.cbegin(), m_Vertices.cend(), Vertex{ {}, s, 0_1Icm, 0_1Icm } ) - m_Vertices.begin());
}

common::Interval<Length> PolygonalChain_Imp::Create(
	const Curve& originalCurve, 
	common::Interval<Length> range, 
	Length maxDeviation, 
	Length minPointDistance )
{
	if( !originalCurve.IsValid() )
		throw std::invalid_argument( "PolygonalChain_Imp::Create: no valid curve to create from." );

	range.Intersection( originalCurve.Range() );
	range.Normalize();
	if( range.Length() < epsilon__length )
		throw std::invalid_argument( "PolygonalChain_Imp::Create: zero length curve." );

	if( maxDeviation <= 0_m )
		throw std::invalid_argument( "PolygonalChain_Imp::Create: invalid parameter maxDeviation." );

	if( minPointDistance <= 0_m )
		throw std::invalid_argument( "PolygonalChain_Imp::Create: invalid parameter minPointDistance." );


	m_Data.clear();
	m_Data.reserve( common::narrow_cast<size_t>(std::ceil( abs(range.Length()) / minPointDistance )) );

	common::Sample( range, [ this, &originalCurve, minPointDistance ]( Length s )
	{
		spat::Position<Length> pos;
		originalCurve.Transition( s, pos );
		m_Data.push_back( pos );
		return minPointDistance;
	} );

	DouglasPeucker( m_Data, m_Data.begin(), m_Data.end() - 1, maxDeviation, []( Data::iterator i ) -> const spat::Position<Length>& { return *i; } );

	return Create( m_Data );
}

common::Interval<Length> PolygonalChain_Imp::Create( 
	std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> originalCurve,
	Length maxDeviation,
	Length minPointDistance )
{
	return Create( *originalCurve.first, originalCurve.second, maxDeviation, minPointDistance );
}

common::Interval<Length> PolygonalChain_Imp::Create( const Data& data )
{	
	Data purifiedData;
	purifiedData.reserve( data.size() );

	for( const auto& vertex : data )
	{
		if( purifiedData.empty() || !purifiedData.back().Equals( vertex, epsilon__length ) )
			purifiedData.push_back( vertex );
	};

	if( purifiedData.size() < 2 )
		throw std::invalid_argument( "PolygonalChain_Imp::Create: not enough consecutive non-collocal vertex points for PolygonalChain" );

	m_Data = purifiedData;
	m_Vertices.clear();
	m_Vertices.reserve( m_Data.size() );

	for( const auto& position : m_Data )
		m_Vertices.push_back( { {position,{},{},{}},{},{},{}} );

	Length s = 0_m;
	for( auto iter = m_Vertices.begin(); iter != m_Vertices.end() - 1; ++iter ){
		const auto pair = spat::Normalize( (iter+1)->F.P - (iter)->F.P );
		(iter)->F.T = pair.second;
		(iter)->s = s;
		s += pair.first;
	}
	m_Vertices.back().s = s;

	if( m_Vertices.size() > 2 ){
		m_Vertices.back().F.T = (m_Vertices.end() - 2)->F.T + ((m_Vertices.end() - 2)->F.T - (m_Vertices.end() - 3)->F.T);
		m_Vertices.back().F.T.Normalize();
	}
	else
		m_Vertices.back().F.T = (m_Vertices.end() - 2)->F.T;

	for( auto iter = m_Vertices.begin(); iter != m_Vertices.end() - 1; ++iter ){
		const auto pair = Normalize( ((iter+1)->F.T - iter->F.T) / ((iter+1)->s - iter->s) );
		iter->k = pair.first;

		Frame<Length,One>& F = iter->F;
		F.N = pair.second;
		F.N -= (F.T*F.N)*F.T;
		if( F.N.Equals( spat::Null<One>, epsilon ) ){
			F.N = F.T % Ez<One>;
			F.N -= (F.T*F.N)*F.T;
		}
		F.N.Normalize();

		F.B = F.T % F.N;
	}
	m_Vertices.back().F.N = (m_Vertices.end() - 2)->F.N;
	m_Vertices.back().F.B = (m_Vertices.end() - 2)->F.B;
	m_Vertices.back().k = (m_Vertices.end() - 2)->k;

	for( auto iter = m_Vertices.begin(); iter != m_Vertices.end() - 1; ++iter )
		iter->t = (iter->F.B - (iter+1)->F.B) * iter->F.N / ((iter+1)->s - iter->s);
	m_Vertices.back().t = (m_Vertices.end() - 2)->t;

	return { 0_m, s };
}

const PolygonalChain::Data& PolygonalChain_Imp::GetData() const noexcept{
	return m_Data;
}

bool PolygonalChain_Imp::IsValid() const noexcept{
	return !m_Vertices.empty();
}

AnglePerLength PolygonalChain_Imp::Curvature( Length s ) const{
	return GetSegmentStart( s )->k;
}

AnglePerLength PolygonalChain_Imp::Torsion( Length s ) const{
	return GetSegmentStart( s )->t;
}

bool PolygonalChain_Imp::IsFlat() const noexcept
{
	for( auto iter = m_Vertices.begin(); iter != m_Vertices.end(); ++iter ){
		if( iter->t != 0_1Im )
			return false;
	}

	return true;
}

void PolygonalChain_Imp::Transition( Length s, spat::Position<Length>& pos ) const{
	const std::vector<Vertex>::const_iterator start = GetSegmentStart( s );
	pos = start->F.P + (s - start->s) * start->F.T;
}

void PolygonalChain_Imp::Transition( Length s, spat::Vector<One>& tan ) const{
	tan = GetSegmentStart( s )->F.T;
}

void PolygonalChain_Imp::Transition( Length s, spat::VectorBundle<Length,One>& bundle ) const
{
	const std::vector<Vertex>::const_iterator start = GetSegmentStart( s );
	bundle.P = start->F.P + (s - start->s) * start->F.T;
	bundle.T = start->F.T;
}

void PolygonalChain_Imp::Transition( Length s, spat::VectorBundle2<Length,One>& bundle ) const
{
	const std::vector<Vertex>::const_iterator start = GetSegmentStart( s );
	bundle.P = start->F.P + (s - start->s) * start->F.T;
	bundle.T = start->F.T;
	bundle.N = start->F.N;
}

void PolygonalChain_Imp::Transition( Length s, spat::Frame<Length,One>& frame ) const
{
	const std::vector<Vertex>::const_iterator start = GetSegmentStart( s );
	frame.P = start->F.P + (s - start->s) * start->F.T;
	frame.T = start->F.T;
	frame.N = start->F.N;
	frame.B = start->F.B;

//	if( start+1 == m_Vertices.end() ){
	//}
	//else{
	//	const Real u = (s - start->s) /  ((start+1)->s - start->s);

	//	Transformation<Real> out;
	//	Slerp( out, Transformation<Real>{ start->F }, Transformation<Real>{ (start+1)->F }, u );
	//	frame = out;

	//	//frame.T = (1-u) * start->F.T + u * (start+1)->F.T;
	//	//frame.N = (1-u) * start->F.N + u * (start+1)->F.N;
	//	//frame.B = (1-u) * start->F.B + u * (start+1)->F.B;
	//}
}

std::vector<Length> PolygonalChain_Imp::ZeroSet() const
{
	std::vector<Length> zeroSet;
	for( auto iter = m_Vertices.begin() + 1; iter < m_Vertices.end(); ++iter )
		if( (iter-1)->F.N * iter->F.N < 0 )
			zeroSet.push_back( iter->s );

	return zeroSet;
}

common::Interval<Length> PolygonalChain_Imp::Range() const{
	return {0_m,m_Vertices.back().s};
}

spat::Vector<One> PolygonalChain_Imp::LocalUp() const noexcept
{
	if( !IsFlat() )
		return {0,0,0};

	for( auto iter = m_Vertices.begin() + 1; iter != m_Vertices.end(); ++iter ){
		if( !m_Vertices.front().F.T.Equals( iter->F.T, One{epsilon} ) )
		{
			return spat::Normalize( m_Vertices.front().F.T % iter->F.T ).second;
		}
	}

	return {0,0,0};
}

std::unique_ptr<Curve> PolygonalChain_Imp::Clone() const{
	return std::make_unique<PolygonalChain_Imp>( *this );
}

bool PolygonalChain_Imp::Mirror( const spat::VectorBundle<Length,One>& mirrorPlane ) noexcept
{
	for( Vertex& vertex : m_Vertices ){
		vertex.F.P -= 2 * ((vertex.F.P - mirrorPlane.P) * mirrorPlane.T) * mirrorPlane.T;
		vertex.F.T -= 2 * (vertex.F.T * mirrorPlane.T) * mirrorPlane.T;
		vertex.F.N -= 2 * (vertex.F.N * mirrorPlane.T) * mirrorPlane.T;
		vertex.F.B = vertex.F.T % vertex.F.N;
		vertex.t *= -1;
	}

	std::copy( m_Vertices.begin(), m_Vertices.end(), m_Data.begin() );

	return true;
}

bool PolygonalChain_Imp::Equals( 
	const Curve& toCurve, 
	common::Interval<Length> range,
	Length epsilon_length,
	Angle epsilon_angle ) const
{
	if( this == &toCurve )
		return true;

	if( auto pToCurve = dynamic_cast<const PolygonalChain*>(&toCurve) )
		return trax::Equals( GetData(), pToCurve->GetData(), range, epsilon_length, epsilon_angle );

	return false;
}
*/
//bool Equals( 
//	const PolygonalChain::Data& A, 
//	const PolygonalChain::Data& B, 
//	common::Interval<Length> /*range*/, 
//	Length epsilon_length, 
//	Angle /*epsilon_angle*/ )
//{	
//	return std::equal(	A.begin(), A.end(),
//						B.begin(), B.end(),
//						[epsilon_length]( const PolygonalChain::Data::value_type& A, const PolygonalChain::Data::value_type& B )
//	{ 
//		return A.Equals( B, epsilon_length );
//	} );
//}
	
///////////////////////////////////////
std::unique_ptr<PolygonalChain> PolygonalChain::Make() noexcept{
	try{
		return std::make_unique<PolygonalChain_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
Length PolygonalChain_Imp::GetParameter( int idx ) const
{
	if( idx < 0 )
		throw std::out_of_range( "PolygonalChain_Imp::GetParameter: idx can not be < 0 " );

	return m_DataSamplesParameters.at( idx );
}

int PolygonalChain_Imp::UpperBound( Length s ) const{
	return common::narrow_cast<int>( 
				std::upper_bound( 
					m_DataSamplesParameters.cbegin(), 
					m_DataSamplesParameters.cend(), s ) - m_DataSamplesParameters.cbegin() );
}

common::Interval<Length> PolygonalChain_Imp::Create(
	const Curve& originalCurve, 
	common::Interval<Length> range, 
	Length maxDeviation, 
	Length minPointDistance )
{
	if( !originalCurve.IsValid() )
		throw std::invalid_argument( "PolygonalChain_Imp::Create(): no valid curve to create from." );

	range.Intersection( originalCurve.Range() );
	range.Normalize();
	if( range.Length() < epsilon__length )
		throw std::invalid_argument( "PolygonalChain_Imp::Create(): zero length curve." );

	if( maxDeviation <= 0_m )
		throw std::invalid_argument( "PolygonalChain_Imp::Create(): invalid parameter maxDeviation." );

	if( minPointDistance <= 0_m )
		throw std::invalid_argument( "PolygonalChain_Imp::Create(): invalid parameter minPointDistance." );


	m_Data.clear();
	m_Data.reserve( common::narrow_cast<size_t>(std::ceil( abs(range.Length()) / minPointDistance )) );

	common::Sample( range, [ this, &originalCurve, minPointDistance ]( Length s )
	{
		VectorBundle<Length,One> bundle;
		originalCurve.Transition( s, bundle );
		m_Data.push_back( bundle );
		return minPointDistance;
	} );

	DouglasPeucker( m_Data, m_Data.begin(), m_Data.end() - 1, maxDeviation, []( Data::iterator i ) -> const spat::Position<Length>& { return i->P; } );

	return Create( m_Data );
}

common::Interval<Length> PolygonalChain_Imp::Create( 
	std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> originalCurve,
	Length maxDeviation,
	Length minPointDistance )
{
	return Create( *originalCurve.first, originalCurve.second, maxDeviation, minPointDistance );
}

common::Interval<Length> PolygonalChain_Imp::Create( 
	const std::vector<spat::Position<Length>>& samples, 
	WrapTypes wrap )
{
	if( samples.size() < 2 )
		throw std::invalid_argument( "PolygonalChain_Imp::Create(): not enough sample points for PolygonalChain" );

	Data data;
	data.reserve( samples.size() + 1 );

	for( auto iter = samples.begin(); iter != samples.end()-1; ++iter )
	{
		data.push_back( { *iter, Normalize(*(iter+1) - *iter).second } );
	}

	if( wrap == WrapTypes::periodic ){
		if( !samples.front().Equals( samples.back(), epsilon__length ) )
			data.push_back( { samples.back(), Normalize( samples.front() - samples.back() ).second } );
		
		data.push_back( data.front() );
	}
	else
		data.push_back( { samples.back(), data.back().T } );

	return Create( data );
}

common::Interval<Length> PolygonalChain_Imp::CreateCatmullRom( 
	const std::vector<spat::Position<Length>>& samples, 
	WrapTypes wrap )
{
	if( samples.size() < 2 )
		throw std::invalid_argument( "PolygonalChain_Imp::CreateCatmullRom(): not enough sample points for PolygonalChain" );

	Data data;
	data.reserve( samples.size() + 1 );

	data.push_back( { samples.front(), Normalize(samples.at(1) - samples.at(0)).second } );
	for( auto iter = samples.begin()+1; iter != samples.end()-1; ++iter )
	{
		data.push_back( { *iter, Normalize(*(iter+1) - *(iter-1)).second } );
	}

	if( wrap == WrapTypes::periodic ){
		if( !samples.front().Equals( samples.back(), epsilon__length ) ){
			data.front().T = Normalize( samples.at(1) - samples.back() ).second;
			data.push_back( { samples.back(), Normalize( samples.front() - *(samples.end()-2) ).second } );
		}
		else{
			data.front().T = Normalize( samples.at(1) - *(samples.end()-2) ).second;
		}
		
		data.push_back( data.front() );
	}
	else
		data.push_back( { samples.back(), data.back().T } );

	return Create( data );
}

common::Interval<Length> PolygonalChain_Imp::Create( const Data& data )
{
	Data purifiedData;
	purifiedData.reserve( data.size() );

	for( const auto& vertex : data )
	{
		if( purifiedData.empty() || !purifiedData.back().P.Equals( vertex.P, epsilon__length ) ){
			if( !vertex.T.IsNormal( 10*epsilon ) ){
				std::ostringstream stream;
				stream << "PolygonalChain_Imp::Create(): A tangent is not normalized! " << vertex << std::endl;
				throw std::invalid_argument( stream.str() );
			}

			purifiedData.push_back( vertex );
		}
	};

	if( purifiedData.size() < 2 )
		throw std::invalid_argument( "PolygonalChain_Imp::Create: not enough consecutive non-collocal vertex points for PolygonalChain" );

	m_Data = purifiedData;
	m_Vertices.clear();
	m_Vertices.reserve( m_Data.size() );

	for( const auto& bundle : m_Data )
		m_Vertices.push_back( { {bundle.P,bundle.T,{},{}},{},{},{}} );

	Length s = 0_m;
	for( auto iter = m_Vertices.begin(); iter != m_Vertices.end() - 1; ++iter )
	{
		(iter)->s = s;
		s += ((iter+1)->F.P - (iter)->F.P).Length();
	}
	m_Vertices.back().s = s;

	for( auto iter = m_Vertices.begin(); iter != m_Vertices.end() - 1; ++iter ){
		const auto pair = Normalize( ((iter+1)->F.T - iter->F.T) / ((iter+1)->s - iter->s) );
		iter->k = pair.first;
		iter->F.N = pair.second;
		iter->F.OrthoNormalize();
	}

	m_Vertices.back().k = (m_Vertices.end() - 2)->k;
	m_Vertices.back().F.N = (m_Vertices.end() - 2)->F.N;
	m_Vertices.back().F.OrthoNormalize();

	for( auto iter = m_Vertices.begin(); iter != m_Vertices.end() - 1; ++iter )
		iter->t = (iter->F.B - (iter+1)->F.B) * iter->F.N / ((iter+1)->s - iter->s);
	m_Vertices.back().t = (m_Vertices.end() - 2)->t;

	m_DataSamplesParameters.clear();
	m_DataSamplesParameters.reserve( m_Vertices.size() );
	for( const auto& vertex : m_Vertices )
		m_DataSamplesParameters.push_back( vertex.s );

	// For interpolation we need to protect the zero points by another closeby
	// vertex, to not have the section before the zero point interpolating the flip:
	m_Vertices.reserve( m_Vertices.size() + ZeroSet().size() );
	for( auto iter = m_Vertices.begin() + 1; iter < m_Vertices.end(); ++iter )
	{
		if( (iter-1)->F.N * iter->F.N < 0 ){
			iter = m_Vertices.insert( iter, *iter );		
			iter->F.T = (iter-1)->F.T;
			iter->F.N = (iter-1)->F.N;
			iter->F.B = (iter-1)->F.B;
			++iter;
		}
	}

	return { 0_m, s };
}

//common::Interval<Length> PolygonalChain_Imp::Create( const Data& data )
//{
//	Data purifiedData;
//	purifiedData.reserve( data.size() );
//
//	for( const auto& vertex : data )
//	{
//		if( purifiedData.empty() || !purifiedData.back().P.Equals( vertex.P, epsilon__length ) ){
//			if( !vertex.T.IsNormal() )
//				throw std::invalid_argument( "PolygonalChain_Imp::Create(): A tangent is not normalized!" );
//
//			purifiedData.push_back( vertex );
//		}
//	};
//
//	if( purifiedData.size() < 2 )
//		throw std::invalid_argument( "PolygonalChain_Imp::Create: not enough consecutive non-collocal vertex points for PolygonalChain" );
//
//	m_Data = purifiedData;
//	m_Vertices.clear();
//	m_Vertices.reserve( m_Data.size() );
//
//	for( const auto& bundle : m_Data )
//		m_Vertices.push_back( { {bundle.P,bundle.T,{},{}},{},{},{}} );
//
//	Length s = 0_m;
//	for( auto iter = m_Vertices.begin(); iter != m_Vertices.end() - 1; ++iter )
//	{
//		(iter)->s = s;
//		s += ((iter+1)->F.P - (iter)->F.P).Length();
//	}
//	m_Vertices.back().s = s;
//
//	for( auto iter = m_Vertices.begin(); iter != m_Vertices.end() - 1; ++iter ){
//		const auto pair = Normalize( ((iter+1)->F.T - iter->F.T) / ((iter+1)->s - iter->s) );
//		if( pair.first < 0.001_1Im )//10000*epsilon__curvature )
//		{
//			iter->k = 0_1Im;
//			iter->F.N = Up % iter->F.T;
//		}
//		else
//		{
//			iter->k = pair.first;
//			iter->F.N = pair.second;
//		}
//		iter->F.OrthoNormalize();
//	}
//
//	m_Vertices.back().k = (m_Vertices.end() - 2)->k;
//	m_Vertices.back().F.N = (m_Vertices.end() - 2)->F.N;
//	m_Vertices.back().F.OrthoNormalize();
//
//	for( auto iter = m_Vertices.begin(); iter != m_Vertices.end() - 1; ++iter )
//		iter->t = (iter->F.B - (iter+1)->F.B) * iter->F.N / ((iter+1)->s - iter->s);
//	m_Vertices.back().t = (m_Vertices.end() - 2)->t;
//
//	m_DataSamplesParameters.clear();
//	m_DataSamplesParameters.reserve( m_Vertices.size() );
//	for( const auto& vertex : m_Vertices )
//		m_DataSamplesParameters.push_back( vertex.s );
//
//	// For interpolation we need to protect the zero points by another closeby
//	// vertex, to not have the section before the zero point interpolating the flip:
//	m_Vertices.reserve( m_Vertices.size() + ZeroSet().size() );
//	for( auto iter = m_Vertices.begin() + 1; iter < m_Vertices.end(); ++iter )
//	{
//		if( (iter-1)->F.N * iter->F.N < 0 ){
//			iter = m_Vertices.insert( iter, *iter );		
//			iter->F.T = (iter-1)->F.T;
//			iter->F.N = (iter-1)->F.N;
//			iter->F.B = (iter-1)->F.B;
//			++iter;
//		}
//	}
//
//	return { 0_m, s };
//}

const PolygonalChain::Data& PolygonalChain_Imp::GetData() const noexcept{
	return m_Data;
}

bool PolygonalChain_Imp::IsValid() const noexcept{
	return !m_Vertices.empty();
}

AnglePerLength PolygonalChain_Imp::Curvature( Length s ) const{
	const std::vector<CurveSample>::const_iterator start = GetSegmentStart( s );

	if( start+1 == m_Vertices.end() ){
		return start->k;
	}
	else
	{
		const One u = (s - start->s) / ((start+1)->s - start->s);
		return start->k + u * ((start+1)->k - start->k);
	}
}

AnglePerLength PolygonalChain_Imp::Torsion( Length s ) const{
	const std::vector<CurveSample>::const_iterator start = GetSegmentStart( s );

	if( start+1 == m_Vertices.end() ){
		return start->t;
	}
	else
	{
		const One u = (s - start->s) / ((start+1)->s - start->s);
		return start->t + u * ((start+1)->t - start->t);
	}
}

bool PolygonalChain_Imp::IsFlat() const noexcept
{
	for( auto iter = m_Vertices.begin(); iter != m_Vertices.end(); ++iter ){
		if( iter->t != 0_1Im )
			return false;
	}

	return true;
}

void PolygonalChain_Imp::Transition( Length s, spat::Position<Length>& pos ) const
{
	const std::vector<CurveSample>::const_iterator start = GetSegmentStart( s );

	if( start+1 == m_Vertices.end() ){
		pos = start->F.P;
	}
	else
	{
		const One u = (s - start->s) / ((start+1)->s - start->s);
		pos = start->F.P + u * ((start+1)->F.P - start->F.P);
	}
}

void PolygonalChain_Imp::Transition( Length s, spat::Vector<One>& tan ) const
{
	const std::vector<CurveSample>::const_iterator start = GetSegmentStart( s );

	if( start+1 == m_Vertices.end() ){
		tan = start->F.T;
	}
	else
	{
		const One u = (s - start->s) / ((start+1)->s - start->s);
		tan = (1_1-u) * start->F.T + u * (start+1)->F.T;
		tan.Normalize();
	}
}

void PolygonalChain_Imp::Transition( Length s, spat::VectorBundle<Length,One>& bundle ) const
{
	const std::vector<CurveSample>::const_iterator start = GetSegmentStart( s );
	if( start+1 == m_Vertices.end() ){
		bundle.P = start->F.P;
		bundle.T = start->F.T;
	}
	else
	{
		const One u = (s - start->s) / ((start+1)->s - start->s);
		bundle.P = start->F.P + u * ((start+1)->F.P - start->F.P);
		bundle.T = (1_1-u) * start->F.T + u * (start+1)->F.T;
		bundle.T.Normalize();
	}
}

void PolygonalChain_Imp::Transition( Length s, spat::VectorBundle2<Length,One>& bundle ) const
{
	const std::vector<CurveSample>::const_iterator start = GetSegmentStart( s );
	if( start+1 == m_Vertices.end() ){
		bundle.P = start->F.P;
		bundle.T = start->F.T;
		bundle.N = start->F.N;
	}
	else
	{
		const One u = (s - start->s) / ((start+1)->s - start->s);
		bundle.P = start->F.P + u * ((start+1)->F.P - start->F.P);
		bundle.T = (1_1-u) * start->F.T + u * (start+1)->F.T;
		bundle.T.Normalize();
		bundle.N = (1_1-u) * start->F.N + u * (start+1)->F.N;
		bundle.N -= (bundle.N*bundle.T)*bundle.T;
		bundle.N.Normalize();
	}
}

void PolygonalChain_Imp::Transition( Length s, spat::Frame<Length,One>& frame ) const
{
	const std::vector<CurveSample>::const_iterator start = GetSegmentStart( s );

	if( start+1 == m_Vertices.end() ){
		frame.P = start->F.P;
		frame.T = start->F.T;
		frame.N = start->F.N;
		frame.B = start->F.B;
	}
	else
	{
		// This is not quite as correct as a Slerp() could be,
		// but much simpler to compute.
		const One u = (s - start->s) / ((start+1)->s - start->s);
		frame.P = start->F.P + u * ((start+1)->F.P - start->F.P);
		frame.T = (1_1-u) * start->F.T + u * (start+1)->F.T;
		frame.N = (1_1-u) * start->F.N + u * (start+1)->F.N;
		frame.OrthoNormalize();
	}
}

std::vector<Length> PolygonalChain_Imp::ZeroSet() const
{
	std::vector<Length> zeroSet;
	for( auto iter = m_Vertices.begin() + 1; iter < m_Vertices.end(); ++iter )
		if( (iter-1)->F.N * iter->F.N < 0 )
			zeroSet.push_back( iter->s );

	return zeroSet;
}

common::Interval<Length> PolygonalChain_Imp::Range() const{
	return {0_m,m_Vertices.back().s};
}

spat::Vector<One> PolygonalChain_Imp::LocalUp() const noexcept
{
	if( !IsFlat() )
		return {0,0,0};

	for( auto iter = m_Vertices.begin() + 1; iter != m_Vertices.end(); ++iter ){
		if( !m_Vertices.front().F.T.Equals( iter->F.T, One{epsilon} ) )
		{
			return spat::Normalize( m_Vertices.front().F.T % iter->F.T ).second;
		}
	}

	return {0,0,0};
}

spat::Frame<Length, One> PolygonalChain_Imp::GetCurveLocalTransformation() const noexcept{
	spat::Frame<Length,One> retval;
	Transition( 0_m, retval );
	return retval;
}

std::unique_ptr<Curve> PolygonalChain_Imp::Clone() const{
	return std::make_unique<PolygonalChain_Imp>( *this );
}

bool PolygonalChain_Imp::Mirror( const spat::VectorBundle<Length,One>& mirrorPlane ) noexcept
{
	for( VectorBundle<Length,One>& sample : m_Data )
	{
		sample.P -= 2 * ((sample.P - mirrorPlane.P) * mirrorPlane.T) * mirrorPlane.T;
		sample.T -= 2 * (sample.T * mirrorPlane.T) * mirrorPlane.T;
	}

	Create( m_Data );
	return true;
}

bool PolygonalChain_Imp::Equals( 
	const Curve& toCurve,
	common::Interval<Length> range,
	Length epsilon_length,
	Angle epsilon_angle ) const
{
	if( this == &toCurve )
		return true;

	if( auto pToCurve = dynamic_cast<const PolygonalChain*>(&toCurve) )
		return trax::Equals( GetData(), pToCurve->GetData(), range, epsilon_length, epsilon_angle );

	return false;
}

bool Equals( 
	const PolygonalChain::Data& A, 
	const PolygonalChain::Data& B, 
	common::Interval<Length> /*range*/, 
	Length epsilon_length, 
	Angle epsilon_angle )
{	
	return std::equal(	A.begin(), A.end(),
						B.begin(), B.end(),
						[epsilon_length,epsilon_angle]( const PolygonalChain::Data::value_type& A, const PolygonalChain::Data::value_type& B ) noexcept
	{ 
		return A.Equals( B, epsilon_length, epsilon_angle );
	} );
}
}
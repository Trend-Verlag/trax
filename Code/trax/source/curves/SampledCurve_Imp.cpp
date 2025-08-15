// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
//
//	trax track library
//	AD 2022
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#include "SampledCurve_Imp.h"

#include "common/NarrowCast.h"

namespace trax{
///////////////////////////////////////
std::unique_ptr<SampledCurve> SampledCurve::Make() noexcept{
	try{
		return std::make_unique<SampledCurve_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}
///////////////////////////////////////
int SampledCurve_Imp::UpperBound( Length s ) const
{
	return common::narrow_cast<int>( 
				std::upper_bound( 
					m_Data.cbegin(), 
					m_Data.cend(), s ) - m_Data.cbegin() );
}

common::Interval<Length> SampledCurve_Imp::Create(
	const Curve& originalCurve,
	common::Interval<Length> range,
	Length maxDeviationLength, 
	Angle maxDeviationAngle, 
	common::Interval<Length> sampleDistanceLimits )
{
	if( !originalCurve.IsValid() )
		throw std::invalid_argument( "SampledCurve_Imp::Create(): no valid curve to create from." );

	range.Intersection( originalCurve.Range() );
	range.Normalize();
	if( range.Length() < epsilon__length )
		throw std::invalid_argument( "SampledCurve_Imp::Create(): zero length curve." );

	if( maxDeviationLength <= 0_m )
		throw std::invalid_argument( "SampledCurve_Imp::Create(): invalid parameter maxDeviationLength." );

	if( maxDeviationAngle <= 0_rad )
		throw std::invalid_argument( "SampledCurve_Imp::Create(): invalid parameter maxDeviationAngle." );

	if( sampleDistanceLimits.Near() <= 0_m || !sampleDistanceLimits.Normal() )
		throw std::invalid_argument( "SampledCurve_Imp::Create(): invalid parameter sampleDistanceLimits." );

	const One minCosDeviationAngle = cos( maxDeviationAngle );

	m_Data.clear();
	common::Sample( range, [ this, &originalCurve, range, sampleDistanceLimits, maxDeviationLength, minCosDeviationAngle ]( const Length s )
	{
		Frame<Length,One> frame;
		originalCurve.Transition( s, frame );
		m_Data.push_back( { frame, s, originalCurve.Curvature(s), originalCurve.Torsion(s) } );

		Length Ds = sampleDistanceLimits.Near();
		const Length ds = 10*epsilon__length;
		Length sx = s + Ds + ds;

		while( range.Touches( sx ) )
		{
			originalCurve.Transition( sx, frame );
			CurveSample nextSample{ frame, sx, originalCurve.Curvature(sx), originalCurve.Torsion(sx) };
			Interpolate( sx, frame, m_Data.back(), nextSample );

			if( (frame.P - nextSample.F.P).Length() > maxDeviationLength )
				break;
			if( frame.N * nextSample.F.N < minCosDeviationAngle )
				break;
			if( Ds > sampleDistanceLimits.Far() )
			{
				Ds = sampleDistanceLimits.Far();
				break;
			}

			Ds += ds;
			sx = s + Ds + ds;
		}

		return Ds;
	} );


	// Our sample curve has to contain the points where curvature gets zero,
	// we add them here:
	std::vector<Length> zeroSet = originalCurve.ZeroSet();
	for( const Length s : zeroSet )
	{
		if( !range.Touches( s ) )
			continue;

		Frame<Length,One> frame;

		if( auto iter = std::upper_bound( m_Data.begin(), m_Data.end(), s ); iter != m_Data.begin() && iter != m_Data.end() ){
			originalCurve.Transition( s, frame.P );

			const One u = (s - (iter-1)->s) / (iter->s - (iter-1)->s);

			frame.T = Normalize(iter->F.P - frame.P).second;
			frame.N = (1_1-u) * -(iter-1)->F.N + u * iter->F.N;
			frame.OrthoNormalize();
		}
		else
			originalCurve.Transition( s, frame );

		m_Data.push_back( { frame, s, 0_1Im, 0_1Im } );
	}

	return Create( m_Data );
}

common::Interval<Length> SampledCurve_Imp::Create(
	std::pair<std::shared_ptr<const Curve>,
	common::Interval<Length>> originalCurve,
	Length maxDeviationLength, 
	Angle maxDeviationAngle, 
	common::Interval<Length> sampleDistanceLimits )
{
	return Create( *originalCurve.first, originalCurve.second, maxDeviationLength, maxDeviationAngle, sampleDistanceLimits );
}

common::Interval<Length> SampledCurve_Imp::Create( const Data& data )
{
	Data sortedData = data;
	std::sort( sortedData.begin(), sortedData.end() );

	Data purifiedData;
	purifiedData.reserve( data.size() );

	for( const CurveSample& sample : sortedData )
	{
		if( purifiedData.empty() || !common::Equals( sample.s, purifiedData.back().s, epsilon__length ) ){
			if( !sample.F.IsOrthoNormal() )
				throw std::invalid_argument( "SampledCurve_Imp::Create(): A frame is not orthonormalized!" );

			purifiedData.push_back( sample );
		}
		else
			purifiedData.back() = sample; // have the last word
	};

	if( purifiedData.size() < 2 )
		throw std::invalid_argument( "SampledCurve_Imp::Create(): too few valid samples in sampling data!" );

	m_Data = purifiedData;

	// For interpolation we need to protect the zero points by a preceding closeby
	// vertex, to not have the section before the zero point interpolating the flip:

	//std::vector<Length> zeroSet = ZeroSet();
	//for( const Length s : zeroSet )
	//{
	//	if( auto iter = std::upper_bound( m_Data.begin(), m_Data.end(), s ); iter != m_Data.begin() && iter != m_Data.end() )
	//	{
	//		iter->t = 0_1Im;
	//		iter = m_Data.insert( iter, *iter );
	//		iter->F.P = (iter-1)->F.P;
	//		iter->s = (iter-1)->s;
	//		iter->k = 0_1Im;
	////		iter->t = 0_1Im;

	//		--iter;
	//		assert( iter->k == 0_1Im );

	//		iter = m_Data.insert( iter, *(iter-1) );		
	//		iter->F.P = (iter+1)->F.P;
	//		iter->s = (iter+1)->s;
	//		iter->k = 0_1Im;
	////		iter->t = 0_1Im;
	////		(iter-1)->t = 0_1Im;
	//	}
	//}


	for( auto iter = m_Data.begin() + 1; iter < m_Data.end(); ++iter )
	{
		if( (iter-1)->F.N * iter->F.N < 0 ){
			iter = m_Data.insert( iter, *iter );		
			iter->F.T = (iter-1)->F.T;
			iter->F.N = (iter-1)->F.N;
			iter->F.B = (iter-1)->F.B;
			iter->k = 0_1Im;
			++iter;
		}
	}

	return Range();
}

const SampledCurve::Data& SampledCurve_Imp::GetData() const noexcept{
	return m_Data;
}

bool SampledCurve_Imp::IsValid() const noexcept{
	return !m_Data.empty();
}

AnglePerLength SampledCurve_Imp::Curvature( Length s ) const
{
	const std::size_t idx = static_cast<std::size_t>(UpperBound( s )) - 1;

	if( idx >= m_Data.size() - 1 )
		return m_Data.at(idx).k;
	else{
		const One u = (s - m_Data.at(idx).s) / (m_Data.at(idx+1).s - m_Data.at(idx).s);
		return m_Data.at(idx).k + u * (m_Data.at(idx+1).k - m_Data.at(idx).k);
	}
}

AnglePerLength SampledCurve_Imp::Torsion( Length s ) const
{
	const std::size_t idx = static_cast<std::size_t>(UpperBound( s )) - 1;

	if( idx >= m_Data.size() - 1 )
		return m_Data.at(idx).t;
	else{
		const One u = (s - m_Data.at(idx).s) / (m_Data.at(idx+1).s - m_Data.at(idx).s);
		return m_Data.at(idx).t + u * (m_Data.at(idx+1).t - m_Data.at(idx).t);
	}
}

bool SampledCurve_Imp::IsFlat() const noexcept
{
	for( auto iter = m_Data.begin(); iter != m_Data.end(); ++iter ){
		if( iter->t != 0_1Im )
			return false;
	}

	return true;
}

void SampledCurve_Imp::Transition( Length s, spat::Position<Length>& pos ) const
{
	spat::Frame<Length,One> frame;
	Transition( s, frame );
	pos = frame.P;
}

void SampledCurve_Imp::Transition( Length s, spat::Vector<One>& tan ) const
{
	spat::Frame<Length,One> frame;
	Transition( s, frame );
	tan = frame.T;
}

void SampledCurve_Imp::Transition( Length s, spat::VectorBundle<Length,One>& bundle ) const
{
	spat::Frame<Length,One> frame;
	Transition( s, frame );
	bundle = frame;
}

void SampledCurve_Imp::Transition( Length s, spat::VectorBundle2<Length,One>& bundle ) const
{
	spat::Frame<Length,One> frame;
	Transition( s, frame );
	bundle = frame;
}

void SampledCurve_Imp::Transition( Length sx, spat::Frame<Length,One>& frame ) const
{
	const std::size_t idx = static_cast<std::size_t>(UpperBound( sx )) - 1;

	frame = m_Data.at(idx).F;
	if( idx >= m_Data.size() - 1 )
		return;

	Interpolate( sx, frame, m_Data.at(idx), m_Data.at(idx+1) );
}

std::vector<Length> SampledCurve_Imp::ZeroSet() const
{
	std::vector<Length> zeroSet;

	for( const CurveSample& sample : m_Data )
		if( sample.k == 0_1Im && 
		   (zeroSet.empty() || !common::Equals( zeroSet.back(), sample.s, epsilon__length ) ) )
			zeroSet.push_back( sample.s );

	return zeroSet;
}

common::Interval<Length> SampledCurve_Imp::Range() const
{
	return { m_Data.front().s, m_Data.back().s };
}

spat::Vector<One> SampledCurve_Imp::LocalUp() const noexcept
{
	if( !IsFlat() )
		return {0,0,0};

	for( auto iter = m_Data.begin() + 1; iter != m_Data.end(); ++iter ){
		if( !m_Data.front().F.T.Equals( iter->F.T, One{epsilon} ) )
		{
			return spat::Normalize( m_Data.front().F.T % iter->F.T ).second;
		}
	}

	return {0,0,0};
}

spat::Frame<Length, One> SampledCurve_Imp::GetCurveLocalTransformation() const noexcept
{
	spat::Frame<Length,One> retval;
	Transition( 0_m, retval );
	return retval;
}

std::unique_ptr<Curve> SampledCurve_Imp::Clone() const{
	return std::make_unique<SampledCurve_Imp>( *this );
}

bool SampledCurve_Imp::Mirror( const spat::VectorBundle<Length,One>& mirrorPlane ) noexcept
{
	for( CurveSample& sample : m_Data )
	{
		sample.F.P -= 2 * ((sample.F.P - mirrorPlane.P) * mirrorPlane.T) * mirrorPlane.T;
		sample.F.T -= 2 * (sample.F.T * mirrorPlane.T) * mirrorPlane.T;
		sample.F.N -= 2 * (sample.F.N * mirrorPlane.T) * mirrorPlane.T;
	}

	Create( m_Data );
	return true;
}

bool SampledCurve_Imp::Equals(
	const Curve& toCurve,common::Interval<Length> range,
	Length epsilon_length,
	Angle epsilon_angle ) const
{
	if( this == &toCurve )
		return true;

	if( auto pToCurve = dynamic_cast<const SampledCurve*>(&toCurve) )
		return trax::Equals( GetData(), pToCurve->GetData(), range, epsilon_length, epsilon_angle );

	return false;
}

void SampledCurve_Imp::Interpolate( Length sx, spat::Frame<Length,One>& F, const CurveSample& start, const CurveSample& end ) const noexcept
{
	F = start.F;
	AnglePerLength k = start.k;
	AnglePerLength t = start.t;
	Length ds = epsilon__length; //Todo: find good ds...
	Length s = start.s;

	while( s < sx )
	{
		s += ds;
		if( s > sx ){
			ds = sx - s + ds;
			s = sx;
		}

		const Vector<One> T = F.T;
		const Vector<One> N = F.N;
		const Vector<One>& B = F.B;

		F.P += ds * T;
		F.T += ds * k * N;
		F.N += ds * (-k * T + t * B);
		F.B += ds * -t * N;

		const One u = (s - start.s) / (end.s - start.s);
		k = start.k + u * (end.k - start.k);
		t = start.t + u * (end.t - start.t);
	}
}

bool Equals( 
	const SampledCurve::Data& A, 
	const SampledCurve::Data& B, 
	common::Interval<Length> /*range*/, 
	Length epsilon_length, 
	Angle epsilon_angle ) noexcept
{
	return std::equal(	A.begin(), A.end(),
						B.begin(), B.end(),
						[epsilon_length,epsilon_angle]( const SampledCurve::Data::value_type& A, const SampledCurve::Data::value_type& B ) noexcept
	{ 
		return A.Equals( B, epsilon_length, epsilon_angle );
	} );
}

}
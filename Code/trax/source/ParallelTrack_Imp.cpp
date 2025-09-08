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

#include "ParallelTrack_Imp.h"

#include "curves/Cubic_Imp.h"
#include "trax/Track.h"
#include "spat/Position2D.h"

namespace trax{
///////////////////////////////////////
std::shared_ptr<ParallelizableTrack> ParallelizableTrack::Make() noexcept{
	try{
		if( std::shared_ptr<ParallelizableTrack_Imp> pRetval = std::make_shared<ParallelizableTrack_Imp>() ){
			pRetval->SetWeakPointerToSelf( pRetval );
			return pRetval;
		}

		return nullptr;
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

std::shared_ptr<ParallelizableTrack> ParallelizableTrack_Imp::GetParallelizableTrack() const noexcept{
	return std::dynamic_pointer_cast<ParallelizableTrack>(This());
}

std::shared_ptr<TrackBuilder> ParallelizableTrack_Imp::AddParallel( const spat::Vector2D<Length>& shift, bool bTrackRelative ){
	m_ParallelTrackList.push_back( std::make_pair( TrackBuilder::Make(), Data{shift,bTrackRelative} ) );
	
	if( IsValid() )
		UpdateParallel( m_ParallelTrackList.back() );

	return m_ParallelTrackList.back().first;
}

void ParallelizableTrack_Imp::RemoveParallel( const TrackBuilder& parallelTrack ){
	for( auto iter = m_ParallelTrackList.begin(); iter != m_ParallelTrackList.end(); ++iter ){
		if( iter->first.get() == &parallelTrack ){
			//if( TrackSystem* pTrackSystem = iter->first->GetTrackSystem() )
			//	pTrackSystem->Remove( iter->first.get(), true );
			//else if( TrackCollection* pTrackCollection = iter->first->GetTrackCollection() )
			//	pTrackCollection->Remove( iter->first.get(), true );

			m_ParallelTrackList.erase( iter );
			return;
		}
	}

	throw std::invalid_argument( "parallelTrack is not a parallel to original!" );
}

bool ParallelizableTrack_Imp::IsParallel( const TrackBuilder& parallelTrack ) const noexcept{
	return ParallelIdx( parallelTrack ) >= 0;
}

const ParallelizableTrack::Data& ParallelizableTrack_Imp::GetParallelData( int idx ) const{
	return m_ParallelTrackList.at(idx).second;
}

void ParallelizableTrack_Imp::Clear(){
	for( auto iter = m_ParallelTrackList.begin(); iter != m_ParallelTrackList.end(); ++iter ){
		//if( TrackSystem* pTrackSystem = iter->first->GetTrackSystem() )
		//	pTrackSystem->Remove( iter->first.get(), true );
		//else if( TrackCollection* pTrackCollection = iter->first->GetTrackCollection() )
		//	pTrackCollection->Remove( iter->first.get(), true );
	}

	m_ParallelTrackList.clear();
}

int ParallelizableTrack_Imp::CountTracks() const{
	return common::narrow_cast<int>(m_ParallelTrackList.size());
}

int ParallelizableTrack_Imp::ParallelIdx( const TrackBuilder& parallelTrack ) const{
	for( int idx = 0; idx < common::narrow_cast<int>(m_ParallelTrackList.size()); ++idx ){
		if( m_ParallelTrackList[idx].first.get() == &parallelTrack )
			return idx;
	}

	return -1;
}

std::shared_ptr<TrackBuilder> ParallelizableTrack_Imp::GetParallel( int idx ) const{
	if( 0 <= idx && idx < CountTracks() )
		return m_ParallelTrackList[idx].first;

	return nullptr;
}

Length ParallelizableTrack_Imp::ParallelParameterFrom( int idx, Length sOriginalParameter ) const{
	return std::dynamic_pointer_cast<const PointToPointParallel>(m_ParallelTrackList.at(idx).first->GetCurve().first)->ParallelParameterFrom( sOriginalParameter );
}

Length ParallelizableTrack_Imp::OriginalParameterFrom( int idx, Length sParallelParameter ) const{
	return std::dynamic_pointer_cast<const PointToPointParallel>(m_ParallelTrackList.at(idx).first->GetCurve().first)->OriginalParameterFrom( sParallelParameter );
}

void ParallelizableTrack_Imp::OnGeometryChanged() noexcept
{
	if( IsValid() )
		UpdateParallels();
}

void ParallelizableTrack_Imp::UpdateParallels(){
	for( const auto& parallel : m_ParallelTrackList )
		UpdateParallel( parallel );
}

void ParallelizableTrack_Imp::UpdateParallel( const std::pair<std::shared_ptr<TrackBuilder>, Data>& parallel )
{
	if( parallel.second.bTrackRelative ){
		auto pParallel = std::make_shared<ParallelCurve_Imp>();
		parallel.first->Attach( pParallel, pParallel->Create( { this, parallel.second.shift } ) );
		parallel.first->Attach( std::make_unique<ParallelTwist>(*this) );
	}
	else{	
		auto pParallel = std::make_shared<PlaneParallelCurve_Imp>();
		parallel.first->Attach( pParallel, pParallel->Create( { this, parallel.second.shift } ) );
		parallel.first->Attach( GetTwist().Clone() );
	}
}
///////////////////////////////////////
Position<Length> ParallelizableTrack_Imp::ParallelFunction::P( One u ) const{
	const Length s{ u };
	m_Data.pTrack->Range().Clip( s ); 
	Frame<Length,One> Fw;
	m_Data.pTrack->Transition( s, Fw );

	return Fw.P + h * Fw.N + v * Fw.B;
}

Vector<Length> ParallelizableTrack_Imp::ParallelFunction::D1( One u ) const
///
/// D1 = dCp(s)/ds = (1 + k * (v*sin(w) - h*cos(w))) * Fw.T - (t + dw/ds) * (v*Fw.N - h*Fw.B);
///
/// where s is the arc length parameter of the original curve, not of the parallel. See traxBook Appendix C.
{
	Length s{ u };
	m_Data.pTrack->Range().Clip( s );
	Frame<Length,One> Fw;
	m_Data.pTrack->Transition( s, Fw );
	const Angle w = m_Data.pTrack->GetTwistValue( s );
	const AnglePerLength k = m_Data.pTrack->Curvature(s);

	//The spatial_cast is needed since we use a dimensionated value s 
	//for what technically would be undimensionated value u.
	return spatial_cast<Length>((1_1 + k * (v*sin(w) - h*cos(w))) * Fw.T - m_Data.pTrack->Torsion(s) * (v*Fw.N - h*Fw.B));
}
//
//Vector<Length> ParallelizableTrack_Imp::ParallelFunction::D2( One /*u*/ ) const
//// D2 = [dk/ds * (v*sin(w) - h*cos(w)) + k * (t + 2*dw/ds) * (v*cos(w) + h*sin(w))] * Fw.T 
////      + k * (1 + k * (v*sin(w) - h*cos(w))) * F.N
////      - (dt/ds + pow<2>(d)w/pow<2>(d)s) * (v*Fw.N - h*Fw.B)
////      - pow<2>(t + dw/ds)      * (h*Fw.N + v*Fw.B);
////
//// dk/ds = pow<3>(d)C(s)/pow<3>(ds) * F.N;
//// dt/ds + pow<2>(d)w/pow<2>(ds) = not available (except for directional twist)
//{
//	return Null<Length>;
//	//Length s{ u };
//	//m_Data.pTrack->Range().Clip( s );
//
//	//Frame<Length,One> Fw, F;
//	//m_Data.pTrack->Transition( s, Fw );
//	//m_Data.pTrack->TNBFrame( s, F );
//
//	//const Value<Dimension<-2, 0, 0>> dkds = m_Data.pTrack->GetCurveD3(s) * F.N;
//	//const Value<Dimension<-1, 0, 0>> dwds = m_Data.pTrack->GetTwistD1(s);
//	//const Angle w = m_Data.pTrack->GetTwistValue( s );
//	//const AnglePerLength k = m_Data.pTrack->Curvature(s);
//	//const AnglePerLength t_p_dwds = m_Data.pTrack->Torsion(s);
//	//const Length vsinw_m_hcosw = v*sin(w) - h*cos(w);
//
//	//const auto f = [this]( Length s ) -> AnglePerLength{
//	//	m_Data.pTrack->Range().Clip( s );
//	//	return m_Data.pTrack->Torsion(s);
//	//};
//
//	//const Value<Dimension<-2, 0, 0>> dtds_p_d2wds2{ finite_difference_derivative6<Value<Dimension<-2, 0, 0>>>( f, s ) };
//
//	//return spatial_cast<Length>(	  (dkds * vsinw_m_hcosw + k * (t_p_dwds + dwds) * (v*cos(w) + h*sin(w))) * Fw.T 
//	//								+ k * (1 + k * vsinw_m_hcosw) * F.N
//	//								- dtds_p_d2wds2 * (v*Fw.N - h*Fw.B)
//	//								- t_p_dwds * t_p_dwds * (h*Fw.N + v*Fw.B) );
//}
//
//Vector<Length> ParallelizableTrack_Imp::ParallelFunction::D3( One u ) const
//{
//	const auto d2 = [this]( One u ) -> Vector<Length>{
//		return D2( u );
//	};
//
//	return finite_difference_derivative6<Vector<Length>>( d2, u );
//}

bool ParallelizableTrack_Imp::ParallelFunction::IsValid() const noexcept{
	return m_Data.pTrack && m_Data.pTrack->IsValid();
}

Interval<Real> ParallelizableTrack_Imp::ParallelFunction::Create( const DataType& data ) noexcept{
	m_Data = data;
	return { 0, m_Data.pTrack ? m_Data.pTrack->GetLength().Units() : 0 };
}
///////////////////////////////////////
const char* ParallelizableTrack_Imp::ParallelCurve_Imp::TypeName() const noexcept{
	return "Parallel";
}

Curve::CurveType ParallelizableTrack_Imp::ParallelCurve_Imp::GetCurveType() const noexcept{
	return Curve::CurveType::Parallel;
}

std::unique_ptr<Curve> ParallelizableTrack_Imp::ParallelCurve_Imp::Clone() const{
	return std::make_unique<ParallelCurve_Imp>( *this );
}
///////////////////////////////////////
ParallelizableTrack_Imp::ParallelTwist::ParallelTwist( const ParallelizableTrack& Parent ) noexcept
	: m_ParallelizableTrack{Parent}
{
	RoadwayTwist_Imp<RoadwayTwist>::ZeroFlip( false );
}

const char* ParallelizableTrack_Imp::ParallelTwist::TypeName() const noexcept{
	return "ParallelTwist";
}

RoadwayTwist::TwistType ParallelizableTrack_Imp::ParallelTwist::GetTwistType() const noexcept{
	return RoadwayTwist::TwistType::Parallel;
}

std::unique_ptr<RoadwayTwist> ParallelizableTrack_Imp::ParallelTwist::Clone() const{
	return std::make_unique<ParallelTwist>( *this );
}

void ParallelizableTrack_Imp::ParallelTwist::OnAttach( const TrackBuilder& track ) noexcept{
	RoadwayTwist_Imp<RoadwayTwist>::OnAttach( track );

	m_pCurve = std::dynamic_pointer_cast<const ParallelCurve_Imp>(track.GetCurve().first);
}

void ParallelizableTrack_Imp::ParallelTwist::OnDetach() noexcept{
	m_pCurve.reset();

	RoadwayTwist_Imp<RoadwayTwist>::OnDetach();
}

Angle ParallelizableTrack_Imp::ParallelTwist::DoTwist( Length s ) const{
	if( m_pCurve ){
		m_ParallelizableTrack.Range().Clip( s );

		Frame<Length,One> frameCurve, frameOriginal;
		m_pCurve->Transition( c(s), frameCurve );
		m_ParallelizableTrack.Transition( m_pCurve->OriginalParameterFrom( c(s) ), frameOriginal );

		//std::cout << "parallel twist: " << -std::atan2(frameOriginal.B*frameCurve.N,frameOriginal.B*frameCurve.B) << std::endl;
		return -std::atan2(frameOriginal.B*frameCurve.N,frameOriginal.B*frameCurve.B); //see DirectionalTwist_Imp
	}

	return 0_deg;
}

AnglePerLength ParallelizableTrack_Imp::ParallelTwist::DoD1( Length s ) const{
	const auto f = [this]( Length s ) -> Angle{
		return DoTwist( s );
	};

	const AnglePerLength retval = finite_difference_derivative6<AnglePerLength>( f, s );
	return abs(retval) < 0.1_1Im ? retval : 0.0_1Im;
}
///////////////////////////////////////
bool Equals( 
	const PointToPointParallel::Data& A, 
	const PointToPointParallel::Data& B, 
	common::Interval<Length> /*range*/, 
	Length epsilon_length, 
	Angle /*epsilon_angle*/ ) noexcept
{
	return A.pTrack == B.pTrack && A.d.Equals( B.d, epsilon_length );
}
///////////////////////////////////////
Position<Length> ParallelizableTrack_Imp::PlaneParallelFunction::P( One u ) const{
	const Length s{ u };
	m_Data.pTrack->Range().Clip( s ); 
	Frame<Length,One> Fw;
	m_Data.pTrack->TNBFrame( s, Fw );

	const Angle w = -std::atan2( Up*Fw.N, Up*Fw.B ); //(directional twist)
	Fw.RotateTan( w );

	return Fw.P + h * Fw.N + v * Up;
}

Vector<Length> ParallelizableTrack_Imp::PlaneParallelFunction::D1( One u ) const
///
/// D1 = (1-k*h*cos(w)) * F.T - (dw/ds+t)*h*F.B;
/// 
/// (dw/ds+t) = k * Up*T/Up*B / (1 + pow<2>(Up*N/Up*B)) // see trax book appendix C
{
	Length s{ u };
	m_Data.pTrack->Range().Clip( s );
	Frame<Length,One> Fw;
	m_Data.pTrack->TNBFrame( s, Fw );

	const AnglePerLength k = m_Data.pTrack->Curvature(s);
	const One UT = Up*Fw.T;
	const One UN = Up*Fw.N;
	const One UB = Up*Fw.B;
	const AnglePerLength dwds_p_t = (abs(UB) > epsilon) ? k * UT/UB / (1 + pow<2>(UN)/UB) : 0_1Im;
	const Angle w = -std::atan2( UN, UB ); //(directional twist)
	Fw.RotateTan( w );

	return spatial_cast<Length>( (1_1-k*h*cos(w)) * Fw.T + dwds_p_t * h * Fw.B );
}
//
//Vector<Length> ParallelizableTrack_Imp::PlaneParallelFunction::D2( One ) const{
//	return Null<Length>;
//	//throw std::runtime_error( "Not implemented!" );
//}
//
//Vector<Length> ParallelizableTrack_Imp::PlaneParallelFunction::D3( One ) const{
//	return Null<Length>;
//	//throw std::runtime_error( "Not implemented!" );
//}

bool ParallelizableTrack_Imp::PlaneParallelFunction::IsValid() const noexcept{
	return m_Data.pTrack && m_Data.pTrack->IsValid();
}
///////////////////////////////////////
Interval<Real> ParallelizableTrack_Imp::PlaneParallelFunction::Create( const DataType& data ) noexcept{
	m_Data = data;
	return { 0, m_Data.pTrack ? m_Data.pTrack->GetLength().Units() : 0 };
}

const char* ParallelizableTrack_Imp::PlaneParallelCurve_Imp::TypeName() const noexcept{
	return "PlaneParallel";
}

Curve::CurveType ParallelizableTrack_Imp::PlaneParallelCurve_Imp::GetCurveType() const noexcept{
	return Curve::CurveType::Parallel;
}

std::unique_ptr<Curve> ParallelizableTrack_Imp::PlaneParallelCurve_Imp::Clone() const{
	return std::make_unique<PlaneParallelCurve_Imp>( *this );
}
///////////////////////////////////////
spat::VectorBundle2<Length,One> Parallel( const TrackBuilder& originalTrack, Length s, const spat::Vector2D<Length> shift )
{
	originalTrack.Range().Clip( s ); 

	const Length& h = shift.dx;
	const Length& v = shift.dy;

	Frame<Length,One> Fw;
	originalTrack.Transition( s, Fw );
	const AnglePerLength k = originalTrack.Curvature(s);
	const Angle w = originalTrack.GetTwistValue( s );

	return { 
		Fw.P + h * Fw.N + v * Fw.B, 
		(1_1 + k * (v*sin(w) - h*cos(w))) * Fw.T - originalTrack.Torsion(s) * (v*Fw.N - h*Fw.B),
		Fw.B };
}

spat::VectorBundle2<Length,One> ParallelToUp( const TrackBuilder& originalTrack, Length s, spat::Vector2D<Length> shift, Vector<One> up )
{
	originalTrack.Range().Clip( s ); 

	const Length& h = shift.dx;
	const Length& v = shift.dy;

	Frame<Length,One> F, Fw;
	originalTrack.TNBFrame( s, F );
	originalTrack.Transition( s, Fw );
	const AnglePerLength k = originalTrack.Curvature(s);

	if( spat::Parallel( F.T, up ) )
	// no rotation will bring us up; Fw.N is level already, so we use
	// the frame Fw as twisted by track.
	{
		const Angle w = originalTrack.GetTwistValue( s );
		const AnglePerLength dwds_p_t = originalTrack.Torsion( s );

		return {
			Fw.P + h * Fw.N + v * up,
			(1_1-k*h*cos(w)) * Fw.T + dwds_p_t * h * Fw.B,
			Fw.B };
	}	
	else
	// ignore track twist, use directional twist to level out Fw.N; apply
	// formular from Chapter4 for dw/ds + t:
	{
		const Angle w = -std::atan2( up*F.N, up*F.B ); //(directional twist)
		const AnglePerLength dwds_p_t = k * (up*F.T)*(up*F.B) / (pow<2>(up*F.B) + pow<2>(up*F.N));
		const Vector<One> FworiginalB = Fw.B;
		Fw = F;
		Fw.RotateTan( w );

		return {
			Fw.P + h * Fw.N + v * up,
			(1_1-k*h*cos(w)) * Fw.T + dwds_p_t * h * Fw.B,
			FworiginalB };
	}
}

std::shared_ptr<TrackBuilder> MakeParallelTrack( 
	const TrackBuilder& originalTrack, 
	common::Interval<Length> range, 
	spat::Vector2D<Length> shift, 
	bool bShiftRelativeToTwistOrUp, 
	spat::Vector<One> up, 
	Length maxDeviationLength, 
	Angle maxDeviationAngle )
{
	if( !originalTrack.IsValid() )
		return nullptr;

	if( maxDeviationLength <= 0_m )
		return nullptr;

	if( maxDeviationAngle <= 0_rad )
		return nullptr;

	if( shift.Equals( spat::Null2D<Length>, maxDeviationLength ) )
		return CopyTrack( originalTrack, range, true );

	if( !bShiftRelativeToTwistOrUp && originalTrack.GetCurve().first->GetCurveType() == Curve::CurveType::Line )
	// The strech function will loose the orientation of a line, so we do it here.
	{	
		std::shared_ptr<TrackBuilder> pTrack = CopyTrack( originalTrack, range, true );
		Frame<Length,One> frame, shiftFrame;
		pTrack->Transition( 0_m, frame );
		shiftFrame = frame;
		shiftFrame.RotateTan( up );
		pTrack->SetFrame( frame.TransportBy( shift.dx * shiftFrame.N + shift.dy * up ), 0_m );
		return pTrack;
	}

	//if( bShiftRelativeToTwistOrUp && originalTrack.GetTwist().IsNull() )
	//// Special case not handled well by strech method
	//{
	//	if( std::shared_ptr<const HelixP> pHelixP = std::dynamic_pointer_cast<const HelixP>(originalTrack.GetCurve().first); pHelixP )
	//	{
	//		std::shared_ptr<TrackBuilder> pTrack = CopyTrack( originalTrack, range, true );
	//		auto curve = pTrack->DetachCurve();
	//		HelixP::Data data = pHelixP->GetData();
	//		HelixP::Data dataNew = data;
	//		
	//		dataNew.a = data.a - shift.dx;
	//		dataNew.b = (data.a - shift.dx)*data.b/data.a;
	//		std::unique_ptr<HelixP> pHelixPNew = HelixP::Make();
	//		pHelixPNew->Create( dataNew );


	//		//pHelixP->Create( data );
	//	}
	//}

	if( auto pTrack = TrackBuilder::Make() )
	{
		spat::VectorBundle2<Length,One> start, end;

		if( bShiftRelativeToTwistOrUp ){
			start = Parallel( originalTrack, range.Near(), shift );
			end = Parallel( originalTrack, range.Far(), shift );
		}
		else{
			start = ParallelToUp( originalTrack, range.Near(), shift );
			end = ParallelToUp( originalTrack, range.Far(), shift );
		}

		start.T.Normalize();
		end.T.Normalize();

		if( Strech( *pTrack, { start.P, start.T }, { end.P, end.T }, {0_m,+infinite__length}, {0_m,+infinite__length}, up, maxDeviationLength, maxDeviationAngle ) &&
			pTrack->GetCurve().first->GetCurveType() != Curve::CurveType::Cubic )
		{
			pTrack->Attach( originalTrack.GetTwist().Clone() ); // this twist might not be sufficient...

			// having a little test reduces false parallels to deliberately constructed cases:

			const Position2D<Length> pos2D = Origin2D<Length> - shift;
			const Position<Length> pos3D{ 0_m, pos2D.x, pos2D.y };

			spat::Frame<Length,One> parallelFrame, originalFrame;
			originalTrack.Transition( range.Near(), originalFrame );
			pTrack->Transition( 0_m, parallelFrame );
			if( parallelFrame.N.Equals( originalFrame.N, maxDeviationAngle ) )
			{
				if( common::Equals( (parallelFrame.P - originalFrame.P).Length(), shift.Length(), maxDeviationLength ) )
				{
					parallelFrame.TransportTo( pos3D );
					if( !bShiftRelativeToTwistOrUp  || common::Equals( (parallelFrame.P - originalFrame.P).Length(), 0_m, maxDeviationLength ) )
					{
						originalTrack.Transition( range.Center(), originalFrame );
						pTrack->Transition( pTrack->GetLength() / 2, parallelFrame );
						if( common::Equals( (parallelFrame.P - originalFrame.P).Length(), shift.Length(), maxDeviationLength ) )
						{
							parallelFrame.TransportTo( pos3D );
							if( !bShiftRelativeToTwistOrUp  || common::Equals( (parallelFrame.P - originalFrame.P).Length(), 0_m, maxDeviationLength ) )
							{
								originalTrack.Transition( range.Far(), originalFrame );
								pTrack->Transition( pTrack->GetLength(), parallelFrame );
								if( common::Equals( (parallelFrame.P - originalFrame.P).Length(), shift.Length(), maxDeviationLength ) )
								{
									parallelFrame.TransportTo( pos3D );
									if( !bShiftRelativeToTwistOrUp  || common::Equals( (parallelFrame.P - originalFrame.P).Length(), 0_m, maxDeviationLength ) )
									{
										return pTrack;
									}
								}
							}
						}
					}
				}
			}
			else
				std::cout << "Normal mismatch on track parallel." << std::endl;
		}
	}

	return nullptr;
}

std::shared_ptr<TrackBuilder> MakeParallelTrackWithPolygonalChain(
	const TrackBuilder& originalTrack, 
	common::Interval<Length> range,
	spat::Vector2D<Length> shift,
	bool bShiftRelativeToTwistOrUp,
	spat::Vector<One> up,
	Length maxDeviation, 
	common::Interval<Length> sampleDistanceLimits )
{
	if( !originalTrack.IsValid() )
		return nullptr;

	if( maxDeviation <= 0_m )
		return nullptr;

	if( sampleDistanceLimits.Near() <= 0_m || !sampleDistanceLimits.Normal() )
		return nullptr;

	if( auto pTrack = TrackBuilder::Make() )
	{
		if( std::shared_ptr<PolygonalChain> pPoly = PolygonalChain::Make() )
		{
			using Data = std::vector<std::pair<PolygonalChain::Data::value_type,spat::Vector<One>>>;
			Data data;

			const auto EvaluateParallel = [&originalTrack,&data,&shift,sampleDistanceLimits]( Length s ) -> Length{
				const auto parallel = Parallel( originalTrack, s, shift );
				data.push_back( { parallel, parallel.N } );
				const One dCIds = parallel.T.Length();
				if( dCIds > epsilon )
					return sampleDistanceLimits.Near() / dCIds;
				return sampleDistanceLimits.Near();
			};

			const auto EvaluateParallelToUp = [&originalTrack,&data,&shift,sampleDistanceLimits,&up]( Length s ) -> Length{
				const auto parallel = ParallelToUp( originalTrack, s, shift, up );
				data.push_back( { parallel, parallel.N } );
				const One dCIds = parallel.T.Length();
				if( dCIds > epsilon )
					return sampleDistanceLimits.Near() / dCIds;
				return sampleDistanceLimits.Near();
			};

			if( auto pPolygonalChain = std::dynamic_pointer_cast<const PolygonalChain>(originalTrack.GetCurve().first); pPolygonalChain )
			// a polygonal chain would detoriate fast, if sampled at arbitray points...
			{
				data.reserve( pPolygonalChain->GetData().size() );
				common::Interval<Length> curveI = originalTrack.GetCurve().second;

				if( bShiftRelativeToTwistOrUp )
					for( int idx = 0; idx < common::narrow_cast<int>(pPolygonalChain->GetData().size()); ++idx )
						EvaluateParallel( (pPolygonalChain->GetParameter( idx ) - curveI.Near()) / (curveI.Normal() ? +1.f : -1.f) );
				else
					for( int idx = 0; idx < common::narrow_cast<int>(pPolygonalChain->GetData().size()); ++idx )
						EvaluateParallelToUp( (pPolygonalChain->GetParameter( idx ) - curveI.Near()) / (curveI.Normal() ? +1.f : -1.f) );

				if( data.size() > 3 )
					DouglasPeucker( data, data.begin(), data.end() - 1, maxDeviation, []( Data::iterator i ) -> const spat::Position<Length>& { return i->first.P; } );
			}
			else{
				data.reserve( common::narrow_cast<size_t>(std::ceil( abs(range.Length()) / sampleDistanceLimits.Near() )) + 3 );

				if( bShiftRelativeToTwistOrUp )
					common::Sample( range, EvaluateParallel );
				else
					common::Sample( range, EvaluateParallelToUp );

				DouglasPeucker( data, data.begin(), data.end() - 1, maxDeviation, []( Data::iterator i ) -> const spat::Position<Length>& { return i->first.P; } );
			}


			PolygonalChain::Data curveData;
			curveData.reserve( data.size() );

			for( const auto& pair : data ){
				curveData.push_back(pair.first);
				curveData.back().T.Normalize();
			}

			pTrack->Attach( pPoly, pPoly->Create( curveData ) );

			PiecewiseTwist::Data twistdata;
			twistdata.reserve( data.size() );

			for( auto iter = data.begin(); iter < data.end(); ++iter ){
				const spat::Vector<One>& FwB = iter->second;	
				Frame<Length,One> F;
				const Length s = pPoly->GetParameter( common::narrow_cast<int>(iter - data.begin()) );

				auto zeroSet = pPoly->ZeroSet();
				if( std::find( zeroSet.begin(), zeroSet.end(), s ) != zeroSet.end() )
				{
					pTrack->Transition( s - epsilon__length, F );
					twistdata.push_back( { s - epsilon__length, -std::atan2( FwB * F.N, FwB * F.B ) });
				}

				pTrack->Transition( s , F );
				twistdata.push_back( { s, -std::atan2( FwB * F.N, FwB * F.B ) });		//see DirectionalTwist_Imp
			}

			auto pTwist = PiecewiseTwist::Make( RoadwayTwist::TwistType::PiecewiseCircular );
			pTwist->Create( twistdata );
			pTrack->Attach( std::move(pTwist) );

			return pTrack;
		}
	}

	return nullptr;
}

std::shared_ptr<TrackBuilder> MakeParallelTrackWithSampledCurve(
	const TrackBuilder& originalTrack, 
	common::Interval<Length> range,
	spat::Vector2D<Length> shift,
	bool bShiftRelativeToTwistOrUp,
	spat::Vector<One> /*up*/,
	Length maxDeviationLength, 
	Angle maxDeviationAngle, 
	common::Interval<Length> sampleDistanceLimits )
{
	if( !originalTrack.IsValid() )
		return nullptr;

	if( maxDeviationLength <= 0_m )
		return nullptr;

	if( maxDeviationAngle <= 0_rad )
		return nullptr;

	if( sampleDistanceLimits.Near() <= 0_m || !sampleDistanceLimits.Normal() )
		return nullptr;

	if( auto pTrack = ParallelizableTrack::Make() )
	{
		pTrack->SetFrame( originalTrack.GetFrame() );
		pTrack->Attach( originalTrack.GetCurve() );
		pTrack->Attach( originalTrack.GetTwist().Clone() );

		if( auto pParallelTrack = pTrack->AddParallel( shift, bShiftRelativeToTwistOrUp ) )
		{
			if( auto pNewTrack = TrackBuilder::Make() )
			{
				if( std::shared_ptr<SampledCurve> pSampledCurve = SampledCurve::Make() )
				{		
					const PointToPointParallel& parallelCurve = static_cast<const PointToPointParallel&>(*pParallelTrack->GetCurve().first);
					
					pNewTrack->Attach( pSampledCurve, pSampledCurve->Create(	parallelCurve, 
																				{ parallelCurve.ParallelParameterFrom( range.Near() ),
																				  parallelCurve.ParallelParameterFrom( range.Far() ) },
																				maxDeviationLength, 
																				maxDeviationAngle, 
																				sampleDistanceLimits ) );

					PiecewiseTwist::Data twistdata;
					twistdata.reserve( pSampledCurve->GetData().size() );

					for( auto iter = pSampledCurve->GetData().begin(); iter < pSampledCurve->GetData().end(); ++iter )
					{
						Frame<Length,One> F, Fw;
						const Length s = iter->s;
						pParallelTrack->Transition( s , Fw );

						auto zeroSet = pSampledCurve->ZeroSet();
						if( std::find( zeroSet.begin(), zeroSet.end(), s ) != zeroSet.end() )
						{
							if( s - epsilon__length >= 0_m )
							{
								pNewTrack->Transition( s - epsilon__length, F );
								twistdata.push_back( { s - epsilon__length, -std::atan2( Fw.B * F.N, Fw.B * F.B ) });
							}
						}

						pNewTrack->Transition( s , F );
						twistdata.push_back( { s, -std::atan2( Fw.B * F.N, Fw.B * F.B ) });		//see DirectionalTwist_Imp
					}

					auto pTwist = PiecewiseTwist::Make( RoadwayTwist::TwistType::PiecewiseCircular );
					pTwist->Create( twistdata );
					pNewTrack->Attach( std::move(pTwist) );

					//Length ds = 1_cm;
					//for( Length s = 0_m; s < pNewTrack->GetLength(); s += ds )
					//{
					//	Frame<Length,One> F;
					//	pNewTrack->Transition( s , F );

					//	if( F.B * Up < cos( 10_deg ) ){
					//		Angle a = acos( F.B * Up );
					//		std::cout << "Suspicious twist on track: s(" << s << "), a(" << _deg << a << ")" << std::endl;
					//	}
					//}

					return pNewTrack;
				}
			}
		}
	}

	return nullptr;
}

std::shared_ptr<TrackBuilder> MakeParallelTrackWithSpline(
	const TrackBuilder& originalTrack, 
	common::Interval<Length> range,
	spat::Vector2D<Length> shift,
	bool bShiftRelativeToTwistOrUp,
	spat::Vector<One> up,
	Length maxDeviationLength,
	common::Interval<Length> sampleDistanceLimits )
{
	if( !originalTrack.IsValid() )
		return nullptr;

	if( auto pTrack = ParallelizableTrack::Make() )
	{
		pTrack->SetFrame( originalTrack.GetFrame() );
		pTrack->Attach( originalTrack.GetCurve() );
		pTrack->Attach( originalTrack.GetTwist().Clone() );

		if( auto pParallelTrack = pTrack->AddParallel( shift, bShiftRelativeToTwistOrUp ) )
		{
			if( auto pNewTrack = TrackBuilder::Make() )
			{
				if( std::shared_ptr<Spline> pSpline = Spline::Make() )
				{
					const PointToPointParallel& parallelCurve = static_cast<const PointToPointParallel&>(*pParallelTrack->GetCurve().first);
					Interval<Length> parallelRange = { parallelCurve.ParallelParameterFrom( range.Near() ), parallelCurve.ParallelParameterFrom( range.Far() ) };
					pNewTrack->Attach( pSpline, pSpline->Create( parallelCurve, parallelRange, maxDeviationLength, sampleDistanceLimits ) );
					pNewTrack->Attach( DirectionalTwist::Make(up) );
					Interval<Length> approximationRange = pNewTrack->Range();

					PiecewiseTwist::Data twistdata;
					twistdata.reserve( pSpline->CountControlPoints() );

					for( int idx = 0; idx < pSpline->CountControlPoints(); ++idx )
					{
						const Length s = pSpline->GetParameter( idx );
						Frame<Length,One> F, Fw;
						pParallelTrack->Transition( parallelRange.Length() / approximationRange.Length() * s, Fw );
						pNewTrack->Transition( s , F );
						twistdata.push_back( { s, -std::atan2( Fw.B * F.N, Fw.B * F.B ) });		//see DirectionalTwist_Imp
					}

					std::unique_ptr<CombinedTwist> pNewTwist = CombinedTwist::Make();
					pNewTwist->AttachTwist1( DirectionalTwist::Make(up) );
					if( twistdata.size() == 2 ){
						pNewTwist->AttachTwist2( LinearTwist::Make( twistdata.front().second, twistdata.back().second ) );
					}
					else{
						auto pPiecewiseTwist = PiecewiseTwist::Make( RoadwayTwist::TwistType::PiecewiseCircular );
						pPiecewiseTwist->Create( twistdata );
						pNewTwist->AttachTwist2( std::move(pPiecewiseTwist) );
					}			
					pNewTrack->Attach( std::move(pNewTwist) );

					return pNewTrack;
				}
			}
		}
	}

	return nullptr;
}

//std::shared_ptr<TrackBuilder> MakeParallelTrackWithSpline(
//	const TrackBuilder& originalTrack, 
//  const Factory& factory,
//	common::Interval<Length> range,
//	spat::Vector2D<Length> shift,
//	bool bShiftRelativeToTwistOrUp,
//	spat::Vector<One> up,
//	Angle maxDeltaAnglePerSection,
//	common::Interval<Length> sampleDistanceLimits )
//{
//	if( !originalTrack.IsValid() )
//		return nullptr;
//
//	if( sampleDistanceLimits.Near() <= 0_m || !sampleDistanceLimits.Normal() )
//		return nullptr;
//
//	if( maxDeltaAnglePerSection <= 0_1 )
//		return nullptr;
//
//
//	if( auto pTrack = Factory::Instance().CreateTrack( originalTrack.GetTrackType() ) ){
//		if( std::shared_ptr<Spline> pSpline = Spline::Make() )
//		{
//			Length ds = epsilon__length;
//
//			using Data = std::vector<std::pair<spat::VectorBundle2<Length,One>,One>>;
//			Data data;
//			data.reserve( common::narrow_cast<size_t>(std::ceil( abs(range.Length()) / ds )) + 1 );
//
//			const auto EvaluateParallel =
//				[&originalTrack,&data,&shift,ds]( Length s )->Length{
//					const auto bundle = Parallel( originalTrack, s, shift );
//					const auto tangent = spat::Normalize( bundle.T );
//					data.push_back( { { bundle.P, tangent.second, bundle.N}, tangent.first } );
//					return ds;
//				};
//			
//			const auto EvaluateParallelToUp =
//				[&originalTrack,&data,&shift,ds,&up]( Length s )->Length{
//					const auto bundle = ParallelToUp( originalTrack, s, shift, up );
//					const auto tangent = spat::Normalize( bundle.T );
//					data.push_back( { { bundle.P, tangent.second, bundle.N}, tangent.first } );
//					return ds;
//				};
//
//			if( bShiftRelativeToTwistOrUp )
//				common::Sample( range, EvaluateParallel );
//			else
//				common::Sample( range, EvaluateParallelToUp );
//
//			std::vector<spat::VectorBundle<Length,One>> controlPoints;
//			std::vector<spat::Vector<One>> upVectors;
//			Angle totalAngleOfParallel = 0_rad;
//			Length Parallel_delta_s = 0_m;
//
//			Data::const_iterator iterLast = data.cbegin(), iterNextToLast = data.cbegin();
//			controlPoints.push_back( data.front().first );
//			upVectors.push_back( data.front().first.N );
//
//
//			// TODO: have control points at the zeroes...
//			//std::vector<Length> zeroSet = originalTrack.GetCurve().first->ZeroSet();
//			//for( Length s : zeroSet )
//
//
//
//			for( Data::const_iterator iter = data.cbegin(); iter != data.cend() - 1; ++iter ){
//				Parallel_delta_s += iter->second * ds;
//				const Angle da = ((iter+1)->first.T - iter->first.T).Length();
//				if( (totalAngleOfParallel += da) > maxDeltaAnglePerSection && Parallel_delta_s >= sampleDistanceLimits.Near() ){
//					controlPoints.push_back( iter->first );
//					upVectors.push_back( iter->first.N );
//					iterNextToLast = iterLast;
//					iterLast = iter;
//					totalAngleOfParallel = da;
//					Parallel_delta_s = 0_m;
//				}
//			}
//			controlPoints.push_back( data.back().first );
//			upVectors.push_back( data.back().first.N );
//			if( controlPoints.size() >= 3 )
//			// the control point next to last might be very close to last, move it to the middle:
//			{
//				Data::const_iterator midPointIter = iterNextToLast + (data.cend() - iterNextToLast) / 2;
//				*(controlPoints.end() - 2) = midPointIter->first;
//				*(upVectors.end() - 2) = midPointIter->first.N;
//			}
//
//			pTrack->Attach( pSpline, pSpline->CreateCatmullRom( 
//				controlPoints, 0.5f ) );
//
//			PiecewiseTwist::Data twistdata;
//			twistdata.reserve( controlPoints.size() );
//			for( int idx = 0; idx < common::narrow_cast<int>(controlPoints.size()); ++idx ){
//				spat::Frame<Length,One> F;
//				const Length s = pSpline->GetParameter(idx);
//				pSpline->Transition( s, F );
//				const spat::Vector<One>& FwB = upVectors.at(idx);
//				twistdata.push_back( { s, -std::atan2( FwB * F.N, FwB * F.B ) } ); 		//see DirectionalTwist_Imp
//			}
//
//			auto pTwist = FactoryBase::CreatePiecewiseLinearTwist();
//			pTwist->Create( twistdata );
//			pTrack->Attach( std::move(pTwist) );
//			//pTrack->Attach( originalTrack.GetTwist().Clone() );
//
//			return pTrack;
//		}
//	}
//
//	return nullptr;
//}
///////////////////////////////////////
}
//	trax track library
//	AD 2013 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//
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

#include "Track_Imp.h"

#include "trax/Connector.h"
#include "trax/Sensor.h"
#include "trax/Signal.h"
#include "trax/TrackData.h"

#include "SectionTrack_Imp.h"
#include "RoadwayTwist_Imp.h"
////#include "Parallel_Imp.h"

#include "dim/support/DimSupportStream.h"

#include <algorithm>
#include <iostream>


namespace trax{
	using namespace spat;

std::shared_ptr<TrackBuilder> TrackBuilder::Make( TrackType type ) noexcept
{
	try{
		std::shared_ptr<Track_Imp> pRetval;

		switch( type ){
		case TrackType::standard:
			pRetval = std::make_shared<Track_Imp>();
			break;
		case TrackType::movable:
	//		pRetval = std::make_shared<MovableTrack_Imp>();
			break;
		case TrackType::withGeoms:
			pRetval = std::make_shared<SectionTrack_Imp>();
			break;
		case TrackType::parallel:
		//	pRetval = std::make_shared<ParallelizableTrack_Imp>();
			break;
		case TrackType::unknown:
		case TrackType::none:
		case TrackType::movable_autoconnecting:
		default:
			break;
		}
		
		if( pRetval )
			pRetval->SetWeakPointerToSelf( pRetval );
	
		return pRetval;
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

//TrackBuilder& TrackBuilder::operator=( const TrackBuilder& source ) noexcept
//{
//	SetFrame( source.GetFrame() );
//	Attach( source.GetCurve() );
//	Attach( source.GetTwist().Clone() );
//
//	return *this;
//}

Track_Imp::Track_Imp() noexcept
	:	m_pTwist			{std::make_unique<ConstantTwist_Imp>()},
		m_pConnectorFront	{nullptr},
		m_pConnectorEnd		{nullptr},
		m_LoopBraker		{false}
{
	m_TotalFrame.Init();
}

Track_Imp::Track_Imp( const Track_Imp& ti )
	:	m_pCurve			{ti.m_pCurve},
		m_CurveSegment		{ti.m_CurveSegment},
		m_pTwist			{ti.m_pTwist->Clone()},
		m_TotalFrame		{ti.m_TotalFrame},
		m_pConnectorFront	{nullptr},
		m_pConnectorEnd		{nullptr},
		m_LoopBraker		{false}
{
	// no copy of m_pThis or the couplings etc.
}

void Track_Imp::SetWeakPointerToSelf( std::weak_ptr<Track_Imp> pThis ) noexcept{
	m_pThis = pThis;
}

void Track_Imp::AddConnector( Connector* pConnector, EndType atend ) noexcept{
	switch( atend ){
	case EndType::any:
	case EndType::front:
		m_pConnectorFront = pConnector;
		break;
	case EndType::end:
		m_pConnectorEnd = pConnector;
		break;
	case EndType::none:
	default:
		return;
	}
}

void Track_Imp::PropagateAbsoluteFrameToClients() noexcept{
	TrackBase::PropagateAbsoluteFrameToClients();
	m_TotalFrame = GetAbsoluteFrame() * GetFrame();
	try{
		m_pTwist->OnDetach();
		m_pTwist->OnAttach( *this );
	}
	catch( const std::exception& ){
	}
}

const char* Track_Imp::TypeName() const noexcept{
	return ToString( GetTrackType() );
}

Track::TrackType Track_Imp::GetTrackType() const noexcept{
	return TrackType::standard;
}

std::shared_ptr<TrackBuilder> Track_Imp::This() const noexcept{
	return m_pThis.lock();
}

std::shared_ptr<MovableTrack> Track_Imp::GetMovableTrack() const noexcept{
	return nullptr;
}

std::shared_ptr<ParallelizableTrack> Track_Imp::GetParallelizableTrack() const noexcept{
	return nullptr;
}


//TODO check unhandled throw
// it's very slowly. it's very slowly. it's very slowly. it's very slowly. it's very slowly. it's very slowly.  tralala BCA
void Track_Imp::SetFrame( 
	const spat::Frame<Length,One>& S, 
	Length s, 
	bool bAllowTwistOffset )
// If we have a frame F from the curve at parameter s, the normal 
// transformation of it would be:
//
// Ft = Tg o Tf o Tt o F	(Tg: AbsoluteFrame, Tf: Frame, Tt twist transformation)
//
// This Ft is supposed to be S at that very point s. So multiplication with
// inverse transformations gives for Tf:
//
// Tf = Inv(Tg) o S o Inv(Tt o F)
{
	if( !IsValid() )
		throw std::runtime_error( "Track not valid!" );
	//spat::OrthogonalityDump( std::cout, start );
	assert( S.IsOrthoNormal(epsilon__length/Length{1},epsilon__angle) );

	TestTransition( s );

	Frame<Length,One> base;
	m_pCurve->Transition( c(s), base );
	if( !m_CurveSegment.Normal() ){
		base.T *= -1;
		base.B *= -1;
	}

	const Angle twist = m_pTwist->Twist( s );
	base.RotateTan( twist );

	Frame<Length,One> frame{	Transformation<One>{ GetAbsoluteFrame() }.Invert() * 
								Transformation<One>{ S } * 
								Transformation<One>{ base }.Invert() };
	if( !frame.IsOrthoNormal() )
		frame.OrthoNormalize();

	const spat::Frame<Length,One> oldFrame{ GetFrame() };
	SetFrame( frame );

	auto offset = common::Wrap( twist - m_pTwist->Twist( s ), -pi, +pi ); 
	if( abs(offset) > epsilon )
	// sometimes the twist is not independend of the frame.
	{
		if( bAllowTwistOffset )
		// I think we can correct the resulting error here ...
		{
			if( !m_pTwist->Offset( offset ) ){
				Transition( s, base );
				if( !base.Equals( S, epsilon__length, epsilon__angle ) ){
					SetFrame( oldFrame );
					throw std::logic_error{ "TrackBuilder::SetFrame() was called with a RoadwayTwist configuration that can not guarantee the method's promise!" };
				}
			}
		}
		else{
			Transition( s, base );
			if( !base.Equals( S, epsilon__length, epsilon__angle ) ){
				SetFrame( oldFrame );
				throw std::logic_error{ "TrackBuilder::SetFrame() was called with contradictive frame and twist settings!" };
			}
		}
	}

#ifdef _DEBUG
		Transition( s, base );
		assert( base.Equals( S, epsilon__length, epsilon__angle ) );
#endif
}

bool Track_Imp::IsValid() const noexcept{
	return  GetFrame().IsOrthoNormal() &&
			GetAbsoluteFrame().IsOrthoNormal() &&
			m_pCurve && m_pCurve->IsValid() &&
			m_pTwist && m_pTwist->IsValid();
}

Length Track_Imp::GetLength() const noexcept{
	return abs(m_CurveSegment.Length());
}

Interval<Length> Track_Imp::Range() const noexcept{
	return { 0_m, GetLength() };
}

Track::TrackEnd Track_Imp::TransitionEnd( EndType thisEnd ) const noexcept{
	if( thisEnd == EndType::front )
		return m_TrackFront;

	if( thisEnd == EndType::end )
		return m_TrackEnd;

	return std::make_pair( nullptr, EndType::none );
}

bool Track_Imp::IsCoupled( EndType atend ) const noexcept{
	if( atend == EndType::any )
		return IsCoupled( EndType::front ) || IsCoupled( EndType::end );

	return (atend == EndType::front && m_TrackFront.first) || (atend == EndType::end && m_TrackEnd.first);
}

void Track_Imp::TNBFrame( Length s, Frame<Length,One>& frame ) const{
	TestTransition( s );

	m_pCurve->Transition( c(s), frame );

	if( !m_CurveSegment.Normal() ){
		frame.T *= -1;
		frame.B *= -1;
	}

	frame = m_TotalFrame * frame;
}

AnglePerLength Track_Imp::Curvature( Length s ) const{
	TestTransition( s );
	return m_pCurve->Curvature( c(s) );
}

AnglePerLength Track_Imp::Torsion( Length s ) const{
	TestTransition( s );
	return m_pCurve->Torsion( c(s) ) + m_pTwist->D1( s );
}

Angle Track_Imp::GetTwistValue( Length s ) const{
	if( m_pTwist )
		return m_pTwist->Twist( s );

	return 0;
}

AnglePerLength Track_Imp::GetTwistD1( Length s ) const{
	if( m_pTwist )
		return m_pTwist->D1( s );

	return 0_1Im;
}

bool Track_Imp::IsFlat() const noexcept{
	if( m_pCurve && m_pTwist )
		return m_pCurve->IsFlat() && m_pTwist->IsNull();

	return false;
}

spat::Vector<One> Track_Imp::LocalUp() const{
	if( m_pCurve )
		return m_pCurve->LocalUp();

	return spat::Null<One>;
}

void Track_Imp::Transition( Length s, TrackData<Real>& td, Real engine_meters_per_unit ) const{
	Frame<Length,One> F, wF;
	TNBFrame( s, F );
	Transition( s, wF );

	td.F.P.x = _m(F.P.x) / engine_meters_per_unit;
	td.F.P.y = _m(F.P.y) / engine_meters_per_unit;
	td.F.P.z = _m(F.P.z) / engine_meters_per_unit;
	td.F.T = spatial_cast<Real>(F.T);
	td.F.N = spatial_cast<Real>(F.N);
	td.F.B = spatial_cast<Real>(F.B);

	td.wF.P.x = _m(wF.P.x) / engine_meters_per_unit;
	td.wF.P.y = _m(wF.P.y) / engine_meters_per_unit;
	td.wF.P.z = _m(wF.P.z) / engine_meters_per_unit;
	td.wF.T = spatial_cast<Real>(wF.T);
	td.wF.N = spatial_cast<Real>(wF.N);
	td.wF.B = spatial_cast<Real>(wF.B);

	td.c = _1Im(Curvature( s )) * engine_meters_per_unit;
	td.t = _1Im(Torsion( s )) * engine_meters_per_unit;
}

void Track_Imp::Transition( Length s, Position<Length>& pos ) const{
	TestTransition( s );

	m_pCurve->Transition( c(s), pos );

	pos = m_TotalFrame * pos;
}

void Track_Imp::Transition( Length s, Vector<One>& tan ) const{
	TestTransition( s );

	m_pCurve->Transition( c(s), tan );

	if( !m_CurveSegment.Normal() )
		tan *= -1;

	tan = m_TotalFrame * tan;
}

void Track_Imp::Transition( Length s, VectorBundle<Length,One>& bundle ) const{
	TestTransition( s );

	m_pCurve->Transition( c(s), bundle );

	if( !m_CurveSegment.Normal() )
		bundle.T *= -1;

	bundle = m_TotalFrame * bundle;
}

void Track_Imp::Transition( Length s, VectorBundle2<Length,One>& bundle ) const{
	TestTransition( s );

	m_pCurve->Transition( c(s), bundle );

	if( !m_CurveSegment.Normal() )
		bundle.T *= -1;

	if( const Angle twist = m_pTwist->Twist( s ) )
		bundle.RotateTan( twist );

	bundle = m_TotalFrame * bundle;
}

void Track_Imp::Transition( Length s, Frame<Length,One>& frame ) const{
	TestTransition( s );

	m_pCurve->Transition( c(s), frame );
	if( !m_CurveSegment.Normal() ){
		frame.T *= -1;
		frame.B *= -1;
	}

	if( const Angle twist = m_pTwist->Twist( s ) )
		frame.RotateTan( twist );

	frame = m_TotalFrame * frame;
}

void Track_Imp::DoTrigger( const Interval<Length>& range, const Event& _event ) const{
	if( range.Normal() ){
		for( auto iter = std::find_if( m_Sensors.begin(), m_Sensors.end(),
						[&range]( const std::pair<TrackLocation,std::shared_ptr<Sensor>>& pair ) noexcept->bool
						{
							if( pair.first.orientation == Orientation::Value::anti )
								return false;

							return pair.first.parameter >= range.Near();
						} );
				iter != m_Sensors.end() && iter->first.parameter < range.Far();
				++iter )
		{
			if( iter->first.orientation == Orientation::Value::anti )
				continue;

			iter->second->Trigger( _event );
		}
	}
	else{
		for( auto iter = std::find_if( m_Sensors.rbegin(), m_Sensors.rend(),
						[&range]( const std::pair<TrackLocation,std::shared_ptr<Sensor>>& pair ) noexcept->bool
						{
							if( pair.first.orientation == Orientation::Value::para )
								return false;

							return pair.first.parameter <= range.Near();
						} );
				iter != m_Sensors.rend() && iter->first.parameter > range.Far(); 
				++iter )
		{
			if( iter->first.orientation == Orientation::Value::para )
				continue;

			iter->second->Trigger( _event );
		}
	}
}

bool Track_Imp::DoSignal( const Interval<Length>& range, Orientation orientation, SignalTarget& signalTarget ) const{
	for( const auto& pair : m_Signals )
		if( pair.first.m_Orientation == orientation && Intersecting( pair.first, range ) ){
			if( !signalTarget.Notify( 
				*pair.second,
				-pair.second->GetLocation().Distance( Location{ m_pThis.lock(), TrackLocation{ range.m_Far, orientation } }, -1_km ) ) )
				return false;
		}

	return true;
}

Signal* Track_Imp::GetSignal( const TrackLocation& loc ) const noexcept{
	if( m_LoopBraker ){
		m_LoopBraker = false;
		return nullptr;
	}

	for( const auto& pair : m_Signals )
		if( pair.first.m_Orientation == loc.orientation && Intersecting( pair.first, Interval<Length>{ loc.parameter, loc.orientation ? GetLength() : 0_m } ) )
			return pair.second.get();

	if( TrackEnd nextTrackEnd = TransitionEnd( loc.orientation ? EndType::end : EndType::front ); nextTrackEnd.first ){
		m_LoopBraker = true;
		Signal* pSignal = nextTrackEnd.first->GetSignal( nextTrackEnd.second == EndType::front ? TrackLocation{ 0_m, Orientation::Value::para } : TrackLocation{ nextTrackEnd.first->GetLength(), Orientation::Value::anti } );
		m_LoopBraker = false;
		return pSignal;
	}

	return nullptr;
}

void Track_Imp::Reserve( Interval<Length> range, IDType forID )
{
	if( forID < IDType{1u} )
		return;
	if( range.Length() == 0_m )
		return;
	if( !Intersecting(Range(),range) ){
		if( Touching(Range(),range) )
			return;
		throw std::range_error( "Reservation outside of this track!" );
	}

	for( auto& tuple : m_Reservations )
		if( std::get<0>(tuple) == forID && 
			std::get<1>(tuple).Normal() == range.Normal() &&
			Intersecting(std::get<1>(tuple),range) )
		{
			std::get<1>(tuple).Union( range );
			ReserveConnected( tuple );
			return;
		}

	m_Reservations.push_back( std::make_tuple( forID, range ) );
	ReserveConnected( m_Reservations.back() );
}

Track::ReservationIterator Track_Imp::BeginReservations() const noexcept{
	return m_Reservations.cbegin();
}

Track::ReservationIterator Track_Imp::EndReservations() const noexcept{
	return m_Reservations.cend();
}

void Track_Imp::ReserveConnected( const Reservation& reservation )
{
	if( std::get<1>(reservation).Includes(0_m) && !m_ReserveLoopBrakerFront ){
		common::FlagBlocker block(m_ReserveLoopBrakerFront);
		if( TrackEnd trackEnd = TransitionEnd( EndType::front ); trackEnd.first )
			trackEnd.first->Reserve( 
				trackEnd.second == EndType::front ? -std::get<1>(reservation) : trackEnd.first->GetLength() + std::get<1>(reservation),
				std::get<0>(reservation) );
	}

	if( std::get<1>(reservation).Includes(GetLength()) && !m_ReserveLoopBrakerEnd ){
		common::FlagBlocker block(m_ReserveLoopBrakerEnd);
		if( TrackEnd trackEnd = TransitionEnd( EndType::end ); trackEnd.first )
			trackEnd.first->Reserve( 
				trackEnd.second == EndType::front ? std::get<1>(reservation) - GetLength() : GetLength() + trackEnd.first->GetLength() - std::get<1>(reservation),
				std::get<0>(reservation) );
	}
}

void Track_Imp::DeleteReservation( Interval<Length> range, IDType forID ) noexcept
{
	if( m_ReserveLoopBrakerFront || m_ReserveLoopBrakerEnd )
		return;

	auto iter = m_Reservations.begin();
	while( iter != m_Reservations.end() ){
		if( (forID == anyID || forID == std::get<0>(*iter)) && 
			Intersecting(std::get<1>(*iter),range) )
		{
			//if( (std::get<1>(*iter).Touches( GetLength() ) && !std::get<1>(*iter).Touches( 0_m )) ||
			//	(std::get<1>(*iter).Touches( 0_m ) && !std::get<1>(*iter).Touches( GetLength() )) )
			//{




			//	if( auto coda = std::find_if( m_Reservations.begin(), m_Reservations.end(), [iter]( const Reservation& reservation )
			//	{
			//		return	&reservation != &*iter && 
			//				std::get<0>(reservation) == std::get<0>(*iter) &&
			//				abs(std::get<1>(reservation).Length() - std::get<1>(*iter).Length()) < epsilon__length;
			//	}); coda != m_Reservations.end() )
			//	{
			//		if( (std::get<1>(*iter).Touches( GetLength() ) && !(std::get<1>(*iter).Touches( 0_m ) ){
			//			Location location{ *this, {GetLength(),range.Normal()} };
			//			location.Move( range.Length() );
			//			if( location.GetTrack().get() == this && location.Orient() == range.Normal() )
			//				DeleteReservation( {0_m,0_m}, forID );
			//		}
			//		else if( (std::get<1>(*iter).Touches( 0_m ) && !(std::get<1>(*iter).Touches( GetLength() ) ){
			//			Location location{ *this, {0_m,range.Normal()} };
			//			location.Move( -range.Length() );
			//			if( location.GetTrack().get() == this && location.Orient() == range.Normal() )
			//				DeleteReservation( {GetLength(),GetLength()}, forID );
			//		}


			//	}



			DeleteConnected( *iter );
			iter = m_Reservations.erase( iter );
			continue;
		}
		
		++iter;
	}


	//if( range.Length() > 0_m )
	//// On cyclic connected tracks, there might be a coda left at the other
	//// end of the track:
	//{
	//	if( range.Touches( GetLength() ) && !range.Touches( 0_m ) ){
	//		Location location{ *this, {GetLength(),range.Normal()} };
	//		location.Move( range.Length() );
	//		if( location.GetTrack().get() == this && location.Orient() == range.Normal() )
	//			DeleteReservation( {0_m,0_m}, forID );
	//	}
	//	else if( range.Touches( 0_m ) && !range.Touches( GetLength() ) ){
	//		Location location{ *this, {0_m,range.Normal()} };
	//		location.Move( -range.Length() );
	//		if( location.GetTrack().get() == this && location.Orient() == range.Normal() )
	//			DeleteReservation( {GetLength(),GetLength()}, forID );
	//	}
	//}
}

void Track_Imp::DeleteConnected( const Reservation& reservation ) noexcept
{
	if( std::get<1>(reservation).Includes(0_m) && !m_ReserveLoopBrakerFront ){
		common::FlagBlocker block(m_ReserveLoopBrakerFront);

		if( m_pConnectorFront )
		// Propagate deletion request to all connected tracks, because since
		// reservation the connector might have switched connections.
		{
			for( int i = 0; i < m_pConnectorFront->CntSlots(); ++i ){
				auto slotContent = m_pConnectorFront->Slot( i ); 
				if( slotContent.first && 
					slotContent.first.get() != this )
					slotContent.first->DeleteReservation( 				
						slotContent.second == EndType::front ? -std::get<1>(reservation) : slotContent.first->GetLength() + std::get<1>(reservation), 			
						std::get<0>(reservation) );
			}
		}
		else{
			if( TrackEnd trackEnd = TransitionEnd( EndType::front ); trackEnd.first )
				trackEnd.first->DeleteReservation( 
					trackEnd.second == EndType::front ? -std::get<1>(reservation) : trackEnd.first->GetLength() + std::get<1>(reservation),
					std::get<0>(reservation) );
		}
	}

	if( std::get<1>(reservation).Includes(GetLength()) && !m_ReserveLoopBrakerEnd ){
		common::FlagBlocker block(m_ReserveLoopBrakerEnd);

		if( m_pConnectorEnd )
		// Propagate deletion request to all connected tracks, because since
		// reservation the connector might have switched connections.
		{
			for( int i = 0; i < m_pConnectorEnd->CntSlots(); ++i ){
				auto slotContent = m_pConnectorEnd->Slot( i ); 
				if( slotContent.first && 
					slotContent.first.get() != this )
					slotContent.first->DeleteReservation( 				
						slotContent.second == EndType::front ? std::get<1>(reservation) - GetLength() : GetLength() + slotContent.first->GetLength() - std::get<1>(reservation),			
						std::get<0>(reservation) );
			}
		}
		else{
			if( TrackEnd trackEnd = TransitionEnd( EndType::end ); trackEnd.first )
				trackEnd.first->DeleteReservation( 
					trackEnd.second == EndType::front ? std::get<1>(reservation) - GetLength() : GetLength() + trackEnd.first->GetLength() - std::get<1>(reservation),
					std::get<0>(reservation) );
		}
	}
}

bool Track_Imp::IsReserved( Interval<Length> range, IDType forID ) const noexcept
{
	for( const auto& tuple : m_Reservations )
		if( (forID == anyID || std::get<0>(tuple) == forID) && 
			Intersecting(std::get<1>(tuple),range) )
			return true;

	return false;
}

std::vector<Track::Overlap> Track_Imp::Overlaps( IDType withID ) const
{
	std::vector<Track::Overlap> overlaps;

	if( !m_LoopBraker ){
		for( const auto& tuple1 : m_Reservations )
			if( std::get<0>(tuple1) == withID )
			{
				for( const auto& tuple2 : m_Reservations )
					if( tuple1 != tuple2 &&
						Intersecting( std::get<1>(tuple1), std::get<1>(tuple2) ) )
						overlaps.push_back( {	std::get<0>(tuple1),
												std::get<1>(tuple2).Includes( std::get<1>(tuple1).Near() ),
												std::get<1>(tuple2).Includes( std::get<1>(tuple1).Far() ),
												std::get<0>(tuple2),
												std::get<1>(tuple1).Includes( std::get<1>(tuple2).Near() ),
												std::get<1>(tuple1).Includes( std::get<1>(tuple2).Far() ) } );

				common::FlagBlocker block{m_LoopBraker};

				if( TrackEnd lastTrack = TransitionEnd( EndType::front ); lastTrack.first ){
					std::vector<Track::Overlap> overlapsLast = lastTrack.first->Overlaps( withID );
					overlaps.insert( overlaps.end(), overlapsLast.begin(), overlapsLast.end() );
				}

				if( TrackEnd nextTrack = TransitionEnd( EndType::end ); nextTrack.first ){
					std::vector<Track::Overlap> overlapsNext = nextTrack.first->Overlaps( withID );
					overlaps.insert( overlaps.end(), overlapsNext.begin(), overlapsNext.end() );
				}
			}

		//remove duplicates...
		std::sort( overlaps.begin(), overlaps.end() );
		overlaps.erase( std::unique( overlaps.begin(), overlaps.end() ), overlaps.end() );
	}
	
	return overlaps;
}

void Track_Imp::Couple( 
	std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> thisEnd, 
	std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> othersEnd  )
{
	if( thisEnd.first.get() != this )
		throw std::logic_error( "pThisTrack has to point to this." );

	if( thisEnd == othersEnd )
		throw std::logic_error( "Can not couple track coupling to itself!" );

	if( thisEnd.second == Track::EndType::any || othersEnd.second == Track::EndType::any )
		throw std::invalid_argument( "Unspecific end type to couple; use Track::EndType::front or Track::EndType::end." );

	if( thisEnd.second == EndType::front ){
		if( (m_TrackFront.first = std::dynamic_pointer_cast<Track_Imp>(othersEnd.first)) != nullptr ){
			if( othersEnd.second == EndType::front ){
				m_TrackFront.first->m_TrackFront.first = std::dynamic_pointer_cast<Track_Imp>(thisEnd.first);
				m_TrackFront.first->m_TrackFront.second = EndType::front;

				if( m_TrackFront.first->m_pTETFront )
					m_TrackFront.first->CreateEndTransitionSignal( othersEnd.second );
			}
			else if( othersEnd.second == EndType::end ){
				m_TrackFront.first->m_TrackEnd.first = std::dynamic_pointer_cast<Track_Imp>(thisEnd.first);
				m_TrackFront.first->m_TrackEnd.second = EndType::front;

				if( m_TrackFront.first->m_pTETEnd )
					m_TrackFront.first->CreateEndTransitionSignal( othersEnd.second );
			}
			
			m_TrackFront.second = othersEnd.second;

			if( m_pTETFront )
				CreateEndTransitionSignal( EndType::front );
		}
	}
	else if( thisEnd.second == EndType::end ){
		if( (m_TrackEnd.first = std::dynamic_pointer_cast<Track_Imp>(othersEnd.first)) != nullptr ){
			if( othersEnd.second == EndType::front ){
				m_TrackEnd.first->m_TrackFront.first = std::dynamic_pointer_cast<Track_Imp>(thisEnd.first);
				m_TrackEnd.first->m_TrackFront.second = EndType::end;

				if( m_TrackEnd.first->m_pTETFront )
					m_TrackEnd.first->CreateEndTransitionSignal( EndType::front );
			}
			else if( othersEnd.second == EndType::end ){
				m_TrackEnd.first->m_TrackEnd.first = std::dynamic_pointer_cast<Track_Imp>(thisEnd.first);
				m_TrackEnd.first->m_TrackEnd.second = EndType::end;

				if( m_TrackEnd.first->m_pTETEnd )
					m_TrackEnd.first->CreateEndTransitionSignal( EndType::end );
			}

			m_TrackEnd.second = othersEnd.second;

			if( m_pTETEnd )
				CreateEndTransitionSignal( EndType::end );
		}
	}
}

void Track_Imp::DeCouple( EndType thisend, bool oneSided ){
	if( thisend == EndType::any ){
		DeCouple( EndType::front, oneSided );
		DeCouple( EndType::end, oneSided );
		return;
	}

	if( thisend == EndType::front ){
		if( m_TrackFront.first ){
			if( !oneSided ){
				if( m_TrackFront.second == EndType::front ){
					m_TrackFront.first->m_TrackFront.first.reset();

					if( m_TrackFront.first->m_pTETFront )
						m_TrackFront.first->CreateEndTransitionSignal( EndType::front );
				}
				else if( m_TrackFront.second == EndType::end ){
					m_TrackFront.first->m_TrackEnd.first.reset();

					if( m_TrackFront.first->m_pTETEnd )
						m_TrackFront.first->CreateEndTransitionSignal( EndType::end );
				}
			}

			m_TrackFront.first.reset();

			if( m_pTETFront )
				CreateEndTransitionSignal( EndType::front );
		}
	}
	else if( thisend == EndType::end ){
		if( m_TrackEnd.first ){
			if( !oneSided ){
				if( m_TrackEnd.second == EndType::front ){
					m_TrackEnd.first->m_TrackFront.first.reset();

					if( m_TrackEnd.first->m_pTETFront )
						m_TrackEnd.first->CreateEndTransitionSignal( EndType::front );
				}
				else if( m_TrackEnd.second == EndType::end ){
					m_TrackEnd.first->m_TrackEnd.first.reset();

					if( m_TrackEnd.first->m_pTETEnd )
						m_TrackEnd.first->CreateEndTransitionSignal( EndType::end );
				}
			}

			m_TrackEnd.first.reset();

			if( m_pTETEnd )
				CreateEndTransitionSignal( EndType::end );
		}
	}
}

void Track_Imp::Attach( std::shared_ptr<const Curve> pCurve, common::Interval<Length> curveSegment ){
	if( abs(curveSegment.Near()) == infinite__length )
		throw std::invalid_argument( "The near value for the curve segment can not be infinite on attaching a curve to a track!" );

#ifdef _DEBUG
	if ((m_pCurve) && (pCurve))
		if (m_pCurve->GetCurveType() != pCurve->GetCurveType())
		{
			std::cout << Verbosity::verbose << m_pCurve->TypeName() <<  " curve changed to: " << pCurve->TypeName() << std::endl;
		}
#endif

	m_pTwist->OnDetach();
	m_pCurve = pCurve;
	m_CurveSegment = curveSegment;

	if( m_pCurve )
		m_CurveSegment.Intersection( m_pCurve->Range() );

	m_pTwist->OnAttach( *this );
	OnGeometryChanged();
}

void Track_Imp::Attach( std::pair<std::shared_ptr<const Curve>, common::Interval<Length>> curve ){
	Attach( curve.first, curve.second );
}

void Track_Imp::Attach( std::pair<std::unique_ptr<const Curve>,common::Interval<Length>>& curve ){
	Attach( std::move(curve.first), curve.second );
}

std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> Track_Imp::DetachCurve() noexcept{
	auto retval = std::make_pair( m_pCurve, m_CurveSegment );
	m_pCurve.reset();
	m_CurveSegment = {-infinite__length,+infinite__length};
	OnGeometryChanged();
	return retval;
}

std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> Track_Imp::GetCurve() const noexcept{
	return std::make_pair( m_pCurve, m_CurveSegment );
}

void Track_Imp::Attach( std::unique_ptr<RoadwayTwist> pTwist ){
	if( !pTwist )
		return;

	m_pTwist->OnDetach();
	m_pTwist = std::move(pTwist);
	m_pTwist->OnAttach( *this );
	OnGeometryChanged();
}

std::unique_ptr<RoadwayTwist> Track_Imp::DetachTwist(){
	m_pTwist->OnDetach();

	std::unique_ptr<RoadwayTwist> retval{ std::move(m_pTwist) };
	m_pTwist.reset( new ConstantTwist_Imp );

	m_pTwist->OnAttach( *this );
	OnGeometryChanged();
	return retval;
}

RoadwayTwist& Track_Imp::GetTwist() const noexcept{
	return *m_pTwist;
}

void Track_Imp::Attach( std::shared_ptr<Sensor> pSensor, const TrackLocation& tl ){
	if( !pSensor )
		throw std::invalid_argument( "Invalid Sensor pointer." );

	if( InvalidParameter( tl.parameter ) ){
		std::ostringstream stream;
		stream << "Parameter out of track range for Sensor (" << pSensor->ID() << ") at track (";
		stream << ID() << ") with length: " << Length() << "m at parameter value of: " << tl.parameter << std::endl;
		throw std::invalid_argument( stream.str().c_str() );
	}

	if( auto pTrack = pSensor->TrackAttached() )
		pTrack->Detach( *pSensor.get() ); // a sensor can be attached to one track only

	auto iter = std::find_if( m_Sensors.begin(), m_Sensors.end(),
		[&tl]( const std::pair<TrackLocation,std::shared_ptr<Sensor>>& pair )->bool
		{
			//if( tl.orientation != pair.first.orientation )
			//	return false;

			return pair.first.parameter > tl.parameter;
		} );

	m_Sensors.insert( iter, std::make_pair(tl,pSensor) );

	pSensor->Attach( this );
}

bool Track_Imp::Attached( const Sensor& sensor, TrackLocation* pTrackLocation ) const{
	auto iter = std::find_if( m_Sensors.begin(), m_Sensors.end(),
		[&sensor]( const std::pair<TrackLocation,std::shared_ptr<Sensor>>& pair ) noexcept
		{
			return pair.second.get() == &sensor;
	} );

	if( iter == m_Sensors.end() )
		return false;

	if( pTrackLocation )
		*pTrackLocation = iter->first;

	return true;
}

void Track_Imp::Detach( const Sensor& sensor ) noexcept{
	auto iter = std::find_if( m_Sensors.begin(), m_Sensors.end(),
		[&sensor]( const std::pair<TrackLocation,std::shared_ptr<Sensor>>& pair ) noexcept
		{
			return pair.second.get() == &sensor;
		} );

	if( iter == m_Sensors.end() )
		return;

	m_Sensors.erase( iter );
}

std::shared_ptr<Sensor> Track_Imp::GetSensor( int idx ) const noexcept{
	if( idx < 0 || static_cast<unsigned>(idx) >= m_Sensors.size() )
		return nullptr;

	return m_Sensors[static_cast<unsigned>(idx)].second;
}

int Track_Imp::CountSensors() const{
	return common::narrow_cast<int>(m_Sensors.size());
}

void Track_Imp::Attach( std::shared_ptr<Signal> pSignal, const Interval<Length>& trackRange ){
	if( !pSignal )
		throw std::invalid_argument( "Invalid Signal pointer." );
	if( !Touching( Range(), trackRange ) )
		throw std::invalid_argument( "trackRange not part of this track." );

	pSignal->Attach( this );

	auto list = GetRanges( trackRange );
	for( const auto& pair : list )
	{
		if( pair.first.Attached( *pSignal ) )
			throw std::logic_error( "Cannot attach a signal to the same track twice." );

		pair.first.m_Signals.push_back( std::make_pair( Track_Imp::OrientedInterval(pair.second), pSignal ) );
	}
}

void Track_Imp::Attach( std::shared_ptr<Signal> pSignal, const TrackLocation& location ){
	if( !pSignal )
		throw std::invalid_argument( "Invalid Signal pointer." );

	Location loc;
	loc.PutOn( m_pThis.lock(), location ); // implicitly resolves the track location

	if( auto pTrack = dynamic_cast<Track_Imp*>(loc.GetTrack().get()) ){
		if( pTrack->Attached( *pSignal ) )
			throw std::logic_error( "Cannot attach a signal to the same track twice." );

		pTrack->m_Signals.push_back( std::make_pair( OrientedInterval{loc.Param(),loc.Param(),loc.Orient()}, pSignal ) );
	}
}

bool Track_Imp::Attached( const Signal& signal, TrackRange* pTrackRange ) const{
	auto iter = std::find_if( m_Signals.begin(), m_Signals.end(),
		[&signal]( const std::pair<Interval<Length>,std::shared_ptr<Signal>>& pair ) noexcept { return pair.second.get() == &signal; } );

	if( iter == m_Signals.end() )
		return false;

	if( pTrackRange ){
		*pTrackRange = TrackRange{ iter->first };
		pTrackRange->refid = ID();
	}

	return true;
}

void Track_Imp::Detach( const Signal& signal ){
	auto iter = std::find_if( m_Signals.begin(), m_Signals.end(),
		[&signal]( const std::pair<OrientedInterval,std::shared_ptr<Signal>>& pair ) noexcept ->bool { return pair.second.get() == &signal; } );

	if( iter != m_Signals.end() ){
		iter->second->Attach( nullptr );

		auto list = GetRanges( iter->first );
		for( const auto& pair : list ){
			for( auto iter2 = pair.first.m_Signals.begin(); iter2 != pair.first.m_Signals.end(); ++iter2 ){
				if( iter2->second.get() == &signal ){
					pair.first.m_Signals.erase( iter2 );
					break;
				}
			}
		}
	}
}

void Track_Imp::Flip( bool flipAttached )
{
	GetTwist().Flip();

	auto curve = DetachCurve();
	curve.second.Flip();
	Attach( curve );

	std::swap( m_TrackFront, m_TrackEnd );
	if( m_TrackFront.first )
		(m_TrackFront.second == EndType::front ? m_TrackFront.first->m_TrackFront.second : m_TrackFront.first->m_TrackEnd.second) = EndType::front;
	if( m_TrackEnd.first )
		(m_TrackEnd.second == EndType::front ? m_TrackEnd.first->m_TrackFront.second : m_TrackEnd.first->m_TrackEnd.second) = EndType::end;

	if( flipAttached ){
		std::swap( m_pConnectorFront, m_pConnectorEnd );
		if( m_pConnectorFront ){
			const int slot = m_pConnectorFront->Slot( *this, EndType::end );
			m_pConnectorFront->Clear( slot );
			Connector* pCaller = nullptr;
			std::swap( pCaller, m_pConnectorFront );
			pCaller->Slot( slot, This(), EndType::front );
		}
		if( m_pConnectorEnd ){
			const int slot = m_pConnectorEnd->Slot( *this, EndType::front );
			m_pConnectorEnd->Clear( slot );
			Connector* pCaller = nullptr;
			std::swap( pCaller, m_pConnectorEnd );
			pCaller->Slot( slot, This(), EndType::end );
		}

		std::swap( m_pTETFront, m_pTETEnd );

		for( auto& pair : m_Sensors ){
			pair.first.parameter = GetLength() - pair.first.parameter;
			pair.first.orientation = !pair.first.orientation;
		}
		std::reverse( m_Sensors.begin(), m_Sensors.end() );

		for( auto& pair : m_Signals ){
			pair.first = GetLength() - pair.first;
			pair.first.m_Orientation = !pair.first.m_Orientation;
		}
		std::reverse( m_Signals.begin(), m_Signals.end() );

		for( auto& tuple : m_Reservations )
			std::get<1>(tuple) = GetLength() - std::get<1>(tuple);
	}
	else
	{
		const bool bTSEnd = m_pTETFront != nullptr;
		const bool bTSFront = m_pTETEnd != nullptr;
		DestroyEndTransitionSignal( EndType::front );
		DestroyEndTransitionSignal( EndType::end );
		if( bTSFront )
			CreateEndTransitionSignal( EndType::front );
		if( bTSEnd )
			CreateEndTransitionSignal( EndType::end );
	}
}

void Track_Imp::CreateEndTransitionSignal( EndType atend ){
	if( atend == EndType::front ){
		if( m_pTETFront )
			DestroyEndTransitionSignal( EndType::front );

		m_pTETFront = TrackEndTransition::Make( this, EndType::front );
		Attach( m_pTETFront, TrackLocation{ 0_m, Orientation::Value::para } );
	}
	else if( atend == EndType::end ){
		if( m_pTETEnd )
			DestroyEndTransitionSignal( EndType::end );

		m_pTETEnd = TrackEndTransition::Make( this, EndType::end );
		Attach( m_pTETEnd, TrackLocation( GetLength(), Orientation::Value::anti ) );
	}
}

void Track_Imp::DestroyEndTransitionSignal( EndType atend ){
	if( atend == EndType::front ){
		if( m_pTETFront ){
			Detach( *m_pTETFront );
			m_pTETFront = nullptr;
		}
	}
	else if( atend == EndType::end ){
		if( m_pTETEnd ){
			Detach( *m_pTETEnd );
			m_pTETEnd = nullptr;
		}
	}
}

Connector* Track_Imp::GetConnector( EndType atend ) const noexcept{
	switch( atend ){
	case EndType::any:
	case EndType::front:
		return m_pConnectorFront;
	case EndType::end:
		return m_pConnectorEnd;
	case EndType::none:
	default:
		return nullptr;
	}
}

Connector* Track_Imp::GetConnector( const Orientation& inDirection ) const noexcept{
	if( m_LoopBraker ){
		m_LoopBraker = false;
		return nullptr;
	}

	if( Connector* pConnector = GetConnector( inDirection ? EndType::end : EndType::front ) )
		return pConnector;

	if( TrackEnd nextTrack = TransitionEnd( inDirection ? EndType::end : EndType::front ); nextTrack.first ){
		m_LoopBraker = true;
		Connector* pConnector = nextTrack.first->GetConnector( nextTrack.second == EndType::front ? Orientation::Value::para : Orientation::Value::anti );
		m_LoopBraker = false;
		return pConnector;
	}

	return nullptr;
}

int Track_Imp::CountJacks() const noexcept{
	int counter = 0;
	for( auto& pair : m_Sensors )
		if( const auto pJackEnumerator = dynamic_cast<const JackEnumerator*>(pair.second.get()) )
			counter += pJackEnumerator->CountJacks();
	for( auto& pair : m_Signals )
		if( const auto pJackEnumerator = dynamic_cast<const JackEnumerator*>(pair.second.get()) )
			counter += pJackEnumerator->CountJacks();

	if( const auto pJackEnumerator = dynamic_cast<const JackEnumerator*>(m_pConnectorFront) )
		counter += pJackEnumerator->CountJacks();
	if( const auto pJackEnumerator = dynamic_cast<const JackEnumerator*>(m_pConnectorEnd) )
		counter += pJackEnumerator->CountJacks();

	return counter;
}

const Plug& Track_Imp::_GetPlug( int idx ) const{
	std::vector<Plug*> plugs;

	for( auto& pair : m_Sensors )
		if( auto pPlugEnumerator = dynamic_cast<PlugEnumerator*>(pair.second.get()) )
			for( Plug& plug : *pPlugEnumerator )
				plugs.push_back( &plug );

	for( auto& pair : m_Signals )
		if( auto pPlugEnumerator = dynamic_cast<PlugEnumerator*>(pair.second.get()) )
			for( Plug& plug : *pPlugEnumerator )
				plugs.push_back( &plug );

	if( auto pPlugEnumerator = dynamic_cast<PlugEnumerator*>(m_pConnectorFront) )
		for( Plug& plug : *pPlugEnumerator )
			plugs.push_back( &plug );

	if( auto pPlugEnumerator = dynamic_cast<PlugEnumerator*>(m_pConnectorEnd) )
		for( Plug& plug : *pPlugEnumerator )
			plugs.push_back( &plug );

	return *plugs.at(common::narrow_cast<size_t>(idx));
}

int Track_Imp::CountPlugs() const{
	int counter = 0;
	for( auto& pair : m_Sensors )
		if( const auto pPlugEnumerator = dynamic_cast<const PlugEnumerator*>(pair.second.get()) )
			counter += pPlugEnumerator->CountPlugs();
	for( auto& pair : m_Signals )
		if( const auto pPlugEnumerator = dynamic_cast<const PlugEnumerator*>(pair.second.get()) )
			counter += pPlugEnumerator->CountPlugs();

	if( const auto pPlugEnumerator = dynamic_cast<const PlugEnumerator*>(m_pConnectorFront) )
		counter += pPlugEnumerator->CountPlugs();
	if( const auto pPlugEnumerator = dynamic_cast<const PlugEnumerator*>(m_pConnectorEnd) )
		counter += pPlugEnumerator->CountPlugs();

	return counter;
}

const Jack& Track_Imp::_GetJack( int idx ) const{
	std::vector<Jack*> jacks;

	for( auto& pair : m_Sensors )
		if( auto pJackEnumerator = dynamic_cast<JackEnumerator*>(pair.second.get()) )
			for( Jack& jack : *pJackEnumerator )
				jacks.push_back( &jack );

	for( auto& pair : m_Signals )
		if( auto pJackEnumerator = dynamic_cast<JackEnumerator*>(pair.second.get()) )
			for( Jack& jack : *pJackEnumerator )
				jacks.push_back( &jack );

	if( auto pJackEnumerator = dynamic_cast<JackEnumerator*>(m_pConnectorFront) )
		for( Jack& jack : *pJackEnumerator )
			jacks.push_back( &jack );

	if( auto pJackEnumerator = dynamic_cast<JackEnumerator*>(m_pConnectorEnd) )
		for( Jack& jack : *pJackEnumerator )
			jacks.push_back( &jack );

	return *jacks.at(common::narrow_cast<size_t>(idx));
}

void Track_Imp::TestTransition( Length s ) const{
	if( !m_pCurve )
		throw std::logic_error( "Tried to parametrize track with no Curve attached" );
	if( InvalidParameter( s ) ){
		std::ostringstream stream;
		stream << "Parameter value out of range for track. Range==" << Range() << " s = " << s << std::endl;
		throw std::range_error( stream.str() );
	}
}

std::vector<std::pair<Track_Imp&, common::Interval<Length>>> Track_Imp::GetRanges( const common::Interval<Length>& range ){
	std::vector<std::pair<Track_Imp&,common::Interval<Length>>> list;
	if( Touching( range, Range() ) ){
		list.push_back( std::pair<Track_Imp&,common::Interval<Length>>{ *this, range } );

		EndType theEnd = EndType::front;
		Interval<Length> transformedRange( range );
		RangeAt( theEnd, transformedRange, list );

		theEnd = EndType::end;
		transformedRange = range;
		RangeAt( theEnd, transformedRange, list );
	}

	return list;
}

void Track_Imp::RangeAt( Track::EndType& theEnd, Interval<Length>& range, std::vector<std::pair<Track_Imp&,Interval<Length>>>& list ){
	if( auto pTrack = dynamic_cast<Track_Imp*>(Transform( range, theEnd )) ){
		if( Touching( range, pTrack->Range() ) ){
			list.push_back( { *pTrack, range } );
			pTrack->RangeAt( theEnd, range, list );
		}
	}
}

Track* Track_Imp::Transform( Interval<Length>& range, Track::EndType& toTrackAtEnd ) const noexcept{
	if( TrackEnd trackEnd = TransitionEnd( toTrackAtEnd ); trackEnd.first )
	{
		if( toTrackAtEnd == EndType::front ){
			if( trackEnd.second == EndType::front ){
				range.m_Near= -range.m_Near;
				range.m_Far	= -range.m_Far;
			}
			else{
				range.m_Near+= trackEnd.first->GetLength();
				range.m_Far += trackEnd.first->GetLength();
			}
		}
		else{
			if( trackEnd.second == EndType::front ){
				range.m_Near -= GetLength();
				range.m_Far -= GetLength();
			}
			else{
				range.m_Near = GetLength() + trackEnd.first->GetLength() - range.m_Near;
				range.m_Far = GetLength() + trackEnd.first->GetLength() - range.m_Far;
			}
		}

		toTrackAtEnd = trackEnd.second == EndType::front ? EndType::end : EndType::front;
		return trackEnd.first.get();
	}

	return nullptr;
}
///////////////////////////////////////
const char* ToString( Track::TrackType type ){
	switch( type ){
	case Track::TrackType::unknown:
		return "unknown";
	case Track::TrackType::none:
		return "none";
	case Track::TrackType::standard:
		return "standard";
	case Track::TrackType::withGeoms:
		return "withGeoms";
	case Track::TrackType::movable:
		return "movable";
	case Track::TrackType::movable_autoconnecting:
		return "movable_autoconnecting";
	case Track::TrackType::parallel:
		return "parallel";
	default:
		assert( !"Unknown Track type!" );
		return "unknown";
	}
}

Track::TrackType TrackType( const std::string& type ) noexcept{
	if( type == "standard" )
		return Track::TrackType::standard;
	else if( type == "withGeoms" )
		return Track::TrackType::withGeoms;
	else if( type == "movable" )
		return Track::TrackType::movable;
	else if( type == "movable_autoconnecting" )
		return Track::TrackType::movable_autoconnecting;
	else if( type == "parallel" )
		return Track::TrackType::parallel;
	else if( type == "none" )
		return Track::TrackType::none;
	else
		return Track::TrackType::unknown;
}

std::string ToString( Track::EndType end ){
	switch(end){
	case Track::EndType::none:
		return "none";
	case Track::EndType::front:
		return "front";
	case Track::EndType::end:
		return "end";
	case Track::EndType::any:
		return "any";
	default:
		assert( !"Unknown Track::EndType enumerator!" );
		return "unknown";
	}
}

Track::EndType ToEndType( const std::string& end ){
	if( end == "front" )
		return Track::EndType::front;
	else if( end == "end" )
		return Track::EndType::end;
	else if( end == "any" )
		return Track::EndType::any;
	else if( end == "none" )
		return Track::EndType::none;

	throw std::invalid_argument( "Invalid Track::EndType string!" );
}

static inline Length Calculate_ds( const Track& track, Length s, Length e, bool ignoreCuvesTorsion ){
	const AnglePerLength sqrtk2t2 = sqrt( pow<2>(track.Curvature(s)) + pow<2>( ignoreCuvesTorsion ? track.GetTwistD1(s) : track.Torsion(s) ) );
	return (sqrtk2t2 > 0_1Im ? sqrt(e / sqrtk2t2) : +infinite__length);
}

Length Segment( const Track& track, Length s, Length e, Interval<Length> segmentLimits, bool ignoreCuvesTorsion ){
	assert( 0_m <= s && s <= track.GetLength() );
	assert( 0_m < e );

	Length ds = Calculate_ds( track, s, e, ignoreCuvesTorsion );
	segmentLimits.Clip( ds );
	if( s + ds > track.GetLength() )
		ds = track.GetLength() - s;

	return ds;
}

static inline Length Calculate_ds( const Track& track, Length s, Length e, Length w, Length h, bool ignoreCuvesTorsion ){
	const AnglePerLength k = track.Curvature(s);
	const AnglePerLength t = ignoreCuvesTorsion ? track.GetTwistD1(s) : track.Torsion(s);

	const AnglePerLength sqrtk2t2 = sqrt( pow<2>(k) + pow<2>(t) );
	return std::min( std::min( 
					k > 0_1Im ? sqrt(e/k) : +infinite__length,
					sqrtk2t2 > 0_1Im ? ( (w > 0_m) ? e/(w/2*sqrtk2t2) : sqrt(e/sqrtk2t2) ) : +infinite__length ),
					fabs(t) > 0_1Im ? ( (h > 0_m) ? e/(h/2*fabs(t)) : sqrt(e/fabs(t)) ) : +infinite__length );
}

Length Segment( const Track& track, Length s, Length e, Length w, Length h, common::Interval<Length> segmentLimits, bool ignoreCuvesTorsion ){
	assert( 0_m <= s && s <= track.GetLength() );
	assert( 0_m < e );
	assert( 0_m <= w );
	assert( 0_m <= h );

	Length ds = Calculate_ds( track, s, e, w, h, ignoreCuvesTorsion );
	segmentLimits.Clip( ds );
	if( s + ds > track.GetLength() )
		ds = track.GetLength() - s;

	return ds;
}

Length Segment_Checked( const Track& track, Length s, Length e, common::Interval<Length> segmentLimits, bool ignoreCuvesTorsion )
{
	Length ds = Segment( track, s, e, segmentLimits, ignoreCuvesTorsion );
	
	while( ds > segmentLimits.Near() && ds > Calculate_ds( track, s+ds, e, ignoreCuvesTorsion ) )
		ds = std::max( segmentLimits.Near(), 0.9f*ds );

	return ds;
}

Length Segment_Checked( const Track& track, Length s, Length e, Length w, Length h, common::Interval<Length> segmentLimits, bool ignoreCuvesTorsion )
{
	Length ds = Segment( track, s, e, w, h, segmentLimits, ignoreCuvesTorsion );
	
	while( ds > segmentLimits.Near() && ds > Calculate_ds( track, s+ds, e, w, h, ignoreCuvesTorsion ) )
		ds = std::max( segmentLimits.Near(), 0.9f*ds );

	return ds;
}

Length Segment_TotallyChecked( 
	const Track& track, 
	Length s, 
	Length e, 
	common::Interval<Length> segmentLimits, 
	bool ignoreCuvesTorsion )
{
	Length ds = segmentLimits.Near();
	if( s + ds > track.GetLength() )
		return track.GetLength() - s;

	Length dsMax = Segment( track, s, e, segmentLimits, ignoreCuvesTorsion );

	while( ds < dsMax ){
		const Length dsMaxNew = Calculate_ds( track, s+ds, e, ignoreCuvesTorsion );
		if( ds > dsMaxNew )
			return std::max( segmentLimits.Near(), ds - segmentLimits.Near() );

		dsMax = std::min( dsMax, dsMaxNew );
		ds += segmentLimits.Near();
	}

	return dsMax;
}

Length Segment_TotallyChecked( 
	const Track& track, 
	Length s, 
	Length e, 
	Length w, 
	Length h, 
	common::Interval<Length> segmentLimits, 
	bool ignoreCuvesTorsion )
{
	Length ds = segmentLimits.Near();
	if( s + ds > track.GetLength() )
		return track.GetLength() - s;

	//const TrackBuilder& trackBuilder = dynamic_cast<const TrackBuilder&>(track);
	////if( false )
	//{
	//	if( auto pPoly = std::dynamic_pointer_cast<const PolygonalChain>( trackBuilder.GetCurve().first ); pPoly ){

	//		return pPoly->GetParameter( pPoly->UpperBound( s ) ) - s; //Todo: limits ...
	//	}
	//
	//	if( auto pPoly = std::dynamic_pointer_cast<const SampledCurve>( trackBuilder.GetCurve().first ); pPoly ){
	//	
	//		return pPoly->GetData().at( pPoly->UpperBound( s ) ).s - s; //Todo: limits ...-------
	//	}

	//	if( auto pSpline = std::dynamic_pointer_cast<const Spline>( trackBuilder.GetCurve().first ); pSpline ){
	//	
	//		return pSpline->GetParameter( pSpline->UpperBound( s + epsilon__length ) ) - s; //Todo: limits ...-------
	//	}
	//}

	Length dsMax = Segment( track, s, e, w, h, segmentLimits, ignoreCuvesTorsion );

	while( ds < dsMax ){
		const Length dsMaxNew = Calculate_ds( track, s+ds, e, w, h, ignoreCuvesTorsion );
		if( ds > dsMaxNew )
			return std::max( segmentLimits.Near(), ds - segmentLimits.Near() );

		dsMax = std::min( dsMax, dsMaxNew );
		ds += segmentLimits.Near();
	}

	return dsMax;
}

bool SetFrame( TrackBuilder& track, const spat::Frame<Length, One>& start, Length s ) noexcept{
	try{
		track.SetFrame( start, s );
		return true;
	}
	catch( ... ){
		return false;
	}
}

bool Mirror( 
	TrackBuilder& track, 
	VectorBundle<Length,One> mirrorPlane, 
	bool bTest )
{
	auto curve = track.GetCurve(); 
	if( curve.first ){
		if( std::shared_ptr<Curve> pNewCurve = curve.first->Clone() )
		{
			mirrorPlane.T.Normalize();
			track.GetAbsoluteFrame().FromParent( mirrorPlane );
			VectorBundle<Length,One> mirrorPlaneLocal{ mirrorPlane };
			track.GetFrame().FromParent( mirrorPlaneLocal );

			if( !pNewCurve->Mirror( mirrorPlaneLocal ) ){
				if( pNewCurve->IsFlat() ){

					spat::Transformation<One> M;
					M.CreateMirror( mirrorPlane );

					Frame<Length,One> frame{
						M * track.GetFrame().P,
						M * track.GetFrame().T,
						M * track.GetFrame().N,
						M * track.GetFrame().B };

					const Vector<One> plane = pNewCurve->LocalUp();
					if( plane == Ex<One> )
						frame.T *= -1;
					else if( plane == Ey<One> )
						frame.N *= -1;
					else if( plane == Ez<One> )
						frame.B *= -1;
					else 
						return false;

					if( bTest )
						return true;
					track.SetFrame(frame);

					if( DirectionalTwist* pDirectionalTwist = FindTwist<DirectionalTwist>( track.GetTwist() ) ){
						if( pDirectionalTwist->IsFrozen() ){
							auto pTwist = track.DetachTwist();
							pDirectionalTwist->Attractor( -pDirectionalTwist->Attractor() );
							track.Attach( std::move(pTwist) );
						}
					}
					else
						track.GetTwist().Offset( -pi );

					return true;
				}
			}
			else{
				if( bTest )
					return true;

				track.GetTwist().Mirror();
				track.Attach( pNewCurve, curve.second );
				return true;
			}
		}
	}

	return false;
}

bool Normalize( TrackBuilder& track, bool bTest )
{
	auto curve = track.GetCurve(); 
	if( curve.first ){
		if( curve.second == track.Range() )
			return true;

		if( std::shared_ptr<Curve> pCurve = curve.first->Clone() ){
			Frame<Length,One> start, newFrame{ Identity<Length,One> };
			auto pTwist = track.DetachTwist();
			track.Transition( 0_m, start );
			track.Attach( std::move(pTwist) );

			if( auto pLine = std::dynamic_pointer_cast<LineP>(pCurve) )
				pLine->Create( start );
			else if( auto pArc = std::dynamic_pointer_cast<ArcP>(pCurve) )
				pArc->Create( VectorBundle2<Length,One>{start}, curve.first->Curvature( 0_m ) );
			else if( auto pHelix = std::dynamic_pointer_cast<HelixP>(pCurve) )
				pHelix->Create( start, curve.first->Curvature( 0_m ), curve.first->Torsion( 0_m ) );
			else{		
				auto pRotator = std::dynamic_pointer_cast<Rotator>(pCurve); 
				if( pRotator && pRotator->GetData().a * pRotator->GetData().b == 0_1Im2 )
				{
					if( !curve.second.Normal() ){
						Rotator::Data characteristic = pRotator->GetData();
						characteristic.a *= -1;
						characteristic.b *= -1;
						pRotator->Create( characteristic );
					}

					VectorBundle2<Length,One> curveStart;
					pCurve->Transition( 0_m, curveStart );
					newFrame = start;
					newFrame.RotateTan( -atan2( curveStart.N.dz, curveStart.N.dy ) );
				}
				else{
					return false;
				}
			}

			if( !bTest ){
				track.Attach( pCurve, track.Range() );

				DirectionalTwist* pDirTwist = FindTwist<DirectionalTwist>( track.GetTwist() );
				bool bFrozen = false;
				if( pDirTwist )
					if( (bFrozen = pDirTwist->IsFrozen()) != false )
						pDirTwist->Freeze( false );

				track.SetFrame( newFrame );
				
				if( pDirTwist && bFrozen )
					pDirTwist->Freeze();
			}

			return true;
		}
	}

	return false;
}

void StraightenUp( TrackBuilder& track, const spat::Vector<One>& U )
// If there is a local up vector (L) for the curve of the track, by the frames
// it gets translated to the global vector:
// 
// Fa o F o L;
// 
// with Fa being the absolute and F the track's relative frame. This is supposed to be
// U for a new frame Fn:
// 
// U = Fa o Fn o L;
//
// We get this new frame Fn by rotating Fa o F o L around the axis: 
//
// (Fa o F o L) % U by the angle a = asin( |(Fa o F o L) % U| ) for Lg*U >= 0,
// pi - asin( |(Fa o F o L) % U| ) for Lg*U < 0;
//
// We call this rotation R. So U = R o Fa o F o L, so that it holds:
//
// Fa o Fn = R o Fa o F; or:
//
// Fn = Inv(Fa) o R o Fa o F;
{
	if( !track.IsValid() )
		throw std::invalid_argument( "StraightenUp(): Track not valid!" );

	const Vector<One> L = track.GetCurve().first->LocalUp();
	const Transformation<One> Fa{ track.GetAbsoluteFrame() };
	const Transformation<One> F{ track.GetFrame() };

	Vector<One> Lg = Fa * F * L;
	Lg.Normalize();
	const auto Vr = Normalize( Lg % U );

	Transformation<One> R;
	Angle a = Lg*U >= 0 ? asin( Vr.first ) : pi - asin( Vr.first );
	R.CreateRotation( a * Vr.second );

	Frame<Length,One> Fn{ Inverted(Fa) * R * Fa * F };
	track.SetFrame( Fn.OrthoNormalize() );
}

bool Simplify( TrackBuilder& track ) noexcept
{
	bool retval = false;
	std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> Curve = track.GetCurve();

	// unfreeze to maintain global attractor directions while attaching new curve:
	bool bFrozen= track.GetTwist().IsFrozen();
	track.GetTwist().Freeze( false ); 

	if( std::shared_ptr<const LineP> lineP = std::dynamic_pointer_cast<const LineP>(Curve.first); lineP )
	{
		const LineP::Data& data = lineP->GetData();
		spat::Frame<Length,One> SupplementalFrame{ data.vb.P, data.vb.T, data.up % data.vb.T, data.up };
		track.SetFrame(	(track.GetFrame() * SupplementalFrame).OrthoNormalize() );
		if( std::shared_ptr<Line> pLine = Line::Make(); pLine )
		{
			track.Attach( std::make_pair( pLine, Curve.second ) );
			retval = true;
		}
	}
	else if( std::shared_ptr<const ArcP> arcP = std::dynamic_pointer_cast<const ArcP>(Curve.first); arcP )
	{
		ArcP::Data data = arcP->GetData();
		Length radius = _m(data.vb2.N.Normalize());
		spat::Frame<Length,One> SupplementalFrame{ data.vb2.P, -data.vb2.N, data.vb2.T, data.vb2.T % data.vb2.N };
		track.SetFrame(	(track.GetFrame() * SupplementalFrame).OrthoNormalize() );
		if( std::shared_ptr<Arc> pArc = Arc::Make(); pArc )
		{
			pArc->Create( Arc::Data{ 1 / radius } );
			track.Attach( std::make_pair( pArc, Curve.second ) );
			retval = true;
		}
	}
	else if( std::shared_ptr<const HelixP> helixP = std::dynamic_pointer_cast<const HelixP>(Curve.first); helixP )
	{
		const HelixP::Data& data = helixP->GetData();
		spat::Frame<Length,One> SupplementalFrame{ data.center.P, data.center.T, data.center.N, data.center.T % data.center.N };
		track.SetFrame( (track.GetFrame() * SupplementalFrame).OrthoNormalize() );
		if( std::shared_ptr<Helix> pHelix = Helix::Make(); pHelix )
		{
			pHelix->Create( Helix::Data{ data.a, data.b } );
			track.Attach( std::make_pair( pHelix, Curve.second ) );
			retval = true;
		}
	}

	if( bFrozen )
		track.GetTwist().Freeze();

	return retval;
}

std::shared_ptr<TrackBuilder> CopyTrack( const TrackBuilder& originalTrack, TrackBuilder& copyTrack, const bool cloneCurve ){
	return CopyTrack( originalTrack, copyTrack, originalTrack.Range(), cloneCurve );
}

std::shared_ptr<TrackBuilder> CopyTrack( 
	const TrackBuilder& originalTrack, 
	bool cloneCurve )
{
	return CopyTrack( originalTrack, originalTrack.Range(), cloneCurve );
}

std::shared_ptr<TrackBuilder> CopyTrack( 
	const TrackBuilder& track, 
	TrackBuilder& copyTrack, 
	common::Interval<Length> copyRange, 
	const bool cloneCurve )
{
	copyRange.Intersection( track.Range() );

	if( track.IsValid() && copyRange.Length() )
	{
		const Interval<Length> originalCurveRange{ track.GetCurve().second };
		const Interval<Length> pieceCurveRange = originalCurveRange.Near() + (originalCurveRange.Normal() ? +1.f : -1.f) * copyRange;

		copyTrack.SetFrame( track.GetFrame() );
		copyTrack.Attach( cloneCurve ? track.GetCurve().first->Clone() : track.GetCurve().first, pieceCurveRange );

		copyTrack.Attach( track.GetTwist().Clone() );
		if( const LinearTwist* pOriginalTwist = FindTwist<const LinearTwist>(track.GetTwist()) ) {
			if( LinearTwist* pTwist = FindTwist<LinearTwist>(copyTrack.GetTwist()) ) {
				pTwist->From( pOriginalTwist->DoTwist(copyRange.Near()) );
				pTwist->To( pOriginalTwist->DoTwist(copyRange.Far()) );
			}
		}

		return copyTrack.This();
	}

	return nullptr;
}

std::shared_ptr<TrackBuilder> CopyTrack( 
	const TrackBuilder& originalTrack, 
	common::Interval<Length> copyRange, 
	bool cloneCurve )
{
	//Todo: make copied track same type...
	return CopyTrack( originalTrack, *TrackBuilder::Make(), copyRange, cloneCurve );
}

std::vector<std::shared_ptr<TrackBuilder>> SplitTrack( const TrackBuilder& track, const int numPieces, const bool cloneCurve )
{
	if( track.IsValid() && numPieces >= 1 )
	{
		const Length pieceLength = track.GetLength() / numPieces;
		common::Interval<Length> pieceRange{ 0_m, pieceLength };

		std::vector<std::shared_ptr<TrackBuilder>> list{ static_cast<std::size_t>(numPieces) };
		for( auto& pTrack : list ){
			pTrack = CopyTrack( track, pieceRange, cloneCurve );
			pieceRange.Move( pieceLength );
		}

		return list;
	}

	return {};
}

std::vector<std::shared_ptr<TrackBuilder>> SplitTrack( const TrackBuilder& track, Length atParameter, bool cloneCurve )
{
	if( track.IsValid() && atParameter > 0_m && atParameter < track.GetLength() )
		return { CopyTrack( track, { 0_m, atParameter } , cloneCurve ),
				 CopyTrack( track, { atParameter, track.GetLength() } , cloneCurve ) };

	return {};
}

std::vector<std::shared_ptr<TrackBuilder>> CloseGap( 
	spat::Frame<Length,One> start, 
	spat::Frame<Length,One> end, 
	common::Interval<Length> limits,
	const spat::Vector<One>& up, 
	Length e_length, 
	Angle e_angle,
	bool cloneCurve )
{
	if( std::shared_ptr<TrackBuilder> pTemplateTrack = TrackBuilder::Make(); pTemplateTrack )
	{
		std::unique_ptr<CombinedTwist> pTwist = CombinedTwist::Make();
		pTwist->AttachTwist1( DirectionalTwist::Make() );
		pTwist->AttachTwist2( LinearTwist::Make( atan2( up*start.N, up*start.B ), atan2( up*end.N, up*end.B ) ) );
		pTemplateTrack->Attach( std::move(pTwist) );

		if( Strech( *pTemplateTrack, start, end, {0_m,+infinite__length}, {0_m,+infinite__length}, up, e_length, e_angle ) )
		{
			if( pTemplateTrack->GetLength() < limits.Near() )
				return {};
				
			if( pTemplateTrack->GetLength() <= limits.Far() )
				return { pTemplateTrack };

			return SplitTrack( *pTemplateTrack, common::narrow_cast<int>(std::ceil( pTemplateTrack->GetLength() / limits.Far() )), cloneCurve );
		}
	}

	return {};
}
///////////////////////////////////////
}
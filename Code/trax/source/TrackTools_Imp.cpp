//	trax track library
//	AD 2025
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

#include "trax/UnitsHelper.h"
#include "trax/Track.h"

#include <iostream>

namespace trax{
	using namespace spat;

std::pair<bool,Length> Foot( const Position<Length>& ofPoint, const Track& toTrack ){
	return Foot( ofPoint, toTrack, toTrack.Range().Center(), toTrack.Range() );
}

std::pair<bool,Length> Foot( const Position<Length>& ofPoint, const Track& toTrack, Length bestGuess ){
	return Foot( ofPoint, toTrack, bestGuess, toTrack.Range() );
}

std::pair<bool,Length> Foot(
	const Position<Length>& A, 
	const Track& toTrack, 
	Length bestGuess, 
	common::Interval<Length> inRange )
// For a Newton-Raphson approach the zeroes of a function f have to make
// our solution. This f would be:
//
// f (s) = (F.P - A) * F.T;
// df/ds = dF.P/ds * F.T + (F.P - A) * dF.T/ds;
//       = F.T * F.T + (F.P - A) * k * F.N;   // see Seret/Ferret
//		 = 1 + k * (F.P - A) * F.N;
//
// A being the target point, F(s) being the track's frame at s.
{
	if( !toTrack.IsValid() ){
		std::ostringstream err;
		toTrack.Diagnose( err );
		throw std::runtime_error( "Invalid track! " + err.str() );
	}

	inRange.Intersection( toTrack.Range() );
	if( !inRange.Touches( bestGuess ) )
		throw std::invalid_argument( "best guess value outside the given range!" );


	const auto f = [&A,&toTrack]( Length s ) -> std::pair<Length,One>{ 
		Frame<Length,One> F;
		toTrack.TNBFrame( s, F );
		return std::make_pair(
			(F.P - A) * F.T,
			1_1 + toTrack.Curvature(s) * ((F.P - A) * F.N) );
	};

	constexpr int digits = std::numeric_limits<Real>::digits;	// Maximum possible binary digits accuracy for type Real.
	constexpr int get_digits = static_cast<int>(digits * 0.6);	// Accuracy doubles with each step, so stop when we have
																// just over half the digits correct.

	constexpr boost::uintmax_t maxit = 20;
	boost::uintmax_t it = maxit;

	try{
		Length s = newton_raphson_iterate( 
			f, 
			bestGuess, 
			inRange, 
			get_digits, 
			it );

		if( trax::isnan( s ) || it >= maxit )
			return { false, 0_m };

		return { true, s };
	}
	catch( const boost::math::evaluation_error& e ){
		std::cerr << "Foot finding failed: " << e.what() << std::endl;
		return { false, 0_m };
	}
	catch( ... ){
		std::cerr << "Foot finding failed: " << "unknown failure." << std::endl;
		return { false, 0_m };
	}
}

std::pair<bool,Length> Foot( const VectorBundle<Length,One>& ofRay, const Track& toTrack ){
	return Foot( ofRay, toTrack, toTrack.Range().Center(), toTrack.Range() );
}

std::pair<bool,Length> Foot( const VectorBundle<Length,One>& ofRay, const Track& toTrack, Length bestGuess ){
	return Foot( ofRay, toTrack, bestGuess, toTrack.Range() );
}

std::pair<bool,Length> Foot( const VectorBundle<Length,One>& R, const Track& toTrack, Length bestGuess, common::Interval<Length> inRange )
// As a wild guess: The locally shortest distance vector between two curves is perpendicular on both, or the shortest distance is zero.
// 
// At the closest point of a track to a ray the (squared) distance D2 gets 
// minimal. That would mean dD(s)/ds*D(s) == 0. For a Newton-Raphson 
// approach we'll also need the derivative of our f(s):
// 
// df/ds = d2D(s)/ds2*D(s) + dD(s)/ds*dD(s)/ds.
// 
// Using Serret-Frennet:
//
// D(s)       = (F.P - R.P) - ((F.P - R.P) * R.T) * R.T;
// dD(s)/ds   = F.T - (F.T * R.T) * R.T;
// d2D(s)/ds2 = k * (F.N - (F.N * R.T) * R.T);
{
	if( !toTrack.IsValid() ){
		std::ostringstream err;
		toTrack.Diagnose( err );
		throw std::runtime_error( "Invalid track! " + err.str() );
	}

	inRange.Intersection( toTrack.Range() );
	if( !inRange.Touches( bestGuess ) )
		throw std::invalid_argument( "best guess value outside the given range!" );


	const auto f = [&R,&toTrack]( Length s ) -> std::pair<Length,One>{ 
		Frame<Length,One> F;
		toTrack.TNBFrame( s, F );

		const Vector<Length> D = (F.P - R.P) - ((F.P - R.P) * R.T) * R.T;
		const Vector<One> dDIds = F.T - (F.T * R.T) * R.T;
		const Vector<AnglePerLength> d2DIds2 = toTrack.Curvature(s) * (F.N - (F.N * R.T) * R.T);

		return std::make_pair(
			D * dDIds,
			D * d2DIds2 + dDIds * dDIds );
	};


	constexpr int digits = std::numeric_limits<Real>::digits;	// Maximum possible binary digits accuracy for type Real.
	constexpr int get_digits = static_cast<int>(digits * 0.6);	// Accuracy doubles with each step, so stop when we have
																// just over half the digits correct.

	constexpr boost::uintmax_t maxit = 20;
	boost::uintmax_t it = maxit;

	try{
		Length s = newton_raphson_iterate( 
			f, 
			bestGuess, 
			inRange, 
			get_digits, 
			it );

		if( trax::isnan( s ) || it >= maxit )
			return { false, 0_m };

		return { true, s };
	}
	catch( const boost::math::evaluation_error& e ){
		std::cerr << "Foot finding failed: " << e.what() << std::endl;
		return { false, 0_m };
	}
	catch( ... ){
		std::cerr << "Foot finding failed: " << "unknown failure." << std::endl;
		return { false, 0_m };
	}
}

Length Closest( const Position<Length>& A, const Track& onTrack, bool includeEndpoints )
{
	Position<Length> F, E;
	onTrack.Transition( 0_m, F );
	onTrack.Transition( onTrack.GetLength(), E );

	std::vector<std::pair<Length,Length>> candidates;
	if( includeEndpoints ){
		candidates.push_back( std::make_pair( 0_m, (F - A).Length() ) );
		candidates.push_back( std::make_pair( onTrack.GetLength(), (E - A).Length() ) );
	}

	std::pair<bool,Length> foot = Foot( A, onTrack ); 
	if( foot.first ){
		Position<Length> C;
		onTrack.Transition( foot.second, C );
		candidates.push_back( std::make_pair( foot.second, (C - A).Length() ) );

		std::pair<bool,Length> footF = Foot( A, onTrack, 0_m, { 0_m, foot.second } );
		if( footF.first && !common::Equals( footF.second, foot.second, epsilon__length ) ){
			onTrack.Transition( footF.second, C );
			candidates.push_back( std::make_pair( footF.second, (C - A).Length() ) );
		}

		std::pair<bool,Length> footE = Foot( A, onTrack, onTrack.GetLength(), { foot.second, onTrack.GetLength() } );
		if( footE.first && !common::Equals( footE.second, foot.second, epsilon__length ) ){
			onTrack.Transition( footE.second, C );
			candidates.push_back( std::make_pair( footE.second, (C - A).Length() ) );
		}
	}

	if( candidates.empty() )
		return Length{-1};
	else
		return std::min_element( 
			candidates.begin(), 
			candidates.end(), 
			[]( const std::pair<Length,Length>& a, const std::pair<Length,Length>& b ){ return a.second < b.second; } )->first;
}

Length Closest( const VectorBundle<Length,One>& R, const Track& onTrack, bool includeEndpoints )
{
	const auto DistanceFromRay = [&R]( const Position<Length>& P ){ return ((P - R.P) - ((P - R.P) * R.T) * R.T).Length(); };

	Position<Length> F, E;
	onTrack.Transition( 0_m, F );
	onTrack.Transition( onTrack.GetLength(), E );

	std::vector<std::pair<Length,Length>> candidates;
	if( includeEndpoints ){
		candidates.push_back( std::make_pair( 0_m, DistanceFromRay(F) ) );
		candidates.push_back( std::make_pair( onTrack.GetLength(), DistanceFromRay(E) ) );
	}

	std::pair<bool,Length> foot = Foot( R, onTrack ); 
	if( foot.first ){
		Position<Length> C;
		onTrack.Transition( foot.second, C );
		candidates.push_back( std::make_pair( foot.second, DistanceFromRay(C) ) );

		std::pair<bool,Length> footF = Foot( R, onTrack, 0_m, { 0_m, foot.second } );
		if( footF.first && !common::Equals( footF.second, foot.second, epsilon__length ) ){
			onTrack.Transition( footF.second, C );
			candidates.push_back( std::make_pair( footF.second, DistanceFromRay(C) ) );
		}

		std::pair<bool,Length> footE = Foot( R, onTrack, onTrack.GetLength(), { foot.second, onTrack.GetLength() } );
		if( footE.first && !common::Equals( footE.second, foot.second, epsilon__length ) ){
			onTrack.Transition( footE.second, C );
			candidates.push_back( std::make_pair( footE.second, DistanceFromRay(C) ) );
		}
	}

	if( candidates.empty() )
		return Length{-1};
	else
		return std::min_element( 
			candidates.begin(), 
			candidates.end(), 
			[]( const std::pair<Length,Length>& a, const std::pair<Length,Length>& b ){ return a.second < b.second; } )->first;
}

void Couple( 
	std::pair<std::shared_ptr<Track>,Track::EndType> trackA, 
	std::pair<std::shared_ptr<Track>,Track::EndType> trackB )
{
	if( TrackBuilder* pTrackBuilder = dynamic_cast<TrackBuilder*>(trackA.first.get()) )
		pTrackBuilder->Couple(	std::make_pair(std::dynamic_pointer_cast<TrackBuilder>(trackA.first), trackA.second), 
								std::make_pair(std::dynamic_pointer_cast<TrackBuilder>(trackB.first), trackB.second) );
}

Length DistanceOf( 
	const Track::cTrackEnd trackEndA, 
	const Track::cTrackEnd trackEndB )
{
	if( trackEndA.first == nullptr || !trackEndA.first->IsValid() )
		throw std::invalid_argument( "CalculateGapSize: trackEndA has no valid track!" );
	if( trackEndB.first == nullptr || !trackEndB.first->IsValid() )
		throw std::invalid_argument( "CalculateGapSize: trackEndB has no valid track!" );

	Position<Length> posTheOne, posTheOther;
	trackEndA.first->Transition( trackEndA.second == Track::EndType::front ? 0_m : trackEndA.first->GetLength(), posTheOne );
	trackEndB.first->Transition( trackEndB.second == Track::EndType::front ? 0_m : trackEndB.first->GetLength(), posTheOther );
	return (posTheOther - posTheOne).Length();
}

Length DistanceToCoupled( const Track& track, Track::EndType atEnd )
{
	if( !track.IsCoupled( atEnd ) )
		throw std::logic_error( "DistanceToCoupled: track end is not coupled!" );

	return DistanceOf( std::make_pair( track.This(), atEnd ), track.TransitionEnd( atEnd ) );
}

Angle KinkToCoupled( const Track& track, Track::EndType atEnd )
{
	if( !track.IsCoupled( atEnd ) )
		throw std::logic_error( "KinkToCoupled: track end is not coupled!" );

	Vector<One> t1, t2;
	Track::TrackEnd otherEnd = track.TransitionEnd( atEnd );

	switch( otherEnd.second ){
	case Track::EndType::any:
	case Track::EndType::front:
		otherEnd.first->Transition( 0_m, t2 );
		break;
	case Track::EndType::end:
		otherEnd.first->Transition( otherEnd.first->GetLength(), t2 );
		break;
	case Track::EndType::none:
	default:
		return pi;
	}

	switch( atEnd ){
	case Track::EndType::any:
	case Track::EndType::front:
		track.Transition( 0_m, t1 );
		break;
	case Track::EndType::end:
		track.Transition( track.GetLength(), t1 );
		break;
	case Track::EndType::none:
	default:
		return pi;
	}

	if( atEnd == otherEnd.second )
		t2 *= -1;

	return atan2( (t1 % t2).Length(), t1 * t2 );
}

Angle TwistToCoupled( const Track& track, Track::EndType atEnd )
{
	if( !track.IsCoupled( atEnd ) )
		throw std::logic_error( "TwistToCoupled: track end is not coupled!" );

	Frame<Length,One> b1, b2;
	Track::TrackEnd otherEnd = track.TransitionEnd( atEnd );

	switch( otherEnd.second ){
	case Track::EndType::any:
	case Track::EndType::front:
		otherEnd.first->Transition( 0_m, b2 );
		break;
	case Track::EndType::end:
		otherEnd.first->Transition( otherEnd.first->GetLength(), b2 );
		break;
	case Track::EndType::none:
	default:
		return pi;
	}

	switch( atEnd ){
	case Track::EndType::any:
	case Track::EndType::front:
		track.Transition( 0_m, b1 );
		break;
	case Track::EndType::end:
		track.Transition( track.GetLength(), b1 );
		break;
	case Track::EndType::none:
	default:
		return pi;
	}

	return atan2( (b1.B % b2.B).Length(), b1.B * b2.B );
}

}
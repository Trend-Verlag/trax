//	trax track library
//	AD 2023 
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

#include "MovableTrackAutoConnecting_Imp.h"
#include "trax/rigid/Body.h"
#include "trax/collections/TrackSystem.h"

#include "spat/Sphere.h"

#include <iostream>

namespace trax{
	using namespace spat;

std::shared_ptr<MovableTrackAutoConnecting> MovableTrackAutoConnecting::Make() noexcept{
	try{
		return std::make_shared<MovableTrackAutoConnecting_Imp>();
	}
	catch( const std::bad_alloc& ){
		return nullptr;
	}
}

MovableTrackAutoConnecting_Imp::MovableTrackAutoConnecting_Imp()
	: m_AutoConnectingDistance{ default_derailing_distance }
{
}

void MovableTrackAutoConnecting_Imp::AutoConnecting( Length distance ) noexcept{
	m_AutoConnectingDistance = distance;
}

Length MovableTrackAutoConnecting_Imp::AutoConnecting() const noexcept{
	return m_AutoConnectingDistance;
}

bool MovableTrackAutoConnecting_Imp::IsAutoConnecting() const noexcept{
	return epsilon__length < m_AutoConnectingDistance;
}

void MovableTrackAutoConnecting_Imp::Update( const TrackSystem& trackSystem ){
	if( IsAutoConnecting() ){
		if( GetBody() && !GetBody()->IsSleeping() ){
			Position<Length> posFront, posEnd;
			Transition( 0_m, posFront );
			Transition( GetLength(), posEnd );

			if( TrackEnd coupledEnd = TransitionEnd( EndType::front ); coupledEnd.first ){
				Position<Length> posOther;
				coupledEnd.first->Transition( coupledEnd.second == EndType::front ? 0_m : coupledEnd.first->GetLength(), posOther );
				if( (posOther-posFront).Length() > m_AutoConnectingDistance ){
					DeCouple( EndType::front );
					std::clog << "trax: Trackfront decoupled!" << std::endl;
				}
			}

			if( TrackEnd coupledEnd = TransitionEnd( EndType::end ); coupledEnd.first ){
				Position<Length> posOther;
				coupledEnd.first->Transition( coupledEnd.second == EndType::front ? 0_m : coupledEnd.first->GetLength(), posOther );
				if( (posOther-posEnd).Length() > m_AutoConnectingDistance ){
					DeCouple( EndType::end );
					std::clog << "trax: Trackend decoupled!" << std::endl;
				}
			}
	
			if( !IsCoupled( EndType::front ) ){
				std::vector<Track::End> ends = trackSystem.GetUncoupledIn( Sphere<Length>{posFront,m_AutoConnectingDistance} );
				assert( !ends.empty() );
				if( ends.size() > 1 )
					for( const Track::End foundEnd : ends )
						if( foundEnd.id != ID() ){
							trackSystem.Couple( Track::Coupling{ {ID(),EndType::front}, foundEnd } );
							std::clog << "trax: Trackfront coupled!" << std::endl;
							break;
						}
			}

			if( !IsCoupled( EndType::end ) ){
				std::vector<Track::End> ends = trackSystem.GetUncoupledIn( Sphere<Length>{posEnd,m_AutoConnectingDistance} );
				assert( !ends.empty() );
				if( ends.size() > 1 )
					for( const Track::End foundEnd : ends )
						if( foundEnd.id != ID() ){
							trackSystem.Couple( Track::Coupling{ {ID(),EndType::end}, foundEnd } );
							std::clog << "trax: Trackend coupled!" << std::endl;
							break;
						}
			}
		}
	}
}

}
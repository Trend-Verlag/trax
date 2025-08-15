// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2022 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#include "trax/Track.h"
#include "trax/Curve.h"
#include "spat/Vector.h"
#include "spat/Matrix.h"

namespace trax
{
	using namespace spat;

void CleanCurveTransformation( TrackBuilder& track )
{
	// Line???

	if( auto pArc = std::dynamic_pointer_cast<const ArcP>(track.GetCurve().first); pArc )
	// Move most of the transformation from the Arc curve to the track's frame:
	// Pt(s) = F o T(s) o C o P(s)			with T: twist.
	//	  = F o T(s) o Ct o Cc o P		with Cc: 'clean' arc's frame supposed to be in simple form and C = Ct o Cc
	//    != Ft o Cc o P				with Ft = F o Ct = F o C o Inverse(Cc)
		// IMPOSSIBLE for general twists!!!
	{
		ArcP::Data d = pArc->GetData();
		const Vector<One> N = Normalize(d.vb2.N).second;
		Transformation<One> C{ Frame<Length,One>{ d.vb2.P, d.vb2.T, N, d.vb2.T % N} };

		d.vb2.P = {0_m, pArc->Radius(), 0_m };
		d.vb2.T = {1,0,0};
		d.vb2.N = {0,_m(pArc->Radius()),0};
		auto pNewArc = ArcP::Make();
		pNewArc->Create(d);
		Transformation<One> Cc{ Frame<Length,One>{ {0_m, pArc->Radius(), 0_m }, {1,0,0}, {0,1,0}, {0,0,1} } };
		Transformation<One> F{ track.GetFrame() };

		track.Attach( std::move(pNewArc), track.GetCurve().second );
		track.SetFrame( Frame<Length,One>{ F * C * Cc.Invert() } );
	}
	else if( auto pHelix = std::dynamic_pointer_cast<const HelixP>(track.GetCurve().first); pHelix )
	// Move the transformation from the Helix curve to the track's frame, see Arc: 
	{
		HelixP::Data d = pHelix->GetData();
		Transformation<One> C{ Frame<Length,One>{ d.center.P, d.center.T, d.center.N, d.center.T % d.center.N} };

		d.center.P = {0_m, d.a, 0_m };
		d.center.T = {0,-1,0};
		d.center.N = {1,0,0};
		auto pNewHelix = HelixP::Make();
		pNewHelix->Create(d);
		Transformation<One> Cc{ Frame<Length,One>{ {0_m, d.a, 0_m }, {0,-1,0}, {1,0,0}, {0,0,1} } };
		Transformation<One> F{ track.GetFrame() };

		track.Attach( std::move(pNewHelix), track.GetCurve().second );
		track.SetFrame( Frame<Length,One>{ F * C* Cc.Invert() } );
	}
	//// Move the transformation from the Helix curve to the track's frame (Cc is unit matrix, see Arc): 
	//{
	//	HelixP::Data d = pHelix->GetData();
	//	Transformation<One> C{ Frame<Length,One>{ d.center.P, d.center.T, d.center.N, d.center.T % d.center.N} };

	//	d.center.Init();
	//	auto pNewHelix = HelixP::Make();
	//	pNewHelix->Create(d);
	//	Transformation<One> F{ track.GetFrame() };

	//	track.Attach( std::move(pNewHelix), track.GetCurve().second );
	//	track.SetFrame( Frame<Length,One>{ F * C } );
	//}
}

void TransformToNonEEPCurve(TrackBuilder& track) {
	if (auto pEEPCurve = std::dynamic_pointer_cast<const trax::EEPCurve>(track.GetCurve().first); pEEPCurve) {
		if (std::shared_ptr<Curve> pCurve = trax::CreateNonEEPCurve(pEEPCurve->GetData()))
		{
			track.Attach(pCurve, { 0_m, _cm(pEEPCurve->GetData().gc_Laenge) });

			switch (pCurve->GetCurveType()) {
			case Curve::CurveType::ArcP:
			case Curve::CurveType::HelixP:
			{
				const bool bFrozen = track.GetTwist().IsFrozen();
				track.GetTwist().Freeze(false); // we need to keep the global orientations
				CleanCurveTransformation(track);
				track.GetTwist().Freeze(bFrozen);
				break;
			}
			case Curve::CurveType::Rotator:
			{
				spat::Frame<Length, One> frame{ spat::spatial_cast<Length,One>(pEEPCurve->GetData().m_AnfangsBein) };
				frame.P /= 100;
				frame.RotateTan(-pEEPCurve->GetData().m_FuehrungsVerdrehung);	// The starting Frame includes the twist at the starting point. 
				// For trax this is not the case, so we calculate this out here:

				track.GetFrame().ToParent(frame);
				track.SetFrame(frame);
				break;
			}
			default:
				break;
			}

			return;
		}

		throw std::logic_error{ "Could not convert EEPCurve!" };
	}
}

inline bool CheckLimits( 
	const std::pair<std::unique_ptr<Curve>,common::Interval<Length>>& curve,
	const common::Interval<Length>& length_limits,
	const common::Interval<Length>& overshoot_limits,
	Length e_length )
{
	if( curve.first &&
		length_limits.Touches(curve.second.Length(),e_length) )
	{
		if( auto pCubic = dynamic_cast<const Cubic*>(curve.first.get()) )
			return	overshoot_limits.Touches( pCubic->CubicOvershootAt( curve.second.Near() ).Length(), e_length ) &&
					overshoot_limits.Touches( pCubic->CubicOvershootAt( curve.second.Far() ).Length(), e_length );
		
		return true;
	}

	return false;
}

bool Strech( TrackBuilder& track,
	spat::VectorBundle<Length,One> start, 
	spat::VectorBundle<Length,One> end,
	common::Interval<Length> length_limits,
	common::Interval<Length> overshoot_limits,
	const spat::Vector<One>& up, 
	Length e_length, Angle e_angle )
{
	track.GetAbsoluteFrame().FromParent( start );
	track.GetFrame().FromParent( start );
	track.GetAbsoluteFrame().FromParent( end );
	track.GetFrame().FromParent( end );

	auto pair = CreateCurve( start, end, up, e_length, e_angle ); 
	if( CheckLimits( pair, length_limits, overshoot_limits, e_length ) ){
		track.Attach( std::move(pair.first), pair.second );
		return true;
	}

	return false;
}

bool dclspc StrechStart( TrackBuilder& track,
	spat::Position<Length> start,
	common::Interval<Length> length_limits,
	const spat::Vector<One>& up, Length e_length  )
{
	VectorBundle<Length,One> end;
	auto curve = track.GetCurve();
	curve.first->Transition( curve.second.Far(), end );
	if( !curve.second.Normal() )
		end.T *= -1;

	track.GetAbsoluteFrame().FromParent( start );
	track.GetFrame().FromParent( start );

	auto pair = CreateCurve( start, end, up, e_length ); 
	if( pair.first && length_limits.Touches(pair.second.Length(),e_length) ){
		track.Attach( std::move(pair.first), pair.second );
		return true;
	}

	return false;
}

bool StrechStart( TrackBuilder& track,
	spat::Position<Length> start,
	bool bKeepStartTangent,
	Length cubicOvershootStart,
	Length cubicOvershootEnd,
	common::Interval<Length> length_limits,
	common::Interval<Length> overshoot_limits,
	const spat::Vector<One>& up, Length e_length, Angle e_angle )
{
	track.GetAbsoluteFrame().FromParent( start );
	track.GetFrame().FromParent( start );

	VectorBundle<Length,One> end;
	auto curve = track.GetCurve();
	curve.first->Transition( curve.second.Far(), end );
	if( !curve.second.Normal() )
		end.T *= -1;

	if( bKeepStartTangent ){
		VectorBundle<Length,One> start_{ start };
		curve.first->Transition( curve.second.Near(), start_.T );
		if( !curve.second.Normal() )
			start_.T *= -1;

		start_.T *= cubicOvershootStart.Units();
		end.T *= cubicOvershootEnd.Units();

		auto pair = CreateCurve( start_, end, up, e_length, e_angle ); 
		if( CheckLimits( pair, length_limits, overshoot_limits, e_length ) ){
			track.Attach( std::move(pair.first), pair.second );
			return true;
		}
	}
	else{
		auto pair = CreateCurve( start, end, up, e_length );
		if( CheckLimits( pair, length_limits, overshoot_limits, e_length ) ){
			track.Attach( std::move(pair.first), pair.second );
			return true;
		}
	}

	return false;
}

bool StrechStart( TrackBuilder& track,
	spat::VectorBundle<Length,One> start,
	Length cubicOvershootEnd,
	common::Interval<Length> length_limits,
	common::Interval<Length> overshoot_limits,
	const spat::Vector<One>& up, Length e_length, Angle e_angle )
{
	track.GetAbsoluteFrame().FromParent( start );
	track.GetFrame().FromParent( start );

	VectorBundle<Length,One> end;
	auto curve = track.GetCurve();
	curve.first->Transition( curve.second.Far(), end );
	if( !curve.second.Normal() )
		end.T *= -1;
	end.T *= cubicOvershootEnd.Units();

	auto pair = CreateCurve( start, end, up, e_length, e_angle ); 
	if( CheckLimits( pair, length_limits, overshoot_limits, e_length ) ){
		track.Attach( std::move(pair.first), pair.second );
		return true;
	}

	return false;
}

bool StrechEnd( TrackBuilder& track,
	spat::Position<Length> end,
	common::Interval<Length> length_limits,
	const spat::Vector<One>& up, Length e_length )
{
	VectorBundle<Length,One> start;
	auto curve = track.GetCurve();
	curve.first->Transition( curve.second.Near(), start );
	if( !curve.second.Normal() )
		start.T *= -1;

	track.GetAbsoluteFrame().FromParent( end );
	track.GetFrame().FromParent( end );

	auto pair = CreateCurve( start, end, up, e_length ); 
	if( pair.first && length_limits.Touches(pair.second.Length(),e_length) ){
		track.Attach( std::move(pair.first), pair.second );
		return true;
	}

	return false;
}

bool StrechEnd( TrackBuilder& track,
	spat::Position<Length> end,
	bool bkeepEndTangent, 
	Length cubicOvershootStart,
	Length cubicOvershootEnd,
	common::Interval<Length> length_limits,
	common::Interval<Length> overshoot_limits,
	const spat::Vector<One>& up, Length e_length, Angle e_angle )
{
	VectorBundle<Length,One> start;
	auto curve = track.GetCurve();
	curve.first->Transition( curve.second.Near(), start );
	if( !curve.second.Normal() )
		start.T *= -1;

	track.GetAbsoluteFrame().FromParent( end );
	track.GetFrame().FromParent( end );

	if( bkeepEndTangent ){
		VectorBundle<Length,One> end_{ end };
		curve.first->Transition( curve.second.Far(), end_.T );
		if( !curve.second.Normal() )
			end_.T *= -1;

		start.T *= cubicOvershootStart.Units();
		end_.T *= cubicOvershootEnd.Units();

		auto pair = CreateCurve( start, end_, up, e_length, e_angle ); 
		if( CheckLimits( pair, length_limits, overshoot_limits, e_length ) ){
			track.Attach( std::move(pair.first), pair.second );
			return true;
		}
	}
	else{
		auto pair = CreateCurve( start, end, up, e_length ); 
		if( CheckLimits( pair, length_limits, overshoot_limits, e_length ) ){
			track.Attach( std::move(pair.first), pair.second );
			return true;
		}
	}

	return false;
}

bool StrechEnd( TrackBuilder& track,
	spat::VectorBundle<Length,One> end,
	Length cubicOvershootStart,
	common::Interval<Length> length_limits,
	common::Interval<Length> overshoot_limits,
	const spat::Vector<One>& up, Length e_length, Angle e_angle )
{
	VectorBundle<Length,One> start;
	auto curve = track.GetCurve();
	curve.first->Transition( curve.second.Near(), start );
	if( !curve.second.Normal() ){
		start.T *= -1;
	}

	track.GetAbsoluteFrame().FromParent( end );
	track.GetFrame().FromParent( end );

	start.T *= cubicOvershootStart.Units();

	auto pair = CreateCurve( start, end, up, e_length, e_angle ); 
	if( CheckLimits( pair, length_limits, overshoot_limits, e_length ) )
	{
		track.Attach( std::move(pair.first), pair.second );
		return true;
	}

	return false;
}

}
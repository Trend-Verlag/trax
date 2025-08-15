// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/Parser.h"

#include "trax/Curve.h"
#include "trax/IDType.h"
#include "trax/Section.h"
#include "trax/Track.h"

namespace trax{

	struct TrackLocationRef;
	struct TrackSystem;

	struct TrackSystemParser : virtual Parser{

		virtual bool TrackSystemStart() noexcept(false) { return true; }

		virtual bool TrackCollectionStart(
			IDType /*id*/ ) noexcept(false) { return true; }

		virtual bool TrackStart(
			IDType /*id*/, 
			const std::string& /*reference*/ ) noexcept(false) { return true; }

		virtual bool TrackConnection(
			const Track::Coupling& /*coupling*/ ) noexcept(false) { return true; }

		virtual bool BufferStop(
			Track::End /*theOne*/ ) noexcept(false) { return true; }

		virtual bool OpenEnd(
			Track::End /*theOne*/ ) noexcept(false) { return true; }


		virtual bool CurveStart() noexcept(false) { return true; }

		virtual bool Line( 
			const LineP::Data& /*data*/ ) noexcept(false) { return true; }

		virtual bool Arc( 
			const ArcP::Data& /*data*/ ) noexcept(false) { return true; }

		virtual bool Helix( 
			const HelixP::Data& /*data*/ ) noexcept(false) { return true; }

		virtual bool Clothoid( 
			const Clothoid::Data& /*data */) noexcept(false) { return true; }

		virtual bool Cubic( 
			const Cubic::Data& /*data*/ ) noexcept(false) { return true; }

		virtual bool Spline( 
			const Spline::Data& /*data*/ ) noexcept(false) { return true; }

		virtual bool Rotator( 
			const Rotator::Data& /*data*/ ) noexcept(false) { return true; }

		virtual bool RotatorChain( 
			const RotatorChain::Data& /*data*/ ) noexcept(false) { return true; }

		virtual bool PolygonalChain( 
			const PolygonalChain::Data& /*data*/ ) noexcept(false) { return true; }

		virtual bool SampledCurve( 
			const SampledCurve::Data& /*data*/ ) noexcept(false) { return true; }

		virtual bool EEPCurve( 
			const EEPCurve::Data& /*data*/ ) noexcept(false) { return true; }

		virtual void CurveEnd() noexcept(false) {}


		virtual bool TwistStart() noexcept(false) { return true; }
		
		virtual bool ConstantTwist( 
			Angle /*twist*/ ) noexcept(false) { return true; }

		virtual bool LinearTwist( 
			Angle /*startAngle*/, 
			Angle /*endAngle*/ ) noexcept(false) { return true; }

		virtual bool PiecewiseTwist( 
			PiecewiseTwist::Data /*data*/ ) noexcept(false) { return true; }

		virtual bool PiecewiseLinearTwist( 
			PiecewiseTwist::Data /*data*/ ) noexcept(false) { return true; }

		virtual bool DirectionalTwist( 
			spat::Vector<One> /*attractor*/ ) noexcept(false) { return true; }

		virtual bool CombinedTwistStart() noexcept(false) { return true; }

		virtual void CombinedTwistEnd() noexcept(false) {}


		virtual void TwistEnd() noexcept(false) {}


		virtual bool Section( 
			Section::SpecialSections /*type*/ ) noexcept(false) { return true; }

		virtual void TrackEnd() noexcept(false) {}

		virtual void TrackCollectionEnd() noexcept(false) {}

		virtual bool ConnectorCollectionStart() noexcept(false) { return true; }

		virtual void ConnectorCollectionEnd() noexcept(false) {}

		virtual void TrackSystemEnd(
			IDType /*activeTrack*/ ) noexcept(false) {}

	};

	bool dclspc ParseTrackSystem( std::basic_istream<char>& stream, TrackSystemParser& callback ) noexcept(false);

	bool dclspc ParseTrackSystem( std::string filePath, TrackSystemParser& callback ) noexcept(false);
	 
	bool dclspc ParseTrackSystem( const TrackSystem& trackSystem, TrackSystemParser& callback ) noexcept(false);




} // namespace trax

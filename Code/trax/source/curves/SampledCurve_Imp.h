//	trax track library
//	AD 2022 
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

#pragma once

#include "trax/Curve_Imp.h"

namespace trax{

	class SampledCurve_Imp : public SampledCurve
	{
	public:
		// SampledCurve:
		int UpperBound( Length s ) const override;

		common::Interval<Length> Create( const Curve& originalCurve, common::Interval<Length> range, Length maxDeviationLength = epsilon__length, Angle maxDeviationAngle = epsilon__angle, common::Interval<Length> sampleDistanceLimits = { epsilon__length, 20_m } ) override;

		common::Interval<Length> Create( std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> originalCurve, Length maxDeviationLength = epsilon__length, Angle maxDeviationAngle = epsilon__angle, common::Interval<Length> sampleDistanceLimits = { epsilon__length, 20_m } ) override;

		common::Interval<Length> Create( const Data& data ) override;

		const Data& GetData() const noexcept override;


		// Curve:
		const char*	TypeName() const noexcept override{
			return "SampledCurve";
		}

		CurveType GetCurveType() const noexcept override{
			return CurveType::SampledCurve;
		}

		bool IsValid() const noexcept override;

		AnglePerLength Curvature( Length s ) const override;

		AnglePerLength Torsion( Length s ) const override;

		bool IsFlat() const noexcept override;

		void Transition( Length s, spat::Position<Length>& pos ) const override;

		void Transition( Length s, spat::Vector<One>& tan ) const override;

		void Transition( Length s, spat::VectorBundle<Length,One>& bundle ) const override;

		void Transition( Length s, spat::VectorBundle2<Length,One>& bundle ) const override;

		void Transition( Length s, spat::Frame<Length,One>& frame ) const override;

		std::vector<Length> ZeroSet() const override;

		common::Interval<Length> Range() const override;

		spat::Vector<One> LocalUp() const noexcept override;

		spat::Frame<Length,One> GetCurveLocalTransformation() const noexcept override;

		std::unique_ptr<Curve> Clone() const override;

		bool Mirror( const spat::VectorBundle<Length,One>& mirrorPlane ) noexcept override;

		bool Equals( const Curve& toCurve, common::Interval<Length> range, 
			Length epsilon_length = epsilon__length, 
			Angle epsilon_angle = epsilon__angle ) const override;
	private:
		Data m_Data;

		void Interpolate( Length s, spat::Frame<Length,One>& frame, const CurveSample& start, const CurveSample& end ) const noexcept;
	};
}
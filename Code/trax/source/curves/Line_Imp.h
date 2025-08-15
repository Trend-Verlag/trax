// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2022 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/Curve.h"

namespace trax{

	class Line_Imp : public Line {
	public:

		// Curve:
		const char*	TypeName() const noexcept override;

		CurveType GetCurveType() const noexcept override;

		std::unique_ptr<Curve> Clone() const override;

		bool IsValid() const noexcept override;

		AnglePerLength Curvature( Length s ) const noexcept override;

		AnglePerLength Torsion( Length s ) const noexcept override;

		bool IsFlat() const noexcept override;

		void Transition( Length s, spat::Position<Length>& pos ) const noexcept override;

		void Transition( Length s, spat::Vector<One>& tan ) const noexcept override;

		void Transition( Length s, spat::VectorBundle<Length,One>& bundle ) const noexcept override;

		void Transition( Length s, spat::VectorBundle2<Length,One>& bundle ) const noexcept override;

		void Transition( Length s, spat::Frame<Length,One>& frame ) const noexcept override;

		std::vector<Length> ZeroSet() const noexcept override;

		common::Interval<Length> Range() const noexcept override;

		spat::Vector<One> LocalUp() const noexcept override;

		spat::Frame<Length,One> GetCurveLocalTransformation() const noexcept override;

		bool Mirror( const spat::VectorBundle<Length,One>& mirrorPlane ) noexcept override;

		bool Equals( const Curve& toCurve, common::Interval<Length> range, 
			Length epsilon_length = epsilon__length, 
			Angle epsilon_angle = epsilon__angle ) const noexcept override;
	};
}
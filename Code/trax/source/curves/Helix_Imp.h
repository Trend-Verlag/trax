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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/Curve_Imp.h"

namespace trax{

	class Helix_Imp : public CurvatureStrecher_Imp<Helix> {
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


		//Helix:
		spat::SquareMatrix<One,3> Jacobian( Length s ) const override;

		common::Interval<Length> Create( const Data& data ) override;

		const Data& GetData() const noexcept override;

		//CurvatureStrecher:
		spat::Vector<One> Direction( Length s ) const override;

		AnglePerLength Strech( Length s, const spat::Position<Length>& Z ) override;
	private:
		Data m_Data;
		const AnglePerLength& k = m_Data.k;
		const AnglePerLength& t = m_Data.t;
		Length a, b, sqrta2b2;
	};
};
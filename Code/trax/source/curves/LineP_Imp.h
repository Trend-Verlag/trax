//	trax track library
//	AD 2013 
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

	class LineFunction{
	public:
		using DataType = typename LineP::Data;

		LineFunction() = default;
		LineFunction( const LineFunction& func ) noexcept
			:	m_Data	{ func.m_Data },
				C		{ m_Data.vb.P },
				T		{ m_Data.vb.T }
		{}
		LineFunction( LineFunction&& func ) = delete;
		~LineFunction() = default;

		LineFunction& operator=( const LineFunction& func ) noexcept{
			m_Data = func.m_Data;
			return *this;
		}

		LineFunction& operator=( LineFunction&& ) = delete;


		inline Position<Length> operator()( Length s ) const noexcept{
			return P(s);
		}

		bool IsValid() const noexcept{
			return true;
		}

		AnglePerLength Curvature() const noexcept{
			return 0_1Im;
		}

		AnglePerLength Torsion() const noexcept{
			return 0_1Im;
		}

		Position<Length> P( Length s ) const noexcept{
			return C + s * T;
		}

		Vector<One> D1( Length ) const noexcept{
			return T;
		}

		Vector<Value<Dimension<-1,0,0>>> D2( Length ) const noexcept{
			return Null<Value<Dimension<-1,0,0>>>;
		}

		Vector<Value<Dimension<-2,0,0>>> D3( Length ) const noexcept{
			return Null<Value<Dimension<-2,0,0>>>;
		}

		common::Interval<Length> Range() const noexcept{ 
			return {-infinite__length,+infinite__length};
		}

		bool Mirror( const spat::VectorBundle<Length,One>& mirrorPlane );

		common::Interval<Length> Create( const DataType& data );

		const DataType& GetData() const noexcept{
			return m_Data;
		}
	private:
		DataType m_Data;
		const Position<Length>& C = m_Data.vb.P;
		const Vector<One>& T = m_Data.vb.T;
	};


	using Line_Base = CurveArcLength_Imp<LineFunction,LineP>;

	class LineP_Imp : public Line_Base{
	public:
		// Curve:
		const char*	TypeName() const noexcept override;

		CurveType GetCurveType() const noexcept override;

		std::unique_ptr<Curve> Clone() const override;

		bool Equals( const Curve& toCurve, Length epsilon_length = epsilon__length, Angle epsilon_angle = epsilon__angle ) const noexcept;

	//	using Line_Base::Equals;

		AnglePerLength Curvature( Length ) const noexcept override{
			return f.Curvature();
		}

		AnglePerLength Torsion( Length ) const noexcept override{
			return f.Torsion();
		}

		bool IsFlat() const noexcept override{
			return true;
		}

		spat::Vector<One> LocalUp() const noexcept override;

		// Line:
		using Line_Base::Create;

		common::Interval<Length> Create( const Position<Length>& start, const Position<Length>& end, const Vector<One>& up = Up ) override;

		void Create( const VectorBundle<Length,One>& vb, const Vector<One>& up = Up ) override;

		void Create( const spat::VectorBundle2<Length,One>& start ) override;

		void Create( const spat::Frame<Length,One>& start ) override;

		void Create( const Position<Length>& start, const Vector<One>& tan, const Vector<One>& up = Up ) override;
	};
}
//	trax track library
//	AD 2013 
//
// "Today is gonna be the day
//	That they're gonna throw it back to you"
//
//						Oasis
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

	using namespace spat;


	class ArcFunction{
	public:
		using DataType = typename ArcP::Data;

		ArcFunction() = default;
		ArcFunction( const ArcFunction& func ) noexcept
			:	m_Data	{ func.m_Data },
				C		{ m_Data.vb2.P },
				T		{ m_Data.vb2.T },
				N		{ func.N },
				r		{ func.r },
				c		{ func.c }
		{}
		ArcFunction( ArcFunction&& func ) = delete;
		~ArcFunction() = default;

		ArcFunction& operator=( const ArcFunction& func ){
			m_Data = func.m_Data;
			N = func.N;
			r = func.r;
			c = func.c;
			return *this;
		}

		ArcFunction& operator=( ArcFunction&& func ) = delete;


		inline Position<Length> operator()( Length s ) const noexcept{
			return P(s);
		}

		bool IsValid() const noexcept{
			return c > 0_1Im;
		}

		AnglePerLength Curvature() const noexcept{
			return c;
		}

		AnglePerLength Torsion() const noexcept{
			return AnglePerLength{0};
		}

		Position<Length> P( Length s ) const noexcept{
			return C + r * (sin(c*s) * T - cos(c*s) * N);
		}

		Vector<One> D1( Length s ) const noexcept{
			return cos(c*s) * T + sin(c*s) * N;
		}

		Vector<Value<Dimension<-1,0,0>>> D2( Length s ) const noexcept{
			return c * (-sin(c*s) * T + cos(c*s) * N);
		}

		Vector<Value<Dimension<-2,0,0>>> D3( Length s ) const noexcept{
			return -c*c * (cos(c*s) * T + sin(c*s) * N);
		}

		/// \verbatim
		/// 
		///            {  sin(s/r) }
		/// P(s) = r * { -cos(s/r) }
		///            {     0     }
		///
		///         { sin()  + r * cos() * (-s/pow<2>(r)) }
		/// dPIdr = { -cos() + r * sin() * (-s/pow<2>(r)) }
		///         {     0     }
		///
		///         { sin()  - s/r * cos() }
		/// dPIdr = { -cos() - s/r * sin() }
		///         {     0     }
		///              
		/// dPIdr = -N(s) - s/r * T(s)
		///
		/// \endverbatim
		spat::Matrix<One,2,3> Jacobian( Length s ) const{
			spat::Matrix<One,2,3> jacobian;
			if( !IsValid() )
				return jacobian;

			const One sin_ = sin(s/r);
			const One cos_ = cos(s/r);
			Vector<One> dPIdr = (sin_  - s/r * cos_) * T - (cos_ + s/r * sin_) * N;

			jacobian(0,0) = dPIdr[0u];
			jacobian(0,1) = dPIdr[1u];
			jacobian(0,2) = dPIdr[2u];

			Vector<One> _D1 = D1(s);
			jacobian(1,0) = _D1[0u];
			jacobian(1,1) = _D1[1u];
			jacobian(1,2) = _D1[2u];

			return jacobian;
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
		const Position<Length>& C = m_Data.vb2.P;
		const Vector<One>& T = m_Data.vb2.T;
		Vector<One> N;
		Length r{0};
		AnglePerLength c{0};
	};


	class ArcP_Imp : public CurvatureStrecher_Imp<CurveArcLength_Imp<ArcFunction,ArcP>>{
	public:
		// Curve:
		const char*	TypeName() const noexcept override;
		
		CurveType GetCurveType() const noexcept override;

		std::unique_ptr<Curve> Clone() const override;

		bool Equals( const Curve& toCurve, Length epsilon_length = epsilon__length, Angle epsilon_angle = epsilon__angle ) const noexcept;

		using CurveArcLength_Imp<ArcFunction,ArcP>::Equals;

		using CurveArcLength_Imp<ArcFunction,ArcP>::Curvature;

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

		// Arc:
		Length Radius() const noexcept override;

		spat::Matrix<One,2,3> Jacobian( Length s ) const override;

		using CurveArcLength_Imp<ArcFunction,ArcP>::Create;

		void Create( const VectorBundle2<Length,One>& center ) override;

		common::Interval<Length> Create( const VectorBundle<Length,One>& start, const Position<Length>& end, Angle e_angle = epsilon__angle ) override;

		common::Interval<Length> Create( const Position<Length>& start, const VectorBundle<Length,One>& end, Angle e_angle = epsilon__angle ) override;

		void Create( const Position<Length>& start, const Vector<One>& tan, const Vector<One>& nor, AnglePerLength curvature ) override;

		void Create( const VectorBundle2<Length,One>& start, AnglePerLength curvature ) override;

		// CurvatureStrecher:
		spat::Vector<One> Direction( Length s ) const override;

		AnglePerLength Strech( Length s, const spat::Position<Length>& Z ) override;
	};
}
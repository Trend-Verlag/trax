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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/Curve_Imp.h"

namespace trax{

	class CubicFunction{
		Vector<Length> _2c, _3d, _6d;
		bool m_bCreated;
	public:
		using DataType = typename Cubic::Data;
		DataType m_Data;

		CubicFunction() noexcept
			:	_2c{Null<Length>}, 
				_3d{Null<Length>}, 
				_6d{Null<Length>},
				m_bCreated{false}
		{}

		bool IsValid() const noexcept{
			return m_bCreated;
		}

		bool IsFlat() const noexcept;

		Position<Length> P( One t ) const noexcept{
			return m_Data.a + t*(m_Data.b + t*(m_Data.c + t*m_Data.d));
		}

		Vector<Length> D1( One t ) const noexcept{
			return m_Data.b + t*(_2c + t*_3d);
		}

		Vector<Length> D2( One t ) const noexcept{
			return _2c + t*_6d;
		}

		Vector<Length> D3( One ) const noexcept{
			return _6d;
		}

		Position<Length> operator()( One t ) const noexcept{
			return P(t);
		}

		spat::Vector<One> LocalUp() const;

		bool Mirror( const spat::VectorBundle<Length,One>& mirrorPlane );

		void Shorten( Interval<One> toRange );

		void Create( const Position<Length>& a, const Vector<Length>& b, const Vector<Length>& c, const Vector<Length>& d );

		void Create( const VectorBundle<Length>& start, const VectorBundle<Length>& end );

		void CreateBezier( const spat::Position<Length>& P0, const spat::Position<Length>& P1, const spat::Position<Length>& P2, const spat::Position<Length>& P3 );

		void Create( const DataType& data );

		const DataType& GetData() const noexcept{
			return m_Data;
		}
	};

	class Cubic_Imp : public Curve_Imp<CubicFunction,Cubic> {
	public:
		// Curve:
		const char*	TypeName() const noexcept override;

		CurveType GetCurveType() const noexcept override;

		std::unique_ptr<Curve> Clone() const override;

		bool IsFlat() const noexcept override;

		spat::Vector<One> LocalUp() const override;

		// Cubic:
		spat::Vector<Length> CubicOvershootAt( Length s ) const noexcept override;

		using Curve_Imp<CubicFunction,Cubic>::Create;

		Interval<Length> Create( const VectorBundle<Length,Length>& start, const VectorBundle<Length,Length>& end ) override;

		Interval<Length> CreateBezier( const spat::Position<Length>& P0, const spat::Position<Length>& P1, const spat::Position<Length>& P2, const spat::Position<Length>& P3 ) override;
		
		std::pair<common::Interval<Length>,Length> Create( const Curve& originalCurve, common::Interval<Length> range, Length maxDeviation = epsilon__length ) override;

		Interval<Length> Shorten( Interval<Length> toRange ) override;
	private:
		Length MaxDistance( const Curve& originalCurve, common::Interval<Length> range ) const;		
	};


	class SplineFunction{
	public:
		using DataType = typename Spline::Data;

		bool IsValid() const noexcept{
			for( const auto& cubic : m_CubicFunctions ){
				if( !cubic.IsValid() )
					return false;
			}

			return !m_CubicFunctions.empty();
		}

		bool IsFlat() const noexcept;

		Position<Length> P( One t ) const{
			return m_CubicFunctions.at(CalcCubicIdx(t)).P(t);
		}

		Vector<Length> D1( One t ) const{
			return m_CubicFunctions.at(CalcCubicIdx(t)).D1(t);
		}

		Vector<Length> D2( One t ) const{
			return m_CubicFunctions.at(CalcCubicIdx(t)).D2(t);
		}

		Vector<Length> D3( One t ) const{
			return m_CubicFunctions.at(CalcCubicIdx(t)).D3(t);
		}

		Position<Length> operator()( One t ) const{
			return P(t);
		}

		spat::Vector<One> LocalUp() const;

		bool Mirror( const spat::VectorBundle<Length,One>& mirrorPlane );

		Interval<One> Shorten( Interval<One> toRange );

		int CountControlPoints() const noexcept;

		Interval<One> ResetControlPoint( int idx, const VectorBundle<Length>& bundle );

		VectorBundle<Length> GetControlPoint( int idx ) const;

		bool HasGaps( Length epsilon = epsilon__length ) const noexcept;

		bool HasKinks( Angle epsilon = epsilon__angle ) const noexcept;

		Interval<One> Create( const std::vector<VectorBundle<Length>>& controlPoints );

		Interval<One> Create( const std::vector<Position<Length>>& controlPoints );

		Interval<One> Create( const std::vector<Position<Length>>& controlPoints, bool bClampedAtStart, const Vector<Length>& startTangent, bool bClampedAtEnd, const Vector<Length>& endTangent );

		Interval<One> CreateBezier( const std::vector<spat::Position<Length>>& controlPoints, Spline::WrapTypes wrap );

		Interval<One> Create( const DataType& data );

		const DataType& GetData() const noexcept{
			return m_Data;
		}
	private:
		DataType m_Data;
		std::vector<CubicFunction> m_CubicFunctions;

		inline std::size_t CalcCubicIdx( One& t ) const noexcept{
			Real idx;
			t = std::modf( t, &idx );
			if( idx == m_CubicFunctions.size() && t < epsilon ){
				idx -= 1;
				t = 1_1;
			}
			return static_cast<std::size_t>(idx);
		}

		//// For mathematical explanation see: Numerical Recipes in C++, Second Edition, Chapter3.3. W.Press, Cambridge University Press 2002.
		//// or https://www.math.ntnu.no/emner/TMA4215/2008h/cubicsplines.pdf
		static std::vector<double> SplineCalculateD2( const std::vector<double>& Y, bool bClampedAtStart, const double Yp1, bool bClampedAtEnd, const double Ypn );
		static double SplineEvaluate( const std::vector<double>& Y, const std::vector<double>& Y2, double x ) noexcept;
	};


	class Spline_Imp : public Curve_Imp<Reparametrization<SplineFunction>,Spline> {
	public:
		// Curve:
		const char*	TypeName() const noexcept override;

		CurveType GetCurveType() const noexcept override;

		std::unique_ptr<Curve> Clone() const override;

		bool IsFlat() const noexcept override;

		Vector<One> LocalUp() const override;


		// Spline:
		int CountControlPoints() const noexcept override;

		Interval<Length> ResetControlPointAndTangent( int idx, const VectorBundle<Length>& bundle ) override;

		VectorBundle<Length> GetControlPointAndTangent( int idx ) const override;

		Length GetParameter( int idx ) const override;

		int UpperBound( Length s ) const override;

		bool HasGaps( Length epsilon = epsilon__length ) const noexcept override;

		bool HasKinks( Angle epsilon = epsilon__angle ) const noexcept override;

		using Curve_Imp<Reparametrization<SplineFunction>,Spline>::Create;

		Interval<Length> Create( const Curve& originalCurve, common::Interval<Length> range, Length maxDeviation = epsilon__length, common::Interval<Length> sampleDistanceLimits = { 1_m, 1000_m } ) override;
		
		common::Interval<Length> Create( std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> originalCurve, Length maxDeviation = epsilon__length, common::Interval<Length> sampleDistanceLimits = { 1_m, 1000_m } ) override;

		Interval<Length> Create( const std::vector<VectorBundle<Length>>& controlPointsAndTangents ) override;

		Interval<Length> Create( const std::vector<Position<Length>>& controlPoints ) override;

		Interval<Length> Create( const std::vector<Position<Length>>& controlPoints, bool bClampedAtStart, const Vector<Length>& startTangent, bool bClampedAtEnd, const Vector<Length>& endTangent ) override;

		Interval<Length> CreateCatmullRom( const std::vector<Position<Length>>& controlPoints, One tension = 0.5f, WrapTypes wrap = WrapTypes::nonperiodic ) override;

		Interval<Length> CreateCatmullRom( const std::vector<spat::VectorBundle<Length,One>>& controlPoints, One tension = 0.5f, WrapTypes wrap = WrapTypes::nonperiodic ) override;

		Interval<Length> CreateBezier( const std::vector<spat::Position<Length>>& controlPoints, WrapTypes wrap = WrapTypes::nonperiodic ) override;

		Interval<Length> Shorten( common::Interval<Length> toRange ) override;
	private:

		// \returns The end parameter of the approximisation.
		Length Subdivide( Spline::Data& data, const Curve& originalCurve, common::Interval<Length> range, Length maxDeviation, common::Interval<Length> sampleDistanceLimits, bool bShorten = false );
	};
}
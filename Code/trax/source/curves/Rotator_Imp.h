//	trax track library
//	AD 2018 
//
//
//  You spin me right 'round, baby
//  Right 'round like a record, baby
//  Right 'round, 'round, 'round
//
//					Dead Or Alive
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

	class RotatorFunction{
	public:
		using DataType = typename Rotator::Data;

		RotatorFunction() = default;
		RotatorFunction( const RotatorFunction& other ) noexcept
			:	m_Data	{ other.m_Data },
				a		{ m_Data.a },
				b		{ m_Data.b },
				amb		{ other.amb },
				apb		{ other.apb }
		{}
		RotatorFunction( RotatorFunction&& other ) = delete;
		~RotatorFunction() = default;

		RotatorFunction& operator=( const RotatorFunction& other ) noexcept{
			m_Data	= other.m_Data;
			amb		= other.amb;
			apb		= other.apb;
			return *this;
		}
		RotatorFunction& operator=( RotatorFunction&& ) = delete;


		bool IsValid() const noexcept{
			return true;
		}

		AnglePerLength Curvature( Length s ) const noexcept{
			return sqrt( pow<2>(a*cos(b*s)) + pow<2>(b) );
		}

		Position<Length> operator()( const Length s ) const noexcept{
			return P(s);
		}

		Position<Length> P( const Length s ) const noexcept{
			if( a == 0_1Im && b == 0_1Im )
				return Position<Length>{ s,
										 0_m,
										 0_m };

			if( a == 0_1Im )
				return Position<Length>{ sin(b*s)/b,
										 0_m,
										 (1-cos(b*s))/b };
			
			if( b == 0_1Im )
				return Position<Length>{ sin(a*s)/a,
										 (1-cos(a*s))/a,
										 0_m };

			if( amb == 0_1Im || apb == 0_1Im )
				return Position<Length>{ s + sin(2*a*s)/(2*a),
										 (1-cos(2*a*s))/(2*a),
										 2*(1-cos(b*s))/b } / 2;

			return Position<Length>{ sin(amb*s)/amb + sin(apb*s)/apb,
									 (1-cos(amb*s))/amb + (1-cos(apb*s))/apb,
									 2*(1-cos(b*s))/b } / 2;
		}

		Vector<One> D1( const Length s ) const noexcept{
			return { cos(a*s) * cos(b*s), 
					 sin(a*s) * cos(b*s),
								sin(b*s) };
		}

		Vector<Value<Dimension<-1,0,0>>> D2( const Length s ) const noexcept{
			return { -a*sin(a*s) * cos(b*s) - b*cos(a*s) * sin(b*s), 
					  a*cos(a*s) * cos(b*s) - b*sin(a*s) * sin(b*s),
											  b*           cos(b*s) };
		}

		Vector<Value<Dimension<-2,0,0>>> D3( const Length s ) const noexcept{
			return {  2*a*b*sin(a*s) * sin(b*s) - (a*a+b*b) * cos(a*s) * cos(b*s),
					 -2*a*b*cos(a*s) * sin(b*s) - (a*a+b*b) * sin(a*s) * cos(b*s),
							   - b*b * sin(b*s) };
		}

		SquareMatrix<Real,3> Jacobian( const Length s ) const{
			SquareMatrix<Real,3> jacobian;

			if( a == 0_1Im && b == 0_1Im ){
				jacobian(0,0) = 0;
				jacobian(1,0) = 0;
				jacobian(0,1) = (s*s/2).Units();
				jacobian(1,1) = 0;
				jacobian(0,2) = 0;
				jacobian(1,2) = (s*s/2).Units();
			}

			else if( b == 0_1Im ){
				jacobian(0,0) = (s * cos(a*s)/a - sin(a*s)/(a*a)).Units();
				jacobian(1,0) = 0;
				jacobian(0,1) = (s * sin(a*s)/a - (1-cos(a*s))/(a*a)).Units();
				jacobian(1,1) = 0;
				jacobian(0,2) = 0;
				jacobian(1,2) = (s*s/2).Units();
			}

			else if( amb == 0_1Im ){
				jacobian(0,0) = (s * cos(apb*s)/apb - sin(apb*s)/(apb*apb)).Units() / 2;
				jacobian(1,0) = (s * cos(apb*s)/apb - sin(apb*s)/(apb*apb)).Units() / 2;
				jacobian(0,1) = (s * ( + s/2 + sin(apb*s)/apb ) - (1-cos(apb*s))/(apb*apb)).Units() / 2;
				jacobian(1,1) = (s * ( - s/2 + sin(apb*s)/apb ) - (1-cos(apb*s))/(apb*apb)).Units() / 2;
				jacobian(0,2) = 0;
				jacobian(1,2) = (s*sin(b*s)/b - (1-cos(b*s))/(b*b)).Units();
			}

			else if( apb == 0_1Im ){
				jacobian(0,0) = (s * cos(amb*s)/amb - sin(amb*s)/(amb*amb)).Units() / 2;
				jacobian(1,0) = (s * -cos(amb*s)/amb + sin(amb*s)/(amb*amb)).Units() / 2;
				jacobian(0,1) = (s * ( + sin(amb*s)/amb + s/2 ) - (1-cos(amb*s))/(amb*amb)).Units() / 2;
				jacobian(1,1) = (s * ( - sin(amb*s)/amb + s/2 ) + (1-cos(amb*s))/(amb*amb)).Units() / 2;
				jacobian(0,2) = 0;
				jacobian(1,2) = (s*sin(b*s)/b - (1-cos(b*s))/(b*b)).Units();
			}

			else{
				jacobian(0,0) = (s * ( + cos(amb*s)/amb + cos(apb*s)/apb ) - sin(amb*s)/(amb*amb) - sin(apb*s)/(apb*apb)).Units() / 2;
				jacobian(1,0) = (s * ( - cos(amb*s)/amb + cos(apb*s)/apb ) + sin(amb*s)/(amb*amb) - sin(apb*s)/(apb*apb)).Units() / 2;
				jacobian(0,1) = (s * ( + sin(amb*s)/amb + sin(apb*s)/apb ) - (1-cos(amb*s))/(amb*amb) - (1-cos(apb*s))/(apb*apb)).Units() / 2;
				jacobian(1,1) = (s * ( - sin(amb*s)/amb + sin(apb*s)/apb ) + (1-cos(amb*s))/(amb*amb) - (1-cos(apb*s))/(apb*apb)).Units() / 2;
				jacobian(0,2) = 0;
				jacobian(1,2) = (s*sin(b*s)/b - (1-cos(b*s))/(b*b)).Units();
			}

			const Vector<One> _D1{ D1(s) };
			jacobian(2,0) = _D1.dx;
			jacobian(2,1) = _D1.dy;
			jacobian(2,2) = _D1.dz;

			return jacobian;
		}

		common::Interval<Length> Range() const noexcept{ 
			return {-infinite__length,+infinite__length};
		}

		bool Mirror( const spat::VectorBundle<Length,One>& /*mirrorPlane*/ ) noexcept{
			assert( !"Not implemented yet!" );
			return false;
		}

		void Create( const DataType& data ) noexcept{
			m_Data = data;
			amb = a - b;
			apb = a + b;
		}

		const DataType& GetData() const noexcept{
			return m_Data;
		}
	private:
		DataType m_Data;
		const AnglePerLength& a = m_Data.a;
		const AnglePerLength& b = m_Data.b;
		AnglePerLength amb{0}, apb{0};
	};


	class Rotator_Imp : public CurveArcLength_Imp<RotatorFunction,Rotator>{
	public:
		using Base = CurveArcLength_Imp<RotatorFunction,Rotator>;

		const char*	TypeName() const noexcept override;

		CurveType GetCurveType() const noexcept override;

		std::unique_ptr<Curve> Clone() const override;

		AnglePerLength Curvature( Length s ) const noexcept override;

		bool IsFlat() const noexcept override;

		spat::Vector<One> LocalUp() const noexcept override;

		spat::Frame<Length,One> GetCurveLocalTransformation() const noexcept override;

		SquareMatrix<Real,3> Jacobian( Length s ) const override;

		common::Interval<Length> Create( const Data& data ) override;
	};


	class RotatorFunctionWithOffset{
	public:
		using DataType = typename Rotator::Data;

		RotatorFunctionWithOffset() = default;
		RotatorFunctionWithOffset( const RotatorFunctionWithOffset& other ) noexcept
			:	m_Data	{ other.m_Data },
				a0		{ m_Data.a0 },
				b0		{ m_Data.b0 },
				a		{ m_Data.a },
				b		{ m_Data.b },
				a0mb0	{ other.a0mb0 },
				a0pb0	{ other.a0pb0 },
				amb		{ other.amb },
				apb		{ other.apb }
		{}
		RotatorFunctionWithOffset( RotatorFunctionWithOffset&& other ) = delete;
		~RotatorFunctionWithOffset() = default;

		RotatorFunctionWithOffset& operator=( const RotatorFunctionWithOffset& other ) noexcept{
			m_Data	= other.m_Data;
			a0mb0	= other.a0mb0;
			a0pb0	= other.a0pb0;
			amb		= other.amb;
			apb		= other.apb;
			return *this;
		}
		RotatorFunctionWithOffset& operator=( RotatorFunctionWithOffset&& ) = delete;

		bool IsValid() const noexcept{
			return true;
		}

		AnglePerLength Curvature() const noexcept{
			return sqrt(b*b+a*a);
		}

		Position<Length> operator()( Length s ) const noexcept{
			return P(s);
		}

		Position<Length> P( Length s ) const noexcept{
			if( a == 0_1Im && b == 0_1Im )
				return Position<Length>{s * cos(a0) * cos(b0),
										s * sin(a0) * cos(b0),
										s * sin(b0) };
			
			if( a == 0_1Im )
				return Position<Length>{(sin(a0mb0 - b*s) - sin(a0mb0))/-b + (sin(a0pb0 + b*s) - sin(a0pb0))/b,
										(cos(a0mb0) - cos(a0mb0 - b*s))/-b + (cos(a0pb0) - cos(a0pb0 + b*s))/b,
										2*(cos(b0)-cos(b0 + b*s))/b } / 2;

			if( b == 0_1Im )
				return Position<Length>{(sin(a0+a*s) - sin(a0)) * cos(b0)/a,
										(cos(a0) - cos(a0+a*s)) * cos(b0)/a,
										sin(b0)*s };

			if( amb == 0_1Im )
				return Position<Length>{cos(a0mb0)*s + (sin(a0pb0 + apb*s) - sin(a0pb0)) / apb,
										sin(a0mb0)*s + (cos(a0pb0) - cos(a0pb0 + apb*s)) / apb,
										2*(cos(b0)-cos(b0+b*s))/b } / 2;

			if( apb == 0_1Im )
				return Position<Length>{cos(a0pb0)*s + (sin(a0mb0 + amb*s) - sin(a0mb0)) / amb,
										sin(a0pb0)*s + (cos(a0mb0) - cos(a0mb0 + amb*s)) / amb,
										2*(cos(b0)-cos(b0+b*s))/b } / 2;

			return Position<Length>{(sin(a0mb0 + amb*s) - sin(a0mb0))/amb + (sin(a0pb0 + apb*s) - sin(a0pb0))/apb,
									(cos(a0mb0) - cos(a0mb0 + amb*s))/amb + (cos(a0pb0) - cos(a0pb0 + apb*s))/apb,
									2*(cos(b0)-cos(b0 + b*s))/b } / 2;
		}

		Vector<One> D1( Length s ) const noexcept{
			return { cos(a0+a*s) * cos(b0+b*s), 
					 sin(a0+a*s) * cos(b0+b*s),
								sin(b0+b*s) };
		}

		Vector<Value<Dimension<-1,0,0>>> D2( Length s ) const noexcept{
			return { -a*sin(a0+a*s) * cos(b0+b*s) - b*cos(a0+a*s) * sin(b0+b*s), 
					  a*cos(a0+a*s) * cos(b0+b*s) - b*sin(a0+a*s) * sin(b0+b*s),
											  b*           cos(b0+b*s) };
		}

		Vector<Value<Dimension<-2,0,0>>> D3( Length s ) const noexcept{
			return {  2*a*b*sin(a0+a*s) * sin(b0+b*s) - (a*a+b*b) * cos(a0+a*s) * cos(b0+b*s),
					 -2*a*b*cos(a0+a*s) * sin(b0+b*s) - (a*a+b*b) * sin(a0+a*s) * cos(b0+b*s),
							   - b*b * sin(b0+b*s) };
		}

		bool Mirror( const spat::VectorBundle<Length,One>& /*mirrorPlane*/ ) noexcept{
			return false;
		}

		void Create( const DataType& data ) noexcept{
			m_Data = data;
			a0mb0	= a0 - b0;
			a0pb0	= a0 + b0;
			amb		= a - b;
			apb		= a + b;
		}

		common::Interval<Length> Range() const noexcept{ 
			return {-infinite__length,+infinite__length};
		}

		const DataType& GetData() const noexcept{
			return m_Data;
		}
	private:
		DataType m_Data;
		const Angle& a0 = m_Data.a0;
		const Angle& b0 = m_Data.b0;
		const AnglePerLength& a = m_Data.a;
		const AnglePerLength& b = m_Data.b;
		Angle a0mb0{0}, a0pb0{0};
		AnglePerLength amb{0}, apb{0};
	};


	class RotatorWithOffset_Imp : public CurveArcLength_Imp<RotatorFunctionWithOffset,Rotator>{
	public:
		const char*	TypeName() const noexcept override;

		CurveType GetCurveType() const noexcept override;

		bool IsFlat() const noexcept override;

		spat::Vector<One> LocalUp() const noexcept override;

		spat::Frame<Length,One> GetCurveLocalTransformation() const noexcept override;

		SquareMatrix<Real,3> Jacobian( Length s ) const noexcept override;

		std::unique_ptr<Curve> Clone() const override;
	};


	class RotatorChainFunction{
	public:
		using DataType = typename RotatorChain::Data;

		bool IsValid() const noexcept{
			for( const auto& pair : m_FunctionsAndTotalPositionOffsets ){
				if( !pair.first.IsValid() )
					return false;
			}

			return !m_FunctionsAndTotalPositionOffsets.empty();
		}

		AnglePerLength Curvature() const noexcept{
			return m_MaxCurvature;
		}

		Position<Length> operator()( Length s ) const{
			return P(s);
		}

		Position<Length> P( const Length s ) const{
			Length si{0};	
			for( std::size_t i = 0; i < m_Data.size(); ++i ){
				if( si + std::get<2>(m_Data[i]) >= s )
					return m_FunctionsAndTotalPositionOffsets[i].first( s-si ) + m_FunctionsAndTotalPositionOffsets[i].second;
				si += std::get<2>(m_Data[i]);
			}

			throw std::invalid_argument( "Curve parameter out of range!" );
		}

		Vector<One> D1( Length s ) const{
			Length si{0};
			for( std::size_t i = 0; i < m_Data.size(); ++i ){
				if( si + std::get<2>(m_Data[i]) >= s )
					return m_FunctionsAndTotalPositionOffsets[i].first.D1( s-si );
				si += std::get<2>(m_Data[i]);
			}

			throw std::invalid_argument( "Curve parameter out of range!" );
		}

		Vector<Value<Dimension<-1,0,0>>> D2( Length s ) const{
			Length si{0};
			for( std::size_t i = 0; i < m_Data.size(); ++i ){
				if( si + std::get<2>(m_Data[i]) >= s )
					return m_FunctionsAndTotalPositionOffsets[i].first.D2( s-si );
				si += std::get<2>(m_Data[i]);
			}

			throw std::invalid_argument( "Curve parameter out of range!" );
		}

		Vector<Value<Dimension<-2,0,0>>> D3( Length s ) const{
			Length si{0};
			for( std::size_t i = 0; i < m_Data.size(); ++i ){
				if( si + std::get<2>(m_Data[i]) >= s )
					return m_FunctionsAndTotalPositionOffsets[i].first.D3( s-si );
				si += std::get<2>(m_Data[i]);
			}

			throw std::invalid_argument( "Curve parameter out of range!" );
		}

		bool Mirror( const spat::VectorBundle<Length,One>& /*mirrorPlane*/ ) noexcept{
			return false;
		}

		void Create( const DataType& data ){
			m_Data = data;

			m_FunctionsAndTotalPositionOffsets.clear();
			m_Range.Init();
			Angle as = 0;
			Angle bs = 0;
			m_MaxCurvature = 0_1Im;
			Vector<Length> offset{ Null<Length> };

			for( const auto& tuple : m_Data ){
				const auto& length = std::get<2>(tuple);

				m_Range.m_Far += length;

				RotatorFunctionWithOffset f;
				f.Create( {
					std::get<0>(tuple)/length, 
					std::get<1>(tuple)/length,
					as,
					bs } );

				m_MaxCurvature = std::max( m_MaxCurvature, f.Curvature() );

				as += std::get<0>(tuple);
				bs += std::get<1>(tuple);

				m_FunctionsAndTotalPositionOffsets.push_back( std::make_pair(f,offset) );

				offset += f(length) - Origin3D<Length>;
			}
		}

		common::Interval<Length> Range() const noexcept{ 
			return m_Range;
		}

		const DataType& GetData() const noexcept{
			return m_Data;
		}
	private:
		DataType m_Data;
		common::Interval<Length> m_Range{0_m,0_m};
		AnglePerLength m_MaxCurvature{0};
		std::vector<std::pair<RotatorFunctionWithOffset,Vector<Length>>> m_FunctionsAndTotalPositionOffsets;
	};


	class RotatorChain_Imp : public CurveArcLength_Imp<RotatorChainFunction,RotatorChain>{
	public:
		using Base = CurveArcLength_Imp<RotatorChainFunction,RotatorChain>;

		const char*	TypeName() const noexcept override;

		CurveType GetCurveType() const noexcept override;

		std::unique_ptr<Curve> Clone() const override;

		bool IsFlat() const noexcept override;

		std::vector<Length> ZeroSet() const override;

		spat::Vector<One> LocalUp() const noexcept override;

		spat::Frame<Length,One> GetCurveLocalTransformation() const noexcept override;

		common::Interval<Length> Create( const spat::Vector<Length>& advance ) override;

		common::Interval<Length> Create( const Data& data ) override;
	};

}

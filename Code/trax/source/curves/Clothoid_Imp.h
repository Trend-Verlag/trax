//	trax track library
//	AD 2018 
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

#include "trax/Units.h"
#include "spat/Position2D.h"
#include "spat/Vector2D.h"

namespace trax{


	class ClothoidFunction{
	public:
		using DataType = typename Clothoid::Data;

		ClothoidFunction() = default;
		ClothoidFunction( const ClothoidFunction& func ) noexcept
			:	m_Data	{ func.m_Data },
				a		{ m_Data.a },
				a2		{ func.a2 }
		{}
		ClothoidFunction( ClothoidFunction&& func ) = delete;
		~ClothoidFunction() = default;

		ClothoidFunction& operator=( const ClothoidFunction& func ) noexcept{
			m_Data = func.m_Data;
			a2 = func.a2;
			return *this;
		}

		ClothoidFunction& operator=( ClothoidFunction&& ) = delete;


		bool IsValid() const noexcept{
			return a2 > 0_m2;
		}

		AnglePerLength Curvature( Length s ) const noexcept{
			return s/a2;
		}

		AnglePerLength Curvature() const noexcept{
			return Curvature( MaximumLength() );
		}

		AnglePerLength Torsion() const noexcept{
			return 0_1Im;
		}

		static const Angle maximumAngle;

		/// From series expansion of the positional integral (https://de.wikipedia.org/wiki/Klothoide)
		/// it follows:
		///	\verbatim
		/// xi = pow(-1,i) * s * pow(T,2i) / ((2i)! * (4i+1))
		/// yi = pow(-1,i) * s * pow(T,2i+1) / ((2i+1)! * (4i+3))
		/// x = sum( xi ) i= [0,infinite]
		/// y = sum( yi ) i= [0,infinite]
		/// \endverbatim
		/// Because the above formula evaluates very badly 
		/// due to the huge nominators and divisors, we evaluate
		/// each addend by using the prevous one: 
		///	\verbatim
		/// xi = x(i-1) * kxi,
		/// yi = x(i-1) * kyi, 
		///
		/// with i > 0 and
		///
		/// kxi = - pow<2>(T) * (4(i-1)+1) / (2i*(2i-1)*(4i+1))
		/// kyi = - pow<2>(T) * (4(i-1)+3) / ((2i+1)*2i*(4i+3))
		/// \endverbatim
		Position<Length> P( Length s ) const{
			const Angle T = s*s/(2*a2);
			if( T > maximumAngle + epsilon__angle )
				throw std::invalid_argument( "The clothoid can not be evaluated for s greater than " + std::to_string( MaximumLength().Units() ) );
			
			Vector2D<Length> addend{ s, s*T/3 };
			Position2D<Length> Pos{ Origin2D<Length> + addend };
			for( int i = 1; addend.Length() > epsilon__length; ++i ){
				const auto factors = - T*T/(2*i) * Vector2D<Real>{ static_cast<Real>(4*(i-1)+1) / ((2*i-1)*(4*i+1)),
															 static_cast<Real>(4*(i-1)+3) / ((2*i+1)*(4*i+3)) };
				addend.dx *= factors.dx;
				addend.dy *= factors.dy;
				Pos += addend;
			}

			return Position<Length>{Pos};
		}

		Vector<One> D1( Length s ) const noexcept{
			return { cos(s*s/(2*a2)), 
					 sin(s*s/(2*a2)), 
					 0_1 };
		}

		Vector<Value<Dimension<-1,0,0>>> D2( Length s ) const noexcept{
			return s/a2 * Vector<One>{ -sin(s*s/(2*a2)), 
										cos(s*s/(2*a2)), 
										0_1 };
		}

		Vector<Value<Dimension<-2,0,0>>> D3( Length s ) const noexcept{
			return -1_1/a2 * Vector<One>{ cos(s*s/(2*a2)) * s*s/a2 + sin(s*s/(2*a2)),
										  sin(s*s/(2*a2)) * s*s/a2 - cos(s*s/(2*a2)),
										  0_1 };
		}


		/// It is (with a2 being a for notational simplification):
		///	\verbatim
		///
		///        s { cos(t*t/(2*a)) }
		/// P(s) = S { sin(t*t/(2*a)) } * dt
		///        0 {       0        }
		/// 
		///            s                { -sin(t*t/(2*a)) }
		/// dP(s)/da = S -t*t/(2*a*a) * {  cos(t*t/(2*a)) } * dt
		///            0                {       0         }
		///
		///            s                { sum( pow<i+1>(-1) * pow<2i+1>(t*t/(2*a)) / (2i+1)! ) }
		/// dP(s)/da = S -t*t/(2*a*a) * { sum( pow<i>(-1)   * pow<2i>(t*t/(2*a))   / (2i)!     } * dt
		///            0                {                     0                                }
		///
		///                       s              { pow<i+1>(-1) * pow<2i+1>(t*t/(2*a)) / (2i+1)! }
		/// dP(s)/da = 1/a * sum( S -t*t/(2*a) * { pow<i>(-1)   * pow<2i>(t*t/(2*a))   / (2i)!   } * dt )
		///                       0              {                     0                         }
		///
		///                       s { pow<i>(-1)   * pow<2i+2>(t*t/(2*a)) / (2i+1)! }
		/// dP(s)/da = 1/a * sum( S { pow<i+1>(-1) * pow<2i+1>(t*t/(2*a)) / (2i)!   } * dt )
		///                       0 {                     0                         }
		///
		///                       s { pow<i>(-1)   * pow<4i+4>(t) / pow<2i+2>(2*a) / (2i+1)! }
		/// dP(s)/da = 1/a * sum( S { pow<i+1>(-1) * pow<4i+2>(t) / pow<2i+1>(2*a) / (2i)!   } * dt )
		///                       0 {                     0                                  }
		///
		///                       { pow<i>(-1)   * pow<4i+5>(s) / pow<2i+2>(2*a) / (2i+1)! / (4i+5) }
		/// dP(s)/da = 1/a * sum( { pow<i+1>(-1) * pow<4i+3>(s) / pow<2i+1>(2*a) / (2i)! / (4i+3)   } )
		///                       {                     0                                           }
		///
		/// xi = x(i-1) * (-1) * pow<4>(s) / pow<2>(2*a) / (2i) * (4i+1) / (2i+1) / (4i+5);
		/// yi = y(i-1) * (-1) * pow<4>(s) / pow<2>(2*a) / (2i) * (4i-1) / (2i-1) / (4i+3);
		///
		/// x0 = pow<5>(s) / (20*a*a);
		/// y0 = -pow<3>(s) / (6*a);
		///
		/// \endverbatim
		/// 
		SquareMatrix<One,2> Jacobian( Length s ) const{
			SquareMatrix<One,2> jacobian;

			const Angle T = pow<4>(s) / (4*a2*a2);
			Vector2D<Length> addend{ pow<5>(s) / (20*a2*a2), -pow<3>(s) / (6*a2) };
			Vector2D<Length> dPIda2{ addend };

			for( int i = 1; addend.Length() > epsilon__length; ++i ){
				const auto factors = -T/(2*i) * Vector2D<One>{	static_cast<Real>( (4*i+1) / (2*i+1) / (4*i+5) ),
															static_cast<Real>( (4*i-1) / (2*i-1) / (4*i+3) )};

				addend.dx *= factors.dx;
				addend.dy *= factors.dy;
				dPIda2 += addend;
			}

			jacobian(0,0) = 2 * (dPIda2.dx/a);
			jacobian(0,1) = 2 * (dPIda2.dy/a);

			const auto _D1 = D1( s );
			jacobian(1,0) = _D1.dx;
			jacobian(1,1) = _D1.dy;

			return jacobian;
		}


		Position<Length> operator()( Length s ) const{
			return P(s);
		}

		common::Interval<Length> Range() const noexcept{ 
			return {0_m,MaximumLength()};
		}

		inline Length MaximumLength() const noexcept{
			return sqrt(2*maximumAngle*a2);
		}
		
		bool Mirror( const spat::VectorBundle<Length,One>& /*mirrorPlane*/ ) noexcept{
			return false;
		}

		common::Interval<Length> Create( const DataType& data );

		const DataType& GetData() const noexcept{
			return m_Data;
		}
	private:
		Clothoid::Data m_Data;
		const Length& a = m_Data.a;
		Area a2;
	};


	class Clothoid_Imp : public CurvatureStrecher_Imp<CurveArcLength_Imp<ClothoidFunction,Clothoid>>{
	public:
		// Curve:
		const char*	TypeName() const noexcept override;

		CurveType GetCurveType() const noexcept override;

		std::unique_ptr<Curve> Clone() const override;

		AnglePerLength Curvature( Length s ) const noexcept override{
			return f.Curvature(s);
		}

		AnglePerLength Torsion( Length ) const noexcept override{
			return f.Torsion();
		}
		
		bool IsFlat() const noexcept override{
			return true;
		}

		spat::Vector<One> LocalUp() const noexcept override;

		//Clothoid:
		spat::SquareMatrix<One,2> Jacobian( Length s ) const override;

		using CurveArcLength_Imp<ClothoidFunction,Clothoid>::Create;

		common::Interval<Length> Create( Area a2 ) override;

		common::Interval<Length> Create( AnglePerLength curvature0, AnglePerLength curvature1, Length length ) override;

		common::Interval<Length> Create( Length radius0, Length radius1, Length length ) override;

		// CurvatureStrecher:
		spat::Vector<One> Direction( Length s ) const override;

		AnglePerLength Strech( Length s, const spat::Position<Length>& Z ) override;
	};
}

// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2013 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#pragma once

#include "trax/Curve_Imp.h"

namespace trax{

	using namespace spat;

	class HelixFunction{
	public:
		using DataType = typename HelixP::Data;

		HelixFunction() = default;
		HelixFunction( const HelixFunction& func ) noexcept
			:	m_Data		{ func.m_Data },
				a			{ m_Data.a },
				b			{ m_Data.b },
				C			{ m_Data.center.P },
				T			{ m_Data.center.T },
				N			{ m_Data.center.N },
				B			{ func.B },
				m_sqrta2b2	{ func.m_sqrta2b2 },
				c			{ func.c },
				t			{ func.t }
		{}
		HelixFunction( HelixFunction&& func ) = delete;
		~HelixFunction() = default;

		HelixFunction& operator=( const HelixFunction& func ) noexcept{
			m_Data = func.m_Data;
			B = func.B;
			m_sqrta2b2 = func.m_sqrta2b2;
			c = func.c;
			t = func.t;
			return *this;
		}

		HelixFunction& operator=( HelixFunction&& ) = delete;


		bool IsValid() const noexcept{
			return m_sqrta2b2 > 0_m;
		}

		AnglePerLength Curvature() const noexcept{
			return c;
		}

		AnglePerLength Torsion() const noexcept{
			return t;
		}

		Position<Length> P( Length s ) const noexcept{
			return C + (a * cos(s/m_sqrta2b2)) * T + 
					   (a * sin(s/m_sqrta2b2)) * N + 
					   (b *     s/m_sqrta2b2)  * B;
		}

		Vector<One> D1( Length s ) const noexcept{
			return	(a/m_sqrta2b2 * -sin(s/m_sqrta2b2)) * T + 
					(a/m_sqrta2b2 *  cos(s/m_sqrta2b2)) * N + 
					(b/m_sqrta2b2					  ) * B;
		}

		Vector<Value<Dimension<-1,0,0>>> D2( Length s ) const noexcept{
			return	(a/pow<2>(m_sqrta2b2) * -cos(s/m_sqrta2b2)) * T + 
					(a/pow<2>(m_sqrta2b2) * -sin(s/m_sqrta2b2)) * N;
		}

		Vector<Value<Dimension<-2,0,0>>> D3( Length s ) const noexcept{
			return	(a/pow<3>(m_sqrta2b2) *  sin(s/m_sqrta2b2)) * T + 
					(a/pow<3>(m_sqrta2b2) * -cos(s/m_sqrta2b2)) * N;
		}

		/// \verbatim
		/// a2pb2 = pow<2>(a) + pow<2>((b)
		/// a2 = pow<2>(a)
		/// b2 = pow<2>(b)
		/// 
		///         { a*cos( s/sqrt(a2pb2) ) }
		/// P(s) =  { a*sin( s/sqrt(a2pb2) ) }
		///         { b*s/sqrt(a2pb2)		 }
		/// 
		///         { cos() - a*sin() * (-1/2) * s/sqrt(a2pb2)/(a2pb2) * 2a }
		/// dP/da = { sin() + a*cos() * (-1/2) * s/sqrt(a2pb2)/(a2pb2) * 2a }
		///         {               b * (-1/2) * s/sqrt(a2pb2)/(a2pb2) * 2a }
		///
		///         { cos() + a2*s*sin() / (sqrt(a2pb2)*(a2pb2)) }
		/// dP/da = { sin() - a2*s*cos() / (sqrt(a2pb2)*(a2pb2)) } 
		///         {              -ab*s / (sqrt(a2pb2)*(a2pb2)) }
		///
		///         {         - a*sin() * (-1/2) * s/sqrt(a2pb2)/(a2pb2) * 2b }
		/// dP/db = {           a*cos() * (-1/2) * s/sqrt(a2pb2)/(a2pb2) * 2b } 
		///         { s/sqrt(a2pb2) + b * (-1/2) * s/sqrt(a2pb2)/(a2pb2) * 2b }
		///
		///         {   ba*sin()/(a2pb2) }
		/// dP/db = {  -ba*cos()/(a2pb2) } * s/sqrt(a2pb2)
		///         { 1 - b2/(a2pb2)     }
		///
		///         {   b*sin() }
		/// dP/db = {  -b*cos() } * a*s/sqrt(a2pb2)/(a2pb2)
		///         {    a      }
		///
		/// \endverbatim
		spat::SquareMatrix<One,3> Jacobian(Length s) const{
			spat::SquareMatrix<One,3> jacobian;
			if( !IsValid() )
				return jacobian;

			const One sin_ = sin(s/m_sqrta2b2);
			const One cos_ = cos(s/m_sqrta2b2);
			const Area a2b2 = a*a + b*b;

			Vector<One> dPIda = (cos_ + sin_ * (a / a2b2) * (a / m_sqrta2b2) * s ) * T +
								(sin_ - cos_ * (a / a2b2) * (a / m_sqrta2b2) * s ) * N +
								(             -(a / a2b2) * (b / m_sqrta2b2) * s ) * B;

			Vector<One> dPIdb = (s/a2b2) * (a/m_sqrta2b2) *
                                ((  b * sin_ ) * T + 
								 ( -b * cos_ ) * N +
								 (  a        ) * B);

			jacobian(0,0) = dPIda[0];
			jacobian(0,1) = dPIda[1];
			jacobian(0,2) = dPIda[2];

			jacobian(1,0) = dPIdb[0];
			jacobian(1,1) = dPIdb[1];
			jacobian(1,2) = dPIdb[2];

			Vector<One> D1_ = D1( s );
			jacobian(2,0) = D1_[0];
			jacobian(2,1) = D1_[1];
			jacobian(2,2) = D1_[2];

			return jacobian;
		}

		Position<Length> operator()( Length s ) const noexcept{
			return P(s);
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
		const Length& a = m_Data.a;
		const Length& b = m_Data.b;
		const Position<Length>& C = m_Data.center.P;
		const Vector<One>& T = m_Data.center.T;
		const Vector<One>& N = m_Data.center.N;
		Vector<One> B;
		Length m_sqrta2b2{0};
		AnglePerLength c{0};
		AnglePerLength t{0};
	};


	class HelixP_Imp : public CurvatureStrecher_Imp<CurveArcLength_Imp<HelixFunction,HelixP>>
	{
	public:
		// Curve:
		const char*	TypeName() const noexcept override;

		CurveType GetCurveType() const noexcept override;

		std::unique_ptr<Curve> Clone() const override;

		bool Equals( const Curve& toCurve, Length epsilon_length = epsilon__length, Angle epsilon_angle = epsilon__angle ) const noexcept;

		using CurveArcLength_Imp<HelixFunction,HelixP>::Equals;

		AnglePerLength Curvature( Length ) const noexcept override{
			return f.Curvature();
		}

		AnglePerLength Torsion( Length ) const noexcept override{
			return f.Torsion();
		}

		bool IsFlat() const noexcept override;

		spat::Vector<One> LocalUp() const noexcept override;

		spat::Frame<Length,One> GetCurveLocalTransformation() const noexcept override;

		// Helix:
		VectorBundle2<Length,One> Center() const noexcept override;

		Length Radius() const noexcept override;

		One Slope() const noexcept override;

		spat::SquareMatrix<One,3> Jacobian( Length s ) const override;

		using CurveArcLength_Imp<HelixFunction,HelixP>::Create;

		void Create( const Frame<Length,One>& start, AnglePerLength curvature, AnglePerLength torsion ) override;

		common::Interval<Length> Create( const VectorBundle<Length,One>& start, const Position<Length>& end, const Vector<One>& up = Up, Angle e_angle = epsilon__angle ) override;

		common::Interval<Length> Create( const Position<Length>& start, const VectorBundle<Length,One>& end, const Vector<One>& up = Up, Angle e_angle = epsilon__angle ) override;

		void Create( const VectorBundle2<Length,One>& center, AnglePerLength curvature, AnglePerLength torsion ) override;

		// CurvatureStrecher:
		spat::Vector<One> Direction( Length s ) const override;

		AnglePerLength Strech( Length s, const spat::Position<Length>& Z ) override;
	};

}
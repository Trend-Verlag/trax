//	trax track library
//	AD 2014
//
//  "Here comes the story of the Hurricane"
//
//							Bob Dylan
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

#include "trax/RoadwayTwist.h"
#include "trax/Curve.h"
#include "trax/Track.h"

#include <algorithm>
#include <cassert>
#include <vector>

namespace trax{

	template<class Base>
	class RoadwayTwist_Imp : public Base{
	public:
		const char*	TypeName() const noexcept override{
			return "ZeroTwist"; }

		typename Base::TwistType GetTwistType() const noexcept override{
			return Base::TwistType::Zero;
		}

		bool IsValid() const noexcept override{
			return true;
		}

		using Base::Twist;

		Angle Twist( Length s ) const override{
			return DoTwist(s) + ZeroFlip(s);
		}

		AnglePerLength D1( Length s ) const override{
			return DoD1(s);
		}

		bool ZeroFlip( bool bActive ) noexcept override{
			const bool retval = m_bZeroFlip;
			m_bZeroFlip = bActive;
			return retval;
		}

		void Flip() noexcept override{
			if( m_bZeroFlip && m_ZeroSet.size() % 2 )
				Offset( pi );
		}

		void Mirror() noexcept override{}

		bool Offset( Angle ) noexcept override{
			return false;
		}

		void Freeze( bool /*bFreeze = true*/ ) noexcept override{
		}

		bool IsDynamic() const noexcept override{
			return false;
		}

		bool IsDirectionalConstant() const noexcept override{
			return false;
		}

		bool IsConstant() const noexcept override{
			return false;
		}

		bool IsFrozen() const noexcept override{
			return true;
		}

		bool IsNull() const noexcept override{
			return false;
		}

		bool Equals( const RoadwayTwist& toTwist, Angle /*epsilon_angle = epsilon__angle*/ ) const noexcept override{
			if( this == &toTwist )
				return true;

			return false;
		}

		void OnAttach( const TrackBuilder& track ) noexcept override{
			auto curve = track.GetCurve();
			m_CurveSegment = curve.second;
			if( curve.first ){
				m_ZeroSet = curve.first->ZeroSet();

				for( auto& c : m_ZeroSet )
					c = s(c);

				if( !m_CurveSegment.Normal() )
					std::reverse( m_ZeroSet.begin(), m_ZeroSet.end() );
			}
		}

		void OnDetach() noexcept override{
			m_CurveSegment = {0_m,0_m};
			m_ZeroSet.clear();
		}

		Angle DoTwist( Length ) const override{ 
			return 0_rad; 
		}

		AnglePerLength DoD1( Length ) const override{ 
			return 0_1Im;
		}
	protected:
		inline Length TrackLength() const noexcept{ return abs(m_CurveSegment.Length()); }

		inline One Turn() const noexcept{
			return m_CurveSegment.Normal() ? +1.f : -1.f;
		}
		inline Length s( Length c ) const noexcept{
			return (c - m_CurveSegment.Near()) / Turn();
		}
		inline Length c( Length s ) const noexcept{
			return m_CurveSegment.Near() + Turn() * s;
		}

		common::Interval<Length> m_CurveSegment = {0_m,0_m};
	private:
		std::vector<Length> m_ZeroSet;
		bool m_bZeroFlip = true;
	//	bool m_bFlip = false;

		Angle ZeroFlip( Length s ) const noexcept{
			if( !m_bZeroFlip || m_ZeroSet.empty() )
				return 0_rad;

			for( int i = common::narrow_cast<int>(m_ZeroSet.size()) - 1; i >= 0; --i )
				if( s >= m_ZeroSet[i] )
					return (i % 2) ? 0_rad : pi;

			return 0_rad;
		}
	};


	class ConstantTwist_Imp : public RoadwayTwist_Imp<ConstantTwist>{
	public:
		ConstantTwist_Imp( Real twistvalue = 0.0f ) noexcept;
		ConstantTwist_Imp( const RoadwayTwist& fromTwist );

		const char*	TypeName() const noexcept override;

		TwistType GetTwistType() const noexcept override;

		std::unique_ptr<RoadwayTwist> Clone() const override;

		void Flip() noexcept override;

		void Mirror() noexcept override;

		bool Offset( Angle offset ) noexcept override;

		bool IsConstant() const noexcept override{
			return true;
		}

		bool IsNull() const noexcept override{
			return m_Twist == 0_deg;
		}

		bool Equals( const RoadwayTwist& toTwist, Angle epsilon_angle = epsilon__angle ) const noexcept override;

		Angle DoTwist( Length s ) const noexcept override;

		AnglePerLength DoD1( Length s ) const override;

		void TwistValue( Angle twist ) noexcept override;

		Angle TwistValue() const noexcept override;

		const Data& GetData() const noexcept override;

		void Create( const Data& data ) noexcept override;
	private:
		Data m_Twist;
	};


	class LinearTwist_Imp : public RoadwayTwist_Imp<LinearTwist>{
	public:
		LinearTwist_Imp( Angle from = 0_deg, Angle to = 0_deg ) noexcept;
		LinearTwist_Imp( const RoadwayTwist& fromTwist );

		const char*	TypeName() const noexcept override;

		TwistType GetTwistType() const noexcept override;

		std::unique_ptr<RoadwayTwist> Clone	() const override;

		void Flip() noexcept override;

		void Mirror() noexcept override;

		bool Offset( Angle offset ) noexcept override;

		bool IsConstant() const noexcept override{
			return m_Data.Near() == m_Data.Far();
		}

		bool IsNull() const noexcept override{
			return m_Data.Near() == 0_deg && m_Data.Far() == 0_deg;
		}

		bool Equals( const RoadwayTwist& toTwist, Angle epsilon_angle = epsilon__angle ) const noexcept override;

		Angle DoTwist( Length s ) const override;

		AnglePerLength DoD1( Length s ) const override;

		void From( Angle twist ) noexcept override;

		Angle From() const noexcept override;

		void To( Angle twist ) noexcept override;

		Angle To() const noexcept override;

		const Data& GetData() const noexcept override;

		void Create( const Data& data ) noexcept override;

	private:
		Data m_Data;
	};


	template<class Base>
	class PiecewiseTwist_ImpBase : public RoadwayTwist_Imp<Base>{
	public:
		using DataType = typename Base::Data;
		using PairType = typename Base::Data::value_type;

		//RoadwayTwist:
		void Flip() noexcept override{
			RoadwayTwist_Imp<Base>::Flip();

			std::reverse( m_Data.begin(), m_Data.end() );
			for( auto& value : m_Data )
				value.first = RoadwayTwist_Imp<Base>::TrackLength() - value.first;
		}

		//PiecewiseTwist:
		void Add( Length s, const typename PairType::second_type& twist ) override{
			if( s < 0_m )
				throw std::invalid_argument( this->TypeName() + std::string{ "::Add: Inavid parameter s." } );

			typename DataType::iterator iter = Upper_Bound( s );
			if( iter != m_Data.begin() && common::Equals( (iter-1)->first, s, epsilon__length ) )
				(iter-1)->second = twist;
			else if( iter != m_Data.end() && common::Equals( iter->first, s, epsilon__length ) )
				iter->second = twist;
			else
				m_Data.insert( iter, {s,twist} );
		}

		void Add( const PairType& pair ) override{
			Add( pair.first, pair.second );
		}

		int CntTwistValues() const override{
			return common::narrow_cast<int>(m_Data.size());
		}

		using RoadwayTwist_Imp<Base>::Twist;

		PairType Twist( int idx ) const override{
			return m_Data.at( common::narrow_cast<size_t>(idx) );
		}
		
		void Create( const DataType& data ) override{
			if( data.size() < 2 )
				throw std::logic_error( this->TypeName() + std::string{ " not enough control points." } );

			m_Data = data;
			std::sort( m_Data.begin(), m_Data.end(), 
				[]( const PairType& pairA, const PairType& pairB ){ return pairA.first < pairB.first; } );
		}

		const DataType& GetData() const noexcept override{
			return m_Data;
		}

	protected:
		void Bracket( Length s, typename DataType::const_iterator& iA, typename DataType::const_iterator& iB ) const{
			assert( m_Data.size() >= 2 );

			auto iter = Upper_Bound( s );

			if( iter == m_Data.begin() ){
				iA = iter;
				iB = iter+1;
			}
			else if( iter == m_Data.end() ){
				iA = iter-2;
				iB = iter-1;
			}
			else{
				iA = iter-1;
				iB = iter;
			}
		}

		DataType m_Data;
	private:
		
		inline typename DataType::const_iterator Upper_Bound( Length s ) const{
			return std::upper_bound( 
				m_Data.cbegin(), 
				m_Data.cend(), 
				PairType{s,typename PairType::second_type{}}, 
				[]( const PairType& pairA, const PairType& pairB ){ return pairA.first < pairB.first; } );
		}

		inline typename DataType::iterator Upper_Bound( Length s ){
			return std::upper_bound( 
				m_Data.begin(), 
				m_Data.end(), 
				PairType{s,typename PairType::second_type{}}, 
				[]( const PairType& pairA, const PairType& pairB ){ return pairA.first < pairB.first; } );
		}

	};


	class PiecewiseTwist_Imp : public PiecewiseTwist_ImpBase<PiecewiseTwist>{
	public:
		PiecewiseTwist_Imp() = default;
		PiecewiseTwist_Imp( const RoadwayTwist& fromTwist );

		//RoadwayTwist:
		const char*	TypeName() const noexcept override;

		TwistType GetTwistType() const noexcept override;

		std::unique_ptr<RoadwayTwist> Clone() const override;

		void Flip() noexcept override;

		void Mirror() noexcept override;

		bool Offset( Angle offset ) noexcept override;

		bool IsConstant() const noexcept override;

		bool IsNull() const noexcept override;

		bool Equals( const RoadwayTwist& toTwist, Angle epsilon_angle = epsilon__angle ) const noexcept override;

		Angle DoTwist( Length s ) const override;

		AnglePerLength DoD1( Length s ) const override;
	};


	class PiecewiseLinearTwist_Imp : public PiecewiseTwist_Imp{
	public:
		PiecewiseLinearTwist_Imp() = default;
		PiecewiseLinearTwist_Imp( const RoadwayTwist& fromTwist );

		const char*	TypeName() const noexcept override;

		TwistType GetTwistType() const noexcept override;

		std::unique_ptr<RoadwayTwist> Clone() const override;

		Angle DoTwist( Length s ) const override;
	};


	class PiecewiseCircularTwist_Imp : public PiecewiseTwist_Imp{
	public:
		PiecewiseCircularTwist_Imp() = default;
		PiecewiseCircularTwist_Imp( const RoadwayTwist& fromTwist );

		const char*	TypeName() const noexcept override;

		TwistType GetTwistType() const noexcept override;

		std::unique_ptr<RoadwayTwist> Clone() const override;

		Angle DoTwist( Length s ) const override;

		AnglePerLength DoD1( Length s ) const override;
	};


	/// The function of a B vector rotated around T by 'a' at parameter value s is:
	///
	///	B(a,s) = cos(a)*B(s) - sin(a)*N(s)  (1)
	///
	/// With T,N,B being the comoving frame vectors and B(a,s) happening to be 
	/// a unit vector. For an attractor A being a unit vector, the angle 'b' be 
	/// the angle between B(a,s) and A:
	///
	/// cos(b(a,s)) = B(a,s) * A	b:[0,pi]	(2)
	///
	/// We are looking for an a = a0(s) for that the b(a0,s) would get minimal. For this 
	/// the cos in (2) would become maximal. And this happens exactly where:
	///
	/// d cos(b) / da = 0, meaning:  0 = dB/da * A
	///	
	/// -> 0 = -AB sin(a) - AN cos(a) for a=a0
	///
	/// If AB == 0 -> a0 = +-pi/2, else a0 = -arctan(AN/AB) (4)
	/// 
	/// Now, D1 = da0/ds = -1 / (1 + pow<2>(AN/AB)) * (A*dN/ds / AB - AN/pow<2>(AB) * A*dB/ds)
	/// 
	/// But from the Frenet - Formulas it follows: dN/ds = -kT + tB and dB/ds = -tN, so:
	///
	/// D1	= -1 / (1 + pow<2>(AN/AB))  * (A*(-kT + tB) / AB - AN/pow<2>(AB) * A*(-tN))
	///		= -1 / (1 + pow<2>(AN/AB))  * (-k*AT/AB + t*(1 + pow<2>(AN/AB)))
	///		= k * AT/AB / (1 + pow<2>(AN/AB)) - t  (5)
	template<class Base>
	class DirectionalTwist_ImpBase : public RoadwayTwist_Imp<Base>{
	public:
		DirectionalTwist_ImpBase(){
			RoadwayTwist_Imp<Base>::ZeroFlip( false );
		}

		// RoadwayTwist:
		bool IsValid() const noexcept override{
			return m_pCurve && m_pCurve->IsValid();
		}

		bool ZeroFlip( bool ) noexcept override { return false; }

		void Flip() noexcept override{}

		bool IsDirectionalConstant() const noexcept override{
			return true;
		}

		void OnAttach( const TrackBuilder& track ) noexcept override{
			RoadwayTwist_Imp<Base>::OnAttach( track );
			m_pCurve = track.GetCurve().first;
		}

		void OnDetach() noexcept override{
			m_pCurve.reset();
			RoadwayTwist_Imp<Base>::OnDetach();
		}
	protected:
		std::shared_ptr<const Curve> m_pCurve;

		using RoadwayTwist_Imp<Base>::c;
		using RoadwayTwist_Imp<Base>::m_CurveSegment;
		using RoadwayTwist_Imp<Base>::Turn;

		inline Angle DoTwist_( Length s, const spat::Vector<One>& A ) const{
			if( m_pCurve == nullptr )
				return 0;

			spat::Frame<Length,One> F;
			m_pCurve->Transition( c(s), F );

			if( !m_CurveSegment.Normal() ){
				F.T *= -1;
				F.B *= -1;
			}

			const Real AB = A*F.B;
			const Real AN = A*F.N;
			if( std::abs(AB) < epsilon && std::abs(AN) < epsilon )
			// This is a special situation in which our above solution brakes down; it 
			// delivers no information at all. We solve this problem, by evaluating a nearby
			// point of the curve and take its solution:
			{
				if( m_pCurve->GetCurveType() == Curve::CurveType::Line )
					return 0;

				if( !m_bReentryBlock ) // Still there might exist curves, that do that..	
				{
					common::FlagBlocker block{ m_bReentryBlock };
					return DoTwist_( s + (s < epsilon__length ? +epsilon__length : -epsilon__length), A );
				}
			}
			
			return -std::atan2( AN, AB );
		}

		inline AnglePerLength DoD1_( Length s, const spat::Vector<One>& A ) const{
			if( m_pCurve == nullptr )
				return 0_1Im;

			spat::Frame<Length,One> F;
			m_pCurve->Transition( c(s), F );

			if( !m_CurveSegment.Normal() ){
				F.T *= -1;
				F.B *= -1;
			}

			if( const Real AB = A*F.B )
				return m_pCurve->Curvature(c(s)) * (A*F.T/AB) / (1 + std::pow(A*F.N/AB,2)) - Turn() * m_pCurve->Torsion(c(s));
			else
				return -Turn() * m_pCurve->Torsion(c(s));
		}

	private:
		mutable bool m_bReentryBlock = false;
	};


	class DirectionalTwist_Imp : public DirectionalTwist_ImpBase<DirectionalTwist>
	{
	public:
		DirectionalTwist_Imp() noexcept;
		DirectionalTwist_Imp( const RoadwayTwist& fromTwist );
		DirectionalTwist_Imp( const DirectionalTwist_Imp& twist );
		DirectionalTwist_Imp( DirectionalTwist_Imp&& ) = delete;
		DirectionalTwist_Imp( const spat::Vector<One>& attractor );
		~DirectionalTwist_Imp() = default;

		DirectionalTwist_Imp& operator=( const DirectionalTwist_Imp& twist ) = delete;
		DirectionalTwist_Imp& operator=( DirectionalTwist_Imp&& ) = delete;


		// DirectionalTwist:
		void Attractor( const spat::Vector<One>& attractor ) override;

		const spat::Vector<One>& Attractor() const noexcept override;

		const spat::Vector<One>& Attractor( bool bGlobal ) const noexcept override;
		
		std::shared_ptr<const Curve> GetCurve() const noexcept override{
			return m_pCurve;
		}

		const Data& GetData() const noexcept override;

		void Create( const Data& data ) override;


		// RoadwayTwist:
		const char*	TypeName() const noexcept override;
		
		TwistType GetTwistType() const noexcept override;

		std::unique_ptr<RoadwayTwist> Clone() const override;

		bool IsDynamic() const noexcept override{
			return true;
		}

		void Freeze( bool bFreeze = true ) noexcept override{
			m_bFrozen = bFreeze;
		}

		bool IsFrozen() const noexcept override{
			return m_bFrozen;
		}

		bool IsNull() const noexcept override;

		bool Equals( const RoadwayTwist& toTwist, Angle epsilon_angle = epsilon__angle ) const noexcept override;

		void OnAttach( const TrackBuilder& track ) noexcept override;

		void OnDetach() noexcept override;

		Angle DoTwist( Length s ) const override;
		
		AnglePerLength DoD1( Length s ) const override;
	private:
		spat::Vector<One> GlobalA, A;
		bool m_bFrozen = false;
		bool m_bAttached = false;
	};


	class PiecewiseDirectionalTwist_Imp : public DirectionalTwist_ImpBase<PiecewiseTwist_ImpBase<PiecewiseDirectionalTwist>>{
	public:
		PiecewiseDirectionalTwist_Imp() = default;
		PiecewiseDirectionalTwist_Imp( const RoadwayTwist& fromTwist );

		//RoadwayTwist:
		const char*	TypeName() const noexcept override;

		TwistType GetTwistType() const noexcept override;

		std::unique_ptr<RoadwayTwist> Clone() const override;

		bool IsDirectionalConstant() const noexcept override{
			return false;
		}

		bool Equals( const RoadwayTwist& toTwist, Angle epsilon_angle ) const noexcept override;

		Angle DoTwist( Length s ) const override;

		AnglePerLength DoD1( Length s ) const override;

		//PiecewiseTwist:
		void Add( Length s, const spat::Vector<One>& attractor ) override;

		void Create( const Data& data ) override;
	};


	class CombinedTwist_Imp : public RoadwayTwist_Imp<CombinedTwist>{
	public:
		CombinedTwist_Imp();
		CombinedTwist_Imp( const RoadwayTwist& fromTwist );
		CombinedTwist_Imp( const CombinedTwist_Imp& twist );
		CombinedTwist_Imp( CombinedTwist_Imp&& ) = delete;
		~CombinedTwist_Imp() = default;

		CombinedTwist_Imp& operator=( const CombinedTwist_Imp& twist ) = delete;
		CombinedTwist_Imp& operator=( CombinedTwist_Imp&& ) = delete;


		// Inherited via CombinedTwist
		const char*	TypeName() const noexcept override;
		
		TwistType GetTwistType() const noexcept override;

		std::unique_ptr<RoadwayTwist> Clone() const override;

		bool IsValid() const noexcept override;

		void Flip() noexcept override;

		void Mirror() noexcept override;

		bool Offset( Angle offset ) noexcept override;

		void Freeze( bool bFreeze = true ) noexcept override;

		bool IsDynamic() const noexcept override;

		bool IsDirectionalConstant() const noexcept override;

		bool IsConstant() const noexcept override;

		bool IsNull() const noexcept override;

		bool IsFrozen() const noexcept override;

		bool Equals( const RoadwayTwist& toTwist, Angle epsilon_angle = epsilon__angle ) const noexcept override;

		Angle DoTwist( Length s ) const override;

		AnglePerLength DoD1( Length s ) const override;

		void OnAttach( const TrackBuilder& track ) noexcept override;

		void OnDetach() noexcept override;

		void AttachTwist1( std::unique_ptr<RoadwayTwist> pTwist ) override;

		RoadwayTwist& Twist1() const noexcept override;

		std::unique_ptr<RoadwayTwist> DetachTwist1() override;

		void AttachTwist2( std::unique_ptr<RoadwayTwist> pTwist ) override;

		RoadwayTwist& Twist2() const noexcept override;

		std::unique_ptr<RoadwayTwist> DetachTwist2() override;
	private:
		std::unique_ptr<RoadwayTwist> m_pTwist1;
		std::unique_ptr<RoadwayTwist> m_pTwist2;
	};

}
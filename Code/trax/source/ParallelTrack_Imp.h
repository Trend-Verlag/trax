//	trax track library
//	AD 2019 
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

#include "trax/ParallelTrack.h"
#include "Track_Imp.h"
#include "trax/Curve_Imp.h"
#include "RoadwayTwist_Imp.h"


namespace trax{

	class ParallelizableTrack_Imp : public Track_Imp,
									public ParallelizableTrack
	{
	public:
		// Track:
		std::shared_ptr<const ParallelizableTrack> GetParallelizableTrack() const noexcept override;

		std::shared_ptr<ParallelizableTrack> GetParallelizableTrack() noexcept override;


		// ParallelTrack:
		std::shared_ptr<TrackBuilder> AddParallel( const spat::Vector2D<Length>& shift, bool bTrackRelative ) override;

		void RemoveParallel( const TrackBuilder& parallelTrack ) override;

		bool IsParallel( const TrackBuilder& parallelTrack ) const noexcept override;

		void Clear() override;

		int CountTracks() const override;

		int ParallelIdx( const TrackBuilder& parallelTrack ) const override;

		std::shared_ptr<TrackBuilder> GetParallel( int idx ) const override;

		const Data& GetParallelData( int idx ) const override;

		Length ParallelParameterFrom( int idx, Length sOriginalParameter ) const override;

		Length OriginalParameterFrom( int idx, Length sParallelParameter ) const override;
	protected:
		// TrackBuilder:
		void OnGeometryChanged() noexcept override;
	private:
		std::vector<std::pair<std::shared_ptr<TrackBuilder>,Data>> m_ParallelTrackList;

		void UpdateParallels();
		void UpdateParallel( const std::pair<std::shared_ptr<TrackBuilder>,Data>& parallel );

		class ParallelFunction{
		public:
			using DataType = typename PointToPointParallel::Data;
			DataType m_Data;

			Position<Length> P( One u ) const;

			Vector<Length> D1( One u ) const;

			Vector<Length> D2( One ) const{
				return Null<Length>;
			}

			Vector<Length> D3( One ) const{
				return Null<Length>;
			}

			Position<Length> operator()( One u ) const{
				return P(u);
			}

			bool IsValid() const noexcept;

			bool Mirror( const spat::VectorBundle<Length,One>& /*mirrorPlane*/ ) noexcept{
				return false;
			}

			Interval<Real> Create( const DataType& data ) noexcept;

			const DataType& GetData() const noexcept{
				return m_Data;
			} 	
		private:
			const Length& h = m_Data.d.dx;
			const Length& v = m_Data.d.dy;
		};


		template<class Function, class Base>
		class ParallelBase_Imp : public CurveTInterpolator_Imp<Function,Base>{
		public:
			using CurveTInterpolator_Imp<Function,Base>::GetData;

			bool IsFlat() const noexcept override{
				if( f.f.m_Data.pTrack && f.f.m_Data.pTrack->IsValid() )
					return f.f.m_Data.pTrack->IsFlat();

				return false;
			}

			std::vector<Length> ZeroSet() const override{
				if( f.f.m_Data.pTrack )
				{
					const TrackBuilder& track = dynamic_cast<const TrackBuilder&>(*f.f.m_Data.pTrack);

					if( track.GetTwist().IsDirectionalConstant() )
					{
						std::vector<Length> zeroSet = track.GetCurve().first->ZeroSet();
						for( Length& zero : zeroSet )
							zero = ParallelParameterFrom( zero );

						return zeroSet;
					}
				}

				return CurveTInterpolator_Imp<Function,Base>::ZeroSet();
			}

			spat::Vector<One> LocalUp() const override{
				if( f.f.m_Data.pTrack && f.f.m_Data.pTrack->IsValid() )
					return f.f.m_Data.pTrack->LocalUp();

				return spat::Null<One>;
			}

			Length OriginalParameterFrom( Length sParallelParameter ) const noexcept override{
				assert( GetData().pTrack );
				return GetData().pTrack->GetLength() * t(sParallelParameter);
			}

			Length ParallelParameterFrom( Length sOriginalParameter ) const noexcept override{
				assert( GetData().pTrack );
				return s( sOriginalParameter / GetData().pTrack->GetLength() );
			}
		protected:
			using CurveTInterpolator_Imp<Function,Base>::f;
			using CurveTInterpolator_Imp<Function,Base>::t;
			using CurveTInterpolator_Imp<Function,Base>::s;
		};


		class ParallelCurve_Imp : public ParallelBase_Imp<Reparametrization<ParallelFunction>,PointToPointParallel>{
		public:
			const char*	TypeName() const noexcept override;

			CurveType GetCurveType() const noexcept override;

			std::unique_ptr<Curve> Clone() const override;

			bool IsTrackRelative() const noexcept override{
				return true;
			}
		};

		friend bool dclspc Equals( const PointToPointParallel::Data& A, const PointToPointParallel::Data& B, common::Interval<Length> range, 
			Length epsilon_length, Angle epsilon_angle ) noexcept;


		class ParallelTwist : public RoadwayTwist_Imp<RoadwayTwist>{
		public:
			ParallelTwist( const ParallelizableTrack& m_Parent ) noexcept;

			const char* TypeName() const noexcept override;

			TwistType GetTwistType() const noexcept override;

			std::unique_ptr<RoadwayTwist> Clone	() const override;

			bool ZeroFlip( bool ) noexcept override { return false; }

			void Flip() noexcept override{}

			void OnAttach( const TrackBuilder& track ) noexcept override;

			void OnDetach() noexcept override;

			Angle DoTwist( Length s ) const override;

			AnglePerLength DoD1( Length s ) const override;

		private:
			const ParallelizableTrack& m_ParallelizableTrack;
			std::shared_ptr<const ParallelCurve_Imp> m_pCurve;
		};


		class PlaneParallelFunction{
		public:
			using DataType = typename PointToPointParallel::Data;
			DataType m_Data;

			Position<Length> P( One u ) const;

			Vector<Length> D1( One u ) const;

			Vector<Length> D2( One ) const{
				return Null<Length>;
			}

			Vector<Length> D3( One ) const{
				return Null<Length>;
			}

			Position<Length> operator()( One u ) const{
				return P(u);
			}

			bool IsValid() const noexcept;

			bool Mirror( const spat::VectorBundle<Length,One>& /*mirrorPlane*/ ) noexcept{
				return false;
			}

			Interval<Real> Create( const DataType& data ) noexcept;

			const DataType& GetData() const noexcept{
				return m_Data;
			} 	
		private:
			const Length& h = m_Data.d.dx;
			const Length& v = m_Data.d.dy;
		};


		class PlaneParallelCurve_Imp : public ParallelBase_Imp<Reparametrization<PlaneParallelFunction>,PointToPointParallel>{
		public:
			const char* TypeName() const noexcept override;

			CurveType GetCurveType() const noexcept override;

			std::unique_ptr<Curve> Clone() const override;

			bool IsTrackRelative() const noexcept override{
				return false;
			}
		};
	};
}
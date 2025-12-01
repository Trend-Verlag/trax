//	trax track library
//	AD 2022 
//
//  "99 Kriegsminister
//   Streichholz und Benzinkanister"
//
//				Nena
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

#include <algorithm>

namespace trax{

/*
	class PolygonalChain_Imp : public PolygonalChain{
	public:

		// PolygonalChain
		Length GetParameter( int idx ) const override;

		int UpperBound( Length s ) const  override;

		common::Interval<Length> Create( const Curve& originalCurve, common::Interval<Length> range, Length maxDeviation, Length minPointDistance ) override;

		common::Interval<Length> Create( std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> originalCurve, Length maxDeviation, Length minPointDistance ) override;

		common::Interval<Length> Create( const Data& data ) override;

		const Data& GetData() const noexcept override;	

		
		// Curve:
		std::string	TypeName() const override{
			return "PolygonalChain";
		}

		CurveType GetCurveType() const noexcept override{
			return CurveType::PolygonalChain;
		}

		bool IsValid() const noexcept override;

		AnglePerLength Curvature( Length s ) const override;

		AnglePerLength Torsion( Length s ) const override;

		bool IsFlat() const noexcept override;

		void Transition( Length s, spat::Position<Length>& pos ) const override;

		void Transition( Length s, spat::Vector<One>& tan ) const override;

		void Transition( Length s, spat::VectorBundle<Length,One>& bundle ) const override;

		void Transition( Length s, spat::VectorBundle2<Length,One>& bundle ) const override;

		void Transition( Length s, spat::Frame<Length,One>& frame ) const override;

		std::vector<Length> ZeroSet() const override;

		common::Interval<Length> Range() const override;

		spat::Vector<One> LocalUp() const noexcept override;

		std::unique_ptr<Curve> Clone() const override;

		bool Mirror( const spat::VectorBundle<Length,One>& mirrorPlane ) noexcept override;

		bool Equals( const Curve& toCurve, common::Interval<Length> range, 
			Length epsilon_length = epsilon__length, 
			Angle epsilon_angle = epsilon__angle ) const override;

		const CurvatureStrecher* GetCurvatureStrecher() const noexcept override{
			return nullptr;
		}

		CurvatureStrecher* GetCurvatureStrecher() noexcept override{
			return nullptr;
		}

	private:
		Data m_Data;

		struct Vertex{
			spat::Frame<Length,One> F;
			Length s;
			AnglePerLength k;
			AnglePerLength t;

			 inline operator const Data::value_type&() noexcept{
				 return F.P;
			}
		};

		std::vector<Vertex> m_Vertices;

		inline friend bool operator<( 
			const Vertex& a, 
			const Vertex& b ) noexcept
		{
			return a.s < b.s;
		}

		inline std::vector<Vertex>::const_iterator GetSegmentStart( const Length s ) const{
			std::vector<Vertex>::const_iterator iter = std::upper_bound( m_Vertices.cbegin(), m_Vertices.cend(), Vertex{ {}, s, 0_1Icm, 0_1Icm } );
			if( iter == m_Vertices.cend() )
				--iter;
			return iter == m_Vertices.begin() ? iter : iter - 1;
		}
	};

*/
	class PolygonalChain_Imp : public PolygonalChain
	{
	public:

		// PolygonalChain:
		Length GetParameter( int idx ) const override;

		int UpperBound( Length s ) const override;

		common::Interval<Length> Create( const Curve& originalCurve, common::Interval<Length> range, Length maxDeviation, Length minPointDistance ) override;

		common::Interval<Length> Create( std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> originalCurve, Length maxDeviation, Length minPointDistance ) override;

		common::Interval<Length> Create( const std::vector<spat::Position<Length>>& samples, WrapTypes wrap = WrapTypes::nonperiodic ) override;

		common::Interval<Length> CreateCatmullRom( const std::vector<spat::Position<Length>>& samples, WrapTypes wrap = WrapTypes::nonperiodic ) override;

		common::Interval<Length> Create( const Data& data ) override;

		const Data& GetData() const noexcept override;


		// Curve:
		const char*	TypeName() const noexcept override{
			return "PolygonalChain";
		}

		CurveType GetCurveType() const noexcept override{
			return CurveType::PolygonalChain;
		}

		bool IsValid() const noexcept override;

		AnglePerLength Curvature( Length s ) const override;

		AnglePerLength Torsion( Length s ) const override;

		bool IsFlat() const noexcept override;

		void Transition( Length s, spat::Position<Length>& pos ) const override;

		void Transition( Length s, spat::Vector<One>& tan ) const override;

		void Transition( Length s, spat::VectorBundle<Length,One>& bundle ) const override;

		void Transition( Length s, spat::VectorBundle2<Length,One>& bundle ) const override;

		void Transition( Length s, spat::Frame<Length,One>& frame ) const override;

		std::vector<Length> ZeroSet() const override;

		common::Interval<Length> Range() const override;

		spat::Vector<One> LocalUp() const noexcept override;

		spat::Frame<Length,One> GetCurveLocalTransformation() const noexcept override;

		std::unique_ptr<Curve> Clone() const override;

		bool Mirror( const spat::VectorBundle<Length,One>& mirrorPlane ) noexcept override;

		bool Equals( const Curve& toCurve, common::Interval<Length> range, 
			Length epsilon_length = epsilon__length, 
			Angle epsilon_angle = epsilon__angle ) const override;

	private:
		Data m_Data;
		std::vector<Length> m_DataSamplesParameters;
		std::vector<CurveSample> m_Vertices;

		inline std::vector<CurveSample>::const_iterator GetSegmentStart( const Length s ) const{
			std::vector<CurveSample>::const_iterator iter = std::upper_bound( m_Vertices.cbegin(), m_Vertices.cend(), CurveSample{ {}, s, 0_1Icm, 0_1Icm } );
			if( iter == m_Vertices.cend() )
				--iter;
			return iter == m_Vertices.begin() ? iter : iter - 1;
		}
	};

}

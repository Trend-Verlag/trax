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

#include "trax/Section.h"

#include <vector>

namespace trax{

	class Section_Imp : public Section{
	public:
		using Section::SectionPoint;

		Section_Imp() noexcept;

		// Section:
		std::string	TypeName() const noexcept override;

		SpecialSections	GetSectionType() const noexcept override;

		void Create( SpecialSections specialSection ) override;

		int	CountPoints() const override;

		void SetCntPoints( int cnt ) override;

		const SectionPoint&	Get( int id ) const override;

		common::Interval<One> TextureExtent() const override;

		Length PolygonChainLength() const noexcept override;

		Length Gauge() const noexcept override;

		spat::Rect<Length> Clearance() const noexcept override;

		void Set( int id, const SectionPoint& pt, bool bGaugeLeft = false, bool bGaugeRight = false ) override;

		void Scale( One x_scale, One y_scale ) override;

		void CalculateNormals() noexcept override;

		void CalculateTextureCoordinates() override;

	protected:
		SpecialSections				m_SectionType;
		std::vector<SectionPoint>	m_SectionPoints;
		int							m_GaugeLeft, m_GaugeRight;
	};

	void CreateStandardProfile		( Section& section );

	void CreateVignolProfile_UIC60	( Section& section );

	void CreatePipeline				( Section& section, int segments );


}
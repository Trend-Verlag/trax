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

#include "trax/Section.h"

#include <vector>

namespace trax{

	class Section_Imp : public Section{
	public:
		using Section::SectionPoint;

		Section_Imp() noexcept;

		// Section:
		std::string	TypeName() const override;

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
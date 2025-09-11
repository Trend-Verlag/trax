//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//
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

#include "../StaticTrack.h"
#include "../Material.h"
#include "trax/source/SectionTrack_Imp.h"

namespace trax{

	class StaticTrack_Imp : public StaticTrack,
							public SectionTrack_Imp			
	{
	public:
		StaticTrack_Imp( const Scene& scene );


		// Track:
		TrackType GetTrackType() const noexcept override;

		bool IsValid() const noexcept override;


		// StaticTrack:
		void SetMaterial( const Material& material ) noexcept override;

		const Material& GetMaterial() const noexcept override;

		const Shape& GetShape() const noexcept override;

	protected:
		// PoseImp:
		void PropagateAbsoluteFrameToClients() noexcept override;

		// TrackBuilder:
		void OnGeometryChanged() noexcept override;
	private:
		const Scene&					m_Scene;
		std::unique_ptr<Shape>			m_pShape;
		Material						m_Material;

		void CreateShape() noexcept;
	};

}
//	trax track library
//	AD 2023 
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

#include "../Shape.h"
#include "Geom_Imp.h"

#ifdef _WIN32
#	pragma warning( disable : 4250 ) // inherits via dominance warning
#endif

namespace trax{

	class Shape_ImpBase : public virtual Shape{
	public:
		Shape_ImpBase() noexcept;


		//Shape:
		spat::Box<Length> GetExtent() const noexcept override;

		int Attach( std::unique_ptr<Geom> pGeom ) override;

		int Attach( std::vector<std::unique_ptr<Geom>>& geoms ) override;

		std::unique_ptr<Geom> Remove( int idx ) override;

		void Clear() noexcept override;

		int Count() const override;

		Geom& Get( int idx ) const override;
	private:
		std::vector<std::unique_ptr<Geom>> m_Geoms;
	};
}
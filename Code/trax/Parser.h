//	trax track library
//	AD 2024 
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

#include "common/Interval.h"
#include "dim/DimensionedValues.h"
#include "spat/Frame.h"
#include "spat/Box.h"


namespace trax{

	class Parser{
	public:
		Parser(){
			m_Area.Init();
			m_Frame.Init();
		};
		virtual ~Parser() = default;

		virtual bool ParsingStart() noexcept(false) { return true; }

		virtual void Frame( 
			const spat::Frame<dim::Length,dim::One>& frame ) noexcept 
		{ 
			m_Frame = frame;
		}

		virtual void Area( 
			const spat::Box<dim::Length>& area ) noexcept 
		{ 
			m_Area = area;
		}

		virtual void Interval( 
			const common::Interval<dim::Length>& interval ) noexcept 
		{ 
			m_Interval = interval;
		}

		virtual void DisplayName( 
			const std::string& /*language*/, 
			const std::string& /*name*/ ) noexcept {}

		virtual void Description( 
			const std::string& /*language*/, 
			const std::string& /*description*/ ) noexcept {}

		virtual void ParsingEnd() noexcept {}
	protected:
		common::Interval<dim::Length> m_Interval;
		spat::Box<dim::Length> m_Area;
		spat::Frame<dim::Length,dim::One> m_Frame;
	};


} // namespace trax

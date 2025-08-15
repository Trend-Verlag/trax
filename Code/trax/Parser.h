// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2025 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2024 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

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

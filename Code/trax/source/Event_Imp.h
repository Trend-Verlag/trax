//	trax track library
//	AD 2014 
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

#include "trax/Sensor.h"
#include "trax/Event.h"
#include "trax/ImplementationHelper.h"

namespace trax{

	class Enevt_Imp : public Event{
	public:

	};


	//class EventType : public EventFilter{
	//public:
	//	EventType() = default;
	//	EventType( EventFilter::Type type, const std::string& name )
	//		:	m_Type			{ type },
	//			m_Name			{ name },
	//			m_bAtTrainTip	{ false },
	//			m_bAtTrainEnd	{ false }
	//	{}

	//	std::string Name() const override{
	//		return m_Name;
	//	}

	//	void Name( const std::string& name ) override{
	//		m_Name = name;
	//	}

	//	Type GetType() const noexcept override{ 
	//		return m_Type; 
	//	}

	//	void SetType( Type type ) noexcept override{
	//		m_Type = type;
	//	}

	//	void TrainTip( bool bTrainTip ) noexcept override{
	//		m_bAtTrainTip = bTrainTip;
	//	}

	//	bool TrainTip() const noexcept override{
	//		return m_bAtTrainTip;
	//	}

	//	void TrainEnd( bool bTrainEnd ) noexcept override{
	//		m_bAtTrainEnd = bTrainEnd;
	//	}

	//	bool TrainEnd() const noexcept override{
	//		return m_bAtTrainEnd;
	//	}
	//private:
	//	EventFilter::Type m_Type = EventFilter::Type::none;
	//	std::string m_Name;
	//	bool m_bAtTrainTip = false;
	//	bool m_bAtTrainEnd = false;
	//};
}
// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/Sensor.h"
#include "trax/Event.h"
#include "trax/ImplementationHelper.h"

namespace trax{

	class EventType : public EventFilter{
	public:
		EventType() = default;
		EventType( EventFilter::Type type, const std::string& name )
			:	m_Type			{ type },
				m_Name			{ name },
				m_bAtTrainTip	{ false },
				m_bAtTrainEnd	{ false }
		{}

		//void Moving( const Track& /*track*/, const common::Interval<Real>& /*range*/ ) const override
		bool Moving( const Location& /*_location*/, const common::Interval<Length>& /*range*/ ) const noexcept override
		{return true;}

		void Transitioning( const Track& /*fromTrack*/, Track::EndType /*fromEnd*/, const Track& /*toTrack*/, Track::EndType /*toEnd*/ ) const noexcept override
		{}

		std::string Name() const override{
			return m_Name;
		}

		void Name( const std::string& name ) override{
			m_Name = name;
		}

		Type GetType() const noexcept override{ 
			return m_Type; 
		}

		void SetType( Type type ) noexcept override{
			m_Type = type;
		}

		void TrainTip( bool bTrainTip ) noexcept override{
			m_bAtTrainTip = bTrainTip;
		}

		bool TrainTip() const noexcept override{
			return m_bAtTrainTip;
		}

		void TrainEnd( bool bTrainEnd ) noexcept override{
			m_bAtTrainEnd = bTrainEnd;
		}

		bool TrainEnd() const noexcept override{
			return m_bAtTrainEnd;
		}
	private:
		EventFilter::Type m_Type = EventFilter::Type::none;
		std::string m_Name;
		bool m_bAtTrainTip = false;
		bool m_bAtTrainEnd = false;
	};

}
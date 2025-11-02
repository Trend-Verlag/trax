//	trax track library
//	AD 2025 
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

#include "../TrainCreator.h"
#include "../RollingStockCreator.h"
#include "trax/rigid/trains/RollingStock.h"
#include "trax/rigid/trains/Train.h"

namespace trax{

TrainCreator::TrainCreator( Scene& scene ) noexcept
	: RollingStockCreator{ scene }
{
}

std::shared_ptr<Train> TrainCreator::GetTrain() const noexcept
{
	return m_pTrain;
}

bool TrainCreator::ParsingStart() noexcept(false)
{
	if( !m_TrainStack.empty() )
		return RollingStockCreator::ParsingStart();

	return TrainParser::ParsingStart();
}

bool TrainCreator::TrainStart(
	[[maybe_unused]] IDType id,
	[[maybe_unused]] const std::string& name,
	[[maybe_unused]] const std::string& reference,
	[[maybe_unused]] Velocity travel_velocity,
	[[maybe_unused]] Velocity velocity,
	[[maybe_unused]] One trainThrust,
	[[maybe_unused]] One trainBrake,
	[[maybe_unused]] int32_t credit ) noexcept(false)
{
	m_TrainStack.push( {} );
	return true;
}

RollingStockParser* TrainCreator::AxisArrangementStart( 
	[[maybe_unused]] const std::string& name,
	[[maybe_unused]] const std::string& reference ) noexcept(false)
{
	return this;
}

void TrainCreator::AxisArrangementEnd( 
	Orientation orientation ) noexcept(false)
{
	if( GetRollingStock() )
		m_TrainStack.top().push_back( std::make_pair( GetRollingStock(), orientation ) );
}

RollingStockParser* TrainCreator::RollingStockStart( 
	[[maybe_unused]] const std::string& name,
	[[maybe_unused]] const std::string& reference ) noexcept(false)
{
	return this;
}

void TrainCreator::RollingStockEnd( 
	Orientation orientation ) noexcept(false)
{
	if( GetRollingStock() )
		m_TrainStack.top().push_back( std::make_pair( GetRollingStock(), orientation ) );
}

void TrainCreator::TrainEnd(		
	Orientation orientation,
	[[maybe_unused]] bool automatic,
	[[maybe_unused]] bool couplingNorthActivated,
	[[maybe_unused]] bool couplingSouthActivated ) noexcept(false)
{
	if( !m_TrainStack.empty() )
	{
		if( !m_TrainStack.top().empty() )
		{
			if( m_TrainStack.top().size() == 1 && std::dynamic_pointer_cast<Train>(m_TrainStack.top().front().first) )
			// Don't make Trains with single Trains in it.
			{
				m_pTrain = std::dynamic_pointer_cast<Train>(m_TrainStack.top().front().first);

				m_TrainStack.pop();

				if( !m_TrainStack.empty() )
					m_TrainStack.top().push_back( std::make_pair( m_pTrain, orientation ) );

				return;
			}

			if( m_pTrain = Train::Make(); m_pTrain )
			{
				for( std::pair<std::shared_ptr<trax::TrainComponent>,trax::Orientation> pair : m_TrainStack.top() )
				{
					m_pTrain->Align( trax::RailRunner::EndType::south, *pair.first, pair.second );
					m_pTrain->Append( trax::RailRunner::EndType::south, pair.first, pair.second );
				}

				m_TrainStack.pop();

				if( !m_TrainStack.empty() )
					m_TrainStack.top().push_back( std::make_pair( m_pTrain, orientation ) );

				return;
			}
		}
		
		m_TrainStack.pop();
	}
}

void TrainCreator::ParsingEnd() noexcept
{
	if( !m_TrainStack.empty() )
		RollingStockCreator::ParsingEnd();

	TrainParser::ParsingEnd();
}

} // namespace trax

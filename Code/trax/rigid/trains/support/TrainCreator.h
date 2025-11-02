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

#pragma once

#include "RailRunnerParser.h"
#include "RollingStockCreator.h"

#include <stack>

namespace trax
{
	struct Scene;

	class TrainCreator :	public TrainParser, 
							protected RollingStockCreator
	{
	public:
		TrainCreator( Scene& scene ) noexcept;

		std::shared_ptr<Train> GetTrain() const noexcept;
	protected:
		// TrainParser:
		bool ParsingStart() noexcept(false) override;

		bool TrainStart(
			IDType id,
			const std::string& name,
			const std::string& reference,
			Velocity travel_velocity,
			Velocity velocity,
			One trainThrust,
			One trainBrake,
			int32_t credit ) noexcept(false) override;

		RollingStockParser* AxisArrangementStart( 
			const std::string& name,
			const std::string& reference ) noexcept(false) override;

		void AxisArrangementEnd( Orientation orientation ) noexcept(false) override;

		RollingStockParser* RollingStockStart( 
			const std::string& name,
			const std::string& reference ) noexcept(false) override;

		void RollingStockEnd( Orientation orientation ) noexcept(false) override;

		void TrainEnd(		
			trax::Orientation orientation,
			bool automatic,
			bool couplingNorthActivated,
			bool couplingSouthActivated ) noexcept(false) override;
		
		void ParsingEnd() noexcept override;
	private:
		std::stack<std::vector<std::pair<std::shared_ptr<struct TrainComponent>,struct Orientation>>> m_TrainStack;
		std::shared_ptr<Train> m_pTrain;
	};

}

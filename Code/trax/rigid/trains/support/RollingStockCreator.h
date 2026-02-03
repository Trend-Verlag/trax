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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "RailRunnerParser.h"
#include "trax/rigid/Material.h"

#include <stack>

namespace trax{

	struct Scene;

	class RollingStockCreator : public RollingStockParser{
	public:
		RollingStockCreator( Scene& scene );

		std::shared_ptr<RollingStock> GetRollingStock() const noexcept;
	protected:
		// RollingStockParser:
		bool ParsingStart() noexcept(false) override;

		bool RollingStockStart() noexcept(false) override;

		bool BogieStart( 
			IDType id,
			const std::string& name,
			const std::string& model ) noexcept(false) override;

		void BogieFrame( 
			const spat::Frame<dim::Length,dim::One>& frame ) noexcept override;

		bool GeomStart( 
			const std::string& name, 
			Geom::Filters filter, 
			GeomType type, 
			Mass mass ) noexcept(false) override;

		void Material( 
			const trax::Material& material ) noexcept override;

		void GeomEnd( 
			const std::string& name, 
			Geom::Filters filter, 
			GeomType type, 
			Mass mass ) noexcept(false) override;

		void Coupling(
			EndType end,
			Length bufferLength,
			Force maxForce,
		    IDType typeID,
			const spat::Frame<dim::Length,dim::One>& frame ) noexcept(false) override;

		void Swivel( 
			EndType slot,
			IDType childID,
			const spat::Frame<dim::Length,dim::One>& frame ) noexcept(false) override;

		void BogieEnd( 
			const std::string& sndGroup ) noexcept override;


		bool WheelFrameStart( 
			IDType id,
			const std::string& name,
			const std::string& model ) noexcept(false) override;

		bool TractionForceCharacteristicStart() noexcept(false) override;

		void SpeedStep( 
			Velocity velocity, 
			One value ) noexcept override;

		void TractionForceCharacteristicEnd() noexcept(false) override;

		void TrackJointLimits( 
			const trax::TrackJointLimits& trackJointLimits ) noexcept override;

		void Wheelset( 
			const trax::Wheelset& wheelset ) noexcept override;

		bool TrackLocation(			
			const TrackLocationRef& trackLocation ) noexcept(false) override;

		void Anchor(			
			const spat::Frame<dim::Length,dim::One>& anchor ) noexcept override;

		void WheelFrameEnd(
			const std::string& sndGroup ) noexcept override;


		void RollingStockEnd() noexcept(false) override;

		void ParsingEnd() noexcept override;
	private:
		Scene& m_Scene;
		std::shared_ptr<RollingStock> m_pRollingStock;

		std::vector<std::shared_ptr<Bogie>> m_AllBogies;
		std::stack<std::shared_ptr<Bogie>> m_ActiveBogie;
		std::stack<std::shared_ptr<struct WheelFrame>> m_ActiveWheelFrame;
		std::shared_ptr<Bogie> GetBogie( int idx ) const;

		std::stack<spat::Frame<dim::Length,dim::One>> m_ActiveFrames;
		std::vector<std::tuple<std::shared_ptr<Bogie>, EndType, IDType, spat::Frame<dim::Length,dim::One>>> m_Swivels;
		std::shared_ptr<struct TractionForceCharacteristic> m_pCharacteristic;
		trax::Material m_Material;

		std::unique_ptr<Geom> CreateGeom( GeomType type, const spat::Box<Length>& box ) const;
	};

} // namespace trax
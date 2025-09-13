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

#include "trax/Parser.h"

#include "trax/rigid/Geom.h"
#include "trax/rigid/trains/RailRunner.h"

#include <filesystem>
#include <string>

namespace trax{

	struct Bogie;
	struct Fleet;
	struct Material;
	struct RollingStock;
	struct TrackJointLimits;
	struct Train;
	struct WheelFrameLimits;
	struct Wheelset;


	class RollingStockParser : public virtual Parser{
	public:
		virtual bool RollingStockStart() noexcept(false) { return true; }


		virtual bool BogieStart( 
			IDType /*id*/,
			const std::string& /*name*/,
			const std::string& /*model*/ ) noexcept(false) { return true; }

		virtual void BogieFrame( 
			const spat::Frame<dim::Length,dim::One>& /*frame*/ ) noexcept{}

		virtual bool GeomStart( 
			const std::string& /*name*/, 
			Geom::Filters /*filter*/, 
			GeomType /*type*/, 
			Mass /*mass*/ ) noexcept(false) { return true; }

		virtual void Material( 
			const Material& /*material*/ ) noexcept {}

		virtual void GeomEnd( 
			const std::string& /*name*/, 
			Geom::Filters /*filter*/, 
			GeomType /*type*/, 
			Mass /*mass*/ ) noexcept(false) {}

		virtual void Coupling(
			RailRunner::EndType /*end*/,
			Length /*bufferLength*/,
			Force /*maxForce*/,
		    IDType /*typeID*/,
			const spat::Frame<dim::Length,dim::One>& /*frame*/ ) noexcept(false) {}

		virtual void Swivel( 
			RailRunner::EndType /*slot*/,
			IDType /*childID*/,
			const spat::Frame<dim::Length,dim::One>& /*frame*/ ) noexcept(false) {}

		virtual void BogieEnd( 
			const std::string& /*sndGroup*/ ) noexcept {}


		virtual bool WheelFrameStart( 
			IDType /*id*/,
			const std::string& /*name*/,
			const std::string& /*model*/ ) noexcept(false) { return true; }

		virtual bool TractionForceCharacteristicStart() noexcept(false) { return true; }

		virtual void SpeedStep( 
			Velocity /*velocity*/, 
			One /*value*/ ) noexcept {}

		virtual void TractionForceCharacteristicEnd() noexcept(false) {}

		virtual void TrackJointLimits( 
			const TrackJointLimits& /*trackJointLimits*/ ) noexcept {}

		virtual void Wheelset( 
			const Wheelset& /*wheelset*/ ) noexcept {}

		virtual bool TrackLocation(			
			const TrackLocationRef& /*trackLocation*/ ) noexcept(false) { return true; }

		virtual void Anchor(			
			const spat::Frame<dim::Length,dim::One>& /*anchor*/ ) noexcept {}

		virtual void WheelFrameEnd(
			const std::string& /*sndGroup*/ ) noexcept {}


		virtual void RollingStockEnd() noexcept(false) {}
	};


	class TrainParser : public virtual Parser{
	public:

		virtual bool TrainStart(
			IDType /*id*/,
			const std::string& /*name*/,
			const std::string& /*reference*/,
			Velocity /*travel_velocity*/,
			Velocity /*velocity*/,
			One /*trainThrust*/,
			One /*trainBrake*/,
			int32_t /*credit*/ ) noexcept(false) {
			return true;
		}

		virtual void TrackLocation(			
			const TrackLocationRef& /*trackLocation*/ ) noexcept(false) {}

		virtual RollingStockParser* AxisArrangementStart( 
			const std::string& /*name*/,
			const std::string& /*reference*/ ) noexcept(false){
			return nullptr;
		}

		virtual void AxisArrangementEnd( 
			Orientation /*orientation*/ ) noexcept(false){}

		virtual RollingStockParser* RollingStockStart( 
			const std::string& /*name*/,
			const std::string& /*reference*/ ) noexcept(false){
			return nullptr;
		}

		virtual void RollingStockEnd( 
			Orientation /*orientation*/ ) noexcept(false){}

		virtual void TrainEnd(		
			trax::Orientation /*orientation*/,
			bool /*automatic*/,
			bool /*couplingNorthActivated*/,
			bool /*couplingSouthActivated*/ ) noexcept(false) {}
	};


	class FleetParser : public virtual TrainParser{
	public:
		virtual bool FleetStart(
			IDType /*id*/ ) noexcept(false) {
			return true;
		}

		virtual void FleetEnd() noexcept(false) {}
	};


	void dclspc ParseRollingStock( std::basic_istream<char>& stream, RollingStockParser& callback ) noexcept(false);

	void dclspc ParseRollingStock( const std::filesystem::path& filePath, RollingStockParser& callback ) noexcept(false);
	 
	void dclspc ParseRollingStock( const Bogie& rollingStock, RollingStockParser& callback ) noexcept(false);

	void dclspc ParseTrain( const unsigned char* bufferStart, const unsigned char* bufferEnd, TrainParser& callback ) noexcept(false);

	void dclspc ParseTrain( std::basic_istream<char>& stream, TrainParser& callback ) noexcept(false);

	void dclspc ParseTrain( const std::filesystem::path& filePath, TrainParser& callback ) noexcept(false);

	void dclspc ParseTrain( const Train& train, TrainParser& callback ) noexcept(false);

	bool dclspc ParseFleet( std::basic_istream<char>& stream, FleetParser& callback ) noexcept(false);

	bool dclspc ParseFleet( const std::filesystem::path& filePath, FleetParser& callback ) noexcept(false);

	bool dclspc ParseFleet( const Fleet& fleet, FleetParser& callback ) noexcept(false);

}

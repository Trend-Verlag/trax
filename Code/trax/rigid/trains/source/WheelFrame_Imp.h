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

#include "../WheelFrame.h"
#include "trax/rigid/TrackJointLimits.h"
#include "Bogie_Imp.h"

#if defined(_MSC_VER)
#	pragma warning(disable: 4250) // virtual inheritance of methods via dominance
#endif

namespace trax{

	class WheelFrame_Imp : public WheelFrame,
						   public Bogie_Imp
	{
	public:
		WheelFrame_Imp( TrainScene& scene, std::shared_ptr<Gestalt> pGestalt );
		WheelFrame_Imp( const WheelFrame_Imp& ) = delete;
		WheelFrame_Imp( WheelFrame_Imp&& bogie ) = delete;
		~WheelFrame_Imp();

		WheelFrame_Imp& operator=( const WheelFrame_Imp& ) = delete;
		WheelFrame_Imp& operator=( WheelFrame_Imp&& ) = delete;


		// RailRunner:
		const char* TypeName() const noexcept override;

		bool IsValid() const noexcept override;

		void Rail( const Location& location, bool bMoveTo = true ) override;

		Location GetLocation() const noexcept override;

		void DeRail() noexcept override;

		bool IsRailed() const noexcept override;

		spat::Frame<Length,One> GetGlobalAnchor() const override;

		void ResetToGlobalAnchor( const spat::Frame<Length,One>& anchor ) override;

		void SetVelocity( Velocity velocity ) override;

		Velocity GetVelocity() const override;

		void TargetVelocity( Velocity tvelocity ) override;

		Velocity TargetVelocity() const noexcept override;

		Power MaximumPowerOutput() const noexcept override;

		Power CurrentPowerOutput() const noexcept override;

		Velocity MaximumVelocity() const noexcept override;

		void Thrust( One byfactor ) override;

		Force ThrustAbsolute() const noexcept override;

		Force MaxThrust() const noexcept override;

		Force MaxThrust( Velocity velocity ) const noexcept override;

		void Brake( One byfactor ) override;

		Force BrakeAbsolute() const noexcept override;

		Force MaxBrake() const noexcept override;

		
		// Bogie:


		// WheelFrame:

		void Limits( const TrackJointLimits& wfl ) override;

		const TrackJointLimits& Limits() const noexcept override;

		std::shared_ptr<const TractionForceCharacteristic> SetTractionForceCharacteristic( 
			std::shared_ptr<const TractionForceCharacteristic> pCharacteristic ) override;

		std::shared_ptr<const TractionForceCharacteristic> GetTractionForceCharacteristic() const override;

		void EnableDerailing( bool bEnable = true ) noexcept;

		bool IsDerailingEnabled() const noexcept;

		void Anchor( const spat::Frame<Length,One>& frame ) noexcept override;

		spat::Frame<Length,One> Anchor() const noexcept override;

		spat::Frame<Length,One> UnflippedAnchor() const noexcept override;


		int Attach( const Wheelset& ws ) override;

		void DetachAllWheelsets() override;

		int CntWheelsets() const noexcept override;

		const Wheelset& GetWheelset( int idx ) const override;


		// Bogie_Imp:
		void Update( Time dt = fixed_timestep ) noexcept override;


		Jack& JackOnRail() noexcept override;

		Jack& JackOnDerail() noexcept override;

		Jack& JackOnRollingStart() noexcept override;

		Jack& JackOnRollingStop() noexcept override;

		Jack& JackOnMotorActive() noexcept override;

		Jack& JackOnMotorInactive() noexcept override;

		Jack& JackOnBrakeActive() noexcept override;

		Jack& JackOnBrakeInactive() noexcept override;

		Jack& JackOnStartTilting() noexcept override;

		Jack& JackOnStopTilting() noexcept override;

		Jack& JackOnTrackTransition() noexcept override;


		// Inherited via Jack/PlugEnumerator
		int CountJacks() const noexcept override;
	//	int CountPlugs() const override;
	protected:
		const Jack& _GetJack( int idx ) const override;
	//	const Plug& _GetPlug( int idx ) const override;

	private:
		std::shared_ptr<class TrackJointFeederMotorModel>	m_pTrackJointFeederMotorModel;
		TrackJointLimits 									m_TrackJointLimits;
		bool												m_bEnableDerailing;

		void SetLimits( const TrackJointLimits& wfl );
		void CalculateWheelsetFramesFromAnchor() noexcept;
		void AlignWithAnchorTo( const spat::Frame<Length,One>& frame ) noexcept;

		// Distances from the anchor to the parent and child bogie swivel axes.
		// Will be signed positive if the axis lies in Anchor().T direction; 
		// 0_m if no parent/child element is present.
		Length AnchorToParentNorth() const noexcept override;
		Length AnchorToParentSouth() const noexcept override;
		Length AnchorToChildSouth() const noexcept override;

		spat::Vector<One> LocalHorizontalDirection() const noexcept override;
	};

}


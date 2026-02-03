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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "../RollingStock.h"

#include "trax/ObjectID.h"
#include "trax/source/Plug_Imp.h"
#include "Bogie_Imp.h"
#include "TrainComponent_Imp.h"

namespace trax{

	struct WheelFrame;

	typedef TrainComponent_Imp<ObjectID_Imp<RollingStock>> RollingStock_Base;

	class RollingStock_Imp : public RollingStock_Base
	{
	public:
		RollingStock_Imp() noexcept;
		~RollingStock_Imp();

		// RailRunner:
		const char* TypeName() const noexcept override;

		bool IsValid() const noexcept override;

		Location GetLocation() const noexcept override;
		
		void DeRail() noexcept override;

		bool IsRailed() const noexcept override;

		spat::Frame<Length,One> GetGlobalAnchor() const override;

		void ResetToGlobalAnchor( const spat::Frame<Length,One>& anchor ) override;

		void ResetToGlobalAnchor( const spat::Frame<Length, One>& anchorPose, DistanceType distance ) override;

		void SetVelocity( Velocity velocity ) override;

		Velocity GetVelocity() const override;

		void TargetVelocity( Velocity tvelocity ) override;

		Velocity TargetVelocity() const noexcept override;

		EndType TargetDirection() const noexcept override;

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

		Mass TotalMass() const noexcept override;

		void EnableSimulation( bool enable = true ) noexcept override;

		bool IsSimulationEnabled() const noexcept override;


		// TrainComponent:
		bool IsFinal() const noexcept override;

		void Rail( const Location& location, bool bMoveTo, DistanceType data, bool bFailOnReservationConflicts = false ) override;

		Length GetOverhang( EndType end, DistanceType distance = DistanceType::actual ) const noexcept override;

		Length GetLength( DistanceType distance = DistanceType::actual ) const noexcept override;

		std::pair<Bogie&,EndType> GetTipAt( EndType end ) override;

		std::pair<const Bogie&,EndType> GetTipAt( EndType end ) const override;


		// RollingStock:
		std::shared_ptr<RollingStock> ThisRollingStock() const noexcept override;

		bool Attach( Bogie& bogie ) noexcept override;

		int GetNumberOfBogies() const noexcept override;

		Bogie& GetBogie( int idx ) const override;

		int GetNumberOfTopmostParents() const noexcept override;

		Bogie& GetTopmostParent( int idx = 0 ) const override;

		int GetNumberOfWheelFrames() const noexcept override;

		WheelFrame& GetWheelFrame( int idx ) const override;

		std::pair<std::shared_ptr<RollingStock>,EndType> GetCoupledRollingStock( EndType end ) const override;


		// Jacks 'N Plugs:
		Jack& JackOnHoot() noexcept;

		Jack& JackOnSteam() noexcept;

		// Inherited via JackEnumerator
		int CountJacks() const noexcept override;

		// Inherited via PlugEnumerator
	//	int CountPlugs() const override;

	protected:
		const Jack& _GetJack( int idx ) const override;
	//	const Plug& _GetPlug( int idx ) const override;

		virtual void ConnectJacks() override;
	private:
		//ordered by track position (north to south):
		std::vector<std::shared_ptr<Bogie_Imp>>		m_Bogies; // all bogies including wheelframes
		std::vector<std::shared_ptr<Bogie>>			m_TopmostBogies;
		std::vector<std::shared_ptr<WheelFrame>>	m_WheelFrames;

		Bogie* m_pNorthTip;
		Bogie* m_pSouthTip;
		std::pair<Length,Length> m_Overhangs;
		void CalculateOverhangs() noexcept;
		const WheelFrame& GetNorthmostChildWheelFrame( const Bogie& bogie ) const noexcept;

		Jack_Imp m_JackOnHoot{ "JackOnHoot" };
		Jack_Imp m_JackOnSteam{ "JackOnSteam" };

		void Clear() noexcept override;
	};


	void FillWheelFrame( 
		SimulatorTrain& simulator, 
		WheelFrame& wheelFrame, 
		std::istringstream& istream, 
		spat::Box<Length> lengthWidthHeight, 
		Mass mass );

	std::shared_ptr<Bogie> FillBogie( 
		SimulatorTrain& simulator, 
		std::istringstream& istream, 
		spat::Box<Length> lengthWidthHeight, 
		Mass mass );
}
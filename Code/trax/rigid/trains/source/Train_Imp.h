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

#include "../Train.h"

#include "trax/ObjectID.h"
#include "trax/rigid/trains/source/TrainComponent_Imp.h"

#include <deque>

namespace trax{

	typedef TrainComponent_Imp<ObjectID_Imp<Train>> Train_Base;

	class Train_Imp : public Train_Base{
	public:
		Train_Imp();
		~Train_Imp();

		// RailRunner:
		const char* TypeName() const noexcept override;

		bool IsValid() const noexcept override;

		void Rail( const class Location& location, bool bMoveTo = true ) override;

		void Rail( const Location& location, bool bMoveTo, DistanceType data ) override;

		Location GetLocation() const noexcept override;
		
		void DeRail() noexcept override;

		bool IsRailed() const noexcept override;

		spat::Frame<Length,One> GetGlobalAnchor() const override;

		void ResetToGlobalAnchor( const spat::Frame<Length,One>& anchor ) override;

		void ResetToGlobalAnchor( const spat::Frame<Length,One>& anchorPose, DistanceType distance ) override;

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

		// TrainComponent:
		Length GetOverhang( EndType end, DistanceType distance = DistanceType::actual ) const noexcept override;

		Length GetLength( DistanceType type = DistanceType::actual ) const noexcept override;

		std::pair<Bogie&,EndType> GetTipAt( EndType end ) override;

		std::pair<const Bogie&,EndType> GetTipAt( EndType end ) const override;


		// Train:
		std::shared_ptr<Train> ThisTrain() const noexcept override;

		int GetNumberOfComponents() const noexcept override;

		std::shared_ptr<TrainComponent> GetComponent( int index ) const override;

		int GetIndexOf( const TrainComponent& component ) const noexcept override;

		bool Create( TrainComponent& trainComponent, Orientation orientation = Orientation::Value::para ) override;

		bool Create( const std::vector<std::pair<std::shared_ptr<TrainComponent>,Orientation>>& trainComponents, 
			Orientation orientation = Orientation::Value::para, 
			bool bMoveTo = true,
			DistanceType distance = DistanceType::actual ) override;

		void Align( EndType atEnd, TrainComponent& component, EndType withEnd ) const override;

		void Align( EndType atEnd, TrainComponent& component, Orientation orientation ) const override;

		void Append( EndType atEnd, std::shared_ptr<TrainComponent> pComponent, EndType withEnd, bool bCouple = true ) override;

		void Append( EndType atEnd, std::shared_ptr<TrainComponent> pComponent, Orientation orientation, bool bCouple = true ) override;
		
		void Take( EndType atEnd, Train& from, EndType withEnd ) override;

		std::pair<std::shared_ptr<struct Train>, std::shared_ptr<struct Train>> SplitAfter( int at ) override;

		std::shared_ptr<Train> Separate() override;
	
		void Reduce( bool bRecursive = true ) noexcept override;

		void Clear() noexcept override;

		bool Couple( EndType end, Train& with, EndType withEnd ) override;
				
		bool IsUnCoupledInternally() const noexcept override;
		

		Jack& JackOnUnCoupleInternal() noexcept override;

		// Inherited via JackEnumerator
		int CountJacks() const noexcept override;

	protected:
		const Jack& _GetJack( int idx ) const override;

		void DisconnectJacks() override;
		void ConnectJacks() override;
	private:
		std::deque<std::shared_ptr<TrainComponent>> m_Train;
 
		std::pair<std::shared_ptr<TrainComponent>,EndType> GetTipAt( const TrainComponent& trainComponent, EndType end ) const;

		void Recouple() noexcept;

		// Jack for internal uncouple events, triggers on decouple events and
		// sub_Train's OnUnCoupleInternal events.
		MultiPlugJack_Imp m_JackOnUnCoupleInternal{ "JackOnUnCoupleInternal" };
	};
	
}

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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "RailRunner.h"
#include "trax/Orientation.h"

namespace trax{

	struct Bogie;
	struct Train;

	/// \brief A TrainComponent is a part of a Train.
	struct TrainComponent : RailRunner
	{
		/// \brief Gets a shared pointer to this.
		virtual std::shared_ptr<TrainComponent> ThisTrainComponent() const noexcept = 0;


		/// \returns the train this component belongs to.
		virtual Train* GetTrain() const noexcept = 0;


		/// \returns the orientation of this component in the Train.
		virtual Orientation GetOrientation() const noexcept = 0;


		/// \returns The topmost parent Train in the TrainComponent 
		/// hierarchy and this components orientation relative to it.
		virtual std::pair<Train*,Orientation> GetTopmostTrain() const noexcept = 0;


		/// \returns true if this can not hold TrainComponents itself.
		virtual bool IsFinal() const noexcept = 0;


		/// \returns true if train is a direct or indirect parent train of 
		/// this component. 
		virtual bool IsParent( const Train& train ) const noexcept = 0;


		enum class DistanceType{
			actual,
			min,
			max,
			half,
		};

		using RailRunner::Rail;

		/// \brief Rails this TrainComponent at the given location.
		///
		/// If bMoveTo is true, the TrainComponent will be moved to the location,
		/// the internal distances of coupled components are resolved by 'distance'
		/// if this happens to be a Train.
		virtual void Rail( const Location& location, bool bMoveTo, DistanceType distance ) = 0;


		using RailRunner::ResetToGlobalAnchor;

		virtual void ResetToGlobalAnchor( const spat::Frame<Length,One>& anchorPose, DistanceType distance ) = 0;



		// Coupled:

		/// \brief Structure describing a coupling between two TrainComponents.
		struct Coupling{
			std::shared_ptr<TrainComponent> pTrainComponentA;	///< The TrainComponent A.
			EndType endA;										///< The end of A.
			std::shared_ptr<TrainComponent> pTrainComponentB;	///< The TrainComponent B.
			EndType endB;										///< The end of B.
		};


		/// \brief Couples this TrainComponent with another at the respective ends.
		///
		/// It doesn't matter, wether the two couplings are near each other or not. 
		/// The coupling will try to maintain the actual distance of the two 
		/// TrainComponent's ends.
		/// The coupling attempt will fail if the two TrainComponent are already coupled
		/// (with different partners or the same parters at different ends) or 
		/// if one of the couplings is deactivated. An coupling attempt will 
		/// also fail, when two couplings wear different coupling type indices. 
		/// \see Bogie::CouplingProps.
		/// \param end End of this TrainComponent.
		/// \param with Other TrainComponent.
		/// \param withEnd Other TrainComponent's end to couple with.
		/// \param btriggerPulses If true the respective JackOnCouple jacks are pulsing.
		/// \returns true if a new coupling was established or the two TrainComponent
		/// were already properly coupled. 
		virtual bool Couple( EndType end, TrainComponent& with, EndType withEnd, bool btriggerPulses = true ) noexcept = 0;


		/// \brief Get the coupled TrainComponent.
		/// 
		/// The coupled train component is the parent to the Bogie coupled at this
		/// TrainComponent's tip Bogie, that is topmost in its hierarchy of TrainComponents,
		/// but not also a parent to this TrainComponent.
		/// \param end End type of this TrainComponent.
		/// \returns A pointer to the other TrainComponent coupled at end, or  
		/// nullptr if none and the other TrainComponent's end type.	
		virtual std::pair<std::shared_ptr<TrainComponent>,EndType> GetCoupledTrainComponent( EndType end ) const noexcept = 0;


		/// \brief Gets the length of the coupling from TrainComponent edge to
		/// TrainComponent edge.
		/// 
		/// This is the length of the coupling, not the distance between the
		/// two TrainComponents. It is the sum of the two radii of the couplings
		/// trigger spheres; or 0_m if not coupled.
		/// \param end End of this TrainComponent to get the coupling length for.
		/// \param distance The type of the data of the coupling to get the length for.
		/// \returns The length of the coupling at the denoted end.
		virtual Length GetCouplingLength( EndType end, DistanceType distance = DistanceType::max ) const noexcept = 0;


		/// \name Train Tip
		/// \brief Gets a reference to the Bogie which makes the 
		/// outmost tip of this RailRunner.
		/// 
		/// For a RollingStock and a Train it would get the Bogie
		/// with the coupling that reaches out the furthest in the 
		/// denoted direction.
		///@{
		
		/// \param end Denotes the Railrunner's end to search for 
		/// the outmost Bogie.
		/// \throws std::invalid_argument if end is none.
		/// \returns The Bogie at the tip, and the end type of the 
		/// coupling at the tip.
		virtual std::pair<Bogie&,EndType> GetTipAt( EndType end ) = 0;

		/// \param end Denotes the Railrunner's end to search for 
		/// the outmost Bogie.
		/// \throws std::invalid_argument if end is none.
		/// \returns The Bogie at the tip, and the end type of the 
		/// coupling at the tip.
		virtual std::pair<const Bogie&,EndType> GetTipAt( EndType end ) const = 0;
		///@}


		// Geometry:

		/// \brief Gets the distances from the track location 
		/// (i.e. global anchor) to the north or south tip 
		/// of the RailRunner.
		/// 
		/// The tip of a RailRunner is at the outmost coupling position.
		/// \returns southern overhang.
		virtual Length GetOverhang( EndType end, DistanceType distance = DistanceType::actual ) const noexcept = 0;


		/// \brief Gets the total length of this RailRunner.
		///
		/// \returns the sum of the two overhangs.
		virtual Length GetLength( DistanceType distance = DistanceType::actual ) const noexcept = 0;


		// Jacks 'N Plugs:

		/// \brief Gets a Jack that pulses its Plug if the RailRunner
		/// gets railed on a track.
		virtual Jack& JackOnRail() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the RailRunner
		/// derails.
		virtual Jack& JackOnDerail() noexcept = 0;


	protected:
		friend class Train_Imp;
		// TrainComponent_Imp:
		virtual void SetTrain( Train* pTrain ) noexcept = 0;

		virtual void SetOrientation( Orientation orientation ) noexcept = 0;
	};


	/// \brief Couples two TrainComponents.
	dclspc bool Couple( const TrainComponent::Coupling& coupling ) noexcept;


	/// \returns the first common Train parent if the two TrainComponents have one.
	dclspc Train* CommonParent( const TrainComponent& a, const TrainComponent& b ) noexcept;


}
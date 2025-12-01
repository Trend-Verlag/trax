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

#include "Bogie.h"
#include "trax/Event.h"

namespace trax{

	struct TractionForceCharacteristic;
	struct Wheelset;
	struct TrackJointLimits;

	/// \brief A WheelFrame is a Bogie that hosts Wheelsets 
	/// and a TrackJoint and therefore can directly get 
	/// attached to a track. It also is responsible for managing
	/// a TractionForceCharacteristic and feeding the TrackJoint
	/// with data. 
	struct WheelFrame : virtual Bogie
	{

		/// \brief Makes a WheelFrame object.
		/// \param scene The scene to insert the WheelFrame into.
		/// \param pGestalt The Gestalt to use. If no Gestalt is given 
		/// one is automatically created.
		static dclspc std::shared_ptr<WheelFrame> Make( Scene& scene, std::shared_ptr<Gestalt> pGestalt = nullptr ) noexcept;



		/// \brief Sets all the limits for this wheelframe.
		virtual void Limits( const TrackJointLimits& wfl ) = 0;


		/// \returns A reference to the WheelFrames Limits.
		virtual const TrackJointLimits& Limits() const noexcept = 0;


		/// \brief applies a TractionForceCharacteristic to the thrust calculations.
		///
		/// The default is a characteristic wich will allways be 1.
		/// \param pCharacteristic Pointer to functor.
		/// \returns the old value or nullptr if the default was set.
		virtual std::shared_ptr<const TractionForceCharacteristic> SetTractionForceCharacteristic( std::shared_ptr<const TractionForceCharacteristic> pCharacteristic ) = 0;


		/// \returns A pointer to the actually applied characteristic
		virtual std::shared_ptr<const TractionForceCharacteristic> GetTractionForceCharacteristic() const = 0;


		/// \brief A WheelFrame might derail during the simulation,
		/// if the stress of forces gets big enough. This switches this 
		/// possibility on or off.
		virtual void EnableDerailing( bool bEnable = true ) noexcept = 0;


		/// \returns true if the derailing feature is enabled.
		virtual bool IsDerailingEnabled() const noexcept = 0;



		/// \name Anchor
		/// \brief Gets or sets the local anchor that gets aligned to the track by the track joint.
		///
		/// The frame in local coordinates relative to the WheelFrame's body frame, 
		/// which will get aligned to the track. If the WheelFrame becomes railed 
		/// in anti direction on a track, this anchor will get flipped.
		/// \see RailRunner::GetGlobalAnchor()
		///@{
		
		/// \param anchor Orthogonal frame, describing the anchor in body relative coordinates.
		virtual void Anchor( const spat::Frame<Length,One>& anchor ) noexcept = 0;

		/// \returns The local anchor actually in use.
		virtual spat::Frame<Length,One> Anchor() const noexcept = 0;

		/// \returns The unflipped local anchor.
		/// \throws std::runtime_error if the wheelframe is not railed.
		virtual spat::Frame<Length,One> UnflippedAnchor() const noexcept = 0;
		///@}


		//Wheelsets:

		using Bogie::Attach;

		/// \brief Attaches a Wheelset to this WheelFrame.
		/// \returns The index of the attached Wheelset.
		/// \throws std::logic_error If the WheelFrame is railed
		/// \throws std::invalid_argument for inavild Wheelset parameters.
		virtual int Attach( const Wheelset& ws ) = 0;


		/// \brief Removes all the Wheelsets.
		virtual void DetachAllWheelsets() = 0;


		/// \brief Gets the number of Wheelsets attached directly to this WheelFrame.
		virtual int CntWheelsets() const noexcept = 0;


		/// \brief Gets the Wheelset denoted by idx.
		/// \param idx Index of the Wheelset 0 <= idx < CntWheelsets()
		virtual const Wheelset& GetWheelset( int idx ) const = 0;


		struct Event : trax::Event{

			virtual WheelFrame& GetWheelFrame() const noexcept = 0;
		};

		// Jacks 'N Plugs:

		/// \brief Gets a Jack that pulses its Plug if the WheelFrame
		/// gets railed on a track.
		virtual Jack& JackOnRail() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the WheelFrame
		/// derails.
		virtual Jack& JackOnDerail() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the WheelFrame
		/// starts to roll.
		virtual Jack& JackOnRollingStart() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the WheelFrame
		/// stops to roll.
		virtual Jack& JackOnRollingStop() noexcept = 0;

		
		/// \brief Gets a Jack that pulses its Plug if the WheelFrame's 
		/// motor gets activated in order to reach the target velocity.
		virtual Jack& JackOnMotorActive() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the WheelFrame's 
		/// motor gets deactivated.
		virtual Jack& JackOnMotorInactive() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the WheelFrame's 
		/// brake gets activated in order to reach the target velocity.
		virtual Jack& JackOnBrakeActive() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the WheelFrame's 
		/// brake gets deactivated.
		virtual Jack& JackOnBrakeInactive() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the WheelFrame
		/// passes the connection between two tracks.
		virtual Jack& JackOnTrackTransition() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the WheelFrame
		/// starts tilting.
		virtual Jack& JackOnStartTilting() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the WheelFrame
		/// starts tilting.
		virtual Jack& JackOnStopTilting() noexcept = 0;
	};

	// Were is this used?
	/// \brief Reverts all WheelFrames that has no wheels to Bogies.
	//std::shared_ptr<Bogie> RemoveUnusedWheelFrames( std::shared_ptr<Bogie> pBogie );

}


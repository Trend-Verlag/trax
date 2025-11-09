//	trax track library
//	AD 2014 
//
//  "Voelker hoert die Signale,"
//	 auf zum letzten Gefecht!"
//
//				Emil Luckhardt
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

/// \page docu_signal Signals
/// 
/// \section signal_intro Introduction
/// 
/// 

#include "Identified.h"
#include "Track.h"
#include "Location.h"

namespace trax{

	struct Jack;
	struct MultiPlug;
	struct SocketRegistry;


	/// \brief A Signal is positioned along a track and is submitted to a Train
	/// if it runs over it.
	///
	/// All messages that count potentially for all trains should be implemented as
	/// Signals.
	struct Signal : Identified<Signal>
	{
		/// \returns the name for the object type that implements this interface. 
		virtual const char*	TypeName() const noexcept = 0;


		/// \brief Various Signal status settings.
		enum class Status{
			none		= -1,
			stop		= 0,
			clear		= 1,
			slow,
			velocity0 = stop,
			velocity1 = clear,
			velocity2 = slow,
			velocity3,
			velocity4,
			velocity5,
			velocity6,
			velocity7,
			velocity8,
			velocity9,
			velocity10,
			velocity11,
			velocity12,
			velocityMax = velocity12,
			caution,
			doors_open,
			doors_close,
			uncouple,
			lights_on,
			jump,
			count,
			toggle,
			change
		};


		/// \brief Set the Signal's setting.
		/// \param status Status to set the signal to.
		/// \param pulse True if the action shall pulse a plug.
		/// \returns The previous setting.
		virtual Status Set( Status status, bool pulse = true ) = 0;


		/// \brief Sets the signal state to the next setting.
		/// \param pulse True if the action shall pulse a plug.
		virtual void Toggle( bool pulse = true ) = 0;


		/// \brief Get the Signal's setting.
		virtual Status Get() const = 0;


		/// \brief Gets the number of signal status the signal provides.
		virtual int CountStatus() const = 0;


		/// \brief Attaches the Signal to a TrackBuilder.
		virtual void Attach( TrackBuilder* pTrackBuilder ) noexcept = 0;


		/// \returns Pointer to the track this Signal is attached to, or nullptr,
		///	if not attached.
		virtual TrackBuilder* TrackAttached() const = 0;


		/// \brief Gets the range on the track on wich the signal
		/// is effective.
		virtual void GetTrackRange( TrackRange& trackRange ) const = 0;


		/// \brief Creates a Location for the signals reference point on the tracks.
		/// 
		/// This would be the Far() value of the range it is registered with.
		/// \returns A location on the track.
		virtual Location GetLocation() const = 0;


		/// \returns A Plug to trigger the toggle method.
		virtual MultiPlug& PlugToToggle() = 0;


		/// \returns A Jack that pulses its Plug if the connector setting is changed.
		virtual Jack& JackOnChange() = 0;


		/// \brief Gets a Jack that pulses its Plug if the specified Status is set. 
		virtual Jack& JackOn( Status status ) = 0;


		/// \brief Gets a Jack that pulses its Plug if the specified Status is set and was not set before.
		virtual Jack& JackOnChangeTo( Status status ) = 0;


		/// \brief Gets a Plug that switches to the specified Status. 
		virtual MultiPlug& PlugTo( Status status ) = 0;


		/// \brief Registers all the Plugs with the module and supplies the
		/// Jacks with plug ids for connecting.
		virtual void RegisterSockets( SocketRegistry& modul ) = 0;


		/// \brief Removes all references to Plugs and Jacks from the module.
		virtual void UnregisterSockets( SocketRegistry& modul ) = 0;
	};

	dclspc constexpr bool IsVelocityStatus( Signal::Status status );


	/// \brief Interface for signals that control the velocity of trains.
	///
	/// A VelocityControl can have several control states; each velocity control maintains a 
	/// minimum and a maximum velocity, wich a train might feel compelled to maintain
	/// if in the signal area.
	struct VelocityControl : virtual Signal
	{
		/// \brief Makes a VelocityControl object.
		static dclspc std::unique_ptr<VelocityControl> Make() noexcept;


		/// \brief Set if the signal should affect the velocity permanently.
		///
		/// A train maintains a separate travel velocity and a target velocity.
		/// If the train enters a signal's area its target velocity will get
		/// influenced, but as soon as it leaves the area the target velocity 
		/// will get set back to travel velocity.
		/// The default setting is false.
		/// \param bTravel true if the travel velocity should be affected, false if not.
		/// \param status Velocity status of signal control.
		/// \returns The previous setting.
		/// \throws std::invalid_argument if status is not a velocity signal status.
		/// \throws std::out_of_range if status is not a control state, prefiously set.
		virtual bool AffectTravelVelocity( Status status, bool bTravel ) = 0;


		/// \returns true if the signal control state affects the travel velocity.
		/// \throws std::invalid_argument if status is not a velocity signal status.
		/// \throws std::out_of_range if status is not a control state, prefiously set.
		virtual bool AffectTravelVelocity( Status status ) const = 0;


		/// \brief Set the target velocities.
		///
		/// Set the velocities for one control state. It creates the control 
		/// state if not created yet.
		/// \param status Velocity status of signal control.
		/// \param velocityMin Lower limit of the velocity.
		/// \param velocityMax Upper limit of the velocity.
		/// \throws std::invalid_argument if status is not a velocity signal status.
		/// \throws std::invalid_argument if velocityMin > velocityMax.
		virtual void SetVelocity( Status status, Velocity velocityMin, Velocity velocityMax ) = 0;


		/// \brief Set the target velocities.
		///
		/// Set the velocities for one control state to the same value both.
		/// It creates the control state if not created yet.
		/// This will yield a control that restricts the velocity of
		/// a train to a specific value; e.g. 0 for a stop sign.
		/// \param velocity Velocity for the train to maintain.
		/// \param status Velocity status of signal control.
		/// \throws std::invalid_argument if status is not a velocity signal status.
		virtual void SetVelocity( Status status, Velocity velocity ) = 0;


		/// \brief Sets the minimum velocity.
		///
		/// The maximum velocity will be set to an infinite value.
		/// It creates the control state if not created yet.
		/// \param velocityMin Lower limit of the velocity.
		/// \param status Velocity status of signal control.
		/// \throws std::invalid_argument if status is not a velocity signal status.
		virtual void VelocityMin( Status status, Velocity velocityMin ) = 0;


		/// \brief Sets the maximum velocity.
		///
		/// The minimum velocity will be set to zero. 
		/// It creates the control state if not created yet.
		/// \param velocityMax Upper limit of the velocity.
		/// \param status Velocity status of signal control.
		/// \throws std::invalid_argument if status is not a velocity signal status.
		virtual void VelocityMax( Status status, Velocity velocityMax) = 0;


		/// \brief Get the minimum target velocity.
		/// \param status Velocity status of signal control.
		/// \returns The minimum velocity setting for status.
		/// \throws std::invalid_argument if status is not a velocity signal status.
		/// \throws std::out_of_range if status is not a control state, previously created.
		virtual Velocity VelocityMin( Status status ) const = 0;


		/// \brief Get the maximum target velocity.
		/// \param status Velocity status of signal control.
		/// \returns The maximum velocity setting for status.
		/// \throws std::invalid_argument if status is not a velocity signal status.
		/// \throws std::out_of_range if status is not a control state, prefiously set.
		virtual Velocity VelocityMax( Status status ) const = 0;


		/// \returns true if the signal state for status was defined before.
		virtual bool IsValidState( Status status ) const = 0;


		/// \brief Removes thje signal state with index idx.
		/// \param status Velocity status of signal control.
		/// \throws std::invalid_argument if status is not a velocity signal status.
		virtual void Remove( Status status ) = 0;


		/// \brief Removes all signal states.
		virtual void Clear() = 0;


		/// \brief Sets a distance from the far edge to stop at.
		/// \param distance Distance to stop earlyer than the far edge.
		virtual void StopDistance( Length distance ) = 0;


		/// \returns The distance from the far edge to stop at.
		virtual Length StopDistance() const = 0;


		/// Gets a Jack that pulses its Plug if the signal is 
		/// set to its idx'es speed control state.
		/// \param status Velocity status of signal control.
		/// \throws std::invalid_argument if status is not a velocity signal status.
		/// \throws std::out_of_range if status is not a control state, prefiously set.
		virtual Jack& JackOnVelocity( Status status ) = 0;


		/// Gets a Plug that sets the signal to its idx'es speed 
		/// control state.
		/// \param status Velocity status of signal control.
		/// \throws std::invalid_argument if status is not a velocity signal status.
		/// \throws std::out_of_range if status is not a control state, prefiously set.
		virtual MultiPlug& PlugToVelocity( Status status ) = 0;
	};

	struct Depot;

	/// \brief A Signal that triggers a Train to jump to a jump site.
	///
	/// A 'train jump' will be performed as soon as the train's first
	/// WheelFrame will pass the JumpSite's range starting point and
	/// will jump to the attached JumpSite's range end point plus that
	/// piece of arc length that it overshot the starting point in the
	/// simulation step. The train will keep its velocities and will 
	/// move in the target JumpSite's direction.
	/// A JumpSite with range 0 and itself as a jump target will not
	/// alter the train movement, since it performes a jump to the 
	/// location of the next simulation step.
	struct JumpSite : virtual Signal{

		/// \brief Makes a JumpSite object.
		static dclspc std::unique_ptr<JumpSite> Make() noexcept;

		/// \brief Sets the target to a location.
		virtual void Target( const Location& target ) = 0;


		/// \brief Sets the target to a depot.
		virtual void Target( std::shared_ptr<Depot> pDepot ) = 0;


		/// \returns true if the target is a track location, false if
		/// it is a depot or the jump signal has no target
		virtual bool IsTargetLocation() const = 0;


		/// \returns The target location. 
		virtual const Location& TargetLocation() const = 0;


		/// \returns A pointer to the target depot or nullptr if the
		/// target is a location or tzhere is no target.
		virtual std::shared_ptr<Depot> TargetDepot() const = 0;


		/// \brief Gets a Jack that pulses its Plug if the signal is set to jump.
		virtual Jack& JackOnJump() = 0;


		/// \brief Gets a Jack that pulses its Plug if the signal is set to jump and was not set so before.
		virtual Jack& JackOnChangeToJump() = 0;


		/// \brief Gets a Plug that sets the signal to jump.
		virtual MultiPlug& PlugToJump() = 0;


		/// \brief Gets a Jack that pulses its Plug if the signal is set to clear. 
		virtual Jack& JackOnClear() = 0;


		/// \brief Gets a Jack that pulses its Plug if the signal is set to clear and was not set so before.
		virtual Jack& JackOnChangeToClear() = 0;


		/// \brief Gets a Plug that sets the signal to clear.
		virtual MultiPlug& PlugToClear() = 0;
	};


	/// \brief A Signal that can be placed at track couplings to signal 
	/// that a Train runs over the coupling between two tracks.
	struct TrackEndTransition : virtual Signal{

		/// \brief Makes a standard TrackEndTransition object.
		static dclspc std::unique_ptr<TrackEndTransition> Make( Track* pToTrack, Track::EndType toEnd ) noexcept;

		/// \param end Reference to Track::EndType to receive the tracks end.
		/// \returns The origin track at a track transition.
		virtual Track* From( Track::EndType& end ) const = 0;


		/// \returns The origin track at a track transition.
		virtual Track* From() const = 0;


		/// \param end Reference to Track::EndType to receive the tracks end.
		/// \returns The target track at a track transition.
		virtual Track* To( Track::EndType& end ) const = 0;


		/// \returns The target track at a track transition.
		virtual Track* To() const = 0;
	};


	/// \brief Makes a string from the status
	dclspc std::string ToString( Signal::Status status );


	/// \brief Makes a status value from a status string.
	/// \throws std::invalid_argument if the string was not recognized.
	dclspc Signal::Status ToSignalStatus( const std::string& status );


	dclspc Signal::Status SignalStatusFrom( const std::string& socketName );



	/// \brief Target for signal notification.
	struct SignalTarget{

		/// \brief Method called to notify about a signalling state.
		/// \param signal Reference to signal that notifies.
		/// \param distance The distance from the location the SignalTarget
		/// is moved with up to the head of the signal range.
		/// \returns true if the movement along the track should get calculated, 
		/// false if this is supposed to stop, because the train was
		/// relocated by the Notify function.
		virtual bool Notify( const Signal& signal, Length distance ) = 0;


		/// \brief Switches the target on or off to receive notifications
		/// from signals.
		/// \param bActive true to receive, the default.
		/// \returns The previous activity state.
		virtual bool Active( bool bActive ) noexcept = 0;


		/// \returns Activity state of the target.
		virtual bool Active() const noexcept = 0;


		virtual ~SignalTarget() = default;
		SignalTarget( const SignalTarget& ) = delete;
		SignalTarget( SignalTarget&& ) = delete;
		SignalTarget& operator=( const SignalTarget& ) = delete;
		SignalTarget& operator=( SignalTarget&& ) = delete;
	protected:
		SignalTarget() = default;
	};

	/// Alignes the object to a signal at a track. The Objects
	/// Frame will be relative to the signals position.
	struct SignalAligned{

		virtual void Attach( std::shared_ptr<Signal> pSignal, Length parameterOffset = 0_m ) = 0;

		virtual IDType GetSignalRef() const = 0;

		virtual Length ParameterOffset() const = 0;

		virtual void ParameterOffset( Length parameterOffset ) = 0;

		virtual bool PreserveUpDirection() const = 0;

		virtual void PreserveUpDirection( bool bPreserveUpDirection ) = 0;

		virtual ~SignalAligned() = default;
		SignalAligned( const SignalAligned& ) = delete;
		SignalAligned( SignalAligned&& ) = delete;
		SignalAligned& operator=( const SignalAligned& ) = delete;
		SignalAligned& operator=( SignalAligned&& ) = delete;
	protected:
		SignalAligned() = default;
	};

}
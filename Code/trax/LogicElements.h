//	trax track library
//	AD 2014 
//
//  "You taint the fox and now you let him run"
//
//							Tenfold Loadstar
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

#include "Configuration.h"
#include "Identified.h"

#include <memory>

namespace trax{

	struct Jack;
	struct MultiPlug;
	struct SocketRegistry;

	/// \brief Interface for locking and unlocking an object.
	///
	/// Signals and Switches provide this type of interface. Together with
	/// a PulseCounter a lock counting mechanism can be built and from that
	/// mutual exclusive Fahrstrassen are possible.
	struct Lockable{

		/// \brief Locks the object for further input.
		virtual void Lock() = 0;


		/// \brief Unlocks the object to execute input again.
		virtual void Unlock() = 0;


		/// \brief Toggles the lock state.
		virtual void ToggleLock() = 0;


		/// \returns true if the object is locked, false elsewise
		virtual bool IsLocked() const = 0;


		/// \returns Plug to lock the object by pulse.
		virtual MultiPlug& PlugToLock() = 0;


		/// \returns Jack that pulses on lock.
		virtual Jack& JackOnLock() = 0;


		/// \returns Plug to unlock the object by pulse.
		virtual MultiPlug& PlugToUnlock() = 0;


		/// \brief Jack that pulses on unlock.
		virtual Jack& JackOnUnlock() = 0;


		/// \brief Plug to toggle the lock state by pulse.
		virtual MultiPlug& PlugToLockToggle() = 0;


		/// \brief Jack that pulses if the lock state changes.
		virtual Jack& JackOnLockChange() = 0;


		virtual ~Lockable() = default;
	};


	/// \brief A counter can be incremented or decremented by pulses. It will itself
	/// issue a pulse every time a certain threshold value is reached.
	///
	/// The default threshold and counter values are 0. 
	struct PulseCounter : Identified<PulseCounter>
	{
		/// \brief Makes a PulseCounter object.
		static dclspc std::unique_ptr<PulseCounter> Make() noexcept;


		/// \returns the name for the object type that implements this interface. 
		virtual const char*	TypeName() const noexcept = 0;


		/// \brief Increments the counter by 1.
		virtual void CountUp() noexcept = 0;


		/// \brief Decrements the counter by 1.
		virtual void CountDown() noexcept = 0;


		/// \brief Sets the threshold to a specific value. 
		/// \param threshold Number of pulses to count up (or down to). 
		/// Negative values are allowed.
		virtual void Threshold( int threshold ) = 0;


		/// \brief Gets the threshold value, set with this.
		virtual int Threshold() const noexcept = 0;


		/// \brief Sets the counter to a specific value. 
		/// \param counter Preset of the counter. 
		/// Negative values are allowed. Default value is 0.
		virtual void Counter( int counter ) = 0;


		/// \brief Gets the value of the counter
		virtual int Counter() const noexcept = 0;


		/// \brief Resets the counter to 0.
		virtual void Reset() noexcept = 0;


		/// \brief Plug whose pulses increment the counter by 1.
		virtual MultiPlug& PlugToCountUp() = 0;


		/// \brief Plug whose pulses decrement the counter by 1.
		virtual MultiPlug& PlugToCountDown() = 0;


		/// \brief Plug whose pulses reset the counter to 0.
		virtual MultiPlug& PlugToReset() = 0;


		/// \brief If the counter reaches the threshold from either side 
		/// the counter triggers a pulse in this Jack.
		virtual Jack& JackOnReachThreshold() = 0;


		/// \brief If the counter leaves the threshold to either side 
		/// the counter triggers a pulse in this Jack.
		virtual Jack& JackOnLeaveThreshold() = 0;


		/// \brief If the counter gets counted up it triggers a pulse in this jack.
		virtual Jack& JackOnCountUp() = 0;


		/// \brief If the counter gets counted down it triggers a pulse in this jack.
		virtual Jack& JackOnCountDown() = 0;


		/// \brief Registers all the Plugs with the module and supplies the
		/// Jacks with plug ids for connecting.
		virtual void RegisterSockets( SocketRegistry& registry ) = 0;


		/// \brief Removes all references to Plugs and Jacks from the module.
		virtual void UnregisterSockets( SocketRegistry& registry ) = 0;
	};

}

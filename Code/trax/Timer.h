// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2016 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel


#pragma once

#include "Configuration.h"
#include "Units.h"
#include "Identified.h"

namespace trax{

	struct Jack;
	struct MultiPlug;
	struct SocketRegistry;

	/// \brief A timer to meassure time lapses.
	///
	/// The timer will pulse its Jack in that simulation step 
	/// in which the time lapse is reached. This limits the accuracy 
	/// of the timing event to +- fixed_timestep.
	struct Timer : Identified<Timer>
	{
		/// \brief Makes a Timer object.
		static dclspc std::unique_ptr<Timer> Make() noexcept;


		/// \returns the name for the object type that implements this interface. 
		virtual const char*	TypeName() const = 0;


		/// \brief Starts the timer.
		///
		/// The timer will tick after the time lapse is over.
		virtual void Start() = 0;


		/// \returns true if the timer is running.
		virtual bool IsStarted() const = 0;


		/// \brief Resets the timer. 
		///
		/// Sets the time value to 0.
		/// It will not tick until it gets started again.
		virtual void Reset() = 0;


		/// \brief Pauses the timer.
		///
		/// Does not change the time value.
		/// It will not tick until it gets started again.
		virtual void Pause() = 0;


		/// \brief Sets the time span for the timer.
		///
		/// The default time span is fixed_timestep.
		/// \param timeLapse time span.
		/// \throw std::invalid_argument if timeLapse < fixed_timestep.
		virtual void Lapse( Time timeLapse ) = 0;


		/// \returns the time span for the timer.
		virtual Time Lapse() const = 0;


		/// \brief Sets wether the timer tick is happening
		/// only once or periodically.
		///
		/// The default is to pulse only once.
		virtual void Periodic( bool bPeriodic ) = 0;


		/// \returns true if the timer is set to be periodic.
		virtual bool Periodic() const = 0;


		/// \brief Sets the currect time of the timer.
		/// \param time Time in milliseconds.
		virtual void SetTime( Time time ) = 0;


		/// \returns the current time of the timer.
		virtual Time GetTime() const = 0;


		/// \brief Plug whose pulses starts the timer.
		virtual MultiPlug& PlugToStart() noexcept = 0;


		/// \brief Plug whose pulses resets the timer.
		virtual MultiPlug& PlugToReset() noexcept = 0;


		/// \brief Plug whose pulses pauses the timer.
		virtual MultiPlug& PlugToPause() noexcept = 0;


		/// \brief If the timer is started, this 
		/// Jack is triggered.
		virtual Jack& JackOnStart() noexcept = 0;


		/// \brief If the timer is reset, this 
		/// Jack is triggered.
		virtual Jack& JackOnReset() noexcept = 0;


		/// \brief If the timer is set to paused, this 
		/// Jack is triggered.
		virtual Jack& JackOnPause() noexcept = 0;


		/// \brief If the timer decides that time is up
		/// it triggers a pulse in this Jack.
		virtual Jack& JackOnTick() noexcept = 0;


		/// \brief Registers all the Plugs with the registry and supplies the
		/// Jacks with plug ids for connecting.
		virtual void RegisterSockets( SocketRegistry& registry ) = 0;


		/// \brief Removes all references to Plugs and Jacks from the registry.
		virtual void UnregisterSockets( SocketRegistry& registry ) = 0;


		virtual void Update() = 0;
	};


	//struct TimerCollection : Collection<TimerCollection,Timer>
	//{
	//	virtual void Update() = 0;
	//};

}
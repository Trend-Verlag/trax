//	trax track library
//	AD 2014 
//
//  "It aint me"
//
//		Creedence Clearwater Revival
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

/// \page docu_sensor Sensors
/// 
/// \section sensor_intro Introduction
/// A \link trax::Sensor Sensor \endlink can get attached to a Track at 
/// a certain position and with an orientation and will trigger when a 
/// \link trax::Location Location \endlink passes by, fulfilling certain 
/// filter conditions. It's up to the Sensor, to determine whether to 
/// trigger a pulse or not. When triggered, the Sensor will pulse its
/// \link trax::Sensor::JackOnTrigger JackOnTrigger() \endlink.
/// 
/// \image html Sensor.png
///
/// \section sensor_details Details
/// Location, relaying an Event to trigger the Sensor: 
/// 
/// \image html SensorTrigger.png
/// 
/// While a Location is moved along the tracks, thereby for example 
/// representing a train tip, it is fed by an Event, carrying the
/// necesssary information for a specialized Sensor to figure out
/// whether to trigger. If the Sensor triggers, that means it creates 
/// a pulse in an associated Jack.
/// 

#include "Configuration.h"
#include "Units.h"
#include "Identified.h"

#include "common/Interval.h"

namespace trax{

	struct Event;
	struct Jack;
	struct Plug;
	struct SocketRegistry;
	struct TrackBuilder;

	class Location;

	/// \brief A Sensor can get attached to a track and be triggerd when a Location runs over it.
	struct Sensor : Identified<Sensor>
	{

		/// \returns the name for the object type that implements this interface. 
		virtual const char*	TypeName() const noexcept = 0;


		/// \brief Triggers this Sensor.
		///
		/// Triggering means to pulse a registered Socket if a
		/// match between the Event and the sensor's fiter settings is found.
		/// \param _event An Event object that is used to filter 
		/// wether the sensor should actually trigger.
		virtual void Trigger( const Event& _event ) = 0;


		/// \returns Pointer to the track this Sensor is attached to, or nullptr,
		///	if not attached.
		virtual TrackBuilder* TrackAttached() const = 0;


		/// \brief Attach the pointer to the track to the sensor.
		///
		/// This message is called during attachement and detachment of a sensor. 
		/// Never call it directly; implement it for user sensors.
		virtual void Attach( TrackBuilder* pTrackBuilder ) = 0;


		/// \brief Gets a Jack that pulses its Plug if the sensor is triggering. 
		virtual Jack& JackOnTrigger() = 0;


		/// \brief Registers all the Plugs with the module and supplies the
		/// Jacks with plug ids for connecting.
		virtual void RegisterSockets( SocketRegistry& modul ) = 0;


		/// \brief Removes all references to Plugs and Jacks from the module.
		virtual void UnregisterSockets( SocketRegistry& modul ) = 0;
	};

	/// \brief A sensor can get attached to a track and will trigger
	/// by a call to Track::Trigger(). Rolling stocks and trains will
	/// trigger the sensors if so configured.
	///
	/// A Sensor is responsible to filter out certain events and decide
	/// wether to actually trigger on a call to Sensor::Trigger(). This then
	/// will result in a Pulse() for the registered Sockets.
	struct SensorFilterJack : Sensor
	{
		/// \brief Flags for filtering the trigger events.
		enum Effect{
			none			= 0x0000,	///< triggers never
			train_tip		= 0x0001,	///< triggers if the train tip with respect to moving direction is encountered
			train_end		= 0x0002,	///< triggers if the train end with respect to moving direction is encountered
			train_north		= 0x0004,	///< triggers if the north end of the train is encountered
			train_south		= 0x0008,	///< triggers if the south end of the train is encountered
			wheelframe		= 0x0010,	///< triggers on every wheelframe
			name			= 0x0020,	///< triggers if the name of the sensor is a substring of the wheelset's name path (Train_Name/RollingStock_Name/Bogie_Name/SubBogieName/ ... /WheelframeName).
			not_name		= 0x0040,	///< triggers if the name of the sensor is not a substring of the wheelset's name path.
			any				= 0xffff	///< triggers always
		};

		/// \brief Makes a SensorFilterJack object.
		static dclspc std::unique_ptr<SensorFilterJack> Make( int flags = SensorFilterJack::any ) noexcept;


		virtual int GetEffect() const = 0;


		virtual void SetEffect( int effect ) = 0;
	};


	//Todo: general interval - sensor for the next two

	/// \brief A sensor that detects, wether a wheelframe passing by 
	/// does this within certain velocity limits.
	struct VelocitySensor : virtual SensorFilterJack{

		/// \brief Makes a VelocitySensor object.
		static dclspc std::unique_ptr<VelocitySensor> Make( int flags = SensorFilterJack::any ) noexcept;

		/// \name Velocity
		/// \brief Sets and Gets the velocity limits for the sensor.
		/// \param velocityMin lower limit for the velocity interval.
		/// \param velocityMax upper limit for the velocity interval.
		///@{
		virtual void SetVelocity( Velocity velocityMin, Velocity velocityMax ) = 0;

		virtual void SetVelocity( const common::Interval<Velocity>& velocity ) = 0;

		virtual const common::Interval<Velocity>& GetVelocity() const = 0;

		virtual void VelocityMin( Velocity velocityMin ) = 0;

		virtual Velocity VelocityMin() const = 0;

		virtual void VelocityMax( Velocity velocityMax ) = 0;

		virtual Velocity VelocityMax( ) const = 0;
		///@}

		/// \name Trigger Condition
		/// \brief Sets the sensor to wether it riggers if the wheelframe's velocity
		/// is inside the interval (true, the default) or outside (false).
		///@{
		virtual void TriggerInside( bool bTriggerInside ) = 0;

		virtual bool TriggerInside() const = 0;
		///@}
	};

	/// \brief A sensor that detects wether the weight of a passing railrunner
	/// falls into certain limits.
	struct WeighSensor : virtual SensorFilterJack{

		/// \brief Makes a WeighSensor object.
		static dclspc std::unique_ptr<WeighSensor> Make( int flags = SensorFilterJack::any ) noexcept;

		/// \name Weight
		/// \brief Sets and Gets the weight limits for the sensor.
		/// \param weightMin lower limit for the weight interval.
		/// \param weightMax upper limit for the weight interval.
		///@{
		virtual void Weight( Mass weightMin, Mass weightMax ) = 0;

		virtual void Weight( const common::Interval<Mass>& weight ) = 0;

		virtual const common::Interval<Mass>& Weight() const = 0;

		virtual void WeightMin( Mass weightMin ) = 0;

		virtual Mass WeightMin() const = 0;

		virtual void WeightMax( Mass weightMax ) = 0;

		virtual Mass WeightMax( ) const = 0;
		///@}

		/// \name Trigger Condition
		/// \brief Sets the sensor to wether it riggers if the wheelframe's weight
		/// is inside the interval (true, the default) or outside (false).
		///@{
		virtual void TriggerInside( bool bTriggerInside ) = 0;

		virtual bool TriggerInside() const = 0;
		///@}


		/// \name WeighTrain
		/// \brief Determines wether to weigh the whole train or only the wagon, the wheelframe belongs to.
		///
		/// \param bTrain true for wighing the train, the default.
		///@{
		virtual void WeighTrain( bool bTrain ) = 0;

		virtual bool WeighTrain() const = 0;
		///@}
	};


	/// \brief A sensor that detects, wether a wheelframe passing by 
	/// does this with some value like weight, velocity or traction in certain limits.
	struct IntervalSensor : virtual SensorFilterJack{

		/// \brief Makes a IntervalSensor object.
		static dclspc std::unique_ptr<IntervalSensor> Make( int flags = SensorFilterJack::any ) noexcept;

		/// \name Value
		/// \brief Sets and Gets the value limits for the sensor.
		/// \param valueMin lower limit for the interval.
		/// \param valueMax upper limit for the interval.
		///@{
		virtual void Value( Real valueMin, Real valueMax ) = 0;

		virtual void Value( const common::Interval<Real>& interval ) = 0;

		virtual const common::Interval<Real>& Value() const = 0;

		virtual void ValueMin( Real valueMin ) = 0;

		virtual Real ValueMin() const = 0;

		virtual void ValueMax( Real valueMax ) = 0;

		virtual Real ValueMax( ) const = 0;
		///@}

		/// \name Trigger Condition
		/// \brief Sets the sensor to wether it riggers if the wheelframe's weight
		/// is inside the interval (true, the default) or outside (false).
		///@{
		virtual void TriggerInside( bool bTriggerInside ) = 0;

		virtual bool TriggerInside() const = 0;
		///@}
	};

	dclspc std::string ToString( SensorFilterJack::Effect effect );

	dclspc SensorFilterJack::Effect SensorEffectFromString( const std::string& effect );
	
}
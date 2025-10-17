//	trax track library
//	AD 2014 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
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

#include "Configuration.h"
#include "Units.h"
#include "Identified.h"

#include "spat/Frame.h"

namespace trax{

	struct Plug;
	struct Jack;
	struct SocketRegistry;

	/// \brief Interface for objects that indicates system states somehow.
	///
	/// Typically an Indicator would be represented by a semaphore in 3D space.
	struct Indicator : Identified<Indicator>
	{
		enum class Type : char{
			unknown = -1,
			none  = 0,
			switch_mono,
			switch_multi,
			velocity_control,
		};

		/// \brief Makes a Indicator object.
		static dclspc std::unique_ptr<Indicator> Make( Type type ) noexcept;


		/// \returns the name for the object type that implements this interface. 
		virtual const char*	TypeName() const noexcept = 0;


		/// \brief Possible Indicator states:
		enum class Status{
			none	= -1,	///< Invalid status 
			unknown	= 0,	///< unknown status
			one		= 1,
			two		= 2,
			three	= 3,
			four	= 4,
			five	= 5,
			six		= 6,
			seven	= 7,
			eight   = 8,
			nine	= 9,
			ten		= 10,
			eleven	= 11,
			twelve	= 12,
			thirteen= 13,
			count,			///< Number of valid status
			toggle,			///< status toggle
			change			///< status change
		};


		/// \brief Set frame of reference.
		///
		/// The frame sets the position and orientation relative
		/// to a parent object.
		virtual void SetFrame( const spat::Frame<Length,One>& frame ) = 0;


		/// \brief Get frame of reference.
		/// \returns const reference to internal frame object.
		virtual const spat::Frame<Length,One>& GetFrame() const = 0;


		///// \brief Sets an absolut frame of reference (used by collections)
		//virtual void SetAbsoluteFrame( const Frame<Real>& frame ) = 0;


		///// \brief Gets the frame relative to the Module, this Indicator is attached to.
		//virtual const Frame<Real>& GetAbsoluteFrame() const = 0;


		/// \brief Gets the pose of the Indicator in global coordinates.
		/// \param frame Frame to receive the pose data.
		virtual void Get( spat::Frame<Length,One>& frame ) const = 0;


		/// \brief Sets the indicator to the next setting.
		/// \param pulse If true, the JackOnChange Jack will pulse.
		virtual void Toggle( bool pulse = true ) = 0;


		/// \brief Turns the indicator to the specified setting.
		/// \param status The status to set the indicator to.
		/// \param pulse If true, the JackOnChange Jack will pulse.
		/// \returns The previous status of the indicator.
		virtual Status Set( Status status, bool pulse = true ) = 0;


		/// \brief Gets the setting of the indicator.
		virtual Status Get() const = 0;


		/// \returns true if the indicator state for status is a valid state 
		/// for the indicator.
		virtual bool IsValidState( Status status ) const = 0;


		/// \brief Set an angle to rotate the indicator by with its status.
		virtual void RotateWithStatus( Status status, Real angle ) = 0;


		/// \returns the rotation angle for a status.
		virtual Real RotateWithStatus( Status status ) const = 0;


		/// \brief Gets a Jack that pulses its Plug if the specified Status is set. 
		virtual Jack& JackOn( Status status ) = 0;


		/// \brief Gets a Plug that switches to the specified Status. 
		virtual Plug& PlugTo( Status status ) = 0;


		/// \returns A Plug to trigger the toggle method.
		virtual Plug& PlugToToggle() noexcept = 0;


		/// \returns A Jack that pulses its Plug if the indicator setting is changed.
		virtual Jack& JackOnChange() noexcept = 0;


		/// \brief Registers all the Plugs with the module and supplies the
		/// Jacks with plug ids for connecting.
		virtual void RegisterSockets( SocketRegistry& registry ) = 0;


		/// \brief Removes all references to Plugs and Jacks from the module.
		virtual void UnregisterSockets( SocketRegistry& registry ) = 0;
	};


	/// \brief Converts a TrackType to readable string.
	dclspc std::string ToString( Indicator::Type type );

	/// \brief Makes a string from the status
	dclspc std::string ToString( Indicator::Status status );


	/// \brief Makes a status value from a status string.
	/// \throws std::invalid_argument if the string was not recognized.
	dclspc Indicator::Status ToIndicatorStatus( const std::string& status );


	dclspc Indicator::Status IndicatorStatusFrom( const std::string& socketName );


	/// \brief An Indicator with two states.
	struct BinaryIndicator : Indicator{

		/// \brief Makes a BinaryIndicator object.
		static dclspc std::unique_ptr<BinaryIndicator> Make( Type type ) noexcept;
		

		/// \brief Gets a Jack that pulses its Plug if the BinaryIndicator is set to one. 
		virtual Jack& JackOnOne() noexcept = 0;


		/// \brief Gets a Plug that sets the BinaryIndicator to one.
		virtual Plug& PlugToOne() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the BinaryIndicator is set to zero. 
		virtual Jack& JackOnTwo() noexcept = 0;


		/// \brief Gets a Plug that sets the BinaryIndicator to zero.
		virtual Plug& PlugToTwo() noexcept = 0;
	};
}
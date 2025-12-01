//	trax track library
//	AD 2016
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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "Configuration.h"

#include <memory>

namespace trax{

	struct IDType;
	struct Plug;
	struct Jack;

	/// \brief Interface for a registry that reconnects Plugs and Jacks according to their IDs.
	struct SocketRegistry{

		/// \brief Makes a standard SocketRegistry object.
		static dclspc std::unique_ptr<SocketRegistry> Make() noexcept;


		/// \brief Registers a Plug with the module. 
		/// 
		/// If the Plug carries no id it gets assigned some unique id. If it comes with an unique 
		/// id, that one gets used. If a matching Jack is found, the Plug will get connected.
		/// \param plug Reference to Plug to register.
		/// \exception std::logic_error Thrown if it is tried to register a Plug with an id that 
		/// is already occupied by another registered Plug.
		virtual void RegisterPlug( Plug& plug ) = 0;


		/// \brief Removes the supplied Plug from registration database.
		/// \param plug Reference to Plug to unregister.
		virtual void UnRegisterPlug( const Plug& plug ) = 0;


		/// \returns A Plug previously registered with RegisterPlug() that carries the id or nullptr
		/// if no such plug exists.
		virtual Plug* GetPlug( IDType id ) const = 0;


		/// \returns the maximum valid plug id.
		virtual IDType MaxValidPlugID() const noexcept = 0;


		/// \brief Registers a Jack for connecting with a plug.
		///
		/// This will only reconnect a Jack wiches RefPlugID is set, otherwise will do nothing. A registerd
		/// Jack will be kept until a Plug appears with RegisterPlug that carries the same id. If a Plug gets
		/// registered with zero id all registered Jacks are removed.
		/// \param jack Reference to jack that needs reconnection.
		virtual void ConnectJack( Jack& jack ) = 0;


		/// \brief Remove Jacks foremerely registered with ConnectJack().
		/// \param jack Reference to jack that needs to be removed.
		virtual void RemoveJack( Jack& jack ) noexcept = 0;


		/// \brief Clears the registry.
		virtual void Clear() = 0;


		virtual ~SocketRegistry() = default;
		SocketRegistry( const SocketRegistry& ) = delete;
		SocketRegistry( SocketRegistry&& ) = delete;
		SocketRegistry& operator=( const SocketRegistry& ) = delete;
		SocketRegistry& operator=( SocketRegistry&& ) = delete;
	protected:
		SocketRegistry() = default;	
	};

}


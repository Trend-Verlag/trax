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
	struct MultiPlug;
	struct Plug;
	struct Jack;

	/// \brief Interface for a registry that reconnects Plugs and Jacks 
	/// according to their IDs.
	///
	/// A SocketRegistry can be used with Plugs and Jacks that carry unique 
	/// IDs and ref-IDs respectively. E.g., after reading from a file. Also 
	/// the registry mechanism can get used to assign unique IDs for plugs 
	/// and set the ref-IDs of jacks to the connected plug's ID.
	struct SocketRegistry{

		/// \brief Makes a standard SocketRegistry object.
		static dclspc std::unique_ptr<SocketRegistry> Make() noexcept;


		/// \brief Registers a Plug by assigning a unique ID() to it 
		/// and making it available for Jacks to connect with.
		/// 
		/// If the Plug carries no id it gets assigned some unique id 
		/// All Jacks previously Connected() will get removed from the 
		/// registry in that case, because their RefIDs can not be valid.
		/// If the Plug comes with an unique id, that one gets used. If 
		/// for an unconnected Plug a matching Jack is found, the Plug 
		/// will get connected.
		/// \param plug Reference to Plug to register.
		/// \exception std::logic_error Thrown if it is tried to register 
		/// a Plug with an id that is already occupied by another registered 
		/// Plug.
		///@{
		
		/// \brief Registers a single plug.
		virtual void RegisterPlug( Plug& plug ) = 0;

		/// \brief Registers all plugs of a multi-plug.
		virtual void RegisterPlug( MultiPlug& plug ) = 0;
		///@}


		/// \brief Removes the supplied Plug from registration database.
		/// \param plug Reference to Plug to unregister.
		///@{

		/// \brief Unregisters a single plug.
		virtual void UnRegisterPlug( const Plug& plug ) = 0;

		/// \brief Unregisters all plugs of a multi-plug.
		virtual void UnRegisterPlug( const MultiPlug& plug ) = 0;
		///@}


		/// \returns A Plug previously registered with RegisterPlug() that carries the id or nullptr
		/// if no such plug exists.
		virtual Plug* GetPlug( IDType id ) const = 0;


		/// \returns the maximum valid plug id.
		virtual IDType MaxValidPlugID() const noexcept = 0;


		/// \brief Registers a Jack for connecting with a plug.
		///
		/// This will only reconnect a Jack whose RefPlugID is set, 
		/// otherwise will do nothing if unconnected, or set the RefID
		/// if already connected to a Plug. A registerd Jack will be kept 
		/// until a Plug appears with RegisterPlug that carries the 
		/// referenced id. If a Plug gets registered with zero id, all 
		/// registered Jacks are removed.
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


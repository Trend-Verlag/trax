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


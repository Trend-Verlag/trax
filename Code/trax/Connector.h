//	trax track library
//	AD 2014 
//
//  "Hold up that light 
//   I'm lost at sea
//	 But I'm coming way home"
//
//				Arkells
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

#include "Identified.h"
#include "Track.h"
#include "ConnectorType.h"

namespace trax{

	struct MultiPlug;
	struct Jack;
	struct SocketRegistry;

	class Location;

	struct ConnectorUserData;

	/// \brief A Connector rearranges the connections between several track ends.
	///
	/// Tracks only have one connection at each end. A connector will reconnect 
	/// these connections according to its setting.
	struct Connector : Identified<Connector>
	{
		/// \returns Type that implements this interface. 
		virtual ConnectorType GetConnectorType() const noexcept = 0;


		/// \returns the name for the object type that implements this interface. 
		virtual const char*	TypeName() const = 0;


		/// \brief Sets the connector to the next setting.
		/// \param pulse If true the call will trigger a pulse on a plug if plugged
		/// into an appropriate jack.
		virtual void Toggle( bool pulse = true ) = 0;


		/// \brief Sets the Connector to the setting that connects the two track ends.
		/// \throws std::invalid_argument if one of the two track ends is not in a slot.
		/// \throws std::logic_error, if the two ends can not get connected by the Connector.
		virtual void Set( const Track& trackA, Track::EndType trackendA, const Track& trackB, Track::EndType trackendB, bool pulse = true ) = 0;


		/// \brief Disconnects all track ends from each other.
		virtual void Disconnect() = 0;


		/// \name Populate the Connector
		/// \param slot Zero based index of the slot.
		/// \param pTrack Pointer to track to attach to the slot. If nullptr, the slot gets released.
		/// \param trackend Denotes the tracks end.
		/// \param connectAnonymous If true and if the track end is already member of a Connector, it will
		/// not get deconnected from the first Connector, but be ruled by both.
		/// \throws std::out_of_range If the slot id is not available for the connector.
		/// \throws std::logic_error If the trackend is not applicable for beeing asssigned to the slot, e.g
		/// because it belongs to a track already assigned to some other slot in an impossible way.
		/// \returns The slot's id that got populated, or -1 if there wasn't a free slot.
		///@{
		/// \brief Populates a slot with a tracks end.
		virtual int Slot( int slot, std::shared_ptr<TrackBuilder> pTrack, Track::EndType trackend, bool connectAnonymous = false ) = 0;


		/// \brief Populates the slot with the smallest ID that is still unpopulated.
		virtual int Slot( std::shared_ptr<TrackBuilder> pTrack, Track::EndType trackend, bool connectAnonymous = false ) = 0;
		///@}


		/// \param slot Zero based index of the slot.
		/// \returns The track assigned to slot slot and the assigned end type.
		virtual std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> Slot( int slot ) const noexcept = 0;


		/// \returns the slot index, the track end is registered with or -1 if the track is not registered.
		virtual int Slot( const Track& track, Track::EndType trackend ) const = 0;


		/// \returns The number of slots, the connector provides.
		virtual int CntSlots() const noexcept = 0;


		/// \brief Exchanges the content of the two slots.
		/// \throws std::out_of_range If the slot id is not available for the connector.
		virtual void SwapSlots( int slotA, int slotB ) = 0;


		/// \returns True if all the track slots are populated.
		virtual bool IsComplete() const = 0;


		/// \brief Checks the connector for consistency.
		/// 
		/// Certain track ends in slots are supposed to align with each other.
		/// The method checks the the degree of general smoothness of the track ends
		/// that get connected by the connector.
		/// \param os Stream to write the error messages to.
		/// \param e_distance Allowed deviation for the distance between the tracks ends.
		/// \param e_kink Allowed deviation for the kink angle between the tracks.
		/// \param e_twist Allowed deviation for the twist angle between the tracks.
		/// \returns True if the connector is consistent with respect to the given tolerances.
		virtual bool Check( std::ostream& os, Length e_distance = epsilon__length, Angle e_kink = epsilon__angle, Angle e_twist = epsilon__angle ) const noexcept = 0;


		/// \brief clears the slot with index slot.
		/// \param slot Zero based index of the slot to clear. The method does nothing, if the slot is not existing.
		virtual void Clear( int slot ) = 0;


		/// \brief Releases all the tracks from the slots.
		virtual void Clear() = 0;


		/// \brief Gets the center of the tracks the connector is connecting.
		virtual void GetCenter( spat::Frame<Length,One>& center ) const = 0;


		/// \returns A Plug to trigger the toggle method.
		virtual MultiPlug& PlugToToggle() noexcept = 0;


		/// \returns A Plug to trigger the toggle method.
		virtual const MultiPlug& PlugToToggle() const noexcept = 0;


		/// \returns A Jack that pulses its Plug if the connector setting is changed.
		virtual Jack& JackOnChange() noexcept = 0;


		/// \returns A Jack that pulses its Plug if the connector setting is changed.
		virtual const Jack& JackOnChange() const noexcept = 0;


		/// \brief Registers all the Plugs with the module and supplies the
		/// Jacks with plug ids for connecting.
		virtual void RegisterSockets( SocketRegistry& modul ) = 0;


		/// \brief Removes all references to Plugs and Jacks from the module.
		virtual void UnregisterSockets( SocketRegistry& modul ) = 0;


		/// \brief Sets a pointer to user provided data.
		///
		/// Define ConnectorUserData to specify your data types.
		virtual void UserData( ConnectorUserData* pData ) noexcept = 0;


		/// \returns User provided data.
		virtual ConnectorUserData* UserData() const noexcept = 0;
	};


	/// \brief Connect two track ends by inserting them into Connectors, if available.
	///
	///	The two given track ends will get coupled by the function. If one end
	/// is populating a Connector's slot, the other is inserted to a slot of that
	/// Connector first. The slot would be the one designated by slotA for a switch found
	/// with trackEndA or slotB for a switch found with trackEndB, or the next
	/// free slot index, if slotA/B happens to be -1. In the former case any other track
	/// in that slot would be freed.
	/// After inserting or if both track ends are members of the same Connector already,
	/// the Connector is used to connect the ends, resulting in an exception thrown if the
	/// Connector is not able to do this. This behaviour can be overriden by setting the connectAnyway
	/// flag; the two switches then would be allowed to deal with the track end concurrently.
	/// 
	/// \param trackEndA denotes a track's end to get coupled and inserted into a Connector's slot.
	/// \param trackEndB denotes another track's end to get coupled and inserted into a Connector's slot.
	/// \param slotA Determines the slot index of a Connector found with trackEndA, trackEndB is 
	/// inserted to, if only one track end is member of a Connector or connectAnyway is set. 
	/// If -1, the next empty slot with the lowest index number would be choosen.
	/// \param slotB Determines the slot index of a Connector found with trackEndB, trackEndA is 
	/// inserted to, if only one track end is member of a Connector or connectAnyway is set. 
	/// If -1, the next empty slot with the lowest index number would be choosen.
	/// \param connectAnyway If both track ends are already members of different Connectors,
	/// a std::runtime_error will get thrown if this flag is false. If it is true, both
	/// connectors gain control over the track ends simultaneously, but Track::GetConnector()
	/// will continue to return the original connector.
	/// \throws std::runtime_error if the track ends are members of different Connectors.
	/// throws std::out_of_range, if the slot does not exist.
	/// \throws std::logic_error, if the two ends can not get connected by the Connector.
	/// \throws std::inalid_argument If one of the tracks is not valid or both are the same, or 
	/// slot is not existing.
	dclspc void Couple( 
		std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> trackEndA, 
		std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> trackEndB, 
		int slotA = -1,
		int slotB = -1,
		bool connectAnyway = false );


	/// \brief Uncouples a track end by clearing any Connectors.
	///
	/// The track end gets decoupled and is removed from any Connector.
	/// \param track The track to decouple an end of.
	/// \param trackEnd The track end to decouple.
	/// \param ownerSlot A slot type that is seen as the owner of the
	/// connector, so the track end will not get removed from it.
	dclspc void DeCouple( TrackBuilder& track, Track::EndType trackEnd, int ownerSlot = -1 );

	
	/// A location on a connected track. The Location will be oriented away from the 
	/// connected track end and sit on that track's end that is not connected by the connector
	/// in slotID.
	/// \param connector Connector to investigate.
	/// \param slotID ID of the connectors slot.
	/// \returns A Location sitting on the connected track's far end or nullptr, if the slot is
	/// not populated.
	///@{
	dclspc Location GetFarEndLocation( const Connector& connector, int slotID );

	dclspc Location GetEndLocation( const Connector& connector, int slotID );
	///@}


	/// \param connector Connector to investigate.
	/// \param fromSlot Slot to evaluate the track's end distance for.
	/// \returns A Vector, signifying the distance from the branching point
	/// of the switch to the tracks others end, that is not assigned to the
	/// slot.
	/// \throws std::runtime_error, if the slot is empty.
	dclspc spat::Vector<Length> DistanceToOtherEnd( const Connector& connector, int fromSlot );
}

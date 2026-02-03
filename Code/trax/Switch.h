//	trax track library
//	AD 2014 
//
//  "Doesn't anybody ever know ..."
//
//						Our Lady Peace
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

#include "Connector.h"

namespace trax{

	class Location;

	/// \brief Common interface for switch types that maintain an incomming (narrow)
	/// track, an outgoing straight track and an arbitrary number of outgoing diverging tracks.
	struct NarrowSwitch : virtual Connector{

		/// \brief Makes a standard NarrowSwitch object.
		static dclspc std::unique_ptr<NarrowSwitch> Make( unsigned char numBranches ) noexcept;

		enum SlotNames : char{
			slot_none = -1,
			slot_0 = 0,		///< narrow track
			slot_1,			///< straight track
			slot_2,			///< first diverging track
			slot_3,			///< second diverging track
			slot_4,			///< third diverging track
			slot_5,			///< fourth diverging track
			slot_N = 125,	///< Nth diverging track
			maxSlots
		};


		/// \brief Status 
		enum class Status : char{
			none = -1,
			go,					///< go over straight track
			branch  = 1,		///< go over first diverging track
			branch1 = 1,		///< go over first diverging track
			branch2 = 2,		///< go over second diverging track
			branch3 = 3,
			branch4 = 4,
			bramch5 = 5,
			branchN = 124,
			maxBranches = 124,
			toggle,				///< toggle the switch
			change,		
			empty				///< set to first empty from go over branch1 to CntDivergedTracks()
		};


		/// \brief Turns the switch to the specified setting.
		/// \returns The previous status of the switch.
		virtual Status Set( Status to, bool pulse = true ) = 0;

		using Connector::Set;


		/// \brief Gets the setting of the switch.
		virtual Status Get() const = 0;


		/// \brief Occupies the slot for the narrow track.
		///
		/// The narrow track is the incomming track of a switch.
		/// \param pNarrowTrack Shared pointer to track.
		/// \param trackEnd End of track to connect.
		virtual void NarrowTrack( std::shared_ptr<TrackBuilder> pNarrowTrack, EndType trackEnd ) = 0;


		/// \returns the track and the end type in the narrow slot.
		virtual std::pair<std::shared_ptr<TrackBuilder>,EndType> NarrowTrack() const = 0;


		/// \brief Clears the slot for the narrow track.
		virtual void ClearNarrowTrack() = 0;


		/// \brief Occupies the slot for the straight track.
		///
		/// The straight track is the go through track of a switch.
		/// \param pStaightTrack Shared pointer to track.
		/// \param trackEnd End of track to connect.
		virtual void StraightTrack( std::shared_ptr<TrackBuilder> pStaightTrack, EndType trackEnd ) = 0;


		/// \returns the track and the end type in the straight slot.
		virtual std::pair<std::shared_ptr<TrackBuilder>,EndType> StraightTrack() const = 0;


		/// \brief Clears the slot for the straight track.
		virtual void ClearStraightTrack() = 0;


		/// \brief Occupies the slot for the diverging track.
		///
		/// The diverging track is the branching track of a switch.
		/// \param divTrackID zero based id of the diverging track to set.
		/// \param pDivergedTrack Shared pointer to track.
		/// \param trackEnd End of track to connect.
		virtual void DivergedTrack( int divTrackID, std::shared_ptr<TrackBuilder> pDivergedTrack, EndType trackEnd ) = 0;


		/// \returns the track and the end type in the straight slot.
		virtual std::pair<std::shared_ptr<TrackBuilder>,EndType> DivergedTrack( int divTrackID ) const = 0;


		/// \brief Clears the slot for the straight track.
		virtual void ClearDivergedTrack( int divTrackID ) = 0;


		/// \returns The number of diverging tracks. This does not include the outgoing straight track.
		virtual int CntDivergedTracks() const = 0;


		/// \brief Gets a track Location object right at the bifurcation point 
		/// of the switch with a foreward direction aiming towards the branching
		/// tracks.
		virtual Location Bifurcation() const = 0;


		/// \brief Gets a Jack that pulses its Plug if the switch is set to go. 
		virtual Jack& JackOnGo() noexcept = 0;


		/// \brief Gets a Plug that sets the switch to go.
		virtual MultiPlug& PlugToGo() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the specified Status is set. 
		virtual Jack& JackOn( Status status ) = 0;


		/// \brief Gets a Plug that switches to the specified Status. 
		virtual MultiPlug& PlugTo( Status status ) = 0;


		/// \brief Gets a Jack that pulses its Plug if the switch is set to the branching track. 
		virtual Jack& JackOnDivergedTrack( int divTrackID ) = 0;


		/// \brief Gets a Plug that sets the switch to the outgoing track.
		virtual MultiPlug& PlugToDivergedTrack( int divTrackID ) = 0;
	};


	/// \brief Makes a string from the status
	dclspc std::string ToString( NarrowSwitch::Status status );


	/// \brief Makes a status value from a status string.
	/// \throws std::invalid_argument if the string was not recognized.
	dclspc NarrowSwitch::Status ToNarrowSwitchStatus( const std::string& status );


	/// \returns The status a specific socket is related with.
	dclspc NarrowSwitch::Status NarrowSwitchStatusFrom( const std::string& socketName );


	/// \brief A Switch rearranges the connections between three track ends.
	///
	/// Tracks only have one connection at each end. A switch will reconnect 
	/// these connections according to its setting.
	struct Switch : virtual NarrowSwitch
	{
		/// \brief Makes a standard Switch object.
		static dclspc std::unique_ptr<Switch> Make() noexcept;


		static constexpr char slot_count = 3;
		static constexpr char status_count = 2;
		static constexpr char branch_count = 1;

		using NarrowSwitch::DivergedTrack;

		/// \brief Occupies the slot for the diverged track.
		///
		/// The diverged track is the branching track of a switch.
		/// \param pDivergedTrack Shared pointer to track.
		/// \param trackEnd End of track to connect.
		virtual void DivergedTrack( std::shared_ptr<TrackBuilder> pDivergedTrack, EndType trackEnd ) = 0;


		/// \returns the track and the end type in the diverged slot.
		virtual std::pair<std::shared_ptr<TrackBuilder>,EndType> DivergedTrack() const = 0;


		using NarrowSwitch::ClearDivergedTrack;

		/// \brief Clears the slot for the diverged track.
		virtual void ClearDivergedTrack() = 0;


		///\brief Swaps the straight and the diverged track.
		virtual void SwapTracks() = 0;


		/// \brief Returns true, if the straight track has fewer deviation from the 
		/// narrow's track direction than the diverged track. 
		///
		/// This property should typically be assigned with the straight track.
		/// Use SwapTracks() to make a switch normal. The test is done by the
		/// track locations at the minimum length of both outgoing tracks
		/// if bStopAtFirstTrack is true, only limited by maxDistance.
		/// \param bStopAtFirstTrack If true the test will be done on the shorter length of the
		/// two outgoing tracks, if maxDistance doesn't provide a narrower limit. Limit the maxDistance
		/// carefully if you set bStopAtFirstTrack to false.
		/// \param maxDistance Maximum distance along the outgoing tracks to look for comparison points.
		/// If there is an open end that would define the maximum distance.
		/// \returns true if the switch is normal.
		/// \throws std::invalid_argument if bStopAtFirstTrack == false and maxDistance == infinite_length.
		virtual bool IsNormal( bool bStopAtFirstTrack = true, Length maxDistance = infinite__length ) const = 0;


		/// \brief Swaps the outgoing tracks if this switch is not normal.
		/// \return True if the track was normal at in the first palce.
		virtual bool Normalize() = 0;


		/// \brief Is this an Y - switch?
		///
		/// An Y - switch is a switch with its straight and diverged tracks going in
		/// opposite directions with respect to the narrow track's direction. The test 
		/// is done by the track locations at the minimum length of both outgoing tracks
		/// if bStopAtFirstTrack is true, only limited by maxDistance. 
		/// There is a margin that qualifies a track for diverting from the 'straight direction': 
		/// if it just does so by a smaller or equal angle than margin, it would be considered
		/// to be straight and the switch would not qualify for being an Y-switch.
		/// \param bStopAtFirstTrack If true the test will be done on the shorter length of the
		/// two outgoing tracks, if maxDistance doesn't provide a narrower limit. Limit the maxDistance
		/// carefully if you set bStopAtFirstTrack to false.
		/// \param maxDistance Maximum distance along the outgoing tracks to look for comparison points.
		/// If there is an open end that would define the maximum distance.
		/// \param margin if the two branches divert to opposite sides but one only 
		/// be equal or less the deviation given by margin, the switch will not be Y.
		/// \returns true if the switch qualifies for being an Y-switch.
		/// \throws std::invalid_argument if bStopAtFirstTrack == false and maxDistance == infinite_length.
		virtual bool IsY( bool bStopAtFirstTrack = true, Length maxDistance = infinite__length, AnglePerLength margin = epsilon__angle / 80_m ) const = 0;


		/// \param bStopAtFirstTrack If true the test will be done on the shorter length of the
		/// two outgoing tracks, if maxDistance doesn't provide a narrower limit. Limit the maxDistance
		/// carefully if you set bStopAtFirstTrack to false.
		/// \param maxDistance Maximum distance along the outgoing tracks to look for comparison points.
		/// If there is an open end that would define the maximum distance.
		/// \returns true if the branch develops horizontally more than vertically.
		/// \see BranchLeftOrRight
		virtual bool IsHorizontal( bool bStopAtFirstTrack = true, Length maxDistance = infinite__length ) const = 0;


		/// \param bStopAtFirstTrack If true the test will be done on the shorter length of the
		/// two outgoing tracks, if maxDistance doesn't provide a narrower limit. Limit the maxDistance
		/// carefully if you set bStopAtFirstTrack to false.
		/// \param maxDistance Maximum distance along the outgoing tracks to look for comparison points.
		/// If there is an open end that would define the maximum distance.
		/// \returns true if the branch develops vertically more than horizontally.
		/// \see BranchUpOrDown
		virtual bool IsVertical( bool bStopAtFirstTrack = true, Length maxDistance = infinite__length ) const = 0;


		/// \brief Tests wether the branching track develops to the left or right 
		/// side of the straight track.
		/// 
		/// The test is done by the track locations at the minimum length of both outgoing 
		/// tracks. The directions are understood in terms of the frame at the bifurcation:
		/// branching left would mean that the branch end develops to the positive N direction of
		/// that frame more or equally as the straight track does.
		/// \param bStopAtFirstTrack If true the test will be done on the shorter length of the
		/// two outgoing tracks, if maxDistance doesn't provide a narrower limit. Limit the maxDistance
		/// carefully if you set bStopAtFirstTrack to false.
		/// \param maxDistance Maximum distance along the outgoing tracks to look for comparison points.
		/// If there is an open end that would define the maximum distance.
		/// \returns true if the branch develops to the left, false if to the right. If
		/// there is no difference, the method returns true.
		virtual bool BranchLeftOrRight( bool bStopAtFirstTrack = true, Length maxDistance = infinite__length ) const = 0;


		/// \brief Tests wether the branching track develops to the up or down 
		/// side of the straight track.
		/// 
		/// The test is done by the track locations at the minimum length of both outgoing 
		/// tracks. The directions are understood in terms of the frame at the bifurcation:
		/// branching up would mean that the branch end develops to the positive B direction of
		/// that frame more or equally as the straight track does.
		/// \param bStopAtFirstTrack If true the test will be done on the shorter length of the
		/// two outgoing tracks, if maxDistance doesn't provide a narrower limit. Limit the maxDistance
		/// carefully if you set bStopAtFirstTrack to false.
		/// \param maxDistance Maximum distance along the outgoing tracks to look for comparison points.
		/// If there is an open end that would define the maximum distance.
		/// \returns true if the branch develops to the up, false if to the down. If
		/// there is no difference, the method returns true.
		virtual bool BranchUpOrDown( bool bStopAtFirstTrack = true, Length maxDistance = infinite__length ) const = 0;


		/// \brief Gets a Jack that pulses its Plug if the switch is set to branch. 
		virtual Jack& JackOnBranch() noexcept = 0;


		/// \brief Gets a Plug that sets the switch to branch.
		virtual MultiPlug& PlugToBranch() noexcept = 0;
	};


	/// \brief A switch that is connecting one narrow end to three outgoing track ends.
	struct ThreeWaySwitch : virtual NarrowSwitch{

		/// \brief Makes a standard ThreeWaySwitch object.
		static dclspc std::unique_ptr<ThreeWaySwitch> Make() noexcept;


		static constexpr char slot_count = 4;
		static constexpr char status_count = 3;
		static constexpr char branch_count = 2;


		/// \brief Occupies the slot for the diverged track.
		///
		/// The diverged track is the branching track of a switch.
		/// \param pDivergedTrack1 Shared pointer to track.
		/// \param trackEnd End of track to connect.
		virtual void DivergedTrack1( std::shared_ptr<TrackBuilder> pDivergedTrack1, EndType trackEnd ) = 0;


		/// \returns the track and the end type in the diverged slot.
		virtual std::pair<std::shared_ptr<TrackBuilder>,EndType> DivergedTrack1() const = 0;


		/// \brief Clears the slot for the diverged1 track.
		virtual void ClearDivergedTrack1() = 0;


		/// \brief Occupies the slot for the diverged track.
		///
		/// The diverged track is the branching track of a switch.
		/// \param pDivergedTrack2 Shared pointer to track.
		/// \param trackEnd End of track to connect.
		virtual void DivergedTrack2( std::shared_ptr<TrackBuilder> pDivergedTrack2, EndType trackEnd ) = 0;


		/// \returns the track and the end type in the diverged slot.
		virtual std::pair<std::shared_ptr<TrackBuilder>,EndType> DivergedTrack2() const = 0;


		/// \brief Clears the slot for the diverged2 track.
		virtual void ClearDivergedTrack2() = 0;


		///\brief Swaps the straight and the diverged track.
		virtual void SwapStraightWithDiverged() = 0;

		///\brief Swaps the straight and the diverged2 track.
		virtual void SwapStraightWithDiverged2() = 0;

		///\brief Swaps the diverged and the diverged2 track.
		virtual void SwapDivergedWithDiverged2() = 0;


		/// \brief Returns true, if the straight track lies in the middle of the 
		/// two diverging tracks and the first diverging track is on its left.
		///
		/// Use Normalize() to make a switch normal. The test is done by the
		/// track locations at the minimum length of the three outgoing tracks.
		virtual bool IsNormal() const = 0;


		/// \brief Rearranges the outgoing tracks if this switch is not normal.
		/// \return True if the track was normal at in the first palce.
		virtual bool Normalize() = 0;


		/// \returns the status that would lead over the left outgoing track.
		virtual Status StatusToLeft() const = 0;


		/// \returns the status that would lead over the middle outgoing track.
		virtual Status StatusToMiddle() const = 0;


		/// \returns the status that would lead over the right outgoing track.
		virtual Status StatusToRight() const = 0;


		/// \brief Gets a Jack that pulses its Plug if the switch is set to branch. 
		virtual Jack& JackOnBranch1() noexcept = 0;


		/// \brief Gets a Plug that sets the switch to branch.
		virtual MultiPlug& PlugToBranch1() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the switch is set to branch. 
		virtual Jack& JackOnBranch2() noexcept = 0;


		/// \brief Gets a Plug that sets the switch to branch.
		virtual MultiPlug& PlugToBranch2() noexcept = 0;
	};


	/// \brief A single slip switch is made from five tracks that form a crossing
	/// with the possibility to divert the route to connect the two narrow (incoming)
	/// tracks.
	/// There are two narrowing (incoming) tracks, two crossing tracks and one diverting 
	/// track, connecting the two narrowing ones.
	/// The two narrow '\','/' and two straight tracks 'X' - the straight tracks are expected
    /// to cross each other - and a diverging track ')' (ignore the dots).
    /// \verbatim
	/// 
	/// in
	/// 
    /// \. 
    /// )X
    /// / 
	/// 
	/// out
	/// 
    ///\endverbatim
	struct SingleSlipSwitch : virtual Connector{

		/// \brief Makes a standard SingleSlipSwitch object.
		static dclspc std::unique_ptr<SingleSlipSwitch> Make() noexcept;

		enum SlotNames{
			slot_none = -1,
			slot_0 = 0,	///< first narrow track end
			slot_1,		///< second narrow track end
			slot_2,		///< first straight track end
			slot_3,		///< second straight track end
			slot_4,		///< diverging track end connecting to slot_0
			slot_5,		///< diverging track end connecting to slot_1
			slot_count
		};

		/// \brief Status values of a switch.
		///
		/// Note that there are actually only two states for the
		/// SingleSlipSwitch: go and branch. The others are redundant 
		/// and play a role only in toggling and pulsing.
		enum class Status{
			none = -1,			///< status not specified.
			go,					///< the train runs over the straight tracks.
			go1 = go,			///< the train runs over the straight tracks.
			go2,				///< the train runs over the straight tracks. Second go status only differs in in the jack that pulses.
			branch,				///< the train runs over the diverging track.
			count,				///< number of valid status values.
			toggle,				///< toggle through the valid status (including go2).
			change				///< signal a status change (used with Jacks'n Plugs).
		};

		/// \brief Turns the switch to the specified setting.
		/// \returns The previous status of the switch.
		virtual Status Set( Status to, bool pulse = true ) = 0;

		using Connector::Set;


		/// \brief Gets the setting of the switch.
		virtual Status Get() const = 0;


		/// \brief Sets the center of the switch.
		virtual void SetCenter( const spat::Frame<Length,One>& center ) = 0;


		/// \brief Gets a Jack that pulses its Plug if the specified Status is set. 
		virtual Jack& JackOn( Status status ) = 0;


		/// \brief Gets a Plug that switches to the specified Status. 
		virtual MultiPlug& PlugTo( Status status ) = 0;
	};


	/// \brief Makes a string from the status
	dclspc std::string ToString( SingleSlipSwitch::Status status );


	/// \brief Makes a status value from a status string.
	/// \throws std::invalid_argument if the string was not recognized.
	dclspc SingleSlipSwitch::Status ToSingleSlipSwitchStatus( const std::string& status );


	/// \returns The status a specific socket is related with.
	dclspc SingleSlipSwitch::Status SingleSlipSwitchStatusFrom( const std::string& socketName );


	/// \brief A double slip switch is made from eight tracks that form a crossing
	/// with the possibility to divert the route to connect two of the narrowing (incoming)
	/// tracks pairwise.
	/// There are four narrowing (incoming) tracks, two crossing tracks and two diverting 
	/// tracks, connecting two narrowing ones each.
	/// The two straight tracks 'X', are crossing each other; two incoming narrowing tracks, 
	/// two outgoing narrowing tracks - all four connecting to the straight track ends - and 
	/// two diverging tracks ')' and '(', connecting incoming and outgoing tracks (ignore the dots):
    /// \verbatim
	/// 
	/// in
	/// 
    /// \./
    /// )X(
    /// / \.  
	/// 
	/// out
	/// 
    /// \endverbatim
	struct DoubleSlipSwitch : virtual Connector{

		/// \brief Makes a standard DoubleSlipSwitch object.
		static dclspc std::unique_ptr<DoubleSlipSwitch> Make() noexcept;


		enum SlotNames{
			slot_none = -1,
			slot_0 = 0,		///< first narrowing track end
			slot_1,			///< second narrowing track end on the same side of first
			slot_2,			///< third narrowing track end on opposite side to the first
			slot_3,			///< fourth narrowing track end on same side to third
			slot_4,			///< crossing track end connected with first narrowing
			slot_5,			///< crossing track end connected with second narrowing
			slot_6,			///< crossing track end connected with third narrowing
			slot_7,			///< crossing track end connected with fourth narrowing
			slot_8,			///< diverging track end connected with first narrowing
			slot_9,			///< diverging track end connected with fourth narrowing
			slot_10,		///< diverging track end connected with third narrowing
			slot_11,		///< diverging track end connected with second narrowing
			slot_count
		};


		/// \brief Status values of a switch.
		///
		/// Note that there are actually only two states for the
		/// DoubleSlipSwitch: go and branch. The others are redundant 
		/// and play a role only in toggling and pulsing.
		enum class Status{
			none = -1,			///< status not specified.
			go,					///< the train runs over the crossing tracks.
			go1 = go,			///< the train runs over the crossing tracks.
			go2,				///< the train runs over the crossing tracks. Second go status only differs in the jack that pulses.
			branch,				///< the train runs over the diverging tracks.
			branch1 = branch,	///< the train runs over the diverging tracks.
			branch2,			///< the train runs over the diverging tracks. Second branch status only differs in the jack that pulses.
			count,				///< number of valid status values.
			toggle,				///< toggle through the valid status (including go2 and branch2).
			change				///< signal a status change (used with Jacks'n Plugs).
		};

		/// \brief Turns the switch to the specified setting.
		/// \returns The previous status of the switch.
		virtual Status Set( Status to, bool pulse = true ) = 0;

		using Connector::Set;


		/// \brief Gets the setting of the switch.
		virtual Status Get() const = 0;


		/// \brief Sets the center of the switch.
		virtual void SetCenter( const spat::Frame<Length,One>& center ) = 0;


		/// \brief Gets a Jack that pulses its Plug if the specified Status is set. 
		virtual Jack& JackOn( Status status ) = 0;


		/// \brief Gets a Plug that switches to the specified Status. 
		virtual MultiPlug& PlugTo( Status status ) = 0;
	};


	/// \brief Makes a string from the status
	dclspc std::string ToString( DoubleSlipSwitch::Status status );


	/// \brief Makes a status value from a status string.
	/// \throws std::invalid_argument if the string was not recognized.
	dclspc DoubleSlipSwitch::Status ToDoubleSlipSwitchStatus( const std::string& status );


	/// \returns The status a specific socket is related with.
	dclspc DoubleSlipSwitch::Status DoubleSlipSwitchStatusFrom( const std::string& socketName );


	struct SwitchAligned{

		virtual void Attach( std::shared_ptr<Connector> pSwitch ) = 0;

		virtual bool PreserveUpDirection() const = 0;

		virtual void PreserveUpDirection( bool bPreserveUpDirection ) = 0;

		virtual IDType GetSwitchRef() const = 0;

		virtual std::shared_ptr<Connector> GetSwitch() const = 0;


		virtual ~SwitchAligned() = default;
		SwitchAligned( const SwitchAligned& ) = delete;
		SwitchAligned( SwitchAligned&& ) = delete;
		SwitchAligned& operator=( const SwitchAligned& ) = delete;
		SwitchAligned& operator=( SwitchAligned&& ) = delete;
	protected:
		SwitchAligned() = default;
	};

}
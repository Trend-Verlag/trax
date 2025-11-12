//	trax track library
//	AD 2013 
//
//  "Sooner or later in life
//	 the things you love you'll loose"
//
//				Florence + The Machine
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

/// \page docu_location Location
/// \section location_intro Introduction
/// A Location helps with moving along a system of connected tracks. It provides methods for 
/// determining the exact position on the track system, which would be the Track and some 
/// TrackLocation that specifies the parameter and an Orientation; as well as \ref location_getters getter
/// methods to transition the track's position to 3D positional and orientational data.
/// Besides from moving the Location, the Location::Move methods will trigger Sensors and help communicating 
/// with Signals along the way.
/// 
/// \image html Tracksystem.png
/// \section location_examples Example:
///
/// To move on a track circle:
/// \code
///	auto pTrack1 = TrackBuilder::Make();
///	auto pTrack2 = TrackBuilder::Make();
///	auto pTrack3 = TrackBuilder::Make();
///	auto pTrack4 = TrackBuilder::Make();
///	
/// // We build a track circle with radius 10_m around the Origin3D from the four pieces:
/// auto pArc1 = ArcP::Make();
/// pArc1->Create( { Origin3D<Length>, { 1, 0, 0 }, { 0, 10, 0 } } );
/// pTrack1->Attach( std::move(pArc1), {0_m,10_m*pi/2} );
///
/// auto pArc2 = ArcP::Make();
/// pArc2->Create( { Origin3D<Length>, { 0, 1, 0 }, { -10, 0, 0 } } );
/// pTrack2->Attach( std::move(pArc2), {0_m,10_m*pi/2} );
///
/// auto pArc3 = ArcP::Make();
/// pArc3->Create( { Origin3D<Length>, { -1, 0, 0 }, { 0, -10, 0 } } );
/// pTrack3->Attach( std::move(pArc3), {0_m,10_m*pi/2} );
///
/// auto pArc4 = ArcP::Make();
/// pArc4->Create( { Origin3D<Length>, { 0, -1, 0 }, { 10, 0, 0 } } );
/// pTrack4->Attach( std::move(pArc4), {0_m, 10_m*pi/2} );
/// 
/// pTrack1->Couple( std::make_pair(pTrack1, Track::EndType::end), std::make_pair(pTrack2, Track::EndType::front) );
/// pTrack2->Couple( std::make_pair(pTrack2, Track::EndType::end), std::make_pair(pTrack3, Track::EndType::front) );
/// pTrack3->Couple( std::make_pair(pTrack3, Track::EndType::end), std::make_pair(pTrack4, Track::EndType::front) );
/// pTrack4->Couple( std::make_pair(pTrack4, Track::EndType::end), std::make_pair(pTrack1, Track::EndType::front) );
/// 
/// Location loc{ pTrack1, { 0_m, Orientation::Value::para } };
/// 
/// Frame<Length,One> frame1, frame2;
/// loc.Transition( frame1 );
/// loc.Move( pTrack1->GetLength() + pTrack2->GetLength() + pTrack3->GetLength() + pTrack4->GetLength() );
/// loc.Transition( frame2 );
/// BOOST_CHECK( frame1.Equals( frame2, epsilon__length, 0.001f ) );
/// 
/// loc.Move( pTrack1->GetLength() + pTrack2->GetLength() + pTrack3->GetLength()/2 );
/// auto pTrack = loc.GetTrack();
/// BOOST_CHECK( pTrack == pTrack3 );
/// 
/// pTrack1->DeCouple(); // Coupled tracks will prevent each others destruction \see TrackSystem
/// pTrack3->DeCouple();
/// \endcode
///
/// Do also look at the tests in the test suit for further examples.

#include "Units.h"
#include "Track.h"
#include "TrackLocation.h"

#include <memory>

namespace trax
{
	struct Event;
	struct SignalTarget;

	struct TrackLocation;
	template<typename> struct TrackData;

	///	\brief A Location specifies a position on a track system 
	///	by referencing a track and a TrackLocation on it. 
	/// 
	///	It provides member functions for moving along a track, triggering Sensors
	///	and receiving Signals that are attached along the tracks. Use it 
	/// to get information such as 3D position about a location on a track
	/// system.
	class Location{
	public:
		/// \brief Construction
		///
		/// The non-default constructor will resolve the location (calculating the 
		/// correct track if tl is outside pTrack's range). 
		/// \throws std::out_of_range if the track system location could not get resolved.
		///@{
		dclspc Location() noexcept = default;
		dclspc Location( const Location& location ) noexcept = default;
		dclspc Location( Location&& location ) noexcept = default;
		dclspc Location( std::shared_ptr<Track> pTrack, const TrackLocation& tl );
		dclspc Location( const Track& track, const TrackLocation& tl );
		///@}

		dclspc virtual ~Location() = default;

		/// \brief Assignment
		///@{
		dclspc Location& operator=( const Location& location ) noexcept = default;
		dclspc Location& operator=( Location&& location ) noexcept = default;
		///@}


		/// \brief Attaches this Location onto the specified track. 
		///
		///	The track location gets resolved; this means that if the parameter value is 
		/// out of Track::Range() for this track, the method tries to find the track for 
		/// the position on coupled tracks.
		/// If a dead end is encountered in this process a std::out_of_range exception gets
		/// thrown.
		/// \param pTrack The track relativ to wich tl is formulated.
		/// \param tl Parameter value and orientation on the track.
		/// \throws std::invalid_argument if pTrack is nullptr or the track is invalid.
		/// \throws std::out_of_range if the location could not get resolved.
		dclspc void PutOn( std::shared_ptr<Track> pTrack, const TrackLocation& tl );


		/// \brief Removes the Location from its position on the track.
		dclspc void PutOff() noexcept;


		/// \returns True if the Location is put on a rail; false otherwise.
		dclspc bool IsOnTrack() const noexcept;


		/// \brief Gets the track this Location is on.
		///
		/// \returns The track or nullptr if no track is assigned.
		dclspc std::shared_ptr<Track> GetTrack() const noexcept;


		/// \returns the arc length of the location.
		dclspc Length Param() const noexcept;


		/// \returns the orientation of the location.
		dclspc Orientation Orient() const noexcept;


		/// \returns the curvature of the curve at the location.
		/// \throws std::exception if the curvature was not available.
		dclspc AnglePerLength Curvature() const;


		/// \returns the torsion of the curve at the location.
		/// \throws std::exception if the torsion was not available.
		dclspc AnglePerLength Torsion() const;


		/// \brief Receives the TNB - Frame of the curve at the location.
		///
		///	The Frame received by the Transition() methods is aligned to Locations orientation 
		/// on the tracks and a twisting of the line. For dynamic calculations the
		/// real geometry of the curve is needed.
		dclspc void TNBFrame( spat::Frame<Length,One>& frame ) const;


		/// \brief Gets the TrackLocation - data of this track location.
		dclspc void Get( TrackLocation& tl ) const noexcept;


		/// \brief Gets the TrackLocationRef - data of this track location.
		dclspc void Get( TrackLocationRef& tlr ) const noexcept;


		/// \name Transition
		/// \anchor location_getters
		///	\brief Copys the 3D position and orientation of this Location.
		///
		/// Coordinates are calculated in the global frame.
		///	\param pos Reference to a Position to receive the 3d location.
		///	\param tan Reference to a Vector to receive the 3D tangential vector.
		///	\param bundle Reference to a VectorBundle to receive the full 3D 
		/// position and tangential vector.
		///	\param frame Reference to a Frame to receive the full 3d location 
		/// and orientation.
		/// \param td Reference to a track data struct. Receives the data in engine units.
		/// Used by trackjoints and trackfeeders internally.
		/// \throws std::logic_error if the location is not on track.
		/// \throws std::exception if the transition could not get performed.
		///@{
		dclspc void Transition( spat::Position<Length>& pos ) const;

		dclspc void Transition( spat::Vector<One>& tan ) const;

		dclspc void Transition( spat::VectorBundle<Length,One>& bundle ) const;

		dclspc void Transition( spat::VectorBundle2<Length,One>& bundle ) const;

		dclspc void Transition( spat::Frame<Length,One>& frame ) const;

		dclspc void Transition( TrackData<Real>& td, Real engine_meters_per_unit ) const;
		///@}
	

		/// \name Move
		/// \brief Moves this Location by dParam along the tracks with respect to 
		/// actual switch settings.
		///
		/// Optionally the function receives signals and triggers sensors. If a dead
		/// end of a track (uncoupled track end) is reached that end's position will be the 
		/// resulting location. All sensors in the valid range get triggered for the given
		/// Event and all signals be dispatched to the SignalTarget.
		///
		///	\param dParam Incremental value to move the location in the direction of the track
		/// (not the locations positive orientation). Be aware that huge values 
		///	in combination with cyclic track systems might result in long execution times 
		/// of this function.
		///
		/// \param principalDirection Specifies the direction of movement 
		/// relative to the Location's orientation. This is used to receive 
		/// signals and might differ from the sign of ds. For one, ds might 
		/// be zero, for second it might happen that an actual movement is 
		/// oriented against the principal direction.
		///
		///	\param pEvent Trigger sensors and dispatch Event.
		///
		///	\param pSignalTarget Receive signals.
		///
		///
		/// \return 0m if the movement could get accomplished. If a dead end was encountered 
		/// during movement the leftover parameter value is returned. I.e.: if the dead end 
		/// would be connected a new move operation with that very value would lead to the 
		/// correct position. The bool flag indicates wether one or more track transitions 
		/// have been performed during the operation.
		/// \throws std::exception if the move could not get performed.
		///@{
		dclspc std::pair<Length,bool> Move( Length dParam );

		dclspc std::pair<Length,bool> Move( Length dParam, const Event* pEvent );

		dclspc std::pair<Length,bool> Move( Length dParam, Orientation principalDirection, SignalTarget* pSignalTarget );

		dclspc std::pair<Length,bool> Move( Length dParam, Orientation principalDirection, const Event* pEvent, SignalTarget* pSignalTarget );
		///@}


		/// \name MoveToEnd
		/// \brief Moves this Location to the specified end of the track without transitioning to the next.
		///
		/// \param direction para moves to the tracks end in Track's direction, anti against it.
		///	\param pEvent Trigger sensors and dispatch Event.
		///	\param pSignalTarget Receive signals.
		/// \returns The parameter length the location moved.		
		/// \throws std::exception if the move could not get performed.
		dclspc Length MoveToEnd( Orientation direction = Orientation::Value::para, const Event* pEvent = nullptr, SignalTarget* pSignalTarget = nullptr );


		/// \brief Moves the location to the track's end and transitions it
		/// to the coupled track's end if any.
		///
		/// \param direction para moves to the tracks end in Track's direction, anti against it.
		///	\param pEvent Trigger sensors and dispatch Event.
		///	\param pSignalTarget Receive signals.
		/// \returns the distance that the Location was moved and a flag that indicates wether
		/// a track transition actually happened.
		/// \throws std::exception if the move could not get performed.
		dclspc std::pair<Length,bool> MoveTransit( Orientation direction = Orientation::Value::para, const Event* pEvent = nullptr, SignalTarget* pSignalTarget = nullptr );


		/// \brief Turns the direction of this location to the other side.
		dclspc Location& Flip() noexcept;


		/// \brief Compares two Locations.
		///
		/// The comparison will work with Locations on coupled tracks.
		/// \returns true if the two Locations stand for the same locations
		/// along the track system with a difference smaller than epsilon in
		/// parameter difference; false otherwise.
		dclspc bool Equals( const Location& loc, Length epsilon ) const noexcept;


		/// \brief Gets the parameter distance between two locations.
		///
		/// \param loc Location to search for behind this location if maxdistance < 0 or 
		/// in front of it if maxdistance > 0.
		/// \param maxdistance If the distance outgrews maxdistance, maxdistance is returned.
		/// \throws std::logic_error exception if one of the locations is not attached to a track.
		/// \throws std::out_of_range exception if it hits a not connected track end 
		/// before finding loc.
		/// \returns The distance along the curve between this location and loc or maxdistance if
		/// loc was not found.
		/// \throws std::logic_error if any of the locations is not at a valid position.
		/// \throws std::out_of_range if the search ended at an open track end.
		dclspc Length Distance( const Location& loc, Length maxdistance ) const;


		/// \name Reservation mechanism
		/// \see Track
		///@{
		/// \brief Reserves a range on the track relative to this Location.
		/// \throws std::logic_error if the location is not on track.
		/// \throws std::range_error If the range lays completely outside the track.
		dclspc void Reserve( common::Interval<Length> range, IDType forID );


		/// \brief Deletes a reservation.
		/// \throws std::logic_error if the location is not on track.
		dclspc void DeleteReservation( IDType forID = anyID );


		/// \brief Finds overlapping areas for reservations.
		/// \throws std::logic_error if the location is not on track.
		/// \throws std::exception if the overlaps could not get collected.
		dclspc std::vector<Track::Overlap> Overlaps( IDType forID ) const;
		///@}

	private:
		std::shared_ptr<Track>	m_pTrack;
		TrackLocation			m_TLocation;

		// Calculates this track location relative to the coupled track at front or end.
		// This might lead to a parameter out of track ranges.
		// Returns false if the track is uncoupled at the specified end.
		bool TrackTransition( Track::EndType frontend, const Event* pEvent ) noexcept;

		// Transition tracks until parameter is on track.
		bool Resolve() noexcept;

		bool IsResolved() const noexcept;
	};



	inline bool operator==( const Location& locationA, const Location& locationB ) noexcept{
		return locationA.Equals( locationB, epsilon__length );
	}

	inline bool operator!=( const Location& locationA, const Location& locationB ) noexcept{
		return !locationA.Equals( locationB, epsilon__length );
	}


	/// \brief Calculate parameter for given 3D distance.
	/// \param location  starting location.
	/// \param distanceIn3D target distance in space between the two locations on track. Negative
	/// values would return negative result values.
	/// \param maxParameter maximum parameter to return. Specify a reasonable value to prevent
	/// infinite recursion in cases no result can be found.
	/// \param _epsilon accuracy to yield for the result.
	/// \returns The parameter, the location has to be moved, to yield a certain 3D distance or maxParameter if no solution was found.
	dclspc Length ParameterDistanceFrom3DDistance( const Location& location, Length distanceIn3D, Length maxParameter, Length _epsilon = epsilon__length );


	/// \brief Gets the end of a line.
	///
	/// A line is a continuous passable path on a track system. This might be a section 
	/// from a switch or dead end to another switch or dead end (bDeadConnectorOnly = false) or 
	/// from a dead switch or dead end to another dead switch or dead end (bDeadConnectorOnly = true),
	/// meaning correctly passable switches are included into the line.
	/// Since lines are of potentially unlimited length, a maximum length has to be provided. 
	/// \param location Location to start the search from.
	/// \param maxDistance A maximum distance to search for. Pick the value in a responsible manner.
	/// \param bDeadConnectorOnly If true the search does not stop on a connector that can get passed
	/// propperly.
	/// \returns If an end of the line is found in the range of maxDistance, the track and its end is returned. These
	/// represent the end of the line. With Track::GetConnector, a Connector can be retrieved, if any. If the maxDistance
	/// is exceeded the track pointer would be nullptr and the returned length would equal maxDistance.
	dclspc std::tuple<std::shared_ptr<Track>,Track::EndType,Length> EndOfLine( const Location& location, Length maxDistance, bool bDeadConnectorOnly = false );
}
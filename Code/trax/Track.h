//	trax track library
//	AD 2013 
//
//  "I will love you 'till the end of time"
//
//								Lana Del Rey
/// \file 
/// \brief Home of the Track and TrackBuilder interfaces.
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

/// \page docu_track Tracks
/// \section track_intro Introduction
/// A \link trax::Track Track \endlink is something with two ends - the \link 
/// trax::Track::front front end \endlink the \link trax::Track::end end end 
/// \endlink - which can get connected with the ends of other tracks; it can 
/// provide geometry, by taking a Curve as a path in 3D space and a \link 
/// trax::RoadwayTwist RoadwayTwist \endlink for determining the up direction
/// of the path; there is a \link trax::Location Location \endlink that can 
/// travel the tracks, including transitioning between tracks if they are 
/// coupled and deliver 3D poses by its \ref location_getters getter methods -
/// \link trax::Connector Connectors \endlink are taking track ends and help 
/// to dynamically reconnect them according to their special patterns.
///
/// \image html TrackCurve.jpg
///
/// Trax' tracks provide two interfaces: the Track interface for using a track, e.g. \ref transition
/// the track, to get a 3D location for a track parameter value; and the TrackBuilder interface, for 
/// creating track geometry by attaching curves and twists.
///
/// \image html CurvesInterfaces.png
///
/// \section track_details Details
/// The overall pose of a track is governed by its \link trax::TrackBuilder::GetFrame Frame \endlink.  
/// It specifies the transformation between a Curve and the 3d space local to an \ref absolute_frame ,
/// specified by a TrackCollection. If the Track is not a member of a TrackCollection, the frame will 
/// be the global pose. To set a desired pose for the track, a \link trax::TrackBuilder::SetFrame SetFrame \endlink 
/// method can be used. This way any position along the track can get aligned 
/// to a global (or TrackCollection - local) pose.
///
/// All Curve's parameter values are counted in arc length (named s and typed trax::Length, if so),
/// meaning that the parameter s2 - s1 is measuring the geometrical length of the Curve between
/// any two valid values s2, s1 along the Curve. If attached to a track, a Curve has to be augmented 
/// with a parameter range that gets mapped to the track's 0_m to length parameter range. This can 
/// be any parameter range in the Curve's valid range; note that it also can be reverted.
///
/// Curves can be shared between multiple tracks. Twists are unique to their tracks.
///
/// There are several trax::Strech - functions to help with solving certain geometrical problems, by 
/// finding a most simple Curve, that does the trick and attaching it directly to the track.
/// 
/// \section track_examples Examples:
/// 
/// To properly create a track attach a curve to it:
/// \code
///	auto pTrack = TrackBuilder::Make();
///	std::shared_ptr<RotatorChain> pCurve = RotatorChain::Make();
///	pCurve->Create( RotatorChain::Data{ 
///		{ 20_deg, 10_deg, 90_m }, 
///		{ 20_deg, -10_deg, 60_m }, 
///		{ -40_deg, 0_deg, 50_m } } );
///	pTrack->Attach( pCurve, { 0_m, 90_m+60_m+50_m} );
/// BOOST_CHECK( pTrack->IsValid() );
/// \endcode
/// 
/// To set a track's starting position to a special place and orientation in 3D space:
/// \code
///	spat::Frame<Length,One> myPose{ {100_m,200_m,300_m},
///									{0,1,0},
///									{0,0,1},
///									{1,0,0} };
///	pTrack->SetFrame( myPose, 0_m );
/// \endcode
///
/// To set a track's middle position to a special place and orientation in 3D space:
/// \code
/// pTrack->SetFrame( myPose, pTrack->Range().Center() );
/// \endcode
///
/// To Detach a Curve:
///
/// \code
/// auto curve = pTrack->DetachCurve();
///	// do something with the curve ...
///	pTrack->Attach( curve );
/// \endcode
///
/// To store geometry and pose for later use:
///
/// \code
/// spat::Frame<Length,One> frame = pTrack->GetFrame();
///	auto curve = pTrack->GetCurve();
///	auto pTwist = pTrack->GetTwist().Clone();
///	// something else is happening to the track ...
///	// ...
///	// For some reason restore the track geometry:
///	pTrack->Attach( std::move(pTwist) );
///	pTrack->Attach( curve );
///	pTrack->SetFrame( frame );
/// \endcode
///
/// To make the track connect two points and maintaining specific tangents at those:
///
/// \code
///	spat::VectorBundle<Length,One> start{ {10_m, 10_m, 10_m}, {250,0,0} };
///	spat::VectorBundle<Length,One> end{ {10_m,70_m,10_m}, {250,0,0} };
///	Strech( *pTrack, start, end );
/// BOOST_CHECK( pTrack->IsValid() );
/// \endcode
///
/// Do also look at the tests in the test suit for further examples.

#include "IDType.h"
#include "Units.h"
#include "Identified.h"
#include "Orientation.h"
#include "RoadwayTwist.h"

#include "common/Interval.h"
#include "spat/Box.h"
#include "spat/Frame.h"
#include "spat/Vector2D.h"
#include "spat/VectorBundle.h"
#include "spat/VectorBundle2.h"

namespace trax
{
	struct Body;
	struct Connector;
	struct Curve;
	struct Event;
	struct MovableTrack;
	struct ParallelizableTrack;
	struct RoadwayTwist;
	struct Sensor;
	struct Signal;
	struct SignalTarget;
	template<typename> struct TrackData;
	struct TrackBuilder;
	struct TrackLocation;
	struct TrackRange;

	/// \brief Data to be defined by the user.
	struct TrackUserData;


	///	\brief Interface for a track used to calculate 3D positions.
	///
	/// Trains use this kind of interfaces to calculate their positions in 3D
	struct Track : Identified<Track>
	{
		/// \returns the name for the object type that implements this interface. 
		virtual const char*	TypeName() const noexcept = 0;

		enum class TrackType : char{
			unknown = -1,
			none  = 0,
			standard,
			withGeoms,
			movable,
			movable_autoconnecting,
			parallel
		};

		virtual TrackType GetTrackType() const noexcept = 0;

		/// \brief Designates one of the two ends of a track.
		enum class EndType : char{
			none	= -1,
			front	= 0,	///< Designates the front end.
			end		= 1,	///< Designates the far end.
			any				///< Denotes the north or south end of a Railrunner.
		};

		typedef std::pair<std::shared_ptr<Track>,EndType> TrackEnd;
		typedef std::pair<std::shared_ptr<const Track>,const EndType> cTrackEnd;

		/// \brief Designates an end at a specific track.
		struct End{
			End() noexcept = default;
			End( IDType	id, EndType	type ) noexcept
			: id	{ id },
			  type	{ type }
			{}

			End( const TrackEnd& trackEnd ) noexcept
			: id	{ trackEnd.first ? trackEnd.first->ID() : IDType{0u} },
			  type	{ trackEnd.second }
			{}

			End( const std::pair<std::shared_ptr<TrackBuilder>,Track::EndType>& trackEnd ) noexcept;

			IDType	id;						///< Identifier of the track.
			EndType	type = EndType::none;	///< front or end end of the track.
		};


		/// \brief Designates two track ends. Typically used for coupling relations.
		///
		/// An End with zero id will mean an uncoupled end of the other track.
		struct Coupling{
			End theOne;		///< First end to be coupled.
			End theOther;	///< Second end to be coupled.
		};


		/// \brief Gets a shared pointer to this.
		virtual std::shared_ptr<TrackBuilder> This() const noexcept = 0;


		/// \returns A shared pointer to the movable track interface or nullptr, if none.
		virtual std::shared_ptr<MovableTrack> GetMovableTrack() const noexcept = 0;


		/// \returns A shared pointer to the parallel track interface or nullptr, if none.
		virtual std::shared_ptr<ParallelizableTrack> GetParallelizableTrack() const noexcept = 0;


		/// \returns Returns the body a track is assigned to or nullptr.
		virtual std::shared_ptr<const Body> GetBody() const noexcept = 0;


		/// Tests wether a valid curve and twist are attached.
		/// \returns true if the track is able to deliver valid geometry.
		virtual bool IsValid() const noexcept = 0;


		///	\returns the maximum valid parameter value of the track.
		virtual Length GetLength() const noexcept = 0;


		/// \returns the track's parameter range [0,Length()[
		virtual common::Interval<Length> Range() const noexcept = 0;


		/// \returns true if the respective end is coupled to another track end.
		virtual bool IsCoupled( EndType atend = EndType::any ) const noexcept = 0;


		/// \param s Parameter value 0 <= s <= Length()
		/// \returns The geometrical curvature at parameter value s.
		/// \throws std::logic_error if no proper curve is attached with the track.
		/// \throws std::range_error if s is outside the [0,Length()] range.
		virtual AnglePerLength Curvature( Length s ) const = 0;


		/// \param s Parameter value 0 <= s <= Length()
		/// \returns The geometrical torsion including the first derivative of the twist 
		/// (making up the total geometrical torsion) at parameter value s.
		/// \throws std::logic_error if no proper curve is attached with trhe track.
		/// \throws std::range_error if s is outside the [0,Length()] range.
		virtual AnglePerLength Torsion( Length s ) const = 0;


		/// \returns The twist value at s that gets applied by the attached RoadwayTwist.
		virtual Angle GetTwistValue( Length s ) const = 0;


		/// \returns The first derivative of the twist function at s that 
		/// gets applied by the attached RoadwayTwist.
		virtual AnglePerLength GetTwistD1( Length s ) const = 0;


		/// \returns true if it is guaranteed, that Torsion() will return 0_1Im for every s.
		virtual bool IsFlat() const noexcept = 0;


		/// \brief Gives the Curve's idiosyncratic up direction transformed by the track's frame.
		/// Some curves maintain some idea about where they have their upside, either
		/// because of their form (e.g Helix) or because it is extra defined (e.g. for Line).
		/// Some curves maintain no such notion (e.g. Cubic).
		/// \throws std::runtime_error if the Curve has no notion of up.
		/// \returns The Curve's up direction.
		virtual spat::Vector<One> LocalUp() const = 0;


		/// \brief Receives the TNB - Frame of the curve at the location.
		///
		///	The Frame received by the Transition() methods is aligned to Locations orientation 
		/// on the tracks and a twisting of the line. For dynamic calculations the
		/// real geometry of the curve is needed.
		/// \throws std::logic_error if no proper curve is attached with the track.
		/// \throws std::range_error if s is outside the [0,Length()] range.
		virtual void TNBFrame( Length s, spat::Frame<Length,One>& frame ) const = 0;


		/// \anchor transition
		/// \name Transition to Space
		/// \brief Gives geometrical data for a specific location along the track. 
		/// 
		/// The coordinates are relative to the global frame and take into accout 
		/// all the Frame offset and the twist of the track.
		/// \param s Length along the track counted from front to get the spatial data for.
		/// The minimum valid value is 0, the maximum value is given by Length() otherwise throws an
		/// error.
		/// \param pos Position on the track at s.
		/// \param tan Tangent along the track at s.
		/// \param bundle Position and vectors along the track at s.
		/// \param frame TNB - frame along the track at s.
		/// \param td The full track data set in engine units. Used internally by trackjoints and trackfeeders.
		/// \throws std::logic_error if no proper curve is attached with the track.
		/// \throws std::range_error if s is outside the [0,Length()] range.
		///@{

		/// \brief Gives information about the pose on the track at the specified parameter value.
		virtual void Transition( Length s, spat::Position<Length>& pos ) const = 0;

		virtual void Transition( Length s, spat::Vector<One>& tan ) const = 0;

		virtual void Transition( Length s, spat::VectorBundle<Length,One>& bundle ) const = 0;

		virtual void Transition( Length s, spat::VectorBundle2<Length,One>& bundle ) const = 0;

		virtual void Transition( Length s, spat::Frame<Length,One>& frame ) const = 0;

		virtual void Transition( Length s, TrackData<Real>& td, Real engine_meters_per_unit ) const = 0;
		///@}


		/// \brief Gives the Track coupled to this at the specified end.
		/// \param thisEnd End of this track to get the coupled track for.
		/// \returns A shared pointer to the coupled track or a nullptr if
		/// no track is connected. If not nullptr, othersend will receive 
		/// the type of end of the other track the coupling is existing for.
		virtual TrackEnd TransitionEnd( EndType thisEnd ) const noexcept = 0;


		/// Triggers all sensors in the range in the order of their positioning along the track
		/// according to start and end of the range.
		///
		/// Only the sensors wich are parallel to the range orientation get triggered. Wether
		/// the sensor actually sends a pulse will depend from the match between the Event
		/// settings and the Effect filter of the sensor.
		///	\param range Trackrange to trigger contacts in.
		/// \param _event An Event to match with the sensor's filter settings. 
		virtual void DoTrigger( const common::Interval<Length>& range, const Event& _event ) const = 0;


		/// Receives the notifications in the range along the track.
		/// \param range on the track to receive the notifications for.
		/// \param orientation Direction along the track to receive the signals for.
		/// \param signalTarget Target to receive the notifications.
		/// \returns true if the movement along the track should get calculated, 
		/// false if this is supposed to stop, because the train was
		/// relocated by the Notify function.
		virtual bool DoSignal( const common::Interval<Length>& range, Orientation orientation, SignalTarget& signalTarget ) const = 0;


		/// \brief Gets the first Signal in the direction given by loc.
		///
		/// This transitions track borders.
		/// \param loc The point from wich to search and the direction in wich to search.
		/// \returns A pointer to a Signal if found, nullptr otherwise.
		virtual Signal* GetSignal( const TrackLocation& loc ) const noexcept = 0;


		/// \brief retreives a connector (e.g. a Switch) at the respective track end.
		/// \returns A pointer to the connector or nullptr if none is present.
		virtual Connector* GetConnector( EndType atend ) const noexcept = 0;


		/// \brief retrieves the next connector (e.g. a Switch) that can be found in the
		/// given direction.
		/// \param inDirection direction relative to this track to search for a Connector.
		/// \returns A pointer to a Connector or nullptr if none could be found.
		virtual Connector* GetConnector( const Orientation& inDirection ) const noexcept = 0;


		/// \name Reservation mechanism
		///	\brief Reservations are areas on a track that are marked as reserved for some id. 
		/// They allow to detemine overlappings between different reservations, distinguished 
		/// by IDs.
		///	
		/// A reservation interval's near value will be considered inside the reservation, the far 
		/// value as being outside. 
		///@{

		/// \brief Reserves a parameter range on the track for a specified id.
		///
		/// If the range is outside the track's parameter range, the reservation request 
		/// will be forwarded to the connected tracks according to the active connections,
		/// as determined by switches.
		/// \param range The range to reserve in parameter space. A zero range will count 
		/// as no reservation at all; the reservations carry direction according to their
		/// normalized state. A range completely outside the track will throw an exception.
		/// \param forID An idintifier to be stored with the reservation. If the reservations 
		/// are used for trains, the IDs of trains or Railrunners in a collection would
		/// make a good candidate for the IDs of reservations.
		/// \throws std::range_error If the range lays completely outside the track.
		virtual void Reserve( common::Interval<Length> range, IDType forID ) = 0;

		using Reservation = std::tuple<IDType,common::Interval<Length>>;
		using ReservationIterator = std::vector<Reservation>::const_iterator;

		/// \returns begin iterator to the reservations on the track.
		virtual ReservationIterator BeginReservations() const noexcept = 0;

		/// \returns end iterator to the reservations on the track.
		virtual ReservationIterator EndReservations() const noexcept = 0;

		/// \brief Deletes a reservation with specified ID, if it overlaps with the specified range
		///
		/// If the reservation transitions the track boundaries, the request will be forewarded 
		/// to the connected tracks for that reservation. If a reservation overlaps inRange, it is
		/// deleted completely (not only the intersectional part).
		/// \param inRange Range to test for.
		/// \param forID ID to delete a reservation for. If it is anyID all reservations in range will get deleted.
		virtual void DeleteReservation( common::Interval<Length> inRange, IDType forID = anyID ) noexcept = 0;


		/// \brief Is here a reservation for the specified ID inside the range on the track?
		virtual bool IsReserved( common::Interval<Length> inRange, IDType forID = anyID ) const noexcept = 0;


		/// \brief Overlapping data.
		struct Overlap{
			IDType forID;	///< ID that was tested for.
			bool forNear;	///< true if the for range's near end is overlapped.
			bool forFar;	///< true if the for range's far end is overlapped.
			IDType withID;	///< ID of the reservation that overlaps.
			bool withNear;	///< true if the with range's near end is overlapped.
			bool withFar;	///< true if the with range's far end is overlapped.
		};


		/// \brief Finds overlapping areas for reservations.
		///
		/// A reservation for forID has to exist on the track, otherwise the
		/// result would always be an empty vector. The method will not self-overlap
		/// any reservation, but will deliver overlaps for different reservations
		/// even with the same id.
		/// \throws std::exception if the overlaps could not get collected.
		virtual std::vector<Overlap> Overlaps( IDType forID ) const = 0;
		///@}


		/// \brief Sets a pointer to user provided data.
		///
		/// Define TrackUserData to specify your data types.
		virtual void UserData( TrackUserData* pData ) noexcept = 0;


		/// \returns User provided data.
		virtual TrackUserData* UserData() const noexcept = 0;
	};

	/// \brief Converts a TrackType to readable string.
	dclspc const char* ToString( Track::TrackType type );

	/// \brief Finds TrackType for string.
	dclspc Track::TrackType TrackType( const std::string& type ) noexcept;

	/// \brief Tests for equality.
	inline bool operator==( const Track::End& theone, const Track::End& theother ) noexcept;


	/// \brief Tests for inequality.
	inline bool operator!=( const Track::End& theone, const Track::End& theother ) noexcept;


	/// \brief Tests for equality.
	inline bool operator==( const Track::Coupling& theone, const Track::Coupling& theother ) noexcept;


	/// \brief Tests for inequality.
	inline bool operator!=( const Track::Coupling& theone, const Track::Coupling& theother ) noexcept;


	inline bool operator==( const Track::Overlap& a, const Track::Overlap& b ) noexcept;

	inline bool operator<( const Track::Overlap& a, const Track::Overlap& b ) noexcept;


	/// \brief Makes a string from the status
	dclspc std::string ToString( Track::EndType end );


	/// \brief Makes a status value from a status string.
	/// \throws std::invalid_argument if the string was not recognized.
	dclspc Track::EndType ToEndType( const std::string& end );


	/// \returns The other end.
	inline Track::EndType operator!( Track::EndType end ) noexcept{
		return end == Track::EndType::front ? Track::EndType::end : Track::EndType::front;
	}


	/// \defgroup Group_Segments Track's Segments
	/// \brief Gets a segement length ds, so that the visible failure in a track would become smaller than some
	/// marginal distance of e.
	/// \param track Track to get the segment for.
	/// \param s Parameter to evaluate the situation for [0,length].
	/// \param e Distance failure that is assumed not to matter visibly.
	/// \param segmentLimits The minimum and maximum segment length to deliver. The last segment returned might be 
	/// shorter, to go to the track's end.
	/// \param ignoreCuvesTorsion If true the torsion of the curve is not taken into consideration (but the twist still is). 
	/// \returns ds so that s+ds will not exceed the end of track. ds == 0 therefore shows 
	/// that s == track.Length().
	///
	/// If it comes to rendering, we might want to break down a track into segments that are straight lines. 
	/// This means, we graphically are working with incorrect T,N,B vectors over the length ds of this segment. 
	/// This might result in visible errors that are about:
	/// \verbatim
	/// |dT| * ds	<= e (1)
	/// |dN| * w/2	<= e (2)
	/// |dB| * h/2	<= e (3)
	/// \endverbatim
	/// For small errors the dX = X(s+ds) - X(s) happen to be angles - times a radius it is an error in length. With w and h 
	/// being the dimensions of the track model and ds being the segment length we are looking for.
	/// For a track model, we might want to limit the model's extents, so that w/2,h/2 <= ds. That would mean, we can guarantee
	/// (1)-(3) if we guarantee the following:
	/// \verbatim
	/// |dT| * ds	<= e (4)
	/// |dN| * ds	<= e (5)
	/// |dB| * ds	<= e (6)
	/// \endverbatim
	/// From Frenet https://en.wikipedia.org/wiki/Frenet%E2%80%93Serret_formulas it follows:
	///
	/// \verbatim
	/// (4) => ds <= sqrt(e/k)
	/// (5) => ds <= sqrt(e/sqrt(pow<2>(k)+pow<2>(t)))
	/// (6) => ds <= sqrt(e/|t|)
	///
	/// k: curvature
	/// t: torsion
	/// \endverbatim
	/// (5) Is the most restrictive and is targeted. s+ds will not exceed the end of the track.
	/// The curve is evaluated locally at s; so the ds has to be further limited to reasonably small
	/// values (compared to the changes of curvature and torsion) to guarantee that the condition 
	/// really holds.
	/// \throws std::logic_error if no proper curve is attached with trhe track.
	/// \throws std::range_error if s is outside the [0,Length()] range.
	///@{

	dclspc Length Segment( const Track& track, Length s, Length e, common::Interval<Length> segmentLimits, bool ignoreCuvesTorsion = false );


	/// \brief Works like the first overload, but takes the dimensions of the track model
	/// into account.
	///
	/// In the above derivation it would follow:
	///
	/// \verbatim
	/// (4) => ds <= sqrt(e/k)
	/// (5) => ds <= 2*e/(w * sqrt(pow<2>(k)+pow<2>(t)))
	/// (6) => ds <= 2*e/(h*|t|)
	/// \endverbatim
	///
	/// \param w maximum width of the track model. For asymmetric tracks take two times the widest side.
	/// \param h maximum height of the track model. For asymmetric tracks take two times the highest side.
	dclspc Length Segment( const Track& track, Length s, Length e, Length w, Length h, common::Interval<Length> segmentLimits, bool ignoreCuvesTorsion = false );


	/// \brief Works like Segment(), but checks the result and shrinks it if necessary.
	///
	/// This method might still deliver too large segments if the curve has huge changes inside the limits; for
	/// this reason do not choose the upper segment limit too big, since some sudden curve inside such a segment
	/// might get missed.
	dclspc Length Segment_Checked( const Track& track, Length s, Length e, common::Interval<Length> segmentLimits, bool ignoreCuvesTorsion = false );


	/// \brief Works like the first overload, but takes the dimensions of the track model
	/// into account.
	dclspc Length Segment_Checked( const Track& track, Length s, Length e, Length w, Length h, common::Interval<Length> segmentLimits, bool ignoreCuvesTorsion = false );


	/// \brief Works like Segment(), but checks the result.
	///
	/// This method might take a long time to compute, if segmentLimits.Near() is too small, but is guaranteed to not miss any
	/// curves with lengthes greater than segmentLimits.Near().
	dclspc Length Segment_TotallyChecked( const Track& track, Length s, Length e, common::Interval<Length> segmentLimits, bool ignoreCuvesTorsion = false );


	/// \brief Works like the first overload, but takes the dimensions of the track model.
	/// into account.
	dclspc Length Segment_TotallyChecked( const Track& track, Length s, Length e, Length w, Length h, common::Interval<Length> segmentLimits, bool ignoreCuvesTorsion = false );
	///@}


	/// \defgroup Group_Foot Track's Foot Point 
	/// \brief Foot of a point in space on a track. Finds a point on a track so that the line from it to the target point stands 
	/// perpendicular on the track's tangent at that point.
	///
	/// Feets are also locally shortest distanced to the reference point, i.e. the neighbouring points of
	/// the curve would be of greater distance to the point. For complex curves the function might not find the
	/// foot with the globally shortest distance.
	/// 
	/// \param ofPoint The point to find the foot of the perpendicular for.
	/// \param ofRay A ray to find the foot of the perpendicular for. 
	/// The perpendicular will also be prependicular to the ray.
	/// \param toTrack The track to find the foot on.
	/// \param bestGuess A value of s that might be close to the desired solution.
	/// \param inRange A range on the track to limit the search to.
	/// \throws std::invalid_argument if bestGuess is not inside the tracks range or not inside inRange.
	/// \throws std::runtime_error if the track is not properly built.
	/// \returns A pair whiches first element is true if a solution was found. The second element then is
	/// the track parameter s for the foot point in question.
	/// \throws std::runtime_error if the track is not valid.
	/// \throws std::invalid_argument if bestGuess is out of the valid range.
	///@{
	
	/// \brief Foot for point in space.
	dclspc std::pair<bool,Length> Foot( const spat::Position<Length>& ofPoint, const Track& toTrack );

	dclspc std::pair<bool,Length> Foot( const spat::Position<Length>& ofPoint, const Track& toTrack, Length bestGuess );

	dclspc std::pair<bool,Length> Foot( const spat::Position<Length>& ofPoint, const Track& toTrack, Length bestGuess, common::Interval<Length> inRange );

	/// \brief Foot for a ray in space.
	dclspc std::pair<bool,Length> Foot( const spat::VectorBundle<Length,One>& ofRay, const Track& toTrack );

	dclspc std::pair<bool,Length> Foot( const spat::VectorBundle<Length,One>& ofRay, const Track& toTrack, Length bestGuess );

	dclspc std::pair<bool,Length> Foot( const spat::VectorBundle<Length,One>& ofRay, const Track& toTrack, Length bestGuess, common::Interval<Length> inRange );
	///@}


	/// \defgroup Group_Closest Track's Closest Point
	///
	/// \brief Finds a closest point on a track. 
	/// If there are more than three foot points on the 
	/// track to the point in question, the point found might not be the globally closest 
	/// point on the track, but might be one
	/// locally closest. 
	/// \param toPoint target point in 3D space.
	/// \param toRay target ray in 3D space.
	/// \param onTrack track to look for a close position on.
	/// \param includeEndpoints If true, the endpoints of the track are included in the evaluation, even if they are
	/// no foot points.
	/// \returns Parameter value of the point on the track that was found to be closest. If no result was found 
	/// (only if !includeEndpoints) the return value would be < 0_m.
	/// \throws std::runtime_error if the track is not valid.
	///@{
	
	/// \brief Closest point on a track to a point in space.
	dclspc Length Closest( const spat::Position<Length>& toPoint, const Track& onTrack, bool includeEndpoints = true );

	/// \brief Closest point on a track to a ray in space.
	dclspc Length Closest( const spat::VectorBundle<Length,One>& toRay, const Track& onTrack, bool includeEndpoints = true );
	///@}


	/// \brief Couple this trackA at endA with trackB at endB.
	///
	/// Couples two tracks at the specified ends together so that a Location transitions
	/// between them respectively. There is exactly one coupling per end; use Points to
	/// reconnect tracks and by that build switches. 
	/// There is no need for the specified track ends to be adjacent in a spatial sense, but
	/// with physics engines in general trains will derail if the gap becomes too large.
	/// Be aware that tracks coupled to each other will hold cyclic strong references, so 
	/// they will produce a memory leak if not uncoupled. For this reason make tracks members
	/// of a TrackSystem.
	/// \param trackA shared pointer and track end.
	/// \param trackB track and end trackA is to be coupled with.
	/// \throws std::invalid_argument If the end type is not recocnised.
	/// \throws std::logic_error If the track end could not coupled, e.g. because of self coupling.
	dclspc void Couple( std::pair<std::shared_ptr<Track>,Track::EndType> trackA, std::pair<std::shared_ptr<Track>,Track::EndType> trackB );


	/// \returns the 3D distance between two track ends. If 
	/// the distance can not be calculated, +infinite_length is returned.
	/// \throws std::invalid_argument if any of the track ends is invalid.
	dclspc Length DistanceOf( Track::cTrackEnd trackEndA, Track::cTrackEnd trackEndB );


	/// \returns The 3D distance of the coupled track, if any. 
	/// \throws std::invalid_argument if any of the track ends is invalid.
	/// \throws std::logic_error if the track end is not coupled.
	dclspc Length DistanceToCoupled( const Track& track, Track::EndType atEnd );


	/// \returns The angle of the tangent of the coupled track, if any. 
	/// \throws std::invalid_argument if any of the track ends is invalid.
	/// \throws std::logic_error if the track end is not coupled.
	dclspc Angle KinkToCoupled( const Track& track, Track::EndType atEnd );


	/// \returns The angle of the binormal of the coupled track, if any. 
	/// \throws std::invalid_argument if any of the track ends is invalid.
	/// \throws std::logic_error if the track end is not coupled.
	dclspc Angle TwistToCoupled( const Track& track, Track::EndType atEnd );



	///	\brief Interface for making and shaping tracks.
	struct TrackBuilder : Track
	{
		/// \brief Makes a standard Track object.
		static dclspc std::shared_ptr<TrackBuilder> Make( TrackType type = TrackType::standard ) noexcept;


		/// \name Set Frame
		/// \brief Sets the Frame the geometry is transformed with.
		///
		///	This frame can be used to move a track in space without changing its 
		/// Curve geometry. It defines its position and orientation relative to the 
		/// parent TrackCollection.
		///@{
		
		/// \brief Sets the frame directly.
		virtual void SetFrame( const spat::Frame<Length,One>& frame ) noexcept = 0;


		/// \brief Sets the frame so that Transition(s,start) will give start.
		///
		/// The transformation of the track (accessed by GetFrame()) is reverse calculated in a way that the
		/// track at parameter s will return the pose that is given by start.
		/// If the twist is dynamic, a logical contradiction might occure, since the
		/// dynamic twist might not allow for a solution. If possible these cases are resolved
		/// by offsetting the twist. If not possible, a std::logic_error exception is
		/// thrown.
		/// Note that this method is computational demanding. For animations or moving several tracks at 
		/// once the  SetFrame( const spat::Frame<Length,One>& ) method might be better suited. If you
		/// want to animate movable tracks, consider using MovableTrack (see below) together with a Body.
		/// \param start The frame to set the track to at s.
		/// \param s The parameter value on the track to set the frame for. The minimum valid value is 0, 
		/// the maximum value is given by Length() otherwise throws an exception.
		/// \param bAllowTwistOffset Allow twist correction on contradiction.
		/// \throws std::runtime_error if the track is not valid.
		/// \throws std::range_error if s is outside the [0,Length()] range.
		/// \throws std::logic_error if for geometrical reasons, the method can not work successfully.
		virtual void SetFrame( const spat::Frame<Length,One>& start, Length s, bool bAllowTwistOffset = true ) = 0;
		///@}


		/// \brief Gets the pose of the track relative to its parent TrackCollection.
		virtual const spat::Frame<Length,One>& GetFrame() const noexcept = 0;


		/// \anchor absolute_frame
		/// \brief Gets the frame relative to the Module, this track 
		/// is attached to. I.e. the TrackCollection's transformation.
		virtual const spat::Frame<Length,One>& GetAbsoluteFrame() const noexcept = 0;


		/// \brief Couple this track at thisend with pOtherTrack at othersend.
		///
		/// Couples two tracks at the specified ends together so that a Location transitions
		/// between them respectively. There is exactly one coupling per end; use Points to
		/// reconnect tracks and by that build switches. Existing connections are overriden.
		/// There is no need for the specified track ends to be adjacent in a spatial sense, but
		/// with physics engines in general trains will derail if the gap becomes too large.
		/// Be aware that tracks coupled to each other will hold cyclic strong references, so 
		/// they will produce a memory leak if not uncoupled. For this reason make tracks members
		/// of a TrackSystem.
		/// \param thisEnd Shared pointer to this track and end of this track to be coupled. This is not derivable from this, 
		/// so it has to be supplied with the function call...
		/// \param othersEnd track and end this one is to be coupled with.
		virtual void Couple( std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> thisEnd, std::pair<std::shared_ptr<TrackBuilder>,Track::EndType> othersEnd ) = 0;


		/// \brief Remove coupling from this end.
		/// \param thisEnd What track end to decouple.
		/// \param oneSided If true only the coupling of this track to another will be removed,
		/// the other track stays connected to this.
		virtual void DeCouple( EndType thisEnd = EndType::any, bool oneSided = false ) = 0;


		/// \name Attach a Curve
		///	\brief Sets the Curve object used by this track to
		///	define the geometry of the track.
		///
		/// If the curve provides a maximum length, the track's length will 
		/// get truncated to it.
		///	\param pCurve Pointer to the Curve object to use.
		/// \param curveSegment parameter range of the curve to use from s==0 at curveSegment.Near() 
		/// \param curve A piece of curve to attach.
		/// up to s == Length() at curveSegment.Far(). The curve must support this parameter range, 
		/// otherwise the range gets truncated.
		/// \throws std::invalid_argument if curve is not valid or range.Near() is infinite length.
		///@{
		
		/// \brief Attaches a Curve to the track.
		virtual void Attach( std::shared_ptr<const Curve> pCurve, common::Interval<Length> curveSegment = {0_m,+infinite__length} ) = 0;

		virtual void Attach( std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> curve ) = 0;

		virtual void Attach( std::pair<std::unique_ptr<const Curve>, common::Interval<Length>>& curve ) = 0;


		/// \brief Detaches the Curve from the track.
		/// \returns The curve's parameter segment and a pointer to the previous attached Curve or nullptr if none.
		virtual std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> DetachCurve() noexcept = 0;


		/// \returns a pointer to the curve or nullptr if none; and the Parameter range on the curve to be mapped to the track's 0 to GetLength() range.
		virtual std::pair<std::shared_ptr<const Curve>,common::Interval<Length>> GetCurve() const noexcept = 0;
		///@}


		/// \name Attach a Twist
		///	\brief Sets the RoadwayTwist object used by this track to
		///	define the up orientation of a track. If no twist is set,
		///	the default constant zero twist will be used.
		///@{
		
		///	\param pTwist Pointer to the RoadwayTwist object to use.
		virtual void Attach( std::unique_ptr<RoadwayTwist> pTwist ) = 0;


		/// \brief Detaches the current Twist from the track.
		///
		/// There always will be a default Twist with a track. On 
		/// detachment the default will be substituted. It is a constant 
		/// zero twist.
		/// \returns A pointer to the previous active Twist.This might be the 
		/// default Twist.
		virtual std::unique_ptr<RoadwayTwist> DetachTwist() = 0;


		/// \returns A reference to the attached Twist. This might be the 
		/// default Twist.
		virtual RoadwayTwist& GetTwist() const noexcept = 0;
		///@}


		/// \name Attach a Sensor
		///@{
		
		/// \brief Attaches a Sensor at the specified location to this track.
		/// If already attached, the Sensor will be detached from its previous location.
		/// \param pSensor Pointer to Sensor to be attached.
		/// \param tracklocation Parameter and orientation to attach at.
		virtual void Attach( std::shared_ptr<Sensor> pSensor, const TrackLocation& tracklocation ) = 0;


		/// \brief Gets information about an attached Sensor.
		virtual	bool Attached( const Sensor& sensor, TrackLocation* pTrackLocation = nullptr ) const = 0;


		/// \brief Detaches a Sensor from the track.
		virtual void Detach( const Sensor& sensor ) noexcept = 0;


		/// \returns the Sensor with the specified index.
		virtual std::shared_ptr<Sensor> GetSensor( int idx ) const noexcept = 0;


		/// \returns The number of sensors attached to the track.
		virtual int CountSensors() const = 0;
		///@}


		/// \name Attach a Signal
		/// \brief Attaches a Signal to the specified range to this track.
		///
		/// A Signal is influencing a train while in a certain range on the track
		/// if its MovingDirection() is in start-end orientation of the TrackRange.
		/// If the TrackRange goes over the ends of this track, the signal will
		/// also get registered with the adjacent tracks, respecting the switch settings.
		/// A Train will receive Signals if ...
		/// \param pSignal Pointer to Signal to be attached.
		/// \param trackRange Range the Signal should influence trains in.
		/// \param TrackLocation location of a Signal with a zero length track range.
		/// \throws std::invalid_argument if an invalid Signal was provided.
		///@{
		
		/// \brief Attaches a Signal to the specified range to this track.
		virtual void Attach( std::shared_ptr<Signal> pSignal, const common::Interval<Length>& trackRange ) = 0;

		/// \brief Attaches a Signal to the specified location to this track.
		virtual void Attach( std::shared_ptr<Signal> pSignal, const TrackLocation& location ) = 0;


		/// \brief Gets information about an attached Signal.
		virtual bool Attached( const Signal& signal, TrackRange* pTrackRange = nullptr ) const = 0;


		/// \brief Detaches a Signal from the track and all adjacent tracks
		virtual void Detach( const Signal& signal ) = 0;
		///@}


		/// \brief Exchanges start and end of the track, but keeps geometry. 
		///
		/// The track will run along the same path, but start and end will
		/// be swapped after this operation. The couplings get updated accordingly.
		/// The binormal direction of the track is kept if a capable twist is 
		/// present; the normal direction then will flip.
		/// 
		/// \param flipAttached If true, Sensors and Signals will keep their 
		/// geometrical location along the track, thereby changing their parameter 
		/// positions and orientations.
		virtual void Flip( bool flipAttached = false ) = 0;


		/// \brief Creates an internal Signal at the end of the track, so that
		/// a SignalTarget receives a message if it enters the track.
		/// \param atend The end at wich to create the Signal.
		virtual void CreateEndTransitionSignal( EndType atend ) = 0;


		/// \brief Destroys the internal transition Signal.
		/// \param atend The end at wich to destroy the Signal.
		virtual void DestroyEndTransitionSignal( EndType atend ) = 0;
	};


	/// \brief Same as track.SetFrame( start, s ), but does not throw.
	/// \returns true on success, false if the frame could not be set.
	bool dclspc SetFrame( TrackBuilder& track, const spat::Frame<Length,One>& start, Length s ) noexcept;


	/// \brief Creates a matching Curve, Twist and Frame from an EEPCurve.
	///
	/// If the track has a EEPCurve it gets substituted by closest matching non EEPCurve.
	/// If the track has not an EEPCurve the function does nothing.
	/// \throws std::logic_error if no conversion was possible.
	void dclspc TransformToNonEEPCurve( TrackBuilder& track );


	/// \name Equip a track by a streched curve.
	/// \brief Creates a most simple curve for the track, so that the endpoints get connected.
	///
	/// A new curve will get created and applied to the track, to make the geometry of the track fit the demand. The
	/// different overloads differ in what values of the track ends will be preserved. The methods give the strong guarantee,
	/// i.e. they will either succeed or keep the curve of the track untouched.
	/// \param track Track to change the curve for.
	/// \param start starting position and tangent of the track; length of tangent varies the curve by an overshoot value.
	/// \param bKeepStartTangent Preserve the start tangent. In this case the cubic overshoot values have to
	/// be provided, because a Cubic might get created.
	/// \param cubicOvershootStart cubic overshoot value at starting point.
	/// \param cubicOvershootEnd cubic overshoot value at ending point.
	/// \param end ending position and tangent of the track; length of tangent varies the curve by an overshoot value.
	/// \param bKeepEndTangent Preserve the end tangent. In this case the cubic overshoot values have to
	/// \param length_limits Specifies a lower and an upper limit for the track length. If exceeded, the track remains unchanged.
	/// \param overshoot_limits Specifies a lower and an upper limit for the cubic overshoots at the ends of the track. 
	/// If exceeded, the track remains unchanged.
	/// \param up Vector pointing in up direction. Must be of unit length.
	/// \param e_length error margin for the positional values. 
	/// \param e_angle error margin for the directional values.
	/// be provided, because a Cubic might get created.
	/// 
	/// \returns true if the track was successfully modified, false if streching within the limits for length, overshoot, positional 
	/// and angular epsilon was not possible.
	/// \throws std::invalid_argument if no curve could get created, e.g. because start and end are collocated.
	///@{
	/// Will create a Line, an Arc, a Helix or a Cubic.
	bool dclspc Strech( TrackBuilder& track, 
		spat::VectorBundle<Length,One> start, spat::VectorBundle<Length,One> end,
		common::Interval<Length> length_limits = {0_m,+infinite__length},
		common::Interval<Length> overshoot_limits = { 0_m, +infinite__length },
		const spat::Vector<One>& up = Up, 
		Length e_length = epsilon__length, Angle e_angle = epsilon__angle );
	
	/// Keeps the end position and the direction of the end tangent perpendicular to the Up direction.
	/// Will create a Line, an Arc or a Helix.
	bool dclspc StrechStart( TrackBuilder& track, 
		spat::Position<Length> start,
		common::Interval<Length> length_limits = {0_m,+infinite__length},
		const spat::Vector<One>& up = Up, Length e_length = epsilon__length );
	
	/// Keeps the end position and - if bKeepStartTangent is set true - the start tangent.
	/// Will create a Line, an Arc, a Helix or - if bKeepStartTangent is set true - a Cubic.
	bool dclspc StrechStart( TrackBuilder& track, 
		spat::Position<Length> start, bool bKeepStartTangent, Length cubicOvershootStart, Length cubicOvershootEnd,
		common::Interval<Length> length_limits = {0_m,+infinite__length},
		common::Interval<Length> overshoot_limits = { 0_m, +infinite__length },
		const spat::Vector<One>& up = Up, Length e_length = epsilon__length, Angle e_angle = epsilon__angle );
	
	/// Keeps the end position and tangent
	/// Will create a Line, an Arc, a Helix or a Cubic.
	bool dclspc StrechStart( TrackBuilder& track, 
		spat::VectorBundle<Length,One> start, Length cubicOvershootEnd,
		common::Interval<Length> length_limits = {0_m,+infinite__length},
		common::Interval<Length> overshoot_limits = { 0_m, +infinite__length },
		const spat::Vector<One>& up = Up, Length e_length = epsilon__length, Angle e_angle = epsilon__angle );
	
	/// Keeps the start position and the direction of the start tangent perpendicular to the Up direction.
	/// Will create a Line, an Arc or a Helix.
	bool dclspc StrechEnd( TrackBuilder& track, 
		spat::Position<Length> end,
		common::Interval<Length> length_limits = {0_m,+infinite__length},
		const spat::Vector<One>& up = Up, Length e_length = epsilon__length );
	
	/// Keeps the start position and - if bKeepEndTangent is set true - the end tangent.
	/// Will create a Line, an Arc, a Helix or - if bKeepEndTangent is set true - a Cubic.
	bool dclspc StrechEnd( TrackBuilder& track, 
		spat::Position<Length> end, bool bKeepEndTangent, Length cubicOvershootStart, Length cubicOvershootEnd,
		common::Interval<Length> length_limits = {0_m,+infinite__length},
		common::Interval<Length> overshoot_limits = { 0_m, +infinite__length },
		const spat::Vector<One>& up = Up, Length e_length = epsilon__length, Angle e_angle = epsilon__angle );
	
	/// Keeps the start position and tangent
	/// Will create a Line, an Arc, a Helix or a Cubic.
	bool dclspc StrechEnd( TrackBuilder& track, 
		spat::VectorBundle<Length,One> end, Length cubicOvershootStart,
		common::Interval<Length> length_limits = {0_m,+infinite__length},
		common::Interval<Length> overshoot_limits = { 0_m, +infinite__length },
		const spat::Vector<One>& up = Up, Length e_length = epsilon__length, Angle e_angle = epsilon__angle );
	///@}


	/// \brief Mirrors the track's geometry on a plane.
	/// \param track The track to be changed.
	/// \param mirrorPlane A normal vector for the plane and one point on the plane to be used as a mirror.
	/// \param bTest If true the track will not be changed, but the return value would signal,
	/// wether a mirror operation can get performed for real.
	/// \returns true if the track was mirrored correctly, false if for some
	/// reason the track can not get mirrored and therefore stayed untouched.
	bool dclspc Mirror( TrackBuilder& track, 
		const spat::VectorBundle<Length,One> mirrorPlane, bool bTest = false );


	/// \brief Normalizes the attached Curve's projected parameter length
	/// to the 0_m to track.GetLength() range.
	///
	/// The operation keeps the track unchanged, but only recalculates
	/// the curve and its relationship to the track. This will only 
	/// be possible for symmetric curves. If this is not the case,
	/// the track remains unchanged and the method returns false.
	/// \param track The track to get normalized.
	/// \param bTest If true the track will not be changed, but the return value would signal,
	/// wether a normalization operation can get performed for real.
	/// \returns true if the track could get normalized, false if not.
	bool dclspc Normalize( TrackBuilder& track, bool bTest = false );


	/// \brief Some curves maintain a local up direction. It might be pleasing
	/// for the user to have his curve straightended up in the sense
	/// that this direction alignes with what he thinks is being 'up'.
	/// 
	/// Manipulates the track's frame, so that the curves direction shows up
	/// in global coordinates.
	/// \param track The track to get straightened up.
	/// \param up The direction to align the curves up direction with.
	/// \throws std::invalid_argument if the track is not valid.
	/// \throws std::runtime_erro if the curve does not have an up direction.
	void dclspc StraightenUp( TrackBuilder& track, const spat::Vector<One>& up = Up );


	/// \brief Simplifies the track's curve.
	///
	/// For some curves it is possible to pick simpler ones, while keeping the
	/// geometry of the track. This method will exchange the LineP, ArcP, HelixP
	/// curves by the simpler Line, Arc, Helix curves.
	/// \param track The track to simplify.
	/// \returns true if the track was simplified, false if not.
	bool dclspc Simplify( TrackBuilder& track ) noexcept;


	/// \name Copying Tracks
	/// \brief Creates a new track with the parameter range copyRange and
	/// the same geometry as a template track has inside this range.
	///
	/// The copyRange gets clipped to the original track's range. The first
	/// method creates a new track; the second one takes an existing one to
	/// make it the copy. This can be used to change the track type (e.g. to movable or
	/// paralleizable track).
	/// \param originalTrack template track to create the track piece for.
	/// \param copyTrack target track to become the track piece.
	/// \param copyRange Range of the track to copy. Must have an intersection with
	/// the original track's range.
	/// \param cloneCurve If true, the copy track gets its own cloned curve, otherwise
	/// it shares the original curve.
	/// \returns the track copy or nullptr if copying was not possible.
	///@{
	
	/// \brief Creates a new track from a template track.
	//std::shared_ptr<TrackBuilder> dclspc CopyTrack( const TrackBuilder& originalTrack, const Factory& factory, bool cloneCurve = false );

	std::shared_ptr<TrackBuilder> dclspc CopyTrack( const TrackBuilder& originalTrack, TrackBuilder& copyTrack, bool cloneCurve = false );

	std::shared_ptr<TrackBuilder> dclspc CopyTrack( const TrackBuilder& originalTrack, bool cloneCurve = false );

	//std::shared_ptr<TrackBuilder> dclspc CopyTrack( const TrackBuilder& originalTrack, const Factory& factory, common::Interval<Length> copyRange, bool cloneCurve = false );

	std::shared_ptr<TrackBuilder> dclspc CopyTrack( const TrackBuilder& originalTrack, TrackBuilder& copyTrack, common::Interval<Length> copyRange, bool cloneCurve = false );

	std::shared_ptr<TrackBuilder> dclspc CopyTrack( const TrackBuilder& originalTrack, common::Interval<Length> copyRange, bool cloneCurve = false );
	///@}


	/// \name Splitting Tracks
	/// \brief Creates new tracks that follow the same path of a given track.
	///
	/// The tracks will have all the length track.GetLength()/numPieces.
	/// \param track template track to create the track pieces for.
	/// \param numPieces how many new tracks to create.
	/// \param atParameter Parameter value along the track to split at.
	/// \param cloneCurve If true, each track gets its own cloned curve, otherwise
	/// all the tracks share the original curve.
	/// \param factory Factory to be used to create the new tracks.
	/// \returns a list with newly created consecutive tracks.
	///@{
	
	//std::vector<std::shared_ptr<TrackBuilder>> dclspc SplitTrack( const TrackBuilder& track, const Factory& factory, int numPieces = 2, bool cloneCurve = false );
	
	std::vector<std::shared_ptr<TrackBuilder>> dclspc SplitTrack( const TrackBuilder& track, int numPieces = 2, bool cloneCurve = false );

	//std::vector<std::shared_ptr<TrackBuilder>> dclspc SplitTrack( const TrackBuilder& track, const Factory& factory, Length atParameter, bool cloneCurve = false );
	
	std::vector<std::shared_ptr<TrackBuilder>> dclspc SplitTrack( const TrackBuilder& track, Length atParameter, bool cloneCurve = false );
	///@}


	/// \brief Creates a series of new tracks that close the gap between start and end.
	/// 
	/// \param start The start point of the gap including tangent and twisted normal.
	/// \param end The end point of the gap including tangent and twisted normal. 
	/// \param limits The limits for the length of the new tracks.
	/// \param up The up vector to use for the new tracks. If not set, the default Up vector is used.
	/// \param e_length The error margin for the length of the new tracks.
	/// \param e_angle The error margin for the angle of the new tracks.
	/// \param cloneCurve If true, the new tracks will have their own cloned curves, otherwise
	/// a common curve might be shared.
	std::vector<std::shared_ptr<TrackBuilder>> dclspc CloseGap( 
		spat::Frame<Length,One> start, 
		spat::Frame<Length,One> end, 
		common::Interval<Length> limits,
		const spat::Vector<One>& up = Up, 
		Length e_length = epsilon__length, 
		Angle e_angle = epsilon__angle,
		bool cloneCurve = false );


	/// \name Parallel points and tangents to track
	/// \brief Parallel to twisted track.
	/// 
	///	\verbatim
	/// Cp = Fw.P + h * Fw.N + v * Fw.B
	/// D1 = dCp(s)/ds = (1 + k * (v*sin(w) - h*cos(w))) * Fw.T - (t + dw/ds) * (v*Fw.N - h*Fw.B);
	/// \endverbatim
	///
	/// where s is the arc length parameter of the original curve, not of the parallel. See traxBook Appendix C.
	/// \returns A vector bundle, with P being the position on the parallel curve, T being the D1 of the parallel at
	/// that point with T.Length() * ds == Parallel_ds (for small ds) and T/T.Length() being the unit tangent of the parallel.
	/// N gives the up vector of the original track at s.
	///@{

	spat::VectorBundle2<Length,One> dclspc Parallel( const TrackBuilder& originalTrack, Length s, spat::Vector2D<Length> shift );

	/// \brief Parallel with respect to some plane, defined by an up-direction.
	/// 
	/// The parallel relative to the plane that can be gotten by rotating the frame to the Up vector, 
	/// like a directional twist does, and then moving the corresponding point along Fw.N and Up: 
	/// 
	///	\verbatim
	/// Cp = F.P(s) + h * Fw.N(s) + v * Up;
	/// D1 = (1-k*h*cos(w)) * F.T - (dw/ds+t)*h*sin(w) * F.N + (dw/ds+t)*h*cos(w) * F.B;
	/// (dw/ds+t) = k * Up*T/Up*B / (1 + pow<2>(Up*N/Up*B))
	/// \endverbatim
	/// 
	/// \returns A vector bundle, with P being the position on the parallel curve, T being the D1 of the parallel at
	/// that point with T.Length() * ds == Parallel_ds (for small ds) and T/T.Length() being the unit tangent of the parallel.
	/// N gives the up vector of the original track at s.
	spat::VectorBundle2<Length,One> dclspc ParallelToUp( const TrackBuilder& originalTrack, Length s, spat::Vector2D<Length> shift, spat::Vector<One> up = Up );
	///@}


	/// \name Parallel Tracks
	/// \brief Make a parallel track.
	/// \param originalTrack the original track to make a parallel for.
	/// \param range the range on the original track, to make a parallel for.
	/// \param shift the distance of the parallel to the original. If shift == spat::Null2D it 
	/// returns a copy of the original track.
	/// \param bShiftRelativeToTwistOrUp mode of parallelity
	/// \param minPointDistance minimum distance for the polygonal or spline vertices along the 
	/// parallel curve. This might get smaller for the last two segments.
	/// \param maxDeviationLength maximum distance between the true parallel and the polygonal 
	/// approximation.
	/// \param up Plane defining vector for plane oriented parallelity mode.
	/// \param sampleDistanceLimits The minimum and maximum distance between the sample points
	/// \return The parallel track or nullptr, if it couldn't be built.
	/// \throws std::invalid_argument if the original track is not valid.
	/// \throws std::runtime_error if the parallel track could not be built.
	///@{
	/// 
	
	/// \brief Makes a parallel track by trying Line, Arc and Helix first. 
	/// If that does not apply it returns nullptr
	std::shared_ptr<TrackBuilder> dclspc MakeParallelTrack( 
		const TrackBuilder& originalTrack, 
		common::Interval<Length> range, 
		spat::Vector2D<Length> shift = spat::Null2D<trax::Length>, 
		bool bShiftRelativeToTwistOrUp = true, 
		spat::Vector<One> up = Up, 
		Length maxDeviationLength = epsilon__length,
		Angle maxDeviationAngle = epsilon__angle );

	/// \brief Parallel approximation by Polygonal Chain.
	std::shared_ptr<TrackBuilder> dclspc MakeParallelTrackWithPolygonalChain( 
		const TrackBuilder& originalTrack, 
		common::Interval<Length> range, 
		spat::Vector2D<Length> shift = spat::Null2D<trax::Length>, 
		bool bShiftRelativeToTwistOrUp = true, 
		spat::Vector<One> up = Up,
		Length maxDeviationLength = epsilon__length, 
		common::Interval<Length> sampleDistanceLimits = { 1_m, 10_m } );

	/// \brief Parallel approximation by SampledCurve.
	std::shared_ptr<TrackBuilder> dclspc MakeParallelTrackWithSampledCurve( 
		const TrackBuilder& originalTrack, 
		common::Interval<Length> range, 
		spat::Vector2D<Length> shift = spat::Null2D<trax::Length>, 
		bool bShiftRelativeToTwistOrUp = true,
		spat::Vector<One> up = Up,
		Length maxDeviationLength = epsilon__length, 
		Angle maxDeviationAngle = epsilon__angle, 
		common::Interval<Length> sampleDistanceLimits = { epsilon__length, 20_m } );

	/// \brief Parallel approximation by Spline.
	std::shared_ptr<TrackBuilder> dclspc MakeParallelTrackWithSpline(
		const TrackBuilder& originalTrack, 
		common::Interval<Length> range,
		spat::Vector2D<Length> shift,
		bool bShiftRelativeToTwistOrUp,
		spat::Vector<One> up = Up,
		Length maxDeviationLength = epsilon__length,
		common::Interval<Length> sampleDistanceLimits = { 1_m, 1000_m } );
	///@}


///////////////////////////////////////
// inlines:
inline Track::End::End( const std::pair<std::shared_ptr<TrackBuilder>,Track::EndType>& trackEnd ) noexcept
	: id	{ trackEnd.first ? trackEnd.first->ID() : IDType{0u} }
	, type	{ trackEnd.second }
{}

inline bool operator==( const Track::End& theone, const Track::End& theother ) noexcept{
	return theone.id == theother.id && theone.type == theother.type;
}

inline bool operator!=( const Track::End& theone, const Track::End& theother ) noexcept{
	return !operator==( theone, theother );
}

inline bool operator==( const Track::Coupling& a, const Track::Coupling& b ) noexcept{
	return	a.theOne == b.theOne &&
			a.theOther == b.theOther;
}

inline bool operator!=( const Track::Coupling& a, const Track::Coupling& b ) noexcept{
	return !operator==( a, b );
}

inline bool operator==( const Track::Overlap& a, const Track::Overlap& b ) noexcept{
	return	a.forID == b.forID &&
			a.forNear == b.forNear &&
			a.forFar == b.forFar &&
			a.withID == b.withID &&
			a.withNear == b.withNear &&
			a.withFar == b.withFar;
}

inline bool operator<( const Track::Overlap& a, const Track::Overlap& b ) noexcept{
	if( a.forID < b.forID )
		return true;
	if( a.forID == b.forID ){
		if( a.withID < b.withID )
			return true;
		if( a.withID == b.withID ){
			if( a.forNear < b.forNear )
				return true;
			if( a.forNear == b.forNear ){
				if( a.forFar < b.forFar )
					return true;
				if( a.forFar == b.forFar ){
					if( a.withNear < b.withNear )
						return true;
					if( a.withNear == b.withNear )
						if( a.withFar < b.withFar )
							return true;
				}
			}
		}
	}

	return false;
}
//
//inline bool operator==( const TrackBuilder& tbA, const TrackBuilder& tbB ){
//	if( !tbA.GetCurve() && tbB.GetCurve() ||
//		tbA.GetCurve() && !tbB.GetCurve() )
//		return false;
//
//	if( tbA.GetCurve() && !tbA.GetCurve()->Equal( *tbB.GetCurve() ) )
//		return false;
//
//	return	tbA.ID()					== tbB.ID()					&&
//			tbA.Reference( "name" )		== tbB.Reference( "name" )	&&
//			tbA.GetFrame()				== tbB.GetFrame()			&&
//			tbA.GetTwist()->Equal( tbB.GetTwist().get());
//}
//
//inline bool operator!=( const TrackBuilder& tbA, const TrackBuilder& tbB ){
//		return !(tbA == tbB);
//}

}
//	trax track library
//	AD 2014 
//
//  "Twist and Shout!"
//
//				The Beatles
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

#include "common/Interval.h"
#include "spat/Vector.h"

#include <memory>

namespace trax{
/// \page twist_docu Twists
/// \section twist_intro Introduction
/// Since Curves maintain no direct notion of what is to be an up direction (see \ref curve_docu), with the RoadwayTwist 
/// we introduced an additional rotation around the Curve's tangent axis T. This will rotate the TNB frame returned by
/// a track relative to the Curve's TNB frame. E.g., there is a DirectionalTwist that would rotate the frame in a way
/// that the B vector as returned by the track would have the most minimal angle with an DirectionalTwist::Attractor() axis.
/// If the attractor would be the world's up direction, that would bring the track's B vector to be something that looks
/// quite reasonable like an upside for the track. With a CombinedTwist such a DirectionalTwist can be combined with
/// e.g. a LinearTwist, so that the rotations of the latter would count from this up direction.
///
/// \section twist_details Details
/// A track has always a RoadwayTwist, which would be the ConstantTwist with a zero angle; on detaching a twist
/// from the track that default twist gets automatically reattached to it.
///
/// A twist is unique to a track, it can not be shared between tracks.
///
/// If a track is flipped, i.e. front and end are exchanged, a twist might change its charakteristics. E.g. a LinearTwist
/// that runs from 0_deg to 90_deg would revert to go from -90_deg to 0_deg, to keep the geomery of the track.
/// 
/// If on a curve the curvature becomes zero at one point, this might be a location where the Curve's TNB frame flips
/// around by 180_deg all of a sudden, since the main curvature transitions through zero to the other side (one might say 
/// it becomes negative, but curvature is always positive; the N vector flips and with it the B vector). These cases are
/// counterbalanced by the twist by adding a 180_deg rotation at each such point automaticly.
///
/// \section twist_examples Example:
///
/// To make a track's B vector rotate from 0_deg to 7_deg around T with respect to the world's trax::Up during the course of the track:
/// \code
/// Factory& factory = Factory::Instance();
/// TrackBuilder& track = someTrack;
/// auto pCombinedTist = factory.CreateCombinedTwist();
///	pCombinedTist->AttachTwist1( factory.CreateDirectionalTwist(Up) );	
///	pCombinedTist->AttachTwist2( factory.CreateLinearTwist(0_deg,7_deg) );
///
///	track.Attach( std::move(pCombined) );
/// \endcode
///
/// Do also look at the tests in the test suit for further examples.

	struct TrackBuilder;

	/// \brief A RoadwayTwist is used to define the actual rotating angle
	/// around the tangent at a curve to get the correct alignment to the lane.
	struct RoadwayTwist{

		enum class TwistType : char{
			Unknown = -2,
			None = -1,
			Zero = 0,				///< A twist that always applies 0_rad as a twist angle.
			Constant,				///< A twist that applies a constant twist angle at every point along a track.
			Linear,					///< A twist that linearly interpolates the twist angle from front to end of a track.
			Piecewise,				///< A twist that applies different twist angles to segments long the track.
			PiecewiseLinear,		///< A twist that interpolates the angles linearly along the segments.
			PiecewiseCircular,		///< A twist that interpolates the angles in a 2pi interval and uses the shortest angular distance.
			PiecewiseDirectional,	///< A twist that interpolates directions (slerp) and uses the result as directional twist.
			Directional,			///< A twist that rotates the frame's B closest to some attractor vector.
			Positional,				///< A twist that rotates the frame's B to point closest to some point in space.
			Combined,				///< A twist that combines two twists additively.
			Parallel
		};


		/// \brief Makes a RoadwayTwist object.
		static dclspc std::unique_ptr<RoadwayTwist> Make( TwistType type ) noexcept;

		/// \brief Makes a RoadwayTwist object from another twist.
		static dclspc std::unique_ptr<RoadwayTwist> Make( TwistType type, const RoadwayTwist& fromTwist ) noexcept;


		/// \brief Name for the object type that implements this interface.
		virtual const char*	TypeName() const noexcept = 0;


		virtual TwistType GetTwistType() const noexcept = 0;


		/// \brief Make a clone from this object.
		virtual std::unique_ptr<RoadwayTwist> Clone() const = 0;


		/// Tests wether the twist was properly created.
		/// \returns true if the twist is able to deliver valid geometry.
		virtual bool IsValid() const noexcept = 0;


		/// \param s track parameter to get the twist for [0,Track::GetLength()].
		/// \returns Twisting angle in radiants.
		virtual Angle Twist( Length s ) const = 0;


		/// \param s track parameter to get the derivative for [0,Track::GetLength()].
		/// \returns First derivative of twist.
		virtual AnglePerLength D1( Length s ) const = 0;


		/// Activates a rotation of pi around the tangent vector, if
		/// the curve attached to the track would transition a point of 
		/// zero curvature and is flipping its main normal vector around.
		/// This counteracts the sudden turn of tracks up direction.
		/// Several zero curvature points along the curve are handled accordingly.
		/// \param bActive if true, zero flip will be activated, which is the default
		/// if this makes sense for the twist.
		/// \returns The previous state of the flag.
		virtual bool ZeroFlip( bool bActive ) noexcept = 0;


		/// \brief Flips the effect of the twist along the track. 
		/// After calling this function, the Twist() and D1() methods will give
		/// pi - Twist(Track::GetLength() - s) and -D1(Track::GetLength() - s). Calling 
		/// Flip() a second time will nullify the effect.
		virtual void Flip() noexcept = 0;


		/// \brief Multiplies all twist angle by -1.
		///
		/// Note that some Twists like DirectionalTwist will not be able to 
		/// compute such a mirror operation, due to their nature.
		virtual void Mirror() noexcept = 0;


		/// \brief Adds a global offset angle to the Twist.
		///
		/// Note that some Twists like DirectionalTwist will not be able to 
		/// compute such an offset, due to their nature.
		/// \param offset angle to offset all twist values, returned by this twist.
		/// \returns true, if the offset was properly added, false if that could not
		/// be done.
		virtual bool Offset( Angle offset ) noexcept = 0;


		/// \brief A twist is dynamic if it relates to geometry global to the track. 
		///
		/// Directional or positional twists will be dynamic.
		/// \returns true if the twist will be updated on changing the track's pose.
		virtual bool IsDynamic() const noexcept = 0;


		/// \brief Find wether the twist is directional only offsetted by a constant.
		/// 
		/// If this returns true, the relation:
		/// \verbatim
		/// t + dw/ds = k * AF.T/AF.B / (1 + pow<2>(AF.N/AF.B));
		/// \endverbatim
		/// holds.
		/// \returns true if the twist is a directional twist or a constant twist 
		/// or a combinatin of those.
		virtual bool IsDirectionalConstant() const noexcept = 0;


		/// \returns if the twist is constant in fact.
		virtual bool IsConstant() const noexcept = 0;


		/// \brief Freezes the twist, so that dynamic twists
		/// like DirectionalTwist don't update anymore. 
		virtual void Freeze( bool bFreeze = true ) noexcept = 0;


		/// \returns the freeze state of the twist. Non-dynamic twists will always return true.
		virtual bool IsFrozen() const noexcept = 0;


		/// \returns true if the twist is 0 at all s.
		virtual bool IsNull() const noexcept = 0;


		/// \brief Comparison
		///
		/// Two twists of different types are considered not equal.
		/// \returns true if the twists are equal up to the given thresholds.
		/// \param toTwist twist to compare with.
		/// \param epsilon_angle maximum vector rotational deviation.
		virtual bool Equals( const RoadwayTwist& toTwist, Angle epsilon_angle = epsilon__angle ) const noexcept = 0;


		/// \brief Called on attachment to a track.
		/// \param track Track that the Twist is attached to.
		virtual void OnAttach( const TrackBuilder& track ) noexcept = 0;


		/// \brief Called on detachment from a track.
		virtual void OnDetach() noexcept = 0;


		/// \returns The twist without ZeroFlip or addaptions for general flip.
		virtual Angle DoTwist( Length ) const = 0;


		/// \returns The twist without ZeroFlip or addaptions for general flip.
		virtual AnglePerLength DoD1( Length ) const = 0;


		virtual ~RoadwayTwist() = default;
		RoadwayTwist( RoadwayTwist&& ) = delete;
		RoadwayTwist& operator=( const RoadwayTwist& ) = delete;
		RoadwayTwist& operator=( RoadwayTwist&& ) = delete;
	protected:
		RoadwayTwist() = default;
		RoadwayTwist( const RoadwayTwist& ) = default;
	};


	/// \brief Comparison
	///
	/// Two twists of different types are considered not equal.
	/// \returns true if the twists are equal up to the given thresholds.
	/// \param twistA twist to compare.
	/// \param twistB twist to compare with.
	/// \param epsilon_angle maximum vector rotational deviation.
	bool dclspc Equals( 
		const RoadwayTwist& twistA, 
		const RoadwayTwist& twistB, 
		Angle epsilon_angle = epsilon__angle ) noexcept;


	/// \brief Gets the type name of a twist from its type enumerator.
	dclspc const char* TypeToName( RoadwayTwist::TwistType type );


	/// \brief Gets the type enumerator of a twist from its type name.
	dclspc RoadwayTwist::TwistType TwistNameToType( const std::string& name ) noexcept;


	/// \brief A twist that stays constant over the whole track range.
	struct ConstantTwist : RoadwayTwist{

		/// \brief Makes a ConstantTwist object.
		static dclspc std::unique_ptr<ConstantTwist> Make() noexcept;

		/// \brief Makes a ConstantTwist object from another twist.
		static dclspc std::unique_ptr<ConstantTwist> Make( const RoadwayTwist& fromTwist ) noexcept;


		/// \brief Set twist value.
		virtual void TwistValue( Angle twist ) noexcept = 0;


		/// \brief Get twist value.
		virtual Angle TwistValue() const noexcept = 0;

		using Data = Angle;

		virtual const Data& GetData() const noexcept = 0;

		virtual void Create( const Data& data ) noexcept = 0;
	};


	/// \brief A twist that varies linearly between two values.
	struct LinearTwist : RoadwayTwist{

		/// \brief Makes a LinearTwist object.
		static dclspc std::unique_ptr<LinearTwist> Make( Angle from = 0_deg, Angle to = 0_deg ) noexcept;
		
		/// \brief Makes a LinearTwist object from another twist.
		static dclspc std::unique_ptr<LinearTwist> Make( const RoadwayTwist& fromTwist ) noexcept;


		/// \brief Data describing the twist.
		///
		/// This is used for persistence. GetData() receives the data and
		/// there is a Create() method to create the twist from this data 
		/// set without any caculations made that might distort the values
		/// during many read/write cycles.
		using Data = common::Interval<Angle>;


		/// \brief Set starting twist value.
		virtual void From( Angle twist ) noexcept = 0;


		/// \brief Get starting twist value.
		virtual Angle From() const noexcept = 0;


		/// \brief Set ending twist value.
		virtual void To( Angle twist ) noexcept = 0;


		/// \brief Get ending twist value.
		virtual Angle To() const noexcept = 0;


		virtual const Data& GetData() const noexcept = 0;

		virtual void Create( const Data& data ) noexcept = 0;
	};


	/// \brief A twist with values at control points and either none or linear
	/// interpolation for the points in between.
	struct PiecewiseTwist : RoadwayTwist
	{
		/// \brief Makes a PiecewiseTwist object.
		static dclspc std::unique_ptr<PiecewiseTwist> Make( TwistType type = TwistType::Piecewise ) noexcept;
		
		/// \brief Makes a PiecewiseTwist object from another twist.
		static dclspc std::unique_ptr<PiecewiseTwist> Make( TwistType type, const RoadwayTwist& fromTwist ) noexcept;

		using SegmentValueType = std::pair<Length, Angle>;
		using Data = std::vector<SegmentValueType>;

		/// \brief Add a control point to the Twist.
		virtual void Add( Length s, const Angle& twist ) = 0;


		/// \brief Add a control point to the Twist.
		virtual void Add( const std::pair<Length,Angle>& pair ) = 0;


		/// \returns the number of added control points 
		virtual int CntTwistValues() const = 0;


		/// \param idx Zero based index of control point.
		/// \returns A control point.
		virtual std::pair<Length,Angle> Twist( int idx ) const = 0;

		virtual void Create( const Data& data ) = 0;

		virtual const Data& GetData() const noexcept = 0;

		using RoadwayTwist::Twist;
	};


	/// \brief A twist that rotates the frame arount T in a way that B is nearest
	/// a certain direction.
	///
	/// The attractor is understood to be in the global or TrackSystem relative frame (if any).
	/// If the twist gets frozen, the attractor from then on will stay local to the track.
	struct DirectionalTwist : RoadwayTwist{

		/// \brief Makes a DirectionalTwist object.
		static dclspc std::unique_ptr<DirectionalTwist> Make( const spat::Vector<One>& attractor = Up ) noexcept;
		
		/// \brief Makes a DirectionalTwist object from another twist.
		static dclspc std::unique_ptr<DirectionalTwist> Make( const RoadwayTwist& fromTwist ) noexcept;

		using Data = spat::Vector<One>;

		/// \brief Set the direction for the twist.
		///
		/// The twist has to be reattached to a track, to make 
		/// \param attractor The vector to get approximated by the binormal vector in global coordinates
		/// \throws std::invalid_argument if attractor is a null vector.
		/// \throws std::logic_error If the twist is already attached to a track.
		virtual void Attractor( const spat::Vector<One>& attractor ) = 0;

		/// Normally this method returns the attractor in global coordinates, but in frozen state
		/// it will be relative to the track.
		/// \returns the attractor vector.
		virtual const spat::Vector<One>& Attractor() const noexcept = 0;


		/// This method returns the attractor in global or local coordinates.
		/// \param bGlobal If true the attractor will be given in global coordinates; local coordinates, if false.
		/// \returns the attractor vector.
		virtual const spat::Vector<One>& Attractor( bool bGlobal ) const noexcept = 0;


		/// \returns a pointer to the track's curve if attached, nullptr elsewise.
		virtual std::shared_ptr<const struct Curve> GetCurve() const noexcept = 0;

		virtual const Data& GetData() const noexcept = 0;


		/// \brief Creates the twist from the data.
		/// \throws std::invalid_argument if attractor is a null vector.
		/// \throws std::logic_error If the twist is already attached to a track.
		virtual void Create( const Data& data ) = 0;
	};


	/// \brief A twist that works like a DirectionalTwist but interpolates (linearly) two
	/// neighbouring attractors according to arc length and uses the resulting attractor.
	/// 
	/// The vectors in Data are local to the track.
	struct PiecewiseDirectionalTwist : RoadwayTwist{

		/// \brief Makes a PiecewiseDirectionalTwist object.
		static dclspc std::unique_ptr<PiecewiseDirectionalTwist> Make() noexcept;
		
		/// \brief Makes a PiecewiseDirectionalTwist object from another twist.
		static dclspc std::unique_ptr<PiecewiseDirectionalTwist> Make( const RoadwayTwist& fromTwist ) noexcept;


		using Data = std::vector<std::pair<Length,spat::Vector<One>>>;


		/// \name Adding Data Points
		/// \brief Add a control point to the Twist.
		/// \param s Arc length along the track to place the attractor.
		/// \param attractor Directional attractor in coordinates local to the track.
		/// \param pair attractor pair.
		///@{	
		virtual void Add( Length s, const spat::Vector<One>& attractor ) = 0;

		virtual void Add( const std::pair<Length,spat::Vector<One>>& pair ) = 0;
		///@}


		/// \returns the number of added control points 
		virtual int CntTwistValues() const = 0;

		/// \brief Creates the twist from the data.
		/// \throws std::invalid_argument if the vecors are not normalized.
		/// \throws std::logic_error if there are too few data points.
		virtual void Create( const Data& data ) = 0;

		/// \param idx Zero based index of control point.
		/// \returns A control point.
		virtual std::pair<Length,spat::Vector<One>> Twist( int idx ) const = 0;

		virtual const Data& GetData() const noexcept = 0;

		using RoadwayTwist::Twist;
	};


	/// \brief Combines the effect of two twists by adding them.
	///
	/// If a slot of the combined twist is empty, it will be populated
	/// with a ConstantTwist with a zero angle. The combined twist is
	/// symmetric in the twist1 slot and the twist2 slot, meaning 
	/// RoadwayTwist::Equals() returns true if only the twists in the 
	/// slots are switched.
	struct CombinedTwist : RoadwayTwist{

		/// \brief Makes a CombinedTwist object.
		static dclspc std::unique_ptr<CombinedTwist> Make() noexcept;

		/// \brief Makes a CombinedTwist object from another twist.
		static dclspc std::unique_ptr<CombinedTwist> Make( const RoadwayTwist& fromTwist ) noexcept;


		/// \brief Populates the first slot
		/// \throws std::invalid_argument if pTwist is null or the combined twist
		/// would end up with two dynamic twists.
		/// \throws std::logic_error if this twist is attached to a track.
		virtual void AttachTwist1( std::unique_ptr<RoadwayTwist> pTwist ) = 0;

		virtual RoadwayTwist& Twist1() const noexcept = 0;

		virtual std::unique_ptr<RoadwayTwist> DetachTwist1() = 0;


		/// \brief Populates the second slot
		/// \throws std::invalid_argument if pTwist is null or the combined twist
		/// would end up with two dynamic twists.
		/// \throws std::logic_error if this twist is attached to a track.
		virtual void AttachTwist2( std::unique_ptr<RoadwayTwist> pTwist ) = 0;

		virtual RoadwayTwist& Twist2() const noexcept = 0;

		virtual std::unique_ptr<RoadwayTwist> DetachTwist2() = 0;
	};


	/// \name FindTwist
	/// \brief In a tree of combined twists finds a twist of a certain type.
	/// \tparam TwistType Type of twist to find.
	/// \param twist Twist to start with.
	/// \return Pointer to found twist or nullptr.
	///@{
	template<class TwistType>
	const TwistType* FindTwist( const RoadwayTwist& twist ) noexcept{
		if( const auto* pTwist = dynamic_cast<const TwistType*>(&twist) )
			return pTwist;

		if( const auto* pCombinedTwist = dynamic_cast<const CombinedTwist*>(&twist) ){
			if( const auto* pTwist2 = FindTwist<const TwistType>(pCombinedTwist->Twist2()) )
				return pTwist2;
			else if( const auto* pTwist1 = FindTwist<const TwistType>(pCombinedTwist->Twist1()) )
				return pTwist1;
		}

		return nullptr;
	}
	
	template<class TwistType>
	TwistType* FindTwist( RoadwayTwist& twist ) noexcept{
		if( auto pTwist = dynamic_cast<TwistType*>(&twist) )
			return pTwist;

		if( auto pCombinedTwist = dynamic_cast<const CombinedTwist*>(&twist) ){
			if( auto pTwist2 = FindTwist<TwistType>(pCombinedTwist->Twist2()) )
				return pTwist2;
			else if( auto pTwist1 = FindTwist<TwistType>(pCombinedTwist->Twist1()) )
				return pTwist1;
		}

		return nullptr;
	}
	///@}

	// for /GR- we export the template instantiations from trax:
	extern template dclspc const ConstantTwist* FindTwist<ConstantTwist>( const RoadwayTwist& twist ) noexcept;
	extern template dclspc const LinearTwist* FindTwist<LinearTwist>( const RoadwayTwist& twist ) noexcept;
	extern template dclspc const PiecewiseTwist* FindTwist<PiecewiseTwist>( const RoadwayTwist& twist ) noexcept;
	extern template dclspc const DirectionalTwist* FindTwist<DirectionalTwist>( const RoadwayTwist& twist ) noexcept;
	extern template dclspc const CombinedTwist* FindTwist<CombinedTwist>( const RoadwayTwist& twist ) noexcept;

	extern template dclspc ConstantTwist* FindTwist<ConstantTwist>( RoadwayTwist& twist ) noexcept;
	extern template dclspc LinearTwist* FindTwist<LinearTwist>( RoadwayTwist& twist ) noexcept;
	extern template dclspc PiecewiseTwist* FindTwist<PiecewiseTwist>( RoadwayTwist& twist ) noexcept;
	extern template dclspc DirectionalTwist* FindTwist<DirectionalTwist>( RoadwayTwist& twist ) noexcept;
	extern template dclspc CombinedTwist* FindTwist<CombinedTwist>( RoadwayTwist& twist ) noexcept;


	//std::unique_ptr<RoadwayTwist> dclspc CreateTwistFrom( RoadwayTwist::TwistType type, const RoadwayTwist& twist );
	//std::unique_ptr<RoadwayTwist> dclspc CreateZeroFrom( const RoadwayTwist& twist );
	//std::unique_ptr<ConstantTwist> dclspc CreateConstantFrom( const RoadwayTwist& twist );
	//std::unique_ptr<LinearTwist> dclspc CreateLinearFrom( const RoadwayTwist& twist );
	//std::unique_ptr<PiecewiseTwist> dclspc CreatePiecewiseFrom( const RoadwayTwist& twist );
	//std::unique_ptr<PiecewiseTwist> dclspc CreatePiecewiseLinearFrom( const RoadwayTwist& twist );
	//std::unique_ptr<PiecewiseTwist> dclspc CreatePiecewiseCircularFrom( const RoadwayTwist& twist );
	//std::unique_ptr<PiecewiseDirectionalTwist> dclspc CreatePiecewiseDirectionalFrom( const RoadwayTwist& twist );
	//std::unique_ptr<DirectionalTwist> dclspc CreateDirectionalFrom( const RoadwayTwist& twist );
}
//	trax track library
//	AD 2023 
//
//  "Slide!"
//
//		Fight Club
// 
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

/// \page docu_railrunners RailRunners
/// 
/// \section railrunners_intro Introduction
/// A rail runner can be railed on a track. The \link trax::RailRunner 
/// RailRunner \endlink interface provides all means for Railing/Derailing, 
/// coupling and for steering something on a track system like thrust 
/// and brake. 
/// 
/// \image html RailRunner.png
/// 
/// \section railrunners_coupling Coupling
/// \link trax::Bogie Bogies \endlink provide couplings at their tip ends 
/// that can form a stable connection to other Bogies. Those coupling 
/// relay to \link trax::RollingStock RollingStock \endlink and
/// \link trax::Train Trains \endlink. Automatic couplings are realised 
/// in the trax library. Those couplings can be switched to active, passive 
/// or beeing coupled. An active coupling would couple another active one, 
/// as soon as their trigger spheres intersect. If one of the two couplings 
/// is passive, nothing would happen.
/// 
/// \section railrunners_anchor Anchor
/// The anchor is the position of the TrackJoint in a 
/// \link trax::WheelFrame WheelFrame \endlink. It will define the Location 
/// of the RailRunner on a Track. For a RollingStock, which is built up 
/// from several WheelFrames and Bogies, the anchor will be the northmost 
/// WheelFrame's anchor; for a Train it will be the northmost RollingStock's 
/// anchor.
/// 
/// \section railrunners_train Train
/// A Train is a RailRunner that maintains an ordered list of 
/// \link trax::TrainComponent TrainComponents \endlink. RollingStock and Train 
/// itself qualify as TrainComponents. That way a Train might be composed 
/// from other Trains. On splitting a Train, a new Train might be created 
/// automatically.
/// 
/// \section railrunners_drivemodel Drive Model
/// Moving a RailRunner might look a little odd, since the velocity of a 
/// WheelFrame is a constraint for the physics engine: first the Wheelsets 
/// have to specify the maximum available braking and driving forces as 
/// the capabilities of the machine. Of these limits a RailRunner can specify 
/// what fraction to actually apply at a given moment by the 
/// \link trax::RailRunner::Thrust() Thrust() \endlink and the 
/// \link trax::RailRunner::Brake() Brake() \endlink methods. Secondly there 
/// is a target velocity to be set for a RailRunner by the 
/// \link trax::RailRunner::TargetVelocity() TargetVelocity() \endlink method, 
/// so the physics engine knows what velocity to head for. It is clear that 
/// these limits might not be sufficient to actually reach vTarget. If they 
/// would, vTarget would be reached immediately and be kept that way. In 
/// practice the braking and driving forces will compete with other forces 
/// like gravity or mass inertia and friction, as they in reality do. To 
/// simulate the behaviour of real locomotive controls, as handled by an 
/// engineer, it is necessary to map those controls to the braking, thrust 
/// and vTarget values properly.
/// 

#include "trax/Units.h"
#include "trax/Identified.h"
#include "trax/Location.h"

#include <memory>


namespace trax{

	struct Jack;

	/// \brief Data to be defined by the user.
	struct RailRunnerUserData;
		
		
	/// \brief Interface for train and rolling stock that can be railed on a track 
	/// and might provide brakes and power engines.
	/// 
	/// This maps the way the physics engines used with trax work: a traget velocity
	/// is defined and the the maximum thrust and braking force that can get applied to 
	/// reach it. The braking force will only have effect in opposite direction of
	/// the target velocity; the thrust force only in that direction:
	///
	///		   <--- brake    target --->    thrust ---> 
	///
	/// The braking force is not applied to fight off a velocity against the target velocity, 
	/// only thrust will have an effect here. If target velocity is zero, only braking force 
	/// gets applied:
	///
	///			<--- brake    target o    brake --->
	///	
	struct RailRunner : Identified<RailRunner>
	{
		/// \returns the name for the object type that implements this interface. 
		virtual const char*	TypeName() const noexcept = 0;


		/// \brief Types of the RailRunner's end's.
		enum class EndType : char{
			none = 0,
			north,		///< Denotes the front end of a RailRunner.
			south,		///< Denotes the back end of a RailRunner.
			any,		///< Denotes the north or south end of a Railrunner.
			all			///< Denotes both ends of a RailRunner.
		};

		/// \brief Gets a shared pointer to this.
		virtual std::shared_ptr<RailRunner> This() const noexcept = 0;


		/// \returns true if this is a valid object.
		virtual bool IsValid() const noexcept = 0;


		// Rail/DeRail:


		/// \brief Attaches this RailRunner onto the specified track location.
		///
		/// The trackjoints of the WheelFrames will be attached to the track at
		/// locations that might differ from location, depending in the RailRunners
		/// definition of its location on the tracks. If a WheelFrame gets railed,
		/// in this railing process, its JackOnRail will pulse. If some railing is
		/// present already it gets overriden.
		/// \param location The location to rail this RailRunner. If railing is
		/// successful this location will be returned by GetLocation().
		/// \param bMoveTo If true the RailRunner will be moved to the transitional
		/// 3D pose of location, prior to railing. If this is a Train, the actual 
		/// coupling distances will be respected. See \see TrainComponent::Rail for 
		/// details.
		/// \throws std::out_of_range if a dead end is encountered on the track system.
		/// \throws std::logic_error If there are no suitable WheelFrame children in
		/// the RailRunner to rail.
		/// \throws std::invalid_argument If location is not on a track.
		/// \throws std::out_of_range If the railrunner would end up off the track system.
		/// \throws std::logic_error If the present configuration does not allow railing.
		virtual void Rail( const Location& location, bool bMoveTo = true ) = 0;


		/// \brief Gets the track location were the RailRunner is railed.
		///
		/// \see RailRunner::GetGlobalAnchor for more details.
		/// \returns Location that denotes a valid track location if railed.
		virtual Location GetLocation() const noexcept = 0;


		/// \brief Take the RailRunner from the track.
		///
		/// The JackOnDerail of all RailRunners involved will pulse.
		virtual void DeRail() noexcept = 0;


		/// \brief Is the RailRunner on a track?
		///
		/// \returns true if the RailRunner is completely railed; false if one or
		/// more child WheelFrames are derailed.
		virtual bool IsRailed() const noexcept = 0;


		// Locating in space

		/// \brief Gets the global anchor of the RailRunner.
		///
		/// Albeit only the WheelFrames are attached to the track, every 
		/// RailRunner tries to assign a privileged anchor to denote its 'official'
		/// position along the tracks. Only if properly railed, this will 
		/// be the same frame as can be gotten from \see RailRunner::GetLocation
		/// (But flipped if railed on a Track in anti direction).
		/// For a WheelFrame, the anchor will be the anchor of its TrackJoint. 
		/// A Bogie will ask its northernmost child and then its southermost for a
		/// anchor and throws an exception if none can befound. The parent Bogies 
		/// are never considered in finding an anchor. A RollingStock will ask its 
		/// topmost and northernmost Bogie, a Train its northernmost TrainComponent for
		/// an anchor.
		/// Note that while a Location is useless if not railed, the anchor is always
		/// a specified pose in space. The differences between the two stand for 
		/// the degree of derailment.
		/// \returns The assigned anchor of the RailRunner in global coordinates.
		/// \throws std::logic_error if there is no privilged anchor.
		virtual spat::Frame<Length,One> GetGlobalAnchor() const = 0;


		/// \brief Sets the position of the RailRunner so that its global anchor alignes
		/// to the given global pose.
		/// 
		/// The whole RailRunner will be moved so the assigned anchor alignes with
		/// the given global pose. This can be used to relocate the RailRunner
		/// when not railed.
		/// \param anchorPose The pose to align the global anchor with.
		/// \throws std::logic_error if there is no privilged anchor.
		virtual void ResetToGlobalAnchor( const spat::Frame<Length,One>& anchorPose ) = 0;


		// Driving:

		/// \brief Sets the velocity of this RailRunner along the track.
		///
		/// This will be a momentary velocity that will change over time due 
		/// to simulation events. This should only get used to set a certain 
		/// starting velocity, e.g. after inserting a RailRunner into the scene,
		/// constant reset of velocity will result in unstable simulation.
		/// \throws std::logic_error Thrown if the railrunner is not
		/// railed.
		virtual void SetVelocity( Velocity velocity ) = 0;


		/// \brief Gets the momentary velocity of this RailRunner along the track.
		virtual Velocity GetVelocity() const = 0;


		/// \brief Sets the target velocity to aim for by using the supplied
		/// Thrust() and Brake() settings.
		///
		/// The simulator will try to reach this by applying up to the fraction
		/// of maximum thrust, specified by Thrust() and up to the fraction of
		/// maximum braking force specified by Brake() if necessary to reduce the
		/// actual velocity. The brake will only be used, if the velocity exceeds 
		/// the target velocity. E.g. on a hill the RailRunner might roll back due to
		/// too few thrust but the brake will not get applied in this case.
		/// \param velocity Signed target velocity to aim for. This very well might 
		/// be set to extreme high values, wich can never be reached by the motor. 
		virtual void TargetVelocity( Velocity velocity ) = 0;


		/// \brief Gets the target velocity set for this RailRunner.
		virtual Velocity TargetVelocity() const noexcept = 0;


		/// \brief Gets the principle direction of movement according to the 
		/// sign of the target velocity. 
		/// 
		/// Note that if the target velocity is zero, this will still give the 
		/// orientation of the last movement.
		/// \returns north if the item is targeted to move to the north direction, 
		/// south if it is moving to the south (backwards).
		virtual EndType TargetDirection() const noexcept = 0;


		/// \returns The maximal power output the engine can supply. 
		virtual Power MaximumPowerOutput() const noexcept = 0;


		/// \returns The power output the engine is currently delivering.
		virtual Power CurrentPowerOutput() const noexcept = 0;


		/// \returns The maximum velocity the RailRunner can theoretically
		/// propell itself to on even track.
		virtual Velocity MaximumVelocity() const noexcept = 0;


		/// \brief Sets the fraction of the maximum thrust to apply in order to 
		/// reach the target velocity specified by TargetVelocity().
		/// \param byfactor Value between [0,1] specifying the fraction of the total 
		/// available acceleration.
		virtual void Thrust( One byfactor ) = 0;


		/// \brief Gets the fraction of the maximum thrust applied.
		/// \returns Value between [0,1], specifying the fraction of the 
		/// total available thrust.
		virtual One Thrust() const noexcept = 0;


		/// \brief Sets the thrust as absolute force values. If the 
		/// value exceeds MaxThrust() it get clipped.
		virtual void ThrustAbsolute( Force thrust ) = 0;


		/// \brief Gets the thrust as absolute momentary available force value.
		virtual Force ThrustAbsolute() const noexcept = 0;


		/// \name Maximum Thrust
		/// \brief Gets the maximal available thrust. This value is determined by 
		/// the properties of the Wheelsets attached to WheelFrames, which in turn are 
		/// attached to a bogie. It is also determined by the engine characteristic
		/// and the actual velocity.
		/// \see trax::Wheelset
		/// \see trax::TractionForceCharacteristic	
		///@{

		/// \returns The maximum thrust that can be applied prior to modulation
		/// by the traction force characteristic.
		virtual Force MaxThrust() const noexcept = 0;

		/// \param velocity the velocity the maximum thrust is requested for.
		/// \returns The maximum thrust that can be applied at the given velocity.
		virtual Force MaxThrust( Velocity velocity ) const noexcept = 0;
		///@}


		/// \brief Sets the fraction of the maximum braking to apply in order  
		/// to reach a zero velocity.
		/// \param byfactor Value between [0,1] specifying the fraction of the total 
		/// available braking.
		virtual void Brake( One byfactor ) = 0;


		/// \brief Gets the fraction of the maximum braking applied.
		/// \returns Value between [0,1], specifying the fraction of the  
		/// total available braking.
		virtual One Brake() const noexcept = 0;


		/// \brief Sets the brake as absolute force value. If the 
		/// value exceeds MaxBrake() it gets clipped.
		virtual void BrakeAbsolute( Force brake ) = 0;


		/// \brief Gets the brake as absolute force value.
		virtual Force BrakeAbsolute() const noexcept = 0;


		/// \brief Gets the maximal available braking. This value is determined by the 
		/// properties of the Wheelsets attached to WheelFrames wich in turn are attached
		/// to a bogie.
		virtual Force MaxBrake() const noexcept = 0;


		// Coupling:

		/// \brief Activates the coupling at the respective end of the RailRunner.
		/// 
		/// Non active couplings can not get coupled. Coupled couplings can neither
		/// be activated nor deactivated. A coupling is deactivated by default. 
		/// \param end Denotes the coupling.
		/// \throws std::runtime_error if coupling is coupled already or has no valid 
		/// couling index.
		virtual void ActivateCoupling( EndType end ) = 0;


		/// \brief Deactivates the coupling at the respective end of the RailRunner.
		/// 
		/// Non active couplings can not get coupled. Coupled couplings can neither
		/// be activated nor deactivated.
		/// \param end Denotes the coupling.
		/// \throws std::runtime_error if coupling is coupled.
		virtual void DeactivateCoupling( EndType end ) = 0;


		/// \returns True if the coupling is active (coupled or not). False for a 
		/// deactivated coupling.
		/// \param end Denotes the coupling.
		virtual bool IsActivated( EndType end ) const noexcept = 0;


		/// \brief Uncouples the RailRunner at the specified end.
		///
		/// \param end End of this RailRunner.
		/// \param btriggerPulses If true the respective JackOnUnCouple jacks are pulsing.
		/// \returns True if actually some uncoupling happened.
		virtual bool Uncouple( EndType end = EndType::all, bool btriggerPulses = true ) noexcept = 0;


		/// \brief Is the coupling coupled?
		///
		/// \param end End of this RailRunner.
		/// \returns True if the coupling is coupled.
		virtual bool IsCoupled( EndType end = EndType::any ) const noexcept = 0;


		/// \brief Gets the height of the coupling over the track.
		/// \param end End of this TrainComponent to get the coupling height for.
		/// \returns The height of the coupling over the track.
		virtual Length GetCouplingHeight( EndType end ) const noexcept = 0;


		// Jacks 'N Plugs:

		/// \brief Gets a Jack that pulses its Plug if one (or any) of 
		/// the RailRunner's outmost couplings get coupled.
		virtual Jack& JackOnCouple( EndType end ) = 0;


		/// \brief Gets a Jack that pulses its Plug if one (or any) of 
		/// the RailRunner's outmost couplings get uncoupled or breaks.
		virtual Jack& JackOnUnCouple( EndType end ) = 0;


		/// \brief Gets a Jack that pulses its Plug if one (or any) of 
		/// the RailRunner's outmost couplings get activated.
		virtual Jack& JackOnCouplingActivated( EndType end ) = 0;


		/// \brief Gets a Jack that pulses its Plug if one (or any) of 
		/// the RailRunner's outmost couplings get deactivated.
		virtual Jack& JackOnCouplingDeactivated( EndType end ) = 0;


		/// \brief Sets a pointer to user provided data.
		///
		/// Define TrackUserData to specify your data types.
		virtual void UserData( RailRunnerUserData* pData ) noexcept = 0;


		/// \returns User provided data.
		virtual RailRunnerUserData* UserData() const noexcept = 0;
	};


	/// \brief Gets the opposite end type.
	inline RailRunner::EndType operator!( RailRunner::EndType end ) noexcept{
		switch( end ){
			case RailRunner::EndType::north:	return RailRunner::EndType::south;
			case RailRunner::EndType::south:	return RailRunner::EndType::north;
			default:							return RailRunner::EndType::none;
		}
	}

	/// \brief Makes a string from the status
	dclspc std::string ToString( RailRunner::EndType end );


	/// \brief Makes a end type value from a string.
	/// \throws std::invalid_argument if the string was not recognized.
	dclspc RailRunner::EndType ToRailRunnerEndType( const std::string& endtype );

}


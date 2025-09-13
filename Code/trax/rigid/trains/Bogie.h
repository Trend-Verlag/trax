//	trax track library
//	AD 2014
//
//  "here in the land of opportunity"
//
//					New Model Army
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

#include "RailRunner.h"

#include "trax/Simulated.h"
#include "spat/Frame.h"
#include "spat/Sphere.h"

namespace trax{

	struct Gestalt;
	struct RollingStock;
	struct TrainScene;

	/// \brief A Bogie is a RailRunner that can be attached to another
	///  Bogie by a swivel and can be coupled to another Bogie by a coupling.
	///
	/// \verbatim
	///  <-- north				south -->
	/// 
	///          |        |
	///       [      B       ]      
	///    |    |          |    |
	///   [     A    ] [   C     ]
	/// _____O__O__O_____O__O__O______________
	///
	/// '|' are the north and south swivel hinges (some of them not connected).
	/// \endverbatim
	/// The north and south parent/child slots are populated in the order
	/// along the track. So the north child bogie would have been connected by 
	/// its south parent slot and the other way around. As a rule north and
	/// south and parent and child connects respectively; all other combinations
	/// are not allowed.
	/// In reality there are configurations with three bogies, sharing one
	/// parent, e.g. SBB Re 6/6, which carries a Bo'Bo'Bo' configuration, but
	/// the center child bogie is not a Bogie in our sense, since it is not rotatable,
	/// but shifts aside. These wheels would be attached to the parent.
	struct Bogie : RailRunner,
	               Simulated
	{
		/// \brief Makes a Bogie object.
		/// \param scene The scene to put the Bogie into.
		/// \param pGestalt The Gestalt to use. If no Gestalt is given 
		/// one is automatically created.
		static dclspc std::shared_ptr<Bogie> Make( TrainScene& scene, std::shared_ptr<Gestalt> pGestalt = nullptr ) noexcept;


		/// \brief Gets a shared pointer to this.
		virtual std::shared_ptr<Bogie> ThisBogie() const noexcept = 0;


		virtual Gestalt& GetGestalt() const noexcept = 0;


		/// \returns The RollingStock this Bogie is member of or nullptr if none.
		virtual RollingStock* GetRollingStock() const noexcept = 0;


		// Bogies's arrangement:

		/// \name Attachement
		/// \brief Attaches a Bogie by a swivel as a child to this Bogie.
		/// 
		/// There are two child bogies per parent bogie max. It is possible 
		/// to attach a Bogie to more than one parent as is the case for
		/// Jacobs bogies. Seen from a rolling stock a child bogie at the edge
		/// will always be connected with its innermost parent slot (i.e.
		/// north_child get assigned to south_parent and vice versa).
		///@{
		
		/// \param pChildBogie Pointer to a Bogie to get attached.
		/// \param atEnd The end of this Bogie to attach the child Bogie to.
		/// \param swivelPose The pose relative to this Bogie's 
		/// Body frame through whiches Bin vector the hinge axis will run. It
		/// defaults to the child Bogie's Anchor position or the center of its
		/// two child swivels.
		/// \throws std::runtime_error if the child Bogie is already attached 
		/// to a parent at the respective slot or there is already a child bogie
		/// attached to this parent Bogie in the respective slot. Also attaching 
		/// a child and parent twice would count as an error. Also if one of 
		/// the Bogies is already member of a RollingStock the exception is thrown.
		virtual void Attach( std::shared_ptr<Bogie> pChildBogie, EndType atEnd, const spat::Frame<Length,One>& swivelPose ) = 0;

		/// \param pChildBogie Pointer to a Bogie to get attached.
		/// \param atEnd The end of this Bogie to attach the child Bogie to.
		/// \throws std::runtime_error if the child Bogie is already attached 
		/// to a parent at the respective slot or there is already a child bogie
		/// attached to this parent Bogie in the respective slot. Also attaching 
		/// a child and parent twice would count as an error. Also if one of 
		/// the Bogies is already member of a RollingStock the exception is thrown.
		virtual void Attach( std::shared_ptr<Bogie> pChildBogie, EndType atEnd ) = 0;
		///@}


		/// \brief Gets the a pointer to a child bogie and the 
		/// pose relative to this Bogie, were the swivel axis is running 
		/// through the Bin vector.
		/// \param atEnd The child on the north or south end of this Bogie.
		/// \returns The north or the south child bogie and the relative swivel position.
		virtual std::pair<std::shared_ptr<Bogie>,spat::Frame<Length,One>> GetChild( EndType atEnd ) const noexcept = 0;


		/// \brief Gets the a pointer to a parent bogie and the  
		/// pose relative to this Bogie, were the swivel axis is running 
		/// through the Bin vector.
		/// \param atEnd The parent on the north or south end of this Bogie.
		/// \returns The north or the south parent bogie and the relative swivel position.
		virtual std::pair<std::shared_ptr<Bogie>,spat::Frame<Length,One>> GetParent( EndType atEnd = EndType::any ) const noexcept = 0;


		/// \brief Gets the first bogie in the rolling stock.
		virtual std::shared_ptr<Bogie> GetFirst() const noexcept = 0;


		/// \brief Gets the next bogie in the rolling stock.
		///
		/// The bogies of an bogie arrangement form a unique order 
		/// along the track.
		virtual std::shared_ptr<Bogie> GetNext() const noexcept = 0;

		
		/// \brief Gets the previous bogie in the rolling stock.
		///
		/// The bogies of an bogie arrangement form a unique order 
		/// along the track.
		virtual std::shared_ptr<Bogie> GetPrevious() const noexcept = 0;


		/// \brief Gets the last bogie in the rolling stock.
		virtual std::shared_ptr<Bogie> GetLast() const noexcept = 0;


		/// \brief Detaches a bogie from its parent(s).
		virtual void Detach( EndType atEnd = EndType::all ) noexcept = 0;



		// Coupling:

		/// \brief Couples this Bogie with another at the respective ends.
		///
		/// It doesn't matter, wether the two couplings are near each other or not. 
		/// The coupling will try to maintain the actual distance of the two 
		/// Bogies's ends.
		/// The coupling attempt will fail if the two Bogies are already coupled
		/// (with different partners or the same parters at different ends) or 
		/// if one of the couplings is deactivated. An coupling attempt will 
		/// also fail, when two couplings wear different coupling type indices. 
		/// \see Bogie::CouplingProps.
		/// \param end End of this Bogie.
		/// \param with Other Bogie.
		/// \param withEnd Other Bogie's end to couple with.
		/// \param btriggerPulses If true the respective JackOnCouple jacks are pulsing.
		/// \returns true if a new coupling was established or the two Bogies
		/// were already properly coupled. 
		virtual bool Couple( EndType end, Bogie& with, EndType withEnd, bool btriggerPulses = true ) noexcept = 0;


		/// \brief Get the coupled Bogie.
		/// \param end End type of this Bogie.
		/// \returns A pointer to the other Bogie coupled at end, or  
		/// nullptr if none and the other Bogie's end type.	
		virtual std::pair<std::shared_ptr<Bogie>,EndType> GetCoupledBogie( EndType end ) const noexcept = 0;


		/// \brief Describing data of a coupling.
		struct CouplingProps{
			enum BreakingType{ 
				onNever,							///< The coupling never breaks.
				onForceLimitReached,				///< The coupling breaks if the force limit is reached.
				onMomentumConsumed,					///< The coupling breaks if the momentum is consumed.
				onMomentumConsumedWithRecuperation	///< The coupling breaks if the momentum is consumed, but the momentum can be recuperated.
			} Type = onNever;						///< The type of the breaking condition.

			spat::Sphere<Length>	Position = { spat::Origin3D<Length>, 0.5_m };	///< Position and trigger radius of the coupling relative to the bogie's body frame.             
			Force					BreakingForce = infinite__force;				///< Breaking force of the coupling. infinite for unbreakable.
			Momentum				BreakingSpareMomentum = 0_Ns;					///< Breaking momentum of the coupling to be consumed, before the coupling breaks.
			Force  					RecuperationForce = 0_N;						///< Force to recuperate the spare momentum of the coupling.	
			int						CouplingTypeIdx = 0;							///< Index of the coupling type. Only coupling with same index greater 0 can be coupled.
		};


		/// \brief Sets the coupling properties.
		/// \throws std::runtime_error if the coupling is already coupled.
		virtual void SetCouplingProps( EndType atEnd, const CouplingProps& props ) = 0;


		/// \brief Gets the coupling properties.
		virtual CouplingProps GetCouplingProps( EndType atEnd ) const noexcept = 0;


		virtual spat::Sphere<Length> GetCoupling( EndType end ) const noexcept = 0;


		/// \brief Sets the height of the coupling over the track.
		/// 
		/// This changes the CouplingProps::Position of the coupling.
		/// \param end End of this TrainComponent to set the coupling height for.
		/// \param height The height of the coupling over the track.
		/// \throws std::runtime_error if the coupling is already coupled or
		/// the Bogie neither is a WheelFrame nor has a northern child WheelFrame.
		virtual void SetCouplingHeight( EndType end, Length height ) = 0;


		/// \brief Breaks (not uncouples) the coupling at the enoted end.
		///
		/// Same as uncouple but sends ForceLimitExceeded pulse as if the
		/// coupling would have actually broken due to overstrech.
		/// \param atEnd Coupling to break.
		/// \returns True if a coupling was defined and actually coupled.
		virtual bool BreakCoupling( EndType atEnd, bool btriggerPulses = true ) noexcept = 0;



		// Jacks 'N Plugs:

		/// \brief Gets a Jack that pulses its Plug if the RailRunner's 
		/// motor starts, i.e. if the thrust is set to nonzero value.
		virtual Jack& JackOnMotorStart() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the RailRunner's 
		/// motor stops, i.e. if the thrust is set to zero.
		virtual Jack& JackOnMotorStop() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the WheelFrame's brakes
		/// are applied, i.e. if the braking value is set to nonzero.
		virtual Jack& JackOnBrakeApply() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the WheelFrame's brakes
		/// are released, i.e. if the braking value is set to zero.
		virtual Jack& JackOnBrakeRelease() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if one (or any) of 
		/// the RailRunner's outmost couplings experiences a force 
		/// greater than the maximum coupling force.
		virtual Jack& JackOnCouplingForceLimitExceeded( EndType end ) = 0;


		/// \brief Gets a Jack that pulses its Plug if the Bogie's hinge,
		/// that connects it to its north child, is bending against its 
		/// natural turning direction about more than pi/64.
		virtual Jack& JackOnNorthSwivelBendingIn() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the Bogie's hinge,
		/// that connects it to its north child, is returning from its bending 
		/// against its natural turning direction.
		virtual Jack& JackOnNorthSwivelBendingOut() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the Bogie's hinge,
		/// that connects it to its south child, is bending against its 
		/// natural turning direction about more than pi/64.
		virtual Jack& JackOnSouthSwivelBendingIn() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the Bogie's hinge,
		/// that connects it to its south child, is returning from its bending 
		/// against its natural turning direction.
		virtual Jack& JackOnSouthSwivelBendingOut() noexcept = 0;
	};

	dclspc bool Couple( std::pair<Bogie&,RailRunner::EndType> a, std::pair<Bogie&,RailRunner::EndType> b ) noexcept;

	dclspc bool AreCoupled( const Bogie& a, const Bogie& b ) noexcept;

	dclspc Length GetCouplingDistance( std::pair<const Bogie&,RailRunner::EndType> a, std::pair<const Bogie&,RailRunner::EndType> b ) noexcept;
}


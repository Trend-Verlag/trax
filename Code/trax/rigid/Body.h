//	trax track library
//	AD 2023 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
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
// For further information, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/Configuration.h"
#include "trax/Units.h"

#include "spat/Frame.h"
#include "spat/Matrix.h"
#include "spat/Box.h"
#include "spat/Sphere.h"

namespace trax{

	/// \brief A dynamic object in physical space.
	///
	/// For a good introduction in dynamics of solid bodies see 
	/// Landau/Lifschitz, Mechanics Vol I, Nauka, Moskau. 
	struct Body{

		// Body's name:

		/// \brief Sets the name of the Body.
		/// 
		/// This is for debug purposes; the string is not copied,
		/// only the pointer is stored.
		virtual void SetName( const char* name ) noexcept = 0;


		/// \returns The name of the Body.
		virtual const char* GetName() const noexcept = 0;


		// Body's pose:

		///	\brief Sets position and orientation (pose) in global coordinates.
		/// 
		/// This will set the position and orientation of the Body in global
		/// simulation space. 
		/// \param frame Frame of reference. Must be orthonormal.
		virtual void SetFrame( const spat::Frame<Length,One>& frame ) noexcept = 0;


		///	\brief Gets the pose in global coordinates.
		virtual void GetFrame( spat::Frame<Length,One>& frame ) const noexcept = 0;


		/// \brief Wakes the Body if it was deactivated due to not
		/// being in move.
		virtual void WakeUp() = 0;


		/// \brief Is the body deactivated?
		virtual bool IsSleeping() const noexcept = 0;


		virtual void SetSleepDelay( Time threshold ) = 0;

		virtual Time GetSleepDelay() const noexcept = 0;

		virtual void SetSleepThreshold( decltype(Velocity{}*Velocity{}) threshold ) = 0;

		virtual decltype(Velocity{}*Velocity{}) GetSleepThreshold() const noexcept = 0;


		/// \name Mass and inertia
		/// \brief The behaviour of a rigid body under forces is determined
		/// by its mass: the amount and the distribution, given by the inertia tensor
		/// and the barycenter of mass (CoM). There is a local 'mass space' in which 
		/// the inertia tensor is diagonal, meaning the main axes of the inertia ellipsoid 
		/// are aligned with the axes of the 'mass space' frame, with the CoM at its origin.
		///@{

		/// \brief Sets the mass of the Body.
		///
		/// This only sets the mass but lets the distribution of it, given by 
		/// CenterOfMassLocalPose and PrincipalMomentsOfInertia unchanged.
		/// 
		/// \see CenterOfMassLocalPose
		/// \see PrincipalMomentsOfInertia
		/// \param mass Mass to set for the Body. Must be > 0_kg.
		/// \throws std::invalid_argument if the mass is < 0_kg.
		virtual void SetMass( Mass mass ) = 0;


		/// \returns The mass of the Body.
		virtual Mass GetMass() const noexcept = 0;


		/// \brief Sets the center of mass in Body space.
		virtual void CenterOfMass( const spat::Position<Length>& pos ) noexcept = 0;


		/// \returns The center of mass in body space.
		virtual spat::Position<Length> CenterOfMass() const noexcept = 0;


		/// \brief Sets the local pose and the center of mass in Body space.
		///
		/// This sets both the position of the center of mass and the orientation
		/// of the main inertia axes body space.
		/// \see CenterOfMass
		/// \see PrincipalMomentsOfInertia
		virtual void CenterOfMassLocalPose( const spat::Frame<Length,One>& frame ) noexcept = 0;


		/// \returns the local pose of the center of mass in Body space.
		virtual spat::Frame<Length,One> CenterOfMassLocalPose() const noexcept = 0;


		/// \brief Sets the principal moments of inertia for the Body in local mass space.
		///
		/// The inertia tensor in body space is a symmetrical 3x3 matrix I'. It can be
		/// transformed from a diagonal matrix I by a proper rotation of the frame of
		/// reference R: I' = R*I*Transposed(R). The diagonal elements are
		/// called the principal moments of inertia. R is then the rotational part 
		/// of the mass local pose.
		virtual void PrincipalMomentsOfInertia( const spat::Vector<MomentOfInertia>& principalMoments ) noexcept = 0;


		/// \returns the principal moments of inertia for the Body in local mass space.
		virtual spat::Vector<MomentOfInertia> PrincipalMomentsOfInertia() const noexcept = 0;
		///@}


		/// \brief Sets the velocity of the barycenter of this Body.
		///
		/// This will be a momentary velocity that will change over time due 
		/// to simulation events. This should only get used to set a certain 
		/// starting velocity, e.g. after inserting a Body into the scene,
		/// constant reset of velocity will result in unstable simulation.
		virtual void SetLinearVelocity( const spat::Vector<Velocity>& v ) = 0;


		/// \brief Gets the velocity of the barycenter of this Body.
		virtual void GetLinearVelocity( spat::Vector<Velocity>& v ) const = 0;


		/// \brief Sets the rotational velocity around the barycenter of this Body.
		///
		/// This will be a momentary velocity that will change over time due 
		/// to simulation events. This should only get used to set a certain 
		/// starting velocity, e.g. after inserting a RailRunner into the scene,
		/// constant reset of velocity will result in unstable simulation.
		virtual void SetAngularVelocity( const spat::Vector<AngularVelocity>& w ) = 0;


		/// \brief Sets the rotational velocity around the barycenter of this Body.
		virtual void GetAngularVelocity( spat::Vector<AngularVelocity>& w ) const = 0;


		/// \brief Enables or disables the simulation of this Body.
		///
		/// While simulation is disabled, the Body will not be affected by forces,
		/// and not be moved by the simulator; connected joints will not influence
		/// the simulation.
		/// \param enable If true, the simulation is enabled; if false it is 
		/// disabled.
		virtual void EnableSimulation( bool enable = true ) noexcept = 0;


		/// \returns true if the simulation of this Body is enabled; false if disabled.
		virtual bool IsSimulationEnabled() const noexcept = 0;



		virtual ~Body() = default;
	};


	/// \brief Calculates the specific inertia tensor for a special shape.
	///
	/// \returns the inertial tensor diagonal for a shape of mass 1. For shapes with
	/// uniform density this can be multiplied with the mass to get the actual inertia.
	///@{
	dclspc spat::Vector<Area> SpecificInertiaTensorFor( const spat::Box<Length>& box );

	dclspc spat::Vector<Area> SpecificInertiaTensorFor( const spat::Sphere<Length>& sphere );

	dclspc spat::Vector<Area> SpecificInertiaTensorForCylinder( Length radius, Length length );
	///@}
}


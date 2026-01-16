//	trax track library
//	AD 2024 
//
//  "A ghost is spirit without form. I believe what we're looking
//   for and what we're seeing here, is ... is form without spirit. 
//   Something called a Golem."
//
//								Agent Mulder
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

/// \page docu_gestalt Gestalts
/// 
/// \section gestalt_intro Introduction
/// A \link trax::Geom Geom \endlink is a simple geometrical object like a 
/// \link trax::GeomCuboid Cuboid \endlink or a \link trax::GeomSphere Sphere 
/// \endlink. A \link trax::Shape Shape \endlink is a collection of Geoms that
/// forms a more complex shape and can be used for collision detection.
/// A \link trax::Body Body \endlink is an object with mass and inertia that can
/// be positioned and oriented in space. Together, a Body with a Shape is a
/// \link trax::Gestalt Gestalt \endlink, that can be used in a physics simulation
/// as a dynamic object with collision detection.
/// 
/// @startuml
/// skinparam classAttributeIconSize 0
/// left to right direction
/// 
/// together {
/// interface Body {
///   +SetFrame(Frame&)
///   +GetFrame(Frame&)
///   +SetMass(Mass)
///   +GetMass() : Mass
/// }
/// 
/// interface Shape {
///   +Attach(Geom*) : int
///   +Count() : int
///   +Clear()
/// }
/// 
/// interface Geom {
///   +SetFrame(Frame&)
///   +GetGeomType() : GeomType
///   +SetMaterial(Material&)
/// }
/// }
/// 
/// interface Gestalt {
///   +SetName(char*)
///   +Attach(Geom*, Mass) : int
///   +GeomMass(int) : Mass
/// }
/// 
/// Shape o-right- "0..*" Geom
/// Body <|-down- Gestalt
/// Shape <|-down- Gestalt
/// 
/// note bottom of Gestalt
///   Combines physical and
///   geometric properties
/// end note
/// @enduml
///

#include "Body.h"
#include "Shape.h"

namespace trax
{

	/// \brief A Gestalt is a Body with a Shape.
	///
	/// A Gestalt combines the physical properties of a Body with the geometric
	/// properties of a Shape. The latter combines multiple Geoms into a single
	/// collision shape.
	/// \see Geom
	struct Gestalt : virtual Body,
					 virtual Shape
	{
	public:

		// Gestalt's name:

		/// \brief Sets the name of the Gestalt.
		virtual void SetName( const char* name ) noexcept = 0;


		/// \returns The name of the Gestalt.
		virtual const char* GetName() const noexcept = 0;

		// Gestalt's pose:

		///	\brief Sets position and orientation (pose) in global coordinates.
		/// 
		/// This will set the position and orientation of the Gestalt in global
		/// simulation space. 
		/// \param frame Frame of reference. Must be orthonormal.
		virtual void SetFrame( const spat::Frame<Length,One>& frame ) noexcept = 0;


		///	\brief Gets the pose in global coordinates.
		virtual void GetFrame( spat::Frame<Length,One>& frame ) const noexcept = 0;


		using Shape::Attach;
		
		/// \brief Attach Geoms with dedicated mass properties to the Gestalt.
		///
		/// This will be used for collision testing. The Geom carries a
		/// frame of reference to specify its pose relative to the Body's pose.
		/// The geoms are added to already existing attached geoms; mass 
		/// properties are added to the existing mass properties of the Body.
		/// \param pGeom A single Geom to be attached to the Body.
		/// \param mass A mass to be used when calculating the mass properties.
		/// \param massLocalPose The local pose of the mass in Body space.
		/// \param inertiaTensor The inertia tensor for the mass local to 
		/// massLocalPose.
		/// \param geoms A vector of Geoms or Geom/Mass pairs to be attached to 
		/// the Body.Will be empty after the call if successful, untouched if not.
		/// \returns The zero based index of the Geom that was attached first; the 
		/// consecutive geoms will be assigned indexes from that in increasing 
		/// order. There will be all geoms attached or none, in which case -1 is
		/// returned. nullptr gets ignored.
		/// \throws std::invalid_argument if a pGeom is not of positive volume
		/// or the mass is <= 0_kg or invalid otherwise.
		/// \throws std::runtime_error if pGeom could not be attached due to 
		/// system issues including range errors.
		///@{
		virtual int Attach( std::unique_ptr<Geom> pGeom, Mass mass ) = 0;

		virtual int Attach( std::unique_ptr<Geom> pGeom, Mass mass, const spat::Frame<Length,One>& massLocalPose, const spat::SquareMatrix<MomentOfInertia,3>& inertiaTensor ) = 0;
		
		virtual int Attach( std::vector<std::pair<std::unique_ptr<Geom>,Mass>>& geoms ) noexcept = 0;

		virtual int Attach( std::vector<std::unique_ptr<Geom>>& geoms, Mass mass, const spat::Frame<Length,One>& massLocalPose, const spat::SquareMatrix<MomentOfInertia,3>& inertiaTensor ) = 0;
		///@}
	};
}
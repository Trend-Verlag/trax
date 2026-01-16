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

#include "Geom.h"

namespace trax{

	struct Axis;

	/// \brief A Shape is a collection of Geoms.
	///
	/// A Shape forms a more complex geometrical object by combining
	/// multiple Geoms into a single entity.
	/// \see trax::Geom for simple shapes.
	struct Shape{

		// Shape's name:

		/// \brief Sets the name of the Shape.
		virtual void SetName( const char* name ) noexcept = 0;


		/// \returns The name of the Shape.
		virtual const char* GetName() const noexcept = 0;


		// Shape's pose:

		///	\brief Sets position and orientation (pose) in global coordinates.
		/// 
		/// This will set the position and orientation of the Shape in global
		/// simulation space. 
		/// \param frame Frame of reference. Must be orthonormal.
		virtual void SetFrame( const spat::Frame<Length,One>& frame ) noexcept = 0;


		///	\brief Gets the pose in global coordinates.
		virtual void GetFrame( spat::Frame<Length,One>& frame ) const noexcept = 0;


		/// \brief Gets the minimum axis aligned box that includes the 
		/// the Geoms completely.
		virtual spat::Box<Length> GetExtent() const noexcept = 0;


		/// \brief Attach a Geom to the Shape.
		///
		/// This will be used for collision testing. The Geom carries a
		/// frame of reference to specify its pose relative to the Shape's pose.
		///@{

		/// \param pGeom A Geom to be attached to the Shape.
		/// \returns The zero based index of the Geom attached; -1 if
		/// no geom was attached.
		/// \throws std::invalid_argument if pGeom is of negative volume
		/// or invalid otherwise.
		/// \throws std::runtime_error if pGeom could not be attached
		/// due to system issues including range errors.
		virtual int Attach( std::unique_ptr<Geom> pGeom ) = 0;

		/// \param geoms A list of Geoms to be attached to the Shape. Will
		/// be empty after the call if successful, untouched if not.
		/// \returns The zero based index of the first Geom attached; the 
		/// consecutive geoms will be assigned indexes from that in increasing 
		/// order. There will be all geoms attached or none, in which case -1 is
		/// returned. nullptr gets ignored.
		virtual int Attach( std::vector<std::unique_ptr<Geom>>& geoms ) noexcept = 0;
		///@}


		/// \brief Removes the Geom with index idx from the Shape.
		///
		/// This call invalidates all the Geom's indexes >= idx.
		/// \returns a pointer to the removed Geom.
		/// \throws std::out_of_range when idx is outside [0,Clear()[
		virtual std::unique_ptr<Geom> Remove( int idx ) = 0;


		/// \brief Remove all Geoms from the Shape.
		///
		/// This might not be a good idea for objects like bogies that have added 
		/// some Geoms internally.
		virtual void Clear() noexcept = 0;


		/// \brief Counts the Geoms attached to this Shape.
		/// \returns the number of Geoms attached to this Shape.
		virtual int Count() const = 0;


		/// \brief Accesses the Geom at index idx.
		/// The geoms indices are consecutive zero based,
		/// so [0,Count()[ are valid indexes.
		/// \param idx zero based index of the Geom.
		/// \returns a reference to the Geom at index idx.
		virtual Geom& Get( int idx = 0 ) const = 0;


		/// \brief Tests for overlap with another Shape in global space.
		/// \param other The other Shape to test against.
		/// \returns true if there is an overlap.
		virtual bool IsOverlapping( const Shape& other ) const noexcept = 0;


		///// \returns The number of axes, this is the dominant shape for.
		//virtual int CountAxes() const = 0;


		///// \returns a shared pointer to the Axis at index idx.
		//virtual std::shared_ptr<Axis> GetAxis( int idx = 0 ) const = 0;


		virtual ~Shape() = default;
	};

}

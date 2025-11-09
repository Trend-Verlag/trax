//	trax track library
//	AD 2024 
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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

/// \page docu_gestalt Gestalts
/// 
/// \section gestalt_intro Introduction
/// 
/// 

#include "Body.h"
#include "Shape.h"

namespace trax
{
	struct Gestalt : virtual Body,
					 virtual Shape
	{
	public:

		// Gestalt's name:

		/// \brief Sets the name of the Gestalt.
		/// 
		/// This is for debug purposes; the string is not copied,
		/// only the pointer is stored.
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
		
		/// \brief Attach a Geom with a dedicated mass to the Gestalt.
		///
		/// This will be used for collision testing. The Geom carries a
		/// frame of reference to specify it's pose relative to the Body's pose.
		/// \param pGeom A Geom to be attached to the Body.
		/// \param mass A mass to be used when calculating the mass properties 
		/// for the Body by CalculateMassPropertiesFromShapes().
		/// \returns The zero based index of the Geom.
		virtual int Attach( std::unique_ptr<Geom> pGeom, Mass mass ) = 0;


		/// \param idx zero based index of the Geom.
		/// \returns a pointer to the Geom at index idx.
		virtual Mass GeomMass( int idx = 0 ) const = 0;

	};
}
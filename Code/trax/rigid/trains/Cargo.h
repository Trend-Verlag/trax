//	trax track library
//	AD 2025 
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

#include "trax/Identified.h"
#include "trax/rigid/trains/CargoType.h"


namespace trax{

	struct Gestalt;
	struct Shape;

	/// \brief Data to be defined by the user.
	struct CargoUserData;


	struct Cargo : Identified<Cargo>
	{
		static dclspc std::unique_ptr<Cargo> Make( CargoType type ) noexcept;


		/// \returns the name for the object type that implements this interface. 
		virtual const char*	TypeName() const noexcept = 0;


		/// \returns The Type of this cargo piece.
		virtual CargoType GetType() const = 0;


		/// \brief Sets a shape the track is attached to (if any).
		virtual void SetShape( std::shared_ptr<Shape> pShape ) noexcept = 0;


		/// \returns Returns the shape a track is assigned to or nullptr.
		virtual std::shared_ptr<Shape> GetShape() const noexcept = 0;


		/// \brief Sets a pointer to user provided data.
		///
		/// Define CargoUserData to specify your data types.
		virtual void UserData( CargoUserData* pData ) noexcept = 0;


		/// \returns User provided data.
		virtual CargoUserData* UserData() const noexcept = 0;
	};


	struct MoveableCargo : virtual Cargo
	{
		static dclspc std::unique_ptr<MoveableCargo> Make( CargoType type ) noexcept;


		/// \brief Sets a gestalt the track is attached to (if any).
		virtual void SetGestalt( std::shared_ptr<Gestalt> pGestalt ) noexcept = 0;


		/// \returns Returns the gestalt a track is assigned to or nullptr.
		virtual std::shared_ptr<Gestalt> GetGestalt() const noexcept = 0;



	};

}

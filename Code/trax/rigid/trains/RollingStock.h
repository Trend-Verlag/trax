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

#include "TrainComponent.h"

#include <memory>

namespace trax{

	struct Bogie;
	struct Simulator;
	struct Train;
	struct WheelFrame;
	struct SimulatorTrain;


	/// \brief A RollingStock is a RailRunner that manages one or more connected Bogies.
	///
	struct RollingStock : TrainComponent{

		/// \brief Makes a RollingStock object.
		static dclspc std::shared_ptr<RollingStock> Make() noexcept;


		/// \brief Gets a shared pointer to this.
		virtual std::shared_ptr<RollingStock> ThisRollingStock() const noexcept = 0;


		/// \brief Attaches a configured Bogie arrangement to the RollingStock.
		/// 
		/// It is only necessarry to call this method once; from a bogie arrangement, 
		/// any Bogie can be taken, to make the whole arrangement be managed by 
		/// the RollingStock.
		/// The Bogie arrangement is expected to be build after the rules \see Bogie.
		/// The couplings should be properly positioned (or canceled by index < 0) to
		/// make geometrical values like length calculatable.
		/// \param bogie The Bogie arrangement to be managed by the RollingStock. Any Bogie
		/// out of the arrangement will do here.
		/// \return true if the Bogie arrangement was successfully attached.
		virtual bool Attach( Bogie& bogie ) noexcept = 0;


		/// \returns The number of bogies.
		virtual int GetNumberOfBogies() const noexcept = 0;


		/// \brief Gets the Bogie with index number idx.
		///
		/// The idx count from north to south.
		/// \param idx The index of the Bogie. 0 is the
		/// northernmost of several Bogies.
		/// \throws std::out_of_range if idx is negative or 
		/// idx >= GetNumberOfBogies().
		virtual Bogie& GetBogie( int idx ) const = 0;


		/// \returns The number of topmost parents.
		virtual int GetNumberOfTopmostParents() const noexcept = 0;


		/// \brief Gets the topmost parent Bogie.
		///
		/// For rolling stock with jakobs bogies there can be 
		/// more than one parent Bogie.
		/// \param idx The index of the parent Bogie. 0 is the
		/// northernmost of several top level Bogies.
		/// \throws std::out_of_range if idx is negative or 
		/// idx >= GetNumberOfParents().
		virtual Bogie& GetTopmostParent( int idx = 0 ) const = 0;


		/// \returns The number of WheelFrames.
		virtual int GetNumberOfWheelFrames() const noexcept = 0;


		/// \brief Gets the WheelFrame at index, counting from north
		/// to south.
		/// \param idx The index of the WheelFrame. 0 is the northernmost.
		/// \throws std::out_of_range if idx is negative or
		/// idx >= GetNumberOfWheelFrame().
		virtual WheelFrame& GetWheelFrame( int idx ) const = 0;


		//Coupling:


		/// \brief Get the coupled RollingStock.
		/// \param end End type of this RollingStock.
		/// \throws std::logic_error if this object is not valid.
		/// \returns A pointer to the other RollingStock coupled at 
		/// end or nullptr if none, and the other RollingStock's end 
		/// type.	
		virtual std::pair<std::shared_ptr<RollingStock>,EndType> GetCoupledRollingStock( EndType end ) const = 0;


		// Jacks 'N Plugs:


		/// \brief Gets a Jack that pulses its Plug if the RailRunner's 
		/// Hoot() member is called
		virtual Jack& JackOnHoot() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the RailRunner's 
		/// Steam() member is called
		virtual Jack& JackOnSteam() noexcept = 0;
	};

};


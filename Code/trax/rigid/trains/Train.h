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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "TrainComponent.h"

namespace trax{

	struct SimulatorTrain;


	/// \brief A Train is a collection of TrainComponents that are coupled in a row.
	///
	/// A Train can be railed and moved on a track. It can be coupled with other
	/// Trains and can be split into two Trains. A Train can be reduced to a
	/// collection of RollingStock. If a Train splits by a breaking coupling
	/// the Train object dereferences all its components and becomes invalid.
	struct Train : TrainComponent{

		/// \brief Makes a Train object.
		static dclspc std::shared_ptr<Train> Make() noexcept;


		/// \brief Gets a shared pointer to this.
		virtual std::shared_ptr<Train> ThisTrain() const noexcept = 0;



		/// \returns The number of train components.
		virtual int GetNumberOfComponents() const noexcept = 0;


		/// \brief Gets the TrainComponent at index.
		/// \param index zero based Index of the TrainComponent.
		virtual std::shared_ptr<TrainComponent> GetComponent( int index ) const = 0;


		/// \returns the valid index of the component if it is part of this Train or -1.
		/// \param component The TrainComponent to search for.
		virtual int GetIndexOf( const TrainComponent& component ) const noexcept = 0;


		/// \brief Removes all Train objects from this Train and includes
		/// its components directly to this train.
		/// 
		/// The sub - Train objects loose their references and should be deleted.
		/// Their IsValid() method will return false. The method works recursively, 
		/// so the Train will become a Train of RollingStock. Reduce the sub - Trains 
		/// to keep a level of Trains.
		/// \see IsValid()
		virtual void Reduce() noexcept = 0;


		// Creation:
		/// \name Create
		/// \brief Creates a train from components
		///@{
		
		/// This method will find all already coupled components and build a Train from them.
		/// \param trainComponent The TrainComponent to make a Train from.
		/// \param orientation The orientation of the given components in the Train.
		/// location, prior to coupling.
		/// \returns True if the train was successfully created.
		virtual bool Create( 
			TrainComponent& trainComponent, 
			Orientation orientation = Orientation::Value::para ) = 0;

		/// This method takes a list of uncoupled TrainComponents and builds a Train from them.
		/// It will do the coupling of the elements properly.
		/// \param trainComponents A list of TrainComponents to make a Train from.
		/// \param orientation The orientation of the given components in the Train.
		/// location, prior to coupling.
		/// \param bMoveTo If true, the TrainComponents will be moved to a proper position.
		/// \param distance The distance to use between the TrainComponents in moving.
		/// \returns True if the train was successfully created.
		virtual bool Create( 
			const std::vector<std::pair<std::shared_ptr<TrainComponent>,Orientation>>& trainComponents, 
			Orientation orientation = Orientation::Value::para, 
			bool bMoveTo = true,
			DistanceType distance = DistanceType::actual ) = 0;
		///@}


		// Appending:

		/// \name Align
		/// \brief Positions the component at the respective Train end, but does not 
		/// couple it or include it in the train.
		///@{
		
		virtual void Align( EndType atEnd, TrainComponent& component, EndType withEnd ) const noexcept = 0;

		virtual void Align( EndType atEnd, TrainComponent& component, Orientation orientation ) const noexcept = 0;
		///@}


		/// \name Append
		/// \brief Appends a TrainComponent to this Train.
		///
		/// If this Train is railed, the component will be railed at the 
		/// respective end. If the Train is not railed, the component will be
		/// moved to the respective end. In both cases it gets coupled
		/// and becomes a part of this Train.
		///@{
		
		/// \param atEnd End of this Train to append to.
		/// \param pComponent The TrainComponent to append. Does nothing if nullptr.
		/// \param withEnd End of the TrainComponent to append.
		/// \throws std::invalid_argument if the TrainComponent is already part of a Train.
		/// \throws std::runtime_error if the TrainComponent is of unknown type.
		virtual void Append( EndType atEnd, std::shared_ptr<TrainComponent> pComponent, EndType withEnd ) = 0;

		/// \param atEnd End of this Train to append to.
		/// \param pComponent The TrainComponent to append. Does nothing if nullptr.
		/// \param orientation Orientation of the TrainComponent to append it in.
		/// \throws std::invalid_argument if the TrainComponent is already part of a Train.
		/// \throws std::runtime_error if the TrainComponent is of unknown type.
		virtual void Append( EndType atEnd, std::shared_ptr<TrainComponent> pComponent, Orientation orientation ) = 0;
		///@}
		


		virtual void Take( EndType atEnd, Train& from, EndType withEnd ) = 0;
		
		
		//Coupling:

		using TrainComponent::Couple;

		/// \brief Couples this Train with another at the respective ends.
		///
		/// It doesn't matter, wether the two couplings are near each other or not. 
		/// The coupling will try to maintain the actual distance of the two 
		/// Train's ends.
		/// The coupling attempt will fail if the two Trains are already coupled
		/// (with different partners or the same parters at different ends) or 
		/// if one of the couplings is deactivated. An coupling attempt will 
		/// also fail, when two couplings wear different coupling type indices. 
		/// \see Bogie::CouplingProps.
		/// \param end End of this Train.
		/// \param with Other Train.
		/// \param withEnd Other Train's end to couple with.
		/// \returns true if a new coupling was established or the two Trains
		/// were already properly coupled.
		virtual bool Couple( EndType end, Train& with, EndType withEnd ) noexcept = 0;


		/// \returns true if all the internal couplings of this Train are coupled
		/// propery.
		virtual bool IsUnCoupledInternally() const noexcept = 0;


		// \brief Get the coupled Train.
		// \param end End type of this Train.
		// \returns A pointer to the other Train coupled at end, or  
		// nullptr if none and the other Train's end type.	
	//	virtual std::pair<std::shared_ptr<Train>,EndType> GetCoupledTrain( EndType end ) const noexcept = 0;


		/// \brief Splits this Train into two Trains at the specified index.
		///
		/// Two new trains are created from the components, leaving the
		/// idxAt component at the first and idxAt+1 at the second fragment.
		///	This train becomes the parent train of the two new trains. Use
		/// Clear() to remove all components from this train.
		/// Note that the TrainComponents idxAt and idxAt+1 do not get
		/// uncoupled from each other.
		/// \param idxAt The index to split at. 0 is the first component.
		virtual std::pair<std::shared_ptr<Train>,std::shared_ptr<Train>> Split( int idxAt ) noexcept = 0;


		/// \brief Removes everything south of a broken internal coupling 
		/// from this Train, and returns a new Train with the components 
		/// that were removed. The new train as well as this train might
		/// be left with a single component; use Reduce() or Clear() to 
		/// remove redundant Trains.
		/// 
		/// \returns a new Train with the components that were removed or 
		/// nullptr if there was no broken coupling.
		virtual std::shared_ptr<Train> Separate() = 0;


		/// \brief Removes all components from this train.
		///
		/// Decouples all components and removes them from this train.
		virtual void Clear() noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if the a coupling
		/// inside the train is uncoupled for whatever reason.
		virtual struct Jack& JackOnSeparation() noexcept = 0;
	};


	inline RailRunner::EndType Front( Orientation orientation ) noexcept{
		return orientation ? RailRunner::EndType::north : RailRunner::EndType::south;
	}

	inline RailRunner::EndType Back( Orientation orientation ) noexcept{
		return orientation ? RailRunner::EndType::south : RailRunner::EndType::north;
	}

	inline RailRunner::EndType Front( const TrainComponent& trainComponent ) noexcept{
		return Front( trainComponent.GetOrientation() );
	}

	inline RailRunner::EndType Back( const TrainComponent& trainComponent ) noexcept{
		return Back( trainComponent.GetOrientation() );
	}

}

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


		/// \brief Gets the TrainComponent at zero based index.
		/// \param index zero based Index of the TrainComponent.
		virtual std::shared_ptr<TrainComponent> GetComponent( int index ) const = 0;


		/// \returns the valid zero based index of the component if it is part of this Train or -1.
		/// \param component The TrainComponent to search for.
		virtual int GetIndexOf( const TrainComponent& component ) const noexcept = 0;


		// Creation:

		/// \name Create
		/// \brief Creates a train from components
		///@{
		
		/// This method will find all already coupled components and build a Train 
		/// from them.
		/// \param trainComponent The TrainComponent to make a Train from.
		/// \param orientation The orientation of the given components in the Train.
		/// location, prior to coupling.
		/// \returns True if the train was successfully created.
		virtual bool Create( 
			TrainComponent& trainComponent, 
			Orientation orientation = Orientation::Value::para ) = 0;

		/// This method takes a list of uncoupled TrainComponents and builds a Train 
		/// from them. It will do the coupling of the elements properly.
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
		/// 
		/// If this Train is railed, the component will be railed at the respective 
		/// end; if the Train is not railed, the component will be moved to the 
		/// respective end.	
		/// \throws std::exception if there is an error in railing the component.
		/// \throws std::logic_error if there are invalid components.
		///@{
		
		/// \param atEnd End of this Train to append to.
		/// \param component The TrainComponent to align.
		/// \param withEnd End of the TrainComponent to align with.
		virtual void Align( EndType atEnd, TrainComponent& component, EndType withEnd ) const = 0;

		/// \param atEnd End of this Train to append to.
		/// \param component The TrainComponent to align.
		/// \param orientation The orientation of the TrainComponent to align it in.
		virtual void Align( EndType atEnd, TrainComponent& component, Orientation orientation ) const = 0;
		///@}


		/// \name Append
		/// \brief Appends a TrainComponent to this Train.
		/// 
		/// The component gets coupled (if bCouple) and becomes a part of this Train, 
		/// but it will not get moved or railed. Use Align() to position or rail it 
		/// properly first.
		/// \param atEnd End of this Train to append to.
		/// \param pComponent The TrainComponent to append. Does nothing if nullptr.
		/// \param bCouple if true, the omponent gets coupled properly, if false the
		/// coupling state remains unchanged.
		/// \param withEnd End of the TrainComponent to append.
		/// \param orientation Orientation of the TrainComponent to append it in.
		/// \throws std::invalid_argument if the TrainComponent is already part of 
		/// a Train.
		/// \throws std::runtime_error if the TrainComponent is of unknown type.
		///@{
		
		virtual void Append( EndType atEnd, std::shared_ptr<TrainComponent> pComponent, EndType withEnd, bool bCouple = true ) = 0;

		virtual void Append( EndType atEnd, std::shared_ptr<TrainComponent> pComponent, Orientation orientation, bool bCouple = true ) = 0;
		///@}
		

		/// \brief Takes all components from another Train.
		/// 
		/// Takes all components from another Train and Append()s them to this Train.
		/// As with Append() the components are not railed or moved, but get coupled 
		/// if not already so. This methods does not change the poses of the components.
		/// \param atEnd End of this Train to append to.
		/// \param fromTrain The Train to take components from.
		/// \param withEnd End of the fromTrain to take components from.
		virtual void Take( EndType atEnd, Train& fromTrain, EndType withEnd ) = 0;


		/// \brief Splits this Train into two Trains at the specified index.
		///
		/// Two new trains are created from the components, leaving the
		/// idxAt component at the first and idxAt+1 at the second fragment.
		///	This train becomes the parent train of the two new trains. 
		/// Note that the TrainComponents idxAt and idxAt+1 do not get
		/// uncoupled from each other; use Clear() to remove all components 
		/// from this train and decouple them.
		/// \param idxAt The zero based index to split at. 0 is the first 
		/// component.
		/// \returns A pair of shared pointers to the two new trains. Will 
		/// be nullptrs if idxAt is out of range.
		virtual std::pair<std::shared_ptr<Train>,std::shared_ptr<Train>> SplitAfter( int idxAt ) = 0;


		//Removing:

		/// \brief Removes everything south of a broken internal coupling 
		/// from this Train, and returns a new Train with the components 
		/// that were removed. 
		/// 
		/// No coupling or railing as well as no change of poses is done. 
		/// The new train as well as this train might be left with a single 
		/// component; use Reduce() or Clear() to remove redundant Trains.
		/// 
		/// \returns a new Train with the components that were removed or 
		/// nullptr if there was no broken coupling.
		virtual std::shared_ptr<Train> Separate() = 0;


		/// \brief Removes TrainComponents from this Train and includes them directly 
		/// to the parent train. This Train will become empty and !IsValid().
		/// 
		/// Does nothing if this Train has no parent Train. The coupling and railing 
		/// states as well as the poses of the components remain unchanged.
		/// \param bRecursive if true, the method will be applied recursively to all 
		/// sub-components, leaving the Train reduced to its RollingStock components 
		/// only.
		virtual void Reduce( bool bRecursive = true ) noexcept = 0;


		/// \brief Removes all components from this train.
		///
		/// Decouples all components and removes them from this train.
		/// It leaves subtrains intact.
		virtual void Clear() noexcept = 0;


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
		/// \param thisEnd End of this Train.
		/// \param with Other Train.
		/// \param withEnd Other Train's end to couple with.
		/// \returns true if a new coupling was established or the two Trains
		/// were already properly coupled.
		virtual bool Couple( EndType thisEnd, Train& with, EndType withEnd ) noexcept = 0;


		/// \returns false if all the internal couplings of this Train (inluding 
		/// that of sub-Trains) are coupled propery. True if one such coupling
		/// is broken or uncoupled.
		virtual bool IsUnCoupledInternally() const noexcept = 0;


		// \brief Get the coupled Train.
		// \param end End type of this Train.
		// \returns A pointer to the other Train coupled at end, or  
		// nullptr if none and the other Train's end type.	
	//	virtual std::pair<std::shared_ptr<Train>,EndType> GetCoupledTrain( EndType end ) const noexcept = 0;


		/// \brief Gets a Jack that pulses its Plug if a coupling inside the 
		/// train (including all sub-Trains) is uncoupled for whatever reason.
		virtual struct Jack& JackOnUnCoupleInternal() noexcept = 0;
	};


	/// \defgroup Group_RailRunnerEndHelpers RailRunner's End Helpers
	/// \brief Helper functions to get the front and back ends of an oriented element.

	/// \name End Helpers
	/// @{
	
	/// \brief Get the front end of an oriented element.
	/// \ingroup Group_RailRunnerEndHelpers
	/// \param orientation The orientation of the element.
	/// \returns The end type corresponding to the given orientation.
	RailRunner::EndType Front( Orientation orientation ) noexcept;

	/// \brief Get the back end of an oriented element.
	/// \ingroup Group_RailRunnerEndHelpers
	/// \param orientation The orientation of the element.
	/// \returns The end type corresponding to the given orientation.
	RailRunner::EndType Back( Orientation orientation ) noexcept;

	/// \brief Get the front end of a TrainComponent according to its orientation in its Train.
	/// \ingroup Group_RailRunnerEndHelpers
	/// \param trainComponent The TrainComponent to get the end for according to its orientation in its Train.
	/// \returns The end type corresponding to the given orientation.
	RailRunner::EndType Front( const TrainComponent& trainComponent ) noexcept;

	/// \brief Get the back end of a TrainComponent according to its orientation in its Train.
	/// \ingroup Group_RailRunnerEndHelpers
	/// \param trainComponent The TrainComponent to get the end for according to its orientation in its Train.
	/// \returns The end type corresponding to the given orientation.
	RailRunner::EndType Back( const TrainComponent& trainComponent ) noexcept;

	///@}



// inlines:
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

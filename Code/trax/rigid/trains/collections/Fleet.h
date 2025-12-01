//	trax track library
//	AD 2014 
//
//  "If you want to be a hero
//   well just follow me."
//
//				John Lennon
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
#include "trax/collections/Collection.h"

#include "trax/Simulated.h"
//#include "trax/collections/CollectionDecorator.h"

namespace trax{

//	struct DepotCollection;
//	struct Materials;
	struct Train;


	/// \brief A Fleet holds Trains and (topmost) Bogies via their common interface
	/// RailRunner.
	struct Fleet : Collection<Fleet,Train>,
	               Simulated
	{
		/// \brief Makes a standard TrackCollection object.
		static dclspc std::unique_ptr<Fleet> Make() noexcept;


		///// \brief Couple two Bogie's ends, contained in this Fleet.
		/////
		///// The Bogies are coupled in the distance they are at the time of 
		///// coupling and the coupling will try to maintain this distance.
		///// \param coupling Reference values for Bogie's ends.
		//virtual void Couple( const Bogie::Coupling& coupling ) const = 0;


		///// \brief Gets the Train for a Bogie, if there is any.
		/////
		///// \param pRailRunner Pointer to a Bogie to get the train for. Or to a train to convert
		///// the pointer type.
		///// \returns A ponter to the train, pRailRunner is contained in or a coverted pointer if
		///// pRailRunner is in fact a Train.
		//virtual Train* GetTrain( const RailRunner* pRailRunner ) const = 0;


		///// \brief Searches for a RailRunner by name.
		/////
		///// The method includes the depots into the search.
		///// \returns The first RailRunner with the given name to be found. 
		//virtual std::shared_ptr<RailRunner> GetAny( const std::string& name ) const = 0;


		/// \brief Enables or disables generation of trains due to decoupling.
		/// \param bEnable true to enable, false to disable.
		virtual void EnableTrainGeneration( bool bEnable = true ) noexcept = 0;


		/// \returns true if the train generation feature is enabled.
		virtual bool IsTrainGenerationEnabled() const noexcept = 0;





		///// \brief Attaches a DepotCollection to this Fleet.
		///// \param pDepotCollection Pointer to object to attach or nullptr to remove.
		///// \returns Pointer to previously attached object.
		//virtual std::shared_ptr<DepotCollection> Attach( std::shared_ptr<DepotCollection> pDepotCollection ) = 0;


		///// \returns A pointer to the attached DepotCollection.
		//virtual DepotCollection* GetDepotCollection() const noexcept = 0;


		///// \brief Called from the engine if a collision with the Bogie has happened.
		/////
		///// Override Fleet by using a decorator to receive these events.
		///// \param pBogie Pointer to Bogie that has received the hit. The Bogie will be
		///// a member of the Fleet.
		///// \param hitData Position and impulse of the hit in coordinates relative to
		///// the global frame. The impulse is delivered to the item in one time frame,
		///// dividing it by fixed_timestep_seconds would yield the active force.
		///// \param filterCombination The combined filter bitpattern of the two colliding objects.
		///// \param materialCombination The two materials that are involved in the hit.
		//virtual void Hit( const Bogie* pBogie, const spat::VectorBundle<Real>& hitData, int filterCombination, const Materials& materialCombination ) = 0;
	};


	///// \brief Adds the RailRunner and all its sub - RailRunners to the fleet.
	/////
	///// Only the RailRunner the function is called for will be put on the stack.
	///// \param fleet Fleet to add the RailRunner to.
	///// \param pRailRunner Shared pointer to a train or a bogie.
	//void AddRecursive( Fleet& fleet, std::shared_ptr<RailRunner> pRailRunner );


	///// \brief Removes the RailRunner and all its sub - RailRunners from the fleet.
	/////
	///// \param fleet Fleet to remove the RailRunner from.
	///// \param pRailRunner Pointer to a train or a bogie.
	//void RemoveRecursiveFrom( Fleet& fleet, RailRunner& railRunner );


	/// \brief A decorator for Fleet.
	///
	/// With trax decorators can get used to augment trax objects with additional 
	/// behaviour and services. Derive your own class from the decorator and overwrite
	/// its methods. Create your decorator with the trax object and supply it to 
	/// the library instead of the original object.
/*	class FleetDecorator : public CollectionDecorator<Decorator<Fleet>>{
	public:
		/// \brief Type of the decoree.		
		typedef Fleet ComponentType;


		/// \brief Constructs the decorator.
		/// \param pFleet Pointer to object to decorate.
		FleetDecorator( std::shared_ptr<Fleet> pFleet )
			: CollectionDecorator<Decorator<Fleet>>{ pFleet }
		{}
		
		/// \name Forwarding
		/// \brief Fleet member function forwarding.
		//@{
		const Module* GetModule() const override{
			return m_pComponent->GetModule();
		}

		void Update() override{
			return m_pComponent->Update();
		}

		void Couple( const Bogie::Coupling& coupling ) const override{
			return m_pComponent->Couple( coupling );
		}

		Train* GetTrain( const RailRunner* pRailRunner ) const override{
			return m_pComponent->GetTrain( pRailRunner );
		}

		std::shared_ptr<RailRunner> GetAny( const std::string& name ) const override{
			return m_pComponent->GetAny( name );
		}

		void EnableDerailing( bool bEnable ) override{
			return m_pComponent->EnableDerailing( bEnable );
		}

		bool IsDerailingEnabled() const override{
			return m_pComponent->IsDerailingEnabled();
		}

		void EnableCouplingForceLimits( bool bEnable ) override{
			return m_pComponent->EnableCouplingForceLimits( bEnable );
		}

		bool IsCouplingForceLimitsEnabled() const override{
			return m_pComponent->IsCouplingForceLimitsEnabled();
		}

		std::shared_ptr<DepotCollection> Attach( std::shared_ptr<DepotCollection> pDepotCollection ) override{
			return m_pComponent->Attach( pDepotCollection );
		}

		DepotCollection* GetDepotCollection() const noexcept override{
			return m_pComponent->GetDepotCollection();
		}

		void Hit( const Bogie* pBogie, const spat::VectorBundle<Real>& hitData, int filterCombination, const Materials& materialCombination ) override{
			return m_pComponent->Hit(pBogie,hitData,filterCombination,materialCombination);
		}
		//@}
	};*/
}

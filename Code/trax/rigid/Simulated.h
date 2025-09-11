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
// For additional permissions, please contact: horstmann.marc@trendverlag.de

#pragma once

#include "trax/Units.h"


namespace trax{

	struct Scene;

	/// \brief A simulated object. Register the object with a scene
	/// to get called back for Update, Pause, Resume and Stop.
	struct Simulated
	{
		/// \brief Called if the simulation is started.
		/// \param scene The scene the simulation is started in.
		/// \returns true if the simulated object wants to get
		/// called back for Update, Pause, Resume and Stop.
		virtual bool Start( Scene& scene ) = 0;


		/// \brief Called to update the simulated object.
		/// \param dt The time step to update the object with.
		virtual void Update( Time dt ) = 0;


		virtual void Pause() noexcept = 0;

		virtual void Resume() noexcept = 0;

		virtual void Stop() noexcept = 0;


		virtual ~Simulated() = default;
	};


	template<class BaseDecorator>
	class SimulatedDecorator : public BaseDecorator{
	protected:
		using BaseDecorator::m_pComponent;
	public:
		/// \brief Type of the interface the decorator is decorating.
		typedef typename BaseDecorator::InterfaceType InterfaceType;

		/// \brief Decorator constructor.
		/// \param pComponent Pointer to object that is to be decorated.
		SimulatedDecorator( std::shared_ptr<InterfaceType> pComponent )
			: BaseDecorator{ pComponent }
		{}

		bool Start( Scene& scene ) override{
			return m_pComponent->Start( scene );
		}

		void Update( Time dt ) override{
			m_pComponent->Update( dt );
		}

		void Pause() noexcept override{
			m_pComponent->Pause();
		}

		void Resume() noexcept override{
			m_pComponent->Resume();
		}

		void Stop() noexcept override{
			m_pComponent->Stop();
		}
	};
}
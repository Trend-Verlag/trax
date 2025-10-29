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

#include "../Scene.h"
#include "trax/ObjectID.h"
#include "trax/source/Plug_Imp.h"

namespace trax{

	class Scene_Imp :	public virtual ObjectID_Imp<Scene>,
						public PlugEnumerator,
						public JackEnumerator{
	public:
		Scene_Imp();

		const char*	TypeName() const noexcept override{
			return "Scene";
		}

		void Register( Simulated& simulated ) override;

		void Unregister( const Simulated& simulated ) override;

		void UnregisterAllSimulated() override;

		void Update( Time dt = fixed_timestep ) noexcept override;

		void Step( Time dt = fixed_timestep ) noexcept override;

		void Loop() noexcept override;

		void Loop( Time forTimePeriod ) noexcept override;

		void Stop() noexcept override;

		using Scene::CreateGestalt;

		std::unique_ptr<Gestalt> CreateGestalt( spat::Box<Length> box, Mass mass ) const noexcept override;

		using Scene::CreateGeom;

		std::unique_ptr<Geom> CreateGeom( GeomType type ) const override;
	
		Jack& JackOnSimulationStep() noexcept override;

		MultiPlug& PlugToStop() noexcept override;

		// Inherited via PlugEnumerator
		int CountPlugs() const override;

		// Inherited via JackEnumerator
		int CountJacks() const noexcept override;
	protected:
		const Jack& _GetJack( int idx ) const override;
		const Plug& _GetPlug( int idx ) const override;

	private:
		bool m_bLoopRunning = false;
		Time m_LoopTime = 0.0_s;
		std::vector<Simulated*> m_Simulated;
	
		Jack_Imp m_JackOnSimulationStep{ "JackOnSimulationStep" };
		MultiPlug_Imp<Plug_Imp_ParentPointer<Scene_Imp>> m_PlugToStop;
	};
}

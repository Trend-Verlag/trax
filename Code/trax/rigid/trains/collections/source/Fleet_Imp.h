//	trax track library
//	AD 2014 
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

#include "../Fleet.h"

#include "trax/rigid/trains/RailRunner.h"
#include "trax/ImplementationHelper.h"
#include "trax/Plug.h"

namespace trax{

	
	typedef Container_Imp<Train,Fleet> Fleet_Base;

	class Fleet_Imp : public Fleet_Base{
	public:
		Fleet_Imp() noexcept;

		const char* TypeName() const override;


		// Collection:
		IDType Add( std::shared_ptr<Train> pTrain ) override;
		
		IDType AddRelaxed( std::shared_ptr<Train> pTrain ) override;

		bool Remove( Train* pTrain, bool zeroIDs = false ) override;

		void Clear() noexcept override;

	/*	using Fleet_Base::Get;

		std::shared_ptr<Train> Get( const std::string& name ) const override;*/


		// Fleet:
		void EnableTrainGeneration( bool bEnable = true ) noexcept override;

		bool IsTrainGenerationEnabled() const noexcept override;



		//std::shared_ptr<DepotCollection> Attach( std::shared_ptr<DepotCollection> pDepotCollection ) noexcept override;

		//DepotCollection* GetDepotCollection() const noexcept override;

		//void Hit( const Bogie* /*pBogie*/, const VectorBundle<Real>& /*hitData*/, int /*filterCombination*/, const Materials& /*materialCombination*/ ) noexcept override{}


		// Simulated:
		bool Start( Scene& scene ) override;

		void Update( Time dt ) override;

		void Pause() noexcept override;

		void Resume() noexcept override;

		void Stop() noexcept override;

	private:
		std::vector<struct Bogie*> m_Bogies;
		bool m_bTrainGenerationEnabled;

		void RegisterBogies( const Train& train ) noexcept;
		void UnregisterBogies( const Train& train ) noexcept;

		class FPlugEnumerator : public trax::PlugEnumerator
		{
		public:
			FPlugEnumerator( Fleet_Imp& parent, Train& train );

			bool operator==( const Train& train ) const noexcept{
				return &m_Train == &train;
			};

			int CountPlugs() const override{
				return 1;
			};

		protected:
			const trax::Plug& _GetPlug( int idx ) const override;

		private:
			Fleet_Imp& m_Parent;
			Train& m_Train;

			void PulseToSeparate();

			trax::Plug_Imp_ParentPointer<FPlugEnumerator> m_PlugToSeparate;
		};

		std::vector<std::unique_ptr<FPlugEnumerator>> m_PlugEnumerators;
		std::vector<Train*> m_TrainsSeparated;
		void SeparateTrains();
		void ProduceCommonTrain( const Bogie& bogieA, RailRunner::EndType endA, const Bogie& bogieB, RailRunner::EndType endB );
	};
}
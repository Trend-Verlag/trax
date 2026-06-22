//	trax track library
//	AD 2026 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
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

#include "dim/DimensionedValues.h"
#include "spat/Frame.h"
#include <ostream>

namespace trax{
	using namespace dim;

	template< class Base, class TypeToSync >
	class FrameSync_Imp : public Base
	{
	public:
		FrameSync_Imp();


		// Pose_Imp:
		void PropagateAbsoluteFrameToClients() noexcept override;


		// Track:
		bool IsValid() const noexcept override;

		bool Diagnose( std::ostream& os ) const noexcept override;


		// Base:
		void UpdateTrackPose() noexcept override;

	protected:
		std::shared_ptr<TypeToSync>	m_pSyncee;

		void ReadRelativePose();
	private:
		spat::Frame<Length,One>		m_RelativePose;
		bool 						m_bFramePropagationToSynceeOnSetFrame;
	};


	template<class Base, class TypeToSync>
	inline FrameSync_Imp<Base, TypeToSync>::FrameSync_Imp()
		: m_RelativePose						{ spat::Identity<Length,One> }
		, m_bFramePropagationToSynceeOnSetFrame	{ true }
	{}

	template<class Base, class TypeToSync>
	inline void FrameSync_Imp<Base,TypeToSync>::PropagateAbsoluteFrameToClients() noexcept
	{
		Base::PropagateAbsoluteFrameToClients();

		if( m_bFramePropagationToSynceeOnSetFrame && m_pSyncee )
		{
			spat::Frame<Length, One> synceeFrame = Base::GetFrame();
			m_RelativePose.ToParent( synceeFrame );
			m_pSyncee->SetFrame( synceeFrame );
		}
	}

	template<class Base, class TypeToSync>
	inline bool FrameSync_Imp<Base,TypeToSync>::IsValid() const noexcept
	{
		if( !Base::IsValid() )
			return false;

		if( m_pSyncee )
		{
			spat::Frame<Length, One> synceeFrame, relative = m_RelativePose;
			m_pSyncee->GetFrame( synceeFrame );
			synceeFrame.ToParent( relative );
			return relative.Equals( Base::GetFrame(), epsilon__length, epsilon__angle );
		}

		return true;	
	}

	template<class Base, class TypeToSync>
	inline bool FrameSync_Imp<Base,TypeToSync>::Diagnose( std::ostream& os ) const noexcept
	{
		if( !Base::Diagnose( os ) )
			return false;

		if( m_pSyncee )
		{
			spat::Frame<Length, One> synceeFrame, relative = m_RelativePose;
			m_pSyncee->GetFrame( synceeFrame );
			synceeFrame.ToParent( relative );
			if( !relative.Equals( Base::GetFrame(), epsilon__length, epsilon__angle ) )
			{
				os << "FrameSync_Imp::Diagnose: Track frame does not match syncee frame + relative pose." << std::endl;
				return false;
			}
		}
		else
		{
			os << "FrameSync_Imp::Diagnose: No syncee assigned." << std::endl;
		}

		return true;
	}

	template<class Base, class TypeToSync>
	inline void FrameSync_Imp<Base, TypeToSync>::UpdateTrackPose() noexcept
	{
		if( m_pSyncee )
		{
			spat::Frame<Length,One> synceeFrame, relative = m_RelativePose;
			m_pSyncee->GetFrame( synceeFrame );
			synceeFrame.ToParent( relative );
			common::FlagBlocker fb{ m_bFramePropagationToSynceeOnSetFrame };
			Base::SetFrame( synceeFrame );
		}
	}

	template<class Base, class TypeToSync>
	inline void FrameSync_Imp<Base, TypeToSync>::ReadRelativePose()
	{
		if( m_pSyncee )
		{
			spat::Frame<Length,One> synceeFrame;
			m_pSyncee->GetFrame( synceeFrame );
			m_RelativePose = Base::GetFrame();
			synceeFrame.FromParent( m_RelativePose );
		}
	}
}
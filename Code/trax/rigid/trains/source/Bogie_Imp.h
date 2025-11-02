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

#include "../Bogie.h"

#include "trax/ObjectID.h"
#include "trax/source/Plug_Imp.h"
#include "RailRunner_Imp.h"

#include<memory>

namespace trax{

	struct DistanceJoint;
	struct HingeJoint;

	typedef RailRunner_Imp<ObjectID_Imp<Bogie>> Bogie_Base;


	class Bogie_Imp : public virtual Bogie_Base//,
						//public PlugEnumerator
	{
	public:
		Bogie_Imp( Scene& scene, std::shared_ptr<Gestalt> pGestalt );
		Bogie_Imp( const Bogie_Imp& ) = delete;
	//	Bogie_Imp( Bogie_Imp&& bogie ) noexcept;
		~Bogie_Imp() noexcept;

		Bogie_Imp& operator=( const Bogie_Imp& ) = delete;
		Bogie_Imp& operator=( Bogie_Imp&& ) = delete;

		void SetWeakPointerToSelf( std::weak_ptr<RailRunner_Imp> pThis ) noexcept;


		// Simulated:
		bool Start( Scene& scene ) noexcept override;

		void Idle() noexcept override;

		void Update( Time dt ) noexcept override;

		void Pause() noexcept override;

		void Resume() noexcept override;

		void Stop() noexcept override;


		// Identified<RailRunner>:
		using Bogie_Base::Reference;

		void Reference( const std::string& name, const std::string& reference ) override;


		// RailRunner:
		const char* TypeName() const noexcept override;

		bool IsValid() const noexcept override;

		void Rail( const Location& location, bool bMoveTo = true ) override;

		Location GetLocation() const noexcept override;
		
		void DeRail() noexcept override;

		bool IsRailed() const noexcept override;

		spat::Frame<Length,One> GetGlobalAnchor() const override;

		void ResetToGlobalAnchor( const spat::Frame<Length,One>& anchor ) override;

		void SetVelocity( Velocity velocity ) override;

		Velocity GetVelocity() const override;

		void TargetVelocity( Velocity tvelocity ) override;

		Velocity TargetVelocity() const noexcept override;

		EndType TargetDirection() const noexcept override;

		Power MaximumPowerOutput() const noexcept override;

		Power CurrentPowerOutput() const noexcept override;

		Velocity MaximumVelocity() const noexcept override;

		using Bogie_Base::Thrust;

		void Thrust( One byfactor ) override;

		Force ThrustAbsolute() const noexcept override;

		Force MaxThrust() const noexcept override;

		Force MaxThrust( Velocity velocity ) const noexcept override;

		using Bogie_Base::Brake;

		void Brake( One byfactor ) override;

		Force BrakeAbsolute() const noexcept override;

		Force MaxBrake() const noexcept override;

		void ActivateCoupling( EndType end ) override;

		void DeactivateCoupling( EndType end ) override;

		bool IsActivated( EndType end ) const noexcept override;

		bool Uncouple( EndType end, bool btriggerPulses = true ) noexcept override;

		bool IsCoupled( EndType end = EndType::any ) const noexcept override;

		Length GetCouplingHeight( EndType end ) const noexcept override;

		// Bogie:
		std::shared_ptr<Bogie> ThisBogie() const noexcept override;

		RollingStock* GetRollingStock() const noexcept override;

		Gestalt& GetGestalt() const noexcept final; //final: GetGestalt() used in Joint::SetBogie via Bogie_Imp move constructor.

		void Attach( std::shared_ptr<Bogie> pChildBogie, EndType atEnd, const spat::Frame<Length,One>& swivelPose ) override;

		void Attach( std::shared_ptr<Bogie> pChildBogie, EndType atEnd ) override;

		std::pair<std::shared_ptr<Bogie>,spat::Frame<Length,One>> GetChild( EndType atEnd ) const noexcept override;

		std::pair<std::shared_ptr<Bogie>,spat::Frame<Length,One>> GetParent( EndType atEnd = EndType::any ) const noexcept override;

		std::shared_ptr<Bogie> GetFirst() const noexcept override;

		std::shared_ptr<Bogie> GetNext() const noexcept override;
	
		std::shared_ptr<Bogie> GetPrevious() const noexcept override;

		std::shared_ptr<Bogie> GetLast() const noexcept override;

		void Detach( EndType atEnd = EndType::all ) noexcept override;
				
		bool Couple( EndType end, Bogie& with, EndType withEnd, bool btriggerPulses = true ) noexcept override;

		std::pair<std::shared_ptr<Bogie>,EndType> GetCoupledBogie( EndType end ) const noexcept override;
		
		void SetCouplingProps( EndType atEnd, const CouplingProps& props ) override;

		CouplingProps GetCouplingProps( EndType atEnd ) const noexcept override;

		spat::Sphere<Length> GetCoupling( EndType end ) const noexcept override;

		void SetCouplingHeight( EndType end, Length height ) override;

		bool BreakCoupling( EndType atEnd, bool btriggerPulses = true ) noexcept override;


		// Jacks 'N Plugs:
		Jack& JackOnCouple( EndType end ) override;

		Jack& JackOnUnCouple( EndType end ) override;

		Jack& JackOnCouplingActivated( EndType end ) override;

		Jack& JackOnCouplingDeactivated( EndType end ) override;

		Jack& JackOnMotorStart() noexcept override;

		Jack& JackOnMotorStop() noexcept override;

		Jack& JackOnBrakeApply() noexcept override;

		Jack& JackOnBrakeRelease() noexcept override;

		Jack& JackOnCouplingForceLimitExceeded( EndType end ) override;
	
		Jack& JackOnNorthSwivelBendingIn() noexcept override;

		Jack& JackOnNorthSwivelBendingOut() noexcept override;

		Jack& JackOnSouthSwivelBendingIn() noexcept override;

		Jack& JackOnSouthSwivelBendingOut() noexcept override;

		// Inherited via JackEnumerator
		int CountJacks() const noexcept override;

		// Inherited via PlugEnumerator
	//	int CountPlugs() const override;


		// Bogie_Imp:
		void SetRollingStock( RollingStock* pRollingStock ) noexcept;

	protected:
		const Jack& _GetJack( int idx ) const override;
	//	const Plug& _GetPlug( int idx ) const override;

		Scene& m_Scene;

		void RailChildBogies( const trax::Location& location, bool bMoveTo );
		void SetGlobalAnchorToChildBogies( const spat::Frame<Length,One>& anchor );
		void AlignToChildBogies();

		template<typename JointType>
		class BogieJoint{
		public:
			BogieJoint(	std::unique_ptr<JointType> pJoint,
						Bogie_Imp& bogieParent, 
						Bogie_Imp& bogieChild )
				: m_pJoint( std::move( pJoint ) )
				, m_BogieA( bogieParent )
				, m_BogieB( bogieChild )
			{
			}

			void SetName( const char* name ) noexcept{
				m_pJoint->SetName( name );
			}

			std::shared_ptr<Bogie_Imp> BogieA() const noexcept{
				return std::dynamic_pointer_cast<Bogie_Imp>(m_BogieA.This());
			}

			spat::Frame<Length,One> LocalPoseA() const noexcept{
				return m_pJoint->LocalPoseA();
			}

			std::shared_ptr<Bogie_Imp> BogieB() const noexcept{
				return std::dynamic_pointer_cast<Bogie_Imp>(m_BogieB.This());
			}

			spat::Frame<Length, One> LocalPoseB() const noexcept{
				return m_pJoint->LocalPoseB();
			}
		protected:
			Bogie_Imp& m_BogieA;
			Bogie_Imp& m_BogieB;
			std::unique_ptr<JointType> m_pJoint;
		private:
			Real m_Engine_meters_per_unit = 1.0f;
		};

		class BogieSwivel : public BogieJoint<HingeJoint>{
		public:
			BogieSwivel(	const Scene& scene,
							Bogie_Imp& bogieParent, 
							const spat::Frame<Length, One>& poseParent, 
							Bogie_Imp& bogieChild, 
							const spat::Frame<Length, One>& poseChild );

			spat::VectorBundle<Length,One> LocalAxisA() const noexcept;

			spat::VectorBundle<Length,One> LocalAxisB() const noexcept;

			Angle GetBendAngle() const noexcept;
		private:
		};

		class BogieCoupling : public BogieJoint<DistanceJoint>{
		public:
			BogieCoupling(	const Scene& scene,
							Bogie_Imp& bogieParent, 
							const spat::Frame<Length, One>& poseParent, 
							Bogie_Imp& bogieChild, 
							const spat::Frame<Length, One>& poseChild );

			std::shared_ptr<Bogie_Imp> GetCoupledBogie( const Bogie_Imp& toBogie ) const noexcept;

			Length GetLength() const noexcept;

			void SetLength( Length length ) noexcept;

			spat::Vector<Force> GetForce() const noexcept;
		};

		std::shared_ptr<BogieSwivel>	m_pSwivelParentNorth, m_pSwivelParentSouth;
		std::shared_ptr<BogieSwivel>	m_pSwivelChildNorth, m_pSwivelChildSouth;
		std::shared_ptr<Bogie_Imp>		m_pBogieChildNorth, m_pBogieChildSouth;		// keep references to child bogies (but not to parent) to prevent them from being deleted.

		virtual Length AnchorToParentNorth() const noexcept;
		virtual Length AnchorToParentSouth() const noexcept;
		virtual Length AnchorToChildSouth() const noexcept;
		virtual Length AnchorToChildNorth() const noexcept;
	private:
		spat::Position<Length> GetLocalAnchor() const;

		// Coupling:
		struct CouplingProps_Ext : CouplingProps{
			CouplingProps_Ext( const Bogie_Imp& parentBogie ) noexcept;
			const Bogie_Imp& ParentBogie;
			std::shared_ptr<BogieCoupling> pCoupling;
			bool bActivated = false;
			Momentum ActualBreakingSpareMomentum = BreakingSpareMomentum;

			CouplingProps_Ext& operator=( const CouplingProps& props ) noexcept;

			bool CheckCoupling( Time dt ) noexcept;
			bool Uncouple( bool btriggerPulses = true ) noexcept;
			bool Break( bool btriggerPulses = true ) noexcept;

			Jack_Imp m_JackOnCouple					{ "JackOnCouple" };
			Jack_Imp m_JackOnUnCouple				{ "JackOnUnCouple" };
			Jack_Imp m_JackOnCouplingActivated		{ "JackOnCouplingActivated" };
			Jack_Imp m_JackOnCouplingDeactivated	{ "JackOnCouplingDeactivated" };
			Jack_Imp m_JackOnForceLimitExceeded		{ "JackOnCouplingForceLimitExceeded" };
		} m_CouplingNorth, m_CouplingSouth;

		MultiPlugJack_Imp m_JackOnCouple				{ "JackOnCouple" };
		MultiPlugJack_Imp m_JackOnUnCouple				{ "JackOnUnCouple" };
		MultiPlugJack_Imp m_JackOnCouplingActivated		{ "JackOnCouplingActivated" };
		MultiPlugJack_Imp m_JackOnCouplingDeactivated	{ "JackOnCouplingDeactivated" };
		MultiPlugJack_Imp m_JackOnForceLimitExceeded	{ "JackOnCouplingForceLimitExceeded" };


		std::shared_ptr<Gestalt> m_pGestalt;
		RollingStock* m_pRollingStock;

		//Todo: all the Joint stuff could be moved here...
		Jack_Imp m_JackOnMotorStart				{ "JackOnMotorStart" };
		Jack_Imp m_JackOnMotorStop				{ "JackOnMotorStop" };
		Jack_Imp m_JackOnBrakeApply				{ "JackOnBrakeApply" };
		Jack_Imp m_JackOnBrakeRelease			{ "JackOnBrakeRelease" };
		Jack_Imp m_JackOnNorthSwivelBendingIn	{ "JackOnNorthSwivelBendingIn" };
		Jack_Imp m_JackOnNorthSwivelBendingOut	{ "JackOnNorthSwivelBendingOut" };
		Jack_Imp m_JackOnSouthSwivelBendingIn	{ "JackOnSouthSwivelBendingIn" };
		Jack_Imp m_JackOnSouthSwivelBendingOut	{ "JackOnSouthSwivelBendingOut" };

		bool m_bNorthSwivelBending;
		bool m_bSouthSwivelBending;
		static const Angle scm_BendingAngle;


		EndType m_TargetDirection;

				spat::Vector<One> LocalVerticalDirection() const noexcept;
		virtual spat::Vector<One> LocalHorizontalDirection() const noexcept;
	};

}


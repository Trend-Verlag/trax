//	trax track library
//	AD 2022 
//
//  "Und liegt ein Koerper regungslos im Schnee,
//   schmeckt erst so richtig der Kaffee."
//
//								Reinhard Fendrich
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

#include "TrackJoint.h"
#include "Location.h"
#include "Wheelset.h"
#include "TrackJointLimits.h"
#include "Jack.h"

namespace trax{

	struct TractionForceCharacteristic;

	/// \brief Class with minimal functionality to support a track joint 
	/// on a track system.
	class TrackJointFeeder{
	public:
		dclspc TrackJointFeeder( Real engine_meters_per_unit, Real engine_kilograms_per_unit ) noexcept;

		/// \brief Simulation update method, to be called,
		/// after a simulation step is finished.
		bool dclspc virtual Update( Time dt );

		bool dclspc Rail( std::shared_ptr<Track> pTrack, const TrackLocation& tl, bool doPulse = true );

		bool dclspc Rail( const Location& location, bool doPulse = true );

		void dclspc DeRail( bool doPulse = true );

		bool dclspc IsRailed() const noexcept;

		bool dclspc IsMovableBodySleeping() const noexcept;

		void dclspc WakeUp() noexcept;

		void dclspc SetVelocity( Velocity velocity );

		Velocity dclspc GetVelocity() const noexcept;

		dclspc const Location& GetLocation() const noexcept;

		virtual void SetLimits( const TrackJointLimits& wfl ) = 0;

		virtual void SetMotorForceLimits( Force min, Force max ) noexcept = 0;

		virtual void SetMotorTarget( Velocity ) noexcept = 0;

		virtual Velocity GetMotorTarget() noexcept = 0;


		/// \brief Gets the anchor in global coordinates.
		///
		/// Will be flipped if railed on track in anti direction.
		virtual spat::Frame<Length,One> GlobalAnchor() const = 0;

		/// \brief Gets the anchor in local coordinates.
		///
		/// Will be flipped if railed on track in anti direction.
		virtual spat::Frame<Length,One> Anchor() const noexcept = 0;

		/// \brief Sets the anchor in local coordinates.
		virtual void Anchor( const spat::Frame<Length,One>& frame ) = 0;


		// Jacks 'N Plugs:

		/// \brief Gets a Jack that pulses its Plug if the WheelFrame
		/// gets railed on a track.
		dclspc Jack& JackOnRail() noexcept;

		/// \brief Gets a Jack that pulses its Plug if the WheelFrame
		/// derails.
		dclspc Jack& JackOnDerail() noexcept;

		/// \brief Gets a Jack that pulses its Plug if the WheelFrame
		/// starts to roll.
		dclspc Jack& JackOnRollingStart() noexcept;

		/// \brief Gets a Jack that pulses its Plug if the WheelFrame
		/// stops to roll.
		dclspc Jack& JackOnRollingStop() noexcept;

		/// \brief Gets a Jack that pulses its Plug if the WheelFrame
		/// starts tilting.
		dclspc Jack& JackOnStartTilting() noexcept;

		/// \brief Gets a Jack that pulses its Plug if the WheelFrame
		/// starts tilting.
		dclspc Jack& JackOnStopTilting() noexcept;

		/// \brief Gets a Jack that pulses its Plug if the WheelFrame
		/// passes the connection between two tracks.
		dclspc Jack& JackOnTrackTransition() noexcept;


		inline Real EngineMetersPerUnit() const noexcept{
			return m_EngineMetersPerUnit;
		}
		inline Real EngineKilogramsPerUnit() const noexcept{
			return m_EngineKilogramsPerUnit;
		}
	protected:
		virtual void TurnAnchor() const = 0;
		virtual void TrackBodyChange( std::shared_ptr<const Body> pToBody ) = 0;
		virtual void SetTimeStep( Time dt ) = 0;
		virtual void UpdateBodies() const = 0;
		virtual bool SetValues( const TrackData<Real>& trackData ) = 0;
		virtual bool DoIsMovableBodySleeping() const noexcept = 0;
		virtual void DoWakeUp() const = 0;
		virtual void DoRail() = 0;
		virtual void DoDerail() = 0;

		virtual std::pair<Length,bool> MoveBy( Length ds );
	private:
		Location m_Location;
		Real m_EngineMetersPerUnit;
		Real m_EngineKilogramsPerUnit;
		bool m_WakeupFlag;

		Velocity m_CurrentVelocity = 0_mIs;
		bool m_bRolling = false;

		/// \brief Simulation's step ds relative to track.
		inline Length ds() const
		// ds = (A.P(t+dt) - wF.P(s,t+dt)) * wF.T(s,t)
		{
			const spat::Position<Length> AP{ GlobalAnchor().P };
			spat::VectorBundle<Length,One> wF;
			m_Location.Transition( wF );

			return (AP - wF.P) * wF.T;
		}

		Jack_Imp m_JackOnRail			{ "JackOnRail" };
		Jack_Imp m_JackOnDerail			{ "JackOnDerail" };
		Jack_Imp m_JackOnRollingStart	{ "JackOnRollingStart" };
		Jack_Imp m_JackOnRollingStop	{ "JackOnRollingStop" };
		Jack_Imp m_JackOnStartTilting	{ "JackOnStartTilting" };
		Jack_Imp m_JackOnStopTilting	{ "JackOnStopTilting" };
		Jack_Imp m_JackOnTrackTransition{ "JackOnTrackTransition" };
	};


	/// \brief Class with functionality to support a track joint 
	/// on a track system. Provides a model for friction, brake and drive.
	class TrackJointFeederMotorModel : public TrackJointFeeder{
	public:
		TrackJointFeederMotorModel( Real engine_meters_per_unit, Real engine_kilograms_per_unit );

		bool dclspc Update( Time dt ) override;

		/// \brief Sets the target velocity to aim for by using the supplied
		/// Thrust() and Brake() settings.
		///
		/// The simulator will try to reach this by applying up to the fraction
		/// of maximum thrust, specified by Thrust() and up to the fraction of
		/// maximum braking force specified by Brake() if necessary to reduce the
		/// actual velocity. The brake will only be used, if the velocity exceeds 
		/// the target velocity. E.g. on a hill the RailRunner might roll back due to
		/// too few thrust but the brake will not get applied in this case.
		/// These values will get propagated to all the child bogies.
		/// \param vTarget Signed target velocity to aim for. This very well might 
		/// be set to extreme high values, wich can never be reached by the motor. 
		void dclspc TargetVelocity( Velocity vTarget ) noexcept;


		/// \brief Gets the target velocity set for this RailRunner.
		Velocity dclspc TargetVelocity() const noexcept;

	//	EndType dclspc TargetDirection() const noexcept;

		/// \returns The maximal power output the engine can supply. 
		Power dclspc MaximumPowerOutput() const noexcept;


		/// \returns The power output the engine is currently delivering.
		Power dclspc CurrentPowerOutput() const noexcept;


		/// \returns The maximum velocity the RailRunner can theoretically
		/// propell itself to on even track.
		Velocity dclspc MaximumVelocity() const noexcept;


		/// \brief Sets the fraction of the maximum thrust to apply in order to 
		/// reach the target velocity specified by TargetVelocity().
		/// \param byfactor Value between [0,1] specifying the fraction of the total 
		/// available acceleration.
		void dclspc Thrust( One byfactor ) noexcept;


		/// \brief Gets the fraction of the maximum thrust applied.
		/// \returns Value between [0,1], specifying the fraction of the 
		/// total available thrust.
		One dclspc Thrust() const noexcept;


		/// \brief Sets the thrust as absolute force values. If the 
		/// value exceeds MaxThrust() it get clipped.
		void dclspc ThrustAbsolute( Force thrust ) noexcept;


		/// \brief Gets the thrust as absolute momentary force value.
		Force dclspc ThrustAbsolute() const noexcept;


		/// \name Maximum Thrust
		/// \brief Gets the maximal available thrust. This value is determined by 
		/// the properties of the Wheelsets attached to WheelFrames, wich in turn are 
		/// attached to a bogie. It is also determined by the engine characteristic
		/// and the actual velocity.
		/// \param velocity the velocity the maximum thrust is requested for.
		///@{
		Force dclspc MaxThrust() const noexcept;

		Force dclspc MaxThrust( Velocity velocity ) const noexcept;
		///@}


		/// \brief Sets the fraction of the maximum braking to apply in order  
		/// to reach a zero velocity.
		/// \param byfactor Value between [0,1] specifying the fraction of the total 
		/// available braking.
		void dclspc Brake( One byfactor ) noexcept;


		/// \brief Gets the fraction of the maximum braking applied.
		/// \returns Value between [0,1], specifying the fraction of the  
		/// total available braking.
		One dclspc Brake() const noexcept;


		/// \brief Sets the brake as absolute force value. If the 
		/// value exceeds MaxBrake() it gets clipped.
		void dclspc BrakeAbsolute( Force brake ) noexcept;


		/// \brief Gets the brake as absolute force value.
		Force dclspc BrakeAbsolute() const noexcept;


		/// \brief Gets the maximal available braking. This value is determined by the 
		/// properties of the Wheelsets attached to WheelFrames wich in turn are attached
		/// to a bogie.
		Force dclspc MaxBrake() const noexcept;


		/// \brief applies a TractionForceCharacteristic to the thrust calculations.
		///
		/// The default is a characteristic wich will allways be 1.
		/// \param pCharacteristic Pointer to functor.
		void dclspc SetTractionForceCharacteristic( std::shared_ptr<const struct TractionForceCharacteristic> pCharacteristic );


		/// \returns A pointer to the actually applied characteristic
		std::shared_ptr<const TractionForceCharacteristic> dclspc GetTractionForceCharacteristic() const noexcept;


		/// \brief Attaches a Wheelset to this WheelFrame.
		/// \returns The index of the attached Wheelset.
		int dclspc Attach( const Wheelset& ws );


		/// \brief Removes all the Wheelsets.
		void dclspc DetachAllWheelsets() noexcept;


		/// \brief Gets the number of Wheelsets attached directly to this WheelFrame.
		int dclspc CntWheelsets() const;


		/// \brief Gets the Wheelset denoted by idx.
		/// \param idx Index of the Wheelset 0 <= idx < CntWheelsets()
		dclspc const Wheelset& GetWheelset( int idx ) const;


		/// \brief Gets a Jack that pulses its Plug if the Feeder's 
		/// motor gets activated in order to reach the target velocity.
		dclspc Jack& JackOnMotorActive() noexcept;

		/// \brief Gets a Jack that pulses its Plug if the Feeder's 
		/// motor gets deactivated.
		dclspc Jack& JackOnMotorInactive() noexcept;

		/// \brief Gets a Jack that pulses its Plug if the Feeder's 
		/// brake gets activated in order to reach the target velocity.
		dclspc Jack& JackOnBrakeActive() noexcept;

		/// \brief Gets a Jack that pulses its Plug if the Feeder's 
		/// brake gets deactivated.
		dclspc Jack& JackOnBrakeInactive() noexcept;

	protected:
		std::pair<Length,bool> MoveBy( Length ds ) override;
	private:
		std::shared_ptr<const TractionForceCharacteristic> m_pTFC;
		std::vector<Wheelset> m_Wheelsets;

		One m_ThrustFactor;
		One m_BrakingFactor;
		Velocity m_TargetVelocity;

		Force MaxPossibleWheelThrust() const noexcept;
		Force MaxWheelThrust() const noexcept;
		Force MaxWheelBraking() const noexcept;
		Force MaxWheelFriction() const noexcept;
		Real Characteristic( Velocity velocity ) const noexcept;

		void UpdateMotorValues() noexcept;

		bool CalculateMotorAction() noexcept;
		bool CalculateBrakeAction() noexcept;
		bool m_bMotorAction;
		bool m_bBrakeAction;

		Jack_Imp m_JackOnMotorActive{ "JackOnMotorActive" };
		Jack_Imp m_JackOnMotorInactive{ "JackOnMotorInactive" };
		Jack_Imp m_JackOnBrakeActive{ "JackOnBrakeActive" };
		Jack_Imp m_JackOnBrakeInactive{ "JackOnBrakeInactive" };
	};


	/// \brief implementation for a specific value type.
	///
	/// Keeps base class physics engine agnostic.
	template<typename Valtype,class Base>
	class TrackJointFeeder_Imp : public Base{
	public:
		TrackJointFeeder_Imp() = delete;
		TrackJointFeeder_Imp( TrackJoint<Valtype>& rTrackJoint, Valtype engine_meters_per_unit, Valtype engine_kilograms_per_unit ) noexcept;
		~TrackJointFeeder_Imp();

		void SetLimits( const TrackJointLimits& wfl ) override;

		void SetMotorForceLimits( Force min, Force max ) noexcept override;

		void SetMotorTarget( Velocity ) noexcept override;

		Velocity GetMotorTarget() noexcept override;

		spat::Frame<Length,One> GlobalAnchor() const override;

		spat::Frame<Length,One> Anchor() const noexcept override;

		void Anchor( const spat::Frame<Length,One>& frame ) noexcept override;
	protected:
		void TurnAnchor() const override;
		void TrackBodyChange( std::shared_ptr<const Body> pToBody ) override;
		void SetTimeStep( Time dt ) override;
		void UpdateBodies() const override;
		bool SetValues( const TrackData<Real>& trackData ) override;
		bool DoIsMovableBodySleeping() const noexcept override;
		void DoWakeUp() const override;
		void DoRail() override;
		void DoDerail() override;
	private:
		TrackJoint<Valtype>& m_TrackJoint;
	};

///////////////////////////////////////
template<typename Valtype,class Base>
inline TrackJointFeeder_Imp<Valtype,Base>::TrackJointFeeder_Imp( 
	TrackJoint<Valtype>& rTrackJoint, 
	Valtype engine_meters_per_unit, 
	Valtype engine_kilograms_per_unit ) noexcept
	:	Base		{ engine_meters_per_unit, engine_kilograms_per_unit },
		m_TrackJoint{ rTrackJoint }
{
}

template<typename Valtype, class Base>
inline trax::TrackJointFeeder_Imp<Valtype, Base>::~TrackJointFeeder_Imp()
{
	m_TrackJoint.release(); //trackjoints deletes on release.
}

template<typename Valtype, class Base>
inline void TrackJointFeeder_Imp<Valtype, Base>::SetLimits( const TrackJointLimits& wfl )
{
	m_TrackJoint.NormalForceLimits( 
		_N(wfl.NormalForceLimitMin) / Base::EngineKilogramsPerUnit() / Base::EngineMetersPerUnit(), 
		_N(wfl.NormalForceLimitMax) / Base::EngineKilogramsPerUnit() / Base::EngineMetersPerUnit() );
	m_TrackJoint.BinormalForceLimits(
		_N(wfl.BinormalForceLimitMin) / Base::EngineKilogramsPerUnit() / Base::EngineMetersPerUnit(),
		_N(wfl.BinormalForceLimitMax) / Base::EngineKilogramsPerUnit() / Base::EngineMetersPerUnit() );
	m_TrackJoint.NormalTorqueLimit( 
		_Nm(wfl.NormalTorqueLimit) / Base::EngineKilogramsPerUnit() / Base::EngineMetersPerUnit() / Base::EngineMetersPerUnit() );
	m_TrackJoint.TorqueLimit( 
		_Nm(wfl.TorqueLimit) / Base::EngineKilogramsPerUnit() / Base::EngineMetersPerUnit() / Base::EngineMetersPerUnit() );
	m_TrackJoint.ThresholdP( _m(wfl.ThresholdPosition) / Base::EngineMetersPerUnit() );
	m_TrackJoint.ThresholdT( wfl.ThresholdTangent );
	m_TrackJoint.ThresholdN( wfl.ThresholdNormal );
	m_TrackJoint.ThresholdB( wfl.ThresholdBinormal );
	m_TrackJoint.ErrorReductionParameter( wfl.ErrorReduction );
}

template<typename Valtype,class Base>
inline void TrackJointFeeder_Imp<Valtype,Base>::SetMotorForceLimits( Force min, Force max ) noexcept{
	if( Base::GetLocation().Orient() == Orientation::Value::anti ){
		const Force temp = min;
		min = -max;
		max = -temp;
	}

	m_TrackJoint.MotorForceLimits( 
		_N(min) / Base::EngineKilogramsPerUnit() / Base::EngineMetersPerUnit(), 
		_N(max) / Base::EngineKilogramsPerUnit() / Base::EngineMetersPerUnit() );
}

template<typename Valtype,class Base>
inline void TrackJointFeeder_Imp<Valtype,Base>::SetMotorTarget( Velocity vTarget ) noexcept{
	m_TrackJoint.MotorTarget( (Base::GetLocation().Orient() == Orientation::Value::para ? +1.0f : -1.0f) * _mIs(vTarget) / Base::EngineMetersPerUnit() );
}

template<typename Valtype,class Base>
inline Velocity TrackJointFeeder_Imp<Valtype,Base>::GetMotorTarget() noexcept
{
	return _mIs( (Base::GetLocation().Orient() == Orientation::Value::para ? +1.0f : -1.0f) * Base::EngineMetersPerUnit() * m_TrackJoint.MotorTarget() );
}

template<typename Valtype,class Base>
spat::Frame<Length,One> TrackJointFeeder_Imp<Valtype,Base>::GlobalAnchor() const
{
	return { 
		{	_m(Base::EngineMetersPerUnit() * m_TrackJoint.GlobalAnchor().P.x), 
			_m(Base::EngineMetersPerUnit() * m_TrackJoint.GlobalAnchor().P.y), 
			_m(Base::EngineMetersPerUnit() * m_TrackJoint.GlobalAnchor().P.z) },
            spat::spatial_cast<One>(m_TrackJoint.GlobalAnchor().T),
            spat::spatial_cast<One>(m_TrackJoint.GlobalAnchor().N),
            spat::spatial_cast<One>(m_TrackJoint.GlobalAnchor().B) 
	};
}

template<typename Valtype, class Base>
inline spat::Frame<Length,One> TrackJointFeeder_Imp<Valtype, Base>::Anchor() const noexcept
{
	return { 
		{	_m(Base::EngineMetersPerUnit() * m_TrackJoint.Anchor().P.x), 
			_m(Base::EngineMetersPerUnit() * m_TrackJoint.Anchor().P.y), 
			_m(Base::EngineMetersPerUnit() * m_TrackJoint.Anchor().P.z) },
			spat::spatial_cast<One>(m_TrackJoint.Anchor().T),
			spat::spatial_cast<One>(m_TrackJoint.Anchor().N),
			spat::spatial_cast<One>(m_TrackJoint.Anchor().B) 
	};
}

template<typename Valtype, class Base>
inline void TrackJointFeeder_Imp<Valtype, Base>::Anchor( const spat::Frame<Length, One>& frame ) noexcept
{
	m_TrackJoint.Anchor( { 
		{	_m(frame.P.x) / Base::EngineMetersPerUnit(), 
			_m(frame.P.y) / Base::EngineMetersPerUnit(), 
			_m(frame.P.z) / Base::EngineMetersPerUnit() },
			spat::spatial_cast<Valtype>(frame.T),
			spat::spatial_cast<Valtype>(frame.N),
			spat::spatial_cast<Valtype>(frame.B) 
	} );
}

template<typename Valtype,class Base>
void TrackJointFeeder_Imp<Valtype,Base>::TurnAnchor() const
{
	spat::Frame<Valtype> A = m_TrackJoint.Anchor();
	m_TrackJoint.Anchor( { A.P, -A.T, -A.N, A.B } );
	m_TrackJoint.MotorTarget( -m_TrackJoint.MotorTarget() );
	m_TrackJoint.MotorForceLimits( -m_TrackJoint.MotorForceMax(), -m_TrackJoint.MotorForceMin() );
}

template<typename Valtype,class Base>
inline void TrackJointFeeder_Imp<Valtype,Base>::TrackBodyChange( std::shared_ptr<const Body> pToBody ){
	m_TrackJoint.UpdateTrackBody( pToBody );
}

template<typename Valtype,class Base>
void TrackJointFeeder_Imp<Valtype,Base>::SetTimeStep( Time dt ){
	m_TrackJoint.SetTimeStep( dt );
}

template<typename Valtype,class Base>
inline void TrackJointFeeder_Imp<Valtype,Base>::UpdateBodies() const{
	m_TrackJoint.UpdateBodies();
}

template<typename Valtype,class Base>
bool TrackJointFeeder_Imp<Valtype,Base>::SetValues( const TrackData<Real>& trackData ){
	return m_TrackJoint.SetValues( trackData );
}

template<typename Valtype,class Base>
inline bool TrackJointFeeder_Imp<Valtype,Base>::DoIsMovableBodySleeping() const noexcept{
	return m_TrackJoint.IsMovableBodySleeping();
}

template<typename Valtype,class Base>
void TrackJointFeeder_Imp<Valtype,Base>::DoWakeUp() const{
	return m_TrackJoint.WakeUp();
}

template<typename Valtype, class Base>
inline void TrackJointFeeder_Imp<Valtype, Base>::DoRail()
{
	m_TrackJoint.SetDerailed( false );
}

template<typename Valtype,class Base>
inline void TrackJointFeeder_Imp<Valtype,Base>::DoDerail()
{
	m_TrackJoint.SetDerailed( true );
}
///////////////////////////////////////
}

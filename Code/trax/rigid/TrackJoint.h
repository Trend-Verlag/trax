//	trax track library
//	AD 2014 
//
//  "we are all the voices under the tracks"
//
//								Funeral Party
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
#include "trax/TrackData.h"
#include "trax/Units.h"

#include <iostream>
#include <memory>
#include <sstream>

namespace trax{

	/// \brief Physics engine agnostic part of track joint logic. 
	///
	/// Implemented are the case for static tracks as well as the case
	/// were the track system is attached to a moving body. E.g. turn tables
	/// and the like.
	template<typename Valtype>
	class TrackJoint : public TrackData<Valtype>{
	public:
		TrackJoint() noexcept;
		virtual ~TrackJoint(){}


		/// \brief Sets the time step for this trackjoint.
		///
		/// Influences force and torque strengthes. This should be called on using non-fixed timesteps.
		/// Default is fixed_timestep.
		void SetTimeStep( Time step ) noexcept;


		/// \returns the time step used by this joint.
		Time GetTimeStep() const noexcept;


		/// \brief Set the global pose of the body moving along the track
		bool SetMovingBody( const spat::Frame<Valtype>& bodyFrame ) noexcept;


		/// \returns The global pose of the body moving along the track
		const spat::Frame<Valtype>& GetMovingBody() const noexcept;


		/// \brief Set the moving bodie's center of mass relative to the bodie's pose.
		void SetMovingBodyCOM( const spat::Frame<Valtype>& localCom ) noexcept;


		/// \brief Set the global pose of the body moving the track system.
		bool SetTrackBody( const spat::Frame<Valtype>& bodyFrame ) noexcept;


		/// \returns The global pose of the body moving the track system.
		const spat::Frame<Valtype>& GetTrackBody() const noexcept;


		/// \brief Set the track bodie's center of mass relative to the track bodie's pose.
		void SetTrackBodyCOM( const spat::Frame<Valtype>& localCom ) noexcept;


		/// \brief Sets the anchor on the moving body that is to be aligned
		/// with a track position on the track body.
		/// \param anchor Anchor of the moving body in local coordinates
		void Anchor( const spat::Frame<Valtype>& anchor ) noexcept;


		/// \return The anchor of the moving body in local coordinates
		inline const spat::Frame<Valtype>& Anchor() const noexcept;


		/// \return The anchor of the moving body in global coordinates
		inline const spat::Frame<Valtype>& GlobalAnchor() const noexcept;


		/// \name Force Limits
		/// \brief Specifies the maximal forces to be applied by the physics engine
		/// to maintain the constraints that hold the movable object on the track.
		/// \param min Minimum force or torque to be applied at the movable body, min <= max.
		/// \param max Maximum force or torque to be applied at the movable body, min <= max.
		/// \param targetVelocity Target velocity of the movable object to be maintained by 
		/// the engine.
		/// \returns The respective values returned by the getter methods.
		///@{

		/// \brief Sets the limits for the normal force to be applied at the anchor point.
		void NormalForceLimits( Valtype min, Valtype max );


		/// \brief Sets the limits for the binormal force to be applied at the anchor point.
		void BinormalForceLimits( Valtype min, Valtype max );


		/// \brief Sets the limit for the normal torque to be applied at the anchor point [-max,max].
		void NormalTorqueLimit( Valtype max );


		/// \brief Sets the limit for the tangent and binormal torque to be applied at the anchor point [-max,max].
		void TorqueLimit( Valtype max );


		/// \brief Sets the target velocity of the movable object to be maintained by the engine.
		void MotorTarget( Valtype targetVelocity ) noexcept;


		/// \brief Sets the limits for the motor force to be applied at the anchor point.
		void MotorForceLimits( Valtype min, Valtype max );


		/// \brief Sets the maximum motor force to be applied in order to reach target velocity at the anchor point.
		void MotorForceMax( Valtype max );


		/// \brief Sets the minimum motor force to be applied in order to reach target velocity at the anchor point.
		void MotorForceMin( Valtype min );


		/// \returns The minimum motor force to be applied at the anchor point.
		Valtype MotorTarget() const noexcept;


		/// \returns The maximum motor force to be applied at the anchor point.
		Valtype MotorForceMax() const noexcept;


		/// \returns The minimum motor force to be applied at the anchor point.
		Valtype MotorForceMin() const noexcept;
		///@}


		/// \brief Sets the threshold to switch off the joint when violated.
		///
		/// The threshold gives the distance from the TBN frame position to 
		/// the real body position from wich on the joint will have no effect 
		/// any more.
		/// \param threshold linear threshold distance.
		void ThresholdP( Valtype threshold ) noexcept;

		/// \brief Sets the threshold to switch off the joint when violated.
		///
		/// The threshold sets the angle in radiants, so that a 
		/// violation of the joint conditions greater than that will result
		/// in the joint having no effect any more.
		/// This will not work for T, if tilting is enabled.
		/// \param threshold Angular threshold for rotations around tangent T.
		void ThresholdT( Valtype threshold ) noexcept;

		/// \brief Sets the threshold to switch off the joint when violated.
		///
		/// The threshold sets the angle in radiants, so that a 
		/// violation of the joint conditions greater than that will result
		/// in the joint having no effect any more.
		/// \param threshold Angular threshold for rotations around normal N.
		void ThresholdN( Valtype threshold ) noexcept;

		/// \brief Sets the threshold to switch off the joint when violated.
		///
		/// The threshold sets the angle in radiants, so that a 
		/// violation of the joint conditions greater than that will result
		/// in the joint having no effect any more.
		/// \param threshold Angular threshold for rotations around binormal B.
		void ThresholdB( Valtype threshold ) noexcept;


		/// \brief If there is no holding force in binormal direction (BinormalForceLimitMin >= 0)
		/// the flange height will be used to switch off the joint's effect if the anchor levitates
		/// more than by flange height.
		/// 
		/// Note that error correction stays in effect while still in the flange range. 
		/// This makes the trackjoint a bit sticky while in flange range. So flange should 
		/// be typically a small value to achieve realistic levitating.
		void Flange( Valtype flange ) noexcept;


		/// \brief Sets the error reduction parameter for the track joint.
		///
		/// With the track joint comes a mechanism that handles translational or
		/// angular aberrations of the anchor from the track. The erp specifies
		/// what fraction of the error should be corrected in the next simulation step.
		/// 1 would mean all of the error (not recommended) and 0 means no correction
		/// (also not recommended). The default value is 0.25. In connection with other
		/// joints and under certain mass conditions it might happen that the error 
		/// correction of two or more joints interfere in an unfortunate way, so 
		/// the simualtion becomes instable. This can be cured by choosing a different
		/// erp.
		/// \param erp Error reduction parameter. The value should be between 0 and 1.
		void ErrorReductionParameter( Valtype erp ) noexcept;


		/// \returns the error reduction parameter for the track joint.
		Valtype ErrorReductionParameter() const noexcept;


		/// \brief Sets the status of the track joint to derailed.
		///
		/// A derailed trackjoint will not influence the simulation anymore.
		/// If derailment is a permanent situation it is recommended to
		/// remove the joint from the simulation completely, since a derailed
		/// joint will still consume some resources.
		void SetDerailed( bool bDerailed = true ) noexcept;


		/// \returns true if the track joint is set for derailment.
		bool IsDerailed() const noexcept;


		/// \name Constraints
		/// \brief The track joint assembles a set of different constraints to keep
		/// the anchor aligned with a track.
		/// \param dim index for the constraint 0 < dim < DimensionsTotal(). This
		/// is the ordinal number of the constraint minus one.
		///@{
		/// \returns the number of active constraints.
		inline unsigned short DimensionsTotal() const noexcept;

		/// \returns true if the constraint involves a rotational dimension.
		inline bool IsAngularConstraint( unsigned short dim ) const noexcept;

		/// \returns true if the constraint is amotor constraint.
		inline bool IsDriveConstraint( unsigned short dim ) const noexcept;

		/// \returns true if the constraint's maximum or minimum force or torque is not infinite.
		inline bool IsConstraintLimited( unsigned short dim ) const noexcept;

		/// Fills the matrix values for a specific constraint.
		inline void Jacobian( 
						unsigned short dim,
						spat::Vector<Valtype>& Jm,
						spat::Vector<Valtype>& Rm,
						spat::Vector<Valtype>& Jt,
						spat::Vector<Valtype>& Rt,
						Valtype& VTarget,
						Valtype& E,
						Valtype& fmin,
						Valtype& fmax  ) const noexcept
		{
			if( bMovingTrack ){
				switch( dim ){
				case 0:
					Constraint1( Jm, Rm, Jt, Rt, E, fmin, fmax );
					break;
				case 1:
					Constraint2( Jm, Rm, Jt, Rt, E, fmin, fmax );
					break;
				case 2:
					Constraint3( Jm, Rm, Jt, Rt, E, fmin, fmax );
					break;
				case 3:
					Constraint4( Rm, Rt, E, fmin, fmax );
					break;
				case 4:
					Constraint5( Jm, Rm, Jt, Rt, E, fmin, fmax );
					break;
				case 5:
					Constraint6( Jm, Rm, Jt, Rt, VTarget, fmin, fmax );
					break;
				default:
					assert( !"Dimension index invalid!" );
				}
			}
			else{
				switch( dim ){
				case 0:
					Constraint1( Jm, Rm, E, fmin, fmax );
					break;
				case 1:
					Constraint2( Jm, Rm, E, fmin, fmax );
					break;
				case 2:
					Constraint3( Jm, Rm, E, fmin, fmax );
					break;
				case 3:
					Constraint4( Rm, E, fmin, fmax );
					break;
				case 4:
					Constraint5( Jm, Rm, E, fmin, fmax );
					break;
				case 5:
					Constraint6( Jm, Rm, VTarget, fmin, fmax );
					break;
				default:
					assert( !"Dimension index invalid!" );
				}
			}

			E *= erp;
		}
		///@}

		using TrackData<Valtype>::F;
		using TrackData<Valtype>::wF;
		using TrackData<Valtype>::c;
		using TrackData<Valtype>::t;


		/// \returns The vector from the center of mass (COM) 
		/// to the anchor for the moving body in global coordinates
		inline spat::Vector<Valtype> MovingBodyGlobalCOM2AnchorOffset() const noexcept{
			return dA;
		}

		/// \brief Set the global pose of the body moving along the track and precalculates
		/// internal values like the global anchor.
		inline void Precalculate( const spat::Frame<Valtype>& bodyFrame ) noexcept;

		virtual bool IsMovableBodySleeping() const noexcept = 0;

		virtual void WakeUp() const = 0;

		virtual void UpdateBodies() = 0;

		virtual void UpdateTrackBody( std::shared_ptr<const struct Body> pToBody ) = 0;

		virtual void release() = 0;
	protected:
		virtual void markDirty() noexcept = 0;

	private:
		spat::Frame<Valtype> Fm;				///< The pose of the moving body Bm's COM at simulation step t.
		spat::Frame<Valtype> Ft;				///< The pose of the body Bt's COM at simulation step t.
		spat::Frame<Valtype> A;					///< Anchor of Bm in global coordinates at simulation step t.

		spat::Frame<Valtype> m_MovingBodyCOM;	///< The center of mass of Bm in local coordinates.
		spat::Frame<Valtype> m_TrackBodyCOM;	///< The center of mass of Bt in local coordinates.
		spat::Frame<Valtype> m_MovingBodyFrame; ///< The global frame of Bm.
		spat::Frame<Valtype> m_TrackBodyFrame;	///< The global frame of Bt.
		spat::Frame<Valtype> m_LocalAnchor;		// The frame relative to Fm to be aligned with the track frame.


		Valtype				erp;	///< Error Reduction Parameter [0,1]
		Valtype				vTarget;
		Valtype				motor_force_min;
		Valtype				motor_force_max;
		bool				motor_force_limited;
		Valtype				flange;

		// Relative rotations to be applied to fix error between 
		// body orientation and supposed track orientation in global coordinates.
		spat::Vector<Valtype>	eT;	///< eT = A.T x wF.T
		spat::Vector<Valtype>	eN;	///< eN = A.N x wF.N
		spat::Vector<Valtype>	eB;	///< eB = A.B x wF.B				
		spat::Vector<Valtype>	eP;	///< eP = wF.P - A.P;  Distance from body anchor position to supposed track joint position in global coordinates.
		//precalculated values:
		spat::Vector<Valtype>	dA; ///< dA = A.P - Fm.P
		spat::Vector<Valtype>	dF; ///< dF = wF.P - Ft.P

		unsigned short m_Dims, m_LimDims;
		bool bDerailed = false;

		Valtype normal_force_min;
		Valtype normal_force_max;
		bool	normal_force_limited;
		Valtype binormal_force_min;
		Valtype binormal_force_max;
		bool	binormal_force_limited;
		Valtype normal_torque_max;
		bool	normal_torque_limited;
		Valtype torque_max;
		bool	torque_limited;

		bool	thresholdPViolated;
		Valtype	thresholdP;
		bool	thresholdTViolated;
		Valtype	thresholdT;
		bool	thresholdNViolated;
		Valtype	thresholdN;
		bool	thresholdBViolated;
		Valtype	thresholdB;

		bool	bMovingTrack;
		Time	timeStep = fixed_timestep;

		static const Valtype maxval;

		// Wt,Wm: angular velocities of the two bodies
		// Vt,Vm: linear velocities of the two bodies
		// E: error correction factors

		//Va = Vm + Wm x (A.P - Fm.P)	: linear velocity of anchor point
		//Wa = Wm						: angular velocity of anchor point
		//Vp = Vt + Wt x (wF.P - Ft.P)	: linear velocitiy of track point
		//Wp = Wt						: angular velocity of track point

		// Jm*Vm + Rm*Wm + Jt*Vt + Rt*Wt = erp/dt * E	: constraint equation


		/// \brief Constraint 1, no relative movement along wF.N;
		///
		/// (Va - Vp) * wF.N = erp/dt * (wF.P - A.P) * wF.N 
		/// See [TrackConstrain] equation (6)
		inline void Constraint1( 
						spat::Vector<Valtype>& Jm1,
						spat::Vector<Valtype>& Rm1,
						spat::Vector<Valtype>& Jt1,
						spat::Vector<Valtype>& Rt1,
						Valtype& E1,
						Valtype& f1min,
						Valtype& f1max ) const noexcept
		{
			//Jm1	= wF.N;
			//Rm1	= (A.P - Fm.P) % wF.N;
			//Jt1	= -wF.N;
			//Rt1	= wF.N % (wF.P - Ft.P);
			//E1	= (wF.P - A.P) * wF.N;
			//f1min	= normal_force_min;
			//f1max	= normal_force_max;
		
			Jm1		= wF.N;
			Rm1		= dA % wF.N;
			Jt1		= -wF.N;
			Rt1		= wF.N % dF;
			E1		= eP * wF.N;
			f1min	= normal_force_min;
			f1max	= normal_force_max;
		}

		inline void Constraint1( 
						spat::Vector<Valtype>& Jm1,
						spat::Vector<Valtype>& Rm1,
						Valtype& E1,
						Valtype& f1min,
						Valtype& f1max ) const noexcept
		{		
			Jm1		= wF.N;
			Rm1		= dA % wF.N;
			E1		= eP * wF.N;
			f1min	= normal_force_min;
			f1max	= normal_force_max;
		}


		///\brief Constrain 2, no relative movement in -wF.B direction;
		///
		/// (Va - Vp) * wF.B = erp/dt * -(A.P - wF.P) * wF.B
		/// See [TrackConstrain] equation (7)
		inline void Constraint2(						
						spat::Vector<Valtype>& Jm2,
						spat::Vector<Valtype>& Rm2,
						spat::Vector<Valtype>& Jt2,
						spat::Vector<Valtype>& Rt2,
						Valtype& E2,
						Valtype& f2min,
						Valtype& f2max  ) const noexcept
		{
			//Jm2	= wF.B;
			//Rm2	= (A.P - Fm.P) % wF.B;
			//Jt2	= -wF.B;
			//Rt2	= wF.B % (wF.P - Ft.P);
			//E2	= (wF.P - A.P) * wF.B;
			//f2min	= binormal_force_min;
			//f2max	= binormal_force_max;

			Jm2		= wF.B;
			Rm2		= dA % wF.B;
			Jt2		= -wF.B;
			Rt2		= wF.B % dF;
			E2		= eP * wF.B;
			f2min	= binormal_force_min;
			f2max	= binormal_force_max;
		}

		inline void Constraint2(						
						spat::Vector<Valtype>& Jm2,
						spat::Vector<Valtype>& Rm2,
						Valtype& E2,
						Valtype& f2min,
						Valtype& f2max  ) const noexcept
		{
			Jm2		= wF.B;
			Rm2		= dA % wF.B;
			E2		= eP * wF.B;
			f2min	= binormal_force_min;
			f2max	= binormal_force_max;
		}

		/// \brief Constraint 3, a screwdriver rotation around wF.T;
		///
		/// wF.T * (Wa-Wp) - (t(s) + dwt(s)/ds) * wF.T * (Va-Vp) = erp/dt * (A.TxwF.T + A.NxwF.N + A.BxwF.B) * wF.T
		/// See [TrackConstrain] equation (8)
		inline void Constraint3( 
						spat::Vector<Valtype>& Jm3,
						spat::Vector<Valtype>& Rm3,
						spat::Vector<Valtype>& Jt3,
						spat::Vector<Valtype>& Rt3,
						Valtype& E3,
						Valtype& f3min,
						Valtype& f3max  ) const noexcept
		{
			//Jm3	= -t * wF.T;
			//Rm3	= wF.T - t * (A.P - Fm.P) % wF.T;
			//Jt3	= t * wF.T;
			//Rt3	= -wF.T + t * (wF.P - Ft.P) % wF.T;
			//E3	= (A.N % wF.N + A.B % wF.B) * wF.T;
			//f3min	= -torque_max;
			//f3max	= torque_max;

			Jm3		= -t * wF.T;
			Rm3		= wF.T - t * dA % wF.T;
			Jt3		= t * wF.T;
			Rt3		= -wF.T + t * dF % wF.T;
			E3		= (eN + eB) * wF.T;
			f3min	= -torque_max;
			f3max	= torque_max;
		}

		inline void Constraint3( 
						spat::Vector<Valtype>& Jm3,
						spat::Vector<Valtype>& Rm3,
						Valtype& E3,
						Valtype& f3min,
						Valtype& f3max  ) const noexcept
		{
			Jm3		= -t * wF.T;
			Rm3		= wF.T - t * dA % wF.T;
			E3		= (eN + eB) * wF.T;
			f3min	= -torque_max;
			f3max	= torque_max;
		}


		/// \brief Constraint 4, no rotation around the normal direction;
		///
		/// F.N * (Wa-Wp) = erp/dt * (A.TxwF.T + A.NxwF.N + A.BxwF.B)*F.N
		/// See [TrackConstrain] equation (9)
		inline void Constraint4( 
						spat::Vector<Valtype>& Rm4,
						spat::Vector<Valtype>& Rt4,
						Valtype& E4,
						Valtype& f4min,
						Valtype& f4max  ) const noexcept
		{
			//Rm4	= F.N;
			//Rt4	= -F.N;
			//E4	= (A.T % wF.T + A.N % wF.N + A.B % wF.B) * F.N;
			//f4min	= -normal_torque_max;
			//f4max	= normal_torque_max;

			Rm4		= F.N;
			Rt4		= -F.N;
			E4		= (eT + eN + eB) * F.N;
			f4min	= -normal_torque_max;
			f4max	= normal_torque_max;

		}

		inline void Constraint4( 
						spat::Vector<Valtype>& Rm4,
						Valtype& E4,
						Valtype& f4min,
						Valtype& f4max  ) const noexcept
		{
			Rm4		= F.N;
			E4		= (eT + eN + eB) * F.N;
			f4min	= -normal_torque_max;
			f4max	= normal_torque_max;
		}


		/// \brief Constraint 5, proper rotation around F.B;
		///
		/// F.B * (Wa-Wp) - c(s) * wF.T * (Va-Vp) = erp/dt * (A.T x wF.T + A.N x wF.N + A.B x wF.B) * F.B
		/// See [TrackConstrain] equation (10)
		inline void Constraint5( 
						spat::Vector<Valtype>& Jm5,
						spat::Vector<Valtype>& Rm5,
						spat::Vector<Valtype>& Jt5,
						spat::Vector<Valtype>& Rt5,
						Valtype& E5,
						Valtype& f5min,
						Valtype& f5max  ) const noexcept
		{
			//Jm5	= -c * wF.T;
			//Rm5	= F.B - c * (A.P - Fm.P) % wF.T;
			//Jt5	= c * wF.T;
			//Rt5	= -F.B + c * (wF.P - Ft.P) % wF.T;
			//E5	= (A.T % wF.T + A.N % wF.N + A.B % wF.B) * F.B;
			//f5min	= -torque_max;
			//f5max	= torque_max;

			Jm5		= -c * wF.T;
			Rm5		= F.B - c * dA % wF.T;
			Jt5		= c * wF.T;
			Rt5		= -F.B + c * dF % wF.T;
			E5		= (eT + eN + eB) * F.B;
			f5min	= -torque_max;
			f5max	= torque_max;
		}

		inline void Constraint5( 
						spat::Vector<Valtype>& Jm5,
						spat::Vector<Valtype>& Rm5,
						Valtype& E5,
						Valtype& f5min,
						Valtype& f5max  ) const noexcept
		{
			Jm5		= -c * wF.T;
			Rm5		= F.B - c * dA % wF.T;
			E5		= (eT + eN + eB) * F.B;
			f5min	= -torque_max;
			f5max	= torque_max;
		}


		/// \brief Constraint 6, a motor for accelerating and braking;
		///
		/// wF.T * (Va-Vp) = vTarget
		/// See [TrackConstrain] equation (11)
		inline void Constraint6(
						spat::Vector<Valtype>& Jm6,
						spat::Vector<Valtype>& Rm6,
						spat::Vector<Valtype>& Jt6,
						spat::Vector<Valtype>& Rt6,
						Valtype& VTarget6,
						Valtype& f6min,
						Valtype& f6max  ) const noexcept
		{
			//Jm6	= wF.T;
			//Rm6	= (A.P - Fm.P) % wF.T;
			//Jt6	= -wF.T;
			//Rt6	= wF.T % (wF.P - Ft.P);
			//VTarget6= vTarget;
			//f6min	= motor_force_min;
			//f6max	= motor_force_max;

			Jm6		= wF.T;
			Rm6		= dA % wF.T;
			Jt6		= -wF.T;
			Rt6		= wF.T % dF;
			VTarget6= vTarget;
			f6min	= motor_force_min;
			f6max	= motor_force_max;
		}

		inline void Constraint6(
						spat::Vector<Valtype>& Jm6,
						spat::Vector<Valtype>& Rm6,
						Valtype& VTarget6,
						Valtype& f6min,
						Valtype& f6max  ) const noexcept
		{
			Jm6		= wF.T;
			Rm6		= dA % wF.T;
			VTarget6= vTarget;
			f6min	= motor_force_min;
			f6max	= motor_force_max;
		}
	};

template<typename Valtype>
const Valtype TrackJoint<Valtype>::maxval = infinite;

template<typename Valtype>
inline TrackJoint<Valtype>::TrackJoint() noexcept
	: 	erp						{0.25f},
		vTarget					{0},
		motor_force_min			{0},
		motor_force_max			{0},
		motor_force_limited		{true},
		flange					{maxval},
		eT						{spat::Vector<Valtype>::specials::null},
		eN						{spat::Vector<Valtype>::specials::null},
		eB						{spat::Vector<Valtype>::specials::null},
		eP						{spat::Vector<Valtype>::specials::null},
		dA						{spat::Vector<Valtype>::specials::null},
		dF						{spat::Vector<Valtype>::specials::null},
		m_Dims					{0}, 
		m_LimDims				{0},
		normal_force_min		{-maxval},
		normal_force_max		{maxval},
		normal_force_limited	{false},
		binormal_force_min		{0},
		binormal_force_max		{maxval},
		binormal_force_limited	{true},
		normal_torque_max		{maxval},
		normal_torque_limited	{false},
		torque_max				{maxval},
		torque_limited			{false},
		thresholdPViolated		{false},
		thresholdP				{static_cast<Valtype>(default_derailing_distance.Units())},
		thresholdTViolated		{false},
		thresholdT				{1},
		thresholdNViolated		{false},
		thresholdN				{1},
		thresholdBViolated		{false},
		thresholdB				{1},
		bMovingTrack			{false}
{
	TrackData<Valtype>::Init();
	Fm.Init();
	Ft.Init();
	A.Init();
	m_MovingBodyCOM.Init();
	m_TrackBodyCOM.Init();
	m_MovingBodyFrame.Init();
	m_TrackBodyFrame.Init();
	m_LocalAnchor.Init();
}

template<typename Valtype>
inline void TrackJoint<Valtype>::SetTimeStep(Time step) noexcept{
	assert( step >= 0_s );
	timeStep = step;
}

template<typename Valtype>
inline Time TrackJoint<Valtype>::GetTimeStep() const noexcept{
	return timeStep;
}

template<typename Valtype>
inline void TrackJoint<Valtype>::Precalculate( const spat::Frame<Valtype>& bodyFrame ) noexcept{
	m_MovingBodyFrame = bodyFrame;
	A = m_LocalAnchor;
	m_MovingBodyFrame.ToParent( A );
	Fm = m_MovingBodyCOM;
	m_MovingBodyFrame.ToParent( Fm );

	// calculate error vectors:
	eT = A.T % wF.T;	// the angular errors are accurate only if the 
	eN = A.N % wF.N;	// angle is < pi/2. Especially if the vectors 
	eB = A.B % wF.B;	// are antiparallel, no derailment will happen.
	eP = wF.P - A.P;

	//precalculated values:
	dA = A.P - Fm.P;
}

template<typename Valtype>
inline bool TrackJoint<Valtype>::SetMovingBody( const spat::Frame<Valtype>& bodyFrame ) noexcept{
	Precalculate( bodyFrame );

	// threshold:
	//std::cout << "Positional error: " << eP.Length() << std::endl;

	if( eP.Length() > thresholdP ){
		thresholdPViolated = true;
		m_Dims = 0;
		m_LimDims = 0;
		std::clog << Verbosity::detailed << "Trackjoint: thresholdPViolated violated! (" << eP.Length() << " > " << thresholdP << ")" << std::endl;
		return false;
	}
	else
		thresholdPViolated = false;

	if( eN.Length() > thresholdT &&
		eB.Length() > thresholdT )
	{
		thresholdTViolated = true;
		m_Dims = 0;
		m_LimDims = 0;
		std::clog << Verbosity::detailed << "Trackjoint: thresholdTViolated violated!" << std::endl;
		return false;
	}
	else
		thresholdTViolated = false;

	if( eT.Length() > thresholdN &&
		eB.Length() > thresholdN ){
		thresholdNViolated = true;
		m_Dims = 0;
		m_LimDims = 0;
		std::clog << Verbosity::detailed << "Trackjoint: thresholdNViolated violated!" << std::endl;
		return false;
	}
	else
		thresholdNViolated = false;

	if( eT.Length() > thresholdB &&
		eN.Length() > thresholdB )
	{
		thresholdBViolated = true;
		m_Dims = 0;
		m_LimDims = 0;
		std::clog << Verbosity::detailed << "Trackjoint: thresholdBViolated violated!" << std::endl;
		return false;
	}
	else
		thresholdBViolated = false;
	
	if( motor_force_min < 0 || motor_force_max > 0 ){
		m_Dims = 6;
		m_LimDims = (normal_force_limited ? 1u : 0)		+ 
					(binormal_force_limited ? 1u : 0)	+ 
					(normal_torque_limited ? 1u : 0)	+ 
					(torque_limited ? 2u : 0)			+ 
					(motor_force_limited ? 1u : 0);
	}
	else{
		m_Dims = 5;
		m_LimDims = (normal_force_limited ? 1u : 0)		+ 
					(binormal_force_limited ? 1u : 0)	+ 
					(normal_torque_limited ? 1u : 0)	+
					(torque_limited ? 2u : 0);
	}

	if( binormal_force_min >= Valtype{0} && eP * wF.B < -flange )
	// out of grip of the track; leave things to external forces ...
	{
		m_Dims = 0;
		m_LimDims = 0;
		std::clog << Verbosity::verbose << "Trackjoint: levitating!" << std::endl;
	}

	if( bDerailed )
	{
		m_Dims = 0;
		m_LimDims = 0;
	}

	bMovingTrack = false;
	return true;
}

template<typename Valtype>
inline const spat::Frame<Valtype>& TrackJoint<Valtype>::GetMovingBody() const noexcept{
	return m_MovingBodyFrame;
}

template<typename Valtype>
inline void TrackJoint<Valtype>::SetMovingBodyCOM( const spat::Frame<Valtype>& localCom ) noexcept{
	m_MovingBodyCOM = localCom;
}

template<typename Valtype>
inline bool TrackJoint<Valtype>::SetTrackBody( const spat::Frame<Valtype>& bodyFrame ) noexcept{
	m_TrackBodyFrame = bodyFrame;
	Ft = m_TrackBodyCOM;
	m_TrackBodyFrame.ToParent( Ft );

	//precalculated values:
	dF = wF.P - Ft.P;

	bMovingTrack = true;
	return true;
}

template<typename Valtype>
inline const spat::Frame<Valtype>& TrackJoint<Valtype>::GetTrackBody() const noexcept{
	return m_TrackBodyFrame;
}

template<typename Valtype>
inline void TrackJoint<Valtype>::SetTrackBodyCOM( const spat::Frame<Valtype>& localCom ) noexcept{
	m_TrackBodyCOM = localCom;
}

template<typename Valtype>
inline void TrackJoint<Valtype>::Anchor( const spat::Frame<Valtype>& anchor ) noexcept{
	m_LocalAnchor = anchor;
	UpdateBodies();
}

template<typename Valtype>
inline const spat::Frame<Valtype>& TrackJoint<Valtype>::Anchor() const noexcept{
	return m_LocalAnchor;
}

template<typename Valtype>
inline const spat::Frame<Valtype>& TrackJoint<Valtype>::GlobalAnchor() const noexcept{
	return A;
}

template<typename Valtype>
inline void TrackJoint<Valtype>::NormalForceLimits( Valtype min, Valtype max ){
	if( min > max )
		throw std::invalid_argument( "NormalForceLimits min can't be greater than max!" );

	normal_force_min = min;
	normal_force_max = max;

	normal_force_limited =	normal_force_min > -maxval ||
							normal_force_max < maxval;

	markDirty();
}

template<typename Valtype>
inline void TrackJoint<Valtype>::BinormalForceLimits(Valtype min,Valtype max){
	if( min > max )
		throw std::invalid_argument( "BinormalForceLimits min can't be greater than max!" );

	binormal_force_min = min;
	binormal_force_max = max;

	binormal_force_limited =	binormal_force_min > -maxval ||
								binormal_force_max < maxval;

	markDirty();
}

template<typename Valtype>
inline void TrackJoint<Valtype>::NormalTorqueLimit(Valtype max){
	if( max < 0 )
		throw std::invalid_argument( "NormalTorqueLimit max can not be negative!" );

	normal_torque_max = max;
	normal_torque_limited = normal_torque_max < maxval;
	
	markDirty();
}

template<typename Valtype>
inline void TrackJoint<Valtype>::TorqueLimit( Valtype max ){
	if( max < 0 )
		throw std::invalid_argument( "TorqueLimit max can not be negative!" );

	torque_max = max;
	torque_limited = torque_max < maxval;

	markDirty();
}

template<typename Valtype>
inline void TrackJoint<Valtype>::MotorTarget( Valtype velocity ) noexcept{
	vTarget = velocity;
	markDirty();
}

template<typename Valtype>
inline void TrackJoint<Valtype>::MotorForceLimits( Valtype min, Valtype max ){
	if( min > max ){
		std::ostringstream stream;
		stream << "Track_Joint: motor_force_min (" << min << ") can't be greater than motor_force_max (" << max << ")!";
		throw std::invalid_argument( stream.str() );
	}

	motor_force_min = min;
	motor_force_max = max;

	motor_force_limited =	motor_force_min > -maxval ||
							motor_force_max < maxval;

	markDirty();
}

template<typename Valtype>
inline void TrackJoint<Valtype>::MotorForceMax( Valtype force ){
	if( force < motor_force_min ){
		std::ostringstream stream;
		stream << "Track_Joint: motor force (" << force << ") can not be smaller than motor_force_min (" << motor_force_min << ")!";
		throw std::invalid_argument( stream.str() );
	}

	motor_force_max		= force;
	motor_force_limited =	motor_force_min > -maxval ||
							motor_force_max < maxval;

	markDirty();
}

template<typename Valtype>
inline void TrackJoint<Valtype>::MotorForceMin( Valtype force ){
	if( force > motor_force_max ){
		std::ostringstream stream;
		stream << "Track_Joint motor force (" << force << ") can not be greater than motor_force_max (" << motor_force_max << ")!";
		throw std::invalid_argument( stream.str() );
	}

	motor_force_min		= force;
	motor_force_limited =	motor_force_min > -maxval ||
							motor_force_max < maxval;

	markDirty();
}

template<typename Valtype>
inline Valtype TrackJoint<Valtype>::MotorTarget() const noexcept
{
	return vTarget;
}

template<typename Valtype>
inline Valtype TrackJoint<Valtype>::MotorForceMax() const noexcept
{
	return motor_force_max;
}

template<typename Valtype>
inline Valtype TrackJoint<Valtype>::MotorForceMin() const noexcept
{
	return motor_force_min;
}

template<typename Valtype>
inline void TrackJoint<Valtype>::ThresholdP( Valtype threshold ) noexcept{
	thresholdP = threshold;
	markDirty();
}

template<typename Valtype>
inline void TrackJoint<Valtype>::ThresholdT( Valtype threshold ) noexcept{
	thresholdT = std::abs(std::sin( threshold ));
	markDirty();
}

template<typename Valtype>
inline void TrackJoint<Valtype>::ThresholdN( Valtype threshold ) noexcept{
	thresholdN = std::abs(std::sin( threshold ));
	markDirty();
}

template<typename Valtype>
inline void TrackJoint<Valtype>::ThresholdB( Valtype threshold ) noexcept{
	thresholdB = std::abs(std::sin( threshold ));
	markDirty();
}

template<typename Valtype>
inline void TrackJoint<Valtype>::Flange( const Valtype flange_ ) noexcept{
	flange = flange_;
}

template<typename Valtype>
inline void TrackJoint<Valtype>::ErrorReductionParameter( Valtype _erp ) noexcept
{
	erp = std::clamp( _erp, static_cast<Valtype>(0), static_cast<Valtype>(1) );
}

template<typename Valtype>
inline Valtype TrackJoint<Valtype>::ErrorReductionParameter() const noexcept
{
	return erp;
}

template<typename Valtype>
void TrackJoint<Valtype>::SetDerailed( bool _bDerailed ) noexcept{
	m_Dims = 0;
	m_LimDims = 0;
	bDerailed = _bDerailed;
	markDirty();
}

template<typename Valtype>
bool TrackJoint<Valtype>::IsDerailed() const noexcept{
	return bDerailed;
}

template<typename Valtype>
inline unsigned short TrackJoint<Valtype>::DimensionsTotal() const noexcept{
	return m_Dims;
}

template<typename Valtype>
inline bool TrackJoint<Valtype>::IsAngularConstraint( unsigned short dim ) const noexcept{
	return dim == 2 || dim == 3 || dim == 4;
}

template<typename Valtype>
inline bool TrackJoint<Valtype>::IsDriveConstraint( unsigned short dim ) const noexcept{
	return dim == 5;
}

template<typename Valtype>
inline bool TrackJoint<Valtype>::IsConstraintLimited( unsigned short dim ) const noexcept
{
	switch( dim ){
	case 0:
		return normal_force_limited;
	case 1:
		return binormal_force_limited;
	case 2:
		return torque_limited;
	case 3:
		return normal_torque_limited;
	case 4:
		return torque_limited;
	case 5:
		return motor_force_limited;
	}

	return false;
}

}
// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel

#pragma once

#include "trax/Units.h"
#include "spat/Frame.h"

namespace trax{

	/// \brief The maximum forces and spatial deviations of a
	/// WheelFrame used by physics calculations. 
	///
	/// Violation of of this limits typically will lead to derailment.
	struct TrackJointLimits{
		Force NormalForceLimitMin;		///< The maximum force (typically < 0) to prevent movement of the Wheelframe in normal direction.
		Force NormalForceLimitMax;		///< The maximum force to prevent movement of the Wheelframe in negative normal direction.
		Force BinormalForceLimitMin;	///< The maximum force (typically < 0) to prevent movement of the Wheelframe in binormal direction.
		Force BinormalForceLimitMax;	///< The maximum force to prevent movement of the Wheelframe in negative binormal direction.
		Torque NormalTorqueLimit;		///< The maximum torque to prevent rotation around the normal direction N.
		Torque TorqueLimit;				///< The maximum torque to prevent violating rotations around T and B.
		Length ThresholdPosition;		///< The maximum deviation of the WheelFrame's position from its supposed position on the track.
		Angle ThresholdTangent;			///< The maximum angle deviation of the WheelFrame's tangent vector from its supposed direction on the track.
		Angle ThresholdNormal;			///< The maximum angle deviation of the WheelFrame's normal vector from its supposed direction on the track.
		Angle ThresholdBinormal;		///< The maximum angle deviation of the WheelFrame's binormal vector from its supposed direction on the track.
		One ErrorReduction;				///< The error reduction parameter for the track joint.
		bool bTiltingEnabled;			///< Tilting on track enabled.

		TrackJointLimits() noexcept
			:	NormalForceLimitMin		{-infinite},
				NormalForceLimitMax		{infinite},
				BinormalForceLimitMin	{0},
				BinormalForceLimitMax	{infinite},
				NormalTorqueLimit		{infinite},
				TorqueLimit				{infinite},
				ThresholdPosition		{default_derailing_distance},
				ThresholdTangent		{_rad(pi/4)},
				ThresholdNormal			{_rad(pi/4)},
				ThresholdBinormal		{_rad(pi/4)},
				ErrorReduction			{ 0.25_1 },
				bTiltingEnabled			{false}
		{};

		TrackJointLimits(
			Force normalForceLimitMin,
			Force normalForceLimitMax,
			Force binormalForceLimitMin,
			Force binormalForceLimitMax,
			Torque normalTorqueLimit,
			Torque torqueLimit,
			Length thresholdPosition,
			Angle thresholdTangent,
			Angle thresholdNormal,
			Angle thresholdBinormal,
			One errorReduction,
			bool bTiltingEnabled ) noexcept
			:	NormalForceLimitMin		{normalForceLimitMin},
				NormalForceLimitMax		{normalForceLimitMax},
				BinormalForceLimitMin	{binormalForceLimitMin},
				BinormalForceLimitMax	{binormalForceLimitMax},
				NormalTorqueLimit		{normalTorqueLimit},
				TorqueLimit				{torqueLimit},
				ThresholdPosition		{thresholdPosition},
				ThresholdTangent		{thresholdTangent},
				ThresholdNormal			{thresholdNormal},
				ThresholdBinormal		{thresholdBinormal},
				ErrorReduction			{ errorReduction },
				bTiltingEnabled			{bTiltingEnabled}
		{};

		inline bool IsInsideLimits( const spat::Frame<Length,One>& frame, const spat::Frame<Length,One>& anchor ) const noexcept{
			const One eT = std::abs(std::asin((anchor.T % frame.T).Length()));
			const One eN = std::abs(std::asin((anchor.N % frame.N).Length()));
			const One eB = std::abs(std::asin((anchor.B % frame.B).Length()));

			return	anchor.P.Equals( frame.P, ThresholdPosition ) &&
					eN <= ThresholdTangent && eB <= ThresholdTangent &&
					eT <= ThresholdNormal && eB <= ThresholdNormal &&
					eT <= ThresholdBinormal && eN <= ThresholdBinormal;
		}
	};

}
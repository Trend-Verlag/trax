// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
//
//	trax track library
//	AD 2014
//
//  "It's all over and I'm standin' pretty
//   In this dust that was a city."
//
//								Nena


#pragma once

#include "Configuration.h"
#include "Units.h"

namespace trax{


	/// \brief Function that defines the characteristic behaviour of a
	/// motor to change its maximum available tracktion force with velocity.
	///
	///
	/// The Factory provides one sample implementation for each TractionType. These are:
	///
	///	\verbatim
	///	mechanic:
	/// f = fstep for vstep <= v < vnextstep
	///
	/// hydrodynamic:
	/// f = fstep + (fnextstep - fstep) (v - vstep)/(vnextstep - vstep)  for vstep <= v < vnextstep
	///
	/// electric:
	/// f = a/(v+x) - y
	/// with:
	/// x = sqrt( (vm-vt)(a + (vm-vt)/4) ) - (vm+vt)/2 with vt < vm
	/// y = a / (vm+x)
	/// f = a(vm-v)/(v+x)(vm+x)
	///
	/// steam:
	/// as electric with vt = 0
	/// \endverbatim
	struct TractionForceCharacteristic{

		/// \brief Some exemplaric traction types.
		enum class TractionType : char{
			unknown = 0,
			mechanic,		///< mechanical gear with different discrete gear settings
			hydrodynamic,	///< continuous linear gear settings
			electric,		///< constant traction force followed by hyperbolic decline
			steam,			///< hyperbolic decline from the start
			speedsteps,
			other,
		};

		static dclspc std::unique_ptr<TractionForceCharacteristic> Make( TractionType type = TractionType::speedsteps, Velocity vmax = 0_mIs, Velocity vtransit = 0_mIs ) noexcept;

		virtual TractionType GetTractionType() const noexcept = 0;

		/// \brief Mapping function.
		///
		/// The characteristic gets multiplied with the maximum torque.
		/// A value of 1 means that the maximum torques are applied as 
		/// specified by the Wheelsets attached to a WheelFrame. Typically
		/// the value will drop linearly or hyperbolically if the absolute 
		/// value of velocity gets high; it might get even negative at some 
		/// extreme point.
		/// \param velocity Velocity along the track.
		/// \returns Factor in the range [-1,1].
		virtual One operator()( Velocity velocity ) const noexcept = 0;


		/// \returns The maximum power the engine can develop with the
		/// characteristic divided by the maximum traction force (just
		/// multiply with max force to yield power)
		virtual decltype(Power{}/Force{}) MaximumPowerFraction() const noexcept = 0;


		/// \returns The maximum velocity the engine can theoretically 
		/// achieve under the assumtion that no work has to be done by her.
		virtual Velocity MaximumVelocity() const noexcept = 0;


		/// \brief Add a velocity/value pair to the characteristic.
		virtual void AddSpeedStep( Velocity velocity, One value ) = 0;


		/// \brief Get all the speedsteps.
		virtual void GetSpeedSteps( std::vector<std::pair<Velocity,One>>& speedsteps ) const = 0;

		virtual ~TractionForceCharacteristic() = default;
		TractionForceCharacteristic( const TractionForceCharacteristic& ) = delete;
		TractionForceCharacteristic( TractionForceCharacteristic&& ) = delete;
		TractionForceCharacteristic& operator=( const TractionForceCharacteristic& ) = delete;
		TractionForceCharacteristic& operator=( TractionForceCharacteristic&& ) = delete;
	protected:
		TractionForceCharacteristic() = default;
	};

}
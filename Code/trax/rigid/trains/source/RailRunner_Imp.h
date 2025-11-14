// Copyright (c) 2013 - 2019 Marc-Michael Horstmann;
// Copyright (c) 2020 - 2024 Trend Verlag;
// All Rights Reserved.
//
//
//	trax track library
//	AD 2023 
//
//  "Only the young,
//   only the young,
//   only the young,
//   only the young,
//   can run."
//
//			Taylor Swift

#pragma once

#include "../RailRunner.h"
#include "trax/Jack.h"

namespace trax
{
	template<class Base>
	class RailRunner_Imp :	public Base,
							public JackEnumerator{
	public:
		RailRunner_Imp() = default;
		RailRunner_Imp( const RailRunner_Imp& ) = delete;
		RailRunner_Imp( RailRunner_Imp&& ) noexcept = default;
		~RailRunner_Imp() noexcept = default;

		RailRunner_Imp& operator=( const RailRunner_Imp& ) = delete;
		RailRunner_Imp& operator=( RailRunner_Imp&& ) noexcept = default;

		/// \brief to return correct shared_ptr to this, this object have 
		/// to know about the use count structure.
		virtual void SetWeakPointerToSelf( std::weak_ptr<RailRunner_Imp> pThis ) noexcept;


		// RailRunner:
		std::shared_ptr<RailRunner> This() const noexcept override;

		bool IsValid() const noexcept override;

		using Base::Thrust;

		One Thrust() const noexcept override;

		using Base::ThrustAbsolute;

		void ThrustAbsolute( Force thrust ) override;

		using Base::Brake;

		One Brake() const noexcept override;

		using Base::BrakeAbsolute;

		void BrakeAbsolute( Force brake ) override;

		void UserData( RailRunnerUserData* pData ) noexcept override;

		RailRunnerUserData* UserData() const noexcept override;

	protected:

		inline void MoveLocation( trax::Location& location, Length ds ){
			if( location.Move( ds ).first )
				throw std::out_of_range( "RailRunner: location moved over loose track end!" );
		}
	private:
		std::weak_ptr<RailRunner_Imp> m_pThis;
		RailRunnerUserData* m_pUserData = nullptr;
	};

template<class Base>
void RailRunner_Imp<Base>::SetWeakPointerToSelf( std::weak_ptr<RailRunner_Imp> pThis ) noexcept{
	m_pThis = pThis;
}

template<class Base>
std::shared_ptr<RailRunner> RailRunner_Imp<Base>::This() const noexcept{
	return m_pThis.lock();
}

template<class Base>
inline bool RailRunner_Imp<Base>::IsValid() const noexcept
{
	if( !This() )
		return false;

	return true;
}

template<class Base>
One RailRunner_Imp<Base>::Thrust() const noexcept{
	Force maxThrust = this->MaxThrust();
	return maxThrust ? this->ThrustAbsolute() / maxThrust : 0_1;
}

template<class Base>
void RailRunner_Imp<Base>::ThrustAbsolute( Force thrust ){
	Force maxThrust = this->MaxThrust();
	this->Thrust( maxThrust ? thrust / this->MaxThrust() : 0_1 );
}

template<class Base>
inline One RailRunner_Imp<Base>::Brake() const noexcept{
	Force maxBrake = this->MaxBrake();
	return maxBrake ? this->BrakeAbsolute() / maxBrake : 0_1;
}

template<class Base>
inline void RailRunner_Imp<Base>::BrakeAbsolute( Force brake ){	
	Force maxBrake = this->MaxBrake();
	this->Brake( maxBrake ? brake / maxBrake : 1_1 );
}

template<class Base>
inline void RailRunner_Imp<Base>::UserData( RailRunnerUserData* pData ) noexcept
{
	m_pUserData = pData;
}

template<class Base>
inline RailRunnerUserData* RailRunner_Imp<Base>::UserData() const noexcept
{
	return m_pUserData;
}

}